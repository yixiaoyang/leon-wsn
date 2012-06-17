#
# Makefile for leon3_ecos_install project 
#
# log:
# 	2012-5-6 create
#

# Usage:   make INSTALL_DIR=/path/to/ecos/install
INSTALL_DIR=/opt/cvs/ecos/ecos-3.0/spi_test_install
#INSTALL_DIR=./spi_test_install

# Cross compilers
SPARC_PREFIX=sparc-elf-
CC=$(SPARC_PREFIX)gcc
LD=$(CC)
OBJCOPY=$(SPARC_PREFIX)objcopy
OBJDUMP=$(SPARC_PREFIX)objdump
AR=$(SPARC_PREFIX)ar
RANLIB=$(SPARC_PREFIX)ranlib
MKPROM=mkprom2

export CC LD OBJCOPY OBJDUMP AR RANLIB

# Project related settings
TOPDIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
OUTPUTDIR=$(TOPDIR)/objs
export	TOPDIR

PRJNAME = leon3

# Make Flags
SILENT= 
CFLAGS=-msoft-float -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Woverloaded-virtual -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc  -I$(TOPDIR)/includes  -I$(INSTALL_DIR)/include

LDFLAGS=-msoft-float -g -nostdlib -fvtable-gc -Wl,--gc-sections -Wl,-static -nostartfiles -L$(INSTALL_DIR)/lib -Ttarget.ld
PROM_FLAG=-msoft-float -freq 50 -rmw -romwidth 8 -romws 3

#TOPLDFLAGS = $(CFLAGS) -Wl,-static,-Ttext=0x40000000
export CFLAGS LDFLAGS

# The order is important!
SUBDIRS = App Adpcm Stack Stack Tools Wireless Video Fat32 BSP Misc HAL
LIBS = App/application.a Adpcm/adpcm.a Stack/stack.a Stack/stack.a Tools/tools.a Wireless/wireless.a Video/video.a Fat32/fat32.a  BSP/bsp.a Misc/misc.a HAL/hal.a 

#others

all: $(PRJNAME).elf $(PRJNAME).dis $(PRJNAME).srec
	$(SILENT)@echo "End compiling~~~"
	
$(PRJNAME).elf: depend subdirs lemon.o $(LIBS)
	$(SILENT)$(LD) $(LDFLAGS)  -o $@ lemon.o  $(LIBS)
	$(SILENT)@echo "elf OK..."
	
$(PRJNAME).srec: $(PRJNAME).elf
	$(SILENT)$(OBJCOPY) -O srec $< $@
	$(SILENT)@echo ".srec OK..."
	
$(PRJNAME).dis: $(PRJNAME).elf
	$(OBJDUMP) -D -x -s $< > $@
	
%.o:%.c
	$(SILENT)$(CC) -c $(CFLAGS) $< -o $@
	
depend dep:
	@for dir in $(SUBDIRS) ; do make -C $$dir .depend ; done

subdirs:
	@for dir in $(SUBDIRS) ; do make -C $$dir || exit 1 ; done
	
clean: subclean
	$(SILENT)rm -f *.o
	$(SILENT)rm -f $(PRJNAME).elf
	$(SILENT)rm -f $(PRJNAME).srec
	$(SILENT)rm -f xdump.s tmp.out
	$(SILENT)rm -f $(PRJNAME).dis
	$(SILENT)echo "clean OK..."
subclean:
	@for dir in $(SUBDIRS) ; do make -C $$dir clean ; done
	
lemon-move: $(PRJNAME).srec $(PRJNAME).elf
	@rm -f $(DESIGN_DIR)/$(PRJNAME).srec
	@rm -f $(DESIGN_DIR)/$(PRJNAME).elf
	cp $^ $(DESIGN_DIR)/ 
	@echo "move file to design"

