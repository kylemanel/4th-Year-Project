#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#define DEFAULT_CONFIG "telenurse.conf"
#define DEFAULT_LOG "telenurse.log"
/*********
This section for App_config options definitions
*********/
#define O_SECTION_B "SECTION_B"
#define O_SECTION_E "SECTION_E"
//String opts
#define O_PAT_NAME "PAT_NAME"
#define O_DEV_FILE "DEV_FILE"
//Bool opts
#define O_BLKING_READ "BLKING_READ"
#define O_PARITY "PARITY"
//Integer opts
#define O_BAUD_RATE "BAUD_RATE"
#define O_DCON_WAIT "DCON_WAIT" //Wait time before attempting to reconnect to device
#define O_DCON_WAIT_MAX 10
#define O_DCON_WAIT_MIN 1
#define O_LIBID "LIBID"
/**End of option definitions**/


//These definitions should only be used to identify the option items from the APP_CONFIG_ITEM struct in the main program
#define DEVICE_NAME 0x01
#define LIB_CONFIG_FILE 0x02
#define BAUD_RATE 0x03
#define PARITY 0x04
#define BLOCKING_READ 0x05
typedef char bool;
typedef struct{
	int item_id;
	char *opt_val;
}APP_CONFIG_ITEM;


typedef struct {
	char *patient_name;
	char *device_file;
	bool set_blocking;
	int baud_rate;
	bool parity;
	int dcon_wait;
	unsigned char lib_id;
	void *next;
} MAIN_VAR;

int prepAppConfig();
int parseConfig(MAIN_VAR**);
int closeAppConfig();
void logMessage(char*,int);
//Only deletes link list items not including the LL pointer
int delMainVarList(MAIN_VAR*);

#endif

