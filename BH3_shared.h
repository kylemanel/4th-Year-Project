#ifndef BH3_SHARED_H
#define BH3_SHARED_H
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
/*This section for BH3 PKT definitions

Section 1: Standard Packets
Section 2: Periodic Packets
Section 3: MISC definitions

Please refer to the "BioHarness Bluetooth Comms Link Specification" documents for further details
*/

//--------Standard Packets--------
#define NUM_OF_STD_PKTS 45
#define RD_LOG_DATA 0x01
#define DEL_LOG_FILE 0x02
#define SET_RTC_DATE 0x07
#define GET_RTC_DATE 0x08
#define GET_BOOT_VER 0x09
#define GET_APP_VER 0x0A
#define GET_SRLN 0x0B
#define GET_HW_NUM 0x0C
#define GET_BOOTL_NUM 0x0D
#define GET_APP_NUM 0x0E
#define SET_NET_ID 0x10
#define GET_NET_ID 0x11
#define GET_UNIT_MAC 0x12
#define SET_GEN_PKT_RATE 0x14
#define SET_BREATHE_WF_RATE 0x15
#define SET_ECG_WF_RATE 0x16
#define GET_BT_NAME 0x17
#define SET_RR_RATE 0x19
#define SET_ACCL_RATE 0x1E
#define SET_ROG 0x9B
#define GET_ROG 0x9C
#define SET_BT_UCONFIG 0xA2
#define GET_BT_UCONFIG 0xA3
#define SET_BT_LCONFIG 0xA4
#define GET_BT_LCONFIG 0xA5
#define SET_BH_UCONFIG 0xA6
#define GET_BH_UCONFIG 0xA7
#define GET_BATT_STAT 0xAC
#define REBOOT 0x1F
#define BT_PERI_MSG 0xB0
#define RESET_CONFIG 0xB3
#define SET_ACCL_AXIS 0xB4
#define GET_ACCL_AXIS 0xB5
#define SET_ALG_CONFIG 0xB6
#define GET_ALG_CONFIG 0xB7
#define SET_EXT_RATE 0xB8
#define SET_BH_UCONFIG_ITEM 0xB9
#define SET_ACCL_100MG_RATE 0xBC
#define SET_SUMM_RATE 0xBD
#define SET_SUBJ_INFO 0xBE
#define GET_SUBJ_INFO 0xBF
#define SET_REMOTE_MAC_PIN 0xD0
#define GET_REMOTE_MAC_PIN 0xD1
#define GET_REMOTE_DESCR 0xD4
#define GET_SUP_LOG_FORMAT 0xD5
//---------End of Section-------

//---------Periodic Packets----------
#define GEN_PKT 0x20
#define BRTH_PKT 0x21
#define ECG_WF_PKT 0x22
#define LIFESIGN 0x23
#define RR_PKT 0x24
#define ACCL_PKT 0x25
#define BT_DEV_DATA_PKT 0x27
#define EXT_DATA_PKT 0x28
#define ACCL_100MG_PKT 0x2A
#define SUMM_PKT 0x2B
#define EVENT_PKT 0x2C
//----------End of Section-----------

//---------MISC Definitions--------
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15
#define MAX_PKT_SIZE 133
#define MAX_DATA_SIZE 128
//---------End of Section----------

