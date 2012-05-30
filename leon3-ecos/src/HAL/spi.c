/*
 * spi.c
 *
 *  Created on: 2011-2-22
 *      Author: LiangShuang
 */
#include "fpga.h"
#include "spi.h"
#include "types.h"
#include "gpio.h"

/* 定义 SPI 线程的句柄*/
static cyg_io_handle_t hDrvSPI;
static cyg_uint32 _spi_blen = 1;
static cyg_uint8 _spi_buf[2] = {0x0,0x0};

/* Reverse data*/
#define REV	1

/******************************************************************
 * Initialize the SPI Controller
 *
 *****************************************************************/
int8u_t spi_init(void)
{
    if (ENOERR != cyg_io_lookup("/dev/spi", &hDrvSPI))
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/spi");
        return -1;
    }
 
    printf("Open /dev/spi OK\n");
    return 1;
}
 
/********************************************************************
 * SPI TX and RX
 *
 *******************************************************************/
inline int8u_t spi_wbyte(int8u_t val)
{
    _spi_blen = 1;
    _spi_buf[1] = val;
    if(ENOERR != cyg_io_write(hDrvSPI, _spi_buf+1, &_spi_blen))
    {
        CYG_TEST_FINISH("Error write /dev/spi");
    }
}
 
inline int8u_t spi_rbyte(int8u_t val)
{
    _spi_blen = 1;
    if(ENOERR != cyg_io_read(hDrvSPI, (cyg_uint8*)(&val), &_spi_blen))
    {
        CYG_TEST_FINISH("Error Read /dev/spi");
    }
    return val;
}
 
inline int8u_t spi_wbuf(int8u_t* buf, int32u_t len)
{
    if(ENOERR != cyg_io_write(hDrvSPI, (cyg_uint8*)(buf), (cyg_uint32*)(&len) ))
    {
        CYG_TEST_FINISH("Error write /dev/spi");
        return -1;
    }
 
    return 1;
}
 
/*******************************************************************
 * Reconfigure SPI Mode Register
 *
 ******************************************************************/
int8s_t spi_reconfig(int32u_t mask, int32u_t val)
{
    /*
    int32u_t mod;
    if(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_TIP) {
    	return -1;
    }
    // Disable the SPI core
    REG32(SPI_BASE+SPI_MOD) &= (~SPI_MOD_EN);

    mod = REG32(SPI_BASE+SPI_MOD);
    mod = (mod & ~mask) | val;
    REG32(SPI_BASE+SPI_MOD) = mod;

    REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_EN;
    */
    return 0;
}
 
 
//---------------------------------------------------------------------------
// 	below is code for spi2 of vs1003
// 	add @2011.4.27
//---------------------------------------------------------------------------
int16u_t spi2_delay_time = SPI2_HIGH;  //adjust speed of spi2,low value,higher speed
 
void spi2_delay(int16u_t ch)
{
    while(ch-- > 0)
        ;
}
 
//---------------------------------------------------------------------------
// 	interface of initialization,spi2 for vs1003
// 	add @2011.4.27
//---------------------------------------------------------------------------
void spi2_init(void)
{
    /* set GPIO pins,0=input, 1=output*/
    gpio_make_out( PORTB,SPI2_CLK );
    gpio_make_out( PORTB,SPI2_MOSI);
    gpio_make_in ( PORTB,SPI2_MISO);
    spi2_delay_time = SPI2_LOW;
    //SPI2_SELECT();
}
 
//---------------------------------------------------------------------------
// 	pull up speed of spi2 for vs1003
// 	add @2011.4.27
//---------------------------------------------------------------------------
void spi2_high_speed_mode(void)
{
    spi2_delay_time = SPI2_HIGH;
}
 
void spi2_low_speed_mode(void)
{
    spi2_delay_time = SPI2_LOW;
}
//---------------------------------------------------------------------------
// 	Transmit and receive through SPI port
// 	Transfer 8 bits each time
//---------------------------------------------------------------------------
int8u_t spi2_8TxRx(int8u_t byte)
{
    int8s_t bit;
 
    //SPI2_SELECT();
    for (bit = 0; bit < 8; bit++)
    {
        /* write MOSI on trailing edge of previous clock */
        if (byte & 0x80)
        {
            SETMOSI();
        }
        else
        {
            CLRMOSI();
        }
        byte <<= 1;
 
        /* half a clock cycle before leading/rising edge */
        //spi2_delay(spi2_delay_time);
        SETCLK();
 
        /* half a clock cycle before trailing/falling edge */
        //spi2_delay(spi2_delay_time);
 
        /* read MISO on trailing edge */
        byte |= READMISO();
        CLRCLK();
    }
 
    //SPI2_DESELECT();
    return byte;
}
 
//----------------------------------------------------------------------
// test code...
//----------------------------------------------------------------------
void spi2_PmodCLS_SendString(char* str)
{
    //SPI3_SELECT();
    while(*str)
    {
        spi2_8TxRx(*str++);
    }
 
    //SPI3_DESELECT();
}
 
void spi2_PmodCLS_SendCmd(char* cmd)
{
    //SPI3_SELECT();
    spi2_8TxRx(0x1B);
    spi2_8TxRx('[');
    while(*cmd)
    {
        spi2_8TxRx(*cmd++);
    }
 
    //SPI3_DESELECT();
 
}
 
void spi2_PmodCLS_init(void)
{
    spi2_init();
    spi2_PmodCLS_SendCmd("0j");				// Clr disply and home cursor
    spi2_PmodCLS_SendCmd("3e");			// Enable display and backlight
    spi2_PmodCLS_SendCmd("2c");			// Set Cursor Blink Mode
}
 
void spi2_test(void)
{
    spi2_PmodCLS_init();
    spi2_PmodCLS_SendString("hello,leon3!");
}
 
