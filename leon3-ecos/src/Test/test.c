#include <stdio.h>
#include "gpio.h"
#include "rf.h"
#include "netif.h"
#include "audio.h"
#include "i2cmst.h"
#include "spi.h"
#include "ck_gen.h"

void delay(unsigned long ms)
{
	unsigned long i;
  while(ms--) {
		for(i=0;i<10000;i++);
	}
}
void hellospi(void)
{
	unsigned char data_write[2][2] = {
		{((0x05<<1&0x7E)|0x01),0x55},
		{(0x05<<1&0x7E), 0x00}
	};
	unsigned char rcv;
	printf("Hello, world!\n");
	spi_init();

	SLV_SELECT();
	spi_xmit8(data_write[0][0]);
	spi_xmit8(data_write[0][1]);
	SLV_DESELECT();
//*
	SLV_SELECT();
	spi_xmit8(data_write[1][0]);
	rcv = spi_xmit8(data_write[1][1]);
	SLV_DESELECT();

//*/
	printf("Received %x\n",rcv);
	return 0;
}
void test_i2s(void)
{
	int i = 20;

	ain_master_enable(1);
	ain_init();
	//ck_enable(CTR_DLR|CTR_AIN);
  for(;i;i--) {
		dac_fifo_write32(0x55555555);
		dac_fifo_write32(0xaaaaaaaa);
	}
	//ck_disable(CTR_DLR|CTR_AIN);
}

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
int test_gpio(void)
{
	struct gpio_port* port_f = gpio_get_port(PORTF);
	int i=10;
	printf("Test gpio\n");
	gpio_selectno(port_f, 0x08);
  gpio_make_out(port_f, 0x08);
	gpio_set(port_f, 0x08);
	while(i--) {
		/*
		gpio_set(port_f, 0x08);
		delay(1000);
		gpio_clr(port_f, 0x08);
		delay(1000);
		*/
		gpio_toggle(port_f, 0x08);
		delay(1000);
	}
	return 0;
}
int test_rf_tx(void)
{
	char* str = "Hello, world!\n";
	unsigned char test_data[4] = {0xAA, 0x22, 0x33, 0x44};
	unsigned short dst_addr = 0x2222;
	int rc, i;
	RF_CHIP_INITIALIZE();
	/* Print network configuration */
	netconfig();
	printf("==================================================\n");
	/* Audio */
#define MUSIC_ADDR 0x41100000
#define NSAMPLES 109715
	unsigned long fifo32;
	unsigned char* pbuf = (unsigned char*)MUSIC_ADDR;
	unsigned long count = 0;
	ain_init();
	ck_enable(CTR_AX|CTR_AIN|CTR_ALR);
	printf("Recording and sending...\n");
	while(count<NSAMPLES) {
		//fifo32 = adc_fifo_read32();
		rc = mac_send(pbuf, 
		                100, (unsigned char*)&dst_addr, 1);
		pbuf+=100;
		count+=25;

		/*
		if(rc!=0) {
			printf("-");
		}else {
			printf("+");
		}
		*/
		delay(1);
	}
	/*
	printf("Sent out %d bytes: ", sizeof(test_data));
	for(i=0; i<sizeof(test_data); i++) printf("%#x ", test_data[i]);
	printf("\n");
	*/
	if(rc!=0) {
		printf("Sending string failed.\n");
	}	else {
		printf("String sent out.\n");
	}
	return 0;
}
void test_rf_rx(void)
{
	unsigned char rxbuf[144] = {0};
	unsigned long *ps32;
	unsigned char* music_buf;
	int len, i;
	RF_CHIP_INITIALIZE();
	/* Print network configuration */
	netconfig();
	printf("==================================================\n");
	/*
	if( (len=RF_Rx(rxbuf))>0 ) {
		printf("Received %d chars:\n", len);
		for(i=0; i<len; i++) {
			printf("\t%#x ", rxbuf[i]);
		}
		printf("\n");
	}
	*/
#define MUSIC_ADDR 0x41100000
#define NSAMPLES 109715
	unsigned long sample32, nsample;
	music_buf = (unsigned char*)MUSIC_ADDR;
	nsample = 0;
	ain_init();
	ck_enable(CTR_AX|CTR_DLR|CTR_AIN);
	printf("Receiving...\n");
	while(nsample<NSAMPLES) {
		//len = mac_recv(rxbuf);
		len = mac_recv(music_buf);
		music_buf+=len;
	  nsample += len/4;	
//		ps32 = (unsigned long*)rxbuf;
		if(len!=-1) {
			/*
			for(i=0; i<len; i++) {
				printf("\t%#x ", rxbuf[i]);
			}
			printf("\n");
			*/
			/*
		  while(len) {	
				if(!dac_fifo_full()) {
					dac_fifo_write32(*ps32++);
				}
			len-=4;
			}
			*/
		}
	}
	printf("Playing...\n");
	ps32 = (unsigned long*)MUSIC_ADDR;
	nsample = 0;
	while(nsample<NSAMPLES) {
		if(!dac_fifo_full()) {
			dac_fifo_write32(*ps32++);
			nsample++;
		}
	}

}
int test_i2cmst(void)
{
  unsigned short data16;
	int rc=0;
	printf("Start I2C master test.\n");
	ck_disable(3);
	i2c_init();
	/* Reset device */
	data16 = CD_RST;
	rc = i2c_write(SLVADDR, &data16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Left & right line in mute */
	//data16 = CD_LLI | LLI_LINMUTE | LLI_LRINBOTH; 
	//i2c_write(SLVADDR, &data16, 2);
	/* Side tone mode of mic */
	data16 = CD_AAPC|AAPC_SIDETONE;
	rc = i2c_write(SLVADDR, &data16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Set headphone output volume */
	//data16 = CD_LHO|LHO_LRHPBOTH|0x7F;
	//rc = i2c_write(SLVADDR, &data16, 2);
	/* Power down line input, crystal oscillator, clock output */
	data16 = CD_PDCT | PDCT_LINEINPD | PDCT_OSCPD ;//| PDCT_CLKOUTPD;
	rc = i2c_write(SLVADDR, &data16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Clock output divide by 2 */
	data16 = CD_SCT|SCT_CLKODIV2;
	rc = i2c_write(SLVADDR, &data16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	/* Activate codec device */
	data16 = CD_ACT|ACT_EN;
	rc = i2c_write(SLVADDR, &data16, 2);
	if(rc!=2) {
		printf("I2C write failed, returned %d.\n", rc);
	}
	ck_enable(3);
  printf("End I2C master test.\n");
	return 0;
}
