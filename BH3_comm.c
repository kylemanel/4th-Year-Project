#include "BH3_comm.h"

int (*read_periodic_pkts[13])(unsigned char*,const short*,DATA_ITEM**,FILE*);
unsigned char standard_pkt_index_map[NUM_OF_STD_PKTS];
int (*read_standard_pkts[NUM_OF_STD_PKTS])(unsigned char*,const short*,FILE*);
short (*make_standard_pkts[NUM_OF_STD_PKTS])(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE*);

char read_periodic_pkts_set=0;
char read_standard_pkts_set=0;
char make_standard_pkts_set=0;

int log_fd;

static int MSGIDCompare(const void* e1, const void* e2) {
   const unsigned char s1 = *((unsigned char*)e1);
   const unsigned char s2 = *((unsigned char*)e2);

        if(s1<s2)
                return -1;
        else if(s1>s2)
                return 1;
return 0;
}

static int MSGIDLookup(const unsigned char target){
       	const unsigned char *array=standard_pkt_index_map;
        int begin=0;
        int end=NUM_OF_STD_PKTS-1;
        int middle;
        while(end>=begin){
                
                middle = begin+((end-begin)/2);
                
                if(MSGIDCompare(&array[middle],&target) == 0){
                        return middle;}
                else if(MSGIDCompare(&array[middle],&target)<0){
                        begin = middle + 1;}
                else
                        end = middle - 1;               
        
        }
return -1;      
}

static void crc8PushByte(uint8_t *crc, uint8_t ch){

	uint8_t i;
	*crc = *crc ^ ch;
	for(i=0;i<8;i++){
		if(*crc & 1)
			*crc = (*crc >> 1 )^0x8C;
		else
			*crc = (*crc >> 1);
	}
}

static uint8_t crc8PushBlock(uint8_t *pcrc,uint8_t *block,uint16_t count){

	uint8_t crc = pcrc ? *pcrc : 0;
	for (;count >0; --count,block++){
		pthread_mutex_lock(&crc_mutex);
		crc8PushByte(&crc,*block);
		pthread_mutex_unlock(&crc_mutex);
	}
	if(pcrc)
		*pcrc=crc;
	return crc;
}
int BH3_readPKT(unsigned char *buffer,const short *size,unsigned short *data_size,unsigned short *data_start_index,DATA_ITEM** data_list,FILE* log_fd){

		*data_size=buffer[2];
		*data_start_index=buffer[3];
		int standard_function_index=MSGIDLookup(buffer[1]);
		//printf("\nRead Pkt type: %#04x, Pkt CRC %d\n",buffer[1],crc8PushBlock(NULL,&buffer[3],buffer[2]));
	//if(buffer[1]==0x20)
		if(buffer[1] >=0x20 && buffer[1] <=0x2C)
			return read_periodic_pkts[buffer[1]-0x20](buffer,size,data_list,log_fd);
		else if(standard_function_index > -1)
			return read_standard_pkts[standard_function_index](buffer,size,log_fd);
return -1;
}

int BH3_makePKT(unsigned char MSGID,unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){

	int PKT_SIZE=0;
	memset(PKT,0,MAX_PKT_SIZE);
	if(data == NULL)
		PKT_SIZE=5;
	else
		PKT_SIZE=5+data_size;
	
	uint8_t crc = crc8PushBlock(NULL,&data[0],data_size);
	switch(MSGID){
		case LIFESIGN: 		PKT[0]=STX;
					PKT[1]=LIFESIGN;
					PKT[2]=0;
					PKT[3]=0;
					PKT[4]=ETX;
					break;
		case SET_BT_LCONFIG:	PKT[0]=STX;
					PKT[1]=SET_BT_LCONFIG;
					PKT[2]=data_size;
					PKT[3]=data[0];
					PKT[4]=data[1];
					PKT[5]=data[2];
					PKT[6]=data[3];
					PKT[7]=crc;
					PKT[8]=ETX;
					break;
		default:		make_standard_pkts[MSGIDLookup(MSGID)](data,data_size,PKT,log_fd);
					break;
	}
return PKT_SIZE;
}

