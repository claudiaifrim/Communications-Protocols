#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST 		"127.0.0.1"
#define PORT 		10000
#define FRAME_SIZE	1404
#define BITS_NO		8

int main(int argc, char *argv[])
{
	msg t;
	int i, res;
	long bdp, window;
	
	printf("[SENDER] Sender starts.\n");	
	init(HOST, PORT);

	/* miliseconds for delay & megabits for speed */	
	bdp = atol(argv[1]) * 1000;
	printf("[SENDER] BDP = %ld b(bits).\n", bdp);
	printf("[SENDER] Each frame has %d B(bytes).\n", FRAME_SIZE);
	printf("[SENDER] Gonna send %d frames.\n", COUNT);
	
	/* window = number of frames in the 'network', unacked */
	window = bdp / (FRAME_SIZE * BITS_NO); 
	printf("[SENDER] window = %ld frames\n", window);

	/* cleanup msg */
	memset(&t, 0, sizeof(msg));

	/* Fill the link = send window messages */
	for (i = 0; i < window; i++) {
		/* gonna send an empty msg */
		t.len = MSGSIZE;
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	/* From now on, ack clocking, i.e., a new ack will inform 
	   us about the space released in the link */
	for (i = 0; i < COUNT - window; i++) {
		/* wait for ACK */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}

		/* gonna send an empty msg */
		t.len = MSGSIZE;
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	/* so far: COUNT x send
		   (COUNT - window) x ack
	   So we need to wait for another 'window' acks */
	for (i = 0; i < window; i++) {
		/* send msg */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
	}


	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
