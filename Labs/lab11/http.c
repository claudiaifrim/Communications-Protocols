#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <unistd.h>     
#include <sys/types.h>  
 #include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define SMTP_PORT 250
#define MAXLEN 500

/**
 * Citeste maxim maxlen octeti din socket-ul sockfd. Intoarce
 * numarul de octeti cititi.
 */
 ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
  ssize_t n, rc;
  char    c, *buffer;

  buffer = vptr;

  for ( n = 1; n < maxlen; n++ ) {	
   if ( (rc = read(sockd, &c, 1)) == 1 ) {
     *buffer++ = c;
     if ( c == '\n' )
      break;
  }
  else if ( rc == 0 ) {
   if ( n == 1 )
    return 0;
  else
    break;
}
else {
 if ( errno == EINTR )
  continue;
return -1;
}
}

*buffer = 0;
return n;
}

/**
 * Trimite o comanda SMTP si asteapta raspuns de la server.
 * Comanda trebuie sa fie in buffer-ul sendbuf.
 * Sirul expected contine inceputul raspunsului pe care
 * trebuie sa-l trimita serverul in caz de succes (de ex. codul
 * 250). Daca raspunsul semnaleaza o eroare se iese din program.
 */
void send_command(int sockfd, char sendbuf[]) {
  char recvbuf[MAXLEN];
  int nbytes;
  char CRLF[3];
  
  //CRLF[0] = 13; CRLF[1] = 10; CRLF[2] = 0;
  //strcat(sendbuf, CRLF);
  printf("Trimit: %s", sendbuf);
  write(sockfd, sendbuf, strlen(sendbuf));
 // nbytes = Readline(sockfd, recvbuf, MAXLEN - 1);
 // recvbuf[nbytes] = 0;
 // printf("Am primit: %s", recvbuf);
}

int main(int argc, char **argv) {
  int sockfd;
  int port = 80;
  char server_ip[10];
  struct sockaddr_in servaddr;
  char adresa_conectare[20];
  char recvbuf[MAXLEN];
  char sendbuf[MAXLEN];

  printf("Introdu adresa de conectare: ");
  scanf("%s",adresa_conectare);
  printf("%s\n",adresa_conectare );
  struct hostent * host = gethostbyname(adresa_conectare);

  char * elem = host->h_addr_list[0];
  char * b = inet_ntoa(*(struct in_addr *) elem);

  printf("The address is %s\n", b);

///////////////////////////////////////////////////

  if (inet_aton(b, &servaddr.sin_addr) <= 0 ) {
    printf("Adresa IP invalida.\n");
    exit(-1);
  }

  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    printf("Eroare la  creare socket.\n");
    exit(-1);
  }  

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);


  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
    printf("Eroare la conectare\n");
    exit(-1);
  }
  memset(&sendbuf, 0, sizeof(sendbuf));
  sprintf(sendbuf,"GET / HTTP/1.1\nHost: www.google.com\n\n");
  printf("Trimit: %s", sendbuf);
  write(sockfd, sendbuf, strlen(sendbuf));
  memset(&recvbuf, 0, sizeof(recvbuf));
  int i = 20;
  while(i>0){
  Readline(sockfd, recvbuf, MAXLEN -1);
  //if(recvbuf)
  printf("%s", recvbuf);
  i--;
  }

  return 0;

}