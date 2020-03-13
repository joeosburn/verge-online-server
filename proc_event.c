/*
 *
 * proc_event.c
 *         by joe osburn
 *
 * This contains the functions which handle identifying and
 * processing incoming event packets
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

int process_packet_event(int player, struct packet *pkt)
{
	switch(pkt->header.command) {
		case C_MAP_VC:
			process_map_vc(player, (struct packet_event_map_vc *)pkt);
			break;
		default:
			vo_log(log_alert, "Unknown event packet command %d received from player %s", pkt->header.command, players[player].name);
			break;
	}

	return 0;
}

int process_map_vc(int player, struct packet_event_map_vc *pkt)
{
	unsigned short playerid, eventid;
	struct packet vc_packet;

	playerid	= NTOHS(pkt->playerid);
	eventid		= NTOHS(pkt->eventid);

	if (player != playerid)
		return -1;

	create_packet_map_vc(&vc_packet, player, eventid);

	player_send_all_except(player, &vc_packet);

	return 0;
}
