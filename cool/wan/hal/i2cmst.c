//===================================================================
//                            i2cmst.c
//           Hardware abstraction of I2C master I/F
//            Jason Liang: co.liang.ol@gmail.com
//===================================================================

#include "i2cmst.h"
#include "fpga.h"
#include <cyg/infra/cyg_type.h>

//#define PRESCALE 0x63
#define PRESCALE 0x31
void i2c_init()
{
	struct i2cmst* regs = (struct i2cmst*)I2CMST_BASE;
	printf("%p\n", regs);
	/* Sanity check the i2c core */
	if(regs->prer!=0xffff) {
		printf("Error reading prescale register reset value %ld\n", \
			regs->prer);
	}
	/* Setup clock, Prescale=50M/(5x100k)-1=0x63 */
  regs->prer = PRESCALE;
	if(regs->prer!=PRESCALE) {
		printf("Unable to set prescale register.\n");
	}
	/* Enable I2C master core */
	regs->ctr = CTR_EN;
}
int i2c_write(unsigned char slvaddr, unsigned char* buf, \
							int len) 
{
	struct i2cmst* regs = (struct i2cmst*)I2CMST_BASE;
	unsigned char addr = slvaddr<<1;
	int ret = -1;
	/* Set address */
	regs->xr = addr;
	/* STA & WR */
	regs->csr = CR_STA|CR_WR;
	while(regs->csr&SR_TIP);
	if((regs->csr&SR_RxACK)) return ret;
	//printf("Sent out %#x\n", addr);
	ret++;
	/* Write data */
	//*
	regs->xr = *buf++;
	regs->csr = CR_WR;
	while(regs->csr&SR_TIP);
	if((regs->csr&SR_RxACK)) return ret;
	ret++;

	regs->xr = *buf++;
	regs->csr = CR_WR|CR_STO;
	while(regs->csr&SR_TIP);
	if((regs->csr&SR_RxACK)) return ret;
	ret++;
	// */
	/*
	while(len--) {
		regs->xr = (buf!=NULL) ? *buf : 0;
	  regs->csr = CR_WR;	
		while(regs->csr&SR_TIP);
		if((regs->csr&SR_RxACK)) break;
		printf("Sent out %#x\n", (buf!=NULL)?*buf:0);
		buf++;
		ret++;
	}
	regs->csr = CR_STO;
	while(regs->csr&SR_TIP);
//	*/
	return ret;
}
