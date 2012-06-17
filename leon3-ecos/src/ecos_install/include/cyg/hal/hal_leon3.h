#ifndef CYGONCE_HAL_LEON3_H
#define CYGONCE_HAL_LEON3_H

//=============================================================================
//
//      hal_leon3.h
//
//      HAL AMBA defined 
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Gaisler Research (Konrad Eisele <eiselekd@web.de>)
// Contributors: 
// Date:        2004-01-12
// Purpose:     Define Interrupt support
// Description: 
// Usage:
//              #include <cyg/hal/hal_leon3.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================


#include <cyg/infra/cyg_type.h>         // Base types
#include <pkgconf/hal_sparc.h>
#include <cyg/hal/hal_amba.h>

/* ASI codes */
#define ASI_LEON_CACHEMISS      0x01
#define ASI_LEON_PCI		0x04
#define ASI_LEON_IFLUSH		0x05
#define ASI_LEON_DFLUSH		0x06
#define ASI_LEON_ITAG		0x0c
#define ASI_LEON_IDATA		0x0d
#define ASI_LEON_DTAG		0x0e
#define ASI_LEON_DDATA		0x0f
#define ASI_LEON_MMUFLUSH	0x18
#define ASI_LEON_MMUREGS	0x19
#define ASI_LEON_BYPASS		0x1c
#define ASI_LEON_FLUSH_PAGE	0x10
/*
#define ASI_LEON_FLUSH_SEGMENT	0x11
#define ASI_LEON_FLUSH_REGION	0x12
*/
#define ASI_LEON_FLUSH_CTX	0x13
#define ASI_LEON_DCTX		0x14
#define ASI_LEON_ICTX		0x15
#define ASI_MMU_DIAG		0x1d

#define ASI_LEON3_IFLUSH		0x10
#define ASI_LEON3_DFLUSH		0x11

#define ASI_LEON3_SYSCTRL		0x02

#define ASI_LEON3_SYSCTRL_ICFG		0x08
#define ASI_LEON3_SYSCTRL_DCFG		0x0c
#define ASI_LEON3_SYSCTRL_CFG_SNOOPING (1<<27)

/*
 *  The following defines the bits in the LEON UART Status Registers.
 */

#define LEON_REG_UART_STATUS_DR   0x00000001 /* Data Ready */
#define LEON_REG_UART_STATUS_TSE  0x00000002 /* TX Send Register Empty */
#define LEON_REG_UART_STATUS_THE  0x00000004 /* TX Hold Register Empty */
#define LEON_REG_UART_STATUS_BR   0x00000008 /* Break Error */
#define LEON_REG_UART_STATUS_OE   0x00000010 /* RX Overrun Error */
#define LEON_REG_UART_STATUS_PE   0x00000020 /* RX Parity Error */
#define LEON_REG_UART_STATUS_FE   0x00000040 /* RX Framing Error */
#define LEON_REG_UART_STATUS_ERR  0x00000078 /* Error Mask */

 
/*
 *  The following defines the bits in the LEON UART Ctrl Registers.
 */

#define LEON_REG_UART_CTRL_RE     0x00000001 /* Receiver enable */
#define LEON_REG_UART_CTRL_TE     0x00000002 /* Transmitter enable */
#define LEON_REG_UART_CTRL_RI     0x00000004 /* Receiver interrupt enable */
#define LEON_REG_UART_CTRL_TI     0x00000008 /* Transmitter interrupt enable */
#define LEON_REG_UART_CTRL_PS     0x00000010 /* Parity select */
#define LEON_REG_UART_CTRL_PE     0x00000020 /* Parity enable */
#define LEON_REG_UART_CTRL_FL     0x00000040 /* Flow control enable */
#define LEON_REG_UART_CTRL_LB     0x00000080 /* Loop Back enable */

#define LEON3_GPTIMER_EN 1
#define LEON3_GPTIMER_RL 2
#define LEON3_GPTIMER_LD 4
#define LEON3_GPTIMER_IRQEN 8


/* irq masks */
#define LEON_HARD_INT(x)	(1 << (x)) /* irq 0-15 */
#define LEON_IRQMASK_R		0x0000fffe /* bit 15- 1   */

#define LEON3_IRQMPSTATUS_CPUNR 28

#ifndef __ASSEMBLER__

typedef struct {
  volatile unsigned int ilevel;
  volatile unsigned int ipend;
  volatile unsigned int iforce;
  volatile unsigned int iclear;
  volatile unsigned int mpstatus;
  volatile unsigned int notused01;
  volatile unsigned int notused02;
  volatile unsigned int notused03;
  volatile unsigned int notused10;
  volatile unsigned int notused11;
  volatile unsigned int notused12;
  volatile unsigned int notused13;
  volatile unsigned int notused20;
  volatile unsigned int notused21;
  volatile unsigned int notused22;
  volatile unsigned int notused23;
  volatile unsigned int mask[16];
  volatile unsigned int force[16];
} LEON3_IrqCtrl_Regs_Map; 
externC volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs; 

typedef struct {
  volatile unsigned int data;
  volatile unsigned int status;
  volatile unsigned int ctrl;
  volatile unsigned int scaler;
} LEON3_APBUART_Regs_Map;


typedef struct {
  volatile unsigned int val;
  volatile unsigned int rld;
  volatile unsigned int ctrl;
  volatile unsigned int unused;
} LEON3_GpTimerElem_Regs_Map; 

typedef struct {
  volatile unsigned int scalar;
  volatile unsigned int scalar_reload;
  volatile unsigned int config;
  volatile unsigned int unused;
  volatile LEON3_GpTimerElem_Regs_Map e[8];
} LEON3_GpTimer_Regs_Map; 

typedef struct {
  volatile unsigned int iodata;
  volatile unsigned int ioout;
  volatile unsigned int iodir;
  volatile unsigned int irqmask;
  volatile unsigned int irqpol;
  volatile unsigned int irqedge;
} LEON3_IOPORT_Regs_Map;

/*
 *  Types and structure used for AMBA Plug & Play bus scanning 
 */

typedef struct amba_device_table {
  int            devnr;           /* numbrer of devices on AHB or APB bus */
  unsigned int   *addr[16];       /* addresses to the devices configuration tables */
  unsigned int   allocbits[1];       /* 0=unallocated, 1=allocated driver */
} amba_device_table;

typedef struct amba_confarea_type {
  amba_device_table ahbmst;
  amba_device_table ahbslv;
  amba_device_table apbslv;
  unsigned int      apbmst;
} amba_confarea_type;

extern unsigned long amba_find_apbslv_addr(unsigned long vendor, unsigned long device, unsigned long *irq);

// collect apb slaves
typedef struct amba_apb_device {
  unsigned int   start, irq;
} amba_apb_device;
extern int amba_get_free_apbslv_devices (int vendor, int device, amba_apb_device *dev,int nr);

// collect ahb slaves
typedef struct amba_ahb_device {
  unsigned int   start[4], irq;
} amba_ahb_device;
extern int amba_get_free_ahbslv_devices (int vendor, int device, amba_ahb_device *dev,int nr);
extern void amba_init(void);

#endif //!__ASSEMBLER__



//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_LEON3_H
// End of hal_leon3.h
