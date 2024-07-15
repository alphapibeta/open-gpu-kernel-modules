#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include "sum_array_impl.h"
#include <linux/slab.h>

#define DEVICE_NAME "cascading_sum_dev"
#define CLASS_NAME "cascading_sum_class"

static int majorNumber;
static struct class* cascadingSumClass = NULL;
static struct device* cascadingSumDevice = NULL;
static long long result;

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

static int __init cascading_sum_init(void) {
    printk(KERN_INFO "Cascading Sum: Initializing the module\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "Cascading Sum failed to register a major number\n");
        return majorNumber;
    }

    cascadingSumClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cascadingSumClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(cascadingSumClass);
    }

    cascadingSumDevice = device_create(cascadingSumClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(cascadingSumDevice)) {
        class_destroy(cascadingSumClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(cascadingSumDevice);
    }

    printk(KERN_INFO "Cascading Sum: device class created correctly\n");
    return 0;
}

static void __exit cascading_sum_exit(void) {
    device_destroy(cascadingSumClass, MKDEV(majorNumber, 0));
    class_unregister(cascadingSumClass);
    class_destroy(cascadingSumClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Cascading Sum: Goodbye from the LKM!\n");
}

static int device_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Cascading Sum: Device has been opened\n");
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Cascading Sum: Device successfully closed\n");
    return 0;
}

static ssize_t device_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    size_t bytes_to_copy = min(len, sizeof(result) - (size_t)*offset);

    if (*offset >= sizeof(result)) {
        return 0;
    }

    error_count = copy_to_user(buffer, (char *)&result + *offset, bytes_to_copy);

    if (error_count == 0) {
        *offset += bytes_to_copy;
        return bytes_to_copy;
    } else {
        return -EFAULT;
    }
}

static ssize_t device_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    int *array;
    int size;
    int error_count = 0;
    ktime_t start, end;
    s64 duration;

    printk(KERN_INFO "Cascading Sum: device_write called with len=%zu\n", len);

    if (len < sizeof(int)) {
        printk(KERN_INFO "Cascading Sum: Invalid length\n");
        return -EINVAL;
    }

    error_count = copy_from_user(&size, buffer, sizeof(int));
    if (error_count != 0) {
        printk(KERN_INFO "Cascading Sum: copy_from_user failed for size\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Cascading Sum: size=%d\n", size);

    if (len != sizeof(int) + size * sizeof(int)) {
        printk(KERN_INFO "Cascading Sum: Length mismatch\n");
        return -EINVAL;
    }

    array = kmalloc(size * sizeof(int), GFP_KERNEL);
    if (!array) {
        printk(KERN_INFO "Cascading Sum: Memory allocation failed\n");
        return -ENOMEM;
    }

    error_count = copy_from_user(array, buffer + sizeof(int), size * sizeof(int));
    if (error_count != 0) {
        printk(KERN_INFO "Cascading Sum: copy_from_user failed for array\n");
        kfree(array);
        return -EFAULT;
    }

    printk(KERN_INFO "Cascading Sum: Starting computation\n");
    start = ktime_get();
    result = cascading_sum(array, size);
    end = ktime_get();
    duration = ktime_to_ns(ktime_sub(end, start));
    printk(KERN_INFO "Cascading sum execution time: %lld nanoseconds\n", duration);

    kfree(array);
    return len;
}

module_init(cascading_sum_init);
module_exit(cascading_sum_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Cascading sum calculation");
