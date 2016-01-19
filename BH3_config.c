#include "BH3_config.h"
#define MAX_GET_OPTS 5
#define MAX_SET_OPTS 2
   char *get_requests[]={"GET_RTC_DATE","GET_BOOT_SV","GET_MAC","GET_SRLN","GET_APP_VER"};
   char *set_requests[]={"SET_GEN_PKT","SET_ACCL"};
   int (*get_functions[NUM_OF_STD_PKTS])(char*,int,CONFIG_ITEM*,FILE*);
   int (*set_functions[NUM_OF_STD_PKTS])(char*,int,CONFIG_ITEM*,FILE*);
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

static int OPTLower(char *target,int size){
	int i=0;
	for(;i<size;i++){
		target[i]=tolower(target[i]);
	}
return 0;
}

static int OPTLookup(char **array,const char *target,int size){

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


static int prep_MSG0x08(char *parameter,int parameter_size,CONFIG_ITEM* item,FILE* log_fp){
	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=GET_RTC_DATE;
			item->data=NULL;
			item->data_size=0;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option GET_RTC_DATE"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option GET_RTC_DATE"),log_fp);
	}
return -1;
}

static int prep_MSG0x09(char *parameter,int parameter_size,CONFIG_ITEM *item,FILE* log_fp){
	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=GET_BOOT_VER;
			item->data=NULL;
			item->data_size=0;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option GET_BOOT_SV"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option GET_BOOT_SV"),log_fp);
	}
return -1;
}

static int prep_MSG0x0A(char *parameter,int parameter_size,CONFIG_ITEM *item,FILE* log_fp){
	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=GET_APP_VER;
			item->data=NULL;
			item->data_size=0;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option GET_APP_VER"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option GET_APP_VER"),log_fp);
	}
return -1;
}

static int prep_MSG0x0B(char *parameter,int parameter_size,CONFIG_ITEM *item,FILE* log_fp){
	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=GET_SRLN;
			item->data=NULL;
			item->data_size=0;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option GET_SRLN"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option GET_SRLN"),log_fp);
	}
return -1;
}

static int prep_MSG0x12(char *parameter,int parameter_size,CONFIG_ITEM *item,FILE* log_fp){
	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=GET_UNIT_MAC;
			item->data=NULL;
			item->data_size=0;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option GET_MAC"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option GET_MAC"),log_fp);
	}
return -1;
}

static int prep_MSG0x14(char *parameter,int parameter_size,CONFIG_ITEM* item,FILE* log_fp){

	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=SET_GEN_PKT_RATE;
			item->data=malloc(sizeof(char));
			((char*)item->data)[0]=1;
			item->data_size=1;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option SET_GEN_PKT"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option SET_GEN_PKT"),log_fp);
	}
return -1;
}

static int prep_MSG0x1E(char *parameter,int parameter_size,CONFIG_ITEM* item,FILE* log_fp){

	if(parameter!=NULL){
		OPTLower(parameter,parameter_size);
		if(strcmp(parameter,"true")==0){
			item->id=SET_ACCL_RATE;
			item->data=malloc(sizeof(char));
			((char*)item->data)[0]=1;
			item->data_size=1;
		}
		else{
			char message[100];
			BH3_logMessage(message,sprintf(message,"[Lib config] Unknown value set for option SET_ACCL"),log_fp);
			return -2;
		}
	 	return 0;
	}
	else{
	char message[100];
	BH3_logMessage(message,sprintf(message,"[Lib config] No value set for option SET_ACCL"),log_fp);
	}
return -1;
}
static int OPTCompare(const void* e1, const void* e2) {
   char* s1 = *(char**)e1;
   char* s2 = *(char**)e2;
   return strcmp(s1, s2);
}

int BH3_prepConfigParse(FILE**fp,char* config_file){
		
	*fp=fopen(config_file,"r");
	if(*fp == NULL)
		return -1;
	qsort(get_requests,SIZEOF(get_requests),sizeof(char*),OPTCompare);
   	qsort(set_requests,SIZEOF(set_requests),sizeof(char*),OPTCompare);
	get_functions[OPTLookup(get_requests,"GET_RTC_DATE",MAX_GET_OPTS)]=prep_MSG0x08;
	get_functions[OPTLookup(get_requests,"GET_BOOT_SV",MAX_GET_OPTS)]=prep_MSG0x09;
	get_functions[OPTLookup(get_requests,"GET_APP_VER",MAX_GET_OPTS)]=prep_MSG0x0A;
	get_functions[OPTLookup(get_requests,"GET_SRLN",MAX_GET_OPTS)]=prep_MSG0x0B;
	get_functions[OPTLookup(get_requests,"GET_MAC",MAX_GET_OPTS)]=prep_MSG0x12;
	set_functions[OPTLookup(set_requests,"SET_GEN_PKT",MAX_SET_OPTS)]=prep_MSG0x14;
	set_functions[OPTLookup(set_requests,"SET_ACCL",MAX_SET_OPTS)]=prep_MSG0x1E;
return 0;
}

static int parseOPT(char* line,char** opt,char** data){
        
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
                                                             
int BH3_parseConfigFile(FILE* log_fp,FILE* config_fp,CONFIG_ITEM** ITEM_LL){
	char *option=NULL;
	int data_len;
	char *data=NULL;
	char *line;
	size_t len=0;
	int set_index,get_index;
	int readBytes=0;
	unsigned long line_num=1;
	*ITEM_LL=malloc(sizeof(CONFIG_ITEM));
	CONFIG_ITEM* head=*ITEM_LL;
	readBytes=getline(&line,&len,config_fp);	
	while(readBytes>-1){
                if(parseOPT(line,&option,&data)>-1){
			set_index=OPTLookup(set_requests,option,MAX_SET_OPTS);
			get_index=OPTLookup(get_requests,option,MAX_GET_OPTS);
			if(data!=NULL)
				data_len=strlen(data)+1;
                	if(get_index>-1){
        			if(get_functions[get_index](data,data_len,*ITEM_LL,log_fp)==0){
				(*ITEM_LL)->next=malloc(sizeof(CONFIG_ITEM));
				*ITEM_LL=(CONFIG_ITEM*)(*ITEM_LL)->next;
				(*ITEM_LL)->next=NULL;
				}
			}
                	else if(set_index>-1){
               			 if(set_functions[set_index](data,data_len,*ITEM_LL,log_fp)==0){
				(*ITEM_LL)->next=malloc(sizeof(CONFIG_ITEM));
				*ITEM_LL=(CONFIG_ITEM*)(*ITEM_LL)->next;
				(*ITEM_LL)->next=NULL;
				}
			}
			else{
				char message[100];
				BH3_logMessage(message,sprintf(message,"[Lib config] Invalid or Unknown option at line %lu\n",line_num),log_fp);
			}
			free(option);
                	option=NULL;
                	free(data);
                	data=NULL;
                	len=0;
		}
                readBytes=getline(&line,&len,config_fp);
		line_num++;
	}
	*ITEM_LL=head;
return 0;
}

int BH3_freeConfigItem(CONFIG_ITEM* ci){
	if(ci!=NULL){
		if(ci->data!=NULL)
			free(ci->data);
		free(ci);
	}
return 0;
}

int BH3_endConfigParse(FILE* conf_fp){
	fclose(conf_fp);
return 0;
}


