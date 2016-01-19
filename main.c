#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
//#include "BH3_comm.h"
#include "c_api.h"
#include "app_config.h"
#include <poll.h>
#define DEVICE_PATH 30
#define MAX_DEVS 4


//Struct for KeepAlive Pkt thread arguments
typedef struct {
	int file_d;
	PKT* ka_pkt;
}KA_ARGS;

//Struct for Read from file descriptor thread arguments
typedef struct {
	pthread_t tid;
	PKT* pkt;
	PKT_LIB* lib;
	int readBytes;
} RD_ARG;

//Struct to hold information for each sensor thread
typedef struct {
	PKT_LIB *Library;
	MAIN_VAR *main_args;
	int flags;
}ST_ARGS;

void *sensorThread(void *wt_args);
void *writeKeepAlive(void* ka_args);
void *read_threaded(void *rd_arg);

//Return a serial port profile  file descriptor based on options in FD_OPTS struct
//int prep_spp_fd(void*);

/*****************
Returns a spp fd after performing a specific order of operations related to the device (calls prep_spp_fd)
Also fills 

****************/
//int prep_BH3(char* device_file,char* device_config_file,char* lib_config_file);

int set_interface_attribs (int fd, int speed, int parity)
{
	        struct termios tty;
	        memset (&tty, 0, sizeof tty);
	        if (tcgetattr (fd, &tty) != 0)
	        {
	                fprintf(stderr,"Error %d from tcgetattr", errno);
	                return -1;
	        }

	        cfsetospeed (&tty, speed);
	        cfsetispeed (&tty, speed);
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit char
		// disable IGNBRK for mismatched speed tests; otherwise receive break
		//         // as \000 chars
		tty.c_iflag &= ~IGNBRK;         // disable break processing
		tty.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);                // no signaling chars, no echo,
		                                // no canonical processing
		tty.c_oflag = 0;                // no remapping, no delays
		tty.c_cc[VMIN] = 0;            // read doesn't block
		tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
		tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
		tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
			                        // enable reading
		tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
		tty.c_cflag &=~PARENB;
		tty.c_cflag |= parity;
		tty.c_cflag &= ~CSTOPB;
		tty.c_cflag &= ~CSIZE;
		tty.c_cflag |= CS8;
		//tty.c_cflag &= ~CRTSCTS;
                if (tcsetattr (fd, TCSANOW, &tty) != 0)
		{
                  fprintf(stderr,"error %d from tcsetattr", errno);
		 return -1;
		}
      return 0;
     }
						//
void set_blocking (int fd, int should_block){
		
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0){
	 	fprintf (stderr,"error %d from tggetattr", errno);
		return;
	}
						//
	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 10;            	// 1 second read timeout
						//
	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	         fprintf (stderr,"error %d setting term attributes", errno);
}

int main(int argc,char *arg[]){
	if(argc < 2){
		printf("Usage: BH3 <serial device file>\n");
	return 1;
	}
	
	
	/*int serial_fd = open(arg[1],O_RDWR|O_NOCTTY|O_SYNC);
	if(serial_fd<0){
		fprintf(stderr,"Error %d opening %s: %s",errno,arg[1],strerror(errno));
		return serial_fd;
	}*/
	MAIN_VAR *main_args=NULL;
	if(prepAppConfig()!=0){
		//char message[100];
		//logMessage(message,sprintf(message,"[Main] Application exiting: failed to prepare configuration file"));
		return 1;
	}
	if(parseConfig(&main_args)!=0){
		char message[100];
		logMessage(message,sprintf(message,"[Main] Application exiting: failed to parse config file"));
		return 2;

	}
	closeAppConfig();
	PrepareLib(BH3_LIB);
	PKT_LIB *BH3_lib=NULL;
	ST_ARGS *st_args=NULL;

	while(main_args!=NULL){	
		/*int serial_fd = open(main_args->device_file,O_RDWR|O_NOCTTY|O_SYNC);
		if(serial_fd<0){
			fprintf(stderr,"Error %d opening %s: %s",errno,arg[1],strerror(errno));
			return serial_fd;
		}
		set_interface_attribs(serial_fd,main_args->baud_rate,main_args->parity);
		set_blocking(serial_fd,1);*/
		regLIB(main_args->lib_id,&BH3_lib,main_args->patient_name);
		st_args=malloc(sizeof(ST_ARGS));
		st_args->Library=BH3_lib;
		st_args->main_args=main_args;
		//st_args->sensor_file=malloc(sizeof(char)*(strlen(arg[0])+1));
		//memcpy(st_args->sensor_file,arg[1],strlen(arg[1])+1);
		//st_args->sensor_fd=serial_fd;
		pthread_t device;
		pthread_create(&device,NULL,sensorThread,st_args);
		main_args=main_args->next;
	}
		while(1){
		sleep(5);
		}
return 0;
}

void* writeKeepAlive(void* ka_struct){

	while(1){
		write(((KA_ARGS*)ka_struct)->file_d,((KA_ARGS*)ka_struct)->ka_pkt->packet,((KA_ARGS*)ka_struct)->ka_pkt->pkt_size);
		sleep(2);
	}
	

}

void* read_threaded(void *rd){
		RD_ARG *rd_arg=(RD_ARG*)rd;
		rd_arg->lib->readPKT(rd_arg->pkt,rd_arg->readBytes,NULL,rd_arg->lib->log_id);
		deletePKT(rd_arg->pkt);
		rd_arg->lib=NULL;
		free(rd_arg->pkt);
		free(rd_arg);
		
	pthread_exit(NULL);	
return 0;
}


