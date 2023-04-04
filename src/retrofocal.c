/* Interpreter for RetroFOCAL
 Copyright (C) 2023 Maury Markowitz

 Based on RetroBASIC
 Copyright (C) 2020 Maury Markowitz

 Based on gnbasic
 Copyright (C) 1998 James Bowman

This file is part of RetroFOCAL.

RetroFOCAL is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RetroFOCAL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RetroFOCAL; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <sys/time.h>

#include "retrofocal.h"
#include "parse.h"

/* here's the actual definition of the interpreter state which is extern in the header */
interpreterstate_t interpreter_state;

/* and the same for the various flags */
bool run_program = true;                // default to running the program, not just parsing it
bool print_stats = false;               // do not print or write stats by default
bool write_stats = false;
int tab_columns = 10;                   // based on PET BASIC, which is a good enough target
bool trace_lines = false;								// turned on or off with a ?
bool ask_colon = true;          				// should ASK print a colon?
bool type_equals = false;								// don't print the = in TYPEs, that's FOCAL-69
bool type_space = true;								  // print a leading space in TYPE
bool upper_case = true;          				// force ASK input to upper case, which is generally the case for DEC
double random_seed = -1;                // reset with RANDOMIZE, if -1 then auto-seeds

char *source_file = "";
char *input_file = "";
char *print_file = "";
char *stats_file = "";

/* private types used only within the interpreter */

/* value_t is used to store (and process) the results of an evaluation */
// NOTE: this currently cannot return an array, so MAT instructions are not possible
typedef struct {
  int type;            /* NUMBER, STRING */
  char *string;
  double number;
} value_t;

/* function_storage_t holds the expression from the DEF */
typedef struct {
  int type;               /* NUMBER, STRING */
  list_t *parameters;     // parameters, if any, as variable_t
  expression_t *formula;	// related formula
} function_storage_t;

/* forward declares */
static value_t evaluate_expression(expression_t *e);
static double line_for_statement(const list_t *s);
static double current_line(void);

static void print_variables(void);
static void delete_variables(void);
static void delete_functions(void);
static void delete_lines(void);

/* defitions of variables used by the static analyzer */
clock_t start_ticks = 0, end_ticks = 0;	// start and end ticks, for calculating CPU time
struct timeval start_time, end_time;    // start and end clock, for total run time
struct timeval reset_time;     					// if the user resets the time with TIME$, this replaces start_time

/************************************************************************/

static void focal_error(const char *message)
{
  fprintf(stderr, "%s at line %2.2f\n", message, current_line());
}

/* Returns an either_t containing a string or a number for the underlying
   variable, along with its type in the out-parameter 'type'. If the
   variable has not been encountered before it will be created here. */
either_t *variable_value(const variable_t *variable, int *type)
{
  variable_storage_t *storage;
	char *storage_name;
	int index;
  
	// in contrast to BASIC, in FOCAL all variables can be arrays, and A and A()
	// refer to the same variable, so we don't have to munge on the "(" to make
	// it into a separate variable
	storage_name = str_new(variable->name);

	// see if we can find the entry in the symbol list
	storage = lst_data_with_key(interpreter_state.variable_values, storage_name);
  
  // if not, make a new variable slot in values and set it up
  if (storage == NULL) {
    int slots;
    
    // malloc a single slot, if it's an array we'll use this slot as the template
    storage = malloc(sizeof(*storage));
    storage->type = NUMBER;	// this is all we have in FOCAL, but leaving in the type for simplicity
    		
    // see if this reference includes subscripts
    if (variable->subscripts != NULL) {
			// in FOCAL, there is no equivalent of a DIM, and all arrays are -2048 to +2047.
			// I suspect that they used the subscripts as pseudo-names, so A(100) becomes
			// a separate variable A100. That would make it only create entries for those
			// indexes that are acually stored.
			//
			// given the small amount of memory this represents on a modern machine, we'll
			// just go ahead and dim all 4k slots for any variable we see with ()
			slots = 4096;
			storage->actual_dimensions = lst_append(NULL, INT_TO_POINTER(slots));;
    }
    // if it doesn't include subscripts, null them out and set the number of slots to 1
    else {
			// BUG: is it possible that we get a non-subscript ref after we set up the refs above?
      storage->actual_dimensions = NULL;
      slots = 1;
    }
    
    // now malloc the result and insert it into the values tree
    storage->value = malloc(slots * sizeof(storage->value[0]));
		interpreter_state.variable_values = lst_insert_with_key_sorted(interpreter_state.variable_values, storage, storage_name);
  }
  
  // if we haven't started runnning yet, we were being called during parsing to
  // populate the variable table. In that case, we don't need the value, so...
  if (interpreter_state.running_state == 0)
    return NULL;
  
  // at this point we have either found or created the variable, so...
  
  // compute array index, or leave it at zero if there is none
	index = 0;
	
	// there is only ever one dimension in FOCAL, so this is pretty simple,
	// the only crazy bit is that we have to map the input to negative values
	list_t *variable_index;       	// list of indices in this variable reference, each is an expression, likely a constant
	variable_index = lst_first_node(variable->subscripts);
	
	// if there are no subscripts, get the value at 0
	if (lst_length(variable_index) == 0) {
		index = 0;
	}
	else if (lst_length(variable_index) > 1)
		focal_error("Array access has more than one subscript"); // should we exit at this point?
	else
		if (variable_index != NULL) {
			// evaluate the variable reference's index for a given dimension
			value_t this_index = evaluate_expression(variable_index->data);
			
			// make sure the index is within the originally DIMed bounds
			// NOTE: should check against array_base, not 0, but this doesn't work in Dartmouth. see notes
			if ((this_index.number < -2048) || (this_index.number > 2047)) {
				focal_error("Array subscript out of bounds");
				this_index.number = 0;
			}
		
			index = this_index.number;
		} else {
			focal_error("Array subscript is empty");
			index = 0;
		}
  
  // returning the type, always a number in this case
  *type = NUMBER;

  // all done, return the value at that index
  return &storage->value[index];
}

