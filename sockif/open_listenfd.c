/*
 * filename: open_listenfd.c
 */
#include <sys/socket.h>

int open_listenfd(int port)
{
	int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	/* Create a socket descriptor */
        /* int socket(int domain, int type, int protocol); */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	
	/* Eliminates "Address already in use" error from bind */
