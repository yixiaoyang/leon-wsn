/*
 * rf.h
 *
 *  Created on: 2011-3-7
 *      Author: hgdytz
 */

#ifndef RF_H_
#define RF_H_

#include "types.h"
/* Power saving modes */
#define 	HALT			0x01
#define 	STANDBY			0x02


/* Function Enabled */
#define 	RF_TxN_EN		1


/* Exported Functions */
result_t RF_TxN(int8u_t *tx_data, int8u_t tx_data_len, mybool ack_req);
void RF_CHIP_INITIALIZE(void);
int8u_t RF_Rx(int8u_t *buffer);
#endif /* RF_H_ */
