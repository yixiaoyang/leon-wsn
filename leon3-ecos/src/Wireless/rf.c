/*
 * rf.c
 *
 *  Created on: 2011-3-7
 *      Author: hgdytz
 */

#include "includes.h"

/*rf_delay*/
void rf_delay(int32u_t us)
{
	int m_count = 128;
	while(us-- > 0){
		while(m_count-->0)
			;
	}
}

/******************************************************************************
 * Function: Calibrate sleep clock source with selected clock divisor
 * 			   and return the period of clock source.
 * Arg1: clk_div: divisor of sleep clock
 * Ret: The period of clock source, unit: ns
 *****************************************************************************/
int32u_t RF_CAL_SLEEP_CLK(int8u_t clk_div)
{
	int32u_t cal_cnt = 0, slp_clk;

	uz2400d_lw(0x220 , clk_div & 0x1f); 	//set clock div
	uz2400d_lw(0x20b, 0x10); 				//start calibrate sleep clock
	while(!(uz2400d_lr(0x20b) & 0x80)); 	//wait calibration ready

	slp_clk = uz2400d_lr(0x20b)& 0xf; 	//get calibrated value
	cal_cnt |= slp_clk << 16;
	slp_clk = uz2400d_lr(0x20a);
	cal_cnt |= slp_clk << 8;
	cal_cnt |= uz2400d_lr(0x209);
	slp_clk = ((cal_cnt*125)/32); 		//calibrate clock period, unit:ns
						// (cal_cnt * (62.5 * 2)) /(16 * 2)
	return slp_clk;
}
#if 0
/******************************************************************************
 * Function: Calculate "wake count" and "wake time" for each power
 * 				saving mode.
 * Args: xtal_stable_time: crystal stable time, unit:us
  		  early_wakeup_time: the early wake up time, unit:us
		  clk_div: divisor of sleep clock
		  power_saving_mode: the power saving mode user wants to use
		  use_ext_wakeup: use external wake up mode flag( TRUE or FALSE )
 * Ret: The period of clock source, unit: ns
 *****************************************************************************/
int32u_t RF_SET_CLK_RECOVERY_TIME(int16u_t xtal_stable_time,
		int16u_t early_wakeup_time, int8u_t clk_div,
		int8u_t power_saving_mode, int8u_t use_ext_wakeup)
{
	int32u_t sleep_clk, wake_count, wake_time;
	/* calculate low speed clock, unit:ns */
	sleep_clk = RF_CAL_SLEEP_CLK(clk_div);
	/* calculate wake count */
	wake_count = (((int32u_t)xtal_stable_time*1000)/sleep_clk)+1;
	/* fine tune the wake count for each power saving mode */
	switch(power_saving_mode) {
	case HALT:
		wake_count = 1;
		/* system clock will not be closed under HALT mode, */
		/*the clock recovery time can be reduced to the shortest value */
		break;
	case STANDBY:
		/*external wake up circuit will engage 6 clocks of recovery time,
		 it can be reduced. */
		if(use_ext_wakeup) {
			if(clk_div == 0) {
				if((wake_count-6)>0)
					wake_count = wake_count-6;
				else
					wake_count = 1;
			}
			else if(clk_div == 1) {
				if((wake_count-3)>0) 	//(6/2) = 3
					wake_count = wake_count-3;
				else
					wake_count = 1;
			}
			else if(clk_div == 2) {
				if((wake_count-1)>0) 	//(6/4) = 1.5 = 1
					wake_count = wake_count-1;
				else
					wake_count = 1;
			}
		}
		else {
			if(wake_count<1)
				wake_count = 1;
		}
		break;
	default:
		break;
	}
	uz2400d_sw(0x36, (int8u_t)((wake_count & 0x0180)>> 4)); //set wake count
	uz2400d_sw(0x35, (int8u_t)(wake_count & 0x007f));
	/* calculate wake time */
	wake_time = (((int32u_t)early_wakeup_time*1000)/sleep_clk)+1;
	/* wake time must be larger than wake count */
	if(wake_time <= wake_count)
		wake_time = wake_count+1;
	uz2400d_lw(0x223, (int8u_t)((wake_time >> 8)&0x07)); //set wake time
	uz2400d_lw(0x222, (int8u_t)wake_time);
	return sleep_clk;
}


