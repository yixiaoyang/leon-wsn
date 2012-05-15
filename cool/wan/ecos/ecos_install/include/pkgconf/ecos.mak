ECOS_GLOBAL_CFLAGS = -msoft-float -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc
ECOS_GLOBAL_LDFLAGS = -msoft-float -g -nostdlib -fvtable-gc -Wl,--gc-sections -Wl,-static
ECOS_COMMAND_PREFIX = sparc-elf-
