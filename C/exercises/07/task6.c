#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
        if(argc != 2) {
                errx(1, "One argument needed");
        }

        pid_t pid = fork();

        if(pid == -1) {
                err(2, "fork");
        }

        if(pid == 0) {
                //child process
                if(execlp(argv[1], argv[1], (char*)NULL) == -1) {
                        err(3, "exec");
                }
        }

        //parent process
        int status;
        if(wait(&status) == -1) {
                err(4, "wait");
        }
        if(WIFEXITED(status)) {
                if(WEXITSTATUS(status) != 0) {
                        errx(1, "%s exited with status code %d", argv[1], WEXITSTATUS(status));
                }
        } else {
                errx(2, "%s was killed", argv[1]);
        }

        char buff[256];
        snprintf(buff, sizeof(buff), "%s executed successfuly\n", argv[1]);
        if(write(1, buff, strlen(buff)) == -1) {
                err(3, "write");
        }

        return 0;
}
