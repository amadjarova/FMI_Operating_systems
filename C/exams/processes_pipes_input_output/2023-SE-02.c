#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <err.h>
#include <signal.h>
#include <stdlib.h>

pid_t fork_wrapper(void) {
    pid_t pid = fork();

    if(pid == -1) {
        err(26, "fork");
    }

    return pid;
}

ssize_t read_wrapper(int fd, void* buff, size_t size) {
    ssize_t res = read(fd, buff, size);

    if(res == -1) {
        err(26, "read");
    }

    return res;
}

int pipe_wrapper(int pipeFd[2]) {
    int p = pipe(pipeFd);

    if(p == -1) {
        err(26, "pipe");
    }

    return p;
}

int dup2_wrapper(int oldFd, int newFd) {
    int fd = dup2(oldFd, newFd);

    if(fd == -1) {
        err(26, "dup2");
    }

    return fd;
}

int kill_wrapper(pid_t pid, int signal) {
    int k = kill(pid, signal);

    if(k == -1) {
        err(26, "kill");
    }

    return k;
}

int wait_wrapper(void) {
    int status;

    if(wait(&status) == -1) {
        err(26, "wait");
    }
    if(WIFEXITED(status)) {
        if(WEXITSTATUS(status) != 0) {
            errx(26, "Child exited with status %d", WEXITSTATUS(status));
        }
    } else {
        errx(26, "Child was killed");
    }

    return status;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        errx(26, "At least one argument needed");
    }

    pid_t pids[4096];


    int pipeFd[2];
    pipe_wrapper(pipeFd);

    for(int i = 1; i < argc; i++) {
        pid_t pid = fork_wrapper();

        if(pid == 0) {
            close(pipeFd[0]);

            dup2_wrapper(pipeFd[1], 1);
            close(pipeFd[1]);

            execlp(argv[i], argv[i], (const char*)NULL);
            err(26, "exec");
        }

        pids[i - 1] = pid;
    }

    close(pipeFd[1]);
    const char* pattern = "found it!";

    char ch;
    size_t index = 0;

    while(read_wrapper(pipeFd[0], &ch, sizeof(ch)) == sizeof(ch)) {
        if(ch  == pattern[index]) {
            index++;
        } else {
            index = 0;
        }

        if(index == strlen(pattern)) {
            for(int i = 0; i < argc - 1; i++) {
                kill_wrapper(pids[i], SIGTERM);
            }

            for(int i = 0; i < argc -1; i++) {
                if(wait(NULL) == -1) {
                    err(26, "wait");
                }
            }

            exit(0);
        }
    }

    for(int i = 0; i < argc -1; i++) {
        wait_wrapper();
    }

    exit(1);
}
