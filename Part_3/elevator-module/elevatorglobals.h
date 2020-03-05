#ifndef __ELEVATORGLOBALS_H
#define __ELEVATORGLOBALS_H
#include <linux/list.h>

#define MAXLOAD 15

struct list_head floor_list[10];

int ELEV_STATE;
enum STATE {OFFLINE = 0, IDLE, LOADING, UP, DOWN};
int ELEV_SHUTDOWN;

int ELEV_PET_TYPE;
enum PET_TYPE {none = 0, cat, dog};

int ELEV_FLOOR;
int ELEV_PSNGRS;
int ELEV_WEIGHT;
int ELEV_SERVICED;

int PASSENGERS_WAITING;

//List of passengers inside elevator
struct{
	int total_weight;
	int total_passengers;
	struct list_head list;
}passengers;

//Passenger linked list object
typedef struct Passenger{
	int num_pets;
	int pet_type;
	int weight;
	int start;
	int dest;
	struct list_head list;
} Passenger;

#endif


