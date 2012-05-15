//===================================================================
//                 netif.h
//        Wireless network interface
//    Jason Liang: co.liang.ol@gmail.com
//
//===================================================================
#ifndef NETIF_H_
#define NETIF_H_
#include "types.h"

void netconfig(void);
int mac_send( u8_t* buf, u16_t size, u8_t* dst_addr, u8_t ack_bit );
int mac_recv(unsigned char *buf);
#endif /* NETIF_H_ */
