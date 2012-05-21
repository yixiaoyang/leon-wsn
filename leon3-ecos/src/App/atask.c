#include "atask.h"

/*
 * thread handle entry for audio collection
 *
 *
 */
void thread_misc(cyg_addrword_t data) {
	while(1){
		led_off(LEDG0);
		cyg_thread_delay(400);
		led_on(LEDG0);
		cyg_thread_delay(400);
	}

	dprintf("test end.\n");
}

