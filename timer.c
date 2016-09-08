/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - timer.c                             */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */
/*******************************************************************/
#include <time.h>
#include "timer.h"

struct timespec start_timer () {
    struct timespec clock;
    clock_gettime (CLOCK_MONOTONIC, &clock);
    return clock;
}

float check_timer (struct timespec timer) {
    struct timespec now;
    clock_gettime (CLOCK_MONOTONIC, &now);
    return elapsed (now, timer);
}

float elapsed (struct timespec last, struct timespec ini) {
    float lastf, inif;
    lastf = ((float) last.tv_sec) + 1e-9 * ((float) last.tv_nsec);
    inif  = ((float) ini.tv_sec) + 1e-9 * ((float) ini.tv_nsec);

    return lastf - inif;
}