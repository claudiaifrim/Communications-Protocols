#ifndef LIB
#define LIB

#define MSGSIZE		1400
#define PKTSIZE		1396

#define MSG		"ana"
#define PKTS_NO		20
#define BITS_NO		8
#define MASK		1 << 7

typedef struct {
  	int len;
  	char payload[MSGSIZE];
} msg;

typedef struct {
	int parity;
	char payload[PKTSIZE];	
} my_pkt;

void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);

/* Added for PC, Lab3 */
void print_bits(char);
int calc_byte_parity(char);
int calc_pkt_parity(char*, int);

#endif

