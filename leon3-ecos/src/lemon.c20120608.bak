#include "includes.h"
#include "led.h"

#include "systask.h"
#include "vtask.h"

/*task stack and handle*/
/*systask*/
#define SYSTSK_STK_SIZE	51200
static cyg_handle_t systsk_hdl;
static cyg_thread systsk_thr;
static char systsk_stack[SYSTSK_STK_SIZE];

/*video task*/
static cyg_handle_t vtsk_hdl;
static cyg_thread vtsk_thr;
static char vtsk_stack[204800];

/*global parameters*/
cyg_mutex_t cliblock;
int8u_t txbuf[128] = { 0 };
int8u_t rxbuf[144] = { 0 };

/* for ov7670,RGB565 out, 240*320*2 */
int8u_t pix_buf[OV7670_COL_SIZE*OV7670_ROW_SIZE*2];

/*extern parameters*/
extern const short music_data[FILELEN];


/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:SW & HW init
 */
void system_init(void) {
	dprintf("system init begin.\n");
	/*gpio*/
	gpio_init_gpio(PORTA, 0xff);//set io[0-7] as gpio pins
	gpio_init_gpio(PORTB, 0xff);//set io[8-15] as gpio pins
	//gpio_init_gpio(PORTC, 0x03);//set io[16-17] as gpio(LEDs)
	//gpio_init(PORTC,0x3E00);
	//gpio_init_gpio(PORTE, 0x03);
	gpio_init_gpio(PORTF, 0x0C);

	/*led*/
	led_init();
	led_off(LEDG0);
	led_off(LEDG1);

	/*spi init for RF*/
	printf("spi init begin ..\n");
	spi_init();
	printf("spi init end...\n");

	/*RF init*/
	RF_CHIP_INITIALIZE();
	printf("RF_CHIP_INITIALIZE over..\n");
	netconfig();
	printf("netconfig over..\n");

	/*big or little endien check*/
	testing();
	if(check_cpu_endien() == BIG_ENDIEND){
		dprintf("CPU endiend:big.\n");
	}else{
		dprintf("CPU endiend:little.\n");
	}
	dprintf("system init end.\n");
}


/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:entry of program
 */
int main(void) {
	unsigned int i = 0;
	/*configueration*/
	system_init();

	/*ecos work*/
	dprintf("\neCos start:[ok]\n");

	/*init and setup working threads*/
	cyg_thread_create(
		4,/*priority*/ 
		vtsk_handle,/*entry*/ 
		(cyg_addrword_t) 0,/*entry data*/
		"video_thread",/*thread name*/
		(void *) vtsk_stack,/*stack base*/ 
		204800,/*stack size*/
		&vtsk_hdl, /*thread handle*/
		&vtsk_thr);/*thread entity address*/

	cyg_thread_create(
		3, 
		systsk_handle,
		(cyg_addrword_t) 1, 
		"sysctrl_thread",
		(void *) systsk_stack, 
		SYSTSK_STK_SIZE, 
		&systsk_hdl, 
		&systsk_thr);
		
	/*setup*/
	cyg_thread_resume(vtsk_hdl);
	cyg_thread_resume(systsk_hdl);
	cyg_scheduler_start();

	dprintf("@ok,main exit(0).\n");

	/*do some clean works*/

	return 0;
}

