obj-m := led.o

KDIR := /home/S5-driver/lesson7/linux-ok6410

build: all led_app

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm
	
led_app:
	arm-linux-gcc -static -o led_app led_app.c

clean:
	rm -f *.order *.symvers *.mod.o *.o *.ko *.mod.c led_app
