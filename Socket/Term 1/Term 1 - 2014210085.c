/*
 *  2014210085.c
 *  Computer Network
 *  Author : Cho Youngwoo
 *  2016.09.27
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void){

	int s;
	char buffer[100];
	struct sockaddr_in server;

		// set my student ID
		strncpy(buffer, "2014210085", 10);

		// set addressing system
		bzero((char *)&server, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_port = htons(47500);

		// create socket and check it is valid
		if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){

			printf("Not good : socket\n");
			
			return -1;
		}

		// connect to server with socket
		if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0){

			printf("Not good : connect\n");
			close(s);

			return -1;
		}

		// send data
		send(s, buffer, strlen(buffer) + 1, 0);

		// close socekt
		close (s);

	return 0;
}
