/*
 * gpio.c
 *
 *  Created on: 2011-11-10
 *  xiaoyang for OPDK2,using GPIO aux
 */

#include "types.h"
#include "fpga.h"
#include "gpio.h"

/*
 * 0 as gpio
 * 1 as other aux pins
 *
 * example:
 *  	gpio_init(PORTA,0x00); //to set io[0-7] as gpio
 */
inline void gpio_init(int8u_t pbase, int16u_t pmask) {
	REG32(GPIO_BASE+pbase+0x18) = pmask;
}

inline void gpio_init_gpio(int8u_t pbase, int8u_t pmask) {
	REG32(GPIO_BASE+pbase+0x18) &= (~pmask);
	REG32(GPIO_BASE+pbase+0x18) &= (8 << (~pmask));
}

/*(set out:0,set in:1)*/
inline void gpio_make_out(int8u_t pbase, int8u_t pmask) {
	//port dir
	REG32(GPIO_BASE+pbase+0x08) &= (~pmask);
}

inline void gpio_make_in(int8u_t pbase, int8u_t pmask) {
	//port dir
	REG32(GPIO_BASE+pbase+0x08) |= pmask;
}

inline void gpio_set(int8u_t pbase, int8u_t pmask) {
	//port out
	REG32(GPIO_BASE+pbase+0x04) |= pmask;
}

inline void gpio_clr(int8u_t pbase, int8u_t pmask) {
	//port out
	REG32(GPIO_BASE+pbase+0x04) &= (~pmask);
}

inline void gpio_toggle(int8u_t pbase, int8u_t pmask) {
	//port out
	REG32(GPIO_BASE+pbase+0x04) ^= pmask;
}

inline int16u_t gpio_read(int8u_t pbase, int8u_t pmask) {
	//port in
	return ((REG32(GPIO_BASE+pbase) & pmask) ? 1 : 0);
}

inline int8u_t gpio_read_group(int8u_t pbase)
{
	return (int8u_t)(REG32(GPIO_BASE+pbase));
}

/*xiaoyang add @2012-5-15 for int test*/
inline void gpio_int_msk(int8u_t pbase,int8u_t pmask)
{
	/*interrupt mask(0:masked,1:enabled)*/
	REG32(GPIO_BASE+pbase+0x0C) &= (~pmask);
}

inline void gpio_int_en(int8u_t pbase,int8u_t pmask)
{	
	/*interrupt mask(0:masked,1:enabled)*/
	REG32(GPIO_BASE+pbase+0x0C) |= pmask;
}

inline void gpio_pol(int8u_t pbase,int8u_t pmask)
{
	/*interrupt polarity(0:low/failing,1:high/rising)*/
	REG32(GPIO_BASE+pbase+0x10) = pmask;
}

inline void gpio_set_lev(int8u_t pbase,int8u_t pmask)
{
	/*interrupt edge(0:level,1:edge)*/
	REG32(GPIO_BASE+pbase+0x14) &= (~pmask);
}
inline void gpio_set_edg(int8u_t pbase,int8u_t pmask)
{
	/*interrupt edge(0:level,1:edge)*/
	REG32(GPIO_BASE+pbase+0x14) = pmask;
}

/*reading test*/
void gpio_test() {
	//gpio_make_in(PORTA, 0xff);//make io[0-7] as pin
	printf("read PORTA,PCLK,VSYNC,HREF:%4x,%4x,%4x,%4x\n",
			gpio_read_group(PORTA),
			gpio_read(PORTB,1<<1),
			gpio_read(PORTB,1<<6),
			gpio_read(PORTB,1<<7));
}

/*end of file*/

