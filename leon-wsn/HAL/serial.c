/* 
 * Written 1999-03-19 by Jonathan Larmour, Cygnus Solutions
 * This file is in the public domain and may be used for any purpose
 *
 */

/* CONFIGURATION CHECKS */

#include <pkgconf/system.h>     /* which packages are enabled/disabled */
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif
#ifdef CYGPKG_LIBC
# include <pkgconf/libc.h>
#endif
#ifdef CYGPKG_IO_SERIAL
# include <pkgconf/io_serial.h>
#endif

#ifndef CYGFUN_KERNEL_API_C
# error Kernel API must be enabled to build this example
#endif

#ifndef CYGPKG_LIBC_STDIO
# error C library standard I/O must be enabled to build this example
#endif

#ifndef CYGPKG_IO_SERIAL_HALDIAG
# error I/O HALDIAG pseudo-device driver must be enabled to build this example
#endif

/* INCLUDES */

#include <stdio.h>                      /* printf */
#include <string.h>                     /* strlen */
#include <cyg/kernel/kapi.h>            /* All the kernel specific stuff */
#include <cyg/io/io.h>                  /* I/O functions */
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/io/serialio.h>


/*
 * FUNCTIONS:serial testing
 * xiaoyang@2012-1-2
 */
void serial_test()
{
    cyg_io_handle_t handle;
    Cyg_ErrNo err;
    const char test_string[] = "serial example is working correctly!\n";
    cyg_uint32 len = strlen(test_string);
    char getch;

    char rcvstr[64]="";
    cyg_uint32 rcvlen = 8;
    cyg_uint32 res;
    cyg_uint32 res_len = 0;
    cyg_serial_buf_info_t serial_info;

    printf("Starting serial example\n");
    err = cyg_io_lookup( "/dev/haldiag", &handle );

    printf("lookup retur:%d \nread cfg info of serial...\n",err);
    res = cyg_io_get_config(handle,CYG_IO_GET_CONFIG_SERIAL_BUFFER_INFO, &serial_info,&res_len);
    if(res == ENOERR){
    	printf("get info succeed!\n");
    }else{
    	printf("get info failed!\n");
    }
    printf("rx_bufsize=%d,rx_count=%d\n",serial_info.rx_bufsize, serial_info.rx_count);

    if (ENOERR == err) {
        printf("Found /dev/haldiag. Writing string....\n");
        err = cyg_io_write( handle, test_string, &len );
        printf("\nI think I wrote the string. Did you see it?\n");
    }

    getch = 'n';
    rcvlen = 1;
    while(getch != 'y'){
    	printf("\ninput y please:");
    	err = cyg_io_read(handle, rcvstr, &rcvlen);
    	getch = rcvstr[0];
		if(err == -EINTR){
			printf("err:%d return!\n",err);
		}else{
			rcvstr[9]='\0';
		}
    }

    printf("\ninput 12345678 please:");
    rcvlen = 8;
    err = cyg_io_read(handle, rcvstr, &rcvlen);
    if(err == -EINTR){
    	printf("err:%d return!\n",err);
    }else{
        rcvstr[9]='\0';
        printf("read str:%s\n",rcvstr);
    }
    printf("Serial example finished\n");
}
