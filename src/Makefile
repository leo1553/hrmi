# Executables
FLEX=flex
BISON=bison

################################################################################################################################
# Compiler
COMPILER_SCANNER=compiler/scanner
COMPILER_PARSER=compiler/parser

all-before: build_scanner

build_scanner: build_parser
	$(FLEX) -o$(COMPILER_SCANNER).yy.c $(COMPILER_SCANNER).l

build_parser:
	$(BISON) -d -v -o $(COMPILER_PARSER).tab.c $(COMPILER_PARSER).y

clean-custom:
	$(RM) -f $(COMPILER_PARSER).output \
          	 $(COMPILER_PARSER).tab.c \
          	 $(COMPILER_PARSER).tab.h \
          	 $(COMPILER_SCANNER).yy.c


