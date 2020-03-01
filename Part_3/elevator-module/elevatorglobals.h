#ifndef __ELEVATORGLOBALS_H
#define __ELEVATORGLOBALS_H

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
};

#endif
