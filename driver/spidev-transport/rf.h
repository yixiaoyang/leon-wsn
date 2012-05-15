/*
 * rf.h
 *
 *  Created on: 2011-3-7
 *      Author: 
 */

#ifndef RF_H_
#define RF_H_

#include "ieee802_15_4.h"
#include "heads.h"
#include "spidev.h"

/* Power saving modes */
#define 	HALT			0x01
#define 	STANDBY			0x02

/* Function Enabled */
#define 	RF_TxN_EN		1

/* Exported Functions */
void RF_CHIP_INITIALIZE(struct spidev_data* rf_dev);
void RF_NET_CONFIG(struct spidev_data* rf_dev);
void RF_TEST(struct spidev_data* rf_dev);
s8 RF_TxN(struct spidev_data* rf_dev, u8 *tx_data, u8 tx_data_len, u8 ack_req);
u32 RF_Rx(struct spidev_data* rf_dev, u8 *buffer);

#endif /* RF_H_ */






