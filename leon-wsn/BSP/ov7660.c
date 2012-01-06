#include "ov7660.h"
#include "tools.h"

/*for FATFS*/
#include "sd.h"
#include "FAT32.h"
#include "ff.h"
#include "diskio.h"

// OV7660 寄存器配置参数
const int8u_t reg_conf1[][2] =
{
	{ 0x00 , 0x50 },//COM1
	{ 0x01 , 0x80 },//COM1
	{ 0x02 , 0x80 },//COM1
	//{ 0x03 , 0b11101111 },//COM1
	{ 0x03 , 0xef },//COM1
	{ 0x04 , 0x00 },//COM1
	{ 0x09 , 0x03 },//COM2
	{ 0x10 , 0x40 },//
	{ 0x0C , 0x04 },//COM3 0x42=MSB LSB swap
	//{ 0x0C , 0x42 },//COM3 0x42=MSB LSB swap
	{ 0x0F , 0xcB },//COM6

	{ 0x11 , 0x2f },//CLKRC  24/(7+1) PCLK
	{ 0x12 , 0x14 }, //COM7  QVGA RGB
	{ 0x13 , 0x8F }, //COM8
	{ 0x14 , 0x7a },//COM9
	//{ 0x15 , 0x31 }, //COM10  PCLK reverse
	{ 0x15 , 0x31 }, //COM10  PCLK reverse

	{ 0x24 , 0xa0 }, //AEW
	{ 0x25 , 0x75 }, //AEB
	{ 0x37 , 0x00 }, //
	{ 0x3D , 0x00 }, //
	{ 0x40 , 0xd0 }, //COM15  RGB565 out
	{ 0x69 , 0x50 }, //HV
	{ 0x27 , 0x85 }, //HV
	{ 0x28 , 0xfd}, //HV
	{ 0x41 , 0x20 }, //HV
	{ 0x20 , 0xff } ,//COM7  QVGA RGB

	{0x70, 0x00},//00
	{0x71, 0x80}//0x85测试
};

//王广福的
const int8u_t reg_conf[][2]={
	  	{0x3a, 0x04},
		{0x40, 0x10},
		{0x12, 0x14},
		{0x32, 0x80},
		{0x17, 0x16},

		{0x18, 0x04},
		{0x19, 0x03},
		{0x1a, 0x7b},
		{0x03, 0x06},
		{0x0c, 0x0c},

		{0x3e, 0x00},


		//test mode
		{0x70, 0x00},
		{0x71, 0x80},

		{0x72, 0x11},
		{0x73, 0x09},

		{0xa2, 0x02},
		{0x11, 0x2f},//PCLK,24/48=500KHZ
		{0x7a, 0x20},
		{0x7b, 0x1c},
		{0x7c, 0x28},

		{0x7d, 0x3c},
		{0x7e, 0x55},
		{0x7f, 0x68},
		{0x80, 0x76},
		{0x81, 0x80},

		{0x82, 0x88},
		{0x83, 0x8f},
		{0x84, 0x96},
		{0x85, 0xa3},
		{0x86, 0xaf},

		{0x87, 0xc4},
		{0x88, 0xd7},
		{0x89, 0xe8},
		{0x13, 0xe0},
		{0x00, 0x00},//AGC

		{0x10, 0x00},
		{0x0d, 0x00},
		{0x14, 0x20},//0x38, limit the max gain
		{0xa5, 0x05},
		{0xab, 0x07},

		{0x24, 0x75},
		{0x25, 0x63},
		{0x26, 0xA5},
		{0x9f, 0x78},
		{0xa0, 0x68},

		{0xa1, 0x03},//0x0b,
		{0xa6, 0xdf},//0xd8,
		{0xa7, 0xdf},//0xd8,
		{0xa8, 0xf0},
		{0xa9, 0x90},

		{0xaa, 0x94},//50
		{0x13, 0xe5},
		{0x0e, 0x61},
		{0x0f, 0x4b},
		{0x16, 0x02},

		{0x1e, 0x07},//0x07,
		{0x21, 0x02},
		{0x22, 0x91},
		{0x29, 0x07},
		{0x33, 0x0b},

		{0x35, 0x0b},//60
		{0x37, 0x1d},
		{0x38, 0x71},
		{0x39, 0x2a},
		{0x3c, 0x78},

		{0x4d, 0x40},
		{0x4e, 0x20},
		{0x69, 0x5d},
		{0x6b, 0x40},//PLL
		{0x74, 0x19},
		{0x8d, 0x4f},

		{0x8e, 0x00},//70
		{0x8f, 0x00},
		{0x90, 0x00},
		{0x91, 0x00},
		{0x92, 0x00},//0x19,//0x66

		{0x96, 0x00},
		{0x9a, 0x80},
		{0xb0, 0x84},
		{0xb1, 0x0c},
		{0xb2, 0x0e},

		{0xb3, 0x82},//80
		{0xb8, 0x0a},
		{0x43, 0x14},
		{0x44, 0xf0},
		{0x45, 0x34},

		{0x46, 0x58},
		{0x47, 0x28},
		{0x48, 0x3a},
		{0x59, 0x88},
		{0x5a, 0x88},

		{0x5b, 0x44},//90
		{0x5c, 0x67},
		{0x5d, 0x49},
		{0x5e, 0x0e},
		{0x64, 0x04},
		{0x65, 0x20},

		{0x66, 0x05},
		{0x94, 0x04},
		{0x95, 0x08},
		{0x6c, 0x0a},
		{0x6d, 0x55},

		{0x4f, 0x80},
		{0x50, 0x80},
		{0x51, 0x00},
		{0x52, 0x22},
		{0x53, 0x5e},
		{0x54, 0x80},

		//{0x54, 0x40},//110
		{0x6e, 0x11},//100
		{0x6f, 0x9f},//0x9e for advance AWB
	    {0x55, 0x00},//亮度
	    {0x56, 0x40},//对比度
	    {0x57, 0x80}//0x40,  change according to Jim's request
};

