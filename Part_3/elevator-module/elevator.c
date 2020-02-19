#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include "elevatorglobals.h"
#include "readhandler.h"
MODULE_LICENSE("GPL");

#define MAXLOAD 15

//Passenger linked list object
struct Passenger{
	int num_pets;
	int pet_type;
	int weight;
	int start;
	int dest;
};

extern int (*STUB_start_elevator)(void);
int start_elevator(void) {
	printk(KERN_NOTICE "%s: Start elevator syscall\n", __FUNCTION__);
	ELEV_STATE = IDLE;
	ELEV_SHUTDOWN = 0;
	ELEV_PET_TYPE = none;
	ELEV_FLOOR = 1;
	ELEV_PSNGRS = 0;
	ELEV_WEIGHT = 0;
	ELEV_SERVICED = 0;
	return 0;
}

extern int (*STUB_issue_request)(int, int, int, int);
int issue_request(int num_pets, int pet_type, int start, int dest) {
	struct Passenger psngr = {num_pets, pet_type, 3+(num_pets*pet_type), start, dest};

	if(num_pets < 0 || num_pets > 3 ||
	pet_type < 0 || pet_type > 2 ||
	start < 1 || start > 10 ||
	dest < 1 || dest > 10)
	{
		printk(KERN_NOTICE "%s: Invalid arguments\n", __FUNCTION__);
		return 1;
	}

	printk(KERN_NOTICE "---------------------------------\n");
	printk(KERN_NOTICE "Request issued:\n");
	printk(KERN_NOTICE "NumPets: %d\n", psngr.num_pets);
	if(psngr.pet_type == 0) printk(KERN_NOTICE "Pet Type: None\n");
	if(psngr.pet_type == 1) printk(KERN_NOTICE "Pet Type: Cat\n");
	if(psngr.pet_type == 2) printk(KERN_NOTICE "Pet Type: Dog\n");
	printk(KERN_NOTICE "Weight: %d\n", psngr.weight);
	printk(KERN_NOTICE "Start: %d\n", psngr.start);
	printk(KERN_NOTICE "Destination: %d\n", psngr.dest);
	printk(KERN_NOTICE "---------------------------------\n");
	if(ELEV_SHUTDOWN == 1 || ELEV_STATE == 0 || ELEV_WEIGHT + psngr.weight > MAXLOAD) return 0;
	ELEV_PSNGRS += 1+psngr.num_pets;
	ELEV_PET_TYPE = psngr.pet_type;
	ELEV_WEIGHT += psngr.weight;
	ELEV_FLOOR = psngr.start;
	return 0;
}

extern int (*STUB_stop_elevator)(void);
int stop_elevator(void) {
	printk(KERN_NOTICE "%s: Stop elevator syscall\n", __FUNCTION__);
	if (ELEV_SHUTDOWN == 1) return 1;
	ELEV_SHUTDOWN = 1;
	while(ELEV_PSNGRS > 0) {
		//unload all passengers at their destinations
		ELEV_PSNGRS--;
		ELEV_WEIGHT--;
	}
	ELEV_STATE = OFFLINE;
	ELEV_PET_TYPE = none;
	ELEV_FLOOR = 1;
	return 0;
}

static int syscallModule_init(void) {
	printk(KERN_ALERT "Module inserted\n");
	proc_entry = proc_create("elevator", 0666, NULL, &procfile_fops);
	if(proc_entry == NULL) return -ENOMEM;
	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;
	return 0;
}
module_init(syscallModule_init);

static void syscallModule_exit(void) {
	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;
	proc_remove(proc_entry);
	printk(KERN_ALERT "Module removed\n");
}
module_exit(syscallModule_exit);

