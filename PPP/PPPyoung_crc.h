#ifndef PPPYOUNG_CRC_H_
#define PPPYOUNG_CRC_H_

typedef unsigned short u16;
typedef unsigned long u32;

u16 pppfcs16(u16 fcs, char* cp, int len);
int tryfcs16(char* cp, int len);

#endif /* PPPYOUNG_CRC_H_ */
