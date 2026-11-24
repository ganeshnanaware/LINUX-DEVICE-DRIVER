/*****



******/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

#define DEVICE_COUNT 4

static dev_t first_device;

static int __init my_init(void)
{
	int i, ret;
	dev_t current_device;
	
	ret = alloc_chrdev_region(&first_device,0,DEVICE_COUNT,"first_character_device");
	if(ret < 0)
	{
		return ret;
	}
	
	for(i = 0;i < DEVICE_COUNT; i++)
	{
		current_device = first_device+i;
		pr_info("character_device: Major = %u and Minor = %u \n",MAJOR(current_device), MINOR(current_device));
	}
	return 0;
}

static void __exit my_exit(void)
{
	unregister_chrdev_region(first_device,DEVICE_COUNT);
	pr_info("Character_device:Device number is released\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
