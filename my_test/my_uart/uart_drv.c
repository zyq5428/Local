#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/major.h>
#include "uart.h"


struct work_struct * work1;

struct timer_list uart_timer;

unsigned int uart_num = 0;

wait_queue_head_t uart_q;

struct resource * res_irq;
struct resource * res_mem_io;
struct resource * res_mem_ctl;

unsigned int * uart_io; 
unsigned int * uart_base; 

static short div_val[16] = {
	0x0000, 0x0080, 0x0808, 0x0888,
	0x2222, 0x4924, 0x4A52, 0x54AA,
	0x5555, 0xD555, 0xD5D5, 0xDDD5,
	0xDDDD, 0xDFDD, 0xDFDF, 0xFFDF
};


void work1_func(struct work_struct * work)
{
	mod_timer(&uart_timer, jiffies + HZ/10);
}

void uart_timer_func(unsigned long data)
{
	unsigned int uart_val;
	
	uart_val = readl(uart_base + 1) & 0b11;
	
	if (uart_val == 0b10) {
		uart_num = 1;
	}
	
	if (uart_val == 0b01) {
		uart_num = 2;
	}
	
	wake_up(&uart_q);
}

static irqreturn_t uart_int(int irq, void *dev_id)
{
	/*Check if a uart interrupt has occurred */
	
	/*Clear uart interrupts that have occurred(If it is a CPU internal interrupt (non-peripheral), the system will help clear) */
	
	/*Submit the bottom half */
		/*queue work*/
	schedule_work(work1);
	
	//return 0;
	return IRQ_HANDLED;
}

short select_uart_div(int num)
{
	if (num > 16) {
		printk(KERN_WARNING"set is error");
		return 0;
	}
	return div_val[num];
}

void io_test(void)
{
	unsigned int data;
	
	/*set RXD3 and TXD3 output*/
	data = readl(uart_io);
	data &= ~(0xff << 8);					
	data |= (0x11 << 8);
	writel(data, uart_io);
	
	/*set RXD3 1 and TXD3 0*/
	data = readl(uart_io + 4);
	data &= ~(0xc << 8);					
	data |= (0x4 << 8);
	writel(data, uart_io + 4);
}

void uart_hw_init(void)
{
	int int_enable = 0;
	unsigned int data;
	
	printk(KERN_WARNING"uart init");
	
	/*io set RXD3 and TXD3*/
	data = readl(uart_io);
	data &= ~(0xff << 8);					
	data |= (0x22 << 8);
	writel(data, uart_io);
	
	/*UART line control registers*/
	data = readl(uart_base + ULCON3);
	data &= 0x0;
	/*Normal mode,No parity,One stop bit per frame,8-bit Word Length*/
	data |= (0x3 << 0);
	writel(data, uart_base + ULCON3);

	/*UART control registers*/
	data = readl(uart_base + UCON3);
	data &= 0x0;
	/*Select PCLK,TX Level,RX Pulse,Enable Rx time-out interrupts,Interrupt request or polling mode*/
	data |= (0x0 << 10) | (0x1 << 9) | (0x0 << 8) | (0x1 << 7) | (0x1 << 2) | (0x1 << 0);
	writel(data, uart_base + UCON3);
	
	/*UART FIFO control registers*/
	data = readl(uart_base + UFCON3);
	data &= 0x0;
	/*FIFO Disable*/
	data |= 0;
	writel(data, uart_base + UFCON3);
	
	/*Baud rate divisior register*/
	data = readl(uart_base + UBRDIV3);
	data &= 0x0;
	/*set div*/
	data |= UBRDIVn;
	writel(data, uart_base + UBRDIV3);
	
	/*Baud rate divisior register*/
	data = readl(uart_base + UDIVSLOT3);
	data &= 0x0;
	/*set div*/
	data |= select_uart_div(UDIVSLOTn);
	writel(data, uart_base + UDIVSLOT3);
	
	/*UART FIFO control registers*/
	data = readl(uart_base + UINTM3);
	data &= 0x0;
	if (int_enable)
		/*Mask Modem interrupt,Mask Error interrupt*/
		data |= (0x1 << 3) | (0x1 << 1);
	else
		/*disable interrupt*/
		data |= (0x1 << 3) | (0x1 << 2) | (0x1 << 1) | (0x1 << 0);
	writel(data, uart_base + UINTM3);
	
	/*Interrupt pending register contains*/
	data = readl(uart_base + UINTP3);
	
	printk(KERN_WARNING"UINTP3 value is %d", data);
	
	data &= 0x0;
	/*clear interrupt*/
	data |= 0xf;
	writel(data, uart_base + UDIVSLOT3);
	
	/*read UART TX/RX STATUS REGISTER initial state*/
	data = readl(uart_base + UTRSTAT3);
	
	printk(KERN_WARNING"UART TX/RX STATUS REGISTER initial value is %d", data);
	
}

int uart_open(struct inode * node, struct file * filp)
{
	printk(KERN_WARNING"uart open");
	
	return 0;
}

ssize_t uart_read (struct file * filp, char __user * buf, size_t size, loff_t * pos)
{
	unsigned int data;
	char rx_buf;
	
	printk(KERN_WARNING"uart read");
	
	data = readl(uart_base + UTRSTAT3);
	
	printk(KERN_WARNING"UTRSTAT3 value is %d", data);
	
	//wait_event(uart_q, uart_num);
	
	while (!(data & 0x1)) {
		;
	}
	/*UART receiving buffer registers*/
	rx_buf = readb(uart_base + URXH3);
	copy_to_user(buf, &rx_buf, 1);
		
	return 1;
}

