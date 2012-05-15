//===================================================================
//                         audio.c
//
//     Description: wm8731 audio codec software interface
//     Author: Jason Liang (co.liang.ol@gmal.com)
//
//===================================================================
#include "fpga.h"
#include "audio.h"
#include "types.h"
#include "ck_gen.h"
#include "gpio.h"
#include "i2cmst.h"
//-------------------------------------------------------------------
// Audio codec initialization 
//
//-------------------------------------------------------------------
void ain_init()
{
	printf("Start audio codec initialization.\n");
	/* Generate clocks audio sampling */
#if 0
	REG32(CK_GEN_BASE+CG_ADCLRCLK) = 0x600;   //32.55kHz
	REG32(CK_GEN_BASE+CG_AINCLK) = 0x020;
	REG32(CK_GEN_BASE+CG_DACLRCLK) = 0x600;
	REG32(CK_GEN_BASE+CG_AUDXCLK) = 0x004;
#else
	REG32(CK_GEN_BASE+CG_ADCLRCLK) = 0x186A;
	REG32(CK_GEN_BASE+CG_AINCLK) = 0x82;
	REG32(CK_GEN_BASE+CG_DACLRCLK) = 0x186A;
	REG32(CK_GEN_BASE+CG_AUDXCLK) = 0x03;//0x19;   // 2M
#endif
	//REG32(CK_GEN_BASE+CG_CTR) = 0x2BB;
	printf("I2S clock initialized\n");
	/* Initialize audio codec */
	unsigned short d16;
	int rc;
	/* Disable MCLK */
	ck_disable(CTR_AX);
	struct gpio_port* portb = gpio_get_port(PORTB);
	//gpio_selectno(portb, 0x04);
	/* Init i2c and enable */
	i2c_init();
	/* Reset device */
	d16 = CD_RST;
	rc = i2c_write(SLVADDR, (unsigned char *)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Initialize power control */
	d16 = CD_PDCT | PDCT_LINEINPD | PDCT_OSCPD | PDCT_CLKOUTPD;
	rc = i2c_write(SLVADDR, (unsigned char*)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Mute line input */
	d16 = CD_LLI | LLI_LINMUTE | LLI_LRINBOTH;
	rc = i2c_write(SLVADDR, (unsigned char*)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}

	/* Init headphone output volumn */
	d16 = CD_LHO | 0x5B | LHO_LRHPBOTH;
	rc = i2c_write(SLVADDR,  (unsigned char*)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Init analogue audio path control */
	d16 = CD_AAPC | AAPC_DACSEL | AAPC_INSEL | AAPC_MICBOOST;
	rc = i2c_write(SLVADDR,  (unsigned char*)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Init digital audio path control */
  d16 = CD_DAPC;
	rc = i2c_write(SLVADDR,  (unsigned char*)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Init digital audio interface format */
  d16 = CD_DAIF | DAIF_MS | DAIF_LRP | 0x02;
	//d16 = CD_DAIF | DAIF_LRP | 0x02;
  //              					Left   32bits
	rc = i2c_write(SLVADDR,(unsigned char *)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Sampling control */
  //d16 = CD_SCT | SCT_USB | 0x18; // 32KHz
	//d16 = CD_SCT | SCT_USB | 0x0C; // 8KHz
	// Normal mode
	//d16 = CD_SCT | 0x18;
	d16 = CD_SCT | SCT_BOSR | 0x2C; // 8kHz
	rc = i2c_write(SLVADDR, (unsigned char *)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Activate device */
	d16 = CD_ACT|ACT_EN;
	rc = i2c_write(SLVADDR, (unsigned char *)&d16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
  /* Re-enable codec mclk */
	ck_enable(CTR_AX);
	gpio_select0(portb, 0x04);
  printf("Audio codec initialized!\n");
}
//-------------------------------------------------------------------
// Audio master and slave mode select
//-------------------------------------------------------------------
void ain_master_enable(int ms_en)
{
	if(ms_en) {
		REG32(AIN_BASE+AIN_CMD) |=  MODE_MS;
	}else {
		REG32(AIN_BASE+AIN_CMD) &=  ~MODE_MS;
	}
}
//-------------------------------------------------------------------
//
// Read 16-bit sample data from adc fifo
//
//-------------------------------------------------------------------
int32u_t adc_fifo_read32()
{
	int32u_t fifo_data32;
	/* Wait for data available */
	while(REG32(AIN_BASE+AIN_STATUS)&ADC_FIFO_EMPTY);
	fifo_data32 = REG32(AIN_BASE+AIN_ADC_FIFO);
	return fifo_data32;
}
//-------------------------------------------------------------------
//
// Write 32-bit sample to dac fifo 
//
//-------------------------------------------------------------------
void dac_fifo_write32(unsigned long d32)
{
	//while(REG32(AIN_BASE+AIN_STATUS)&DAC_FIFO_FULL);
	REG32(AIN_BASE+AIN_DAC_FIFO) = d32;
}
int dac_fifo_full()
{
	if(REG32(AIN_BASE+AIN_STATUS)&DAC_FIFO_FULL) return 1;
	else return 0;
}

//-------------------------------------------------------------------
//
// ain test 
//
//-------------------------------------------------------------------
int ain_record()
{
#define NSAMPLES 109715
	unsigned long *ain_data_start = (unsigned long*)0x40100000;
	unsigned long *ain_data_end = (unsigned long*)0x40200000; /* 0.5mins */
	unsigned long *cur = (unsigned long*)ain_data_start;
	//unsigned long cur = ain_data_start;
	unsigned long fifo32;
	ck_enable(CTR_AX|CTR_AIN|CTR_ALR);
	RF_CHIP_INITIALIZE();
	printf("Speaking:\n");
	while(cur<ain_data_end) {
		fifo32 = adc_fifo_read32();
		*cur++ = fifo32;
		dac_fifo_write32(fifo32);
		//cur+=4;
	}
	/* Sending */
	unsigned short dst_addr = 0x2222;
	unsigned long nsample;
	unsigned char* pbuf = (unsigned char*)ain_data_start;
	nsample = 0;
	printf("Recording stops, start sending...\n");
	while(nsample<NSAMPLES) {
		mac_send(pbuf, 100, (unsigned char*)&dst_addr, 1);

		pbuf+=100;
		nsample+=25;
		delay(1);
	}

	printf("Stop record.\n");
	return 0;
}
void ain_playback(unsigned long addr, unsigned long wdlen)
{
#define PLAY_ADDR 0x41100000
//#define NSAMPLES 438858
#define NSAMPLES 109715
	/* Start play */
	unsigned long *sample32 = addr ? (unsigned long*)addr : \
																		(unsigned long*)PLAY_ADDR;
	unsigned long NumSamples = wdlen ? wdlen : NSAMPLES;
	ck_enable(CTR_AX|CTR_DLR|CTR_AIN);
	unsigned short dst_addr = 0x2222;
	RF_CHIP_INITIALIZE();
	printf("Start play:\n");
	while(NumSamples) {
		if(!dac_fifo_full()) {
			dac_fifo_write32(*sample32);
			/*
			mac_send((unsigned char*)sample32, 
		                sizeof(*sample32), (unsigned char*)&dst_addr, 1);
			*/
			NumSamples--;
			sample32++;
		} else {
			//printf("DAC fifo full, delay.\n");
			//delay(1);
		}

	}
	//ck_disable(CTR_AX|CTR_DLR|CTR_AIN);
	//printf();


}
