#include <linux/init.h>
#include <linux/module.h>

int add(int a, int b)
{
	return a+b;
}

static int add_init()
{
    return 0;	
}

static void add_exit()
{
    
}

module_init(add_init);
module_exit(add_exit);

EXPORT_SYMBOL(add);

MODULE_AUTHOR("Johnson Zhou");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple add module");
MODULE_ALIAS("A add module");
MODULE_VERSION("V1.0");
