#include "includes.h"
#include "ov7660.h"
#include "led.h"

//for FS
#include "sd.h"
#include "FAT32.h"
#include "ff.h"
#include "diskio.h"

/*local parameters*/
static cyg_handle_t threads_voice, threads_isr;
static cyg_thread thread_s[NTHREADS];
static char thread_stacks[NTHREADS][STACKSIZE];

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
 * Description:printf with cyg_mutex_lock() protection
 */
void _dprintf_(const char *format, ...) {
#if _DEBUG_
	va_list arg;
	char tmp[256];
	va_start(arg, format);
	vsprintf(tmp, format, arg);
	va_end(arg);
	/* note: printf() must be protected by a call to cyg_mutex_lock() */
	cyg_mutex_lock(&cliblock);
	{
		printf("%s", tmp);
	}
	cyg_mutex_unlock(&cliblock);
#endif
}

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
	spi_init();

	/*RF init*/
	RF_CHIP_INITIALIZE();

	//netconfig();

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
 * Description:ecos thread for collecting
 */
void thread_collect(cyg_addrword_t data) {
	//gpio_make_out(PORTB, 0xc0);//make io[14-15] as pout
	while (1) {
		/*do your own work here*/
		//led_on(LEDG1);
		cyg_thread_delay(50);
		//led_off(LEDG1);
		cyg_thread_delay(50);
		/*do your own work here*/
	}
}

/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:ecos thread for controlling
 */
void thread_ctrl(cyg_addrword_t data) {
	// uart test
	led_off(LEDG0);
	dprintf("test begin.\n");
	cyg_thread_delay(1000);
	led_on(LEDG0);
	apbuart_test(UART0_BASE);
	//serial test
	dprintf("serial test begin.\n");
	serial_test();

	//OV7660_init();
	//OV7660_work();
	while (1) {
			/*do your own work here*/
			dprintf("uart0 test ok.\n");
			led_on(LEDG0);
			cyg_thread_delay(50);
			led_off(LEDG0);
			cyg_thread_delay(50);
			/*do your own work here*/
	}
	//fs_test();
	/*OV7660_init();
	OV7660_work();
	while(1){
		ov7070_pin_test();
		cyg_thread_delay(400);
	}*/

	//dprintf("test end.\n");
}


/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:entry of program
 */
int main(void) {
	/*configueration*/
	dprintf("\nsystem init.\n");
	system_init();
	/*ecos work*/
	dprintf("\nstart main:[ok]\n");

	/*init and setup working threads*/
	cyg_thread_create(4, thread_collect, (cyg_addrword_t) 0, "isr_voice",
			(void *) thread_stacks[0], STACKSIZE, &threads_isr, &thread_s[0]);

	cyg_thread_create(5, thread_ctrl, (cyg_addrword_t) 1, "thread_voice",
			(void *) thread_stacks[1], 204800, &threads_voice, &thread_s[1]);

	/*setup*/
	cyg_thread_resume(threads_isr);
	cyg_thread_resume(threads_voice);
	cyg_scheduler_start();

	end: dprintf("@ok,main exit(0).\n");

	/*do some clean works*/

	return 0;
}

