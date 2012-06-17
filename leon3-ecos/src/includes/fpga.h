/*
 * fpga.h
 *
 *  Created on: 2012-5-6
 *      Author: xiaoyang
 */

#ifndef FPGA_H_
#define FPGA_H_

#define SYSFRE			50000000		/* 50M */

/*see detail in asic_core.vhd*/
#define SPI_BASE		0x80000600
#define GPIO_BASE		0x80000800

/*apb uart*/
#define UART0_BASE	0x80000100
/*uart1 0x80000400,apbuart,uart0 0x80000100 AHB uart*/

#define IRQMP_BASE	0x80000200
#define I2CMST_BASE 0x80000700
#define AIN_BASE		0x80000400
#define CK_GEN_BASE 0x80000B00

#define VIN_BASE	0x80000900

#endif /* FPGA_H_ */
