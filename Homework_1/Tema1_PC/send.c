#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"
#define FRAME_SIZE	1404
#define BITS_NO		8

static msg t;
static char *filename;
static int task_index, speed, delay;
int task_0()
{
	long bdp,window;
	/* miliseconds for delay & megabits for speed */	
	bdp = speed * delay * 1000;
	printf("[SENDER] BDP = %ld b(bits).\n", bdp);

	/* window = number of frames in the 'network', unacked */
	window = bdp / (FRAME_SIZE * BITS_NO); 
	printf("[SENDER] window = %ld frames\n", window);

	/* cleanup msg */
//	memset(&t, 0, sizeof(msg));


	sprintf(t.payload, "Hello World of PC");
	t.len = strlen(t.payload) + 1;
	send_message(&t);

	if (recv_message(&t) < 0) {
		perror("[SENDER] receive error");
	} else {
		printf("[SENDER] Got reply with payload: %s\n", t.payload);
	}

	printf("[SENDER] Job done.\n");
	
	 return 0;
}
int task_1()
{

	sprintf(t.payload, "Hello World of PC");
	t.len = strlen(t.payload) + 1;
	send_message(&t);

	if (recv_message(&t) < 0) {
		perror("[SENDER] receive error");
	} else {
		printf("[SENDER] Got reply with payload: %s\n", t.payload);
	}

	printf("[SENDER] Job done.\n");
	
	 return 0;
}
int task_2()
{

	sprintf(t.payload, "Hello World of PC");
	t.len = strlen(t.payload) + 1;
	send_message(&t);

	if (recv_message(&t) < 0) {
		perror("[SENDER] receive error");
	} else {
		printf("[SENDER] Got reply with payload: %s\n", t.payload);
	}

	printf("[SENDER] Job done.\n");
	
	 return 0;
}
int task_3()
{

	sprintf(t.payload, "Hello World of PC");
	t.len = strlen(t.payload) + 1;
	send_message(&t);

	if (recv_message(&t) < 0) {
		perror("[SENDER] receive error");
	} else {
		printf("[SENDER] Got reply with payload: %s\n", t.payload);
	}

	printf("[SENDER] Job done.\n");
	
	 return 0;
}
int task_4()
{

	sprintf(t.payload, "Hello World of PC");
	t.len = strlen(t.payload) + 1;
	send_message(&t);

	if (recv_message(&t) < 0) {
		perror("[SENDER] receive error");
	} else {
		printf("[SENDER] Got reply with payload: %s\n", t.payload);
	}

	printf("[SENDER] Job done.\n");
	
	 return 0;
}
int main(int argc, char *argv[])
{
	task_index = atoi(argv[1]);
	filename = argv[2];
	speed = atoi(argv[3]);
	delay = atoi(argv[4]);
	
	printf("[SENDER] Sender starts.\n");
	printf("[SENDER] Filename=%s, task_index=%d, speed=%d, delay=%d\n", filename, task_index, speed, delay);
	
	init(HOST, PORT1);

	switch (task_index){
		case 0 :
		task_0();
		break;
		case 1 :
		task_1();
		break;
		case 2 :
		task_2();
		break;
		case 3 :
		task_3();
		break;
		case 4 :
		task_4();
		break;
		default:
		printf("Bad task\n");
		 return 0;
	}
	return 0;
}
