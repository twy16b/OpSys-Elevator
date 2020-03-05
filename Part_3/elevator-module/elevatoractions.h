#ifndef __ELEVATORACTIONS_H
#define __ELEVATORACTIONS_H

#include "elevatorglobals.h"

int getPetType(void) {
	struct list_head *pos;
	struct Passenger *psngr = NULL;

	if(ELEV_PSNGRS == 0) return none;

	list_for_each(pos, &current_passengers) {
		psngr = list_entry(pos, struct Passenger, mylist);
		if(psngr->num_pets > 0) return psngr->pet_type;
	}
	return none;
}

void elevSetState(int state) {
	if(mutex_lock_interruptible(&global_lock) == 0) ELEV_STATE = state;
	mutex_unlock(&global_lock);
}

void elevMove(int state) {
	ssleep(2);
	if(state == UP) {
		if(mutex_lock_interruptible(&global_lock) == 0) ++ELEV_FLOOR;
	}
	else if(state == DOWN) {
		if(mutex_lock_interruptible(&global_lock) == 0) --ELEV_FLOOR;
	}
	mutex_unlock(&global_lock);
}

void elevUnload(void) {
	struct list_head *pos;
	struct list_head *dummy;
	struct Passenger *psngr;

	list_for_each_safe(pos, dummy, &current_passengers) {
		psngr = list_entry(pos, struct Passenger, mylist);
		if (psngr->dest == ELEV_FLOOR) {
			ssleep(1);
			if(mutex_lock_interruptible(&global_lock) == 0) {
				ELEV_PSNGRS -= 1 + psngr->num_pets;
				ELEV_SERVICED += 1 + psngr->num_pets;
				ELEV_WEIGHT -= psngr->weight;
				list_del(&psngr->mylist);
				kfree(psngr);
			}
			mutex_unlock(&global_lock);
		}
	}
}

int elevLoad(void) {
	struct list_head *pos;
	struct list_head *dummy;
	struct Passenger *temp;
	struct Passenger *psngr;
	
	list_for_each_safe(pos, dummy, &queue) {
		if(ELEV_WEIGHT > 12) return 1;
		temp = list_entry(pos, struct Passenger, mylist);
		if (temp->start == ELEV_FLOOR) {
			if (temp->upOrDown != ELEV_STATE && ELEV_STATE != IDLE && ELEV_STATE != LOADING) continue;
			if (temp->weight + ELEV_WEIGHT > MAXLOAD) continue;
			if (temp->num_pets > 0  && 
				getPetType() != 0 &&
				temp->pet_type != getPetType()) 
					continue;

			elevSetState(LOADING);
			ssleep(1);

			psngr = kmalloc(sizeof(struct Passenger), __GFP_RECLAIM);
			if(!psngr) {
				printk(KERN_ALERT "NO MEMORY LEFT!\n");
				return -ENOMEM;
			}

			*psngr = *temp;
			INIT_LIST_HEAD(&psngr->mylist);

			if(mutex_lock_interruptible(&global_lock) == 0) {
				ELEV_PSNGRS += 1 + psngr->num_pets;
				ELEV_WEIGHT += psngr->weight;
				ELEV_PET_TYPE = psngr->pet_type;
				list_add_tail(&psngr->mylist, &current_passengers);
				waiting_count[ELEV_FLOOR-1] -= 1 + psngr->num_pets;
				list_del(&temp->mylist);
			}
			mutex_unlock(&global_lock);
		}
	}
	return 0;	
}

void elevClear(void) {
	struct list_head *pos;
	struct list_head *dummy;
	struct Passenger *psngr;

	list_for_each_safe(pos, dummy, &current_passengers) {
		psngr = list_entry(pos, struct Passenger, mylist);
		if(mutex_lock_interruptible(&global_lock) == 0) {
			list_del(&psngr->mylist);
			kfree(psngr);
		}
		mutex_unlock(&global_lock);
	}

	list_for_each_safe(pos, dummy, &queue) {
		psngr = list_entry(pos, struct Passenger, mylist);
		if(mutex_lock_interruptible(&global_lock) == 0) {
			list_del(&psngr->mylist);
			kfree(psngr);
		}
		mutex_unlock(&global_lock);
	}
}

int total_waiting(void) {
	int i, sum = 0;
	for(i = 0; i < 10; ++i) {
		sum += waiting_count[i];
	}
	return sum;
}

#endif
