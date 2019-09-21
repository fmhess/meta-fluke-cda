/*
 *  linux/drivers/char/fluke_keypad.c
 *  
 *  Copyright (C) 2004 Microtronix Datacom Ltd
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 * 
 *  Written by Dennis Scott <dennis@microtronix.com>
 */

// #include <linux/config.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/preempt.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/of_address.h>

#define FLUKE_5X5_KEYPAD_MAJOR 240
static int FLUKE_KEYPAD_MAJOR = FLUKE_5X5_KEYPAD_MAJOR;

#define FLUKE_KEYPAD_BUF_SIZE 128

static struct class  *fluke_keypad_class;
static struct device *fkpd_device = NULL;

struct fluke_keypad_data {
    void *mapbase;
	struct resource iomem_resource;
	int irq;
	int minor;
	unsigned int open_count;
	unsigned int buffer[FLUKE_KEYPAD_BUF_SIZE];
	unsigned int head;
	unsigned int tail;
	wait_queue_head_t queue;
	struct semaphore sem;
	struct cdev cdev;
};

static int fluke_keypad_open(struct inode* inode, struct file *filp)
{
	struct fluke_keypad_data *drv_data;
    // printk("fluke_keypad_open entry\n");

	drv_data = container_of(inode->i_cdev, struct fluke_keypad_data, cdev);
	filp->private_data = drv_data;

	preempt_disable();
	if (drv_data->open_count != 0) {
		drv_data->open_count++;
		preempt_enable();
		return 0;
	}

	/* Initialize buffer, queue and semaphore */
	drv_data->head = 0;
	drv_data->tail = 0;
    /*
	init_waitqueue_head(&drv_data->queue);
    sema_init(&drv_data->sem, 1);
    */
	
	drv_data->open_count++;
	preempt_enable();
    // printk("fluke_keypad_open returning success\n");
	return 0;
}

static int fluke_keypad_release(struct inode *inode, struct file *filp)
{
	struct fluke_keypad_data *drv_data = (struct fluke_keypad_data*)filp->private_data;

	preempt_disable();
	drv_data->open_count = 0;
	preempt_enable();

	return 0;
}

static ssize_t fluke_keypad_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	size_t total, int_count;
	struct fluke_keypad_data *drv_data = (struct fluke_keypad_data*)filp->private_data;
		
	if(count < 8)
	{
		printk("fluke_keypad%d: read requires at least 8 byte buffer, got %zu\n",
				drv_data->minor, count);
		return -EIO;
	}
    
	// printk("fluke_keypad_read: entry\n");


	if (down_interruptible(&drv_data->sem)) {
		// printk("fluke_keypad: down_interruptible\n");
		return -ERESTARTSYS;
    }

	while (drv_data->head == drv_data->tail) {
		DEFINE_WAIT(wait);
		up(&drv_data->sem);
		if (filp->f_flags & O_NONBLOCK) {
            // printk("O_NONBLOCK\n");
			return -EAGAIN;
        }

		/* In all cases but a pointer wrap, only one process is needed
		   to clear the buffer, so just wake one at a time with "exclusive" */
		prepare_to_wait_exclusive(&drv_data->queue, &wait, TASK_INTERRUPTIBLE);
		if(drv_data->head == drv_data->tail)
			schedule();
		finish_wait(&drv_data->queue, &wait);
		if (down_interruptible(&drv_data->sem)) {
            // printk("down_interruptible again\n");
			return -ERESTARTSYS;
        }
	}
	
	int_count = count / 4;
	if( drv_data->head > drv_data->tail ) {
		total = min(int_count, (size_t)(drv_data->head - drv_data->tail));
	} else {
		/* pointers wrapped, copy from tail to the end of the buffer */
		total = min(int_count, (size_t)(FLUKE_KEYPAD_BUF_SIZE - drv_data->tail));
	}
	
	if(copy_to_user(buf, &drv_data->buffer[drv_data->tail], total * 4)) {
		up(&drv_data->sem);
        // printk(KERN_WARNING "COPY TO USER \n");
        // printk("COPY TO USER \n");
		return -EFAULT;
	}

	drv_data->tail += total;
	if( drv_data->tail >= FLUKE_KEYPAD_BUF_SIZE)
		drv_data->tail -= FLUKE_KEYPAD_BUF_SIZE;

	up(&drv_data->sem);
	// printk("fluke_keypad_read: read %d bytes\n", total);
	return total;
}

