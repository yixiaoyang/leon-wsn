
LIB	= stack.a
COBJS	= netif.o translate.o 

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
