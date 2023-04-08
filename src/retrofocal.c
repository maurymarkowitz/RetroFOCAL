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
bool ask_colon = false;          				// should ASK print a colon? was only added in FOCAL-71
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
static void delete_lines(void);

/* defitions of variables used by the static analyzer */
clock_t start_ticks = 0, end_ticks = 0;	// start and end ticks, for calculating CPU time
struct timeval start_time, end_time;    // start and end clock, for total run time
struct timeval reset_time;     					// if the user resets the time with TIME$, this replaces start_time

/************************************************************************/

/**
 * Prints a formatted error message along with the offending line number.
 *
 * @param message The error message.
 */
static void focal_error(const char *message)
{
  fprintf(stderr, "%s at line %2.2f\n", message, current_line());
}

/** Returns an either_t containing a string or a number for the underlying
 * variable, along with its type in the out-parameter 'type'. If the
 * variable has not been encountered before it will be created here.
 *
 * @param variable The variable reference to look up.
 * @paramout type The variable type as found in storage.
 * @returns An either_t containing a numeric result.
 */
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
} /* variable_value */

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

/* converts a number to a new value_t */
static value_t double_to_value(const double v)
{
  value_t r;
  r.type = NUMBER;
  r.number = v;
  return r;
} /* double_to_value */

/** Returns a number encoding a string using DEC's 6-bit codes
 *
 * This system follows the rules found in MS BASICs like the PET
 * that is, generally:
 * 1) if the number is zero, return 0
 * 2) otherwise, move the decimal until the mantissa is 1e8 <= FAC < 1e9
 * 3) **round** the resulting 9-digit value
 * 4) if the number of decimal places moved is -10 < TMPEXP > 1 then just print the result with the decimal moved back
 * 5) otherwise, use E format
 *
 * in all cases, add a leading space for 0 or +ve values, - for -ve
 *
 * Item (3) means that 9,999,999,999 is printed as 1E+10, which is precisely the G format in C.
 * So the code below it needlessly complex as anything other than 0 uses G. However, we'll leave
 * in the IFs so that if we find new versions in the future that follow other rules its easy to
 * add them.
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
} /* number_to_string */

/** Returns the DEC 6-bit character code for any given character.
 *
 * @param one_char The character to convert.
 * @return The DEC character code number.
 */
static int char_code_for_character(const char one_char)
{
	switch (one_char) {
		case '@' ... 'Z' : return ((int)one_char - '@'); break;
		case '0' ... '9' : return ((int)one_char - '0'); break;
		case ':' ... '?' : return ((int)one_char + 22); break;
		case '[' ... '_' : return ((int)one_char - 58); break;
		case ' ' ... '/' : return ((int)one_char + 8); break;
		default: return 0;
	}
} /* char_code_for_character */

/** Returns a number encoding a string using DEC's 6-bit codes. The value
 * is always an integer, but may have an exponent if the string has an E.
 *
 * FOCAL was built on a machine with no inherant string support, and the
 * language does not have any internal string handling. However, the need
 * to input short strings for things like "yes or no" remained, so the
 * solution was to use the 6-bit teletype codes where A=1 and Z=26 and then
 * just string them together so that "A" produces 1, "A1" produces 11 and
 * "Z1" produces 261. The weird part is that if the number is two digits,
 * the digits will overlap, so "AZ" produces 126, but "ZZ" produces 286.
 *
 * To emulate this behaviour, this code loops backward through the string,
 * multiplying each character by its position*10.
 *
 * See section 3.4.8: http://bitsavers.trailing-edge.com/pdf/dec/pdp8/focal/DEC-08-AJBB-DL_Advanced_FOCAL_Technical_Specification_Apr69.pdf
 *
 * @param string The string to convert to a numeric representation.
 * @return A numeric representation of the string.
 */
