#include <stdio.h>
#include "types.h"
#include "sd.h"

#define IF_SGOW_DATA	0
#define DBG_FAT32		1
/*
 分区记录结构如下
*/
struct PartRecord
{
    unsigned char Active;         //0x80表示此分区有效
    unsigned char StartHead;      //分区的开始头
    unsigned char StartCylSect[2];//开始柱面与扇区
    unsigned char PartType;       //分区类型
    unsigned char EndHead;        //分区的结束头
    unsigned char EndCylSect[2];  //结束柱面与扇区
    unsigned char StartLBA[4];    //分区的第一个扇区
    unsigned char Size[4];        //分区的大小
};

/*
 分区扇区（绝对0扇区）定义如下
*/
struct PartSector
{
    unsigned char PartCode[446]; //MBR的引导程序
    struct PartRecord Part[4];      //4个分区记录
    unsigned char BootSectSig0;
    unsigned char BootSectSig1;
};

struct FAT32_FAT_Item
{
    unsigned char Item[4];
};

struct FAT32_FAT
{
    struct FAT32_FAT_Item Items[128];
};

/*
 FAT32中对BPB的定义如下  一共占用90个字节
*/
struct FAT32_BPB
{
    unsigned char BS_jmpBoot[3];     //跳转指令            offset: 0
    unsigned char BS_OEMName[8];     //                    offset: 3
    unsigned char BPB_BytesPerSec[2];//每扇区字节数        offset:11
    unsigned char BPB_SecPerClus[1]; //每簇扇区数          offset:13
    unsigned char BPB_RsvdSecCnt[2]; //保留扇区数目        offset:14
    unsigned char BPB_NumFATs[1];    //此卷中FAT表数       offset:16
    unsigned char BPB_RootEntCnt[2]; //FAT32为0            offset:17
    unsigned char BPB_TotSec16[2];   //FAT32为0            offset:19
    unsigned char BPB_Media[1];      //存储介质            offset:21
    unsigned char BPB_FATSz16[2];    //FAT32为0            offset:22
    unsigned char BPB_SecPerTrk[2];  //磁道扇区数          offset:24
    unsigned char BPB_NumHeads[2];   //磁头数              offset:26
    unsigned char BPB_HiddSec[4];    //FAT区前隐扇区数     offset:28
    unsigned char BPB_TotSec32[4];   //该卷总扇区数        offset:32

    unsigned char BPB_FATSz32[4];    //一个FAT表扇区数     offset:36
    unsigned char BPB_ExtFlags[2];   //FAT32特有           offset:40
    unsigned char BPB_FSVer[2];      //FAT32特有           offset:42
    unsigned char BPB_RootClus[4];   //根目录簇号          offset:44
    unsigned char FSInfo[2];         //保留扇区FSINFO扇区数offset:48
    unsigned char BPB_BkBootSec[2];  //通常为6             offset:50
    unsigned char BPB_Reserved[12];  //扩展用              offset:52
    unsigned char BS_DrvNum[1];      //                    offset:64
    unsigned char BS_Reserved1[1];   //                    offset:65
    unsigned char BS_BootSig[1];     //                    offset:66
    unsigned char BS_VolID[4];       //                    offset:67
    unsigned char BS_FilSysType[11]; //	                offset:71
    unsigned char BS_FilSysType1[8]; //"FAT32    "         offset:82
};

// Structure of a dos directory entry.
#define DIR_NAME_LEN	8
struct direntry
{
    unsigned char deName[8];       // filename, blank filled
    unsigned char deExtension[3]; 	// extension, blank filled
    unsigned char deAttributes;   	// file attributes
    unsigned char deLowerCase;    	// NT VFAT lower case flags  (set to zero)
    unsigned char deCHundredth;   	// hundredth of seconds in CTime
    unsigned char deCTime[2];     	// create time
    unsigned char deCDate[2];     	// create date
    unsigned char deADate[2];     	// access date
    unsigned char deHighClust[2];     // high unsigned chars of cluster number
    unsigned char deMTime[2];     	// last update time
    unsigned char deMDate[2];     	// last update date
    unsigned char deLowCluster[2]; 	// starting cluster of file
    unsigned char deFileSize[4];      // size of file in unsigned chars
};

// Stuctures
#define FILE_NAME_LEN	64
struct FileInfoStruct
{
    unsigned char  FileName[12];
    unsigned long  FileStartCluster;			//< file starting cluster for last file accessed
    unsigned long  FileCurCluster;
    unsigned long  FileNextCluster;
    unsigned long  FileSize;					//< file size for last file accessed
    unsigned char  FileAttr;					//< file attr for last file accessed
    unsigned short FileCreateTime;			//< file creation time for last file accessed
    unsigned short FileCreateDate;			//< file creation date for last file accessed
    unsigned short FileMTime;
    unsigned short FileMDate;
    unsigned long  FileSector;				    //<file record place
    unsigned int   FileOffset;				    //<file record offset
};

/*
  FAT32初始化时初始参数装入如下结构体中
*/
struct FAT32_Init_Arg
{
    unsigned char BPB_Sector_No;   //BPB所在扇区号
    unsigned long Total_Size;            //磁盘的总容量
    unsigned long FirstDirClust;       //根目录的开始簇
    unsigned long FirstDataSector;	 //文件数据开始扇区号
    unsigned int  BytesPerSector;	 //每个扇区的字节数
    unsigned int  FATsectors;            //FAT表所占扇区数
    unsigned int  SectorsPerClust;	 //每簇的扇区数
    unsigned long FirstFATSector;	 //第一个FAT表所在扇区
    unsigned long FirstDirSector;	 //第一个目录所在扇区
    unsigned long RootDirSectors;	 //根目录所占扇区数
    unsigned long RootDirCount;	 //根目录下的目录与文件数
};

//文件目录分隔符';'
//#define FIND_BPB_UP_RANGE 2000  //BPB不一定在0扇区，对0～FINE_BPB_UP_RANGE扇区进行扫描

void FAT32_Test();

/*end of file*/

