obj-m := elevator.o

PWD := `pwd`
KERNELDIR := /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	insmod elevator.ko

insert:
	insmod elevator.ko

remove:
	rmmod elevator.ko

start:
	/home/yelverto/Documents/Project_2/Part_3/consumer.x --start

stop:
	/home/yelverto/Documents/Project_2/Part_3/consumer.x --stop

produce:
	/home/yelverto/Documents/Project_2/producer.x

clean:
	rm -f *.ko *.o Module* *mod*
