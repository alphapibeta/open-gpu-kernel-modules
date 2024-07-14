#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include "sincospif_gpu_impl.h"

#define DEVICE_NAME "sincospif_gpu_taylor_dev"
#define CLASS_NAME "sincospif_gpu_taylor_class"

static int majorNumber;
static struct class* sincospifClass = NULL;
static struct device* sincospifDevice = NULL;
static float results[39];

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

static int __init sincospif_gpu_taylor_init(void) {
    printk(KERN_INFO "SinCosPif GPU Taylor: Initializing the module\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "SinCosPif GPU Taylor failed to register a major number\n");
        return majorNumber;
    }

    sincospifClass = class_create(CLASS_NAME);
    if (IS_ERR(sincospifClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(sincospifClass);
    }

    sincospifDevice = device_create(sincospifClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(sincospifDevice)) {
        class_destroy(sincospifClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(sincospifDevice);
    }

    printk(KERN_INFO "SinCosPif GPU Taylor: device class created correctly\n");
    return 0;
}

static void __exit sincospif_gpu_taylor_exit(void) {
    device_destroy(sincospifClass, MKDEV(majorNumber, 0));
    class_unregister(sincospifClass);
    class_destroy(sincospifClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "SinCosPif GPU Taylor: Goodbye from the LKM!\n");
}

static int device_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "SinCosPif GPU Taylor: Device has been opened\n");
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "SinCosPif GPU Taylor: Device successfully closed\n");
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
    float angles[13];
    float sin_results[13], cos_results[13];
    int error_count = 0;
    ktime_t start, end;
    s64 duration;

    if (len != sizeof(angles)) {
        return -EINVAL;
    }

    error_count = copy_from_user(angles, buffer, len);

    if (error_count == 0) {
        start = ktime_get();
        sincospif_gpu_calculate_taylor(angles, sin_results, cos_results, 13);
        end = ktime_get();
        duration = ktime_to_ns(ktime_sub(end, start));
        printk(KERN_INFO "Kernel execution time: %lld nanoseconds\n", duration);

        memcpy(results, angles, sizeof(angles));
        memcpy(results + 13, sin_results, sizeof(sin_results));
        memcpy(results + 26, cos_results, sizeof(cos_results));
        return len;
    } else {
        return -EFAULT;
    }
}

module_init(sincospif_gpu_taylor_init);
module_exit(sincospif_gpu_taylor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ronak");
MODULE_DESCRIPTION("cos calcultion using tyler expansion");
