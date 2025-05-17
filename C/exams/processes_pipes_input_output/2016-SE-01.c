#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        errx(1, "args count");
    }

    int catToSort[2];

    if(pipe(catToSort) == -1) {
        err(2, "pipe");
    }

    pid_t pid = fork();

    if(pid == -1) {
        err(3, "fork");
    }
    if(pid == 0) {
        close(catToSort[0]);
        if(dup2(catToSort[1], 1) == -1) {
            err(4, "dup2");
        }
        close(catToSort[1]);
        if(execlp("cat", "cat", argv[1], (char*)NULL) == -1) {
            err(5, "exec");
        }
    }

    close(catToSort[1]);
    if(dup2(catToSort[0], 0) == -1) {
        err(6, "dup2");
    }
    close(catToSort[0]);
    if(execlp("sort", "sort", (char*)NULL) == -1) {
        err(7, "exec");
    }
}
