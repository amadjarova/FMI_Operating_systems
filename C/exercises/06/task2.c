﻿/*
	Реализирайте команда head без опции (т.е. винаги да извежда
	на стандартния изход само първите 10 реда от съдържанието на
	файл подаден като първи параматър)
*/

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
	int fd1;
	int i = 0;
	char c;

	if (argc != 2) {
		errx(1, "Invalid arguments count.");
	}

	if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
		err(2, "File %s failed to open in read mode", argv[1]);
	}

    	int res;

	while ((res = read(fd1, &c, sizeof(c))) == sizeof(c)) {
		if (c == '\n') {
			i=i+1;
		}

		if(write(1, &c, sizeof(c)) != res) {
            		err(4, "failed to write to stdout");
       		} 

		if (i == 10) {
			break;
		}
	}

    	if(res == -1) {
        	err(4, "failed to read from %s", argv[1]);
    	}

	close(fd1);
	exit(0);
}
