objs := nand.o button.o interrupt.o led.o mmu.o timer.o uart.o dma.o lcd.o snow_image.o tsc.o dm9000a.o

all : $(objs)
	arm-linux-ld -r -o dev.o $^
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o		
	
