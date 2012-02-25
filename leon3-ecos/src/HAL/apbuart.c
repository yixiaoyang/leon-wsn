#include "uart.h"
#include "led.h"

void inline fail(int fail_code) {
	printf("error:uart err_code=%d\n", fail_code);
	return;
}

/*
 * uart[0] = data
 * uart[1] = status
 * uart[2] = control
 * uart[3] = scaler
 */

static inline int loadmem(int addr)
{
  int tmp;
  asm volatile (" lda [%1]1, %0 "
      : "=r"(tmp)
      : "r"(addr)
    );
  return tmp;
}

static char test[] = "abcdefghijklmnopqrstuvwxyz";

int apbuart_test(unsigned int base)
{
        //struct uart_regs *uart = (struct uart_regs *) addr;
        unsigned int temp;
        unsigned int i;
        unsigned int fifosize;

        printf("report_device:%8x\n",base);
        /* set scaler to low value to speed up simulations */
        /*scaler = (((system_clk*10)/(baudrate*8))-5)/10
                	 		  = ( (50M*10)/9600*8-5 )/10 = 650.54*/
        REG32(base+APB_UART_SCALER) = 651;
        REG32(base+APB_UART_STAT) = 0;
        REG32(base+APB_UART_DATA)= 0;

        printf("2:<0>,con addr =%4x\n",base+APB_UART_CON);
        printf("2:<1>,con value=%4x\n",REG32(base+APB_UART_CON));
        REG32(base+APB_UART_CON) |= (ENABLE_TX | ENABLE_RX);
        printf("2:<1>,con value=%4x\n",REG32(base+APB_UART_CON));

        for(i = 0; i < 20; i++) {
        	//wait for the TS
        	while( (REG32(base+APB_UART_STAT) & STAT_TS) == 0 )
        		;
        	//while( (REG32(base+APB_UART_STAT) & STAT_TE) == 0 )
        	  //      		;
        	REG32(base+APB_UART_DATA) = (unsigned int) test[i];
        }

        printf("\napb_uart test over!\n");
        return 0;
}