/* cover method for variable_value, allows it to be exported to the parser
 without it having to know about either_t, which is private.
 */
void insert_variable(const variable_t *variable)
{
  int ignore = 0;
  variable_value(variable, &ignore);
}

/* and another version which takes the type for use with DEFINT etc. */
void insert_typed_variable(const variable_t *variable, int type)
{
  variable_value(variable, &type);
}

/* Similar to variable_value in concept, this code looks through the list of
 user-defined functions to find a matching name and/or inserts it if it's new.
 the difference is that this returns an expression which we then evaluate.
 */
expression_t *function_expression(const variable_t *function, expression_t *expression)
{
  // see if we can find the entry in the symbol list
  function_storage_t *storage;
	storage = lst_data_with_key(interpreter_state.functions, function->name);
  
  // if not, make a new slot in functions and set it up
  if (!storage) {
    // malloc an entry
    storage = malloc(sizeof(*storage));
    
    // set the return type based on the name
    char trailer = function->name[strlen(function->name) - 1];
    if (trailer == '$')
      storage->type = STRING;
    else
      storage->type = NUMBER; // this works for all of them, int, dbl, etc.
    
    // copy over the list of parameters
    storage->parameters = lst_copy(function->subscripts);
    
    // now store the expression/formula
    storage->formula = expression;
    
    // and insert it into storage
		interpreter_state.functions = lst_insert_with_key_sorted(interpreter_state.functions, storage, function->name);
  }
  
  // at this point we have either found or created the formula, so...
  return storage->formula;
}

/* converts a number to a new value_t */
static value_t double_to_value(const double v)
{
  value_t r;
  r.type = NUMBER;
  r.number = v;
  return r;
}

/* converts a number to a string */
/* this system follows the rules found in MS BASICs like the PET
 that is, generally:
 1) if the number is zero, return 0
 2) otherwise, move the decimal until the mantissa is 1e8 <= FAC < 1e9
 3) **round** the resulting 9-digit value
 4) if the number of decimal places moved is -10 < TMPEXP > 1 then just print the result with the decimal moved back
 5) otherwise, use E format
 in all cases, add a leading space for 0 or +ve values, - for -ve
 
 Item (3) means that 9,999,999,999 is printed as 1E+10, which is precisely the G format in C.
 So the code below it needlessly complex as anything other than 0 uses G. However, we'll leave
 in the IFs so that if we find new versions in the future that follow other rules its easy to
 add them.
 */
static char *number_to_string(const double d)
{
  static char str[40]; // use static so we know it won't be collected between calls
  if (d == 0.0) {
    sprintf(str, " 0"); // note the leading space, here and below
  } else if (d >= 0.01 && d <= 999999999) {
    sprintf(str, "% -.9G", d);
  } else if (d <= -0.01 && d >= -999999999) {
    sprintf(str, "% -.9G", d);
  } else {
    sprintf(str, "% -.9G", d);
  }
  return str;
}

