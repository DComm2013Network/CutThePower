/** @ingroup Network */
/** @{ */
/**
 * This is the main file for the client-side Network layer. This layer/component will contain
 * all network related functions and will basically grab game data from the Gameplay module, dispatch 
 * it to the server and vice-versa.
 *
 * @todo Change name of send (don't really want to use the name of a function)
 *
 * @file NetworkRouter.cpp
 */
/** @} */
#include "Packets.h"
#include "ServerCommunication.h"
#include "GameplayCommunication.h"
#include "PipeUtils.h"
#include "NetworkRouter.h"
#include <list>

extern int game_net_signalfd;
extern int network_ready;
extern uint32_t packet_sizes[NUM_PACKETS];
static int chat_packets_rcv = 0;
static std::list<PKT_SND_CHAT*> cached_chat;
sem_t err_sem;
int send_failure_fd;

/**
 * This is the main client-side network function that the Gameplay module will call inside
 * a new thread. 
 * 
 * This function will basically be the main driver of the networking layer and will be responsible
 * for reading data from Gameplay and sending it to the server and then reading data from the server
 * and sending it back to gameplay.
 *
 * It will automatically determine the origin and destination of each data packet and deliver it.
 *
 * Uses both TCP and UDP to communicate with the server and uses the good old IPC pipes for communicating
 * with the gameplay module.
 * 
 * @param[in]   args  A void pointer to the PDATA data structure.
 *
 * @return  <ul>
 *              <li>Returns NULL if there are any problems.</li>
 *          </ul>
 *
 * @designer    Abhishek Bhardwaj
 * @author      Abhishek Bhardwaj
 */
void *networkRouter(void *args)
{
    int 		sendfd[2];
    int 		recvfd[2];
    fd_set 		listen_fds;
    fd_set		active;
    int 		max_fd;
    uint32_t 	type;
    uint64_t	timestamps[NUM_PACKETS] = {0}, timestamp;
    void 		*packet, *cached_packets[NUM_PACKETS] = {0};
    pthread_t 	thread_send;
    pthread_t 	thread_receive;
    PDATA 		gameplay = (PDATA)args;
    
    NDATA 		send_data = (NDATA) malloc(sizeof(WNETWORK_DATA));
    NDATA 		receive_data = (NDATA) malloc(sizeof(WNETWORK_DATA));

    sem_init(&err_sem, 0, 1);

    if(init_router(&max_fd, send_data, receive_data, gameplay, sendfd, recvfd, &thread_receive, &thread_send, gameplay->ip) == -1)
    {     
        net_cleanup(send_data, receive_data, gameplay, cached_packets);
    	return NULL;
    }
	
    FD_ZERO(&listen_fds);
    FD_SET(recvfd[READ_END], &listen_fds);
    FD_SET(gameplay->read_pipe, &listen_fds);
    FD_SET(game_net_signalfd, &listen_fds);
    FD_SET(send_failure_fd, &listen_fds);

    network_ready = 1;

    while(1)
    {
    	int ret;
        active = listen_fds;
        ret = select(max_fd + 1, &active, NULL, NULL, NULL);

        if(ret && FD_ISSET(send_failure_fd, &active))
        	break;

        if(ret && FD_ISSET(recvfd[READ_END], &active))
        {
			packet = read_data(recvfd[READ_END], &type);

            if(!packet)
            {
                break;
            }
            
            if(type == P_CHAT)
            {
                read(recvfd[READ_END], &timestamp, sizeof(timestamp));
                chat_packets_rcv++;
                cached_chat.push_back((PKT_SND_CHAT*)packet);
            }
            else
            {
    			read(recvfd[READ_END], &timestamp, sizeof(timestamp));
                if(timestamps[type - 1] < timestamp)     // If the received packet is more recent, replace the cached one
                {
                    timestamps[type - 1] = timestamp;
                    free(cached_packets[type - 1]);
                    cached_packets[type - 1] = packet;
                }
                else                                    // Otherwise discard it
                    free(packet);
            }
			--ret;
        }

        if(ret && FD_ISSET(gameplay->read_pipe, &active))
        {
        	packet = read_data(gameplay->read_pipe, &type);
            if(!packet)
            {
                break;
            }
            
            write_packet(sendfd[WRITE_END], type, packet);
			
            --ret;
        }

        if(ret && FD_ISSET(game_net_signalfd, &active))
        {
        	if(update_gameplay(gameplay->write_pipe, cached_packets, timestamps) == -1)
                return NULL;

            --ret;
		}
    }

    pthread_cancel(thread_receive);
    pthread_cancel(thread_send);
    // Kill the send thread... forgot how this was supposed to happen, oops
    net_cleanup(send_data, receive_data, gameplay, cached_packets);
    return NULL;
}
/**
 * Sends all cached chat packets down the gameplay pipe.
 *
 * Takes the linked CHAT_LIST and sends all its chat packets to gameplay for processing.
 *
 * @return  <ul>
 *              <li>0 on success, or -1 on failure.</li>
 *          </ul>
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 */
int send_cached_chat(int gameplay_write_fd)
{
    uint32_t type = P_CHAT;
    for(int i = 0; i < chat_packets_rcv; i++)
    {
        if(write(gameplay_write_fd, &type, sizeof(type)) == -1 ||
           write(gameplay_write_fd, cached_chat.front(), packet_sizes[P_CHAT - 1]) == -1)
        {
            perror("update_gameplay: write");
            return -1;
        }

        free(cached_chat.front());
        cached_chat.pop_front();

    }
    
    chat_packets_rcv = 0;

    return 0;
}
/**
 * Creates and dispatches a new thread.
 *
 * The thread runs independently of the thread that distpaches it. The thread handle is
 * stored in @a handle.
 * 
 * @param[in]  function	Function which the new thread will run.
 * @param[in]  params	The parameter(s) to pass to the thread function.
 * @param[out] handle	Stores a handle to the thread (for sending signals etc).
 *
 * @return  <ul>
 *              <li>0 on success, or -1 on failure.</li>
 *          </ul>
 *
 * @designer Shane Spoor
 * @author	 Shane Spoor
 */
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle)
{
	if(pthread_create(handle, NULL, function, params) == -1)
	{
		perror("dispatch_thread");
		return -1;
	}

	pthread_detach(*handle);
	return 0;
}

