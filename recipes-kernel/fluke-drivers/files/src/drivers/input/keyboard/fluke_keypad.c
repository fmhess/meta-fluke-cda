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
#include <linux/input.h>
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

static int fluke_keypad_remove(struct platform_device *pdev);

#define FLUKE_KEYPAD_NUM_REGISTERS 2
#define FLUKE_KEYPAD_NUM_BITS_PER_REGISTER 32
#define FLUKE_KEYPAD_KEYMAP_SIZE (FLUKE_KEYPAD_NUM_REGISTERS * FLUKE_KEYPAD_NUM_BITS_PER_REGISTER)

struct fluke_keypad_data {
    void *mapbase;
	struct resource iomem_resource;
	int irq;
	struct input_dev *input_dev;
	unsigned char keycode[FLUKE_KEYPAD_KEYMAP_SIZE];
};

static irqreturn_t fluke_keypad_handler(int irq, void *dev_id)
{
	struct device *dev = (struct device *)dev_id;
	struct fluke_keypad_data *drv_data = dev_get_drvdata(dev);
	unsigned int kpd_reg;
	unsigned i;
	unsigned j;


	if (!dev_id) {
		// printk("interrupt routine NULL DEVICE YIKES\n");
		return IRQ_NONE;
	}

	// printk("fluke keypad interrupt routine\n");


	/* read the keypad code, clears the interrupt */
	for (i = 0; i < FLUKE_KEYPAD_NUM_REGISTERS; ++i) {
		kpd_reg = readl(drv_data->mapbase + 4 * i);
		// printk("interrupt routine kpd_register1 = %d\n", kpd_reg);
		for (j = 0; j < FLUKE_KEYPAD_NUM_BITS_PER_REGISTER; ++j) {
			input_report_key(drv_data->input_dev, 
				drv_data->keycode[i * FLUKE_KEYPAD_NUM_BITS_PER_REGISTER + j], 
				kpd_reg & (0x1 << j));
		}
	}
	input_sync(drv_data->input_dev);

	return IRQ_HANDLED;
}

static struct of_device_id fluke_keypad_of_match[] = {
	{ .compatible = "flk,fkeypad-1.0", },
	{},
};

MODULE_DEVICE_TABLE(of, fluke_keypad_of_match);

static int fluke_keypad_probe(struct platform_device *pdev)
{
	int rc = 0;
	int irq;
	unsigned i;
	struct fluke_keypad_data *drv_data;
	struct input_dev *input_dev;

	drv_data = kzalloc(sizeof(struct fluke_keypad_data), GFP_KERNEL);
	if (drv_data == NULL) return -ENOMEM;
	dev_set_drvdata(&pdev->dev, drv_data);
	for (i = 0; i < FLUKE_KEYPAD_KEYMAP_SIZE; ++i)
	{
		drv_data->keycode[i] = i + 1;
	}
	
	input_dev = input_allocate_device();
	if (input_dev == NULL)
	{
		fluke_keypad_remove(pdev);
		return -ENOMEM;
	}
	input_dev->name = "Fluke keypad";
	input_dev->id.bustype	= BUS_HOST;
	input_dev->id.vendor	= 0x0001;
	input_dev->id.product	= 0x0001;
	input_dev->id.version	= 0x0001;
	input_dev->keycode = drv_data->keycode;
	input_dev->keycodesize = sizeof(unsigned char);
	input_dev->keycodemax = FLUKE_KEYPAD_KEYMAP_SIZE;
	for (i = 0; i < FLUKE_KEYPAD_KEYMAP_SIZE; i++) 
	{
		input_set_capability(input_dev, EV_KEY, drv_data->keycode[i]);
	}
	rc = input_register_device(input_dev);
	if (rc)
	{
		input_free_device(input_dev);
		fluke_keypad_remove(pdev);
		return rc;
	}
	drv_data->input_dev = input_dev;
	
	rc = of_address_to_resource(pdev->dev.of_node, 0, &drv_data->iomem_resource);
	if (rc) {
		printk("FLUKE KEYPAD PROBE Can't get address of resource\n");
		fluke_keypad_remove(pdev);
		return rc;
	}
	// printk("FLUKE KEYPAD PROBE Assigning address (FDT) %x to minor %x\n", res.start, 0);

	if (!request_mem_region(drv_data->iomem_resource.start, resource_size(&drv_data->iomem_resource), "fluke_keypad")) {
		drv_data->iomem_resource.start = 0; // zero start so fluke_keypad_remove  knows the mem region was not obtained
		printk (KERN_INFO "fluke_keypad: can't get memory region %pa for fkeypd%d\n", &drv_data->iomem_resource.start, 0);
		fluke_keypad_remove(pdev);
		return -ENODEV;
	}
	drv_data->mapbase = ioremap_nocache(drv_data->iomem_resource.start, resource_size(&drv_data->iomem_resource));

	irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
	// printk("FLUKE KEYPAD PROBE: irq_of_parse_and_map returned irq number %d\n", irq);

	rc = request_irq(irq, fluke_keypad_handler, IRQF_SHARED, "fkeypd", &pdev->dev);
	if (rc) {
		printk("FLUKE KEYPAD PROBE can't register IRQ\n");
		fluke_keypad_remove(pdev);
		return -ENODEV;
	}

	printk (KERN_INFO "fluke_keypad: probe succeeded.\n");
	return 0;
}

static int fluke_keypad_remove(struct platform_device *pdev)
{
	struct fluke_keypad_data *drv_data = dev_get_drvdata(&pdev->dev);

	if(drv_data) {
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
		if (drv_data->input_dev)
		{
			input_unregister_device(drv_data->input_dev);
		}
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

	result = platform_driver_register(&fkeypd_platform_driver);
	if (result) {
		printk (KERN_INFO "fkeypd: platform_register failed!\n");
		return result;
	}

	return 0;
}

static void fluke_keypad_exit(void)
{
	platform_driver_unregister(&fkeypd_platform_driver);
}

module_init(fluke_keypad_init);
module_exit(fluke_keypad_exit);

MODULE_LICENSE("GPL");