static double string_to_number(const char *string)
{
	int len = (int)strlen(string);

	// empty/useless string?
	if (len == 0)
		return 0.0;

	int e_location = -1;
	int p_location = -1;
	int integer = 0;
	int fraction = 0;
	int exponent = 0;
	int mantissa_sign = 1;
	int exponent_sign = 1;
	
	// look for Es and .s in the string
	for (int i = 0; i < (int)len; i++) {
		char c = string[i];
		if (c == 'E' || c == 'e') {
			if (e_location != -1) {
				focal_error("More than one E in string value");
				return 0;
			}
			e_location = i;
		}
		else if (c == '.') {
			if (p_location != -1) {
				focal_error("More than one decimal/period in string value");
				return 0;
			}
			p_location = i;
		}
	}
	// if there wasn't an E or period, move it to the end
	if (e_location == -1)
		e_location = len;
	if (p_location == -1)
		p_location = e_location;
	
	// process the integer part, everything to the left of the period and/or E
	for (int i = 0; i < p_location; i++) {
		char c = string[i];
		int val = 0;
		if (c == '-')
			mantissa_sign = !mantissa_sign;
		else if (c == '+')
			mantissa_sign = 1;
		else if (isalpha(c))
			val = char_code_for_character(c);
		else if (isdigit(c))
			val = (c - '0');
		else {
			focal_error("Invalid character in string value");
			return 0;
		}
		integer = integer * 10 + val;
	}
	
	// and the fraction, if there is one
	for (int i = p_location + 1; i < e_location; i++) {
		char c = string[i];
		int val = 0;
		if (isalpha(c))
			val = char_code_for_character(c);
		else if (isdigit(c))
			val = (c - '0');
		else {
			focal_error("Invalid character in string value");
			return 0;
		}
		fraction = fraction * 10 + val;
	}
	
	// and then for the exponent, if there is any
	for (int i = e_location + 1; i < len; i++) {
		char c = string[i];
		int val = 0;
		if (isalpha(c))
			val = char_code_for_character(c);
		else if (isdigit(c))
			val = (c - '0');
		else if (c == '-')
			exponent_sign = !exponent_sign;
		else if (c == '+')
			exponent_sign = 1;
		else {
			focal_error("Invalid character in string value");
			return 0;
		}
		exponent = exponent * 10 + val;
	}
	
	// and construct the final number
	double val = ((integer * mantissa_sign) + (fraction / pow(10, e_location - p_location - 1))) * pow(10, (exponent * exponent_sign));
	return val;
} /* string_to_number */

/** Number of jiffies since program start (or reset) 1/60th in Commodore/Atari format.
 *
 */
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
} /* elapsed_jiffies */

/** Recursively evaluates an expression and returns a value_t with the result.
 *
 * @param expression The expression to evaluate.
 * @return The result, either a number or string.
 */
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
		case numstr:
			result.type = NUMBER;
			result.number = string_to_number(expression->parms.string);
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
						
					case FNEW: // these are both unhandled and return 0
						result.number = 0.0;
						break;
					case FCOM:
						result.number = 0.0;
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
} /* evaluate_expression */

/** Prints a single printitem_t, which may be an expression, a field
 * separator which includes ! for newlines, or a formatter.
 *
 * @param item The printitem to interpret.
 */
static void print_item(printitem_t *item)
{
	// first, see if there is an expression associated with this item,
	// which would imply its something that can actually be printed
	expression_t *e = item->expression;
	
	// if the expression is empty, then its some sort of control, which
	// will either be in the format or the sepatator
	if (e == NULL) {
		// is it a separator?
		if (item->separator > 0) {
			switch (item->separator) {
				case '!':
					printf("\n");
					interpreter_state.cursor_column = 0;
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
				default: { } 					// do nothing, consider this a non-error?
			}
		}
		// is it a formatter?
		else if (item->format != 0) {
			interpreter_state.format = item->format;
		}
		// is it totally empty?
		else {
			focal_error("Print item has no expression, format or separator");
		}
		
	}
	// if e is not null, then its an expression we want to evaluate and print
	else {
		// get the value of the expression for this item
		value_t v = evaluate_expression(e);
		
		switch (v.type) {
			case NUMBER:
			{
				// if it's a number, make a c-style formatter for the output
				char fmtstr[MAXSTRING];
				int width = trunc(interpreter_state.format);
				int prec = interpreter_state.format - trunc(interpreter_state.format);
				sprintf(fmtstr, "%%%d.%df", width,prec);
				
				interpreter_state.cursor_column += printf(fmtstr, width, prec, v.number);
			}
				break;
				
			case STRING:
				// if it's a string, just print it out
				interpreter_state.cursor_column += printf("%-s", v.string);
				break;
		}
	} // e != NULL
} /* print_item */

