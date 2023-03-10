
#ifndef __TCP_SERVER
#define __TCP_SERVER


#define MAX_TCP_CLIENTS			10 //Max number of clients that can connect to the server at a time
#define SERVER_PORT		 		5000 //Max number of clients that can connect to the server at a time

typedef void (*tcp_rx_cb)(char *, int, struct sockaddr_in *, int);


void tcp_rx_callback(char *buff, int len, struct sockaddr_in *s_addr, int fd);
void *tcp_server_thread(void *vargp);
int send_to_clients(int fd, char *pBuff, int len);

#endif // __TCP_SERVER
