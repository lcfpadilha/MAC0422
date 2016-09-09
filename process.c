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
#include <time.h>
#include <sched.h>
#include "process.h"
#include "timer.h"

/* Lista de processos global. */
PROCESS *head;

/*-------------------------Funções públicas-------------------------*/

void printLog (int type, char *p1, int line, float time) {
    fprintf (stderr, "[%f] ", time);
    unsigned int cpu;
    cpu = sched_getcpu();
    if (type == CPU_EXIT)
        fprintf (stderr, "<- Processo %s saindo da CPU %d.\n", p1, cpu);
    else if (type == CPU_ENTER)
        fprintf (stderr, "-> Processo %s entrando da CPU %d.\n", p1, cpu);
    else if (type == PROC_END) {
        fprintf (stderr, "Finalização de execução do processo %s, ",  p1);
        fprintf (stderr, "escrevendo na linha %d\n", line);
    }
    else if (type == PROC_ARRIVE) {
        fprintf (stderr, "Processo %s da linha %d do trace ", p1, line);
        fprintf (stderr, "chegou no sistema\n");
    }

}

void *func (void *arg) {
    struct timespec t_now, t_ini;
    int x = 0;
    PARAMS *args = (PARAMS *) arg;
    PROCESS *run = args->p;

    t_ini = start_timer ();
    while (run->rem > 0.0) {
        t_now = start_timer ();
        if (run->canRun) {
            x++;
            run->rem -= elapsed (t_now, t_ini);
        }
        t_ini = t_now;
    }

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