#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFLEN 256
#define MAX_CLIENTS	10

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char const *argv[])
{
	int sockfd, newsockfd, portno, clilen;
	char buffer[BUFLEN],buffer_send[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, j;

     fd_set read_fds;	// multimea de citire folosita in select()
     fd_set tmp_fds;	// multime folosita temporar
     int fdmax;			//valoare maxima file descriptor din multimea read_fds

     if (argc < 2) {
     	fprintf(stderr,"Usage : %s port\n", argv[0]);
     	exit(1);
     }

     //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds
     FD_ZERO(&read_fds);
     FD_ZERO(&tmp_fds);

     //init socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) error("ERROR opening socket");

     //set port
     portno = atoi(argv[1]);

     //init serv_addr
     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     serv_addr.sin_port = htons(portno);
     
     //bind socket
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
     	error("ERROR on binding");
     
     //set it to listen
     listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
     FD_SET(sockfd, &read_fds);
     fdmax = sockfd;

     

     return 0;
 }