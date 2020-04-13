/*
    fgpio.c -- This driver supports the Fluke custom Altera GPIO ports
               for the  common platform.
        
    Copyright (C) 2011 Alex Dorchak (Fluke Corp)

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
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "fgpio-dev.h"
#include "gpio-fluke.h"

#define NR_PORTS	6	
#define NR_DEVICES     0x100 

MODULE_AUTHOR ("Alex j. Dorchak");
MODULE_DESCRIPTION("Fluke Custom Driver for Altera GPIO");
MODULE_LICENSE("GPL");

static struct class *fluke_gpio_class;
static unsigned fgpio_major;
static DEFINE_IDA(minor_allocator);

static int q_full(struct Queue *Q) {
    // printk(KERN_INFO "q_full: Size = %d, Capacity = %d\n", Q->Size, Q->Capacity);
    return Q->Size == Q->Capacity;
}

static int q_empty(struct Queue *Q) {
    return Q->Size == 0;
}

static void q_init(struct Queue *Q) {
    Q->Capacity = Q_SIZE;
    Q->Size     = 0;
    Q->Front    = 1;
    Q->Rear     = 0;
}

static int q_next(int Value, struct Queue *Q) {
    if (++Value == Q->Capacity) {
        Value = 0;
    }
    return Value;
}

static int q_enqueue(unsigned int X, struct Queue *Q) {
    if (q_full(Q)) {
        return -1;
    } 
    else {
        Q->Size++;
        Q->Rear = q_next(Q->Rear, Q);
        Q->Array[Q->Rear] = X;
        return 0;
    }
}

static unsigned int q_dequeue(struct Queue *Q) {

    unsigned int x = 0;

    if (!q_empty(Q)) {
        Q->Size--;
        x = Q->Array[Q->Front];
        Q->Front = q_next(Q->Front, Q);
    }
    return x;
}

static int hw_open (struct inode *inode, struct file *filp) {

    struct fgpio_port *fgpiop; /* device information */

    // printk("FGPIO open called \n");
    fgpiop =  container_of(inode->i_cdev, struct fgpio_port, cdev);
    filp->private_data = fgpiop;
    // printk("FGPIO open Exit \n");

    return 0;
}

static int hw_release (struct inode *inode, struct file *filp) {
    // printk("FGPIO release called \n");
	return 0;
}

static ssize_t hw_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    char *tbuf;
    int i;

    int retval = 0;
    
    struct fgpio_port *fgpiop = filp->private_data;

    // printk ("fgpio: read function CALLED!\n");

    if (down_interruptible(&fgpiop->sem))
        return -ERESTARTSYS;
    
    if (!(tbuf = kmalloc(count, GFP_ATOMIC))) {
        up(&fgpiop->sem);
        return -ENOMEM;
    }

    if(fgpiop->irq) {    /* if we're in IRQ mode use the Q */
        for (i = 0; i < count; i++) {
            tbuf[i] = q_dequeue(&fgpiop->Q);
            if (tbuf[i] == 0) {
                break;
            }
        }
        count = i;
    }
    else {                /* otherwise just take count readings (probably 1) */
        if (fgpiop->direction & 0x100)
            iowrite32(0x00, (int*)(fgpiop->mapbase + 4)); // try to make everything readable
        for (i = 0; i < count; i++) {
            tbuf[i] = (ioread32((int*)fgpiop->mapbase) & fgpiop->bits);
            rmb();
        }
        count = i;
    }

    if (copy_to_user(buf, tbuf, count)) {
        printk (KERN_INFO "fgpio: read function FAILED! addr = %p\n", fgpiop->mapbase);
        retval = -EFAULT; 
    }
    else 
        retval = count;

    kfree(tbuf);
    up(&fgpiop->sem);

    return retval;
}

static ssize_t hw_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    int i;
    int retval = 0;
    unsigned char data;
    unsigned char direction;
    unsigned char *tbuf;
    
    struct fgpio_port *fgpiop = filp->private_data;

    if (down_interruptible(&fgpiop->sem))
        return -ERESTARTSYS;

    if (!(tbuf = kmalloc(count, GFP_ATOMIC))) {
        printk("fgpio: write, kmalloc failed!\n");
        up(&fgpiop->sem);
        return -ENOMEM;
    }

    if (copy_from_user(tbuf, buf, count)) {
        printk (KERN_INFO "fgpio: write function FAILED! addr = %p\n", fgpiop->mapbase);
        retval = -EFAULT; 
    }
    else {
        direction = (fgpiop->direction & 0x100) ? 0xff : fgpiop->direction; 
        iowrite32(direction, (int*)(fgpiop->mapbase + 4)); // try to make everything writeable
        for (i = 0; i < count; i++) {
            data = tbuf[i] & fgpiop->bits & direction;
            iowrite32(data, (int*)fgpiop->mapbase);
        }
        retval = count;
    }

    kfree(tbuf);
    up(&fgpiop->sem);
    return retval;
}

