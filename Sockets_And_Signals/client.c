// Monica Ovalekar	NetId: mso269	N#: N12316668
// client.c
/* client.c takes input from the user.
server.c checks if it had the code "C00L" and dumps the data (alongwith the codeword) and the count (which does not include the two zeroes of the code word) in the file secrets.out
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>

#define PORT "8000"

// getting local IP address
void get_ip_addr (char *ip_addr)
{
	char buffer[200];
	struct hostent* host;

	gethostname(buffer, 200);
	host = (struct hostent *) gethostbyname(buffer);

	strcpy(ip_addr,inet_ntoa(*((struct in_addr *)host->h_addr)));
}

// getting socket address (IPv4 or IPv6)
void *get_socket_addr(struct sockaddr *socket_addr)
{
	if (socket_addr->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)socket_addr)->sin_addr);
	}

	return &(((struct sockaddr_in6*)socket_addr)->sin6_addr);
}

int main(void)
{
	int socket_FileD, gai;
	struct addrinfo server_addr, *server_info, *p;
	char ip_addr[200], s[INET6_ADDRSTRLEN];
	char *input = malloc(sizeof(char)*BUFSIZ);

	// infinite loop
	while (1)
	{

		// configuring server information
		memset(&server_addr, 0, sizeof server_addr);
		server_addr.ai_family = AF_UNSPEC;
		server_addr.ai_socktype = SOCK_STREAM;

		// getting IP address
		get_ip_addr(ip_addr);

		// setting address info
		if ((gai = getaddrinfo(ip_addr, PORT, &server_addr, &server_info)) != 0)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(gai));
			return 5;
		}

		// go through server_info using pointer operator(->) to next option and bind it to the available option
		for(p = server_info; p != NULL; p = p->ai_next)
		{
			// creating socket file descriptor
			if ((socket_FileD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			{
				perror("Socket Not Created");
				continue;
			}

			// connecting to server
			if (connect(socket_FileD, p->ai_addr, p->ai_addrlen) < 0)
			{
				close(socket_FileD);
				perror("Could not Connect");
				continue;
			}
			break;
		}

		if (p == NULL)
		{
			fprintf(stderr, "Connecting failed");
			return 6;
		}

		// converting from binary to text form
		inet_ntop(p->ai_family, get_socket_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

		// connection completed message
		printf("CLIENT CONNECTED TO %s SERVER\n", s);

		// free server info
		freeaddrinfo(server_info);

		// input is taken from user
		printf("Enter a string with code and type quit to quit: ");
		fgets(input, BUFSIZ, stdin);

		// sending input to server
		if (send(socket_FileD, input, strlen(input), 0) < 0)
			perror("sending error");

		// closing socket
		close(socket_FileD);

		//to quit
		if (strncmp(input, "quit", 4) == 0)
		{
			break;
		}
	}

	return 0;
}