/* number of jiffies since program start (or reset) 1/60th in Commodore/Atari format */
static int elapsed_jiffies() {
	struct timeval current_time, elapsed_time, reset_delta;
	
	// get the delta between the original start time and the reset time (likely zero)
	timersub(&reset_time, &start_time, &reset_delta);
	
	// then add that to the current time (seconds only, the format has no jiffies)
	gettimeofday(&current_time, NULL);
	timersub(&current_time, &start_time, &elapsed_time);

	// adjust for any seconds in the reset
	elapsed_time.tv_sec += reset_delta.tv_sec;
	
	// convert to jiffies
	long jiffies = elapsed_time.tv_sec * 60;
	jiffies += (int)(elapsed_time.tv_usec / 1000000 * 60);
	
	// the result has to be clamped to 5183999 (=24*60^3 -1) to match MS BASIC
	// we'll mod the number to prevent an overflow in BASIC, but given the run
	// times this should never occur
	jiffies %= 5183999;
	
	return (int)jiffies;
}

/* recursively evaluates an expression and returns a value_t with the result */
static value_t evaluate_expression(expression_t *expression)
{
  value_t result;
  value_t parameters[3];
  
  switch (expression->type) {
      // for number and string constants, simply copy the value and return it
    case number:
      result.type = NUMBER;
      result.number = expression->parms.number;
      break;
    case string:
      result.type = STRING;
      result.string = expression->parms.string;
      break;
      
      // variables are also easy, just copy over their value from storage
    case variable:
    {
      int type = 0;
      either_t *p = variable_value(expression->parms.variable, &type);
      result.type = type;
			
			// user functions will call this method while being set up and at that time the
			// values are not valid, so...
			if (type == STRING) {
				if (p)
					result.string = p->string;
				else
					result.string = "";
			}
			else {
				if (p)
					result.number = p->number;
				else
					result.string = 0;
			}
    }
      break;
      
      // user functions are not so simple, because the variable references
      // in the original definition have to be mapped onto the parameters
      // being passed in here. So this code looks at the original definition
      // to get the list of variable names, caches those values on a stack,
      // recalculates those variables based on the inputs to the function call,
      // runs the calculation using those variables, and then pops the original
      // values back off the stack.
    case fn:
    {
      result.type = NUMBER;
      result.number = 0;
      // get the original definition or error out if we can't find it
      char *func_name = expression->parms.variable->name;
			function_storage_t *original_definition;
			original_definition = lst_data_with_key(interpreter_state.functions, func_name);
      if (original_definition == NULL) {
        char buffer[80];
        sprintf(buffer, "User-defined function '%s' is being called but has not been defined", func_name);
        focal_error(buffer);
        result.type = NUMBER;
        result.number = 0;
        break;
      }
      // if we found the function, check that it has the same number of parameters as this function call
      if (lst_length(original_definition->parameters) != lst_length(expression->parms.variable->subscripts)) {
        focal_error("User-defined function '%s' is being called with the wrong number of parameters");
        break;
      }

      // for each parameter name in the original function call, copy the current value
      // of any global version of that variable's value onto a temporary stack...
			list_t *stack = NULL;
      variable_storage_t *storage;
      either_t *stored_val;
      expression_t *original_parameter = original_definition->parameters->data; // pre-flight for the first time through
      int type = 0;
      for (list_t *param = expression->parms.variable->subscripts; param != NULL; param = lst_next(param)) {
        // retrieve the original value
        stored_val = variable_value(original_parameter->parms.variable, &type);

        // make a slot and push the value onto the stack
        storage = malloc(sizeof(*storage));
        storage->type = type;
        storage->value = malloc(sizeof(either_t));
          if (type == STRING)
            storage->value->string = stored_val->string;
          else
            storage->value->number = stored_val->number;
				stack = lst_insert_with_key_sorted(stack, storage, original_parameter->parms.variable->name);
        
        // move to the next item in the original parameter list, if there's any left
        if (original_definition->parameters->next != NULL)
          original_parameter = lst_next(original_definition->parameters)->data;
      }
      
      // now loop over the list of inputs, calculate them, and update the global variable
      value_t updated_val;
      for (list_t *param = expression->parms.variable->subscripts; param != NULL; param = lst_next(param)) {
        // calculate the value for the parameter expression
        updated_val = evaluate_expression(param->data);
        
        // find and update the global value
        stored_val = variable_value(original_parameter->parms.variable, &type);
        if (updated_val.type == type) {
          if (type == STRING)
            stored_val->string = updated_val.string;
          else
            stored_val->number = updated_val.number;
        } else {
          // if the type we stored last time is different than this time...
          focal_error("Type mismatch in user-defined function call");
          break;
        }
        
        // move to the next parameter
        if (original_definition->parameters->next != NULL)
          original_parameter = lst_next(original_definition->parameters)->data;
      }
      
      // with the global values updated, we can calculate our expression
      expression_t *p = NULL;
      p = function_expression(expression->parms.variable, p);
      if (p == NULL) {
        char buffer[80];
        sprintf(buffer, "User-defined function '%s' is being called but has not been defined", expression->parms.variable->name);
        focal_error(buffer);
      } else {
        result = evaluate_expression(p);
      }
      
      // and then pop the values back off the stack into the globals
      variable_storage_t *temp_val;
      original_parameter = original_definition->parameters->data; // pre-flight for the first time through
      for (list_t *param = expression->parms.variable->subscripts; param != NULL; param = lst_next(param)) {
        // retrieve the original name and value
        either_t *global_val = variable_value(original_parameter->parms.variable, &type);
        
        // find the original value in the stack
				temp_val = lst_data_with_key(stack, original_parameter->parms.variable->name);
        
        // copy the value back
        if (type == STRING)
          global_val->string = temp_val->value->string;
        else
          global_val->number = temp_val->value->number;
        
        // kill the stack entry
				stack = lst_remove_node_with_key(stack, original_parameter->parms.variable->name);
        free(temp_val);
        
        // move to the next parameter
        if (original_definition->parameters->next != NULL)
          original_parameter = lst_next(original_definition->parameters)->data;
      }
    }
      break;
      
      // and now for the fun bit, the operators list...
    case op:
      // build a list of values for each of the parameters by recursing
      // on them until they return a value
      for (int i = 0; i < expression->parms.op.arity; i++)
        parameters[i] = evaluate_expression(expression->parms.op.p[i]);
      
      // now calculate the results based on those values
      if (expression->parms.op.arity == 0) {
        // so far all of these are numbers
        result.type = NUMBER;

        switch (expression->parms.op.opcode) {
          case FRAN:
            result.number = ((double)rand() / (double)RAND_MAX); // don't forget the cast!
            break;
						
						// TIME is the number of jiffies since the last restart, always 1/60 even on PAL.
						// In our code, we treat the start of the program as the restart time. This can
						// be changed by setting the value of TIME$ (which is really weird if you think
						// about it, why not set TIME?) so we keep track of that value in reset_ticks
						// and modify the value if it's not zero
						
						// returns the number of seconds since restart in HMS format
						// this is the case where TIME$ appears on its own in a function-like call,
						// the other syntax is TIME=value, which is handled as a statement, not a function

					default:
						focal_error("Unhandled arity-0 function");
        }
      }
      else if (expression->parms.op.arity == 1) {
        // most of the following functions return numbers, so default to that
        result.type = NUMBER;
        
        // get the first parameter, in this case, the only one
        double a = parameters[0].number;
        
        switch (expression->parms.op.opcode) {
          case '-':
            result.number = -a;
            break;
          case FABS:
            result.number = fabs(a);
            break;
          case FATN:
            result.number = atan(a);
            break;
					case FCOS:
						result.number = cos(a);
						break;
           case FEXP:
            result.number = exp(a);
            break;
					case FIN:
					{
						char c[2];
						c[0] = (char)a;
						c[1] = '\0';
						result.type = STRING;
						result.string = str_new(c);
					}
						break;
					case FITR:
						result.number = floor(a);
						break;
          case FOUT:
            result.type = STRING;
            result.string = str_new(number_to_string(a));
            break;
          case FLOG:
            result.number = log(a);
            break;
          case FSIN:
            result.number = sin(a);
            break;
					case FSGN:
						// early MS variants return 1 for 0, this implements the newer version where 0 returns 0
						if (a < 0)
							result.number = -1;
						else if (a == 0)
							result.number = 0;
						else
							result.number = 1;
						break;
         case FSQT:
            result.number = sqrt(a);
            break;
            												
          default:
            focal_error("Unhandled arity-1 function");
        } //switch
      } //arity = 1
      
      // now the functions that take two parameters
      else if (expression->parms.op.arity == 2) {
        // cache the parameters
        double a = parameters[0].number;
        double b = parameters[1].number;
        
        switch (expression->parms.op.opcode) {
          case '+':
            if (parameters[0].type >= NUMBER && parameters[1].type >= NUMBER)
              result = double_to_value(a + b);
            else {
              result.number = 0;
              focal_error("Type mismatch, string and number in addition");
            }
            break;
          case '-':
            result = double_to_value(a - b);
            break;
          case '*':
            result = double_to_value(a * b);
            break;
          case '/':
            if (b == 0)
              focal_error("Division by zero");
            result = double_to_value(a / b);
            break;
          case '^':
            result = double_to_value(pow(a, b));
            break;
          case '=':
            if (parameters[0].type >= NUMBER)
              result = double_to_value(-(a == b));
            else
							result.number = 0;
							focal_error("Type mismatch, string and number in comparison"); // it should not be possible for this to happen
            break;

          default:
            result.number = 0;
            focal_error("Unhandled arity-2 function");
            break;
        }
      }
      
      else {
        result.number = 0;
        break;
      }
  }
  return result;
}

