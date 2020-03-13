/*
 *
 * player_output.c
 *         by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

int player_flush(int flushee)
{
	if (players[flushee].state != state_disconnected && players[flushee].comms.output_size > 0) {
		int sent;

		sent = tcp_rawsend(players[flushee].comms.sock,players[flushee].comms.output,players[flushee].comms.output_size);

		if (sent > 0)
			players[flushee].comms.output_size = 0;

		
                if (sent < 0) {
                        vo_log(log_notice, "send() error.  Socket closing");
                        vo_log(log_notice, "Player '%s' connection broken on send", players[flushee].name);

                        remove_player(flushee);

                        return -1;
                } else if (sent == 0) {
			vo_log(log_debug, "No data sent on send().  Socket about to close");
		}
	}

	return 0;
}

int player_message(int player, char *message, ...)
{
        va_list argptr;
        char msg[SMALL_BUFFER];
	struct packet console;

	va_start(argptr, message);
	vsprintf(msg, message, argptr);
	va_end(argptr);

	player_send(player, create_packet_console(&console, msg));

	return 0;
}

int send_player_raw(int writeto, void *data, int size)
{
	if (players[writeto].comms.output_size + size > SMALL_BUFFER)
		player_flush(writeto);

	memcpy(players[writeto].comms.output+players[writeto].comms.output_size,data,size);
	players[writeto].comms.output_size+=size;
	players[writeto].comms.outgoing_idle = 0;

	return 0;
}

int player_send(int writeto, struct packet *pkt)
{
	send_player_raw(writeto, pkt, pkt->header.length);

	return 0;
}

int player_send_all(struct packet *pkt)
{
	int i;

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].state == state_ingame)
			send_player_raw(i, pkt, pkt->header.length);
	}

	return 0;
}

int player_send_all_except(int player, struct packet *pkt)
{
	int i;

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].state == state_ingame && i != player)
			send_player_raw(i, pkt, pkt->header.length);
	}

	return 0;
}
