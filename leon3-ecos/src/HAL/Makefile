
LIB	= hal.a
COBJS	= spi.o gpio.o apbuart.o serial.o sccb.o i2cmst.o ck_gen.o intr.o


all:	$(LIB)
$(LIB):	$(COBJS)
	#$(LD) $(LDFLAGS) -r -o $@ $(COBJS)
	$(AR) crv $@ $(COBJS)
	$(RANLIB) $@
clean:
	@rm -f *.o
	@rm -f *.a
	@rm -f .depend
	
.depend:	Makefile $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(COBJS:.o=.c) > $@
sinclude .depend
