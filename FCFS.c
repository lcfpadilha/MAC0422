/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - FCFS.c                              */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "process.h"
#include "FCFS.h"
/*-------------------------Funções privadas------------------------*/

/* arrival: função usada para mostrar quando os processos chegam no*/
/*sistema.                                                         */
static void *arrival (void *args) {
    struct timespec *arg = (struct timespec *) args;
    struct timespec t_now, t_ini = *arg;
    float ini, now;
    PROCESS *p;
    p = head->next;
    ini = ((float) t_ini.tv_sec) + 1e-9 * ((float) t_ini.tv_nsec);

    while (p != NULL) {
        clock_gettime (CLOCK_MONOTONIC, &t_now);
        now = ((float) t_now.tv_sec) + 1e-9 * ((float) t_now.tv_nsec);

        if (p->t0 <= now - ini) {
            printLog (4, p->name, p->line);
            p = p->next;
        }
    }
    return NULL;
}

/*-------------------------Funções públicas-------------------------*/
void FCFS (FILE *out, char *d) {
    struct timespec t_ini, t_now;
    float now, ini;
    int count = 0;
    pthread_t idA = 0;
    PARAMS *args;
    PROCESS *p, *temp;

    clock_gettime (CLOCK_MONOTONIC, &t_ini);
    ini = ((float) t_ini.tv_sec) + 1e-9 * ((float) t_ini.tv_nsec);

    /* Rodando thread para verificar as chegadas dos processos. */
    if (d != NULL)
        pthread_create (&idA, NULL, &arrival, (void *) &t_ini);
    
    /* Enquanto houver processo querendo entrar no sistema.     */
    p = head->next;
    while (p != NULL) {
        /* Calculo do tempo atual. */
        clock_gettime (CLOCK_MONOTONIC, &t_now);
        now = ((float) t_now.tv_sec) + 1e-9 * ((float) t_now.tv_nsec);

        /* Se o próximo processo chegou no sistema. */
        if (p->t0 < now - ini) {
            /* Deixa o processo ser executado.      */
            p->canRun = TRUE;
            args = malloc (sizeof (PARAMS));
            args->p     = p;
            args->log   = d;
            
            if (d != NULL)
                printLog (2, p->name, 0);
            
            /* Cria-se a thread e espera ela terminar. */
            pthread_create (&p->id, NULL, &func, args);

            pthread_join (p->id, NULL);
            
            /* Tempo final.                            */
            clock_gettime (CLOCK_MONOTONIC, &t_now);
            now = ((float) t_now.tv_sec) + 1e-9 * ((float) t_now.tv_nsec);
            
            /* Mostra o log na saida de erro padrão.   */
            if (d != NULL) {
                printLog (1, p->name, 0);
                printLog (3, p->name, count++);
            }
            /* Imprime o tempo de retorno.             */
            fprintf(out, "%s %.3f (deadline: %.3f)\n", p->name, now - ini, p->deadline);
            temp = p->next;
            free (p);
            p = temp;
        }
    }
    /* Imprime a quantidade de mudança de contextos.   */
    fprintf (out, "0\n");

    if (d) {
        pthread_join (idA, NULL);
        fprintf (stderr, "Mudanças de contextos: 0\n");
    }
}