/* handles the TYPE statements, which can get complex */
static void print_expression(expression_t *e, const char *format)
{
  // get the value of the expression for this item
  value_t v = evaluate_expression(e);
  
  // if there is a USING string, build a c-style format string from it
  if (format) {
    char copy[MAXSTRING];
    char *hash;
    int width = 0, prec = 0;
    
    // TODO: this doesn't handle string formatters, see GW-BASIC manual
    strcpy(copy, format);
    // look for any hash characters in front and behind a period
    hash = strchr(copy, '#');
    if (hash) {
      char *pc = hash;
      while (*pc == '#') {
        width++;
        pc++;
      }
      while (*pc == '.') {
        width++;
        pc++;
      }
      while (*pc == '#') {
        prec++;
        width++;
        pc++;
      }
      sprintf(hash, "%%*.*f");  // replace ##.## with % spec
      strcat(hash, pc);         // append the rest of string
    }
    // and now print it out using that format
    switch (v.type) {
      case NUMBER:
        interpreter_state.cursor_column += printf(copy, width, prec, v.number);
        break;
      case STRING:
        interpreter_state.cursor_column += printf(copy, width, prec, str_escape(v.string));
        break;
    }
  }
  // there was no USING format, so use a default formatter instead,
  // potentially limited to the fieldwidth if its tabular
  else {
    switch (v.type) {
      case NUMBER:
      {
        // for some reason, PRINT adds a space at the end of numbers
        char* a = number_to_string(v.number);
        interpreter_state.cursor_column += printf("%s ", a); // note the trailing space
      }
        break;
      case STRING:
        interpreter_state.cursor_column += printf("%-s", v.string);
        break;
    }
  }
}

