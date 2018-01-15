obj-m := key.o

KDIR := /home/S5-driver/lesson7/linux-ok6410

APP = uart_app

build: all $(APP)

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
	
$(APP):
	arm-linux-gcc -static -o $@ $(APP).c

clean:
	rm -f *.order *.symvers *.mod.o *.o *.ko *.mod.c *_app
