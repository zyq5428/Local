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
#include <linux/input.h>

#define GPNCON	0x7f008830
#define GPNDAT	0x7f008834

struct work_struct * work1;

struct timer_list key_timer;

unsigned int * gpio_data;
unsigned int key_num = 0;

wait_queue_head_t key_q;

struct input_dev * button_dev;

void work1_func(struct work_struct * work)
{
	mod_timer(&key_timer, jiffies + HZ/10);
}

void key_timer_func(unsigned long data)
{
	unsigned int key_val;
	
	key_val = readl(gpio_data) & 0b11;
	
	if (key_val == 0b10) {
		key_num = 1;
		input_report_key(button_dev,KEY_2,1);
	}
	
	if (key_val == 0b01) {
		key_num = 2;
		input_report_key(button_dev,KEY_3,1);
	}
	
	//wake_up(&key_q);
	input_sync(button_dev);
}

static irqreturn_t key_int(int irq, void *dev_id)
{
	/*Check if a key interrupt has occurred */
	
	/*Clear key interrupts that have occurred(If it is a CPU internal interrupt (non-peripheral), the system will help clear) */
	
	/*Submit the bottom half */
		/*queue work*/
	schedule_work(work1);
	
	return IRQ_HANDLED;
}

void key_hw_init(void)
{
	unsigned int data;
	unsigned int * gpio_config;
	
	gpio_config = ioremap(GPNCON, 4);
	data = readl(gpio_config);
	data &= ~0b1111;					//set key1 and key2
	data |= 0b1010;
	writel(data, gpio_config);
	
	gpio_data = ioremap(GPNDAT, 4);
}


static int __init ok6410_key_init(void)
{
	printk(KERN_WARNING"key init\n");
	
	/*Assign input device structure*/
	button_dev = input_allocate_device();
	
	/*Declare the type of event that may be reported*/
	set_bit(EV_KEY, button_dev->evbit);
	
	/*Declare the button number that may be reported*/
	set_bit(KEY_2, button_dev->keybit);
	set_bit(KEY_3, button_dev->keybit);
	
	/*register input device*/
	input_register_device(button_dev);
	
	request_irq(S3C_EINT(0), key_int, IRQF_TRIGGER_FALLING, "key", (void *)0);
	request_irq(S3C_EINT(1), key_int, IRQF_TRIGGER_FALLING, "key", (void *)1);
	
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
	
	return 0;
}

static void __exit ok6410_key_exit(void)
{
	printk(KERN_WARNING"key exit\n");
	
	input_unregister_device(button_dev);
}


module_init(ok6410_key_init);
module_exit(ok6410_key_exit);
MODULE_LICENSE("GPL");