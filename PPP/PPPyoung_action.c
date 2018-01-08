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
#define NOACCOMP 0
#define NOPCOMP 1

#define PPPINITFCS16 0xffff
#define PPPGOODFCS16 0xf0b8

#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "PPPyoung_crc.h"
#include "PPPyoung_headers.h"

unsigned char * TLU(unsigned char *frame, int * event){

	// This layer up : the next higher layer should be sent UP event

		// 상위 계층의 event 를 UP 으로 변경
		*event = UP;

	return frame;
}

unsigned char * TLD(unsigned char *frame, int * event){

	// This layer down : the next higher layer should be sent Down event

		// 상위 계층의 event 를 DOWN 으로 변경
		*event = DOWN;

	return frame;
}

unsigned char * TLS(unsigned char *frame, int * event){

	// This layer started : the next higher layer should be sent Open event

		// 상위 계층의 event 를 OPEN 으로 변경
		*event = OPEN;

	return frame;
}

unsigned char * TLF(unsigned char *frame, int * event){

	// This layer finished : the next higher layer should be sent Close event

		// 상위 계층의 event 를 CLOSE 으로 변경
		*event = CLOSE;

	return frame;
}

unsigned char * IRC(unsigned char *frame){

	// Initialize restart timer : set the counter back to the proper value (Max retransmit) and set the timer interval back to the default

		// 구현하지 않음
		memset(frame, 0x00, 2048);

	return frame;
}

unsigned char * ZRC(unsigned char *frame){

	// Zero restart counter : set the counter to zero and it causes the state machine to pause quietly in Stopping state for one time out period before proceeding to tear the next lower layer

		// 구현하지 않음
		memset(frame, 0x00, 2048);

	return frame;
}

unsigned char * SCR(int mode, unsigned char *frame, int * negotiation){

	// Send new Configure-Request
	int len;
	u16 fcs;
	static int magic = 0;
	unsigned char ID = 0x10;
	unsigned char IP[4] = {0x0A, 0x01, 0x03, 0x14};
	static unsigned char magicnumber[4];

		// 프레임을 정리
		memset(frame, 0x00, 2048);

		if (mode == LCP){

			// Magic-number 를 생성
			if (magic == 0){

				magic = 1;
				srand((unsigned)time(NULL));
	        	magicnumber[0] = (unsigned char)(rand() % 257);
	        	magicnumber[1] = (unsigned char)(rand() % 257);
	        	magicnumber[2] = (unsigned char)(rand() % 257);
	        	magicnumber[3] = (unsigned char)(rand() % 257);
			}

			// LCP 프레임을 제작
			frame[0] = 0x7E;
			frame[1] = 0xFF;
			frame[2] = 0x03;
			frame[3] = 0xC0;
			frame[4] = 0x21;

			// Configure-Req code : 0x01
			frame[5] = 0x01;
			frame[6] = ID;
			frame[7] = 0x00;
			frame[8] = 0x10;

			frame[9] = 0x02;
			frame[10] = 0x06;
			frame[11] = 0x00;
			frame[12] = 0x00;
			frame[13] = 0x00;
			frame[14] = 0x00;

			frame[15] = 0x05;
			frame[16] = 0x06;
			frame[17] = magicnumber[0];
			frame[18] = magicnumber[1];
			frame[19] = magicnumber[2];
			frame[20] = magicnumber[3];

			len = 21;

			// Protocol field compression 옵션 설정
			// 코드 설계상 Configue-Nak 이나 Configure-Reject 에 반응하지 못함
			if (negotiation[NOPCOMP] != 1){

				// PCOMP is on
				frame[len++] = 0x07;
				frame[len++] = 0x02;
				frame[8] += 0x02;
			}

			// Address and Control field Compression 옵션 설정
			// 코드 설계상 Configue-Nak 이나 Configure-Reject 에 반응하지 못함
			if (negotiation[NOACCOMP] != 1){

				// ACCOMP is on
				frame[len++] = 0x08;
				frame[len++] = 0x02;
				frame[8] += 0x02;
			}

			// FCS 값을 생성
			fcs = pppfcs16(PPPINITFCS16, frame + 1, len - 1);
			fcs = fcs ^ 0xFFFF;
			frame[len++] = fcs >> 8;
			frame[len++] = fcs % 0xFF;
			frame[len] = 0x7E;
			tryfcs16(frame + 1, len - 3);

			// Escape 추가
			len = make_escape(frame, len);
		}
		else if (mode == NCP){

			// NCP (IPCP) 프레임을 제작
			frame[0] = 0x7E;

			// IPCP negotiation
			frame[1] = 0x80;
			frame[2] = 0x21;

			// Configure-Req code : 0x01
			frame[3] = 0x01;
			frame[4] = ID;
			frame[5] = 0x00;
			frame[6] = 0x10;

			// IP Conpression protocol 옵션
			// Van Jacobson Compressed TCP / IP : 0x002D
			frame[7] = 0x02;
			frame[8] = 0x06;
			frame[9] = 0x00;
			frame[10] = 0x2D;
			frame[11] = 0x0F;
			frame[12] = 0x01;

			// IP Address 옵션
			// IP : 10.1.3.20 으로 고정
			frame[13] = 0x03;
			frame[14] = 0x06;
			frame[15] = IP[0];
			frame[16] = IP[1];
			frame[17] = IP[2];
			frame[18] = IP[3];

			// FCS 값을 생성
			fcs = pppfcs16(PPPINITFCS16, frame + 1, 18);
			fcs = fcs ^ 0xFFFF;
			frame[19] = fcs >> 8;
			frame[20] = fcs % 0xFF;
			frame[21] = 0x7E;
			tryfcs16(frame + 1, 18);
		}

	return frame;
}

