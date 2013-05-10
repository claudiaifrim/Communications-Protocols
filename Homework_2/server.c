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
	time_t timer;
} date_client;

//Global variables
socklen_t clilen;
int sockfd, newsockfd, portno;
char buffer[BUFLEN],buffer_send[BUFLEN];
struct sockaddr_in serv_addr, cli_addr;
int i,n,clienti_curenti=0,debug=0;
date_client lista_clienti[10]; //nu vor fi mai mult de 10

fd_set read_fds;	// multimea de citire folosita in select()
fd_set tmp_fds;	// multime folosita temporar
int fdmax;			//valoare maxima file descriptor din multimea read_fds


//Throws error and closes program
void error(char *msg){
	perror(msg);
	exit(1);
}

//Accepts a client if it's not registered yet
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
				printf("Access denied:Client already connected\n");
				memset(buffer,0,sizeof(buffer));
				sprintf(buffer,"Disconnect");
				n = send(newsockfd,buffer,sizeof(buffer),0);
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
		// Adaug timpul la care s-a conectat
		time(&client.timer);
		// Completez campul corespunzator fd
		client.fd = newsockfd;
		// Adaug clientul curent in lista de clienti
		lista_clienti[clienti_curenti] = client;
		clienti_curenti++;
		// Adaug noul socket intors de accept() la multimea descriptorilor de citire
		FD_SET(newsockfd, &read_fds);
		// Actualizez fdmax daca e nevoie
		if (newsockfd > fdmax) { 
			fdmax = newsockfd;
		}
	}
	return;
}

//Removes client from list
void delete_client(date_client delclient){
	int i,j;
	for ( i = 0; i < clienti_curenti ; i++)
	{
		if(lista_clienti[i].fd == delclient.fd ) //compar dupa fd
		{
			//eliminal din vector
			for(j=i;j<clienti_curenti-1;j++)
			{
				lista_clienti[j] = lista_clienti[j+1];
			}
			clienti_curenti--;
			return;
		}
	}
}

//Responsable for server side commands like
//status - displays list of active clients
//kick - kicks client
//quit - closes server
void switch_command(char* buffer){
	int i;
	char kickname[BUFLEN],kick[BUFLEN];

	//implementare status
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
	//implementare quit
	else if (strncmp(buffer,"quit",strlen("quit")) == 0)
	{
		printf("SERVER:SHUTTING DOWN...Sending quit broadcast\n");
		// Construiesc mesaj de inchidere
		memset(buffer_send, 0 , BUFLEN);
		sprintf(buffer_send,"%s","Forceclose");
		for(i = 0 ; i < clienti_curenti ; i++)
		{
			// Trimit mesaj de inchidere
			n = send(lista_clienti[i].fd,buffer_send,sizeof(buffer_send),0);
			if(n<0)
				printf("ERROR sending shut down fd %i\n",lista_clienti[i].fd);
		}

		error("SERVER: SHUT DOWN");
	}
	//implementare kick
	else{
		sscanf(buffer,"%s %s",kick,kickname);
		if(strncmp(kick,"kick",strlen("kick")) != 0)
		{
			fprintf(stderr, "ERROR:Unknown command\n");
			return;
		}
		for(i = 0 ; i < clienti_curenti ; i++)
		{
			if(strncmp(lista_clienti[i].nume,kickname,strlen(kickname))==0)
			{
				printf("SERVER:Kicking user %s\n",kickname);
				// Ii trimit mesaj ca sa se inchida
				memset(buffer_send, 0 , BUFLEN);
				sprintf(buffer_send,"%s","Forceclose");
				n = send(lista_clienti[i].fd,buffer_send,sizeof(buffer_send),0);
				if(n<0)
					printf("ERROR sending kick notice\n");
				// Delete client from list
				FD_CLR(lista_clienti[i].fd,&read_fds);
				delete_client(lista_clienti[i]);

				return;
			}
		}
		printf("SERVER:User not found\n");
	}
	
	return;
}

void client_quit(){
	int j;
	//caut intai clientul in lista dupa sockfd
	for(j=0;j<clienti_curenti;j++)
	{
		if(lista_clienti[j].fd == i)
		{
			printf("SERVER:Clientul %s se deconecteaza.\n",
				lista_clienti[j].nume);
			delete_client(lista_clienti[j]);
			close(i);
			FD_CLR(i,&read_fds);
			return;
		}
	}
}

