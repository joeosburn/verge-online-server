/*
 *
 * sql.c
 *	by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

MYSQL *_mysql_conn;

extern struct s_player players[];

//
// init_sql
//
// Initializes MySQL DB Connection
//
int init_sql()
{
	extern char *get_param(char *name);

	_mysql_conn = mysql_init(NULL);
	
	if (!_mysql_conn) {
		vo_die("mysql_init() failed");
		return -1;
	}
	
	if (!mysql_real_connect(_mysql_conn,get_param("sql.host"),get_param("sql.user"),get_param("sql.password"),get_param("sql.db"),0,NULL,0)) {
		sql_error("Unable to connect to mysql database");
		vo_die("Unable to create database connection");
		return -1;
	}
 
	vo_log(log_debug, "Initialized and connected to %s database", get_param("sql.db"));
	
	return 0;
}

//
// destroy_sql
//
// Called to close MySQL DB connection when program ends
//
int destroy_sql()
{
	mysql_close(_mysql_conn);
	
	vo_log(log_debug, "Closing sql database connection");
	
	return 0;
}

//
// sql_query
//
// Queries MySQL database
//
int sql_query(char *query, ...)
{
        va_list argptr;
        char rquery[BUFFER];

        va_start(argptr, query);
        vsprintf(rquery, query, argptr);
        va_end(argptr);

	vo_log(log_debug, "DB Query: %s", rquery);

	return mysql_query(_mysql_conn, rquery);
}

//
// sql_update_player_map
//
// Updates a players x,y location and the map they're on
//
int sql_update_player_map(char *player, struct s_server server)
{
	sql_query("UPDATE PLAYERS SET MAP = '%s' WHERE USERNAME = '%s'", server.mapfile, player);

	return 0;
}

//
// sql_getserver
//
// Puts info about server for mapname in *server
//
int sql_getserver(char *mapname, struct s_server *server)
{
	int result = 0;

	vo_log(log_debug, "Getting server info for map %s", mapname);

	if (sql_query("SELECT IP_ADDRESS,PORT FROM SERVERS WHERE MAPFILE = '%s'", mapname) != 0) {
		sql_error("sql_getserver");
	} else {
		MYSQL_ROW	row;
		MYSQL_RES	*res;

		res = mysql_use_result(_mysql_conn);

		if ((row = mysql_fetch_row(res)) == NULL) {
			vo_log(log_alert, "Server for mapfile '%s' not found", mapname);
			result = 0;
		} else {
			result = 1;

			strcpy(server->ip_address, row[0]);
			strcpy(server->mapfile, mapname);
			server->port = atoi(row[1]);
		}

		mysql_free_result(res);
	}

	return result;
}

//
// sql_authenticate
//
// Authenticates a given username and password
//
int sql_authenticate(char *username, char *password, struct s_player *player)
{
	int result = 0;

	vo_log(log_debug, "Trying to authenticate user %s at %s", username, player->comms.ip_address);

	if (sql_query("SELECT X_POS,Y_POS,ACCESS,COSTUME,MAP FROM PLAYERS WHERE USERNAME = '%s' AND PASSWORD = PASSWORD('%s')", username, password) != 0) {
		sql_error("sql_authenticate");
	} else {
		MYSQL_ROW	row;
		MYSQL_RES	*res;

		res = mysql_use_result(_mysql_conn);

		if ((row = mysql_fetch_row(res)) == NULL) {
			result = 0;
		} else {
			result = 1;

			player->access	= atoi(row[2]);
			player->x 	= atoi(row[0]);
			player->y 	= atoi(row[1]);
			player->costume	= atoi(row[3]);
			strcpy(player->map, row[4]);
		}

		mysql_free_result(res);
	}

	return result;
}

//
// sql_saveplayer
//
// Updates a player to database
//
void sql_saveplayer(struct s_player *player)
{
	vo_log(log_debug, "Saving player %s into database", player->name);
	sql_query("UPDATE PLAYERS SET X_POS = %d,Y_POS = %d, COSTUME = %d WHERE USERNAME = '%s'", player->x, player->y, player->costume, player->name);
}

//
// sql_error
//
// Displays MySQL error in addition to specified error
//
void sql_error(char *message)
{
	vo_log(log_error, "SQL ERROR: %s: %u (%s)", message, mysql_errno(_mysql_conn), mysql_error(_mysql_conn));
}
