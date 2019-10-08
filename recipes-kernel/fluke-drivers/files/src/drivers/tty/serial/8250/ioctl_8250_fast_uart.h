/*
 *  ioctl_8250_fast_uart.h -- This driver supports the Fluke custom FAST UART
 *       
 *  Copyright (C) 2016 Joseph Joshy
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 
#ifndef IOCTL_8250_FAST_UART_H
#define IOCTL_8250_FAST_UART_H

#include <asm/ioctl.h>
#include <linux/types.h>

#define FAST_UART_CODE 170

enum fast_uart_ioctl
{
	READ_UART_WATERMARK = _IOWR( FAST_UART_CODE, 1, int ),
	WRITE_UART_WATERMARK = _IOWR( FAST_UART_CODE, 2, int ),
	READ_UART_TIMEOUT = _IOWR( FAST_UART_CODE, 3, int ),
	WRITE_UART_TIMEOUT = _IOWR( FAST_UART_CODE, 4, int )
};

#endif /* IOCTL_8250_FAST_UART_H */
