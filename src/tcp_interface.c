
#include "headers.h"


// typedef void (*tcp_rx_cb)(char *, int, struct sockaddr_in *);

// void callback_(char *buff, int len, struct sockaddr_in *s_addr){
// 	ILOG("[TCP] client Rx[%d]: %s\r\n", len, buff);
// 	ILOG("[TCP] client add: 0x%08X, port: %d, family: %d\r\n", s_addr->sin_addr, s_addr->sin_port, s_addr->sin_family);
// }

struct sockaddr_in cliaddr, client_address[MAX_TCP_CLIENTS];
int client_sock_fd[MAX_TCP_CLIENTS];
int conn_client_cnt = 0;




void tcp_rx_callback(char *buff, int len, struct sockaddr_in *s_addr, int fd){
	char str[INET_ADDRSTRLEN];
	

	inet_ntop(AF_INET, &(s_addr->sin_addr), str, INET_ADDRSTRLEN);
	ILOG("[TCP] client addr: %s:%d, family: %d\r\n", str, s_addr->sin_port, s_addr->sin_family);

	ILOG("[TCP] client Rx[%d]: %s\r\n", len, buff);
	
	ILOG("Size of buff: %ld\n", sizeof(buff));
	
	//Parsing the JSON frames
	request_frame_parser(buff, len, fd);


	//send_to_clients(-1, buff, len);
	//send ACK if needed from the given 
	// write(fd, (const char *)buff, len);
	
}
/**
 * TCP Uses 2 types of sockets, the connection socket and the listen socket.
 * The Goal is to separate the connection phase from the data exchange phase.
 * */

void *tcp_server_thread(void *vargp)
{
	//port to start the server on
	int server_port = SERVER_PORT;
	int ret = -1;
	int i = 0, j = 0;
	int timeout_ms = 1000;
	struct timeval tv_timeout;
	char temp_string[300];
	// socket address used for the server
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	tcp_rx_cb cb = vargp;
	int max_fds = 0;
	

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(server_port);

	// htonl: host to network long: same as htons but to long
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);


	char str_addr[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &(server_address.sin_addr.s_addr), str_addr, INET_ADDRSTRLEN);


	DLOG("[TCP] server address: %s:%d\n", str_addr, server_port);
	//ILOG("[TCP] server address: %s:%d\n", str_addr, server_port);
	// create a TCP socket, creation returns -1 on failure
	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		ILOG("could not create listen socket\n");
		return (void *)1;
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(listen_sock, (struct sockaddr *)&server_address,
			  sizeof(server_address))) < 0)
	{
		ILOG("could not bind socket\n");
		return (void *)1;
	}

	int wait_size = 16; // maximum number of waiting clients, after which
						// dropping begins
	if (listen(listen_sock, wait_size) < 0)
	{
		ILOG("could not open socket for listening!!\n");
	}

	// socket address used to store client address

	fd_set rfds;
	// run indefinitely
	while (true)
	{
		//check if anything receive on socket
		FD_ZERO(&rfds);
		FD_SET(listen_sock, &rfds);
		max_fds = listen_sock;
		for (i = 0; i < conn_client_cnt; i++)
		{
			FD_SET(client_sock_fd[i], &rfds);
			if (client_sock_fd[i] > max_fds)
			{
				max_fds = client_sock_fd[i];
			}
		}

		tv_timeout.tv_sec = timeout_ms / 1000;
		tv_timeout.tv_usec = (timeout_ms % 1000) * 1000;

		if ((ret = select(max_fds + 1, &rfds, NULL, NULL, (timeout_ms < 0) ? NULL : &tv_timeout)) < 0)
			ILOG("select() on socket error!!!");

		/* Timeout */
		if (ret == 0)
		{
			// ILOG("[TCP] select Timeout....\r\n");
			continue;
		}
		//If receive anything on server socket
		if (FD_ISSET(listen_sock, &rfds))
		{
			socklen_t len = sizeof(cliaddr);
			int connfd = accept(listen_sock, (struct sockaddr *)&cliaddr, &len);
			if (conn_client_cnt < MAX_TCP_CLIENTS)
			{
				ILOG("Got new client fds[%d]: %d\r\n", conn_client_cnt, connfd);
				memcpy((char *)&client_address[conn_client_cnt], (const char *)&cliaddr, sizeof(cliaddr));
				client_sock_fd[conn_client_cnt] = connfd;
				//Send some data to the new client to indicate that it is connected to the server.....
				// sprintf(temp_string, "You are connected to server......");
				// write(client_sock_fd[conn_client_cnt], temp_string, strlen(temp_string)+1);

				//conn_client_cnt
				conn_client_cnt++;
			}
			else
			{
				ILOG("Max clients are already connected, closing new client socket.....!");

				//Send some data to the new client to indicate that it is connected to the server.....
				
				sprintf(temp_string, "Max client limit reached, Max clients: %d, SErver closing the socket.....\r\n", MAX_TCP_CLIENTS);
				ret = write(connfd, temp_string, strlen(temp_string) + 1);

				close(connfd);
			}
		}

		//check if any clients are connected then just read its data as well..
		for (i = 0; i < conn_client_cnt; i++)
		{
			if (FD_ISSET(client_sock_fd[i], &rfds))
			{
				char buffer[256];
				// int len = sizeof(cliaddr);
				bzero(buffer, sizeof(buffer));
				// ILOG("Message from the client: ");
				ssize_t n = read(client_sock_fd[i], buffer, sizeof(buffer));

				

				//, 0,(struct sockaddr*)&cliaddr, &len);
				if (n > 0)
				{
					buffer[n] = '\n';
					// puts(buffer);
					cb(buffer, n, &client_address[i], client_sock_fd[i]);
					//write(client_sock_fd[i], (const char *)buffer, sizeof(buffer));
					//, 0,(struct sockaddr*)&cliaddr, sizeof(cliaddr));
				}
				else
				{
					ILOG("Closing the client socket Fds[%d]: %d\r\n", i, client_sock_fd[i]);
					close(client_sock_fd[i]);
					for(j = i; j < conn_client_cnt - 1; j++){
						client_sock_fd[j] = client_sock_fd[j + 1];
					}
					conn_client_cnt--;
				}
			}
		}
	}
	//Closing all the client socket if it is still open
	for(j = 0; j < conn_client_cnt - 1; j++){
		close(client_sock_fd[j]);
	}
	//Closing server socket
	close(listen_sock);
}


/**
 * @brief 		: used to send data to one or all the client.
 * 
 * @param fd 	: 0 or greater file descriptor for one client, less than 1 will be use for broadcast to all the clients at once
 * @param pBuff : pointer to buffer
 * @param len  	: data size to be transferred
 * @return int  : status 
 */
int send_to_clients(int fd, char *pBuff, int len){

	int i = 0;
	size_t wlen = -1;
	if(fd > 0){ // unicast on socket or fifo
		wlen = write(fd, (const char *)pBuff, len);
	} else if(fd == 0) { //broadcast to the client sockets
		for (i = 0; i < conn_client_cnt; i++)
		{
			wlen = write(client_sock_fd[i], (const char *)pBuff, len);
		}
	} 
#ifdef MQTT_INTERFACE	
	else if(fd == -1){ //publish to the MQTT broker.
		publish(pBuff, len);
	}

#endif
	else {
		DLOG("Invalid FD for sending a frame\n");
	}
	return wlen;
}
// int main(void){

// 	pthread_t thread_id;
//     ILOG("Before Thread\n");
//     pthread_create(&thread_id, NULL, tcp_server_thread, (void *)&callback_);
//     pthread_join(thread_id, NULL);
// }