//*********************************************************************
//**********************Standard Messages Requests*********************
//*********************************************************************
//Read Logging Data Request
static short make_MSG0x01(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Delete Log File
static short make_MSG0x02(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set RTC Date/Time
static short make_MSG0x07(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get RTC Date/Time
static short make_MSG0x08(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		if(PKT!=NULL){	
			PKT[0]=STX;
			PKT[1]=GET_RTC_DATE;
			PKT[2]=0;
			PKT[3]=0;
			PKT[4]=ETX;
		return 5;}
return -1;
}

//Get Boot Software version
static short make_MSG0x09(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		if(PKT!=NULL){
			PKT[0]=STX;
			PKT[1]=GET_BOOT_VER;
			PKT[2]=0;
			PKT[3]=0;
			PKT[4]=ETX;	
			return 5;
		}
return -1;
}

//Get Application Software Version
static short make_MSG0x0A(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
	
		if(PKT!=NULL){
			PKT[0]=STX;
			PKT[1]=GET_APP_VER;
			PKT[2]=0;
			PKT[3]=0;
			PKT[4]=ETX;
			return 5;
		}	
	return -1;
}

//Get Serial Number
static short make_MSG0x0B(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
			
		if(PKT!=NULL){
			PKT[0]=STX;
			PKT[1]=GET_SRLN;
			PKT[2]=0;
			PKT[3]=0;
			PKT[4]=ETX;
			return 5;
		}
return -1;
}

//Get Hardware Part Number
static short make_MSG0x0C(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Bootloader Part Number
static short make_MSG0x0D(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Application Part Number
static short make_MSG0x0E(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Network ID
static short make_MSG0x10(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Network ID
static short make_MSG0x11(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Unit MAC Address
static short make_MSG0x12(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
	if(PKT!=NULL){
		PKT[0]=STX;
		PKT[1]=GET_UNIT_MAC;
		PKT[2]=0;
		PKT[3]=0;
		PKT[4]=ETX;	
		return 5;
	}
return -1;	
}
//Set General Data Packet Transmit State
static short make_MSG0x14(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
	if(PKT!=NULL){
		PKT[0]=STX;
		PKT[1]=SET_GEN_PKT_RATE;
		PKT[2]=data_size;
		PKT[3]=data[0];
		PKT[4]=crc8PushBlock(NULL,&data[0],data_size);
		PKT[5]=ETX;
		return 6;}
return -1;
}

//Set Breathing Wavefor Packet Transmit State
static short make_MSG0x15(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set ECG Waveform Packet Transmist State
static short make_MSG0x16(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Unit Bluetooth Friendly Name
static short make_MSG0x17(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

// Set R to R Data Packet Transmit State
static short make_MSG0x19(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Accelerometer Packet Transmit State
static short make_MSG0x1E(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
	if(PKT!=NULL){
		PKT[0]=STX;
		PKT[1]=SET_ACCL_RATE;
		PKT[2]=data_size;
		PKT[3]=data[0];
		PKT[4]=crc8PushBlock(NULL,&data[0],data_size);
		PKT[5]=ETX;
		return 6;}
return -1;	
	return 0;
}

//Set ROG Settings
static short make_MSG0x9B(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get ROG Settings
static short make_MSG0x9C(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Bluetooth user Config
static short make_MSG0xA2(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Bluetooth User Config
static short make_MSG0xA3(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set BT Link Config
static short make_MSG0xA4(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get BT Link Config
static short make_MSG0xA5(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set BioHarness User Config
static short make_MSG0xA6(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Bio Harness User Config
static short make_MSG0xA7(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Battery Status
static short make_MSG0xAC(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Reboot Unit
static short make_MSG0x1F(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Bluetooth Peripheral Message
static short make_MSG0xB0(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

// Reset Configuration
static short make_MSG0xB3(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Accelerometer Axis Mapping
static short make_MSG0xB4(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Accelerometer Axis Mapping
static short make_MSG0xB5(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Algorithm Config
static short make_MSG0xB6(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Algorithm Config
static short make_MSG0xB7(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Extended Data Packet Transmit State
static short make_MSG0xB8(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set BioHarness User Config Item
static short make_MSG0xB9(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Accelerometer 100mg Packet Transmit State
static short make_MSG0xBC(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Summary Data Packet Update Rate
static short make_MSG0xBD(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Subject Info Settings
static short make_MSG0xBE(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Subject Info Settings
static short make_MSG0xBF(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Set Remote MAC Address & PIN
static short make_MSG0xD0(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Remote MAC Address & PIN
static short make_MSG0xD1(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Remote Device Description
static short make_MSG0xD4(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}

//Get Supported Log Formats
static short make_MSG0xD5(unsigned char *data,unsigned short data_size,unsigned char *PKT,FILE* log_fd){
		
	return 0;
}
//*********************************************************************
//**********************Standard Messages Responses********************
//*********************************************************************

//Read Logging Data Response
static int read_MSG0x01(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Read Logging Data Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Delete Log File
static int read_MSG0x02(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Delete Log File Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set RTC Date/Time
static int read_MSG0x07(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[4]==ACK){
			BH3_logMessage(message,sprintf(message,"[0x07] Date/Time was successfully set"),log_fd);
			return 0;	
		}
		else
			BH3_logMessage(message,sprintf(message,"[0x07] Data/Time was not successfully set"),log_fd);
	return -1;
}

//Get RTC Date/Time
static int read_MSG0x08(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[10]==crc8PushBlock(NULL,&pkt[3],pkt[2])&&pkt[11]==ACK){
			unsigned short year=pkt[6];
			BH3_logMessage(message,sprintf(message,"[0x08] Got RTC Date/Time: %d-%d-%d %d:%d:%d",pkt[3],pkt[4],(year<<8)+pkt[5],pkt[7],pkt[8],pkt[9]),log_fd);
		}
	
		else{
			BH3_logMessage(message,sprintf(message,"[0x08] Get RTC Date/Time: CRC was wrong or Set Packet was invalid"),log_fd);
		}
	return -1;
}

//Get Boot Software version
static int read_MSG0x09(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[11]==crc8PushBlock(NULL,&pkt[3],pkt[2]) && pkt[12]==ACK){
			unsigned short major=pkt[4];
			unsigned short minor=pkt[6];
			unsigned short reserved=pkt[8];
			unsigned short build=pkt[10];
			BH3_logMessage(message,sprintf(message,"[0x09] Got Boot SW Version: %d.%d.%d %d",(major<<8)+pkt[3],(minor<<8)+pkt[5],(reserved<<8)+pkt[7],(build<<8)+pkt[9]),log_fd);
		return 0;
		}
		else{
		BH3_logMessage(message,sprintf(message,"[0x09] Get Boot Version: CRC was wrong or Set Packet was invalid"),log_fd);
		}	
	return 0;
}

//Get Application Software Version
static int read_MSG0x0A(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[11]==crc8PushBlock(NULL,&pkt[3],pkt[2]) && pkt[12]==ACK){
			unsigned short major=pkt[4];
			unsigned short minor=pkt[6];
			unsigned short reserved=pkt[8];
			unsigned short build=pkt[10];
			BH3_logMessage(message,sprintf(message,"[0x0A] Got App SW Version: %d.%d.%d %d",(major<<8)+pkt[3],(minor<<8)+pkt[5],(reserved<<8)+pkt[7],(build<<8)+pkt[9]),log_fd);
		return 0;
		}
		else{
		BH3_logMessage(message,sprintf(message,"[0x0A] Get App SW Version: CRC was wrong or Set Packet was invalid"),log_fd);
		}	
	return -1;
}

//Get Serial Number
static int read_MSG0x0B(unsigned char *pkt,const short * size,FILE* log_fd){
		
		unsigned char serial_num[pkt[2]];
		int count;
		char message[100];
		for(count=0;count<pkt[2];count++)
			serial_num[count]=pkt[3+count];
		if(crc8PushBlock(NULL,&serial_num[0],pkt[2])==pkt[15] && pkt[16]==ACK){
				BH3_logMessage(message,sprintf(message,"[0x0B] Got Serial Number: %s",serial_num),log_fd);
		return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x0B] Get Serial Number: CRC was wrong or Set Packet was invalid"),log_fd);}
return -1;
}
//Get Hardware Part Number
static int read_MSG0x0C(unsigned char *pkt,const short * size,FILE* log_fd){
		unsigned char serial_num[pkt[2]];
		int count;
		char message[100];
		for(count=0;count<pkt[2];count++)
			serial_num[count]=pkt[3+count];
		if(crc8PushBlock(NULL,&serial_num[0],pkt[2])==pkt[15] && pkt[16]==ACK){
				BH3_logMessage(message,sprintf(message,"[0x0C] Got HW Part Number: %s",serial_num),log_fd);
		return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x0C] Get HW Part Number: CRC was wrong or Set Packet was invalid"),log_fd);}
	return -1;
}

//Get Bootloader Part Number
static int read_MSG0x0D(unsigned char *pkt,const short * size,FILE* log_fd){
		unsigned char serial_num[pkt[2]];
		int count;
		char message[100];
		for(count=0;count<pkt[2];count++)
			serial_num[count]=pkt[3+count];
		if(crc8PushBlock(NULL,&serial_num[0],pkt[2])==pkt[15] && pkt[16]==ACK){
				BH3_logMessage(message,sprintf(message,"[0x0C] Got Boot Loader Part Number: %s",serial_num),log_fd);
		return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x0D] Get Boot Loader Part Number: CRC was wrong or Set Packet was invalid"),log_fd);}
	return -1;
}

//Get Application Part Number
static int read_MSG0x0E(unsigned char *pkt,const short * size,FILE* log_fd){
		unsigned char serial_num[pkt[2]];
		int count;
		char message[100];
		for(count=0;count<pkt[2];count++)
			serial_num[count]=pkt[3+count];
		if(crc8PushBlock(NULL,&serial_num[0],pkt[2])==pkt[15] && pkt[16]==ACK){
				BH3_logMessage(message,sprintf(message,"[0x0E] Got App Part Number: %s",serial_num),log_fd);
		return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x0E] Get App Part Number: CRC was wrong or Set Packet was invalid"),log_fd);}
	return -1;
}

//Set Network ID
static int read_MSG0x10(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[4]==ACK){
			BH3_logMessage(message,sprintf(message,"[0x10] Network ID was successfully set"),log_fd);
			return 0;	
		}
		else
			BH3_logMessage(message,sprintf(message,"[0x10] Network ID: CRC was wrong or Set Packt was invalid"),log_fd);
	return -1;
	return 0;
}

//Get Network ID
static int read_MSG0x11(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if((pkt[2]+4) == crc8PushBlock(NULL,&pkt[3],pkt[2]) && (pkt[2]+5)==ACK){
			unsigned char net_id[pkt[2]];
			int count=0;
			for(;count<pkt[2];count++)
				net_id[count]=pkt[3+count];
			BH3_logMessage(message,sprintf(message,"[0x11] Got Network ID: %s",net_id),log_fd);
			return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x11] Get Network ID: CRC was wrong or Set Packet was invalid"),log_fd);
		}
	return -1;
}

//Get Unit MAC Address
static int read_MSG0x12(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if((pkt[20]) == crc8PushBlock(NULL,&pkt[3],pkt[2]) && (pkt[21])==ACK){
			unsigned char mac_addr[pkt[2]+1];
			int count=0;
			for(;count<pkt[2];count++)
				mac_addr[count]=pkt[3+count];
			mac_addr[pkt[2]]=0;
			BH3_logMessage(message,sprintf(message,"[0x12] Got MAC address: %s",mac_addr),log_fd);
			return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x12] Get MAC address: CRC was wrong or Set Packet was invalid"),log_fd);
		}
	return -1;
}
//Set General Data Packet Transmit State
static int read_MSG0x14(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[4]==ACK){
			BH3_logMessage(message,sprintf(message,"[0x14] Genral Packet Transmission Enabled"),log_fd);
			return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0x14] General Packet Transmission: was not enabled or the response was corrupted"),log_fd);
		}
	return 0;
}

//Set Breathing Wavefor Packet Transmit State
static int read_MSG0x15(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Breathing Waveform Packet Transmit State Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set ECG Waveform Packet Transmist State
static int read_MSG0x16(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set ECG Waveform Packet Transmit State Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Unit Bluetooth Friendly Name
static int read_MSG0x17(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Unit BT Friendly Name Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

// Set R to R Data Packet Transmit State
static int read_MSG0x19(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set R to R Data Packet Transmist State Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Accelerometer Packet Transmit State
static int read_MSG0x1E(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Accelerometer Packet Transmit State Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set ROG Settings
static int read_MSG0x9B(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set ROG Settings Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get ROG Settings
static int read_MSG0x9C(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get ROG Settings Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Bluetooth user Config
static int read_MSG0xA2(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set BT User Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Bluetooth User Config
static int read_MSG0xA3(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get BT User Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set BT Link Config
static int read_MSG0xA4(unsigned char *pkt,const short * size,FILE* log_fd){
		char message[100];
		if(pkt[4]==ACK){
			BH3_logMessage(message,sprintf(message,"[0xA4] BT Link Config Set"),log_fd);
			return 0;
		}
		else{
			BH3_logMessage(message,sprintf(message,"[0xA4] BT Link Config: Failed to set BT Link config or response message was corrupted"),log_fd);
		}
	return -1;
}

//Get BT Link Config
static int read_MSG0xA5(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get BT Link Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set BioHarness User Config
static int read_MSG0xA6(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set BH User Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Bio Harness User Config
static int read_MSG0xA7(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get BH User config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Battery Status
static int read_MSG0xAC(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Battery Status Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Reboot Unit
static int read_MSG0x1F(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Reboot Unit Reponse packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Bluetooth Peripheral Message
static int read_MSG0xB0(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("BT Peripheral Message Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

// Reset Configuration
static int read_MSG0xB3(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Reset Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Accelerometer Axis Mapping
static int read_MSG0xB4(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Accelerometer Axis Mapping Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Accelerometer Axis Mapping
static int read_MSG0xB5(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Accelerometer Axis Mapping Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Algorithm Config
static int read_MSG0xB6(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Algorithm Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Algorithm Config
static int read_MSG0xB7(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Algorithm Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Extended Data Packet Transmit State
static int read_MSG0xB8(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Extended Data Packet Transmist State Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set BioHarness User Config Item
static int read_MSG0xB9(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set BH User Config Item Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Accelerometer 100mg Packet Transmit State
static int read_MSG0xBC(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Accl. 100mg Packet Trans. State Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Summary Data Packet Update Rate
static int read_MSG0xBD(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Summary Data Packet Update Rate Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Subject Info Settings
static int read_MSG0xBE(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Subject Info Settings Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Subject Info Settings
static int read_MSG0xBF(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Subject Info Settings Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Set Remote MAC Address & PIN
static int read_MSG0xD0(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Set Remote MAC Addr & PIN Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Remote MAC Address & PIN
static int read_MSG0xD1(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Remote MAC Addr & PIN Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Remote Device Description
static int read_MSG0xD4(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Remote Device Config Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}

//Get Supported Log Formats
static int read_MSG0xD5(unsigned char *pkt,const short * size,FILE* log_fd){
		printf("Get Supported Log Formates Response packet: %#0x with CRC %d\n",pkt[1],pkt[2]+2);
	return 0;
}
//*****************************************************************
//****************************Periodic Messages********************
//*****************************************************************
//General Data Packet
static int read_MSG0x20(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		
		unsigned short year = (pkt[5]<<8)+pkt[4];
		//short skin_temp=(pkt[17]<<8)+pkt[16];
		unsigned long timestamp=(((unsigned long)pkt[11]<<24)+((unsigned long)pkt[10]<<16)+((unsigned long)pkt[9]<<8)+pkt[8])/1000;
		short heart_rate=(pkt[13]<<8)+pkt[12];
		char battery=pkt[54];
		printf("General Data Packet: %#0x with CRC: %d, battery %d, heart_rate %d, date %d-%d-%d %lu:%lu:%lu\n",pkt[1],pkt[pkt[2]+2],battery,heart_rate,year,pkt[6],pkt[5],timestamp/3600,(timestamp/60)%60,timestamp%60);
		 
	return 0;
}

//Breathing Waveform Packet
static int read_MSG0x21(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("Breathing Waveform Packet: %#0x with CRC: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}

//ECG Waveform Packet
static int read_MSG0x22(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("ECG Waveform Packet: %#0x with CRC: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}
//LifeSign Packet
static int read_MSG0x23(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("LifeSign Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}
// R to R Packet
static int read_MSG0x24(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("R to R Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}

//Accelerometer packet
static int read_MSG0x25(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		//printf("Accelerometer Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
		unsigned short year = (pkt[5]<<8)+pkt[4];
		unsigned long timestamp=(((unsigned long)pkt[11]<<24)+((unsigned long)pkt[10]<<16)+((unsigned long)pkt[9]<<8)+pkt[8])/1000;
		printf("Accelerometer Packet: %#0x with crc: %d, timestamp: %d-%d-%d %lu:%lu:%lu sample1(x\\y\\z): %d\\%d\\%d\n",pkt[1],pkt[pkt[2]+3],year,pkt[6],pkt[7],timestamp/3600,(timestamp/60)%60,timestamp%60,pkt[12]+(pkt[13]&0x03),(pkt[13]&0xFC)+(pkt[14]&0x0F),(pkt[14]&0xF0)+(pkt[15]&0x3F));
	return 0;
}
//BlueTooth Device Data Packet
static int read_MSG0x27(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("Bluetooth Device Data Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}
//Extended Data Packet
static int read_MSG0x28(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("Extended Data Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}
//Accelerometer 100mg Packet
static int read_MSG0x2A(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("Accelerometer 100mg Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}
//Summary Data Packet
static int read_MSG0x2B(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("Summary Data Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}
//Event Packet
static int read_MSG0x2C(unsigned char *pkt,const short *size,DATA_ITEM** data_list,FILE* log_fd){
		printf("Event Packet: %#0x with crc: %d\n",pkt[1],pkt[pkt[2]+2]);
	return 0;
}

/*****************End of Periodic Packet functions******************/


int BH3_prepLib(){
	pthread_mutexattr_init(&crc_mutexattr);
	pthread_mutexattr_settype(&crc_mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&crc_mutex,&crc_mutexattr);
	if(read_periodic_pkts_set==0){
		read_periodic_pkts_set=1;
		read_periodic_pkts[0]=read_MSG0x20;
		read_periodic_pkts[1]=read_MSG0x21;
		read_periodic_pkts[2]=read_MSG0x22;
		read_periodic_pkts[3]=read_MSG0x23;
		read_periodic_pkts[4]=read_MSG0x24;
		read_periodic_pkts[5]=read_MSG0x25;
		read_periodic_pkts[7]=read_MSG0x27;
		read_periodic_pkts[8]=read_MSG0x28;
		read_periodic_pkts[10]=read_MSG0x2A;
		read_periodic_pkts[11]=read_MSG0x2B;
		read_periodic_pkts[12]=read_MSG0x2C;
	}

		if(read_standard_pkts_set==0 && make_standard_pkts_set==0){
			read_standard_pkts_set=make_standard_pkts_set=1;
			standard_pkt_index_map[0]=RD_LOG_DATA;
			standard_pkt_index_map[1]=DEL_LOG_FILE;
			standard_pkt_index_map[2]=SET_RTC_DATE;
			standard_pkt_index_map[3]=GET_RTC_DATE;
			standard_pkt_index_map[4]=GET_BOOT_VER;
			standard_pkt_index_map[5]=GET_APP_VER;
			standard_pkt_index_map[6]=GET_SRLN;
			standard_pkt_index_map[7]=GET_HW_NUM;
			standard_pkt_index_map[8]=GET_BOOTL_NUM;
			standard_pkt_index_map[9]=GET_APP_NUM;
			standard_pkt_index_map[10]=SET_NET_ID;
			standard_pkt_index_map[11]=GET_NET_ID;
			standard_pkt_index_map[12]=GET_UNIT_MAC;
			standard_pkt_index_map[13]=SET_GEN_PKT_RATE;
			standard_pkt_index_map[14]=SET_BREATHE_WF_RATE;
			standard_pkt_index_map[15]=SET_ECG_WF_RATE;
			standard_pkt_index_map[16]=GET_BT_NAME;
			standard_pkt_index_map[17]=SET_RR_RATE;
			standard_pkt_index_map[18]=SET_ACCL_RATE;
			standard_pkt_index_map[19]=SET_ROG;
			standard_pkt_index_map[20]=GET_ROG;
			standard_pkt_index_map[21]=SET_BT_UCONFIG;
			standard_pkt_index_map[22]=GET_BT_UCONFIG;
			standard_pkt_index_map[23]=SET_BT_LCONFIG;
			standard_pkt_index_map[24]=GET_BT_LCONFIG;
			standard_pkt_index_map[25]=SET_BH_UCONFIG;
			standard_pkt_index_map[26]=GET_BH_UCONFIG;
			standard_pkt_index_map[27]=GET_BATT_STAT;
			standard_pkt_index_map[28]=REBOOT;
			standard_pkt_index_map[29]=BT_PERI_MSG;
			standard_pkt_index_map[30]=RESET_CONFIG;
			standard_pkt_index_map[31]=SET_ACCL_AXIS;
			standard_pkt_index_map[32]=GET_ACCL_AXIS;
			standard_pkt_index_map[33]=SET_ALG_CONFIG;
			standard_pkt_index_map[34]=GET_ALG_CONFIG;
			standard_pkt_index_map[35]=SET_EXT_RATE;
			standard_pkt_index_map[36]=SET_BH_UCONFIG_ITEM;
			standard_pkt_index_map[37]=SET_ACCL_100MG_RATE;
			standard_pkt_index_map[38]=SET_SUMM_RATE;
			standard_pkt_index_map[39]=SET_SUBJ_INFO;
			standard_pkt_index_map[40]=GET_SUBJ_INFO;
			standard_pkt_index_map[41]=SET_REMOTE_MAC_PIN;
			standard_pkt_index_map[42]=GET_REMOTE_MAC_PIN;
			standard_pkt_index_map[43]=GET_REMOTE_DESCR;
			standard_pkt_index_map[44]=GET_SUP_LOG_FORMAT;
			qsort(standard_pkt_index_map, sizeof(standard_pkt_index_map), sizeof(unsigned char), MSGIDCompare);
			make_standard_pkts[MSGIDLookup(RD_LOG_DATA)]=make_MSG0x01;
			read_standard_pkts[MSGIDLookup(RD_LOG_DATA)]=read_MSG0x01;
			make_standard_pkts[MSGIDLookup(DEL_LOG_FILE)]=make_MSG0x02;
			read_standard_pkts[MSGIDLookup(DEL_LOG_FILE)]=read_MSG0x02;
			make_standard_pkts[MSGIDLookup(SET_RTC_DATE)]=make_MSG0x07;
			read_standard_pkts[MSGIDLookup(SET_RTC_DATE)]=read_MSG0x07;
			make_standard_pkts[MSGIDLookup(GET_RTC_DATE)]=make_MSG0x08;
			read_standard_pkts[MSGIDLookup(GET_RTC_DATE)]=read_MSG0x08;
			make_standard_pkts[MSGIDLookup(GET_BOOT_VER)]=make_MSG0x09;
			read_standard_pkts[MSGIDLookup(GET_BOOT_VER)]=read_MSG0x09;
			make_standard_pkts[MSGIDLookup(GET_APP_VER)]=make_MSG0x0A;
			read_standard_pkts[MSGIDLookup(GET_APP_VER)]=read_MSG0x0A;
			make_standard_pkts[MSGIDLookup(GET_SRLN)]=make_MSG0x0B;
			read_standard_pkts[MSGIDLookup(GET_SRLN)]=read_MSG0x0B;
			make_standard_pkts[MSGIDLookup(GET_HW_NUM)]=make_MSG0x0C;
			read_standard_pkts[MSGIDLookup(GET_HW_NUM)]=read_MSG0x0C;
			make_standard_pkts[MSGIDLookup(GET_BOOTL_NUM)]=make_MSG0x0D;
			read_standard_pkts[MSGIDLookup(GET_BOOTL_NUM)]=read_MSG0x0D;
			make_standard_pkts[MSGIDLookup(GET_APP_NUM)]=make_MSG0x0E;
			read_standard_pkts[MSGIDLookup(GET_APP_NUM)]=read_MSG0x0E;
			make_standard_pkts[MSGIDLookup(SET_NET_ID)]=make_MSG0x10;
			read_standard_pkts[MSGIDLookup(SET_NET_ID)]=read_MSG0x10;
			make_standard_pkts[MSGIDLookup(GET_NET_ID)]=make_MSG0x11;
			read_standard_pkts[MSGIDLookup(GET_NET_ID)]=read_MSG0x11;
			make_standard_pkts[MSGIDLookup(GET_UNIT_MAC)]=make_MSG0x12;
			read_standard_pkts[MSGIDLookup(GET_UNIT_MAC)]=read_MSG0x12;
			make_standard_pkts[MSGIDLookup(SET_GEN_PKT_RATE)]=make_MSG0x14;
			read_standard_pkts[MSGIDLookup(SET_GEN_PKT_RATE)]=read_MSG0x14;
			make_standard_pkts[MSGIDLookup(SET_BREATHE_WF_RATE)]=make_MSG0x15;
			read_standard_pkts[MSGIDLookup(SET_BREATHE_WF_RATE)]=read_MSG0x15;
			make_standard_pkts[MSGIDLookup(SET_ECG_WF_RATE)]=make_MSG0x16;
			read_standard_pkts[MSGIDLookup(SET_ECG_WF_RATE)]=read_MSG0x16;
			make_standard_pkts[MSGIDLookup(GET_BT_NAME)]=make_MSG0x17;
			read_standard_pkts[MSGIDLookup(GET_BT_NAME)]=read_MSG0x17;
			make_standard_pkts[MSGIDLookup(SET_RR_RATE)]=make_MSG0x19;
			read_standard_pkts[MSGIDLookup(SET_RR_RATE)]=read_MSG0x19;
			make_standard_pkts[MSGIDLookup(SET_ACCL_RATE)]=make_MSG0x1E;
			read_standard_pkts[MSGIDLookup(SET_ACCL_RATE)]=read_MSG0x1E;
			make_standard_pkts[MSGIDLookup(SET_ROG)]=make_MSG0x9B;
			read_standard_pkts[MSGIDLookup(SET_ROG)]=read_MSG0x9B;
			make_standard_pkts[MSGIDLookup(GET_ROG)]=make_MSG0x9C;
			read_standard_pkts[MSGIDLookup(GET_ROG)]=read_MSG0x9C;
			make_standard_pkts[MSGIDLookup(SET_BT_UCONFIG)]=make_MSG0xA2;
			read_standard_pkts[MSGIDLookup(SET_BT_UCONFIG)]=read_MSG0xA2;
			make_standard_pkts[MSGIDLookup(GET_BT_UCONFIG)]=make_MSG0xA3;
			read_standard_pkts[MSGIDLookup(GET_BT_UCONFIG)]=read_MSG0xA3;
			make_standard_pkts[MSGIDLookup(SET_BT_LCONFIG)]=make_MSG0xA4;
			read_standard_pkts[MSGIDLookup(SET_BT_LCONFIG)]=read_MSG0xA4;
			make_standard_pkts[MSGIDLookup(GET_BT_LCONFIG)]=make_MSG0xA5;
			read_standard_pkts[MSGIDLookup(GET_BT_LCONFIG)]=read_MSG0xA5;
			make_standard_pkts[MSGIDLookup(SET_BH_UCONFIG)]=make_MSG0xA6;
			read_standard_pkts[MSGIDLookup(SET_BH_UCONFIG)]=read_MSG0xA6;
			make_standard_pkts[MSGIDLookup(GET_BH_UCONFIG)]=make_MSG0xA7;
			read_standard_pkts[MSGIDLookup(GET_BH_UCONFIG)]=read_MSG0xA7;
			make_standard_pkts[MSGIDLookup(GET_BATT_STAT)]=make_MSG0xAC;
			read_standard_pkts[MSGIDLookup(GET_BATT_STAT)]=read_MSG0xAC;
			make_standard_pkts[MSGIDLookup(REBOOT)]=make_MSG0x1F;
			read_standard_pkts[MSGIDLookup(REBOOT)]=read_MSG0x1F;
			make_standard_pkts[MSGIDLookup(BT_PERI_MSG)]=make_MSG0xB0;
			read_standard_pkts[MSGIDLookup(BT_PERI_MSG)]=read_MSG0xB0;
			make_standard_pkts[MSGIDLookup(RESET_CONFIG)]=make_MSG0xB3;
			read_standard_pkts[MSGIDLookup(RESET_CONFIG)]=read_MSG0xB3;
			make_standard_pkts[MSGIDLookup(SET_ACCL_AXIS)]=make_MSG0xB4;
			read_standard_pkts[MSGIDLookup(SET_ACCL_AXIS)]=read_MSG0xB4;
			make_standard_pkts[MSGIDLookup(GET_ACCL_AXIS)]=make_MSG0xB5;
			read_standard_pkts[MSGIDLookup(GET_ACCL_AXIS)]=read_MSG0xB5;
			make_standard_pkts[MSGIDLookup(SET_ALG_CONFIG)]=make_MSG0xB6;
			read_standard_pkts[MSGIDLookup(SET_ALG_CONFIG)]=read_MSG0xB6;
			make_standard_pkts[MSGIDLookup(GET_ALG_CONFIG)]=make_MSG0xB7;
			read_standard_pkts[MSGIDLookup(GET_ALG_CONFIG)]=read_MSG0xB7;
			make_standard_pkts[MSGIDLookup(SET_EXT_RATE)]=make_MSG0xB8;
			read_standard_pkts[MSGIDLookup(SET_EXT_RATE)]=read_MSG0xB8;
			make_standard_pkts[MSGIDLookup(SET_BH_UCONFIG_ITEM)]=make_MSG0xB9;
			read_standard_pkts[MSGIDLookup(SET_BH_UCONFIG_ITEM)]=read_MSG0xB9;
			make_standard_pkts[MSGIDLookup(SET_ACCL_100MG_RATE)]=make_MSG0xBC;
			read_standard_pkts[MSGIDLookup(SET_ACCL_100MG_RATE)]=read_MSG0xBC;
			make_standard_pkts[MSGIDLookup(SET_SUMM_RATE)]=make_MSG0xBD;
			read_standard_pkts[MSGIDLookup(SET_SUMM_RATE)]=read_MSG0xBD;
			make_standard_pkts[MSGIDLookup(SET_SUBJ_INFO)]=make_MSG0xBE;
			read_standard_pkts[MSGIDLookup(SET_SUBJ_INFO)]=read_MSG0xBE;
			make_standard_pkts[MSGIDLookup(GET_SUBJ_INFO)]=make_MSG0xBF;
			read_standard_pkts[MSGIDLookup(GET_SUBJ_INFO)]=read_MSG0xBF;
			make_standard_pkts[MSGIDLookup(SET_REMOTE_MAC_PIN)]=make_MSG0xD0;
			read_standard_pkts[MSGIDLookup(SET_REMOTE_MAC_PIN)]=read_MSG0xD0;
			make_standard_pkts[MSGIDLookup(GET_REMOTE_MAC_PIN)]=make_MSG0xD1;
			read_standard_pkts[MSGIDLookup(GET_REMOTE_MAC_PIN)]=read_MSG0xD1;
			make_standard_pkts[MSGIDLookup(GET_REMOTE_DESCR)]=make_MSG0xD4;
			read_standard_pkts[MSGIDLookup(GET_REMOTE_DESCR)]=read_MSG0xD4;
			make_standard_pkts[MSGIDLookup(GET_SUP_LOG_FORMAT)]=make_MSG0xD5;
			read_standard_pkts[MSGIDLookup(GET_SUP_LOG_FORMAT)]=read_MSG0xD5;


		}
return 1;
}

int BH3_genPkt(CONFIG_ITEM* OPTS,unsigned char* PKT,FILE* log_fp){
	int index=MSGIDLookup(OPTS->id);
	if(index>-1){
		return make_standard_pkts[index](OPTS->data,OPTS->data_size,PKT,log_fp);
	}
	char message[100];
	BH3_logMessage(message,sprintf(message,"Lib: Option ID %d is not valid",OPTS->id),log_fp);
return index;
}
