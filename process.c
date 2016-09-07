/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - process.c                           */
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

/* Lista de processos global. */
PROCESS *head;

/*-------------------------Funções públicas-------------------------*/

void printLog (int type, char *p1, int line) {
    if (type == 1)
        fprintf (stderr, "-> Processo %s saindo na CPU.\n", p1);
    else if (type == 2)
        fprintf (stderr, "-> Processo %s entrando da CPU.\n", p1);
    else if (type == 3) {
        fprintf (stderr, "Finalização de execução do processo %s, ",  p1);
        fprintf (stderr, "escrevendo na linha %d\n", line);
    }
    else if (type == 4) {
        fprintf (stderr, "Processo %s da linha %d do trace ", p1, line);
        fprintf (stderr, "chegou no sistema\n");
    }
}

void *func (void *arg) {
    struct timespec t_last, t_now;
    float last, now;
    int x;
    PARAMS *args = (PARAMS *) arg;
    PROCESS *run = args->p;

    if (args->log != NULL) 
        fprintf (stderr, "Processo %s começou a usar a CPU\n", run->name);

    t_last = start_timer();
    while (run->rem > 0.0) {
        x++;
        if (run->canRun) {
            run->rem = run->rem - check_timer(t_last);
        }
        t_last = t_now;
    }

    if (args->log != NULL) 
        fprintf (stderr, "Processo %s terminou de usar a CPU\n", run->name);

    free (arg);
    pthread_exit (NULL);
}

void insertProcess (char *n, float t, float dt, float dl, int line) {
    PROCESS *new, *p, *temp;
    new = malloc (sizeof (PROCESS));
    strcpy (new->name, n);
    new->t0 = t;
    new->dt = dt;
    new->deadline = dl;
    new->line = line;
    new->rem = dt;
    new->canRun = FALSE;
    new->id = -1;

    p = head;
    while (p->next != NULL && p->next->t0 <= new->t0)
        p = p->next;
    temp = p->next;
    p->next = new;
    new->next = temp;
}