#ifndef  APP
#define  APP

#include "types.h"
#include "data.h"

/* define for file types */
#define WAV16MONE  0x01

void audioFSend  (void * buffer, int32u_t length, int8u_t types, int32u_t delaytime);
void audioDHandle(void * buffer, int32u_t length);

#endif
