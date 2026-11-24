/***

Problem Statement:2
Write a Linux kernel module named marks_analyzer.c 
that accepts an array of student marks from user space and calculates statistics.
This assignment introduces array parameters, an important extension of module_param().
Parameter Type Description
marks Integer array Marks of up to 5 students
passing_marks Integer Minimum marks required to pass; 
default 40

Requirements:

1. Accept up to five marks using module_param_array().
2. Determine how many marks were supplied.
3. Validate that each mark is between 0 and 100.
4. Ensure at least one mark is supplied.
5. Calculate the total marks, average, highest mark and lowest mark.
6. Count how many students passed using passing_marks.
7. Reject invalid inputs with -EINVAL.
9. Expose parameters through sysfs with read-only permissions.

***/


#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/moduleparam.h>

static int marks[5];

static int num = 5;
static int min,max;
static int average = 0,sum = 0;

int i,j;
static int count =0;
static int pass_marks = 65;
static int numberofmarks = 0;

module_param_array(marks,int,&num,0444);



static int __init display(void)
{
	//pr_info("Displaying student's Marks\n");
		
	min = marks[0];
	max = marks[0];
	
	if(marks[0] == 0)
	{
		pr_info("There are no marks entered by user\n");
		return 0;
	}
	
	for(j =0; j < num; j++)
	{
		if(marks[j] >= 0 && marks[j] <=100)
		{
			numberofmarks++;
			if(min > marks[j])min = marks[j];
			if(max < marks[j])max = marks[j];
		}
		else
		{
			return -EINVAL;
		}
	}
	
	pr_info("Number of marks entered = %d\n",numberofmarks); 
	
	for(i = 0; i<=num; i++)
	{			
		if(marks[i] >= pass_marks)
		{
			pr_info("Student[%d] marks = %d\n",i,marks[i]);
			count++;
		}else
		{
			pr_info("Student[%d] did not received passing marks = %d\n",i,marks[i]);
		}		
		sum = sum + marks[i];
	}
	
	average = sum/numberofmarks;

	pr_info("Number of students who received passing marks = %d\n",count);
	pr_info("Total Marks = %d\n",sum);
	pr_info("Average Marks = %d\n",average);
	pr_info("Minimum Marks = %d\n", min);
	pr_info("Maximum Marks = %d\n", max);
	return 0;
}

static void __exit display_exit(void)
{
	pr_info("Module Exited..\n");	
}

module_init(display);
module_exit(display_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is module for student marks");
MODULE_AUTHOR("Shantanu");
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
