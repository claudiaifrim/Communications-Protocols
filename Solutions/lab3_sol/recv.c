#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST 		"127.0.0.1"
#define PORT 		10001

/* XXX: parity functions are implemented in ./link_emulator/lib.c */

int main(void)
{
	msg t;
	my_pkt p;
	int i, count = 0;
	
	init(HOST, PORT);

	for (i = 0; i < PKTS_NO; i++) {
		memset(&t, 0, sizeof(msg));
		recv_message(&t);
	
		p = *((my_pkt*) t.payload);
		if (p.parity != calc_pkt_parity(p.payload, t.len - sizeof(int))) {
			count++;
		}			
	
		/* Send dummy ACK */	
  		send_message(&t);
	}
	
	printf("[RECEIVER] Done receiving.\n");
	printf("[RECEIVER] Corrupt pkts: %d\n", count);
	
  	return 0;
}
