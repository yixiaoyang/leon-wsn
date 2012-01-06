#ifndef AHBUART_H
#define AHBUART_H


/* Frame structure of docm transmission packet */
//*
typedef struct {
	unsigned char ctrl;       // Control byte
	unsigned long addr;       // Address
	unsigned long data[0];    // Data write/read
} __attribute__((packed)) DcomPacket;
//*/

extern int uartFd;
/* Function prototype */
int WriteWord(unsigned long dstaddr, unsigned long word);
int ReadWord(unsigned long dstaddr, unsigned long *word);
int ReadNWords(int, unsigned long, unsigned char buf[][4], int);
int connect_dcom();
#endif // AHBUART_H
