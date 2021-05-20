#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"

#include <pthread.h>

int main(int n_args, char *ll_args[])
{
    /*if (n_args < 2)
    {   fprintf(stderr,"proces: pal_ord3 indice ipo_pf ipo_pc l_pal v_pal retard po_pf id_numPelotas id_fin id_win n_fil n_col\n");
        exit(0);
    }*/
    int indice = atoi(ll_args[1]);
    /*int ipo_pf = atoi(ll_args[2]);
    int ipo_pc = atoi(ll_args[3]);
    int l_pal = atoi(ll_args[4]);
    float v_pal = atof(ll_args[5]);
    int retard = atoi(ll_args[6]);
    float po_pf = atof(ll_args[7]);
    int id_numPelotas = atoi(ll_args[8]);
    int id_fin = atoi(ll_args[9]);
    int id_win = atoi(ll_args[1]);
    int n_fil = atoi(ll_args[2]);
    int n_col = atoi(ll_args[3]);
    int f_h;*/
    //char index = (intptr_t)indice + '1';
    /*void *p_win;

    p_win = map_mem(id_win);

    win_set(p_win,n_fil,n_col);*/

    FILE *f;
    f = fopen("error","a");
    if(f != NULL){
      //se ha abierto bien
      fprintf(f,"Identificador: %d\n",indice);
    }


  return (0);
}