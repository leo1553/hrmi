%{
#include "scanner.yy.h"
#include "../interp.h"
%}

%union
{
	char *str_ptr;
	astnode_t *node;
}

/* token definition */
%token				INPUT OUTPUT VAR
%token				INBOX OUTBOX COPYFROM COPYTO ADD SUB BUMPUP BUMPDN JUMP JUMPZ JUMPN
%token				MUL DIV MOD JUMPP JUMPNZ JUMPEQ JUMPNEQ JUMPLT JUMPLTEQ JUMPGT JUMPGTEQ
%token				COLON SEMICOLON COMMA EQUAL
%token<str_ptr>		TEXT

%start program

%type<node> program

%type<node> header 
%type<node> header_inputs header_input_loop header_input
%type<node> header_outputs_optional header_outputs header_output_loop header_output 
%type<node> header_var_optional header_var var_loop var

%type<node> commands command_loop command
%type<node> label

%type<node> param_loop param

/* expression priorities and rules */

%%

 /* ---------------------------- *
  * PROGRAM						 *
  * ---------------------------- */
program
	: header commands { 
			yyast = ast_create(); 
			yyast->root = ast_create_program_node($1, $2);
		}
	;

 /* ---------------------------- *
  * HEADER					 	 *
  * ---------------------------- */
header
	: header_inputs
	  header_outputs_optional
	  header_var_optional			{ $$ = ast_create_header_node($1, $2, $3); }
	;
header_inputs
	: INPUT header_input_loop		{ $$ = $2; }
	;
header_input_loop
	: header_input SEMICOLON header_input_loop		{ $$ = ast_create_list_node($3,   $1); }
	| header_input									{ $$ = ast_create_list_node(NULL, $1); }
	;
header_input
	: param_loop					{ $$ = $1; }
	;
header_outputs_optional
	: /* empty */ 					{ $$ = NULL; }
	| header_outputs				{ $$ = $1; }
	;
header_outputs
	: OUTPUT header_output_loop		{ $$ = $2; }
	;
header_output_loop
	: header_output SEMICOLON header_output_loop	{ $$ = ast_create_list_node($3,   $1); }
	| header_output									{ $$ = ast_create_list_node(NULL, $1); }
	;
header_output
	: param_loop					{ $$ = $1; }
	;
header_var_optional
	: /* empty */ 					{ $$ = NULL; }
	| header_var					{ $$ = $1; }
	;
header_var
	: VAR var_loop					{ $$ = $2; }
	;
var_loop
	: var COMMA var_loop			{ $$ = ast_create_list_node($3,   $1); }
	| var							{ $$ = ast_create_list_node(NULL, $1); }
	;
var
	: TEXT EQUAL TEXT				{ $$ = ast_create_var_node($1, $3); }
	;

 /* ---------------------------- *
  * COMMANDS					 *
  * ---------------------------- */
commands
	: /* empty */ 					{ $$ = NULL; }
	| command_loop 					{ $$ = $1; }
	;
command_loop
	: command command_loop			{ $$ = ast_create_list_node($2,   $1); }
	| command						{ $$ = ast_create_list_node(NULL, $1); }
	;
command
	: INBOX 						{ $$ = ast_create_line_node(COMMAND_TYPE_INBOX, NULL); }
	| OUTBOX 						{ $$ = ast_create_line_node(COMMAND_TYPE_OUTBOX, NULL); }
	| COPYFROM param				{ $$ = ast_create_line_node(COMMAND_TYPE_COPYFROM, ast_create_list_node(NULL, $2)); }
	| COPYTO param					{ $$ = ast_create_line_node(COMMAND_TYPE_COPYTO, ast_create_list_node(NULL, $2)); }
	| ADD param						{ $$ = ast_create_line_node(COMMAND_TYPE_ADD, ast_create_list_node(NULL, $2)); }
	| SUB param						{ $$ = ast_create_line_node(COMMAND_TYPE_SUB, ast_create_list_node(NULL, $2)); }
	| BUMPUP param					{ $$ = ast_create_line_node(COMMAND_TYPE_BUMPUP, ast_create_list_node(NULL, $2)); }
	| BUMPDN param					{ $$ = ast_create_line_node(COMMAND_TYPE_BUMPDN, ast_create_list_node(NULL, $2)); }
	| JUMP param					{ $$ = ast_create_line_node(COMMAND_TYPE_JUMP, ast_create_list_node(NULL, $2)); }
	| JUMPZ param					{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPZ, ast_create_list_node(NULL, $2)); }
	| JUMPN param					{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPN, ast_create_list_node(NULL, $2)); }
	| label							{ $$ = ast_create_line_node(COMMAND_TYPE_LABEL, ast_create_list_node(NULL, $1)); }
	/* custom */
	| MUL param						{ $$ = ast_create_line_node(COMMAND_TYPE_MUL, ast_create_list_node(NULL, $2)); }
	| DIV param						{ $$ = ast_create_line_node(COMMAND_TYPE_DIV, ast_create_list_node(NULL, $2)); }
	| MOD param						{ $$ = ast_create_line_node(COMMAND_TYPE_MOD, ast_create_list_node(NULL, $2)); }
	| JUMPP param					{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPP, ast_create_list_node(NULL, $2)); }
	| JUMPNZ param					{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPNZ, ast_create_list_node(NULL, $2)); }
	| JUMPEQ param COMMA param		{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPEQ, ast_create_list_node(ast_create_list_node(NULL, $4), $2)); }
	| JUMPNEQ param COMMA param		{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPNEQ, ast_create_list_node(ast_create_list_node(NULL, $4), $2)); }
	| JUMPLT param COMMA param		{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPLT, ast_create_list_node(ast_create_list_node(NULL, $4), $2)); }
	| JUMPLTEQ param COMMA param	{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPLTEQ, ast_create_list_node(ast_create_list_node(NULL, $4), $2)); }
	| JUMPGT param COMMA param		{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPGT, ast_create_list_node(ast_create_list_node(NULL, $4), $2)); }
	| JUMPGTEQ param COMMA param	{ $$ = ast_create_line_node(COMMAND_TYPE_JUMPGTEQ, ast_create_list_node(ast_create_list_node(NULL, $4), $2)); }
	;
label
	: TEXT COLON 					{ $$ = ast_create_text_node($1); }
	;

 /* ---------------------------- *
  * PARAMS						 *
  * ---------------------------- */
param_loop
	: param COMMA param_loop		{ $$ = ast_create_list_node($3,   $1); }
	| param							{ $$ = ast_create_list_node(NULL, $1); }
	;
param
	: TEXT 							{ $$ = ast_create_text_node($1); }
	;

%%

void yyerror(char *message) {
	if(message != NULL && strcmp(message, "syntax error") != 0)
		fprintf(stderr, "[%d] %s\n", yylineno, message);
	else
		fprintf(stderr, "[%d] Syntax error '%s'\n", yylineno, yytext);
  	// exit(1);
}


