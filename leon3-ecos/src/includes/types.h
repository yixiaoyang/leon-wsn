/*
 * types.h
 *
 *  Created on: 2011-2-22
 *      Author: hgdytz
 */

#ifndef TYPES_H_
#define TYPES_H_

#define REG32(x)	*((volatile unsigned long*)(x))

typedef unsigned char 	int8u_t;
typedef signed char 	int8s_t;

typedef unsigned short	int16u_t;
typedef signed short 	int16s_t;

typedef unsigned long	int32u_t;
typedef signed long	int32s_t;
/** Another Style */
typedef unsigned char 	u8_t;
typedef signed char 	s8_t;

typedef unsigned short	u16_t;
typedef signed short 	s16_t;

typedef unsigned long	u32_t;
typedef signed long	s32_t;

typedef int8u_t 	mybool;
typedef int8s_t 	result_t;

#define FALSE		0
#define TRUE		!FALSE
#define FAIL		-1
#define PASS		1

#define BIT_0		0x01
#define BIT_1 		0x02
#define BIT_2		0x04
#define BIT_3		0x08
#define BIT_4		0x10
#define BIT_5		0x20
#define BIT_6		0x40
#define BIT_7		0x80

#define BIT_8		0x100
#define BIT_9		0x200
#define BIT_10		0x400


#endif /* TYPES_H_ */
