#include "VS1003x.h"	 
#include "spi.h"
#include "gpio.h"
#include <stdio.h>
 
//-----------------------------------------------------------------
// transport driver for vs1003
// author hgdytz
//-----------------------------------------------------------------
// port driver for DE2-70 @2011.4.30 
//-----------------------------------------------------------------
// Function used:
//	spi2_8TxRx
//	spi2_high_speed_mode
//	spi2_init
//-----------------------------------------------------------------
// 
//-----------------------------------------------------------------
#define VS1003_DEBUG	1

#if VS1003_DEBUG
#define DATA_LEN 127936
//336300 wav file
extern const unsigned int music_data[];
#endif

static unsigned long count = 0;
 
void delay_ms(unsigned int num)
{
	unsigned int i = num * 1024;
	while( (i--) > 0 ){
		;
	}
}

//------------------------------------------------------------------------------  
//	write cmd to vs1003
//	address,data
//------------------------------------------------------------------------------
void Vs1003_CMD_Write(int8u_t address,int16u_t data)
{
	unsigned char high = data >> 8;
	unsigned char low = data;
	
	//spi2_init();//low speed init
	//spi2_low_speed_mode();		//high speed mode for data transport
	//printf("Write_cmd:%2x,%2x\n",high, low);
	
	//wait for free
    	while( MP3_READ_DREQ() == 0)
		;
	//MP3_DCS_DES(); 				//MP3_DATA_CS=1;
	//MP3_CCS_DES();
	//delay_ms(1);
	
	MP3_CCS_SEL(); 					//MP3_CMD_CS=0;pull down XCS 
	
	spi2_8TxRx(VS_WRITE_COMMAND);		//send write command
	spi2_8TxRx(address); 			//send addr
	
	//spi2_16TxRx(data);
	spi2_8TxRx(high); 			//8bits high of data
	spi2_8TxRx(low);	 		//8bits low of data
	
	MP3_CCS_DES();   			//MP3_CMD_CS=1;
	//spi2_high_speed_mode();		//high speed mode for data transport
}

int16u_t Vs1003_CMD_Read(int8u_t address)
{
	int16u_t data;
	
	//spi2_init();					//low speed init
	while( MP3_READ_DREQ() == 0)	//wait for free
		;
		
	MP3_CCS_SEL();					//pull down XCS
	
	
	spi2_8TxRx(VS_READ_COMMAND);	//send cmd:VS_READ_COMMAND
	spi2_8TxRx(address);			//senb addr
	
	//get the uint16 data
	data = spi2_8TxRx(0X00);
	data <<= 8;
	data += spi2_8TxRx(0X00);
	
	MP3_CCS_DES();
	//spi2_high_speed_mode();			//high speed mode for data transport
	return data;
}

//------------------------------------------------------------------------------  
//	configure vs1003
//	address,data
//------------------------------------------------------------------------------
void Vs1003_Init(void)
{
	//char i = 0;
	//int16u_t data = 0x0;
	
	spi2_init();//low speed init
	//SPI2_SELECT();
	
	//set gpio AS output
	gpio_make_in(PORTC,MP3_DREQ);
	gpio_make_out(PORTC,MP3_CMD_CS);
	gpio_make_out(PORTC,MP3_DATA_CS);
	
	//MP3_RST_SET(0);
	delay_ms(100);
	
	MP3_CCS_DES();  		//set cs pin high/low
	//MP3_RST_SET(1); 		//reset
	MP3_DCS_DES(); 			//xcs = 0;
	
	Vs1003_CMD_Write(SPI_MODE,0x0804);
	
	//wait for DREQ==1
#if 1
	printf("Debug:wait for DREQ==1	@vs1003.c:113\n");
#endif
	while( MP3_READ_DREQ() == 0 )
		;
#if 1
	printf("Debug:wait DREQ over	@vs1003.c:114\n");
#endif	
	//step 5-9
	Vs1003_CMD_Write(SPI_CLOCKF,0xe800);
	delay_ms(1);
	Vs1003_CMD_Write(SPI_AUDATA,0XBB81);
	delay_ms(1);
	Vs1003_CMD_Write(SPI_BASS,0x0055);
	delay_ms(1);
	Vs1003_CMD_Write(SPI_VOL,0x2020);
	delay_ms(1);
	
	// sned 4byte unused data to vs1003,start SPI
	MP3_DCS_SEL();		//data transfer
	spi2_8TxRx(0XFF);
	spi2_8TxRx(0XFF);
	spi2_8TxRx(0XFF);
	spi2_8TxRx(0XFF);
	MP3_DCS_DES();		//cancel data transfer
	delay_ms(200);
	
	#if VS1003_DEBUG
	printf("vs1003 init:init over\n");
	#endif
	
	//spi2_high_speed_mode();//high speed mode for data transport
}

