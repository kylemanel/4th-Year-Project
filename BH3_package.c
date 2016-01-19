
/*typeof struct DATA_ITEM{
	unsigned char maj_id; //id of packet type (i.e. General, Summary)
	unsigned char min_id; //sequence number, id of packet data type (i.e. heartrate, battery, temperature)
	int data_size;
	char *data;
	void *next;
} BH3_DATA;*/

/* create an array of all the post variable name strings, their index matches up with their data id.*/
/* postvarnames was created with the shell one-liner:
 * awk '/VAR_/{gsub(/VAR_/,"");print "\""$2"\", "}' BH3_shared.h|tr 'A-Z' 'a-z'|awk -vORS="" '1'
 * If the definitions are changed then the script should be rerun
 * */
const static char postvarnames[80][14] = {"null","timestamp", "hrt_rate", "resp_rate", "skin_temp", "posture", "vmu", "peak_accel", "batt_vol", "breathe_wa", "ecg_amp", "ecg_noise", "vert_accel_min", "vert_accel_peak", "lat_accel_min", "lat_accel_peak", "sag_accel_min", "sag_accel_peak", "zephyr_sys_chnl", "gsr", "rog_status", "rog_time", "alarm", "batt_stat", "pmws", "uibs", "bhhsl", "bhesc", "breathe_wf", "ecg_wf", "rtr", "accel_data", "dev_manufac_code", "dev_type_code", "bluetooth_mac", "status", "bdms_valid", "bdms_unreliable", "bdms_unable_connect", "bdms_auth_err", "bdms_comm_err", "bdms_fail_measure", "hrr_peak", "hrr_30_ex", "hrr_60_ex", "hrr_120_ex", "hrr_180_ex", "oh_avg_60", "oh_inst_hr", "oh_peak_hr", "oh_avg_hr", "vj_airb", "vj_peak_accel", "40y_dash", "accel_100mg", "activity", "batt_lvl", "breathe_wn", "breathe_conf", "hr_conf", "hr_varia", "sys_conf", "dev_temp", "stati_dwdl", "stati_bpdf", "stati_nftg", "stati_hruf", "stati_rruf", "stati_stuf", "stati_puf", "stati_acuf", "stati_hrvuf", "stati_ectuf", "link_qual", "rssi", "tx_pow", "est_core_temp", "aux_adc1", "aux_adc2", "aux_adc3"};

/* serialize converts our linked list representation of the packet into post variable encoding.
 * Takes a pointer to a linked list containing the data,
 * takes a pointer to the (empty) post string?
 * takes a pointer to the logfile file descriptor
 * */
int serialize(char *post, DATA_ITEM *list, File *log)
{
 int check;
 char buf[2000];
 do{
  check = snprintf(buf + strlen(buf), 1999-strlen(buf), "%s%s=%s&", buf, postvarnames[list.dataid], list.data);
  *list = list.next;
 }while(list != NULL && check > 0);
 if(check < 0){
  BH3_logMessage("Output error from snprintf in serialize",120,log)
  return -1;
 }else{
  post = (char *)malloc(strlen(buf)+1);
  strcpy(post, buf);
  return 0; //no errors detected, post string creation successful
 }
} 

/* Todo:
 * 	fix this up so it actually works (I haven't tried it but I think something is missing)
 * 	add detection of undefined post variables (at the moment, anything greater than 79 is invalid
 * 	Post var index mapping starts at 0x1, index starts at 0. Problem?
 * */
