/*
 * filename: open_listenfd.c
 */
#include <sys/socket.h>
#include <string.h>

int open_listenfd(int port)
{
	int listenfd, optval = 1;
       /*  struct sockaddr_in {
	   sa_family_t	   sin_family;	   // AF_INET 
	   in_port_t	   sin_port;	   // Port number 
	   struct in_addr  sin_addr;	   // IPv4 address 
       }; 

       struct in_addr {
	   in_addr_t s_addr;
       }; */
	struct sockaddr_in serveraddr;

	/* Create a socket descriptor */
        /* int socket(int domain, int type, int protocol); */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	
	/* Eliminates "Address already in use" error from bind */
        /* int setsockopt(int sockfd, int level, int optname,
		      const void optval[.optlen],
		      socklen_t optlen);
	*/
	if (setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&optval, sizeof(int)) < 0)
		return -1;

	/* Listenfd will be an end point for all requests to port on any IP
	 * address for this host */

       /* void bzero(void s[.n], size_t n);
	* The zero() function erases the data in the n bytes of the memory
	* starting at the location pointed to by s, by writing zeros (bytes
	* containing '\0') to that area.
	*/
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
       /* int bind(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen); */
       /* typedef sturct sockaddr SA; */
	
	if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;

	/* Make it a listenening socket ready to accept connection requests */
        /* int listen(int sockfd, int backlog); */

        /* #define LISTENQ  1024  // Second argument to listen() */
	if (listen(listenfd, LISTENQ) < 0)
		return -1;

	return listenfd;
}
