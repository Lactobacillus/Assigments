#ifndef PPPYOUNG_HEADERS_H_
#define PPPYOUNG_HEADERS_H_

int isQueueEmpty(int front, int rear);
int isQueueFull(int front, int rear);
int nextIdxQueue(int idx);
int previousIdxQueue(int idx);
int del_escape(unsigned char * frame, int len);
int make_escape(unsigned char * frame, int len);
void change_event(int * LCP_event, int * NCP_event, unsigned char * frame);

int initial(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int starting(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int closed(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int stopped(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int closing(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int stopping(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int req_sent(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int ack_rcvd(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int ack_sent(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);
int opened(int mode, int * This_event, int * UPPER_event, unsigned char * frame, int * negotiation);

#endif /* PPPYOUNG_HEADERS_H_ */
