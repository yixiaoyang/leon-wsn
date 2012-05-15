#ifndef CYGONCE_HAL_XPIC_H
#define CYGONCE_HAL_XPIC_H

//=============================================================================
//
//      hal_xpic.h
//
//      HAL eXternal Programmable Interrupt Controller support
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
// Author(s):   nickg, gthomas, hmt
// Contributors:        nickg, gthomas, hmt
// Date:        1999-01-28
// Purpose:     Define Interrupt support
// Description: The macros defined here provide the HAL APIs for handling
//              an external interrupt controller, and which interrupt is
//              used for what.
//              
// Usage:
//              #include <cyg/hal/hal_intr.h> // which includes this file
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_smp.h>
#include <cyg/hal/hal_leon3.h>

//-----------------------------------------------------------------------------
// Interrupt controller access

// The vector used by the Real time clock
externC cyg_uint32 hal_sparc_leon3_clock_irqnr(void);
#define CYGNUM_HAL_INTERRUPT_RTC hal_sparc_leon3_clock_irqnr()


externC void leon3_disable_irq(unsigned int irq_nr);
externC void leon3_enable_irq(unsigned int irq_nr);

#define HAL_INTERRUPT_MASK( _vector_ ) CYG_MACRO_START                      \
    cyg_uint32 _traps_, _mask_;                                             \
    HAL_DISABLE_INTERRUPTS( _traps_ );                                      \
    leon3_disable_irq(_vector_);                                            \
    HAL_RESTORE_INTERRUPTS( _traps_ );                                      \
CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ ) CYG_MACRO_START                    \
    cyg_uint32 _traps_, _mask_;                                             \
    HAL_DISABLE_INTERRUPTS( _traps_ );                                      \
    leon3_enable_irq(_vector_);                                             \
    HAL_RESTORE_INTERRUPTS( _traps_ );                                      \
CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )              /* nothing */

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ ) /* nothing */

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ ) /* nothing */


//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_XPIC_H
// End of hal_xpic.h
