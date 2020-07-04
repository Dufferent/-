KERNELDIR := /home/xny/git/qemu/imx6ull/qemu_imx6ull_kernel-master
CURRENT_PATH := $(shell pwd)
obj-m := inputsys_mod.o

build : kenel_modules

kenel_modules :
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules 

clean :
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

app   :
	arm-linux-gnueabihf-gcc -o inputsys_app ./inputsys_app.c

clapp :
	rm -rf inputsys_app inputsys_app.o