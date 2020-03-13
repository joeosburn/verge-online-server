/*
 *
 * proc_object.c
 *         by joe osburn
 *
 * This contains the functions which handle identifying and
 * processing incoming objectpackets
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

int process_packet_object(int player, struct packet *pkt)
{
	switch(pkt->header.command) {
		case C_PMOVE:
			process_pmove(player, (struct packet_object_pmove *)pkt->data);
			break;
		case C_PCREATE:
			break;
		case C_PDESTROY:
			break;
		case C_PWARP:
			process_pwarp(player, (struct packet_object_pwarp *)pkt->data);
			break;
		case C_PID:
			break;
		default:
			vo_log(log_alert, "Unknown object packet command %d received from player %s", pkt->header.command, players[player].name);
			break;
	}

	return 0;
}

int process_pwarp(int player, struct packet_object_pwarp *pkt)
{
	unsigned short x, y;
	unsigned char facing;
	struct packet warp;

	if (player != NTOHS(pkt->playerid))
		return -1;

	x	= NTOHS(pkt->x);
	y	= NTOHS(pkt->y);
	facing	= pkt->facing;

	players[player].x = x;
	players[player].y = y;

	create_packet_pwarp(&warp, player, x, y, facing);

	player_send_all_except(player, &warp);

	return 0;
}	

int process_pmove(int player, struct packet_object_pmove *pkt)
{
	char direction, speed;
	unsigned short startx, starty;
	struct packet update_packet;

	// Players can only send move packets for their player id
	if (player != NTOHS(pkt->playerid))
		return -1;

	direction	= pkt->direction;
	speed		= pkt->speed;
	startx		= NTOHS(pkt->startx);
	starty		= NTOHS(pkt->starty);

	// validate their starting position
	if (startx != players[player].x || starty != players[player].y) {
		player_send(player, create_packet_pwarp(&update_packet, player, players[player].x, players[player].y, DOWN));
		return -1;
	}

	switch (direction) {
		case UP:
			if (get_obstruct_at(startx, starty-STEP)) {
				create_packet_pwarp(&update_packet, player, startx, starty, UP);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].y = starty-STEP;
			players[player].x = startx;
			break;
		case DOWN:
			if (get_obstruct_at(startx, starty+STEP)) {
				create_packet_pwarp(&update_packet, player, startx, starty, DOWN);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].y = starty+STEP;
			players[player].x = startx;
			break;
		case LEFT:
			if (get_obstruct_at(startx-STEP, starty)) {
				create_packet_pwarp(&update_packet, player, startx, starty, LEFT);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].x = startx-STEP;
			players[player].y = starty;
			break;
		case RIGHT:
			if (get_obstruct_at(startx+STEP, starty)) {
				create_packet_pwarp(&update_packet, player, startx, starty, RIGHT);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].x = startx+STEP;
			players[player].y = starty;
			break;
		case UPLEFT:
			if (get_obstruct_at(startx-STEP, starty-STEP)) {
				create_packet_pwarp(&update_packet, player, startx, starty, LEFT);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].y = starty-STEP;
			players[player].x = startx-STEP;
			break;
		case DNLEFT:
			if (get_obstruct_at(startx-STEP, starty+STEP)) {
				create_packet_pwarp(&update_packet, player, startx, starty, LEFT);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].y = starty+STEP;
			players[player].x = startx-STEP;
			break;
		case UPRIGHT:
			if (get_obstruct_at(startx+STEP, starty-STEP)) {
				create_packet_pwarp(&update_packet, player, startx, starty, RIGHT);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].y = starty-STEP;
			players[player].x = startx+STEP;
			break;
		case DNRIGHT:
			if (get_obstruct_at(startx+STEP, starty+STEP)) {
				create_packet_pwarp(&update_packet, player, startx, starty, RIGHT);
				player_send(player, &update_packet);
				return -1;
			}
			players[player].y = starty+STEP;
			players[player].x = startx+STEP;
			break;
	}


	create_packet_pmove(&update_packet, player, direction, speed, startx, starty);
	player_send_all_except(player, &update_packet);

	return 0;
}
