#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/timer.h>

#define GPNCON	0x7f008830
#define GPNDAT	0x7f008834

struct work_struct * work1;

struct timer_list key_timer;

unsigned int * gpio_data;

void work1_func(struct work_struct * work)
{
	mod_timer(&key_timer, jiffies + HZ/10);
}

void key_timer_func(unsigned long data)
{
	unsigned int key_val;
	
	key_val = readl(gpio_data) & 0x01;
	
	if (key_val == 0) {
		printk(KERN_WARNING"key down!\n");
	}
}

static irqreturn_t key_int(int irq, void *dev_id)
{
	/*Check if a key interrupt has occurred */
	
	/*Clear key interrupts that have occurred(If it is a CPU internal interrupt (non-peripheral), the system will help clear) */
	
	/*Submit the bottom half */
		/*queue work*/
	schedule_work(work1);
	
	return 0;
}

void key_hw_init(void)
{
	unsigned int data;
	unsigned int * gpio_config;
	
	gpio_config = ioremap(GPNCON, 4);
	data = readl(gpio_config);
	data &= ~0b11;					//set key1
	data |= 0b10;
	writel(data, gpio_config);
	
	gpio_data = ioremap(GPNDAT, 4);
}

int key_open(struct inode * node, struct file * filp)
{
	return 0;
}


struct file_operations key_fops = {
	.open = key_open,
};


struct miscdevice key_miscdev = {
	.minor = 200,
	.name = "key",
	.fops = &key_fops,
};

static int key_init(void)
{
	printk(KERN_WARNING"key init\n");
	
	misc_register(&key_miscdev);
	
	request_irq(S3C_EINT(0), key_int, IRQF_TRIGGER_FALLING, "key", 0);
	
	key_hw_init();
	
	/*init work*/
	work1 = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
	
	INIT_WORK(work1, work1_func);
	
	/*init timer */
	init_timer(&key_timer);
	key_timer.function = key_timer_func;
	
	/*register timer */
	add_timer(&key_timer);
	
	return 0;
}

static void key_exit(void)
{
	printk(KERN_WARNING"key exit\n");
	
	free_irq(S3C_EINT(0), 0);
	
	misc_deregister(&key_miscdev);
}


module_init(key_init);
module_exit(key_exit);
MODULE_LICENSE("GPL");