/*
 * uz2400d.c
 * 2012-3-4     xiaoyang
 */

#include "uz2400d.h"

static u8 rf_tbuf[RF_BUF_SIZE];
static u8 rf_rbuf[RF_BUF_SIZE];
//spidev_sync_wr(struct spidev_data *spidev,u8* rf_tbuf, u8* rf_rbuf,size_t len);

/******************************************************************************
 * Function: Write short address registers
 * Arg1: saddr: range from 0x00 to 0x3F
 * Arg2: value: the value to set(8bit)
 * Ret :bytes written successfully, or errno if failed
 *****************************************************************************/
ssize_t uz2400d_sw(struct spidev_data* spidev,u8 saddr, u8 value)
{
	ssize_t status = 0;
	SLV_SELECT();
	rf_tbuf[0] = ((saddr<<1&0x7E)|0x01);
	rf_tbuf[1] = value;
	status = spidev_sync_wr(spidev,rf_tbuf,rf_rbuf,2);
	//spi_wbyte((saddr<<1&0x7E)|0x01);	/* Write saddr */
	//spi_wbyte(value);						/* Write Value */
	SLV_DESELECT();
	return status;
}

/******************************************************************************
 * Function: Read short address registers
 * Arg1: saddr: range from 0x00 to 0x3F
 * Ret: the data read from registers(8bit)
 *****************************************************************************/
u8 uz2400d_sr(struct spidev_data* spidev,u8 saddr)
{
	u8 data;
	ssize_t status = 0;
	SLV_SELECT();
	rf_tbuf[0] = (saddr<<1&0x7E);
	rf_tbuf[1] = 0x00;
	status = spidev_sync_wr(spidev,rf_tbuf,rf_rbuf,2);
	data = rf_rbuf[1];
	//spi_wbyte(saddr<<1&0x7E);
	//data = spi_rbyte(0x00);			/* Read phase */
	SLV_DESELECT();
	return data;

}
/******************************************************************************
 * Function: Write long address registers
 * Arg1: laddr: range from 0x200 to 0x27F
 * Arg2: value: the value to set(8bit)
 * Ret :bytes written successfully, or errno if failed
 *****************************************************************************/
ssize_t uz2400d_lw(struct spidev_data* spidev,u16 laddr, u8 value)
{
	ssize_t status = 0;
	SLV_SELECT();	/* Start long address register write transaction */
	
	rf_tbuf[0] = ((u8)((laddr>>3&0x007F)|0x0080));
	rf_tbuf[1] = ((u8)((laddr<<5&0x00E0)|0x0010));
	rf_tbuf[2] = value;
	status = spidev_sync_wr(spidev,rf_tbuf,rf_rbuf,3);
	
	//spi_wbyte((u8)((laddr>>3&0x007F)|0x0080));	/* Address phase */
	//spi_wbyte((u8)((laddr<<5&0x00E0)|0x0010));
	//spi_wbyte(value);			/* Data phase */
	
	SLV_DESELECT();
	return status;
}

/******************************************************************************
 * Function: Read long address registers
 * Arg1: laddr: range from 0x200 to 0x27F
 * Ret: the data read from registers(8bit)
 *****************************************************************************/
u8 uz2400d_lr(struct spidev_data* spidev,u16 laddr)
{
	u8 data;
	SLV_SELECT();
	
	rf_tbuf[0] = ((u8)((laddr>>3&0x007F)|0x0080));
	rf_tbuf[1] = ((u8)(laddr<<5&0x00E0));
	rf_tbuf[2] = 0x00;
	spidev_sync_wr(spidev,rf_tbuf,rf_rbuf,3);
	data = rf_rbuf[2];
	
	//spi_wbyte((u8)((laddr>>3&0x007F)|0x0080));	/* Address phase */
	//spi_wbyte((u8)(laddr<<5&0x00E0));
	//data = spi_rbyte(0x00);			/* Read phase */
	
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
 Ret :bytes written successfully, or errno if failed
 *****************************************************************************/
ssize_t uz2400d_lw_block(struct spidev_data* spidev,u16 laddr, u8* buf, u16 size)
{
	ssize_t status = 0;
	SLV_SELECT();
	if(size + 2 > RF_BUF_SIZE){
		printk("error: uz2400d_lw_block size too long!\n");
		return -1;
	}
	laddr = ((laddr<<5)&0x7FEF)|0x8010;
	
	rf_tbuf[0] = (((u8*)&laddr)[0]);
	rf_tbuf[1] = (((u8*)&laddr)[1]);
	memcpy(rf_tbuf+2,buf,size);
	status = spidev_sync_wr(spidev,rf_tbuf,rf_rbuf,size+2);
	
	//laddr = ((laddr<<5)&0x7FEF)|0x8010;
	//spi_wbyte(((u8*)&laddr)[0]);		/* High 8 bits */
	//spi_wbyte(((u8*)&laddr)[1]);		/* Low 8 bits */
	//while(size--) {
	//	spi_wbyte(*buf++);
	//}

	SLV_DESELECT();
	return status;
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
ssize_t uz2400d_lr_block(struct spidev_data* spidev,u16 laddr, u8* buf, u16 size)
{
	ssize_t status = 0;
	SLV_SELECT();	
	if(size + 2 > RF_BUF_SIZE){
		printk("error: uz2400d_lr_block size too long!\n");
		return -1;
	}
	
	laddr = ((laddr<<5)&0x7FEF)|0x8000;	/* Set address and command */
	rf_tbuf[0] = (((u8*)&laddr)[0]);
	rf_tbuf[1] = (((u8*)&laddr)[1]);
	status = spidev_sync_wr(spidev,rf_tbuf,rf_rbuf,size+2);
	if(status >= 0){
		memcpy(buf,rf_rbuf+2,size);
		status = size;
	}//laddr = ((laddr<<5)&0x7FEF)|0x8000;	/* Set address and command */
	//spi_wbyte(((u8*)&laddr)[0]);		/* High 8 bits */
	//spi_wbyte(((u8*)&laddr)[1]);		/* Low 8 bits */
	//while(size--) {
	//	*buf++ = spi_rbyte(0x00);
	//}
	
	SLV_DESELECT();
	return status;
}
