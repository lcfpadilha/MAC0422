/*******************************************************************/
/*  EP1 de MAC0422 - Sistemas Operacionais                         */
/*                                                                 */
/*  Shell Básico                                                   */
/*                                                                 */         
/*  Autores: Gustavo Silva e Leonardo Padilha                      */
/*                                                                 */             
/*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

/*get_args: recebe uma linha de comando e retorna um vetor de strings
//contendo todos os argumentos da linha                             */
char **get_args (char *line);

/*exec_comm: recebe um array de strings e executa o comando, se ele
//estiver na lista de comandos. Retorna 1 se foi executado e 0 caso
//contrário                                                         */
int exec_comm (char **args);

int main (int argc, char **argv) {
    char *line, cwd[10000], path[10000];
    char **args;

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            sprintf(path, "(%s): ", cwd);
            line = readline (path);
        }
        else exit (1);

        if (!line) break;

        add_history (line);
        args = get_args (line);
        
        if (!exec_comm (args)) {
            if ((fork() != 0))
                waitpid (-1, NULL, 0);
            else
                execve (args[0], args, 0);
        }

        free(args);
        free(line);
    }

    exit (0);
}

char **get_args (char *line) {
    char **args;
    int i = 0;
    char *tok;

    args = NULL;
    tok = strtok (line, " ");

    while (tok != NULL) {
        args = realloc (args, sizeof (char *) * ++i);
        args[i-1] = tok;
        tok = strtok (NULL, " ");
    }
    args = realloc (args, sizeof (char *) * ++i);
    args[i-1] = 0;

    return args;
}

int exec_comm (char **args) {
    if (strcmp (args[0], "chmod") == 0) {
        chmod (args[2], strtol(args[1], 0, 8));
        return 1;
    }
    else if ((strcmp (args[0], "id") == 0) && (strcmp (args[1], "-u") == 0)) {
        printf("%d\n", getuid());
        return 1;
    }
    return 0;
}