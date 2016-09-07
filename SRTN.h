/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - arquivo-cabeçalho do SRTN.c         */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/
#ifndef SRTN_H
#define SRTN_H

/* SRTN: recebe um FILE out e imprime em out o resultado do escalo-*/
/*namento dos processos da lista head com o algoritmo SRTN.        */
void SRTN (FILE *out, char *d);

#endif