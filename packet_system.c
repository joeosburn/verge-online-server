/*
 *
 * packet_system.c
 *         by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];


//
// create_packet_disconnect
//
// Creates a new disconnect packet
//
struct packet *create_packet_disconnect(struct packet *new_packet)
{
	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_header));
	new_packet->header.crc		= 0;
	new_packet->header.type		= P_SYSTEM;
	new_packet->header.command	= C_DISCONNECT;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_ping
//
// Creates a new ping packet
//
struct packet *create_packet_ping(struct packet *new_packet)
{
	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_header));
	new_packet->header.crc		= 0;
	new_packet->header.type		= P_SYSTEM;
	new_packet->header.command	= C_PING;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}
