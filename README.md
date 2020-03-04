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

The elevator module can be compiled by running `make` in the containing folder along with all the header files.

Insert the module using `sudo insmod elevator.ko` after it has compiled.

Once inserted, you may invoke the `issue_request(int, int, int, int)` syscall to queue some passengers or immediately invoke the `start_elevator(void)` syscall to put it in idle.

Once `start_elevator(void)` is called, the elevator will run indefinitely in a kthread delivering the current passengers until it is empty. If it is empty, it will search for passengers to pick up based on the earliest request.

The `issue_request(int, int, int, int)` can be called at any time and as many times as needed, however if there are too many requests at one time the kernel may run out of memory and crash.

On each floor, it will first unload all passengers that want to get off. Then, it will pick up passengers if possible. There are 2 possible functions that can be used to pick up passengers. 

- The first, `elevLoadFast(void)`, will deliver more passengers over a period of time. However, it has a tendency to stick to one pet type if requests keep coming in. This makes it possible for one pet type to be neglected for a long period, even if they are at the front of the line.
- The other function, `elevLoadFair(void)` is a bit slower to deliver passengers as it will only pick up people at the front of the line, but this ensures people in the front of the line do not get skipped over repeatedly because of their pet type.

The user may invoke `stop_elevator(void)` at any time. Once this shutdown process begins, the elevator will pick up no new passengers and deliver all passengers currently on board. Once the elevator is empty, it goes offline.
