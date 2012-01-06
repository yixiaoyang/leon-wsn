
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ahbuart.h"
#include "ambapnp.h"

//#include "toolkit.h"
void aboutopmon()
{
	printf("\n");
	printf("OPDK LEON debug monitor\n");
	printf("Copyright (C) 2011-2012 Harbin Institute of Tech - all rights reserved.\n");
	printf("Comments or bug-reports to co.liang.ol@gmail.com\n");
	printf("\n");
}
int main(int argc, char* argv[])
{
	aboutopmon();
	/* Command line options */
	char portname[20]="";
	unsigned long baud = 0;
	argc--; argv++;
	while(argc>0) {
		if(**argv=='-') {
			(*argv)++;
			if(strcmp(*argv, "uart")==0) {
				/* uart port definition */
				argv++; argc--;
				strcpy(portname, *argv);
				//printf("uart port: %s\n", portname);
			}
			else if(strcmp(*argv, "baud")==0){
				/* baud rate */
				argv++; argc--;
        baud = strtoul(*argv, NULL, 10);
				//printf("Baud: %lu\n", baud);
			}
		}else {
			printf("%s\n", *argv);
		}
		argc--;
		argv++;
	}
	connect_dcom(portname, baud);	
	/* AMBA Configuraion */
	amba_probe();
	/* command monitor */
	test_init();
	memory_init();
  while(mon_commands()==0);
	close(uartFd);
	return 0;
}
