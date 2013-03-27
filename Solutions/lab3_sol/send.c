#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST		"127.0.0.1"
#define PORT 		10000

/* XXX: parity functions are implemented in ./link_emulator/lib.c */

int main(int argc, char *argv[])
{
	msg t, r;
	my_pkt pkt;
	int i;
		
	init(HOST, PORT);

	printf("[SENDER] Parity MSG: %d\n", calc_pkt_parity(MSG, strlen(MSG)));
		
	pkt.parity = calc_pkt_parity(MSG, strlen(MSG));
	memcpy(pkt.payload, MSG, strlen(MSG));
	
	t.len = sizeof(int) + strlen(MSG);
	memcpy(t.payload, &pkt, t.len);
	
	for (i = 0; i < PKTS_NO; i++) {
		send_message(&t);

		/* Wait for ACK */
		recv_message(&r);
	}

	printf("[SENDER] Done sending.\n");
		
	return 0;
}
