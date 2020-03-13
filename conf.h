#ifndef _CONF_H_

// You can modify any of the values in this file and the server should continue to work

#define MAX_PARAMETERS	50		// maximum number of configuration parameters there can be
#define MAX_PLAYERS	25		// unsigned short - max number of players that can connect
#define MAX_NPCS	25		// unsigned short - max number of npcs that can be spawned
#define MAX_LOGS	32		// maximum number of logfiles
#define NPC_MOVE	30		// the higher the value, the less often they'll move
#define ACCESS_ADMIN	100		// level you need to access admin functions
#define ACCESS_USER	10		// level for basic user commands
#define ACCESS_NONE	0		// level for no access
#define OUTGOING_IDLE	100		// the number of tenths of a second that can pass before ping packet is sent

#define	CONFFILE	"vo.conf"	// default name of configuration file

// default values-
// All of these values are default values that
// can be overridden in the configuration file
// They MUST exist for the server to compile properly.
// See config.y init_config() for more info.
#define GENERAL_LOGFILE	"vo.log"	// name of the file to write log to
#define GENERAL_MAPFILE	"towna.map"	// default map file to open
#define TCP_TIMEOUT	"60"		// In seconds
#define	TCP_INCOMING	"5"		// number of incoming connections to listen for
#define TCP_PORT	"7593"		// port to run on

#define _CONF_H_
#endif
