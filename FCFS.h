/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - arquivo de cabeçalho de FCFS.c      */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/
#ifndef FCFS_H
#define FCFS_H

/* FCFS: recebe um FILE out e imprime em out o resultado do escalo-*/
/*namento dos processos da lista head com o algoritmo FCFS.        */
void FCFS (FILE *out, char *d);

#endif