//static ssize_t hw_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) {
static long hw_ioctl (struct file *filp, unsigned int cmd, unsigned long arg) {

    int error  = 0;
    struct fgpio_port *fgpiop = filp->private_data;

    // printk ("fgpio/ioctl: %8x\n", fgpiop->mapbase);

    if (_IOC_TYPE(cmd) != FGPIO_IOC_MAGIC_NUMBER)  return -ENOTTY;
    if (_IOC_NR(cmd)   >= FGPIO_IOC_MAXNUMBER) return -ENOTTY;

    if (_IOC_DIR(cmd) & _IOC_READ)
        error = !access_ok(/*VERIFY_WRITE, */(void __user *)arg, _IOC_SIZE(cmd));
    else
        error = !access_ok(/*VERIFY_READ, */(void __user *)arg, _IOC_SIZE(cmd));
    if (error) return -EFAULT; 

    if(down_interruptible(&fgpiop->sem)) return -ERESTARTSYS;

    switch(cmd) {
    case FGPIO_INTS_OFF:
        // printk (KERN_INFO "fgpio/ioctl: Turning interrupts off at addr: %8x\n", fgpiop->mapbase);
        if(fgpiop->irq) {    /* if we're in IRQ mode */
            iowrite32(0x00, (int*)(fgpiop->mapbase + (4 * 2)));
        }
        break;
    case FGPIO_GETINT_MASK:
        // printk (KERN_INFO "fgpio/ioctl: returning interrupt mask at addr: %8x\n", fgpiop->mapbase);
        if (access_ok(/*VERIFY_WRITE, */(const void *)arg, sizeof(unsigned int))) {
            if(fgpiop->irq) {    /* if we're in IRQ mode */
                *((unsigned int *)arg) = ioread32((unsigned int*)(fgpiop->mapbase + (4 * 2)));
            }
        }
        break;
    case FGPIO_SETINT_MASK:
        /*
        printk (KERN_INFO "fgpio/ioctl: setting interrupt mask (%x) at addr: %8x\n", 
                (unsigned int)arg, fgpiop->mapbase);
        */
        if (access_ok(/*VERIFY_READ, */(const void *)arg, sizeof(unsigned int))) {
            if(fgpiop->irq) {    /* if we're in IRQ mode */
                iowrite32(arg, (int*)(fgpiop->mapbase + (4 * 2)));
            }
        }
        break;
    case FGPIO_FLUSH_QUEUE:
        // printk (KERN_INFO "fgpio/ioctl: Flushing Queue (minor)%d\n", fgpiop->minor);
        q_init(&fgpiop->Q);
        break;
    case FGPIO_READ_REG0:
        // printk (KERN_INFO "fgpio/ioctl: returning Data register: %8x\n", fgpiop->mapbase);
        if (access_ok(/*VERIFY_WRITE, */(const void *)arg, sizeof(unsigned int))) {
            *((unsigned int *)arg) = ioread32((unsigned int*)(fgpiop->mapbase));
        }
        break;
    default:
        up(&fgpiop->sem);
        return -ENOTTY;  // NOTE: can't happen as we already checked against FGPIO_IOC_MAXNUMBER
    }
    up(&fgpiop->sem);
    return error;
}

irqreturn_t fgpio_irq(int irq, void *dev_id, struct pt_regs *regs) {
    int buf;
    struct fgpio_port *fgpiop = dev_id;

    down(&fgpiop->sem);

    if (fgpiop->direction & 0x100)
        iowrite32(0x00, (int*)(fgpiop->mapbase + 4)); // try to make everything readable

    buf = (ioread32((int*)(fgpiop->mapbase + (4 * 3))) & fgpiop->bits & ~fgpiop->direction);
    rmb();
    if (q_enqueue(buf, &fgpiop->Q)) {
        printk (KERN_INFO "fgpio: IRQ function! QUEUE FULL ERROR: Q[%x], data = %x, Cap = %d\n", 
                fgpiop->minor, (buf & fgpiop->bits), fgpiop->Q.Capacity);
    }
    iowrite32(0xff, (int*)(fgpiop->mapbase + (4 * 3)));

    up(&fgpiop->sem);
    return IRQ_HANDLED;
}


