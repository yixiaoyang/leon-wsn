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
// 
static cyg_drv_mutex_t cyg_spi_lock0;
//-----------------------------------------------------------------------------
// API function call forward references.

static void leon3_transaction_begin(cyg_spi_device*);
static void leon3_transaction_transfer(cyg_spi_device*, cyg_bool, cyg_uint32,
		const cyg_uint8*, cyg_uint8*, cyg_bool);
static void leon3_transaction_tick(cyg_spi_device*, cyg_bool, cyg_uint32);
static void leon3_transaction_end(cyg_spi_device*);
static int leon3_get_config(cyg_spi_device*, cyg_uint32, void*, cyg_uint32*);
static int leon3_set_config(cyg_spi_device*, cyg_uint32, const void*,
		cyg_uint32*);

//-----------------------------------------------------------------------------
// Null data source and sink must be placed in the on-chip SRAM.  This is
// either done explicitly (bounce buffers instantiated) or implicitly (no
// bounce buffers implies that the data area is already on SRAM).


static cyg_uint16 dma_tx_null __attribute__((section (".sram"))) = 0xFFFF;
static cyg_uint16 dma_rx_null __attribute__((section (".sram"))) = 0xFFFF;

//static cyg_uint16 dma_tx_null = 0xFFFF;
//static cyg_uint16 dma_rx_null = 0xFFFF;

//-----------------------------------------------------------------------------
// Instantiate the bus state data structures.
typedef cyg_spi_bus cyg_spi_leon3_bus_t;
cyg_spi_bus cyg_spi_leon3_bus = { 
		.spi_lock = cyg_spi_lock0,
		.spi_bus.spi_transaction_begin = leon3_transaction_begin,
		.spi_bus.spi_transaction_transfer = leon3_transaction_transfer,
		.spi_bus.spi_transaction_tick = leon3_transaction_tick,
		.spi_bus.spi_transaction_end = leon3_transaction_end,
		.spi_bus.spi_get_config = leon3_get_config,
		.spi_bus.spi_set_config = leon3_set_config
};

//-----------------------------------------------------------------------------
// Useful GPIO macros for 'dynamic' pin setup.


//-----------------------------------------------------------------------------
// Configure a GPIO pin as a SPI chip select line.

static inline void leon3_spi_gpio_cs_setup(cyg_uint32 gpio_num) {
	
}

//-----------------------------------------------------------------------------
// Drive a GPIO pin as a SPI chip select line.

static inline void leon3_spi_chip_select(cyg_uint32 gpio_num, cyg_bool assert) {
	
}

//-----------------------------------------------------------------------------
// Implement DMA ISRs.  These disable the DMA channel, mask the interrupt 
// condition and schedule their respective DSRs.

static cyg_uint32 leon3_tx_ISR(cyg_vector_t vector, cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;
	cyg_uint32 chan = leon3_bus->setup->dma_tx_channel;
	cyg_haladdress reg_addr;

	// Disable the DMA channel.
	cyg_drv_isr_lock ();
	reg_addr = leon3_bus->setup->dma_reg_base + CYGHWR_HAL_leon3_DMA_CCR(chan);
	HAL_WRITE_UINT32 (reg_addr, 0);

	// Clear down the interrupts.
	reg_addr = leon3_bus->setup->dma_reg_base + CYGHWR_HAL_leon3_DMA_IFCR;
	HAL_WRITE_UINT32 (reg_addr, CYGHWR_HAL_leon3_DMA_IFCR_MASK (chan));

	cyg_drv_interrupt_acknowledge (vector);
	cyg_drv_interrupt_mask (vector);
	cyg_drv_isr_unlock ();

	return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

static cyg_uint32 leon3_rx_ISR(cyg_vector_t vector, cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;
	cyg_uint32 chan = leon3_bus->setup->dma_rx_channel;
	cyg_haladdress reg_addr;

	// Disable the DMA channel.
	cyg_drv_isr_lock ();
	reg_addr = leon3_bus->setup->dma_reg_base + CYGHWR_HAL_leon3_DMA_CCR(chan);
	HAL_WRITE_UINT32 (reg_addr, 0);

	// Clear down the interrupts.
	reg_addr = leon3_bus->setup->dma_reg_base + CYGHWR_HAL_leon3_DMA_IFCR;
	HAL_WRITE_UINT32 (reg_addr, CYGHWR_HAL_leon3_DMA_IFCR_MASK (chan));

	cyg_drv_interrupt_acknowledge (vector);
	cyg_drv_interrupt_mask (vector);
	cyg_drv_isr_unlock ();

	return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

//-----------------------------------------------------------------------------
// Implement DMA DSRs.  These clear down the interrupt conditions and assert 
// their respective 'transaction complete' flags.

static void leon3_tx_DSR(cyg_vector_t vector, cyg_ucount32 count,
		cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;

	cyg_drv_dsr_lock ();
	leon3_bus->tx_dma_done = true;
	cyg_drv_cond_signal (&leon3_bus->condvar);
	cyg_drv_dsr_unlock ();
}

static void leon3_rx_DSR(cyg_vector_t vector, cyg_ucount32 count,
		cyg_addrword_t data) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) data;

	cyg_drv_dsr_lock ();
	leon3_bus->rx_dma_done = true;
	cyg_drv_cond_signal (&leon3_bus->condvar);
	cyg_drv_dsr_unlock ();
}

