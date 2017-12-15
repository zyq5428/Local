#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>

extern struct bus_type my_bus_type;

int my_probe (struct device *dev)
{
	printk(KERN_WARNING"driverfound device it can handle!\n");
	return 0;
}

struct device_driver my_driver = {
	.name = "my_dev",
	.bus = &my_bus_type,
	.probe = my_probe,
};

static int __init my_driver_init(void)
{
	int ret;
	
	ret = driver_register(&my_driver);
	
	return ret;
}

static void __exit my_driver_exit(void)
{
	driver_unregister(&my_driver);
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");