#ifndef CYGONCE_DEVS_SPI_SPARC_LEON3_H
#define CYGONCE_DEVS_SPI_SPARC_LEON3_H

//===================================================================
// SPI driver for leon3
//===================================================================
#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>

#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/spi.h>

struct spi_leon3_regs {
	volatile cyg_uint32 spcap;
	volatile cyg_uint32 reserved[7];
	volatile cyg_uint32 spmod;
	volatile cyg_uint32 speve;
	volatile cyg_uint32 spmsk;
	volatile cyg_uint32 spcmd;
	volatile cyg_uint32 sptx;
	volatile cyg_uint32 sprx;
	volatile cyg_uint32 spslv;
};

//===================================================================
// struct: cyg_spi_leon3_dev_t
// leon3 spi device structure
//===================================================================
typedef struct {
	cyg_spi_device spi_device;  /* defined in io/spi.h */

  cyg_uint32 spi_baud;
//	char* descrip;
  void *dev_priv;

	void (*spi_cs)(cyg_bool);
} CYG_HAL_TABLE_TYPE cyg_spi_leon3_dev_t;

//===================================================================
// struct: cyg_spi_leon3_bus_t
// spi bus entity
//===================================================================
typedef struct {
	cyg_spi_bus spi_bus;       /* defined in io/spi.h */
  /* interrupt and currency control */
	cyg_interrupt spi_intr;
	cyg_handle_t spi_hand;
	cyg_vector_t spi_vect;
	cyg_priority_t spi_prio;
	cyg_drv_mutex_t spi_lock;
	cyg_drv_cond_t spi_wait;
	/* Data related definitions */
	struct spi_leon3_regs *spi_regs;

	volatile cyg_uint32 count;
	volatile const cyg_uint8 *tx;
	volatile cyg_uint8 *rx;
} cyg_spi_leon3_bus_t;
externC cyg_spi_leon3_bus_t cyg_spi_leon3_bus0;
//CYG_SPI_DEFINE_BUS_TABLE(cyg_spi_leon3_dev_t, 0);

#endif // CYGONCE_DEVS_SPI_SPARC_LEON3_H
