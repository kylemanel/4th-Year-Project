#ifndef C_API_H
#define C_API_H
#include "BH3_lib.h"
//This section for Library IDs
#define BH3_LIB 0x0
//
typedef struct{
	unsigned char ID;
	unsigned char *packet;
	short pkt_size;
	unsigned short data_size;
	unsigned short data_start_index;
	void *next;	
} PKT;


typedef struct{
	const unsigned char *LIB_ID;
	unsigned short max_pkt_size;
	unsigned short max_data_size;
	char* device_name;
	FILE* log_id;
	int (*makePKT)(PKT*,unsigned char*,unsigned short,FILE*);
	int (*readPKT)(PKT*,unsigned short,void**,FILE*);
	int (*logMessage)(char*,int,FILE*);
	PKT *writePkts;
} PKT_LIB;

int PrepareLib(unsigned char);
int regLIB(unsigned char,PKT_LIB**,char*);
void initPKT(PKT*,PKT_LIB*);
//Empties all content in the PKT structure
void clearPKT(PKT*);
//Deletes all heap memory allocations in the PKT structure
void deletePKT(PKT*);
void deepCopy(PKT* src,PKT* dst,int max_pkt_size);
#endif
