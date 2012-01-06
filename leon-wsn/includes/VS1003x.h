#ifndef __VS1003_H__
#define __VS1003_H__

#include "fpga.h"
#include "types.h"

//-----------------------------------------------------------------
// driver for vs1003 
// xiaoyang yi
//-----------------------------------------------------------------

//command for vs1003
#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03

//VS1003 Registers
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
  
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f  
 
#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 		 

//-------------------------------------------------------------------
// gpio function defination
// xiaoyang @2011.4.30
//-------------------------------------------------------------------
#define MP3_CMD_CS     (1<<0) 	//gpio10 connect to xcs
//#define MP3_XREST      (1<<1)  	//RE1 connect to reset
#define MP3_DREQ       (1<<1)  	//gpio12 connect to dreq,set it as input
#define MP3_DATA_CS    (1<<2)  	//gpio11 connect to xdcs

//general function
#define MP3_CCS_SEL() 	gpio_clr(PORTC,MP3_CMD_CS)	//gpio_clr(MP3_CMD_CS)		//
#define MP3_CCS_DES() 	gpio_set(PORTC,MP3_CMD_CS)	//gpio_set(MP3_CMD_CS)		//set cs pin high/low

//#define MP3_RST_SET(x)  PORTESET = MP3_XREST

#define MP3_DCS_SEL()  	gpio_clr(PORTC,MP3_DATA_CS)	//gpio_clr(MP3_DATA_CS)	//set dcs pin high/low
#define MP3_DCS_DES()		gpio_set(PORTC,MP3_DATA_CS)	//gpio_set(MP3_DATA_CS)

#define MP3_READ_DREQ()	gpio_read(PORTC,MP3_DREQ)	//gpio_read(MP3_DREQ)

//interfaces
void Vs1003_CMD_Write(int8u_t address,int16u_t data);
int16u_t Vs1003_CMD_Read(int8u_t address);
void Vs1003_Init(void);
void Vs1003_Sine_Test(unsigned char x);
//data write with diff width
void Vs1003_DATA_Write32(int32u_t *data,int32u_t len);
void Vs1003_DATA_Write16(int16u_t *data,int32u_t len);
void Vs1003_DATA_Write8(int8u_t *data,int32u_t len);
void Vs1003_Test(void);

#endif

















