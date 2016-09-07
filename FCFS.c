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
#include <time.h>
#include "process.h"
#include "FCFS.h"
#include "timer.h"
/*-------------------------Funções privadas------------------------*/

/* arrival: função usada para mostrar quando os processos chegam no*/
/*sistema.                                                         */
static void *arrival (void *args) {
    struct timespec *arg = (struct timespec *) args;
    struct timespec t_ini = *arg;
    PROCESS *p;
    p = head->next;

    while (p != NULL) {

        if (p->t0 <= check_timer(t_ini)) {
            printLog (PROC_ARRIVE, p->name, p->line);
            p = p->next;
        }
    }
    return NULL;
}

/*-------------------------Funções públicas-------------------------*/
void FCFS (FILE *out, char *d) {
    struct timespec t_ini;
    float total;
    int count = 0;
    pthread_t idA = 0;
    PARAMS *args;
    PROCESS *p, *temp;

    t_ini = start_timer();

    /* Rodando thread para verificar as chegadas dos processos. */
    if (d != NULL)
        pthread_create (&idA, NULL, &arrival, (void *) &t_ini);
    
    /* Enquanto houver processo querendo entrar no sistema.     */
    p = head->next;
    while (p != NULL) {

        /* Se o próximo processo chegou no sistema. */
        if (p->t0 < check_timer(t_ini)) {
            /* Deixa o processo ser executado.      */
            p->canRun = TRUE;
            args = malloc (sizeof (PARAMS));
            args->p = p;
            
            if (d != NULL)
                printLog (2, p->name, 0);
            
            /* Cria-se a thread e espera ela terminar. */
            pthread_create (&p->id, NULL, &func, args);

            pthread_join (p->id, NULL);
            
            /* Tempo final.                            */
            total = check_timer(t_ini);
            
            /* Mostra o log na saida de erro padrão.   */
            if (d != NULL) {
                printLog (CPU_EXIT, p->name, 0);
                printLog (PROC_END, p->name, count++);
            }
            /* Imprime o tempo de retorno.             */
            fprintf(out, "%s %.3f (deadline: %.3f)\n", p->name, total, p->deadline);
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