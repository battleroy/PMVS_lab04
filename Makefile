obj-m := chrdev.o
KDIR := /usr/src/linux-headers-3.19.0-30-generic/
PWD := $(shell pwd)
all: clean
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean