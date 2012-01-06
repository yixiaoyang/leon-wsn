#include <stdio.h>
#include "command.h"
typedef union {
	    unsigned long my_int;
			unsigned char  my_bytes[4];
} endian_tester;
int test_endian(int argc, char* argv[]) 
{
	/*
	int i;
	for (i=0; i<argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
	*/
	endian_tester et;
	et.my_int = 0x0a0b0c0d;
	if(et.my_bytes[0] == 0x0a )
		printf( "I'm on a big-endian system\n" );
	else
		printf( "I'm on a little-endian system\n" );
	 return 0; 	
}
void test_init()
{
  register_command("test", "", "Test the endian of host platform", test_endian);
}
