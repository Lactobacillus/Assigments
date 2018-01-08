/*
 *  2014210085.c
 *  Computer Network
 *  Author : Cho Youngwoo
 *  2016.11.10
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define Error -1 				// Error == -1

#define Initial 0				// state 0 : Initial
#define Hello_sent 1 			// state 1 : Hello_sent
#define Wait_instruction 2 		// state 2 : Wait_instruction
#define Terminated 3 			// state 3 : Terminated

#define Start 0					// event 0 : Start
#define Sent_hello 1 			// event 1 : Sent_hello
#define Recieve_hello 2 		// event 2 : Recieve_hello
#define Recieve_instruction 3 	// event 3 : Recieve_instruction
#define Sent_response 4 		// event 4 : Sent_response
#define Recieve_terminate 5 	// event 5 : Recieve_terminate

// state functions
int initial(int * event, int id, unsigned char * frame, int len);
int hello_sent(int * event, int id, unsigned char * frame, int len);
int wait_instruction(int * event, int id, unsigned char * frame, int len);
int response_sent(int * event, int id, unsigned char * frame, int len);
int terminated(int * event, int id, unsigned char * frame, int len);

// action functions
void send_hello(int id, unsigned char * frame, int len);
void inst_echo(int id, unsigned char * frame, int len);
void inst_increment(int id, unsigned char * frame, int len);
void inst_decrement(int id, unsigned char * frame, int len);

// other function
int get_frame(int id, unsigned char * frame);
int change_event(int event, unsigned char * frame, int len);

// main function
int main(void){

	int s;
	int len;
	int state;
	int event;
	unsigned char frame[1024];
	struct sockaddr_in server;
	int (* funcPtr[4]) (int *, int, unsigned char *, int) = {

		initial, hello_sent, wait_instruction, terminated
	};

		// set initial state and event
		len = 12;
		state = Initial;
		event = Start;

		// set addressing system
		bzero((char *)&server, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_port = htons(47500);

		// create socket and check it is valid
		if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){

			printf("[Error] : Exception from creating socket\n");
			
			return Error;
		}

		// connect to server with socket
		if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0){

			printf("[Error] : Exception from connect to server\n");
			close(s);

			return Error;
		}

		// state machine of IR protocol
		while(1){

			if(state == Error || event == Error){

				printf("[Error] : Unexpected state or event\n");

				break;
			}
			else{

				// change state
				state = funcPtr[state](&event, s, frame, len);

				if(state == Terminated){

					printf("\n== Connection has been terminated ==\n");

					break;
				}

				// get new frame and change event
				len = get_frame(s, frame);
				event = change_event(event, frame, len);
			}
		}

		// close socket
		close (s);

	return 0;
}

// initial state
int initial(int * event, int id, unsigned char * frame, int len){

	int new_state;

		switch(*event){

			case Start:

				send_hello(id, frame, len);
				*event = Sent_hello;
				new_state = Hello_sent;

				break;

			default:

				new_state = Error;

				break;
		}

	return new_state;
}

// hello_sent state
int hello_sent(int * event, int id, unsigned char * frame, int len){

	int new_state;

		switch(*event){

			case Sent_hello:

				new_state = Hello_sent;

				break;

			case Recieve_hello:

				new_state = Wait_instruction;
				
				break;

			default:

				new_state = Error;

				break;
		}

	return new_state;
}

// wait_instruction state
int wait_instruction(int * event, int id, unsigned char * frame, int len){

	int new_state;
	unsigned char op;

		switch(*event){

			case Recieve_instruction:

				op = frame[1];

				if(op == 0x00){

					// Echo request
					printf(">> OP : Echo\n");
					inst_echo(id, frame, len);

					*event = Sent_response;
					new_state = Wait_instruction;
				}
				else if(op == 0x01){

					// Increment request
					printf(">> OP : Increment\n");
					inst_increment(id, frame, len);

					*event = Sent_response;
					new_state = Wait_instruction;
				}
				else if(op == 0x02){

					// Decrement request
					printf(">> OP : Decrement\n");
					inst_decrement(id, frame, len);

					*event = Sent_response;
					new_state = Wait_instruction;
				}
				else{

					printf(">> OP : Unknown (Error)\n");
					new_state = Error;
				}

				break;

			case Recieve_terminate:

				new_state = Terminated;

				break;

			default:

				new_state = Error;

				break;
		}

	return new_state;
}

// terminated state
int terminated(int * event, int id, unsigned char * frame, int len){

	return Terminated;
}

// send_hello action
void send_hello(int id, unsigned char * frame, int len){

	int temp;	

		// clear frame
		memset(frame, 0, 1024);

		// writing request
		frame[0] = 0x80;
		frame[7] = 0x04;
		frame[8] = 0x78;
		frame[9] = 0x0E;
		frame[10] = 0x68;
		frame[11] = 0x25;

		// send packet
		send(id, frame, len, 0);

		printf("[Sent]\n");
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[0], frame[1], frame[2], frame[3]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[4], frame[5], frame[6], frame[7]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[8], frame[9], frame[10], frame[11]);
}

// inst_echo action
void inst_echo(int id, unsigned char * frame, int len){

		// writing response
		frame[0] = 0x20;

		// send packet
		send(id, frame, len, 0);

		printf(">> Content : %s\n", frame + 8);
		printf("[Sent]\n");
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[0], frame[1], frame[2], frame[3]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[4], frame[5], frame[6], frame[7]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[8], frame[9], frame[10], frame[11]);
}

// inst_increment action
void inst_increment(int id, unsigned char * frame, int len){

	int value;
	int * value_ptr;
	unsigned char temp_frame[1024];

		// set temp_frame
		memset(temp_frame, 0, 1024);
		memcpy(temp_frame, frame, 8);

		// writing response
		value = ntohl(*(int *)(frame + 8));
		temp_frame[0] = 0x20;
		temp_frame[1] = 0x00;
		value = htonl(value + 1);
		value_ptr = &value;
		memcpy(temp_frame + 8, value_ptr, 4);

		// send packet
		send(id, temp_frame, 12, 0);

		printf(">> Content : %d\n", value);
		printf("[Sent]\n");
		printf("%02X\t%02X\t%02X\t%02X\t\n", temp_frame[0], temp_frame[1], temp_frame[2], temp_frame[3]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", temp_frame[4], temp_frame[5], temp_frame[6], temp_frame[7]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", temp_frame[8], temp_frame[9], temp_frame[10], temp_frame[11]);
}

// inst_decrement action
void inst_decrement(int id, unsigned char * frame, int len){

	int value;
	int * value_ptr;
	unsigned char temp_frame[1024];

		// set temp_frame
		memset(temp_frame, 0, 1024);
		memcpy(temp_frame, frame, 8);

		// writing response
		value = ntohl(*(int *)(frame + 8));
		temp_frame[0] = 0x20;
		temp_frame[1] = 0x00;
		value = htonl(value - 1);
		value_ptr = &value;
		memcpy(temp_frame + 8, value_ptr, 4);
		
		// send packet
		send(id, temp_frame, 12, 0);

		printf(">> Content : %d\n", value);
		printf("[Sent]\n");
		printf("%02X\t%02X\t%02X\t%02X\t\n", temp_frame[0], temp_frame[1], temp_frame[2], temp_frame[3]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", temp_frame[4], temp_frame[5], temp_frame[6], temp_frame[7]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", temp_frame[8], temp_frame[9], temp_frame[10], temp_frame[11]);
}

// get_frame function
int get_frame(int id, unsigned char * frame){

	int idx;
	int len_header;
	int len_data;

		// set frame empty
		memset(frame, 0, 1024);

		// recieve header
		if((len_header = recv(id, frame, 8, MSG_WAITALL)) < 0){

			printf("[Error] : Exception from socket connection\n");

			return Error;
		}

		// recieve data
		if((len_data = recv(id, frame + 8, ntohl(*(int *)(frame + 4)), MSG_WAITALL)) < 0){

			printf("[Error] : Exception from frame header or frame data\n");

			return Error;
		}

		printf("\n[Recieved]\n");
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[0], frame[1], frame[2], frame[3]);
		printf("%02X\t%02X\t%02X\t%02X\t\n", frame[4], frame[5], frame[6], frame[7]);
		for (idx = 0; idx < len_data; idx++){

			printf("%02X\t", frame[8 + idx]);
		}
		printf("\n>> Data Length : %d\n", len_data);

	return len_header + len_data;
}

// change_event function
int change_event(int event, unsigned char * frame, int len){

	int new_event;

		// exception from get_frame function
		if(len < 0){

			printf("[Error] : Exception from get_frame\n");

			return Error;
		}

		// check flag
		switch(frame[0]){

			case 0x10:

				// Terminate
				new_event = Recieve_terminate;
				printf(">> Flag : Terminate\n");

				break;

			case 0x20:

				// Response
				new_event = Error;
				printf(">> Flag : Response\n");

				break;

			case 0x40:

				// Instruction
				new_event = Recieve_instruction;
				printf(">> Flag : Instruction\n");

				break;

			case 0x80:

				// Hello
				new_event = Recieve_hello;
				printf(">> Flag : Hello\n");

				break;

			default:

				new_event = Error;
				printf(">> Flag : Unknown (Error)\n");

				break;
		}

	return new_event;
}