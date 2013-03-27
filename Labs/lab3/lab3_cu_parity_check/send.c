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

  //initializari
  init(HOST,PORT);
  msg t;

  unsigned char cuv[8]="Hello PC"; //cuvantul care va fi transmis
  unsigned char biti_control = 0;   //aici prelucram fiecare litera in parte

  int lungime_cuvant=8;//strlen(cuv)/sizeof(unsigned char);
  printf("%i\n",lungime_cuvant );
  int i;
  
  for(i = 0 ; i<lungime_cuvant ; i++)
    if(getParity(cuv[i])){
      setBit(&biti_control, i, 1);
    }
    //daca byte-ul i e par seteaza bitul i din biti_control pe 1
    //altfel ramane 8

  char terminator='\0'; //adaugam terminator la sf de string
  memcpy(t.payload, cuv,lungime_cuvant);                     //punem cuvantul
  memcpy(t.payload + lungime_cuvant, &terminator  , 1);      //terminator
  memcpy(t.payload + lungime_cuvant + 1, &biti_control , 1); //biti control


  t.len = strlen(t.payload)+1;
  send_message(&t);

  if (recv_message(&t)<0){
    perror("receive error");
  }
  else {
    printf("[%s] Got reply with payload: %s\n",argv[0],t.payload);
  }

  return 0;
}
