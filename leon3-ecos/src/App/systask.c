#include "systask.h"

/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:ecos thread for controlling
 */
void systsk_handle(cyg_addrword_t data){
	//gpio_make_out(PORTB, 0xc0);//make io[14-15] as pout
	while (1) {
		/*do your own work here*/
		led_on(LEDG1);
		cyg_thread_delay(50);
		led_off(LEDG1);
		cyg_thread_delay(50);
		/*do your own work here*/
	}
}
