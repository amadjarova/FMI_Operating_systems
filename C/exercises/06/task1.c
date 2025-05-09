﻿// копирайте съдържанието на файл1 във файл2
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(void) {
	int fd1;
	int fd2;
	char c;

	fd1 = open("foo.txt", O_RDONLY);
	if (fd1 == -1) {
		err(1, "File failed to open in read mode");
	}

	if ( ( fd2 = open("bar.txt", O_CREAT|O_WRONLY, S_IRUSR | S_IWUSR) ) == -1 ){
		err(2, "File failed to open in write mode");
	}

    	int res;
	while ((res = read(fd1, &c, sizeof(c))) == sizeof(c)) {
		if(write(fd2, &c, sizeof(c)) != res) {
            		err(3, "failed to write to bar.txt");
        	}
	}

    	if(res != 0) {
        	err(4, "failed to read from foo.txt");
    	}

	close(fd1);
	close(fd2);
	exit(0);
}
