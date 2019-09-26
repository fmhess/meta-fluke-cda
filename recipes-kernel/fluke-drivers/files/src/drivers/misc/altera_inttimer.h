/*
    This driver supports the Altera interval timer. 
        
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

#ifndef alt_inttimer_h
#define alt_inttimer_h

#include <linux/cdev.h>
#include <linux/semaphore.h>


#define INTTIMER_IOC_MAXNUMBER    3


struct inttimer_port {
    void *mapbase;
    struct resource iomem_resource;
    struct       semaphore sem;
    struct       cdev cdev;
    struct device* dev;
	int minor;
    int cdev_added : 1;
};


#endif /* alt_inttimer_h */