//pic18项目
const int8u_t reg_conf2[][2] =
{
    {0x00, 0x00}, // GAIN
    {0x01, 0x00}, // BLUE
    {0x02, 0x00}, // RED
    {0x03, 0x00}, // VREF
    {0x04, 0x00}, // COM1
    {0x09, 0x03}, // COM2
    {0x0c, 0x04}, // COM3
    {0x0f, 0xcb}, // COM6

    {0x10, 0x40}, // AECH
    {0x11, 0x2f}, // CLKRC 500KH
    {0x12, 0x14}, // COM7  QVGA RGB
    {0x13, 0x8f}, // COM8
    {0x14, 0x4a}, // COM9

    {0x15, 0x20}, // COM10

    {0x20, 0xff}, // BOS
    {0x24, 0x30}, // AEW
    {0x25, 0x85}, // AEB

    {0x27, 0x95}, // BBIAS
    {0x28, 0xe0}, // GbBIAS
    {0x32, 0x00}, // HREF
    {0x37, 0x00}, // ADC
    {0x3d, 0x00}, // COM13
    {0x39, 0x57}, // OFON
    {0x40, 0xd0}, // COM15 RGB565 out
    {0x41, 0x20}, // COM16
    {0x43, 0xf0},
    {0x44, 0x10},
    {0x45, 0x78},
    {0x46, 0xa8},
    {0x47, 0x60},
    {0x48, 0x80},
    {0x4F, 0x80}, // MTX1
    {0x50, 0x86}, // MTX2
    {0x51, 0x06}, // MTX3
    {0x52, 0x20}, // MTX4
    {0x53, 0x50}, // MTX5
    {0x54, 0x70}, // MTX6
    {0x59, 0xba},
    {0x5a, 0x9a},
    {0x5b, 0x22},
    {0x5c, 0xb9},
    {0x5d, 0x9b},
    {0x5e, 0x10},
    {0x5f, 0xe0},
    {0x60, 0x85},
    {0x61, 0x60},
    {0x69, 0x50}, // HV
    {0x6c, 0x40}, // GSP
    {0x6d, 0x30},
    {0x6e, 0x4b},
    {0x6f, 0x60},

    //{0x70, 0x70},
    //{0x71, 0x70},
    {0x70, 0x00},
    {0x71, 0x80},
/*
    {0x72, 0x70},
    {0x73, 0x70},
    {0x74, 0x60},
    {0x75, 0x60},
    {0x76, 0x50},
    {0x77, 0x48},
    {0x78, 0x3a},
    {0x79, 0x2e},
    {0x7a, 0x28},
    {0x7C, 0x04},
    {0x7d, 0x07},
    {0x7e, 0x10},
    {0x7f, 0x28},
    {0x80, 0x36},
    {0x81, 0x44},
    {0x82, 0x52},
    {0x83, 0x60},
    {0x84, 0x6c},
    {0x85, 0x78},
    {0x86, 0x8c},
    {0x87, 0x9e},
    {0x88, 0xbb},
    {0x89, 0xd2},
    {0x8a, 0xe6},
    {0x9f, 0x9d},
    {0xa0, 0xa0},
    {0xa1, 0x07}
    */
};

