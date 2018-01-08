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

#define INITIAL 0
#define STARTING 1
#define CLOSED 2
#define STOPPED 3
#define CLOSING 4
#define STOPPING 5
#define REQ_SENT 6
#define ACK_RCVD 7
#define ACK_SENT 8
#define OPENED 9

#define tlu 1
#define tld 2
#define tls 3
#define tlf 4
#define irc 5
#define zrc 6
#define scr 7
#define sca 8
#define scn 9
#define str 10
#define sta 11
#define scj 12
#define ser 13

#define LCP 0
#define NCP 1
#define ACCOMP 0
#define PCOMP 1

#include <stdio.h>
#include <time.h>
#include "PPPyoung_action.h"

// [p] Passive option; see Stopped State discussion.
// [r] Restart option; see Open event discussion.
// [x] Crossed connection; see RCA event discussion.

int initial(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Up : 2 (Closed)
	// Open : TLS / 1 (Starting)
	// Close : 0 (Initial)

		switch (*This_event){

			case UP:

				return CLOSED;
				break;

			case OPEN:

				frame = TLS(frame, UPPER_event);
				return STARTING;
				break;

			case CLOSE:

				return INITIAL;
				break;

			default:

				// There is something wrong
				printf("INITIAL\n");
				return -1;
				break;
		}
}

int starting(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Up : IRC, SCR / 6 (Req-Sent)
	// Open : 1 (Starting)
	// Close : TLF / 0 (Initial)

		if (mode == LCP){

			// LCP 의 경우에는 UP 이벤트를 강제로 줌
			*This_event  = UP;
			printf("makeUP\n");
		}

		switch (*This_event){

			case UP:

				frame = IRC(frame);
				frame = SCR(mode, frame, negotiation);
				return REQ_SENT;
				break;

			case OPEN:

				return STARTING;
				break;

			case CLOSE:

				frame = TLF(frame, UPPER_event);
				return INITIAL;
				break;

			default:

				// There is something wrong
				printf("STARTING\n");
				return -1;
				break;
		}
}

int closed(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : 0 (Initial)
	// Open : IRC, SCR / 6 (Req-Sent)
	// Close : 2 (Closed)
	// RCR+ : STA / 2 (Closed)
	// RCR- : STA / 2 (Closed)
	// RCA : STA / 2 (Closed)
	// RCN : STA / 2 (Closed)
	// RTR : STA / 2 (Closed)
	// RTA : 2 (Closed)
	// RUC : SCJ / 2 (Closed)
	// RXJ+ : 2 (Closed)
	// RXJ- : TLF / 2 (Closed)
	// RXR : 2 (Closed)

		switch (*This_event){

			case DOWN:

				return INITIAL;
				break;

			case OPEN:

				frame = IRC(frame);
				frame = SCR(mode, frame, negotiation);
				return REQ_SENT;
				break;

			case CLOSE:

				return CLOSED;
				break;

			case RCR_P:
			case RCR_M:
			case RCA:
			case RCN:
			case RTR:

				frame = STA(frame);
				return CLOSED;
				break;

			case RTA:

				return CLOSED;
				break;

			case RUC:

				frame = SCJ(frame);
				return CLOSED;
				break;

			case RXJ_P:

				return CLOSED;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return CLOSED;
				break;

			case RXR:

				return CLOSED;
				break;

			default:

				// There is something wrong
				printf("CLOSED\n");
				return -1;
				break;
		}
}

int stopped(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : TLS / 1 (Starting)
	// Open : 3r (Stopped)
	// Close : 2 (Closed)
	// RCR+ : IRC, SCR, SCA / 8 (Ack-Sent)
	// RCR- : IRC, SCR, SCN / 6 (Req-Sent)
	// RCA : STA / 3 (Stopped)
	// RCN : STA / 3 (Stopped)
	// RTR : STA / 3 (Stopped)
	// RTA : 3 (Stopped)
	// RUC : SCJ / 3 (Stopped)
	// RXJ+ : 3 (Stopped)
	// RXJ- : TLF / 3 (Closed)
	// RXR : 3 (Stopped)

		switch (*This_event){

			case DOWN:

				frame = TLS(frame, UPPER_event);
				return STARTING;
				break;

			case OPEN:

				// restart option
				return STOPPED;
				break;

			case CLOSE:

				return STOPPED;
				break;

			case RCR_P:

				frame = IRC(frame);
				frame = SCR(mode, frame, negotiation);
				frame = SCA(frame);
				return ACK_SENT;
				break;

			case RCR_M:

				frame = IRC(frame);
				frame = SCR(mode, frame, negotiation);
				frame = SCN(frame);
				return REQ_SENT;
				break;

			case RCA:
			case RCN:
			case RTR:

				frame = STA(frame);
				return STOPPED;
				break;

			case RTA:

				return STOPPED;
				break;

			case RUC:

				frame = SCJ(frame);
				return STOPPED;
				break;

			case RXJ_P:

				return STOPPED;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RXR:

				return STOPPED;
				break;

			default:

				// There is something wrong
				printf("STOPPED\n");
				return -1;
				break;
		}
}

