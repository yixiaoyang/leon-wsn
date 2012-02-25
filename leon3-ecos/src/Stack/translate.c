#include "includes.h"

#define MAXRFRAME 144
#define MAXSFRAME 100

struct PortKey portKeys[PORTNUM];

void trans_delay(int32u_t us)
{
	int m_count = 128;
	while(us-- > 0){
		while(m_count-->0)
			;
	}
}

#if 1
/******************************************************************************
 * init all needed devices and config net
 *	>> None
 *	<< None
 *	--
 ******************************************************************************/

mybool initnet(void)
{
	/* Device Initialize */
	printf("sip init ok!\n");
	RF_CHIP_INITIALIZE();
	printf("chip init ok!\n");
	/* Print NIC Configuration */
	netconfig();
	printf("net config");
	return TRUE;
}


/******************************************************************************
 * init PortKeys, set default values 
 *	>> None
 *	<< None
 *	--
 ******************************************************************************/
mybool initkeys(void)
{
	int index=0;
	for(index=0; index<PORTNUM; index++)
	{
		portKeys[index].used=FALSE;
		portKeys[index].PB.indexa=0;
		portKeys[index].PB.indexb=0;
		portKeys[index].PB.fulla=FALSE;
		portKeys[index].PB.fullb=FALSE;
		portKeys[index].PB.reada=TRUE;
		portKeys[index].PB.writea=TRUE;
	}
	return TRUE;
}

mybool getkey(s8_t keynum)
{
	if(keynum<0 || keynum>=PORTNUM)
	{
		return FALSE;
	}
	if(portKeys[keynum].used==FALSE)
	{
		portKeys[keynum].used=TRUE;	
		return TRUE;
	}else
	{
		return FALSE;
	}
}


