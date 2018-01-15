#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

int main(void)
{
    int fd, i;
    char buf[4];
	i = 0;

    //struct termios tio;

    if ((fd = open("/dev/uart3", O_RDWR | O_NDELAY | O_NOCTTY)) < 0) {
	printf("could not open\n");
	exit(1);
    } else {
	printf("file open success\n");
    }

    while (0) {
		i = read(fd, buf, 1);
		//printf("recives is %s", buf);
		if (i > 0) {
	    write(fd, buf, 1);
		}
    }
	
	while (1) {
		buf[0] = i;
		write(fd, buf, 1);
		i++;
		sleep(2);
	}

    close(fd);
}