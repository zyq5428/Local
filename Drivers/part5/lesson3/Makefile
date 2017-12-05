obj-m := helloworld.o add.o

KDIR := /home/S5-driver/lesson7/linux-ok6410

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
	
clean:
	rm -f *.o *.ko *.order *.symvers
	

