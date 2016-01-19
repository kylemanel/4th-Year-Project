#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "BH3_comm.h"


int main(){
	struct sockaddr_rc addr = {0};
	int s, status;
	char dest[18]="C8:3E:99:0D:6B:EB";
	
	s = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);

	addr.rc_family=AF_BLUETOOTH;
	addr.rc_channel=1;
	str2ba(dest,&addr.rc_bdaddr);

	status = connect(s,(struct sockaddr*)&addr, sizeof(addr));
	int writeBytes,readBytes=-1;
	unsigned char buffer[133];
	unsigned char enable[1]={1};
	int pkt_bytes=0;
		writeBytes=makePKT(GEN_DATA,enable,1,buffer);
		for(;pkt_bytes<writeBytes;pkt_bytes++)
			printf("%d",buffer[pkt_bytes]);
		printf("\n");
	while(1){
		send(s,&buffer[0],writeBytes,0);
		readBytes=recv(s,buffer,133,0);
		readPKT(buffer,readBytes);	
	}
	close(s);
	return 0;
}
