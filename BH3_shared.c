#include "BH3_shared.h"
//int log_fd;
static void BH3log_getTimestamp(char *timestamp){
        time_t ltime; /* calendar time */
        ltime=time(NULL); /* get current cal time */
        int size=sprintf(timestamp,"%s",asctime( localtime(&ltime) ) );
        timestamp[size-1]=0;
}

int BH3_prepLogging(char *file_path,FILE **fp){
	
	*fp=fopen(file_path,"a");
	if(fp==NULL)
		return -1;
	/*int log_fd;
	if(access(file_path,F_OK)!=0){
            log_fd=open(file_path,O_CREAT|O_APPEND|O_RDWR,S_IRWXU|S_IRGRP);}
        else{
            log_fd=open(file_path,O_APPEND|O_RDWR,0);}*/

	//if(log_fd>0){
	pthread_mutex_init(&log_mutex,NULL);
        //--pthread_attr_init(&log_attr);
        //--pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);	
	//	return log_fd;}
//return -1;
return 0;
}

int BH3_logMessage(char *message,int size,FILE *fp){
	
	if(fp==NULL)
		return -1;
	pthread_mutex_lock(&log_mutex);
	char timestamp[30];
	//char final_message[30+size];
	int writeBytes;
	BH3log_getTimestamp(timestamp);
	//writeBytes=sprintf(final_message,"[%s] %s\n",timestamp,message);
	//write(log_fd,final_message,writeBytes);
	writeBytes=fprintf(fp,"[%s] %s\n",timestamp,message);
	fflush(fp);
	pthread_mutex_unlock(&log_mutex);
return writeBytes;
}

int BH3_stopLogging(FILE* fp){

	fclose(fp);
	pthread_mutex_destroy(&log_mutex);

return 0;
}