//-------------------------------------------------------------------------
// sine test
//-------------------------------------------------------------------------
void Vs1003_Sine_Test(unsigned char x)
{
 	Vs1003_CMD_Write(SPI_MODE,0x0820);   
	
	while ( MP3_READ_DREQ() == 0)
		;
    //SPI2_SELECT();
	MP3_CCS_DES();
	MP3_DCS_SEL();
	
	spi2_8TxRx(0x53);
	spi2_8TxRx(0xef);
	spi2_8TxRx(0x6e);
	spi2_8TxRx(x);
	spi2_8TxRx(0x00);
	spi2_8TxRx(0x00);
	spi2_8TxRx(0x00);
	spi2_8TxRx(0x00);
	delay_ms(1000);
	
	spi2_8TxRx(0x45);
	spi2_8TxRx(0x78);
	spi2_8TxRx(0x69);
	spi2_8TxRx(0x74);
	spi2_8TxRx(0x00);
	spi2_8TxRx(0x00);
	spi2_8TxRx(0x00);
	spi2_8TxRx(0x00);
	MP3_DCS_DES();	 
	delay_ms(1000);
}	 

//------------------------------------------------------------------------------ 
//	xiaoyang yi@2011.3.14 HIT
//	write data to vs1003
//	address,8bit data
//------------------------------------------------------------------------------
void Vs1003_DATA_Write8(int8u_t *data,int32u_t len)
{
	unsigned int i = 0;
#if VS1003_DEBUG
	printf("Vs1003_DATA_Write8\r\n");
#endif	
	MP3_DCS_SEL();   			//MP3_DATA_CS=0;
	
	//send data
	for(i = 0; i < len; i++)
    	{
		while ( MP3_READ_DREQ() == 0)
			;					//wait for DREQ high
		spi2_8TxRx(data[i]);
    	}
		
	MP3_DCS_DES();   			//MP3_DATA_CS=1;
}         

//------------------------------------------------------------------------------ 
//	xiaoyang yi@2011.3.14 HIT
//	write data to vs1003
//	address,32bit data
//------------------------------------------------------------------------------
void Vs1003_DATA_Write32(int32u_t *data,int32u_t len)
{
	unsigned int i = 0;
#if VS1003_DEBUG
	printf("Vs1003_DATA_Write32\r\n");
#endif	
	MP3_DCS_SEL();   			//MP3_DATA_CS=0;
	
	//send data
	for(i = 0; i < len; i++)
	{
		while ( MP3_READ_DREQ() == 0)
			;					//wait for DREQ high
		spi2_8TxRx(data[i] >> 24);
		spi2_8TxRx(data[i] >> 16);
		spi2_8TxRx(data[i] >> 8);
		spi2_8TxRx(data[i]);
	}
		
	MP3_DCS_DES();   			//MP3_DATA_CS=1;
}         

//------------------------------------------------------------------------------ 
//	xiaoyang yi@2011.3.14 HIT
//	write data to vs1003
//	address,16bit data
//------------------------------------------------------------------------------
void Vs1003_DATA_Write16(int16u_t *data,int32u_t len)
{
	unsigned int i = 0;
#if VS1003_DEBUG
	printf("Vs1003_DATA_Write16\r\n");
#endif	
	MP3_DCS_SEL();   			//MP3_DATA_CS=0;
	
	//send data
	for(i = 0; i < len; i++)
	{
		while ( MP3_READ_DREQ() == 0)
			;					//wait for DREQ high
		spi2_8TxRx(data[i] >> 8);
		spi2_8TxRx(data[i]);
	}
		
	MP3_DCS_DES();   			//MP3_DATA_CS=1;
} 

//------------------------------------------------------------------------------  
//	write data to vs1003
//	address,data
//------------------------------------------------------------------------------
void Vs1003_Test(void)
{
	int i = 0;
	int j = 0;
	int16u_t data;
	Vs1003_Init();
	
	//raw testing
	printf("RAW test..\n") ;
	for(i = 0; i< 20; i++)
	{
		data = 0x0;
		printf("Reg=%x\t",data);
		Vs1003_CMD_Write(SPI_VOL,0x2020);
		data = Vs1003_CMD_Read(SPI_VOL);
		printf(", %x\r\n",data);
		delay_ms(1000);
	}
	
	//sine test
	for(j =0;j <4; j++)
	{
		for(i = 0; i< 15; i++)
		{
			Vs1003_Sine_Test(i);
		}
		for(i = 15; i > 0; i--)
		{
			Vs1003_Sine_Test(i);
		}
		printf("sine over..\n") ;
	}
	
	printf("begin playing\n");
	//play data
	while(1){
		Vs1003_DATA_Write8(music_data,DATA_LEN);
		printf("play over!\r\n");
	}
}
