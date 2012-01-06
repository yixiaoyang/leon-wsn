/*
 * xiaoyang yi
 * driver for SD CARD
 * 2011.11.1
 *
 * this is part of Graduation Thesis of xiaoyang.yi
 */

#include "sd.h"
#include "ff.h"
#include "diskio.h"

BUFFER_TYPE sectorBuffer; //512 bytes for sector buffer
//--------------------------------------------------------------
unsigned int readPos = 0;
unsigned char sectorPos = 0;
unsigned char LBA_Opened = 0; //Set to 1 when a sector is opened.
unsigned char Init_Flag; //Set it to 1 when Init is processing.

//extern unsigned int spi3_delay_time ;
extern int16u_t spi2_delay_time;

static void delayus(unsigned int num) {
	unsigned int i = num >> 4;
	while ((i--) > 0) {
		;
	}
}

//------------------------------------------------------------------------
// 	function:	init io ports for SD card
//	date:		2011.3.16
// 	note:
//------------------------------------------------------------------------
void sd_init() {
	spi2_init();
}

//------------------------------------------------------------------------
// 	function:	command send
//	date:		2011.3.16
// 	note:		when Set as SPI mode,the SD card will ignore CRC check
//------------------------------------------------------------------------
//	function:	Send a Command to MMC/SD-Card
//	Return: 	the second byte of response register of MMC/SD-Card
//------------------------------------------------------------------------
unsigned char Write_Command_MMC(unsigned char cmd, unsigned long arg) {
	unsigned char tmp;
	unsigned char retry = 0; //重复操作次数

	//SPI3_DESELECT();		//set MMC_Chip_Select to high (MMC/SD-Card disable)
	spi2_8TxRx(0xFF); //send 8 Clock Impulse
	//SPI3_SELECT(); 		//set MMC_Chip_Select to low (MMC/SD-Card active)

	spi2_8TxRx(cmd | 0x40); //分别写入命令
	spi2_8TxRx(arg >> 24); //数据段第4字节
	spi2_8TxRx(arg >> 16); //数据段第3字节
	spi2_8TxRx(arg >> 8); //数据段第2字节
	spi2_8TxRx(arg); //数据段第1字节
	spi2_8TxRx(0x95); //CRC效验和

	//get 16 bit response
	spi2_8TxRx(0xff); //read the first byte,ignore it.
	do {
		//Only last 8 bit is used here.Read it out.
		tmp = spi2_8TxRx(0xff);
		retry++;
		if (retry >= 128) {
			//if (SD_DEBUG)
			printf("Get Command Response error,code:0x%x\n", tmp);
			return READ_BLOCK_ERROR;
		}
	} while (tmp == 0xff);

	return tmp; //return status
}

//------------------------------------------------------------------------
//	Routine for Init MMC/SD card(SPI-MODE)
//------------------------------------------------------------------------
unsigned char MMC_Init() {
	unsigned char retry;
	unsigned char temp;
	unsigned char i;

	//spi3_delay_time = 1024;//low speed
	spi2_delay_time = SPI2_LOW;

	sd_init();//Init SPI port
	delayus(8);

	Init_Flag = 1; //Set the init flag
	for (i = 0; i < 100; i++) {
		spi2_8TxRx(0xff); //send 74 clock at least!!!
	}

	//Send Command CMD0 to MMC/SD Card
	retry = 0;
	do {
		//retry 200 times to send CMD0 command
		temp = Write_Command_MMC(0, 0);
		retry++;
		if (retry == 256) {
			//time out
			if (SD_DEBUG)
				printf("error:CMD0 send time out!\r\n");
			//spi3_delay_time= SPI3_HIGH;//high speed
			spi2_delay_time = SPI2_HIGH;
			return (INIT_CMD0_ERROR);//CMD0 Error!
		}
	} while (temp != 1);

	for (i = 0; i < 16; i++) {
		spi2_8TxRx(0xff); //send 74 clock at least!!!
	}

	//Send Command CMD1 to MMC/SD-Card
	retry = 0;
	do {
		//retry 100 times to send CMD1 command
		temp = Write_Command_MMC(1, 0);
		delayus(32);
		retry++;
		if (retry == 256) {
			//time out
			if (SD_DEBUG)
				printf("error:CMD1 send time out!\r\n");
			spi2_delay_time = SPI2_HIGH;
			return (INIT_CMD1_ERROR);//CMD1 Error!
		}
	} while (temp != 0);

	Write_Command_MMC(59, 0); //disable CRC
	Write_Command_MMC(16, 512); //set sector size to 512

	Init_Flag = 0; //Init is completed,clear the flag

	//SPI3_DESELECT();	//set MMC_Chip_Select to high
	spi2_delay_time = SPI2_HIGH;
	return (0x55); //All commands have been taken.
}

