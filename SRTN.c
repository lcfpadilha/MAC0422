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
#include "timer.h"
#include "SRTN.h"

/*-------------------------Funções privadas------------------------  */

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
    struct timespec t_ini;  
    int context = 0, count = 0; 
    PARAMS *args;                      
    PROCESS *p, *q, *temp, *ready, *older;

    /* Cabeça da lista de processos prontos.                         */
    ready = malloc (sizeof (PROCESS));
    ready->next = NULL;
    p = head->next;

    /* Inicio da contagem de tempo.                                  */
    t_ini = start_timer ();

    /* Enquanto não houver processos na fila apontada por ready->next*/ 
    /*e não houverem processos querendo entrar no sistema (p)        */
    while (p != NULL || ready->next != NULL) {

        /* Se houver processo querendo entrar no sistema, gera uma  */
        /*preempção.                                                */
        if (p != NULL && p->t0 <= check_timer (t_ini)) {
            if (d != NULL)
                printLog (PROC_ARRIVAL, p->name, p->line);

            /* Desabilita a thread que está ativa. */
            if (ready->next && ready->next->canRun)
                ready->next->canRun = FALSE;
            
            older = ready->next;
            /* Primeiro processo que ainda não chegou no*/
            /*sistema.                                  */
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
                pthread_create (&ready->next->id, NULL, &func, args);
            }
            /* Libera a thread.                                    */
            ready->next->canRun = TRUE;

            /* Verifica se houve troca de contexto.                */
            if (older != ready->next) {
                if (older) {
                    printLog (CPU_OUT, older->name, 0);
                    context++;
                }
                printLog (CPU_ENTER, ready->next->name, 0);
            }

            /* p agora é o próximo processo que quer entrar        */
            p = q;
        }

        /* Se o próximo processo já terminou de executar. */
        if (ready->next != NULL && ready->next->rem < 1e-10) {
            pthread_join (ready->next->id, NULL);

            if (d != NULL) {
                printLog (CPU_OUT, ready->next->name, 0);
                printLog (PROC_END, ready->next->name, count++);
            }

            /* Imprime na saida o resultado.                */
            fprintf(out, "%s %.3f (deadline: %.3f)\n", ready->next->name, check_timer (t_ini), ready->next->deadline);

            /* Avança para o próximo processo e, se existir,*/
            /*libera-o.                                     */
            temp = ready->next;
            ready->next = ready->next->next;
            if (ready->next != NULL) {
                if (ready->next->id == -1) {
                    args = malloc (sizeof (PARAMS));
                    args->p     = ready->next;
                    pthread_create (&ready->next->id, NULL, &func, args);
                }
                ready->next->canRun = TRUE;
                printLog (CPU_ENTER, ready->next->name, 0);
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