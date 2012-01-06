/*************************
 *translate.h
 *date 2011.4.9
 *author hgdytz
 *************************/

#ifndef TRANSLATE
#define TRANSLATE

#include"types.h"

struct PacketHead
{
	u8_t   d_port;        /*destination port number*/
	u16_t  d_mac;         /*destination mac address*/
	u8_t   s_port;        /*source port number     */
	u16_t  s_mac;         /*source mac  address    */
	s16_t  pl_len;        /*packet body length     */
	u8_t   mode;          /*user can define protocal by using this u8_t*/
                              /* (for audio wav translate)mode :
				 0x00 audio file head
				 0x01 audio file data(wav 16bit mone)
				 0x02 audio file data(adpcm compressed wav 16bit mone)
				 0x0f audio file last data
                                 you can continue add some;
			      */
};


/*****************************************************************
*in mac frame body  first 3 byte is framehead.it is used
*for assenble frames to a package and tell frame reciver 
*some key infomation
*****************************************************************/
struct FrameHead	      
{
	u8_t    d_port;       /*destion port number, decide which PortBuf put this frame to */
	s8_t    fl_len;       /*frame body length beside framehead and mac head  	    */
	mybool  more;	      /*whether this is the last frame of a packet         	    */
};

/****************************************************************
*port buffer, used as a double buffers to receive frames
*and assemble to a complete packate;
****************************************************************/
#define REVBUFSIZE 3000       /*define the max pocket size*/
struct PortBuf
{
	u8_t   buffera[REVBUFSIZE];       
	u8_t   bufferb[REVBUFSIZE];
	s16_t  indexa;        /*an index of buffera, to indicate useful data in buffera*/
	s16_t  indexb;	      /*an index of bufferb, to indicate useful data in bufferb*/
	mybool fulla;	      /*whether buffer a receive a complete packet*/
	mybool fullb;         /*whether buffer b receive a complete packet*/
	mybool reada;	      /*which buffer to read  */
	mybool writea;	      /*which buffer to write */               
};


/*****************************************************************************
* a struct contain portbuf and record whether buf is used
*****************************************************************************/
struct PortKey
{
	struct PortBuf   PB;
	mybool           used;
};
#define PORTNUM     4
extern struct PortKey portKeys[PORTNUM];


/*****************************************************************************
**************  ****** simulator of net interupt dealer **********************
*****************************************************************************/
void dsr_network(void);

mybool initnet(void);

/***** init PortKeys, set default values******/
mybool initkeys(void);

/***** get a empty key to used******/
mybool getkey(s8_t keynum);


/*******************************************************************************
* send a packet by send frames
* >> s_buffer  : packet source buffer adderse
* >> maxlen    : packet source buffer size, should not bigger than REVBUFSIZE
* >> head_p    : packet infomation
* << whether send complete
*******************************************************************************/
mybool sendPacket(void * s_buffer, s16_t maxlen, struct PacketHead*  head_p , int32u_t delaytime);

/********************************************************************************
* receive a packet from a port buffer,if get one return packet size,else return 0;
* >> d_buffer  : destionatin buffer address, used to store data
* >> maxlen    : the size of destionatin buffer
* >> portkey_p : port infomation that get packet from
* << s16_t     : get packet size;
*********************************************************************************/
s16_t  recePacket(void * d_buffer, s16_t maxlen, struct PortKey *  portkey_p);

#endif
