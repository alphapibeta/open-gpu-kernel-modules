#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include "sum_array_impl.h"

#define DEVICE_NAME "sum_array_dev"
#define CLASS_NAME "sum_array_class"

static int majorNumber;
static struct class* sumArrayClass = NULL;
static struct device* sumArrayDevice = NULL;
static long long results[2];

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

static int __init sum_array_init(void) {
    printk(KERN_INFO "Sum Array: Initializing the module\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "Sum Array failed to register a major number\n");
        return majorNumber;
    }

    sumArrayClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(sumArrayClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(sumArrayClass);
    }

    sumArrayDevice = device_create(sumArrayClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(sumArrayDevice)) {
        class_destroy(sumArrayClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(sumArrayDevice);
    }

    printk(KERN_INFO "Sum Array: device class created correctly\n");
    return 0;
}

static void __exit sum_array_exit(void) {
    device_destroy(sumArrayClass, MKDEV(majorNumber, 0));
    class_unregister(sumArrayClass);
    class_destroy(sumArrayClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Sum Array: Goodbye from the LKM!\n");
}

static int device_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Sum Array: Device has been opened\n");
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Sum Array: Device successfully closed\n");
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
    int *array;
    int size;
    int error_count = 0;
    ktime_t start, end;
    s64 duration_normal, duration_cascading;

    printk(KERN_INFO "Sum Array: device_write called with len=%zu\n", len);

    if (len < sizeof(int)) {
        printk(KERN_INFO "Sum Array: Invalid length\n");
        return -EINVAL;
    }

    error_count = copy_from_user(&size, buffer, sizeof(int));
    if (error_count != 0) {
        printk(KERN_INFO "Sum Array: copy_from_user failed for size\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Sum Array: size=%d\n", size);

    if (len != sizeof(int) + size * sizeof(int)) {
        printk(KERN_INFO "Sum Array: Length mismatch\n");
        return -EINVAL;
    }

    array = kmalloc(size * sizeof(int), GFP_KERNEL);
    if (!array) {
        printk(KERN_INFO "Sum Array: Memory allocation failed\n");
        return -ENOMEM;
    }

    error_count = copy_from_user(array, buffer + sizeof(int), size * sizeof(int));
    if (error_count != 0) {
        printk(KERN_INFO "Sum Array: copy_from_user failed for array\n");
        kfree(array);
        return -EFAULT;
    }

    printk(KERN_INFO "Sum Array: Starting computation\n");
    start = ktime_get();
    results[0] = normal_sum(array, size);
    end = ktime_get();
    duration_normal = ktime_to_ns(ktime_sub(end, start));
    printk(KERN_INFO "Normal sum execution time: %lld nanoseconds\n", duration_normal);

    start = ktime_get();
    results[1] = cascading_sum(array, size);
    end = ktime_get();
    duration_cascading = ktime_to_ns(ktime_sub(end, start));
    printk(KERN_INFO "Cascading sum execution time: %lld nanoseconds\n", duration_cascading);

    kfree(array);
    return len;
}

module_init(sum_array_init);
module_exit(sum_array_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Sum array calculation using normal and cascading methods");