/**
 * Determines which packets have new data.
 *
 * 
 * @param   packets     The array containing all currently cached packets.
 * @param   changed     A mask indicating which packets have new data.
 *
 * @return <ul> 
 *              <li>The number of packets with new data.</li>
 *         </ul>
 *
 * @author 		Shane Spoor
 * @designer 	Shane Spoor
 */
uint32_t determine_changed(void **packets, unsigned *changed)
{
	size_t i;
	uint32_t nchanged = 0;
	
	for(i = 0; i < NUM_PACKETS; ++i)
	{
		if(packets[i])
		{
			*changed |= 1 << i;
			++nchanged;
		}
	}
	
	return nchanged + chat_packets_rcv;
}

/**
 * Tells the gameplay module that network is shutting down, possibly with an error message.
 *
 * Writes the network shutdown code to 
 *
 * @param[in] gameplay_pipe The write end of the pipe to gameplay.
 * @param[in] err_str       The error string; may be null.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 14, 2014
 */
void write_shutdown(int gameplay_pipe, const char *err_str)
{
    uint32_t shutdown = (uint32_t)NET_SHUTDOWN;

    /* Tell gameplay we're shutting down */
    write_pipe(gameplay_pipe, &shutdown, sizeof(shutdown));
    shutdown = 0;
    if(err_str) // if there was an error, write the error string to the pipe
    {
        shutdown = strlen(err_str);
        write_pipe(gameplay_pipe, &shutdown, sizeof(shutdown));
        write_pipe(gameplay_pipe, err_str, shutdown);
    }
    else
        write_pipe(gameplay_pipe, &shutdown, sizeof(shutdown)); // write 0 instead (no string)
    
    return;
}

/**
 * Initialises the variables used in the router thread or the send/receive threads.
 *
 * @param[out] max_fd         The highest value of the gameplay write file descriptor, the receive
 *                            thread read file descriptor, and the semaphore file descriptor (for select()).
 * @param[out] send           Pointer to a struct to contain the data passed to the send thread.
 * @param[out] receive        Pointer to a struct to contain the data passed to the receive thread.
 * @param[out] gameplay       Pointer to a struct containing the objects necessary for communication with gameplay.
 * @param[out] fd             An integer array to hold two ends of a pipe.
 * @param[out] thread_receive Holds a handle to the receive thread.
 * @param[out] thread_send    Holds a handle to the send thread.
 *
 * @return <ul>
 *              <li>0 on success, or -1 on failure.</li>
 *         </ul>
 *
 * @designer Shane Spoor
 * @designer Abhishek Bhardwaj
 *
 * @author   Shane Spoor
 */
