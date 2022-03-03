KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
MOD_NAME := vinput

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONEY:clean
clean :
	$(MAKE) -C $(KDIR) M=$(PWD) clean

obj-m += ${MOD_NAME}.o
#${MOD_NAME}-objs += hello.o
#${MOD_NAME}-objs += test.o
