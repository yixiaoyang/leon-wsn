//=============================================================================
//
//      spi_leon3.c
//
//      SPI driver implementation for leon3
//
//=============================================================================
// Author(s):   Chris Holgate
// Date:        2008-11-27
// Purpose:     leon3 SPI driver implementation
//=============================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/io/spi.h>
#include <string.h>

#include "spi_leon3.h"

//-----------------------------------------------------------------------------
static cyg_drv_mutex_t cyg_spi_lock0;
//-----------------------------------------------------------------------------
/* API function call forward references.*/
static void leon3_transaction_begin(cyg_spi_device*);
static void leon3_transaction_transfer(cyg_spi_device*, cyg_bool, cyg_uint32,
		const cyg_uint8*, cyg_uint8*, cyg_bool);
static void leon3_transaction_tick(cyg_spi_device*, cyg_bool, cyg_uint32);
static void leon3_transaction_end(cyg_spi_device*);
static int leon3_get_config(cyg_spi_device*, cyg_uint32, void*, cyg_uint32*);
static int leon3_set_config(cyg_spi_device*, cyg_uint32, const void*,
		cyg_uint32*);

//-----------------------------------------------------------------------------

/* Instantiate the bus state data structures.*/
const cyg_spi_leon3_bus_t cyg_spi_leon3_bus0 = { 
		.spi_bus.spi_lock = cyg_spi_lock0,
		.spi_bus.spi_transaction_begin = leon3_transaction_begin,
		.spi_bus.spi_transaction_transfer = leon3_transaction_transfer,
		.spi_bus.spi_transaction_tick = leon3_transaction_tick,
		.spi_bus.spi_transaction_end = leon3_transaction_end,
		.spi_bus.spi_get_config = leon3_get_config,
		.spi_bus.spi_set_config = leon3_set_config,
		.cs_up = true
};

//-----------------------------------------------------------------------------
// device table
CHAR_DEVIO_TABLE(
	leon3_spi_handlers, // SPI 设备 I/O 函数表句柄
	spi_write,		// SPI 设备写函数
	spi_read,		// SPI 设备读函数
	NULL,			//
	spi_get_config,	// SPI 读设备设置状态函数
	spi_set_config	// SPI 设备设置函数
);

CHAR_DEVIO_TABLE(...);
CHAR_DEVTAB_ENTRY(...);
//-----------------------------------------------------------------------------

/*Configure a GPIO pin as a SPI chip select line.*/
static inline void leon3_spi_gpio_cs_setup(cyg_uint32 gpio_num) {
	
}

/* Drive a GPIO pin as a SPI chip select line.*/
static inline void leon3_spi_chip_select(cyg_uint32 gpio_num, cyg_bool assert) {
	
}

//-----------------------------------------------------------------------------

/* Implement DMA ISRs.  These disable the DMA channel, mask the interrupt 
 * condition and schedule their respective DSRs.
 */
static cyg_uint32 leon3_tx_ISR(cyg_vector_t vector, cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;
	cyg_haladdress reg_addr;

	/* Disable the DMA channel.*/

	/* Clear down the interrupts.*/

	return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

/**/
static cyg_uint32 leon3_rx_ISR(cyg_vector_t vector, cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;
	cyg_haladdress reg_addr;

	/* Disable the DMA channel.*/
	
	/* Clear down the interrupts.*/

	return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}
/*
 *  Implement DMA DSRs.  These clear down the interrupt conditions and assert 
 *  their respective 'transaction complete' flags.
 */
static void leon3_tx_DSR(cyg_vector_t vector, cyg_ucount32 count,
		cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;

}

static void leon3_rx_DSR(cyg_vector_t vector, cyg_ucount32 count,
		cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;

}


/* Set up a new SPI bus on initialisation.*/
static void leon3_spi_bus_setup(cyg_spi_leon3_bus_t* leon3_bus) {
	CYG_ASSERT (leon3_bus != null,"null pointer parameter!");
	cyg_uint32 SPI_BASE = leon3_bus->base_addr;
	REG32(SPI_BASE+SPI_CAP) = 0x01018002;
		/* Configure the capability register */
					/* SPI_CAP_FDEPTH = 0x80 => Support FIFO size = 128 */
					/* SPI_CAP_SSEN = 1 */
					/* SPI_CAP_MAXWLEN = 0x0 => Support 32-bit length */
					/* SPI_CAP_SSSZ = 0x01 */
	#if leon3_bus->rcv_mode
		REG32(SPI_BASE+SPI_MOD) |= 0x06740000;
		/* Configure the mode register */
										/* SPI_MOD_FACT = 0*/
										/* SPI_MOD_PM = 4 */
										/* SPI_MOD_LEN = 7 */
										/* SPI_MOD_EN = 0 */
				 	 	 	 	 	 	/* SPI_MOD_MS = 1 */
										/* SPI_MOD_REV = 0 */
										/* SPI_MOD_DIV16 = 0 */
										/* SPI_MOD_CPHA = 0 */
										/* SPI_MOD_CPOL = 0 */
										/* SPI_MOD_REV = 1 */ /* MSB transmitted first */
		// REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_CPOL;
		// REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_CPHA;
	#else //SPI_MOD_REV = 0, LSB transmitted first
		REG32(SPI_BASE+SPI_MOD) |= 0x02740000;
	#endif // REV
		REG32(SPI_BASE+SPI_MSK) = 0x0;	/* Disable all interrupts */
		REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_EN;		/* Activate Core */
	//Call upper layer bus init.
	CYG_SPI_BUS_COMMON_INIT(&leon3_bus->spi_bus);
}


//-----------------------------------------------------------------------------
// Initialise SPI interfaces on startup.

static void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_BUS_SPI)
leon3_spi_init(void) {
#ifdef CYGHWR_DEVS_SPI_LEON3_BUS0
	leon3_spi_bus_setup (&cyg_spi_leon3_bus0);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_leon3_BUS1
	leon3_spi_bus_setup (&cyg_spi_leon3_bus1);
#endif
}

