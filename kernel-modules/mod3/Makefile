obj-m += task_dfs.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(pwd) clean
clean_up:
	sudo rmmod task_dfs
	sudo dmesg -c