/*
 *
 *
 *
 */
int8u_t write_OV7660_reg(int8u_t reg_addr, int8u_t reg_data) {
	SCCB_start();
	// 写操作指令
	if (SCCB_NACK == SCCB_write_byte(OV7660_WR)) {
		SCCB_stop();
		return FAIL;
	}
	// 寄存器地址
	if (SCCB_NACK == SCCB_write_byte(reg_addr)) {
		SCCB_stop();
		return FAIL;
	}
	// 寄存器数据
	if (SCCB_NACK == SCCB_write_byte(reg_data)) {
		SCCB_stop();
		return FAIL;
	}
	SCCB_stop();
	return PASS;
}

/*
 *
 *
 *
 */
int8u_t read_OV7660_reg(int8u_t reg_addr, int8u_t *reg_ptr) {
	SCCB_start();
	// 写操作指令
	if (SCCB_NACK == SCCB_write_byte(OV7660_WR)) {
		SCCB_stop();
		return FAIL;
	}
	// 寄存器地址
	if (SCCB_NACK == SCCB_write_byte(reg_addr)) {
		SCCB_stop();
		return FAIL;
	}
	SCCB_stop();
	SCCB_start();
	// 读操作指令
	if (SCCB_NACK == SCCB_write_byte(OV7660_RD)) {
		SCCB_stop();
		return FAIL;
	}
	// 读取寄存器
	*reg_ptr = SCCB_read_byte(SCCB_NACK);
	SCCB_stop();
	return PASS;
}

/*
 *
 *
 *
 */
void OV7660_conf_window(int16u_t startx, int16u_t starty, int16u_t width,
		int16u_t height) {
	int8u_t reg_temp;
	int16u_t endx, endy;

	endx = (startx + width);
	endy = (starty + height + height);

	// 水平像素设置
	read_OV7660_reg(0x32, &reg_temp);
	reg_temp = (int8u_t) ((reg_temp & 0xc0) | ((endx & 0x7) << 3) | (startx
			& 0x7));

	write_OV7660_reg(0x32, reg_temp);
	write_OV7660_reg(0x17, (int8u_t) (startx >> 3));
	write_OV7660_reg(0x18, (int8u_t) (endx >> 3));

	// 垂直像素设置
	read_OV7660_reg(0x03, &reg_temp);
	reg_temp = (int8u_t) ((reg_temp & 0xf0) | ((endy & 0x3) << 2) | (starty
			& 0x3));

	write_OV7660_reg(0x03, reg_temp);
	write_OV7660_reg(0x19, (int8u_t) (starty >> 2));
	write_OV7660_reg(0x1a, (int8u_t) (endy >> 2));
}

