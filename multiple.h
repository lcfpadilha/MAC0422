/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - arquivo-cabeçalho do multiple.c         */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/
#ifndef MULTIPLE_H
#define MULTIPLE_H

/* MFBQ: recebe um FILE out e imprime em out o resultado do escalo-*/
/*namento dos processos da lista head com o algoritmo de Múltiplas */
/*filas.                                                           */
void multiple (FILE *out, char *d);

#endif