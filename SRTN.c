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
    /*Vamos procurar o ultimo processo da lista que tenha menos rem
      que o processo que quer entrar */
    while (p->next != NULL && p->next->rem <= new->rem) 
        p = p->next;
    
    /* Adicionamos ele logo em seguida */
    temp = p->next;
    p->next = new;
    new->next = temp;
}    

void SRTN (FILE *out, char *d) { 
    struct timespec t_ini;  
    int context = 0, traceline = 0; 
    float dt;
    PARAMS *args;                      
    PROCESS *p, *temp, *ready, *old, *running;

    /* Cabeça da lista de processos prontos.  (é uma cabeça vazia)   
       O primeiro elemento da lista é o que vai estar rodando.      */
    ready = malloc (sizeof (PROCESS));
    ready->next = NULL;
    running = ready->next;
    p = head->next;

    /* Inicio da contagem de tempo.                                  */
    t_ini = start_timer ();

    /* Enquanto não houver processos na fila apontada por ready->next*/ 
    /*e não houverem processos querendo entrar no sistema (p)        */
    while (p != NULL || ready->next != NULL) {

        /* Se tiver um processo p querendo entrar neste momento */
        dt = check_timer(t_ini);
        if (p != NULL && p->t0 <= dt) {

            if (d != NULL)
                printLog (PROC_ARRIVE, p->name, p->line);

            /* Desabilita a thread ativa. */
            if (running && running->canRun)
                running->canRun = FALSE;
            
            old = running;

            /* guardamos a referencia ao proximo da fila
               e então inserimos p e todos os outros  que entraram
               junto na fila de execução ordenados pelo seu tempo
               remanescente.                                       */
            while (p != NULL && p->t0 <= dt) {
                temp = p->next;
                sortByRemaining (ready, p);
                p = temp;
            }
            /* Ao fim desse while, p já está apontando para o proximo
               processo que vai entrar, daqui a alguns momentos    */


            /* Depois dessa reorganizaçao, o primeiro da fila ready
               é quem vai ser executado.                           */
            running = ready->next;


            /* Cria uma thread nova se o próximo processo ainda não*/
            /*começou a ser executado.                             */
            if (running->id == -1) {
                args = malloc (sizeof (PARAMS));
                args->p     = running;
                pthread_create (&running->id, NULL, &func, args);
            }
            /* Libera a thread.                                    */
            running->canRun = TRUE;

            /* Verifica se houve troca de contexto.                */
            if (old != running) {
                if (old) {
                    printLog (CPU_EXIT, old->name, 0);
                    context++;
                }
                printLog (CPU_ENTER, ready->next->name, 0);
            }
        }

        /* Se o processo que está rodando terminou de executar. */
        if (running != NULL && running->rem < 1e-10) {
            pthread_join (running->id, NULL);

            /* Retiramos ele da CPU, printamos o numero da linha
               dele no trace, e incrementamos esse valor.       */
            if (d != NULL) {
                printLog (CPU_EXIT, running->name, 0);
                printLog (PROC_END, running->name, traceline++);
            }

            /* Imprime na saida o resultado.                */
            fprintf(out, "%s %.3f (deadline: %.3f)\n", ready->next->name, check_timer (t_ini), ready->next->deadline);

            /* Avança para o próximo processo na fila ready */
            temp = running;
            running = running->next;

            /* Verificamos se ele existe. Pode ser que nao tenha ninguem
               na fila, e a CPU fica ociosa.                           */
            if (running != NULL) {
                /* Cria a thread do proximo processo, se necessario.   */
                if (running->id == -1) {
                    args = malloc (sizeof (PARAMS));
                    args->p     = running;
                    pthread_create (&running->id, NULL, &func, args);
                }
                running->canRun = TRUE;
                printLog (CPU_ENTER, running->name, 0);
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