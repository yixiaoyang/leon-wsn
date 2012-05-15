#ifndef CYGONCE_HAL_VECTORS_H
#define CYGONCE_HAL_VECTORS_H

//=============================================================================
//
//      vectors.h
//
//      SPARC Architecture specific vector numbers &c
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):   hmt
// Contributors:hmt
// Date:        1998-12-10
// Purpose:     Define architecture abstractions and some shared info;
//              this file is included by assembler files as well as C/C++.
// Usage:       #include <cyg/hal/vectors.h>

//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_sparc.h>

#define __WINSIZE CYGPKG_HAL_SPARC_REGISTER_WINDOWS

#if (__WINSIZE < 2 || __WINSIZE > 32)
# error CYGPKG_HAL_SPARC_REGISTER_WINDOWS not supported
#endif
#if (__WINSIZE == 2 && !defined(CYGHWR_HAL_SPARC_FLAT) )
# error window size of 2 only works with -mflat option
#endif

#if (__WINSIZE == 2 || __WINSIZE == 4 || __WINSIZE == 8 || __WINSIZE == 16 || __WINSIZE == 32)
#define CYGPKG_HAL_SPARC_REGISTER_WINDOWS_POW2 1
#endif

#if (__WINSIZE <= 2)
#define __WINBITS (2-1)
#else
# if (__WINSIZE <= 4)
# define __WINBITS (4-1)
# else
#  if (__WINSIZE <= 8)
#  define __WINBITS (8-1)
#  else
#   if (__WINSIZE <= 16)
#   define __WINBITS (16-1)
#   else
#    define __WINBITS (32-1)
#   endif
#  endif
# endif
#endif

// These should be generic to all SPARCs:

#define __WINBITS_MAXIMAL 0x1f

#define __WIN_INIT (__WINSIZE - 1)

#define __WIM_INIT (1 << __WIN_INIT)

// ------------------------------------------------------------------------

#define TRAP_WUNDER     6       // Window Underflow trap number
#define TRAP_WOVER      5       // Window Overflow trap number
#define TRAP_FPDIS      4       // FPU Disabled trap number

#define TRAP_INTR_MIN   17      // smallest interrupt trap number
#define TRAP_INTR_MAX   31      // largest interrupt trap number

#define TT_MASK         0xff0   // trap type mask from tbr
#define TT_SHL          4       // shift to get a tbr value

// Alternatively, detect an interrupt by testing tbr for being in the range
// 16-31 by masking &c:
#define TT_IS_INTR_MASK         0xf00
#define TT_IS_INTR_VALUE        0x100

#if TT_IS_INTR_VALUE != ((TRAP_INTR_MIN << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (1)"
#endif

#if TT_IS_INTR_VALUE != ((TRAP_INTR_MAX << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (2)"
#endif

#if TT_IS_INTR_VALUE != (((TRAP_INTR_MIN+1) << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (3)"
#endif

#if TT_IS_INTR_VALUE != (((TRAP_INTR_MAX-1) << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (4)"
#endif


        
//#define SCHED_LOCK_MANGLED_NAME _18Cyg_Scheduler_Base.sched_lock
#define SCHED_LOCK_MANGLED_NAME cyg_scheduler_sched_lock


#ifdef CYGPKG_HAL_SPARC_LAZY_FPU_SWITCH	
#define SAVE_REGS_SIZE (4 * (32+2)) // 32 words of 4 bytes each + fpu ctx + align
#define SAVE_FPUSTATE_SIZE (4 * (32+2)) // 32 fregs + fsr + _dummy
#elif defined(CYGHWR_HAL_SPARC_FPU)
#define SAVE_REGS_SIZE (4 * (32+32+2)) // 32 words of 4 bytes each + 32 FP + FSR + align
#else
#define SAVE_REGS_SIZE (4 * 32) // 32 words of 4 bytes each
#endif


#define SYS_exit        1
#define SYS_irqdis      2
#define SYS_irqen       3

#endif // CYGONCE_HAL_VECTORS_H
// EOF vectors.h
