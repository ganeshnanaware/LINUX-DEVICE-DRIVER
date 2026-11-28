#include <linux/module.h>		// module_init/exit, MODULE_*, THIS_MODULE
#include <linux/init.h>			// __init, __exit, __initdata, __exitdata
#include <linux/moduleparam.h>		// module_param(): load-time param
#include <linux/kernel.h>		// pr_info(), printk(); kernel logging
					
static char *name= "student";

module_param(name, charp, 0444);

static int __init my_init(void)
{
	pr_info("Hello %s ...!\nWelcome to LDD\n",name);
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Hello student: Exiting kernel module\n");
}


module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello student Module");


