ifeq ($(KERNELRELEASE),)
KERNELDIR ?= ~/kernel-4.9 
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules* a.out 

else
    obj-m := mpu_driver.o
endif