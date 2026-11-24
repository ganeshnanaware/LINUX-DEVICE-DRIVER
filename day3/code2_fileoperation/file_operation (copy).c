#include <linux/module.h>		//module entry point and access point
#include <linux/kernel.h>		//
#include <linux/init.h>			//
#include <linux/fs.h>			//file objects
#include <linux/cdev.h>			//struct cdev, cdev_init(), cdev_add()
#include <linux/device.h>		//Class create, device create
#include <linux/err.h >			//IS_ERR, PTR_ERR
#include <linux/container_of.h>		//Recover device object address
#include <linux/slab.h>			//Allocate free memory
#include <linux/uaccess.h>		//copy across the userspace

#define DEVICE_NAME "beginner"
#define CLASS_NAME "beginner_class"
#define CAPACITY 64

static bool fail_open;

struct demo_device
{	
	dev_t number;		//Denotes major and minor numbers
	struct cdev cdev;	//Character device object
	struct class *class;	//Model class during init
	struct device *device;	//Model device during init
	bool fail_open;		//Test behaviour OPTIONAL!!!
};

static struct demo_device demo;

struct session
{
	struct demo_device *owner;	//Device associated with per open state
	size_t length;			//Valis data length
	u8 data[CAPACITY];		//Binary byte. Not a null terminated string
};

static const struct file_operations demo_operation = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
	.llseek = my_seek,
	.unlocked_ioctl = my_ioctl,
};

static int __init my_init(void)
{
	int ret;

	demo.fail_open = fail_open;		//initalize callback-visible state
	
	/*Allocate one device - MAJOR and MINOR numbers*/
	ret = alloc_chrdev_region(&demo.number, 0, 1,DEVICE_NAME);

	if(ret < 0) return ret;

	/*Initalize cdev structure and associate it with the drivers file operation structure. It does not register the character device only initalize it */

	cdev_init(&demo.cdev, &demo_operations);
	demo.cdev.owner = THIS_MODULE;

	/*Register the initalized character device with kernel character device subsystem and associates it with a range of device number*/
	
	ret = cdev_add(&demo.cdev, demo.number, 1);

	if(ret < 0)
	{
		goto err_numbers;
	}
	
	demo.class = class_create(CLASS_NAME);
	if(IS_ERR(demo.class))
	{
		ret = PTR_ERR(demo.class);
		goto err_cdev;
	}

	demo.device = device_create(demo.class, NULL, demo.number, &demo, DEVICE_NAME);
	if(IS_ERR(demo.device))
        {
                ret = PTR_ERR(demo.device);
                goto err_class;
        }
	
	pr_info("beginner:register major %u and minor %u\n",MAJOR(demo.number),MINOR(demo.number));

	return 0;
	
err_class: 
	class_destroy(demo.class);
	
err_cdev:
	cdev_del(&demo.cdev);

err_numbers:
	unregister_chrdev_region(demo.number, 1);
	return ret;

}


static void __exit my_exit(void)
{

}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Manual cdev registration and file operation implementaion");

MODULE_AUTHOR("GANESHA");







