/*
 *
 * sql.h
 *	by joe osburn
 *
 */

#ifndef _SQL_H_

int init_sql();
int destroy_sql();
int sql_query(char *query, ...);
int sql_authenticate(char *username, char *password, struct s_player *player);
int sql_update_player_map(char *username, struct s_server server);
int sql_getserver(char *mapname, struct s_server *server);
void sql_saveplayer(struct s_player *player);
void sql_error(char *message);

#define _SQL_H_
#endif
