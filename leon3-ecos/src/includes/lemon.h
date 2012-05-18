/*
 * lemon.h
 *
 *  Created on: 2011-2-22
 *      Author: hgdytz
 */

#ifndef LEMON_H_
#define LEMON_H_

#include "types.h"

/**/
#define DEV_TYPE_TX	0

#define NTHREADS		2
#define STACKSIZE		20480

extern int8u_t txbuf[];			/* In monitor.h */
extern int8u_t rxbuf[];

#define PANIDL		0xAA
#define PANIDH		0xAA

/* Zigbee NIC 64-bit extended address */

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

/*for app*/
#define DEV_MISC	0
#define DEV_VIDEO	1
#define DEV_ROUTER	0

#endif /* LEMON_H_ */
