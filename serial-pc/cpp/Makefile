#
# xiaoyang 2012-1-3
# serial program for pc 
# 

SRCS =	main.cpp datatrans.cpp 
HDRS = 	datatrans.h
OBJS =	main.o datatrans.o
CC = g++
CFLAGS =  -g -O 
#CFLAGS =  -g -Wall -ansi -pedantic 

# If you want to compile with -Wall -pedantic -ansi on a Sun, use 
# -DBROKEN_SUN_HEADERS to avoid a lot of compiler warnings


PROGRAM = pc

all: $(PROGRAM)

$(PROGRAM):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM)  -lm

main.o:		main.cpp datatrans.h
datatrans.o:		datatrans.h datatrans.cpp

clean:		
		rm -f *.o *~ $(PROGRAM)

tar:		$(SRCS) $(HDRS) 
		tar cvf serial-pc.tar $(SRCS) $(HDRS) 
		gzip serial-pc.tar
		mv serial-pc.tar.gz serial-pc.tgz

