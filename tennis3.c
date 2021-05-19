/*****************************************************************************/
/*									     */
/*				     Tennis3.c				     */
/*									     */
/*  Programa inicial d'exemple per a les practiques 2 i 3 de ISO.	     */
/*     Es tracta del joc del tennis: es dibuixa un camp de joc rectangular    */
/*     amb una porteria a cada costat, una paleta per l'usuari, una paleta   */
/*     per l'ordinador i una pilota que va rebotant per tot arreu; l'usuari  */
/*     disposa de dues tecles per controlar la seva paleta, mentre que l'or- */
/*     dinador mou la seva automaticament (amunt i avall). Evidentment, es   */
/*     tracta d'intentar col.locar la pilota a la porteria de l'ordinador    */
/*     (porteria de la dreta), abans que l'ordinador aconseguixi col.locar   */
/*     la pilota dins la porteria de l'usuari (porteria de l'esquerra).      */
/*									     */
/*  Arguments del programa:						     */
/*     per controlar la posicio de tots els elements del joc, cal indicar    */
/*     el nom d'un fitxer de text que contindra la seguent informacio:	     */
/*		n_fil n_col m_por l_pal					     */
/*		pil_pf pil_pc pil_vf pil_vc				     */
/*		ipo_pf ipo_pc po_vf					     */
/*									     */
/*     on 'n_fil', 'n_col' son les dimensions del taulell de joc, 'm_por'    */
/*     es la mida de les dues porteries, 'l_pal' es la longitud de les dues  */
/*     paletes; 'pil_pf', 'pil_pc' es la posicio inicial (fila,columna) de   */
/*     la pilota, mentre que 'pil_vf', 'pil_vc' es la velocitat inicial;     */
/*     finalment, 'ipo_pf', 'ipo_pc' indicara la posicio del primer caracter */
/*     de la paleta de l'ordinador, mentre que la seva velocitat vertical    */
/*     ve determinada pel parametre 'po_fv'.				     */
/*									     */
/*     A mes, es podra afegir un segon argument opcional per indicar el      */
/*     retard de moviment de la pilota i la paleta de l'ordinador (en ms);   */
/*     el valor d'aquest parametre per defecte es 100 (1 decima de segon).   */
/*									     */
/*  Compilar i executar:					  	     */
/*     El programa invoca les funcions definides en 'winsuport.o', les       */
/*     quals proporcionen una interficie senzilla per a crear una finestra   */
/*     de text on es poden imprimir caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				     */
/*									     */
/*	   $ gcc tennis0.c winsuport.o -o tennis0 -lcurses		     */
/*	   $ tennis0 fit_param [retard]					     */
/*									     */
/*  Codis de retorn:						  	     */
/*     El programa retorna algun dels seguents codis al SO:		     */
/*	0  ==>  funcionament normal					     */
/*	1  ==>  numero d'arguments incorrecte 				     */
/*	2  ==>  fitxer no accessible					     */
/*	3  ==>  dimensions del taulell incorrectes			     */
/*	4  ==>  parametres de la pilota incorrectes			     */
/*	5  ==>  parametres d'alguna de les paletes incorrectes		     */
/*	6  ==>  no s'ha pogut crear el camp de joc (no pot iniciar CURSES)   */
/*****************************************************************************/

#include <stdio.h> /* incloure definicions de funcions estandard */
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "memoria.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "winsuport2.h" /* incloure definicions de funcions propies */

#define MIN_FIL 7 /* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MIN_PAL 3
#define MIN_VEL -1.0
#define MAX_VEL 1.0

#define MAX_PROCS	10
#define MAX_THREADS 2

/* variables globals */
int n_fil, n_col, m_por; /* dimensions del taulell i porteries */
int l_pal;               /* longitud de les paletes */
float v_pal[MAX_PROCS-1];             /* velocitat de la paleta del programa */

int ipu_pf, ipu_pc; /* posicio del la paleta d'usuari */
int ipo_pf[MAX_PROCS-1]; /* fila del la paleta de l'ordinador */
int ipo_pc[MAX_PROCS-1]; /* columna del la paleta de l'ordinador */
float po_pf[MAX_PROCS-1];        /* pos. vertical de la paleta de l'ordinador, en valor real */

int ipil_pf, ipil_pc; /* posicio de la pilota, en valor enter */
float pil_pf, pil_pc; /* posicio de la pilota, en valor real */
float pil_vf, pil_vc; /* velocitat de la pilota, en valor real*/

