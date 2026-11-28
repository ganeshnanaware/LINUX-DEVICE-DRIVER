#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

static int i, count = 0;
static int num=5;
static int marks[5];

module_param_array(marks, int, &num, 0444);

static int __init my_init(void)
{
/*	if(marks[0] == 0)
	{
		pr_info("There are no elements entered\n");
	}
*/	
	count = 0;
	for (i = 0; i < num; i++)
	{
		pr_info("Student_%d, Marks = %d\n", i+1, marks[i]);
		count++;
	}
	pr_info("Count = %d\n",count);
	return 0;
}


static void __exit my_exit(void)
{
	pr_info("Mark Analuzer: Exited\n");
}


module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("student Mark Analyzer");