static struct file_operations hw_fops = {
	.owner	         = THIS_MODULE,
	.read	         = hw_read,
	.write	         = hw_write,
	.unlocked_ioctl	 = hw_ioctl,
	.open	         = hw_open,
	.release         = hw_release,
};

static struct of_device_id fluke_gpio_of_match[] = {
    { .compatible = "flk,fgpio-1.0", },
    {},
};
MODULE_DEVICE_TABLE(of, fluke_gpio_of_match);

static int fluke_gpio_remove(struct platform_device *pdev) {
    struct fgpio_port *fgpio_port = dev_get_drvdata (&pdev->dev);

    if (fgpio_port)
    {
        if (fgpio_port->cdev_added)
        {
            cdev_del(&fgpio_port->cdev);
        }
        
        if (fgpio_port->mapbase)
        {
            iounmap(fgpio_port->mapbase);
        }
        
        if (fgpio_port->iomem_resource.start)
        {
            release_mem_region(fgpio_port->iomem_resource.start, resource_size(&fgpio_port->iomem_resource));
        }
        
        if (!IS_ERR(fgpio_port->dev))
        {
            device_destroy(fluke_gpio_class, fgpio_port->dev->devt);
        }
        
        if (fgpio_port->minor >= 0)
        {
            ida_simple_remove(&minor_allocator, fgpio_port->minor);
        }
        
        kfree (fgpio_port);
    }
    return 0;
}

