#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

static struct hrtimer my_hrt;
static ktime_t interval; 
static ktime_t last;
static unsigned int count;

static enum hrtimer_restart my_callback(struct hrtimer *my_timer)
{
	ktime_t now = ktime_get();
	
	pr_info("\n\nnow = %lld ", now);
	if(++count % 2000 == 0)
	pr_infp("\n\nperiod = %lld\n", ktime_us_delta(now last));
	last = now;

	hrtimer_forward_now(my_timer, interval);
	return HR_TIMER_RESTART;
}



static int __init my_init(void)
{
	 
}

