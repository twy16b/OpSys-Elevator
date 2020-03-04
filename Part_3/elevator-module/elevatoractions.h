#ifndef __ELEVATORACTIONS_H
#define __ELEVATORACTIONS_H

#include "elevatorglobals.h"

void setElevState(int state) {
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
				ELEV_WEIGHT -= psngr->weight;
				list_del(&psngr->mylist);
				kfree(psngr);
			}
			mutex_unlock(&global_lock);
		}
	}
}

struct Passenger*  elevGetFirstInLine(void) {
	struct list_head *pos;
	struct Passenger *psngr = NULL;
	list_for_each(pos, &queue) {
		psngr = list_entry(pos, struct Passenger, mylist);
		if(psngr->start == ELEV_FLOOR) return psngr;
	}
	return psngr;
}

int inline elevator_load_passenger(struct Passenger psngr) {
	if(ELEV_WEIGHT + psngr.weight > MAXLOAD)
		return 1;
	if(ELEV_PET_TYPE != 0 && psngr.pet_type != ELEV_PET_TYPE)
		return 1;
	ELEV_PSNGRS += 1 + psngr.num_pets;
	ELEV_WEIGHT += psngr.weight;
	return 0;
}

int total_waiting(void) {
	int i, sum = 0;
	for(i = 0; i < 10; ++i) {
		sum += waiting_count[i];
	}
	return sum;
}

#endif
