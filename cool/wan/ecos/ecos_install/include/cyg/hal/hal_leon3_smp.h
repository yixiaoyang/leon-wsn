#ifndef CYGONCE_HAL_LEON3_SMP_H
#define CYGONCE_HAL_LEON3_SMP_H

//=============================================================================
//
//      hal_leon3_cmp.h
//
//      HAL smp defined 
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
#include <cyg/hal/hal_leon3.h>

#ifdef CYGPKG_HAL_SMP_SUPPORT

//-----------------------------------------------------------------------------
// CPU numbering macros

externC cyg_uint32 cyg_hal_smp_cpu_running_count;

#define HAL_SMP_CPU_TYPE        cyg_uint32

#define HAL_SMP_CPU_MAX         CYGPKG_HAL_SMP_CPU_MAX

#define HAL_SMP_CPU_COUNT()     cyg_hal_smp_cpu_running_count

#define HAL_SMP_CPU_START_COUNT() HAL_SMP_CPU_MAX


externC cyg_uint32 cyg_hal_smp_cpu_count2idx(cyg_uint32 n);
externC cyg_uint32 cyg_hal_smp_cpu_idx2count(cyg_uint32 n);
#define HAL_SMP_CPU_COUNT2IDX(n)  cyg_hal_smp_cpu_count2idx(n)
#define HAL_SMP_CPU_IDX2COUNT(n)  cyg_hal_smp_cpu_idx2count(n)

externC cyg_uint32 sparc_leon3_get_cpuid(void);

#define HAL_SMP_CPU_THIS()                      \
({                                              \
    HAL_SMP_CPU_TYPE __id;                      \
    __asm__ __volatile__(                       \
         "rd     %%asr17,%0\n\t"                \
	: "=r" (__id) : );                      \
    ((__id >> 28) & 0xff);                      \
})

#define HAL_SMP_CPU_NONE        (CYGPKG_HAL_SMP_CPU_MAX+1)

//-----------------------------------------------------------------------------
// CPU startup

__externC void cyg_hal_cpu_reset(HAL_SMP_CPU_TYPE cpu);

#define HAL_SMP_CPU_START( __cpu ) cyg_hal_cpu_reset( __cpu );

#define HAL_SMP_CPU_RESCHEDULE_INTERRUPT( __cpu, __wait ) \
        cyg_hal_cpu_message( __cpu, HAL_SMP_MESSAGE_RESCHEDULE, 0, __wait);

#define HAL_SMP_CPU_TIMESLICE_INTERRUPT( __cpu, __wait ) \
        cyg_hal_cpu_message( __cpu, HAL_SMP_MESSAGE_TIMESLICE, 0, __wait);

//-----------------------------------------------------------------------------
// CPU message exchange

__externC void cyg_hal_cpu_message( HAL_SMP_CPU_TYPE cpu,
                                    CYG_WORD32 msg,
                                    CYG_WORD32 arg,
                                    CYG_WORD32 wait);

#define HAL_SMP_MESSAGE_TYPE            0xF0000000
#define HAL_SMP_MESSAGE_ARG             (~HAL_SMP_MESSAGE_TYPE)

#define HAL_SMP_MESSAGE_RESCHEDULE      0x10000000
#define HAL_SMP_MESSAGE_MASK            0x20000000
#define HAL_SMP_MESSAGE_UNMASK          0x30000000
#define HAL_SMP_MESSAGE_REVECTOR        0x40000000
#define HAL_SMP_MESSAGE_TIMESLICE       0x50000000


//-----------------------------------------------------------------------------
// Test-and-set support
// These macros provide test-and-set support for the least significant bit
// in a word. 

#define HAL_TAS_TYPE    volatile CYG_WORD32

#define HAL_TAS_SET( _tas_, _oldb_ )                    \
CYG_MACRO_START                                         \
{                                                       \
    register CYG_WORD32 __old = 1;                      \
    __asm__ volatile ("swapa [%2] %3, %0" :             \
			      "=r" (__old) :            \
			      "0" (__old),              \
			      "r" (&_tas_),             \
			      "i" (1));                 \
    _oldb_ = ( __old & 1 ) != 0;                        \
}                                                       \
CYG_MACRO_END

