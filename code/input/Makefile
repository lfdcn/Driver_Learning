#KDIR := /lib/modules/$(shell uname -r)/build
KDIR := /lib/modules/5.13.0-35-generic/build
PWD := $(shell pwd)
MOD_NAME := tp_drv

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONEY:clean
clean :
	$(MAKE) -C $(KDIR) M=$(PWD) clean

obj-m += ${MOD_NAME}.o
#${MOD_NAME}-objs += hello.o
#${MOD_NAME}-objs += test.o