//------------------------------------------------------------------------
//	Routine for Init MMC/SD card(SPI-MODE)
//------------------------------------------------------------------------


//------------------------------------------------------------------------
//	function:	Routine for reading data Registers of MMC/SD-Card
//	Return:		0 if no Error.
//	Params:
//------------------------------------------------------------------------
unsigned char MMC_Read_Block(unsigned char CMD, unsigned long arg,
		unsigned char *Buffer, unsigned int Bytes) {
	unsigned int i;
	unsigned retry, temp;

	//Send Command CMD to MMC/SD-Card
	retry = 0;
	//SPI3_SELECT();

	do {
		temp = Write_Command_MMC(CMD, arg);
		retry++;
		if (retry == 128) //Retry 100 times to send command.
		{
			//if (SD_DEBUG)
			printf("Error Read Block,error code:0x%x\n", temp);
			return (READ_BLOCK_ERROR); //block write Error!
		}
	} while (temp != 0);

	//Read Start Byte form MMC/SD-Card (FEh/Start Byte)
	while (spi2_8TxRx(0xff) != 0xfe)
		;

	//Write blocks(normal 512Bytes) to MMC/SD-Card
	for (i = 0; i < Bytes; i++) {
		*Buffer++ = spi2_8TxRx(0xff);
	}

	//CRC-Byte
	spi2_8TxRx(0xff);//CRC - Byte
	spi2_8TxRx(0xff);//CRC - Byte

	//set MMC_Chip_Select to high (MMC/SD-Card invalid)
	//SPI3_DESELECT();
	return (0);
}

//------------------------------------------------------------------------
//Routine for reading CSD Registers from MMC/SD-Card (16Bytes)
//Return 0 if no Error.
unsigned char Read_CSD_MMC(unsigned char *Buffer)
//------------------------------------------------------------------------
{
	//Command for reading CSD Registers
	unsigned char temp;
	temp = MMC_Read_Block(9, 0, Buffer, 16); //read 16 bytes

	return (temp);
}

//------------------------------------------------------------------------
//Routine for reading CID Registers from MMC/SD-Card (16Bytes)
//Return 0 if no Error.
unsigned char Read_CID_MMC(unsigned char *Buffer)
//------------------------------------------------------------------------
{
	//Command for reading CID Registers
	//unsigned char CMD[] = {0x4A,0x00,0x00,0x00,0x00,0xFF};
	unsigned char temp;
	temp = MMC_Read_Block(10, 0, Buffer, 16); //read 16 bytes

	return (temp);
}

