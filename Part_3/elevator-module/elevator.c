#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include "elevatorglobals.h"
#include "elevatoractions.h"
#include "readhandler.h"
MODULE_LICENSE("GPL");

//-------------------------------------------------------------------------------------

//----------------------------------Thread Structs-------------------------------------
struct thread_parameter {
	int id;
	struct task_struct *kthread;
};
struct thread_parameter elevator_thread;

//-----------------------------Elevator Thread Function--------------------------------
int run_elevator(void *data) {
	//struct thread_parameter *parm = data;

	//Elevator process loop
	while(!kthread_should_stop()) {

		//TODO: Schedule pickup, move to floor

		//TODO: Pick up passengers

		//TODO: Deliver passengers
	}

	//Elevator shutdown protocol

	//TODO: unload all passengers at their destinations

	while(ELEV_FLOOR > 1) {--ELEV_FLOOR; ssleep(2);}

	ELEV_STATE = OFFLINE;

	return 0;
}

//------------------------------Start Thread-------------------------------------------
void thread_init_parameter(struct thread_parameter *parm) {
	static int id = 1;
	parm->id = id++;
	parm->kthread = kthread_run(run_elevator, parm, "Elevator thread %d", parm->id);
}

//------------------------Start Elevator System Call------------------------------------
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

	thread_init_parameter(&elevator_thread);

	return 0;
}

extern int (*STUB_issue_request)(int, int, int, int);
int issue_request(int num_pets, int pet_type, int start, int dest) {
	struct Passenger psngr = {num_pets, pet_type, 3+(num_pets*pet_type), start, dest};

	//--------------Check for valid arguments
	if(num_pets < 0 || num_pets > 3 ||
	pet_type < 0 || pet_type > 2 ||
	start < 1 || start > 10 ||
	dest < 1 || dest > 10)
	{
		printk(KERN_NOTICE "%s: Invalid arguments\n", __FUNCTION__);
		return 1;
	}

	//--------------Print request to kernel log
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


	//TODO: Place passenger in list for start floor

	return 0;
}

//--------------------------Stop Elevator System Call----------------------------------
extern int (*STUB_stop_elevator)(void);
int stop_elevator(void) {
	printk(KERN_NOTICE "%s: Stop elevator syscall\n", __FUNCTION__);

	if (ELEV_STATE == OFFLINE) return 1;

	kthread_stop(elevator_thread.kthread);

	return 0;
}

//-----------------------------Elevator Module Init------------------------------------
static int elevator_init(void) {
	printk(KERN_ALERT "Module inserted\n");

	proc_entry = proc_create("elevator", 0666, NULL, &procfile_fops);
	if(proc_entry == NULL) return -ENOMEM;

	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;

	ELEV_STATE = OFFLINE;

	return 0;
}
module_init(elevator_init);

//-----------------------------Elevator Module Exit------------------------------------
static void elevator_exit(void) {
	stop_elevator();

	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;

	proc_remove(proc_entry);
	printk(KERN_ALERT "Module removed\n");
}
module_exit(elevator_exit);