int retard; /* valor del retard de moviment, en mil.lisegons */
pid_t tpid[MAX_PROCS];		/* taula d'identificadors dels processos fill */
pthread_t tid[MAX_THREADS]; /*tabla de identificadores de los threads*/
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;	/* crear un sem. Global*/
int *fin;
int cont = -1;
int tecla;
int num_opo = 0;
int *num_pelotas; 
int golesOrdenador = 0, golesUsuario = 0;

/* funcio per realitzar la carrega dels parametres de joc emmagatzemats */
/* dins un fitxer de text, el nom del qual es passa per referencia en   */
/* 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio */
/* enviant un missatge per la sortida d'error i retornant el codi per-	*/
/* tinent al SO (segons comentaris del principi del programa).		*/
void carrega_parametres(const char *nom_fit)
{
  FILE *fit;

  fit = fopen(nom_fit,"rt");		/* intenta obrir fitxer */
  if (fit == NULL)
  {	fprintf(stderr,"No s'ha pogut obrir el fitxer \'%s\'\n",nom_fit);
  	exit(2);
  }

  /*cargar dimensiones del campo*/
  if (!feof(fit)) fscanf(fit,"%d %d %d %d\n",&n_fil,&n_col,&m_por,&l_pal);
  if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) ||
	(n_col < MIN_COL) || (n_col > MAX_COL) || (m_por < 0) ||
	 (m_por > n_fil-3) || (l_pal < MIN_PAL) || (l_pal > n_fil-3))
  {
	fprintf(stderr,"Error: dimensions del camp de joc incorrectes:\n");
	fprintf(stderr,"\t%d =< n_fil (%d) =< %d\n",MIN_FIL,n_fil,MAX_FIL);
	fprintf(stderr,"\t%d =< n_col (%d) =< %d\n",MIN_COL,n_col,MAX_COL);
	fprintf(stderr,"\t0 =< m_por (%d) =< n_fil-3 (%d)\n",m_por,(n_fil-3));
	fprintf(stderr,"\t%d =< l_pal (%d) =< n_fil-3 (%d)\n",MIN_PAL,l_pal,(n_fil-3));
	fclose(fit);
	exit(3);
  }

  /*cargar posicion y velocidad de la pelota*/
  if (!feof(fit)) fscanf(fit,"%d %d %f %f\n",&ipil_pf,&ipil_pc,&pil_vf,&pil_vc);
  if ((ipil_pf < 1) || (ipil_pf > n_fil-3) ||
	(ipil_pc < 1) || (ipil_pc > n_col-2) ||
	(pil_vf < MIN_VEL) || (pil_vf > MAX_VEL) ||
	(pil_vc < MIN_VEL) || (pil_vc > MAX_VEL))
  {
	fprintf(stderr,"Error: parametre pilota incorrectes:\n");
	fprintf(stderr,"\t1 =< ipil_pf (%d) =< n_fil-3 (%d)\n",ipil_pf,(n_fil-3));
	fprintf(stderr,"\t1 =< ipil_pc (%d) =< n_col-2 (%d)\n",ipil_pc,(n_col-2));
	fprintf(stderr,"\t%.1f =< pil_vf (%.1f) =< %.1f\n",MIN_VEL,pil_vf,MAX_VEL);
	fprintf(stderr,"\t%.1f =< pil_vc (%.1f) =< %.1f\n",MIN_VEL,pil_vc,MAX_VEL);
	fclose(fit);
	exit(4);
  }

  /*cargar posiciones y velocidades de las paletas del ordenador*/
  while (!feof(fit))
  {
    fscanf(fit,"%d %d %f\n",&ipo_pf[num_opo],&ipo_pc[num_opo],&v_pal[num_opo]);
    if ((ipo_pf[num_opo] < 1) || (ipo_pf[num_opo]+l_pal > n_fil-2) ||
    (ipo_pc[num_opo] < 5) || (ipo_pc[num_opo] > n_col-2) ||
    (v_pal[num_opo] < MIN_VEL) || (v_pal[num_opo] > MAX_VEL))
      {
    fprintf(stderr,"Error: parametres paleta ordinador incorrectes:\n");
    fprintf(stderr,"\t1 =< ipo_pf (%d) =< n_fil-l_pal-3 (%d)\n",ipo_pf[num_opo],
        (n_fil-l_pal-3));
    fprintf(stderr,"\t5 =< ipo_pc (%d) =< n_col-2 (%d)\n",ipo_pc[num_opo],
        (n_col-2));
    fprintf(stderr,"\t%.1f =< v_pal (%.1f) =< %.1f\n",MIN_VEL,v_pal[num_opo],MAX_VEL);
    fclose(fit);
    exit(5);
      }
      num_opo++;
  }
  
  fclose(fit);			/* fitxer carregat: tot OK! */
}