//-----------------------------------------------------------------------------
// Set up a new SPI bus on initialisation.

static void leon3_spi_bus_setup(cyg_spi_leon3_bus_t* leon3_bus) {
	int i;
	cyg_haladdress reg_addr;
	cyg_uint32 pin, pinspec, reg_data;

	// Set up the GPIOs for use as chip select lines.
	for (i = 0; i < leon3_bus->setup->cs_gpio_num; i ++) {
		leon3_spi_gpio_cs_setup(leon3_bus->setup->cs_gpio_list[i]);
	}

	// Configure the SPI clock output pin.
	pin = leon3_bus->setup->spi_gpio_list[0];
	pinspec = leon3_GPIO_PINSPEC ((pin >> 4), (pin & 0xF), OUT_SPI, ALT_PUSHPULL);
	CYGHWR_HAL_leon3_GPIO_SET(pinspec);

	// Configure the SPI MISO input.
	pin = leon3_bus->setup->spi_gpio_list[1];
	pinspec = leon3_GPIO_PINSPEC ((pin >> 4), (pin & 0xF), IN, PULLUP);
	CYGHWR_HAL_leon3_GPIO_SET(pinspec);

	// Configure the SPI MOSI output.  
	pin = leon3_bus->setup->spi_gpio_list[2];
	pinspec = leon3_GPIO_PINSPEC ((pin >> 4), (pin & 0xF), OUT_SPI, ALT_PUSHPULL);
	CYGHWR_HAL_leon3_GPIO_SET(pinspec);

	// Set up SPI default configuration.
	reg_addr = leon3_bus->setup->spi_reg_base + CYGHWR_HAL_leon3_SPI_CR2;
	reg_data = CYGHWR_HAL_leon3_SPI_CR2_TXDMAEN
			| CYGHWR_HAL_leon3_SPI_CR2_RXDMAEN;
	HAL_WRITE_UINT32 (reg_addr, reg_data);

	// Ensure that the DMA clocks are enabled.
	reg_addr = CYGHWR_HAL_leon3_RCC + CYGHWR_HAL_leon3_RCC_AHBENR;
	HAL_READ_UINT32 (reg_addr, reg_data);
	if (leon3_bus->setup->dma_reg_base == CYGHWR_HAL_leon3_DMA1)
		reg_data |= CYGHWR_HAL_leon3_RCC_AHBENR_DMA1;
	else
		reg_data |= CYGHWR_HAL_leon3_RCC_AHBENR_DMA2;
	HAL_WRITE_UINT32 (reg_addr, reg_data);

	// Initialise the synchronisation primitivies.
	cyg_drv_mutex_init (&leon3_bus->mutex);
	cyg_drv_cond_init (&leon3_bus->condvar, &leon3_bus->mutex);

	// Hook up the ISRs and DSRs.
	cyg_drv_interrupt_create (leon3_bus->setup->dma_tx_intr, leon3_bus->setup->dma_tx_intr_pri,
			(cyg_addrword_t) leon3_bus, leon3_tx_ISR, leon3_tx_DSR, &leon3_bus->tx_intr_handle,
			&leon3_bus->tx_intr_data);
	cyg_drv_interrupt_attach (leon3_bus->tx_intr_handle);

	cyg_drv_interrupt_create (leon3_bus->setup->dma_rx_intr, leon3_bus->setup->dma_rx_intr_pri,
			(cyg_addrword_t) leon3_bus, leon3_rx_ISR, leon3_rx_DSR, &leon3_bus->rx_intr_handle,
			&leon3_bus->rx_intr_data);
	cyg_drv_interrupt_attach (leon3_bus->rx_intr_handle);

	// Call upper layer bus init.
	CYG_SPI_BUS_COMMON_INIT(&leon3_bus->spi_bus);
}

