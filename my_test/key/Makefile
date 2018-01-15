obj-m := key_dev.o key_drv.o

KDIR := /home/S5-driver/lesson7/linux-ok6410

APP = key_app

build: all $(APP)

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
	
$(APP):
	arm-linux-gcc -static -o $@ $(APP).c

clean:
	rm -f *.order *.symvers *.mod.o *.o *.ko *.mod.c *_app
