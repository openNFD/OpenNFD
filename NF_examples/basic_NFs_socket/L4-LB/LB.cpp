#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <unordered_map>
#include <signal.h>
//#include "basic_classes.h"
#include "TCPClient.h"

#define SERVER_PORT  12345
#define MAX_PACKET_SIZE 1000000
#define TRUE             1
#define FALSE            0


unordered_map<int, int> connections;
int round = 0;
int max_round = 5;
unordered_map<int, IP> ippool;
unordered_map<int, int> portpool;
void stop(int sig){
	exit(0);
}

main (int argc, char *argv[])
{
	int    i, len, rc, on = 1;
	int    listen_sd, max_sd, new_sd;
	int    desc_ready, end_server = FALSE;
	int    close_conn;
	char   buffer[MAX_PACKET_SIZE];
	struct sockaddr_in   addr;
	struct timeval       timeout, timeout_t;
	fd_set        master_set, working_set, master_set_t;
	signal(SIGINT,stop);	

	ippool[0]= *(new IP("10.0.1.10/32"));
	ippool[1]= *(new IP("10.0.1.20/32"));
	ippool[2]= *(new IP("10.0.1.30/32"));
	ippool[3]= *(new IP("10.0.1.40/32"));
	ippool[4]= *(new IP("10.0.1.50/32"));
	//printf("%s", ippool[0].showAddr());
	portpool[0] = 11111;
	portpool[1] = 11111;
	portpool[2] = 11111;
	portpool[3] = 11111;
	portpool[4] = 11111;

	/*************************************************************/
	/* Create an AF_INET stream socket to receive incoming       */
	/* connections on                                            */
	/*************************************************************/
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(-1);
	}

	/*************************************************************/
	/* Allow socket descriptor to be reuseable                   */
	/*************************************************************/
	rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
			(char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Set socket to be nonblocking. All of the sockets for    */
	/* the incoming connections will also be nonblocking since  */
	/* they will inherit that state from the listening socket.   */
	/*************************************************************/
	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Bind the socket                                           */
	/*************************************************************/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	inet_pton(AF_INET, "10.0.0.2", &addr.sin_addr);
	addr.sin_port        = htons(SERVER_PORT);
	rc = bind(listen_sd,
			(struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Set the listen back log                                   */
	/*************************************************************/
	rc = listen(listen_sd, 32);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Initialize the master fd_set                              */
	/*************************************************************/
	FD_ZERO(&master_set);
	max_sd = listen_sd;
	FD_SET(listen_sd, &master_set);

	/*************************************************************/
	/* Initialize the timeval struct to 3 minutes.  If no        */
	/* activity after 3 minutes this program will end.           */
	/*************************************************************/
	timeout.tv_sec  = 0;
	timeout.tv_usec = 0;
	timeout_t.tv_sec = 0;
	// PARAM to config
	timeout_t.tv_usec = 0;
	/*************************************************************/
	/* Loop waiting for incoming connects or for incoming data   */
	/* on any of the connected sockets.                          */
	/*************************************************************/
	int fd_change = FALSE;
	do
	{
		/**********************************************************/
		/* Copy the master fd_set over to the working fd_set.     */
		/**********************************************************/
		//if (fd_change==TRUE){
			memcpy(&working_set, &master_set, sizeof(master_set));
		//}
	
		/**********************************************************/
		/* Call select() and wait 5 minutes for it to complete.   */
		/**********************************************************/
		//printf("Waiting on select()...\n");
		rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

		/**********************************************************/
		/* Check to see if the select call failed.                */
		/**********************************************************/
		if (rc < 0)
		{
			perror("  select() failed");
			break;
		}

		/**********************************************************/
		/* Check to see if the 5 minute time out expired.         */
		/**********************************************************/
		if (rc == 0)
		{
			//printf("  select() timed out.  End program.\n");
			sleep(1);
			continue;
		}

		/**********************************************************/
		/* One or more descriptors are readable.  Need to         */
		/* determine which ones they are.                         */
		/**********************************************************/
		desc_ready = rc;
		for (i=0; i <= max_sd  &&  desc_ready > 0; ++i)
		{
			/*******************************************************/
			/* Check to see if this descriptor is ready            */
			/*******************************************************/
			if (FD_ISSET(i, &working_set))
			{
				/****************************************************/
				/* A descriptor was found that was readable - one   */
				/* less has to be looked for.  This is being done   */
				/* so that we can stop looking at the working set   */
				/* once we have found all of the descriptors that   */
				/* were ready.                                      */
				/****************************************************/
				desc_ready -= 1;

				/****************************************************/
				/* Check to see if this is the listening socket     */
				/****************************************************/
				if (i == listen_sd)
				{
					//printf("  Listening socket is readable\n");
					/*************************************************/
					/* Accept all incoming connections that are      */
					/* queued up on the listening socket before we   */
					/* loop back and call select again.              */
					/*************************************************/
					/**********************************************/
					/* Accept each incoming connection.  If       */
					/* accept fails with EWOULDBLOCK, then we     */
					/* have accepted all of them.  Any other      */
					/* failure on accept will cause us to end the */
					/* server.                                    */
					/**********************************************/
					new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
							end_server = TRUE;
						}
						break;
					}

					/**********************************************/
					/* Add the new incoming connection to the     */
					/* master read set                            */
					/**********************************************/
					//printf("  New incoming connection - %d\n", new_sd);
					FD_SET(new_sd, &master_set);
					max_sd = new_sd;
					//fd_change=TRUE;
					/*Connect to another server*/
					TCPClient tcp;

					tcp.setup(ippool[round].showAddr(), portpool[round]);
					round = (round+1) % max_round;

					connections[new_sd]=tcp.sock;
					connections[tcp.sock]=new_sd;
					FD_SET(tcp.sock, &master_set);
					if (new_sd > max_sd || tcp.sock > max_sd)
						max_sd = new_sd > tcp.sock ? new_sd : tcp.sock;

					/**********************************************/
					/* Loop back up and accept another incoming   */
					/* connection                                 */
					/**********************************************/
				}

				/****************************************************/
				/* This is not the listening socket, therefore an   */
				/* existing connection must be readable             */
				/****************************************************/
				else
				{
					//printf("  Descriptor %d is readable\n", i);
					close_conn = FALSE;
					/*************************************************/
					/* Receive all incoming data on this socket      */
					/* before we loop back and call select again.    */
					/*************************************************/
					int size = 0;
					//do{
						/* hongyi: cancel loop */
						/**********************************************/
						/* Receive data on this connection until the  */
						/* recv fails with EWOULDBLOCK.  If any other */
						/* failure occurs, we will close the          */
						/* connection.                                */
						/**********************************************/
						rc = recv(i, buffer, sizeof(buffer), 0);
						if (rc < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								perror("  recv() failed");
								close_conn = TRUE;
							}
						}

						/**********************************************/
						/* Check to see if the connection has been    */
						/* closed by the client                       */
						/**********************************************/
						if (rc == 0)
						{
							printf("  Connection closed\n");
							close_conn = TRUE;
						}

						/**********************************************/
						/* Data was received                          */
						/**********************************************/
						len = rc;
						//printf("  %d bytes received\n", len);

						/**********************************************/
						/* Echo the data back to the client           */
						/**********************************************/

						/* Send to connected server */
						rc = send(connections[i], buffer, len, 0);
						/* Send back */
						// rc = send(i, buffer, len, 0)
						if (rc < 0)
						{
							perror("  send() failed");
							close_conn = TRUE;
						}
						//size++;
						//FD_ZERO(&master_set_t);
						//FD_SET(i, &master_set_t);
					//} while ( size <=1000 && select(i+1,&master_set_t,NULL, NULL, &timeout_t )>0);
					//}while (size <=100);
					/*************************************************/
					/* If the close_conn flag was turned on, we need */
					/* to clean up this active connection.  This     */
					/* clean up process includes removing the        */
					/* descriptor from the master set and            */
					/* determining the new maximum descriptor value  */
					/* based on the bits that are still turned on in */
					/* the master set.                               */
					/*************************************************/
					if (close_conn)
					{
						//printf("closing sockets %d and %d ", i, connections[i]);
						close(i);
						close(connections[i]);
						fd_change=TRUE;
						FD_CLR(i, &master_set);
						FD_CLR(connections[i], &master_set);
						if (i == max_sd || connections[i] == max_sd)
						{
							while (FD_ISSET(max_sd, &master_set) == FALSE)
								max_sd -= 1;
						}
					}
				} /* End of existing connection is readable */
			} /* End of if (FD_ISSET(i, &working_set)) */
		} /* End of loop through selectable descriptors */

	} while (end_server == FALSE);

					/*************************************************************/
					/* Clean up all of the sockets that are open                  */
					/*************************************************************/
					for (i=0; i <= max_sd; ++i)
					{
						if (FD_ISSET(i, &master_set))
							close(i);
					}
}
