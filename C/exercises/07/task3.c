/*
        Да се напише програма на C, която изпълнява команда sleep (за 60 секунди).
*/

#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main(void)
{
        if (execl("/bin/sleep", "sleep", "60", (char*)NULL) == -1) {
                err(99, "err execling");
        }
}