//------------------------------------------------------------------------
//	returns:
// 		size of the card in MB ( ret * 1024^2) == bytes
// 		sector count and multiplier MB are in u08 == C_SIZE / (2^(9-C_SIZE_MULT))
// 		name of the media
//------------------------------------------------------------------------
void MMC_get_volume_info(void) {
	unsigned char i;
	int temp = 0;
	VOLUME_INFO_TYPE MMC_volume_Info, *vinf;
	//if(SD_DEBUG) printf("SD CARD Information Read Begin\n");
	vinf = &MMC_volume_Info; //Init the pointoer;

	//SPI3_SELECT();
	// read the CSD register
	temp = Read_CSD_MMC(sectorBuffer.dat);
	if (temp != 0) {
		if (SD_DEBUG)
			printf("Error:Read CSD Registers!\r\n");
	}

	// get the C_SIZE value. bits [73:62] of data
	// [73:72] == sectorBuffer.data[6] && 0x03
	// [71:64] == sectorBuffer.data[7]
	// [63:62] == sectorBuffer.data[8] && 0xc0
	vinf->sector_count = sectorBuffer.dat[6] & 0x03;
	vinf->sector_count <<= 8;
	vinf->sector_count += sectorBuffer.dat[7];
	vinf->sector_count <<= 2;
	vinf->sector_count += (sectorBuffer.dat[8] & 0xc0) >> 6;

	// get the val for C_SIZE_MULT. bits [49:47] of sectorBuffer.data
	// [49:48] == sectorBuffer.data[5] && 0x03
	// [47]    == sectorBuffer.data[4] && 0x80
	vinf->sector_multiply = sectorBuffer.dat[9] & 0x03;
	vinf->sector_multiply <<= 1;
	vinf->sector_multiply += (sectorBuffer.dat[10] & 0x80) >> 7;

	// work out the MBs
	// mega bytes in u08 == C_SIZE / (2^(9-C_SIZE_MULT))
	vinf->size_MB = vinf->sector_count >> (9 - vinf->sector_multiply);
	//if(SD_DEBUG) printf("The Total Size:%d MB\n",vinf->size_MB);
	// get the name of the card

	Read_CID_MMC(sectorBuffer.dat);
	vinf->name[0] = sectorBuffer.dat[3];
	vinf->name[1] = sectorBuffer.dat[4];
	vinf->name[2] = sectorBuffer.dat[5];
	vinf->name[3] = sectorBuffer.dat[6];
	vinf->name[4] = sectorBuffer.dat[7];
	vinf->name[5] = 0x00; //end flag

	//if(SD_DEBUG) printf("SD Card Name:%s\n",vinf->name);

}

//------------------------------------------------------------------------
//	Routine for writing a Block(512Byte) to MMC/SD-Card
//	Return 0 if sector writing is completed.
//	Note:ther interface will surpport the FAT32/FAT16 filesystem
//------------------------------------------------------------------------
unsigned char MMC_write_sector(unsigned long addr, unsigned char *Buffer) {
	unsigned char tmp, retry;
	unsigned int i;
	//Command 24 is a writing blocks command for MMC/SD-Card.
	//unsigned char CMD[] = {0x58,0x00,0x00,0x00,0x00,0xFF};

	retry = 0;
	//SPI3_SELECT();

	do {
		//Retry 100 times to send command.
		tmp = Write_Command_MMC(24, addr << 9); //write Command
		retry++;
		if (retry == 100) {
			if (SD_DEBUG)
				printf("error code:0x%x\n", tmp);
			return (tmp); //send commamd Error!
		}
	} while (tmp != 0);

	//Before writing,send 100 clock to MMC/SD-Card
	for (i = 0; i < 100; i++) {
		spi2_8TxRx(0xff);
	}

	//Send Start Byte to MMC/SD-Card
	spi2_8TxRx(0xFE);

	//Now send real data Bolck (512Bytes) to MMC/SD-Card
	for (i = 0; i < 512; i++) {
		//if(SD_DEBUG) printf("Write:%d\n",*Buffer);
		spi2_8TxRx(*Buffer++); //send 512 bytes to Card
	}

	//CRC-Byte
	//spi2_8TxRx(0xFF); //Dummy CRC
	//spi2_8TxRx(0xFF); //CRC Code
	spi2_8TxRx(0x95);
	spi2_8TxRx(0x95); //16-bits CRC

	tmp = spi2_8TxRx(0xff); // read response
	if ((tmp & 0x1F) != 0x05) // data block accepted ?
	{
		//SPI3_DESELECT();
		if (SD_DEBUG)
			printf("error code:0x%x\n", tmp);
		return (WRITE_BLOCK_ERROR); //Error!
	}
	//Wait till MMC/SD-Card is not busy
	while (spi2_8TxRx(0xff) != 0xff) {
	};

	//set MMC_Chip_Select to high (MMC/SD-Card Invalid)
	//SPI3_DESELECT();
	return (0);
}

