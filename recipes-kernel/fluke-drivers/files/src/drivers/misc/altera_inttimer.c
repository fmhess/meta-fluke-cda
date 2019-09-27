/*
    altera_inttimer.c -- This driver provides generic support for the Altera
    Interval Timer. 
        
    Copyright (C) 2015 Alex Dorchak (Fluke Corp)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
*/

#include <linux/module.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/idr.h>
#include <linux/ioport.h>
#include <linux/types.h> 
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "altera_inttimer-dev.h"
#include "altera_inttimer.h"

#define NR_PORTS	6	
#define MAX_INTTIMER_DEVICES 0x100

MODULE_AUTHOR ("Alex j. Dorchak");
MODULE_DESCRIPTION("Driver for Altera Interval Timer Core");
MODULE_LICENSE("GPL");

static struct class *alt_intervaltimer_class;
static unsigned major_number;
static const int BEEPER_COUNTER_FREQ = 60000000;
static DEFINE_IDA(minor_allocator);

static int hw_open (struct inode *inode, struct file *filp) {

    struct inttimer_port *inttimerp; /* device information */

    // printk("INTTIMER open called \n");
    inttimerp =  container_of(inode->i_cdev, struct inttimer_port, cdev);
    filp->private_data = inttimerp;
    // printk("INTTIMER open Exit \n");

    return 0;
}

static int hw_release (struct inode *inode, struct file *filp) {
    // printk("INTTIMER release called \n");
	return 0;
}

static ssize_t hw_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    char *tbuf;
    int i;

    int retval = 0;
    
    struct inttimer_port *inttimerp = filp->private_data;

    // printk ("int timer: read function CALLED!\n");

    if (down_interruptible(&inttimerp->sem))
        return -ERESTARTSYS;
    
    if (!(tbuf = kmalloc(count, GFP_ATOMIC))) {
        return -ENOMEM;
    }

    for (i = 0; i < count; i++) {
        tbuf[i] = ioread32(inttimerp->mapbase + 4);
        rmb();
    }
    count = i;

    if (copy_to_user(buf, tbuf, count)) {
        printk (KERN_INFO "int timer: read function FAILED! addr = %p\n", inttimerp->mapbase);
        retval = -EFAULT; 
    }
    else 
        retval = count;

    kfree(tbuf);
    up(&inttimerp->sem);

    return retval;
}

static ssize_t hw_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    int i;
    int retval = 0;
    int note = 1500;
    int CountVal = 0;
    unsigned char *tbuf;
    
    struct inttimer_port *inttimerp = filp->private_data;

    if (down_interruptible(&inttimerp->sem))
        return -ERESTARTSYS;

    if (!(tbuf = kmalloc(count, GFP_ATOMIC))) {
        printk("int timer: write, kmalloc failed!\n");
        return -ENOMEM;
    }

    if (copy_from_user(tbuf, buf, count)) {
        printk (KERN_INFO "int timer: write function FAILED! addr = %p\n", inttimerp->mapbase);
        retval = -EFAULT; 
    }
    else {
        CountVal = BEEPER_COUNTER_FREQ/(2*note);
        for (i = 0; i < count; i++) {
            iowrite32(tbuf[i], inttimerp->mapbase + 0x04);
            // iowrite32(0x06, inttimerp->mapbase + 0x04);
        }
        retval = count;
    }

    kfree(tbuf);
    up(&inttimerp->sem);
    return retval;
}