//(140,16,640,480) is good for VGA
//(272,16,320,240) is good for QVGA
/* config_OV7660_window */
void OV7660_config_window(int32u_t startx,int32u_t starty,int32u_t width, int32u_t height)
{
	int32u_t endx=(startx+width);
	int32u_t endy=(starty+height*2);// "v*2"必须
	int8u_t temp_reg1, temp_reg2;
	int8u_t state,temp;

	state = read_OV7660_reg(0x03, &temp_reg1 );
	temp_reg1 &= 0xC0;
	state = read_OV7660_reg(0x32, &temp_reg2 );
	temp_reg2 &= 0xC0;

	// Horizontal
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	state = write_OV7660_reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	state = write_OV7660_reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	state = write_OV7660_reg(0x18, temp );

	// Vertical
	temp = temp_reg1|((endy&0x7)<<3)|(starty&0x7);
	state = write_OV7660_reg(0x03, temp );
	temp = (starty&0x7F8)>>3;
	state = write_OV7660_reg(0x19, temp );
	temp = (endy&0x7F8)>>3;
	state = write_OV7660_reg(0x1A, temp );
}

/*
 *
 *
 *
 */
int8u_t OV7660_init(void) {
	int8u_t i;
	volatile int16u_t time;
	int8u_t pid, ver;
	int32u_t regs_cnt = 0;

	/*
	 * set pins
	 * SCCB:SCL,SDL,out
	 * D0-D7:in
	 * pclk:in
	 * xclk_enable:OV7660_XCLK_EN,PORTE[0],out
	 * VSYNC:in
	 * HREF:in
	 */
	gpio_make_out(PORTB, SCCB_CLK);
	gpio_make_out(PORTB, SCCB_DATA);
	gpio_make_in(PORTA, 0xff);
	gpio_make_in(PORTB, OV7660_PCLK);
	gpio_make_in(PORTB, OV7660_VSYNC);
	gpio_make_in(PORTB, OV7660_HREF);
	gpio_make_out(PORTE, OV7660_XCLK_EN);

	// 设备输入时钟
	XCLK_EN();
	delayus(10);

	// 寄存器  复位
	if (FAIL == write_OV7660_reg(0x12, 0x80)) {
		return FAIL;
	}
	delayus(10);

	// 设备PID 检查
	printf("-----------------------------------\n");
	for (i = 0; i < 1; i++) {
		pid = 0;
		if (FAIL == read_OV7660_reg(0x0a, &pid)) {
			printf("REAd 0x0A error!\n");
		} else {
			printf("REG[OA](76)=%2x\n", pid);
		}
	}
	printf("-----------------------------------\n");
	for (i = 0; i < 1; i++) {
		ver = 0;
		if (FAIL == read_OV7660_reg(0x0b, &ver)) {
			printf("Read 0x0A error!\n");
		} else {
			printf("REG[0B](73)=%2x\n", ver);
		}
	}

	// 寄存器  配置
	printf("start write regs\n");
	regs_cnt = sizeof(reg_conf) / sizeof(reg_conf[0]);
	for (i = 0; i < regs_cnt; i++) {
		if (FAIL == write_OV7660_reg(reg_conf[i][0], reg_conf[i][1])) {
			printf("*");//error
		} else {
			printf("#");//success
		}
	}
	printf("\n");
	OV7660_conf_window(272, 16, OV7670_ROW_SIZE, OV7670_COL_SIZE);
	 //OV7660_config_window(272,16,320,240);
	printf("write %d regs ok!\n",i);
	//read registers
	printf("read regs testing...\n");
	printf("-----------------------------------\n");
	regs_cnt = sizeof(reg_conf) / sizeof(reg_conf[0]);
	for (i = 0; i < regs_cnt; i++) {
		ver = 0;
		if (FAIL == read_OV7660_reg(reg_conf[i][0], &ver)) {
			printf("Read REG[%4x] error!\n",reg_conf[i][0]);
		} else {
			printf("REG[%4x](%4x)=%4x\n",reg_conf[i][0], reg_conf[i][1] ,ver);
		}
	}
	printf("-----------------------------------\n");
}

