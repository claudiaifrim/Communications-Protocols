#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

int main(int argc, char *argv[])
{
	msg r, t;
	int task_index;

	task_index = atoi(argv[1]);	
	printf("[RECEIVER] Receiver starts.\n");
	printf("[RECEIVER] Task index=%d\n", task_index);
		
	init(HOST, PORT2);

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
