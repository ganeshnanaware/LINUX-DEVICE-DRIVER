#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list my_timer;
static int count;

static void my_callback(struct timer_list *t)
{
    count++;
    pr_info("timer fired: %d\n", count);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(10000));   /* re-arm */
}

static int __init timer_init(void)
{
    timer_setup(&my_timer, my_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(10000));
    pr_info("timer module loaded\n");
    return 0;
}

static void __exit timer_exit(void)
{
    del_timer_sync(&my_timer);   /* must stop it before unload */
    pr_info("timer module removed\n");
}

module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");      
		
