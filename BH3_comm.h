#ifndef BH3_COMM_H
#define BH3_COMM_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "BH3_shared.h"
/*#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NACK 0x15
#define GEN_DATA 0x14
#define GET_SRLN 0x0B
#define BT_LINK_CONF 0xA4

#define NAK 0x15
#define MAX_PKT_SIZE 133
#define MAX_DATA_SIZE 128*/


//MSG ID definitions
pthread_mutex_t crc_mutex;
pthread_mutexattr_t crc_mutexattr;

int BH3_prepLib();
int BH3_readPKT(unsigned char *,const short*,unsigned short *,unsigned short *,DATA_ITEM**,FILE*);
int BH3_makePKT(unsigned char,unsigned char*,unsigned short,unsigned char*,FILE*);
int BH3_genPkt(CONFIG_ITEM*,unsigned char*,FILE*);
#endif
