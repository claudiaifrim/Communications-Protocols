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
	cadru t;
	int i, res;
	
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);
	int BDP = atoi(argv[1]);
	int dimF;
	dimF = 2;
	char a='0';
	printf("dimF = %d\n", dimF);
	t.seq_no = 0;
	/* printf("[SENDER]: BDP=%d\n", atoi(argv[1])); */
	
	for (i = 0; i < dimF; i++) {
		/* cleanup msg */
		memset(&t, 0, sizeof(cadru));
		memcpy(&t.data,&a,sizeof(char));
		/* gonna send an empty msg */
		//t.len = MSGSIZE;
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
		
		t.seq_no = i;
	}
	
	for (i = dimF; i < COUNT; i++) {
	
		memset (&t, 0, sizeof(int));
		//t.len = MSGSIZE;
		memcpy(&t,&a,sizeof(char));
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
		
		t.seq_no = i;
		
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
		
		printf("ACK %d\n", i);
		
		
			
		
	}

	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
