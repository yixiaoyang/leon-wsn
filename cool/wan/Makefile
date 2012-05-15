
# Usage:   make INSTALL_DIR=/path/to/ecos/install

# XTOOLS
ifdef INSTALL_DIR
INSTALL_DIR=$$(INSTALL_DIR)
else
INSTALL_DIR=./ecos/ecos_install
endif
include $(INSTALL_DIR)/include/pkgconf/ecos.mak

# Project
PROJECT=wan
TARGET=

TOPDIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
OUTPUT_PATH=build
SRC_PATH=common hal testcases wireless
INCLUDES=-Icommon -Ihal -Itestcases -Iwireless

SRCS=$(foreach d, $(SRC_PATH), $(wildcard $(d)/*.c))  
SRCS+=main.c
#SRCS+=alt/alt_devs_spi_leon3.c
#SRCS+=alt/twothreads.c
#SRCS+=alt/alt_uz2400d.c
OBJS=$(patsubst %.c, $(OUTPUT_PATH)/%.o, $(notdir $(SRCS)))
vpath %.c $(SRC_PATH) alt/
vpath %.o $(OUTPUT_PATH)

XCC           = $(ECOS_COMMAND_PREFIX)gcc
XCXX          = $(XCC)
XLD           = $(XCC)
XOBJCOPY			= $(ECOS_COMMAND_PREFIX)objcopy
XOBJDUMP			= $(ECOS_COMMAND_PREFIX)objdump

CFLAGS        = -I$(INSTALL_DIR)/include $(INCLUDES)
CFLAGS       += -D TRANSMITTER
#CFLAGS       += -D RECEIVER
CXXFLAGS      = $(CFLAGS) -Woverloaded-virtual -fno-rtti -fvtable-gc -finit-priority
LDFLAGS       = -nostartfiles -nostdlib -L$(INSTALL_DIR)/lib -Ttarget.ld
# RULES

.PHONY: all clean

all: $(PROJECT).elf $(OBJS)
	$(XOBJCOPY) -O srec $< $(PROJECT).srec	
	$(XOBJDUMP) -d $< > $(PROJECT).dasm
clean:
	-rm -f *.o *.elf *.srec *.dasm
	-rm -f $(OUTPUT_PATH)/*.o

$(OUTPUT_PATH)/%.o: %.c
	$(XCC) -c -o $@ $(CFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cxx
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.C
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cc
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

$(PROJECT).elf: $(OBJS)
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $^