//------------------------------------------------------------------------
//	Routine for writing a Block(512Byte) to MMC/SD-Card
//	Return 0 if sector writing is completed.
//	Note:ther interface will surpport the FAT32/FAT16 filesystem
//------------------------------------------------------------------------
unsigned char MMC_write_sectors(unsigned long addr, unsigned char *Buffer,
		unsigned int count) {
	unsigned char tmp, retry;
	unsigned int i;
	//Command 24 is a writing blocks command for MMC/SD-Card.

	retry = 0;
	//SPI3_SELECT();

	do {
		//Retry 100 times to send command.
		tmp = Write_Command_MMC(CMD25, addr << 9); //write Command
		retry++;
		if (retry == 64) {
			if (SD_DEBUG)
				printf("error code:0x%x\n", tmp);
			return (tmp); //send commamd Error!
		}
	} while (tmp != 0);

	/*
	 for (i = 0; i < 100; i++) {
	 spi2_8TxRx(0xff);
	 }
	 */
	do {
		//Send Start Byte to MMC/SD-Card
		spi2_8TxRx(0xFC);
		//send data
		for (i = 0; i < 512; i++) {
			spi2_8TxRx(*Buffer++); //send 512 bytes to Card
		}

		//CRC-Byte
		spi2_8TxRx(0xFF); //Dummy CRC
		spi2_8TxRx(0xFF); //CRC Code

		tmp = spi2_8TxRx(0xff); // read response
		if ((tmp & 0x1F) != 0x05) // data block accepted ?
		{
			//SPI3_DESELECT();
			if (SD_DEBUG)
				printf("error code:0x%x\n", tmp);
			return (WRITE_BLOCK_ERROR); //Error!
		}
		while (spi2_8TxRx(0xff) == 0)
			;
	} while (--count);

	//stop
	spi2_8TxRx(0xfd);
	while (spi2_8TxRx(0xff) == 0)
		;

	//SPI3_DESELECT();

	return count;
}

//------------------------------------------------------------------------
//	Return: [0]-success or something error!
//	Params:	sector is the address of Block to read
//------------------------------------------------------------------------
unsigned char MMC_Start_Read_Sector(unsigned long sector) {
	unsigned char retry;
	unsigned char temp;

	readPos = 0;
	retry = 0;
	do {
		//Retry 100 times to send command 17.
		temp = Write_Command_MMC(17, sector << 9);
		retry++;
		if (retry == 100) {
			if (SD_DEBUG)
				printf("error code:0x%x\n", temp);
			return (READ_BLOCK_ERROR); //block write Error!
		}
	} while (temp != 0);

	//Read Start Byte form MMC/SD-Card (FEh/Start Byte)
	//Now data is ready,you can read it out.
	while (spi2_8TxRx(0xff) != 0xfe)
		;

	//spi2_8TxRx(0xff);                   //伪16-bits crc
	//spi2_8TxRx(0xff);

	return 0; //Open a sector successfully!
}

unsigned char MMC_Start_Read_Sectors(unsigned long sector) {
	unsigned char retry;
	unsigned char temp;

	readPos = 0;
	retry = 0;
	do {
		//Retry 100 times to send command 17.
		temp = Write_Command_MMC(CMD18, sector << 9);
		retry++;
		if (retry == 64) {
			if (SD_DEBUG)
				printf("error code:0x%x\n", temp);
			return (READ_BLOCK_ERROR); //block write Error!
		}
	} while (temp != 0);

	//Read Start Byte form MMC/SD-Card (FEh/Start Byte)
	//Now data is ready,you can read it out.
	while (spi2_8TxRx(0xff) != 0xfe)
		;

	//spi2_8TxRx(0xff);                   //伪16-bits crc
	//spi2_8TxRx(0xff);

	return 0; //Open a sector successfully!
}

