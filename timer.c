/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - timer.c                           */
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
    float timerf, nowf;
    clock_gettime (CLOCK_MONOTONIC, &now);
    timerf = ((float) timer.tv_sec) + 1e-9 * ((float) timer.tv_nsec);
    nowf  = ((float) now.tv_sec) + 1e-9 * ((float) now.tv_nsec);
    return nowf - timerf;
}