#ifndef __READHANDLER_H
#define __READHANDLER_H

#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include "elevatorglobals.h"
#define BUFSIZE 500

static struct proc_dir_entry* proc_entry;

unsigned long copy_to_user(void __user *to, const void *from, unsigned long size);

//Read handler
static ssize_t print_status(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	struct list_head *temp;
	Passenger* p;
	int psngr_cnt;
	char buf[BUFSIZE];
	int len = 0, i = 0, j = 0;
	char states[5][10] = { "OFFLINE", "IDLE", "LOADING", "UP", "DOWN" };
	char pets[3][5] = { "None", "Cat", "Dog" };
	if(*ppos > 0 || count < BUFSIZE) return 0;
	len += sprintf(buf+len, "Elevator State: %s\n", states[ELEV_STATE]);
	len += sprintf(buf+len, "ELevator Pet Type: %s\n", pets[ELEV_PET_TYPE]);
	len += sprintf(buf+len, "Current Floor: %d\n", ELEV_FLOOR);
	len += sprintf(buf+len, "Number of Passengers: %d\n", ELEV_PSNGRS);
	len += sprintf(buf+len, "Current Weight: %d\n", ELEV_WEIGHT);
	len += sprintf(buf+len, "Passengers Waiting: %d\n", PASSENGERS_WAITING);
	len += sprintf(buf+len, "Passengers Serviced: %d\n\n\n", ELEV_SERVICED);
	for(i = 10; i > 0; --i) {
		psngr_cnt = 0;
		len += sprintf(buf+len, "[");
		if(ELEV_FLOOR == i) len += sprintf(buf+len, "*");
		else len += sprintf(buf+len,  " ");
		list_for_each(temp, &floor_list[i-1]){
			if((p = list_entry(temp, Passenger, list)) != NULL){
				psngr_cnt++;
				psngr_cnt += p->num_pets;
			}
		}
		len += sprintf(buf+len, "] Floor %d:\t%d\t", i, psngr_cnt);
		list_for_each(temp, &floor_list[i-1]){
                        if((p = list_entry(temp, Passenger, list)) != NULL){
                                len += sprintf(buf+len, "| ");
				if(p->pet_type == 1){	//if cat, x
					for(j = 0; j < p->num_pets; j++){
						len += sprintf(buf+len, "x ");
					}
				}
				else if(p->pet_type == 2){	//if dog, o
					for(j = 0; j < p->num_pets; j++){
                                                len += sprintf(buf+len, "o ");
                                        }
				}
                        }
                }
		len += sprintf(buf+len, "\n");
	}
	if(copy_to_user(ubuf,buf,len)) return -EFAULT;
	*ppos = len;
	return len;
}

static struct file_operations procfile_fops = {
	.owner = THIS_MODULE,
	.read = print_status,
};

#endif