unsigned char * SCA(unsigned char *frame){

	// Send Configure-Ack to last received Configure-Request
	int idx_protocol;
	int len;
	u16 fcs;

		len = 1;
		while (frame[len] != 0x7E){

			if (frame[len] == 0x80 || frame[len] == 0xC0){

				idx_protocol = len;
			}

			len += 1;
		}

		// Configure-Ack code : 0x02
		frame[idx_protocol + 2] = 0x02;

		// FCS 값을 생성
		fcs = pppfcs16(PPPINITFCS16, frame + 1, len - 3);
		fcs = fcs ^ 0xFFFF;
		frame[len - 2] = fcs >> 8;
		frame[len - 1] = fcs % 0xFF;
		tryfcs16(frame + 1, len - 3);

		// Escape 추가
		if (frame[1] != 0x80){

			len = make_escape(frame, len);
		}

	return frame;
}

unsigned char * SCN(unsigned char *frame){

	// Send Configure-Nak or Configure-Reject
	int idx_protocol;
	int len;
	u16 fcs;

		len = 1;
		while (frame[len] != 0x7E){

			if (frame[len] == 0x80 || frame[len] == 0xC0){

				idx_protocol = len;
			}

			len += 1;
		}

		// Configure-Nak code : 0x03
		// 코드 설계상 프레임의 length 를 확인하지 않아 Nak 을 할 이유는 없음
		// 코드 설계상 프레임의 옵션을 확인하지 않아 Reject 를 하는 상황을 구별 못함
		frame[idx_protocol + 2] = 0x03;

		// FCS 값을 생성
		fcs = pppfcs16(PPPINITFCS16, frame + 1, len - 3);
		fcs = fcs ^ 0xFFFF;
		frame[len - 2] = fcs >> 8;
		frame[len - 1] = fcs % 0xFF;
		tryfcs16(frame + 1, len - 3);

		// Escape 추가
		if (frame[1] != 0x80){

			len = make_escape(frame, len);
		}

	return frame;
}

unsigned char * STR(unsigned char *frame){

	// Send Terminate-Request
	u16 fcs;

		// Terminate-Request code is 0x05
		frame[5] = 0x05;

		// Length of field
		frame[7] = 0x00;
		frame[8] = 0x08;

		// change fcs value
		fcs = pppfcs16(PPPINITFCS16, frame + 1, 8);
		fcs = fcs ^ 0xFFFF;
		frame[9] = fcs >> 8;
		frame[10] = fcs % 0xFF;
		tryfcs16(frame + 1, 8);

		// End of frame
		frame[11] = 0x7E;

		// Escaping
		make_escape(frame, 11);

	return frame;
}

unsigned char * STA(unsigned char *frame){

	// Send Terminate-Ack
	u16 fcs;

		// Terminate-Ack code is 0x06
		frame[5] = 0x06;

		// Length of field
		frame[7] = 0x00;
		frame[8] = 0x04;

		// change fcs value
		fcs = pppfcs16(PPPINITFCS16, frame + 1, 8);
		fcs = fcs ^ 0xFFFF;
		frame[9] = fcs >> 8;
		frame[10] = fcs % 0xFF;
		tryfcs16(frame + 1, 8);

		// End of frame
		frame[11] = 0x7E;

		// Escaping
		make_escape(frame, 11);

	return frame;
}

unsigned char * SCJ(unsigned char *frame){

	// Send Code-Reject

		// I did not implement it
		memset(frame, 0x00, 2048);

	return frame;
}

unsigned char * SER(unsigned char *frame){

	// Send Echo-Reply
	int idx;
	int len;
	u16 fcs;

		len = 1;
		while (frame[len] != 0x7E){

			if (frame[len] == 0xC0 || frame[len] ==0x80){

				idx = len;
			}

			len += 1;
		}

		// Echo-Reply code is 0x0A
		frame[len + 2] = 0x0A;

		// change fcs value
		fcs = pppfcs16(PPPINITFCS16, frame + 1, len - 3);
		fcs = fcs ^ 0xFFFF;
		frame[len - 2] = fcs >> 8;
		frame[len - 1] = fcs % 0xFF;
		tryfcs16(frame + 1, len - 3);

	return frame;
}
