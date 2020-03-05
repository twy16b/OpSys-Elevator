# OpSys-Elevator

## Part 1

The file **empty.c** contains the bare minimum code that will allow a C program to compile. It will produce 25 system calls. This is shown by running 

`gcc -o empty.x empty.c`

`strace -o empty.trace ./empty.x`

`wc -l empty.trace`

The file **part1.c** contains `#include <stdio.h>` and a single `printf()` statement which together add 5 system calls to the program, making 30 total. This is shown by running 

`gcc -o part1.x part1.c`

`strace -o part1.trace ./part1.x`

`wc -l part1.trace`

## Part 2

The file **my_timer.c** is compiled with the **Makefile** using `make`.

This generates a **my_timer.ko** kernel module file. It is inserted with `sudo insmod my_timer.ko` or `make insert`.

Once inserted, you may run `cat /proc/timed` to print the current kernel time.

Every subsequent `cat /proc/timed` will print the current kernel time as well as the elapsed time since the last `cat /proc/timed`.

## Part 3 

### Elevator Module

The `elevator` folder should be placed in `/usr/src/`. It can be compiled by running `make` in that folder.

Insert the module using `sudo insmod elevator.ko` in `elevator` or `sudo make insert` using the provided userspace Makefile.

Once inserted, you may invoke `issue_request(int, int, int, int)` calls with `make issue` at any time regardless if the elevator is active or not.

When you are ready to start the elevator, invoke `start_elevator(void)` with `make start` to begin.

On each floor, the elevator will go through a loop as follows:

- Unload all current passengers that want to get off
- Decide which direction it wants to go
- Load compatible passengers.

If there are no passengers inside the elevator or waiting on any floors, the elevator returns to IDLE state until a new request comes.

The user may invoke `stop_elevator(void)` at any time. Once this shutdown process begins, the elevator will pick up no new passengers and deliver all passengers currently on board. Once the elevator is empty, it goes offline.

If the module is removed, all current passengers and those waiting in line are freed from memory to avoid leakage.

### /proc/elevator

The proc entry for this module will print out all needed information at any given time.

- The elevator's movement state:
- The type of animals on the elevator, if any
- The current floor the elevator is on
- The elevator's current load (in terms of both passengers units and weight units)
- The total number of passengers waiting
- The number of passengers serviced

Human passengers are denoted by either a '^' or a 'v' depending on whether they wish to go up or down respectively.

Animal passengers are attached to their owner with a '-' leash. Cats are represented by an 'x' and dogs are represented by an 'o'.

Once loaded onto the elevator, the passenger will be displayed above the floors in the **Passengers on Elevator** section. The number denotes which floor they wish to get off and the pet symbols remain the same.