int closing(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : 0 (Initial)
	// Open : 5r (Stopping)
	// Close : 4 (Closing)
	// TO+ : STR / 4 (Closing)
	// TO- : TLF / 2 (Closed)
	// RCR+ : 4 (Closing)
	// RCR- : 4 (Closing)
	// RCA : 4 (Closing)
	// RCN : 4 (Closing)
	// RTR : STA / 4 (Closing)
	// RTA : TLF / 2 (Closed)
	// RUC : SCJ / 4 (Closing)
	// RXJ+ : 4 (Closing)
	// RXJ- : TLF / 2 (Closed)
	// RXR : 4 (Closing)

		switch (*This_event){

			case DOWN:

				return INITIAL;
				break;

			case OPEN:

				// restart
				return STOPPING;
				break;

			case CLOSE:

				return CLOSING;
				break;

			case TO_P:

				frame = STR(frame);
				return CLOSING;
				break;

			case TO_M:

				frame = TLF(frame, UPPER_event);
				return CLOSED;
				break;

			case RCR_P:
			case RCR_M:
			case RCA:
			case RCN:

				return CLOSING;
				break;

			case RTR:

				frame = STA(frame);
				return CLOSING;
				break;

			case RTA:

				frame = TLF(frame, UPPER_event);
				return CLOSED;
				break;

			case RUC:

				frame = SCJ(frame);
				return CLOSING;
				break;

			case RXJ_P:

				return CLOSING;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return CLOSED;
				break;

			case RXR:

				return CLOSING;
				break;

			default:

				// There is something wrong
				printf("CLOSING\n");
				return -1;
				break;
		}
}

int stopping(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : 1 (Starting)
	// Open : 5r (Stopping)
	// Close : 4 (Closing)
	// TO+ : STR / 5 (Stopping)
	// TO- : TLF / 3 (Stopped)
	// RCR+ : 5 (Stopping)
	// RCR- : 5 (Stopping)
	// RCA : 5 (Stopping)
	// RCN : 5 (Stopping)
	// RTR : STA / 5 (Stopping)
	// RTA : TLF / 3 (Stopped)
	// RUC : SCJ / 5 (Stopping)
	// RXJ+ : 5 (Stopping)
	// RXJ- : TLF / 3 (Stopped)
	// RXR : 5 (Stopping)

		switch (*This_event){

			case DOWN:

				return STARTING;
				break;

			case OPEN:

				// restarrt
				return STOPPING;
				break;

			case CLOSE:

				return CLOSING;
				break;

			case TO_P:

				frame = STR(frame);
				return STOPPING;
				break;

			case TO_M:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RCR_P:
			case RCR_M:
			case RCA:
			case RCN:

				return STOPPING;
				break;

			case RTR:

				frame = STA(frame);
				return STOPPING;
				break;

			case RTA:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RUC:

				frame = SCJ(frame);
				return STOPPING;
				break;

			case RXJ_P:

				return STOPPING;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RXR:

				return STOPPING;
				break;

			default:

				// There is something wrong
				printf("STOPPING");
				return -1;
				break;
		}
}

