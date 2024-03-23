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
 
int32_t value = 0;
#define GPIO_20 (20)
#define GPIO_21 (7)
#define GPIO_22 (67)
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int      __init init_driver(void);
static void     __exit exit_driver(void);
static int      file_open(struct inode *inode, struct file *file);
static int      file_release(struct inode *inode, struct file *file);
static ssize_t  file_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  file_write(struct file *filp, const char *buf, size_t len, loff_t * off);

struct file_operations fops = 
{
    .owner          = THIS_MODULE,
    .read           = file_read,
    .write          = file_write,
    .open           = file_open,
    .release        = file_release,
};

static int file_open(struct inode *inode, struct file *file)
{
    pr_info("File is opened");
    return 0;
}

static int file_release(struct inode *inode, struct file *file)
{
    pr_info("File is realeased!");
    return 0;
}

static ssize_t file_read(struct file *filp, char __user *buf, size_t len,loff_t * off)
{
    pr_info("Read file");
    return 0;
}

static ssize_t file_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
    uint8_t rec_buf[10] = {0};
    if( copy_from_user(rec_buf, buf, len ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }
    if (rec_buf[0]=='1') {
        gpio_set_value(GPIO_20, 1);
        pr_info("Led 1 is on");
    } else if (rec_buf[0]=='2') {
        gpio_set_value(GPIO_21, 1);
        pr_info("Led 2 is on");
    } else if (rec_buf[0]=='3') {
        gpio_set_value(GPIO_22, 1);
        pr_info("Led 3 is on");
    } else if (rec_buf[0]=='4') {
        gpio_set_value(GPIO_20, 0);
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_22, 0);
        pr_info("Off led");
    } 
    else {
        pr_err("Please provide 1 to 4 \n");
    }
    return len;
}

static int init_driver(void)
{
    if(gpio_is_valid(20) ==  false)
    {
        pr_info("ERROR: GPIO %d is not valid/n", GPIO_20);
    }
    if(gpio_request(GPIO_20,"GPIO_20") < 0) {
        pr_err("ERROR: GPIO %d request\n", GPIO_20);
        return -1;
    }

    if(gpio_is_valid(21) ==  false)
    {
        pr_info("ERROR: GPIO %d is not valid/n", GPIO_21);
    }
    if(gpio_request(GPIO_21,"GPIO_20") < 0) {
        pr_err("ERROR: GPIO %d request\n", GPIO_21);
        return -1;
    }

    if(gpio_is_valid(22) ==  false)
    {
        pr_info("ERROR: GPIO %d is not valid/n", GPIO_22);
    }
    if(gpio_request(GPIO_22,"GPIO_22") < 0) {
        pr_err("ERROR: GPIO %d request\n", GPIO_20);
        return -1;
    }

    gpio_direction_output(GPIO_20, 0);
    gpio_direction_output(GPIO_21, 0);
    gpio_direction_output(GPIO_22, 0);
    gpio_export(GPIO_20, false);
    gpio_export(GPIO_21, false);
    gpio_export(GPIO_22, false);

    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
            pr_err("Cannot allocate major number\n");
            return -1;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
    cdev_init(&etx_cdev,&fops);
 
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }

    if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
 
    if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"device_file"))){
        pr_err("Cannot create the Device\n");
        goto r_device;
    }
    pr_info("Insmod Driver...Done!!!\n");
    return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

static void exit_driver(void)
{
    device_destroy(dev_class,dev);
    class_destroy(dev_class);    
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    gpio_unexport(GPIO_20);
    gpio_unexport(GPIO_21);
    gpio_unexport(GPIO_22);
    gpio_free(GPIO_20);
    gpio_free(GPIO_21);
    gpio_free(GPIO_22);
    pr_info("Device Driver Remove...Done!!!\n");
}

module_init(init_driver);
module_exit(exit_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tuan");
MODULE_DESCRIPTION("Control led from User space");
MODULE_VERSION("1.5");
