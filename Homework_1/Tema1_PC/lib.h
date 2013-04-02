/*
 * Author: Flavius Tirnacop
 * Date: 2 April 2012
 * File: lib.h
 * 
 * Description:Header file with main structs
 */

#ifndef LIB
#define LIB

#define HOST "127.0.0.1"
#define PORT1 10000
#define PORT2 10001

#define TYPE1		1
#define TYPE2		2
#define TYPE3		3
#define TYPE4		4
#define ACK_T1		"ACK(TYPE1)"
#define ACK_T2		"ACK(TYPE2)"
#define ACK_T3		"ACK(TYPE3)"

#define MSGSIZE 1400
#define PKTSIZE	1396
#define PKTSIZE1 1392

typedef struct {
	int len;
	char payload[MSGSIZE];
} msg;

/*Structura folosita pentru task 0*/
typedef struct {
	int type;
	char payload[PKTSIZE];	
} my_pkt;

/*Structura folosita pentru task 1*/
typedef struct {
	int type;
	int seq_nr;	
	char payload[PKTSIZE1];
} my_pkt_t1;

void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);
int recv_message_timeout(msg *m, int timeout);

#endif

