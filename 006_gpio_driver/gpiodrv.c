/* Include necessary kernel headers */
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/gpio.h>

/* Define constants */
#define DYNAMIC 1 // Toggle for dynamic allocation of major number
#define DEVICE_NAME "gpio_device" // Name of the device
#define MAJOR_NUM 255 // Static major number
#define MINOR_NUM 0 // Static minor number
#define mem_size 1024 // Size of the memory buffer
#define GPIO_OFFSET 512 // Offset for GPIO base
#define GPIO21 21 // GPIO pin number

/* Define global variables */
static dev_t dev_num; // Device number
static struct class *dev_class; // Device class
static struct device *dev_device; // Device structure
static struct cdev new_cdev; // Character device structure
uint8_t *local_buffer; // Pointer for device memory buffer
static int major_num; // Major number for dynamic allocation

/* Function prototypes */
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/* File operations structure */
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

/* Initialize the driver */
static int __init gpio_driver_init(void)
{
    int ret;

#if DYNAMIC
    /* Dynamically allocate a major number for the device */
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_num < 0)
    {
        pr_err("failed to register device number dynamically \n");
        return ret;
    }
#else
    /* Static allocation of major and minor numbers */
    dev_num = MKDEV(MAJOR_NUM, MINOR_NUM);
    ret = register_chrdev_region(dev_num, 1, DEVICE_NAME);
    if (ret < 0)
    {
        pr_err("failed to register static device number \n");
        return ret;
    }
    pr_info("static allocation Major:%d Minor:%d \n", MAJOR(dev_num), MINOR(dev_num));
#endif

    /* Initialize the cdev structure and add it to the system */
    cdev_init(&new_cdev, &fops);
    new_cdev.owner = THIS_MODULE;
    ret = cdev_add(&new_cdev, MKDEV(major_num, 0), 1);
    if (ret < 0)
    {
        pr_err("unable to create cdev add \n");
        goto cdev_fail;
    }

    /* Create a device class in /sys/class/new_class */
    dev_class = class_create(THIS_MODULE, "new_class");
    if (IS_ERR(dev_class))
    {
        pr_err("unable to create the class \n");
        goto class_fail;
    }

    /* Create device information in /sys/class/new_class/new_device */
    dev_device = device_create(dev_class, NULL, MKDEV(major_num, 0), NULL, DEVICE_NAME);
    if (IS_ERR(dev_device))
    {
        pr_err("unable to create the device \n");
        goto device_fail;
    }

    /* Allocate memory for the device buffer */
    if ((local_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
    {
        pr_err("cannot allocate memory \n");
        goto device_fail;
    }

    /* Request the GPIO pin */
    if (gpio_request((GPIO21 + GPIO_OFFSET), "rpi-gpio-21"))
    {
        pr_err("Can not request gpio 21 \n");
        goto device_fail;
    }

    /* Set GPIO direction to output */
    if (gpio_direction_output((GPIO21 + GPIO_OFFSET), 0))
    {
        pr_err("Can not set the gpio 21 as output \n");
        goto gpio_fail;
    }

    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;

gpio_fail:
    gpio_free(GPIO21 + GPIO_OFFSET);
device_fail:
    class_destroy(dev_class);
cdev_fail:
    cdev_del(&new_cdev);
class_fail:
    unregister_chrdev(major_num, DEVICE_NAME);

    return -1;
}

/* File open function */
static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("New device file open function called \n");
    return 0;
}

/* File release function */
static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("New device file release function called \n");
    return 0;
}

/* File read function */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    pr_info("New device file read function called \n");
    if (copy_to_user(buffer, local_buffer, mem_size))
    {
        pr_err("Data read error \n");
    }
    pr_info("Data read successfully...\n");
    return mem_size;
}

/* File write function */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    char value;
    pr_info("New device file write function called \n");
    if (copy_from_user(local_buffer, buffer, len))
    {
        pr_err("Data write error \n");
    }
    value = (char)*local_buffer;
    pr_info("Data written successfully...local_buffer:%c\n", value);
    switch (value)
    {
        case '0':
            gpio_set_value((GPIO21 + GPIO_OFFSET), 0);
            break;
        case '1':
            gpio_set_value((GPIO21 + GPIO_OFFSET), 1);
            break;
        default:
            pr_info("The given value is invalid\n");
            break;
    }
    return len;
}

/* Exit function to clean up resources */
static void __exit gpio_driver_exit(void)
{
    gpio_set_value((GPIO21 + GPIO_OFFSET), 0);
    gpio_free(GPIO21 + GPIO_OFFSET);
    device_destroy(dev_class, MKDEV(major_num, 0));
    class_unregister(dev_class);
    class_destroy(dev_class);
    cdev_del(&new_cdev);
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Module Removed Successfully...\n");
}

/* Register init and exit functions */
module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

/* Module information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("collect and create");
MODULE_DESCRIPTION("gpio device driver");
MODULE_VERSION("2:1.0");
