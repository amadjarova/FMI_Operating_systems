#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
        uint16_t offset;
        uint8_t length;
        uint8_t notUsed;
} idx;

int file_size(int fd) {
        struct stat file_stats;
        if(fstat(fd, &file_stats) == -1) {
                err(5, "stat");
        }

        return file_stats.st_size;
}

int main(int argc, char* argv[]) {
        if(argc != 5) {
                errx(1, "Wrong args count");
        }

        int fd[4];
        if ((fd[0] = open(argv[1], O_RDONLY)) == -1) {
                err(1,"open");
        }

        if((fd[1] = open(argv[2], O_RDONLY)) == -1) {
                err(2,"open");
        }

        if((fd[2] = open(argv[3], O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU)) == -1) {
                err(3, "open");
        }

        if((fd[3] = open(argv[4], O_TRUNC |  O_WRONLY | O_CREAT, S_IRWXU)) == -1) {
                err(4, "open");
        }

        idx idx1;

        if(file_size(fd[1]) % sizeof(idx1) != 0) {
                errx(6, "file size");
        }

        int res;

        while((res = read(fd[1], &idx1, sizeof(idx1))) == sizeof(idx1)) {
                        if(idx1.offset > file_size(fd[0])) {
                            errx(15, "invalid offset");
                        }
                        if(lseek(fd[0], idx1.offset, SEEK_SET) == -1) {
                                err(7, "lseek");
                        }

                        uint8_t searchedString[256];
                        int readRes = read(fd[0], &searchedString, idx1.length);
                        if(readRes == -1) {
                                err(8, "read");
                        } else if(readRes !=  idx1.length) {
                            errx(9, "Unexpected EOF");
                        }

                        if(searchedString[0]>= 'A' && searchedString[0] <= 'Z') {
                                idx idx2;

                                idx2.length = idx1.length;

                                int offset;
                                if((offset = lseek(fd[2], 0, SEEK_CUR)) == -1) {
                                        err(10, "lseek");
                                }
                                idx2.offset = offset;

                                if(write(fd[2], &searchedString, idx2.length) != idx2.length) {
                                        err(9, "write");
                                }

                                if(write(fd[3], &idx2, sizeof(idx2)) != sizeof(idx2)) {
                                        err(11, "write");
                                }
                        }

        }

        if(res != 0) {
                err(12, "read");
        }

        close(fd[0]);
        close(fd[1]);
        close(fd[2]);
        close(fd[3]);
}
