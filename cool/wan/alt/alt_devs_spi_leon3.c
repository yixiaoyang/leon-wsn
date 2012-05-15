#include <cyg/kernel/kapi.h>
//#include <cyg/io/io.h>
#include <cyg/io/spi.h>
//#include <cyg/io/spi_leon3.h>

#include <stdio.h>
#include <stdlib.h>
#include "test.h"


//*
int main(void)
{
	//test_gpio();
	//test_rf();
  //ain_data_record();
	printf("Start SPI driver test\n");
	cyg_uint8 data_write[2][2] = {
		{((0x05<<1&0x7E)|0x01),0x55},
		{(0x05<<1&0x7E), 0x00}
	};
	
	//extern cyg_spi_leon3_dev_t cyg_spi_bus_0_devs[], cyg_spi_bus_0_devs_end;
	extern cyg_spi_device cyg_spi_bus_0_devs[], cyg_spi_bus_0_devs_end;
	/*
	cyg_spi_leon3_dev_t *pspi;
	int index=0;
	for(pspi=cyg_spi_bus_0_devs;pspi!=&cyg_spi_bus_0_devs_end;pspi++, index++) {
		printf("index: %d, name: %s\n", index, pspi->name);
		printf("index: %d\n", index);
	}
	// */
	//cyg_spi_set_config(&cyg_spi_bus_0_devs[0].spi_device, 0, (void*)0,0);
	cyg_uint8 recv[16]= {0};
	cyg_spi_device *spi_uz2400d = &cyg_spi_bus_0_devs[0];
	cyg_spi_set_config(spi_uz2400d, 0, (void*)0,0);

	cyg_spi_transaction_begin(spi_uz2400d);
	//*
	cyg_spi_transaction_transfer(spi_uz2400d, false, \
						sizeof(data_write[0]), data_write[0], NULL, true);
	cyg_spi_transaction_end(spi_uz2400d);

	// */
	cyg_spi_transaction_begin(spi_uz2400d);
	cyg_spi_transaction_transfer(spi_uz2400d, false, \
						sizeof(data_write[1]), data_write[1], recv, false);

	cyg_spi_transaction_end(spi_uz2400d);
	printf("Received %x\n", recv[1]);

	printf("SPI write test end\n");	
	return 0;
}
//*/

