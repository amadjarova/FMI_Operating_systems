/*
        Да се напише програма на C, която е аналогична на горния пример, но
        принуждава бащата да изчака сина си да завърши.
*/

#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
        int n = 1000;
        int status;

        pid_t a = fork();
        if (a == -1) {
                err(1, "fork");
        }
        if (a > 0) {
                if(wait(&status) == -1) {
                        err(3, "wait");
                }
                if(WIFEXITED(status)) {
                        if(WEXITSTATUS(status) != 0) {
                                errx(5, "Child exited with status %d", WEXITSTATUS(status));
                        }
                } else {
                        errx(2, "Child was killed");
                }

                for (int i = 0; i < n; i++) {
                        if(write(1, "F", 1) == -1) {
                                err(1, "write");
                        }
                }
        } else {
                for (int i = 0; i < n; i++) {
                        if(write(1, "S", 1) == -1) {
                                err(2, "write");
                        }
                }
        }

        exit(0);
}
