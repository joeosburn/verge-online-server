/*
 *
 * packet.c
 *         by joe osburn
 *
 * This contains the functions which handle identifying and
 * processing incoming packets
 *
 */

#include "conf.h"
#include "vo.h"

extern struct s_player players[];

//
// process_packet
//
// Processes packet from player
//
int process_packet(int player, struct packet pkt)
{
	// verify the packet crc is correct
//	if (!packet_crc(pkt))
//		die("INVALID PACKET RECEIVED; BAD CRC OUCH");

	// if they've just connected, the only kind of packet they should be sending is a system packet
	if ((players[player].state == state_username || players[player].state == state_password) && pkt.header.type != P_SYSTEM) {
		disconnect_player(player);
		vo_log(log_notice, "Disconnecting player at %s; bad packets on login", players[player].comms.ip_address);

		return 0;
	}

	// which type of packet is this?
	switch(pkt.header.type) {
		case P_SYSTEM:	// system packet
			process_packet_system(player, &pkt);
			break;
		case P_OBJECT:	// object packet
			process_packet_object(player, &pkt);
			break;
		case P_TEXT:	// text packet
			process_packet_text(player, &pkt);
			break;
		case P_EVENT:	// event packet
			process_packet_event(player, &pkt);
			break;
		case P_RAW:	// raw packet
			break;
		default:
			vo_log(log_alert, "Unknown packet type %d received from %s", pkt.header.type, players[player].name);
			break;
	}

	return 0;
}

//
// packet_crc
//
// Returns TRUE if packet pkt has valid crc header
//
int packet_crc(struct packet pkt)
{
	if (crc(pkt.data, NTOHS(pkt.header.length)-sizeof(struct packet_header)) != NTOHS(pkt.header.crc))
		return FALSE;

	return TRUE;
}

//
// crc
//
// Returns the crc of data for length bytes
//
int crc(char *data, int length)
{
	int count = 0;
	int i;

	for (i = 0; i < length; i++, count+=data[i]);

	return count;
}
