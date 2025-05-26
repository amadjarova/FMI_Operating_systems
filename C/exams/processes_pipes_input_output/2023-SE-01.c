#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>

void processFile(char* fileName) {
        int md5sum[2];
        if(pipe(md5sum) == -1) {
                err(6, "pipe");
        }

        pid_t pid1 = fork();
        if(pid1 == -1) {
                err(7, "fork");
        }

        if(pid1 == 0) {
                close(md5sum[0]);
                if(dup2(md5sum[1], 1) == -1) {
                        err(8, "dup2");
                }
                close(md5sum[1]);

                if(execlp("md5sum", "md5sum", fileName, (char*)NULL) == -1) {
                        err(9, "exec");
                }
        }


        close(md5sum[1]);

        char hashFileName[4096];
        strcpy(hashFileName, fileName);
        strncat(hashFileName, ".hash", 4096 - strlen(hashFileName) - 1);

        int fd = open(hashFileName, O_CREAT | O_TRUNC | O_WRONLY, 0600);
        if(fd == -1) {
                err(12, "open");
        }

        ssize_t res;
        char ch;
        while((res = read(md5sum[0], &ch, sizeof(ch))) == sizeof(ch)) {
                if(ch == ' ') {
                    break;
                }

                if(write(fd, &ch, sizeof(ch)) == -1) {
                    err(13, "write");
                }
        }

        close(md5sum[0]);
        if(wait(NULL) == -1) {
                err(13, "wait");
        }
        close(fd);
}


int main(int argc, char* argv[]) {
        if(argc != 2) {
                errx(1, "args count");
        }

        int find[2];
        if(pipe(find) == -1) {
                err(2, "pipe");
        }

        pid_t pid = fork();
        if(pid == -1) {
                err(3, "fork");
        }
        if(pid == 0) {
                close(find[0]);
                if(dup2(find[1], 1) == -1) {
                        err(4, "dup2");
                }
                close(find[1]);
                if(execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", "-print0", (char*)NULL) == -1) {
                        err(5, "exec");
                }
        }

        close(find[1]);

        char byte;
        char fileName[4096];
        ssize_t res;
        int index = 0;

        int childrenCount = 1;

        while((res = read(find[0], &byte, sizeof(byte))) == sizeof(byte)) {
                fileName[index++] = byte;
                if(byte == '\0') {
                        index = 0;

                        pid_t worker_pid = fork();
                        if(worker_pid == -1) {
                                err(15, "fork");
                        }
                        if(worker_pid == 0) {
                                close(find[0]);
                                processFile(fileName);
                                exit(0);
                        }
                        childrenCount++;
                }
        }

        if(res == -1) {
                err(18, "read");
        }
        close(find[0]);

        for(int i = 0; i < childrenCount; i++) {
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

        return 0;
}
