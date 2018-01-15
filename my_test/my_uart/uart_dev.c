#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include "uart.h"


struct resource uart_resource[] = {
	[0] = {
		.start = GPBCON,
		.end = GPBCON + GPB2_3_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	
	[1] = {
		.start = UART3_BASE,
		.end = UART3_BASE + UART3_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	
	[2] = {
		.start = IRQ_UART3,
		.end = IRQ_UART3,
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device uart3_device = {
	.name = "my-uart",
	.id = 0,
	.num_resources = ARRAY_SIZE(uart_resource),
	.resource = uart_resource,
};

static int __init uartdev_init(void)
{
	platform_device_register(&uart3_device);
	
	return 0;
}

static void __exit uartdev_exit(void)
{
	platform_device_unregister(&uart3_device);
}

module_init(uartdev_init);
module_exit(uartdev_exit);

MODULE_LICENSE("GPL");