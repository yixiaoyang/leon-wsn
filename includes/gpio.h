/*
 * gpio.h
 *
 *  Created on: 2011-2-22
 *      Author: water
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "types.h"
/* GRGPIO Registers APB Address Offset */
/*
 #define GPIO_DAT		0x00
 #define GPIO_OUT		0x04
 #define GPIO_DIR		0x08
 #define GPIO_MSK		0x0C
 #define GPIO_POL		0x10
 #define GPIO_EDG		0x14
 #define GPIO_BY			0x18
 #define GPIO_MAP		0x20
 */
//-------------------------------------------------------------
// gpio driver for OPDK2
// xiaoyang yi @2011.11.9
//-------------------------------------------------------------

// PORTA( GPIO[0-7] )
#define 	PORTA			0x00
#define 	DIN_A				0x00
#define 	DOUR_A				0x04
#define 	DIR_A				0x08
#define 	IMASK_A			0x0C
#define 	ILEVEL_A			0x10
#define 	IEDGE_A			0x14
#define 	AUX_A				0x18
#define 	ISTAT_A				0x1C
// PORTB( GPIO[8-15] )
#define 	PORTB			0x20
#define 	DIN_B				0x20
#define 	DOUR_B				0x24
#define 	DIR_B				0x28
#define 	IMASK_B			0x2C
#define 	ILEVEL_B			0x30
#define 	IEDGE_B			0x34
#define 	AUX_B				0x38
#define 	ISTAT_B			0x3C
// PORTC( GPIO[16-23] )
#define 	PORTC			0x40
#define 	DIN_C				0x40
#define 	DOUR_C				0x44
#define 	DIR_C				0x48
#define 	IMASK_C			0x4C
#define 	ILEVEL_C			0x50
#define 	IEDGE_C			0x54
#define 	AUX_C				0x58
#define 	ISTAT_C			0x5C
// PORTD( GPIO[24-31] )
#define 	PORTD			0x60
#define 	DIN_D				0x60
#define 	DOUR_D				0x64
#define 	DIR_D				0x68
#define 	IMASK_D			0x6C
#define 	ILEVEL_D			0x70
#define 	IEDGE_D			0x74
#define 	AUX_D				0x78
#define 	ISTAT_D			0x7C
// PORTE( GPIO[32-39] )
#define 	PORTE			0x80
#define 	DIN_E				0x80
#define 	DOUR_E				0x84
#define 	DIR_E				0x88
#define 	IMASK_E			0x8C
#define 	ILEVEL_E			0x90
#define 	IEDGE_E			0x94
#define 	AUX_E				0x98
#define 	ISTAT_E			0x9C
// PORTF( GPIO[40-43] )
#define 	PORTF			0xA0
#define 	DIN_F				0xA0
#define 	DOUR_F				0xA4
#define 	DIR_F				0xA8
#define 	IMASK_F			0xAC
#define 	ILEVEL_F			0xB0
#define 	IEDGE_F			0xB4
#define 	AUX_F				0xB8
#define 	ISTAT_F			0xBC

/* Exported Functions */
/*
 void gpio_make_out(int32u_t val);
 void gpio_set(int32u_t val);
 void gpio_clr(int32u_t val);
 void gpio_toggle(int32u_t val);
 int16u_t gpio_read(int32u_t val);
 void gpio_make_in(int32u_t val);
 */

void gpio_init(int8u_t pbase, int16u_t pmask);
void gpio_init_gpio(int8u_t pbase, int8u_t pmask);
void gpio_make_out(int8u_t pbase, int8u_t val);
void gpio_set(int8u_t pbase, int8u_t val);
void gpio_clr(int8u_t pbase, int8u_t val);
void gpio_toggle(int8u_t pbase, int8u_t val);
int16u_t gpio_read(int8u_t pbase, int8u_t val);
void gpio_make_in(int8u_t pbase, int8u_t val);

//
void gpio_test();

#endif /* GPIO_H_ */
