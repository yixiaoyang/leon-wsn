// eCos memory layout - Fri Oct 20 08:18:20 2000

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>
#endif

externC unsigned int cyg_hal_stack_top;
#define CYGHAL_DSUSUPPLIED_STACK_TOP cyg_hal_stack_top

#define CYGMEM_REGION_ram (0x40000000)
#define CYGMEM_REGION_ram_SIZE (CYGHAL_DSUSUPPLIED_STACK_TOP-CYGMEM_REGION_ram)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (CYGHAL_DSUSUPPLIED_STACK_TOP - (size_t) CYG_LABEL_NAME (__heap1))

