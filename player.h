/*
 *
 * player.h
 *      by joe osburn
 *
 */

#ifndef _PLAYER_H_

// player.c
int add_player(int sock, char *address);
int authenticate_player(char *username, char *password, int player);
int player_join(int player);
int disconnect_all();
int next_free_player();
int remove_player(int player);
int disconnect_player(int player);
int process_player(int player);
int return_player(char *name);
int init_players();
int push_player(int player);
int push_npc(int npc);
int pop_player();
int pop_npc(); 

// player_output.c
int player_message(int player, char *message, ...);
int player_flush(int flushee);
int send_player_raw(int writeto, void *data, int size);
int player_send(int writeto, struct packet *pkt);
int player_send_all(struct packet *pkt);
int player_send_all_except(int player, struct packet *pkt);

#define _PLAYER_H_
#endif
