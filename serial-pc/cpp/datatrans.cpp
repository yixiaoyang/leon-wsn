/*
 * Created:2012.1.3
 * by:  xiaoyang yi
 */
 

#include "datatrans.h"

const unsigned int speed_arr[] = {
    B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
    B38400, B19200, B9600, B4800, B2400, B1200, B300
};

const unsigned int name_arr[] = {
    230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300,
    38400, 19200, 9600, 4800, 2400, 1200, 300
};

//
DataTrans::DataTrans()
{
    memset(rx_buffer,'\0',RX_BUF_SIZE);
    memset(tx_buffer,'\0',TX_BUF_SIZE);
}

//
DataTrans::~DataTrans()
{
}

//-----------------------------------------------
//	print content of uart.cfg
//-----------------------------------------------
void DataTrans::print_serialread(unsigned char which_dev)
{
    UART_DPRINT("serialread[%d].dev is:%s.\n",which_dev,uart_cfg[which_dev].serial_dev);
    UART_DPRINT("uart_cfg[%d].speed is:%d.\n",which_dev,uart_cfg[which_dev].serial_speed);
    UART_DPRINT("uart_cfg[%d].databits is:%d.\n",which_dev,uart_cfg[which_dev].databits);
    UART_DPRINT("uart_cfg[%d].stopbits is:%d.\n",which_dev,uart_cfg[which_dev].stopbits);
    UART_DPRINT("uart_cfg[%d].parity is:%c.\n",which_dev,uart_cfg[which_dev].parity);
}

//-----------------------------------------------
//	read uart2.cfg
//-----------------------------------------------
int DataTrans::read_uart_cfg(unsigned char which_dev)
{
    FILE *fp;
    char tmp[10];

    if(which_dev == 0) {
        fp = fopen("./uart0.cfg","r");
    } else  if(which_dev == 1) {
        fp = fopen("./uart1.cfg","r");
    } else if(which_dev == 2) {
        fp = fopen("./uart2.cfg","r");
    } else if(which_dev == 3) {
        fp = fopen("./uart3.cfg","r");
    } else {
        UART_DPRINT("error:param which_dev is too large!\r\n");
        return -1;
    }

    if(NULL == fp) {
        UART_DPRINT("can't open /etc/uart[%d].cfg\r\n",which_dev);
        return -1;
    } else {
        fscanf(fp, "DEV=%s\n", uart_cfg[which_dev].serial_dev);

        fscanf(fp, "SPEED=%s\n", tmp);
        uart_cfg[which_dev].serial_speed = atoi(tmp);

        fscanf(fp, "DATABITS=%s\n", tmp);
        uart_cfg[which_dev].databits = atoi(tmp);

        fscanf(fp, "STOPBITS=%s\n", tmp);
        uart_cfg[which_dev].stopbits = atoi(tmp);

        fscanf(fp, "PARITY=%s\n", tmp);
        uart_cfg[which_dev].parity = tmp[0];
    }

    fclose(fp);
    UART_DPRINT("uart cfg read success!\r\n");
    return 1;
}

