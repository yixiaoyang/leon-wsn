/*
 * uart.h
 *
 *  Created on: 2011-10-23
 *      Author: xiaoyang
 */

#ifndef UART_H_
#define UART_H_

#include "types.h"
#include "fpga.h"

#define DISABLE 0x0
#define ENABLE_RX 0x1
#define ENABLE_TX 0x2
#define RX_INT 0x4
#define TX_INT 0x8
#define EVEN_PARITY 0x20
#define ODD_PARITY 0x30
#define LOOP_BACK 0x80
#define FLOW_CONTROL 0x40
#define FIFO_TX_INT 0x200
#define FIFO_RX_INT 0x400

#define STAT_DR		0x0
#define STAT_TS		0x2
#define STAT_TE		0x4

#define APB_UART_DATA		0x00
#define APB_UART_STAT		0x04
#define APB_UART_CON		0x08
#define APB_UART_SCALER	0x0c

int apbuart_test(unsigned int addr);/*in apbuart.c*/
void serial_test();/*in serial.c*/

#endif /* UART_H_ */
