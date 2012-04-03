/*
 * ieee802_15_4.h
 *
 *  Created on: 2011-3-29
 *      Author: water
 */

#ifndef IEEE802_15_4_H_
#define IEEE802_15_4_H_

/* 802.15.4 frame type */
#define FRM_TYPE_BEACON			0x0
#define FRM_TYPE_DATA			0x1
#define FRM_TYPE_ACK			0x2
#define FRM_TYPE_CMD			0x3

#include <linux/types.h>

/* Addressing modes */
#define NO_ADDRESS_FIELD		0x0
#define ADDR_MOD_RSVD			0x1
#define ADDR_MOD_16			0x2
#define ADDR_MOD_64			0x3

/* Frame Version */
#define FRM_VER_0			0x0		/* 802.15.4-2003 compatible */
#define FRM_VER_1			0x1		/* 802.15.4 frame */

typedef struct frm_ctrl
{
	u16 reserved1:1;
	u16 PAN_compress:1;
	u16 ack_req:1;
	u16 frm_pending:1;
	u16 security_en:1;
	u16 frm_type:3;

	u16 srcaddr_mode:2;
	u16 frm_ver:2;
	u16 dstaddr_mode:2;
	u16 reserved2:2;
} frm_ctrl_t;

typedef struct ieee15_4_mac_hdr {
	frm_ctrl_t frame_ctl;
	u8 sequence;
	u8 des_PANID[2];
	u8 des_ADDR[2];

	u8 src_PANID[2];
	u8 src_ADDR[2];
	u8 payload[0];
#define IEEE_MAC_LEN		11
} ieee_mac_hdr;
//__attribute__((packed));

#endif /* IEEE802_15_4_H_ */