//-----------------------------------------------
//	set boardrate
//-----------------------------------------------
void DataTrans::set_speed(unsigned char which_dev)
{
    unsigned int i;
    int status;
    struct termios Opt;

    if(tcgetattr(serial_fd[which_dev],&Opt) != 0) {
        UART_DPRINT("error: set_speed tcgetattr failed!");
        return ;
    }

    for( i = 0; i < sizeof(speed_arr)/sizeof(int); i++) {
        if(uart_cfg[which_dev].serial_speed == name_arr[i]) {
            tcflush(serial_fd[which_dev], TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(serial_fd[which_dev], TCSANOW, &Opt);
            if(status != 0) {
                UART_DPRINT("error: set_speed tcsetattr failed!");
                return;
            }
            tcflush(serial_fd[which_dev], TCIOFLUSH);
        }
    }
}


//-----------------------------------------------
//	set other parity
//-----------------------------------------------
int DataTrans::set_parity(unsigned char which_dev)
{
    struct termios options;

    if(tcgetattr(serial_fd[which_dev], &options) != 0) {
        UART_DPRINT("error: set_parity tcgetattr failed!");
        return(FALSE);
    }

    options.c_cflag |= (CLOCAL|CREAD);
    options.c_cflag &=~CSIZE;

    //set data bits lenghth
    switch(uart_cfg[which_dev].databits) {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        options.c_cflag |= CS8;
        UART_DPRINT("Unsupported databits:%d\n",uart_cfg[which_dev].databits);
        return(FALSE);
    }

    switch(uart_cfg[which_dev].parity) {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    default:
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        UART_DPRINT("Unsupported parity:%c\n",uart_cfg[which_dev].parity);
        return(FALSE);
    }

    //set stop bits
    switch(uart_cfg[which_dev].stopbits) {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        UART_DPRINT("error: set_parity tcgetattr failed(line 190)!\r\n");
        break;
    default:
        options.c_cflag &= ~CSTOPB;
        UART_DPRINT( "Unsupported stop bits\r\n");
        return(FALSE);
    }

    if(uart_cfg[which_dev].parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 50;	//50,5 seconds
    options.c_cc[VMIN] = 0;

#if 1
    options.c_iflag |= IGNPAR|ICRNL;
    options.c_oflag |= OPOST;
    options.c_iflag &= ~(IXON|IXOFF|IXANY);
#endif
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //ignore \n and \0
    if(which_dev == 1)
    {
    	//options.c_oflag &= ~(INLCR | IGNCR | ICRNL);
    	//options.c_oflag &= ~(ONLCR | OCRNL);
    }
    
    //send directly
    //options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //using hardware control
    //options.c_cflag |= CRTSCTS;

    tcflush(serial_fd[which_dev], TCIFLUSH);
    if(tcsetattr(serial_fd[which_dev], TCSANOW, &options) != 0) {
        UART_DPRINT("error: set_parity tcsetattr failed!(line:210)\r\n");
        return(FALSE);
    }

    return(TRUE);
}

//-----------------------------------------------
//	open device
//-----------------------------------------------
int DataTrans::open_dev(unsigned char *dev)
{
    int fd = open((const char*)dev, O_RDWR);
    //int fd = open((const char*)dev, O_RDWR | O_NONBLOCK);
    //int fd = open((const char*)dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if(-1 == fd) {
        UART_DPRINT("Open Serial Port:%s error!\n",(char*)dev);
        return -1;
    } else
        return fd;
}

//--------------------------------------------------
// init the uart
//--------------------------------------------------

int DataTrans::uart_init(unsigned char which_dev)
{
    unsigned char *dev;

    if(which_dev >= UART_COUNT) {
        UART_DPRINT("error:which_dev is too large!\r\n");
        return -1;
    }

    read_uart_cfg(which_dev);
    /*********************************************************************************/
    this->print_serialread(which_dev);
    UART_DPRINT("uart_init:read port over,which_dev=%d\n",which_dev);
    /*********************************************************************************/

    dev = uart_cfg[which_dev].serial_dev;
    serial_fd[which_dev] = open_dev(dev);

    if(serial_fd[which_dev] > 0)
        set_speed(which_dev);
    else {
        UART_DPRINT("Can't Open Serial Port!\n");
        return -1;
    }

    if (fcntl(serial_fd[which_dev], F_SETFL, O_NONBLOCK) < 0) {
        UART_DPRINT("fcntl failed!\n");
        return -1;
    }

#if 1
    if(isatty(STDIN_FILENO)==0){
        UART_DPRINT("standard input is not a terminal device\n");
   } else{
        UART_DPRINT("isatty success!\n");
	}
#endif

    //if(set_parity(serial_fd[which_dev]) == FALSE)
    if(set_parity(which_dev) == FALSE) {
        UART_DPRINT("Set parity Error\r\n");
        return 1;
    }
    return 1;
}



/**
* @brief read N data from serial port in time out
* @param    fd [in] file description of the serial port
* @param    buffer [in] read data buffer
* @param    size [in] read data buffer size
* @param     readcount [in,out] at last read (readcount) size data.
*                   set it equal to 0, read random count data.
*                   it will return received data count.
*                   return -1, the serial port closed
*                   return N, count of the serial port received
* @param     timeout [in] read N count data's time out
* @return     return 0 : time out\n
*                   return -1 : error\n
*                   return N: received data count\n
*/
int
DataTrans::read_serial_port (int fd, char *buffer, int size,
                             int * readcount, struct timeval *timeout)
{
    assert (size >= *readcount);

    fd_set inputs;
    int result;			//select result
    int nread;			//totla read count
    int iread;			//ont time read count
    int maxfd;

    maxfd = fd + 1;		//NOTE
    nread = 0;

    FD_ZERO (&inputs);
    FD_SET (fd, &inputs);

    do {
        result = select (maxfd, &inputs, (fd_set *) NULL, (fd_set *) NULL,timeout);
        switch (result) {
        case 0:		//time out
        case -1:		//error
            return result;
        default:
            if (FD_ISSET (fd, &inputs)) {
                ioctl (fd, FIONREAD, &iread);
                if (iread == 0) {
                    *readcount = -1;	//port closed
                    return TRUE;
                }
                int count = 0;
                if (*readcount != 0) {
                    //only care buffer size
                    count = min (iread, size - nread);
                }
                nread += read (fd, buffer + nread, count);
            } else {
                assert (FALSE);
            }
        }			//end of switch
    } while ((*readcount != 0) && (nread < *readcount));

    //receive random data, return data count
    if (0 == *readcount) {
        *readcount = nread;
    }

    /*
    if(nread > 2){
    	buffer[nread] = '\0';
    	UART_DPRINT("select get:%s\r\n",buffer);
    }*/

    return TRUE;
}

//--------------------------------------------------
//	uart testing code
//--------------------------------------------------
int DataTrans::uart1_test()
{
    int nread;
    int nwrite;

    if(uart_init(UART1) < 0) {
        return -1;
    }
    strcpy(tx_buffer,"y");
    nwrite = write(serial_fd[UART1],tx_buffer,strlen(tx_buffer));
    UART_DPRINT("nwrite len=%d\r\n",nwrite);
    while(1) {
        if((nread = read(serial_fd[UART1],rx_buffer,512))>0) {
            rx_buffer[nread] = '\0';

            UART_DPRINT("\nrecv len:%d\r\n",nread);
            UART_DPRINT("content:%s",rx_buffer);
            UART_DPRINT("\r\n");
            if(strcmp(rx_buffer+(nread-2),TRANS_END) == 0) {
                break;
            }
        }
    }

  close(serial_fd[UART1]);
  return 1;
}

