#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>

struct workqueue_struct * my_wq;
struct work_struct * work1, work2;

void work1_func(struct work_struct * work)
{
	printk(KERN_WARNING"this is work1->\n");
}

void work2_func(struct work_struct * work)
{
	printk(KERN_WARNING"this is work2->\n");
}

static int queue_init(void)
{
	/*creat workqueue*/
	my_wq = create_workqueue("my_wq");

	/*init work*/
	work1 = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
	
	INIT_WORK(work1, work1_func);

	/*queue work*/
	queue_work(my_wq, work1);
	
	/*init work*/
	work2 = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
	
	INIT_WORK(work2, work2_func);

	/*queue work*/
	queue_work(my_wq, work2);

	return 0;
}

static void queue_exit(void)
{

}


module_init(queue_init);
module_exit(queuw_exit);

MODULE_LICENSE("GPL");
