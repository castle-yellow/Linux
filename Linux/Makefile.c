obj-m := syscall.o
KERNELDIR := /lib/modules/$(shell uname –r)/build PWD := $(shell pwd)
modules:
$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install