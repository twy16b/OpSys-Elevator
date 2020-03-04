#ifndef __ELEVATORGLOBALS_H
#define __ELEVATORGLOBALS_H

#include <linux/list.h>
#include <linux/mutex.h>

#define MAXLOAD 15

int ELEV_STATE;
enum STATE {OFFLINE = 0, IDLE, LOADING, UP, DOWN};
int ELEV_SHUTDOWN;

int ELEV_PET_TYPE;
enum PET_TYPE {none = 0, cat, dog};

int ELEV_FLOOR;
int ELEV_PSNGRS;
int ELEV_WEIGHT;
int ELEV_SERVICED;

//Passenger linked list object
struct Passenger{
	int num_pets;
	int pet_type;
	int weight;
	int start;
	int dest;
	struct list_head mylist;
};

int waiting_count[10];
struct list_head floors[10];
struct list_head queue;
struct list_head current_passengers;

struct mutex global_lock;

int UpDown;

#endif