/******************************************************************************
 Function: Change channel between 2405MHz ~ 2480MHz
 [
 	> frequency: Channel frequency, range: 05-80(multiple values of 5)
	> delay: When change channel, UZ2400 will need delay time,
			  250K: 550us, 1M: 300us, 2M: 250us,
			  This counter value depends on MCU system clock or
			  instruction period.
	< -1: set channel unsuccessfully, frequency is out of range
	< 0:   set channel successfully
 ]
 *****************************************************************************/
int8u_t RF_SET_CHANNEL(int8u_t frequency, int16u_t delay)
{
	int8u_t val;
	if (frequency > 80 || frequency < 05) /* check channel range */
		return -1;
	val = uz2400d_sr(0x26); 				/* save register value */

	uz2400d_sw(0x26, val|0x2); 		/* turn on TX clock */
	uz2400d_lw(0x200, (((frequency/5)-1) << 4)|0x03); /* set channel */

	uz2400d_sw(0x36, 0x04); 		/* reset RF */
	uz2400d_sw(0x36, 0x00);
	while(delay--); 				/* 250K: 550us, 1M: 300us, 2M: 250us */
	uz2400d_sw(0x26, val); /* turn off TX clock, restore original value */
	return 0;
}
#endif
/******************************************************************************
 Function: Enable/Disable external PA(power amplifier)
 	 	 	 automatically control.
 [
 	> None
	< None
 ]
 *****************************************************************************/
void RF_AUTO_EXT_PA_CTRL_ON(void)
{
	uz2400d_lw(0x22f, (uz2400d_lr(0x22f)&~0x7)|0x1);
}
void RF_AUTO_EXT_PA_CTRL_OFF(void)
{
	uz2400d_lw(0x22f, uz2400d_lr(0x22f)&~0x7);
}
/******************************************************************************
 Function: Set transmission speed to 250Kbps.
 [
 	> None
	< None
 ]
 *****************************************************************************/
void RF_NORMAL_SPEED(void)
{
	uz2400d_lw(0x206, 0x30); 						//RF optimized control
	uz2400d_lw(0x207, uz2400d_lr(0x207)&~0x10); 	//RF optimized control

	uz2400d_sw(0x38, 0x80); 					/* select 250kbps mode */
	uz2400d_sw(0x2a, 0x02); 					/* baseband reset */
}
/******************************************************************************
 Function: Set transmission speed to 1Mbps.
 [
 	> None
	< None
 ]
 *****************************************************************************/
void RF_TURBO_SPEED_1M(void)
{
	uz2400d_lw(0x206, 0x70); 			//RF optimized control
	uz2400d_lw(0x207, uz2400d_lr(0x207)|0x10); 	//RF optimized control

	uz2400d_sw(0x38, 0x81); 			/* select 1Mbps mode */
	uz2400d_sw(0x2a, 0x02);			 	/* reset baseband */
}
/******************************************************************************
 Function: Set transmission speed to 2Mbps.
 [
 	> None
	< None
 ]
 *****************************************************************************/
void RF_TURBO_SPEED_2M(void)
{
	uz2400d_lw(0x206, 0x70); 			//RF optimized control
	uz2400d_lw(0x207, uz2400d_lr(0x207)&~0x10); 	//RF optimized control

	uz2400d_sw(0x38, 0x83); 			/* select 2Mbps mode */
	uz2400d_sw(0x2a, 0x02); 			/* reset baseband */
}
/******************************************************************************
 Function: Transmit packet using TX normal FIFO.
 [
 	> *tx_data: the TX data which user wants to transmit
	> tx_data_len: the length of the TX data
	> ack_req: Acknowledgment require flag
	< -1: unsuccessfully, or user data length is larger than 125 bytes
	< 0: successfully transmitted
 ]
 // Security disabled
 *****************************************************************************/
