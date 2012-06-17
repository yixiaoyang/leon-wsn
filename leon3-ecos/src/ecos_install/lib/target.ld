STARTUP(vectors.o)
ENTRY(reset_vector)

INPUT(extras.o)


GROUP(libtarget.a libgcc.a libsupc++.a)
MEMORY
{
    ram : ORIGIN = 0x40000000, LENGTH = 0x40000000
}

SECTIONS
{
   
    .rom_vectors 0x40000000 : { . = .; KEEP (*(.vectors)) } > ram
    .text ALIGN (0x1) : { _stext = ABSOLUTE(.); *(.text*) *(.gnu.warning) *(.gnu.linkonce*) *(.init) } > ram _etext = .; PROVIDE (etext = .);
    .fini ALIGN (0x1) : { . = .; *(.fini) } > ram
    .rodata ALIGN (0x1) : { . = .; *(.rodata*) } > ram
    .rodata1 ALIGN (0x1) : { . = .; *(.rodata1) } > ram
    .fixup ALIGN (0x1) : { . = .; *(.fixup) } > ram
    .gcc_except_table ALIGN (0x1) : { . = .; *(.gcc_except_table) } > ram
    .data ALIGN (0x1) : { __ram_data_start = ABSOLUTE (.); *(.data*) *(.data1) _GOT1_START_ = ABSOLUTE (.); *(.got1) _GOT1_END_ = ABSOLUTE (.); _GOT2_START_ = ABSOLUTE (.); *(.got2) _GOT2_END_ = ABSOLUTE (.); . = ALIGN (4); KEEP(*( SORT (.ecos.table.*))) ; __CTOR_LIST__ = ABSOLUTE (.); KEEP(*(SORT(.ctors*))) __CTOR_END__ = ABSOLUTE (.); __DTOR_LIST__ = ABSOLUTE (.); KEEP(*(SORT(.dtors*))) __DTOR_END__ = ABSOLUTE (.); _GOT_START = ABSOLUTE (.); _GLOBAL_OFFSET_TABLE_ = ABSOLUTE (.) + 32768; _SDA_BASE_ = ABSOLUTE (.); *(.got.plt) *(.got) _GOT_END_ = ABSOLUTE (.); *(.dynamic) *(.sdata*) *(.sbss*) *(.eh_frame) } > ram __rom_data_start = LOADADDR (.data); . = ALIGN (8); __ram_data_end = .; PROVIDE (__ram_data_end = .); _edata = .; PROVIDE (edata = .);
    .bss ALIGN (0x8) (NOLOAD) : { __bss_start = ABSOLUTE (.); *(.scommon) *(.dynbss) *(.bss) *(.bss.*) *(COMMON) __bss_end = ABSOLUTE (.); } > ram
    __heap1 = ALIGN (0x8);
    . = ALIGN(8); _end = .; PROVIDE (end = .); .debug 0 : { *(.debug) } .line 0 : { *(.line) } .debug_srcinfo 0 : { *(.debug_srcinfo) } .debug_sfnames 0 : { *(.debug_sfnames) } .debug_aranges 0 : { *(.debug_aranges) } .debug_pubnames 0 : { *(.debug_pubnames) } .debug_info 0 : { *(.debug_info) } .debug_abbrev 0 : { *(.debug_abbrev) } .debug_line 0 : { *(.debug_line) } .debug_frame 0 : { *(.debug_frame) } .debug_str 0 : { *(.debug_str) } .debug_loc 0 : { *(.debug_loc) } .debug_macinfo 0 : { *(.debug_macinfo) } .debug_weaknames 0 : { *(.debug_weaknames) } .debug_funcnames 0 : { *(.debug_funcnames) } .debug_typenames 0 : { *(.debug_typenames) } .debug_varnames 0 : { *(.debug_varnames) }
}