//-----------------------------------------------------------------------------
// Start a SPI transaction.

static void leon3_transaction_begin(cyg_spi_device* device) {
	CYG_ASSERT (device != null,"null pointer parameter!");
	
	
}

//-----------------------------------------------------------------------------
// Run a transaction transfer.

static void leon3_transaction_transfer(
	cyg_spi_device* device,	/*device ID*/ 
	cyg_bool polled,		/*slect mode:poll or interrupt mode*/
	cyg_uint32 count, 		/*data count */
	const cyg_uint8* tx_data,	/*snd buffer*/
	cyg_uint8* rx_data,			/*ecv buffer*/
	cyg_bool drop_cs			/*if disable CS on the end*/	
) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) device->spi_bus;
	cyg_spi_cortexm_leon3_device_t* leon3_device =
			(cyg_spi_cortexm_leon3_device_t*) device;

	// Check for unsupported transactions.
	CYG_ASSERT (count> 0, "leon3 SPI : Null transfer requested.");

	// We check that the buffers are half-word aligned and that count is a 
	// multiple of two in order to carry out the 16-bit transfer.
	if (leon3_device->bus_16bit) {
		CYG_ASSERT (!(count & 1) && !((cyg_uint32) tx_data & 1) && !((cyg_uint32) rx_data & 1),
				"leon3 SPI : Misaligned data in 16-bit transfer.");
	}

	// Perform transfer via the bounce buffers.  
	if (leon3_bus->setup->bbuf_size != 0) {
		cyg_uint8* tx_local= NULL;
		cyg_uint8* rx_local= NULL;

		// If the requested transfer is too large for the bounce buffer we assert 
		// in debug builds and truncate in production builds.
		if (count > leon3_bus->setup->bbuf_size) {
			CYG_ASSERT (false, "leon3 SPI : Transfer exceeds bounce buffer size.");
			count = leon3_bus->setup->bbuf_size;
		}
		if (tx_data != NULL) {
			tx_local = leon3_bus->setup->bbuf_tx;
			memcpy(tx_local, tx_data, count);
		}
		if (rx_data != NULL) {
			rx_local = leon3_bus->setup->bbuf_rx;
		}
		spi_transaction_dma(device, false, polled, count, tx_local, rx_local,
				drop_cs);
		if (rx_data != NULL) {
			memcpy(rx_data, rx_local, count);
		}
	}

	// Perform conventional transfer.
	else {
		spi_transaction_dma(device, false, polled, count, tx_data, rx_data,
				drop_cs);
	}
}

//-----------------------------------------------------------------------------
// Carry out a bus tick operation - this just pushes the required number of
// zeros onto the bus, leaving the chip select in its current state.

static void leon3_transaction_tick(cyg_spi_device* device, cyg_bool polled,
		cyg_uint32 count) {
	cyg_spi_cortexm_leon3_device_t* leon3_device =
			(cyg_spi_cortexm_leon3_device_t*) device;

	// Check for unsupported transactions.
	CYG_ASSERT (count> 0, "leon3 SPI : Null transfer requested.");

	// We check that count is a multiple of two in order to carry out the 16-bit transfer.
	if (leon3_device->bus_16bit) {
		CYG_ASSERT (!(count & 1),
				"leon3 SPI : Misaligned data in 16-bit transfer.");
	}

	// Perform null transfer.
	spi_transaction_dma(device, true, polled, count, NULL, NULL, false);
}

//-----------------------------------------------------------------------------
// Terminate a SPI transaction, disabling the SPI controller.

static void leon3_transaction_end(cyg_spi_device* device) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) device->spi_bus;
	cyg_spi_cortexm_leon3_device_t* leon3_device =
			(cyg_spi_cortexm_leon3_device_t*) device;

	cyg_haladdress reg_addr;

	// Ensure that the chip select is deasserted.
	if (leon3_bus->cs_up) {
		CYGACC_CALL_IF_DELAY_US(leon3_device->cs_dw_udly);
		leon3_spi_chip_select(
				leon3_bus->setup->cs_gpio_list[leon3_device->dev_num], false);
		leon3_bus->cs_up = false;
	}

	// Ensure the SPI controller is disabled.
	reg_addr = leon3_bus->setup->spi_reg_base + CYGHWR_HAL_leon3_SPI_CR1;
	HAL_WRITE_UINT32 (reg_addr, leon3_device->spi_cr1_val);
}

//-----------------------------------------------------------------------------
// Note that no dynamic configuration options are currently defined.

static int leon3_get_config(cyg_spi_device* dev, cyg_uint32 key, void* buf,
		cyg_uint32* len) {
	return -1;
}

static int leon3_set_config(cyg_spi_device* dev, cyg_uint32 key,
		const void* buf, cyg_uint32* len) {
	return -1;
}

//=============================================================================
