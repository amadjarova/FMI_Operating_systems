#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
        uint16_t offset;
        uint8_t original;
        uint8_t new;
} file_struct;

int file_size(int fd) {
        struct stat file_stats;
        if(fstat(fd, &file_stats) == -1) {
                err(2, "fstat");
        }

        return file_stats.st_size;
}

int main(int argc, char* argv[]) {
        if (argc != 4) {
                errx(1, "Wrong arguments count");
        }

        int fd1;
        int fd2;
        int fd3;

        if((fd1 = open(argv[1], O_RDONLY)) == -1) {
                err(3, "open");
        }
        if((fd2 = open(argv[2], O_RDONLY)) == -1) {
                err(4, "open");
        }
        if((fd3 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) == -1) {
                err(5, "open");
        }

        if(file_size(fd1) != file_size(fd2)) {
                errx(6, "files size");
        }

        uint8_t ch1;
        uint8_t ch2;

        int res;

        while((res = read(fd1, &ch1, sizeof(ch1))) == sizeof(ch1)) {
                if(read(fd2, &ch2, sizeof(ch2)) != sizeof(ch2)) {
                    err(7,"read");
                }

                if(ch1 != ch2) {
                        file_struct tuple;

                        int offset;
                        if((offset = lseek(fd1, 0, SEEK_CUR)) == -1) {
                                err(7, "lseek");
                        }
                        tuple.offset = offset - 1;
                        tuple.original = ch1;
                        tuple.new = ch2;

                        if(write(fd3, &tuple, sizeof(tuple)) != sizeof(tuple)) {
                                err(8, "write");
                        }

                }
        }

        if(res != 0) {
                err(9,"read");
        }

        close(fd1);
        close(fd2);
        close(fd3);
}
