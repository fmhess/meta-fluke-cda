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
#define INTTIMER_MAJOR    94	
#define NR_DEVICES      8

MODULE_AUTHOR ("Alex j. Dorchak");
MODULE_DESCRIPTION("Driver for Altera Interval Timer Core");
MODULE_LICENSE("GPL");

static struct class *alt_intervaltimer_class;
static struct device* inttimer_device = NULL;
static const int BEEPER_COUNTER_FREQ = 60000000;
static struct inttimer_port inttimer_ports[NR_DEVICES];

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

static inline void release_ports (void) {
    int i;

    for (i = 0; i < NR_DEVICES; i++) {
        iounmap (inttimer_ports[i].mapbase);
		//FIXME passing wrong arg to release_mem_region
        //release_mem_region (inttimer_ports[i].mapbase, NR_PORTS);
    }
}

static struct of_device_id altera_inttimer_of_match[] = {
    { .compatible = "altr,cntr-1.0", },
    {},
};
MODULE_DEVICE_TABLE(of, altera_inttimer_of_match);

static int alt_inttimer_probe(struct platform_device *pdev) {

    static int i = 0;
    int result;
    int devno;
    int rc = 0;
    struct resource res;

    devno = MKDEV(INTTIMER_MAJOR, i); 
    inttimer_device = device_create(alt_intervaltimer_class, NULL, devno, NULL, "alttimer%d", i);
    if (IS_ERR(inttimer_device)) {
        printk ("fgpio: can't create alt_inttimer_device %x\n", i);
        // release_ports();
        return PTR_ERR(inttimer_device);
    }

    inttimer_ports[i].minor     = i; 
    rc = of_address_to_resource(pdev->dev.of_node, 0, &res);
    if (rc) {
        printk("GPIO PROBE Can't get address of resource\n");
    }
    // printk("alt_inttimer PROBE Assigning address (FDT) %x to minor %x\n", res.start, i);

    // if (!request_mem_region (res.start, NR_PORTS, "fluke_gpio")) {
    if (!request_mem_region(res.start, (res.end - res.start + 1), "alt_interval_timer")) {
        printk (KERN_INFO "alt_inttimer: can't get memory region %pa for alttimer%d\n", &res.start, i);
        release_ports();
        return -ENODEV;
    }
    inttimer_ports[i].mapbase = ioremap_nocache(res.start, (res.end - res.start + 1));

    cdev_init(&inttimer_ports[i].cdev, &hw_fops);

    sema_init(&inttimer_ports[i].sem, 1);
    
    inttimer_ports[i].cdev.owner = THIS_MODULE;
    inttimer_ports[i].cdev.ops   = &hw_fops;

    result = cdev_add(&inttimer_ports[i].cdev, devno, 1);

    if(result)
        printk (KERN_INFO "alt_timer: Error %d adding alttimer%d\n", result, i);
    else
        printk (KERN_INFO "alt_timer: Registering alttimer%d on Major %d, Minor %d\n", i, INTTIMER_MAJOR, i);
    i++;
    return result;
}

static struct platform_driver inttimer_platform_driver = {
    .probe = alt_inttimer_probe,
//    .remove = alt_inttimer_remove,
    .driver = {
                  .name = "altera_inttimer",
                  .owner = THIS_MODULE,
                  .of_match_table = altera_inttimer_of_match,
              },
};

static int __init inttimer_init (void) {
    int result;
    dev_t dev;

    alt_intervaltimer_class = class_create(THIS_MODULE, "altera-int-timer");    // Create the sys/class entry
    if (IS_ERR(alt_intervaltimer_class)) {
        printk ("inttimer_init: can't create alt_intervaltimer_class\n");
        return PTR_ERR(alt_intervaltimer_class); 
    }

    /* First, let's get the devices we need /dev/alttimer0 - /dev/alttimerN */
    dev = MKDEV(INTTIMER_MAJOR, 0); 
    result = register_chrdev_region(dev, NR_DEVICES, "alttimer");
    if (result < 0) {
        printk (KERN_INFO "inttimer_init: can't register alttimer devices /dev/alttimerX\n");
        // release_ports();
        return result;
    }

    result = platform_driver_register(&inttimer_platform_driver);
    if (result) {
        printk (KERN_INFO "inttimer_init: platform_register failed!\n");
        release_ports();
        return result;
    }

    return 0;
}

static void __exit inttimer_exit(void) {
    release_ports();
    unregister_chrdev_region(MKDEV(INTTIMER_MAJOR, 0), NR_DEVICES);
}

module_init(inttimer_init);
module_exit(inttimer_exit);
