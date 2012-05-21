#include "application.h"
#include "adpcm.h"
#include "translate.h"
#include "VS1003x.h"
#include <stdio.h>

#define ONCELEN  1000

char adpcpd[ONCELEN];
/******************************************
* short[] 
*
*/
void audioFSend(void * buffer, int32u_t length, int8u_t types, int32u_t delaytime )
{
	if( WAV16MONE == types)
	{
		int32u_t index=0;
		struct adpcm_state state;

		struct PacketHead myhead;
		myhead.d_port= 1;
		myhead.d_mac = 0x2222;
		myhead.s_port= 0;
		myhead.s_mac = 0x0000;
		myhead.pl_len= 1000;
		myhead.mode  = 0x00;
		//wav audio file head is 44byte
		sendPacket( (void *)buffer, 44, &myhead, delaytime);
		index +=44;

		myhead.mode  = 0x02;
		while( index+ONCELEN*4 < length )
		{
			state.index  =0;
        		state.valprev=0;
			adpcm_coder(&buffer[index], adpcpd, ONCELEN*2, &state);
			index += ONCELEN*4;
			sendPacket( (void *)adpcpd, ONCELEN, &myhead, delaytime );
				
		}
		state.index  =0;
        	state.valprev=0;
		adpcm_coder(&buffer[index], adpcpd, (length-index)/2, &state);
		myhead.mode = 0x0f;
		sendPacket( (void *)adpcpd, (length-index)/4, &myhead, delaytime);
		printf("send audio file finished!\n");
	}
	else
	{
		printf(" data mode not 16mone!\n");
	}

}


/***********this mymemcpy is used to copy data and change big-little end**************/
static void* mymemcpy( void *destin, void *source, unsigned n )
{
	int countn=0;
	char * dp, * sp;
	if(destin != NULL && source!= NULL && n>0 && 0==n%2)
	{
		dp=destin;
		sp=source;
		for( countn=0; countn<n/2; countn++ )
		{
			dp[countn*2]   = sp[countn*2+1];
			dp[countn*2+1] = sp[countn*2]  ;
		}
	}
	else
	{
		printf("mymemcpy error!\n");
	}

	return NULL;
}


void playwav16m(void * begin, int32u_t len)
{
	int i = 0;
	int j = 0;
	int16u_t data;
	
	printf("get in playwav16m !\n");
	Vs1003_Init();
	
	//raw testing
	printf("RAW test..\n");
	for(i = 0; i< 20; i++)
	{
		data = 0x0;
		printf("Reg=%x\t", data);
		Vs1003_CMD_Write(SPI_VOL, 0x2020);
		data = Vs1003_CMD_Read(SPI_VOL);
		printf(", %x\r\n", data);
		delay_ms(1000);
	}
	
	//sine test
	for(j =0;j <3; j++)
	{
		for(i = 0; i< 15; i++)
		{
			Vs1003_Sine_Test(i);
		}
		for(i = 15; i > 0; i--)
		{
			Vs1003_Sine_Test(i);
		}
		printf("sine over..\n") ;
	}

	printf("begin playing\n");
	while(1)
	{
		Vs1003_DATA_Write8(begin, len );
		printf("play over!\r\n");
	}

}

/************************************************************************************/
extern unsigned char musicdatafile[ FILELEN*2];
int32u_t indexfile =0;


short adpbuffer[ONCELEN*2]= {0};

void audioDHandle(void * buffer, int32u_t length)
{
	if( NULL!=buffer && length>0 )
	{
		struct adpcm_state state;
		struct PacketHead * pp = (struct PacketHead *)buffer;
		if( pp->mode == 0x00 )		
		{
			printf("mode 00    ()\n");
			indexfile=0;
			mymemcpy(musicdatafile,
					(char*)&buffer[sizeof(struct PacketHead)],
					(char*)length-sizeof(struct PacketHead));
			indexfile += (length-sizeof(struct PacketHead));
		}
		else if( pp->mode == 0x02)
		{
			printf("mode 02    ()\n");
			state.index  =0;
        		state.valprev=0;
			adpcm_decoder( &buffer[sizeof(struct PacketHead)],
							adpbuffer,
							(length-sizeof(struct PacketHead))*2, &state );
			mymemcpy( &musicdatafile[indexfile], adpbuffer, (length-sizeof(struct PacketHead))*4 );
			indexfile += (length-sizeof(struct PacketHead))*4;
		}
		else if( pp->mode == 0x0f)
		{
			printf("mode 0f    ()\n");
			state.index  =0;
        		state.valprev=0;
			adpcm_decoder( &buffer[sizeof(struct PacketHead)], adpbuffer, (length-sizeof(struct PacketHead))*2, &state );
			mymemcpy( &musicdatafile[indexfile], adpbuffer, (length-sizeof(struct PacketHead))*4 );
			indexfile += (length-sizeof(struct PacketHead))*4;
	
			/*received a complete file , so play it out.*/
			playwav16m( musicdatafile, FILELEN*2 );
		}
		else 
		{
			printf("packet mode error!\n");
		}
	}
	else
	{
		printf("buffer null or length <=0 \n");
	}
}

