#
# makefile for RetroFOCAL
#
#
# various setup
LEX = flex
LFLAGS = -lfl
YAC = bison
YFLAGS =-dtv
CC = gcc -g
CFLAGS = -DYYDEBUG=1
CLIBS = -ly -ll

rm=/bin/rm -f
mv=/bin/mv -f

# our program name
TARGET = retrofocal

# the final program has three inputs, the lex/yacc and the interpreter source
$(TARGET): $(wildcard src/*.c) parse.tab.c lex.yy.c
	$(CC) -Isrc $^ -o $(TARGET) -lm

# if the lex or .tab.h file is changed, run lex again
lex.yy.c: src/scan.l parse.tab.h
	$(LEX) $(LEXFLAGS) $<

# If the yacc file is changed, run yacc again.
parse.tab.c parse.tab.h: src/parse.y
	$(YAC) $(YFLAGS) $<

clean:
	$(rm) $(TARGET) $(TARGET).o
	$(rm) *.tab.h *.tab.c *.lex.c

# Detect platform for install behavior
ifeq ($(OS),Windows_NT)
    PREFIX ?= $(PROGRAMFILES)\RetroFOCAL
    BINDIR := $(PREFIX)\bin
    DOCDIR := $(PREFIX)\doc
    MANDIR := $(PREFIX)\man
else
    PREFIX ?= /usr/local
    BINDIR ?= $(PREFIX)/bin
    MANDIR ?= $(PREFIX)/share/man
    DOCDIR ?= $(PREFIX)/share/doc/retrofocal
endif

.PHONY: install uninstall

install: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo Installing RetroFOCAL to $(PREFIX)...
	@if not exist "$(BINDIR)" mkdir "$(BINDIR)"
	@if not exist "$(DOCDIR)" mkdir "$(DOCDIR)"
	@if not exist "$(MANDIR)" mkdir "$(MANDIR)"
	copy /Y "$(TARGET)" "$(BINDIR)\"
	copy /Y "docs\\retrofocal.1" "$(MANDIR)\" 2>nul || true
	@echo Copying docs...
	robocopy docs "$(DOCDIR)\\docs" /S /E /Y 2>nul || true
	robocopy examples "$(DOCDIR)\\examples" /S /E /Y 2>nul || true
	@echo Installation complete!
else
	@echo "Installing RetroFOCAL to $(PREFIX)..."
	mkdir -p $(BINDIR) $(MANDIR)/man1 $(DOCDIR)
	install -m 755 $(TARGET) $(BINDIR)/
	install -m 644 docs/retrofocal.1 $(MANDIR)/man1/ 2>/dev/null || true
	cp -r docs $(DOCDIR)/
	cp -r examples $(DOCDIR)/
	@echo "Installation complete!"
endif

uninstall:
ifeq ($(OS),Windows_NT)
	@echo Uninstalling RetroFOCAL from $(PREFIX)...
	@if exist "$(BINDIR)\$(TARGET).exe" del /Q "$(BINDIR)\$(TARGET).exe"
	@if exist "$(MANDIR)\retrofocal.1" del /Q "$(MANDIR)\retrofocal.1"
	@if exist "$(DOCDIR)" rmdir /S /Q "$(DOCDIR)"
	@echo Uninstall complete!
else
	@echo "Uninstalling RetroFOCAL from $(PREFIX)..."
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(MANDIR)/man1/retrofocal.1
	rm -rf $(DOCDIR)
	@echo "Uninstall complete!"
endif

all: retrofocal
