%{
/*
 *
 * config_parser.l
 *	by joe osburn
 *
 * Parses configuration file
 *
 */

#include "conf.h"
#include "vo.h"
#include "y.tab.h"

int num_lines = 0;

%}

%x COMMENT
%%

\[[^\[\n]*[\]\n]	{ yytext[yyleng-2] = 0;
			yylval.string = strdup(yytext+1);
			printf("Loading %s configuration\n", yylval.string); }
[a-zA-Z0-9_./\\\[\]]+	{ yylval.string = strdup(yytext); return STRING; }
[\t ]		/* ignore whitespace */ ;
#		BEGIN COMMENT;
\n		num_lines++;
.		return yytext[0];
<COMMENT>\n	BEGIN INITIAL;
<COMMENT>.	;

%%
