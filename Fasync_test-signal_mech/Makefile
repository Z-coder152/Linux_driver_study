ifeq ($(KERNELRELEASE),)
KERNELDIR ?= ~/kernel-4.9 
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules* a.out 

else
    obj-m := fasync_info_driver.o
endif