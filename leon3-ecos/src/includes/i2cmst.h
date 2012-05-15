//===================================================================
//                      i2cmst.h
//           Hardware abstraction of I2C master I/F
//            Jason Liang: co.liang.ol@gmail.com
//===================================================================
#ifndef I2CMST_H
#define I2CMST_H
//-------------------------------------------------------------------
//                  Bit Definitions of Regs
//-------------------------------------------------------------------

/* Control register */
#define CTR_EN         (1<<7)
#define CTR_IEN				 (1<<6)
/* Command register */
#define CR_STA				 (1<<7)
#define CR_STO				 (1<<6)
#define CR_RD					 (1<<5)
#define CR_WR					 (1<<4)
#define CR_ACK				 (1<<3)
#define CR_IACK				 (1<<0)
/* Status register */
#define SR_RxACK			 (1<<7)
#define SR_BUSY				 (1<<6)
#define SR_AL					 (1<<5)
#define SR_TIP				 (1<<1)
#define SR_IF					 (1<<0)
//-------------------------------------------------------------------
//                  Global Declarations
//-------------------------------------------------------------------
struct i2cmst {
	volatile unsigned long prer;   /* Prescale register */
	volatile unsigned long ctr;    /* Control register */
	volatile unsigned long xr;    /* Transmit and receive register */
	volatile unsigned long csr;   /* Command and status register */
};

#define SLVADDR				0x1A
void i2c_init();
int i2c_write(unsigned char slvaddr, unsigned char* buf, \
							int len);

#endif // I2CMST_H 
