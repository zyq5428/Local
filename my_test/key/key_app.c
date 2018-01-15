#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int fd;
	int key_num;
	
	/*open device*/
	fd = open("/dev/key", 0);
	
	if (fd < 0)
		printf("open device fail!\n");
	
	/*read device*/
	read(fd, &key_num, 4);
	printf("key is %d\n", key_num);

	/*close device*/
	close(fd);
}