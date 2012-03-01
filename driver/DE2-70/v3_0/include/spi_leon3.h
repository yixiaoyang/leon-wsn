/*
 * SPI deriver for ecos on DE2-70
 * xiaoyang 
 * 		2012-2-26 11:03 create it.
 *		2012-3-1  16:44	test ok.
 */
 
#ifndef CYGONCE_DEVS_SPI_LEON3_H
#define CYGONCE_DEVS_SPI_LEON3_H

/* leon3 bus address*/
#define SPI0_BASE		0x80000600

#define REG32(x)		*((volatile unsigned long*)(x))

/* SPICTL Registers APB Address Offset */
#define SPI_CAP			0x0
#define SPI_MOD			0x20
#define SPI_EVE			0x24
#define SPI_MSK			0x28
#define SPI_CMD			0x2C
#define SPI_TX			0x30
#define SPI_RX			0x34
#define SPI_SLV			0x38
/* Bit Definition of SPICTL Capability Register  */
#define SPI_CAP_REV		0x7F
#define SPI_CAP_FDEPTH		0xFF00
#define SPI_CAP_SSEN		0x10000
#define SPI_CAP_ASELA		0x20000
#define SPI_CAP_AMODE		0x40000
#define SPI_CAP_TWEN		0x80000
#define SPI_CAP_MAXWLEN	0xF00000
#define SPI_CAP_SSSZ		0xFF000000
/* Bit Definition of SPICTL Mode Register */
#define SPI_MOD_TAC		0x10
#define SPI_MOD_ASELDEL	0x60
#define SPI_MOD_CG		0x780
#define SPI_MOD_OD		0x1000
#define SPI_MOD_FACT		0x2000
#define SPI_MOD_ASEL		0x4000
#define SPI_MOD_TW		0x8000
#define SPI_MOD_PM		0xF0000		/* Prescale Module*/
#define SPI_MOD_LEN		0xF00000
#define SPI_MOD_EN		0x1000000
#define SPI_MOD_MS		0x2000000		/* Master/Slave */
#define SPI_MOD_REV		0x4000000
#define SPI_MOD_DIV16		0x8000000
#define SPI_MOD_CPHA		0x10000000
#define SPI_MOD_CPOL		0x20000000
#define SPI_MOD_LOOP		0x40000000
#define SPI_MOD_AMEN		0x80000000
/* Bit Definition of SPICTL Event Register */
#define SPI_EVE_NF		0x100
#define SPI_EVE_NE		0x200
#define SPI_EVE_MME		0x400
#define SPI_EVE_UR		0x800
#define SPI_EVE_OV		0x1000
#define SPI_EVE_LT		0x4000
#define SPI_EVE_TIP		0x80000000			/* Transfer in Progress */
/* Bit Definition of SPICTL Mask Register */
#define SPI_MSK_NFE		0x100
#define SPI_MSK_NEE		0x200
#define SPI_MSK_MMEE		0x400
#define SPI_MSK_UNE		0x800
#define SPI_MSK_OVE		0x1000
#define SPI_MSK_LTE		0x4000
#define SPI_MSK_TIPE		0x80000000
/* Bit Definition of SPICTL Command Register */
#define SPI_CMD_LST		0x400000
/* Bit Definition of SPICTL Slave Select Register */
#define SPI_SLV_SSSZ		1
#define SPI_SLV_SLVSEL		(1<<SPI_SLV_SSSZ)-1

/* Some useful routines */
#define SLV_SELECT()			REG32(SPI_BASE+SPI_SLV) &= ~SPI_SLV_SLVSEL
#define SLV_DESELECT()		REG32(SPI_BASE+SPI_SLV) |= SPI_SLV_SLVSEL

#endif // CYGONCE_DEVS_SPI_CORTEXM_LEON3_H

