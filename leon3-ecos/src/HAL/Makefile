
LIB	= hal.a
COBJS	= spi.o gpio.o apbuart.o tools.o serial.o cyg_spi.o


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
