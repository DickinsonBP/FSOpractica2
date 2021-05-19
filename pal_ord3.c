#include <stdio.h> /* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "winsuport.h" /* incloure definicions de funcions propies */
#include "memoria.h"

int main(int n_args, char *ll_args[])
{
  FILE *f;
  int i, indice;

  indice = atoi(ll_args[1]);
  f = fopen("prueba.txt","a");

  if(f == NULL){
    printf("El fichero no se ha podido abrir en modo escritura\n");
    return -1;
  }else{
    i = 0;
    while (!feof(f) && i < 100)
    {
      fprintf(f,"Indice: %d\tIteracion: %d\n",indice, i);
      i++;
    }
    
  }
  fclose(f);
  return 0;
}