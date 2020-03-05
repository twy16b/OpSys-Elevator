obj-m := elevator.o

PWD := `pwd`
KERNELDIR := /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

insert:
	insmod elevator.ko

remove:
	rmmod elevator.ko

clean:
	rm -f *.ko *.o Module* *mod*