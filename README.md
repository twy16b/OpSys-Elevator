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

Something about part 3
