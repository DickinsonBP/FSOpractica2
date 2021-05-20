#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"

#include <pthread.h>

int main(int n_args, char *ll_args[])
{
    int indice = atoi(ll_args[1]);

    FILE *f;
    f = fopen("error","a");
    if(f != NULL){
      //se ha abierto bien
      fprintf(f,"Identificador: %d\n",indice);
    }


  return (0);
}