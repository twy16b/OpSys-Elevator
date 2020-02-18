#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#define BUF_LEN 100

MODULE_LICENSE("Dual BSD/GPL");

static struct proc_dir_entry *proc_entry; //pointer to proc entry

unsigned long copy_to_user(void __user *to, const void *from, unsigned long size);

static ssize_t print_time(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	static struct timespec last_call;
	struct timespec curr_time;
	__kernel_time_t sec_elapsed;
	__kernel_time_t nsec_elapsed;
	char buf[BUF_LEN];
	int len=0;
	
	if(*ppos > 0 || count < BUF_LEN) return 0;
	
	curr_time = current_kernel_time();
	len += sprintf(buf,"Current time:\t%ld.%09ld\n",curr_time.tv_sec,curr_time.tv_nsec);
	
	if(last_call.tv_sec != 0) {
		sec_elapsed = curr_time.tv_sec - last_call.tv_sec;
		nsec_elapsed = curr_time.tv_nsec - last_call.tv_nsec;
		if(nsec_elapsed < 0) {
			sec_elapsed = sec_elapsed - 1;
			nsec_elapsed = 1000000000 + nsec_elapsed;
		}
		len += sprintf(buf+len, "Elapsed time:\t%ld.%09ld\n",sec_elapsed, nsec_elapsed);
	}
	last_call = curr_time;
	
	if(copy_to_user(ubuf, buf, len)) return -EFAULT; //Send buffer to user
	
	printk(KERN_NOTICE "%s", buf); //Print buffer in log

	*ppos = len;

	return len;
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