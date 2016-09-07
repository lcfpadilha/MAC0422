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
#include "MFBQ.h"

#define QUANTUM 1.0 
#define NQUEUE 4

/* Struct relacionada às filas do escalonamento de multiplas filas. */
typedef struct queue {
    float t_start;          /* Último cálculo de tempo da fila.     */
    float rem_quant;        /* Quantum restante da fila.            */
    PROCESS *first;         /* Primeiro processo da fila.           */
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
    int i, index, actual = NQUEUE - 1, count = 0, context = 0;
    struct timespec t_ini, t_now;
    float ini, now;
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
    clock_gettime (CLOCK_MONOTONIC, &t_ini);
    ini = ((float) t_ini.tv_sec) + 1e-9 * ((float) t_ini.tv_nsec);

    p = head->next;

    /* Simulador irá rodar enquanto houver processo querendo entrar */
    /*no sistema (ponteiro p) ou processo na fila atual (q[actual]) */
    while (p != NULL || q[actual]->first != NULL) {
        /* Calculo do tempo atual. */
        clock_gettime (CLOCK_MONOTONIC, &t_now);
        now = ((float) t_now.tv_sec) + 1e-9 * ((float) t_now.tv_nsec);

        /* Primeira chamada do escalonador: verificar se o tempo do */
        /*quantum atual já acabou                                   */
        if (q[actual]->first != NULL) {
            /* Mudando o valor do quantum da fila atual             */
            q[actual]->rem_quant -= now - q[actual]->t_start;

            /* Se o tempo do quantum for menor que zero, fazemos a */
            /*troca de contexto.                                   */
            if (q[actual]->rem_quant < 0.0) {
                /* Desativmos o processo que está rodando          */
                q[actual]->first->canRun = FALSE;
                index = (actual + 1) % NQUEUE;

                /* Mudamos a prioridade do processo que estava ro- */
                /*dando.                                           */
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
                            args->log   = d;
                            pthread_create (&q[index]->first->id, NULL, &func, args);
                        }
                        q[index]->first->canRun = TRUE;
                        break;
                    }
                }
                /* Alteramos o valor da fila atual.              */
                actual = i % NQUEUE;
                q[actual]->rem_quant = (actual + 1) * QUANTUM;

                /* Imprimimos a troca de contexto na saída de er-*/
                /*ro padrão.                                     */
                if (d != NULL && temp != q[actual]->first) {
                    printLog (1, temp->name, 0);
                    printLog (2, q[actual]->first->name, 0);
                    context++;
                }
            }
            /* Ultima contagem de tempo da fila.                    */
            q[actual]->t_start = now;
        }

        /* Segunda chamada do escalonador: verificar se o próximo   */
        /*processo já pode entrar no sistema                        */
        if (p != NULL && p->t0 <= now - ini) {
            /* Imprimir log na saída de erro                        */
            if (d != NULL)
                printLog (4, p->name, p->line);
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
                args->log   = d;
                q[0]->t_start = now;
                pthread_create (&q[0]->first->id, NULL, &func, args);
                if (d != NULL) {
                    printLog (1, q[actual]->first->name, 0);
                    printLog (2, q[0]->first->name, 0);
                }
                actual = 0;
            }
            /* Reiniciamos o processo da fila.                     */
            q[actual]->first->canRun = TRUE;
            p = temp;
        }

        /* Terceira chamada do escalonador: verificar se o processo */
        /*que está sendo executado já terminou.                     */
        if (q[actual]->first != NULL && q[actual]->first->rem <= 0.0) {        
            pthread_join (q[actual]->first->id, NULL);
            temp = q[actual]->first;
            /* Imprimimos na saida de erro.                         */
            if (d != NULL) {
                printLog (1, temp->name, 0);
                printLog (3, temp->name, count++);
            }
            /* Imprimimos o tempo de execução.                      */
            fprintf(out, "%s %.3f (deadline: %.3f)\n", temp->name, now - ini, temp->deadline);

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
                        args->log   = d;
                        pthread_create (&q[index]->first->id, NULL, &func, args);
                    }
                    q[index]->first->canRun = TRUE;
                    break;
                }
            }

            /* Atualizamos o valor da fila atual e imprimimos na sa-*/
            /*ída de erro.                                          */
            actual = i % NQUEUE;
            if (d != NULL && q[actual]->first) 
                printLog (2, q[actual]->first->name, 0);

            q[actual]->t_start = now;
            free (temp);
        }
    }

    /* Imprime a quantidade de mudança de contextos.   */
    fprintf (out, "%d\n", context);
    if (d != NULL)
        fprintf (stderr, "Mudanças de contextos: %d\n", context);

    for (i = 0; i < NQUEUE; i++) 
        free (q[i]);
}
