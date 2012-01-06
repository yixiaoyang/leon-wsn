#ifndef _SD_H
#define _SD_H

#include "spi.h"
#include "gpio.h"
#include <stdio.h>

//
//	using spi3(sim) to drive the SD card module
//
//	by xiaoyang @2011.3.13 HIT
//
#define SD_DEBUG	1
//------------------------------------------------------------
// Error define
//-------------------------------------------------------------
#define INIT_CMD0_ERROR     0x01
#define INIT_CMD1_ERROR		0x02
#define WRITE_BLOCK_ERROR	0x03
#define READ_BLOCK_ERROR   	0x04 
//------------------------------------------------------------
// Command define
//-------------------------------------------------------------
#define CMD_RESET     		0
#define CMD_INIT			1
#define CMD_READ_SINGLE		17
#define	CMD_WRITE_SINGLE   	24 
//------------------------------------------------------------
// Definitions for MMC/SDC command
//------------------------------------------------------------
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

//-------------------------------------------------------------
// data type   
// this structure holds info on the MMC card currently inserted 
//-------------------------------------------------------------
typedef struct MMC_VOLUME_INFO
{ //MMC/SD Card info
  unsigned int  size_MB;
  unsigned char sector_multiply;
  unsigned int  sector_count;
  unsigned char name[6];
} VOLUME_INFO_TYPE; 
//-------------------------------------------------------------
// buffer for Read and Write
//------------------------------------------------------------- 
typedef struct STORE
{ 
  unsigned char dat[100]; 
} BUFFER_TYPE; //256 bytes, 128 words

//test function
void sd_test();

#endif