//-----------------------------------------------------------------------------
// Set up a DMA channel.

static void dma_channel_setup(cyg_spi_device* device, cyg_uint8* data_buf,
		cyg_uint32 count, cyg_bool is_tx, cyg_bool polled) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) device->spi_bus;
	cyg_spi_cortexm_leon3_device_t* leon3_device =
			(cyg_spi_cortexm_leon3_device_t*) device;

	cyg_uint32 chan, reg_data;
	cyg_haladdress dma_reg_base, spi_reg_base, dma_addr, reg_addr;

	// Extract address and channel information.
	dma_reg_base = leon3_bus->setup->dma_reg_base;
	spi_reg_base = leon3_bus->setup->spi_reg_base;
	chan = is_tx ? leon3_bus->setup->dma_tx_channel
			: leon3_bus->setup->dma_rx_channel;

	// Default options for the DMA channel.
	reg_data = CYGHWR_HAL_leon3_DMA_CCR_EN;

	// Do not enable interrupts in polled mode.
	if (!polled)
		reg_data |= CYGHWR_HAL_leon3_DMA_CCR_TCIE
				| CYGHWR_HAL_leon3_DMA_CCR_TEIE;

	// Set DMA channel direction and priority level.  The receive channel has higher
	// priority so that the inbound buffer is always cleared first.
	if (is_tx)
		reg_data |= CYGHWR_HAL_leon3_DMA_CCR_DIR
				| CYGHWR_HAL_leon3_DMA_CCR_PLMEDIUM;
	else
		reg_data |= CYGHWR_HAL_leon3_DMA_CCR_PLHIGH;

	// Set the correct transfer size.
	if (leon3_device->bus_16bit)
		reg_data |= CYGHWR_HAL_leon3_DMA_CCR_PSIZE16
				| CYGHWR_HAL_leon3_DMA_CCR_MSIZE16;
	else
		reg_data |= CYGHWR_HAL_leon3_DMA_CCR_PSIZE8
				| CYGHWR_HAL_leon3_DMA_CCR_MSIZE8;

	// Do not use memory address incrementing for dummy data.
	if (data_buf != NULL) {
		reg_data |= CYGHWR_HAL_leon3_DMA_CCR_MINC;
		dma_addr = (cyg_haladdress) data_buf;
	} else
		dma_addr = (cyg_haladdress) (is_tx ? &dma_tx_null : &dma_rx_null);

	// Program up the DMA memory address.
	reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_CMAR(chan);
	HAL_WRITE_UINT32 (reg_addr, dma_addr);

	// Program up the peripheral memory address.
	dma_addr = spi_reg_base + CYGHWR_HAL_leon3_SPI_DR;
	reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_CPAR(chan);
	HAL_WRITE_UINT32 (reg_addr, dma_addr);

	// Program up the data buffer size.
	reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_CNDTR(chan);
	HAL_WRITE_UINT32 (reg_addr, count);

	// Enable the DMA via the configuration register.
	reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_CCR(chan);
	HAL_WRITE_UINT32 (reg_addr, reg_data);
}

//-----------------------------------------------------------------------------
// Initiate a DMA transfer over the SPI interface.