void *sensorThread(void *st){

	ST_ARGS *st_args=(ST_ARGS*)st;
	PKT_LIB *BH3_lib=st_args->Library;
	int readBytes=-1;
	PKT *BH3_pkt=malloc(sizeof(PKT));
	initPKT(BH3_pkt,BH3_lib);
	PKT *BH3_SETUP=BH3_lib->writePkts;
	if(BH3_SETUP==NULL){
		char message[100];
		logMessage(message,sprintf(message,"[Main] No options were set for the device \n"));
		free(BH3_pkt);
		free(st_args);
		pthread_exit(NULL);
	}
	struct pollfd serial_poll[1];
	int read_timeout_msec=4000;
	serial_poll[0].fd = open(st_args->main_args->device_file,O_RDWR|O_NOCTTY|O_SYNC);
	serial_poll[0].events=POLLIN;
	if(serial_poll[0].fd<0){
		fprintf(stderr,"Error %d opening %s: %s",errno,st_args->main_args->device_file,strerror(errno));
		return serial_poll[0].fd;
	}
	set_interface_attribs(serial_poll[0].fd,st_args->main_args->baud_rate,st_args->main_args->parity);
	set_blocking(serial_poll[0].fd,1);
	PKT *inPKT=malloc(sizeof(PKT));
	initPKT(inPKT,BH3_lib);
		while(BH3_SETUP->next !=NULL){
				write(serial_poll[0].fd,BH3_SETUP->packet,BH3_SETUP->pkt_size);
				BH3_SETUP=(PKT*)BH3_SETUP->next;
				fsync(serial_poll[0].fd);
				readBytes=read(serial_poll[0].fd,inPKT->packet,BH3_lib->max_pkt_size);
				BH3_lib->readPKT(inPKT,readBytes,NULL,BH3_lib->log_id);

		}
		BH3_SETUP=BH3_lib->writePkts;
	PKT *BH3_KA=malloc(sizeof(PKT));
	initPKT(BH3_KA,BH3_lib);
	BH3_KA->ID=LIFESIGN;
	BH3_KA->data_size=0;
	BH3_KA->pkt_size=BH3_lib->makePKT(BH3_KA,0,0,BH3_lib->log_id);
	KA_ARGS *ka_struct=malloc(sizeof(KA_ARGS));
	ka_struct->file_d=serial_poll[0].fd;
	ka_struct->ka_pkt=BH3_KA;
	int rc;
	pthread_t threadid;
	rc=pthread_create(&threadid,NULL,writeKeepAlive,ka_struct);
	if(rc)
		fprintf(stderr,"Error creating thread, error code: %d",rc);

	RD_ARG* rd_arg= malloc(sizeof(RD_ARG));
	rd_arg->pkt=malloc(sizeof(PKT));
	initPKT(rd_arg->pkt,BH3_lib);
	rd_arg->lib=BH3_lib;


	
	pthread_t read_threadid;
	pthread_attr_t read_thread_attr;
	pthread_attr_init(&read_thread_attr);
	pthread_attr_setdetachstate(&read_thread_attr,PTHREAD_CREATE_DETACHED);
		while(1){
			if(poll(serial_poll,1,read_timeout_msec)&&POLLIN){
			readBytes=read(serial_poll[0].fd,rd_arg->pkt->packet,BH3_lib->max_pkt_size);
				if(readBytes>0){
					rd_arg->readBytes=readBytes;
					pthread_create(&read_threadid,&read_thread_attr,read_threaded,rd_arg);
					rd_arg=malloc(sizeof(RD_ARG));
					rd_arg->pkt=malloc(sizeof(PKT));
					initPKT(rd_arg->pkt,BH3_lib);
					rd_arg->lib=BH3_lib;
				}
			}
			else{
				char *message=malloc(sizeof(char)*100);
				BH3_lib->logMessage(message,sprintf(message,"[Main] Connection lost: Sensor is out of range or switched off"),BH3_lib->log_id);
				free(message);
				printf("Connection lost...%d\n",st_args->main_args->dcon_wait);
				close(serial_poll[0].fd);
				serial_poll[0].fd=-1;
				while(serial_poll[0].fd<0){
					serial_poll[0].fd = open(st_args->main_args->device_file,O_RDWR|O_NOCTTY|O_SYNC);
					sleep(st_args->main_args->dcon_wait);
				}
				BH3_lib->logMessage(message,sprintf(message,"[Main] Connection re-established"),BH3_lib->log_id);
				set_interface_attribs(serial_poll[0].fd,st_args->main_args->baud_rate,st_args->main_args->parity);
				set_blocking(serial_poll[0].fd,1);
				rd_arg=malloc(sizeof(RD_ARG));
				rd_arg->pkt=malloc(sizeof(PKT));
				initPKT(rd_arg->pkt,BH3_lib);
				rd_arg->lib=BH3_lib;
				while(BH3_SETUP->next!=NULL){
					write(serial_poll[0].fd,BH3_SETUP->packet,BH3_SETUP->pkt_size);
					BH3_SETUP=(PKT*)BH3_SETUP->next;
					fsync(serial_poll[0].fd);
					readBytes=read(serial_poll[0].fd,inPKT->packet,BH3_lib->max_pkt_size);
					BH3_lib->readPKT(inPKT,readBytes,NULL,BH3_lib->log_id);
				}
				BH3_SETUP=BH3_lib->writePkts;

			}
		}

pthread_exit(NULL);
}
