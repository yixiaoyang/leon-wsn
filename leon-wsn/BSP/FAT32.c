/*
 * xiaoyang yi@2011.3.14
 * Transport the FAT32 & FAT16 on the PIC32
 * 
 * Warinning:The FAT32 fs doesn't surpport multi thread access!
 * you may need to control the synchronization by yourself.
 * 
 * Mail:hityixiaoyang@gmail.com
 */
 
#include "FAT32.h"

unsigned char FAT32_Buffer[512]; //扇区数据读写缓冲区
struct FAT32_Init_Arg Init_Arg;	       //初始化参数结构体实体
struct FileInfoStruct FileInfo;        //文件信息结构体实体

extern unsigned int  readPos;

//---------------------------------------------------------------
// 	by xiaoyang @2011.3.14
//	Note:	hal layer of FAT32 filesystem, using the interface 
//			from SD card driver
//--------------------------------------------------------------- 
unsigned char * FAT32_ReadSector(unsigned long LBA,unsigned char *buf) //FAT32中读取扇区的函数
{
	readPos = 0;
    MMC_get_data_LBA(LBA,512,buf);
    return buf;
}

unsigned char FAT32_WriteSector(unsigned long LBA,unsigned char *buf)//FAT32中写扇区的函数
{
	readPos = 0;
    return MMC_write_sector(LBA,buf);
}


//big endien and little endien convert
unsigned long lb2bb(unsigned char *dat,unsigned char len) //小端转为大端
{
    unsigned long temp=0;
    unsigned long fact=1;
    unsigned char i=0;
    for(i=0; i<len; i++)
    {
        temp+=dat[i]*fact;
        fact*=256;
    }
    return temp;
}

unsigned long  FAT32_FindBPB()  //寻找BPB所在的扇区号
{
    FAT32_ReadSector(0,FAT32_Buffer);
    return lb2bb(((((struct PartSector *)(FAT32_Buffer))->Part[0]).StartLBA),4);
}

unsigned long FAT32_Get_Total_Size() //存储器的总容量，单位为M
{
    FAT32_ReadSector(Init_Arg.BPB_Sector_No,FAT32_Buffer);
    return ((float)(lb2bb((((struct FAT32_BPB *)(FAT32_Buffer))->BPB_TotSec32),4)))*0.0004883;
}

void FAT32_Init(struct FAT32_Init_Arg *arg)
{
    struct FAT32_BPB *bpb=(struct FAT32_BPB *)(FAT32_Buffer);             //将数据缓冲区指针转为struct FAT32_BPB 型指针
    #if DBG_PLAYER
	printf("FAT32_FindBPB()..\n");
	#endif
	arg->BPB_Sector_No   =FAT32_FindBPB();                                             //FAT32_FindBPB()可以返回BPB所在的扇区号
	arg->Total_Size      =FAT32_Get_Total_Size();                                        //FAT32_Get_Total_Size()可以返回磁盘的总容量，单位是兆
    #if DBG_PLAYER
	printf("FAT32_Get_Total_Size:%d\n",arg->Total_Size );
	#endif
	arg->FATsectors      =lb2bb((bpb->BPB_FATSz32)    ,4);                       //装入FAT表占用的扇区数到FATsectors中
    arg->FirstDirClust   =lb2bb((bpb->BPB_RootClus)   ,4);                        //装入根目录簇号到FirstDirClust中
    arg->BytesPerSector  =lb2bb((bpb->BPB_BytesPerSec),2);                   //装入每扇区字节数到BytesPerSector中
    arg->SectorsPerClust =lb2bb((bpb->BPB_SecPerClus) ,1);                   //装入每簇扇区数到SectorsPerClust 中
    arg->FirstFATSector  =lb2bb((bpb->BPB_RsvdSecCnt) ,2)+arg->BPB_Sector_No;//装入第一个FAT表扇区号到FirstFATSector 中
    arg->RootDirCount    =lb2bb((bpb->BPB_RootEntCnt) ,2);                   //装入根目录项数到RootDirCount中
    arg->RootDirSectors  =(arg->RootDirCount)*32>>9;                           //装入根目录占用的扇区数到RootDirSectors中
    arg->FirstDirSector  =(arg->FirstFATSector)+(bpb->BPB_NumFATs[0])*(arg->FATsectors); //装入第一个目录扇区到FirstDirSector中
    arg->FirstDataSector =(arg->FirstDirSector)+(arg->RootDirSectors); //装入第一个数据扇区到FirstDataSector中
	#if DBG_PLAYER
	printf("FAT32_Init over..\n");
	#endif
}

