/*
 *
 * proc_text.c
 *         by joe osburn
 *
 * This contains the functions which handle identifying and
 * processing incoming text packets
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

int process_packet_text(int player, struct packet *pkt)
{
	switch(pkt->header.command) {
		case C_SAY:
			process_say(player, pkt);
			break;
		case C_WHISPER:
			break;
		case C_SHOUT:
			break;
		case C_EMOTE:
			process_emote(player, (struct packet_text_emote *)pkt->data);

		// these packets should not be sent by player.  They will be ignored
		case C_ANNOUNCEMENT:
			break;
		case C_CONSOLE:
			break;
		default:
			vo_log(log_alert, "Unknown text packet command %d received from player %s", pkt->header.command, players[player].name);
			break;
	}

	return 0;
}

int process_emote(int player, struct packet_text_emote *pkt)
{
	unsigned short emoteid;
	struct packet emote_pkt;

	emoteid = NTOHS(pkt->emote_id);

	player_send_all_except(player, create_packet_emote(&emote_pkt, player, emoteid));

	return 0;
}

int process_say(int player, struct packet *pkt)
{
	struct packet text_packet;
	struct packet_text *tmp;
	char *text;
	int length;

	tmp = (struct packet_text *)pkt->data;

	if (player != tmp->playerid)
		return 0;

	length = pkt->header.length-(sizeof(struct packet_header)+sizeof(struct packet_text));
	text = (char *)alloca(length+1);
	memcpy(text, pkt->data+sizeof(struct packet_text), length);

	text[length] = 0;

	create_packet_say(&text_packet, player,text);


	// Check if this is a server command (svr_cmds.c)
	if (text[0] == '/') {
		if (server_command(player, text))
			return 0;
	}

	vo_log(log_debug, "%s says %s", players[player].name, text);
	player_send_all_except(player, &text_packet);

	return 0;
}