/* given a statement, this returns the line number it's part of */
/* NOTE: this is likely expensive, because is uses the index lookup
				methods in list_t, which loop. So only use it when required!
 */
static double line_for_statement(const list_t *statement)
{
  // get a pointer to the program from the first line
  list_t *program = interpreter_state.lines[interpreter_state.first_line];
  
  // get the index of this statement in that list
  int target_index = lst_index_of_data(program, statement->data);
  
  // loop forward through the program until we find a line who's
  // first statement is higher than that index. That means we must
  // be on the previous line
  int this_index, previous_line = interpreter_state.first_line;
  for (int i = interpreter_state.first_line; i < MAXLINE; i++) {
    // skip empty lines
    if (interpreter_state.lines[i] == NULL) continue;
    
    // get the index of the first statement on that line
    this_index = lst_index_of_data(program, interpreter_state.lines[i]->data);
    
    // now see if we're in this line or the previous one
    if (this_index == target_index) return (double)i / 100.0;
    if (this_index > target_index) return (double)previous_line / 100.0;
    
    // otherwise keep looking
    previous_line = i;
  }
  // didn't find it
  return -1;
}

/* curries line_for_statement to return the current line */
static double current_line()
{
  return line_for_statement(interpreter_state.current_statement);
}

/* returns a pointer to the named line or returns an error if it's not found */
static list_t *find_line(double linenumber)
{
  char buffer[50];
  
  // negative numbers are not allowed
  if (linenumber < 0) {
		if (linenumber != floor(linenumber)) {
			sprintf(buffer, "Negative target line %2.2f in branch", linenumber);
			focal_error(buffer);
		} else {
			sprintf(buffer, "Negative target group %i in branch", (int)linenumber);
			focal_error(buffer);
		}
    return NULL;
  }
	
	// in focal, the target we're looking for could be either a specific line
	// or a group number. we will start with the single line, which can never be x.00
	if (linenumber != floor(linenumber)) {
		// check it exists
		if (interpreter_state.lines[(int)(linenumber * 100)] == NULL) {
			sprintf(buffer, "Undefined target line %2.2f in branch", linenumber);
			focal_error(buffer);
			return NULL;
		}
		
		// otherwise we did find a line, so return it
		return interpreter_state.lines[(int)(linenumber * 100)];
	}
	else {
		// for the group lookup, we have to loop through it until we find an item with that value
		list_t *lv = NULL;
		for(int i = (int)(linenumber * 100); i < MAXLINE - 1; i++) {
			if (interpreter_state.lines[i] != NULL) {
				// we found a non-empty line, is it the right group?
				if (linenumber == floor(i / 100)) {
					// we found the first line in the group
					lv = interpreter_state.lines[i];
					break;
				}
			}
		}
		if (lv != NULL) {
			return lv;
		}
		else {
			sprintf(buffer, "Undefined target group %i in branch", (int)linenumber);
			focal_error(buffer);
			return NULL;
		}
	}
	
	// failsafe
	sprintf(buffer, "Undefined target line %2.2f in branch", linenumber);
	focal_error(buffer);
	return NULL;
}

