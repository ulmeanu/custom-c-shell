#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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