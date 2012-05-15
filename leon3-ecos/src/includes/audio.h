#ifndef AUDIO_IF_H
#define AUDIO_IF_H
#include "types.h"

//-------------------------------------------------------------------
// Audio input model registers
//-------------------------------------------------------------------
#define AIN_DAC_FIFO    0x00
#define AIN_ADC_FIFO		0x04
#define AIN_CMD					0X08
#define AIN_STATUS			0x0C
/* Bit definition of status reg */
#define DAC_FIFO_FULL 	0x01
#define ADC_FIFO_EMPTY  0x02
/* Bit definition of command reg */
#define FIFO_CLEAR			0x01
#define MODE_I2S				0x02
#define MODE_MS   			1<<2;
//-------------------------------------------------------------------
//              WM8731 Registers
//-------------------------------------------------------------------
#define CD_LLI				(0x00<<9)
#define CD_RLI				(0x01<<9)
#define CD_LHO				(0x02<<9)
#define CD_RHO				(0x03<<9)
#define CD_AAPC       (0x04<<9)
#define CD_DAPC				(0x05<<9)
#define CD_PDCT				(0x06<<9)
#define CD_DAIF				(0x07<<9)
#define CD_SCT				(0x08<<9)
#define CD_ACT				(0x09<<9)
#define CD_RST				(0x0F<<9)
/* Left line in */
#define LLI_LINMUTE   (1<<7)
#define LLI_LRINBOTH  (1<<8)
/* Left headphone out */
#define LHO_LRHPBOTH  (1<<8)
/* Analogue audio path control */
#define AAPC_MICBOOST (1<<0)
#define AAPC_MUTEMIC  (1<<1)
#define AAPC_INSEL		(1<<2)
#define AAPC_BYPASS   (1<<3)
#define AAPC_DACSEL   (1<<4)
#define AAPC_SIDETONE (1<<5)
/* Power down control */
#define PDCT_LINEINPD (1<<0)
#define PDCT_MICPD	  (1<<1)
#define PDCT_ADCPD    (1<<2)
#define PDCT_DACPD    (1<<3)
#define PDCT_OUTPD    (1<<4)
#define PDCT_OSCPD    (1<<5)
#define PDCT_CLKOUTPD (1<<6)
#define PDCT_POWEROFF (1<<7)
/* Digital audio interface format */
#define DAIF_MS				(1<<6)
#define DAIF_LRP      (1<<4)
#define DAIF_LRSWAP   (1<<5)
#define DAIF_BCLKINV  (1<<7)
/* Sampling control */
#define SCT_USB				(1<<0)
#define SCT_BOSR      (1<<1)
#define SCT_CLKIDIV2  (1<<6)
#define SCT_CLKODIV2  (1<<7)
/* Active control */
#define ACT_EN				(1<<0);

/* Function prototype */
void ain_init(void);
void ain_master_enable();
int32u_t adc_fifo_read32(void);
void dac_fifo_write32(unsigned long);

#endif // AUDIO_IF_H
