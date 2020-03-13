/*
 *
 * vo.c
 *	by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"
#include "map.h"

int		time_to_live		= -1;
int		tcp_timeout		= -1;
int		tcp_port		= 0;
char		announcement[PACKET]	= { 0 };
char		_vo			= TRUE;
char		inited			= FALSE;
unsigned long	announcement_time	= 0;

char		*mapname;
time_t		starttime;

struct s_player players[MAX_ENTITIES];
extern fd_set readfds, writefds, excfds;
extern int max_fds;

int breath()
{
	int i;
	struct packet pkt;

	// Update time_to_live value if it's set
	if (time_to_live > 0) {
		time_to_live--;

		// If time_to_live is 0, shutdown server
		if (time_to_live == 0) {
			vo_log(log_debug, "Shutdown timer ran out; initiating shutdown process");
			_vo = FALSE;
		}
	}

	if (!_vo)
		return -1;

	npc_tick();

	// update the announcement time
	if (announcement_time > 0) {
		announcement_time--;
		if (announcement_time == 0)
			announcement[0] = 0;
	}

	// update all players timeout values
	if (tcp_timeout > -1) {
		for (i = 0; i < MAX_PLAYERS; i++) {
			if (players[i].state >= state_ingame) {
				pthread_mutex_lock(&players[i].lock);
				players[i].comms.incoming_idle++;
				players[i].comms.outgoing_idle++;

				if (players[i].comms.incoming_idle >= tcp_timeout) {
					vo_log(log_notice, "Disconnecting %s due to inactivity", players[i].name);
					disconnect_player(i);
				}

				if (players[i].comms.outgoing_idle >= OUTGOING_IDLE)
					player_send(i, create_packet_ping(&pkt));
					
				pthread_mutex_unlock(&players[i].lock);
			}
		}
	}
	
	return 0;
}

int input()
{
	int player;
	char address[32];

	if (!_vo)
		return -1;

	// Check all players for input
	for (player = 0; player < MAX_PLAYERS; player++) {
		if (FD_ISSET(players[player].comms.sock, &readfds) && players[player].state >= state_ingame) {
			int length;
			char data[SMALL_BUFFER];

			pthread_mutex_lock(&players[player].lock);
			length = tcp_read(players[player].comms.sock, data, (SMALL_BUFFER-players[player].comms.input_size));

			if (length < 0) {
                        	vo_log(log_notice, "Player '%s' connection closing unexpectingly", players[player].name);
                        	remove_player(player);
			}

			if (length > 0) {
				memcpy(players[player].comms.input+players[player].comms.input_size, data, length);
				players[player].comms.input_size+=length;
				players[player].comms.incoming_idle = 0;
				players[player].comms.outgoing_idle = 0;
			}

			pthread_mutex_unlock(&players[player].lock);
		}
	}

	for (player = 0; player < MAX_PLAYERS; player++) {
		if (players[player].state >= state_ingame && players[player].comms.input_size > 0) {
			pthread_mutex_lock(&players[player].lock);
			process_player(player);
			pthread_mutex_unlock(&players[player].lock);
		}
	}

	// Accept any incoming connections
	while (connection_waiting())
		add_player(tcp_accept(address), address);

	return 0;
}

int output()
{
	int player;

	if (!_vo)
		return -1;

	// Flush any output waiting
	for (player = 0; player < MAX_PLAYERS; player++) {
		if (players[player].comms.output_size>0 && players[player].state != state_disconnected) {
			pthread_mutex_lock(&players[player].lock);
			player_flush(player);
			pthread_mutex_unlock(&players[player].lock);
		}
	}

	return 0;
}

//
// game
//
// Main game loop for input and output
//
int game_io()
{
	while (_vo) {
		int i;

		setup_fds();
		select(max_fds, &readfds, (void *)0, &excfds, (void *)0);

		// if any connections are set in excfds, they are disconnecting
		for (i = 0; i < MAX_PLAYERS; i++) {
			if (players[i].state >= state_ingame && FD_ISSET(players[i].comms.sock, &excfds)) {
				FD_CLR(players[i].comms.sock, &readfds);
				FD_CLR(players[i].comms.sock, &writefds);
				vo_log(log_info, "Player %s exception set; closing connection", players[i].name);
				remove_player(i);
			}
		}

		input();
		output();
	}

	return 0;
}

int startup(char *conffile)
{
	extern int init_config(char *filename);
	extern char *get_param(char *name);

	// if we've already started up, return
	if (inited)
		return -1;

	printf("VERGE Online starting up...\n");

	init_config(conffile);
	mapname = get_param("general.mapname");
	init_log();
	init_players();

	srand(getpid());

	init_sql();
	load_map(get_param("general.mapfile"));

	announcement[0]=0;		// no announcement when the world starts

	vo_log(log_info, "System loaded.  VERGE Online server starting: %d", getpid());

	if (tcp_port == 0)
		tcp_init(atoi(get_param("tcp.port")));
	else
		tcp_init(tcp_port);

	tcp_timeout = atoi(get_param("tcp.timeout"))*10;

	if (tcp_timeout == 0)
		tcp_timeout = 10;

	inited = TRUE;

	return 0;
}

void cleanup(void)
{
	finish_log();
}

void game_world(void *argument)
{
	struct timeval sleep_time, last_time, after_sleep, process_time, pulse_time;

	pulse_time.tv_sec = 0;
	pulse_time.tv_usec = PULSE;

	sleep_time.tv_sec = 0;
	sleep_time.tv_usec = PULSE;

	while (_vo) {
		gettimeofday(&last_time, (struct timezone *)0);

		select(0, (void *)0, (void *)0, (void *)0, &sleep_time);

		breath();
		output();

                gettimeofday(&after_sleep, (struct timezone*)0);
		timediff(&process_time, &last_time, &after_sleep);
		timediff(&sleep_time, &pulse_time, &process_time);
	}

	pthread_exit(0);
}

int main(int argc, char **argv)
{
	pthread_t thrGw;

	extern char *optarg;
	char option;

	while ((option = getopt(argc, argv, "p:c:vh")) != -1) {
		switch (option) {
			case 'v':
				printf("Verge Online Server version %s\n", VERSION);
				printf("(C)2001 BJ Eirich and Joe Osburn\n");
				exit(0);
				break;
			case 'c':
				startup(optarg);
				break;
			case 'p':
				tcp_port = atoi(optarg);
				break;
			case 'h':
				printf("Usage: %s [options]\n", argv[0]);
				printf("Possible options are:\n");
				printf("\t-v\t\tVersion information\n");
				printf("\t-c <file>\tAlternative configuration file\n");
				printf("\t-p <port>\tTCP/IP port to use (overrides configuration file)\n");
				printf("\t-h\t\tDisplay this help information\n");
				exit(0);
		}
	}

	startup(CONFFILE);

	// register function to be called whenever program quits
	atexit(&cleanup);

	// spawn the game world thread
	pthread_create(&thrGw, 0, (void *)&game_world, (void *)0);

	// Initialize anything system related for main thread
	init_system();

	starttime = time((void *)0);

	// start game i/o loop
	game_io();

	vo_log(log_alert, "Kill signal received; shutting down.");
	disconnect_all();

	tcp_destroy();
	destroy_sql();
	
	return 0;
}
