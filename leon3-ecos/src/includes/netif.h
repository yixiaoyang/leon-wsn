/*
 * netif.h
 *
 *  Created on: 2011-3-29
 *      Author: hgdytz
 */

#ifndef NETIF_H_
#define NETIF_H_

#include "types.h"

void netconfig(void);
int mac_output( u8_t* buf, u16_t size, u8_t* dst_addr, u8_t ack_bit );
#endif /* NETIF_H_ */
