/*
 *
 * comm.h
 *	by joe osburn
 *
 */


#ifndef _COMM_H_

// comm.c
int setup_fds();
void tcp_init(int port);
int tcp_destroy();
int tcp_close(tcp_socket sock);
int tcp_disconnect(struct s_player *disconnectee);
int connection_waiting();
tcp_socket tcp_accept(char *address);
int tcp_rawsend(tcp_socket sock, char *data, int len);
int tcp_read(tcp_socket sock, char *writeto, int max);

#define _COMM_H_
#endif