/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
int inicialitza_joc(void)
{
  int i, i_port, f_port, retwin;
  char strin[51];

  retwin = win_ini(&n_fil,&n_col,'+',INVERS);   /* intenta crear taulell */

  if (retwin < 0)       /* si no pot crear l'entorn de joc amb les curses */
  { fprintf(stderr,"Error en la creacio del taulell de joc:\t");
    switch (retwin)
    {   case -1: fprintf(stderr,"camp de joc ja creat!\n");
                 break;
        case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
 		 break;
        case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
                 break;
        case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
                 break;
     }
     return(retwin);
  }

  i_port = n_fil/2 - m_por/2;	    /* crea els forats de la porteria */
  if (n_fil%2 == 0) i_port--;
  if (i_port == 0) i_port=1;
  f_port = i_port + m_por -1;
  for (i = i_port; i <= f_port; i++)
  {	win_escricar(i,0,' ',NO_INV);
	win_escricar(i,n_col-1,' ',NO_INV);
  }


  ipu_pf = n_fil/2; ipu_pc = 3;		/* inicialitzar pos. paletes */
  if (ipu_pf+l_pal >= n_fil-3) ipu_pf = 1;

  for (i=0; i< l_pal; i++)	    /* dibuixar paleta inicialment */
  {	
    win_escricar(ipu_pf +i, ipu_pc, '0',INVERS);
  }
  
  pil_pf = ipil_pf; pil_pc = ipil_pc;	/* fixar valor real posicio pilota */
  win_escricar(ipil_pf, ipil_pc, '.',INVERS);	/* dibuix inicial pilota */

  sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir.",
		TEC_AMUNT, TEC_AVALL);
  win_escristr(strin);
  return(0);
}

/* funcio per moure la pilota; retorna un valor amb alguna d'aquestes	*/
/* possibilitats:							*/
/*	-1 ==> la pilota no ha sortit del taulell			*/
/*	 0 ==> la pilota ha sortit per la porteria esquerra		*/
/*	>0 ==> la pilota ha sortit per la porteria dreta		*/
void * moure_pilota(void * null)
{
  int f_h, c_h, result;
  char rh,rv,rd;
  result = -1;		/* inicialment suposem que la pilota no surt */
  //printf("genera pelota\n");

  pthread_mutex_lock(&mutex);
  pil_pf = ipil_pf; pil_pc = ipil_pc;	/* fixar valor real posicio pilota */
  win_escricar(ipil_pf, ipil_pc, '.',INVERS);	/* dibuix inicial pilota */
  pthread_mutex_unlock(&mutex);

  do{

    f_h = pil_pf + pil_vf;		/* posicio hipotetica de la pilota */
    c_h = pil_pc + pil_vc;
    
    rh = rv = rd = ' ';
    if ((f_h != ipil_pf) || (c_h != ipil_pc))
    {		/* si posicio hipotetica no coincideix amb la pos. actual */
      if (f_h != ipil_pf)		/* provar rebot vertical */
      {	
        pthread_mutex_lock(&mutex);
        rv = win_quincar(f_h,ipil_pc);	/* veure si hi ha algun obstacle */
        pthread_mutex_unlock(&mutex);
        if (rv != ' ')			/* si no hi ha res */
        {   
          pil_vf = -pil_vf;		/* canvia velocitat vertical */
          f_h = pil_pf+pil_vf;	/* actualitza posicio hipotetica */
        }
      }
      if (c_h != ipil_pc)		/* provar rebot horitzontal */
      {	
        pthread_mutex_lock(&mutex);
        rh = win_quincar(ipil_pf,c_h);	/* veure si hi ha algun obstacle */
        pthread_mutex_unlock(&mutex);
        if (rh != ' ')			/* si no hi ha res */
        {    
          pil_vc = -pil_vc;		/* canvia velocitat horitzontal */
          c_h = pil_pc+pil_vc;	/* actualitza posicio hipotetica */
        }
      }
      if ((f_h != ipil_pf) && (c_h != ipil_pc))	/* provar rebot diagonal */
      {	
        pthread_mutex_lock(&mutex);
        rd = win_quincar(f_h,c_h);
        pthread_mutex_unlock(&mutex);
        if (rd != ' ')				/* si no hi ha obstacle */
        {    
          pil_vf = -pil_vf; pil_vc = -pil_vc;	/* canvia velocitats */
          f_h = pil_pf+pil_vf;
          c_h = pil_pc+pil_vc;		/* actualitza posicio entera */
        }
      }
      pthread_mutex_lock(&mutex);
      if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
      {						/* si no hi ha obstacle */
      pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        win_escricar(ipil_pf,ipil_pc,' ',NO_INV);	/* esborra pilota */
        pthread_mutex_unlock(&mutex);
        pil_pf += pil_vf; pil_pc += pil_vc;
        ipil_pf = f_h; ipil_pc = c_h;		/* actualitza posicio actual */
        if ((ipil_pc > 0) && (ipil_pc <= n_col)){
          /* si no surt */
          pthread_mutex_lock(&mutex);
          win_escricar(ipil_pf,ipil_pc,'.',INVERS); /* imprimeix pilota */
          pthread_mutex_unlock(&mutex);
        }
        else{
          result = ipil_pc;	/* codi de finalitzacio de partida */
        }
      }
    }
    else { 
      pil_pf += pil_vf; 
      pil_pc += pil_vc; 
    }
    win_retard(retard);
    pthread_mutex_lock(&mutex);
    cont = result;
    pthread_mutex_unlock(&mutex);
  }while((result == -1) && (*fin !=1));

  pthread_mutex_lock(&mutex);
  *num_pelotas = *num_pelotas-1;
  pthread_mutex_unlock(&mutex);
  return((void*)0);
}

