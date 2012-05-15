#ifndef CYGONCE_HAL_CLOCK_H
#define CYGONCE_HAL_CLOCK_H

//=============================================================================
//
//      hal_clock.h
//
//      HAL clock support
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
// Purpose:     Define clock support
// Description: The macros defined here provide the HAL APIs for handling
//              the clock.
//              
// Usage:
//              #include <cyg/hal/hal_clock.h> // which includes this file
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/hal_io.h>


//-----------------------------------------------------------------------------
// Clock control

externC cyg_int32 cyg_hal_sparc_clock_period;
externC void hal_sparc_leon3_clock_init(cyg_uint32 period);
externC cyg_uint32 hal_sparc_leon3_clock_read(void);

#define HAL_CLOCK_INITIALIZE( _period_ ) hal_sparc_leon3_clock_init(_period_);     
#define HAL_CLOCK_RESET( _vector_, _period_ ) 
#define HAL_CLOCK_READ( _pvalue_ ) {                                  \
   cyg_uint32 _read_;                                                 \
   _read_ = hal_sparc_leon3_clock_read();                             \
   *((cyg_uint32 *)(_pvalue_)) =                                      \
                 (cyg_hal_sparc_clock_period - _read_ ); }

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ )         HAL_CLOCK_READ( _pvalue_ )
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CLOCK_H
// End of hal_clock.h