unsigned char MMC_Start_Read_Sectors2(unsigned long sector) {
	unsigned char retry;
	unsigned char temp;

	//Send Command CMD to MMC/SD-Card
	retry = 0;
	do {
		//Retry 100 times to send command 17.
		temp = Write_Command_MMC(CMD18, sector);
		retry++;
		if (retry == 64) {
			if (SD_DEBUG)
				printf("error code:0x%x\n", temp);
			return (READ_BLOCK_ERROR); //block write Error!
		}
	} while (temp != 0);

	//Read Start Byte form MMC/SD-Card (FEh/Start Byte)
	//Now data is ready,you can read it out.
	while (spi2_8TxRx(0xff) != 0xfe)
		;

	//spi2_8TxRx(0xff);                   //伪16-bits crc
	//spi2_8TxRx(0xff);

	return 0; //Open a sector successfully!
}
//------------------------------------------------------------------------
// 	functuon:	Read Data from MMC/SD Card
//	Note:		if Bytes < 512, Read Bytes Bits data; else Read 512 Bits
//				what's more important is readPos!
//------------------------------------------------------------------------
void MMC_get_data(unsigned int Bytes, char *buffer) {
	unsigned int j;
	char* buff_pos = buffer;
	for (j = 0; ((j < Bytes) && (readPos < 512)); j++) {
		*buff_pos++ = spi2_8TxRx(0xff);
		readPos++; //read a byte,increase read position
	}

	if (readPos == 512) {
		//CRC-Bytes
		spi2_8TxRx(0xff);//CRC - Byte
		spi2_8TxRx(0xff);//CRC - Byte
		readPos = 0; //reset sector read offset
		sectorPos++; //Need to read next sector
		LBA_Opened = 0; //Set to 1 when a sector is opened.
		//SPI3_DESELECT();;  //MMC disable
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MMC_get_data_LBA(unsigned long lba, unsigned int Bytes,
		unsigned char *buffer) {
	//get data from lba address of MMC/SD-Card
	//If a new sector has to be read then move head
	if (readPos == 0) {
		if (MMC_Start_Read_Sector(lba) != 0) {
			if (SD_DEBUG)
				printf("Debug:reach MMC_get_data_LBA,error ocured!\n");
			return;
		}
	}
	MMC_get_data(Bytes, buffer);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MMC_LBA_Close() {
	unsigned char temp[1];
	while ((readPos != 0x00) | (LBA_Opened == 1)) {
		//read MMC till readPos==0x00
		MMC_get_data(1, temp); //dummy read,temp is a valid data.
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MMC_GotoSectorOffset(unsigned long LBA, unsigned int offset) {
	//Find the offset in the sector
	unsigned char temp[1];
	MMC_LBA_Close(); //close MMC when read a new sector(readPos=0)
	while (readPos < offset)
		MMC_get_data_LBA(LBA, 1, temp); //go to offset
}

//------------------------------------------------------------------------
// SD card test
//------------------------------------------------------------------------
#define ADDR_START	3200
#define N_BLOCKS	10
void sd_test() {
	unsigned int i = 0;
	unsigned int j = 0;
	char data_buf[512];
	char data_read[512];

	//fill data
	for (i = 0; i < 512; i++) {
		data_buf[i] = i + 1;
		data_read[i] = 0;
	}

	if (SD_DEBUG)
		printf("Ready to read data from SD card\n");
	if (MMC_Init() != 0x55) {
		if (SD_DEBUG)
			printf("SD Card Init Error!\n");
		return;
	}

	if (SD_DEBUG)
		printf("Get SD Card Infomarion...\n");
	MMC_get_volume_info();

	// write 512b into sd card
	if (MMC_write_sector(ADDR_START, data_buf) != 0) {
		if (SD_DEBUG)
			printf("MMC_write_sector error!\n");
		//return ;
	}

	//read data from SD card
	readPos = 0;
	if (MMC_Start_Read_Sector(ADDR_START) != 0) {
		if (SD_DEBUG)
			printf("MMC_Start_Read_Sector error!\n");
		//return ;
	}

	MMC_get_data(512, data_read);
	if (SD_DEBUG)
		printf("MMC_get_data success!\n\n");

	for (i = 0; i < 512; i++) {
		if (SD_DEBUG)
			printf("%d ", data_read[i]);
	}
	if (SD_DEBUG)
		printf("\nSD Card Test over!\n");
}

//
// FAT FS surpport
//

/*
 *
 * xiaoyang yi
 * 2011-11-1
 *
 * prot fatfs to leon3
 */

static volatile DSTATUS Stat = STA_NOINIT;

static BYTE wait_ready(void) {
	BYTE res;
	spi2_8TxRx(0xFF);
	do {
		res = spi2_8TxRx(0xFF);
	} while (res != 0xFF);
	return res;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE drv) {
	/*surpport only one device*/
	if (drv)
		return STA_NOINIT;

	if (MMC_Init() != 0x55) {
		if (SD_DEBUG)
			printf("disk_initialize:SD Card Init Error!\n");
		Stat |= STA_NODISK;
	}

	if (Stat & STA_NODISK)
		return Stat;

	Stat &= ~STA_NOINIT; /*clear STA_NOINIT*/
	if (SD_DEBUG)
		printf("disk_initialize:SD Card Init ok!\n");
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE drv /* Physical drive number (0) */
) {
	if (drv)
		return STA_NOINIT; /* Supports only single drive */
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE drv, /* Physical drive number (0) */
BYTE *buff, /* Pointer to the data buffer to store read data */
DWORD sector, /* Start sector number (LBA) */
BYTE count /* Sector count (1..255) */
) {
	char* mbuff = buff;
	unsigned char retry = 0;
	unsigned char temp;
	int sector_cnt = 0;
	int tmp = 0;

	if (drv || !count)
		return RES_PARERR;
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;

	if (count == 1) { /* Single block read */
		if (MMC_Start_Read_Sector(sector) != 0) {
			if (SD_DEBUG)
				printf("error:single sector read in disk_read.\n");
		}
		MMC_get_data(512, mbuff);
		count = 0;
	} else { /* Multiple block read */
		/*sector_cnt = count;
		 for (tmp = 0; tmp < sector_cnt; tmp++, mbuff = mbuff + 512) {
		 if (MMC_Start_Read_Sector(sector) != 0) {
		 if (SD_DEBUG)
		 printf("error:single sector read in disk_read.\n");
		 }
		 MMC_get_data(512, mbuff);
		 }
		 */
		if (MMC_Start_Read_Sectors(sector) != 0) {
			if (SD_DEBUG)
				printf("error:single sector read in disk_read.\n");
		} else {
			do {
				while (spi2_8TxRx(0xff) != 0xfe)
					;
				MMC_get_data(512, mbuff);
				mbuff += 512;
			} while (--count);

			//send stop cmd
			do {
				temp = Write_Command_MMC(CMD12, 0); // STOP_TRANSMISSION
				retry++;
				if (retry == 64) {
					printf("error code:0x%x\n", temp);
					break;
				}
			} while (temp != 0);
		}

	}

	return count ? RES_ERROR : RES_OK;
}

static
char xmit_datablock(const BYTE *buff, /* 512 byte data block to be transmitted */
BYTE token /* Data/Stop token */
) {
	BYTE resp;
	int i = 0;

	while (spi2_8TxRx(0xff) != 0xff) {
	};

	spi2_8TxRx(token); /* transmit data token */
	if (token != 0xFD) { /* Is data token */
		for (i = 0; i < 512; i++) {
			spi2_8TxRx(*buff++); //send 512 bytes to Card
		}

		spi2_8TxRx(0xFF); /* CRC (Dummy) */
		spi2_8TxRx(0xFF);
		resp = spi2_8TxRx(0xFF); /* Receive data response */
		if ((resp & 0x1F) != 0x05) /* If not accepted, return with error */
			return FALSE;
	}

	return TRUE;
}
/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write2(BYTE drv, /* Physical drive number (0) */
const BYTE *buff, /* Pointer to the data to be written */
DWORD sector, /* Start sector number (LBA) */
BYTE count /* Sector count (1..255) */
) {
	if (drv || !count)
		return RES_PARERR;
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;
	if (Stat & STA_PROTECT)
		return RES_WRPRT;

	if (count == 1) { /* Single block write */
		if ((Write_Command_MMC(CMD24, sector) == 0) /* WRITE_BLOCK */
		//&& MMC_write_sector(0xFE, buff)
				&& xmit_datablock(buff, 0xFE))
			count = 0;
	} else { /* Multiple block write */
		if (Write_Command_MMC(CMD25, sector) == 0) { /* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC))
					break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD)) /* STOP_TRAN token */
				count = 1;
		}
	}

	while (spi2_8TxRx(0xff) == 0)
		;
	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_write(BYTE drv, /* Physical drive number (0) */
const BYTE *buff, /* Pointer to the data to be written */
DWORD sector, /* Start sector number (LBA) */
BYTE count /* Sector count (1..255) */
) {
	int tmp = 0;
	int sector_cnt = 0;
	const BYTE *mbuff = buff;

	if (drv || !count)
		return RES_PARERR;
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;
	if (Stat & STA_PROTECT)
		return RES_WRPRT;

	if (count == 1) { /* Single block write */
		if (MMC_write_sector(sector, buff) < 0) {
			//&& MMC_write_sector(0xFE, buff)
		} else {
			count = 0;
		}
	} else { /* Multiple block write */
		sector_cnt = count;
		mbuff = buff;
		/*for (tmp = 0; tmp < sector_cnt; tmp++, mbuff = mbuff + 512) {
		 if (MMC_write_sector(sector, mbuff) < 0) {
		 //error
		 printf("disk_write:error!\n");
		 break;
		 } else {
		 count--;
		 }
		 }*/

		tmp = MMC_write_sectors(sector, buff, count);
		if (tmp < 0) {
			printf("disk_write:error!\n");
		} else {
			//count = tmp;
			count = 0;
		}
	}

	while (spi2_8TxRx(0xff) == 0)
		;
	return count ? RES_ERROR : RES_OK;
}
DRESULT disk_ioctl(BYTE drv, /* Physical drive number (0) */
BYTE ctrl, /* Control code */
void *buff /* Buffer to send/receive control data */
) {
	DRESULT res;
	//BYTE n, csd[16], *ptr = buff;
	//WORD csize;
	//return RES_OK;
	if (drv)
		return RES_PARERR;

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		//...
	} else {
		if (Stat & STA_NOINIT)
			return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC: /* Make sure that no pending write process */
			//SELECT();//SD enable
			if (wait_ready() == 0xFF)
				res = RES_OK;
			break;
		case GET_SECTOR_COUNT: /* Get number of sectors on the disk (DWORD) */
			break;
		case GET_SECTOR_SIZE: /* Get R/W sector size (WORD) */
			*(WORD*) buff = 512;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
			break;
		case MMC_GET_TYPE: /* Get card type flags (1 byte) */
			//*ptr = CardType;
			res = RES_OK;
			break;
		case MMC_GET_CSD: /* Receive CSD as a data block (16 bytes) */
			break;
		case MMC_GET_CID: /* Receive CID as a data block (16 bytes) */
			break;
		case MMC_GET_OCR: /* Receive OCR as an R3 resp (4 bytes) */
			break;
		case MMC_GET_SDSTAT: /* Receive SD status as a data block (64 bytes) */
			break;
		default:
			res = RES_PARERR;
		}
	}

	return res;
}

/*********************************************************************
 * Function:            DWORD get_fattime(void)
 * PreCondition:
 * Input:           None
 * Output:          Time
 * Side Effects:
 * Overview:        when writing fatfs requires a time stamp
 *                                      in this exmaple we are going to use a counter
 *                                      If the starter kit has the 32kHz crystal
 *                                      installed then the RTCC could be used instead
 * Note:
 ********************************************************************/
DWORD get_fattime(void) {
	DWORD tmr = 0;

	//return 2011-11-11 11:11:11
	tmr = (((DWORD) 21 << 25) | ((DWORD) 11 << 21) | ((DWORD) 11 << 16)
			| (WORD) (11 << 11) | (WORD) (11 << 5) | (WORD) (11 >> 1));

	return tmr;
}

