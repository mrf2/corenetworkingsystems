/*
 * filename: open_clientfd.c
 * typedef sturct sockaddr SA;
 */

#include <strings.h>

int open_clientfd(char *hostname, int port)
{
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;
     /* int socket(int domain, int type, int protocol); */
	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;	/* Check error for cause of error */
	
	/* Fill in the server's IP address and port */
      /* int gethostname(char *name, size_t len); */
	if ((hp = gethostbyname(hostname)) == NULL)
		return -2;	/* Check h_error for cause of error */
	
   /* void bzero(void s[.n], size_t n); */
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
   /* [[deprecated]] void bcopy(const void src[.n], void dest[.n], size_t n); */
	bcopy((char *)hp->h_addr_list[0],
		(char *)&serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = htons(port);

	/* Establish a connection with the server */
     /* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen); */
	if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;

	return clientfd;
}
