#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"

#include <pthread.h>

int main(int n_args, char *ll_args[])
{
    int indice;
    int n_fil, n_col, id_win, ipo_pf, ipo_pc, retard, l_pal, id_numPelotas, id_fin, *p_numPelotas, *p_fin;
    float v_pal, po_pf;
    void *p_win;

    if (n_args < 10)
    {
      fprintf(stderr, "error parametros\n");
      exit(0);
    }

    indice = atoi(ll_args[1]);
    id_win = atoi(ll_args[2]);
    n_fil = atoi(ll_args[3]);
    n_col = atoi(ll_args[4]);
    p_win =map_mem(id_win);
    ipo_pf = atoi(ll_args[5]);
    ipo_pc = atoi(ll_args[6]);
    v_pal = atof(ll_args[7]);
    po_pf =atof(ll_args[8]);
    l_pal = atoi(ll_args[9]);
    retard = atoi(ll_args[10]);
    id_numPelotas = atoi(ll_args[11]);
    p_numPelotas = map_mem(id_numPelotas);
    id_fin = atoi(ll_args[12]);
    p_fin = map_mem(id_fin);

    if (p_win == (int*)-1)
    {
      fprintf(stderr, "error id ventana\n");
      exit(0);
    }
    if (p_numPelotas == (int*)-1)
    {
      fprintf(stderr, "error num_pelotas\n");
      exit(0);
    }
    if (p_fin == (int*)-1)
    {
      fprintf(stderr, "error fin\n");
      exit(0);
    }
    
    /*crear acceso a la ventana del proceso padre*/
    win_set(p_win,n_fil,n_col);

    FILE *f;
    f = fopen("error","a");
    if(f != NULL){
      /*se ha abierto bien*/
      fprintf(f,"Identificador: %d\n",indice);
      fprintf(f,"n fil: %d\n",n_fil);
      fprintf(f,"n col: %d\n",n_col);
      fprintf(f,"ipo_pf: %d\n",ipo_pf);
      fprintf(f,"ipo_pc: %d\n",ipo_pc);
      fprintf(f,"v_pal: %f\n",v_pal);
      fprintf(f,"po_pf: %f\n",po_pf);
      fprintf(f,"l_pal: %d\n",l_pal);
      fprintf(f,"retard: %d\n",retard);
      fprintf(f,"num_pelotas: %d\n",*p_numPelotas);
      fprintf(f,"fin: %d\n",*p_fin);
    }


  return (0);
}