static int fluke_gpio_probe (struct platform_device *pdev) {
    int result;
    int devno;
    int rc = 0;
    struct fgpio_port *fgpio_port;
//    unsigned int x;
//    unsigned int configured_bits;
//    void *ptr_configured_bits;

    fgpio_port = kzalloc (sizeof (struct fgpio_port), GFP_KERNEL);
    if(fgpio_port == NULL) return -ENOMEM;
    sema_init(&fgpio_port->sem, 1);
/*AJD hardwire direction edge etc for now */
    fgpio_port->direction = 0x100;                // All I/O
    fgpio_port->edge      = 0x00;
    fgpio_port->bits = 0xff;
    q_init (&fgpio_port->Q);
/*AJD END hardwire direction edge etc for now */
    dev_set_drvdata (&pdev->dev, fgpio_port);

    fgpio_port->minor = ida_simple_get(&minor_allocator, 0, NR_DEVICES, GFP_KERNEL);
    if (fgpio_port->minor < 0)
    {
        fluke_gpio_remove(pdev);
        return fgpio_port->minor;
    }

    devno = MKDEV(fgpio_major, fgpio_port->minor); 
    fgpio_port->dev = device_create(fluke_gpio_class, NULL, devno, NULL, "fgpio%d", fgpio_port->minor);
    if (IS_ERR(fgpio_port->dev)) {
        printk ("fgpio: can't create fluke_gpio_device %x\n", fgpio_port->minor);
        fluke_gpio_remove(pdev);
        return PTR_ERR(fgpio_port->dev);
    }

    rc = of_address_to_resource(pdev->dev.of_node, 0, &fgpio_port->iomem_resource);
    if (rc) {
        printk("GPIO PROBE Can't get address of resource\n");
        fluke_gpio_remove(pdev);
        return rc;
    }
    // printk("GPIO PROBE Assigning address (FDT) %x to minor %x\n", res.start, i);

    if (!request_mem_region(fgpio_port->iomem_resource.start, resource_size(&fgpio_port->iomem_resource), "fluke_gpio")) {
        printk (KERN_INFO "fluke_gpio: can't get memory region %pa for fgpio%d\n", &fgpio_port->iomem_resource.start, fgpio_port->minor);
        fgpio_port->iomem_resource.start = 0;  // zero resource start so fluke_gpio_remove doesn't try to release region
        fluke_gpio_remove(pdev);
        return -ENODEV;
    }
    
    fgpio_port->mapbase = ioremap_nocache(fgpio_port->iomem_resource.start, resource_size(&fgpio_port->iomem_resource));
/*
    ptr_configured_bits = of_get_property(pdev->dev.of_node, "flk,gpio-bank-width", NULL);
    if (!ptr_configured_bits) {
        printk("GPIO PROBE Can't find any configured bits on port\n");
     // release_ports();  // AJD can I have a release port (singular here????)
        return PTR_ERR(fgpio_device);
    }
    // else {
*/
/*
    rc = of_property_read_u32(pdev->dev.of_node, "flk,gpio-bank-width", &configured_bits);
    if (rc) {
        // dev_warn(&pdev->dev, "No active bits defined for this device\n");
        printk("No active bits defined for this device\n");
    }
    else {

        // configured_bits = be32_to_cpup(ptr_configured_bits); 
        printk("GPIO PROBE building mask for (%x) bits on port %x\n", configured_bits, i);
        fgpio_ports[i].bits = 0;
        for (; configured_bits >= 0; configured_bits--) { 
            fgpio_ports[i].bits += (1 << (configured_bits - 1));
        }
        printk("GPIO PROBE configured bits (%x) on port %x\n", fgpio_ports[i].bits, i);
    }
*/

    /*
    fgpio_ports[i].direction = platp->direction_bits;
    fgpio_ports[i].edge      = platp->is_edge;
    */

    cdev_init(&fgpio_port->cdev, &hw_fops);
    fgpio_port->cdev.owner = THIS_MODULE;
    result = cdev_add(&fgpio_port->cdev, devno, 1);
    fgpio_port->cdev_added = result == 0;
    /*  
    if (platp->irq) {

        fgpio_ports[i].irq       = platp->irq;
        fgpio_ports[i].edge      = platp->is_edge;
        fgpio_ports[i].q_events  = platp->q_events;

        result = request_irq(fgpio_ports[i].irq, fgpio_irq, IRQF_DISABLED, "gpio", &fgpio_ports[i]); 
        if(result) {
            printk (KERN_INFO "fgpio: Can't register irq (%d) for gpio%d\n", fgpio_ports[i].irq, i);
        }
        else {
            // disable interrupts 
            iowrite32(0x00, (int*)(fgpio_ports[i].mapbase + (4 * 2)));
        }

    }
    */
    if(result)
        printk (KERN_INFO "fgpio: Error %d adding fgpio%d\n", result, fgpio_port->minor);
    else
        printk (KERN_INFO "fgpio: Registering fgpio%d on Major %d, Minor %d\n", fgpio_port->minor, fgpio_major, fgpio_port->minor);
    return result;
}

static struct platform_driver fgpio_platform_driver = {
    .probe = fluke_gpio_probe,
    .remove = fluke_gpio_remove,
    .driver = {
                  .name = "fluke_gpio",
                  .owner = THIS_MODULE,
                  .of_match_table = fluke_gpio_of_match,
              },
};

static int __init fgpio_init (void) {
    int result;
    dev_t dev;

    fluke_gpio_class = class_create(THIS_MODULE, "fluke-gpio");    // Create the sys/class entry
    if (IS_ERR(fluke_gpio_class)) {
        printk ("fgpio: can't create fluke_gpio_class.\n");
        return PTR_ERR(fluke_gpio_class); 
    }
    printk (KERN_INFO "fgpio: registered fluke_gpio_class.\n");

    /* First, let's get the devices we need /dev/fgpio0 - /dev/fgpio7 */
    result = alloc_chrdev_region(&dev, 0, NR_DEVICES, "fgpio");
    if (result < 0) {
        printk (KERN_INFO "fgpio: can't register FGPIO devices /dev/fgpioX\n");
        class_destroy(fluke_gpio_class);
        return result;
    }
    fgpio_major = MAJOR(dev);

    result = platform_driver_register(&fgpio_platform_driver);
    if (result) {
        printk (KERN_INFO "fgpio: platform_register failed!\n");
        unregister_chrdev_region(MKDEV(fgpio_major, 0), NR_DEVICES);
        class_destroy(fluke_gpio_class);
        return result;
    }

    return 0;
}

static void __exit fgpio_exit(void) {
    platform_driver_unregister(&fgpio_platform_driver);
    unregister_chrdev_region(MKDEV(fgpio_major, 0), NR_DEVICES);
    class_destroy(fluke_gpio_class);
}

module_init(fgpio_init);
module_exit(fgpio_exit);
