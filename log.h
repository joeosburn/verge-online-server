/*
 *
 * log.h
 *	by joe osburn
 *
 */

#ifndef _LOG_H_

enum log_level {
	log_debug=1,
	log_notice,
	log_info,
	log_warning,
	log_error,
	log_alert,
	log_fatal,
	log_system
};

int vo_log(enum log_level level, char *str, ...);
int vo_die(char *str, ...);
void init_log();
void finish_log();

#define _LOG_H_
#endif
