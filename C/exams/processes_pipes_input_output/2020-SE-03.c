#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
        char name[8];
        uint32_t offset;
        uint32_t length;
} idx;

int open_for_reading(char* file) {
        int fd = open(file, O_RDONLY);
        if(fd == -1) {
                err(1, "open");
        }
        return fd;
}

size_t write_wrapper(int fd, void* buff, size_t size) {
        int bytes;

        if((bytes = write(fd, buff, size)) == -1) {
                err(5, "write");
        }

        return bytes;
}

void pipe_wrapper(int pipeFd[2]) {
        if(pipe(pipeFd) == -1) {
                err(6, "pipe");
        }
}

pid_t fork_wrapper(void) {
        pid_t pid = fork();
        if(pid == -1) {
                err(7, "fork");
        }
        return pid;
}

void wait_wrapper(void) {
        int status;
        if(wait(&status) == -1) {
                err(12, "wait");
        }

        if(WIFEXITED(status)) {
                if(WEXITSTATUS(status) != 0) {
                        errx(15, "Child exited with status %d", status);
                }
        } else {
                errx(16, "Child was killed");
        }
}

int file_size(int fd) {
        struct stat st;
        if(fstat(fd, &st) == -1) {
                err(3, "fstat");
        }
        return st.st_size;
}

void  xorElements(idx index, int pipeFd) {
        int fd = open_for_reading(index.name);

        if((uint32_t)file_size(fd) < (index.offset + index.length)*sizeof(uint16_t)) {
                errx(4, "Invalid file size");
        }

        if(lseek(fd, index.offset*sizeof(uint16_t), SEEK_SET) == -1) {
                err(5, "lseek");
        }

        uint16_t num;
        uint16_t xorRes = 0;
        int res;
        uint32_t numElements = 0;
        while((res = read(fd, &num, sizeof(num))) == sizeof(num) && numElements < index.length) {
                xorRes^=num;
                numElements++;
        }

        if(res == -1) {
                err(6, "read 1");
        }

        write_wrapper(pipeFd, &xorRes, sizeof(xorRes));
        close(fd);
}

int main(int argc, char* argv[]) {
        if(argc != 2) {
                errx(2, "args count");
        }
        int fd = open_for_reading(argv[1]);

        if(file_size(fd) % sizeof(idx) != 0 || file_size(fd) / sizeof(idx) > 8) {
                errx(3, "Invalid file size");
        }

        int res;
        idx index;
        uint16_t finalXorRes = 0;

        int childrenCount = 0;
        int pipeFd[2];
        pipe_wrapper(pipeFd);

        while((res = read(fd, &index, sizeof(idx))) == sizeof(idx)) {
                childrenCount++;

                pid_t pid = fork_wrapper();
                if(pid == 0) {
                        close(fd);
                        close(pipeFd[0]);
                        xorElements(index, pipeFd[1]);
                        close(pipeFd[1]);
                        exit(0);
                }
        }

        if(res == -1) {
                err(12, "read ");
        }
        close(pipeFd[1]);
        uint16_t childXor;
        int res2;
        while((res2 = read(pipeFd[0], &childXor, sizeof(childXor))) == sizeof(childXor)) {
                finalXorRes ^= childXor;
        }

        if(res2 == -1) {
            err(13, "read");
        }
        close(pipeFd[0]);

        for(int i = 0; i < childrenCount; i++) {
                wait_wrapper();
        }

        char buff[256];
        snprintf(buff, sizeof(buff), "result: %.4X\n", finalXorRes);
        write_wrapper(1, buff, strlen(buff));
        close(fd);
        return 0;
}
