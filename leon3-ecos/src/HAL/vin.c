#include "includes.h"

void vin_delay(int32u_t i){
	int j = 0;
	while(i--){
		for(j = 0; j <1024; j++)
			;
	}
}

inline void vin_set_window(int16u_t w,int16u_t h)
{
	//REG32(VIN_BASE+VIN_DIM) = 0;
	REG32(VIN_BASE+VIN_DIM) = ((w << 16) | h);
}

/*
 * set start address of fifo buffer
 */
inline void vin_set_sad(unsigned long addr)
{
	REG32(VIN_BASE+VIN_SAD) = addr;
}
/*
 * set end address of fifo buffer
 * ead = sad + frame_size 
 */
inline void vin_set_ead(unsigned long addr)
{
	REG32(VIN_BASE+VIN_EAD) = addr;
}

inline void vin_en_set()
{
	/*cfg enable*/
	REG32(VIN_BASE + VIN_CFG) |= 0x0200;
}


/*with your own init code*/
/* for ov7670,RGB565 out, 240*320*2 */
int8u_t pix_buf[OV7670_COL_SIZE*OV7670_ROW_SIZE*2];

//extern int8u_t pix_buf[];

void show_pix_data(){
	int i = 0;
	printf("-----------------------------\n");
	for(i =0; i<64; i++){
		dprintf("%02x ",pix_buf[i]);
		if(i%8 == 7)
			dprintf("\n");
	}
	printf("-----------------------------\n");
}

inline void vin_init(void)
{	
	show_pix_data();
	dprintf("befor,sad=[%08x],ead=[%08x],pix_buf=[%08x],dim=[%08x],cfg=[%08x]\n",
		REG32(VIN_BASE+VIN_SAD),
		REG32(VIN_BASE+VIN_EAD),
		(unsigned long)pix_buf,
		REG32(VIN_BASE+VIN_DIM),
		REG32(VIN_BASE+VIN_CFG));
	/*
   		cfg_en=0;cfg_de_en=1;cfg_pr_hs=1'b1;cfg_pr_vs=1'b1;
   		cfg_pr_de=1'b1;cfg_fskip=5'd7;
   		cfg_dstadr=32'h1000;
   		cfg_resadr=32'hF000;
   		cfg_height=16'd240;
   		cfg_width =16'd320;
		cfg_cfmt  =3'd5;
	*/
	//RGB565 format
	//#if USE_OV7670
	//REG32(VIN_BASE + VIN_CFG) = 0x04E7;
	//#elif USE_D5M
	REG32(VIN_BASE + VIN_CFG) = 0x14E7;
	//#endif
	vin_set_window(OV7670_ROW_SIZE,OV7670_COL_SIZE);
	vin_set_sad((unsigned long)pix_buf);
	vin_set_ead(pix_buf + OV7670_COL_SIZE*OV7670_ROW_SIZE*2);
	vin_delay(50);
	
	dprintf("after,sad=[%08x],ead=[%08x],pix_buf=[%08x],dim=[%08x],cfg=[%08x]\n",
		REG32(VIN_BASE+VIN_SAD),
		REG32(VIN_BASE+VIN_EAD),
		(unsigned long)pix_buf,
		REG32(VIN_BASE+VIN_DIM),
		REG32(VIN_BASE+VIN_CFG));
	vin_en_set();
	show_pix_data();
}


