#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

static char getbit(int oct,int pos)
{

  int mask = 1;
  mask = mask << pos; 
  if((oct & mask) != 0)
    return 1;
  else return 0;
  
  //return(oct & (1 << pos)) != 0;  
}
static void setBit(unsigned char* oct,int pos,char val)
{
  if(val == 1)
    *oct |= (1 << pos);
  else 
    *oct &= ~(1 << pos);

  return;
}

void changeBit(unsigned char* oct,int pos) {
  *oct ^= (1 << pos);
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
  //intializari
  msg r,t;
  init(HOST,PORT);

  //initializari variabile
  unsigned char cuv[9];       //8+terminator
  int biti_control;
  int lungime_cuvant=9;
  int i;
  //receive message
  if (recv_message(&r)<0){
    perror("Receive message");
    return -1;
  }
  printf("[%s] Got msg with payload: %s\n",argv[0],r.payload);

  //separam cuvantul de bitii de control
  memcpy(cuv, r.payload, lungime_cuvant);
  printf("Cuvantul primit este: %s\n", cuv);

  memcpy(&biti_control, r.payload + lungime_cuvant, sizeof(int));
  
  //recalculam pentru fiecare litera(octet) bitii de control
  //ii alaturam si verificam octetul respectiv
  
  for(i = 0 ; i<lungime_cuvant-1 ; i++){
    
  //pt fiecare litera calculam bitii de control
    char P0,P1,P2,P3;

    //copiem 4 biti in o variabila temporara
    char b_control_litera = (char)biti_control;
    
    P0 = getbit(biti_control,0+4*i) ^ getbit(cuv[i],0) ^ getbit(cuv[i],1) ^ getbit(cuv[i],3) ^ getbit(cuv[i],4) ^ getbit(cuv[i],6);
    P1 = getbit(biti_control,1+4*i) ^ getbit(cuv[i],0) ^ getbit(cuv[i],2) ^ getbit(cuv[i],3) ^ getbit(cuv[i],5) ^ getbit(cuv[i],6);
    P2 = getbit(biti_control,2+4*i) ^ getbit(cuv[i],1) ^ getbit(cuv[i],2) ^ getbit(cuv[i],3) ^ getbit(cuv[i],7);
    P3 = getbit(biti_control,3+4*i) ^ getbit(cuv[i],4) ^ getbit(cuv[i],5) ^ getbit(cuv[i],6) ^ getbit(cuv[i],7);
    
    //alaturam bitii calculati
    int pozitie_gresita;
    pozitie_gresita = 8 * P3 + 4 * P2 + 2 * P1 + P0;
    //daca pozitie_gresita == 0 litera este OK
    
    if(pozitie_gresita != 0)
      printf("Bitul gresit este pe pozitia %i adica in litera %c \n"
        ,pozitie_gresita,cuv[i] );
    //mapam pozitia gresita pe bitii din sir
    switch(pozitie_gresita) {
      case 1: case 2: case 4: case 8:
      break;
      case 3:
      changeBit(&cuv[i],0);break;
      case 5:
      changeBit(&cuv[i],1);break;
      case 6:
      changeBit(&cuv[i],2);break;
      case 7:
      changeBit(&cuv[i],3);break;
      default:
      changeBit(&cuv[i],pozitie_gresita-4);break;
    }  
    
  }

  sprintf(t.payload,"ACK(%s)",cuv);
  t.len = strlen(t.payload+1);
  printf("S-a terminat receive\n");
  send_message(&t);
  return 0;
}
