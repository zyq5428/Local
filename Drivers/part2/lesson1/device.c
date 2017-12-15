#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>

extern struct bus_type my_bus_type;

struct device my_device = {
	.init_name = "my_dev",
	.bus = &my_bus_type,
};

static int __init my_device_init(void)
{
	int ret;
	
	ret = device_register(&my_device);
	
	return ret;
}

static void __exit my_device_exit(void)
{
	device_unregister(&my_device);
}

module_init(my_device_init);
module_exit(my_device_exit);

MODULE_LICENSE("GPL");