obj-m += sum_array.o cascading_sum.o

sum_array-objs := sum_array_impl.o sum_array_main.o
cascading_sum-objs := sum_array_impl.o cascading_sum_main.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := aarch64-linux-gnu-gcc

ccflags-y := -O3 -std=gnu99
KBUILD_CFLAGS := $(filter-out -mgeneral-regs-only,$(KBUILD_CFLAGS))

all: modules normal_sum_userprogram cascading_sum_userprogram

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		CFLAGS_MODULE="-DMODULE -fno-pic" modules

normal_sum_userprogram: normal_sum_userprogram.c
	$(CC) -O3 -std=gnu99 -o normal_sum_userprogram normal_sum_userprogram.c -lm -lrt

cascading_sum_userprogram: cascading_sum_userprogram.c
	$(CC) -O3 -std=gnu99 -o cascading_sum_userprogram cascading_sum_userprogram.c -lm -lrt

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean
	rm -f normal_sum_userprogram cascading_sum_userprogram
