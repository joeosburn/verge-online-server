/*
 *
 * npc.c
 *	by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

int next_free_npc()
{
	return pop_npc();
}

int new_npc(char costumeid, int x, int y, char *name)
{
	int npc;
	struct packet pkt;

	npc = next_free_npc();

	players[npc].state	= state_npc;
	players[npc].x		= x;
	players[npc].y		= y;
	players[npc].costume	= costumeid;

	strcpy(players[npc].name, name);

	create_packet_pcreate(&pkt, npc, costumeid, x, y, name);

	player_send_all(&pkt);

	vo_log(log_info, "NPC %s created", name);

	return 0;
}

int npc_tick()
{
	if (rand() < RAND_MAX/NPC_MOVE);

	return 0;
}
