/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	udp_clnt.c - A simple UDP client program.
--
--	PROGRAM:		uclnt.exe
--
--	FUNCTIONS:		Winsock 2 API
--
--	DATE:			January 6, 2008
--
--	REVISIONS:		(Date and Description)
--
--	DESIGNERS:
--
--	PROGRAMMERS:	Aman Abdulla
--
--	NOTES:
--	The program will send a UDP packet to a user specifed server.
--  The server can be specified using a fully qualified domain name or and
--	IP address. The packet is time stamped and then sent to the server. The
--  response (echo) is also time stamped and the delay is displayed.
---------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

typedef struct _pkt11
{
	int type;
	int floor_no;
	int arr1[32];
	int arr2[32];
	int arr3[32];
	int arr4[32];
	int arr5[32];
} pkt11;

typedef struct _pkt02
{
	int type;
	unsigned var1;
	int var2;
	int var3;
} pkt02;

#define SERVER_UDP_PORT		17515	// Default port
#define MAXLEN				65000   // Maximum Buffer length
#define DEFLEN				64		// Default Length


int main (int argc, char **argv)
{

	int	data_size = DEFLEN, port = SERVER_UDP_PORT;
	int	i, j, server_len, client_len;
	int     sd;
	int	tcplisten;
	int	tcpaccept;

	char *pname, *host, rbuf[MAXLEN], sbuf[MAXLEN];
	struct	hostent	*hp;
	struct	sockaddr_in server, client;
	pkt11 udptest;
	pkt02 tcptest = {2, 12, 3, 4};
	udptest.type = 11;
	udptest.floor_no = 12;

	// Create a datagram socket
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror ("Can't create a socket\n");
		exit(1);
	}
	
	if ((tcplisten = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		return 2;
	}
	i = 1;
	setsockopt(tcplisten, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int));

	// Store server's information
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_aton(argv[1], &server.sin_addr.s_addr);
	
	connect(sd, (struct sockaddr *)&server, sizeof(server));
	
	// Now the TCP one
	i = sizeof(client);
	server.sin_addr.s_addr = INADDR_ANY;
	bind(tcplisten, (struct sockaddr *)&server, sizeof(server));
	listen(tcplisten, 5);
	tcpaccept = accept(tcplisten, (struct sockaddr *)&client, &i);
	
	if (data_size > MAXLEN)
	{
		fprintf(stderr, "Data is too big\n");
		exit(1);
	}

	if (send (tcpaccept, (char *)&tcptest, sizeof(tcptest), 0) == -1)
	{
		perror("send failure");
		return 4;
	}

	if (send (sd, (char *)&udptest, sizeof(udptest), 0) == -1)
	{
		perror("send failure");
		exit(1);
	}

	close(sd);
	close(tcplisten);
	close(tcpaccept);
	exit(0);
}
