#include <iostream>
#include <sstream>
#include <cstring>
#include <malloc.h>
#include "../ServerCommunication.h"
#include "../Functions.h"
#include "../Packets.h"
#include "../GameplayCommunication.h"

#define PORT 17515

#define READ_END 	0
#define WRITE_END	1

using namespace std;

int main(int argc, char **argv)
{
	IPaddress			ipaddr;
	char 				*ip_address_string = argv[1];
	int					pipes[2];
	UDPsocket			udpsock;
	TCPsocket			tcpsock;
	pkt02				tcp_packet_test = {0, 12, 0};
	PKT_ALL_POS_UPDATE	udp_packet_test;
	UDPpacket *container	= SDLNet_AllocPacket(sizeof(PKT_ALL_POS_UPDATE) + sizeof(int));
	udp_packet_test.floor 	= 2;
	
	int packet_type;
	size_t packet_size;
	
	cerr << "Packet size: " << malloc_usable_size(container->data) << endl;
	
	container->len = sizeof(PKT_ALL_POS_UPDATE) + sizeof(int);
	((size_t *)container->data)[0] = sizeof(PKT_ALL_POS_UPDATE) + sizeof(int);
	memcpy(container->data + sizeof(size_t), (void *)&udp_packet_test, sizeof(PKT_ALL_POS_UPDATE));

	cout << ip_address_string << std::endl;
	resolve_host(&ipaddr, PORT, ip_address_string);

	if(create_pipe(pipes) == -1)
	{
		cerr << "Big probrem number 3!" << std::endl;
		return 3;
	}
	
	if(!(tcpsock = SDLNet_TCP_Open(&ipaddr)))
	{
		cerr << SDLNet_GetError() << endl;
		return 4;
	}
	udpsock = SDLNet_UDP_Open(PORT);
	
	thread recv(recv_thread_func, pipes[WRITE_END], tcpsock, udpsock);
	recv.detach();
	
	
	read(pipes[READ_END], (void *)&packet_size, sizeof(size_t));
	read(pipes[READ_END], (void *)&packet_type, sizeof(int));
	read(pipes[READ_END], (void *)&tcp_packet_test, sizeof(pkt02));
	
	cout << "Packet size: " << packet_size << "; Packet type: " << packet_type << "; Packet contents: ";
	cout << "Connection code: " << tcp_packet_test.connect_code << "; Player number: " << tcp_packet_test.clients_player_number;
	cout << "; Team number: " << tcp_packet_test.clients_team_number << std::endl;
	
	read(pipes[READ_END], (void *)&packet_size, sizeof(size_t));
	read(pipes[READ_END], (void *)&packet_type, sizeof(int));
	read(pipes[READ_END], (void *)&udp_packet_test, sizeof(pkt11));
	
	cout << "Packet size: " << packet_size << "; Packet type: " << packet_type << std::endl;
	
	SDLNet_FreePacket(container);
	return 0;
}

