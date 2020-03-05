obj-m := elevator.o

PWD := `pwd`
KERNELDIR := /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

insert:
	sudo insmod elevator.ko

remove:
	sudo rmmod elevator.ko

clean:
	rm -f *.ko *.o Module* *mod*
