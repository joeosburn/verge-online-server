/*
 *
 * player.c
 *         by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

//
// add_player
//
// Adds socket to player list
//
int add_player(int sock, char *address)
{
	int player;
	struct packet disconnect;

	extern int time_to_live;

	vo_log(log_debug, "Adding new player connection for %s", address);

	player = next_free_player();

	// make sure there is free slots
	if (player < 0) {
		create_packet_disconnect(&disconnect);
		tcp_rawsend(sock, (char *)&disconnect, sizeof(struct packet_header));
		tcp_close(sock);

		vo_log(log_notice,"No free slots available; disconnecting new player at %s", address);

		return 0;
	} else if (time_to_live > 0) {
		create_packet_disconnect(&disconnect);
		tcp_rawsend(sock, (char *)&disconnect, sizeof(struct packet_header));
		tcp_close(sock);

		vo_log(log_info, "Server shutdown command issued.  Connection from %s denied", address);

		return 0;
	}

	players[player].comms.sock = sock;
	strcpy(players[player].comms.ip_address, address);

	players[player].comms.input_size	= 0;
	players[player].comms.output_size	= 0;
	players[player].comms.incoming_idle	= 0;
	players[player].comms.outgoing_idle	= 0;
	players[player].state			= state_username;
	players[player].x			= 0;
	players[player].y			= 0;
	players[player].costume			= 0;

	strcpy(players[player].name, "???");

	return 0;
}

int authenticate_player(char *username, char *password, int player)
{
	struct packet pkt;
	struct s_server s;

	extern char *mapname;

	if (sql_authenticate(username, password, &players[player])) {
		if (!strcmp(players[player].map, mapname)) {
			vo_log(log_info, "Player %s logged in", players[player].name);
			return TRUE;
		} else {
			vo_log(log_info, "Player %s valid login but wrong map; forwarding", players[player].name);
			sql_getserver(players[player].map, &s);
			player_send(player, create_packet_switchmap(&pkt, s));
		}
	} else {
		vo_log(log_notice, "Invalid login attempt of '%s' from %s", players[player].name, players[player].comms.ip_address);
	}

	return FALSE;
}

int player_join(int player)
{
	struct packet pkt;
	int i;

	extern char announcement[];
	extern unsigned long announcement_time;

	vo_log(log_debug, "Player %s entering game world", players[player].name);

	// create and send all players packet notifying them of new player
	create_packet_pcreate(&pkt, player, players[player].costume, players[player].x, players[player].y, players[player].name);
	player_send_all_except(player, &pkt);

	// create and send player pcreate packets for all other players
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (player != i && (players[i].state == state_ingame || players[i].state == state_npc)) {
			create_packet_pcreate(&pkt, i, players[i].costume, players[i].x, players[i].y, players[i].name);
			player_send(player, &pkt);
		}
	}

	// create and send player identification packet to new player
	create_packet_pid(&pkt, player, players[player].x, players[player].y, players[player].costume);
	player_send(player, &pkt);

	// send them the current announcement, if any
	if (announcement_time > 0 && strlen(announcement) > 0) {
		create_packet_announcement(&pkt, announcement_time/10, announcement);
		player_send(player, &pkt);
	}

	return 0;
}

int next_free_player()
{
	return pop_player();
}

int disconnect_all()
{
	int i;
	struct packet disconnect;

	vo_log(log_debug, "Disconnecting all players");

	create_packet_disconnect(&disconnect);

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].state >= state_ingame) {
			player_send(i, &disconnect);
			player_flush(i);
			tcp_disconnect(&players[i]);
			players[i].state = state_disconnected;
		}
	}

	return 0;
}

//
// disconnect_player
//
// Use this function when you want a disconnect packet to be automatically sent
// This function also calls remove_player() 
// If you do not want a disconnect packet sent, use remove_player() by itself
//
int disconnect_player(int player)
{
	struct packet fin;

	if (players[player].state >= state_ingame)
		player_send(player, create_packet_disconnect(&fin));

	player_flush(player);

	return remove_player(player);
}

//
// remove_player
//
// This function does not send a disconnect packet
// Use disconnect_player() for that
//
int remove_player(int player)
{
	struct packet pkt;

	if (players[player].state != state_npc && players[player].state < state_ingame) {
		vo_log(log_debug, "Rejecting player %d with state %d for removal", player, players[player].state);
		return -1;
	}

	vo_log(log_debug, "Removing player %s", players[player].name);

	tcp_disconnect(&players[player]);

	if (players[player].state == state_ingame || players[player].state == state_npc) {
		create_packet_pdestroy(&pkt, player);
		player_send_all_except(player, &pkt);
	}

	if (players[player].state == state_ingame) {
		sql_saveplayer(&players[player]);
	}

	if (players[player].state != state_npc)
		push_player(player);
	else if (players[player].state == state_npc)
		push_npc(player);

	players[player].state = state_disconnected;

	return 0;
}


//
// process player
//
// Processes incoming data buffer for player
//
int process_player(int player)
{
	// process any packets if available
	if (players[player].comms.input_size >= sizeof(struct packet_header)) {
		struct packet current_packet;

		memcpy(&current_packet.header, players[player].comms.input, sizeof(struct packet_header));

		if (current_packet.header.sentinel != VO_ID) {
			vo_log(log_notice, "Received packet not beginning with correct sentinel");

			disconnect_player(player);
		} else if (current_packet.header.length > PACKET) {
			vo_log(log_notice, "Received packet from player '%s' too large, length was: %d", players[player].name, current_packet.header.length);

			// disconnect the offending player
			disconnect_player(player);
		} else if (current_packet.header.length <= players[player].comms.input_size) {
			memcpy(current_packet.data, players[player].comms.input+sizeof(struct packet_header), current_packet.header.length-sizeof(struct packet_header));
			
			if (current_packet.header.length == players[player].comms.input_size) {
				players[player].comms.input_size = 0;
			} else {
				memcpy(players[player].comms.input, players[player].comms.input+current_packet.header.length, players[player].comms.input_size-current_packet.header.length);
				players[player].comms.input_size-=current_packet.header.length;
			}

			process_packet(player, current_packet);
		}
	}

	return 0;
}

//
// return_player
//
// Returns id of player with username
//
int return_player(char *username)
{
	int returnv = -1;
	int i;

	if (strlen(username) < 1)
		return returnv;

	for (i = 0; i < MAX_ENTITIES; i++) {
		if (!strcmp(username,players[i].name) && players[i].state != state_disconnected)
			returnv = i;
	}

	return returnv;
}

int player_stack[MAX_PLAYERS];
int npc_stack[MAX_NPCS];
int player_counter;
int npc_counter;

int pop_player()
{
	player_counter--;

	if (player_counter<0)
		return -1;

	vo_log(log_debug, "Popping player %d from stack", player_stack[player_counter]);

	return player_stack[player_counter];
}

int pop_npc()
{
	npc_counter--;

	if (npc_counter<0)
		return -1;

	return npc_stack[npc_counter];
}

int push_player(int player)
{
	vo_log(log_debug, "Pushing player %d to stack", player);

	player_counter++;

	if (player_counter > MAX_PLAYERS)
		vo_die("Player stack overrun");
	player_stack[player_counter-1] = player;

	return player_counter-1;
}

int push_npc(int npc)
{
	npc_counter++;

	if (npc_counter > (MAX_ENTITIES-MAX_PLAYERS))
		vo_die("NPC stack overrun");

	npc_stack[npc_counter-1] = npc;

	return npc_counter-1;
}

//
// init_players
//
// Initializes player and npc stack, and general player settings
//
int init_players()
{
	int i, j;

	vo_log(log_debug, "Initializing player array");

	for (i = 0; i < MAX_ENTITIES; i++)
		players[i].state = state_disconnected;

	for (i = 0, j = 0; i < MAX_PLAYERS; i++, j++)
		player_stack[i] = j; 

	player_counter = i;

	for (i = 0; i < MAX_ENTITIES-MAX_PLAYERS; i++, j++)
		npc_stack[i] = j;

	npc_counter = i;

	// init all player mutexes
	for (i = 0; i < MAX_PLAYERS; i++) {
		pthread_mutex_init(&players[i].lock, 0);
	}

	return 0;
}
