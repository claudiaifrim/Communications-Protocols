#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"	

#define HOST 	"127.0.0.1"
#define PORT 	10000

int main(int argc, char *argv[])
{
	msg t;
	my_pkt p;
	int fd, count, filesize;
	struct stat f_status;
	char buffer[MSGSIZE];
	
	init(HOST, PORT);
	printf("[SENDER] Server rocks.\n");
	printf("[SENDER] File to send: %s\n", argv[1]);

	fd = open(argv[1], O_RDONLY);
	fstat(fd, &f_status);
	
	filesize = (int) f_status.st_size;
	printf("[SENDER] File size: %d\n", filesize);

	/* Len in message 
		= size(msg payload)
		= amount of data in the my_pkt structure
		= size(type) + data in (pkt payload)
		= sizeof(int) + number of used bytes in (pkt payload)
	*/

	/* Gonna send filename - TYPE 1 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE1;	//TYPE1 trimite numele fisier
	memcpy(p.payload, argv[1], strlen(argv[1])); //pune in my_pkt.payload numele fisierului de trimis
	t.len = sizeof(int) + strlen(argv[1]);//t.len==lungimea pachetului p (my_pkt)
										  //int (type) + strlen(nume) (numele fisierului)
	memcpy(t.payload, &p, t.len); 		  //pune in t.payload pe p
	send_message(&t);
	printf("[SENDER] Filename sent.\n");

	/* Wait for filename ACK */	
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
	
	/* Gonna send filesize - TYPE 2 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE2;
	memcpy(p.payload, &filesize, sizeof(int)); //adauga lungime fisier
	t.len = sizeof(int) * 2;	//2 inturi :type+filesize
	memcpy(t.payload, &p, t.len);
	send_message(&t);
	printf("[SENDER] Filesize sent.\n");
	
	/* Wait for filesize ACK */	
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

	/* Send file contents - TYPE 3 messages */
	printf("[SERVER] File transfer begins.\n");
	while((count = read(fd, buffer, MSGSIZE - sizeof(int))) > 0) {
		//face MSGSIZE - sizeof(int) ca sa poata adauga si intul de la type la payload
		memset(t.payload, 0, sizeof(t.payload));
		memset(p.payload, 0, sizeof(p.payload));
		
		p.type = TYPE3;
		memcpy(p.payload, buffer, count);
		t.len = sizeof(int) + count; //type + nr bytes cititi
		memcpy(t.payload, &p, t.len);
		send_message(&t);

		if (recv_message(&t) < 0) {
			perror("[SENDER] Receive error");
			return -1;
		}
		
		p = *((my_pkt *) t.payload);
		if (p.type != TYPE4) {
			perror("[SENDER] Receive error");
			return -1;
		}
	}
	
	printf("[SERVER] File transfer has ended.\n");	
	close(fd);
	
	return 0;
}