/**************************************************************************
*send a packet by send frames
*>> s_buffer  : packet source buffer adderse
*>> maxlen    : packet source buffer size
*>> head_p    : packet infomation
*<< whether send complete
**************************************************************************/
mybool  sendPacket(void* s_buffer, s16_t maxlen, struct PacketHead*  head_p ,int32u_t delaytime)
{
	int forts=0;
	u8_t  bufferS[MAXSFRAME+sizeof(struct FrameHead)];
	struct FrameHead* fhead_p =(struct FrameHead *)bufferS;
	s16_t sindex=0;
	if(fhead_p!=NULL && s_buffer!=NULL)
	{
		fhead_p->d_port=head_p->d_port;
		fhead_p->fl_len=sizeof(struct PacketHead);
		fhead_p->more=TRUE;

		memcpy(&bufferS[sizeof(struct FrameHead)], head_p, sizeof(struct PacketHead));
		//int mac_output( u8_t* buf, u16_t size, u8_t* dst_addr, u8_t ack_bit );
		mac_output((u8_t *)bufferS, (u16_t)(sizeof(struct PacketHead)+sizeof(struct FrameHead)), (u8_t *)&(head_p->d_mac), 0x1);
		trans_delay(delaytime);
		printf("send head %d\n", (sizeof(struct PacketHead)+sizeof(struct FrameHead)) );

		/**********************for test************************
		for(forts=0; forts<200; forts++)
		{
			printf("sendh ");	
		}
		printf("\n");*/

		for(sindex=0; sindex+MAXSFRAME<maxlen; sindex+=MAXSFRAME)
		{
			fhead_p->fl_len=MAXSFRAME;
			memcpy((void *)&bufferS[sizeof(struct FrameHead)], (void *)&s_buffer[sindex], MAXSFRAME);
			mac_output((u8_t *)bufferS, (u16_t)(MAXSFRAME+sizeof(struct FrameHead)), (u8_t *)&(head_p->d_mac), 0x1);
			trans_delay(delaytime);
			printf( "send midf%d \n", (MAXSFRAME+sizeof(struct FrameHead)) );
			/**********************for test***********************			
			for(forts=0; forts<200; forts++)
			{
				printf("sendm ");	
			}
			printf("\n");*/
		}
		fhead_p->fl_len=maxlen-sindex;
		fhead_p->more=FALSE;
		memcpy((void *)&bufferS[sizeof(struct FrameHead)], (void *)&s_buffer[sindex], maxlen-sindex);
		mac_output((u8_t *)&bufferS, (u16_t)(maxlen-sindex+sizeof(struct FrameHead)), (u8_t *)&(head_p->d_mac), 0x1);		
		trans_delay(delaytime);
		printf("send last f %d\n", (maxlen-sindex+sizeof(struct FrameHead)));
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
*insert a frame into correct Port buffer to assemble packet
*>> buffer  :   source buffer of frame; 
*>> size    :   size of buffer
*<<   if insert success
******************************************************************************/
mybool f_insert(void * buffer, u8_t size)
{
	ieee_mac_hdr * machead_p =buffer;     
	struct FrameHead* fhead_p =&buffer[IEEE_MAC_LEN];;
	u8_t portnum=fhead_p->d_port;
	struct PortBuf * portbuf_p;
	
	machead_p=machead_p; // avoid warning 	
	
	//printf("get in insert\n");
	if(portKeys[portnum].used==FALSE)
	{	
		printf("port used = false, return\n");
		return FALSE;
	}
	portbuf_p= &(portKeys[portnum].PB);
	if(portbuf_p->writea)
	{
		//printf("write a of %d\n",portnum);
		if(portbuf_p->fulla==FALSE)
		{	
			memcpy( &(portbuf_p->buffera[portbuf_p->indexa]), &buffer[IEEE_MAC_LEN+sizeof(struct FrameHead)],size-IEEE_MAC_LEN-sizeof(struct FrameHead));
			portbuf_p->indexa+=(size-IEEE_MAC_LEN-sizeof(struct FrameHead));
			//printf("add frame in %d :%d\n",portnum,portbuf_p->indexa);
			if(fhead_p->more==FALSE)
			{         
				portbuf_p->fulla=TRUE;
				portbuf_p->writea=FALSE;
				//printf("a packet end\n");
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		//printf("write b of %d\n",portnum);
		if(portbuf_p->fullb==FALSE)
		{
			memcpy( &(portbuf_p->bufferb[portbuf_p->indexb]), &buffer[IEEE_MAC_LEN+sizeof(struct FrameHead)], size-IEEE_MAC_LEN-sizeof(struct FrameHead));
			portbuf_p->indexb+=(size-IEEE_MAC_LEN-sizeof(struct FrameHead));
			if(fhead_p->more==FALSE)
			{
				portbuf_p->fullb=TRUE;
				portbuf_p->writea=TRUE;
				//printf("a packet end\n");
			}
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}	


/*************** simulator of net interupt dealer  **************************/
void dsr_network(void)
{
	u8_t getsizeF=0;
	u8_t bufferF[MAXRFRAME];
	
	int aa=0;
	while(TRUE)
	{
		printf("check!!!!!!!!!!!!!!!\n");
		if( (getsizeF=RF_Rx(bufferF))!=0 )
		{	
			printf("get one !!:%d\n",getsizeF);
			/*for(aa=0; aa<getsizeF; aa++)
			{
				printf(" | %d",bufferF[aa]);
			}
			printf("\n");*/
			f_insert((void *)bufferF, getsizeF);
		}
		//printf("end check\n");
	}
}


/***********************************************************************************
* receive a packet from a port buffer, if get one return packet size,else return 0;
* >> d_buffer  : destionatin buffer address, used to store data
* >> maxlen    : the size of destionatin buffer
* >> portkey_p : port infomation that get packet from
* << s16_t     : get packet size;
************************************************************************************/
s16_t recePacket(void * d_buffer, s16_t maxlen, struct PortKey *   portkey_p)
{
	s16_t getnum=0;
	if(portkey_p->used==FALSE)
	{
		printf(" key used =false ");
		return 0;
	}
	if(portkey_p->PB.reada==TRUE)
	{
		printf("read a\n");
		if(portkey_p->PB.fulla==TRUE)
		{	
			printf("a is full read");
			if(portkey_p->PB.indexa>maxlen)
			{
				return 0;
			}
			getnum=portkey_p->PB.indexa;
			memcpy(d_buffer, portkey_p->PB.buffera, portkey_p->PB.indexa);
			portkey_p->PB.indexa=0;
			portkey_p->PB.fulla=FALSE;
			portkey_p->PB.reada=FALSE;
			return getnum;
		}
	}
	else
	{
		printf("read b\n");
		if(portkey_p->PB.fullb==TRUE)
		{
			printf("b is full read");
			if(portkey_p->PB.indexb>maxlen)
			{
				return 0;
			}
			getnum=portkey_p->PB.indexb;
			memcpy(d_buffer, portkey_p->PB.bufferb, portkey_p->PB.indexb);
			portkey_p->PB.indexb=0;
			portkey_p->PB.fullb=FALSE;
			portkey_p->PB.reada=TRUE;
			return getnum;
		}
	}	
	return 0;
}
#endif
