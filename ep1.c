/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - ep1.c                              */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "process.h"
#include "FCFS.h"
#include "SRTN.h"
#include "MFBQ.h"

int main (int argc, char **argv) {
    char *fileIn, *fileOut, *d = NULL, name[100];
    int type, i = 0;
    float t0, dt, dl;
    FILE *in, *out;

    if (argc < 4) exit (EXIT_FAILURE);

    /* Inicialização de variaveis do argumento */
    type    = atoi (argv[1]);
    fileIn  = argv[2];
    fileOut = argv[3];
    if (argc > 4) 
        d = argv[4];
    
    /* Inicialização da lista dos processos */
    head = malloc (sizeof (PROCESS));
    head->t0 = -1;
    head->next = NULL;
    
    /* Abrindo o arquivo de entrada e de saida */
    in = fopen (fileIn, "r");
    if (in == NULL) exit (EXIT_FAILURE);
    out = fopen (fileOut, "w");
    if (out == NULL) exit (EXIT_FAILURE);

    /* Lendo as linhas do arquivo e enfileirando os processos */
    while ((fscanf (in, "%f %s %f %f", &t0, name, &dt, &dl)) == 4)
        insertProcess (name, t0, dt, dl, i++);
    
    if (type == 1) 
        FCFS (out, d);
    else if (type == 2)
        SRTN (out, d);
    else if (type == 3)
        MFBQ (out, d);

    free (head);
    fclose (in);
    fclose (out);
    return 0;
}