/*
 *
 * svr_cmds.c
 *         by joe osburn
 *
 *	Functions for processing and handling incoming administrative commands
 */

#include "conf.h"
#include "vo.h"

extern int	time_to_live;
extern struct	s_player players[];

SVRCMD(cmd_addnpc);
SVRCMD(cmd_announce);
SVRCMD(cmd_costume);
SVRCMD(cmd_disconnect);
SVRCMD(cmd_shutdown);
SVRCMD(cmd_switchmap);
SVRCMD(cmd_uptime);
SVRCMD(cmd_who);
SVRCMD(cmd_nothing);

const struct s_command svr_commands[] = {
	{ "/addnpc",		cmd_addnpc,	ACCESS_ADMIN },
	{ "/announce",		cmd_announce,	ACCESS_ADMIN },
	{ "/costume",		cmd_costume,	ACCESS_ADMIN },
	{ "/dc",		cmd_disconnect,	ACCESS_ADMIN },
	{ "/disconnect",	cmd_disconnect,	ACCESS_ADMIN },
	{ "/setcostume",	cmd_costume,	ACCESS_ADMIN },
	{ "/shutdown",		cmd_shutdown,	ACCESS_ADMIN },
	{ "/switchmap",		cmd_switchmap,	ACCESS_USER },
	{ "/uptime",		cmd_uptime,	ACCESS_USER },
	{ "/who",		cmd_who,	ACCESS_USER },
	{ "\n",			cmd_nothing,	ACCESS_NONE } };

// processes cmdline for player
int server_command(int player, char *cmdline)
{
	char command[SMALL_BUFFER];
	char *argument;
	int cmd, length;

	argument = first_word(cmdline, command);

	for (length = strlen(command), cmd = 0; *svr_commands[cmd].command != '\n'; cmd++) {
		if (!strncmp(svr_commands[cmd].command, command, length) && players[player].access >= svr_commands[cmd].access) {
			svr_commands[cmd].svr_command(player, argument);
			return 1;
		}
	}

	return 0;
}

// server commands

// Creates new NPC (non-mobile)
SVRCMD(cmd_addnpc)
{
	int x,y;
	char buf[SMALL_BUFFER];

	if (!argument) {
		player_message(player, "Usage: /addnpc <x> <y> <name>");
		return -1;
	}

	argument = first_word(argument, buf);
	if (!argument || !buf) {
		player_message(player, "Usage: /addnpc <x> <y> <name>");
		return -1;
	}
	if (!is_number(buf)) {
		player_message(player, "Usage: /addnpc <x> <y> <name>");
		return -1;
	}
	x = atoi(buf);

	argument = first_word(argument, buf);
	if (!argument || !buf) {
		player_message(player, "Usage: /addnpc <x> <y> <name>");
		return -1;
	}
	if (!is_number(buf)) {
		player_message(player, "Usage: /addnpc <x> <y> <name>");
		return -1;
	}
	y = atoi(buf);

	new_npc(0, x, y, argument);

	player_message(player, "NPC %s created", argument);

	return 0;
}

// Sets announcement message for specified time
SVRCMD(cmd_announce)
{
	struct packet announce;
	int time;
	char buf[SMALL_BUFFER];

	extern char announcement[];
	extern unsigned long announcement_time;

	if (!argument)
		return -1;

	argument = first_word(argument, buf);

	if (!argument || !buf) {
		player_message(player, "Usage: /announce <seconds> <message>");
		return -1;
	}

	if (!is_number(buf)) {
		player_message(player, "Usage: /announce <seconds> <message>");
		return -1;
	}

	time = atoi(buf);
	vo_log(log_info, "%s changes announcement to %s for %d seconds", players[player].name, argument, time);

	create_packet_announcement(&announce, time, argument);
	player_send_all(&announce);

	player_message(player, "Announcement set: %s", argument);

	strcpy(announcement, argument);
	announcement_time = time*10;

	return 0;
}

// sets costume id for player
SVRCMD(cmd_costume)
{
	int costume;
	int plyr;
	char buf[SMALL_BUFFER];

	if (!argument || strlen(argument) < 1) {
		player_message(player, "Usage: /setcostume <player name> <costume id>");
		return -1;
	}

	argument = first_word(argument, buf);

	if (!argument || !is_number(argument) || !buf || strlen(argument) < 1 || strlen(buf) < 1) {
		player_message(player, "Usage: /setcostume <player name> <costume id>");
		return -1;
	}

	costume = atoi(argument);


	if ((plyr = return_player(buf)) > -1) {
		players[plyr].costume = costume;

		// send a message to the executer of the command that it was sucessful
		player_message(player, "Player %s's costume set to %d", buf, costume);
		vo_log(log_info, "Player %s changed %s's costume id to %d", players[player].name, players[plyr].name, costume);
	} else {
		player_message(player, "Player %s not found", buf);
	}

	return 0;
}

