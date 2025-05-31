#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>

int convertToInt(char* str) {
        char* endptr;
        int num = strtol(str, &endptr, 10);
        if(str[0] == '\0' || *endptr != '\0') {
                errx(2, "arg should be a number");
        }
        return num;
}

int dup2_wrapper(int newFd, int oldFd) {
    int fd = dup2(newFd, oldFd);
    if(fd == -1) {
        err(1, "dup2");
    }

    return fd;
}

ssize_t read_wrapper(int fd, void* buff, ssize_t bytes) {
    ssize_t res = read(fd, buff, bytes);
    if(res == -1) {
        err(2, "read");
    }
    return res;
}

ssize_t write_wrapper(int fd, const void* buff, ssize_t bytes) {
    ssize_t res = write(fd, buff, bytes);
    if(res == -1) {
        err(3, "write");
    }
    return res;
}

void pipe_wrapper(int fdPipe[2]) {
    if(pipe(fdPipe) == -1) {
        err(4, "pipe");
    }
}

pid_t fork_wrapper(void) {
    pid_t pid = fork();
    if(pid == -1) {
        err(5, "fork");
    }
    return pid;
}

int main(int argc, char* argv[]) {
        if(argc != 4) {
                errx(1, "args count");
        }

        int n = convertToInt(argv[2]);

        int fdUrandom = open("/dev/urandom", O_RDONLY);
        if(fdUrandom == -1) {
                err(3, "open /dev/urandom");
        }

        int fdNull = open("/dev/null", O_WRONLY);
        if(fdNull == -1) {
                err(4, "open /dev/null");
        }

        int fdOut = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0600);
        if(fdOut == -1) {
            err(5, "open");
        }

        int status = 0;
        char buff[UINT16_MAX];

        for(int i = 0; i < n; i++) {
                int fdPipe[2];
                pipe_wrapper(fdPipe);

                pid_t pid = fork_wrapper();
                if(pid == 0) {
                        close(fdUrandom);
                        close(fdOut);
                        close(fdPipe[1]);

                        dup2_wrapper(fdPipe[0], 0);
                        close(fdPipe[0]);

                        dup2_wrapper(fdNull, 1);
                        dup2_wrapper(fdNull, 2);
                        close(fdNull);

                        execl(argv[1], argv[1], (char*)NULL);
                        err(10, "exec");
                }

                close(fdPipe[0]);

                uint16_t toWrite;
                read_wrapper(fdUrandom, &toWrite, sizeof(toWrite));

                read_wrapper(fdUrandom, &buff, toWrite);

                write_wrapper(fdPipe[1], &buff, toWrite);

                close(fdPipe[1]);

                int result;
                if(wait(&result) == -1) {
                        err(14, "wait");
                }
                if(WIFSIGNALED(result)) {
                        write_wrapper(fdOut, &buff, toWrite);
                        status = 42;
                        break;
                }
        }

        close(fdOut);
        close(fdUrandom);
        close(fdNull);

        return status;
}
