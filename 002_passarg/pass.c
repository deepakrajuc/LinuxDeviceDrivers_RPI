/*include header files*/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
/*header for parameters*/
#include<linux/moduleparam.h>

int value, arr_value[3];
static int int_param=0;
char *name;

/*Define macros for parameters*/
module_param(value, int, S_IRUSR|S_IWUSR);
module_param(name, charp, S_IRUSR|S_IWUSR);
module_param_array(arr_value, int, NULL, S_IRUSR|S_IWUSR);

/*callback function*/
static int int_param_set(const char *val, const struct kernel_param *kp)
{
	int ret = param_set_int(val,kp); //parse and set the value
	
	if(ret==0)
		printk(KERN_INFO " Parameter updated to int_param=%d\n", int_param);

	return ret;
}

static int int_param_get(char *buffer, const struct kernel_param *kp)
{
 	printk(KERN_INFO "CALLBACK FUCNTION INT_PARAM_GET\n");
	return sprintf(buffer, "%d", *(int *)(kp->arg)); //read the value
}
/*Define the parameter operation*/
static const struct kernel_param_ops int_param_ops={
	.set = int_param_set,
	.get = int_param_get,
};

/*Register the parameter*/
module_param_cb(int_param, &int_param_ops, &int_param, S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR);

/*init and exit function*/ 
static int __init hello_world_init(void)
{
    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    printk(KERN_INFO "Kernel Module value:%d\n",value);
    printk(KERN_INFO "Kernel Module name:%s\n",name);

    for(int i=0; i<sizeof arr_value/sizeof (int);i++)
	    printk(KERN_INFO "arr_value[%d]:%d",i,arr_value[i]);

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
