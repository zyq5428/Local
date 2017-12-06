#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "memdev.h"

struct cdev mdev;
dev_t devno;

int dev1_regs[5];
int dev2_regs[5];

loff_t mem_lseek (struct file *filp, loff_t offset, int whence)
{
	loff_t new_pos = 0;
	switch (whence) {
		case SEEK_SET:
			new_pos = 0 + offset;
			break;
			
		case SEEK_CUR:
			new_pos = filp->f_pos + offset;
			break;
			
		case SEEK_END:
			new_pos = 5 * sizeof(int) + offset;
			break;
			
		default:
			break;
	}
	
	filp->f_pos = new_pos;
	
	return new_pos;
}

ssize_t mem_read (struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	int *reg_base = filp->private_data;
	
	copy_to_user(buf, reg_base + (*ppos), size);
	
	filp->f_pos += size;
	
	return size;	
}

ssize_t mem_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	int *reg_base = filp->private_data;
	
	copy_from_user(reg_base + (*ppos), buf, size);
	
	filp->f_pos += size;
	
	return size;	
}

int mem_open (struct inode *node, struct file *filp)
{
	int num = MINOR(node->i_rdev);
	
	if (num == 0)
		filp->private_data = dev1_regs;
	
	if (num == 1)
		filp->private_data = dev2_regs;
	
	return 0;
}

int mem_close (struct inode *node, struct file *filp)
{
	return 0;
}

long mem_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case MEM_RESTART:
			printk(KERN_WARNING"restart device!\n");
			return 0;
			
		case MEM_SET:
			printk(KERN_WARNING"ARG IS %d\n", arg);
			return 0;
			
		default:
			return -EINVAL;
	}
}

struct file_operations memfops =
{
	.llseek = mem_lseek,
	.read = mem_read,
	.write = mem_write,
	.open = mem_open,
	.release = mem_close,
	.unlocked_ioctl = mem_ioctl,
};

int memdev_init(void)
{
	cdev_init(&mdev, &memfops);
	
	alloc_chrdev_region(&devno, 0, 2, "memdev");
	
	cdev_add(&mdev, devno, 2);
	
	return 0;
}

void memdev_exit(void)
{
	cdev_del(&mdev);
	unregister_chrdev_region(devno, 2);
}


module_init(memdev_init);
module_exit(memdev_exit);