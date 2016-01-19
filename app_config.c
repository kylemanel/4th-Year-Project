#include "app_config.h"
//Use strtol() from stdlib.h instead of atoi
#define SIZEOF(a) sizeof(a) / sizeof((a)[0])
#define MAX_INT_OPTS 3
#define MAX_BOOL_OPTS 2
#define MAX_STRING_OPTS 2
static pthread_mutex_t app_log_mutex;

static FILE* app_config_log;
static FILE* config_fp;
static char **string_opts;//[]={O_PAT_NAME,O_DEV_FILE,O_LIB_CONF};
static char **bool_opts;//[]={O_BLKING_READ};
static char **int_opts;//[]={O_BAUD_RATE,O_DCON_WAIT};

static int (*string_funcs [MAX_STRING_OPTS])(MAIN_VAR*,char*);
static int (*bool_funcs [MAX_BOOL_OPTS])(MAIN_VAR*,char*);
static int (*int_funcs [MAX_INT_OPTS])(MAIN_VAR*,char*);

char** opt_index_map;

static int setPatientName(MAIN_VAR* main_var,char *data){
	if(strlen(data)>81 || strlen(data)<0){
		char message[100];
		logMessage(message,sprintf(message,"[Main config] %s was longer than 80 characters",O_PAT_NAME));
	return -1;
	}
	main_var->patient_name=malloc(strlen(data)+1);
	memcpy(main_var->patient_name,data,strlen(data)+1);
return 0;
}

static int setDevFileName(MAIN_VAR* main_var,char* data){
	if(strlen(data)>151 || strlen(data)<0){
		char message[150];
		logMessage(message,sprintf(message,"[Main config] %s was longer than 150 characters",O_DEV_FILE));
	return -1;
	}
	main_var->device_file=malloc(strlen(data)+1);
	memcpy(main_var->device_file,data,strlen(data)+1);
return 0;
}

static int setBlocking(MAIN_VAR* main_var,char *data){
	if(strcmp(data,"1")!=0 && strcmp(data,"0")!=0){
		char message[150];
		logMessage(message,sprintf(message,"[Main config] Invalid value for boolean %s",O_BLKING_READ));
		return -1;
	}
	main_var->set_blocking=strtol(data,NULL,10);
return 0;
}

static int setParity(MAIN_VAR* main_var,char* data){
	if(strcmp(data,"1")!=0 && strcmp(data,"0")!=0){
		char message[150];
		logMessage(message,sprintf(message,"[Main config] Invalid value for boolean %s",O_PARITY));
		return -1;
	}
	main_var->parity=strtol(data,NULL,10);
return 0;
}

static int setBaudRate(MAIN_VAR* main_var,char *data){

	int val=strtol(data,NULL,10);
        int begin=0;
	//end=size-1
        int end=11;
        int middle;
	int baud_rates[]={300,1200,2400,4800,9600,14400,19200,28800,38400,57600,115200,230400};
        while(end>=begin){

                middle = begin+((end-begin)/2);
                if(baud_rates[middle] == val){
			main_var->baud_rate=middle;
                        return 0;
		}
                else if(middle<val)
                        begin = middle + 1;
                else
                        end = middle - 1;

        }

	char message[100];
	logMessage(message,sprintf(message,"[Main config] %s was set to an invalid value",O_BAUD_RATE));

	
return -1;
}


static int setDconWait(MAIN_VAR* main_var,char* data){

	int val=strtol(data,NULL,10);
	if(val>10 || val <0){
		char message[100];
		logMessage(message,sprintf(message,"[Main config] %s was set to an invalid value",O_DCON_WAIT));
		return -1;	
	}
	main_var->dcon_wait=val;

return 0;
}

static int setLibID(MAIN_VAR* main_var,char* data){

int val=strtol(data,NULL,10);
	if(val>3 || val <0){
		char message[100];
		logMessage(message,sprintf(message,"[Main config] %s was set to an invalid value",O_LIBID));
		return -1;	
	}
	main_var->lib_id=val;

return 0;
}

