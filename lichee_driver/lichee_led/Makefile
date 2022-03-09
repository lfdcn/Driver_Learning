KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /lib/modules/5.10.63-rockchip64/build
PWD := $(shell pwd)
MOD_NAME1 := lichee_led
#MOD_NAME2 := 

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONEY:clean
clean :
	$(MAKE) -C $(KDIR) M=$(PWD) clean

obj-m += ${MOD_NAME1}.o
#obj-m += ${MOD_NAME2}.o
