/*
 * beeif.c
 *
 *  Created on: 2011-3-29
 *      Author: hgdytz
 */
#include "types.h"
#include "rf.h"
#include "uz2400d.h"
#include "lemon.h"
#include "ieee802_15_4.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************
 * Network Configuration Options
 *	>> None
 *	>>
 *	<< None
 *	--
 *
 *
		#define UZS_EADR_0			0x05
		#define UZS_EADR_1			0x06
		#define UZS_EADR_2			0x07
		#define UZS_EADR_3			0x08
		#define UZS_EADR_4			0x09
		#define UZS_EADR_5			0x0A
		#define UZS_EADR_6			0x0B
		#define UZS_EADR_7			0x0C

		#define EADR_0		0x21
		#define EADR_1		0x22
		#define EADR_2		0x23
		#define EADR_3		0x24
		#define EADR_4		0x25
		#define EADR_5		0x26
		#define EADR_6		0x27
		#define EADR_7		0x28
 *
 ******************************************************************************/
void netconfig(void)
{
	u16 saddr=0;
	u16 panid=0;
	u8 eadr[8]={0},i=0;
	/* Read MAC(64-bit extended address) */
	eadr[0] = uz2400d_sr(UZS_EADR_0);
	eadr[1] = uz2400d_sr(UZS_EADR_1);
	eadr[2] = uz2400d_sr(UZS_EADR_2);
	eadr[3] = uz2400d_sr(UZS_EADR_3);
	eadr[4] = uz2400d_sr(UZS_EADR_4);
	eadr[5] = uz2400d_sr(UZS_EADR_5);
	eadr[6] = uz2400d_sr(UZS_EADR_6);
	eadr[7] = uz2400d_sr(UZS_EADR_7);
	/* Read short address */
	saddr = uz2400d_sr(UZS_SADRH)<<8&0xFF00;
	saddr += uz2400d_sr(UZS_SADRL);
	/* Read PANID */
	panid = uz2400d_sr(UZS_PANIDH)<<8&0xFF00;
	panid += uz2400d_sr(UZS_PANIDL);
	printf("LRF020 MAC Address: ");
	for(i=0;i<8;i++) {
		printf("%2x: ",eadr[i]);
	}

	printf("PANID: 0x%4x\n",panid);
	printf("Short address: 0x%4x\n",saddr);
	return ;
}

/******************************************************************************
 * Low level output packet as it is
 *	>> @buf Pointer to the transfer buffer
 *	>> @len Length of the packet
 *	<< None
 *	--
 ******************************************************************************/
void linkoutput( u8* buf, u16 len )
{

}
/******************************************************************************
 * Add MAC layer header and send out
 * General MAC Frame Format
		+-------------------------+
		| Frame Control	     | 	2 octets
		+-------------------------+
		| Sequence Num	     | 	1 octet
		+-------------------------+
		| Dst. PAN ID		     | 	0/2 octets
		+-------------------------+
		| Dst. Address	     | 	0/2/8 octets
		+-------------------------+
		| Source PAN ID	     | 	0/2 octets
		+-------------------------+
		| Src. Address	     | 	0/2/8 octets
		+-------------------------+
		|  Auxiliary Security  |		0/5/6/10/14 octets
		+-------------------------+
		|  Payload             |    Variable Length
		+-------------------------+
 *
 *	>> @buf Pointer to the data buffer
 *	>> @size Length of the data
 *	>> @dst_saddr Pointer to 16-bit short address
 *	<< None
 *	--
 ******************************************************************************/
int mac_output( u8* buf, u16 size, u8* dst_addr, u8 ack_bit )
{
	/* Add IEEE802.15.4 MAC header */
	ieee_mac_hdr* pkt = (ieee_mac_hdr*)txbuf;
	u8 pkt_len=0;
	static u8 seq = 0;
//*
	pkt->frame_ctl.frm_type = FRM_TYPE_DATA;
	pkt->frame_ctl.security_en = 0x0;
	pkt->frame_ctl.frm_pending = 0x0;
	pkt->frame_ctl.ack_req = ack_bit;
	pkt->frame_ctl.PAN_compress = 0x0;
	pkt->frame_ctl.reserved1 = 0x0;

	pkt->frame_ctl.reserved2 = 0x0;
	pkt->frame_ctl.dstaddr_mode = ADDR_MOD_16;
	pkt->frame_ctl.frm_ver = FRM_VER_0;
	pkt->frame_ctl.srcaddr_mode = ADDR_MOD_16;

	printf("%#x\r\n",*((u16*)&pkt->frame_ctl));
	printf("%#x\r\n",*((u8*)&pkt->frame_ctl));
	printf("%#x\r\n",*((u8*)&pkt->frame_ctl+1));
//*/
	/*
	pkt->frame_ctl[0] = 0x21;
	pkt->frame_ctl[1] = 0x88;
	//*/
	pkt->sequence = seq;

	/* Destination Address Fields */
	pkt->des_PANID[0] = PANIDL;			/* PANID = 0xAAAA */
	pkt->des_PANID[1] = PANIDH;
	pkt->des_ADDR[0] = dst_addr[0];
	pkt->des_ADDR[1] = dst_addr[1];

	/* Source Address Fields */
	pkt->src_PANID[0] = PANIDL;
	pkt->src_PANID[1] = PANIDH;
	pkt->src_ADDR[0] = SADRL;
	pkt->src_ADDR[1] = SADRH;

	pkt_len += IEEE_MAC_LEN;
	/* Copy Data */
	memcpy(txbuf+pkt_len, buf, size);
	pkt_len += size;

	if(RF_TxN(txbuf,pkt_len,ack_bit)==0) {
		seq++;
		return 0;
	} else {
		return -1;
	}



}

