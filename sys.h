/*
 *
 * sys.h
 *	by joe osburn
 *
 */


#ifndef _SYS_H_

// sys.c
void init_system();
void timediff(struct timeval *rslt, struct timeval *a, struct timeval *b);
char *first_word(char *argument, char *first_arg);
char *strlower(char *str, char *dest);
void skip_spaces(char **string);
int is_number(const char *str);

#define _SYS_H_
#endif