//static ssize_t hw_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {
static long hw_ioctl (struct file *filp, unsigned int cmd, unsigned long arg) {

    int error  = 0;
    int data = 0x00;
    struct inttimer_port *inttimerp = filp->private_data;

    // printk ("inttimer/ioctl: %p\n", inttimerp->mapbase);
    down(&inttimerp->sem);

    if (_IOC_TYPE(cmd) != INTTIMER_IOC_MAGIC_NUMBER)  return -ENOTTY;
    if (_IOC_NR(cmd)   >= INTTIMER_IOC_MAXNUMBER) return -ENOTTY;

    if (_IOC_DIR(cmd) & _IOC_READ)
        error = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else
        error = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (error) return -EFAULT; 

    switch(cmd) {
    case INTTIMER_READ_REG0:
        // printk (KERN_INFO "fgpio/ioctl: returning Data register: %p\n", inttimerp->mapbase);
        if (access_ok(VERIFY_WRITE, (const void *)arg, sizeof(unsigned int))) {
            *((unsigned int *)arg) = ioread32(inttimerp->mapbase);
        }
        break;
    case INTTIMER_WRITE_FREQL:
        data = (arg & 0x0ffff);
        printk (KERN_INFO "fgpio/ioctl: Writing freq Low Data <%04x, to register: %p\n",data, (inttimerp->mapbase + 0x08));
        if (access_ok(VERIFY_READ, (const void *)arg, sizeof(unsigned int))) {
            iowrite32(data, inttimerp->mapbase + 0x08);         // Counter Freq Low
        }
        break;
    case INTTIMER_WRITE_FREQH:
        data = (arg & 0x0ffff);
        printk (KERN_INFO "fgpio/ioctl: Writing freq High Data <%04x, to register: %p\n",data, (inttimerp->mapbase + 0x08));
        if (access_ok(VERIFY_READ, (const void *)arg, sizeof(unsigned int))) {
            iowrite32(data, inttimerp->mapbase + 0x0c); // Counter Freq Hi
        }
        break;
    default:
        up(&inttimerp->sem);
        return -ENOTTY;  // NOTE: can't happen as we already checked against FGPIO_IOC_MAXNUMBER
    }
    up(&inttimerp->sem);
    return error;
}

static struct file_operations hw_fops = {
	.owner	         = THIS_MODULE,
	.read	         = hw_read,
	.write	         = hw_write,
	.unlocked_ioctl	 = hw_ioctl,
	.open	         = hw_open,
	.release         = hw_release,
};

static struct of_device_id altera_inttimer_of_match[] = {
    { .compatible = "altr,cntr-1.0", },
    {},
};
MODULE_DEVICE_TABLE(of, altera_inttimer_of_match);

static int alt_inttimer_remove(struct platform_device *pdev) {
    struct inttimer_port *inttimerp = dev_get_drvdata(&pdev->dev);

    if (inttimerp != NULL)
    {
        if (inttimerp->cdev_added)
        {
            cdev_del (&inttimerp->cdev);
        }
        
        if (inttimerp->mapbase != NULL)
        {
            iounmap(inttimerp->mapbase);
        }

        if (inttimerp->iomem_resource.start != 0)
        {
            release_mem_region (inttimerp->iomem_resource.start, resource_size(&inttimerp->iomem_resource));
        }
        
        if (!IS_ERR(inttimerp->dev))
        {
            device_destroy(alt_intervaltimer_class, inttimerp->dev->devt);
        }
        
        if (inttimerp->minor >= 0)
        {
            ida_simple_remove(&minor_allocator, inttimerp->minor);
        }
        
        kfree (inttimerp);
    }
    return 0;
}

