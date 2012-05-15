# eCos makefile

# This is a generated file - do not edit

export PREFIX := /home/water/workspace/WSN-Audio/Software/fpga/wan/ecos/ecos_install
export COMMAND_PREFIX := sparc-elf-
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export HOST := UNIX
export AR := $(COMMAND_PREFIX)ar

export REPOSITORY := /home/water/workspace/WSN-Audio/Software/fpga/ecos/ecos-3.0/packages
PACKAGE := language/c/libc/common/v3_0
OBJECT_PREFIX := language_c_libc_common
CFLAGS := -msoft-float -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc
LDFLAGS := -msoft-float -g -nostdlib -fvtable-gc -Wl,--gc-sections -Wl,-static
VPATH := $(REPOSITORY)/$(PACKAGE)
INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir) -I$(dir)/src -I$(dir)/tests) -I.
MLT := $(wildcard $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.ldi $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.h)
TESTS := 

build: headers

headers: mlt_headers

include $(REPOSITORY)/pkgconf/rules.mak

