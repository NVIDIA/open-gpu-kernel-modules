// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-omap1/serial.c
 *
 * OMAP1 serial support.
 */
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <asm/mach-types.h>

#include <mach/mux.h>

#include "pm.h"
#include "soc.h"

static struct clk * uart1_ck;
static struct clk * uart2_ck;
static struct clk * uart3_ck;

static inline unsigned int omap_serial_in(struct plat_serial8250_port *up,
					  int offset)
{
	offset <<= up->regshift;
	return (unsigned int)__raw_readb(up->membase + offset);
}

static inline void omap_serial_outp(struct plat_serial8250_port *p, int offset,
				    int value)
{
	offset <<= p->regshift;
	__raw_writeb(value, p->membase + offset);
}

/*
 * Internal UARTs need to be initialized for the 8250 autoconfig to work
 * properly. Note that the TX watermark initialization may not be needed
 * once the 8250.c watermark handling code is merged.
 */
static void __init omap_serial_reset(struct plat_serial8250_port *p)
{
	omap_serial_outp(p, UART_OMAP_MDR1,
			UART_OMAP_MDR1_DISABLE);	/* disable UART */
	omap_serial_outp(p, UART_OMAP_SCR, 0x08);	/* TX watermark */
	omap_serial_outp(p, UART_OMAP_MDR1,
			UART_OMAP_MDR1_16X_MODE);	/* enable UART */

	if (!cpu_is_omap15xx()) {
		omap_serial_outp(p, UART_OMAP_SYSC, 0x01);
		while (!(omap_serial_in(p, UART_OMAP_SYSC) & 0x01));
	}
}

static struct plat_serial8250_port serial_platform_data[] = {
	{
		.mapbase	= OMAP1_UART1_BASE,
		.irq		= INT_UART1,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP16XX_BASE_BAUD * 16,
	},
	{
		.mapbase	= OMAP1_UART2_BASE,
		.irq		= INT_UART2,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP16XX_BASE_BAUD * 16,
	},
	{
		.mapbase	= OMAP1_UART3_BASE,
		.irq		= INT_UART3,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= OMAP16XX_BASE_BAUD * 16,
	},
	{ },
};

static struct platform_device serial_device = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_PLATFORM,
	.dev			= {
		.platform_data	= serial_platform_data,
	},
};

/*
 * Note that on Innovator-1510 UART2 pins conflict with USB2.
 * By default UART2 does not work on Innovator-1510 if you have
 * USB OHCI enabled. To use UART2, you must disable USB2 first.
 */
void __init omap_serial_init(void)
{
	int i;

	if (cpu_is_omap7xx()) {
		serial_platform_data[0].regshift = 0;
		serial_platform_data[1].regshift = 0;
		serial_platform_data[0].irq = INT_7XX_UART_MODEM_1;
		serial_platform_data[1].irq = INT_7XX_UART_MODEM_IRDA_2;
	}

	if (cpu_is_omap15xx()) {
		serial_platform_data[0].uartclk = OMAP1510_BASE_BAUD * 16;
		serial_platform_data[1].uartclk = OMAP1510_BASE_BAUD * 16;
		serial_platform_data[2].uartclk = OMAP1510_BASE_BAUD * 16;
	}

	for (i = 0; i < ARRAY_SIZE(serial_platform_data) - 1; i++) {

		/* Don't look at UARTs higher than 2 for omap7xx */
		if (cpu_is_omap7xx() && i > 1) {
			serial_platform_data[i].membase = NULL;
			serial_platform_data[i].mapbase = 0;
			continue;
		}

		/* Static mapping, never released */
		serial_platform_data[i].membase =
			ioremap(serial_platform_data[i].mapbase, SZ_2K);
		if (!serial_platform_data[i].membase) {
			printk(KERN_ERR "Could not ioremap uart%i\n", i);
			continue;
		}
		switch (i) {
		case 0:
			uart1_ck = clk_get(NULL, "uart1_ck");
			if (IS_ERR(uart1_ck))
				printk("Could not get uart1_ck\n");
			else {
				clk_enable(uart1_ck);
				if (cpu_is_omap15xx())
					clk_set_rate(uart1_ck, 12000000);
			}
			break;
		case 1:
			uart2_ck = clk_get(NULL, "uart2_ck");
			if (IS_ERR(uart2_ck))
				printk("Could not get uart2_ck\n");
			else {
				clk_enable(uart2_ck);
				if (cpu_is_omap15xx())
					clk_set_rate(uart2_ck, 12000000);
				else
					clk_set_rate(uart2_ck, 48000000);
			}
			break;
		case 2:
			uart3_ck = clk_get(NULL, "uart3_ck");
			if (IS_ERR(uart3_ck))
				printk("Could not get uart3_ck\n");
			else {
				clk_enable(uart3_ck);
				if (cpu_is_omap15xx())
					clk_set_rate(uart3_ck, 12000000);
			}
			break;
		}
		omap_serial_reset(&serial_platform_data[i]);
	}
}

#ifdef CONFIG_OMAP_SERIAL_WAKE

static irqreturn_t omap_serial_wake_interrupt(int irq, void *dev_id)
{
	/* Need to do something with serial port right after wake-up? */
	return IRQ_HANDLED;
}

/*
 * Reroutes serial RX lines to GPIO lines for the duration of
 * sleep to allow waking up the device from serial port even
 * in deep sleep.
 */
void omap_serial_wake_trigger(int enable)
{
	if (!cpu_is_omap16xx())
		return;

	if (uart1_ck != NULL) {
		if (enable)
			omap_cfg_reg(V14_16XX_GPIO37);
		else
			omap_cfg_reg(V14_16XX_UART1_RX);
	}
	if (uart2_ck != NULL) {
		if (enable)
			omap_cfg_reg(R9_16XX_GPIO18);
		else
			omap_cfg_reg(R9_16XX_UART2_RX);
	}
	if (uart3_ck != NULL) {
		if (enable)
			omap_cfg_reg(L14_16XX_GPIO49);
		else
			omap_cfg_reg(L14_16XX_UART3_RX);
	}
}

static void __init omap_serial_set_port_wakeup(int gpio_nr)
{
	int ret;

	ret = gpio_request(gpio_nr, "UART wake");
	if (ret < 0) {
		printk(KERN_ERR "Could not request UART wake GPIO: %i\n",
		       gpio_nr);
		return;
	}
	gpio_direction_input(gpio_nr);
	ret = request_irq(gpio_to_irq(gpio_nr), &omap_serial_wake_interrupt,
			  IRQF_TRIGGER_RISING, "serial wakeup", NULL);
	if (ret) {
		gpio_free(gpio_nr);
		printk(KERN_ERR "No interrupt for UART wake GPIO: %i\n",
		       gpio_nr);
		return;
	}
	enable_irq_wake(gpio_to_irq(gpio_nr));
}

int __init omap_serial_wakeup_init(void)
{
	if (!cpu_is_omap16xx())
		return 0;

	if (uart1_ck != NULL)
		omap_serial_set_port_wakeup(37);
	if (uart2_ck != NULL)
		omap_serial_set_port_wakeup(18);
	if (uart3_ck != NULL)
		omap_serial_set_port_wakeup(49);

	return 0;
}

#endif	/* CONFIG_OMAP_SERIAL_WAKE */

static int __init omap_init(void)
{
	if (!cpu_class_is_omap1())
		return -ENODEV;

	return platform_device_register(&serial_device);
}
arch_initcall(omap_init);
