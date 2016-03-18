obj-m += task_lin_iter.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(pwd) clean
