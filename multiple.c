/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - multiple.c                              */
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
#include "multiple.h"

#define QUANTUM 0.05 
#define NQUEUE 4

/*-------------------------Funções privadas-------------------------*/
void addByPriority (PROCESS *head, PROCESS *new) {
    PROCESS *p, *temp;
    p = head;
    printf("**Chegando processo %s com prioridade %d**#\n", new->name, new->priority);
    /* Vamos procurar o ultimo processo da fila que tenha prioridade
       menor ou igual a do que está entrando.                       */
    while (p->next != NULL)
        p = p->next;
    
    /* Inserimos o novo processo bem ali.                           */
    temp = p->next;   
    p->next = new;
    new->next = temp;
    
    /*                 
    while (p->next != NULL && p->next->priority <= new->priority)
    */
}

/*-------------------------Funções públicas-------------------------*/
void multiple (FILE *out, char *d) {
    struct timespec t_ini, start_quant;
    int traceline = 0, context = 0, live_count = 0;
    float dt, dt_quant;
    PROCESS *p, *temp, *ready, *running;
    PARAMS *args;

    /* Cabeça da lista de processos prontos.  (é uma cabeça vazia)  */
    /*  O primeiro elemento da lista é o que vai estar rodando.     */
    ready = malloc (sizeof (PROCESS));
    ready->next = NULL;
    running = ready->next;

    p = head->next;

    /* Inicio da contagem de tempo.                                 */
    t_ini = start_timer ();
    start_quant = start_timer ();

    /* Simulador irá rodar enquanto houver processo querendo entrar */
    /*no sistema (ponteiro p) ou processo na fila ready             */
    while (p != NULL || ready->next != NULL) {
        /* Calculo do tempo atual. */
        dt = check_timer (t_ini);

        /* Escalonador é chamado caso um novo processo chegue no   */
        /*sistema.                                                 */
        if (p != NULL && p->t0 <= dt) {
            
            /* Imprime a chegada do processo.                       */
            if (d != NULL)
                printLog (PROC_ARRIVE, p->name, p->line, dt);

            /* Guardamos o próximo processo pra uso posterior.      */
            temp = p->next;

            /* Inserimos o processo no fim da parte
               de prioridade máxima da fila.                        */
            addByPriority (ready, p);

            /* Se não havia ninguém antes, criamos uma thread nova
               para o processo que acabou de chegar.                */
            if (running == NULL) {
                running = ready->next;
                args = malloc (sizeof (PARAMS));
                args->p     = running;
                pthread_create (&running->id, NULL, &func, args);
                start_quant = start_timer ();
                if (d != NULL)
                    printLog (CPU_ENTER, p->name, 0, dt);
                running->canRun = TRUE;
            }
            p = temp;
        }

        /* Verifica se o processo já terminou                  */
        if (running != NULL && running->rem < 1e-10) {
            pthread_join (running->id, NULL);

            /* Retiramos ele da CPU, printamos o numero da linha
               dele no trace, e incrementamos esse valor.       */
            if (d != NULL) {
                printLog (CPU_EXIT, running->name, 0, dt);
                printLog (PROC_END, running->name, traceline++, dt);
            }

            /* Imprime na saida o resultado.                */
            fprintf(out, "%s %.3f\n", running->name, check_timer (t_ini));

            if (running->deadline >= check_timer (t_ini)) live_count++;

            /* Avança para o próximo processo na fila ready */
            temp = running;
            ready->next = running->next;
            running = ready->next;

            /* Verificamos se ele existe. Pode ser que nao tenha ninguem*/
            /* na fila, e a CPU fica ociosa.                            */
            if (running != NULL) {
                /* Cria a thread do proximo processo, se necessario.   */
                if (running->id == -1) {
                    args = malloc (sizeof (PARAMS));
                    args->p     = running;
                    pthread_create (&running->id, NULL, &func, args);
                }
                running->canRun = TRUE;
                if (d != NULL)
                    printLog (CPU_ENTER, running->name, 0, dt);
            }

            /* Começamos a contar o novo quantum para o processo.      */
            start_quant = start_timer ();
            free (temp);
        }
        /* Checando tempo de quantum                           */
        dt_quant = check_timer (start_quant);

        /* Faz a troca de contexto se o quantum do processo já acabou. */
        /* Prioridade vai de 0 a 3, por isso o +1.                     */
        if (running != NULL && dt_quant >= (running->priority + 1) * QUANTUM) {
            /* Imprime no log a saída do processo da CPU.        */
            if (d != NULL)
                printLog (CPU_EXIT, running->name, 0, dt); 

            /* Desabilita a thread que está rodando.             */
            running->canRun = FALSE;

            /* Altera a cabeça da lista para o próximo elemento. */
            temp = running;
            ready->next = running->next;

            /* Altera a prioridade do processo.                  */
            temp->priority = (temp->priority + 1) % NQUEUE;

            /* Realoca o processo, agora com menos prioridade, na lista. */
            addByPriority (ready, temp);
            running = ready->next;

            /* Verifica se o processo running já foi iniciado e, */
            /*se nao, inicia uma nova thread pra ele.            */
            if (running->id == -1) {
                args = malloc (sizeof (PARAMS));
                args->p     = running;
                pthread_create (&running->id, NULL, &func, args);
            }
            running->canRun = TRUE;
            start_quant = start_timer ();
            
            context++;

            if (d != NULL)
                printLog (CPU_ENTER, running->name, 0, dt); 
        }
    }
    /* Imprime a quantidade de mudança de contextos.   */
    fprintf (out, "%d\n", context);
    printf ("%d %d\n", context, live_count);
    if (d != NULL)
        fprintf (stderr, "Mudanças de contextos: %d\n", context);

    /* Liberando a fila de ready e a fila dos processos (head). */
    free (ready);
}