int req_sent(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : 1 (Starting)
	// Open : 6 (Req-Sent)
	// Close : IRC, STR / 4 (Closing)
	// TO+ : SCR / 6 (Req-Sent)
	// TO- : TLF / 3p (Stopped)
	// RCR+ : SCA / 8 (Ack-Sent)
	// RCR- : SCN / 6 (Req-Sent)
	// RCA : IRC / 7 (Ack-Rcvd)
	// RCN : IRC, SCR / 6 (Req-Sent)
	// RTR : STA / 6 (Req-Sent)
	// RTA : 6 (Req-Sent)
	// RUC : SCJ / 6 (Req-Sent)
	// RXJ+ : 6 (Req-Sent)
	// RXJ- : TLF / 3 (Stopped)
	// RXR : 6 (Req-Sent)

		if (mode = NCP && *This_event == UP){

			return REQ_SENT;
		}

		switch (*This_event){

			case DOWN:

				return STARTING;
				break;

			case OPEN:

				return REQ_SENT;
				break;

			case CLOSE:

				frame = IRC(frame);
				frame = STR(frame);
				return CLOSING;
				break;

			case TO_P:

				frame = SCR(mode, frame, negotiation);
				return REQ_SENT;
				break;

			case TO_M:

				frame = TLF(frame, UPPER_event);
				// passive
				return STOPPED;
				break;

			case RCR_P:

				frame = SCA(frame);
				return ACK_SENT;
				break;

			case RCR_M:

				frame = SCN(frame);
				return REQ_SENT;
				break;

			case RCA:

				frame = IRC(frame);
				return ACK_RCVD;
				break;

			case RCN:

				frame = IRC(frame);
				frame = SCR(mode, frame, negotiation);
				return REQ_SENT;
				break;

			case RTR:

				frame = STA(frame);
				return REQ_SENT;
				break;

			case RTA:

				return REQ_SENT;
				break;

			case RUC:

				frame = SCJ(frame);
				return REQ_SENT;
				break;

			case RXJ_P:

				return REQ_SENT;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RXR:

				return REQ_SENT;
				break;

			default:

				// There is something wrong
				printf("REQ_SENT\n");
				return -1;
				break;
		}
}

int ack_rcvd(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : 1 (Starting)
	// Open : 7 (Ack-Rcvd)
	// Close : IRC, STR / 4 (Closing)
	// TO+ : SCR / 6 (Req-Sent)
	// TO- : TLF / 3p (Stopped)
	// RCR+ : SCA, tlu / 9 (Opened)
	// RCR- : SCN / 7 (Ack-Rcvd)
	// RCA : SCR / 6x (Req-Sent)
	// RCN : SCR / 6x (Req-Sent)
	// RTR : STA / 6 (Req-Sent)
	// RTA : 6 (Req-Sent)
	// RUC : SCJ / 7 (Ack-Rcvd)
	// RXJ+ : 6 (Req-Sent)
	// RXJ- : TLF / 3 (Stopped)
	// RXR : 7 (Ack-Rcvd)

		printf("\n\nack_rcvd : %d\n\n", *This_event);

		switch (*This_event){

			case DOWN:

				return STARTING;
				break;

			case OPEN:

				return ACK_RCVD;
				break;

			case CLOSE:

				frame = IRC(frame);
				frame = STR(frame);
				return CLOSING;
				break;

			case TO_P:

				frame = SCR(mode, frame, negotiation);
				return REQ_SENT;
				break;

			case TO_M:

				frame = TLF(frame, UPPER_event);
				// passive
				return STOPPED;
				break;

			case RCR_P:

				frame = SCA(frame);
				frame = TLU(frame, UPPER_event);
				return OPENED;
				break;

			case RCR_M:

				frame = SCN(frame);
				return ACK_RCVD;
				break;

			case RCA:
			case RCN:

				frame = SCR(mode, frame, negotiation);
				// crossed
				return REQ_SENT;
				break;

			case RTR:

				frame = STA(frame);
				return REQ_SENT;
				break;

			case RTA:

				return REQ_SENT;
				break;

			case RUC:

				frame = SCJ(frame);
				return ACK_RCVD;
				break;

			case RXJ_P:

				return REQ_SENT;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RXR:

				return ACK_RCVD;
				break;

			default:

				// There is something wrong
				printf("ACK_RCVD\n");
				return -1;
				break;
		}

}

