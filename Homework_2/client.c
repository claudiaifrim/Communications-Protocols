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

} date_client;

int sockfd,newsockfd,listen_sockfd,n,i;
struct sockaddr_in serv_addr,listen_addr,accept_addr;
char buffer[BUFLEN],buffer_send[BUFLEN];

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

	

	//------------May not need this now------------//
	// Aflu portul pe care asculta clientul
	//socklen_t listen_len = sizeof(listen_addr);
	//if (getsockname(listen_sockfd, (struct sockaddr *) &listen_addr, &listen_len) == -1)
	//	error((char *)"ERROR getting socket name");
	//------------May not need this now------------//

	//-----------------------------------------------------------------------//
	// Urmeaza sa verificam daca avem voie sa ne conectam sau nu
	// Daca exista deja un user cu acelasi nume vom fi deconectati
	
	// Aflu portul pe care asculta clientul
	socklen_t listen_len = sizeof(listen_addr);
	n = getsockname(listen_sockfd, (struct sockaddr *) &listen_addr, &listen_len);
	if ( n < 0 )
		error((char *)"ERROR getting socket details");

	date_client client;
	char* ip;
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
    FD_SET(fileno(stdin), &read_fds); 
    //fd_max va fi listen_fd
    fdmax = listen_sockfd;

    while(1)
    {

    }


    return 0;
}