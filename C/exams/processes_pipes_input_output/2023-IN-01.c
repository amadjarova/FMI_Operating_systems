#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

const char* WORDS[] = {
	"tic ",
	"tac ",
	"toe\n"
};

int convertToInt(char* str) {
    char* endptr;
    int num = strtol(str, &endptr, 10);

    if(str[0] == '\0' || *endptr != '\0') {
        errx(1, "arg should be a number");
    }

    return num;
}

void handleMessages(int totalWords, int fromFD, int toFD) {
	int count;
	ssize_t read_size;

	bool written_final_signal = false;

	while((read_size = read(fromFD, &count, sizeof(count))) > 0) {
		if(count >= totalWords) {
			if(!written_final_signal) {
				if(write(toFD, &count, sizeof(count)) == -1) {
				    err(2, "write");
				}
			}

			return;
		}

		if(write(1, WORDS[count % 3], strlen(WORDS[count % 3])) == -1) {
		    err(2, "write %d", count);
		}

		int next = count + 1;
		if(write(toFD, &next, sizeof(next)) == -1) {
		    err(2, "write");
		}

		if(next >= totalWords) {
			written_final_signal = true;
		}
	}

	if(read_size == 0) {
		errx(2, "unexpected EOF");
	}

	if(read_size < 0) {
		err(2, "read");
	}
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		errx(1, "not enough args");
	}

	int children = convertToInt(argv[1]);

	if(children < 1 || children > 7) {
		errx(1, "NC must be between 1 and 7");
	}

	int totalWords = convertToInt(argv[2]);

	if(totalWords < 1 || totalWords > 35) {
		errx(1, "WC must be between 1 and 35");
	}

	int pipes = children + 1;

	int pipeFDs[8][2];
	for(int p = 0; p < pipes; p++) {
		if(pipe(pipeFDs[p]) == -1) {
		    err(1, "pipe");
		}
	}

	for(int i = 1; i <= children; i++) {
		pid_t pid = fork();
		if(pid == -1) {
		    err(1, "fork");
		}

		if(pid == 0) {
			int fromFD = pipeFDs[i-1][0];
			int toFD = pipeFDs[i][1];

			for(int p = 0; p < pipes; p++) {
				if(pipeFDs[p][0] != fromFD){
					close(pipeFDs[p][0]);
				}
				if(pipeFDs[p][1] != toFD) {
					close(pipeFDs[p][1]);
				}
			}

			handleMessages(totalWords, fromFD, toFD);

			close(fromFD);
			close(toFD);

			exit(0);
		}
	}

	int fromFD = pipeFDs[children][0];
	int toFD = pipeFDs[0][1];

	for(int p = 0; p < pipes; p++) {
		if(pipeFDs[p][0] != fromFD){
			close(pipeFDs[p][0]);
		}
		if(pipeFDs[p][1] != toFD) {
			close(pipeFDs[p][1]);
		}
	}

	if(write(1, WORDS[0], strlen(WORDS[0])) == -1) {
	    err(1, "write");
	}

	int next = 1;
	if(write(toFD, &next, sizeof(next)) == -1) {
	    err(2, "write (next)");
	}

	handleMessages(totalWords, fromFD, toFD);

	close(fromFD);
	close(toFD);

	for(int i = 1; i <= children; i++) {
		pid_t pid;
		int status;
		if((pid = wait(&status)) == -1) {
		    err(1, "wait");
		}

		if (WIFEXITED(status)) {

			if(WEXITSTATUS(status) != 0) {
				errx(1,"child %d exited with code %d", pid, WEXITSTATUS(status));
			}
		} else {
			errx(2,"child %d was killed", pid);
		}
	}

	return 0;
}
