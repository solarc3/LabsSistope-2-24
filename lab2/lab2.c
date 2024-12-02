#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

struct command {
    char **argv;
    int argc;
    int input_from_file;
    int output_to_file;
    struct command *next;
};
int str_cmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*s1 == *s2) ? 0 : (*s1 - *s2);
}
char *str_dup(const char *s) {
    const char *p = s;
    int len = 0;
    while (*p++)
        len++;
    char *dup = malloc(len + 1);
    if (!dup)
        return NULL;
    for (int i = 0; i < len; i++) {
        dup[i] = s[i];
    }
    dup[len] = '\0';
    return dup;
}
struct command *parse_commands(int argc, char *argv[]) {
    struct command *head = NULL;
    struct command *current = NULL;
    int i = 1;

    while (i < argc) {
        struct command *cmd = malloc(sizeof(struct command));
        if (!cmd) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        cmd->argv = NULL;
        cmd->argc = 0;
        cmd->input_from_file = 0;
        cmd->output_to_file = 0;
        cmd->next = NULL;

        int capacity = 4;
        cmd->argv = malloc(capacity * sizeof(char *));
        if (!cmd->argv) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        while (i < argc && !(argv[i][0] == '|' && argv[i][1] == '\0')) {
            if (cmd->argc >= capacity - 1) {
                capacity *= 2;
                cmd->argv = realloc(cmd->argv, capacity * sizeof(char *));
                if (!cmd->argv) {
                    perror("realloc");
                    exit(EXIT_FAILURE);
                }
            }
            cmd->argv[cmd->argc] = argv[i];
            cmd->argc++;

            if ((argv[i][0] == '-') && (argv[i][1] == 'i') &&
                (argv[i][2] == '\0')) {
                cmd->input_from_file = 1;
                i++; // Saltar al siguiente argumento
                if (i < argc) {
                    if (cmd->argc >= capacity - 1) {
                        capacity *= 2;
                        cmd->argv =
                            realloc(cmd->argv, capacity * sizeof(char *));
                        if (!cmd->argv) {
                            perror("realloc");
                            exit(EXIT_FAILURE);
                        }
                    }
                    cmd->argv[cmd->argc] = argv[i];
                    cmd->argc++;
                }
            } else if ((argv[i][0] == '-') && (argv[i][1] == 'o') &&
                       (argv[i][2] == '\0')) {
                cmd->output_to_file = 1;
                i++; // Saltar al siguiente argumento
                if (i < argc) {
                    if (cmd->argc >= capacity - 1) {
                        capacity *= 2;
                        cmd->argv =
                            realloc(cmd->argv, capacity * sizeof(char *));
                        if (!cmd->argv) {
                            perror("realloc");
                            exit(EXIT_FAILURE);
                        }
                    }
                    cmd->argv[cmd->argc] = argv[i];
                    cmd->argc++;
                }
            }

            i++;
        }
        cmd->argv[cmd->argc] = NULL;

        if (head == NULL) {
            head = cmd;
        } else {
            current->next = cmd;
        }
        current = cmd;

        if (i < argc && argv[i][0] == '|' && argv[i][1] == '\0') {
            i++; // Saltar el '|'
        }
    }
    return head;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        write(2, "Uso: ./lab2 [comando1 [args]] [| comando2 [args]] ...\n", 55);
        exit(EXIT_FAILURE);
    }

    struct command *cmd_list = parse_commands(argc, argv);
    struct command *cmd = cmd_list;
    int num_pipes = 0;

    // Contar el número de comandos para crear los pipes necesarios
    struct command *temp = cmd_list;
    while (temp->next != NULL) {
        num_pipes++;
        temp = temp->next;
    }

    int pipes[num_pipes][2];
    int i;
    for (i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    int cmd_index = 0;
    while (cmd != NULL) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            // Proceso hijo

            // Redirección de entrada si es necesario
            if (cmd_index > 0) {
                // No es el primer comando
                if (dup2(pipes[cmd_index - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
            }

            // Redirección de salida si es necesario
            if (cmd->next != NULL) {
                // No es el último comando
                if (dup2(pipes[cmd_index][1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(EXIT_FAILURE);
                }
            }

            // Cerrar todos los pipes en el hijo
            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Ejecutar el comando
            execvp(cmd->argv[0], cmd->argv);
            // Si execvp falla
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        cmd = cmd->next;
        cmd_index++;
    }

    // Cerrar todos los pipes en el padre
    for (i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Esperar a todos los hijos
    for (i = 0; i < cmd_index; i++) {
        wait(NULL);
    }

    // Liberar memoria
    cmd = cmd_list;
    while (cmd != NULL) {
        struct command *next_cmd = cmd->next;
        free(cmd->argv);
        free(cmd);
        cmd = next_cmd;
    }

    return 0;
}
