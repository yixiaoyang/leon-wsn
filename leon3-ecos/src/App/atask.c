#include "atask.h"


void thread_misc(cyg_addrword_t data) {
	// uart test
	led_off(LEDG0);

	while(1){
		cyg_thread_delay(400);
	}

	dprintf("test end.\n");
}

