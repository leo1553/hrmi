#ifndef __INCLUDE_SCANNER_YY_H
#define __INCLUDE_SCANNER_YY_H

#include "../includes.h"
#include "ast.h"

void yyerror(char *message);
int yyparse();
int yylex();

extern FILE *yyin;
extern FILE *yyout;
extern char* yytext;
extern ast_t *yyast;

extern int yylineno;
extern char *string_value;

void ret_print(char *token_type);
void parse_input();

#endif

