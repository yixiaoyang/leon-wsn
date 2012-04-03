#
# xiaoyang.yi 2012-3-20
# linux driver for LRF020(uz2440d core)
#

ARCH=arm-linux-
CC=$(ARCH)gcc
DUMP=$(ARCH)objdump
PWD:=$(shell pwd)
KERNELDIR=/opt/linux/EmbedSky/linux-2.6.30.4
INSTALLDIR=./
INC=$(KERNELDIR)/include #$(PWD)/*.h
MOD_NAME=lrf020
DEBUG=y

#obj-m	+=	rf.o
#obj-m	:=	uz2400d.0
obj-m 	:=	$(MOD_NAME).o
lrf020-objs	:=	rf.o uz2400d.o spidev.o

modules:
	$(shell make clean)
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules -I $(INC)
	@echo "compiled ok!"
uz2400d.o:uz2400d.c
	$(CC) uz2400d.c -I $(INC)
rf.o:rf.c
	$(CC) rf.c -I $(INC)
spidev.o:spidev.c
	$(CC) spidev.c -I $(INC)

dump:
	$(DUMP) -d -S $(MOD_NAME).ko > $(MOD_NAME).dump

debug:
	cat /var/log/messages | tail
result:
	cat /proc/devices
help:
	@echo "install with 'mknod /dev/devname c dev_num 0' cmd"
clean:
	rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers *.dump