int init_router(int *max_fd, NDATA send, NDATA receive, PDATA gameplay, int sendfd[2], 
				int recvfd[2], pthread_t *thread_receive, pthread_t *thread_send, char* ip)
{
    IPaddress ipaddr, udpaddr;
	TCPsocket tcp_sock;
	UDPsocket udp_sock;
    int channel;
	
    send_failure_fd = eventfd(0, 0);

	if(create_pipe(sendfd) == -1 ||
	   create_pipe(recvfd) == -1)
    {
        set_error(ERR_IPC_FAIL);
        return -1;
    }

    *max_fd = recvfd[READ_END] > gameplay->read_pipe ? recvfd[READ_END] : gameplay->read_pipe;
    *max_fd = game_net_signalfd > *max_fd ? game_net_signalfd : *max_fd;
    *max_fd = send_failure_fd > *max_fd ? send_failure_fd : *max_fd;

    /* Initialize SDL_net */
    if (SDLNet_Init() < 0)
    {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        return -1;
    }

    if(resolve_host(&ipaddr, TCP_PORT, ip) == -1)
    {
        set_error(ERR_NO_CONN);
        return -1;
    }

    if(resolve_host(&udpaddr, UDP_PORT, ip) == -1)
    {
        set_error(ERR_NO_CONN);
        return -1;
    }
    
    tcp_sock = SDLNet_TCP_Open(&ipaddr);

    if(!tcp_sock) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        set_error(ERR_NO_CONN);
        return -1;
    }

    udp_sock = SDLNet_UDP_Open(UDP_PORT);
    if(!udp_sock) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        set_error(ERR_ROUTER_INIT);
        return -1;
    }

    channel = SDLNet_UDP_Bind(udp_sock, 0, &udpaddr);
    if(channel ==   -1) {
        fprintf(stderr, "SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        set_error(ERR_ROUTER_INIT);
        return -1;
    }

    send->tcp_sock = tcp_sock;
    send->udp_sock = udp_sock;

    receive->tcp_sock = tcp_sock;
    receive->udp_sock = udp_sock;

    send->read_pipe = sendfd[READ_END];
    send->write_pipe = sendfd[WRITE_END];

    receive->read_pipe = recvfd[READ_END];
    receive->write_pipe = recvfd[WRITE_END];

	if(dispatch_thread(send_thread_func, (void *)send, thread_send) == -1 ||
        dispatch_thread(recv_thread_func, (void *)receive, thread_receive) == -1)
    {
        set_error(ERR_ROUTER_INIT);
        return -1;
    }
	
	return 0;
}

/**
 * Initialises the variables used in the router thread or the send/receive threads.
 *
 * @param[in] gameplay_write_fd Write end of the pipe to the client/gameplay update system.
 * @param[in] packets           The latest packets received from the server.
 * @param[in] timestamps        An array of timestamps associated with each packet.
 *
 * @return <ul>
 *              <li>0 on success</li>
                <li>-1 on failure.</li>
 *         </ul>
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 12, 2014
 */
int update_gameplay(int gameplay_write_fd, void **packets, uint64_t *timestamps)
{
    uint32_t num_changed;
    uint64_t sem_buf;
    unsigned changed_mask = 0;

    read(game_net_signalfd, &sem_buf, sizeof(uint64_t)); /* Receive the signal */
    num_changed = determine_changed(packets, &changed_mask);

    if(write(gameplay_write_fd, &num_changed, sizeof(num_changed)) == -1)
    {
        perror("update_gameplay: write");
        return -1;
    }

    if(!num_changed)
    {
        return 0;
    }

    for(uint32_t type = 1, idx = 0; type <= NUM_PACKETS; ++type, ++idx) // type = packet type, idx = index in packet array
    {
        if(changed_mask & (1 << idx))
        {   
            if(write(gameplay_write_fd, &type, sizeof(type)) == -1 ||
               write(gameplay_write_fd, packets[idx], packet_sizes[idx]) == -1)
            {
                perror("update_gameplay: write");
                return -1;
            }
            free(packets[idx]);
            packets[idx] = NULL;
        }
    }

    if(send_cached_chat(gameplay_write_fd) == -1)
    {
        return -1;
    }

    return 0;
}

/**
 * Cleans up resources held by the network router thread.
 *
 * The thread also notifies the gameplay side that the network module is closing down,
 * possibly also sending an error message to be passed along to the user.
 *
 * @param[in] send           The send thread's data (socket and pipe descriptors).
 * @param[in] received       The receive thread's data (socket and pipe descriptors).
 * @param[in] cached_packets The array of cached packets.
 *
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 14, 2014
 */
void net_cleanup(NDATA send_data, NDATA receive_data, PDATA gameplay, void **cached_packets)
{
    unsigned int changed_mask = 0;
    
    /* Close pipes for thread communication, checking that they're valid first */
    if(fcntl(send_data->read_pipe, F_GETFD) != -1 || errno != EBADF)
    {
        close(send_data->read_pipe);
        close(send_data->write_pipe);
    }
    if(fcntl(receive_data->read_pipe, F_GETFD) != -1 || errno != EBADF)
    {
        close(receive_data->read_pipe);
        close(receive_data->write_pipe);
    }
    
    /* Free NDATA */
    free(send_data);
    free(receive_data);

    write_shutdown(gameplay->write_pipe, get_error_string());

    /* Free any remaining packets */
    determine_changed(cached_packets, &changed_mask);
    for(int i = 0; i < NUM_PACKETS - 1; ++i)
    {
        if(changed_mask & (1 << i))
        {
            free(cached_packets[i]);
            cached_packets[i] = NULL;
        }
    }

    /* Close send thread's eventfd */
    close(send_failure_fd);
}
/*

The gameplay module will basically create the pipe and call the networkRouter() function
with the newly created pipe.

Wrapper functions for creating the pipe, reading and writing to/from the pipe are present
in the Functions.cpp file.

All the functions that the gameplay side will need to use to read/write to the pipe are
present in GameplayCommunication.cpp and Functions.cpp

*/
