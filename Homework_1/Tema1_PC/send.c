#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"
#define FRAME_SIZE  1404
#define BITS_NO     8

static msg t;
static char *filename;
static int task_index, speed, delay;
int task_0()
{
	int i,res,f,count;
	my_pkt p;
	long bdp,window;
	int file_size;
	struct stat f_status;
	char buffer[MSGSIZE];
	/* miliseconds for delay & megabits for speed */    
	bdp = speed * delay * 1000;
	printf("[SENDER] BDP = %ld b(bits).\n", bdp);

	/* window = number of frames in the 'network', unacked */
	window = bdp / (FRAME_SIZE * BITS_NO); 
	printf("[SENDER] window = %ld frames\n", window);

	

	/*opening file and getting size*/
	f = open(filename, O_RDONLY);
	fstat(f,&f_status);
	file_size = (long) f_status.st_size;
	printf("[SENDER] File size: %d\n", file_size);

	/* Gonna send filename - TYPE 1 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE1; /*TYPE1 trimite numele fisier*/
	/*pune in my_pkt.payload numele fisierului de trimis*/
	printf("[SENDER]Filename is %s\n",filename);
	memcpy(p.payload, filename, strlen(filename)); 
	/*t.len==lungimea pachetului p (my_pkt)
	int (type) + strlen(nume) (numele fisierului)*/
	t.len = sizeof(int) + strlen(filename);
	
	memcpy(t.payload, &p, t.len);         
	send_message(&t);
	printf("[SENDER] Filename sent.\n");

	/* Wait for filename ACK */ 
	if (recv_message(&t) < 0) {
		perror("[SENDER] Receive error");
		return -1;
	}

	/*ia valoarea din t.payload si face cast*/
	p = *((my_pkt *) t.payload);
	if (p.type != TYPE4) {
		printf("Tipul este%d\n",p.type );
		perror("[SENDER] Receive error.Wrong TYPE");
		return -1;
	}

	printf("[SENDER] Got reply with payload: %s\n", p.payload);

	/* Gonna send file_size - TYPE 2 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE2;
	memcpy(p.payload, &file_size, sizeof(int)); //adauga lungime fisier
	t.len = sizeof(int) * 2;    //2 inturi :type+file_size
	memcpy(t.payload, &p, t.len);
	send_message(&t);
	printf("[SENDER] file_size sent %d.\n",file_size);
	
	/* Wait for file_size ACK */    
	if (recv_message(&t) < 0) {
		perror("[SENDER] Receive error");
		return -1;
	}
	
	p = *((my_pkt *) t.payload);
	if (p.type != TYPE4) {
		perror("[SENDER] Receive error");
		return -1;
	}
	printf("[SENDER] Got reply with payload: %s\n", p.payload);


	/* cleanup msg */
	memset(&t, 0, sizeof(msg));

	/* Fill the link = send window messages */
	for (i = 0; i < window && (count = read(f, buffer, MSGSIZE - sizeof(int)))>0; i++) {
		/*face MSGSIZE - sizeof(int) ca sa poata adauga si 
		intul de la type la payload*/
		memset(t.payload, 0, sizeof(t.payload));
		memset(p.payload, 0, sizeof(p.payload));
		p.type = TYPE3;
		memcpy(p.payload, buffer, count);
		t.len = sizeof(int) + count; //type + nr bytes cititi
		memcpy(t.payload, &p, t.len);
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	/* From now on, ack clocking, i.e., a new ack will inform 
	   us about the space released in the link */
	for (i = 0; i < (file_size - window) && (count = read(f, buffer, MSGSIZE - sizeof(int)))>0; i++) {
		/* wait for ACK */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
		/*recieve ack*/
		p = *((my_pkt *) t.payload);
		if (p.type != TYPE4) { //adica ack
			perror("[SENDER] Receive error");
			return -1;
		}

		printf("[SENDER]Sedining %d \n",i); 
		/*face MSGSIZE - sizeof(int) ca sa poata adauga si 
		intul de la type la payload*/
		memset(t.payload, 0, sizeof(t.payload));
		memset(p.payload, 0, sizeof(p.payload));

		p.type = TYPE3;
		memcpy(p.payload, buffer, count);
		t.len = sizeof(int) + count; //type + nr bytes cititi
		memcpy(t.payload, &p, t.len);
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	/* so far: file_size x send
	(COUNT - window) x ack
	So we need to wait for another 'window' acks */
	for (i = 0; i < window && 
		(count = read(f, buffer, MSGSIZE - sizeof(int)))>0; i++) {
	/* send msg */
		p = *((my_pkt *) t.payload);
		if (p.type != TYPE4) { //adica ack
			perror("[SENDER] Receive error");
			return -1;
		}
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
	}
	close(f);
	printf("[SENDER] Job done.\n");

	return 0;
}
int task_1()
{
	int seq_nr=0;
	int i,res,f,count;
	my_pkt_t1 p;
	long bdp,window;
	int file_size;
	struct stat f_status;
	char buffer[MSGSIZE];
	/*----------------this code duplicates------*/
	/* miliseconds for delay & megabits for speed */    
	bdp = speed * delay * 1000;
	printf("[SENDER] BDP = %ld b(bits).\n", bdp);

	/* window = number of frames in the 'network', unacked */
	window = bdp / (FRAME_SIZE * BITS_NO); 
	printf("[SENDER] window = %ld frames\n", window);

	

	/*opening file and getting size*/
	f = open(filename, O_RDONLY);
	fstat(f,&f_status);
	file_size = (long) f_status.st_size;
	printf("[SENDER] File size: %d\n", file_size);

	/* Gonna send filename - TYPE 1 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE1; /*TYPE1 trimite numele fisier*/
	/*pune in my_pkt.payload numele fisierului de trimis*/
	printf("[SENDER]Filename is %s\n",filename);
	memcpy(p.payload, filename, strlen(filename));
	/*----------------this code duplicates-end------*/ 
	t.len = 2 * sizeof(int) + strlen(filename);
	p.seq_number = seq_nr;  //int (type) + strlen(nume) (numele fisierului)
	memcpy(t.payload, &p, sizeof(my_pkt_t1));//pune in t.payload pe p

	do {
		send_message(&t);
		printf("[SENDER] Filename sent.\n");

		/* Wait for filename ACK */ 
		res = recv_message_timeout(&t, 1000);
		p = *((my_pkt_t1 *) t.payload);
		printf("-- %d %d\n", p.seq_number, seq_nr);
		if (p.seq_number!=seq_nr)
			res = -1;
	} while (res < 0);

	seq_nr++;


	/* Gonna send filesize - TYPE 2 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE2;
	p.seq_number = seq_nr;
	memcpy(p.payload, &file_size, sizeof(int)); //adauga lungime fisier
	t.len = sizeof(int) * 3;    
	memcpy(t.payload, &p, sizeof(my_pkt_t1));
	
	do {
		send_message(&t);
		printf("[SENDER] Filesize sent.\n");

		/* Wait for filename ACK */ 
		res = recv_message_timeout(&t, 1000);
		p = *((my_pkt_t1 *) t.payload);
		if (p.seq_number!=seq_nr)
			res = -1;
	} while (res < 0);

	seq_nr++;

	//close(f);
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