ssize_t uart_write (struct file * filp, const char __user * buf, size_t size, loff_t * pos)
{
	unsigned int data;
	char tx_buf;
	
	printk(KERN_WARNING"uart write");
	
	copy_from_user(&tx_buf, buf, 1);
	
	printk(KERN_WARNING"tx_buf value is %c", tx_buf);
	
	data = readl(uart_base + UTRSTAT3);
	
	printk(KERN_WARNING"UTRSTAT3 value is %d", data);
	
	//wait_event(uart_q, uart_num);
	
	/*wait tx complete*/
/*	while (!(data & (0x1 << 1))) {
		;
	}*/
	/*UART receiving buffer registers*/
	writel(tx_buf, uart_base + UTXH3);
		
	return 1;
}

int uart_close (struct inode * node, struct file * filp)
{
	printk(KERN_WARNING"uart close");
	
	return 0;
}

struct file_operations uart_fops = {
	.open = uart_open,
	.read = uart_read,
	.write = uart_write,
	.release = uart_close,
};


struct miscdevice uart3_miscdev = {
	.minor = 200,
	.name = "uart3",
	.fops = &uart_fops,
};

static int uart_probe(struct platform_device * pdev)
{
	int ret;
	int size;
	struct resource *addr_io_req;
	struct resource *addr_req;
	
	printk(KERN_WARNING"now is probe uart");
	
	//struct class *uart_class;
	
	ret = misc_register(&uart3_miscdev);
	
	if (ret !=0)
		printk(KERN_WARNING"register fail!\n");
	
	//uart_class = class_create(THIS_MODULE, "uart_class");
	//device_create(uart_class, NULL, MKDEV(MISC_MAJOR,uart3_miscdev.minor), NULL,"ok6410uart");
	
	printk(KERN_WARNING"now is get irq resource");
	
	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	request_irq(res_irq->start, uart_int, IRQF_TRIGGER_FALLING, "uart", (void *)1);
	
	/*init uart*/
	printk(KERN_WARNING"now is get memory resource");
	
	addr_io_req = request_mem_region(GPBCON, GPB2_3_SIZE, "my-uart");

	if (addr_io_req == NULL) {
		printk(KERN_WARNING"cannot claim address io area\n");
		release_mem_region(GPBCON, GPB2_3_SIZE);
		//iounmap((volatile unsigned long *)GPBCON);
		addr_io_req = request_mem_region(GPBCON, GPB2_3_SIZE, "my-uart");
	}
	
	if (addr_io_req == NULL) {
		printk(KERN_WARNING"cannot claim address io area\n");
	}
	
	addr_req = request_mem_region(UART3_BASE, UART3_SIZE, "my-uart");

	if (addr_req == NULL) {
		printk(KERN_WARNING"cannot claim address reg area\n");
		release_mem_region(UART3_BASE, UART3_SIZE);
		//iounmap((volatile unsigned long *)UART3_BASE);
		addr_req = request_mem_region(UART3_BASE, UART3_SIZE, "my-uart");
	}
	
	if (addr_req == NULL) {
		printk(KERN_WARNING"cannot claim address reg area\n");
	}
	
	res_mem_io = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	//size = res_mem_io->end - res_mem_io->start + 1;
	//uart_io = ioremap(res_mem_io->start, size);
	uart_io = ioremap(res_mem_io->start, GPB2_3_SIZE);
	
	res_mem_ctl = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	//size = res_mem_ctl->end - res_mem_ctl->start + 1;
	//uart_base = ioremap(res_mem_ctl->start, size);
	uart_base = ioremap(res_mem_ctl->start, UART3_SIZE);
	
	io_test();
	
	//uart_hw_init();
	
	printk(KERN_WARNING"init work");
	
	/*init work*/
	work1 = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
	
	INIT_WORK(work1, work1_func);
	
	printk(KERN_WARNING"init timer");
	
	/*init timer */
	init_timer(&uart_timer);
	uart_timer.function = uart_timer_func;
	
	/*register timer */
	add_timer(&uart_timer);
	
	printk(KERN_WARNING"init wait queue");
	
	/*init wait queue*/
	init_waitqueue_head(&uart_q);
	
	return ret;
}

static int uart_remove(struct platform_device *pdev)
{
	printk(KERN_WARNING"uart remove");
	
	free_irq(res_irq->start, (void *)1);
	
	iounmap(uart_io);
	
	iounmap(uart_base);
	
	misc_deregister(&uart3_miscdev);
	
	return 0;
}

static struct platform_driver uart3_driver = {
	.probe	= uart_probe,
	.remove	= __devexit_p(uart_remove),
	.driver = {
		.name = "my-uart",
		.owner = THIS_MODULE,
	},
};

static int __init ok6410_uart_init(void)
{
	//printk(KERN_WARNING"uart init\n");
	
	return  platform_driver_register(&uart3_driver);

}

static void __exit ok6410_uart_exit(void)
{
	//printk(KERN_WARNING"uart exit\n");
	
	platform_driver_unregister(&uart3_driver);
}


module_init(ok6410_uart_init);
module_exit(ok6410_uart_exit);
MODULE_LICENSE("GPL");