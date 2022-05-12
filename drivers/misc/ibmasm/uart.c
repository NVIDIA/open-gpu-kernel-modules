// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * IBM ASM Service Processor Device Driver
 *
 * Copyright (C) IBM Corporation, 2004
 *
 * Author: Max Asböck <amax@us.ibm.com>
 */

#include <linux/termios.h>
#include <linux/tty.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <linux/serial_8250.h>
#include "ibmasm.h"
#include "lowlevel.h"


void ibmasm_register_uart(struct service_processor *sp)
{
	struct uart_8250_port uart;
	void __iomem *iomem_base;

	iomem_base = sp->base_address + SCOUT_COM_B_BASE;

	/* read the uart scratch register to determine if the UART
	 * is dedicated to the service processor or if the OS can use it
	 */
	if (0 == readl(iomem_base + UART_SCR)) {
		dev_info(sp->dev, "IBM SP UART not registered, owned by service processor\n");
		sp->serial_line = -1;
		return;
	}

	memset(&uart, 0, sizeof(uart));
	uart.port.irq		= sp->irq;
	uart.port.uartclk	= 3686400;
	uart.port.flags		= UPF_SHARE_IRQ;
	uart.port.iotype	= UPIO_MEM;
	uart.port.membase	= iomem_base;

	sp->serial_line = serial8250_register_8250_port(&uart);
	if (sp->serial_line < 0) {
		dev_err(sp->dev, "Failed to register serial port\n");
		return;
	}
	enable_uart_interrupts(sp->base_address);
}

void ibmasm_unregister_uart(struct service_processor *sp)
{
	if (sp->serial_line < 0)
		return;

	disable_uart_interrupts(sp->base_address);
	serial8250_unregister_port(sp->serial_line);
}
