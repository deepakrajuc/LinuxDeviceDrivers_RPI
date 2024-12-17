/*include header files*/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

/*init and exit function*/ 
static int __init hello_world_init(void)
{
    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;
}

static void __exit hello_world_exit(void)
{
    printk(KERN_INFO "Module Removed Successfully...\n");
}

/*Register init and exit function*/
module_init(hello_world_init);
module_exit(hello_world_exit);

/*module information*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("collect and create");
MODULE_DESCRIPTION("hello world module");
MODULE_VERSION("2:1.0");