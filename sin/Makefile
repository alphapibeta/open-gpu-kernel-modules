obj-m += sincospif_gpu_taylor.o

sincospif_gpu_taylor-objs := sincospif_gpu_taylor_impl.o sincospif_gpu_taylor_main.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ccflags-y := -msse -msse2 -mfma -O3

all: modules test_program

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test_program: userprogram.c
	gcc -O3 -o userprogram userprogram.c -lm

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f userprogram
