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

static char getbit(unsigned char oct,int pos)
{
  unsigned char mask = 1;
  mask = mask << pos; 
  if((oct & mask) != 0)
    return 1;
  else return 0;
}
static void setBit(int* oct,int pos,char val)
{
  if(val == 1)
    *oct |= (1 << pos);
  else 
    *oct &= ~(1 << pos);
  
  
  return;
}
char getParity(unsigned char oct)
{
  unsigned char sum = 0;
  int i;
  for(i = 0; i < 8; i++){
    sum = sum ^ getbit(oct, i);
  }
  return sum;
}

int main(int argc,char** argv){
/* Varianta cu corectare Hamming */
  //initializari
  init(HOST,PORT);
  msg t;

  unsigned char cuv[8]="Hello PC"; //cuvantul care va fi transmis
  int biti_control = 0;   //aici punem 4*8 biti de control
                          //P0 P1 P2 P3
  int lungime_cuvant=8;

  int i=0;

  char terminator='\0'; //adaugam terminator la sf de string
  memcpy(t.payload, cuv,lungime_cuvant);                     //punem cuvantul
  memcpy(t.payload + lungime_cuvant, &terminator  , 1);      //terminator

  
  for(i = 0 ; i<lungime_cuvant; i++){
  //pt fiecare litera calculam bitii de control
    char P0,P1,P2,P3;
    
  //calculam bitii control
    P0 = getbit(cuv[i],0) ^ getbit(cuv[i],1) ^ getbit(cuv[i],3) ^ getbit(cuv[i],4) ^ getbit(cuv[i],6);
    P1 = getbit(cuv[i],0) ^ getbit(cuv[i],2) ^ getbit(cuv[i],3) ^ getbit(cuv[i],5) ^ getbit(cuv[i],6);
    P2 = getbit(cuv[i],1) ^ getbit(cuv[i],2) ^ getbit(cuv[i],3) ^ getbit(cuv[i],7);
    P3 = getbit(cuv[i],4) ^ getbit(cuv[i],5) ^ getbit(cuv[i],6) ^ getbit(cuv[i],7);
  //adaugam bitii control
    setBit(&biti_control,0 + 4*i,P0);
    setBit(&biti_control,1 + 4*i,P1);
    setBit(&biti_control,2 + 4*i,P2);
    setBit(&biti_control,3 + 4*i,P3);    
  }
memcpy(t.payload + lungime_cuvant + 1   , &biti_control , sizeof(int)); //biti control


t.len = strlen(t.payload)+ 1 + sizeof(int);
send_message(&t);

if (recv_message(&t)<0){
  perror("receive error");
}
else {
  printf("[%s] Got reply with payload: %s\n",argv[0],t.payload);
}

return 0;
}
