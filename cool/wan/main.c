#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/io/config_keys.h>
//#include <cyg/io/spi.h>
//#include <cyg/io/spi_leon3.h>

#include <stdio.h>
#include <stdlib.h>
#include "test.h"

extern void delay(unsigned long);
int main(void)
{
	//test_gpio();
/*
#if defined TRANSMITTER
	printf("Transmitter: ========> \n");
	test_rf_tx();
#elif defined RECEIVER
	printf("Receiver: ========> \n");
	test_rf_rx();
#else 
	printf("Undefined device!\n");
#endif
// */

	//hellospi();
 //test_i2cmst(); 
  //test_i2s();
/*
	//ain_master_enable(1);
	ain_init();
	ain_playback(0, 0);
//	*/
//*
	//ain_master_enable(1);
	ain_init();
	ain_record();
	//ain_playback(0x40100000, (0x40200000-0x40100000)/4);
	//ain_playback(0,0);
// */
  printf("Test finished.\n");
	return 0;
}

