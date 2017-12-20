#include <linux/init.h>
#include <linux/module.h>

extern add(int a, int b);

int a = 3;
char * p;

static int hello_init()
{
    printk(KERN_WARNING"Hello world!\n");
	printk(KERN_WARNING"a = %d\n", a);
	printk(KERN_WARNING"p is %s\n", p);
    return 0;	
}

int b;
static void hello_exit()
{
	b = add(1,4);
	printk(KERN_WARNING"b = %d\n", b);
    printk(KERN_WARNING"hello exit!\n");	
}

module_init(hello_init);
module_exit(hello_exit);

module_param(a, int, S_IRUGO|S_IWUSR);
module_param(p, charp, S_IRUGO|S_IWUSR);

MODULE_AUTHOR("Johnson Zhou");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple hello world module");
MODULE_ALIAS("A simpleST module");
MODULE_VERSION("V1.0");
