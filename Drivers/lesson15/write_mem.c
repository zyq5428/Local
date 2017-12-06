#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

int main(void)
{
	int fd = 0;
	int src = 2017;

	fd = open("/dev/memdev0", O_RDWR);

	write(fd, &src, sizeof(int));

	close(fd);
}
