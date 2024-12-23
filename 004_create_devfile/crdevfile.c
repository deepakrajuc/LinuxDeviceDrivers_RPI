/* Include necessary header files for kernel module development */
#include<linux/kernel.h>   // For kernel functions like printk
#include<linux/init.h>     // For module initialization and cleanup macros
#include<linux/module.h>   // For module-related functions and macros
#include<linux/device.h>   // For device creation and management
#include<linux/fs.h>       // For file operations structure and functions
#include<linux/err.h>      // For error handling macros
#include<linux/kdev_t.h>   // For device number macros

/* Macro definitions */
#define DYNAMIC 1            // Set to 1 for dynamic device number allocation
#define DEVICE_NAME "new_device" // Name of the device
#define MAJOR_NUM 255        // Major number for static allocation
#define MINOR_NUM 0          // Minor number for static allocation

/* Global variables */
static dev_t dev_num;         // Device number (for static allocation)
static struct class *dev_class; // Pointer to device class
static struct device *dev_device; // Pointer to device structure

static int major_num;         // Major number for dynamic allocation

/* Function prototypes for file operations */
static int dev_open(struct inode *, struct file *);         // Open function
static int dev_release(struct inode *, struct file *);      // Release function
static ssize_t dev_read(struct file *, char *, size_t, loff_t *); // Read function
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *); // Write function

/* File operations structure */
static struct file_operations fops =
{
    .owner = THIS_MODULE,  // Pointer to the module owning this structure
    .open = dev_open,      // Assign open function
    .read = dev_read,      // Assign read function
    .write = dev_write,    // Assign write function
    .release = dev_release, // Assign release function
};

/* Module initialization function */
static int __init hello_world_init(void)
{
    int ret;

#if DYNAMIC
    /* Dynamic device number allocation */
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_num < 0)
    {
        pr_err("Failed to register device number dynamically\n");
        return ret;
    }
#else
    /* Static device number allocation */
    dev_num = MKDEV(MAJOR_NUM, MINOR_NUM);
    ret = register_chrdev_region(dev_num, 1, DEVICE_NAME);
    if (ret < 0)
    {
        pr_err("Failed to register static device number\n");
        return ret;
    }
    pr_info("Static allocation Major:%d Minor:%d\n", MAJOR(dev_num), MINOR(dev_num));
#endif

    /* Creating struct class for the device */
    dev_class = class_create("new_class");
    if (IS_ERR(dev_class))
    {
        pr_err("Unable to create the class\n");
        goto class_fail;
    }

    /* Creating the device */
    dev_device = device_create(dev_class, NULL, MKDEV(major_num, 0), NULL, "new_device");
    if (IS_ERR(dev_device))
    {
        pr_err("Failed to create the device\n");
        goto device_fail;
    }

    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;

device_fail:
    /* Cleanup class if device creation fails */
    class_destroy(dev_class);
class_fail:
    /* Unregister the device number on failure */
    unregister_chrdev(major_num, DEVICE_NAME);
    return -1;
}

/* Device open function implementation */
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "New Device file open function called\n");
    return 0; // Return 0 to indicate success
}

/* Device read function implementation */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "New Device file read function called\n");
    return 0; // Return 0 for now as no data is being read
}

/* Device write function implementation */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "New Device file write function called\n");
    return 0; // Return 0 for now as no data is being written
}

/* Device release function implementation */
static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "New Device file release function called\n");
    return 0; // Return 0 to indicate success
}

/* Module cleanup function */
static void __exit hello_world_exit(void)
{
    /* Cleanup created device */
    device_destroy(dev_class, MKDEV(major_num, 0));

    /* Cleanup device class */
    class_unregister(dev_class);
    class_destroy(dev_class);

    /* Unregister the device number */
    unregister_chrdev(major_num, DEVICE_NAME);

    printk(KERN_INFO "Module Removed Successfully...\n");
}

/* Register init and exit functions */
module_init(hello_world_init);
module_exit(hello_world_exit);

/* Module metadata */
MODULE_LICENSE("GPL");           // License type
MODULE_AUTHOR("collect and create"); // Module author
MODULE_DESCRIPTION("Character device driver"); // Description of the module
MODULE_VERSION("2:1.0");          // Module version

