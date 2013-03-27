#ifndef LIB
#define LIB

#define MSGSIZE		1400
#define COUNT		10000

typedef struct {
  int len;
  char payload[MSGSIZE];
  int seq_no;
} msg;
typedef struct 
{
	int type;
	int seq_no;
	char data[1392];
}cadru;
void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);

#endif

