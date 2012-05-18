#include "vtask.h"

/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:ecos thread for collecting
 */
void vtsk_handle(cyg_addrword_t data) {
	// uart test
	led_off(LEDG0);
	//dprintf("test begin.\n");
	//cyg_thread_delay(1000);
	//led_on(LEDG0);
	//apbuart_test(UART0_BASE);
	//serial test
	//dprintf("serial test begin.\n");
	//serial_test();
	
	//OV7660_init();
	//OV7660_work();
	//while (1) {
			/*do your own work here*/
			//dprintf("uart0 test ok.\n");
			//led_on(LEDG0);
			//cyg_thread_delay(50);
			//led_off(LEDG0);
			//cyg_thread_delay(50);
			/*do your own work here*/
	//}
	//fs_test();
	OV7660_init();
	OV7660_work();
	while(1){
		ov7660_pin_test();
		cyg_thread_delay(400);
	}

	dprintf("test end.\n");
}
