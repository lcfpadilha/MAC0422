/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - arquivo-cabeçalho do MFBQ.c         */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/
#ifndef MFBQ_H
#define MFBQ_H

/* MFBQ: recebe um FILE out e imprime em out o resultado do escalo-*/
/*namento dos processos da lista head com o algoritmo MFBQ (Multi- */
/*level Feedback Queues). A ideia do algoritmo é tentar executar os*/
/*processos mais rápidos antes, dividindo os processos em 4 classes*/
/*de prioridades, onde cada um possui uma quantidade de quantum.   */
/*Os processos de prioridade mais alta devem ser executados antes  */
/*que os de prioridade mais baixa e, além disso, a classe de priori-*/
/*dade mais alta possui menor quantidade de quantum.               */
/*Quando um processo chega no sistema, ele será inserido na fila de*/
/*prioridade mais alta, e, se ele não finalizar durante o tempo de */
/*quantum dele, diminuimos sua prioridade. Ao chegar no menor nivel*/
/*de prioridade e não tiver finalizado, o processo retorna ao nivel*/
/*mais alto de prioridade.                                         */
void MFBQ (FILE *out, char *d);

#endif