//===================================================================
//                       rf.h
//                 Radio frequency device interface
//               Jason Liang: co.lang.ol@gmail.com
//
//===================================================================
#ifndef RF_H_
#define RF_H_
#include "types.h"
//-------------------------------------------------------------------
//                       NIC device settings
//-------------------------------------------------------------------

/* Power saving modes */
#define 	HALT			0x01
#define 	STANDBY		0x02
/* Function Enabled */
#define 	RF_TxN_EN			1
/* Network and Device address */
//#define DEV_TYPE_TX		0

#define PANIDL		0xAA
#define PANIDH		0xAA

#if defined TRANSMITTER          // Transmitter
/* Zigbee NIC 64-bit extended address */
#define EADR_0		0x11
#define EADR_1		0x12
#define EADR_2		0x13
#define EADR_3		0x14
#define EADR_4		0x15
#define EADR_5		0x16
#define EADR_6		0x17
#define EADR_7		0x18

#define SADRL			0x11
#define SADRH			0x11

#elif defined RECEIVER                  // Receiver
#define EADR_0		0x21
#define EADR_1		0x22
#define EADR_2		0x23
#define EADR_3		0x24
#define EADR_4		0x25
#define EADR_5		0x26
#define EADR_6		0x27
#define EADR_7		0x28

#define SADRL			0x22
#define SADRH			0x22

#else
#define EADR_0		0x00
#define EADR_1		0x00
#define EADR_2		0x00
#define EADR_3		0x00
#define EADR_4		0x00
#define EADR_5		0x00
#define EADR_6		0x00
#define EADR_7		0x00

#define SADRL			0x00
#define SADRH			0x00

#endif // DEV_TYPE_TX
//-------------------------------------------------------------------
//                          Exported Functions
//-------------------------------------------------------------------

result_t RF_TxN(int8u_t *tx_data, int8u_t tx_data_len, bool ack_req);
int8u_t RF_Rx(int8u_t *buffer);

void RF_CHIP_INITIALIZE(void);
void RF_NORMAL_SPEED(void);
void RF_TURBO_SPEED_1M(void);
#endif /* RF_H_ */

