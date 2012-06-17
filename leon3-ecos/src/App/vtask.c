#include "vtask.h"

void vtsk_func(cyg_addrword_t data) 
{
	dprintf("vstask func in!!!!\n");
	while(1){
		dprintf("vtask loop...\n");
		cyg_thread_delay(200);
	}
}

/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:ecos thread for collecting
 */
void vtsk_handle(cyg_addrword_t data) {
/*
	if(VIN_STAT_EMPTY){
		dprintf("vin empty!\n");
	}else{
		dprintf("vin not empty!\n");
	}
	dprintf("vin empty,stat=[%08x]\n",REG32(VIN_BASE+VIN_STAT));
	//OV7660_init();
	vin_init();
	vin_de_set();
	while(VIN_STAT_EMPTY){
		dprintf("vin empty,stat=[%08x]\n",REG32(VIN_BASE+VIN_STAT));
		cyg_thread_delay(500);
	}
	
	while(!VIN_STAT_FULL){
		dprintf("vin not full,stat=[%08x]\n",REG32(VIN_BASE+VIN_STAT));
		cyg_thread_delay(500);
	}
	//OV7660_work();*/
	dprintf("vstask in!!!!\n");
	
	//led
	gpio_init_gpio(PORTF,0x8);
	gpio_make_out(PORTF,0x8);
	while(1){
		gpio_set(PORTF,0x8);
		cyg_thread_delay(100);
		gpio_clr(PORTF,0x8);
		cyg_thread_delay(100);
	}
	
	dprintf("test end.\n");
}
