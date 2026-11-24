#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/errno.h>

static char *name = "Student";

static int count = 1;

module_param(name, charp, 0444);
module_param(count, int, 0444);

static int __init hello_init(void)
{
	if(count >= 7)
	{
		pr_err("Error Occured\n");
		return 0;
	}
	else
	for(int i = 0 ; i < count; i++)
	{
		pr_info("count = %d, name =  %s\n",i,name);
	}
		return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Bye linux kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic linux kernel module for parameter");
MODULE_AUTHOR("Shantanu");