/* performs a single statement */
static void perform_statement(list_t *L)
{
  statement_t *ps = L->data;
  if (ps) {
    switch (ps->type) {
				
			case ASK:
			{
				// ASK is similar to BASIC's INPUT, and allows mixing prompts and inputs.
				// It also has the option of printing a colon, like BASIC's question mark,
				// but does so for every input, not just the first
				
				// loop over the items in the variable/prompt list
				for (list_t *I = ps->parms.input; I != NULL; I = lst_next(I)) {
					either_t *value;
					int type = 0;
					
					printitem_t *ppi = I->data;
					if (ppi->expression->type == variable) {
						char line[80];
						
						// print the colon if that option is turned on
						if (ask_colon)
							printf(":");
						
						// see if we can get some data, we should at least get a return
						fflush(stdout);
						if (fgets(line, sizeof(line), stdin) != line)
							exit(EXIT_FAILURE);
						
						// we got something, so null-terminate the string
						line[strlen(line) - 1] = '\0';
						
						// optionally convert to upper case
						if (upper_case) {
							char *c = line;
							while (*c) {
								*c = toupper((unsigned char) *c);
								c++;
							}
						}
						
						// FOCAL only has numeric variables, but it does have the ability
						// to type in strings at prompts, which it converts to a numeric
						// value. we do that here. note that FOCAL uses an odd version
						// of ASCII, with the high bit set.
						
						// find the storage for this variable, and assign the value
						value = variable_value(ppi->expression->parms.variable, &type);
						if (type >= NUMBER) {
							sscanf(line, "%lg", &value->number);
						} else {
							value->string = str_new(line);
						}
					}
					// if it's not a variable, it's some sort of prompt, so print it
					else {
						print_expression(ppi->expression, NULL);
					}
				}
			}
				break;
				
			case DO:
			{
				// DO is a little different than GOTO because it *might* be a gosub if the
				// line number is an integer. I am also not clear what happens when you GOTO
				// out of a gosub-like DO.
				gosubcontrol_t *new = malloc(sizeof(*new));
				
				new->returnpoint = lst_next(L);
				interpreter_state.gosubstack = lst_append(interpreter_state.gosubstack, new);
				interpreter_state.next_statement = find_line(evaluate_expression(ps->parms.gosub).number);
			}
				break;

      case FOR:
      {
        forcontrol_t *new_for = malloc(sizeof(*new_for));
        either_t *loop_value;
        int type = 0;
        
        new_for->index_variable = ps->parms._for.variable;
        new_for->begin = evaluate_expression(ps->parms._for.begin).number;
        new_for->end = evaluate_expression(ps->parms._for.end).number;
        if (ps->parms._for.step)
          new_for->step = evaluate_expression(ps->parms._for.step).number;
        else {
          new_for->step = 1;
        }
        new_for->head = L;
        loop_value = variable_value(new_for->index_variable, &type);
        loop_value->number = new_for->begin;
        
        interpreter_state.forstack = lst_append(interpreter_state.forstack, new_for);
      }
        break;
        
      case GOTO:
      {
        interpreter_state.next_statement = find_line(evaluate_expression(ps->parms._do).number);
      }
        break;
        
      case IF:
      {
        value_t cond = evaluate_expression(ps->parms._if.condition);
				/* in contrast to BASIC, FOCAL uses the FORTRAN-like model where all comparisons are
				   mathematical and the branch is based on whether the result of the comparison is
				   -ve, 0 or +ve. The 0 and +ve branches are optional. If either is missing, that case
				   is accomplished by running any remaining statements on the line (like BASIC in that
				   respect). This leads to some complexity... */
				if (cond.number < 0 && ps->parms._if.less_line > 0) {
					interpreter_state.next_statement = find_line(ps->parms._if.less_line);
				}
				else if (cond.number == 0 && ps->parms._if.zero_line > 0) {
					interpreter_state.next_statement = find_line(ps->parms._if.zero_line);
				}
				else if (cond.number > 0 && ps->parms._if.more_line > 0) {
					interpreter_state.next_statement = find_line(ps->parms._if.more_line);
				}
				else {
					// if none of those fired, it means we didn't have a line number for the
					// correct case, which means we just continue onto the next statement
				}
      }
        break;
				
			case QUIT:
				// set the instruction pointer to null so it exits below
				interpreter_state.next_statement = NULL;
				break;
								
			case SET:
      {
        either_t *stored_val;
        int type = 0;
        value_t exp_val;
        
        // get/make the storage entry for this variable
        stored_val = variable_value(ps->parms.let.variable, &type);
        
        // evaluate the expression
        exp_val = evaluate_expression(ps->parms.let.expression);
        
        // make sure we got the right type, and assign it if we did
        if (exp_val.type == type) {
          if (type == STRING)
            stored_val->string = exp_val.string;
          else
            stored_val->number = exp_val.number;
        } else {
          // if the type we stored last time is different than this time...
          focal_error("Type mismatch in assignment");
        }
      }
        break;
        
//      case NEXT:
//      {
//				// make sure there is a stack
//				if (interpreter_state.forstack  == NULL || lst_length(interpreter_state.forstack) == 0) {
//					basic_error("NEXT without FOR");
//					break;
//				}
//
//				// get the most-recent FOR, which is the *end* of the list
//				forcontrol_t *pfc = lst_last_node(interpreter_state.forstack)->data;
//
//				// see if the next has any variable names, that is, NEXT I vs. NEXT,
//				// and if so, ensure the latest FOR on the stack is one of those variables
//				if (lst_length(ps->parms.next) > 0) {
//					bool foundIt = false;
//					list_t *var = lst_first_node(ps->parms.next);
//					for (int i = 0; i < lst_length(ps->parms.next); i++) {
//						if (strcmp(pfc->index_variable->name, ((variable_t *)var->data)->name) == 0) {
//							foundIt = true;
//						}
//						else {
//							var = lst_next(ps->parms.next);
//						}
//					}
//					if (!foundIt) {
//						basic_error("NEXT with mismatched FOR");
//						break;
//					}
//				}
//
//				// do a STEP
//        int type = 0;
//				either_t *lv = variable_value(pfc->index_variable, &type);
//        lv->number += pfc->step;
//
//				// and see if we need to go back to the FOR or we're done and we continue on
//        if (((pfc->step < 0) && (lv->number >= pfc->end)) ||
//            ((pfc->step > 0) && (lv->number <= pfc->end))) {
//          // we're not done, go back to the head of the loop
//          interpreter_state.next_statement = lst_next(pfc->head);
//        } else {
//          // we are done, remove this entry from the stack and just keep going
//          interpreter_state.forstack = lst_remove_node_with_data(interpreter_state.forstack, pfc);
//					free(pfc);
//        }
//      }
//        break;
                
			case TYPE:
      {
        printitem_t *pp;
        // loop over the items in the print list
        for (list_t *I = ps->parms.print.item_list; I != NULL; I = lst_next(I)) {
          pp = I->data;
					
					// first off, see if this item is a lone separator
					//
					// unlike BASIC, separators "do things" even if there is no expression,
					// so TYPE !!! will have three items in the printlist and all three
					// with have a separator but no expression
					switch (pp->separator) {
						case '!':
							printf("\n");
							interpreter_state.cursor_column = 0; // and reset this!
							break;
						case '#':
							printf("\r");
							interpreter_state.cursor_column = 0;
							break;
						case ':':
							while (interpreter_state.cursor_column % tab_columns != 0) {
								printf(" ");
								interpreter_state.cursor_column++;
							}
							break;
					}
					
					// now look to see if this item is a formatter
					if (pp->format > 0) {
						
					}

					// check to see if the expression is null, if so, that's because
					// this particular item consists of a separator or format string
					if (pp->expression == NULL) {
						continue;
					}
					else {
						
					}
          // if there's a USING, evaluate the format string it and print using it
          if (ps->parms.print.format) {
            value_t format_string;
            format_string = evaluate_expression(ps->parms.print.format);
            print_expression(pp->expression, format_string.string);
          }
          // otherwise, see if there's a separator and print using the width
          else {
            print_expression(pp->expression, NULL);
          }
          
          // for each item in the list, look at the separator, if there is one
          // and it's a comma, advance the cursor to the next tab column
          if (pp->separator == ',')
            //FIXME: this should wrap at 80 columns
            while (interpreter_state.cursor_column % tab_columns != 0) {
              printf(" ");
              interpreter_state.cursor_column++;
            }
        }
        
        // now get the last item in the list so we can see if it's a ; or ,
        if (lst_last_node(ps->parms.print.item_list))
          pp = (printitem_t *)(lst_last_node(ps->parms.print.item_list)->data);
        else
          pp = NULL;
        
      }
        break;
                
      case RETURN:
      {
				gosubcontrol_t *pgc;
				if (interpreter_state.gosubstack == NULL || lst_length(interpreter_state.gosubstack) == 0) {
					focal_error("RETURN without GOSUB");
					break;
				}

        pgc = lst_last_node(interpreter_state.gosubstack)->data;
        interpreter_state.next_statement = pgc->returnpoint;
        interpreter_state.gosubstack = lst_remove_node_with_data(interpreter_state.gosubstack, pgc);
      }
        break;
				
      case VARLIST:
        print_variables();
        break;
        
      default:
        printf("Unimplemented statement %d\n", ps->type);
				exit(0);
    } //end switch
  }
}

