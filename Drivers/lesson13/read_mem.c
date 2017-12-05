#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

int main(void)
{
	int fd = 0;
	int dst = 0;

	fd = open("/dev/memdev0", O_RDWR);

	read(fd, &dst, sizeof(int));

	printf("dst is %d\n", dst);

	return 0;
}
