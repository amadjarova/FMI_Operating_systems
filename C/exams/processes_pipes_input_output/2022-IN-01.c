#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <err.h>
#include <stdlib.h>

int convertToInt(char* str) {
        char* endPtr;
        int num = strtol(str, &endPtr, 10);
        if(str[0] == '\0' || *endPtr != '\0') {
                errx(2, "argument should be a number");
        }
        return num;
}

void handleParent(int N,int D, int fdFromChild, int fdToChild) {
        for( int i =0; i< N; i++) {
                if(write(1, "DING", 4) == -1) {
                        err(10, "write");
                }

                int dummy1 = 0;
                if(write(fdToChild, &dummy1, sizeof(dummy1)) == -1) {
                        err(11, "write");
                }

                int dummy2;
                if(read(fdFromChild, &dummy2, sizeof(dummy2)) == -1) {
                        err(12, "read");
                }

                sleep(D);
        }
}


void handleChild(int fdFromParent, int fdToParent) {
        int res;
        int dummy1;
        while((res = read(fdFromParent, &dummy1, sizeof(dummy1))) == sizeof(dummy1)) {
                if(write(1, "DONG\n", 5) == -1) {
                        err(13, "write");
                }

                int dummy2 = 0;
                if(write(fdToParent, &dummy2, sizeof(dummy2)) == -1) {
                        err(14, "write");
                }
        }

        if(res == -1) {
                err(15, "read");
        }
}

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(1, "args count");
        }

        int N = convertToInt(argv[1]);
        if(N < 0) {
                errx(3, "arg should be a positive number");
        }

        int D = convertToInt(argv[2]);
        if(D < 0) {
                errx(4, "arg should be a positive number");
        }

        int parentToChild[2];
        int childToParent[2];

        if(pipe(parentToChild) == -1) {
                err(5, "pipe");
        }

        if(pipe(childToParent) == -1) {
                err(6, "pipe");
        }

        pid_t pid = fork();

        if(pid == -1) {
                err(7, "fork");
        }
        if(pid == 0) {
                close(parentToChild[1]);
                close(childToParent[0]);

                handleChild(parentToChild[0], childToParent[1]);

                close(parentToChild[0]);
                close(childToParent[1]);
                exit(0);
        }
        close(parentToChild[0]);
        close(childToParent[1]);

        handleParent(N, D, childToParent[0], parentToChild[1]);

        close(parentToChild[1]);
        close(childToParent[0]);

        int status;
        if(wait(&status) == -1) {
                err(13, "wait");
        }

        if(WIFEXITED(status)) {
                if(WEXITSTATUS(status) != 0) {
                        errx(14, "Child exited with status %d", WEXITSTATUS(status));
                }
        } else {
                errx(15, "child was killed");
        }

        return 0;
}
