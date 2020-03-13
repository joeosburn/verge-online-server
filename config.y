%{
/*
 *
 * config.y
 *	by joe osburn
 *
 * Reads and understands data parsed from config file
 *
 */

#include "conf.h"
#include "vo.h"

int init_config(char *filename);
int set_param(char *name, char *value);
char *get_param(char *name);
int yyerror(char *s);

extern int yylex();

%}

%token YYERROR_VERBOSE

%union {
	char *string;
}

%token <string> STRING
%token <string> HEADER
%%

statements	:
		| statements statement
		;

statement	: STRING '=' STRING { set_param($1, $3); free($1); free($3); }
		;
%%

extern FILE *yyin;

int init_config(char *filename)
{
	if (!hcreate(MAX_PARAMETERS))
		vo_die("Unable to create hash table for parameters");

	// Set default config values
	set_param("general.logfile",	GENERAL_LOGFILE);
	set_param("general.mapfile",	GENERAL_MAPFILE);
	set_param("tcp.timeout", 	TCP_TIMEOUT);
	set_param("tcp.port",		TCP_PORT);
	set_param("tcp.listen",		TCP_INCOMING);

	yyin = fopen(filename, "r");

	if (!yyin) {
		vo_die("Unable to open configuration file %s", filename);
	}

	// grab any config values
	yyparse();

	fclose(yyin);

	return 0;
}

int set_param(char *name, char *value)
{
	char *new_name;
	ENTRY e;

	new_name = (char *)malloc(strlen(name)+1);
	strlower(name, new_name);

	e.key = new_name;
	e.data = malloc(strlen(value)+1);
	strcpy(e.data, value);

	if (!hsearch(e, ENTER)) {
		perror("set_param");
		vo_die("Error setting parameters");
	}

	return 0;
}

int param_exists(char *name)
{
	char new_name[SMALL_BUFFER];
	ENTRY e;

	strlower(name, new_name);
	e.key = new_name;

	return (int)hsearch(e, FIND);
}

char *get_param(char *name)
{
	char new_name[SMALL_BUFFER];
	ENTRY e, *ep;

	strlower(name, new_name);
	e.key = new_name;

	ep = hsearch(e, FIND);

	if (!ep)
		vo_log(log_notice, "Configuration parameter %s requested, not found", name);
	else
		return ep->data;

	return 0;
}

int yyerror(char *s)
{
	vo_log(log_error, "%s: %s", CONFFILE, s);

	vo_die("Unable to read configuration file");

	return 0;
}
