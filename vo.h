/*
 *
 * vo.h
 *      by joe osburn
 *
 */


// If you make changes to this file, the server is not guarenteed to continue working.
// Change values in conf.h if you want to change settings and have things just work.
// vo.conf is an even better option.

#ifndef _VO_H_

#define VERSION		"0.2alpha"

#define TRUE		1
#define FALSE		!TRUE

#define PACKED		__attribute__((packed))

#define VO_ID		0x07

#define MAX_ENTITIES	MAX_PLAYERS+MAX_NPCS
#define MAX_FDS		(MAX_PLAYERS) + 5

#define SMALL_BUFFER	512	// small buffer
#define BUFFER		1024	// buffer
#define LARGE_BUFFER	2048	// large buffer

#define PULSE		100000	// one tenth of a second
#define PACKET		128
#define FAST_PACKETS	1	// setting this to 1 will disable nagle algorithm

#define STEP		16	// number of pixels in a step

// directions for movement
#define UP		1
#define DOWN		2
#define LEFT		3
#define RIGHT		4
#define UPLEFT		5
#define DNLEFT		6
#define UPRIGHT		7
#define DNRIGHT		8


//////////////////////////////////////////
// The target client platform is win32.  If the server
// platform is different, these conversion functions need
// to be defined appropriately

/* Linux x86 */
#define HTONS(x)	x
#define HTONL(x)	x
#define NTOHS(x)	x
#define NTOHL(x)	x

//////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <search.h>
#include <time.h>

typedef int tcp_socket;

enum player_state {
	/* disconnected states */
	state_undefined = 0,	// what?
	state_npc,		// the player is an npc
	state_disconnected,	// the player is not connected

	/* connected states */
	state_ingame,		// the player is in the game
	state_username,		// the player needs to send a username
	state_password		// the player needs to send a password
};


struct tcp_comm {
	tcp_socket	sock;

	char		input[SMALL_BUFFER];
	unsigned short	input_size;

	char		output[BUFFER];
	unsigned short	output_size;

	char		ip_address[33];

	unsigned short	incoming_idle;
	unsigned short	outgoing_idle;
};

struct s_player {
	int	x,y;
	int	access;
	char	costume;
	char	name[17];
	char	map[32];

	enum player_state	state;
	pthread_mutex_t		lock;
	struct tcp_comm		comms;
};

struct s_server {
	char	ip_address[33];
	int	port;
	char	mapfile[33];
};

#include "comm.h"
#include "log.h"
#include "map.h"
#include "npc.h"
#include "packet.h"
#include "player.h"
#include "sql.h"
#include "svr_cmds.h"
#include "sys.h"

#define _VO_H_
#endif
