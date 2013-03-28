#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"
static msg r, t;
static int task_index;
static char *filename;
int task_0() 
{
	my_pkt p;
	int i, res;
	int file_size;
	char *filename_out;
	printf("[RECEIVER] Receiver begins.\n");
	/* Wait for filename */	
	memset(t.payload, 0, sizeof(t.payload));
	if (recv_message(&t) < 0) {
		perror("[RECEIVER] Receive message");
		return -1;
	}
	
	p = *((my_pkt*) t.payload);
	if (p.type != TYPE1) {
		perror("[RECEIVER] Receive message");
		return -1;
	}
	
	/* Extract filename */
	filename =(char *) malloc((t.len-sizeof(int)));
	filename_out =(char *) malloc((t.len-sizeof(int))+strlen("recv_"));
	printf("[RECEIVER] FIlename is %s\n", p.payload);
	memcpy(filename, p.payload, t.len - sizeof(int));
	printf("[RECEIVER] FIlename is %s\n", filename);
	sprintf(filename_out, "recv_%s", filename);//numele fisierului ce va fi creat
	printf("[RECEIVER] Filename: %s\n", filename_out);

	/* Send ACK for filename */	
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE4;
	memcpy(p.payload, ACK_T1, strlen(ACK_T1));//ACK_T1 e un string
  	t.len = strlen(p.payload) + 1 + sizeof(int);//de ce +1 ?
  	memcpy(t.payload, &p, t.len);
  	send_message(&t);
	/* Wait for file_size */
  	memset(t.payload, 0, sizeof(t.payload));
  	if (recv_message(&t) < 0) {
  		perror("Receive message");
  		return -1;
  	}

  	p = *((my_pkt*) t.payload);
  	if (p.type != TYPE2) {
  		perror("[RECEIVER] Receive message");
  		return -1;
  	}

  	printf("[RECEIVER] Message type: %d\n", p.type);
  	memcpy(&file_size, p.payload, sizeof(int));
  	printf("[RECEIVER] file_size: %d\n", file_size);

	/* Send ACK for file_size */
  	memset(t.payload, 0, sizeof(t.payload));
  	memset(p.payload, 0, sizeof(p.payload));

  	p.type = TYPE4;
  	memcpy(p.payload, ACK_T2, strlen(ACK_T2));
  	t.len = strlen(p.payload) + sizeof(int);
  	memcpy(t.payload, &p, t.len);
  	send_message(&t);

  	for (i = 0; i < file_size; i++) {
		/* cleanup msg */
  		memset(&r, 0, sizeof(msg));

		/* wait for message */
  		res = recv_message(&r);
  		if (res < 0) {
  			perror("[RECEIVER] Receive error. Exiting.\n");
  			return -1;
  		}

		/* send dummy ACK */
  		printf("[RECIEVER] Sending ACK %i\n",i);
  		res = send_message(&r);
  		if (res < 0) {
  			perror("[RECEIVER] Send ACK error. Exiting.\n");
  			return -1;
  		}
  	}

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
