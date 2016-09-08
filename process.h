/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Escalonador de processos - arquivo de cabeçalho de process.c   */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */
/*******************************************************************/
#ifndef PROCESS_H
#define PROCESS_H

#define TRUE 1
#define FALSE 0

/* Codigos da funcao printLog.                                     */
#define CPU_EXIT 1
#define CPU_ENTER 2
#define PROC_END 3
#define PROC_ARRIVE 4

/* Struct relacionado aos processos.                               */
typedef struct proc {
    char name[100];    /* Nome do processo.                        */
    float t0;          /* Tempo que o processo chega.              */
    float dt;          /* Tempo real que será gasto na CPU.        */
    float deadline;    /* Tempo máximo que o processo deve ficar exe-
                           cutando.                                */
    float rem;         /* Tempo que falta para o processo terminar.*/
    int line;          /* Linha do Trace.                          */
    int canRun;        /* Boolean que diz se a thread pode rodar ou
                          não.                                     */
    pthread_t id;      /* Identificador da thread criada.          */
    struct proc *next; /* Próximo item da lista.                   */
} PROCESS;

/* Struct relacionado aos parâmetros da função das threads.        */
typedef struct readThreadParams {
    PROCESS *p;          /* Ponteiro para o processo.               */
} PARAMS;

/* head: cabeça da lista encadeada dos processos.                   */
extern PROCESS *head;

/* insertProcess: empilha o processo com nome n, que chegará no tem-*/
/*po t a CPU, gastando dt de tempo na CPU e com deadline de dl. A   */
/*thread relacionada a esse processo deve ser identificada por id.  */
/*A lista deve ser apontada por head->next e o processo será inseri-*/
/*do respeitando a ordenação por t. A linha do trace do processo é  */
/*dada por line.                                                    */
void insertProcess (char *n, float t, float dt, float dl, int line);

/* func: função usada para simulação das threads.                   */
void *func (void *arg); 

/* printLog: Recebbe um inteiro type, uma string p1, um inteiro li- */
/*ne e um float time. Imprime na saída de erro padrão uma mensagem  */
/*que depende do valor de type: Se for 1, imprime que o processo de */
/*nome p1 está saindo da CPU, se for 2, imprime que o processo de   */
/*nome p1 está entrando na CPU, se for 3, imprime que o processo de */
/* nome p1 terminou a execução e que o resultado será impresso na   */
/*linha line e por fim, se type for 4, imprime que o processo p1 que*/
/*estava na linha line do trace chegou no sistema. O tempo é exibido*/
/*em todos os tipos de mensagem.                                    */
void printLog (int type, char *p1, int line, float time);

#endif