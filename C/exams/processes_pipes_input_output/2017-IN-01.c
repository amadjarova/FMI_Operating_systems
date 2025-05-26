#include <err.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

void pipe_wrapper(int fd[2]) {
        if(pipe(fd) == -1) {
                err(1, "pipe");
        }
}

pid_t fork_wrapper(void) {
        pid_t pid = fork();
        if(pid == -1) {
                err(2, "fork");
        }
        return pid;
}

int dup2_wrapper(int oldFd, int newFd) {
        int fd;

        if((fd = dup2(oldFd, newFd)) == -1) {
                err(3, "fork");
        }

        return fd;
}

void wait_wrapper(void) {
        int status;
        if(wait(&status) == -1) {
                err(14, "wait");
        }

        if(WIFEXITED(status)) {
                if(WEXITSTATUS(status) != 0) {
                          errx(15, "child exited with status %d", WEXITSTATUS(status));
                }
        } else {
                errx(16, "child was killed");
        }
}

int main(void) {
        int cutToSort[2];
        pipe_wrapper(cutToSort);
        pid_t pr1 = fork_wrapper();
        if(pr1 == 0) {
                //child process
                close(cutToSort[0]);
                dup2_wrapper(cutToSort[1], 1);
                close(cutToSort[1]);

                if(execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL) == -1) {
                        err(4, "exec");
                }
        }
        close(cutToSort[1]);

        int sortToUniq[2];
        pipe_wrapper(sortToUniq);
        pid_t pr2 = fork_wrapper();

        if(pr2 == 0) {
                close(sortToUniq[0]);

                dup2_wrapper(sortToUniq[1], 1);
                close(sortToUniq[1]);
                dup2_wrapper(cutToSort[0], 0);
                close(cutToSort[0]);

                if(execlp("sort", "sort", (char*)NULL) == -1) {
                        err(5, "exec");
                }
        }

        close(cutToSort[0]);
        close(sortToUniq[1]);

        int uniqToSort[2];
        pipe_wrapper(uniqToSort);
        pid_t pr3 = fork_wrapper();

        if(pr3 == 0) {
                close(uniqToSort[0]);

                dup2_wrapper(uniqToSort[1], 1);
                close(uniqToSort[1]);
                dup2_wrapper(sortToUniq[0], 0);
                close(sortToUniq[0]);

                if(execlp("uniq", "uniq", "-c", (char*)NULL) == -1) {
                        err(6, "exec");
                }
        }

        close(sortToUniq[0]);
        close(uniqToSort[1]);

        pid_t pr4 = fork_wrapper();
        if(pr4 == 0) {
                dup2_wrapper(uniqToSort[0], 0);
                close(uniqToSort[0]);

                if(execlp("sort", "sort", "-n", (char*)NULL) == -1) {
                        err(7, "exec");
                }
        }

        close(uniqToSort[0]);

        for(int i = 0; i<4; i++) {
                wait_wrapper();
        }

        return 0;
}
