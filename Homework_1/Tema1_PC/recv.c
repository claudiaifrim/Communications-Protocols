#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"
static msg r, t;
static int task_index;
int task_0() 
{

	if (recv_message(&r) < 0) {
		perror("[RECEIVER] receive message");
		return -1;
	}

	printf("[RECEIVER] Got msg with payload: %s\n", r.payload);

	sprintf(t.payload, "ACK(%s)", r.payload);
	t.len = strlen(t.payload + 1);
	send_message(&t);

	printf("[RECEIVER] All done.\n");
	
	return 0;

}
int task_1()
{

	if (recv_message(&r) < 0) {
		perror("[RECEIVER] receive message");
		return -1;
	}

	printf("[RECEIVER] Got msg with payload: %s\n", r.payload);

	sprintf(t.payload, "ACK(%s)", r.payload);
	t.len = strlen(t.payload + 1);
	send_message(&t);

	printf("[RECEIVER] All done.\n");
	
	return 0;

}
int task_2()
{

	if (recv_message(&r) < 0) {
		perror("[RECEIVER] receive message");
		return -1;
	}

	printf("[RECEIVER] Got msg with payload: %s\n", r.payload);

	sprintf(t.payload, "ACK(%s)", r.payload);
	t.len = strlen(t.payload + 1);
	send_message(&t);

	printf("[RECEIVER] All done.\n");
	
	return 0;

}
int task_3()
{

	if (recv_message(&r) < 0) {
		perror("[RECEIVER] receive message");
		return -1;
	}

	printf("[RECEIVER] Got msg with payload: %s\n", r.payload);

	sprintf(t.payload, "ACK(%s)", r.payload);
	t.len = strlen(t.payload + 1);
	send_message(&t);

	printf("[RECEIVER] All done.\n");
	
	return 0;

}
int task_4()
{

	if (recv_message(&r) < 0) {
		perror("[RECEIVER] receive message");
		return -1;
	}

	printf("[RECEIVER] Got msg with payload: %s\n", r.payload);

	sprintf(t.payload, "ACK(%s)", r.payload);
	t.len = strlen(t.payload + 1);
	send_message(&t);

	printf("[RECEIVER] All done.\n");
	
	return 0;

}

int main(int argc, char *argv[])
{
	task_index = atoi(argv[1]);	
	printf("[RECEIVER] Receiver starts.\n");
	printf("[RECEIVER] Task index=%d\n", task_index);

	init(HOST, PORT2);

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
