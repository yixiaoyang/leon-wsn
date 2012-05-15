#include "fpga.h"
#include "spi.h"
#include "types.h"
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
	REG32(SPI_BASE+SPI_MOD) = 0x06740000 | SPI_MOD_FACT;	
	//REG32(SPI_BASE+SPI_MOD) |= 0x06720000;
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
	REG32(SPI_BASE+SPI_MOD) = 0x02740000;
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
	//printf("XMIT8 start\n");
  /* Wait for room */
	while(!(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_NF));		
#if REV
	REG32(SPI_BASE+SPI_TX) = val<<24&0xFF000000;
#else
	REG32(SPI_BASE+SPI_TX) = val;
#endif // REV
	//while(!(REG32(SPI_BASE+SPI_EVE)&SPI_EVE_NE));		/* Wait for transmitting over */
	//while(!((REG32(SPI_BASE+SPI_EVE)&SPI_EVE_LT)));
	//printf("Waiting TIP.\n");
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

