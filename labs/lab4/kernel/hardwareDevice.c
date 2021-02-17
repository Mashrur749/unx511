//hardwareDevice.cpp - A kernel level program for writing to a hardware device
//
// 29-Dec-20  M. Watler:        Created.

#include <linux/cdev.h>     /* char device stuff */
#include <linux/delay.h>     /* msleep */
#include <linux/errno.h>    /* error codes */
#include <linux/fs.h> 	    /* file stuff */
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/kernel.h>   /* printk() */
#include <linux/kthread.h>   /* kthread_create */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */
#include <linux/uaccess.h>
#include "hardwareDevice.h"

MODULE_DESCRIPTION("Hardware Device Linux driver");
MODULE_LICENSE("GPL");

#define BUF_LEN 16

int register_device(void);
void unregister_device(void);
//TODO: Function prototypes for the read and ioctl functions
static int hardware_device_open(struct inode *inode, struct file *file);
static int hardware_device_close(struct inode *inode, struct file *file);

/*===============================================================================================*/
static int hardware_device_init(void)
{
    int result = 0;
    printk( KERN_NOTICE "Hardware-Device: Initialization started\n" );

    result = register_device();
    return result;
}

/*===============================================================================================*/
static void hardware_device_exit(void)
{
    printk( KERN_NOTICE "Hardware-Device: Exiting\n" );
    unregister_device();
}

/*===============================================================================================*/
module_init(hardware_device_init);
module_exit(hardware_device_exit);

/*===============================================================================================*/
static struct file_operations simple_driver_fops =
{
    .owner = THIS_MODULE,
    //TODO: Function pointers for the read and ioctl functions
    .open = hardware_device_open,
    .release = hardware_device_close,
};

/*===============================================================================================*/
static int device_file_major_number = 0;
static const char device_name[] = "Hardware-Device";
struct task_struct *threadPtr;
static char buffer[BUF_LEN];
static bool is_running;
static bool is_halt;

int hardwareSim(void *data)
{
    int i;
    printk(KERN_INFO "hardwareSim:\n");
    for(i=0; i<BUF_LEN-1; ++i) {
        buffer[i]='a';
    }
    buffer[BUF_LEN-1]='\0';
    while(is_running) {
        while(!is_halt) {
            for(i=0; i<BUF_LEN-1; ++i) {
                ++buffer[i];
		if(buffer[i]>'z') buffer[i]='a';
            }
            buffer[BUF_LEN-1]='\0';
            msleep(1000);
        }
        msleep(1000);
    }
    return 0;
}

int register_device(void)
{
    int result = 0;

    printk( KERN_NOTICE "Hardware-Device: register_device() is called.\n" );

    result = register_chrdev( 0, device_name, &simple_driver_fops );
    if( result < 0 )
    {
        printk( KERN_WARNING "Hardware-Device:  can\'t register character device with errorcode = %i\n", result );
        return result;
    }

    device_file_major_number = result;
    printk( KERN_NOTICE "Hardware-Device: registered character device with major number = %i and minor numbers 0...255\n"
        , device_file_major_number );

    is_running=true;
    is_halt=false;
    printk(KERN_INFO "Hardware-Device: kthread_create(hardwareSim)\n");
    threadPtr = kthread_create(hardwareSim, NULL, "HardwareSimulator");
    if(threadPtr) {
        wake_up_process(threadPtr);
    }
    return 0;
}

/*===============================================================================================*/
void unregister_device(void)
{
    printk( KERN_NOTICE "Hardware-Device: unregister_device() is called\n" );
    is_running=false;
    is_halt=false;
    if(device_file_major_number != 0)
    {
        unregister_chrdev(device_file_major_number, device_name);
    }
}


/*===============================================================================================*/
static int hardware_device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Hardware-Device: open() is called\n");
    return 0;
}

/*===============================================================================================*/
static int hardware_device_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Hardware-Device: close() is called\n");
    return 0;
}

/*===============================================================================================*/
static ssize_t hardware_device_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    //TODO: Implement the read function
}

/*===============================================================================================*/
static long hardware_device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    //TODO: Implement the ioctl function
}
