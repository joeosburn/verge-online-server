/*
 *
 * packet_text.c
 *         by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];


//
// create_packet_emote
//
// Creates a new emote packet
//
struct packet *create_packet_emote(struct packet *new_packet, unsigned short player, unsigned short emote)
{
	struct packet_text_emote *emote_packet = (struct packet_text_emote *)new_packet->data;

	// set emote parameters
	emote_packet->playerid	= HTONS(player);
	emote_packet->emote_id	= HTONS(emote);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_header) + sizeof(struct packet_text_emote));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(struct packet_text_emote)));
	new_packet->header.type		= P_TEXT;
	new_packet->header.command	= C_EMOTE;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_console
//
// Creates a new console packet
//
struct packet *create_packet_console(struct packet *new_packet, char *text)
{
	// copy text into packet
	memcpy(new_packet->data, text, strlen(text));

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_header) + strlen(text));
	new_packet->header.crc		= HTONS(crc(new_packet->data, strlen(text)));
	new_packet->header.type		= P_TEXT;
	new_packet->header.command	= C_CONSOLE;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_say
//
// Creates a new player say packet
//
struct packet *create_packet_say(struct packet *new_packet, unsigned short player, char *text)
{
	struct packet_text *text_packet = (struct packet_text *)new_packet->data;
	int length = strlen(text);

	// copy text parameters into packet
	text_packet->playerid	= HTONS(player);
	memcpy(new_packet->data+sizeof(struct packet_text), text, length);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_text) + sizeof(struct packet_header) + length);
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(text_packet) + length));
	new_packet->header.type		= P_TEXT;
	new_packet->header.command	= C_SAY;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}


//
// create_packet_annoucement
//
// Creates a new announcement packet
//
struct packet *create_packet_announcement(struct packet *new_packet, unsigned short time, char *message)
{
	struct packet_text_announcement *ann = (struct packet_text_announcement *)new_packet->data;

	// set announcement parameters
	ann->time = HTONS(time);
	memcpy(new_packet->data+sizeof(struct packet_text_announcement), message, strlen(message));

	// set packet header data
	new_packet->header.length 	= HTONS(sizeof(struct packet_text_announcement) + sizeof(struct packet_header) + strlen(message));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(struct packet_text_announcement)));
	new_packet->header.type		= P_TEXT;
	new_packet->header.command	= C_ANNOUNCEMENT;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}
