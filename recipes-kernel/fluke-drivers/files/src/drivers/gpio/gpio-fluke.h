/*
    gpio-fluke.h -- This driver supports the Fluke custom Altera GPIO ports
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

#ifndef gpio_fluke_h
#define gpio_fluke_h

#include <linux/cdev.h>
#include <linux/semaphore.h>


#define FGPIO_IOC_MAXNUMBER    5

#define Q_SIZE               128

struct interrupt_data {
    unsigned int irq_data[256];
    unsigned char irq_count;
};

struct fgpio_port {
    unsigned int mapbase;
    unsigned int irq;
    unsigned int bits;
    unsigned int direction;
    unsigned int edge;
    unsigned int minor;
    unsigned int q_events;
    struct       interrupt_data irqd; /* set by handler on interrupt */
    struct       semaphore sem;
    struct       cdev cdev;
};

struct Queue {
    int Capacity;
    int Front;
    int Rear;
    int Size;
    unsigned int Array[Q_SIZE];
};


#endif /* gpio_fluke_h */
