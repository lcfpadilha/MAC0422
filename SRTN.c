/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - SRTN.c                              */
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
#include "SRTN.h"

/*-------------------------Funções privadas------------------------*/

/* sortByRemaining: reordena a lista apontada por head baseada no re-*/
/*maining time dos processos.                                        */
static void sortByRemaining (PROCESS *ini, PROCESS *new) {
    PROCESS *p, *temp;

    p = ini;
    while (p->next != NULL && p->next->rem <= new->rem) 
        p = p->next;
    
    temp = p->next;
    p->next = new;
    new->next = temp;
}    

void SRTN (FILE *out, char *d) { 
    struct timespec t_ini, t_now;  
    int context = 0, count = 0; 
    float ini, now;          
    PARAMS *args;                      
    PROCESS *p, *q, *temp, *ready, *older;

    /* Cabeça da lista de processos prontos.                         */
    ready = malloc (sizeof (PROCESS));
    ready->next = NULL;
    p = head->next;

    /* Inicio da contagem de tempo.                                  */
    clock_gettime (CLOCK_MONOTONIC, &t_ini);
    ini = ((float) t_ini.tv_sec) + 1e-9 * ((float) t_ini.tv_nsec);

    /* Enquanto não houver processos na fila apontada por ready->next*/ 
    /*e não houverem processos querendo entrar no sistema (p)        */
    while (p != NULL || ready->next != NULL) {
        /* Calculo do tempo atual. */
        clock_gettime (CLOCK_MONOTONIC, &t_now);
        now = ((float) t_now.tv_sec) + 1e-9 * ((float) t_now.tv_nsec);

        /* Se houver processo querendo entrar no sistema, gera uma  */
        /*preempção.                                                */
        if (p != NULL && p->t0 <= now - ini) {
            if (d != NULL)
                printLog (4, p->name, p->line);

            /* Desabilita a thread que está ativa. */
            if (ready->next && ready->next->canRun)
                ready->next->canRun = FALSE;
            
            older = ready->next;
            q = p->next;

            /* Ordena a fila apontada por ready baseado no remaining */
            /*time, inserindo os processos que chegaram nesse instan-*/
            /*te.                                                    */
            sortByRemaining (ready, p);

            while (q != NULL && p->t0 == q->t0) {
                temp = q->next;
                sortByRemaining (ready, q);
                q = temp;
            }

            /* Cria uma thread nova se o próximo processo ainda não*/
            /*começou a ser executado.                             */
            if (ready->next->id == -1) {
                args = malloc (sizeof (PARAMS));
                args->p     = ready->next;
                args->log   = d;
                pthread_create (&ready->next->id, NULL, &func, args);
            }
            /* Libera a thread.                                    */
            ready->next->canRun = TRUE;

            /* Verifica se houve troca de contexto.                */
            if (older != ready->next) {
                if (older) {
                    printLog (1, older->name, 0);
                    context++;
                }
                printLog (2, ready->next->name, 0);
            }

            p = q;
        }

        /* Se o próximo processo já terminou de executar. */
        if (ready->next != NULL && ready->next->rem < 1e-10) {
            pthread_join (ready->next->id, NULL);

            if (d != NULL) {
                fprintf (stderr, "Finalização de execução do processo %s, ", ready->next->name);
                fprintf (stderr, "escrevendo na linha %d\n", count++);
            }

            /* Imprime na saida o resultado.                */
            fprintf(out, "%s %.3f (deadline: %.3f)\n", ready->next->name, now - ini, ready->next->deadline);

            /* Avança para o próximo processo e, se existir,*/
            /*libera-o.                                     */
            temp = ready->next;
            ready->next = ready->next->next;
            if (ready->next != NULL) {
                if (ready->next->id == -1) {
                    args = malloc (sizeof (PARAMS));
                    args->p     = ready->next;
                    args->log   = d;
                    pthread_create (&ready->next->id, NULL, &func, args);
                }
                ready->next->canRun = TRUE;
                printLog (2, ready->next->name, 0);
            }
            free (temp);
        }
    }    

    /* Imprime a quantidade de mudança de contextos.   */
    fprintf (out, "%d\n", context);
    if (d != NULL)
        fprintf (stderr, "Mudanças de contextos: %d\n", context);

    /* Liberando a fila de ready e a fila dos processos (head). */
    free (ready);
}