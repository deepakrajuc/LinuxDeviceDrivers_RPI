/* Include necessary header files for kernel modules */
#include <linux/kernel.h>  // For kernel info, error, and debug macros
#include <linux/init.h>    // For init and exit macros
#include <linux/module.h>  // For module-related macros
#include <linux/device.h>  // For device-specific macros
#include <linux/fs.h>      // For file system operations and macros

/* Macro definitions */
#define DYNAMIC 0            // Flag to enable dynamic allocation of device numbers
#define DEVICE_NAME "new_device" // Name of the device
#define MAJOR_NUM 255        // Static major number for the device (if DYNAMIC is not defined)
#define MINOR_NUM 0          // Static minor number for the device

/* Variable to store device number */
static dev_t dev_num;

/* Initialization function: Called when the module is loaded into the kernel */
static int __init hello_world_init(void)
{
    int ret;

#if DYNAMIC  // If dynamic allocation is enabled
    /* Allocate a dynamic device number */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to register device number dynamically\n");
        return ret; // Return error code if allocation fails
    }
    pr_info("Dynamic allocation Major:%d, Minor:%d\n", MAJOR(dev_num), MINOR(dev_num));
#else
    /* Create a static device number using predefined major and minor numbers */
    dev_num = MKDEV(MAJOR_NUM, MINOR_NUM);
    ret = register_chrdev_region(dev_num, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to register static device number. Return code=%d\n", ret);
        return ret; // Return error code if registration fails
    }
#endif

    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0; // Indicate successful initialization
}

/* Exit function: Called when the module is removed from the kernel */
static void __exit hello_world_exit(void)
{
    /* Unregister the device number */
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Module Removed Successfully...\n");
}

/* Register the initialization and exit functions with the kernel */
module_init(hello_world_init);
module_exit(hello_world_exit);

/* Module metadata */
MODULE_LICENSE("GPL");                    // License type
MODULE_AUTHOR("collect and create");      // Author of the module
MODULE_DESCRIPTION("Major number and minor number"); // Description of the module
MODULE_VERSION("2:1.0");                  // Module version

