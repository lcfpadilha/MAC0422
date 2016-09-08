/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - arquivo de cabeçalho de timer.c     */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */
/*******************************************************************/

#ifndef TIMER_H
#define TIMER_H

/* Retorna um struct contendo o horario atual.
   Seu atributo sec guarda o numero de segundos desde 1970.
   O atributo nsec guarda o numero de nanosegundos desde
   o ultimo segundo cheio.
*/
struct timespec start_timer ();

/* Retorna o tempo decorrido (em segundos) desde o
   momento fornecido no argumento. 
*/
float check_timer (struct timespec timer);

/* elapsed: recebe um timespec last e um timespec ini e retorna o tem-*/
/*po decorrido entre os dois.                                           */
float elapsed (struct timespec last, struct timespec ini);

#endif