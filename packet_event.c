/*
 *
 * packet_event.c
 *         by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];


//
// create_packet_switchmap
//
// Creates a new switchmap packet
//
struct packet *create_packet_switchmap(struct packet *new_packet, struct s_server server)
{
	struct packet_event_switchmap *sm_packet = (struct packet_event_switchmap *)new_packet->data;

	// copy server parameters into packet
	sm_packet->port		= HTONS(server.port);
	sm_packet->ipaddress	= HTONL(inet_network(server.ip_address));

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_event_switchmap) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(struct packet_event_map_vc)));
	new_packet->header.type		= P_EVENT;
	new_packet->header.command	= C_SWITCHMAP;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}

//
// create_packet_map_vc
//
// Creates a event packet for map vc events
//
struct packet *create_packet_map_vc(struct packet *new_packet, unsigned short player, unsigned short event)
{
	struct packet_event_map_vc *vc_packet = (struct packet_event_map_vc *)new_packet->data;

	// copy event parameters into packet
	vc_packet->playerid	= HTONS(player);
	vc_packet->eventid	= HTONS(event);

	// set packet header data
	new_packet->header.length	= HTONS(sizeof(struct packet_event_map_vc) + sizeof(struct packet_header));
	new_packet->header.crc		= HTONS(crc(new_packet->data, sizeof(struct packet_event_map_vc)));
	new_packet->header.type		= P_EVENT;
	new_packet->header.command	= C_MAP_VC;
	new_packet->header.sentinel	= VO_ID;

	return new_packet;
}
