#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include "toolkit.h"
/* display memory command */
int mem_cmd(int argc, char* argv[])
{
	unsigned long len_by_char=0x40;
	unsigned start_addr;
  switch(argc) {
		case 2:
      len_by_char = (strtoul(argv[1], 0, 0)+0xF) & ~0xF; // align(16)
		case 1:
		  start_addr = strtoul(argv[0], 0, 0)&~0x3; // align by word
			break;
		default:
		  return -1;
	}

	printf("Display memory at %08x len: %lu", start_addr, len_by_char);
  int i;
	unsigned long word;
	for(i=0; i<len_by_char/4; i++) {
    ReadWord(start_addr+i*4, &word);
		if(i%4==0) {
			printf("\n%08x", start_addr+i*4);
		}
		printf(" %08x", SWAP32(word));
	}
	//printf("\n");
	return 0;
}
/* Write memory command */
// TODO: Write to memory address not aligned with word
int wmem_cmd(int argc, char* argv[])
{
	unsigned long addr, value;
	switch(argc) {
		case 2:
		addr = strtoul(argv[0], 0, 0);
		value = strtoul(argv[1], 0, 0);
		break;
		default:
		return -1;
	}
	printf("Write word %#x at %08x\n", value, addr);
  WriteWord(addr, value);
	return 0;
}


void memory_init()
{
	register_command("mem", "<start addr> [<lenght by char>]", "disply 32-bit memory locations", mem_cmd);
	register_command("wmem", "<address> <32-bit value>", " write word to memory", wmem_cmd);
}
