#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc,char** argv){

  init(HOST,PORT);
  msg r,t;
  char frame_expected = '0';
  if (recv_message(&r)<0){
    perror("Receive message");
    return -1;
  }

  int dim_ramas=r.len-1;
  int f = open(r.payload, O_WRONLY | O_CREAT);

  if ( f < 0 ){
    perror("Cannot open file");
    return 1;
  }

  while(1){
    if (recv_message(&r)<0){
      perror("Receive message");
      return -1;
    }
    if(r.payload[r.len] == frame_expected) {

      printf("[%s] Got msg with payload: %s.SENDING ACK\n",argv[0],r.payload);          
      send_message(&r); //trimitem ACK
      frame_expected = (frame_expected == '0') ? '1':'0';

      if (write(f, r.payload, r.len -1) < 0) {
        perror("Cannot write to file");
        close(f);
        return -1;
      }
      //scadem din dimensiunea fisierului
      dim_ramas -= r.len-1;
      if(dim_ramas <= 0) 
      {
        break; //GATA
      }
    }
    else{
      printf("SENDING NACK");
      memset(t.payload,'\0',sizeof(t.payload));
      sprintf(t.payload,"%s",r.payload);
      t.len = strlen(t.payload) + 1;
      send_message(&t);
    }
  }
  printf("ENDING Receive - Shutting Down\n");

  fflush(stdout);
  close(f);
  return 0; 
}
