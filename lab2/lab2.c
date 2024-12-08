#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

size_t str_len(char *str) {
    size_t size = 0;
    while (str[size] != '\0') {
        size++;
    }
    return size;
}

int str_compare(char *s1, char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 == '\0' && *s2 == '\0';
}

void str_cat(char *dest, char *src) {
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
}
typedef struct CommandNode {
    char **args;
    int argc;
    struct CommandNode *next;
} CommandNode;

CommandNode *create_command_node() {
    CommandNode *node = (CommandNode *)malloc(sizeof(CommandNode));
    if (node == NULL) {
        fprintf(stderr, "Error en malloc para el commandNode");
        exit(1);
    }
    node->args = NULL;
    node->argc = 0;
    node->next = NULL;
    return node;
}

void free_command_list(CommandNode *head) {
    CommandNode *current = head;
    while (current != NULL) {
        CommandNode *temp = current;
        current = current->next;
        free(temp->args);
        free(temp);
    }
}

char *concatenate_args(int argc, char *argv[]) {
    // Calcular el size necesario
    size_t total_len = 0;
    for (int i = 1; i < argc; i++) {
        total_len += str_len(argv[i]) + 1; // +1 para el espacio
    }
    char *result = (char *)malloc(total_len + 1); // +1 para el null terminator
    if (result == NULL) {
        fprintf(stderr, "Error alocando memoria");
        exit(1);
    }

    // concat los argumentos
    result[0] = '\0';
    for (int i = 1; i < argc; i++) {
        str_cat(result, argv[i]);
        if (i < argc - 1) {
            str_cat(result, " ");
        }
    }

    return result;
}

char *str_tok(char *str, char delim, char **saveptr) {
    if (str == NULL) {
        str = *saveptr;
    }
    if (str == NULL || *str == '\0') {
        return NULL;
    }
    while (*str == ' ') {
        str++;
    }
    char *start = str;
    while (*str != '\0' && *str != delim) {
        str++;
    }
    if (*str == delim) {
        *str = '\0';
        *saveptr = str + 1;
    } else {
        *saveptr = NULL;
    }
    char *end = str - 1;
    while (end > start && *end == ' ') {
        *end = '\0';
        end--;
    }
    return start;
}

void split_command(char *cmd, CommandNode *node) {
    node->argc = 0;
    int capacity = 10;
    node->args = malloc(capacity * sizeof(char *));
    char *word = cmd;
    int in_word = 0;
    while (*cmd != '\0') {
        if (*cmd == ' ' || *cmd == '\t') {
            if (in_word) {
                *cmd = '\0';
                if (node->argc >= capacity) {
                    capacity *= 2;
                    node->args = realloc(node->args, capacity * sizeof(char *));
                }
                node->args[node->argc++] = word;
                in_word = 0;
            }
        } else if (!in_word) {
            word = cmd;
            in_word = 1;
        }
        cmd++;
    }

    if (in_word) {
        if (node->argc >= capacity) {
            capacity *= 2;
            node->args = realloc(node->args, capacity * sizeof(char *));
        }
        node->args[node->argc++] = word;
    }

    node->args = realloc(node->args, (node->argc + 1) * sizeof(char *));
    node->args[node->argc] = NULL;
}

CommandNode *parse_command(char *input) {
    char *saveptr;
    char *token;
    CommandNode *head = NULL;
    CommandNode *current = NULL;

    token = str_tok(input, '|', &saveptr);
    while (token != NULL) {
        CommandNode *new_node = create_command_node();
        split_command(token, new_node);
        if (head == NULL) {
            head = new_node;
            current = new_node;
        } else {
            current->next = new_node;
            current = new_node;
        }
        token = str_tok(NULL, '|', &saveptr);
    }

    return head;
}
// debug
void print_commands(CommandNode *head) {
    CommandNode *current = head;
    int cmd_num = 1;
    while (current != NULL) {
        printf("Comando %d:\n", cmd_num++);
        for (int i = 0; i < current->argc; i++) {
            printf("  arg[%d]: '%s'\n", i, current->args[i]);
        }
        printf("\n");
        current = current->next;
    }
}

void execute_commands(CommandNode *head) {
    CommandNode *current = head;
    int prev_pipe[2]; // pipe para ir viendo la comunicacion con la salida
                      // del anterior
    int first_command = 1;
    pid_t last_pid = -1;

    while (current != NULL) {
        int pipe_fd[2];

        // crear otro pipe si no es el ultimo comando
        if (current->next != NULL) {
            if (pipe(pipe_fd) == -1) {
                fprintf(stderr, "Error creando pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error en fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Proceso hijo
            // Si no es el primer comando, configura la entrada desde el
            // pipe anterior
            if (!first_command) {
                if (dup2(prev_pipe[0], STDIN_FILENO) == -1) {
                    fprintf(stderr, "Error en dup2 para stdin");
                    exit(EXIT_FAILURE);
                }
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            // si no es el ultimo comando, configura la salida hacia el
            // siguiente pipe
            if (current->next != NULL) {
                if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
                    fprintf(stderr, "Error en dup2 para stdout");
                    exit(EXIT_FAILURE);
                }
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            // ejecutar el comando
            execvp(current->args[0], current->args);
            fprintf(stderr, "Error en execvp");
            exit(EXIT_FAILURE);
        }

        // si soy el padre, cerramos
        if (!first_command) {
            close(prev_pipe[0]);
            close(prev_pipe[1]);
        }

        // si aun no se termina, se usan los pipes anteriores para ahora
        if (current->next != NULL) {
            prev_pipe[0] = pipe_fd[0];
            prev_pipe[1] = pipe_fd[1];
        }

        // Esperar a que termine el comando anterior antes de continuar
        if (last_pid != -1) {
            int status;
            waitpid(last_pid, &status, 0);
        }

        last_pid = pid;
        first_command = 0;
        current = current->next;
    }

    if (last_pid != -1) {
        int status;
        waitpid(last_pid, &status, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: No se ingresaron argumentos\n");
        exit(EXIT_FAILURE);
    }

    char *command_line = concatenate_args(argc, argv);
    CommandNode *commands = parse_command(command_line);
    execute_commands(commands);
    free_command_list(commands);
    free(command_line);

    return EXIT_SUCCESS;
}
