/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - MFBQ.c                              */
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
#include "MFBQ.h"

#define QUANTUM 1.0 
#define NQUEUE 4

/* Struct relacionada às filas do escalonamento de multiplas filas. */
typedef struct queue {
    struct timespec t_start; /* Último cálculo de tempo da fila.     */
    float rem_quant;         /* Quantum restante da fila.            */
    PROCESS *first;          /* Primeiro processo da fila.           */
} QUEUE;

/*-------------------------Funções privadas------------------------*/

/* addProcessInQueue: recebe dois ponteiros para processos e insere*/
/*new na lista ligada cujo primeiro elemento é first. Se first for */
/*NULL, retorna new.                                               */
static PROCESS *addProcessInQueue (PROCESS *first, PROCESS *new) {
    PROCESS *p;
    new->next = NULL;

    if (first == NULL)
        return new;

    p = first;
    while (p->next != NULL)
        p = p->next;
    p->next = new;
    return first;
}

/*-------------------------Funções públicas-------------------------*/
void MFBQ (FILE *out, char *d) {
    int i, index, actual = NQUEUE - 1, count = 0, context = 0, live_count = 0;
    struct timespec t_ini, t_now;
    float elaps;
    PROCESS *p, *temp;
    PARAMS *args;
    QUEUE *q[NQUEUE];

    /* Inicialização das queues. */
    for (i = 0; i < NQUEUE; i++) {
        q[i] = malloc (sizeof (QUEUE));
        q[i]->rem_quant = (i + 1) * QUANTUM;
        q[i]->first = NULL;
    }

    /* Inicio da contagem de tempo.                                 */
    t_ini = start_timer ();

    p = head->next;

    /* Simulador irá rodar enquanto houver processo querendo entrar */
    /*no sistema (ponteiro p) ou processo na fila atual (q[actual]) */
    while (p != NULL || q[actual]->first != NULL) {
        /* Calculo do tempo atual. */
        t_now = start_timer ();
        elaps = check_timer (t_ini);

        /* Primeira chamada do escalonador: verificar se o tempo do */
        /*quantum atual já acabou                                   */
        if (q[actual]->first != NULL) {
            /* Mudando o valor do quantum da fila atual             */
            q[actual]->rem_quant -= elapsed(t_now, q[actual]->t_start);

            /* Se o tempo do quantum for menor que zero, fazemos a */
            /*troca de contexto.                                   */
            if (q[actual]->rem_quant <= 0.0) {
                /* Desativmos o processo que está rodando          */
                q[actual]->first->canRun = FALSE;

                /* Mudamos a prioridade do processo que estava ro- */
                /*dando e o inserimos na fila.                     */
                index = (actual + 1) % NQUEUE;
                temp = q[actual]->first;
                q[actual]->first = q[actual]->first->next;
                q[index]->first = addProcessInQueue (q[index]->first, temp);
                
                /* Procuramos o próximo processo, começando na fila*/
                /*atual e, se não existir, vamos para a fila com   */
                /*menor prioridade.                                */
                for (i = actual; i - actual < NQUEUE; i++) {
                    index = i % NQUEUE;
                    if (q[index]->first != NULL) {
                        if (q[index]->first->id == -1) {
                            args = malloc (sizeof (PARAMS));
                            args->p     = q[index]->first;
                            pthread_create (&q[index]->first->id, NULL, &func, args);
                        }
                        q[index]->first->canRun = TRUE;
                        break;
                    }
                }
                /* Alteramos o valor da fila atual e restauramos */
                /*o quantum.                                     */
                actual = i % NQUEUE;
                q[actual]->rem_quant = (actual + 1) * QUANTUM;

                /* Imprimimos a troca de contexto na saída de er-*/
                /*ro padrão.                                     */
                if (temp != q[actual]->first) {
                    if (d != NULL) {
                        if (temp) 
                            printLog (CPU_EXIT, temp->name, 0, elaps);
                        printLog (CPU_ENTER, q[actual]->first->name, 0, elaps);
                    }
                    /* Mudança de contexto só ocorre nesse instante */
                    if (temp) context++;
                }
            }
            q[actual]->t_start = t_now;
        }

        /* Segunda chamada do escalonador: verificar se o próximo   */
        /*processo já pode entrar no sistema                        */
        if (p != NULL && p->t0 <= elaps) {
            /* Imprimir log na saída de erro                        */
            if (d != NULL)
                printLog (PROC_ARRIVE, p->name, p->line, elaps);

            /* Desativando o processo que está rodando atualmente.  */
            if (q[actual]->first != NULL)
                q[actual]->first->canRun = FALSE;

            /* Adicionando o processo na fila de maior prioridade.  */
            temp = p->next;
            q[0]->first = addProcessInQueue (q[0]->first, p);

            /* Se a fila que estava sendo executada for diferente de*/
            /*0, fazemos a preempção pois a fila 0 possui prioridade*/
            /*máxima.                                               */
            if (actual != 0) {
                /* Se existe alguem na fila atual, vai ocorrer troca*/
                /*de contexto.                                      */
                if (q[actual]->first)
                    context++;

                q[0]->rem_quant = QUANTUM;
                args = malloc (sizeof (PARAMS));
                args->p     = q[0]->first;
                q[0]->t_start = t_now;
                pthread_create (&q[0]->first->id, NULL, &func, args);
                if (d != NULL) {
                    if (q[actual]->first)
                        printLog (CPU_EXIT, q[actual]->first->name, 0, elaps);
                    printLog (CPU_ENTER, q[0]->first->name, 0, elaps);
                }
                actual = 0;
            }
            /* Reiniciamos o processo da fila.                     */
            q[actual]->first->canRun = TRUE;
            p = temp;
        }

        /* Terceira chamada do escalonador: verificar se o processo */
        /*que está sendo executado já terminou.                     */
        if (q[actual]->first != NULL && q[actual]->first->rem <= 1e-10) {        
            pthread_join (q[actual]->first->id, NULL);
            temp = q[actual]->first;
            /* Imprimimos na saida de erro.                         */
            if (d != NULL) {
                printLog (CPU_EXIT, temp->name, 0, elaps);
                printLog (PROC_END, temp->name, count++, elaps);
            }
            /* Imprimimos o tempo de execução.                      */
            fprintf(out, "%s %.3f\n", temp->name, elaps);

            if(temp->deadline >= elaps) live_count++;

            /* Avançamos para o próximo processo na fila atual.     */
            q[actual]->first = q[actual]->first->next;

            /* Restauramos o quantum da fila.                       */
            q[actual]->rem_quant = (actual + 1) * QUANTUM;

            /* Procuramos o próximo processo que será executado (ou */
            /*na fila atual ou nas filas de menor prioridade.       */
            for (i = actual; i - actual < NQUEUE; i++) {
                index = i % NQUEUE;
                if (q[index]->first != NULL) {
                    if (q[index]->first->id == -1) {
                        args = malloc (sizeof (PARAMS));
                        args->p     = q[index]->first;
                        pthread_create (&q[index]->first->id, NULL, &func, args);
                    }
                    q[index]->first->canRun = TRUE;
                    break;
                }
            }

            /* Atualizamos o valor da fila atual e imprimimos na sa-*/
            /*ída de erro se houver processo rodando.               */
            actual = i % NQUEUE;
            if (d != NULL && q[actual]->first) 
                printLog (CPU_ENTER, q[actual]->first->name, 0, elaps);

            q[actual]->t_start = t_now;
            free (temp);
        }
    }

    /* Imprime a quantidade de mudança de contextos.   */
    fprintf (out, "%d\n", context);
    printf ("%d %d\n", context, live_count);
    if (d != NULL)
        fprintf (stderr, "Mudanças de contextos: %d\n", context);

    for (i = 0; i < NQUEUE; i++) 
        free (q[i]);
}
