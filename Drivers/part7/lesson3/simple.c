#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/blkdev.h>
#include <linux/bio.h>

static int major = 0;

static unsigned short sect_size = 512;
static unsigned long nsectors = 1024;

struct blk_dev {
	int size;
	unsigned char * data;
	struct request_queue * queue;
	struct gendisk * gd;
};

struct blk_dev * dev;

static struct block_device_operations blk_ops = {
	.owner = THIS_MODULE,
};

void blk_transfer(struct blk_dev * dev, unsigned long sector, unsigned long nsect, char * buffer, int write)
{
	unsigned long offset = sector * sect_size;
	unsigned long nbytes = nsect * sect_size;
	
	if (write)
		memcpy(dev->data + offset, buffer, nbytes);
	else
		memcpy(buffer, dev->data + offset, nbytes);
}

static void blk_request(struct request_queue * q)
{
	struct request * req;
	
	req = blk_fetch_request(q);
	
	while (req != NULL) {
		/*Handle the request*/
		blk_transfer(dev, blk_rq_pos(req), blk_rq_cur_sectors(req), req->buffer, rq_data_dir(req));
		
		if (__blk_end_request_cur(req, 0))
			req = blk_fetch_request(q);
	}
}

void setup_device(void)
{
	dev->size = nsectors * sect_size;
	dev->data= vmalloc(dev->size);
	
	dev->queue = blk_init_queue(blk_request, NULL);
	
	blk_queue_logical_block_size(dev->queue, sect_size);
	
	dev->gd = alloc_disk(1);
	
	dev->gd->major = major;
	dev->gd->first_minor = 0;
	dev->gd->fops = &blk_ops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;
	sprintf(dev->gd->disk_name, "simp_blk%d", 0);
	set_capacity(dev->gd, nsectors);
	
	add_disk(dev->gd);
}

static int blk_init(void)
{
	major = register_blkdev(0, "blk");
	
	if (major <= 0) {
		printk("register blk dev fail!\n");
		return -EBUSY;
	}
	
	dev = kmalloc(sizeof(struct blk_dev), GFP_KERNEL);
	
	setup_device();
	
	return 0;
}

static void blk_exit(void)
{
	del_gendisk(dev->gd);
	blk_cleanup_queue(dev->queue);
	vfree(dev->data);
	unregister_blkdev(major, "blk");
	kfree(dev);
}

module_init(blk_init);
module_exit(blk_exit);
MODULE_LICENSE("GPL");
