/*
 *
 * log.c
 *	by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

FILE	*logs[MAX_LOGS];
int	log_levels[MAX_LOGS];
int	num_logs = 0;

const char *log_strings[] = {
	"",
	"DEBUG:",
	"NOTICE:",
	"INFO:",
	"WARNING:",
	"ERROR:",
	"ALERT:",
	"FATAL:",
	"SYSTEM:"
	};

int vo_log(enum log_level level, char *str, ...)
{
	char	msg[SMALL_BUFFER];
	int	i;
	int	real_level;
	va_list	argptr;
        char timebuf[32];
        time_t current_time = time(0);

	extern char *mapname;

        strftime(timebuf,32,"%b %d %H:%M:%S %Y",localtime(&current_time));

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);

	real_level = pow(2, (level-1));
	if (real_level == 0)
		real_level = 1;

	// If no logfiles are open, just print to screen
	if (num_logs < 1 && level != log_debug) {
		if (level > 0)
			printf("%s %s:%s %s\n", timebuf, mapname?mapname:"", log_strings[level], msg);
		else
			printf("%s %s\n", timebuf, msg);

		return 0;
	}

	// Go through each logfile and print to it if it includes the right log levels
	for (i = 0; i < num_logs; i++) {
		if ((log_levels[i] & real_level) == real_level) {
			if (level > 0)
				fprintf(logs[i], "%s %s:%s %s\n", timebuf, mapname?mapname:"", log_strings[level], msg);
			else
				fprintf(logs[i], "%s %s\n", timebuf, msg);

			fflush(logs[i]);
		}
	}

	return 0;
}

void init_log()
{
	int i, j;
	char *filename;
	char *level;
	char param[32];

	extern char *get_param(char *name);
	extern int param_exists(char *name);

	for (i = 0; i < MAX_LOGS; i++)
		logs[i] = 0;

	i = 0;

	sprintf(param, "general.logfile[%d]", i);
	while (param_exists(param)) {
		filename = get_param(param);
		if (!strcmp(filename, "stdout")) {
			logs[i] = stdout;
		} else if (!strcmp(filename, "stderr")) {
			logs[i] = stderr;
		} else if (!strcmp(filename, "stdin")) {
			logs[i] = stdin;
		} else {
			logs[i] = fopen(filename,"w");

			if (!logs[i]) {
				printf("Unable to open logfile %s for writing", filename);
				vo_die("Exiting due to logfile failure");
			}
		}

		sprintf(param, "general.loglevel[%d]", i);
		level = get_param(param);

		if (!level) {
			vo_log(log_error, "Unable to find LogLevel configuration option for %s", filename);
			vo_die("Exiting due to logfile failure");
		}

		for (j = 0; j < strlen(level); j++) {
			switch (level[j]) {
				case 'd':
					log_levels[i]+=1;
					break;
				case 'n':
					log_levels[i]+=2;
					break;
				case 'i':
					log_levels[i]+=4;
					break;
				case 'w':
					log_levels[i]+=8;
					break;
				case 'e':
					log_levels[i]+=16;
					break;
				case 'a':
					log_levels[i]+=32;
					break;
				case 'f':
					log_levels[i]+=64;
					break;
				case 's':
					log_levels[i]+=128;
					break;
				default:
					vo_log(log_error, "Unknown loglevel '%c'", level[j]);
					break;
			}
		}

		i++;

		vo_log(log_info, "Starting logfile %s", filename);
		sprintf(param, "general.logfile[%d]", i);

		if (i >= MAX_LOGS)
			break;
	}

	// set num_logs equal to the number of logs we opened
	num_logs = i;
}

void finish_log() {
	int i;

	vo_log(log_debug, "Closing logfiles");

	for (i = 0; i < num_logs; i++) {
		if (logs[i] != stdout && logs[i] != stdin && logs[i] != stderr)
			fclose(logs[i]);
	}
}

int vo_die(char *str, ...)
{
	va_list argptr;
	char msg[SMALL_BUFFER];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);

	vo_log(log_fatal, msg);

        disconnect_all();

        tcp_destroy();
        destroy_sql();

	exit(-1);

	return 0;
}