static void spi_transaction_dma(cyg_spi_device* device, cyg_bool tick_only,
		cyg_bool polled, cyg_uint32 count, const cyg_uint8* tx_data,
		cyg_uint8* rx_data, cyg_bool drop_cs) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) device->spi_bus;
	cyg_spi_cortexm_leon3_device_t* leon3_device =
			(cyg_spi_cortexm_leon3_device_t*) device;

	cyg_haladdress reg_addr;
	cyg_uint32 chan, reg_data;

	cyg_haladdress dma_reg_base = leon3_bus->setup->dma_reg_base;
	cyg_haladdress spi_reg_base = leon3_bus->setup->spi_reg_base;

	// Ensure the chip select is asserted, inserting inter-transaction guard 
	// time if required.  Note that when ticking the device we do not touch the CS.
	if (!leon3_bus->cs_up && !tick_only) {
		CYGACC_CALL_IF_DELAY_US(leon3_device->tr_bt_udly);
		leon3_spi_chip_select(
				leon3_bus->setup->cs_gpio_list[leon3_device->dev_num], true);
		leon3_bus->cs_up = true;
		CYGACC_CALL_IF_DELAY_US(leon3_device->cs_up_udly);
	}

	// Set up the DMA channels.
	dma_channel_setup(device, (cyg_uint8*) tx_data, count, true, polled);
	dma_channel_setup(device, rx_data, count, false, polled);

	// Run the DMA (polling for completion).
	if (polled) {
		cyg_bool transfer_done= false;

		// Enable the SPI controller.
		reg_addr = spi_reg_base + CYGHWR_HAL_leon3_SPI_CR1;
		HAL_WRITE_UINT32 (reg_addr, leon3_device->spi_cr1_val | CYGHWR_HAL_leon3_SPI_CR1_SPE);

		// Spin waiting on both DMA status flags.  Trap bus errors and assert in
		// debug builds or return garbage in production builds.
		reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_ISR;
		do {
			HAL_READ_UINT32 (reg_addr, reg_data);
			if ((reg_data
					& CYGHWR_HAL_leon3_DMA_ISR_TEIF(leon3_bus->setup->dma_tx_channel))
					|| (reg_data
							& CYGHWR_HAL_leon3_DMA_ISR_TEIF(leon3_bus->setup->dma_rx_channel))) {
				CYG_ASSERT (false, "leon3 SPI : DMA bus fault - enable bounce buffers.");
				transfer_done = true;
			}
			if ((reg_data
					& CYGHWR_HAL_leon3_DMA_ISR_TCIF(leon3_bus->setup->dma_tx_channel))
					&& (reg_data
							& CYGHWR_HAL_leon3_DMA_ISR_TCIF(leon3_bus->setup->dma_rx_channel))) {
				transfer_done = true;
			}
		} while (!transfer_done);

		// Disable the DMA channels on completion and clear the status flags.
		chan = leon3_bus->setup->dma_tx_channel;
		reg_data = CYGHWR_HAL_leon3_DMA_IFCR_MASK(chan);
		reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_CCR(chan);
		HAL_WRITE_UINT32 (reg_addr, 0);

		chan = leon3_bus->setup->dma_rx_channel;
		reg_data |= CYGHWR_HAL_leon3_DMA_IFCR_MASK(chan);
		reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_CCR(chan);
		HAL_WRITE_UINT32 (reg_addr, 0);

		reg_addr = dma_reg_base + CYGHWR_HAL_leon3_DMA_IFCR;
		HAL_WRITE_UINT32 (reg_addr, reg_data);
	}

	// Run the DMA (interrupt driven).
	else {
		cyg_drv_mutex_lock (&leon3_bus->mutex);
		cyg_drv_dsr_lock ();
		leon3_bus->tx_dma_done = false;
		leon3_bus->rx_dma_done = false;

		cyg_drv_interrupt_unmask (leon3_bus->setup->dma_tx_intr);
		cyg_drv_interrupt_unmask (leon3_bus->setup->dma_rx_intr);

		// Enable the SPI controller.
		reg_addr = spi_reg_base + CYGHWR_HAL_leon3_SPI_CR1;
		HAL_WRITE_UINT32 (reg_addr, leon3_device->spi_cr1_val | CYGHWR_HAL_leon3_SPI_CR1_SPE);

		// Sit back and wait for the ISR/DSRs to signal completion.
		do {
			cyg_drv_cond_wait (&leon3_bus->condvar);
		} while (!(leon3_bus->tx_dma_done && leon3_bus->rx_dma_done));

		cyg_drv_dsr_unlock ();
		cyg_drv_mutex_unlock (&leon3_bus->mutex);
	}

	// Disable the SPI controller.
	reg_addr = spi_reg_base + CYGHWR_HAL_leon3_SPI_CR1;
	HAL_WRITE_UINT32 (reg_addr, leon3_device->spi_cr1_val);

	// Deassert the chip select.
	if (drop_cs && !tick_only) {
		CYGACC_CALL_IF_DELAY_US(leon3_device->cs_dw_udly);
		leon3_spi_chip_select(
				leon3_bus->setup->cs_gpio_list[leon3_device->dev_num], false);
		leon3_bus->cs_up = false;
	}
}