/** Returns the line number for given a statement.
 *
 * NOTE: this is likely expensive, because is uses the index lookup
 *				methods in list_t, which loop. So only use it when required!
 *
 * @param statement The statement you are looking for.
 * @return The line number as a double.
 */
static double line_for_statement(const list_t *statement)
{
  // get a pointer to the program from the first line
  list_t *program = interpreter_state.lines[interpreter_state.first_line_index];
  
  // get the index of this statement in that list
  int target_index = lst_index_of_data(program, statement->data);
  
  // loop forward through the program until we find a line who's
  // first statement is higher than that index. That means we must
  // be on the previous line
  int this_index, previous_line = interpreter_state.first_line_index;
  for (int i = interpreter_state.first_line_index; i < MAXLINE; i++) {
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
	int group = trunc(linenumber);
	int step = round((linenumber - group) * 100);
  
  // negative numbers are not allowed
  if (linenumber < 0) {
		if (linenumber != floor(linenumber)) {
			sprintf(buffer, "Negative target line %i.%i in branch", group, step);
			focal_error(buffer);
		} else {
			sprintf(buffer, "Negative target group %i.%i in branch", group, step);
			focal_error(buffer);
		}
    return NULL;
  }
	
	// in focal, the target we're looking for could be either a specific line
	// or a group number
	//
	// start with the line, which can never be x.00
	if (step != 0) {
		// check it exists
		if (interpreter_state.lines[group * 100 + step] == NULL) {
			sprintf(buffer, "Undefined target line %i.%i in branch", group, step);
			focal_error(buffer);
			return NULL;
		}
		
		// otherwise we did find a line, so return it
		return interpreter_state.lines[group * 100 + step];
	}
	// and here we look for the group
	else {
		// for the group lookup, we have to loop through it until we find an item with that value
		list_t *lv = NULL;
		for(int i = (group * 100); i < MAXLINE - 1; i++) {
			if (interpreter_state.lines[i] != NULL) {
				// we found a non-empty line, is it the right group?
				if (linenumber == trunc(i / 100)) {
					// we found the first line in the group, so we can exit
					lv = interpreter_state.lines[i];
					break;
				}
			}
		}
		if (lv != NULL) {
			return lv;
		}
		else {
			sprintf(buffer, "Undefined target line %i in branch", group);
			focal_error(buffer);
			return NULL;
		}
	}
	
	// failsafe
	sprintf(buffer, "Undefined target line %i.%i in branch", group, step);
	focal_error(buffer);
	return NULL;
}

/** Runs a single statement, like ASK or TYPE
 *
 * @param L A pointer to the list item in the program to perform.
 */
static void perform_statement(list_t *list_item)
{
	statement_t *statement = list_item->data;
	if (statement) {
		switch (statement->type) {
				
			case ASK:
			{
				// ASK is similar to BASIC's INPUT, and allows mixing prompts and inputs.
				// It also has the option of printing a colon, like BASIC's question mark,
				// but does so for every input, not just the first
				//
				// one difference with BASIC: entering nothing will return zero
				
				// loop over the items in the variable/prompt list
				for (list_t *I = statement->parms.input; I != NULL; I = lst_next(I)) {
					// get the item itself
					printitem_t *ppi = I->data;
					
					// if there is no expression, or there is an expression but it's not
					// a variable, then this is part of the prompt and we just want to "print" it
					if (ppi->expression == NULL || ppi->expression->type != variable) {
						print_item(ppi);
					}
					// if it is a variable, get the input
					else {
						char line[80];
						either_t *value;
						int type = 0;
						
						// print the colon if that option is turned on
						if (ask_colon)	
							printf(":");
						
						// see if we can get some data, we should at least get a return
						fflush(stdout);
						if (fgets(line, sizeof(line), stdin) != line)
							exit(EXIT_FAILURE);
						
						// we got something, so null-terminate the string
						line[strlen(line) - 1] = '\0';
						
						// optionally (almost always) convert to upper case
						if (upper_case) {
							char *c = line;
							while (*c) {
								*c = toupper((unsigned char) *c);
								c++;
							}
						}
						
						// trim any leading spaces
						char *trim = line;
						while(isspace(trim[0]))
							trim++;

						// find the storage for this variable
						value = variable_value(ppi->expression->parms.variable, &type);
						
						// FOCAL only has numeric variables, but it does have the ability to
						// type in strings at prompts, so we have to hand-convert the string
						// into a value, we can't simply sscanf it
						value->number = string_to_number(trim);
					}
				} // loop over list of items
			} // ASK
				break;
				
			case DO:
			{
				// DO is a GOSUB which may call a line or a group
				stackentry_t *new_do = malloc(sizeof(*new_do));
				
				new_do->type = DO;
				new_do->original_line = current_line();
				new_do->target_line = evaluate_expression(statement->parms._do).number;
				new_do->returnpoint = lst_next(list_item);
				interpreter_state.stack = lst_append(interpreter_state.stack, new_do);
				interpreter_state.next_statement = find_line(evaluate_expression(statement->parms._do).number);
			}
				break;
				
			case ERASE:
				// clears out variable values
				delete_variables();
				break;
				
			case FOR:
			{
				stackentry_t *new_for = malloc(sizeof(*new_for));
				either_t *loop_value;
				int type = 0;
				
				new_for->type = FOR;
				new_for->original_line = current_line();
				new_for->index_variable = statement->parms._for.variable;
				new_for->begin = evaluate_expression(statement->parms._for.begin).number;
				new_for->end = evaluate_expression(statement->parms._for.end).number;
				if (statement->parms._for.step != NULL)
					new_for->step = evaluate_expression(statement->parms._for.step).number;
				else {
					new_for->step = 1;
				}
				new_for->head = list_item;
				loop_value = variable_value(new_for->index_variable, &type);
				loop_value->number = new_for->begin;
				
				interpreter_state.stack = lst_append(interpreter_state.stack, new_for);
			}
				break;
				
			case GOTO:
			{
				interpreter_state.next_statement = find_line(evaluate_expression(statement->parms.go).number);
			}
				break;
				
			case IF:
			{
				value_t cond = evaluate_expression(statement->parms._if.condition);
				/* in contrast to BASIC, FOCAL uses the FORTRAN-like model where all comparisons are
				 mathematical and the branch is based on whether the result of the comparison is
				 -ve, 0 or +ve. The 0 and +ve branches are optional. If either is missing, that case
				 is accomplished by running any remaining statements on the line (like BASIC in that
				 respect). This leads to some complexity... */
				if (cond.number < 0 && statement->parms._if.less_line > 0) {
					interpreter_state.next_statement = find_line(statement->parms._if.less_line);
				}
				else if (cond.number == 0 && statement->parms._if.zero_line > 0) {
					interpreter_state.next_statement = find_line(statement->parms._if.zero_line);
				}
				else if (cond.number > 0 && statement->parms._if.more_line > 0) {
					interpreter_state.next_statement = find_line(statement->parms._if.more_line);
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
				stored_val = variable_value(statement->parms.set.variable, &type);
				
				// evaluate the expression
				exp_val = evaluate_expression(statement->parms.set.expression);
				
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
								
			case TYPE:
			{
				// loop over the items in the print list and print them out
				for (list_t *item = statement->parms.print; item != NULL; item = lst_next(item)) {
					print_item(item->data);
				}
			}
				break;
				
			case RETURN:
			{
				stackentry_t *se;
				if (lst_length(interpreter_state.stack) == 0 || ((stackentry_t *)lst_last_node(interpreter_state.stack)->data)->type != DO) {
					focal_error("RETURN without DO");
					break;
				}
				
				se = lst_last_node(interpreter_state.stack)->data;
				interpreter_state.next_statement = se->returnpoint;
				interpreter_state.stack = lst_remove_node_with_data(interpreter_state.stack, se);
			}
				break;
				
			case VARLIST:
				print_variables();
				break;
				
			default:
				printf("Unimplemented statement %d\n", statement->type);
				exit(0);
		} //end switch
		
		// because of the way that FOCAL handles FOR loops and DO calls,
		// we have to test whether or not we are the last statement on the line,
		// or at the last statement of a group. if so, we need to determine where
		// to go next, which might be back to the start of a FOR, or doing the
		// equivalent of a RETURN
		double this_line = current_line();
		list_t *next = lst_next(list_item);
		
		// we might be at the end of the program
		double next_line;
		if (next != NULL)
			next_line = line_for_statement(next);
		else
			next_line = -1.0;

		int this_group = trunc(this_line);
		int this_step = (this_line - this_group) * 100;
		int next_group = trunc(next_line);
		int next_step = (next_line - next_group) * 100;
		
		// are we at the end of a group or a line?
		if (this_group != next_group || this_step != next_step) {
			if (lst_length(interpreter_state.stack) > 0) {
				// pull the top item
				stackentry_t *se = lst_last_node(interpreter_state.stack)->data;
				
				// if it's a FOR, we perform a next if we are at the end of any line
				if (se->type == FOR && (this_group != next_group || this_step != next_step)) {
					int type = 0;
					either_t *lv = variable_value(se->index_variable, &type);
					lv->number += se->step;

					// and see if we need to go back to the FOR or we're done and we continue on
					if (((se->step < 0) && (lv->number >= se->end)) ||
							((se->step > 0) && (lv->number <= se->end))) {
						// we're not done, go back to the head of the loop
						interpreter_state.next_statement = lst_next(se->head);
					} else {
						// we are done, remove this entry from the stack and just keep going
						interpreter_state.stack = lst_remove_node_with_data(interpreter_state.stack, se);
						free(se);
					}
					
				}
				// or it might be a DO, in which case we have to check the original
				// target to see if it was a group or single line
				else {
					int target_group = trunc(se->target_line);
					int target_step = (se->target_line - target_group)  * 100;
					
					// if the original DO had only a group, only do the RETURN if we are at the end of this group
					if (target_step == 0 && (this_group != next_group)) {
						interpreter_state.next_statement = se->returnpoint;
						interpreter_state.stack = lst_remove_node_with_data(interpreter_state.stack, se);
					}
					// if it had a group and statement, then only return if both changed
					else if (this_group != next_group && this_step != next_step) {
						interpreter_state.next_statement = se->returnpoint;
						interpreter_state.stack = lst_remove_node_with_data(interpreter_state.stack, se);
					}
				}

			}
		} // end-of-line handling
	} // statement is not null
} /* perform_statement */

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
static void delete_lines() {
  for(int i = MAXLINE - 1; i >= 0; i--) {
    if (interpreter_state.lines[i] != NULL) {
      lst_free(interpreter_state.lines[i]);
    }
  }
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
  interpreter_state.first_line_index = first_line;
  
  // a program runs from the first line, so...
  interpreter_state.current_statement = first_statement;          // the first statement
}

/* the main loop for the program */
void interpreter_run(void)
{
  // the cursor starts in col 0
  interpreter_state.cursor_column = 0;
	
	// the normal format is similar
	interpreter_state.format = 0.0;

  // start the clock and mark us as running
  start_ticks = clock();
	gettimeofday(&start_time, NULL);
  interpreter_state.running_state = 1;
	
	// and set the reset time to now as well
	gettimeofday(&reset_time, NULL);
  
  // last line number we ran, used for tracing/stepping
  int last_line = interpreter_state.first_line_index;
  if (trace_lines)
    printf("[%i]\n", last_line);
  
  // very simple - perform_statement returns the next statement so we just keep
	// looping over perform_statement until it returns a NULL
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
