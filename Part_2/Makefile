obj-m := my_timer.o

PWD := `pwd`
KERNELDIR := /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
insert:
	@sudo insmod my_timer.ko
remove:
	@sudo rmmod my_timer
read:
	@cat /proc/timed
test:
	@sudo insmod my_timer.ko
	cat /proc/timed
	sleep 1
	cat /proc/timed
	sleep 3
	cat /proc/timed
	sleep 5
	cat /proc/timed
	@sudo rmmod my_timer

clean:
	rm -f *.ko *.o Module* *mod*