//-----------------------------------------------------------------------------
// Initialise SPI interfaces on startup.

static void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_BUS_SPI)
leon3_spi_init(void) {
#if defined(CYGHWR_DEVS_SPI_CORTEXM_leon3_BUS3) && \
    defined(CYGHWR_DEVS_SPI_CORTEXM_leon3_BUS3_DISABLE_DEBUG_PORT)
	// Disable debug port, freeing up SPI bus 3 pins.
	cyg_uint32 reg_val;
	HAL_READ_UINT32 (CYGHWR_HAL_leon3_AFIO + CYGHWR_HAL_leon3_AFIO_MAPR, reg_val);
	reg_val &= ~((cyg_uint32) CYGHWR_HAL_leon3_AFIO_MAPR_SWJ_MASK);
	reg_val |= CYGHWR_HAL_leon3_AFIO_MAPR_SWJ_SWDPDIS;
	HAL_WRITE_UINT32 (CYGHWR_HAL_leon3_AFIO + CYGHWR_HAL_leon3_AFIO_MAPR, reg_val);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_leon3_BUS1
	leon3_spi_bus_setup (&cyg_spi_leon3_bus1);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_leon3_BUS2
	leon3_spi_bus_setup (&cyg_spi_leon3_bus2);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_leon3_BUS3
	leon3_spi_bus_setup (&cyg_spi_leon3_bus3);
#endif
}

//-----------------------------------------------------------------------------
// Start a SPI transaction.

static void leon3_transaction_begin(cyg_spi_device* device) {
	cyg_spi_leon3_bus_t* leon3_bus = (cyg_spi_leon3_bus_t*) device->spi_bus;
	cyg_spi_cortexm_leon3_device_t* leon3_device =
			(cyg_spi_cortexm_leon3_device_t*) device;

	cyg_haladdress reg_addr;
	cyg_uint32 reg_data, divided_clk, br;

	// On the first transaction, generate the values to be programmed into the
	// SPI configuration registers for this device and cache them.  This avoids
	// having to recalculate the prescaler for every transaction.
	if (!leon3_device->spi_cr1_val) {
		reg_data = CYGHWR_HAL_leon3_SPI_CR1_MSTR | CYGHWR_HAL_leon3_SPI_CR1_SSI
				| CYGHWR_HAL_leon3_SPI_CR1_SSM;
		if (leon3_device->cl_pol)
			reg_data |= CYGHWR_HAL_leon3_SPI_CR1_CPOL;
		if (leon3_device->cl_pha)
			reg_data |= CYGHWR_HAL_leon3_SPI_CR1_CPHA;
		if (leon3_device->bus_16bit)
			reg_data |= CYGHWR_HAL_leon3_SPI_CR1_DFF;

		// Calculate the maximum viable bus speed.
		divided_clk = leon3_bus->setup->apb_freq / 2;
		for (br = 0; (br < 7) && (divided_clk > leon3_device->cl_brate); br++)
			divided_clk >>= 1;
		CYG_ASSERT (divided_clk <= leon3_device->cl_brate,
				"leon3 SPI : Cannot run bus slowly enough for peripheral.");
		reg_data |= CYGHWR_HAL_leon3_SPI_CR1_BR(br);

		// Cache the configuration register settings.
		leon3_device->spi_cr1_val = reg_data;
	}

	// Set up the SPI controller.
	reg_addr = leon3_bus->setup->spi_reg_base + CYGHWR_HAL_leon3_SPI_CR1;
	HAL_WRITE_UINT32 (reg_addr, leon3_device->spi_cr1_val);
}

//-----------------------------------------------------------------------------
// Run a transaction transfer.

static void leon3_transaction_transfer(cyg_spi_device* device, cyg_bool polled,
		cyg_uint32 count, const cyg_uint8* tx_data, cyg_uint8* rx_data,
		cyg_bool drop_cs) {
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