static int alt_inttimer_probe(struct platform_device *pdev) {

    int result;
    dev_t devno;
    int rc = 0;
    struct inttimer_port *inttimerp;

    inttimerp = kzalloc(sizeof(struct inttimer_port), GFP_KERNEL);
    if (inttimerp == NULL)
    {
        return -ENOMEM;
    }
    sema_init(&inttimerp->sem, 1);
    dev_set_drvdata(&pdev->dev, inttimerp);

    inttimerp->minor = ida_simple_get(&minor_allocator, 0, MAX_INTTIMER_DEVICES, GFP_KERNEL);
    if (inttimerp->minor < 0)
    {
        alt_inttimer_remove(pdev);
        return inttimerp->minor;
    }
    devno = MKDEV(major_number, inttimerp->minor); 
    inttimerp->dev = device_create(alt_intervaltimer_class, NULL, devno, NULL, "alttimer%d", MINOR(devno));
    if (IS_ERR(inttimerp->dev)) {
        printk ("fgpio: can't create alt_inttimer_device %x\n", MINOR(devno));
        alt_inttimer_remove(pdev);
        return PTR_ERR(inttimerp->dev);
    }

    rc = of_address_to_resource(pdev->dev.of_node, 0, &inttimerp->iomem_resource);
    if (rc) {
        printk("GPIO PROBE Can't get address of resource\n");
        alt_inttimer_remove(pdev);
        return rc;
    }
    // printk("alt_inttimer PROBE Assigning address (FDT) %x to minor %x\n", res.start, i);

    // if (!request_mem_region (res.start, NR_PORTS, "fluke_gpio")) {
    if (!request_mem_region(inttimerp->iomem_resource.start, resource_size(&inttimerp->iomem_resource), "alt_interval_timer")) {
        inttimerp->iomem_resource.start = 0; // zero start so alt_inttimer_remove knows iomem was not obtained
        printk (KERN_INFO "alt_inttimer: can't get memory region %pa for alttimer%d\n", &inttimerp->iomem_resource, MINOR(devno));
        alt_inttimer_remove(pdev);
        return -ENODEV;
    }
    
    inttimerp->mapbase = ioremap_nocache(inttimerp->iomem_resource.start, resource_size(&inttimerp->iomem_resource));

    cdev_init(&inttimerp->cdev, &hw_fops);
    inttimerp->cdev.owner = THIS_MODULE;
    result = cdev_add(&inttimerp->cdev, devno, 1);
    inttimerp->cdev_added = result == 0;
    
    if(result)
        printk (KERN_INFO "alt_timer: Error %d adding alttimer%d\n", result, MINOR(devno));
    else
        printk (KERN_INFO "alt_timer: Registering alttimer%d on Major %d, Minor %d\n", MINOR(devno), major_number, MINOR(devno));
    return result;
}

static struct platform_driver inttimer_platform_driver = {
    .probe = alt_inttimer_probe,
    .remove = alt_inttimer_remove,
    .driver = {
                  .name = "altera_inttimer",
                  .owner = THIS_MODULE,
                  .of_match_table = altera_inttimer_of_match,
              },
};

static int __init inttimer_init (void) {
    int result;
    dev_t devt;
    
    alt_intervaltimer_class = class_create(THIS_MODULE, "altera-int-timer");    // Create the sys/class entry
    if (IS_ERR(alt_intervaltimer_class)) {
        printk ("inttimer_init: can't create alt_intervaltimer_class\n");
        return PTR_ERR(alt_intervaltimer_class); 
    }

    /* First, let's get the devices we need /dev/alttimer0 - /dev/alttimerN */
    result = alloc_chrdev_region(&devt, 0, MAX_INTTIMER_DEVICES, "alttimer");
    if (result < 0) {
        printk (KERN_INFO "inttimer_init: can't register alttimer devices /dev/alttimerX\n");
        class_destroy(alt_intervaltimer_class);
        return result;
    }
    major_number = MAJOR(devt);

    result = platform_driver_register(&inttimer_platform_driver);
    if (result) {
        printk (KERN_INFO "inttimer_init: platform_register failed!\n");
        unregister_chrdev_region(MKDEV(major_number, 0), MAX_INTTIMER_DEVICES);
        class_destroy(alt_intervaltimer_class);
        return result;
    }

    return 0;
}

static void __exit inttimer_exit(void) {
    platform_driver_unregister(&inttimer_platform_driver);
    unregister_chrdev_region(MKDEV(major_number, 0), MAX_INTTIMER_DEVICES);
    class_destroy(alt_intervaltimer_class);
}

module_init(inttimer_init);
module_exit(inttimer_exit);
