/****
Problem statement: 1 
Write a Linux kernel module named hello_student.c 
that accepts a student's name as a parameter during module insertion.
Requirements:
Accept a string parameter named student_name.
Default value must be "World".
Print Hello <student_name>, Welcome to Linux Kernel Programming! when the module loads.
****/


#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/moduleparam.h>


static char *name = "World";

module_param(name, charp, 0444);

static int __init display(void)
{
	pr_info("Hello %s, Welcome to Linux Kernel Programming! when the module loads...\n",name);
	return 0;
}

static void __exit diplay_exit(void)
{
	pr_info("Exited successfully.....\n");
}

module_init(display);
module_exit(diplay_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is hello student module");
MODULE_AUTHOR("GANESH");