// disconnects player or removes npc
SVRCMD(cmd_disconnect)
{
	struct packet disconnect;
	int dc_player;

	if (!argument) {
		player_message(player, "Usage: /disconnect <player name>");
		return -1;
	}

	if ((dc_player = return_player(argument)) > -1) {
		vo_log(log_info, "%s disconnects %s", players[player].name, players[dc_player].name);

		if (players[dc_player].state != state_npc && players[dc_player].state != state_disconnected) {
			create_packet_disconnect(&disconnect);
			player_send(dc_player, &disconnect);
			player_flush(dc_player);
		}

		remove_player(dc_player);
		player_message(player, "Player %s disconnected", players[dc_player].name);
	} else {
		player_message(player, "Player %s not found", argument);
	}

	return 0;
}

// shuts down server in specified seconds and sets announcement message
SVRCMD(cmd_shutdown)
{
	struct packet announce;
	int time;
	char buf[SMALL_BUFFER];

	if (!argument)
		return -1;

	argument = first_word(argument, buf);

	if (!argument || !buf) {
		player_message(player, "Usage: /shutdown <seconds> <message>");
		return -1;
	}

	if (!is_number(buf)) {
		player_message(player, "Usage: /shutdown <seconds> <message>");
		return -1;
	}

	time = atoi(buf);
	vo_log(log_notice, "%s issues shutdown command (%s) in %d seconds", players[player].name, argument, time);

	create_packet_announcement(&announce, time, argument);
	player_send_all(&announce);

	time_to_live = time*10;		// this value is in milliseconds

	player_message(player, "Shutdown initiated: %d seconds", time);

	return 0;
}

// initiates a switchmap for player
SVRCMD(cmd_switchmap)
{
	int x,y;
	struct s_server server_info;
	struct packet pkt;
	char buf[SMALL_BUFFER];

	if (!argument || strlen(argument) < 1) {
		player_message(player, "Usage: /switchmap <x> <y> <mapname>");
		return -1;
	}

	argument = first_word(argument, buf);
	if (!argument || !buf) {
		player_message(player, "Usage: /switchmap <x> <y> <mapname>");
		return -1;
	}
	if (!is_number(buf)) {
		player_message(player, "Usage: /switchmap <x> <y> <mapname>");
		return -1;
	}
	x = atoi(buf);

	argument = first_word(argument, buf);
	if (!argument || !buf || strlen(argument) < 1 || strlen(buf) < 1) {
		player_message(player, "Usage: /switchmap <x> <y> <mapname>");
		return -1;
	}
	if (!is_number(buf)) {
		player_message(player, "Usage: /switchmap <x> <y> <mapname>");
		return -1;
	}
	y = atoi(buf);

	if (!sql_getserver(argument, &server_info)) {
		player_message(player, "Map %s not found", argument);
		vo_log(log_notice, "Player %s requested unknown map %s", players[player].name, argument);
		return -1;
	}

	vo_log(log_info, "Player %s switching to map %s at %d,%d", players[player].name, argument, x, y);

	sql_update_player_map(players[player].name, server_info);
	players[player].x = x*STEP;
	players[player].y = y*STEP;

	player_send(player, create_packet_switchmap(&pkt, server_info));
	
	disconnect_player(player);

	return 0;
}

// returns listing and status of all currently connected players
SVRCMD(cmd_who)
{
	int i;

	for (i = 0; i < MAX_ENTITIES; i++) {
		switch(players[i].state) {
			case state_username:
				player_message(player, "%s: USERNAME", players[i].name);
				break;
			case state_password:
				player_message(player, "%s: PASSWORD", players[i].name);
				break;
			case state_ingame:
				player_message(player, "%s: IN GAME", players[i].name);
				break;
			case state_npc:
				player_message(player, "%s: NPC", players[i].name);
				break;
			default:
		}
	}

	return 0;
}

// Returns current server update (broken)
SVRCMD(cmd_uptime)
{
	time_t current_time;
	struct tm *display_time;

	extern time_t starttime;

	current_time = time((void *)0) - starttime;
	display_time = gmtime(&current_time);

	player_message(player, "Server has been up %d years, %d months, %d hours, %d minutes and %d seconds",
		display_time->tm_year, display_time->tm_mon, display_time->tm_hour, display_time->tm_min, display_time->tm_sec);

	return 0;
}

SVRCMD(cmd_nothing)
{
	return 0;
}