static struct file_operations fluke_keypad_fops = {
	.owner   = THIS_MODULE,
	.open    = fluke_keypad_open,
	.release = fluke_keypad_release,
	.read    = fluke_keypad_read,
};

static irq_handler_t fluke_keypad_handler(int irq, void *dev_id, struct pt_regs *regs)
{

    struct device *dev = (struct device *)dev_id;
    struct fluke_keypad_data *drv_data = dev_get_drvdata(dev);
    unsigned int keycode = 0x00;
    unsigned int kpd_reg = 0x00;
    unsigned char i;
    unsigned char j;
    const unsigned char NUM_REGISTERS = 2;
    const unsigned char NUM_BITS_PER_REGISTER = 32;


	if (!dev_id) {
        // printk("interrupt routine NULL DEVICE YIKES\n");
		return IRQ_NONE;
    }

    // printk("fluke keypad interrupt routine\n");

    // drv_data = dev_get_drvdata((struct device *)dev_id);
    // drv_data = dev_get_drvdata(fkpd_device);
    /*
    if (!drv_data) {
        printk("interrupt routine NULL DEVICE DOUBLE YIKES\n");
        return IRQ_NONE;
    }
    */

    kpd_reg = readl(drv_data->mapbase);

	/* read the keypad code, clears the interrupt */
    for (i = 0; i < NUM_REGISTERS; i++) {
        for (j = 0; j < NUM_BITS_PER_REGISTER; j++) {
            if (kpd_reg & (0x01 << j)) {
                keycode = ((j + 1) + (NUM_BITS_PER_REGISTER * i)); 
                // printk ("fkpad: adding %x to keypad buffer\n", keycode);
                goto ADD_TO_BUFF;
            }
        }
        kpd_reg = readl(drv_data->mapbase + 4);
        // printk("interrupt routine kpd_register1 = %d\n", kpd_reg);
    }
    // printk ("fkpad: adding 0 to keypad buffer, could not find key\n");

    ADD_TO_BUFF:

    // printk ("fkpad: adding%d to keypad buffer\n", keycode);

	/* add it to the buffer if the device is open */
	if (drv_data->open_count != 0)
	{
		// if the buffer is full we discard the input (ugly to avoid % in irq)
		if(!(((drv_data->head + 1) == drv_data->tail) ||
				((drv_data->head == (FLUKE_KEYPAD_BUF_SIZE-1)) &&
				 (drv_data->tail == 0)))) {
			drv_data->buffer[drv_data->head] = keycode;
			drv_data->head++;
			if (drv_data->head == FLUKE_KEYPAD_BUF_SIZE)
				drv_data->head = 0;
		}
	}
	wake_up_interruptible(&(drv_data->queue));
    return (irq_handler_t) IRQ_HANDLED;
}

static struct of_device_id fluke_keypad_of_match[] = {
    { .compatible = "flk,fkeypad-1.0", },
    {},
};

MODULE_DEVICE_TABLE(of, fluke_keypad_of_match);

