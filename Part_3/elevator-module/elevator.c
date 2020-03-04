#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "readhandler.h"
MODULE_LICENSE("GPL");

//-------------------------------------------------------------------------------------

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
	struct Passenger *first_in_line;
	struct Passenger *psngr;

	//Elevator process loop
	while(!kthread_should_stop()) {

		//Decide where to go
		if (ELEV_PSNGRS > 0) {
			psngr = list_first_entry(&current_passengers, struct Passenger, mylist);
			if(ELEV_FLOOR < psngr->dest) setElevState(UP);
			if(ELEV_FLOOR > psngr->dest) setElevState(DOWN);
		}
		else if(total_waiting() > 0) {
			psngr = list_first_entry(&queue, struct Passenger, mylist);
			if (ELEV_FLOOR < psngr->start) setElevState(UP);
			if (ELEV_FLOOR > psngr->start) setElevState(DOWN);
		}
		else setElevState(IDLE);

		//Unload Passengers
		elevUnload();

		//Pick up passengers
		while(waiting_count[ELEV_FLOOR-1] > 0) {

			first_in_line = elevGetFirstInLine();

			if(first_in_line->weight + ELEV_WEIGHT > MAXLOAD || (first_in_line->pet_type != ELEV_PET_TYPE && ELEV_PET_TYPE != 0)) break;

			setElevState(LOADING);
			ssleep(1);

			psngr = kmalloc(sizeof(struct Passenger), __GFP_RECLAIM);
			if(!psngr) {
				printk(KERN_ALERT "NO MEMORY LEFT!\n");
				return -ENOMEM;
			}

			*psngr = *first_in_line;
			INIT_LIST_HEAD(&psngr->mylist);

			if(mutex_lock_interruptible(&global_lock) == 0) {
				ELEV_PSNGRS += 1 + psngr->num_pets;
				ELEV_WEIGHT += psngr->weight;
				ELEV_PET_TYPE = psngr->pet_type;
				list_add_tail(&psngr->mylist, &current_passengers);
				waiting_count[ELEV_FLOOR-1] -= 1 + psngr->num_pets;
				list_del(&first_in_line->mylist);
			}
			mutex_unlock(&global_lock);

		//Move floors
		if(ELEV_STATE == UP) elevMove(UP);
		if(ELEV_STATE == DOWN) elevMove(DOWN);

		}
	}

	//Elevator shutdown protocol

	//Unload all passengers at their destinations
	while (ELEV_PSNGRS > 0) {
		psngr = list_first_entry(&current_passengers, struct Passenger, mylist);
		if(ELEV_FLOOR < psngr->dest) setElevState(UP);
		if(ELEV_FLOOR > psngr->dest) setElevState(DOWN);
		if(ELEV_STATE == UP) elevMove(UP);
		if(ELEV_STATE == DOWN) elevMove(DOWN);
		elevUnload();
	}
	setElevState(OFFLINE);

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
	ELEV_PSNGRS = 0;
	ELEV_WEIGHT = 0;
	ELEV_SERVICED = 0;

	thread_init_parameter(&elevator_thread);

	return 0;
}

extern int (*STUB_issue_request)(int, int, int, int);
int issue_request(int num_pets, int pet_type, int start, int dest) {
	struct Passenger *psngr;

	//--------------Check for valid arguments
	if(num_pets < 0 || num_pets > 3 ||
	pet_type < 0 || pet_type > 2 ||
	start < 1 || start > 10 ||
	dest < 1 || dest > 10)
	{
		printk(KERN_NOTICE "%s: Invalid arguments\n", __FUNCTION__);
		return 1;
	}

	psngr = kmalloc(sizeof(struct Passenger), __GFP_RECLAIM);
	if(!psngr) {
		printk(KERN_ALERT "NO MEMORY LEFT!\n");
		return -ENOMEM;
	}

	psngr->num_pets = num_pets;
	psngr->pet_type = pet_type;
	psngr->weight = 3+(pet_type*num_pets);
	psngr->start = start;
	psngr->dest = dest;
	INIT_LIST_HEAD(&psngr->mylist);

	if(mutex_lock_interruptible(&global_lock) == 0) {
		list_add_tail(&psngr->mylist, &queue);
		waiting_count[psngr->start-1] += 1 + psngr->num_pets;
	}
	mutex_unlock(&global_lock);

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

	INIT_LIST_HEAD(&current_passengers);
	INIT_LIST_HEAD(&queue);

	mutex_init(&global_lock);

	ELEV_FLOOR = 1;
	setElevState(OFFLINE);

	return 0;
}
module_init(elevator_init);

//-----------------------------Elevator Module Exit------------------------------------
static void elevator_exit(void) {
	stop_elevator();

	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;

	mutex_destroy(&global_lock);

	proc_remove(proc_entry);
	printk(KERN_ALERT "Module removed\n");
}
module_exit(elevator_exit);
