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

void error(char *msg)
{
	perror(msg);
	exit(1);
}

typedef struct{
	int fd; // fd asignat de server
	int port;
	char nume[BUFLEN];
	char ip[BUFLEN];
	time_t timer;
} date_client;

int sockfd,newsockfd,listen_sockfd,n,i;
struct sockaddr_in serv_addr,listen_addr,accept_addr;
char buffer[BUFLEN],buffer_send[BUFLEN];

void quit(){
	memset(buffer_send,0,BUFLEN);
	sprintf(buffer_send,"%s","Disconnecting");
	n = send(sockfd,buffer_send,sizeof(buffer_send),0);
	if(n<0){
		close(sockfd);
		close(listen_sockfd);
		error("ERROR la trimitere Disconnecting");
	}
	close(sockfd);
	close(listen_sockfd);
	error("CLIENT:Shutting Down...");
}

void listclients(){
	memset(buffer_send,0,BUFLEN);
	sprintf(buffer_send,"%s","Listclients");
	n = send(sockfd,buffer_send,sizeof(buffer_send),0);
	if(n<0){
		fprintf(stderr,"ERROR la trimitere Listclients");
		return; //nu oprim clientu doar afisam eroare
	}

	memset(buffer,0,BUFLEN);
	n = recv(sockfd,buffer,sizeof(buffer),0);
	if(n<=0){
		error("ERROR at recieve from server");
	}else{
		printf("%s\n", buffer);	
	}
	return;
}

void infoclient(char* nume_client)
{
	memset(buffer_send,0,BUFLEN);
	sprintf(buffer_send,"infoclient %s",nume_client);
	n = send(sockfd,buffer_send,sizeof(buffer_send),0);
	if(n<0){
		fprintf(stderr,"ERROR la trimitere cerere infoclient");
		return; //nu oprim clientu doar afisam eroare
	}

	memset(buffer,0,BUFLEN);
	n = recv(sockfd,buffer,sizeof(buffer),0);
	if(n<=0){
		error("ERROR at recieve from server");
	}else{
		printf("%s\n", buffer);	
	}
	return;

	return;
}

//Responsable for client side commands like
//TODO
//TODO
void switch_command(char* buffer){

	//initializari
	char param1[BUFLEN],param2[BUFLEN],param3[BUFLEN];
	memset(param1,0,BUFLEN);
	memset(param2,0,BUFLEN);
	memset(param3,0,BUFLEN);
	sscanf(buffer,"%s %s %s",param1,param2,param3);

	//implementare quit
	if(strncmp(param1,"quit",strlen("quit")) == 0){
		quit();		
		return;
	}
	//implementare infoclient
	else if(strncmp(param1,"infoclient",strlen("infoclient")) == 0)
	{

		if(strcmp(param2,"") == 0){
			fprintf(stderr, 
				"CIENT:Wrong command.Usage: infoclient <nume_client>\n");
			return;
		}

		infoclient(param2);
		
		return;
	}
	//implementare message
	else if(strncmp(param1,"message",strlen("message")) == 0)
	{

		if((strcmp(param2,"") == 0) || (strcmp(param3,"") == 0)) {
			fprintf(stderr, 
				"CIENT:Wrong command.Usage: message <nume_client> <mesaj>\n");
			return;
		}

		printf("CIENT:Comanda data %s %s %s\n",param1,param2,param3);
		
		return;
	}
	//implementare broadcast
	else if(strncmp(param1,"broadcast",strlen("broadcast")) == 0)
	{

		if(strcmp(param2,"") == 0){
			fprintf(stderr, 
				"CIENT:Wrong command.Usage: broadcast <mesaj>\n");
			return;
		}

		printf("CIENT:Comanda data %s %s\n",param1,param2);
		
		return;

	}
	//implementare sendfile
	else if(strncmp(param1,"sendfile",strlen("sendfile")) == 0)
	{
		if((strcmp(param2,"") == 0) || (strcmp(param3,"") == 0)) {
			fprintf(stderr, 
				"CIENT:Wrong command.Usage:"
				"sendfile <client_destinatie> <nume_fisier>\n");
			return;
		}

		printf("CIENT:Comanda data %s %s %s\n",param1,param2,param3);
		
		return;
	}
	//implementare history
	else if(strncmp(param1,"history",strlen("history")) == 0)
	{
		printf("CIENT:Comanda data %s\n",param1);
		
		return;
	}
	//implementare listclients
	else if(strncmp(param1,"listclients",strlen("listclients")) == 0)
	{
		listclients();
		return;
	}

	//else something is wrong
	fprintf(stderr, "ERROR:Unknown command\n");
	return;
}

