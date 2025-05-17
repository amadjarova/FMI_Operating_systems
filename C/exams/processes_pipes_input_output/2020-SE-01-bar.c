#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        err(1, "args count");
    }

    int fd = open("myfifo", O_RDONLY);
    if(fd == -1) {
        err(3, "open");
    }
  
    if(dup2(fd, 0) == -1) {
        err(4, "dup2");
    }
    close(fd);
    if(execl(argv[1], argv[1], (char*)NULL) == -1) {
        err(5, "exec");
    }
}
