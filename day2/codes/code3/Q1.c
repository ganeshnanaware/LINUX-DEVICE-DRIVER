/*****
     
******/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/err.h>

static dev_t device_number;
static struct class *device_class;

static int __init my_init(void)
{
	struct device *devices;
	
	int ret;
	
	ret = alloc_chrdev_region(&device_number,0,1,"my_device");
	
	if(ret < 0){return ret;}				//we have createdkernel module
	
	device_class = class_create("my_device_class");
	
	if(IS_ERR(device_class)){
		ret = PTR_ERR(device_class);
		goto release_number;
	}
	
	devices = device_create(device_class, NULL, device_number, NULL,"my_device_0");
	
	if(IS_ERR(devices)){
		ret = PTR_ERR(devices);
		goto destroy_class;
	}
	
	pr_info("character_device: Major = %u and Minor = %u \n",MAJOR(device_number), MINOR(device_number));

	return 0;
	
destroy_class:
	 class_destroy(device_class);

release_number:
	unregister_chrdev_region(device_number,1);
	
	return ret;
}

static void __exit my_exit(void)
{
		
	class_destroy(device_class);
	unregister_chrdev_region(device_number,1);
	pr_info("Character_device:Device clean up successful\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
