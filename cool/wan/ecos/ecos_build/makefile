# eCos makefile

# This is a generated file - do not edit

export HOST := UNIX
export PREFIX := /home/water/workspace/WSN-Audio/Software/fpga/wan/ecos/ecos_install
export COMMAND_PREFIX := sparc-elf-
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export AR := $(COMMAND_PREFIX)ar

.PHONY: default build clean tests headers

build: headers $(PREFIX)/include/pkgconf/ecos.mak
	$(MAKE) -r -C services/memalloc/common/v3_0 heapgeninc.tcl
	$(MAKE) -r -C services/memalloc/common/v3_0 heaps.cxx
	$(MAKE) -r -C hal/sparc/arch/v3_0 $@
	$(MAKE) -r -C hal/sparc/leon3/v3_0 $@
	$(MAKE) -r -C io/spi/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/leon3/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/opdk/v3_0 $@
	$(MAKE) -r -C hal/common/v3_0 $@
	$(MAKE) -r -C io/common/v3_0 $@
	$(MAKE) -r -C io/serial/v3_0 $@
	$(MAKE) -r -C infra/v3_0 $@
	$(MAKE) -r -C kernel/v3_0 $@
	$(MAKE) -r -C services/memalloc/common/v3_0 $@
	$(MAKE) -r -C isoinfra/v3_0 $@
	$(MAKE) -r -C language/c/libc/common/v3_0 $@
	$(MAKE) -r -C language/c/libc/i18n/v3_0 $@
	$(MAKE) -r -C language/c/libc/setjmp/v3_0 $@
	$(MAKE) -r -C language/c/libc/signals/v3_0 $@
	$(MAKE) -r -C language/c/libc/startup/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdio/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdlib/v3_0 $@
	$(MAKE) -r -C language/c/libc/string/v3_0 $@
	$(MAKE) -r -C language/c/libc/time/v3_0 $@
	$(MAKE) -r -C language/c/libm/v3_0 $@
	$(MAKE) -r -C io/wallclock/v3_0 $@
	$(MAKE) -r -C error/v3_0 $@
	$(MAKE) -r -C hal/common/v3_0 $(PREFIX)/lib/extras.o
	$(MAKE) -r -C hal/sparc/arch/v3_0 $(PREFIX)/lib/vectors.o
	$(MAKE) -r -C hal/sparc/arch/v3_0 $(PREFIX)/lib/target.ld
	@echo $@ finished

clean:
	$(MAKE) -r -C hal/sparc/arch/v3_0 $@
	$(MAKE) -r -C hal/sparc/leon3/v3_0 $@
	$(MAKE) -r -C io/spi/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/leon3/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/opdk/v3_0 $@
	$(MAKE) -r -C hal/common/v3_0 $@
	$(MAKE) -r -C io/common/v3_0 $@
	$(MAKE) -r -C io/serial/v3_0 $@
	$(MAKE) -r -C infra/v3_0 $@
	$(MAKE) -r -C kernel/v3_0 $@
	$(MAKE) -r -C services/memalloc/common/v3_0 $@
	$(MAKE) -r -C isoinfra/v3_0 $@
	$(MAKE) -r -C language/c/libc/common/v3_0 $@
	$(MAKE) -r -C language/c/libc/i18n/v3_0 $@
	$(MAKE) -r -C language/c/libc/setjmp/v3_0 $@
	$(MAKE) -r -C language/c/libc/signals/v3_0 $@
	$(MAKE) -r -C language/c/libc/startup/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdio/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdlib/v3_0 $@
	$(MAKE) -r -C language/c/libc/string/v3_0 $@
	$(MAKE) -r -C language/c/libc/time/v3_0 $@
	$(MAKE) -r -C language/c/libm/v3_0 $@
	$(MAKE) -r -C io/wallclock/v3_0 $@
	$(MAKE) -r -C error/v3_0 $@
	@echo $@ finished

tests: build
	$(MAKE) -r -C hal/sparc/arch/v3_0 $@
	$(MAKE) -r -C hal/sparc/leon3/v3_0 $@
	$(MAKE) -r -C io/spi/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/leon3/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/opdk/v3_0 $@
	$(MAKE) -r -C hal/common/v3_0 $@
	$(MAKE) -r -C io/common/v3_0 $@
	$(MAKE) -r -C io/serial/v3_0 $@
	$(MAKE) -r -C infra/v3_0 $@
	$(MAKE) -r -C kernel/v3_0 $@
	$(MAKE) -r -C services/memalloc/common/v3_0 $@
	$(MAKE) -r -C isoinfra/v3_0 $@
	$(MAKE) -r -C language/c/libc/common/v3_0 $@
	$(MAKE) -r -C language/c/libc/i18n/v3_0 $@
	$(MAKE) -r -C language/c/libc/setjmp/v3_0 $@
	$(MAKE) -r -C language/c/libc/signals/v3_0 $@
	$(MAKE) -r -C language/c/libc/startup/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdio/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdlib/v3_0 $@
	$(MAKE) -r -C language/c/libc/string/v3_0 $@
	$(MAKE) -r -C language/c/libc/time/v3_0 $@
	$(MAKE) -r -C language/c/libm/v3_0 $@
	$(MAKE) -r -C io/wallclock/v3_0 $@
	$(MAKE) -r -C error/v3_0 $@
	@echo $@ finished

headers:
	$(MAKE) -r -C hal/sparc/arch/v3_0 $@
	$(MAKE) -r -C hal/sparc/leon3/v3_0 $@
	$(MAKE) -r -C io/spi/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/leon3/v3_0 $@
	$(MAKE) -r -C devs/spi/sparc/opdk/v3_0 $@
	$(MAKE) -r -C hal/common/v3_0 $@
	$(MAKE) -r -C io/common/v3_0 $@
	$(MAKE) -r -C io/serial/v3_0 $@
	$(MAKE) -r -C infra/v3_0 $@
	$(MAKE) -r -C kernel/v3_0 $@
	$(MAKE) -r -C services/memalloc/common/v3_0 $@
	$(MAKE) -r -C isoinfra/v3_0 $@
	$(MAKE) -r -C language/c/libc/common/v3_0 $@
	$(MAKE) -r -C language/c/libc/i18n/v3_0 $@
	$(MAKE) -r -C language/c/libc/setjmp/v3_0 $@
	$(MAKE) -r -C language/c/libc/signals/v3_0 $@
	$(MAKE) -r -C language/c/libc/startup/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdio/v3_0 $@
	$(MAKE) -r -C language/c/libc/stdlib/v3_0 $@
	$(MAKE) -r -C language/c/libc/string/v3_0 $@
	$(MAKE) -r -C language/c/libc/time/v3_0 $@
	$(MAKE) -r -C language/c/libm/v3_0 $@
	$(MAKE) -r -C io/wallclock/v3_0 $@
	$(MAKE) -r -C error/v3_0 $@
	@echo $@ finished

$(PREFIX)/include/pkgconf/ecos.mak: makefile
	@echo 'ECOS_GLOBAL_CFLAGS = -msoft-float -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc' > $@
	@echo 'ECOS_GLOBAL_LDFLAGS = -msoft-float -g -nostdlib -fvtable-gc -Wl,--gc-sections -Wl,-static' >> $@
	@echo 'ECOS_COMMAND_PREFIX = $(COMMAND_PREFIX)' >> $@

