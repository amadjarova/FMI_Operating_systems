#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>

int convertToInt(char* str) {
    char* endPtr;
    int num = strtol(str, &endPtr, 10);

    if(str[0] == '\0' || *endPtr != '\0') {
        errx(2, "Argument should be a number: %s", str);
    }

    return num;
}

void cOption(int from, int to) {
    int res;
    int current = 1;
    uint8_t ch;
    while((res = read(0, &ch, sizeof(ch))) == sizeof(ch)) {
        if(ch == '\n') {
            if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                err(9, "write");
            }
            current = 1;
            continue;
        }

        if(current >= from && current <= to) {
            if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                err(3, "write");
            }
        }
        current++;
    }

    if(res != 0) {
        err(8, "read");
    }
}

void dOption(char delimiter, int from, int to) {
    int current = 1;
    int res;
    uint8_t ch;
    while((res = read(0, &ch, sizeof(ch))) == sizeof(ch)) {
        if(ch == '\n') {
            current = 1;
            if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                err(10, "write");
            }
            continue;
        }
        if(from <= current && current < to) {
            if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                err(5, "write");
            }
        } else if(current == to && ch != delimiter) {
            if(write(1, &ch, sizeof(ch)) != sizeof(ch)) {
                err(6, "write");
            }
        }
        if(ch == delimiter) {
            current++;
        }

    }

    if(res != 0) {
        err(6, "read");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3 && argc != 5) {
        errx(1, "Wrong args count");
    }

    if(strcmp(argv[1], "-c") == 0) {
        if(argc != 3) {
            errx(2, "Wrong args count");
        }
        if(strlen(argv[2]) == 1) {
            int byte = convertToInt(argv[2]);
            cOption(byte, byte);
        } else if(strlen(argv[2]) == 3 && argv[2][1] == '-') {
            char* fromPtr = argv[2];
            fromPtr[1] = '\0';

            int from = convertToInt(fromPtr);
            int to = convertToInt(&argv[2][2]);

            cOption(from, to);
        } else {
            errx(3, "Invalid argument");
        }
    } else if(strcmp(argv[1], "-d") == 0 && argc == 5 && strcmp(argv[3], "-f") == 0) {
        if(strlen(argv[4]) == 1) {
            int fieldsCount = convertToInt(argv[4]);
            dOption(argv[2][0], fieldsCount, fieldsCount);
       } else if(strlen(argv[4]) == 3) {
            char* fromPtr = argv[4];
            fromPtr[1] = '\0';

            int from = convertToInt(fromPtr);
            int to = convertToInt(&argv[4][2]);

            dOption(argv[2][0], from, to);
        } else {
            errx(4, "Invalid arguments");
        }
    } else {
        errx(5, "Invalid arguments");
    }
}
