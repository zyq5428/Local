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


struct work_struct * work1;

struct timer_list key_timer;

unsigned int key_num = 0;

wait_queue_head_t key_q;

struct resource * res_irq;
struct resource * res_mem;
unsigned int * key_base; 

void work1_func(struct work_struct * work)
{
	mod_timer(&key_timer, jiffies + HZ/10);
}

void key_timer_func(unsigned long data)
{
	unsigned int key_val;
	
	key_val = readl(key_base + 1) & 0b11;
	
	if (key_val == 0b10) {
		key_num = 1;
	}
	
	if (key_val == 0b01) {
		key_num = 2;
	}
	
	wake_up(&key_q);
}

static irqreturn_t key_int(int irq, void *dev_id)
{
	/*Check if a key interrupt has occurred */
	
	/*Clear key interrupts that have occurred(If it is a CPU internal interrupt (non-peripheral), the system will help clear) */
	
	/*Submit the bottom half */
		/*queue work*/
	schedule_work(work1);
	
	//return 0;
	return IRQ_HANDLED;
}

void key_hw_init(void)
{
	unsigned int data;
	
	data = readl(key_base);
	data &= ~0b1111;					//set key1 and key2
	data |= 0b1010;
	writel(data, key_base);
	
	//gpio_data = ioremap(GPNDAT, 4);
}

int key_open(struct inode * node, struct file * filp)
{
	return 0;
}

ssize_t key_read (struct file * filp, char __user * buf, size_t size, loff_t * pos)
{
	wait_event(key_q, key_num);
	
	copy_to_user(buf, &key_num, 4);
	
	key_num = 0;
	
	return 4;
}

ssize_t key_write (struct file * filp, const char __user * buf, size_t size, loff_t * pos)
{
	return 0;
}

int key_close (struct inode * node, struct file * filp)
{
	return 0;
}

struct file_operations key_fops = {
	.open = key_open,
	.read = key_read,
	.write = key_write,
	.release = key_close,
};


struct miscdevice key_miscdev = {
	.minor = 200,
	.name = "key",
	.fops = &key_fops,
};

static int key_probe(struct platform_device * pdev)
{
	int ret;
	int size;
	
	ret = misc_register(&key_miscdev);
	
	if (ret !=0)
		printk(KERN_WARNING"register fail!\n");
	
	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	request_irq(res_irq->start, key_int, IRQF_TRIGGER_FALLING, "key", (void *)1);
	request_irq(res_irq->end, key_int, IRQF_TRIGGER_FALLING, "key", (void *)2);
	
	/*init key*/
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	size = res_mem->end - res_mem->start + 1;
	key_base = ioremap(res_mem->start, size);
	key_hw_init();
	
	/*init work*/
	work1 = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
	
	INIT_WORK(work1, work1_func);
	
	/*init timer */
	init_timer(&key_timer);
	key_timer.function = key_timer_func;
	
	/*register timer */
	add_timer(&key_timer);
	
	/*init wait queue*/
	init_waitqueue_head(&key_q);
	
	return ret;
}

static int key_remove(struct platform_device *pdev)
{
	free_irq(res_irq->start, (void *)1);
	
	free_irq(res_irq->end, (void *)2);
	
	iounmap(key_base);
	
	misc_deregister(&key_miscdev);
	
	return 0;
}

static struct platform_driver key_driver = {
	.probe	= key_probe,
	.remove	= __devexit_p(key_remove),
	.driver = {
		.name = "my-key",
		.owner = THIS_MODULE,
	},
};

static int __init ok6410_key_init(void)
{
	//printk(KERN_WARNING"key init\n");
	
	return  platform_driver_register(&key_driver);

}

static void __exit ok6410_key_exit(void)
{
	//printk(KERN_WARNING"key exit\n");
	
	platform_driver_unregister(&key_driver);
}


module_init(ok6410_key_init);
module_exit(ok6410_key_exit);
MODULE_LICENSE("GPL");