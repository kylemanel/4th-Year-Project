#include "c_api.h"


/*	Zephyr BioHarness 3 Packet Functions
1) Packet Mappings (57 types) : Please refer to DOC_c_api.txt

*/
static int makePKT_BH3(PKT* packet,unsigned char *data,unsigned short flags,FILE* log_fd){
	
	if(packet==NULL)
	return 255;	

	return BH3_makePKT(packet->ID,data,packet->data_size,packet->packet,log_fd);
}

static int readPKT_BH3(PKT* packet,unsigned short flags,void** data_list,FILE* log_fd){
	if(packet==NULL)
		return 255;
	packet->data_size=BH3_readPKT(packet->packet,&packet->pkt_size,&packet->data_size,&packet->data_start_index,(DATA_ITEM**)data_list,log_fd);
	if(packet->data_size)
		return 0;
return -1;
}

static int genWritePkts_BH3(CONFIG_ITEM* LL_OPTS,PKT_LIB *lib,PKT** PKT_L,FILE* log_fp){

	*PKT_L=malloc(sizeof(PKT));
	initPKT(*PKT_L,lib);
	PKT *head=*PKT_L;
	CONFIG_ITEM* temp;
	while(LL_OPTS->next!=NULL){
		(*PKT_L)->pkt_size=BH3_genPkt(LL_OPTS,(*PKT_L)->packet,log_fp);
		temp=(CONFIG_ITEM*)LL_OPTS->next;
		BH3_freeConfigItem(LL_OPTS);
		LL_OPTS=temp;
		if(LL_OPTS!=NULL){
			(*PKT_L)->next=malloc(sizeof(PKT));
			(*PKT_L)=(PKT*)(*PKT_L)->next;
			initPKT(*PKT_L,lib);
		}
	}
	*PKT_L=head;
return 0;	
}

int logMessage_BH3(char *message,int size,FILE* fp){
	return BH3_logMessage(message,size,fp);
}
//End of Zephyr BioHarness 3 Packet Functions

void deepCopy(PKT* src, PKT* dst,int max_pkt_size){
	dst->ID=src->ID;
	dst->pkt_size=src->pkt_size;
	dst->data_size=src->data_size;
	dst->data_start_index=src->data_start_index;
	dst->packet=malloc(sizeof(unsigned char)*max_pkt_size);
	memcpy(dst->packet,src->packet,max_pkt_size);

}

void initPKT(PKT* pkt,PKT_LIB* lib){
	pkt->ID=0;
	pkt->pkt_size=0;
	pkt->data_size=0;
	pkt->data_start_index=0;
	pkt->packet=malloc(lib->max_pkt_size);

}

void clearPKT(PKT* pkt){
	pkt->ID=0;
	pkt->pkt_size=0;
	pkt->data_size=0;
	pkt->data_start_index=0;

}

void deletePKT(PKT *pkt){
	free(pkt->packet);
}


//Prepare the library before using -> Not thread safe
int PrepareLib(unsigned char LIBID){

	switch(LIBID){
		case 0: 
			return BH3_prepLib();
			break;
	}
return -1;
}
//Remember to free heap memory from main when done
int regLIB(unsigned char libID,PKT_LIB** lib_struct,char* patient_name){

int rc=0;

switch(libID){

	case 0:
		(*lib_struct)=malloc(sizeof(PKT_LIB));
		(*lib_struct)->readPKT=readPKT_BH3;
		(*lib_struct)->makePKT=makePKT_BH3;
		(*lib_struct)->logMessage=logMessage_BH3;
		(*lib_struct)->LIB_ID=BH3_LIB;
		(*lib_struct)->max_data_size=128;
		(*lib_struct)->max_pkt_size=133;
		(*lib_struct)->device_name=malloc(strlen(patient_name)+1);
		memcpy((*lib_struct)->device_name,patient_name,strlen(patient_name)+1);
		char log_file[strlen(patient_name)+100];
		char conf_file[strlen(patient_name)+100];
		sprintf(log_file,"/home/%s/telenurse/log/%s_log.log",getlogin(),patient_name);
		sprintf(conf_file,"/home/%s/telenurse/conf/%s_lib.conf",getlogin(),patient_name);
		//(*lib_struct)->log_id=BH3_prepLogging(file_name);
		rc+=BH3_prepLogging(log_file,&(*lib_struct)->log_id);
		FILE* conf_fp;
		CONFIG_ITEM* BH3_opts;
		if(BH3_prepConfigParse(&conf_fp,conf_file)>-1){
			BH3_parseConfigFile((*lib_struct)->log_id,conf_fp,&BH3_opts);
			genWritePkts_BH3(BH3_opts,(*lib_struct),&(*lib_struct)->writePkts,(*lib_struct)->log_id);}
		return rc;
}


return -1;
}
