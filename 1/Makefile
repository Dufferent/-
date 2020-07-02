KERNELDIR := /home/xny/Linux/Kernel_pj
CURRENT_PATH := $(shell pwd)
obj-m := driver_mod.o

build : kenel_modules

kenel_modules :
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules 

clean :
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean