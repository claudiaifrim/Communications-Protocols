#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

char getbit(unsigned char oct,int pos)
{

  unsigned char mask = 1;
  mask = mask << pos; 
  if((oct & mask) != 0)
    return 1;
  else return 0;
  
  //return(oct & (1 << pos)) != 0;  
}
void setBit(unsigned char* oct,int pos,char val)
{
  if(val == 1)
    *oct |= (1 << pos);
  else 
    *oct &= -(1 << pos);

  return;
}
char getParity(unsigned char oct)
{
  //return !(getbit(oct,0) ^ getbit(oct,1) ^ getbit(oct,2) ^getbit(oct,3) ^ getbit(oct,4) ^ getbit(oct,5) ^ getbit(oct,6) ^getbit(oct,7));
  unsigned char sum = 0;
  int i;
  for(i = 0; i < 8; i++){
    sum = sum ^ getbit(oct, i);
  }
  return sum;
}

int main(int argc,char** argv){
  //intializari
  msg r,t;
  init(HOST,PORT);

  //initializari variabile
  unsigned char cuv[9];       //8+terminator
  unsigned char biti_control;
  int lungime_cuvant=9;
  //receive message
  if (recv_message(&r)<0){
    perror("Receive message");
    return -1;
  }
  printf("[%s] Got msg with payload: %s\n",argv[0],r.payload);

  //separam cuvantul de bitii de control
  memcpy(cuv, r.payload, lungime_cuvant);
  printf("Cuvantul primit este: %s\n", cuv);

  memcpy(&biti_control, r.payload + lungime_cuvant, 1);
  
  //verificam bitii de control
  int i;
  for(i = 0; i < 8; i++){
    if(getParity(cuv[i]) != getbit(biti_control, i)){
      printf("Byte-ul gresit este %d adica charul %c \n", i,cuv[i]);
      break;
    }
  }

  sprintf(t.payload,"ACK(%s)",r.payload);
  t.len = strlen(t.payload+1);
  send_message(&t);
  return 0;
}
