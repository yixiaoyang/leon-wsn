/*
 * fpga.h
 *
 *  Created on: 2011-2-22
 *      Author: hgdytz
 */

#ifndef FPGA_H_
#define FPGA_H_

#define SYSFRE			50000000		/* 50M */

/*see detail in asic_core.vhd*/
#define SPI_BASE		0x80000600
#define GPIO_BASE		0x80000800

/*apb uart*/
#define UART0_BASE	0x80000100

//uart1 0x80000400,apbuart,uart0 0x80000100 AHB uart

#endif /* FPGA_H_ */
