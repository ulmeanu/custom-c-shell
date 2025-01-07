#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* Function Declarations for builtin shell commands: */
int ccsChangeDirectory(char** args);
int ccsHelp(char** args);
int ccsExit(char** args);
int ccsPrintWorkingDirectory(char** args);
int ccsList(char** args);

/* List of builtin commands, followed by their corresponding functions. */
char* builtin_str[] = {
  "cd",
  "help",
  "exit",
  "pwd",
  "ls"
};

int (*builtin_func[]) (char**) = {
  &ccsChangeDirectory,
  &ccsHelp,
  &ccsExit,
  &ccsPrintWorkingDirectory,
  &ccsList
};

/* Function to count the number of built-in commands */
int ccsNumBuiltins() {
    return sizeof(builtin_str) / sizeof(char*);
}

/* Built-in command: change directory */
int ccsChangeDirectory(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "ccs: expected argument to \"cd\"\n");
    }
    else {
        if (chdir(args[1]) != 0) {
            perror("ccs");
        }
    }
    return 1;
}

/* Built-in command: print help */
int ccsHelp(char** args) {
    int i;
    printf("The following are built-in commands:\n");

    for (i = 0; i < ccsNumBuiltins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/* Built-in command: exit */
int ccsExit(char** args) {
    return 0;  // Exit status
}

/* Built-in command: print working directory */
int ccsPrintWorkingDirectory(char** args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    }
    else {
        perror("ccs");
    }
    return 1;
}

/* Built-in command: list directory contents */
int ccsList(char** args) {
    if (execvp("ls", args) == -1) {
        perror("ccs");
    }
    return 1;
}

/* Launch a program and wait for it to terminate */
int ccsLaunch(char** args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("ccs");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        // Error forking
        perror("ccs");
    }
    else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/* Execute shell built-in or launch program */
int ccsExecute(char** args) {
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < ccsNumBuiltins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return ccsLaunch(args);
}

/* Read a line of input from stdin */
char* ccsReadLine(void) {
    size_t bufsize = 1024;
    size_t position = 0;
    char* buffer = malloc(bufsize * sizeof(char));
    int c;

    if (!buffer) {
        fprintf(stderr, "ccs: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();
        if (c == EOF) {
            exit(EXIT_SUCCESS);
        }
        else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else {
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

#define CCS_TOK_BUFSIZE 64
#define CCS_TOK_DELIM " \t\r\n\a"

/* Split a line into tokens */
char** ccsSplitLine(char* line) {
    int bufsize = CCS_TOK_BUFSIZE, position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

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
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "ccs: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, CCS_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/* Main shell loop */
void ccsLoop(void) {
    char* line;
    char** args;
    int status;

    do {
        printf("$ "); 
        line = ccsReadLine();
        args = ccsSplitLine(line);
        status = ccsExecute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char** argv) {
    // Run the command loop
    ccsLoop();

    return EXIT_SUCCESS;
}
