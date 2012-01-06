INCLUDES=./
all: ahbuart.o opmon.o ambapnp.o toolkit.o command.o test.o memory.o
	gcc -o opmon $^ -I$(INCLUDES)
%.o: %.c
	gcc -c $< -o $@
clean:
	rm -f *.o opmon
