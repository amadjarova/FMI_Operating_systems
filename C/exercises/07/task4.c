/*
        Да се напише програма на C, която създава процес дете и демонстрира
принцина на конкурентност при процесите.
*/

#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main(void)
{
        int n = 100;

        pid_t a = fork();
        if (a == -1) {
                err(1, "fork");
        }
        if (a > 0) {
                for (int i = 0; i < n; i++) {
                        if(write(1,"F", 1) == -1) {
                                err(1, "write");
                        }
                }
        } else {
                for (int i = 0; i < n; i++) {
                        if(write(1,"S", 1) == -1) {
                                err(2, "write");
                        }
                }
        }

        if(write(1,"\n", 1) == -1) {
                err(3, "write");
        }

        exit(0);
}
