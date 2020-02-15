#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/time.h>

MODULE_LICENSE("Dual BSD/GPL");

static struct proc_dir_entry *proc_entry; //pointer to proc entry

static ssize_t print_time(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	static struct timespec last_call;
	struct timespec curr_time;
	__kernel_time_t sec_elapsed;
	__kernel_time_t nsec_elapsed;
	curr_time = current_kernel_time();
	printk(KERN_INFO "Current time:\t %ld.%09ld\n",curr_time.tv_sec,curr_time.tv_nsec);
	if(last_call.tv_sec != 0) {
		sec_elapsed = curr_time.tv_sec - last_call.tv_sec;
		nsec_elapsed = curr_time.tv_nsec - last_call.tv_nsec;
		if(nsec_elapsed < 0) {
			sec_elapsed = sec_elapsed - 1;
			nsec_elapsed = 1000000000 + nsec_elapsed;
		}
		printk(KERN_INFO "Elapsed time:\t  %ld.%09ld\n\n",sec_elapsed, nsec_elapsed);
	}
	last_call = curr_time;
	return 0;
}

static struct file_operations procfile_fops =
{
	.owner = THIS_MODULE,
	.read = print_time,
};

static int timer_init(void) {
	printk(KERN_ALERT "Module inserted\n");
	proc_entry = proc_create("timed",0666,NULL,&procfile_fops);
	if(proc_entry == NULL) 
		return -ENOMEM;
	return 0;
}

static void timer_exit(void) {
	printk(KERN_ALERT "Module Removed\n");
	proc_remove(proc_entry);
	return;
}

module_init(timer_init);
module_exit(timer_exit);