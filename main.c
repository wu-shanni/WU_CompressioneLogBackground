#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 256        //max 256 caratteri

int main() {
    int fd[2];
    pid_t pid;

    char *logs[] = {                    //array di  stringhe
        "INFO Server started\n",
        "WARNING Disk almost full\n",
        "ERROR Connection lost\n",
        "INFO User login\n",
        "ERROR Database unreachable\n",
        "INFO Shutdown request\n"
    };

    int num_logs = sizeof(logs) / sizeof(logs[0]);          //calcola quante stringhe ci sono nell'array

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {         //figlio
        close(fd[1]);        // chiude scrittura

        char buffer[MAX_LINE];
        FILE *stream = fdopen(fd[0], "r");

        while (fgets(buffer, MAX_LINE, stream) != NULL) {           //legge una riga alla volta, continua finchè ci sono dati
            if (strstr(buffer, "ERROR") != NULL) {
                printf("[SEGNALAZIONE] %s", buffer);
            }
        }

        fclose(stream);
        exit(EXIT_SUCCESS);
    }
    else {         //padre
        close(fd[0]); // chiude lettura

        FILE *stream = fdopen(fd[1], "w");

        for (int i = 0; i < num_logs; i++) {
            fputs(logs[i], stream);
        }

        fclose(stream);

        waitpid(pid, NULL, 0);

        printf("Il padre ha inviato %d righe di log.\n", num_logs);
    }


    return 0;
}