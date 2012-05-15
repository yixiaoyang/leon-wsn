/*
 * uz2400d.h
 *
 *  Created on: 2011-3-6
 *      Author: 
 */

#ifndef UZ2400D_H_
#define UZ2400D_H_

#include "heads.h"
#include "spidev.h"

/* Short address register map */
#define UZS_PANIDL			0x01		/* PAN identifier of this device */
#define UZS_PANIDH			0x02

#define UZS_SADRL			0x03		/* 16-bit short address of this device */
#define UZS_SADRH			0x04

#define UZS_EADR_0			0x05		/* 64-Bit Extended Address of this device */
#define UZS_EADR_1			0x06
#define UZS_EADR_2			0x07
#define UZS_EADR_3			0x08
#define UZS_EADR_4			0x09
#define UZS_EADR_5			0x0A
#define UZS_EADR_6			0x0B
#define UZS_EADR_7			0x0C

#define UZS_RXFLUSH			0x0D
#define UZS_PACON1			0x17		/* Power amplifier control */
#define UZS_FIFOEN			0x18

#define UZS_TXNTRIG			0x1B

#define UZS_TXSR			0x24
#define UZS_GATECLK		0x26
#define UZS_SOFTRST		0x2A
#define UZS_TXPEMISP		0x2E

#define UZS_ISRSTS			0x31
#define UZS_INTMSK			0x32
#define UZS_SLPACK			0x35

#define UZS_RFCTL				0x36

#define UZS_BBREG3			0x3B		/* Baseband register */
#define UZS_BBREG4			0x3C
#define UZS_BBREG5			0x3D
#define UZS_BBREG6 			0x3E

/* Long address register map */
#define UZL_RFCTRL0		0x200
#define UZL_RFCTRL1		0x201
#define UZL_RFCTRL2		0x202
#define UZL_RFCTRL3		0x203
#define UZL_RFCTRL4		0x204
#define UZL_RFCTRL5		0x205
#define UZL_RFCTRL6		0x206
#define UZL_RFCTRL7		0x207
#define UZL_RFCTRL8		0x208

#define UZL_RFSTATE		0x20F
#define UZL_TESTMODE		0x22F

#define UZL_GPIODIR		0x23D
#define UZL_SECCTRL		0x24D


#define UZL_RFCTRL50		0x250
#define UZL_RFCTRL51		0x251
#define UZL_RFCTRL52		0x252
#define UZL_RFCTRL53		0x253
#define UZL_RFCTRL59		0x259
#define UZL_RFCTRL73		0x273
#define UZL_RFCTRL74		0x274
#define UZL_RFCTRL75		0x275
#define UZL_RFCTRL76		0x276

/* Bit definition of interrupt status register */
#define UZS_ISRSTS_TXNIF			BIT_0		/* TX Normal FIFO Transmission Interrupt */
#define UZS_ISRSTS_TXG1IF			BIT_1
#define UZS_ISRSTS_TXG2IF			BIT_2
#define UZS_ISRSTS_RXIF				BIT_3
#define UZS_ISRSTS_SECIF			BIT_4
#define UZS_ISRSTS_MACTMRIF			BIT_5
#define UZS_ISRSTS_WAKEIF			BIT_6
#define UZS_ISRSTS_SLPIF			BIT_7
/* Bit definition of TX status register */
#define UZS_TXSR_TXNS				BIT_0
#define UZS_TXSR_TXG1S				BIT_1
#define UZS_TXSR_TXG2S				BIT_2
#define UZS_TXSR_TXG1FNT			BIT_3
#define UZS_TXSR_TXG2FNT			BIT_4
#define UZS_TXSR_CCAFAIL			BIT_5
#define UZS_TXSR_TXRETRY			BIT_6|BIT7

/******************************************************************************
 * RF register read&write.HW low level driver 
 * 
******************************************************************************/

/*r/w word interface*/
ssize_t uz2400d_sw(struct spidev_data* spidev,u8 saddr, u8 value);
ssize_t uz2400d_sw2(struct spidev_data* spidev,u8 saddr, u8 value);
u8 uz2400d_sr2(struct spidev_data* spidev,u8 saddr);
u8 uz2400d_sr(struct spidev_data* spidev,u8 saddr);
ssize_t uz2400d_lw(struct spidev_data* spidev,u16 laddr, u8 value);
u8 uz2400d_lr(struct spidev_data* spidev,u16 laddr);
ssize_t uz2400d_lr_block(struct spidev_data* spidev,u16 laddr, u8* buf, u16 size);
ssize_t uz2400d_lw_block(struct spidev_data* spidev,u16 laddr, u8* buf, u16 size);

#endif /* UZ2400D_H_ */
