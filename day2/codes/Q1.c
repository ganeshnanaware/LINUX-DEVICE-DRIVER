/*****



******/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

static dev_t device_number;

static int __init my_init(void)
{
	int ret;
	ret = alloc_chrdev_region(&device_number,0,1,"first_character_device");
	if(ret < 0)
	{
		return ret;
	}
	pr_info("character_device: Major = %u and Minor = %u \n",MAJOR(device_number), MINOR(device_number));
	return 0;
}

static void __exit my_exit(void)
{
	unregister_chrdev_region(device_number,1);
	pr_info("Character_device:Device number is released\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
