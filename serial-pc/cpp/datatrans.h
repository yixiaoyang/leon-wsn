/*
 * Created:2012.1.3
 * by:  xiaoyang yi
 */
 

#ifndef DATATRANS_H
#define DATATRANS_H

//try to get data, failed if try more than TRY_COUNT times
#define TRY_COUNT	1

//status
#define STATUS_FAILED	-1
#define STATUS_OK		1

//buffer size defination
#define RX_BUF_SIZE		64
#define TX_BUF_SIZE		64
			
//data end flag
#define TRANS_END		"[["
#define TRANS_OK		"]]"

//
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>		//
#include <termios.h>	//posix
#include <errno.h>		//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include <string>
#include <iostream>
using namespace std;

#ifndef FALSE 
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		1
#endif

#define WORDLEN		32

#define UART_COUNT	4
#define UART0	0
#define UART1	1
#define UART2	2
#define UART3	3

//filename of record
#define RECORD_FILENAME	"data.dat"

#define UART_DEBUG		1
#define UART_DPRINT	\
	if(UART_DEBUG)\
		printf

struct serial_config
{
	unsigned char serial_dev[WORDLEN];
	unsigned int serial_speed;
	unsigned char databits;
	unsigned char stopbits;
	unsigned char parity;
};
//
//class QThread;

class DataTrans
{
	public:
		DataTrans();
		~DataTrans();
	
		//which dev can be UART0~3
		int uart_init(unsigned char which_dev);

		//test
		int uart1_test();
		//test
		
		int get_status(unsigned char which_dev);
		//buffer for data trans,only one uart can use the buffer
		char rx_buffer[RX_BUF_SIZE];
		char tx_buffer[TX_BUF_SIZE];
		//unsigned int which_node;
	protected:
		int status;
		int serial_fd[UART_COUNT];
		struct serial_config uart_cfg[UART_COUNT];
		
		void print_serialread(unsigned char which_dev);
		int read_uart_cfg(unsigned char which_dev);
		void set_speed(unsigned char which_dev);
		int set_parity(unsigned char which_dev);
		int open_dev(unsigned char *dev);

		int read_serial_port (int fd, char *buffer, int size,int * readcount, struct timeval *timeout);
};


#endif
