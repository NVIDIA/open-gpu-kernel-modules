/* Simple expression parser */
%{
#define YYDEBUG 1
#include <stdio.h>
#include "util.h"
#include "util/debug.h"
#include <stdlib.h> // strtod()
#define IN_EXPR_Y 1
#include "expr.h"
#include "smt.h"
#include <string.h>

static double d_ratio(double val0, double val1)
{
	if (val1 == 0) {
		return 0;
	}
	return  val0 / val1;
}

%}

%define api.pure full

%parse-param { double *final_val }
%parse-param { struct expr_parse_ctx *ctx }
%parse-param {void *scanner}
%lex-param {void* scanner}

%union {
	double	 num;
	char	*str;
}

%token EXPR_PARSE EXPR_OTHER EXPR_ERROR
%token <num> NUMBER
%token <str> ID
%destructor { free ($$); } <str>
%token MIN MAX IF ELSE SMT_ON D_RATIO
%left MIN MAX IF
%left '|'
%left '^'
%left '&'
%left '<' '>'
%left '-' '+'
%left '*' '/' '%'
%left NEG NOT
%type <num> expr if_expr

%{
static void expr_error(double *final_val __maybe_unused,
		       struct expr_parse_ctx *ctx __maybe_unused,
		       void *scanner,
		       const char *s)
{
	pr_debug("%s\n", s);
}

%}
%%

start:
EXPR_PARSE all_expr
|
EXPR_OTHER all_other

all_other: all_other other
|

other: ID
{
	expr__add_id(ctx, $1);
}
|
MIN | MAX | IF | ELSE | SMT_ON | NUMBER | '|' | '^' | '&' | '-' | '+' | '*' | '/' | '%' | '(' | ')' | ','
|
'<' | '>' | D_RATIO

all_expr: if_expr			{ *final_val = $1; }
	;

if_expr:
	expr IF expr ELSE expr { $$ = $3 ? $1 : $5; }
	| expr
	;

expr:	  NUMBER
	| ID			{
					struct expr_id_data *data;

					if (expr__resolve_id(ctx, $1, &data)) {
						free($1);
						YYABORT;
					}

					$$ = expr_id_data__value(data);
					free($1);
				}
	| expr '|' expr		{ $$ = (long)$1 | (long)$3; }
	| expr '&' expr		{ $$ = (long)$1 & (long)$3; }
	| expr '^' expr		{ $$ = (long)$1 ^ (long)$3; }
	| expr '<' expr		{ $$ = $1 < $3; }
	| expr '>' expr		{ $$ = $1 > $3; }
	| expr '+' expr		{ $$ = $1 + $3; }
	| expr '-' expr		{ $$ = $1 - $3; }
	| expr '*' expr		{ $$ = $1 * $3; }
	| expr '/' expr		{ if ($3 == 0) {
					pr_debug("division by zero\n");
					YYABORT;
				  }
				  $$ = $1 / $3;
	                        }
	| expr '%' expr		{ if ((long)$3 == 0) {
					pr_debug("division by zero\n");
					YYABORT;
				  }
				  $$ = (long)$1 % (long)$3;
	                        }
	| '-' expr %prec NEG	{ $$ = -$2; }
	| '(' if_expr ')'	{ $$ = $2; }
	| MIN '(' expr ',' expr ')' { $$ = $3 < $5 ? $3 : $5; }
	| MAX '(' expr ',' expr ')' { $$ = $3 > $5 ? $3 : $5; }
	| SMT_ON		 { $$ = smt_on() > 0; }
	| D_RATIO '(' expr ',' expr ')' { $$ = d_ratio($3,$5); }
	;

%%