void FAT32_EnterRootDir()
{
    unsigned long iRootDirSector;
    unsigned long iDir;
    struct direntry *pDir;
    for(iRootDirSector=(Init_Arg.FirstDirSector); iRootDirSector<(Init_Arg.FirstDirSector)+(Init_Arg.SectorsPerClust); iRootDirSector++)
    {
        FAT32_ReadSector(iRootDirSector,FAT32_Buffer);
        for(iDir=0; iDir<Init_Arg.BytesPerSector; iDir+=sizeof(struct direntry))
        {
            pDir=((struct direntry *)(FAT32_Buffer+iDir));
            if((pDir->deName)[0]!=0x00   && (pDir->deName)[0]!=0xe5   && (pDir->deName)[0]!=0x0f  )
            {
                printf("dir-name:%s\n",pDir->deName);
            }
        }
    }
}

void FAT32_CopyName(unsigned char *Dname,unsigned char *filename)
{
    unsigned char i=0;
    for(; i<11; i++)
    {
        Dname[i]=filename[i];
    }
    Dname[i]=0;
}

unsigned long FAT32_EnterDir(char *path)
{
    unsigned long iDirSector;
    unsigned long iCurSector=Init_Arg.FirstDirSector;
    unsigned long iDir;
    struct direntry *pDir;
    unsigned char DirName[12];
    unsigned char depth=0,i=0,j=0;
	unsigned char depth_count = 0;
	unsigned char level_data[32];//最多支持三十二层目录
	char path_buffer[32] = {' '};
	char path_len = strlen(path);
	
	i = 0;
    while( i < path_len )
    {
		if( path[i]!=0 ){
      		if(path[i]==';')
      		{
				level_data[depth++] = i;
      		}
		}else{	
			break;
		}
		
		i++;
    }
	
	#if DBG_FAT32
		printf("deep=%d\r\n",depth);
	#endif
	
    if(depth==1)
    {
		//printf("FAT32.c[4]:reach root\r\n");
        return iCurSector;    //如果是根目录，直接返回当前扇区号
    }
	
	//短文件名只有11个字节
	for(i = 0; i < 11; i++){
		path_buffer[i] = ' ';
	}
	path_buffer[12] = '\0';
	
	
	//直至找到最后一层或者没找到文件为止,level_data[0]放置第一个'\\'
	for(depth_count = 0; depth_count < depth; depth_count++)
	{
		//短文件名只有11个字节
		for(i = 0; i < 11; i++){
			path_buffer[i] = ' ';
		}
		path_buffer[12] = '\0';
		
		//提取文件名
		if(depth_count == depth-1){
			strcpy(path_buffer,&path[level_data[depth_count]+1]);
		}else{
			j = 0;
			for(i = level_data[depth_count]+1; i < level_data[depth_count+1]; i++)
			{
				path_buffer[j++] = path[i];
			}
		}
		
		path_buffer[12] = '\0';
		
		#if DBG_FAT32
			printf("FAT32.c[3]:path_buffer=%s\r\n",path_buffer);
		#endif
					
		//get iCurSector
		for(iDirSector=iCurSector; iDirSector<(Init_Arg.FirstDirSector)+(Init_Arg.SectorsPerClust); iDirSector++)
		{
			FAT32_ReadSector(iDirSector,FAT32_Buffer);
			for(iDir=0; iDir<Init_Arg.BytesPerSector; iDir+=sizeof(struct direntry))
			{
				pDir=((struct direntry *)(FAT32_Buffer+iDir));
				//对比每一个目录或者文件的名称
				if((pDir->deName)[0]!=0x00  && (pDir->deName)[0]!=0xe5 && (pDir->deName)[0]!=0x0f )//目录有效
				{
					#if DBG_FAT32
						printf("FAT32.c[2]:pDir->deName:%s\r\n",pDir->deName);
					#endif
					for(i = 0; i < 11; i++){
						if(pDir->deName[i] != path_buffer[i]){
							goto next_dir;
						}
					}
					//找到对应目录，进入下一层
					iCurSector = iDirSector;//记录扇区位置
					#if DBG_FAT32
						printf("FAT32.c[1.2]:Match Name:%s\r\n",path_buffer);
					#endif
					goto next_sector;//查询下一层目录
				}
				next_dir:
				;
			}//for
			
		}
		//到达这里说明目录没有找到
		#if DBG_FAT32
			printf("FAT32.c[1]:can't find path:%s\r\n",path);
		#endif
		//goto end;
				
		next_sector:
		;
	}
	end:

	return Init_Arg.FirstDirSector;
}


unsigned char FAT32_CompareName(unsigned char *sname,unsigned char *dname)
{
    unsigned char i,j=8;
    unsigned char name_temp[12];
    for(i=0; i<11; i++) name_temp[i]=0x20;
    name_temp[11]=0;
    i=0;
    while(sname[i]!='.')
    {
        name_temp[i]=sname[i];
        i++;
    }
    i++;
    while(sname[i]!=0)
    {
        name_temp[j++]=sname[i];
        i++;
    }
//printf(name_temp,0);
    for(i=0; i<11; i++)
    {
        if(name_temp[i]!=dname[i]) return 0;
    }
    //printf(name_temp,0);
    return 1;
}
/*
 * copy from sname to dname
 */