#define HAL_TAS_CLEAR( _tas_, _oldb_ )                  \
CYG_MACRO_START                                         \
{                                                       \
    register CYG_WORD32 __old = 0;                      \
    __asm__ volatile ("swapa [%2] %3, %0" :             \
			      "=r" (__old) :            \
			      "0" (__old),              \
			      "r" (&_tas_),             \
			      "i" (1));                 \
    _oldb_ = ( __old & 1 ) != 0;                        \
}                                                       \
CYG_MACRO_END



//-----------------------------------------------------------------------------
// Spinlock support.
// Built on top of test-and-set code.

#define HAL_SPINLOCK_TYPE       volatile CYG_WORD32

#define HAL_SPINLOCK_INIT_CLEAR 0

#define HAL_SPINLOCK_INIT_SET   1

#define HAL_SPINLOCK_SPIN( _lock_ )             \
CYG_MACRO_START                                 \
{                                               \
    cyg_bool _val_;                             \
    do                                          \
    {                                           \
        HAL_TAS_SET( _lock_, _val_ );           \
    } while( _val_ );                           \
}                                               \
CYG_MACRO_END

#define HAL_SPINLOCK_CLEAR( _lock_ )            \
CYG_MACRO_START                                 \
{                                               \
    cyg_bool _val_;                             \
    HAL_TAS_CLEAR( _lock_ , _val_ );            \
}                                               \
CYG_MACRO_END

#define HAL_SPINLOCK_TRY( _lock_, _val_ )       \
    HAL_TAS_SET( _lock_, _val_ );               \
    (_val_) = (((_val_) & 1) == 0)

#define HAL_SPINLOCK_TEST( _lock_, _val_ )      \
    (_val_) = (((_lock_) & 1) != 0)

//-----------------------------------------------------------------------------
// Diagnostic output serialization

__externC HAL_SPINLOCK_TYPE cyg_hal_smp_diag_lock;

#define CYG_HAL_DIAG_LOCK_DATA_DEFN \
        HAL_SPINLOCK_TYPE cyg_hal_smp_diag_lock = HAL_SPINLOCK_INIT_CLEAR

#define CYG_HAL_DIAG_LOCK() HAL_SPINLOCK_SPIN( cyg_hal_smp_diag_lock )

#define CYG_HAL_DIAG_UNLOCK() HAL_SPINLOCK_CLEAR( cyg_hal_smp_diag_lock )

#define leon3smp_diag_printf(fmt,arg...)  \
    CYG_HAL_DIAG_LOCK();    \
    diag_printf(fmt,##arg); \
    CYG_HAL_DIAG_UNLOCK();                      

//-----------------------------------------------------------------------------
// Some extra definitions

__externC HAL_SPINLOCK_TYPE cyg_hal_ioapic_lock;

//-----------------------------------------------------------------------------

//--------------------------------------------------------------------------
// irqmp interrupt configuration
// Additional SMP interrupt configuration support.

externC void leon3_interrupt_set_cpu( CYG_WORD32 vector, HAL_SMP_CPU_TYPE cpu );
externC void leon3_interrupt_get_cpu( CYG_WORD32 vector, HAL_SMP_CPU_TYPE *cpu );
externC void leon3_smpirq_raise(CYG_WORD32 cpu);

#define HAL_INTERRUPT_SET_CPU( _vector_, _cpu_ )  leon3_interrupt_set_cpu( _vector_, _cpu_ );
#define HAL_INTERRUPT_GET_CPU( _vector_, _cpu_ )  leon3_interrupt_get_cpu( _vector_, &(_cpu_) );

#endif // CYGPKG_HAL_SMP_SUPPORT

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_LEON3_SMP_H
// End of hal_leon3_smp.h
