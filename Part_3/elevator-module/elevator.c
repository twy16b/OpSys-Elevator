#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "elevatorglobals.h"
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
	Passenger* p;

	//Elevator process loop
	while(!kthread_should_stop()) {

		do{
			ELEV_STATE = LOADING;
			if((p = list_first_entry(&floor_list[ELEV_FLOOR - 1], Passenger, list)) != NULL){
				struct list_head *temp = NULL;
				list_add_tail(&p->list, &passengers.list);

				passengers.total_weight += p->weight;
				passengers.total_passengers += 1;

				p = list_entry(temp, Passenger, list);
				list_del(temp);
				kfree(p);
				PASSENGERS_WAITING--;
			}

		}while(PASSENGERS_WAITING != 0);

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
        Passenger* p;

	//--------------Check for valid arguments
	if(num_pets < 0 || num_pets > 3 ||
	pet_type < 0 || pet_type > 2 ||
	start < 1 || start > 10 ||
	dest < 1 || dest > 10)
	{
		printk(KERN_NOTICE "%s: Invalid arguments\n", __FUNCTION__);
		return 1;
	}

	p = kmalloc(sizeof(Passenger) * 1, __GFP_RECLAIM);
	if(p == NULL)
		return -ENOMEM;

        p->num_pets = num_pets;
        p->pet_type = pet_type;
        p->start = start;
        p->dest = dest;

        p->weight = 3 + (num_pets * pet_type);

	//--------------Print request to kernel log
	printk(KERN_NOTICE "---------------------------------\n");
	printk(KERN_NOTICE "Request issued:\n");
	printk(KERN_NOTICE "NumPets: %d\n", p->num_pets);
	if(p->pet_type == 0) printk(KERN_NOTICE "Pet Type: None\n");
	if(p->pet_type == 1) printk(KERN_NOTICE "Pet Type: Cat\n");
	if(p->pet_type == 2) printk(KERN_NOTICE "Pet Type: Dog\n");
	printk(KERN_NOTICE "Weight: %d\n", p->weight);
	printk(KERN_NOTICE "Start: %d\n", p->start);
	printk(KERN_NOTICE "Destination: %d\n", p->dest);
	printk(KERN_NOTICE "---------------------------------\n");


	//TODO: Place passenger in list for start floor.
	//We need to place the passengers in the

	list_add_tail(&p->list, &floor_list[p->start - 1]);
	PASSENGERS_WAITING++;

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
	int i;
	printk(KERN_ALERT "Module inserted\n");

	passengers.total_weight = 0;
	passengers.total_passengers = 0;
	INIT_LIST_HEAD(&passengers.list);			//initialize the list of passengers inside the elevator
	for(i = 0; i < 10; i++){
		INIT_LIST_HEAD(&floor_list[i]);			//initialize the list, which is the queue of passengers waiting in each floor
	}
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

