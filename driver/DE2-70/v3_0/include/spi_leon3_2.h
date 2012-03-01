#ifndef CYGONCE_DEVS_SPI_CORTEXM_LEON3_H
#define CYGONCE_DEVS_SPI_CORTEXM_LEON3_H
//=============================================================================
//
//      spi_leon3.h
//
//      Header definitions for LEON3 SPI driver.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   xiaoyang.yi
// Date:        2012-2-24
// Purpose:     Leon3 SPI driver definitions.
// Description: 
// Usage:       #include <cyg/io/spi_leon3.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>
#include <pkgconf/hal_sparc_leon3.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>

#include <cyg/io/devtab.h>// 设备 I/O 入口表定义及声明

#define CYGHWR_DEVS_SPI_LEON3_BUS0
#define CYGHWR_DEVS_SPI_LEON3_BUS0_DBG			1

//#define SPI0_BASE		0x80000600

/* SPICTL Registers APB Address Offset */
#define SPI_CAP			0x0
#define SPI_MOD			0x20
#define SPI_EVE			0x24
#define SPI_MSK			0x28
#define SPI_CMD			0x2C
#define SPI_TX			0x30
#define SPI_RX			0x34
#define SPI_SLV			0x38
/* Bit Definition of SPICTL Capability Register  */
#define SPI_CAP_REV		0x7F
#define SPI_CAP_FDEPTH		0xFF00
#define SPI_CAP_SSEN		0x10000
#define SPI_CAP_ASELA		0x20000
#define SPI_CAP_AMODE		0x40000
#define SPI_CAP_TWEN		0x80000
#define SPI_CAP_MAXWLEN	0xF00000
#define SPI_CAP_SSSZ		0xFF000000
/* Bit Definition of SPICTL Mode Register */
#define SPI_MOD_TAC		0x10
#define SPI_MOD_ASELDEL	0x60
#define SPI_MOD_CG		0x780
#define SPI_MOD_OD		0x1000
#define SPI_MOD_FACT		0x2000
#define SPI_MOD_ASEL		0x4000
#define SPI_MOD_TW		0x8000
#define SPI_MOD_PM		0xF0000		/* Prescale Module*/
#define SPI_MOD_LEN		0xF00000
#define SPI_MOD_EN		0x1000000
#define SPI_MOD_MS		0x2000000		/* Master/Slave */
#define SPI_MOD_REV		0x4000000
#define SPI_MOD_DIV16		0x8000000
#define SPI_MOD_CPHA		0x10000000
#define SPI_MOD_CPOL		0x20000000
#define SPI_MOD_LOOP		0x40000000
#define SPI_MOD_AMEN		0x80000000
/* Bit Definition of SPICTL Event Register */
#define SPI_EVE_NF		0x100
#define SPI_EVE_NE		0x200
#define SPI_EVE_MME		0x400
#define SPI_EVE_UR		0x800
#define SPI_EVE_OV		0x1000
#define SPI_EVE_LT		0x4000
#define SPI_EVE_TIP		0x80000000			/* Transfer in Progress */
/* Bit Definition of SPICTL Mask Register */
#define SPI_MSK_NFE		0x100
#define SPI_MSK_NEE		0x200
#define SPI_MSK_MMEE		0x400
#define SPI_MSK_UNE		0x800
#define SPI_MSK_OVE		0x1000
#define SPI_MSK_LTE		0x4000
#define SPI_MSK_TIPE		0x80000000
/* Bit Definition of SPICTL Command Register */
#define SPI_CMD_LST		0x400000
/* Bit Definition of SPICTL Slave Select Register */
#define SPI_SLV_SSSZ		1
#define SPI_SLV_SLVSEL		(1<<SPI_SLV_SSSZ)-1

/* Some useful routines */
#define SLV_SELECT()		REG32(SPI_BASE+SPI_SLV) &= ~SPI_SLV_SLVSEL
#define SLV_DESELECT()		REG32(SPI_BASE+SPI_SLV) |= SPI_SLV_SLVSEL

/* spi mode*/
#define SPI_REV_MOD	1
#define SPI_SND_MOD	0

typedef struct cyg_spi_leon3_bus_t
{
    // ---- Upper layer data ----
    cyg_spi_bus       spi_bus;         // Upper layer SPI bus data.

    // ---- Driver state (private) ----
    cyg_uint32 base_addr;			   // base address of device
    cyg_uint8 mode;					   // master or slave mod	
    cyg_uint8 rcv_mode;					   // snd or rcv mod			
    cyg_drv_mutex_t   mutex;           // Transfer mutex.
    cyg_bool          cs_up;           // Chip select asserted flag.
} cyg_spi_leon3_bus_t;


#endif // CYGONCE_DEVS_SPI_CORTEXM_LEON3_H
