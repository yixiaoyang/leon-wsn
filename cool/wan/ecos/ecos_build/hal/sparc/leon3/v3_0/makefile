# eCos makefile

# This is a generated file - do not edit

export PREFIX := /home/water/workspace/WSN-Audio/Software/fpga/wan/ecos/ecos_install
export COMMAND_PREFIX := sparc-elf-
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export HOST := UNIX
export AR := $(COMMAND_PREFIX)ar

export REPOSITORY := /home/water/workspace/WSN-Audio/Software/fpga/ecos/ecos-3.0/packages
PACKAGE := hal/sparc/leon3/v3_0
OBJECT_PREFIX := hal_sparc_leon3
CFLAGS := -msoft-float -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc
LDFLAGS := -msoft-float -g -nostdlib -fvtable-gc -Wl,--gc-sections -Wl,-static
VPATH := $(REPOSITORY)/$(PACKAGE)
INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir) -I$(dir)/src -I$(dir)/tests) -I.
MLT := $(wildcard $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.ldi $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.h)
TESTS := 

build: headers libtarget.a.stamp

LIBRARY := libtarget.a
COMPILE := src/hal_priv.c src/boot.S src/hal_amba.c src/hal_amba_driver.c src/hal_diag.c src/hal_clock.c src/hal_irq.c src/leon3_smp.c src/boot_smp.S src/hal_help.cxx
OBJECTS := $(COMPILE:.cxx=.o.d)
OBJECTS := $(OBJECTS:.cpp=.o.d)
OBJECTS := $(OBJECTS:.c=.o.d)
OBJECTS := $(OBJECTS:.S=.o.d)

$(LIBRARY).stamp: $(OBJECTS)
	$(AR) rcs $(PREFIX)/lib/$(@:.stamp=) $(foreach obj,$?,$(if $(obj:%.o=),$(dir $(obj))$(OBJECT_PREFIX)_$(notdir $(obj:.o.d=.o)),$(obj)))
	@cat $(foreach obj,$^,$(obj:.o=.o.d)) > $(@:.stamp=.deps)
	@touch $@

headers: mlt_headers $(PREFIX)/include/cyg/hal/leon3_arch.inc $(PREFIX)/include/cyg/hal/hal_leon3.h $(PREFIX)/include/cyg/hal/hal_diag.h $(PREFIX)/include/cyg/hal/hal_cache.h $(PREFIX)/include/cyg/hal/hal_xpic.h $(PREFIX)/include/cyg/hal/halboot.si $(PREFIX)/include/cyg/hal/hal_leon3_smp.h $(PREFIX)/include/cyg/hal/hal_amba.h $(PREFIX)/include/cyg/hal/hal_clock.h

$(PREFIX)/include/cyg/hal/leon3_arch.inc: $(REPOSITORY)/$(PACKAGE)/include/leon3_arch.inc
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_leon3.h: $(REPOSITORY)/$(PACKAGE)/include/hal_leon3.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_diag.h: $(REPOSITORY)/$(PACKAGE)/include/hal_diag.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_cache.h: $(REPOSITORY)/$(PACKAGE)/include/hal_cache.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_xpic.h: $(REPOSITORY)/$(PACKAGE)/include/hal_xpic.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/halboot.si: $(REPOSITORY)/$(PACKAGE)/include/halboot.si
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_leon3_smp.h: $(REPOSITORY)/$(PACKAGE)/include/hal_leon3_smp.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_amba.h: $(REPOSITORY)/$(PACKAGE)/include/hal_amba.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_clock.h: $(REPOSITORY)/$(PACKAGE)/include/hal_clock.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

include $(REPOSITORY)/pkgconf/rules.mak

