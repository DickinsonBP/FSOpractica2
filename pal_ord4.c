#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"
#include "semafor.h"

#include <pthread.h>

int main(int n_args, char *ll_args[])
{
    int indice, id_semafor;
    int f_h, n_fil, n_col, id_win, retard, l_pal, id_numPelotas, id_fin, *p_numPelotas, *p_fin, *p_ipopf, *p_ipopc, id_ipopf, id_ipopc;
    float v_pal, po_pf;
    void *p_win;

    if (n_args < 14)
    {
      fprintf(stderr, "error parametros\n");
      exit(0);
    }

    indice = atoi(ll_args[1]);
    id_win = atoi(ll_args[2]);
    n_fil = atoi(ll_args[3]);
    n_col = atoi(ll_args[4]);
    p_win =map_mem(id_win);
    id_ipopf = atoi(ll_args[5]);
    p_ipopf = map_mem(id_ipopf);
    id_ipopc = atoi(ll_args[6]);
    p_ipopc =map_mem(id_ipopc);
    v_pal = atof(ll_args[7]);
    po_pf =atof(ll_args[8]);
    l_pal = atoi(ll_args[9]);
    retard = atoi(ll_args[10]);
    id_numPelotas = atoi(ll_args[11]);
    p_numPelotas = map_mem(id_numPelotas);
    id_fin = atoi(ll_args[12]);
    p_fin = map_mem(id_fin);
    id_semafor = atoi(ll_args[13]);

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
    waitS(id_semafor);
    win_set(p_win,n_fil,n_col);
    signalS(id_semafor);

    char index = (intptr_t)indice + '0';
    int indc = (intptr_t)indice;

  do
  {
    f_h = po_pf + v_pal; /* posicio hipotetica de la paleta */
    if (f_h != *p_ipopf)   /* si pos. hipotetica no coincideix amb pos. actual */
    {
      if (v_pal > 0.0) /* verificar moviment cap avall */
      {
        if (win_quincar(f_h + l_pal - 1, *p_ipopf) == ' ') /* si no hi ha obstacle */
        {
          waitS(id_semafor);
          win_escricar(*p_ipopf, *p_ipopc, ' ', NO_INV); /* esborra primer bloc */
          signalS(id_semafor);
          po_pf += v_pal;
          *p_ipopf = po_pf;                                        /* actualitza posicio */
          waitS(id_semafor);
          win_escricar(*p_ipopf + l_pal - 1, *p_ipopc, index, INVERS); /* impr. ultim bloc */
          signalS(id_semafor);
        }
        else{
          /* si hi ha obstacle, canvia el sentit del moviment */
          v_pal = -v_pal;   
        }
      }
      else /* verificar moviment cap amunt */
      {
        if (win_quincar(f_h, *p_ipopc) == ' ') /* si no hi ha obstacle */
        {
          waitS(id_semafor);
          win_escricar(*p_ipopf + l_pal - 1, *p_ipopc, ' ', NO_INV); /* esbo. ultim bloc */
          signalS(id_semafor);
          po_pf += v_pal;
          *p_ipopf = po_pf;                            /* actualitza posicio */
          waitS(id_semafor);
          win_escricar(*p_ipopf, *p_ipopc, index, INVERS); /* impr. primer bloc */
          signalS(id_semafor);
        }
        else{
          /* si hi ha obstacle, canvia el sentit del moviment */
          v_pal = -v_pal;
        } 
      } 
    }
    else{
      po_pf += v_pal; /* actualitza posicio vertical real de la paleta */
    }
    win_retard(retard);
  } while((*p_fin != 1) && (*p_numPelotas > 0));
    



  return (0);
}