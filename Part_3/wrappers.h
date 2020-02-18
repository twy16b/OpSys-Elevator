#ifndef __WRAPPERS_H
#define __WRAPPERS_H

#define _GNU_SOURCE
#include <unistd.h>

int ELEV_STATE;
enum STATE {offline = 0, idle, loading, up, down};
int ELEV_SHUTDOWN;

int ELEV_PET_TYPE;
enum PET_TYPE {none = 0, cat, dog};

int ELEV_FLOOR;
int ELEV_PSNGRS;
int ELEV_WEIGHT;
int ELEV_SERVICED;

struct Passenger{
	int num_pets;
	int pet_type;
	int weight;
	int start;
	int dest;
};

int start_elevator(void) {
	printf( "%s: Start elevator syscall\n", __FUNCTION__);
	ELEV_STATE = idle;
	ELEV_SHUTDOWN = 0;
	ELEV_PET_TYPE = none;
	ELEV_FLOOR = 1;
	ELEV_PSNGRS = 0;
	ELEV_WEIGHT = 0;
	ELEV_SERVICED = 0;
	return 0;
}

int issue_request(int num_pets, int pet_type, int start, int dest) {
	struct Passenger psngr = {num_pets, pet_type, 3+(num_pets*pet_type), start, dest};

	if(num_pets < 0 || num_pets > 3 ||
	pet_type < 0 || pet_type > 2 ||
	start < 1 || start > 10 ||
	dest < 1 || dest > 10)
	{
		printf( "%s: Invalid arguments\n", __FUNCTION__);
		return 1;
	}

	printf( "---------------------------------\n");
	printf( "Request issued:\n");
	printf( "NumPets: %d\n", psngr.num_pets);
	if(psngr.pet_type == 0) printf( "Pet Type: None\n");
	if(psngr.pet_type == 1) printf( "Pet Type: Cat\n");
	if(psngr.pet_type == 2) printf( "Pet Type: Dog\n");
	printf( "Weight: %d\n", psngr.weight);
	printf( "Start: %d\n", psngr.start);
	printf( "Destination: %d\n", psngr.dest);
	printf( "---------------------------------\n");
	return 0;
}

int stop_elevator(void) {
	printf( "%s: Stop elevator syscall\n", __FUNCTION__);
	if (ELEV_SHUTDOWN == 1) return 1;
	ELEV_SHUTDOWN = 1;
	while(ELEV_PSNGRS > 0) {
		//unload all passengers at their destinations
	}
	ELEV_STATE = offline;
	return 0;
}
#endif