/* variable tree walking methods */

/* cheater method for printing out all the variable names */
static void print_symbol(void *key, void *value, void *unused)
{
  printf("\n%s ", (char *)key);
}
/* ...and their values */
//static int print_value(void *key, void *value, void *unused)
//{
//	variable_storage_t *storage;
//	storage = lst_data_with_key(interpreter_state.variable_values, key);
//
//  either_t *p = storage->value;
//  int type = storage->type;
//  if (type == STRING)
//    if (p->string == 0)
//      printf("\n%s: %s", (char *)key, "NULL");
//    else
//      printf("\n%s: %s", (char *)key, (char *)p->string);
//  else
//    printf("\n%s: %f", (char *)key, (double)p->number);
//  return FALSE;
//}
/* used for VARLIST in those versions of BASIC that support it */
static void print_variables() {
	lst_foreach(interpreter_state.variable_values, print_symbol, NULL);
  printf("\n\n");
}
/* used for CLEAR, NEW and similar instructions. */
static void delete_variables() {
	lst_free_everything(interpreter_state.variable_values);
	interpreter_state.variable_values = NULL;
}
static void delete_functions() {
	lst_free_everything(interpreter_state.functions);
	interpreter_state.functions = NULL;
}
static void delete_lines() {
  for(int i = MAXLINE - 1; i >= 0; i--) {
    if (interpreter_state.lines[i] != NULL) {
      lst_free(interpreter_state.lines[i]);
    }
  }
}

