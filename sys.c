/*
 *
 * sys.c
 *	by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern char _vo;

void catchsigint(int sig);
void catchsigterm(int sig);

void init_system()
{
	struct sigaction act;
	
	(void) signal(SIGINT, catchsigint);
	(void) signal(SIGTERM, catchsigterm);
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGPIPE, &act, 0);
}

void catchsigint(int sig)
{
	_vo = FALSE;
	vo_log(log_debug, "Caught SIGINT");
}

void catchsigterm(int sig)
{
	_vo = FALSE;
	vo_log(log_debug, "Caught SIGTERM");
}

char *strlower(char *str, char *dest)
{
	for (strcpy(dest, str); *dest; *dest=tolower(*dest), dest++);

	return dest;
}

///////////////////////////////////////////////////////
// NOTE:
// All functions below
// are stolen from other programs or GPLed code.
// They will need to be rewritten at some point
////////////////////////////////////////////////////////

/* Return the difference between two times */
void timediff(struct timeval *rslt, struct timeval *a, struct timeval *b)
{
        struct timeval null_time;
 
        null_time.tv_sec = 0;
        null_time.tv_usec = 0;
 
        if (a->tv_sec < b->tv_sec)
                *rslt = null_time;
        else if (a->tv_sec == b->tv_sec) {
                if (a->tv_usec < b->tv_usec)
                        *rslt = null_time;
                else {
                        rslt->tv_sec = 0;
                        rslt->tv_usec = a->tv_usec - b->tv_usec;
                }
        } else { /* a->tv_sec > b->tv_sec */
                rslt->tv_sec = a->tv_sec - b->tv_sec;
                if (a->tv_usec < b->tv_usec) {
                        rslt->tv_usec = a->tv_usec + 1000000 - b->tv_usec;
                        rslt->tv_sec--;
                } else
                        rslt->tv_usec = a->tv_usec - b->tv_usec;
        }
}

char *first_word(char *argument, char *first_arg)
{
	skip_spaces(&argument);

	while (*argument && !isspace(*argument)) {
		*(first_arg++) = *argument;
		argument++;
	}

	*first_arg = '\0';

	skip_spaces(&argument);

	return argument;
}


/*
 * Function to skip over the leading spaces of a string.
 */
void skip_spaces(char **string)
{
	for (; **string && isspace(**string); (*string)++);
}

int is_number(const char *str)
{
	while (*str)
		if (!isdigit(*(str++)))
			return 0;

	return 1;
}
