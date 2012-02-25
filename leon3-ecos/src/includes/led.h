/*
 * led.h
 *
 *  Created on: 2011-11-15
 *      Author: xiaoyang
 */

#ifndef LED_H_
#define LED_H_

#include "types.h"

#define LEDG0		3		/*PORTC 0*/
#define LEDG1		3		/*PORTC 1*/

void led_init();
void led_off(int8u_t led_no);
void led_on(int8u_t led_no);

#endif /* LED_H_ */