#if RF_TxN_EN
result_t RF_TxN(int8u_t *tx_data, int8u_t tx_data_len, mybool ack_req)
{
	int8u_t val;
	if(tx_data_len > 125) 	/*max data length of UZ2400 is 125 */
		return -1;
	uz2400d_lw(0x0,0x00);
	uz2400d_lw(0x1, tx_data_len); 		/* Set TX data length */
	/* load data into TX normal FIFO */
	uz2400d_lw_block(0x2, tx_data, tx_data_len);

	uz2400d_sw(UZS_SOFTRST, 0x02);		/* Reset base band */
#if 1
	val = uz2400d_sr(UZS_TXNTRIG);		/* Transmit normal FIFO control */
	if(ack_req) 				/* If need wait Ack? */
		val = (val & ~0x02)|0x05;
	else
		val = (val & ~0x06)|0x01;
#endif
	uz2400d_sw(UZS_TXNTRIG, val); 	/* trigger TxN */


	while(!(uz2400d_sr(UZS_ISRSTS)&UZS_ISRSTS_TXNIF)); /* Wait For TxN interrupt */

	if(!(uz2400d_sr(UZS_TXSR)&UZS_TXSR_TXNS)) 		/* Check TxN result */
		return 0;
	return -1;
}
#endif // RF_TX
/******************************************************************************
 Function: Check RX interrupt and read received packet from RX FIFO.
 [
 	> buffer: data buffer pointer
	< length of the received packet or '0' if no packet received
 ]
 *****************************************************************************/
int8u_t RF_Rx(int8u_t *buffer)
{
	int8u_t len=0;
	while(!(uz2400d_sr(UZS_ISRSTS)&UZS_ISRSTS_RXIF))
		cyg_thread_delay(1);
	/* Check RX interrupt */
	//gpio_set(0x3f);	// On LEDs
	len = uz2400d_lr(0x300); 		/* Get received packet length */
	uz2400d_lr_block(0x301, buffer, len); /* Restore received packet */
	uz2400d_sw(UZS_RXFLUSH, 0x01);
	len = len-2; //decrease CRC length
	return len;
}
/******************************************************************************
 Function: Set PAN ID
 [
 	> panid: PAN identifier of current device
	< none
 ]
 *****************************************************************************/
void RF_SET_PAN_ID(int16u_t panid)
{
	uz2400d_sw(UZS_PANIDL, (int8u_t)(panid&0x00FF));
	uz2400d_sw(UZS_PANIDH, (int8u_t)(panid>>8&0x00FF));

}
/******************************************************************************
 Function: Set 16-bit short address of this device
 [
 	> saddr: short address of current device
	< none
 ]
 *****************************************************************************/
void RF_SET_SHORT_ADDRESS(int16u_t saddr)
{
	uz2400d_sw(UZS_SADRL, (int8u_t)(saddr&0x00FF));
	uz2400d_sw(UZS_SADRH, (int8u_t)(saddr>>8&0x00FF));
}
/******************************************************************************
 Function: Set 64-Bit Extended Address of this device
 [
 	> eaddr: pointer to extended address of current device
	< none
 ]
 *****************************************************************************/
void RF_SET_MAC_ADDR(const int8u_t* eadr)
{
	uz2400d_sw(UZS_EADR_0, eadr[0]);
	uz2400d_sw(UZS_EADR_1, eadr[1]);
	uz2400d_sw(UZS_EADR_2, eadr[2]);
	uz2400d_sw(UZS_EADR_3, eadr[3]);
	uz2400d_sw(UZS_EADR_4, eadr[4]);
	uz2400d_sw(UZS_EADR_5, eadr[5]);
	uz2400d_sw(UZS_EADR_6, eadr[6]);
	uz2400d_sw(UZS_EADR_7, eadr[7]);
}


/******************************************************************************
 * Function: Initialize the uz2400d RF chip on reset
 *
 *****************************************************************************/
