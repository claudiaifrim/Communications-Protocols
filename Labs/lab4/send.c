#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char *argv[])
{
	msg t;
	int i, res;
	printf("bdp este %d",atoi(argv[1]));
	int BDP = atoi(argv[1]);
	int dimF = BDP / MSGSIZE;
	dimF /= 8;
	dimF=99;
	printf("[SENDER] Starting.\n");	
	printf("dimF este %d \n", dimF);	
	init(HOST, PORT);


for( i=0; i<dimF; i++) {
	
	printf("Trimit mesaj %d fara confirmare \n", i);
	memset(&t, 0, sizeof(msg));
		fflush(stdout);
		
	t.len = MSGSIZE;
	res = send_message(&t);
	if( res < 0 ) {
		perror("[Sender] Send error. Exiting. \n");
		return -1;
		}
		
		/* wait for ACK */
		//res = recv_message(&t);
	if (res < 0) {
		perror("[SENDER] Receive error. Exiting.\n");
		return -1;
		}
	
	}	
	/* printf("[SENDER]: BDP=%d\n", atoi(argv[1])); */
	
	for (i = dimF; i < COUNT; i++) {
		/* cleanup msg */
		memset(&t, 0, sizeof(msg));
		
		/* gonna send an empty msg */
		t.len = MSGSIZE;
		
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
		
	
		
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}


	}

	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