void client_listclients(){
	int j;
	memset(buffer_send,0,BUFLEN);
	strcat(buffer_send,"Lista clienti conectati: \n");
	for(j=0;j<clienti_curenti;j++)
	{
		strcat(buffer_send,lista_clienti[j].nume);
		strcat(buffer_send," ");
	}
	n = send(i,buffer_send,sizeof(buffer_send),0);
	if(n<0)
		fprintf(stderr,"ERROR la trimitere Listclients");
		//nu oprim server doar afisam eroare
	return;
}

void client_infoclient(char* nume_client){
	int j;
	char aux[BUFLEN];
	time_t cur_timer;
	double connected_time;
	memset(buffer_send,0,BUFLEN);
	strcat(buffer_send,"Informatii despre clientul cerut: \n");
	//caut intai clientul in lista dupa numele sau
	for(j=0;j<clienti_curenti;j++)
	{
		if(strncmp(lista_clienti[j].nume,nume_client,strlen(nume_client)) == 0)
		{
			//adaugam numele
			strcat(buffer_send,"\tNume: ");
			strcat(buffer_send,lista_clienti[j].nume);
			//adaugam portul
			strcat(buffer_send,"\n\tPort: ");
			sprintf(aux,"%d",lista_clienti[j].port);
			strcat(buffer_send,aux);
			//adaugam timp scurs
			time(&cur_timer);
			connected_time = difftime(cur_timer,lista_clienti[j].timer);
			memset(aux,0,sizeof(aux));
			sprintf(aux,"\n\tTimp scurs de la conectare %.2fs",connected_time);
			strcat(buffer_send,aux);
			//dam send
			n = send(i,buffer_send,sizeof(buffer_send),0);
			if(n<0)
				fprintf(stderr,"ERROR la trimitere date client");
			//nu oprim server doar afisam eroare
			return;
		}
	}

	//altfel nu am gasit clientul cerut
	memset(buffer_send,0,BUFLEN);
	sprintf(buffer_send,"ERROR: User not found");
	n = send(i,buffer_send,sizeof(buffer_send),0);
	if(n<0)
		fprintf(stderr,"ERROR la trimitere date client");
	return;
}

//Responsable for querys and messages recieved from clients like
//quit notice "Disconnecting"
//listclients notice
//infoclient notice
//broadcast notice
//sendfile notice
void switch_client_query(char* buffer){
	int j;
	//initializari
	char param1[BUFLEN],param2[BUFLEN],param3[BUFLEN];
	memset(param1,0,BUFLEN);
	memset(param2,0,BUFLEN);
	memset(param3,0,BUFLEN);
	sscanf(buffer,"%s %s %s",param1,param2,param3);

	//Daca in client se apeleaza quit
	if(strncmp(param1,"Disconnecting",strlen("Disconnecting")) == 0){
		client_quit();	
	// Daca in client se apeleaza listclients
	}else if(strncmp(param1,"Listclients",strlen("Listclients")) == 0){
		client_listclients();
	}else if(strncmp(param1,"infoclient",strlen("infoclient")) == 0){
		client_infoclient(param2);
	}

	return;
}

int main(int argc, char const *argv[])
{
	int j;
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

    	for(i = 0; i <= fdmax; i++) {
    		if (FD_ISSET(i, &tmp_fds)) {

    			if (i == 0){
					// citesc de la tastatură o comandă
    				memset(buffer, 0 , BUFLEN);
    				fgets(buffer, BUFLEN-1, stdin);
    				// in functie de ce comanda primesc iau o decizie
    				switch_command(buffer);
    			}
    			else if (i == sockfd){
					// Se conecteaza un nou client
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
    					printf("ERROR at recieve from client on socket %d",i);
    					printf("... Client hung up\n");

    					for (j = 0; j < clienti_curenti; j++){
    						if (lista_clienti[j].fd == i)
								//printeaza ca a iesit clientul
    							printf("SERVER:Clientul %s va fi scos\n", 
    								lista_clienti[j].nume);
    					}
    					for (j = 0; j < clienti_curenti; j++)
    					{
							//caut clientul si il scot
    						if (lista_clienti[j].fd == i)
    							delete_client(lista_clienti[j]);
    					}
    					close(i);
    					// scoatem din multimea de citire socketul
    					FD_CLR(i, &read_fds); 
    				}

    				else
					{ //nu au fost erori, am primit ceva
						printf("DEBUG Am primit %s\n",buffer);
						// in functie de ce primesc de la client iau o decizie
						switch_client_query(buffer);
					}
				}

			}
		}
	}
	close(sockfd);

	return 0;
}