/*
 * spi.c
 *
 *  Created on: 2011-2-22
 *      Author: hgdytz
 */
#include "fpga.h"
#include "spi.h"
#include "types.h"
#include "gpio.h"

/*
int8u_t reverse8(int8u_t data)
{
	int8u_t rdata = 0x00, i=0;
	for(;i<8;i++) {
		rdata |=( (data << i & 0x80) >> (7-i) );
	}
	return rdata;

}
*/


#define REV	1		/* Reverse data*/
/******************************************************************
 * Initialize the SPI Controller
 *
 *****************************************************************/
void spi_init(void) {
	REG32(SPI_BASE+SPI_CAP) = 0x01018002;
	/* Configure the capability register */
				/* SPI_CAP_FDEPTH = 0x80 => Support FIFO size = 128 */
				/* SPI_CAP_SSEN = 1 */
				/* SPI_CAP_MAXWLEN = 0x0 => Support 32-bit length */
				/* SPI_CAP_SSSZ = 0x01 */
#if REV
	REG32(SPI_BASE+SPI_MOD) |= 0x06740000;
	/* Configure the mode register */
									/* SPI_MOD_FACT = 0*/
									/* SPI_MOD_PM = 4 */
									/* SPI_MOD_LEN = 7 */
									/* SPI_MOD_EN = 0 */
			 	 	 	 	 	 	/* SPI_MOD_MS = 1 */
									/* SPI_MOD_REV = 0 */
									/* SPI_MOD_DIV16 = 0 */
									/* SPI_MOD_CPHA = 0 */
									/* SPI_MOD_CPOL = 0 */
									/* SPI_MOD_REV = 1 */ /* MSB transmitted first */
	// REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_CPOL;
	// REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_CPHA;
#else //SPI_MOD_REV = 0, LSB transmitted first
	REG32(SPI_BASE+SPI_MOD) |= 0x02740000;
#endif // REV
	REG32(SPI_BASE+SPI_MSK) = 0x0;	/* Disable all interrupts */
	REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_EN;		/* Activate Core */

}
/********************************************************************
 * SPI TX and RX
 *
 *******************************************************************/
int8u_t spi_xmit8(int8u_t val) {
	int32u_t reg32;
  /* Wait for room */
	while(!(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_NF));
#if REV
	REG32(SPI_BASE+SPI_TX) = val<<24&0xFF000000;
#else
	REG32(SPI_BASE+SPI_TX) = val;
#endif // REV
	//while(!(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_NE));		/* Wait for transmitting over */
	//while(!((REG32(SPI_BASE+SPI_EVE)&SPI_EVE_LT)));
	while((REG32(SPI_BASE+SPI_EVE)&SPI_EVE_TIP));
	reg32 = REG32(SPI_BASE+SPI_RX);
	//printf("1st: 0x%x\n",reg32);
#if REV
	return (int8u_t)((reg32>>16)&0xFF);
#else
	return (int8u_t)((reg32>>8)&0xFF);
#endif // REV
}
int16u_t spi_xmit16(int16u_t val)
{
	while(!(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_NF));
	#if REV
		REG32(SPI_BASE+SPI_TX) = val<<16&0xFFFF0000;
	#else
		REG32(SPI_BASE+SPI_TX) = val;
	#endif // REV
	while((REG32(SPI_BASE+SPI_EVE)&SPI_EVE_TIP));
	int32u_t reg32 = REG32(SPI_BASE+SPI_RX);
	//printf("back32: 0x%8x\n",reg32);
	#if REV
		return (int16u_t)((reg32>>16)&0xFFFF);
	#else
		return (int16u_t)(reg32&0xFFFF);
	#endif // REV
}
/*******************************************************************
 * Reconfigure SPI Mode Register
 *
 ******************************************************************/

int8s_t spi_reconfig(int32u_t mask, int32u_t val)
{
	int32u_t mod;
	if(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_TIP) {
		return -1;
	}
	/* Disable the SPI core */
	REG32(SPI_BASE+SPI_MOD) &= (~SPI_MOD_EN);

	mod = REG32(SPI_BASE+SPI_MOD);
	mod = (mod & ~mask) | val;
	REG32(SPI_BASE+SPI_MOD) = mod;

	REG32(SPI_BASE+SPI_MOD) |= SPI_MOD_EN;
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
