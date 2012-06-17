#ifndef __TRANS_H__
#define __TRANS_H__ 
/*
 * Des:		for wireless data transport
 * Author:	by xiaoyang @2012-6-15
 * Log:
 *			2012-6-15 xiaoyang create it
 *			2012-6-16 add rf_rx and rf_tx
 */
 
int rf_tx(unsigned char*  tx_buf,unsigned int tx_len,unsigned short dst_rf_addr);
void rf_rx(unsigned char* rx_buf,unsigned int rx_len);

#endif
