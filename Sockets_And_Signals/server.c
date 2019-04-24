// Monica Ovalekar	NetId: mso269	N#: N12316668
// server.c
/* client.c takes input from the user.
server.c checks if it had the code "C00L" and dumps the data (alongwith the codeword) and the count (which does not include the two zeroes of the code word) in the file secrets.out
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>

#define PORT "8000"

// When child process terminates it becomes a zombie process which has a PID and entry in process table
//These processes are reaped to remove the unnecessary clutter
static void sigdchild_handler(int sig)
{
	while (waitpid(-1, NULL, WNOHANG) > 0)
	{
	}
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
	int socket_FileD, new_socket, gai, buf_len, sso = 1, digit_count = 0, line_count = 0, n;
	struct addrinfo server_addr, *server_info, *p;
	struct sockaddr_storage client_addr;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN], buf[BUFSIZ];
	char *d;
	socklen_t sin_size;
	FILE *secrets;

	// secrets.out file is created
	secrets = fopen("secrets.out","w");
	fclose(secrets);

	// setting server information
	memset(&server_addr, 0, sizeof server_addr);
	server_addr.ai_family = AF_UNSPEC;
	server_addr.ai_socktype = SOCK_STREAM;
	server_addr.ai_flags = AI_PASSIVE;

	// getting address
	if ((gai = getaddrinfo(NULL, PORT, &server_addr, &server_info)) != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(gai));
		return 5;
	}

	// go through server_info using pointer operator(->) to next option and bind it to the available option
	for (p = server_info; p != NULL; p = p->ai_next)
	{
		// creating socket file descriptor
		if ((socket_FileD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			perror("Socket Not Created");
			continue;
		}

		// attaching socket to port 8000
		if (setsockopt(socket_FileD, SOL_SOCKET, SO_REUSEADDR, &sso, sizeof(int)) < 0)
		{
			perror("Attaching Socket Failed");
			exit(1);
		}

		// attaching socket to port 8000
		if (bind(socket_FileD, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(socket_FileD);
			perror("Binding Socket Failed");
			continue;
		}

		break;
	}

	// If binding fails
	if (p == NULL)
	{
		fprintf(stderr, "Binding Failed");
		return 2;
	}

	// server_info structure won't be used anymore
	freeaddrinfo(server_info);

	// listening to incoming sockets
	if (listen(socket_FileD, 10) < 0)
	{
		perror("Listen Failed");
		exit(2);
	}

	// zombie child processes are reaped
	sa.sa_handler = sigdchild_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	// raise error if sigaction fails
	if (sigaction(SIGCHLD, &sa, NULL) < 0)
	{
		perror("sigaction() Failed");
		exit(3);
	}

	// Infinite loop
	while(1)
	{
		// accept incoming socket
		sin_size = sizeof client_addr;

		new_socket = accept(socket_FileD, (struct sockaddr *)&client_addr, &sin_size);
		if (new_socket < 0)
		{
			perror("accept() Failed");
			continue;
		}

		// convert IPv4 and IPv6 addresses from binary to text form
		inet_ntop(client_addr.ss_family, get_socket_addr((struct sockaddr *)&client_addr), s, sizeof s);

		printf("\nSERVER CONNECTED TO %s CLIENT\n", s);

		// receive
		if ((buf_len = recv(new_socket, buf, BUFSIZ-1, 0)) < 0)
		{
			perror("recv() Failed");
			exit(4);
		}

		// addimg null to indicate end of line
		buf[buf_len] = '\0';

		printf("input received: %s",buf);

		// write to the file only if it has code but display all the inputs
		if (strncmp(buf, "C00L", 4) == 0)
		{
			line_count++;	// for total line count with code
			for (d = buf, n = 0; *d != '\0'; d++)
			{
				if (*d=='0' || *d=='1' || *d=='2' || *d=='3' || *d=='4' || *d=='5' || *d=='6' || *d=='7' || *d=='8' || *d=='9')
				{
					n++;	// for digit count
				}
			}

			// digit_count has the total digit count
 			digit_count += n;

			// input and digit count is appended to the file
			secrets = fopen("secrets.out","a");
			fprintf(secrets, "%d is/are the number of digits in this string %s", n-2, buf);
			fclose(secrets);

		}
		// closing the socket
		close(new_socket);

		// type "quit" to quit from loop
		if (strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
	}

	int a;
	a = digit_count - (2*line_count);

	// this gives summary of input which has code
	printf("\nSummary:");
	printf("\nTotal Number of lines with code: %d\n", line_count);
	printf("Total Number of digits excluding code digits: %d\n", a);
	printf("Total Number of digits including code digits: %d\n", digit_count);
	return 0;
}
