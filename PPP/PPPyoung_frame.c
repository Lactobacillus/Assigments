#define CONFIGURE_REQUEST 1
#define CONFIGURE_ACK 2
#define CONFIGURE_NAK 3
#define CONFIGURE_REJECT 4
#define TERMINATE_REQUEST 5
#define TERMINATE_ACK 6
#define CODE_REJECT 7
#define PROTOCOL_REJECT 8
#define ECHO_REQUEST 9
#define ECHO_REPLY 10
#define DISCARD_REQUEST 11

#define UP 0
#define DOWN 1
#define OPEN 2
#define CLOSE 3
#define TO_P 4
#define TO_M 5
#define RCR_P 6
#define RCR_M 7
#define RCA 8
#define RCN 9
#define RTR 10
#define RTA 11
#define RUC 12
#define RXJ_P 13
#define RXJ_M 14
#define RXR 15

#define LCP 0
#define NCP 1
#define NOACCOMP 0
#define NOPCOMP 1

#define ASYNCMAP 2
#define MAGICNUMBER 5
#define PCOMP 7
#define ACCOMP 8



#define QUEUESIZE 4095

#include <stdio.h>

int isQueueEmpty(int front, int rear){

		if (front == rear){

			return 1;
		}

	return 0;
}

int isQueueFull(int front, int rear){

		if ((front == 0 && rear == QUEUESIZE - 1) || front == rear + 1){

			return 1;
		}

	return 0;
}

int nextIdxQueue(int idx){

		if (idx == QUEUESIZE - 1){

			return 0;
		}

	return idx + 1;
}

int previousIdxQueue(int idx){

		if (idx == 0){

			return QUEUESIZE - 1;
		}

	return idx - 1;
}

int del_escape(unsigned char * frame, int len){

	int idx = 0;
	int idx_temp = 0;
	unsigned char frame_temp[2048];

		if (len < 3){

			return 0;
		}

		while (idx < len){

			if (frame[idx] == 0x7D){

				idx += 1;
				frame_temp[idx_temp] = frame[idx] ^ 0x20;
				idx += 1;
				idx_temp += 1;
			}
			else if (frame[idx] == 0x7D && frame[idx + 1] == 0x7D){

				frame_temp[idx_temp] = 0x7D ^ 0x20;
				idx += 2;
				idx_temp += 1;
			}
			else{

				frame_temp[idx_temp] = frame[idx];
				idx += 1;
				idx_temp += 1;
			}
		}

		for (idx = 0; idx < idx_temp; idx++){

			frame[idx] = frame_temp[idx];
		}

	return idx_temp;
}

int make_escape(unsigned char * frame, int len){

	int idx = 0;
	int idx_temp = 0;
	unsigned char frame_temp[2048];

		while (idx < len){

			if (frame[idx] < 0x20){

				frame_temp[idx_temp] = 0x7D;
				frame_temp[idx_temp + 1] = frame[idx] ^ 0x20;
				idx += 1;
				idx_temp += 2;
			}
			else if (frame[idx] == 0x7D){

				frame_temp[idx_temp] = 0x7D;
				frame_temp[idx_temp + 1] = 0x7D;
				idx += 1;
				idx_temp += 2;
			}
			else{

				frame_temp[idx_temp] = frame[idx];
				idx += 1;
				idx_temp += 1;
			}
		}

		for (idx = 0; idx < idx_temp; idx++){

			frame[idx] = frame_temp[idx];
		}

		frame[idx] = 0x7E;

	return idx_temp + 3;
}

void change_event(int * LCP_event, int * NCP_event, unsigned char * frame){

	int idx_protocol;
	unsigned char code;

		// 프로토콜이 있는 인덱스를 확인
		idx_protocol = 1;
		while (!(frame[idx_protocol] == 0x80 || frame[idx_protocol] == 0xC0)){

			idx_protocol += 1;
		}

		code = frame[idx_protocol + 2];

		if (frame[idx_protocol] == 0xC0 && frame[idx_protocol + 1] == 0x21){

			// LCP 프레임인 경우
            switch (code){

                case CONFIGURE_REQUEST:

                	// RCR_M 에 대한 처리는 하지 않음
                	*LCP_event = RCR_P;
                	printf("New LCP_event : RCR+\n");
                	break;

				case CONFIGURE_ACK:

					*LCP_event = RCA;
					printf("New LCP_event : RCA\n");
					break;

				case CONFIGURE_NAK:
				case CONFIGURE_REJECT:

					*LCP_event = RCN;
					printf("New LCP_event : RCN\n");
					break;

				case TERMINATE_REQUEST:

					*LCP_event = RTR;
					printf("New LCP_event : RTR\n");
					break;

				case TERMINATE_ACK:

					*LCP_event = RTA;
					printf("New LCP_event : RTA\n");
					break;

				case CODE_REJECT:
				case PROTOCOL_REJECT:

					*LCP_event = RXJ_M;
					printf("New LCP_event : RXJ-\n");
					break;

				case ECHO_REQUEST:
				case ECHO_REPLY:
				case DISCARD_REQUEST:

					*LCP_event = RXR;
					printf("New LCP_event : RXR\n");
					break;

				default:

					break;
            }
        }
        else if (frame[idx_protocol] == 0x80 && frame[idx_protocol + 1] == 0x21){

        	// IPCP 프레임인 경우
        	switch (code){

        		case CONFIGURE_REQUEST:

        			// RCR_M 에 대한 처리는 하지 않음
        			*NCP_event = RCR_P;
        			printf("New NCP_event : RCR+\n");
        			break;

				case CONFIGURE_ACK:

					*NCP_event = RCA;
					printf("New NCP_event : RCA\n");
					break;

				case CONFIGURE_NAK:
				case CONFIGURE_REJECT:

					*NCP_event = RCN;
					printf("New NCP_event : RCN\n");
					break;

				case TERMINATE_REQUEST:

					*NCP_event = RTR;
					printf("New NCP_event : RTR\n");
					break;

				case TERMINATE_ACK:

					*NCP_event = RTA;
					printf("New NCP_event : RTA\n");
					break;

				case CODE_REJECT:
				case PROTOCOL_REJECT:

					*NCP_event = RXJ_P;
					printf("New NCP_event : RXJ+\n");
					break;

				case ECHO_REQUEST:
				case ECHO_REPLY:
				case DISCARD_REQUEST:

					*NCP_event = RXR;
					printf("New NCP_event : RXR\n");
					break;

				default:

					break;
        	}
        }
}

