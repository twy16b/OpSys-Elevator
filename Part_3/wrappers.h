#ifndef __WRAPPERS_H
#define __WRAPPERS_H

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>

#define __NR_START_ELEVATOR 335
#define __NR_ISSUE_REQUEST 336
#define __NR_STOP_ELEVATOR 337

int start_elevator() {
	return syscall(__NR_START_ELEVATOR);
}

int issue_request(int num_pets, int pet_type, int start, int dest) {
	return syscall(__NR_ISSUE_REQUEST, num_pets, pet_type, start, dest);
}

int stop_elevator() {
	return syscall(__NR_STOP_ELEVATOR);
}

#endif
