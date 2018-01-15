#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

#define GPNCON	0x7f008830
#define GPNDAT	0x7f008834

struct resource key_resource[] = {
	[0] = {
		.start = GPNCON,
		.end = GPNCON + 8,
		.flags = IORESOURCE_MEM,
	},
	
	[1] = {
		.start = S3C_EINT(0),
		.end = S3C_EINT(1),
		.flags = IORESOURCE_IRQ,
	},
};

struct platform_device key_device = {
	.name = "my-key",
	.id = 0,
	.num_resources = ARRAY_SIZE(key_resource),
	.resource = key_resource,
};

static int __init keydev_init(void)
{
	platform_device_register(&key_device);
	
	return 0;
}

static void __exit keydev_exit(void)
{
	platform_device_unregister(&key_device);
}

module_init(keydev_init);
module_exit(keydev_exit);

MODULE_LICENSE("GPL");