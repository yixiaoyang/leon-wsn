/*
 * uart.c
 *
 *  Created on: 2011-10-23
 *      Author: xiaoyang
 */

#include "uart.h"

static char test[] =
		"40ti94a+0ygiyu05yhap5yi4h+a+iiyxhi4k59j0q905jkoyphoptjrhia4iy0+4";

static int testsize = sizeof test / sizeof test[0];

static inline int loadmem(int addr) {
	int tmp;
	asm volatile (" lda [%1]1, %0 "
			: "=r"(tmp)
			: "r"(addr)
	);
	return tmp;
}

/*
 * uart error handle
 */
void inline uart_fail(int fail_code) {
	printf("error:uart err_code=%d\n", fail_code);
	return;
}

void uart0_init(int addr) {
	int i = 0;
	/*disable uart0*/
	struct uart_regs *uart = (struct uart_regs *) addr;
	int i = 0;
	int temp = 0;

	// scaler = (((system_clk*10)/(baudrate*8))-5)/10
	// 		  = ( (50M*10)/9600*8-5 )/10 = 650.54

	/* set scaler to low value to speed up simulations */
	uart->scaler = 651;
	uart->status = 0;
	uart->data = 0;
	uart->control = ENABLE_TX | ENABLE_RX;
	//uart->control = ENABLE_TX | ENABLE_RX | LOOP_BACK;

}

int apbuart_test(int addr) {
	printf("debug: begin...\n");
	uart0_init(addr);
	//determine fifosize
	for (i = 0; i < 32; i++) {
			uart->data = test[i];
		}
	printf("debug:end...\n");

	return 0;
}