static int fluke_keypad_probe(struct platform_device *pdev)
{
    int result;
    int devno;
    int rc = 0;
    int irq;
	struct fluke_keypad_data *drv_data;

    devno = MKDEV(FLUKE_KEYPAD_MAJOR, 0);
    fkpd_device = device_create(fluke_keypad_class, NULL, devno, NULL, "fkeypd%d", 0);
    if (IS_ERR(fkpd_device )) {
        printk ("fkeypd: can't create fluke_keypad_device %x\n", 0);
        // release_ports();
        return PTR_ERR(fkpd_device);
    }

    drv_data = kzalloc(sizeof(struct fluke_keypad_data), GFP_KERNEL);
	if (drv_data == NULL) return -ENOMEM;
    dev_set_drvdata(&pdev->dev, drv_data);

	drv_data->minor = 0;

    rc = of_address_to_resource(pdev->dev.of_node, 0, &drv_data->iomem_resource);
    if (rc) {
		printk("FLUKE KEYPAD PROBE Can't get address of resource\n");
		return rc;
    }
    // printk("FLUKE KEYPAD PROBE Assigning address (FDT) %x to minor %x\n", res.start, 0);

    if (!request_mem_region(drv_data->iomem_resource.start, resource_size(&drv_data->iomem_resource), "fluke_keypad")) {
        printk (KERN_INFO "fluke_keypad: can't get memory region %pa for fkeypd%d\n", &drv_data->iomem_resource.start, 0);
        // release_ports();
        return -ENODEV;
    }
    drv_data->mapbase = ioremap_nocache(drv_data->iomem_resource.start, resource_size(&drv_data->iomem_resource));

    irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
    // printk("FLUKE KEYPAD PROBE: irq_of_parse_and_map returned irq number %d\n", irq);

#if !defined(IRQF_DISABLED)
#define IRQF_DISABLED 0x00
#endif
    rc = request_irq(irq, (irq_handler_t)fluke_keypad_handler, IRQF_DISABLED, "fkeypd", &pdev->dev);
    // rc = request_irq(irq, (irq_handler_t)fluke_keypad_handler, 0, "fkeypd", NULL);
    if (rc) {
        printk("FLUKE KEYPAD PROBE can't register IRQ\n");
        return -ENODEV;
    }


    cdev_init(&drv_data->cdev, &fluke_keypad_fops);

    sema_init(&drv_data->sem, 1);
	
    drv_data->cdev.owner = THIS_MODULE;
    drv_data->cdev.ops   = &fluke_keypad_fops;

    result = cdev_add(&drv_data->cdev, devno, 1);

	init_waitqueue_head(&drv_data->queue);

    if(result)
        printk (KERN_INFO "fluke_keypad: Error %d adding fkeypd%d\n", result, 0);
    else
        printk (KERN_INFO "fluke_keypad: Registering fkeypd0 on Major %d, Minor %d\n", FLUKE_KEYPAD_MAJOR, 0);
    return result;
}

static int fluke_keypad_remove(struct platform_device *pdev)
{
	struct fluke_keypad_data *drv_data = dev_get_drvdata(&pdev->dev);
	dev_t dev_num;

	if(drv_data) {
		cdev_del(&drv_data->cdev);
		if (drv_data->irq)
		{
			free_irq(drv_data->irq, &pdev->dev);
		}
		if (drv_data->mapbase != NULL)
		{
			iounmap(drv_data->mapbase);
		}
		if (drv_data->iomem_resource.start != 0)
		{
			release_mem_region(drv_data->iomem_resource.start, resource_size(&drv_data->iomem_resource));
		}
		
		dev_num = MKDEV(FLUKE_KEYPAD_MAJOR, drv_data->minor);
		device_destroy (fluke_keypad_class, dev_num);

		kfree(drv_data);
	}
	
	return 0;
}


static struct platform_driver fkeypd_platform_driver = {
    .probe =  fluke_keypad_probe,
    .remove = fluke_keypad_remove,
    .driver = {
                  .name = "fluke_keypad",
                  .owner = THIS_MODULE,
                  .of_match_table = fluke_keypad_of_match,
              },
};

static int __init fluke_keypad_init (void) {
    int result;
    dev_t dev;

    fluke_keypad_class = class_create(THIS_MODULE, "fluke-keypad");    // Create the sys/class entry
    if (IS_ERR(fluke_keypad_class)) {
        printk ("fkeypd: can't create fluke_keypad_class.\n");
        return PTR_ERR(fluke_keypad_class);
    }
    printk (KERN_INFO "fluke_keypad: registered fluke_keypad_class.\n");

    /* First, let's get the devices we need /dev/fkeypd0 - /dev/fkeypd */
    dev = MKDEV(FLUKE_KEYPAD_MAJOR, 0);
    result = register_chrdev_region(dev, 1, "fkeypd");
    if (result < 0) {
        printk (KERN_INFO "fkeypd: can't register keypad /dev/fkeypd0\n");
        // release_ports();
        return result;
    }

    result = platform_driver_register(&fkeypd_platform_driver);
    if (result) {
        printk (KERN_INFO "fkeypd: platform_register failed!\n");
        // release_ports();
        return result;
    }

    return 0;
}

static void fluke_keypad_exit(void)
{
	dev_t dev;
	
    // release_ports();
	platform_driver_unregister(&fkeypd_platform_driver);
	
    dev = MKDEV(FLUKE_KEYPAD_MAJOR, 0);
	unregister_chrdev_region(dev, 1);
	
	class_destroy(fluke_keypad_class);
}

module_init(fluke_keypad_init);
module_exit(fluke_keypad_exit);

MODULE_LICENSE("GPL");
