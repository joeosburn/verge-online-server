/*
 *
 * svr_cmds.h
 *         by joe osburn
 *
 */

#ifndef _SVR_CMDS_H_

#define SVRCMD(name)	int name(int player, char *argument)

struct s_command {
	const char *command;
	int (*svr_command) (int player, char *argument);
	int access;
};


// svr_cmds.c
int server_command(int player, char *cmdline);

#define _SVR_CMDS_H_
#endif