void RF_CHIP_INITIALIZE(void)
{
	//int8u_t i=0;
	uz2400d_sw(UZS_SOFTRST, 0x07);			/* Reset register */
	do {
		uz2400d_sw(UZS_GATECLK, 0x20);		/* Enable SPI synchronous */
	} while((uz2400d_sr(UZS_GATECLK)&0x20)!=0x20);		/* Check status */

	uz2400d_sw(UZS_PACON1, 0x08); //fine-tune TX timing
	uz2400d_sw(UZS_FIFOEN, 0x94); //fine-tune TX timing
	uz2400d_sw(UZS_TXPEMISP, 0x95); //fine-tune TX timing

	//uz2400d_sw(UZS_BBREG3, 0x50);
	//uz2400d_sw(UZS_BBREG5, 0x07);
	uz2400d_sw(UZS_BBREG6, 0x40); //append RSSI value to received packet

	uz2400d_lw(UZL_RFCTRL0, 0x03); //RF optimized control
	uz2400d_lw(UZL_RFCTRL1, 0x02); //RF optimized control
	uz2400d_lw(UZL_RFCTRL2, 0xE6); //RF optimized control
	//uz2400d_lw(UZL_RFCTRL3, 0xF8);

	uz2400d_lw(UZL_RFCTRL4, 0x06); //RF optimized control
	uz2400d_lw(UZL_RFCTRL6, 0x30); //RF optimized control
	uz2400d_lw(UZL_RFCTRL7, 0xE0); //RF optimized control
	uz2400d_lw(UZL_RFCTRL8, 0x8c); //RF optimized control

	uz2400d_lw(UZL_GPIODIR, 0x00); //Setting GPIO to output mode
	uz2400d_lw(UZL_SECCTRL, 0x20); //enable IEEE802.15.4-2006 security support
	uz2400d_lw(UZL_RFCTRL50, 0x07); //for DC-DC off, VDD >= 2.4V
	uz2400d_lw(UZL_RFCTRL51, 0xc0); //RF optimized control
	uz2400d_lw(UZL_RFCTRL52, 0x01); //RF optimized control
	uz2400d_lw(UZL_RFCTRL59, 0x00); //RF optimized control
	uz2400d_lw(UZL_RFCTRL73, 0x40); //RF optimized control, VDD >= 2.4V
	uz2400d_lw(UZL_RFCTRL74, 0xC6); //for DC-DC off, VDD >= 2.4V
	uz2400d_lw(UZL_RFCTRL75, 0x13); //RF optimized control
	uz2400d_lw(UZL_RFCTRL76, 0x07); //RF optimized control

	/* Enable PA */
	uz2400d_lw(UZL_TESTMODE, 0x29);
	uz2400d_lw(UZL_RFCTRL53, 0x09);
	uz2400d_lw(UZL_RFCTRL74, 0x96);
	uz2400d_lw(UZL_RFCTRL3, 0xF8);

	uz2400d_sw(UZS_INTMSK, 0x00); 	//clear all interrupt masks

	RF_NORMAL_SPEED();

	uz2400d_sw(UZS_SOFTRST, 0x02);  //reset baseband

	uz2400d_sw(UZS_RFCTL, 0x04);    //reset RF

	uz2400d_sw(UZS_RFCTL, 0x00);
	uz2400d_sw(UZS_RFCTL, 0x02);
/*
	i=0; while(--i);
	uz2400d_sw(UZS_RFCTL, 0x01);
	i=0; while(--i);
*/
	rf_delay(1);
	uz2400d_sw(UZS_RFCTL, 0x00);
//	uz2400d_sw(UZS_SLPACK, 0x7F);

//	while(uz2400d_lr(UZL_RFSTATE)!=0x50);

	/* Set network parameters */
	uz2400d_sw(UZS_EADR_0, EADR_0);
	uz2400d_sw(UZS_EADR_1, EADR_1);
	uz2400d_sw(UZS_EADR_2, EADR_2);
	uz2400d_sw(UZS_EADR_3, EADR_3);
	uz2400d_sw(UZS_EADR_4, EADR_4);
	uz2400d_sw(UZS_EADR_5, EADR_5);
	uz2400d_sw(UZS_EADR_6, EADR_6);
	uz2400d_sw(UZS_EADR_7, EADR_7);

	uz2400d_sw(UZS_PANIDL, PANIDL);
	uz2400d_sw(UZS_PANIDH, PANIDH);

	uz2400d_sw(UZS_SADRL, SADRL);
	uz2400d_sw(UZS_SADRH, SADRH);
}



/******************************************************************************
 * Function: test recive && transmission of RF module
 *
 *****************************************************************************/
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
