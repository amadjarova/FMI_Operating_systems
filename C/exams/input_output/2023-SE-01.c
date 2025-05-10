#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>

bool readByte(int fd, uint8_t* byte) {
    int res;
    if((res = read(fd, byte, sizeof(uint8_t))) == -1) {
        err(6, "read");
    }

    return (res != 0);
}

bool isMessageValid(int fd, uint8_t* n) {
    uint8_t sum = 0x55;
    if(!readByte(fd, n)) {
        return false;
    }
    sum ^= *n;

    uint8_t ch;
    for( int i = 3; i < *n; i++) {
        if(!readByte(fd, &ch)) {
            return false;
        }
        sum ^= ch;
    }

    uint8_t checkSum;
    if(!readByte(fd, &checkSum)) {
        return false;
    }

    return (checkSum == sum);
}

void writeMessage(int fdFrom, int fdTo, uint8_t n) {
    uint8_t ch;
    for(uint8_t i = 0; i<n; i++) {
        if(!readByte(fdFrom, &ch)) {
            errx(12, "Unexpected EOF");
        }
        if(write(fdTo, &ch, sizeof(ch)) == -1) {
            err(13, "write");
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        errx(1, "Invalid args count.");
    }

    int fd1;
    int fd2;

    if((fd1 = open(argv[1], O_RDONLY)) == -1) {
        err(2, "open");
    }
    if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
        err(3, "open");
    }

    int res;
    uint8_t ch;
    while((res = read(fd1, &ch, sizeof(ch))) == sizeof(ch)) {
        if(ch == 0x55) {
            uint8_t messageLen;
            int currentPosition = lseek(fd1, 0, SEEK_CUR);
            if(currentPosition == -1) {
                err(5, "lseek");
            }

            if(isMessageValid(fd1, &messageLen)) {
                if(lseek(fd1, currentPosition - 1, SEEK_SET) == -1) {
                    err(6, "lseek");
                }

                writeMessage(fd1, fd2, messageLen);
            }

            if(lseek(fd1, currentPosition, SEEK_SET) == -1) {
                    err(7, "lseek");
            }
        }
    }

    if(res == -1) {
        err(14, "read");
    }
  
    close(fd1);
    close(fd2);
}
