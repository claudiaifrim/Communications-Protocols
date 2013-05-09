#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#define BUFLEN 256
#define MAX_CLIENTS	10


typedef struct{
	int port;
	char nume[BUFLEN];
	char ip[BUFLEN];
} date_client;

//Global variables
int sockfd, newsockfd, portno, clilen;
char buffer[BUFLEN],buffer_send[BUFLEN];
struct sockaddr_in serv_addr, cli_addr;
int n, i, j,clienti_curenti=0,debug=0;
date_client lista_clienti[10]; //nu vor fi mai mult de 10


fd_set read_fds;	// multimea de citire folosita in select()
fd_set tmp_fds;	// multime folosita temporar
int fdmax;			//valoare maxima file descriptor din multimea read_fds


void error(char *msg)
{
	perror(msg);
	exit(1);
}


void accept_client(){

	clilen = sizeof(cli_addr);
	if ((newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr, &clilen)) == -1) {
		error("ERROR in accept");
	} 
	else {
		//fac recv ca sa vad daca mai este sau nu
		date_client client;
		memset(&client, 0,sizeof(client));	
		n = recv(newsockfd,&client,sizeof(client),0);
		if(n < 0){
			printf("ERROR la recieve date de handshake de la client");
			return; //nu oprim serverul
		}

		printf("Client connected: Name - %s Listen_Port: %d , Ip: %s\n",
			client.nume,client.port,client.ip );
		
		for(i = 0 ; i < clienti_curenti ; i++)
		{
			if(strcmp(lista_clienti[i].nume,client.nume)==0)
			{
				memset(buffer,0,sizeof(buffer));
				sprintf(buffer,"Disconnect");
				n = send(newsockfd,buffer,sizeof(buffer),0);
				printf("Access denied:Client already connected\n");
				if(n<0){
					printf("ERROR la trimitere Disconnect");
					return; 
				//nu oprim serverul
				}
				return;
				//nu mai adaug nimic la lista
			}
		}

		//Adaug clientul curent in lista de clienti
		lista_clienti[clienti_curenti] = client;
		clienti_curenti++;
		//adaug noul socket intors de accept() la multimea descriptorilor de citire
		FD_SET(newsockfd, &read_fds);
		// actualizez fdmax daca e nevoie
		if (newsockfd > fdmax) { 
			fdmax = newsockfd;
		}
		printf("Am adaugat nou client GG\n");
	}
	return;
}

int main(int argc, char const *argv[])
{
	
    //Usage error
	if (argc < 2) {
		fprintf(stderr,"Usage : %s port\n", argv[0]);
		exit(-1);
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
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr))<0) 
    	error("ERROR on binding");

     //set it to listen
    listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor (socketul pe care se asculta conexiuni)
     // in multimea read_fds
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

    while (1) {

    	tmp_fds = read_fds; 

    	if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
    		error("ERROR in select");

    	for(i = 0; i <= fdmax; i++) {
    		if (FD_ISSET(i, &tmp_fds)) {
    			if (i == sockfd){
					// a venit ceva pe socketul inactiv(cel cu listen) 
					// o noua conexiune
					// actiunea serverului: accept()
    				accept_client();
    			} 

    			//printf("Noua conexiune de la %s, port %d, socket_client %d\n ", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
    		}
    	}
    }
    close(sockfd);

    return 0;
}