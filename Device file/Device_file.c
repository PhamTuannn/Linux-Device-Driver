#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h> 
#include <linux/gpio.h>  
#include <linux/err.h>

#define GPIO_30	30
#define GPIO_31 31
#define GPIO_48 48

static int init_device_file(void); 
static void exit_device_file(void);

static int open_device_file(struct inode *inode, struct file *file);
static ssize_t read_device_file(struct file *file, char __user *buff, size_t len, loff_t *off);
static ssize_t write_device_file(struct file *file, const char __user *buff, size_t len, loff_t *off);
static int release_device_file(struct inode *inode, struct file *file);

dev_t dev = 0;
static struct class *myclass;
static struct cdev mycdev;

struct file_operations my_file = 
{
	.owner = THIS_MODULE,
	.read = read_device_file,
	.write = write_device_file,
	.open = open_device_file,
	.release = release_device_file,
};

static int open_device_file(struct inode *inode, struct file *file)
{
	pr_info("Open file!\n");
	return 0;
}

static ssize_t read_device_file(struct file *file, char __user *buff, size_t len, loff_t *off)
{
	pr_info("Read file!\n");
	return 0;
}

static ssize_t write_device_file(struct file *file, const char __user *buff, size_t len, loff_t *off)
{
	pr_info("Write file!\n");
	char rec_buf[10] = {0};
	if(copy_from_user(rec_buf, buff, len ) > 0)
	{
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}
	if(rec_buf[0]=='1')
	{
		gpio_set_value(GPIO_30, 1);
	}
	if(rec_buf[0]=='2')
	{
		gpio_set_value(GPIO_31, 1);
	}
	if(rec_buf[0]=='3')
	{
		gpio_set_value(GPIO_48, 1);
	}
	if(rec_buf[0]=='4')
	{
		gpio_set_value(GPIO_30, 0);
		gpio_set_value(GPIO_31, 0);
		gpio_set_value(GPIO_48, 0);
	}
	return len;
}

static int release_device_file(struct inode *inode, struct file *file)
{
	pr_info("Release file!\n");
	return 0;
}

static int init_device_file(void)
{
	if(gpio_is_valid(GPIO_30)==false)
	{
		pr_info("GPIO 30 is not valid\n");
	}
	if(gpio_request(GPIO_30, "GPIO 30")<0)
	{
		pr_info("GPIO 30 is not request\n");
	}
	if(gpio_is_valid(GPIO_31)==false)
	{
		pr_info("GPIO 32 is not valid\n");
	}
	if(gpio_request(GPIO_31, "GPIO 31")<0)
	{
		pr_info("GPIO 31 is not request\n");
	}
	if(gpio_is_valid(GPIO_48)==false)
	{
		pr_info("GPIO 48 is not valid\n");
	}
	if(gpio_request(GPIO_48, "GPIO 48")<0)
	{
		pr_info("GPIO 48 is not request\n");
	}
	if(alloc_chrdev_region(&dev, 0, 1, "device_file")<0)
	{
		pr_err("alloc_chrdev_region not working\n");
		return -1;
	}
	myclass = class_create(THIS_MODULE, "my_class");
	if(myclass<0)
	{
		pr_err("class_create not working\n");
		goto r_class;
	}
	cdev_init(&mycdev, &my_file);
	if(cdev_add(&mycdev, dev, 1)<0)
	{
		pr_err("cdev_add not working\n");
	}
	if(device_create(myclass, NULL, dev, NULL, "device_file")<0)
	{
		pr_err("device_create not working\n");
		goto r_device;
	}
	gpio_direction_output(GPIO_30, 0);
	gpio_direction_output(GPIO_31, 0);
	gpio_direction_output(GPIO_48, 0);
	gpio_export(GPIO_30, false);
	gpio_export(GPIO_31, false);
	gpio_export(GPIO_48, false);
	pr_info("Driver is loaded!\n");
	return 0;
r_device:
        class_destroy(myclass);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

static void exit_device_file(void)
{
 	device_destroy(myclass, dev);
    	class_destroy(myclass);    
    	cdev_del(&mycdev);
    	unregister_chrdev_region(dev, 1);
    	gpio_free(GPIO_30);
    	gpio_free(GPIO_31);
    	gpio_free(GPIO_48);
	pr_info("Removed this driver!\n");
}

module_init(init_device_file);
module_exit(exit_device_file);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PhamTuan");
MODULE_DESCRIPTION("Control led from User space");
MODULE_VERSION("1.5");
