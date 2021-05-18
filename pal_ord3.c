#include <stdio.h> /* incloure definicions de funcions estandard */
#include <stdlib.h>
#include "winsuport2.h" /* incloure definicions de funcions propies */
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include "memoria.h"

int main(int n_args, char *ll_args[])
{
    if (n_args < 5)
    {   fprintf(stderr,"proces: pal_ord3 ipo_pf[i]  ipo_pc[i]  l_pal v_pal[i] po_pf[i] retard num_pelotas fin\n");
        exit(0);
    }
    int indice = atoi(ll_args[1]);
    int ipo_pf = atoi(ll_args[2]);
    int ipo_pc = atoi(ll_args[3]);
    int l_pal = atoi(ll_args[4]);
    int v_pal = atoi(ll_args[5]);
    int po_pf = atoi(ll_args[6]);
    int retard = atoi(ll_args[7]);
    int num_pelotas = atoi(ll_args[8]);
    int fin = atoi(ll_args[9]);
    int f_h;
    char index = (intptr_t)indice + '1';
    int indc = (intptr_t)indice;
  do
  {
    f_h = po_pf + v_pal; /* posicio hipotetica de la paleta */
    if (f_h != ipo_pf)   /* si pos. hipotetica no coincideix amb pos. actual */
    {
      if (v_pal > 0.0) /* verificar moviment cap avall */
      {
        if (win_quincar(f_h + l_pal - 1, ipo_pf) == ' ') /* si no hi ha obstacle */
        {
          pthread_mutex_lock(&mutex);
          win_escricar(ipo_pf, ipo_pc, ' ', NO_INV); /* esborra primer bloc */
          pthread_mutex_unlock(&mutex);
          po_pf += v_pal;
          ipo_pf = po_pf;                                        /* actualitza posicio */
          pthread_mutex_lock(&mutex);
          win_escricar(ipo_pf + l_pal - 1, ipo_pc, index, INVERS); /* impr. ultim bloc */
          pthread_mutex_unlock(&mutex);
        }
        else{
          /* si hi ha obstacle, canvia el sentit del moviment */
          v_pal = -v_pal;   
        }
      }
      else /* verificar moviment cap amunt */
      {
        if (win_quincar(f_h, ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          pthread_mutex_lock(&mutex);
          win_escricar(ipo_pf + l_pal - 1, ipo_pc, ' ', NO_INV); /* esbo. ultim bloc */
          pthread_mutex_unlock(&mutex);
          po_pf += v_pal;
          ipo_pf = po_pf;                            /* actualitza posicio */
          pthread_mutex_lock(&mutex);
          win_escricar(ipo_pf, ipo_pc, index, INVERS); /* impr. primer bloc */
          pthread_mutex_unlock(&mutex);
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
  } while((fin != 1) && (num_pelotas > 0));

  return ((void *)0);
}