int main(int argc, char const *argv[])
{
	// Usage error
	if (argc < 4){
		fprintf(stderr,"Usage %s client_name server_address server_port\n", argv[0]);
		exit(-1);
	}

	// Creaza socket pentru ascultare pentru alti clienti
	listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sockfd < 0)
		error("ERROR opening listening socket");

	// Creaza listen_addr
	memset((char *) &listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(0);			// Port dat de catre OS
	listen_addr.sin_addr.s_addr = INADDR_ANY;	// Adresa IP a mașinii

	// Bind listen_sockfd la listen_addr
	if (bind(listen_sockfd, (struct sockaddr *) &listen_addr, sizeof(struct sockaddr)) < 0)
		error((char *)"ERROR on binding listening socket");

	// Start listening
	listen(listen_sockfd, MAX_CLIENTS);

	// Creaza Socket pentru conectare la server
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("ERROR opening server socket");

	// Creaza serv_addr
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	inet_aton(argv[2], &serv_addr.sin_addr);

	// Connect to server
	if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0)
		error((char *)"ERROR connecting to server");

	
	//-----------------------------------------------------------------------//
	// Urmeaza sa verificam daca avem voie sa ne conectam sau nu
	// Daca exista deja un user cu acelasi nume vom fi deconectati
	
	// Aflu portul pe care asculta clientul
	socklen_t listen_len = sizeof(listen_addr);
	n = getsockname(listen_sockfd, (struct sockaddr *) &listen_addr, &listen_len);
	if ( n < 0 )
		error((char *)"ERROR getting socket details");

	date_client client;
	memset(&client, 0,sizeof(client));	
	sprintf(client.nume,"%s",argv[1]);	//adaug numele
	sprintf(client.ip,"%s",inet_ntoa(listen_addr.sin_addr)); 	//adaug ip
	client.port = ntohs(listen_addr.sin_port); //adaug portu pe care face listen
	// trimit la server datele
	n = send(sockfd,&client,sizeof(client), 0);
	if(n < 0)
		error("ERROR sending identity to server");

	memset(buffer,0,BUFLEN);
	n = recv(sockfd,buffer,sizeof(buffer),0);
	if(n <= 0)
	{
	// nu reuseste sa se conecteze din cauze unknown
		if(n == 0) 	printf("Recv a intors 0\n");
		else 		error("ERROR at recv");
		close(sockfd);
		error("ERROR Recv a intors 0");
	}else{
		// verifica daca a primit disconnect sau a fost acceptat
		if(strcmp(buffer, "Disconnect") == 0)
		{
			close(sockfd);
			error("Denied acces from server.Closing");
		}
	}

	printf("%s\n", buffer);
	// Totul a fost ok acum suntem conectati
	//----------------------------------------------------------------------//

	fd_set read_fds;	// multimea de citire folosita in select()
    fd_set tmp_fds;		// multime folosita temporar
    int fdmax;			// valoare maxima file descriptor din multimea read_fds

	//golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    //adaugam fd pentru socketul serverului
    FD_SET(sockfd, &read_fds);
    //adaugam fd pentru socketul de listen
    FD_SET(listen_sockfd, &read_fds);
    //adaugam fd pentru STDIN, ar trebui sa fie 0
    FD_SET(0, &read_fds); 
    //fd_max va fi listen_fd
    fdmax = sockfd;

    while(1)
    {
    	tmp_fds = read_fds; 

    	if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
    		error("ERROR in select");

    	for(i = 0; i <= fdmax; i++) {
    		if (FD_ISSET(i, &tmp_fds)) {

    			if (i == 0){
					// citesc de la tastatură o comandă
    				memset(buffer, 0 , BUFLEN);
    				fgets(buffer, BUFLEN-1, stdin);
    				switch_command(buffer);
    			}

    			else if (i == sockfd)
    			{
    				//Am primit ceva de la server
    				//Se intra aici doar in caz de "Forceclose" de la server
    				memset(buffer, 0 , BUFLEN);
    				if ((n = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
    				{
    					if (n == 0)
    					{
							//conexiunea s-a inchis
    						printf("ERROR: Socket with server hung up\n");
    					} else
    					{
    						error((char *)"ERROR at recv");
    					}
    					close(sockfd);
    					close(listen_sockfd);
    					printf("CLIENT: Server hung up...Shutting Down.\n");
    					exit(0);
    				}
    				else
    				{
						// Verific daca am primit forceclose
    					if (strncmp(buffer, "Forceclose", 
    						strlen("Forceclose")) == 0)
    					{
    						printf("CLIENT:Forceclose recieved from server.\n");
    						printf("CLIENT:Shutting Down...\n");
    						close(listen_sockfd);
    						close(sockfd);
    						exit(0);
    					}

						// Nu ar trebui sa ajung aici
    					error("CLIENT: Unknown message from server");
    				}
    			}
    			else if (i == listen_sockfd)
    			{
    				printf("Debug\n");
    				// Se conecteaza un alt client la mine
    			}
    			else{
    				printf("Debug\n");
    				// Primesc date pe unul din socketii pe care
    				// este conectat un alt client deja

    			}

    		}
    	}
    }

    close(sockfd);
    return 0;
}