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


//*
int main(void)
{
	//test_gpio();
	//test_rf();
  //ain_data_record();
	cyg_uint8 dat[][2] = {
		{(0x05<<1&0x7E|0x01), 0xAA},
		{(0x05<<1&0x7E), 0x0}
	};
	cyg_uint32 len;

	cyg_io_handle_t spi_hdl;
	int err;

	printf("Start SPI driver test\n");
	err = cyg_io_lookup("/dev/spi0", &spi_hdl);
	if(err) {
		diag_printf("ERROR opening device /dev/spi0.\n");
		return;
	}else {
		printf("Device /dev/spi0 opened.\n");
	}
	len = sizeof(cyg_uint8);
  cyg_uint8 saddr = 0x05;
	/* Config short address */
	cyg_io_set_config(spi_hdl, CYG_IO_SET_CONFIG_SPI_UZ2400D_SADDR, \
						&saddr, &len);
	/* Config polled */
	cyg_bool polled = true;
	len = sizeof(cyg_bool);
	cyg_io_set_config(spi_hdl, CYG_IO_SET_CONFIG_SPI_UZ2400D_POLLED, \
						&polled, &len);
	/* Write */
	cyg_uint8 data = 0xAA, buf=0;
  len = sizeof(cyg_uint8);						
	err = cyg_io_write(spi_hdl, &data, &len);
	if(err) {
		diag_printf("ERROR writing device /dev/spi0.\n");
		return;
	}
	/* Read back */
	err = cyg_io_read(spi_hdl, &buf, &len);
	if(err) {
		diag_printf("ERROR writing device /dev/spi0.\n");
		return;
	} else {
		printf("Read: %x\n", buf);
	}

	printf("SPI write test end\n");	
	return 0;
}
//*/

