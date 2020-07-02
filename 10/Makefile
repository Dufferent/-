KERNELDIR := /home/xny/git/qemu/imx6ull/qemu_imx6ull_kernel-master
CURRENT_PATH := $(shell pwd)
obj-m := unblock_mod.o

build : kenel_modules

kenel_modules :
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules 

clean :
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

App   :
	arm-linux-gnueabihf-gcc -o app_unblock ./app_unblock.c

clean_app:
	rm -rf ./*.o app_unblock