/* set up empty trees to store variables and user functions as we find them */
void interpreter_setup()
{
	interpreter_state.variable_values = NULL;
	interpreter_state.functions = NULL;
}

/* after yacc has done it's magic, we form a program by pointing
 the ->next for each line to the head of the next non-empty line.
 that way we don't have to search through the line array for the
 next non-null entry during the run loop, we just keep stepping
 through the ->next until we fall off the end. this is how most
interpreters handled it anyway. */
void interpreter_post_parse(void)
{
  // look for the first entry in the lines array with a non-empty statement list
  int first_line = 0;
  while ((first_line < MAXLINE - 1) && (interpreter_state.lines[first_line] == NULL))
    first_line++;
  
  // that statement is going to be the head of the list when we're done
  list_t *first_statement = interpreter_state.lines[first_line];
  
  // now find the next non-null line and concat it to the first one, and repeat
  for (int i = first_line + 1; i < MAXLINE; i++) {
    if (interpreter_state.lines[i])
      first_statement = lst_concat(first_statement, interpreter_state.lines[i]);
  }
  
  // and set the resulting list back into the first line
  // NOTE: do we need to do this? isn't this already there?
  interpreter_state.lines[first_line] = first_statement;
  // and keep track of this for posterity
  interpreter_state.first_line = first_line;
  
  // a program runs from the first line, so...
  interpreter_state.current_statement = first_statement;          // the first statement
}

/* the main loop for the program */
void interpreter_run(void)
{
  // the cursor starts in col 0
  interpreter_state.cursor_column = 0;
	
	// the normal format is similar
	interpreter_state.format = calloc(MAXSTRING, sizeof(char));

  // start the clock and mark us as running
  start_ticks = clock();
	gettimeofday(&start_time, NULL);
  interpreter_state.running_state = 1;
	
	// and set the reset to now as well
	gettimeofday(&reset_time, NULL);
  
  // last line number we ran, used for tracing/stepping
  int last_line = interpreter_state.first_line;
  if (trace_lines)
    printf("[%i]\n", last_line);
  
  // very simple - perform_statement returns the next statement and the
  // main below set us up to point to the first one, so just call
  // that one function until you get a NULL
  while (interpreter_state.current_statement) {
    // get the next statement from the one we're about to run
    interpreter_state.next_statement = lst_next(interpreter_state.current_statement);

    // run the one we're on
    perform_statement(interpreter_state.current_statement);
    // and move to the next statement, which might have changed inside perform
    interpreter_state.current_statement = interpreter_state.next_statement;
    
    // trace, only on line changes
    if (trace_lines && last_line != current_line()) {
      last_line = current_line();
      printf("[%i]\n", last_line);
    }
  }
  
  // stop the clock and mark us as stopped
  end_ticks = clock();
  gettimeofday(&end_time, NULL);
  interpreter_state.running_state = 0;
}
