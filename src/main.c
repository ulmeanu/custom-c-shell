#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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