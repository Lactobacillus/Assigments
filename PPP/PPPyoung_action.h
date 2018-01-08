#ifndef PPPYOUNG_ACTION_H_
#define PPPYOUNG_ACTION_H_

unsigned char * TLU(unsigned char *frame, int * event);
unsigned char * TLD(unsigned char *frame, int * event);
unsigned char * TLS(unsigned char *frame, int * event);
unsigned char * TLF(unsigned char *frame, int * event);
unsigned char * IRC(unsigned char *frame);
unsigned char * ZRC(unsigned char *frame);
unsigned char * SCR(int mode, unsigned char *frame, int * negotiation);
unsigned char * SCA(unsigned char *frame);
unsigned char * SCN(unsigned char *frame);
unsigned char * STR(unsigned char *frame);
unsigned char * STA(unsigned char *frame);
unsigned char * SCJ(unsigned char *frame);
unsigned char * SER(unsigned char *frame);

#endif /* PPPYOUNG_ACTION_H_ */
