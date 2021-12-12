%option noyywrap
%option yylineno

%{
#include "scanner.yy.h"
#include "parser.tab.h"
%}

%x TEXT_SCOPE

text_char				[^\ \t\r\f\n:;,=]

%%

"input"|"inputs"		{ return INPUT; }
"output"|"outputs"		{ return OUTPUT; }
"var"|"vars"			{ return VAR; }

"inbox"|"INBOX"			{ return INBOX; }
"outbox"|"OUTBOX"		{ return OUTBOX; }
"copyfrom"|"COPYFROM"	{ return COPYFROM; }
"copyto"|"COPYTO"		{ return COPYTO; }
"add"|"ADD"				{ return ADD; }
"sub"|"SUB"				{ return SUB; }
"bumpup"|"BUMPUP"		{ return BUMPUP; }
"bumpdn"|"BUMPDN"		{ return BUMPDN; }
"jump"|"JUMP"			{ return JUMP; }
"jumpz"|"JUMPZ"			{ return JUMPZ; }
"jumpn"|"JUMPN"			{ return JUMPN; }

":"						{ return COLON; }
";"						{ return SEMICOLON; }
","						{ return COMMA; }
"="						{ return EQUAL; }

 /* custom */
"mul"|"MUL"				{ return MUL; }
"div"|"DIV"				{ return DIV; }
"mod"|"MOD"				{ return MOD; }
"jumpp"|"JUMPP"			{ return JUMPP; }
"jumpnz"|"JUMPNZ"		{ return JUMPNZ; }
"jumpeq"|"JUMPEQ"		{ return JUMPEQ; }
"jumpneq"|"JUMPNEQ"		{ return JUMPNEQ; }
"jumplt"|"JUMPLT"		{ return JUMPLT; }
"jumplteq"|"JUMPLTEQ"	{ return JUMPLTEQ; }
"jumpgt"|"JUMPGT"		{ return JUMPGT; }
"jumpgteq"|"JUMPGTEQ"	{ return JUMPGTEQ; }


{text_char}+			{	yylval.str_ptr = strdup(yytext);
							return TEXT;
						}

"--".*\n				/* Ignore comments */

[ \t\r\f\n]+			/* Ignore white spaces */

.						{	char buffer[1024];
						 	sprintf(buffer, "Unrecognized character '%s'", yytext);
							yyerror(buffer);
						}

%%

