# eCos makefile

# This is a generated file - do not edit

export PREFIX := /home/water/workspace/WSN-Audio/Software/fpga/wan/ecos/ecos_install
export COMMAND_PREFIX := sparc-elf-
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export HOST := UNIX
export AR := $(COMMAND_PREFIX)ar

export REPOSITORY := /home/water/workspace/WSN-Audio/Software/fpga/ecos/ecos-3.0/packages
PACKAGE := hal/sparc/arch/v3_0
OBJECT_PREFIX := hal_sparc_arch
CFLAGS := -msoft-float -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc
LDFLAGS := -msoft-float -g -nostdlib -fvtable-gc -Wl,--gc-sections -Wl,-static
VPATH := $(REPOSITORY)/$(PACKAGE)
INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir) -I$(dir)/src -I$(dir)/tests) -I.
MLT := $(wildcard $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.ldi $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.h)
TESTS := tests/sparc_ex

build: headers libtarget.a.stamp

LIBRARY := libtarget.a
COMPILE := src/vectors.S src/vec_ivsr.S src/vec_xvsr.S src/context.S src/icontext.c src/hal_intr.c src/hal_boot.c src/callcons.S
OBJECTS := $(COMPILE:.cxx=.o.d)
OBJECTS := $(OBJECTS:.cpp=.o.d)
OBJECTS := $(OBJECTS:.c=.o.d)
OBJECTS := $(OBJECTS:.S=.o.d)

$(LIBRARY).stamp: $(OBJECTS)
	$(AR) rcs $(PREFIX)/lib/$(@:.stamp=) $(foreach obj,$?,$(if $(obj:%.o=),$(dir $(obj))$(OBJECT_PREFIX)_$(notdir $(obj:.o.d=.o)),$(obj)))
	@cat $(foreach obj,$^,$(obj:.o=.o.d)) > $(@:.stamp=.deps)
	@touch $@

$(PREFIX)/lib/vectors.o: $(wildcard $(REPOSITORY)/$(PACKAGE)/src/vectors.S)
	$(CC) -Wp,-MD,vectors.tmp $(INCLUDE_PATH) $(CFLAGS) -c -o $@ $<
	@echo $@ ": \\" > $(notdir $@).deps
	@tail -n +2 vectors.tmp >> $(notdir $@).deps
	@echo >> $(notdir $@).deps
	@rm vectors.tmp
	

$(PREFIX)/lib/target.ld: $(wildcard $(REPOSITORY)/$(PACKAGE)/src/sparc.ld)
	$(CC) -E -P -Wp,-MD,target.tmp -DEXTRAS=1 -xc $(INCLUDE_PATH) $(CFLAGS) -o $@ $<
	@echo $@ ": \\" > $(notdir $@).deps
	@tail -n +2 target.tmp >> $(notdir $@).deps
	@echo >> $(notdir $@).deps
	@rm target.tmp
	

headers: mlt_headers $(PREFIX)/include/cyg/hal/vectors.h.orig $(PREFIX)/include/cyg/hal/hal_io.h $(PREFIX)/include/cyg/hal/hal_intr.h.orig $(PREFIX)/include/cyg/hal/hal_arch.h $(PREFIX)/include/cyg/hal/hal_intr.h $(PREFIX)/include/cyg/hal/basetype.h $(PREFIX)/include/cyg/hal/arch.inc $(PREFIX)/include/cyg/hal/hal_smp.h $(PREFIX)/include/cyg/hal/hal_arch.h.orig $(PREFIX)/include/cyg/hal/vectors.h

$(PREFIX)/include/cyg/hal/vectors.h.orig: $(REPOSITORY)/$(PACKAGE)/include/vectors.h.orig
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_io.h: $(REPOSITORY)/$(PACKAGE)/include/hal_io.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_intr.h.orig: $(REPOSITORY)/$(PACKAGE)/include/hal_intr.h.orig
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_arch.h: $(REPOSITORY)/$(PACKAGE)/include/hal_arch.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_intr.h: $(REPOSITORY)/$(PACKAGE)/include/hal_intr.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/basetype.h: $(REPOSITORY)/$(PACKAGE)/include/basetype.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/arch.inc: $(REPOSITORY)/$(PACKAGE)/include/arch.inc
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_smp.h: $(REPOSITORY)/$(PACKAGE)/include/hal_smp.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_arch.h.orig: $(REPOSITORY)/$(PACKAGE)/include/hal_arch.h.orig
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/vectors.h: $(REPOSITORY)/$(PACKAGE)/include/vectors.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

include $(REPOSITORY)/pkgconf/rules.mak

