#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include "x_square_impl.h"

#define DEVICE_NAME "x_square_dev"
#define CLASS_NAME "x_square_class"

static int majorNumber;
static struct class* xSquareClass = NULL;
static struct device* xSquareDevice = NULL;
static float results[26];

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .release = device_release,
};

static int __init x_square_init(void) {
    printk(KERN_INFO "x_square: Initializing the module\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "x_square failed to register a major number\n");
        return majorNumber;
    }

    xSquareClass = class_create(CLASS_NAME);
    if (IS_ERR(xSquareClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(xSquareClass);
    }

    xSquareDevice = device_create(xSquareClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(xSquareDevice)) {
        class_destroy(xSquareClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(xSquareDevice);
    }

    printk(KERN_INFO "x_square: device class created correctly\n");
    return 0;
}

static void __exit x_square_exit(void) {
    device_destroy(xSquareClass, MKDEV(majorNumber, 0));
    class_unregister(xSquareClass);
    class_destroy(xSquareClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "x_square: Goodbye from the LKM!\n");
}

static int device_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "x_square: Device has been opened\n");
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "x_square: Device successfully closed\n");
    return 0;
}

static ssize_t device_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    size_t bytes_to_copy = min(len, sizeof(results) - (size_t)*offset);

    if (*offset >= sizeof(results)) {
        return 0;
    }

    error_count = copy_to_user(buffer, (char *)results + *offset, bytes_to_copy);

    if (error_count == 0) {
        *offset += bytes_to_copy;
        return bytes_to_copy;
    } else {
        return -EFAULT;
    }
}

static ssize_t device_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    float inputs[13];
    float forward_results[13], backward_results[13];
    int error_count = 0;
    ktime_t start, end;
    s64 duration;

    if (len != sizeof(inputs)) {
        return -EINVAL;
    }

    error_count = copy_from_user(inputs, buffer, len);

    if (error_count == 0) {
        start = ktime_get();
        x_square_forward(inputs, forward_results, 13);
        x_square_backward(inputs, backward_results, 13);
        end = ktime_get();
        duration = ktime_to_ns(ktime_sub(end, start));
        printk(KERN_INFO "Kernel execution time: %lld nanoseconds\n", duration);

        memcpy(results, forward_results, sizeof(forward_results));
        memcpy(results + 13, backward_results, sizeof(backward_results));
        return len;
    } else {
        return -EFAULT;
    }
}

module_init(x_square_init);
module_exit(x_square_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ronak");
MODULE_DESCRIPTION("x^2 calculation with forward and backward pass");
