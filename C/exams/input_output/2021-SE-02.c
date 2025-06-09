#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>
int file_size(int fd) {
        struct stat st;

        if(fstat(fd, &st) == -1) {
                err(1, "fstat");
        }

        return st.st_size;
}

uint8_t decode(uint8_t byte) {
        uint8_t result = 0;

        for(int i = 0; i < 4; i++) {
                int code = byte & 3;
                byte >>= 2;

                if(code != 1 && code != 2) {
                        errx(5, "bad encoding");
                }

                if(code != 1) {
                        result |= 1 << i;
                }
        }

        return result;
}

int main(int argc, char* argv[]) {
        if(argc != 3) {
                errx(1, "not enough args");
        }

        int in = open(argv[1], O_RDONLY);
        if(in == -1) {
                err(1, "%s", argv[1]);
        }

        if(file_size(in) % 2 != 0) {
                errx(2, "Invalid input file size");
        }

        int out = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0600);
        if(out == -1) {
                err(3, "open %s", argv[2]);
        }

        uint8_t encodedByte[2];
        ssize_t res;

        while((res = read(in, &encodedByte, sizeof(encodedByte))) == sizeof(encodedByte)) {
                uint8_t result = (decode(encodedByte[0]) << 4) | decode(encodedByte[1]);

                if(write(out, &result, sizeof(result)) == -1) {
                        err(4, "write");
                }
        }

        if(res == -1) {
                err(5, "read");
        }

        close(in);
        close(out);

        return 0;
}
