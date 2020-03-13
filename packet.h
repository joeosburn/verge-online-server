/*
 *
 * packet.h
 *         by joe osburn
 *
 */

#ifndef _PACKET_H_

struct packet_header {
	unsigned char	sentinel;	// vo identifier
	unsigned short	length;		// length of the packet, including header
	unsigned short	crc;		// crc of the packet
	unsigned char	type;		// type of packet
	unsigned char	command;	// command
} PACKED;

struct packet {
	struct packet_header	header;
	char			data[PACKET];
} PACKED;


// packets for objects
struct packet_object_pcreate {
	unsigned short	playerid;
	unsigned char	costumeid;
	unsigned short	x;
	unsigned short	y;
	unsigned char	username[17];
} PACKED;

struct packet_object_pdestroy {
	unsigned short	playerid;
} PACKED;

struct packet_object_pid {
	unsigned short	playerid;
	unsigned short	x;
	unsigned short	y;
	unsigned char	costumeid;
} PACKED;
 
struct packet_object_pmove {
	unsigned short	playerid;
	unsigned char	direction;
	unsigned char	speed;
	unsigned short	startx;
	unsigned short	starty;
} PACKED;


struct packet_object_pwarp {
	unsigned short	playerid;
	unsigned short	x;
	unsigned short	y;
	unsigned char	facing;
} PACKED;

struct packet_object_pupdate {
	unsigned short	playerid;
	unsigned short	x;
	unsigned short	y;
} PACKED;

// structs for text packets
struct packet_text {
	unsigned short	playerid;
} PACKED;

struct packet_text_announcement {
	unsigned short	time;
} PACKED;

struct packet_text_emote {
	unsigned short	playerid;
	unsigned short	emote_id;
} PACKED;

// packets for events
struct packet_event_map_vc {
	unsigned short	playerid;
	unsigned short	eventid;
} PACKED;

struct packet_event_switchmap {
	unsigned long ipaddress;
	unsigned short port;
} PACKED;

// packets for system
struct packet_username {
	unsigned char	username[17];
} PACKED;

struct packet_password {
	unsigned char	password[17];
} PACKED;

// no raw packet
///////
// none
// it's not here
// stop looking for one
///////////////////////

#define	FULL_PACKET (PACKET+sizeof(struct packet_header))

#define P_UNDEFINED	0
#define P_OBJECT	10
#define P_TEXT		11
#define P_EVENT		12
#define P_RAW		13
#define P_SYSTEM	14

// commands
#define C_UNDEFINED	0
#define C_DISCONNECT	1
#define C_USERNAME	2
#define C_PASSWORD	3
#define C_PING		4

// object commands
#define C_PCREATE	10
#define C_PDESTROY	11
#define C_PMOVE		14
#define C_PID		15
#define C_PWARP		16
#define C_PUPDATE	17

// text commands
#define C_SAY		75
#define C_WHISPER	76
#define C_MESSAGE	77
#define C_SHOUT		78
#define C_ANNOUNCEMENT	79
#define C_EMOTE		80
#define C_CONSOLE	81

// event commands
#define C_MAP_VC	150
#define	C_SWITCHMAP	151

// raw commands
#define C_FILE		125


// packet.c
int process_packet(int player, struct packet pkt);
int packet_crc(struct packet pkt);
int crc(char *data, int lngth);


// packet_event.c
struct packet *create_packet_map_vc(struct packet *new_packet, unsigned short player, unsigned short event);
struct packet *create_packet_switchmap(struct packet *new_packet, struct s_server server);

// packet_object.c
struct packet *create_packet_pmove(struct packet *new_packet, unsigned short player, unsigned char direction, unsigned char speed, unsigned short startx, unsigned short starty);
struct packet *create_packet_pcreate(struct packet *new_packet, unsigned short player, unsigned short costume, unsigned short x, unsigned short y, char *username);
struct packet *create_packet_pdestroy(struct packet *new_packet, unsigned short player);
struct packet *create_packet_pid(struct packet *new_packet, unsigned short player, unsigned short x, unsigned short y, char costume);
struct packet *create_packet_pwarp(struct packet *new_packet, unsigned short player, unsigned short x, unsigned short y, unsigned char facing);

// packet_text.c
struct packet *create_packet_say(struct packet *new_packet, unsigned short player, char *text);
struct packet *create_packet_announcement(struct packet *new_packet, unsigned short time, char *message);
struct packet *create_packet_console(struct packet *new_packet, char *message);
struct packet *create_packet_emote(struct packet *new_packet, unsigned short player, unsigned short emote);

// packet_system.c
struct packet *create_packet_disconnect(struct packet *new_packet);
struct packet *create_packet_ping(struct packet *new_packet);

// proc_event.c
int process_packet_event(int player, struct packet *pkt);
int process_map_vc(int player, struct packet_event_map_vc *pkt);

// proc_object.c
int process_packet_object(int player, struct packet *pkt);
int process_pwarp(int player, struct packet_object_pwarp *pkt);
int process_pmove(int player, struct packet_object_pmove *pkt);

// proc_text.c
int process_packet_text(int player, struct packet *pkt);
int process_say(int player, struct packet *pkt);
int process_emote(int player, struct packet_text_emote *pkt);

// proc_sys.c
int process_packet_system(int player, struct packet *pkt);
int process_username(int player, struct packet_username *pkt);
int process_password(int player, struct packet_password *pkt);
int process_ping(int player, struct packet *pkt);
int process_disconnect(int player, struct packet *pkt);

#define _PACKET_H_
#endif
