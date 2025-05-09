﻿// Реализирайте команда wc, с един аргумент подаден като входен параметър
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <err.h>
#include <string.h>

int main (int argc, char* argv[]) {
	int fd1;
	char c;
	int lines = 0;
	int words = 0;
	int chars = 0;

	if (argc != 2) {
		errx(1, "Invalid arguments count");
	}

	if ( (fd1 = open(argv[1], O_RDONLY)) == -1 ) {
		err(2, "File %s failed to open in read mode", argv[1]);
	}

	bool word=false;
	
    	int res;

	while ((res = read(fd1, &c, sizeof(c))) == sizeof(c) ) {
		chars++;
		
		if (c == '\n') {
			lines++;

			if(word) {
				words++;
				word = false;
			}
		}
		else if (c == ' ') {
			if(word) {
				words++;
				word = false;
			}
		}
		else {
			word = true;	
		}
	}

   	if(res != 0) {
     		err(4, "failed to read from %s", argv[1]);
  	}

   	char buff[1024];
	snprintf(buff, sizeof(buff),"File %s has:\n%d number of lines.\n%d number of words.\n%d number of chars.\n",
		argv[1], lines, words, chars);
   	if(write(1, buff, strlen(buff)) != (ssize_t)strlen(buff)) {
       		err(5, "failed to write to stdout");
   	}
	close(fd1);
}
