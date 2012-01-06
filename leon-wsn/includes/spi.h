/*
 * spi.h
 *
 *  Created on: 2011-2-22
 *      Author: hgdytz
 */

#ifndef SPI_H_
#define SPI_H_
#include "fpga.h"
#include "types.h"
/* SPICTL Registers APB Address Offset */
#define SPI_CAP			0x0
#define SPI_MOD			0x20
#define SPI_EVE			0x24
#define SPI_MSK			0x28
#define SPI_CMD			0x2C
#define SPI_TX			0x30
#define SPI_RX			0x34
#define SPI_SLV			0x38
/* Bit Definition of SPICTL Capability Register  */
#define SPI_CAP_REV		0x7F
#define SPI_CAP_FDEPTH		0xFF00
#define SPI_CAP_SSEN		0x10000
#define SPI_CAP_ASELA		0x20000
#define SPI_CAP_AMODE		0x40000
#define SPI_CAP_TWEN		0x80000
#define SPI_CAP_MAXWLEN	0xF00000
#define SPI_CAP_SSSZ		0xFF000000
/* Bit Definition of SPICTL Mode Register */
#define SPI_MOD_TAC		0x10
#define SPI_MOD_ASELDEL	0x60
#define SPI_MOD_CG		0x780
#define SPI_MOD_OD		0x1000
#define SPI_MOD_FACT		0x2000
#define SPI_MOD_ASEL		0x4000
#define SPI_MOD_TW		0x8000
#define SPI_MOD_PM		0xF0000		/* Prescale Module*/
#define SPI_MOD_LEN		0xF00000
#define SPI_MOD_EN		0x1000000
#define SPI_MOD_MS		0x2000000		/* Master/Slave */
#define SPI_MOD_REV		0x4000000
#define SPI_MOD_DIV16		0x8000000
#define SPI_MOD_CPHA		0x10000000
#define SPI_MOD_CPOL		0x20000000
#define SPI_MOD_LOOP		0x40000000
#define SPI_MOD_AMEN		0x80000000
/* Bit Definition of SPICTL Event Register */
#define SPI_EVE_NF		0x100
#define SPI_EVE_NE		0x200
#define SPI_EVE_MME		0x400
#define SPI_EVE_UR		0x800
#define SPI_EVE_OV		0x1000
#define SPI_EVE_LT		0x4000
#define SPI_EVE_TIP		0x80000000			/* Transfer in Progress */
/* Bit Definition of SPICTL Mask Register */
#define SPI_MSK_NFE		0x100
#define SPI_MSK_NEE		0x200
#define SPI_MSK_MMEE		0x400
#define SPI_MSK_UNE		0x800
#define SPI_MSK_OVE		0x1000
#define SPI_MSK_LTE		0x4000
#define SPI_MSK_TIPE		0x80000000
/* Bit Definition of SPICTL Command Register */
#define SPI_CMD_LST		0x400000
/* Bit Definition of SPICTL Slave Select Register */
#define SPI_SLV_SSSZ		1
#define SPI_SLV_SLVSEL		(1<<SPI_SLV_SSSZ)-1

/* Some useful routines */
#define SLV_SELECT()		REG32(SPI_BASE+SPI_SLV) &= ~SPI_SLV_SLVSEL
#define SLV_DESELECT()		REG32(SPI_BASE+SPI_SLV) |= SPI_SLV_SLVSEL

/*	spi2 for vs1003*/
/*
#define GPIO_7		//SPI_MOSI
#define GPIO_8		//SPI_MISO
#define GPIO_9		//SPI_CLK
#define	GPIO_10		//XCS
#define	GPIO_11		//XDCS
#define GPIO_12		//DREQ
*/
		
//#define SPI2_CS   (1 << 0)//RB0 as CS
#define SPI2_MOSI   		( 1 << 3) 		//IO[11],PORTB[3]
#define SPI2_MISO   		( 1 << 4)		//IO[12],PORTB[4]
#define SPI2_CLK    		( 1 << 5) 		//IO[13],PORTB[5]

#define SPI2_HIGH		0
#define SPI2_LOW		64

#define SETMOSI()		gpio_set(PORTB,SPI2_MOSI)
#define CLRMOSI()		gpio_clr(PORTB,SPI2_MOSI)
#define SETCLK()		gpio_set(PORTB,SPI2_CLK)
#define CLRCLK()		gpio_clr(PORTB,SPI2_CLK)

#define READMISO()		gpio_read(PORTB,SPI2_MISO)

//#define SPI2_SELECT()    	(PORTBCLR |= SPI2_CS)	// tris control for CS pin
//#define SPI2_DESELECT()		(PORTBSET |= SPI2_CS)	// deselect spi

/*	interface defination*/
void spi_init(void);
int8u_t spi_xmit8(int8u_t val);

/* spi2 for vs1003x, added by xiaoyang*/
void spi2_delay(int16u_t ch);
void spi2_init(void);
void spi2_high_speed_mode(void);
int8u_t spi2_8TxRx(int8u_t byte);

#endif /* SPI_H_ */
