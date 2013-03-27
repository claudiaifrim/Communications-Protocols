#ifndef LIB
#define LIB

typedef struct {
  int len;
  char payload[1400];
} msg;

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

