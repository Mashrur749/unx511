//keyGenerator.c - A kernel level program for generating a key
//
// 11-Feb-21  M. Watler:        Created.

#include <linux/cdev.h>     /* char device stuff */
#include <linux/errno.h>    /* error codes */
#include <linux/fs.h> 	    /* file stuff */
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/kernel.h>   /* printk() */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */
#include <linux/uaccess.h>
#include "keyGenerator.h"

MODULE_DESCRIPTION("Key Generator Linux driver");
//Question 25: What is the full name of this license?
//General Public Licence

MODULE_LICENSE("GPL");
#define BUF_LEN 64

int register_device(void);
void unregister_device(void);
static ssize_t key_generator_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static int key_generator_open(struct inode *inode, struct file *file);
static int key_generator_close(struct inode *inode, struct file *file);
static long key_generator_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*===============================================================================================*/
/* Registers the device with the kernel. See Chapter 3 of Linux Device Drivers pages 55-56. */
static int key_generator_init(void)
{
    //Question 26: When is key_generator_init called?
    //when the module is loaded in the kernel

    int result = 0;
    printk( KERN_NOTICE "Key-Generator: Initialization started\n" );

    result = register_device();
    return result;
}

/*===============================================================================================*/
/* Unregisters the device with the kernel. See Chapter 2 of Linux Device Drivers page 32. */
static void key_generator_exit(void)
{
    //Question 27: When is key_generator_exit called?
    //when module gets unloaded from kernel

    printk( KERN_NOTICE "Key-Generator: Exiting\n" );
    unregister_device();
}

/*===============================================================================================*/
/* Specifies the init and exit functions for the kernel */
module_init(key_generator_init);
module_exit(key_generator_exit);

/*===============================================================================================*/
static struct file_operations simple_driver_fops =
{
    .owner = THIS_MODULE,
    .read = key_generator_read,
    .open = key_generator_open,
    .release = key_generator_close,
    .unlocked_ioctl = key_generator_ioctl
};

/*===============================================================================================*/
static int device_file_major_number = 0;
static const char device_name[] = "Key-Generator";
static char keyBuffer[BUF_LEN];
static int randomNumber=0;

int register_device(void)
{
    int result = 0;

    printk( KERN_NOTICE "Key-Generator: register_device() is called.\n" );

    //Question 28: What variable will hold the major number for this device?
    //result
    result = register_chrdev( 0, device_name, &simple_driver_fops );
    if( result < 0 )
    {
        printk( KERN_WARNING "Key-Generator:  can\'t register character device with errorcode = %i\n", result );
        return result;
    }

    device_file_major_number = result;
    printk( KERN_NOTICE "Key-Generator: registered character device with major number = %i and minor numbers 0...255\n"
        , device_file_major_number );

    randomNumber = ((randomNumber + 1)*100000000/17)%99999999;
    sprintf(keyBuffer, "%d", randomNumber);
    //Question 29: Where does printk appear?
    //in the file /var/log/syslogs and /var/log/kern.log 
    printk(KERN_NOTICE "Key-Generator: randomNumber %d keyBuffer %s\n", randomNumber, keyBuffer);
    return 0;
}

/*===============================================================================================*/
void unregister_device(void)
{
    printk( KERN_NOTICE "Key-Generator: unregister_device() is called\n" );
    if(device_file_major_number != 0)
    {
        unregister_chrdev(device_file_major_number, device_name);
    }
}


/*===============================================================================================*/
static int key_generator_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Key-Generator: open() is called\n");
    return 0;
}

/*===============================================================================================*/
static int key_generator_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Key-Generator: close() is called\n");
    return 0;
}

/*===============================================================================================*/
static ssize_t key_generator_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    unsigned long ret;
    unsigned long length = strlen(keyBuffer)+1;
    printk(KERN_INFO "Key-Generator: read %s\n", keyBuffer);
    //Question 30: Would memcpy work instead (yes/no/probably)?
    //yes, because both are attempting to copy from the specified memory location
    ret=copy_to_user(buf, keyBuffer, length);
    return length-ret;
}

static long key_generator_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd) {
        case KEY_GENERATOR_CHANGE_KEY:
	    printk(KERN_INFO "Key-Generator: ioctl KEY_GENERATOR_CHANGE_KEY\n");
            randomNumber = ((randomNumber + 1)*100000000/17)%99999999;
            sprintf(keyBuffer, "%d", randomNumber);
            break;
    }
    return 0;
}
