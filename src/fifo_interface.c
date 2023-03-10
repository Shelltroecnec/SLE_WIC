
#include "headers.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>




/**
 * @brief 				Callback function for rx fifo interface
 * 
 * @param buff 			character pointer to buffer
 * @param len 			buffer length
 * @param read_fd 		file descriptor for read
 * @param write_fd 		file descriptor for write
 */
void fifo_rx_callback(char *buff, int len, int read_fd, int write_fd){
	int ret = -1;
	ILOG("Size of buff: %d, len: %d\n", strlen(buff), len);
	
	//Parsing the JSON frames
	request_frame_parser(buff, len, write_fd);

	// ret = write(write_fd, buff, len);
	// DLOG("wrote to fd. ret: %d\r\n", ret);
	// if(ret == -1){
	// 	DLOG("Fail ")
	// }
	
}
/**
 * TCP Uses 2 types of sockets, the connection socket and the listen socket.
 * The Goal is to separate the connection phase from the data exchange phase.
 * */

void *fifo_server_thread(void *vargp)
{
	int ret = -1;
	//port to start the server on
	int timeout_ms = 1000;
	struct timeval tv_timeout;
	// socket address used for the server
	fifo_rx_cb cb = vargp;
	int max_fds = 0;

	int write_fd, read_fd;
	char writebuf[100] = "Hello world!";
	char readbuf[100] ;
	char end[10];
	int to_end;
	int read_bytes, write_bytes;
	fd_set rfds;

   DLOG("Fifo: %s\r\n", config.fifo_NR_TO_DR );
   DLOG("Fifo: %s\r\n", config.fifo_DR_TO_NR );
   
   /* Create the FIFO if it does not exist */
   mkfifo(config.fifo_DR_TO_NR, S_IFIFO|0777);
   mkfifo(config.fifo_NR_TO_DR, S_IFIFO|0777);

   strcpy(end, "end");
   
   write_fd = open(config.fifo_DR_TO_NR, O_WRONLY);
   read_fd = open(config.fifo_NR_TO_DR, O_RDONLY | O_NONBLOCK);
	
	// run indefinitely
	while (true)
	{
		//check if anything receive on socket
		FD_ZERO(&rfds);
		// FD_ZERO(&wfds);
		// 
		FD_SET(read_fd, &rfds);
		// FD_SET(write_fd, &wfds);
		max_fds = read_fd ;//> write_fd? read_fd: write_fd;

		tv_timeout.tv_sec = timeout_ms / 1000;
		tv_timeout.tv_usec = (timeout_ms % 1000) * 1000;

		if ((ret = select(max_fds + 1, &rfds, NULL, NULL, (timeout_ms < 0) ? NULL : &tv_timeout)) < 0)
			ILOG("select() on fifo read error!!!");

		/* Timeout */
		if (ret == 0)
		{
			ILOG("[FIFO] select Timeout....\r\n");
			//write(write_fd, "data from DR", 12);
			continue;
		}
		//If receive anything from NR to DR fifo
		if (FD_ISSET(read_fd, &rfds))
		{
			char buffer[256];
			bzero(buffer, sizeof(buffer));
			// ILOG("Message from the client: ");
			ssize_t n = read(read_fd, buffer, sizeof(buffer));

			if (n > 0)
			{
				buffer[n] = '\n';
				// puts(buffer);
				cb(buffer, n, read_fd, write_fd);
			}
			else
			{
				ILOG("Closing the Rx fifo fd:  %d\r\n", read_fd);
				close(read_fd);
				//check if we need to open it again as read only mode
			    read_fd = open(config.fifo_NR_TO_DR, O_RDONLY | O_NONBLOCK);

			}
		}

		// if (FD_ISSET(write_fd, &wfds))
		// {
		// 	char buffer[256];
		// 	bzero(buffer, sizeof(buffer));
		// 	else
		// 	{
		// 		ILOG("Closing the Rx fifo fd:  %d\r\n", write_fd);
		// 		close(write_fd);
		// 		//check if we need to open it again as read only mode
		// 	    write_fd = open(config.fifo_NR_TO_DR, O_RDONLY | O_NONBLOCK);
		// 	}
		// }
	}
	//Closing server socket
	close(read_fd);
}


