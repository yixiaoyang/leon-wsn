//===================================================================
//                 netif.c
//        Wireless network interface
//    Jason Liang: co.liang.ol@gmail.com
//
//===================================================================
#include <stdio.h>
#include "types.h"
#include "uz2400d.h"
#include "ieee802_15_4.h"
#include "rf.h"
#include "netif.h"

int8u_t txbuf[128]={0};
int8u_t rxbuf[144]={0};
/********************************************************************
 * Network Configuration Options
 *	>> None
 *	>>
 *	<< None
 *	--
 ********************************************************************/
void netconfig(void)
{
	int16u_t saddr=0;
	int16u_t panid=0;
	int8u_t eadr[8]={0},i=0;
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
	printf("MAC Address: ");
	for(;i<7;i++) {
		printf("%2x:",eadr[i]);
	}
	printf("%2x\n",eadr[i]);

	printf("PANID: 0x%4x\n",panid);
	printf("Short address: 0x%4x\n",saddr);
}
/********************************************************************
 * Low level output packet as it is
 *	>> @buf Pointer to the transfer buffer
 *	>> @len Length of the packet
 *	<< None
 *	--
 ********************************************************************/
void link_send( u8_t* buf, u16_t len )
{

}
/********************************************************************
 * Add MAC layer header and send out
 * General MAC Frame Format
 *
 *	>> @buf Pointer to the data buffer
 *	>> @size Length of the data
 *	>> @dst_saddr Pointer to 16-bit short address
 *	<< None
 *	--
 	+-------------------------+
	| Frame Control	          | 	2 octets
	+-------------------------+
	| Sequence Num	          | 	1 octet
	+-------------------------+
	| Dst. PAN ID		          | 	0/2 octets
	+-------------------------+
	| Dst. Address	          | 	0/2/8 octets
	+-------------------------+
	| Source PAN ID	          | 	0/2 octets
	+-------------------------+
	| Src. Address	          | 	0/2/8 octets
	+-------------------------+
	|  Auxiliary Security     |		0/5/6/10/14 octets
	+-------------------------+
	|  Payload                |    Variable Length
	+-------------------------+
********************************************************************/
int mac_send( u8_t* buf, u16_t size, u8_t* dst_addr, u8_t ack_bit )
{
	/* Add IEEE802.15.4 MAC header */
	ieee_mac_hdr* pkt = (ieee_mac_hdr*)txbuf;
	int8u_t pkt_len=0;
	static u8_t seq = 0;
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
/*
	printf("%#x\r\n",*((u16_t*)&pkt->frame_ctl));
	printf("%#x\r\n",*((u8_t*)&pkt->frame_ctl));
	printf("%#x\r\n",*((u8_t*)&pkt->frame_ctl+1));
//	*/
	/*
	pkt->frame_ctl[0] = 0x21;
	pkt->frame_ctl[1] = 0x88;
	// */
	pkt->sequence = seq;

	/* Destination Address Fields */
	pkt->des_PANID[0] = PANIDL;			/* PANID = 0xAAAA */
	pkt->des_PANID[1] = PANIDH;
	pkt->des_ADDR[0] = dst_addr[0];
	pkt->des_ADDR[1] = dst_addr[1];

	/* Source Address Fields */
	pkt->src_PANID[0] = PANIDL;
	pkt->src_PANID[1] = PANIDH;
	pkt->src_ADDR[0] = 0x11;//SADRL;
	pkt->src_ADDR[1] = 0x11; //SADRH;

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
int mac_recv(unsigned char *buf)
{
	ieee_mac_hdr* pkt;
	int rc, i;
	int size = -1;
  if((rc=RF_Rx(rxbuf))>0) {
		pkt = (ieee_mac_hdr*)rxbuf;
		size = rc - 11;
		/*
		printf("Received %d bytes from ", size);
		for(i=0; i<2; i++) {
			printf("%#x ", pkt->src_ADDR[i]);
		}
		printf("\n");
//		*/
		/*
		for(i=0; i<size; i++) {
			printf("%#x ", pkt->payload[i]);
		}
		printf("\n");
//		*/
		/* Copy data to buf */
		memcpy(buf, pkt->payload, size);
	}
	return size;

}

