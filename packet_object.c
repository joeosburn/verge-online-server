/*
 *
 * packet_object.c
 *         by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];


//
// create_packet_pmove
//
// Creates a new player move packet
//
struct packet *create_packet_pmove(struct packet *new_packet, unsigned short player, unsigned char direction, unsigned char speed,
			unsigned short startx, unsigned short starty)
{
	struct packet_object_pmove *move_packet = (struct packet_object_pmove *)new_packet->data;

	// copy move parameters into packet
	move_packet->playerid	= HTONS(player);
	move_packet->direction	= direction;
	move_packet->speed	= speed;
	move_packet->startx	= HTONS(startx);
	move_packet->starty	= HTONS(starty);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_object_pmove) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(move_packet)));
	new_packet->header.type		= P_OBJECT;
	new_packet->header.command	= C_PMOVE;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_pcreate
//
// Creates a new player create packet
//
struct packet *create_packet_pcreate(struct packet *new_packet, unsigned short player, unsigned short costume,
							unsigned short x, unsigned short y, char *username)
{
	struct packet_object_pcreate *create_packet = (struct packet_object_pcreate *)new_packet->data;

	// copy create parameters into packet
	create_packet->playerid		= HTONS(player);
	create_packet->costumeid	= costume;
	create_packet->x		= HTONS(x);
	create_packet->y		= HTONS(y);
	strcpy(create_packet->username, username);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_object_pcreate) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(create_packet)));
	new_packet->header.type		= P_OBJECT;
	new_packet->header.command	= C_PCREATE;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_pdestroy
//
// Creates a new player destroy packet
//
struct packet *create_packet_pdestroy(struct packet *new_packet, unsigned short player)
{
	struct packet_object_pdestroy *destroy_packet = (struct packet_object_pdestroy *)new_packet->data;

	// set destroy parameters into packet
	destroy_packet->playerid	= HTONS(player);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_object_pdestroy) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(destroy_packet)));
	new_packet->header.type		= P_OBJECT;
	new_packet->header.command	= C_PDESTROY;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_pid
//
// Creates a new pid (player identification packet
//
struct packet *create_packet_pid(struct packet *new_packet, unsigned short player, unsigned short x, unsigned short y, char costume)
{
	struct packet_object_pid *pid_packet = (struct packet_object_pid *)new_packet->data;

	extern char *mapname;

	// set identification parameters into packet
	pid_packet->playerid		= HTONS(player);
	pid_packet->x			= HTONS(x);
	pid_packet->y			= HTONS(y);
	pid_packet->costumeid		= HTONS(costume);

	memcpy(new_packet->data+sizeof(struct packet_object_pid), mapname, strlen(mapname));

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_object_pid) + sizeof(struct packet_header) + strlen(mapname));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(pid_packet)));
	new_packet->header.type		= P_OBJECT;
	new_packet->header.command	= C_PID;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_warp
//
// Creates a new warp packet
//
struct packet *create_packet_pwarp(struct packet *new_packet, unsigned short player, unsigned short x, unsigned short y, unsigned char facing)
{
	struct packet_object_pwarp *warp_packet = (struct packet_object_pwarp *)new_packet->data;

	// set warp parameters
	warp_packet->playerid		= HTONS(player);
	warp_packet->x			= HTONS(x);
	warp_packet->y			= HTONS(y);
	warp_packet->facing		= facing;

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_object_pwarp) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(warp_packet)));
	new_packet->header.type		= P_OBJECT;
	new_packet->header.command	= C_PWARP;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}


//
// create_packet_pupdate
//
// Creates a new player update packet
//
struct packet *create_packet_pupdate(struct packet *new_packet, unsigned short player, unsigned short x, unsigned short y)
{
	struct packet_object_pupdate *update_packet = (struct packet_object_pupdate *)new_packet->data;

	// set update parameters
	update_packet->playerid		= HTONS(player);
	update_packet->x		= HTONS(x);
	update_packet->y		= HTONS(y);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_object_pupdate) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(update_packet)));
	new_packet->header.type		= P_OBJECT;
	new_packet->header.command	= C_PWARP;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}