/* funcio per moure la paleta de l'usuari en funcio de la tecla premuda */
void * mou_paleta_usuari(void * null)
{

  do{
    pthread_mutex_lock(&mutex);
    tecla = win_gettec();
    pthread_mutex_unlock(&mutex); 
    if ((tecla == TEC_AVALL) && (win_quincar(ipu_pf+l_pal,ipu_pc) == ' '))
    {
      pthread_mutex_lock(&mutex);
      win_escricar(ipu_pf,ipu_pc,' ',NO_INV);	   /* esborra primer bloc */
      ipu_pf++;					   /* actualitza posicio */
      win_escricar(ipu_pf+l_pal-1,ipu_pc,'0',INVERS); /* impri. ultim bloc */
      pthread_mutex_unlock(&mutex);
    }
    if ((tecla == TEC_AMUNT) && (win_quincar(ipu_pf-1,ipu_pc) == ' '))
    {
      pthread_mutex_lock(&mutex);
      win_escricar(ipu_pf+l_pal-1,ipu_pc,' ',NO_INV); /* esborra ultim bloc */
      ipu_pf--;					    /* actualitza posicio */
      win_escricar(ipu_pf,ipu_pc,'0',INVERS);	    /* imprimeix primer bloc */
      pthread_mutex_unlock(&mutex);
    }
  }while((tecla != TEC_RETURN) && (*num_pelotas > 0));
  *fin = 1;
  return((void *)0);
}

/*
  rutina para mostrar el marcador
*/
void * marcador(void * null){
  char strin[100];
  int resultado;

  pthread_mutex_lock(&mutex);
  sprintf(strin,"Goles Usuario = %d\tGoles Ordenador = %d\tP%d",
	golesUsuario,golesOrdenador,*num_pelotas);
  win_escristr(strin);
  pthread_mutex_unlock(&mutex);

  do{
    pthread_mutex_lock(&mutex);
    resultado = cont;
    pthread_mutex_unlock(&mutex);
    if(resultado > 0){
      //marca el usuario
      //printf("Contador marcador: %d\n",cont);
      golesUsuario++;
      pthread_mutex_lock(&mutex);

      sprintf(strin,"Goles Usuario = %d\tGoles Ordenador = %d\tP%d",
	    golesUsuario,golesOrdenador,*num_pelotas);
      win_escristr(strin);
      cont = -1;
      ipil_pc = ipo_pc[0]-2;
      ipil_pf = ipo_pf[0]+3;

      pthread_mutex_unlock(&mutex);
    }
    if(resultado == 0){
      //marca el ordenador
      //printf("Contador marcador: %d\n",cont);
      golesOrdenador++;
      pthread_mutex_lock(&mutex);

      sprintf(strin,"Goles Usuario = %d\tGoles Ordenador = %d\tP%d",
	    golesUsuario,golesOrdenador,*num_pelotas);
      win_escristr(strin);
      cont = -1;
      ipil_pc = ipu_pc+1;
      ipil_pf = ipu_pf+2;

      pthread_mutex_unlock(&mutex);
    }
  }while((*fin != 1) && (*num_pelotas > 0));
  
  return ((void *)0);
}


