#include "ck_gen.h"
#include "types.h"
#include "fpga.h"
/********************************************************************
 * Enable clock by setting clock control bit
 *
 * *****************************************************************/
void ck_enable(int16u_t bit_mask)
{
  REG32(CK_GEN_BASE+CG_CTR) |= bit_mask;
}
/********************************************************************
 * Disable clock by setting clock control bit
 *
 * *****************************************************************/
void ck_disable(int16u_t bit_mask)
{
	REG32(CK_GEN_BASE+CG_CTR) &= ~(bit_mask);
}
void ck_switch(void)
{
	REG32(CK_GEN_BASE+CG_CTR) ^= 0x01;
}
//-------------------------------------------------------------------
//
// Disable clock output 
//-------------------------------------------------------------------
void ck_disall()
{
	REG32(CK_GEN_BASE+CG_CTR) = 0x003;
}
void ck_enall()
{
	REG32(CK_GEN_BASE+CG_CTR) = 0x2BB;
}
/*
 * set ck of vin module
 *
 */
void ck_vin_div(unsigned short div_val)
{
	REG32(CK_GEN_BASE+CG_VGACLK) = div_val;
}

