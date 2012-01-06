#include "ahbuart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "toolkit.h"
int uartFd;
/********************************************************************
 * Connect to DCOM via UART
 * ******************************************************************/
tcflag_t baud2flag(unsigned long baud)
{
	switch(baud) {
		case 9600:
		return B9600;
		case 19200:
		return B19200;
		case 38400:
		return B38400;
		case 57600:
		return B57600;
		case 115200:
		return B115200;
		default:
		return 0;
	}
}
int connect_dcom(const char* port, unsigned long baud)
{
	char uartport[20] = "/dev/ttyUSB0";
	if(strcmp(port, "")!=0) {
		strcpy(uartport, port);
	}
	/* Open UART */
	uartFd = open(uartport, O_RDWR|O_NOCTTY|O_NDELAY);
	if(uartFd==-1) {
		perror("Unable to open uart");
		return -1;
	}else {
		fcntl(uartFd, F_SETFL, 0);
	}
	/* Setup UART */
	struct termios options, old;
	tcgetattr(uartFd, &old);
	tcgetattr(uartFd, &options);          // Current config
	cfsetispeed(&options, baud2flag(baud)==0?B115200:baud2flag(baud));	        // Baudrate=9600
	cfsetospeed(&options, baud2flag(baud)==0?B115200:baud2flag(baud));
	options.c_cflag &= ~(CSIZE|PARENB|CSTOPB); // 8N1
	options.c_cflag |= CS8;

	options.c_lflag &= ~(ICANON|ISIG|ECHO|ECHOE);  // Local options
	options.c_iflag &= ~INPCK;      // Disable input parity check
	options.c_iflag &= ~(IXON|IXOFF|IXANY); // Disable software flow control
	options.c_oflag &= ~OPOST;             // Raw output

  options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 30;               // Timeout = 3s
	tcsetattr(uartFd, TCSAFLUSH, &options);
	/* Connect to AHBUART */
	unsigned char pre[2]={0x55, 0x55};
	int nc = 0;
	nc = write(uartFd, pre, sizeof(pre));
	if(nc<0) {
		perror("Error: synchronous bytes with ahbuart\n");
		return -1;
	} else {
		printf("Conencted through port %s @ %lu baud\n", uartport, baud2flag(baud)==0?115200:baud);
		return 0;
	}
}

// Write:
// +--+-----+------------+---------------+-----------------------+
// |11|LEN-1| ADDR[31:0] | DATA[0][31:0] |...| DATA[LEN-1][31:0] |
// +--+-----+------------+---------------+-----------------------+
int WriteWord(unsigned long dstaddr, unsigned long word)
{
	char buf[512];
	char *ppk=buf;
	DcomPacket *pk = (DcomPacket*)buf;
	pk->ctrl = (3<<6);
	pk->addr = SWAP32(dstaddr);
	ppk+=sizeof(DcomPacket);
	*pk->data = SWAP32(word);
	ppk+=4;
	int NChar = write(uartFd, buf, ppk-buf);
	return 0;
}

// Read:
// +--+-----+------------+----------------+------------------+
// |10|LEN-1| ADDR[31:0] | ~~~~~~~~~~~~~~ | DATA[0][31:0]... |
// +--+-----+------------+----------------+------------------+
int ReadWord(unsigned long dstaddr, unsigned long* word)
{
	DcomPacket pk;
	pk.ctrl = (2<<6);			// Length set to 1 word
	pk.addr = SWAP32(dstaddr);
	int NChar = write(uartFd, (char*)&pk, sizeof(pk));
	int i, count=0;
	char* buf = (unsigned char*)word;
	/* read back */
  for(i=0; i<4; i++){
		if(read(uartFd, &buf[i], 1)<1) {
			break;
		}else {
			count++;
		}
	}
	//printf("receive %d\n", count);
	if(count < 4) return 0;
	else return 1;
}
int ReadNWords(int device, unsigned long dstaddr, unsigned char buf[][4], int n)
{
	int i, j;
	DcomPacket pk;
	pk.ctrl = (2<<6) | (n-1);
	pk.addr = SWAP32(dstaddr);
	int NChar, count=0;
	NChar = write(device, (char*)&pk, sizeof(pk));
	//printf("Send %d bytes\n", NChar);
	for(i=0;i<n;i++) {
		for(j=0; j<4; j++) {
			NChar = read(device, &buf[i][j], 1);
			if(NChar<1) {
    		break;
			}else {
				count++;
			}
		}
	}
	return count;
}
