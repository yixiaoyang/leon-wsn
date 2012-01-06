/*
 * led.c
 *
 *  Created on: 2011-11-15
 *      Author: xiaoyang
 */

#include "led.h"
#include "gpio.h"

void led_init() {
	gpio_make_out(PORTF, 0x0C);
	//gpio_make_out(PORTC, 0x03);//make io[14-15] as pout
}

void led_on(int8u_t led_no) {
	/*
	if ((led_no != LEDG0) && (led_no != LEDG1))
		return;
	gpio_set(PORTC, 1 << led_no);
*/
	gpio_set(PORTF, 1 << led_no);
}

void led_off(int8u_t led_no) {
	/*if ((led_no != LEDG0) && (led_no != LEDG1))
		return;
	gpio_clr(PORTC, 1 << led_no);
*/
	gpio_clr(PORTF, 1 << led_no);
}