//---------------------------------------------------------------
// 每行像素数据缓冲
extern int8u_t pix_buf[OV7670_COL_SIZE * OV7670_ROW_SIZE * 2];
//---------------------------------------------------------------
void OV7660_work() {
	// 每帧行数次数记录
	int32u_t data_cnt;
	// 像素数据储存索引
	int32u_t pix_idx;

	//int8u_t** data = pix_buf;
	int8u_t* file_data = NULL;
	int8u_t* pos = pix_buf;
	int32u_t data_size = OV7670_COL_SIZE * OV7670_ROW_SIZE * 2;
	int32u_t cols = OV7670_ROW_SIZE*2;

	//for write file
	FATFS myfs;
	FATFS *fs = &myfs;
	FRESULT rc;
	FILE fd;
	UINT len, result;
	char *file_name = "cap.raw";

	/* Working buffer */
	int i = 0;
	int j = 0;
	int32u_t count = 0;
	data_cnt = 0;

	printf("READA TEST:\n");
	for (i = 0; i < 64; i++) {
		printf("%2x ", (int8u_t) gpio_read_group(PORTA));
	}
	printf("\n");

	{
		printf("Capture image...\n");
		// 320*240 图像
		while (1== SYNC_PIN_RD())
			;
		while (0== SYNC_PIN_RD())
			;
		led_on(LEDG1);

		pos = pix_buf;
		/*scan evey line*/
		for (data_cnt = 0; data_cnt < OV7670_COL_SIZE; data_cnt++) {
			/*wait for HREF low-high signal transition*/
			while (0 == HREF_PIN_RD())
				;
			/*get a line*/
			for (j = 0; j < cols; j++) {
				while (0 == PCLK_PIN_RD())
					;
				pos[j] = gpio_read_group(PORTA);
				while (1 == PCLK_PIN_RD())
					;
			}
			pos = pos + cols;
			while (1 == HREF_PIN_RD())
				;
		}
	}

	led_off(LEDG1);

	/*write data*/
	printf("Capture image over,saving file...\n");
	/*mount fs*/
	rc = f_mount(0, fs);
	put_rc(rc);

	/*open file*/
	printf("Creating:\"%s\"\n", file_name);
	rc = f_open(&fd, file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if (rc) {
		put_rc(rc);
		return;
	}

	printf("Writing \"%s\"\n", file_name);
	count = 0;
	len = 0;
	file_data = pix_buf;
	for (count = 0; count < data_size - 512;) {
		/*写入数据，长度512*/
		rc = f_write(&fd, (file_data + count), 512, &result);
		count += result;
		if (rc || result < len) {
			/* error or disk full */
			printf("*");
			//break;
		} else {
			printf("#");
		}
	}

	/*写入最后一帧数据，长度可能小于512*/
	rc = f_write(&fd, file_data + count, data_size - count, &result);
	count += result;
	if (rc || result < len) {
		printf("*");
		//break;
		/* error or disk full */
	} else {
		printf("#");
	}

	printf("\n%lu/%lu bytes written.\nWrite file over!\n", count, data_size);
	f_close(&fd);

	return PASS;
}

void ov7070_pin_test() {
	/*
	 * set pins
	 * SCCB:SCL,SDL,out
	 * D0-D7:in
	 * pclk:in
	 * xclk_enable:OV7660_XCLK_EN,PORTE[0],out
	 * VSYNC:in
	 * HREF:in
	 */
	gpio_make_out(PORTB, SCCB_CLK);
	gpio_make_out(PORTB, SCCB_DATA);
	gpio_make_in(PORTA, 0xff);
	gpio_make_in(PORTB, OV7660_PCLK);
	gpio_make_in(PORTB, OV7660_VSYNC);
	gpio_make_in(PORTB, OV7660_HREF);
	gpio_make_out(PORTE, OV7660_XCLK_EN);

	// 设备输入时钟
	XCLK_EN();
	printf("read PORTA,PCLK,VSYNC,HREF:%4x,%4x,%4x,%4x\n",
			gpio_read_group(PORTA), PCLK_PIN_RD(), SYNC_PIN_RD(),
			HREF_PIN_RD());
}