void FAT32_GetName(unsigned char *dname,unsigned char *sname)
{
    unsigned char i,j=8;
    unsigned char name_temp[12];
    for(i=0; i<11; i++) name_temp[i]=0x20;
    name_temp[11]=0;
    i=0;
    while(sname[i]!='.')
    {
        name_temp[i]=sname[i];
        i++;
    }
    i++;
    while(sname[i]!=0)
    {
        name_temp[j++]=sname[i];
        i++;
    }
//printf(name_temp,0);
    for(i=0; i<11; i++)
    {
        if(dname[i]=name_temp[i]);
    }
}

unsigned long FAT32_GetNextCluster(unsigned long LastCluster)
{
    unsigned long temp;
    struct FAT32_FAT *pFAT;
    struct FAT32_FAT_Item *pFAT_Item;
    temp=((LastCluster/128)+Init_Arg.FirstFATSector);
    FAT32_ReadSector(temp,FAT32_Buffer);
    pFAT=(struct FAT32_FAT *)FAT32_Buffer;
    pFAT_Item=&((pFAT->Items)[LastCluster%128]);
    return lb2bb((unsigned char*)pFAT_Item,4);
}
/*
 * search file and return its addr in the SD
 */
void * FAT32_OpenFile(char *filepath)
{
    unsigned char depth=0;
    unsigned char i,index;
    unsigned long iFileSec,iCurFileSec,iFile;
    struct direntry *pFile;
    unsigned char len=strlen(filepath);
    for(i=0; i<len; i++)
    {
        if(filepath[i]==';')
        {
            depth++;
            index=i+1;
        }
    }
	
	//printf("FAT32_OpenFile:filepath=%s\r\n",filepath);
    iCurFileSec=FAT32_EnterDir(filepath)/*Init_Arg.FirstDirSector*/;
	//printf("FAT32_OpenFile:filepath=%s ---> over\r\n",filepath);
    for(iFileSec = iCurFileSec ; iFileSec<iCurFileSec + (Init_Arg.SectorsPerClust); iFileSec++)
    {
        FAT32_ReadSector(iFileSec,FAT32_Buffer);
        for(iFile=0; iFile<Init_Arg.BytesPerSector; iFile+=sizeof(struct direntry))
        {
            pFile=((struct direntry *)(FAT32_Buffer+iFile));
			if((pFile->deName[0] != '.') && (pFile->deName[0] != 0x00) && (pFile->deName[0] != 0xe5)){
	            if(FAT32_CompareName(filepath+index,pFile->deName))
          		{
                	FileInfo.FileSize=lb2bb(pFile->deFileSize,4);
                	strcpy(FileInfo.FileName,filepath+index);
                	FileInfo.FileStartCluster=lb2bb(pFile->deLowCluster,2)+lb2bb(pFile->deHighClust,2)*65536;
                	FileInfo.FileCurCluster=FileInfo.FileStartCluster;
                	FileInfo.FileNextCluster=FAT32_GetNextCluster(FileInfo.FileCurCluster);
                	FileInfo.FileOffset=0;
    
                	return &FileInfo;
            	}	
			}
        }
    }
	return NULL;
}


//-----------------------------------------------------------------------------
//  xiaoyang 2011.3
//  get the file list in the path and put it into file_list
//-----------------------------------------------------------------------------
unsigned char FAT32_get_dir (char *filepath)
{
 	unsigned char depth=0;
    unsigned char i,j,index;
    unsigned long iFileSec,iCurFileSec,iFile;
    struct direntry *pFile;
    unsigned char len=strlen(filepath);
	char path_buffer[256];
	
    for(i=0; i<len; i++)
    {
        if(filepath[i]==';')
        {
            depth++;
            index=i+1;
        }
    }
	
    iCurFileSec=FAT32_EnterDir(filepath);/*Init_Arg.FirstDirSector*/
    for(iFileSec = iCurFileSec ; iFileSec<iCurFileSec + (Init_Arg.SectorsPerClust); iFileSec++)
    {
        FAT32_ReadSector(iFileSec,FAT32_Buffer);
        for(iFile=0; iFile<Init_Arg.BytesPerSector; iFile+=sizeof(struct direntry))
        {
            pFile=((struct direntry *)(FAT32_Buffer+iFile));
			if((pFile->deName[0] != ';') && (pFile->deName[0] != '.') && (pFile->deName[0] != 0x00) && (pFile->deName[0] != 0xe5)){
				//			
				// short name and extend name
				//		|SName   |EName
				//		|--------|---|
				//		|01234567|89A|
				//
				// surpport ".mp3",".wma",".mid" format	 		
				/*(pFile->deName[8]=='W' && pFile->deName[9]=='M' && pFile->deName[10]=='A')*/
				if( (pFile->deName[8]=='M' && pFile->deName[9]=='P' && pFile->deName[10]=='3') ||
					(pFile->deName[8]=='M' && pFile->deName[9]=='I' && pFile->deName[10]=='D') 
				) 					
				{
					//printf("p-file:%s\r\n", pFile->deName);
					j = 1;
					for(i = 0 ; i < 8; i++){
						if(pFile->deName[i] != ' '){
							path_buffer[j++] = pFile->deName[i];
						}
					}
					path_buffer[j++] = '.';		
					for(i = 8; i < 11; i++){
					path_buffer[j++] = pFile->deName[i];
					}	
					path_buffer[j++]='\0';
					path_buffer[0] = ';';
				
					if(menu_add(path_buffer) < 0){
						printf("FAT32_get_dir:menu add failed!\r\n");
					}	
				}else{
					
				}	
			}
        }
    }
	return -1;
}

