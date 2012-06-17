#include "systask.h"

/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Return:
 * Description:ecos thread for controlling
 */
void systsk_handle(cyg_addrword_t data){
	dprintf("systask in...\n");
	//OV7660_init();
	vin_init();
	cyg_thread_delay(10);
	while(VIN_STAT_EMPTY){
		dprintf("vin empty,stat=[%08x],cfg=[%08x]\n",REG32(VIN_BASE+VIN_STAT),REG32(VIN_BASE+VIN_CFG));
		show_pix_data();
		cyg_thread_delay(500);
	}
	
	while(!VIN_STAT_FULL){
		dprintf("vin not full,stat=[%08x]\n",REG32(VIN_BASE+VIN_STAT));
		cyg_thread_delay(500);
	}
	//OV7660_work();
	while (1) {
		/*do your own work here*/
		dprintf("sys task...\n");
		cyg_thread_delay(500);
		/*do your own work here*/
	}
}
