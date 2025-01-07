#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CCS_TOK_BUFSIZE 64
#define CCS_TOK_DELIM " \t\r\n\a"

int ccsChangeDirectory(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "ccs: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("ccs");
        }
    }
    return 1;
}

int ccsHelp(char **args) {
    printf("The following are built-in commands:\n");
    printf(" cd\n");
    printf(" help\n");
    printf(" exit\n");
    return 1;
}

int ccsExit(char **args) {
    return 0;
}

int ccsExecute(char **args) {
    if (args[0] == NULL) {
        return 1; // Empty command
    }

    // Built-in commands
    if (strcmp(args[0], "cd") == 0) {
        return ccsChangeDirectory(args);
    }

    if (strcmp(args[0], "help") == 0) {
        return ccsHelp(args);
    }

    if (strcmp(args[0], "exit") == 0) {
        return ccsExit(args);
    }

    // External commands
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("ccs");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("ccs");
    } else {
        int status;
        waitpid(pid, &status, WUNTRACED);
    }

    return 1;
}

char *ccsReadLine(void){
    size_t bufsize = 1024;
    size_t position = 0;
    char *buffer = malloc(bufsize * sizeof(char));
    int c;

    if (!buffer) {
        fprintf(stderr, "ccs: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();
        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += 1024;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "ccs: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **ccsSplitLine(char *line) {
    int bufsize = CCS_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "ccs: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, CCS_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += CCS_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "ccs: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL,CCS_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void ccsLoop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("$ ");
        line = ccsReadLine();
        args = ccsSplitLine();
        status = ccsExecute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    // Run the command loop
    ccsLoop();

    return EXIT_SUCCESS;
}