#ifndef HEADS_H_
#define HEADS_H_

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/spi/spi.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/types.h>

#define BIT_0		0x01
#define BIT_1 	0x02
#define BIT_2		0x04
#define BIT_3		0x08
#define BIT_4		0x10
#define BIT_5		0x20
#define BIT_6		0x40
#define BIT_7		0x80
#define BIT_8		0x100
#define BIT_9		0x200
#define BIT_10	0x400

#define PANIDL		0xAA
#define PANIDH		0xAA

#define DEV_TYPE_TX	0

#if DEV_TYPE_TX
#define EADR_0		0x11
#define EADR_1		0x12
#define EADR_2		0x13
#define EADR_3		0x14
#define EADR_4		0x15
#define EADR_5		0x16
#define EADR_6		0x17
#define EADR_7		0x18
#define SADRL		0x11
#define SADRH		0x11

#else
#define EADR_0		0x21
#define EADR_1		0x22
#define EADR_2		0x23
#define EADR_3		0x24
#define EADR_4		0x25
#define EADR_5		0x26
#define EADR_6		0x27
#define EADR_7		0x28
#define SADRL		0x22
#define SADRH		0x22
#endif // DEV_TYPE_TX

#define USER_BUFF_SIZE	4096
#define SPI_BUS_SPEED 	5000000
#define RF_DEV_ID			0x0
#define RF_DBG				1

/*spi chip select*/
#define SPI_BUS_CS0			0

/*using s3c2440 spi0*/
#define SPI_BUS			0

#define V_STR "2012-3-27 16:48\n"


#define SLV_SELECT() udelay(1)
#define SLV_DESELECT() udelay(1)
#define rf_delay udelay

#endif
