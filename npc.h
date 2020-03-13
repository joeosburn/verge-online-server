/*
 *
 * npc.h
 *	by joe osburn
 *
 */

#ifndef _NPC_H_

// npc.h
int next_free_npc();
int new_npc(char costumeid, int x, int y, char *name);
int npc_tick();

#define _NPC_H_
#endif
