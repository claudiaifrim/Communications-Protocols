#include <arpa/inet.h>
#include <poll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "lib.h"

#define MSG 		"ana"
#define MASK		1 << 7
#define	BITS_NO		8
#define PKTS_NO		20

struct sockaddr_in addr_local, addr_remote;
int s;
struct pollfd fds[1];

/* print_bits(c)
   Prints all 8 bits in character c.
*/
void print_bits(char c)
{
	unsigned char mask = MASK;
	int i;

	for (i = 0; i < BITS_NO; i++) {
		printf("%d \n", (c & mask) != 0);
		mask >>= 1;
	}
	printf("\n");
}

/* calc_byte_parity(c)
   Computes parity of byte c.
*/
int calc_byte_parity(char c)
{
	unsigned char mask = MASK;
	int i, result = (c & mask) != 0;

	mask >>= 1;
	for (i = 0; i < BITS_NO - 1; i++) {
		result ^= (c & mask) != 0;
		mask >>= 1;
	}
	
	return result;
}

/* calc_pkt_parity(data, len)
   Computes parity of all len bytes found in data.
*/
int calc_pkt_parity(char *data, int len)
{
	int i, result = calc_byte_parity(data[0]);
	
	for (i = 1; i < len; i++) {
		result ^= calc_byte_parity(data[i]);
	}
	
	return result;
}


void set_local_port(int port)
{
	memset((char *)&addr_local, 0, sizeof(addr_local));
	addr_local.sin_family = AF_INET;
	addr_local.sin_port = htons(port);
	addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
}

void set_remote(char *ip, int port)
{
	memset((char *)&addr_remote, 0, sizeof(addr_remote));
	addr_remote.sin_family = AF_INET;
	addr_remote.sin_port = htons(port);
	if (inet_aton(ip, &addr_remote.sin_addr) == 0) {
		perror("inet_aton failed\n");
		exit(1);
	}
}

void init(char *remote, int REMOTE_PORT)
{
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Error creating socket");
		exit(1);
	}

	set_local_port(0);
	set_remote(remote, REMOTE_PORT);

	if (bind(s, (struct sockaddr *)&addr_local, sizeof(addr_local)) == -1) {
		perror("Failed to bind");
		exit(1);
	}

	fds[0].fd = s;
	fds[0].events = POLLIN;

	msg m;
	send_message(&m);
}

int send_message(const msg * m)
{
	return sendto(s, m, sizeof(msg), 0, (struct sockaddr *)&addr_remote,
		      sizeof(addr_remote));
}

int recv_message(msg * ret)
{
	return recvfrom(s, ret, sizeof(msg), 0, NULL, NULL);
}
