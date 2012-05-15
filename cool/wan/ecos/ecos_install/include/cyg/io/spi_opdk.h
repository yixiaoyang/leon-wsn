#ifndef CYGONCE_DEVS_SPI_SPARC_OPDK_H
#define CYGONCE_DEVS_SPI_SPARC_OPDK_H

#include <cyg/infra/cyg_type.h>
/*
#define HAL_SPI_EXPORTED_DEVICES \
	externC cyg_spi_device *cyg_spi_uz2400d;
// */

//
// UZ2400D specific information structure 
//
typedef struct {
  cyg_bool polled; 
#define TRANS_SHORT  0
#define TRANS_LONG	 1
#define TRANS_BURST  2
	cyg_uint8 trans_type;
	cyg_uint8 saddr;
	cyg_uint16 laddr;
	
} uz2400d_info_t;

#endif 
