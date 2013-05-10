#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}
typedef struct {
    int socket_no;
    char payload[256];
} msg;

static msg t;

int main(int argc, char *argv[])
{
    int sockfd, n,i;
    struct sockaddr_in serv_addr;
    struct hostent *server;
     

     char buffer[256];
     if (argc < 3) {
         fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
         exit(0);
     }  

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &serv_addr.sin_addr);
    
    
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");    }


    fd_set read_fds;    //multimea de citire folosita in select()
    fd_set tmp_fds;    //multime folosita temporar 
    int fdmax;     //valoare maxima file descriptor din multimea read_fds

    //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
		
		 //Adaugam Socketul care 
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

    FD_SET(0,&read_fds); //FD for stdin
   
    printf("sockfd = %d \n",sockfd );
 
/* --------------------Varianta fara select--------------------
    

    
    char tmp_nothing ;
    //citesc de la tastatura cui vreau sa trimit
    printf("\nSend to (server is -1) : ");
    scanf("%d",&t.socket_no);
    scanf("%c",&tmp_nothing); //avoids \n from scanf


    //citesc de la tastatura ce vreau sa trimit
    printf("\nWhat to send:");
    memset(t.payload, 0 , 256);
    fgets(t.payload,sizeof(t.payload),stdin);
    //scanf("%s",t.payload);

    //trimit mesaj la server
    printf("Sending message to server\n");
    n = send(sockfd,&t,sizeof(t), 0);
    if (n < 0) 
       error("ERROR writing to socket");
    printf("Message sent to server \n");
    /* daca am trimis Exit opreste clientul */
/*
    memset(buffer, 0 , 256);
    n = recv(sockfd,buffer,255,0);
    printf("Am primit de la server %s \n",buffer );
    if(strcmp(t.payload,"Exit\n") == 0) return 0; 
    --------------------Varianta fara select--------------------
*/
    
    printf("Send to (server is -1) : \n");       
     while(1){
        tmp_fds = read_fds; 
        if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
            error("ERROR in select");
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                
                if(i == sockfd) //primesc de la server ceva
                {
                    memset(buffer, 0 , BUFLEN);
                    n = recv(sockfd,buffer,255,0);
                    printf("Am primit de la server %s \n",buffer );

                }

                if(i == 0)
                {
                    char tmp_nothing ;
                    //citesc de la tastatura cui vreau sa trimit
                    printf("\nSend to (server is -1) : ");
                    scanf("%d",&t.socket_no);
                    scanf("%c",&tmp_nothing); //citeste separat \n de la scanf


                    //citesc de la tastatura ce vreau sa trimit
                    printf("\nWhat to send:");
                    memset(t.payload, 0 , 256);
                    fgets(t.payload,sizeof(t.payload),stdin);
                    //scanf("%s",t.payload);

                    //trimit mesaj la server
                    printf("Sending message to server\n");
                    n = send(sockfd,&t,sizeof(t), 0);
                    if (n < 0) 
                       error("ERROR writing to socket");
                   printf("Message sent to server \n");
                    /* daca am trimis Exit opreste clientul */
                  if(strcmp(t.payload,"Exit\n") == 0) return 0;        
                }

                
        
          }

     }
   }

   return 0;
}


