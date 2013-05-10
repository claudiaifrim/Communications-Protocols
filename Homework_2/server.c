#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#define BUFLEN 256
#define MAX_CLIENTS	10


typedef struct{
	int fd; // fd asignat de server
	int port;
	char nume[BUFLEN];
	char ip[BUFLEN];
} date_client;

//Global variables
socklen_t clilen;
int sockfd, newsockfd, portno;
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
	int i;
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

		// Send accept to client

		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"Accepted");
		n = send(newsockfd,buffer,sizeof(buffer),0);


		client.fd = newsockfd;
		//Adaug clientul curent in lista de clienti
		lista_clienti[clienti_curenti] = client;
		clienti_curenti++;
		//adaug noul socket intors de accept() la multimea descriptorilor de citire
		FD_SET(newsockfd, &read_fds);
		// actualizez fdmax daca e nevoie
		if (newsockfd > fdmax) { 
			fdmax = newsockfd;
		}
	}
	return;
}

void remove_client(date_client client)
{
	return;
}

void switch_command(char* buffer)
{
	int i;
	if(strncmp(buffer,"status",strlen("status")) == 0)
	{
		printf("\nLista clienti conectati:\n");
		for(i = 0 ; i < clienti_curenti ; i++)
		{
			printf("%d.Client %s :\n",i,lista_clienti[i].nume);
			printf("\tIp: %s Port: %d\n",lista_clienti[i].ip,
				lista_clienti[i].port );
		}

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
    FD_SET(0, &read_fds);
    fdmax = sockfd;

    while (1) {

    	tmp_fds = read_fds; 

    	if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
    		error("ERROR in select");

    	for(i = 0; i <= f3dmax; i++) {
    		if (FD_ISSET(i, &tmp_fds)) {

    			if (i == 0){
					// citesc de la tastatură o comandă
    				memset(buffer, 0 , BUFLEN);
    				fgets(buffer, BUFLEN-1, stdin);
    				switch_command(buffer);
    			}
    			else if (i == sockfd){
					// a venit ceva pe socketul inactiv(cel cu listen) 
					// o noua conexiune
					// actiunea serverului: accept()
    				accept_client();
    			} 
    			else
    			{	
					// am primit date pe unul din socketii 
					// cu care vorbesc cu clientii
					// actiunea serverului: recv()
    				memset(buffer, 0, BUFLEN);
    				n = recv(i, buffer, sizeof(buffer), 0);
    				if (n <= 0)
    				{
    					printf("ERROR at recieve from client on socket %d.",i);
    					for (j = 0; j < clienti_curenti; j++){
    						if (lista_clienti[j].fd == i)
								//printeaza ca a iesit clientul
    							printf("Clientul %s pe socket  \
    								%d va fi scos\n", lista_clienti[j].nume, i);
    					}
    					for (j = 0; j < clienti_curenti; j++)
    					{
							//caut clientul si il scot
    						if (lista_clienti[j].fd == i)
    							printf("Shit\n");
								//remove_client(lista_clienti[j]);
    					}
    					close(i);
    					// scoatem din multimea de citire socketul
    					//FD_CLR(i, &read_fds); 
    				}

    				else
					{ //nu au fost erori, am primit ceva
						fprintf (stderr, "Am primit de la clientul de pe socketul %d, mesajul: %s\n", i, buffer);
						char comanda[BUFLEN];

						// TODO
						// Parsam comanda si aplicam functia corespunzatoare

						//sscanf(buffer,"%s %*s", comanda);
						//cerr << "COMANDA: " << comanda << endl;
						//
						//parse_message(buffer, comanda, i, cli_addr);
					}
				}

			}
		}
	}
	close(sockfd);

	return 0;
}