obj-m += x_square.o

x_square-objs := x_square_forward_impl.o x_square_forward_impl_op.o x_square_backward_impl.o x_square_backward_impl_op.o x_square_main.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := aarch64-linux-gnu-gcc

ccflags-y := -O3 -std=gnu99
KBUILD_CFLAGS := $(filter-out -mgeneral-regs-only,$(KBUILD_CFLAGS))

all: modules test_program

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test_program: userprogram.c
	gcc -O3 -o userprogram userprogram.c -lm

# test_program_op: userprogram.c
# 	gcc -O3 -o userprogram_op userprogram.c -lm

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f userprogram userprogram_op