void FAT32_ReadFile(struct FileInfoStruct *pstru)
{
    unsigned long Sub=pstru->FileSize-pstru->FileOffset;
    unsigned long iSectorInCluster=0;
    unsigned long i=0;
    while(pstru->FileNextCluster!=0x0fffffff)  //如果FAT中的簇项为0x0fffffff，说明无后继簇
    {
        for(iSectorInCluster=0; iSectorInCluster<Init_Arg.SectorsPerClust; iSectorInCluster++) //读出整簇数据
        {
            FAT32_ReadSector((((pstru->FileCurCluster)-2)*(Init_Arg.SectorsPerClust))+Init_Arg.FirstDataSector+(iSectorInCluster),FAT32_Buffer);
            pstru->FileOffset+=Init_Arg.BytesPerSector;
            Sub=pstru->FileSize-pstru->FileOffset;
        }
        pstru->FileCurCluster=pstru->FileNextCluster;
        pstru->FileNextCluster=FAT32_GetNextCluster(pstru->FileCurCluster);   //这里是FAT簇链的传递
    }
    iSectorInCluster=0;
    while(Sub>=Init_Arg.BytesPerSector)   //处理不足一簇，而足扇区的数据
    {
        FAT32_ReadSector((((pstru->FileCurCluster)-2)*(Init_Arg.SectorsPerClust))+Init_Arg.FirstDataSector+(iSectorInCluster++),FAT32_Buffer);
        pstru->FileOffset+=Init_Arg.BytesPerSector;
        Sub=pstru->FileSize-pstru->FileOffset;
    }
    FAT32_ReadSector((((pstru->FileCurCluster)-2)*(Init_Arg.SectorsPerClust))+Init_Arg.FirstDataSector+(iSectorInCluster),FAT32_Buffer); //读取最后一个扇区
	//for(i=0; i<Sub; i++)  //Sub为最后剩余的字节数
}


//-----------------------------------------------------------
// FAT32 Test
//-----------------------------------------------------------
void FAT32_Test()
{
	char ch;

	printf("Ready to read data from SD card\n");
	if(MMC_Init() != 0x55)
    {
        printf("SD Card Init Error!\n");
        return ;
    }

    printf("Get SD Card Infomarion...\n");
	MMC_get_volume_info();
	
	#if DBG_FAT32
	printf("FAT16 init...\n");
	printf("FAT32_Init begin..\n");
	#endif
	FAT32_Init(&Init_Arg);	  //FAT32文件系统初始化，装入参数
	printf("BPB_Sector_No:%d\n"  ,Init_Arg.BPB_Sector_No);
	printf("Total_Size:%d\n"     ,Init_Arg.Total_Size   );
	printf("FirstDirClust:%d\n"  ,Init_Arg.FirstDirClust); 
	printf("FirstDataSector:%d\n",Init_Arg.FirstDataSector); 
	printf("BytesPerSector:%d\n" ,Init_Arg.BytesPerSector); 
	printf("FATsectors:%d\n"     ,Init_Arg.FATsectors); 
	printf("SectorsPerClust:%d\n",Init_Arg.SectorsPerClust);
	printf("FirstFATSector:d\n" ,Init_Arg.FirstFATSector); 
	printf("FirstDirSector:%d\n" ,Init_Arg.FirstDirSector);   //以上几个语句用以输出参数值到终端
	
	struct FileInfoStruct* Info = FAT32_OpenFile(";ABC.TXT");
	if(Info == NULL){
		printf("Error:can't find file.\n");
		//return ;
	}
	printf("FAT32_OpenFile Size:%d (chars)\n" ,Info->FileSize); //打开根目录下的TEST.TXT文件，并输出文件大小
	FAT32_ReadFile(Info);  //读取文件数据，输出到终端
}

//test


