/*
 * Question: Add ioctl code in the beginner character driver
 * (manual cdev registration and file operations).
 */

#include <linux/module.h>		//module entry point and access point
#include <linux/kernel.h>		//
#include <linux/init.h>			//
#include <linux/fs.h>			//file objects
#include <linux/cdev.h>			//struct cdev, cdev_init(), cdev_add()
#include <linux/device.h>		//Class create, device create
#include <linux/err.h>			//IS_ERR, PTR_ERR
#include <linux/container_of.h>		//Recover device object address
#include <linux/slab.h>			//Allocate free memory
#include <linux/uaccess.h>		//copy across the userspace, get_user, put_user
#include <linux/ioctl.h>		//_IO, _IOR, _IOW macros

#define DEVICE_NAME "beginner"
#define CLASS_NAME "beginner_class"
#define CAPACITY 64

/* ioctl commands. User space program must use the same definitions */
#define BEGINNER_MAGIC		'B'
#define BEGINNER_CLEAR		_IO(BEGINNER_MAGIC, 0)			//clear buffer, length becomes 0
#define BEGINNER_GET_LENGTH	_IOR(BEGINNER_MAGIC, 1, unsigned int)	//read valid data length
#define BEGINNER_SET_LENGTH	_IOW(BEGINNER_MAGIC, 2, unsigned int)	//change valid data length
#define BEGINNER_GET_CAPACITY	_IOR(BEGINNER_MAGIC, 3, unsigned int)	//read buffer capacity
#define BEGINNER_MAX_NR		3					//highest command number

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

static int my_open(struct inode *inode, struct file *file)
{
	struct session *s;		//this will point to open location
	
	struct demo_device *dev;

	static const char initial[] = "hello from kernel...!!!!\n";

	dev = container_of(inode->i_cdev, struct demo_device, cdev);
	
	if(file->f_flags & (O_APPEND | O_TRUNC))
	{
		return -EINVAL;
	}

	if(dev->fail_open)
	{
		return -ENOMEM;
	}
	s = kzalloc(sizeof(*s),GFP_KERNEL);	//allocate and zero 

       if(!s)
       {
       	return -ENOMEM;
       }       
	
       memcpy(s->data, initial, sizeof(initial)-1);

       s->length = sizeof(initial)-1;

       s->owner = dev;

       file->private_data = s;

       return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	file->private_data = NULL;
	return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t*ppos)
{
	struct session *s = file->private_data;
	size_t pos, available, wanted, copied;
	
	if(!count)
		return 0;
	if(*ppos <0)
		return -EINVAL;
	if(*ppos > s->length)
		return 0;
	pos = (size_t)*ppos;
	available =  s->length - pos;
	wanted = min(count,available);
		copied = copy_to_user(buf, s->data+pos, wanted);
	if(copied)
		return -EFAULT;
	*ppos += wanted;
	return wanted;
}


static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct session *s = file->private_data;
	
	u8 temporary[CAPACITY];
	
	size_t pos, available, wanted, copied;
	
	if(file->f_flags & O_APPEND)
		return -EINVAL;
	if(!count)
		return 0;
	if(*ppos <0)
		return -EINVAL;
	if(*ppos >= CAPACITY)
		return -ENOSPC;
	
	pos = (size_t)*ppos;
	available = CAPACITY - pos;
	wanted = min(count,available);
		copied = copy_from_user(temporary, buf, wanted);
	if(copied)
		return -EFAULT;

	memcpy(s->data+pos, temporary, wanted);
	pr_info("beginner: wrote %zu bytes at pos %zu : %.*s\n", wanted, pos, (int)wanted, (char *)temporary);
	*ppos += wanted;
	if(*ppos > s->length)
		s->length = *ppos;
	return wanted;
}

static loff_t my_seek(struct file *file, loff_t offset, int whence)
{
	struct session *s = file->private_data;	//get this open's buffer
	loff_t max_pos = CAPACITY-1;
	loff_t base;
	loff_t new_pos;				//position after the seek

	switch(whence)
	{
		case SEEK_SET:			//measure from the start of the buffer
			base=0;
			break;
		case SEEK_CUR:			//measure from the current position
			base = file->f_pos;
			break;
		case SEEK_END:			//measure from the end of valid data
			base = s->length;
			break;
		default:			//unknown whence value
			return -EINVAL;
	}

	if(offset < -base || offset > max_pos - base)	//before the start or past the buffer
		return -EINVAL;
	
	new_pos = base + offset;
	file->f_pos = new_pos;		//next read or write starts here
	return new_pos;			//lseek returns the new position
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct session *s = file->private_data;			//get this open's buffer
	unsigned int __user *uptr = (unsigned int __user *)arg;	//arg is a user space address
	unsigned int value;					//value copied to/from user

	if(_IOC_TYPE(cmd) != BEGINNER_MAGIC)		//command is not for this driver
		return -ENOTTY;
	if(_IOC_NR(cmd) > BEGINNER_MAX_NR)		//command number out of range
		return -ENOTTY;

	switch(cmd)
	{
		case BEGINNER_CLEAR:			//empty the buffer
			memset(s->data, 0, CAPACITY);
			s->length = 0;
			pr_info("beginner: ioctl CLEAR\n");
			break;

		case BEGINNER_GET_LENGTH:		//kernel -> user
			value = s->length;
			if(put_user(value, uptr))
				return -EFAULT;
			pr_info("beginner: ioctl GET_LENGTH %u\n", value);
			break;

		case BEGINNER_SET_LENGTH:		//user -> kernel
			if(get_user(value, uptr))
				return -EFAULT;
			if(value > CAPACITY)
				return -EINVAL;
			if(value > s->length)		//growing, so zero the new part
				memset(s->data + s->length, 0, value - s->length);
			s->length = value;
			pr_info("beginner: ioctl SET_LENGTH %u\n", value);
			break;

		case BEGINNER_GET_CAPACITY:		//kernel -> user
			value = CAPACITY;
			if(put_user(value, uptr))
				return -EFAULT;
			pr_info("beginner: ioctl GET_CAPACITY %u\n", value);
			break;

		default:				//unknown command
			return -ENOTTY;
	}

	return 0;
}

static const struct file_operations demo_operation = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
	//.llseek = my_seek,
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

	cdev_init(&demo.cdev, &demo_operation);
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
	device_destroy(demo.class, demo.number);
	class_destroy(demo.class);
	cdev_del(&demo.cdev);
	unregister_chrdev_region(demo.number, 1);
}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Manual cdev registration and file operation implementaion");

MODULE_AUTHOR("GANESHA");
