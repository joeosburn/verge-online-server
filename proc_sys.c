/*
 *
 * proc_sys.c
 *         by joe osburn
 *
 * This contains the functions which handle identifying and
 * processing incoming packets
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

int process_packet_system(int player, struct packet *pkt)
{
	switch(pkt->header.command) {
		case C_DISCONNECT:
			process_disconnect(player, pkt);
			break;
		case C_USERNAME:
			process_username(player, (struct packet_username *)pkt->data);
			break;
		case C_PASSWORD:
			process_password(player, (struct packet_password *)pkt->data);
			break;
		case C_PING:
			process_ping(player, pkt);
			break;
		default:
			vo_log(log_alert, "Unknown system packet command %d received from player %s", pkt->header.command, players[player].name);
			break;
	}

	return 0;
}


int process_disconnect(int player, struct packet *pkt)
{
	vo_log(log_info, "Player %s leaving game", players[player].name);
	remove_player(player);

	return 0;
}

int process_ping(int player, struct packet *pkt)
{
	// don't do anything for ping packets
	return 0;
}

int process_username(int player, struct packet_username *pkt)
{
	int i;

	for (i = 0; i < 17; i++) {
		if (pkt->username[i] == VO_ID) {
			pkt->username[i] = 0;
			break;
		}
	}

	pkt->username[17] = 0;

	strcpy(players[player].name, pkt->username);

	players[player].state = state_password;

	return 0;
}

int process_password(int player, struct packet_password *pkt)
{
	int i;

	for (i = 0; i < 17; i++) {
		if (pkt->password[i] == VO_ID) {
			pkt->password[i] = 0;
			break;
		}
	}

	if (authenticate_player(players[player].name, pkt->password, player)) {
		players[player].state = state_ingame;
		player_join(player);
	} else {
		disconnect_player(player);
	}

	return 0;
}