static size_t trim(char *out,size_t len, const char *str)
{
  if(len == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
  {
    *out = 0;
    return 0;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < len-1 ? (end - str) : len-1;

  // Copy trimmed string and add null terminator
  	memcpy(out, str, out_size);
  	out[out_size] = 0;
  return out_size;
}

/*static int optLower(char *target,int size){
	int i=0;
	for(;i<size;i++){
		target[i]=tolower(target[i]);
	}
return 0;
}*/

static int parseLine(char* line,char** opt,char** data){
        
	char del_1[2]=" ";
        char del_2[2]="\"";
        char *token;
        char temp[160];
        size_t trim_size;
        if(trim(temp,(size_t)160,line)<160){
        	if(temp[0]!=0){
			token=strtok(temp,del_1);
        		*opt=malloc(sizeof(char)*(strlen(token)+1));
        		memcpy(*opt,token,strlen(token)+1);
        		trim_size=trim(temp,160,line);
        		token=strtok(token,del_2);
        		token=strtok(NULL,del_2);
        		if(token!=NULL){
                		trim_size=trim(temp,(size_t)160,token);
                		*data=malloc(sizeof(char)*(trim_size+1));
                		memcpy(*data,temp,trim_size+1);
        		}
		return 0;
		}
	}
return -2;
}

static int optCompare(const void* e1, const void* e2) {
   char* s1 = *(char**)e1;
   char* s2 = *(char**)e2;
   return strcmp(s1, s2);
}

static int optLookup(char **array,const char *target,int size){

        int begin=0;
        int end=size-1;
        int middle;
        while(end>=begin){

                middle = begin+((end-begin)/2);
                if(strcmp(array[middle],target) == 0)
                        return middle;
                else if(strcmp(array[middle],target)<0)
                        begin = middle + 1;
                else
                        end = middle - 1;

        }
return -1;
}

static void applog_getTimestamp(char *timestamp){
        time_t ltime; /* calendar time */
        ltime=time(NULL); /* get current cal time */
        int size=sprintf(timestamp,"%s",asctime( localtime(&ltime) ) );
        timestamp[size-1]=0;
}
void logMessage(char* message,int size){

	char timestamp[30];	
	pthread_mutex_lock(&app_log_mutex);
	applog_getTimestamp(timestamp);
	fprintf(app_config_log,"[%s] %s\n",timestamp,message);
	fflush(app_config_log);
	pthread_mutex_unlock(&app_log_mutex);

}

int prepAppConfig(){
	
	char message[200];
	char log_file[200];
	sprintf(log_file,"/var/log/telenurse/%s",DEFAULT_LOG);
	app_config_log=fopen(log_file,"a");
	//Log file cannot be opened
	if(app_config_log==NULL)
		return -2;
	pthread_mutex_init(&app_log_mutex,NULL);
	char conf_file[200];
	sprintf(conf_file,"/etc/telenurse/%s",DEFAULT_CONFIG);
	config_fp=fopen(conf_file,"r");
	if(config_fp==NULL){
		logMessage(message,sprintf(message,"Could not find config file at path: %s",conf_file));
		//fclose(app_config_log);
			return -2;
		}

	int counter;
	
	//string_opts[]={O_PAT_NAME,O_DEV_FILE,O_LIB_CONF};
	string_opts=malloc(MAX_STRING_OPTS*(sizeof(char*)));
	//bool_opts[]={O_BLKING_READ};
	bool_opts=malloc(MAX_BOOL_OPTS*(sizeof(char*)));
	//int_opts[]={O_BAUD_RATE,O_DCON_WAIT};
	int_opts=malloc(MAX_INT_OPTS*(sizeof(char*)));

	for(counter=0;counter<MAX_STRING_OPTS;counter++)
		string_opts[counter]=malloc(sizeof(char)*15);
	
	for(counter=0;counter<MAX_BOOL_OPTS;counter++)
		bool_opts[counter]=malloc(sizeof(char)*15);

	for(counter=0;counter<MAX_INT_OPTS;counter++)
		int_opts[counter]=malloc(sizeof(char)*15);

//Prepare string opts	
	memcpy(string_opts[0],O_PAT_NAME,strlen(O_PAT_NAME));
	memcpy(string_opts[1],O_DEV_FILE,strlen(O_DEV_FILE));
//Prepare bool opts
	memcpy(bool_opts[0],O_BLKING_READ,strlen(O_BLKING_READ));
	memcpy(bool_opts[1],O_PARITY,strlen(O_PARITY));
//Prepare int opts
	memcpy(int_opts[0],O_BAUD_RATE,strlen(O_BAUD_RATE));
	memcpy(int_opts[1],O_DCON_WAIT,strlen(O_DCON_WAIT));
	memcpy(int_opts[2],O_LIBID,strlen(O_LIBID));	
	qsort(string_opts,MAX_STRING_OPTS,sizeof(char*),optCompare);
	qsort(bool_opts,MAX_BOOL_OPTS,sizeof(char*),optCompare);
	qsort(int_opts,MAX_INT_OPTS,sizeof(char*),optCompare);
	
	string_funcs[optLookup(string_opts,O_PAT_NAME,MAX_STRING_OPTS)]=setPatientName;
	string_funcs[optLookup(string_opts,O_DEV_FILE,MAX_STRING_OPTS)]=setDevFileName;
 	bool_funcs[optLookup(bool_opts,O_BLKING_READ,MAX_BOOL_OPTS)]=setBlocking;
 	bool_funcs[optLookup(bool_opts,O_PARITY,MAX_BOOL_OPTS)]=setParity;
	int_funcs[optLookup(int_opts,O_BAUD_RATE,MAX_INT_OPTS)]=setBaudRate;
 	int_funcs[optLookup(int_opts,O_DCON_WAIT,MAX_INT_OPTS)]=setDconWait;
	int_funcs[optLookup(int_opts,O_LIBID,MAX_INT_OPTS)]=setLibID;
return 0;
}



int parseConfig(MAIN_VAR** main_args){
	char *option=NULL;
	char sec_b,sec_e;
	char *data=NULL;
	char *line;
	size_t len=0;
	int string_index,bool_index,int_index;
	int readBytes=0;
	unsigned long line_num=1;
	//*main_args=malloc(sizeof(MAIN_VAR));
	MAIN_VAR* head=*main_args;
	readBytes=getline(&line,&len,config_fp);
	sec_b=sec_e=0;
	while(readBytes>-1){
		if(parseLine(line,&option,&data)>-1){
		
			if(strcmp(option,O_SECTION_B)==0)
				sec_b++;
			else if(strcmp(option,O_SECTION_E)==0)
				sec_e++;

			free(option);
                	option=NULL;
                	free(data);
                	data=NULL;
                	len=0;

		}
		        readBytes=getline(&line,&len,config_fp);
			//line_num++;

	}
	if((sec_b-sec_e)!=0){
		char message[100];
		logMessage(message,sprintf(message,"[Main Config] Conf file has missing section markers: parsing aborted"));
	return -1;	
	}
	rewind(config_fp);
	readBytes=getline(&line,&len,config_fp);
	while(readBytes>-1){
                if(parseLine(line,&option,&data)>-1 && option!=NULL){
			if(option[0]!='#'){
				if(strcmp(option,O_SECTION_B)==0){
					if(*main_args==NULL){
						*main_args=malloc(sizeof(MAIN_VAR));
						head=*main_args;
					}
					else{
					(*main_args)->next=malloc(sizeof(MAIN_VAR));
					*main_args=(*main_args)->next;}
				}
				string_index=optLookup(string_opts,option,MAX_STRING_OPTS);
				bool_index=optLookup(bool_opts,option,MAX_BOOL_OPTS);
				int_index=optLookup(int_opts,option,MAX_INT_OPTS);
                	
					if(string_index>-1){
						string_funcs[string_index](*main_args,data);
					}
                			else if(bool_index>-1){
						bool_funcs[bool_index](*main_args,data);
					}
					else if(int_index>-1){
						int_funcs[int_index](*main_args,data);
					}
					else {
						if(strcmp(option,O_SECTION_B)!=0 && strcmp(option,O_SECTION_E)!=0){
						char message[100];
						logMessage(message,sprintf(message,"[Main config] Invalid or Unknown option at line %lu: ignoring line\n",line_num));}
					}
				free(option);
                		option=NULL;
                		free(data);
                		data=NULL;
                		len=0;
			}
		}
                readBytes=getline(&line,&len,config_fp);
		line_num++;
	}
	*main_args=head;
	while((*main_args)!=NULL){
	printf("[Main arg values] pat_name:%s dev_file:%s blking:%d baud_r:%d parity :%d dcon_wait:%d \n",(*main_args)->patient_name,(*main_args)->device_file,(*main_args)->set_blocking,(*main_args)->baud_rate,(*main_args)->parity,(*main_args)->dcon_wait);
	*main_args=(*main_args)->next;
	}
	*main_args=head;
	return 0;
}

int closeAppConfig(){

int rc=0;
	int counter;
	for(counter=0;counter<MAX_STRING_OPTS;counter++)
		free(string_opts[counter]);
	for(counter=0;counter<MAX_BOOL_OPTS;counter++)
		free(bool_opts[counter]);
	for(counter=0;counter<MAX_INT_OPTS;counter++)
		free(int_opts[counter]);
	free(string_opts);
	free(bool_opts);
	free(int_opts);
	rc+=fclose(config_fp);
	rc+=fclose(app_config_log);
	pthread_mutex_destroy(&app_log_mutex);
return rc;
}
