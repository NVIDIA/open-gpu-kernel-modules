// SPDX-License-Identifier: GPL-2.0
/*
 * Freescale 16550 UART "driver", Copyright (C) 2011 Paul Gortmaker.
 * Copyright 2020 NXP
 * Copyright 2020 Puresoftware Ltd.
 *
 * This isn't a full driver; it just provides an alternate IRQ
 * handler to deal with an errata and provide ACPI wrapper.
 * Everything else is just using the bog standard 8250 support.
 *
 * We follow code flow of serial8250_default_handle_irq() but add
 * a check for a break and insert a dummy read on the Rx for the
 * immediately following IRQ event.
 *
 * We re-use the already existing "bug handling" lsr_saved_flags
 * field to carry the "what we just did" information from the one
 * IRQ event to the next one.
 */

#include <linux/acpi.h>
#include <linux/serial_reg.h>
#include <linux/serial_8250.h>

#include "8250.h"

struct fsl8250_data {
	int	line;
};

int fsl8250_handle_irq(struct uart_port *port)
{
	unsigned char lsr, orig_lsr;
	unsigned int iir;
	struct uart_8250_port *up = up_to_u8250p(port);

	spin_lock(&up->port.lock);

	iir = port->serial_in(port, UART_IIR);
	if (iir & UART_IIR_NO_INT) {
		spin_unlock(&up->port.lock);
		return 0;
	}

	/* This is the WAR; if last event was BRK, then read and return */
	if (unlikely(up->lsr_saved_flags & UART_LSR_BI)) {
		up->lsr_saved_flags &= ~UART_LSR_BI;
		port->serial_in(port, UART_RX);
		spin_unlock(&up->port.lock);
		return 1;
	}

	lsr = orig_lsr = up->port.serial_in(&up->port, UART_LSR);

	/* Process incoming characters first */
	if ((lsr & (UART_LSR_DR | UART_LSR_BI)) &&
	    (up->ier & (UART_IER_RLSI | UART_IER_RDI))) {
		lsr = serial8250_rx_chars(up, lsr);
	}

	/* Stop processing interrupts on input overrun */
	if ((orig_lsr & UART_LSR_OE) && (up->overrun_backoff_time_ms > 0)) {
		unsigned long delay;

		up->ier = port->serial_in(port, UART_IER);
		if (up->ier & (UART_IER_RLSI | UART_IER_RDI)) {
			port->ops->stop_rx(port);
		} else {
			/* Keep restarting the timer until
			 * the input overrun subsides.
			 */
			cancel_delayed_work(&up->overrun_backoff);
		}

		delay = msecs_to_jiffies(up->overrun_backoff_time_ms);
		schedule_delayed_work(&up->overrun_backoff, delay);
	}

	serial8250_modem_status(up);

	if ((lsr & UART_LSR_THRE) && (up->ier & UART_IER_THRI))
		serial8250_tx_chars(up);

	up->lsr_saved_flags = orig_lsr;

	uart_unlock_and_check_sysrq(&up->port);

	return 1;
}
EXPORT_SYMBOL_GPL(fsl8250_handle_irq);

#ifdef CONFIG_ACPI
static int fsl8250_acpi_probe(struct platform_device *pdev)
{
	struct fsl8250_data *data;
	struct uart_8250_port port8250;
	struct device *dev = &pdev->dev;
	struct resource *regs;

	int ret, irq;

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
		dev_err(dev, "no registers defined\n");
		return -EINVAL;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	memset(&port8250, 0, sizeof(port8250));

	ret = device_property_read_u32(dev, "clock-frequency",
					&port8250.port.uartclk);
	if (ret)
		return ret;

	spin_lock_init(&port8250.port.lock);

	port8250.port.mapbase           = regs->start;
	port8250.port.irq               = irq;
	port8250.port.handle_irq        = fsl8250_handle_irq;
	port8250.port.type              = PORT_16550A;
	port8250.port.flags             = UPF_SHARE_IRQ | UPF_BOOT_AUTOCONF
						| UPF_FIXED_PORT | UPF_IOREMAP
						| UPF_FIXED_TYPE;
	port8250.port.dev               = dev;
	port8250.port.mapsize           = resource_size(regs);
	port8250.port.iotype            = UPIO_MEM;
	port8250.port.irqflags          = IRQF_SHARED;

	port8250.port.membase = devm_ioremap(dev,  port8250.port.mapbase,
							port8250.port.mapsize);
	if (!port8250.port.membase)
		return -ENOMEM;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->line = serial8250_register_8250_port(&port8250);
	if (data->line < 0)
		return data->line;

	platform_set_drvdata(pdev, data);
	return 0;
}

static int fsl8250_acpi_remove(struct platform_device *pdev)
{
	struct fsl8250_data *data = platform_get_drvdata(pdev);

	serial8250_unregister_port(data->line);
	return 0;
}

static const struct acpi_device_id fsl_8250_acpi_id[] = {
	{ "NXP0018", 0 },
	{ },
};
MODULE_DEVICE_TABLE(acpi, fsl_8250_acpi_id);

static struct platform_driver fsl8250_platform_driver = {
	.driver = {
		.name			= "fsl-16550-uart",
		.acpi_match_table	= ACPI_PTR(fsl_8250_acpi_id),
	},
	.probe			= fsl8250_acpi_probe,
	.remove			= fsl8250_acpi_remove,
};

module_platform_driver(fsl8250_platform_driver);
#endif
