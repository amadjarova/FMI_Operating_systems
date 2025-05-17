#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        errx(1, "args count");
    }

    if(mkfifo("myfifo", 0666) == -1 && errno != EEXIST) {
        err(2, "mkfifo");
    }

    int fd = open("myfifo", O_WRONLY);
    if(fd == -1) {
        err(3, "open");
    }

    if (dup2(fd, 1) == -1) {
        err(3, "dup2");
    }
    close(fd);
    if (execlp("cat", "cat", argv[1], (char*)NULL) == -1) {
        err(3, "exec");
    }
}
