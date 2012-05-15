#ifndef CK_GEN_H
#define CK_GEN_H
//-------------------------------------------------------------------
//                      Clock generation registers
//-------------------------------------------------------------------
#define CG_CTR		0x0
#define CG_HCLK		0x4
#define CG_VINCLK	0x8
#define CG_AUDXCLK	0xC
#define CG_ADCLRCLK	0x10
#define CG_DACLRCLK 0x14
#define CG_VGACLK	0x18
#define CG_AINCLK	0x1C /* bclk */
#define CG_PLLCFG	0x20

#define CTR_SW		(1<<0)
#define CTR_HC    (1<<1)
#define CTR_VIN   (1<<2)
#define CTR_AX    (1<<3)
#define CTR_ALR   (1<<4)
#define CTR_DLR   (1<<5)
#define CTR_VGA   (1<<6)
#define CTR_AIN   (1<<7)
#define CTR_SDR   (1<<9)
//-------------------------------------------------------------------
// Exported functions
//-------------------------------------------------------------------
void ck_enable(unsigned short );
void ck_disable(unsigned short );

#endif //CK_GEN_H
