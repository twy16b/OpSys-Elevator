#ifndef __READHANDLER_H
#define __READHANDLER_H

#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include "elevatoractions.h"
#define BUFSIZE 500

static struct proc_dir_entry* proc_entry;

unsigned long copy_to_user(void __user *to, const void *from, unsigned long size);

//Read handler
static ssize_t print_status(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[BUFSIZE];
	int len = 0, floorloop = 0, petsloop = 0;
	struct list_head *pos;
	struct Passenger *psngr;
	char states[5][10] = { "OFFLINE", "IDLE", "LOADING", "UP", "DOWN" };
	char pets[3][5] = { "None", "Cat", "Dog" };
	if(*ppos > 0 || count < BUFSIZE) return 0;
	if(mutex_lock_interruptible(&global_lock) == 0) {
		len += sprintf(buf+len, "Elevator State: %s\n", states[ELEV_STATE]);
		len += sprintf(buf+len, "ELevator Pet Type: %s\n", pets[getPetType()]);
		len += sprintf(buf+len, "Current Floor: %d\n", ELEV_FLOOR);
		len += sprintf(buf+len, "Number of Passengers: %d\n", ELEV_PSNGRS);
		len += sprintf(buf+len, "Current Weight: %d\n", ELEV_WEIGHT);
		len += sprintf(buf+len, "Passengers Waiting: %d\n", total_waiting());
		len += sprintf(buf+len, "Passengers Serviced: %d\n\n\n", ELEV_SERVICED);
		len += sprintf(buf+len, "Passengers On Elevator:");
		list_for_each(pos, &current_passengers) {
			psngr = list_entry(pos, struct Passenger, mylist);
			len += sprintf(buf+len, " %d", psngr->dest);
			if (psngr->num_pets > 0) {
				if (psngr->pet_type == dog) {
					for(petsloop = 0; petsloop < psngr->num_pets; ++petsloop) {
						len += sprintf(buf+len, "o");
					}
				}
				else {
					for(petsloop = 0; petsloop < psngr->num_pets; ++petsloop) {
						len += sprintf(buf+len, "x");
					}
				}
			}
		}
		len += sprintf(buf+len, "\n");

		for(floorloop = 10; floorloop > 0; --floorloop) {
			len += sprintf(buf+len, "[");
			if(ELEV_FLOOR == floorloop) len += sprintf(buf+len, "*");
			else len += sprintf(buf+len,  " ");
			len += sprintf(buf+len, "] Floor %d:\t%d ", floorloop, waiting_count[floorloop-1]);
			list_for_each(pos, &queue) {
				psngr = list_entry(pos, struct Passenger, mylist);
				if (psngr->start == floorloop) {
					if (psngr->upOrDown == UP) len += sprintf(buf+len, " ^");
					else len += sprintf(buf+len, " v");
					if (psngr->num_pets > 0) {
						len += sprintf(buf+len, "-");
						if (psngr->pet_type == dog) {
							for(petsloop = 0; petsloop < psngr->num_pets; ++petsloop) {
								len += sprintf(buf+len, "o");
							}
						}
						else {
							for(petsloop = 0; petsloop < psngr->num_pets; ++petsloop) {
								len += sprintf(buf+len, "x");
							}
						}
					}
				}
			}
			len += sprintf(buf+len, "\n");
		}
		if(copy_to_user(ubuf,buf,len)) return -EFAULT;
		*ppos = len;
	}
	mutex_unlock(&global_lock);
	return len;
}

static struct file_operations procfile_fops = {
	.owner = THIS_MODULE,
	.read = print_status,
};

#endif
