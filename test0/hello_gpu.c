#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void) {
    printk(KERN_INFO "Hello, world - this is the GPU talking\n");
    return 0;
}

void cleanup_module(void) {
    printk(KERN_INFO "Goodbye, world - from the GPU\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ronak is my name and I am testing open-gpu-kernel-modules");
MODULE_DESCRIPTION("A Simple Hello World GPU Module");