int ack_sent(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : 1 (Starting)
	// Open : 8 (Ack-Sent)
	// Close : IRC, STR / 4 (Closing)
	// TO+ : SCR / 8 (Ack-Sent)
	// TO- : TLF / 3p (Stopped)
	// RCR+ : SCA / 8 (Ack-Sent)
	// RCR- : SCN / 6 (Req-Sent)
	// RCA : IRC, tlu / 9 (Opened)
	// RCN : IRC, SCR / 8 (Ack-Sent)
	// RTR : STA / 6 (Req-Sent)
	// RTA : 8 (Ack-Sent)
	// RUC : SCJ / 8 (Ack-Sent)
	// RXJ+ : 8 (Ack-Sent)
	// RXJ- : TLF / 3 (Stopped)
	// RXR : 8 (Ack-Sent)

		switch (*This_event){

			case DOWN:

				return STARTING;
				break;

			case OPEN:

				return ACK_SENT;
				break;

			case CLOSE:

				frame = IRC(frame);
				frame = STR(frame);
				return CLOSING;
				break;

			case TO_P:

				frame = SCR(mode, frame, negotiation);
				return ACK_SENT;
				break;

			case TO_M:

				frame = TLF(frame, UPPER_event);
				// passive
				return STOPPED;
				break;

			case RCR_P:

				frame = SCA(frame);
				return ACK_SENT;
				break;

			case RCR_M:

				frame = SCN(frame);
				return REQ_SENT;
				break;

			case RCA:

				frame = IRC(frame);
				frame = TLU(frame, UPPER_event);
				return OPENED;
				break;

			case RCN:

				frame = IRC(frame);
				frame = SCR(mode, frame, negotiation);
				return ACK_SENT;
				break;

			case RTR:

				frame = STA(frame);
				return REQ_SENT;
				break;

			case RTA:

				return ACK_SENT;
				break;

			case RUC:

				frame = SCJ(frame);
				return ACK_SENT;
				break;

			case RXJ_P:

				return ACK_SENT;
				break;

			case RXJ_M:

				frame = TLF(frame, UPPER_event);
				return STOPPED;
				break;

			case RXR:

				return ACK_SENT;
				break;

			default:

				// There is something wrong
				printf("ACK_SENT\n");
				return -1;
				break;
		}
}

int opened(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation, clock_t * time){

	// Down : TLD / 1 (Initial)
	// Open : 9r (Opened)
	// Close : TLD, IRC, STR / 4 (Closing)
	// RCR+ : TLD, SCR, SCA / 8 (Ack-Sent)
	// RCR- : TLD, SCR, SCN / 6 (Req-Sent)
	// RCA : TLD, SCR / 6x (Req-Sent)
	// RCN : TLD, SCR / 6x (Req-Sent)
	// RTR : TLD, zrc, STA / 5 (Stopping)
	// RTA : TLD, SCR / 6 (Req-Sent)
	// RUC : SCJ / 9 (Opened)
	// RXJ+ : 9 (Opened)
	// RXJ- : TLD, IRC, STR / 5 (Stopping)
	// RXR : SER / 9 (Opened)

		switch (*This_event){

			case DOWN:

				frame = TLD(frame, UPPER_event);
				return INITIAL;
				break;

			case OPEN:

				// restart
				return OPENED;
				break;

			case CLOSE:

				frame = TLD(frame, UPPER_event);
				frame = IRC(frame);
				frame = STR(frame);
				return CLOSING;
				break;

			case RCR_P:

				frame = TLD(frame, UPPER_event);
				frame = SCR(mode, frame, negotiation);
				frame = SCA(frame);
				return ACK_SENT;
				break;

			case RCR_M:

				frame = TLD(frame, UPPER_event);
				frame = SCR(mode, frame, negotiation);
				frame = SCN(frame);
				return REQ_SENT;
				break;

			case RCA:
			case RCN:

				frame = TLD(frame, UPPER_event);
				frame = SCR(mode, frame, negotiation);
				// crossed
				return REQ_SENT;
				break;

			case RTR:

				frame = TLD(frame, UPPER_event);
				frame = ZRC(frame);
				frame = STA(frame);
				return STOPPING;
				break;

			case RTA:

				frame = TLD(frame, UPPER_event);
				frame = SCR(mode, frame, negotiation);
				return REQ_SENT;
				break;

			case RUC:

				frame = SCJ(frame);
				return OPENED;
				break;

			case RXJ_P:

				return OPENED;
				break;

			case RXJ_M:

				frame = TLD(frame, UPPER_event);
				frame = IRC(frame);
				frame = STR(frame);
				return STOPPING;
				break;

			case RXR:

				frame = SER(frame);
				return OPENED;
				break;

			default:

				// There is something wrong
				printf("OPENED\n");
				return -1;
				break;
		}
}
