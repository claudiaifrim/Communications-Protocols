#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc,char** argv){

  init(HOST,PORT);
  msg t;

  char buffer[5];
  int buffer_length = sizeof(buffer) / sizeof(buffer[0]);
  int bytes_read = 0;
  int time_to_end=0;
  char next_frame_to_send = '0';

  int f = open(argv[1] , O_RDONLY);
  //alfam lungime fisier
  struct stat fileStat;
  if(stat(argv[1],&fileStat)<0)
    return 0;
  
  //trimitem nume fisier si lungimea sa
  sprintf(t.payload,"%s","write.txt");
  t.len = fileStat.st_size;
  send_message(&t);

  bytes_read = read(f , buffer , buffer_length);

  while(1)
  {

    sprintf(t.payload,"%s%c",buffer,next_frame_to_send);

    if(time_to_end==1) //daca am ajuns la ultimuk ciclu
      t.len= bytes_read;
    else
      t.len = bytes_read + 1;

    send_message(&t);

    if (recv_message(&t)<0){
      perror("receive error");
      close(f);
      return 1;
    }

    if(time_to_end==1)
    {
      printf("[%s] ACK Got reply with payload: %s\n",argv[0],t.payload);
      break;
    }
    
    if(t.payload[t.len]==next_frame_to_send){
      //am primit ACK bun
      printf("[%s] ACK Got reply with payload: %s\n",argv[0],t.payload);
      memset(buffer,'\0',sizeof(buffer));
      bytes_read = read(f , buffer , buffer_length);

      //daca am ajuns la sfarsitul fisierului (ultimii 0-4 bytes)
      if(bytes_read<buffer_length) 
      {
        time_to_end=1;
      } 
      next_frame_to_send = (next_frame_to_send == '0') ? '1':'0';
    }
  }

  printf("ENDING Send - Shutting Down\n");

  fflush(stdout);
  close(f);
  return 0;
}