//---------Post Var index mapping--
#define VAR_TIMESTAMP 0x01
#define VAR_HRT_RATE 0x02
#define VAR_RESP_RATE 0x03 //Respiration rate
#define VAR_SKIN_TEMP 0x04
#define VAR_POSTURE 0x05
#define VAR_VMU 0x06
#define VAR_PEAK_ACCEL 0x07
#define VAR_BATT_VOL 0x08
#define VAR_BREATHE_WA 0x09 //Breathing Wave Amplitude
#define VAR_ECG_AMP 0x0A
#define VAR_ECG_NOISE 0x0B
#define VAR_VERT_ACCEL_MIN 0x0C
#define VAR_VERT_ACCEL_PEAK 0x0D
#define VAR_LAT_ACCEL_MIN 0x0E //Lateral Acceleration
#define VAR_LAT_ACCEL_PEAK 0x0F 
#define VAR_SAG_ACCEL_MIN 0x10 //Sagittal Acceleration
#define VAR_SAG_ACCEL_PEAK 0x11
#define VAR_ZEPHYR_SYS_CHNL 0x12
#define VAR_GSR 0x13
#define VAR_ROG_STATUS 0x14
#define VAR_ROG_TIME 0x15
#define VAR_ALARM 0x16
#define VAR_BATT_STAT 0x17 //Battery percentage
//****The following VAR types will have true/false data (1 or 0)***
#define VAR_PMWS 0x18 //Physiological Monitor Worn Status (if user is wearing harness or not)
#define VAR_UIBS 0x19 //User interface button pressed or not
#define VAR_BHHSL 0x1A //BH heart-rate signal low
#define VAR_BHESC 0x1B //BH External Sensors connected
//****End of bool vars
#define VAR_BREATHE_WF 0x1C //Breathing waveform, there are 18 samples in one data string
#define VAR_ECG_WF 0x1D //ECG Waveform. there are 63 samples in one data string
#define VAR_RTR 0x1E //R to R data, there are 18 samples in one data string
#define VAR_ACCEL_DATA 0x1F// Accelerometer data, there are 20 samples in one data string
//*****This data is for info from a third party BT device
#define VAR_DEV_MANUFAC_CODE 0x20
#define VAR_DEV_TYPE_CODE 0x21
#define VAR_BLUETOOTH_MAC 0x22
#define VAR_STATUS 0x23
#define VAR_BDMS_VALID 0x24
#define VAR_BDMS_UNRELIABLE 0x25
#define VAR_BDMS_UNABLE_CONNECT 0x26
#define VAR_BDMS_AUTH_ERR 0x27
#define VAR_BDMS_COMM_ERR 0x28
#define VAR_BDMS_FAIL_MEASURE 0x29
//****End of third party var definitions section
//****Heart Rate Recovery variables
#define VAR_HRR_PEAK 0x2A //Peak heart rate after exercise
#define VAR_HRR_30_EX 0x2B // Peak heart rate after 30s of exercise
#define VAR_HRR_60_EX 0x2C
#define VAR_HRR_120_EX 0x2D
#define VAR_HRR_180_EX 0x2E
//****End of HRR section
//****Orthostatic Hypotension definitions
#define VAR_OH_AVG_60 0x2F //Average HR 60s before standing
#define VAR_OH_INST_HR 0x30 //Instantaneous HR before standing
#define VAR_OH_PEAK_HR 0x31 //Peak Heart rate during first 15s of standing
#define VAR_OH_AVG_HR 0x32 //Average HR 60s after standing
//****End of OH section****
//****Vertical Jump test section****
#define VAR_VJ_AIRB 0x33 //Airbone time
#define VAR_VJ_PEAK_ACCEL 0x34 //Peak acceleration during jumping
//****End of vertical jump test section***
//**** Forty yard dash vars
#define VAR_40Y_DASH 0x35 //Peak activity during dash
//**** End of 40Y dash section
#define VAR_ACCEL_100mg 0x36 //Accelerometer data: 20 samples in comma separated string
#define VAR_ACTIVITY 0x37
#define VAR_BATT_LVL 0x38
#define VAR_BREATHE_WN 0x39 //Breathing Wave Noise
#define VAR_BREATHE_CONF 0x3A //Breathing rate confidence
#define VAR_HR_CONF 0x3B //Heart rate confidence
#define VAR_HR_VARIA 0x3C //Heart rate variabilitya
#define VAR_SYS_CONF 0x3D //System Confidence
#define VAR_DEV_TEMP 0x3E //Device internal temp
#define VAR_STATI_DWDL 0x3F //Status Info: Device worn detection level
#define VAR_STATI_BPDF 0x40 //Status Info: Device worn detection flag
#define VAR_STATI_NFTG 0x41 //Status Info: Not fitted to garment
#define VAR_STATI_HRUF 0x42 //Status Info: Heart Rate Unreliable flag
#define VAR_STATI_RRUF 0x43 //Status Info: Resp rate unreliable flag
#define VAR_STATI_STUF 0x44 //Status Info: Skin Temperature unreliable flag
#define VAR_STATI_PUF 0x45 //Status Info: Posture Unreliable flag
#define VAR_STATI_ACUF 0x46 //Status Info: Activity Unreliable flag
#define VAR_STATI_HRVUF 0x47 //Status Info: Heart Rate Variability Unreliable flag
#define VAR_STATI_ECTUF 0x48 //Status Info: Estimated Core temp unreliable flag
#define VAR_LINK_QUAL 0x49
#define VAR_RSSI 0x4A
#define VAR_TX_POW 0x4B
#define VAR_EST_CORE_TEMP 0x4C
#define VAR_AUX_ADC1 0x4D //Auxiliary ADC channel 1
#define VAR_AUX_ADC2 0x4E
#define VAR_AUX_ADC3 0x4F
//---------End of Section----------

//---------Config File definitions--
#define SIZEOF(a) sizeof(a) / sizeof((a)[0])
//
pthread_mutex_t log_mutex;
pthread_attr_t log_attr;

typedef struct{
	unsigned char id;
	void* data;
	unsigned short data_size;
	void* next;	
} CONFIG_ITEM;

//Invalid data from the device will contain the string NA
typedef struct{
	unsigned char data_id;
	char *data;
	int data_size; //Including Null termination
	void *next;
} DATA_ITEM;

int BH3_prepLogging(char*,FILE**);
int BH3_logMessage(char*,int,FILE*);
int BH3_stopLogging(FILE*);
#endif
