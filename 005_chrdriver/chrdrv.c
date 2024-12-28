/* Include necessary header files for kernel modules and character devices */
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/device.h>
#include<linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include<linux/uaccess.h>

/* Macros for configuration */
#define DYNAMIC 1 // Flag for dynamic allocation
#define DEVICE_NAME "new_device" // Name of the device
#define MAJOR_NUM 255 // Major number for static allocation
#define MINOR_NUM 0 // Minor number for static allocation
#define mem_size 1024 // Size of the memory buffer

/* Declare global variables and structures */
static struct cdev new_cdev; // Character device structure
static dev_t dev_num; // Device number
static struct class *dev_class; // Device class
static struct device *dev_device; // Device structure
uint8_t *local_buffer; // Pointer for memory allocation

static int major_num; // Major number for dynamic allocation

/* Function prototypes for file operations */
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/* File operations structure */
static struct file_operations fops=
{
	.owner=THIS_MODULE, // Module owner
	.open=dev_open, // Open function
	.read=dev_read, // Read function
	.write=dev_write, // Write function
	.release=dev_release, // Release function
};

/* Init function for the module */
static int __init hello_world_init(void)
{
    int ret; // Variable for return values
   
#if DYNAMIC
    // Dynamically register the character device number
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if(major_num<0)
    {
	    pr_err("failed to register device number dynamically \n");
	    return ret;
    }
#else
    // Static allocation of character device number
    dev_num=MKDEV(MAJOR_NUM, MINOR_NUM);
    ret=register_chrdev_region(dev_num, 1, DEVICE_NAME);
    if(ret<0)
    {
	    pr_err("failed to register static device number \n");
	    return ret;
    }
    pr_info("static allocation Major:%d Minor:%d \n",MAJOR(dev_num),MINOR(dev_num));
#endif

    // Initialize the cdev structure and add it to the system
    cdev_init(&new_cdev, &fops);
    new_cdev.owner = THIS_MODULE;
    ret = cdev_add(&new_cdev, MKDEV(major_num,0),1);
    if(ret<0)
    {
	    pr_err("unable to create cdev add \n");
	    goto cdev_fail;
    }

    // Create a device class
    dev_class= class_create("new_class");
    if(IS_ERR(dev_class))
    {
	    pr_err("unable to create the class \n");
	    goto class_fail;
    }

    // Create a device and associate it with the class
    dev_device= device_create(dev_class, NULL, MKDEV(major_num, 0), NULL, "new_device");
    if(IS_ERR(dev_device))
    {
	    pr_err("unable to create the device \n");
	    goto device_fail;
    }

    // Allocate memory for the device buffer
    if((local_buffer = kmalloc(mem_size, GFP_KERNEL))== 0)
    {
	    pr_err("cannot allocate memory");
	    goto device_fail;
    }
    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;

device_fail:
    // Cleanup on device creation failure
	class_destroy(dev_class);
cdev_fail:
     cdev_del(&new_cdev);
class_fail:
    // Cleanup on class creation failure
	unregister_chrdev(major_num, DEVICE_NAME);

	return -1;
}

/* Function to handle device file open */
static int dev_open(struct inode *inodep, struct file *filep)
{
	pr_info("New device file open function called \n");
	return 0;
}

/* Function to handle device file release */
static int dev_release(struct inode *inodep, struct file *filep)
{
	pr_info("New device file release function called \n");
	return 0;
}

/* Function to handle read from the device */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	pr_info("New device file read function called \n");
	if(copy_to_user(buffer, local_buffer, mem_size)) // Copy data to user space
	{
		pr_err("Data read error \n");
	}
	pr_info("Data readed successfully ..... \n");

	return mem_size; // Return the size of the data read
}

/* Function to handle write to the device */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	pr_info("New device file write function called \n");
	if(copy_from_user(local_buffer, buffer, len)) // Copy data from user space
	{
		pr_err("Data write Error \n");
	}
	pr_info("Data written successfully");
	return len; // Return the size of the data written
}

/* Exit function for the module */
static void __exit hello_world_exit(void)
{
    kfree(local_buffer); // Free allocated memory
    device_destroy(dev_class, MKDEV(major_num,0)); // Destroy the device
    cdev_del(&new_cdev); // delete the cdev
    class_unregister(dev_class); // Unregister the device class
    class_destroy(dev_class); // Destroy the device class
    unregister_chrdev(major_num,DEVICE_NAME); // Unregister the character device
    printk(KERN_INFO "Module Removed Successfully...\n");
}

/* Register init and exit functions */
module_init(hello_world_init);
module_exit(hello_world_exit);

/* Module information */
MODULE_LICENSE("GPL"); // License for the module
MODULE_AUTHOR("collect and create"); // Author name
MODULE_DESCRIPTION("character device driver"); // Module description
MODULE_VERSION("2:1.0"); // Module version
