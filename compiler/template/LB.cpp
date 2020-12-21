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
#include "basic_classes.h"
#include "TCPClient.h"


#define MAX_PACKET_SIZE 1000000
#define TRUE             1
#define FALSE            0

#define SERVER_ADDR  "10.0.0.2"
#define SERVER_PORT  12345
unordered_map<int, int> connections;
State<int> round(new int(0));
int max_round = 5;
unordered_map<int, IP> ippool;
unordered_map<int, int> portpool;
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



void stop(int sig){
	exit(0);
}
main (int argc, char *argv[])
{
	int    i, len, rc, on = 1;
	int    listen_sd, max_sd;
	int    desc_ready, end_server = FALSE;
	int    close_conn;
	char   buffer[MAX_PACKET_SIZE];
	struct sockaddr_in   addr;
	struct timeval       timeout, timeout_t;
	fd_set        master_set, working_set, master_set_t;
	signal(SIGINT,stop);

	_init_();

	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(-1);
	}
	rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
			(char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}
	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &addr.sin_addr);
	addr.sin_port        = htons(SERVER_PORT);
	rc = bind(listen_sd,
			(struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}

	rc = listen(listen_sd, 32);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}

	FD_ZERO(&master_set);
	max_sd = listen_sd;
	FD_SET(listen_sd, &master_set);

	timeout.tv_sec  = 0;
	timeout.tv_usec = 0;

	// PARAM to config
	timeout_t.tv_sec = 0;
	timeout_t.tv_usec = 0;

	do
	{
		memcpy(&working_set, &master_set, sizeof(master_set));

		rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);
		if (rc < 0)
		{
			perror("  select() failed");
			break;
		}
		if (rc == 0)
		{
			sleep(1);
			continue;
		}
		desc_ready = rc;
		for (i=0; i <= max_sd  &&  desc_ready > 0; ++i)
		{
			if (FD_ISSET(i, &working_set))
			{
				if (i == listen_sd)
				{

					//accept
					int new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
							end_server = TRUE;
						}
						break;
					}
					FD_SET(new_sd, &master_set);
					max_sd = max(max_sd,new_sd);

					round = (round+1) % max_round;

					TCPClient tcp;
					int s1 = tcp.setup(ippool[round].showAddr(), portpool[round]);
					FD_SET(s1, &master_set);
					max_sd = max( max_sd, s1 );

					connections[new_sd]=s1;
					connections[s1]=new_sd;

				}
				else if (!(i == listen_sd))
				{

						recv(i, buffer, sizeof(buffer), 0);


						/* Send to connected server */
						rc = send(connections[i], buffer, len, 0);
						/* Send back */
						// rc = send(i, buffer, len, 0)


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
