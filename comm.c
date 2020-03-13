/*
 *
 * comm.c
 *	by joe osburn
 *
 */

#include "conf.h"
#include "vo.h"

fd_set		readfds, writefds, excfds;
tcp_socket	main_socket;
int		max_fds=0;

extern struct s_player players[];

//
// setup_dfs
//
// Sets up fd_set with appriopriate fds
//
int setup_fds()
{
	int player;

	FD_ZERO(&readfds);		// clear the fd_set
	FD_ZERO(&writefds);
	FD_ZERO(&excfds);
	FD_SET(main_socket,&readfds);	// Always want the main_socket for incoming connections

	if (main_socket >= max_fds)
		max_fds = main_socket+1;

	// Loop through and add any connected players
	for (player = 0; player < MAX_PLAYERS; player++) {
		if (players[player].state >= state_ingame) {
			if (players[player].comms.sock >= max_fds)
				max_fds = players[player].comms.sock+1;
			FD_SET(players[player].comms.sock, &readfds);
			FD_SET(players[player].comms.sock, &writefds);
			FD_SET(players[player].comms.sock, &excfds);
		}
	}

	return 0;
}

//
// tcp_init
//
// Bind to tcp port and listen for connections
//
void tcp_init(int port)
{
	struct sockaddr_in address;
	int opt = TRUE;

	extern char *get_param(char *name);

	vo_log(log_debug, "Going to create socket, bind, and listen");

	if ((main_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		vo_die("Error in socket()");

	if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
		vo_die("Error in setsockopt()");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(main_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
		vo_die("Error binding to port %d", port);

	if (listen(main_socket, atoi(get_param("tcp.listen"))) < 0)
		vo_die("Error setting tcp listen value to %s", get_param("tcp.listen"));

	vo_log(log_info, "Server listening on port %d", port);
}


//
// tcp_destroy
//
// Shuts down main connection (no more connections can be made to server)
//
int tcp_destroy()
{
	vo_log(log_debug, "Closing main socket");

	shutdown(main_socket,2);

	return 0;
}


//
// tcp_close
//
// Close socket
//
int tcp_close(tcp_socket sock)
{
	vo_log(log_debug, "Closing tcp connection");

	shutdown(sock, 2);
	
	return 0;
}

//
// tcp_disconnect
//
// Disconnect specified player socket
//
int tcp_disconnect(struct s_player *disconnectee)
{
	if (disconnectee->state != state_npc && disconnectee->state != state_disconnected) {
		vo_log(log_info, "Disconnecting %s at %s",disconnectee->name, disconnectee->comms.ip_address);

		tcp_close(disconnectee->comms.sock);
	}

	disconnectee->comms.output_size = 0;

	return 0;
}

//
// connection_waiting
//
// Returns TRUE if there is an incoming connection
//
int connection_waiting()
{
	fd_set incomingfds;
	struct timeval sleep_time;

	sleep_time.tv_sec  = 0;
	sleep_time.tv_usec = 0;

	FD_ZERO(&incomingfds);
	FD_SET(main_socket, &incomingfds);

	select(main_socket+1,&incomingfds,(void *)0,(void *)0, &sleep_time);

	if (FD_ISSET(main_socket, &incomingfds)) {
		vo_log(log_debug, "Found connection(s) waiting");
		return TRUE;
	}

	return FALSE;
}

//
// tcp_accept
//
// Returns new incoming socket connection
// Sets address string with ip address of remote connection
//
tcp_socket tcp_accept(char *address)
{
	extern char *get_param(char *name);

	tcp_socket	new_socket;
	int		addrlen;
	int 		flags;
	int 		nodelay_opt	= FAST_PACKETS;		// set in vo.h

	struct sockaddr_in	addr;

	addrlen = sizeof(addr);

	new_socket = accept(main_socket, (struct sockaddr *)&addr, &addrlen);
	vo_log(log_info,"Connection established with %s",inet_ntoa(*(struct in_addr*)&addr.sin_addr));

	sprintf(address,"%s", inet_ntoa(*(struct in_addr*)&addr.sin_addr));

	// Set NODELAY on or off (FAST_PACKETS)
	setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay_opt, sizeof(int));
	
	// turn off blocking on socket
	flags = fcntl(new_socket, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if (fcntl(new_socket, F_SETFL, flags) < 0) {
		vo_die("error turning on nonblock for socket");
	}

	vo_log(log_debug, "New socket: %d", new_socket);

	return new_socket;
}

//
// tcp_rawsend
//
// Sends raw data to socket
//
int tcp_rawsend(tcp_socket sock, char *data, int len)
{
	int bytes_left = len;
	int bytes_sent = 0;
	int sent = 0;

/*	int i;

	vo_log(log_debug, "packet header:\nSentinel %d\nLength %d\nCRC %d\nType %d\nCommand %d\nDATA\n",
	(char)data[0], (unsigned short)data[1], (unsigned short)data[3], (unsigned char)data[5], (unsigned char)data[6]);

	for (i = sizeof(struct packet_header); i < len; i++)
		vo_log(log_debug, "%d ", data[i]);
*/

	do {
		sent = send(sock,data+bytes_sent,len-bytes_sent,0);
		bytes_sent += sent;
		bytes_left -= sent;

		if (sent < 1) {
			// This should never happen
			if (sent == 0) {
				vo_log(log_system, "send() returned 0.  This should not be possible");
				return 0;
			}

			// We don't mind these errors
			else if (errno == EAGAIN)
				return 0;
			else if (errno == EWOULDBLOCK)
				return 0;

			return sent;
		}

	} while (bytes_left > 0);

	return bytes_sent;
}

//
// tcp_read
//
// Reads data from socket into buffer
//
int tcp_read(tcp_socket sock, char *writeto, int max)
{
	int read;

	read = recv(sock, writeto, max, 0);

	if (read > 0) {
		return read;
	} else if (read == 0) {
		vo_log(log_notice, "EOF on read, socket closing");
		return -1;
	} else if (errno == EINTR) {
		return 0;
	} else if (errno == EAGAIN) {
		return 0;
	} else if (errno == EWOULDBLOCK) {
		return 0;
	} else if (errno == ECONNRESET) {
		return -1;
	} else if (errno == EBADF) {
		return -1;
	} else if (errno == ENOTSOCK) {
		return -1;
	} else if (errno == EINVAL) {
		return -1;
	} else if (errno == EPIPE) {
		return -1;
	} else if (errno == EFAULT) {
		vo_log(log_system, "Received EFAULT in tcp_read() on recv()");
		return -1;
	} else {
		perror("tcp_read");
	}

	return -1;
}
