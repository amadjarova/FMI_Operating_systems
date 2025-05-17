/*
        Да се напише програма на C, която изпълнява команда ls с точно един аргумент.
*/

#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main (int argc, char* argv[])
{

        if (argc != 2) {
                errx(1, "wrong args count");
        }

        if (execlp("ls", "ls", argv[1], (char*)NULL) == -1) {
                err(99, "err execling");
        }
}