/* programa principal				    */
int main(int n_args, const char *ll_args[])
{
  /* variables locals */
  int t;
  int id_numPelotas, id_fin;
  char a8[20], a9[20];
  if ((n_args != 3) && (n_args != 4))
  {
    fprintf(stderr, "Comanda: tennis2 fit_param num_pelotas [retard]\n");
    exit(1);
  }
  carrega_parametres(ll_args[1]);
  id_numPelotas = ini_mem(sizeof(int));
  
  if (n_args == 4){
    retard = atoi(ll_args[3]);
    *num_pelotas = atoi(ll_args[2]);
  } 
  else if(n_args == 3){
    retard = 100;
    *num_pelotas = atoi(ll_args[2]);
  }else{
    //por defecto
    retard = 100;
  }
  id_fin= ini_mem(sizeof(int));
  *fin = 0;
  sprintf(a8, "%i", id_numPelotas);
  sprintf(a9, "%i", id_fin);
  //printf("Inicializa juego llamada en main: %d\n",inicialitza_joc());
  if (inicialitza_joc() != 0) /* intenta crear el taulell de joc */
    exit(4);                  /* aborta si hi ha algun problema amb taulell */
  //do /********** bucle principal del joc **********/

  pthread_mutex_init(&mutex,NULL); //inicializar semaforo

  int i, n_proc, t_total;
  int n=0;
  char a1[20];
  n_proc=num_opo;
  for ( i = 0; i < n_proc; i++)
  {
	tpid[n] = fork();		/* crea un nou proces */
	if (tpid[n] == (pid_t) 0)		/* branca del fill */
	{
	    sprintf(a1,"%i",(i+1));
	    execlp("./pal_ord3", "pal_ord3", a1, ipo_pf[i], ipo_pc[i], l_pal, v_pal[i], po_pf[i], retard, a8, (char *)0);
	    fprintf(stderr,"error: no puc executar el process fill \'mp_car\'\n");
	    exit(0);
	}
	else if (tpid[n] > 0) n++;		/* branca del pare */
  }

  pthread_create(&tid[0],NULL,mou_paleta_usuari,NULL);
  pthread_create(&tid[1],NULL,marcador,NULL);

  printf("He creat %d processos fill, espero que acabin!\n\n",n);


  int hayPelota = 0;
  while(*num_pelotas > 0){
    if(cont == -1 && hayPelota == 0){
      //generar nueva pelota
      hayPelota=1;  
      pthread_create(&tid[3],NULL,moure_pilota,NULL);
    }else{
      //hay gol
      pthread_join(tid[3],(void **)&t);
      hayPelota=0;
    }

  }
  
  for (i = 0; i < n; i++)
  {
	waitpid(tpid[i],&t,0);	/* espera finalitzacio d'un fill */
	t = t >> 8;			/* normalitza resultat	*/
 	t_total += t;
  }
  
  pthread_join(tid[0],(void **)&t);
  pthread_join(tid[1],(void **)&t);
  elim_mem(id_numPelotas);
  elim_mem(id_fin);

  win_fi();

  pthread_mutex_destroy(&mutex);//destruir semaforo

  if (tecla == TEC_RETURN)
    printf("S'ha aturat el joc amb la tecla RETURN!\n");
  else
  { 
    if(golesOrdenador > golesUsuario){
      printf("Ha ganado el ordenador!!!\n");
    }else if(golesUsuario > golesOrdenador){
      printf("Ha ganado el usuario!!!\n");
    }else{
      printf("EMPATEEEEEEEEEEEEEE!!!!\n");
    }
  }
  return (0);
}
