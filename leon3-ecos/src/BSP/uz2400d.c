/*
 * uz2400d.c
 *
 *  Created on: 2011-2-23
 *      Author: hgdytz
 */
#include "spi.h"
#include "types.h"
#include "uz2400d.h"

/******************************************************************************
 * Function: Write short address registers
 * Arg1: saddr: range from 0x00 to 0x3F
 * Arg2: value: the value to set(8bit)
 *****************************************************************************/
void uz2400d_sw(int8u_t saddr, int8u_t value)
{
	SLV_SELECT();
	spi_xmit8((saddr<<1&0x7E)|0x01);	/* Write saddr */
	spi_xmit8(value);						/* Write Value */
	SLV_DESELECT();
}
/******************************************************************************
 * Function: Read short address registers
 * Arg1: saddr: range from 0x00 to 0x3F
 * Ret: the data read from registers(8bit)
 *****************************************************************************/
int8u_t uz2400d_sr(int8u_t saddr)
{
	int8u_t data;
	SLV_SELECT();
	spi_xmit8(saddr<<1&0x7E);
	data = spi_xmit8(0x00);			/* Read phase */
	SLV_DESELECT();
	return data;

}
/******************************************************************************
 * Function: Write long address registers
 * Arg1: laddr: range from 0x200 to 0x27F
 * Arg2: value: the value to set(8bit)
 *****************************************************************************/
void uz2400d_lw(int16u_t laddr, int8u_t value)
{
	SLV_SELECT();	/* Start long address register write transaction */
	spi_xmit8((int8u_t)((laddr>>3&0x007F)|0x0080));	/* Address phase */
	spi_xmit8((int8u_t)((laddr<<5&0x00E0)|0x0010));

	spi_xmit8(value);			/* Data phase */
	SLV_DESELECT();

}
/******************************************************************************
 * Function: Read long address registers
 * Arg1: laddr: range from 0x200 to 0x27F
 * Ret: the data read from registers(8bit)
 *****************************************************************************/
int8u_t uz2400d_lr(int16u_t laddr)
{
	int8u_t data;
	SLV_SELECT();
	spi_xmit8((int8u_t)((laddr>>3&0x007F)|0x0080));	/* Address phase */
	spi_xmit8((int8u_t)(laddr<<5&0x00E0));
	data = spi_xmit8(0x00);			/* Read phase */
	SLV_DESELECT();
	return data;

}
/******************************************************************************
 Function: Write long address register block
 [
 	> laddr: long register start address
	> buf: data buffer pointer
	> size: block size

	< none
 ]
 *****************************************************************************/
void uz2400d_lw_block(int16u_t laddr, int8u_t* buf, int16u_t size)
{
	SLV_SELECT();
	laddr = ((laddr<<5)&0x7FEF)|0x8010;
	spi_xmit8(((int8u_t*)&laddr)[0]);		/* High 8 bits */
	spi_xmit8(((int8u_t*)&laddr)[1]);		/* Low 8 bits */
	while(size--) {
		spi_xmit8(*buf++);
	}
	SLV_DESELECT();
}
/******************************************************************************
 Function: Read long address register block
 [
 	> laddr: long register start address
	> buf: data buffer pointer
	> size: block size

	< none
 ]
 // Sanity check omitted( size )
 *****************************************************************************/
void uz2400d_lr_block(int16u_t laddr, int8u_t* buf, int16u_t size)
{
	SLV_SELECT();
	laddr = ((laddr<<5)&0x7FEF)|0x8000;	/* Set address and command */
	spi_xmit8(((int8u_t*)&laddr)[0]);		/* High 8 bits */
	spi_xmit8(((int8u_t*)&laddr)[1]);		/* Low 8 bits */
	while(size--) {
		*buf++ = spi_xmit8(0x00);
	}
	SLV_DESELECT();
}
