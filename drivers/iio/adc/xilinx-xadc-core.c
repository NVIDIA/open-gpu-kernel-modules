// SPDX-License-Identifier: GPL-2.0-only
/*
 * Xilinx XADC driver
 *
 * Copyright 2013-2014 Analog Devices Inc.
 *  Author: Lars-Peter Clausen <lars@metafoo.de>
 *
 * Documentation for the parts can be found at:
 *  - XADC hardmacro: Xilinx UG480
 *  - ZYNQ XADC interface: Xilinx UG585
 *  - AXI XADC interface: Xilinx PG019
 */

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/overflow.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#include <linux/iio/buffer.h>
#include <linux/iio/events.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#include "xilinx-xadc.h"

static const unsigned int XADC_ZYNQ_UNMASK_TIMEOUT = 500;

/* ZYNQ register definitions */
#define XADC_ZYNQ_REG_CFG	0x00
#define XADC_ZYNQ_REG_INTSTS	0x04
#define XADC_ZYNQ_REG_INTMSK	0x08
#define XADC_ZYNQ_REG_STATUS	0x0c
#define XADC_ZYNQ_REG_CFIFO	0x10
#define XADC_ZYNQ_REG_DFIFO	0x14
#define XADC_ZYNQ_REG_CTL		0x18

#define XADC_ZYNQ_CFG_ENABLE		BIT(31)
#define XADC_ZYNQ_CFG_CFIFOTH_MASK	(0xf << 20)
#define XADC_ZYNQ_CFG_CFIFOTH_OFFSET	20
#define XADC_ZYNQ_CFG_DFIFOTH_MASK	(0xf << 16)
#define XADC_ZYNQ_CFG_DFIFOTH_OFFSET	16
#define XADC_ZYNQ_CFG_WEDGE		BIT(13)
#define XADC_ZYNQ_CFG_REDGE		BIT(12)
#define XADC_ZYNQ_CFG_TCKRATE_MASK	(0x3 << 8)
#define XADC_ZYNQ_CFG_TCKRATE_DIV2	(0x0 << 8)
#define XADC_ZYNQ_CFG_TCKRATE_DIV4	(0x1 << 8)
#define XADC_ZYNQ_CFG_TCKRATE_DIV8	(0x2 << 8)
#define XADC_ZYNQ_CFG_TCKRATE_DIV16	(0x3 << 8)
#define XADC_ZYNQ_CFG_IGAP_MASK		0x1f
#define XADC_ZYNQ_CFG_IGAP(x)		(x)

#define XADC_ZYNQ_INT_CFIFO_LTH		BIT(9)
#define XADC_ZYNQ_INT_DFIFO_GTH		BIT(8)
#define XADC_ZYNQ_INT_ALARM_MASK	0xff
#define XADC_ZYNQ_INT_ALARM_OFFSET	0

#define XADC_ZYNQ_STATUS_CFIFO_LVL_MASK	(0xf << 16)
#define XADC_ZYNQ_STATUS_CFIFO_LVL_OFFSET	16
#define XADC_ZYNQ_STATUS_DFIFO_LVL_MASK	(0xf << 12)
#define XADC_ZYNQ_STATUS_DFIFO_LVL_OFFSET	12
#define XADC_ZYNQ_STATUS_CFIFOF		BIT(11)
#define XADC_ZYNQ_STATUS_CFIFOE		BIT(10)
#define XADC_ZYNQ_STATUS_DFIFOF		BIT(9)
#define XADC_ZYNQ_STATUS_DFIFOE		BIT(8)
#define XADC_ZYNQ_STATUS_OT		BIT(7)
#define XADC_ZYNQ_STATUS_ALM(x)		BIT(x)

#define XADC_ZYNQ_CTL_RESET		BIT(4)

#define XADC_ZYNQ_CMD_NOP		0x00
#define XADC_ZYNQ_CMD_READ		0x01
#define XADC_ZYNQ_CMD_WRITE		0x02

#define XADC_ZYNQ_CMD(cmd, addr, data) (((cmd) << 26) | ((addr) << 16) | (data))

/* AXI register definitions */
#define XADC_AXI_REG_RESET		0x00
#define XADC_AXI_REG_STATUS		0x04
#define XADC_AXI_REG_ALARM_STATUS	0x08
#define XADC_AXI_REG_CONVST		0x0c
#define XADC_AXI_REG_XADC_RESET		0x10
#define XADC_AXI_REG_GIER		0x5c
#define XADC_AXI_REG_IPISR		0x60
#define XADC_AXI_REG_IPIER		0x68

/* 7 Series */
#define XADC_7S_AXI_ADC_REG_OFFSET	0x200

/* UltraScale */
#define XADC_US_AXI_ADC_REG_OFFSET	0x400

#define XADC_AXI_RESET_MAGIC		0xa
#define XADC_AXI_GIER_ENABLE		BIT(31)

#define XADC_AXI_INT_EOS		BIT(4)
#define XADC_AXI_INT_ALARM_MASK		0x3c0f

#define XADC_FLAGS_BUFFERED BIT(0)

/*
 * The XADC hardware supports a samplerate of up to 1MSPS. Unfortunately it does
 * not have a hardware FIFO. Which means an interrupt is generated for each
 * conversion sequence. At 1MSPS sample rate the CPU in ZYNQ7000 is completely
 * overloaded by the interrupts that it soft-lockups. For this reason the driver
 * limits the maximum samplerate 150kSPS. At this rate the CPU is fairly busy,
 * but still responsive.
 */
#define XADC_MAX_SAMPLERATE 150000

static void xadc_write_reg(struct xadc *xadc, unsigned int reg,
	uint32_t val)
{
	writel(val, xadc->base + reg);
}

static void xadc_read_reg(struct xadc *xadc, unsigned int reg,
	uint32_t *val)
{
	*val = readl(xadc->base + reg);
}

/*
 * The ZYNQ interface uses two asynchronous FIFOs for communication with the
 * XADC. Reads and writes to the XADC register are performed by submitting a
 * request to the command FIFO (CFIFO), once the request has been completed the
 * result can be read from the data FIFO (DFIFO). The method currently used in
 * this driver is to submit the request for a read/write operation, then go to
 * sleep and wait for an interrupt that signals that a response is available in
 * the data FIFO.
 */

static void xadc_zynq_write_fifo(struct xadc *xadc, uint32_t *cmd,
	unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; i++)
		xadc_write_reg(xadc, XADC_ZYNQ_REG_CFIFO, cmd[i]);
}

static void xadc_zynq_drain_fifo(struct xadc *xadc)
{
	uint32_t status, tmp;

	xadc_read_reg(xadc, XADC_ZYNQ_REG_STATUS, &status);

	while (!(status & XADC_ZYNQ_STATUS_DFIFOE)) {
		xadc_read_reg(xadc, XADC_ZYNQ_REG_DFIFO, &tmp);
		xadc_read_reg(xadc, XADC_ZYNQ_REG_STATUS, &status);
	}
}

static void xadc_zynq_update_intmsk(struct xadc *xadc, unsigned int mask,
	unsigned int val)
{
	xadc->zynq_intmask &= ~mask;
	xadc->zynq_intmask |= val;

	xadc_write_reg(xadc, XADC_ZYNQ_REG_INTMSK,
		xadc->zynq_intmask | xadc->zynq_masked_alarm);
}

static int xadc_zynq_write_adc_reg(struct xadc *xadc, unsigned int reg,
	uint16_t val)
{
	uint32_t cmd[1];
	uint32_t tmp;
	int ret;

	spin_lock_irq(&xadc->lock);
	xadc_zynq_update_intmsk(xadc, XADC_ZYNQ_INT_DFIFO_GTH,
			XADC_ZYNQ_INT_DFIFO_GTH);

	reinit_completion(&xadc->completion);

	cmd[0] = XADC_ZYNQ_CMD(XADC_ZYNQ_CMD_WRITE, reg, val);
	xadc_zynq_write_fifo(xadc, cmd, ARRAY_SIZE(cmd));
	xadc_read_reg(xadc, XADC_ZYNQ_REG_CFG, &tmp);
	tmp &= ~XADC_ZYNQ_CFG_DFIFOTH_MASK;
	tmp |= 0 << XADC_ZYNQ_CFG_DFIFOTH_OFFSET;
	xadc_write_reg(xadc, XADC_ZYNQ_REG_CFG, tmp);

	xadc_zynq_update_intmsk(xadc, XADC_ZYNQ_INT_DFIFO_GTH, 0);
	spin_unlock_irq(&xadc->lock);

	ret = wait_for_completion_interruptible_timeout(&xadc->completion, HZ);
	if (ret == 0)
		ret = -EIO;
	else
		ret = 0;

	xadc_read_reg(xadc, XADC_ZYNQ_REG_DFIFO, &tmp);

	return ret;
}

static int xadc_zynq_read_adc_reg(struct xadc *xadc, unsigned int reg,
	uint16_t *val)
{
	uint32_t cmd[2];
	uint32_t resp, tmp;
	int ret;

	cmd[0] = XADC_ZYNQ_CMD(XADC_ZYNQ_CMD_READ, reg, 0);
	cmd[1] = XADC_ZYNQ_CMD(XADC_ZYNQ_CMD_NOP, 0, 0);

	spin_lock_irq(&xadc->lock);
	xadc_zynq_update_intmsk(xadc, XADC_ZYNQ_INT_DFIFO_GTH,
			XADC_ZYNQ_INT_DFIFO_GTH);
	xadc_zynq_drain_fifo(xadc);
	reinit_completion(&xadc->completion);

	xadc_zynq_write_fifo(xadc, cmd, ARRAY_SIZE(cmd));
	xadc_read_reg(xadc, XADC_ZYNQ_REG_CFG, &tmp);
	tmp &= ~XADC_ZYNQ_CFG_DFIFOTH_MASK;
	tmp |= 1 << XADC_ZYNQ_CFG_DFIFOTH_OFFSET;
	xadc_write_reg(xadc, XADC_ZYNQ_REG_CFG, tmp);

	xadc_zynq_update_intmsk(xadc, XADC_ZYNQ_INT_DFIFO_GTH, 0);
	spin_unlock_irq(&xadc->lock);
	ret = wait_for_completion_interruptible_timeout(&xadc->completion, HZ);
	if (ret == 0)
		ret = -EIO;
	if (ret < 0)
		return ret;

	xadc_read_reg(xadc, XADC_ZYNQ_REG_DFIFO, &resp);
	xadc_read_reg(xadc, XADC_ZYNQ_REG_DFIFO, &resp);

	*val = resp & 0xffff;

	return 0;
}

static unsigned int xadc_zynq_transform_alarm(unsigned int alarm)
{
	return ((alarm & 0x80) >> 4) |
		((alarm & 0x78) << 1) |
		(alarm & 0x07);
}

/*
 * The ZYNQ threshold interrupts are level sensitive. Since we can't make the
 * threshold condition go way from within the interrupt handler, this means as
 * soon as a threshold condition is present we would enter the interrupt handler
 * again and again. To work around this we mask all active thresholds interrupts
 * in the interrupt handler and start a timer. In this timer we poll the
 * interrupt status and only if the interrupt is inactive we unmask it again.
 */
static void xadc_zynq_unmask_worker(struct work_struct *work)
{
	struct xadc *xadc = container_of(work, struct xadc, zynq_unmask_work.work);
	unsigned int misc_sts, unmask;

	xadc_read_reg(xadc, XADC_ZYNQ_REG_STATUS, &misc_sts);

	misc_sts &= XADC_ZYNQ_INT_ALARM_MASK;

	spin_lock_irq(&xadc->lock);

	/* Clear those bits which are not active anymore */
	unmask = (xadc->zynq_masked_alarm ^ misc_sts) & xadc->zynq_masked_alarm;
	xadc->zynq_masked_alarm &= misc_sts;

	/* Also clear those which are masked out anyway */
	xadc->zynq_masked_alarm &= ~xadc->zynq_intmask;

	/* Clear the interrupts before we unmask them */
	xadc_write_reg(xadc, XADC_ZYNQ_REG_INTSTS, unmask);

	xadc_zynq_update_intmsk(xadc, 0, 0);

	spin_unlock_irq(&xadc->lock);

	/* if still pending some alarm re-trigger the timer */
	if (xadc->zynq_masked_alarm) {
		schedule_delayed_work(&xadc->zynq_unmask_work,
				msecs_to_jiffies(XADC_ZYNQ_UNMASK_TIMEOUT));
	}

}

static irqreturn_t xadc_zynq_interrupt_handler(int irq, void *devid)
{
	struct iio_dev *indio_dev = devid;
	struct xadc *xadc = iio_priv(indio_dev);
	uint32_t status;

	xadc_read_reg(xadc, XADC_ZYNQ_REG_INTSTS, &status);

	status &= ~(xadc->zynq_intmask | xadc->zynq_masked_alarm);

	if (!status)
		return IRQ_NONE;

	spin_lock(&xadc->lock);

	xadc_write_reg(xadc, XADC_ZYNQ_REG_INTSTS, status);

	if (status & XADC_ZYNQ_INT_DFIFO_GTH) {
		xadc_zynq_update_intmsk(xadc, XADC_ZYNQ_INT_DFIFO_GTH,
			XADC_ZYNQ_INT_DFIFO_GTH);
		complete(&xadc->completion);
	}

	status &= XADC_ZYNQ_INT_ALARM_MASK;
	if (status) {
		xadc->zynq_masked_alarm |= status;
		/*
		 * mask the current event interrupt,
		 * unmask it when the interrupt is no more active.
		 */
		xadc_zynq_update_intmsk(xadc, 0, 0);

		xadc_handle_events(indio_dev,
				xadc_zynq_transform_alarm(status));

		/* unmask the required interrupts in timer. */
		schedule_delayed_work(&xadc->zynq_unmask_work,
				msecs_to_jiffies(XADC_ZYNQ_UNMASK_TIMEOUT));
	}
	spin_unlock(&xadc->lock);

	return IRQ_HANDLED;
}

#define XADC_ZYNQ_TCK_RATE_MAX 50000000
#define XADC_ZYNQ_IGAP_DEFAULT 20
#define XADC_ZYNQ_PCAP_RATE_MAX 200000000

static int xadc_zynq_setup(struct platform_device *pdev,
	struct iio_dev *indio_dev, int irq)
{
	struct xadc *xadc = iio_priv(indio_dev);
	unsigned long pcap_rate;
	unsigned int tck_div;
	unsigned int div;
	unsigned int igap;
	unsigned int tck_rate;
	int ret;

	/* TODO: Figure out how to make igap and tck_rate configurable */
	igap = XADC_ZYNQ_IGAP_DEFAULT;
	tck_rate = XADC_ZYNQ_TCK_RATE_MAX;

	xadc->zynq_intmask = ~0;

	pcap_rate = clk_get_rate(xadc->clk);
	if (!pcap_rate)
		return -EINVAL;

	if (pcap_rate > XADC_ZYNQ_PCAP_RATE_MAX) {
		ret = clk_set_rate(xadc->clk,
				   (unsigned long)XADC_ZYNQ_PCAP_RATE_MAX);
		if (ret)
			return ret;
	}

	if (tck_rate > pcap_rate / 2) {
		div = 2;
	} else {
		div = pcap_rate / tck_rate;
		if (pcap_rate / div > XADC_ZYNQ_TCK_RATE_MAX)
			div++;
	}

	if (div <= 3)
		tck_div = XADC_ZYNQ_CFG_TCKRATE_DIV2;
	else if (div <= 7)
		tck_div = XADC_ZYNQ_CFG_TCKRATE_DIV4;
	else if (div <= 15)
		tck_div = XADC_ZYNQ_CFG_TCKRATE_DIV8;
	else
		tck_div = XADC_ZYNQ_CFG_TCKRATE_DIV16;

	xadc_write_reg(xadc, XADC_ZYNQ_REG_CTL, XADC_ZYNQ_CTL_RESET);
	xadc_write_reg(xadc, XADC_ZYNQ_REG_CTL, 0);
	xadc_write_reg(xadc, XADC_ZYNQ_REG_INTSTS, ~0);
	xadc_write_reg(xadc, XADC_ZYNQ_REG_INTMSK, xadc->zynq_intmask);
	xadc_write_reg(xadc, XADC_ZYNQ_REG_CFG, XADC_ZYNQ_CFG_ENABLE |
			XADC_ZYNQ_CFG_REDGE | XADC_ZYNQ_CFG_WEDGE |
			tck_div | XADC_ZYNQ_CFG_IGAP(igap));

	if (pcap_rate > XADC_ZYNQ_PCAP_RATE_MAX) {
		ret = clk_set_rate(xadc->clk, pcap_rate);
		if (ret)
			return ret;
	}

	return 0;
}

static unsigned long xadc_zynq_get_dclk_rate(struct xadc *xadc)
{
	unsigned int div;
	uint32_t val;

	xadc_read_reg(xadc, XADC_ZYNQ_REG_CFG, &val);

	switch (val & XADC_ZYNQ_CFG_TCKRATE_MASK) {
	case XADC_ZYNQ_CFG_TCKRATE_DIV4:
		div = 4;
		break;
	case XADC_ZYNQ_CFG_TCKRATE_DIV8:
		div = 8;
		break;
	case XADC_ZYNQ_CFG_TCKRATE_DIV16:
		div = 16;
		break;
	default:
		div = 2;
		break;
	}

	return clk_get_rate(xadc->clk) / div;
}

static void xadc_zynq_update_alarm(struct xadc *xadc, unsigned int alarm)
{
	unsigned long flags;
	uint32_t status;

	/* Move OT to bit 7 */
	alarm = ((alarm & 0x08) << 4) | ((alarm & 0xf0) >> 1) | (alarm & 0x07);

	spin_lock_irqsave(&xadc->lock, flags);

	/* Clear previous interrupts if any. */
	xadc_read_reg(xadc, XADC_ZYNQ_REG_INTSTS, &status);
	xadc_write_reg(xadc, XADC_ZYNQ_REG_INTSTS, status & alarm);

	xadc_zynq_update_intmsk(xadc, XADC_ZYNQ_INT_ALARM_MASK,
		~alarm & XADC_ZYNQ_INT_ALARM_MASK);

	spin_unlock_irqrestore(&xadc->lock, flags);
}

static const struct xadc_ops xadc_zynq_ops = {
	.read = xadc_zynq_read_adc_reg,
	.write = xadc_zynq_write_adc_reg,
	.setup = xadc_zynq_setup,
	.get_dclk_rate = xadc_zynq_get_dclk_rate,
	.interrupt_handler = xadc_zynq_interrupt_handler,
	.update_alarm = xadc_zynq_update_alarm,
	.type = XADC_TYPE_S7,
};

static const unsigned int xadc_axi_reg_offsets[] = {
	[XADC_TYPE_S7] = XADC_7S_AXI_ADC_REG_OFFSET,
	[XADC_TYPE_US] = XADC_US_AXI_ADC_REG_OFFSET,
};

static int xadc_axi_read_adc_reg(struct xadc *xadc, unsigned int reg,
	uint16_t *val)
{
	uint32_t val32;

	xadc_read_reg(xadc, xadc_axi_reg_offsets[xadc->ops->type] + reg * 4,
		&val32);
	*val = val32 & 0xffff;

	return 0;
}

static int xadc_axi_write_adc_reg(struct xadc *xadc, unsigned int reg,
	uint16_t val)
{
	xadc_write_reg(xadc, xadc_axi_reg_offsets[xadc->ops->type] + reg * 4,
		val);

	return 0;
}

static int xadc_axi_setup(struct platform_device *pdev,
	struct iio_dev *indio_dev, int irq)
{
	struct xadc *xadc = iio_priv(indio_dev);

	xadc_write_reg(xadc, XADC_AXI_REG_RESET, XADC_AXI_RESET_MAGIC);
	xadc_write_reg(xadc, XADC_AXI_REG_GIER, XADC_AXI_GIER_ENABLE);

	return 0;
}

static irqreturn_t xadc_axi_interrupt_handler(int irq, void *devid)
{
	struct iio_dev *indio_dev = devid;
	struct xadc *xadc = iio_priv(indio_dev);
	uint32_t status, mask;
	unsigned int events;

	xadc_read_reg(xadc, XADC_AXI_REG_IPISR, &status);
	xadc_read_reg(xadc, XADC_AXI_REG_IPIER, &mask);
	status &= mask;

	if (!status)
		return IRQ_NONE;

	if ((status & XADC_AXI_INT_EOS) && xadc->trigger)
		iio_trigger_poll(xadc->trigger);

	if (status & XADC_AXI_INT_ALARM_MASK) {
		/*
		 * The order of the bits in the AXI-XADC status register does
		 * not match the order of the bits in the XADC alarm enable
		 * register. xadc_handle_events() expects the events to be in
		 * the same order as the XADC alarm enable register.
		 */
		events = (status & 0x000e) >> 1;
		events |= (status & 0x0001) << 3;
		events |= (status & 0x3c00) >> 6;
		xadc_handle_events(indio_dev, events);
	}

	xadc_write_reg(xadc, XADC_AXI_REG_IPISR, status);

	return IRQ_HANDLED;
}

static void xadc_axi_update_alarm(struct xadc *xadc, unsigned int alarm)
{
	uint32_t val;
	unsigned long flags;

	/*
	 * The order of the bits in the AXI-XADC status register does not match
	 * the order of the bits in the XADC alarm enable register. We get
	 * passed the alarm mask in the same order as in the XADC alarm enable
	 * register.
	 */
	alarm = ((alarm & 0x07) << 1) | ((alarm & 0x08) >> 3) |
			((alarm & 0xf0) << 6);

	spin_lock_irqsave(&xadc->lock, flags);
	xadc_read_reg(xadc, XADC_AXI_REG_IPIER, &val);
	val &= ~XADC_AXI_INT_ALARM_MASK;
	val |= alarm;
	xadc_write_reg(xadc, XADC_AXI_REG_IPIER, val);
	spin_unlock_irqrestore(&xadc->lock, flags);
}

static unsigned long xadc_axi_get_dclk(struct xadc *xadc)
{
	return clk_get_rate(xadc->clk);
}

static const struct xadc_ops xadc_7s_axi_ops = {
	.read = xadc_axi_read_adc_reg,
	.write = xadc_axi_write_adc_reg,
	.setup = xadc_axi_setup,
	.get_dclk_rate = xadc_axi_get_dclk,
	.update_alarm = xadc_axi_update_alarm,
	.interrupt_handler = xadc_axi_interrupt_handler,
	.flags = XADC_FLAGS_BUFFERED,
	.type = XADC_TYPE_S7,
};

static const struct xadc_ops xadc_us_axi_ops = {
	.read = xadc_axi_read_adc_reg,
	.write = xadc_axi_write_adc_reg,
	.setup = xadc_axi_setup,
	.get_dclk_rate = xadc_axi_get_dclk,
	.update_alarm = xadc_axi_update_alarm,
	.interrupt_handler = xadc_axi_interrupt_handler,
	.flags = XADC_FLAGS_BUFFERED,
	.type = XADC_TYPE_US,
};

static int _xadc_update_adc_reg(struct xadc *xadc, unsigned int reg,
	uint16_t mask, uint16_t val)
{
	uint16_t tmp;
	int ret;

	ret = _xadc_read_adc_reg(xadc, reg, &tmp);
	if (ret)
		return ret;

	return _xadc_write_adc_reg(xadc, reg, (tmp & ~mask) | val);
}

static int xadc_update_adc_reg(struct xadc *xadc, unsigned int reg,
	uint16_t mask, uint16_t val)
{
	int ret;

	mutex_lock(&xadc->mutex);
	ret = _xadc_update_adc_reg(xadc, reg, mask, val);
	mutex_unlock(&xadc->mutex);

	return ret;
}

static unsigned long xadc_get_dclk_rate(struct xadc *xadc)
{
	return xadc->ops->get_dclk_rate(xadc);
}

static int xadc_update_scan_mode(struct iio_dev *indio_dev,
	const unsigned long *mask)
{
	struct xadc *xadc = iio_priv(indio_dev);
	size_t new_size, n;
	void *data;

	n = bitmap_weight(mask, indio_dev->masklength);

	if (check_mul_overflow(n, sizeof(*xadc->data), &new_size))
		return -ENOMEM;

	data = devm_krealloc(indio_dev->dev.parent, xadc->data,
			     new_size, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	memset(data, 0, new_size);
	xadc->data = data;

	return 0;
}

static unsigned int xadc_scan_index_to_channel(unsigned int scan_index)
{
	switch (scan_index) {
	case 5:
		return XADC_REG_VCCPINT;
	case 6:
		return XADC_REG_VCCPAUX;
	case 7:
		return XADC_REG_VCCO_DDR;
	case 8:
		return XADC_REG_TEMP;
	case 9:
		return XADC_REG_VCCINT;
	case 10:
		return XADC_REG_VCCAUX;
	case 11:
		return XADC_REG_VPVN;
	case 12:
		return XADC_REG_VREFP;
	case 13:
		return XADC_REG_VREFN;
	case 14:
		return XADC_REG_VCCBRAM;
	default:
		return XADC_REG_VAUX(scan_index - 16);
	}
}

static irqreturn_t xadc_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct xadc *xadc = iio_priv(indio_dev);
	unsigned int chan;
	int i, j;

	if (!xadc->data)
		goto out;

	j = 0;
	for_each_set_bit(i, indio_dev->active_scan_mask,
		indio_dev->masklength) {
		chan = xadc_scan_index_to_channel(i);
		xadc_read_adc_reg(xadc, chan, &xadc->data[j]);
		j++;
	}

	iio_push_to_buffers(indio_dev, xadc->data);

out:
	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

static int xadc_trigger_set_state(struct iio_trigger *trigger, bool state)
{
	struct xadc *xadc = iio_trigger_get_drvdata(trigger);
	unsigned long flags;
	unsigned int convst;
	unsigned int val;
	int ret = 0;

	mutex_lock(&xadc->mutex);

	if (state) {
		/* Only one of the two triggers can be active at a time. */
		if (xadc->trigger != NULL) {
			ret = -EBUSY;
			goto err_out;
		} else {
			xadc->trigger = trigger;
			if (trigger == xadc->convst_trigger)
				convst = XADC_CONF0_EC;
			else
				convst = 0;
		}
		ret = _xadc_update_adc_reg(xadc, XADC_REG_CONF1, XADC_CONF0_EC,
					convst);
		if (ret)
			goto err_out;
	} else {
		xadc->trigger = NULL;
	}

	spin_lock_irqsave(&xadc->lock, flags);
	xadc_read_reg(xadc, XADC_AXI_REG_IPIER, &val);
	xadc_write_reg(xadc, XADC_AXI_REG_IPISR, XADC_AXI_INT_EOS);
	if (state)
		val |= XADC_AXI_INT_EOS;
	else
		val &= ~XADC_AXI_INT_EOS;
	xadc_write_reg(xadc, XADC_AXI_REG_IPIER, val);
	spin_unlock_irqrestore(&xadc->lock, flags);

err_out:
	mutex_unlock(&xadc->mutex);

	return ret;
}

static const struct iio_trigger_ops xadc_trigger_ops = {
	.set_trigger_state = &xadc_trigger_set_state,
};

static struct iio_trigger *xadc_alloc_trigger(struct iio_dev *indio_dev,
	const char *name)
{
	struct device *dev = indio_dev->dev.parent;
	struct iio_trigger *trig;
	int ret;

	trig = devm_iio_trigger_alloc(dev, "%s%d-%s", indio_dev->name,
				      indio_dev->id, name);
	if (trig == NULL)
		return ERR_PTR(-ENOMEM);

	trig->ops = &xadc_trigger_ops;
	iio_trigger_set_drvdata(trig, iio_priv(indio_dev));

	ret = devm_iio_trigger_register(dev, trig);
	if (ret)
		return ERR_PTR(ret);

	return trig;
}

static int xadc_power_adc_b(struct xadc *xadc, unsigned int seq_mode)
{
	uint16_t val;

	/*
	 * As per datasheet the power-down bits are don't care in the
	 * UltraScale, but as per reality setting the power-down bit for the
	 * non-existing ADC-B powers down the main ADC, so just return and don't
	 * do anything.
	 */
	if (xadc->ops->type == XADC_TYPE_US)
		return 0;

	/* Powerdown the ADC-B when it is not needed. */
	switch (seq_mode) {
	case XADC_CONF1_SEQ_SIMULTANEOUS:
	case XADC_CONF1_SEQ_INDEPENDENT:
		val = 0;
		break;
	default:
		val = XADC_CONF2_PD_ADC_B;
		break;
	}

	return xadc_update_adc_reg(xadc, XADC_REG_CONF2, XADC_CONF2_PD_MASK,
		val);
}

static int xadc_get_seq_mode(struct xadc *xadc, unsigned long scan_mode)
{
	unsigned int aux_scan_mode = scan_mode >> 16;

	/* UltraScale has only one ADC and supports only continuous mode */
	if (xadc->ops->type == XADC_TYPE_US)
		return XADC_CONF1_SEQ_CONTINUOUS;

	if (xadc->external_mux_mode == XADC_EXTERNAL_MUX_DUAL)
		return XADC_CONF1_SEQ_SIMULTANEOUS;

	if ((aux_scan_mode & 0xff00) == 0 ||
		(aux_scan_mode & 0x00ff) == 0)
		return XADC_CONF1_SEQ_CONTINUOUS;

	return XADC_CONF1_SEQ_SIMULTANEOUS;
}

static int xadc_postdisable(struct iio_dev *indio_dev)
{
	struct xadc *xadc = iio_priv(indio_dev);
	unsigned long scan_mask;
	int ret;
	int i;

	scan_mask = 1; /* Run calibration as part of the sequence */
	for (i = 0; i < indio_dev->num_channels; i++)
		scan_mask |= BIT(indio_dev->channels[i].scan_index);

	/* Enable all channels and calibration */
	ret = xadc_write_adc_reg(xadc, XADC_REG_SEQ(0), scan_mask & 0xffff);
	if (ret)
		return ret;

	ret = xadc_write_adc_reg(xadc, XADC_REG_SEQ(1), scan_mask >> 16);
	if (ret)
		return ret;

	ret = xadc_update_adc_reg(xadc, XADC_REG_CONF1, XADC_CONF1_SEQ_MASK,
		XADC_CONF1_SEQ_CONTINUOUS);
	if (ret)
		return ret;

	return xadc_power_adc_b(xadc, XADC_CONF1_SEQ_CONTINUOUS);
}

static int xadc_preenable(struct iio_dev *indio_dev)
{
	struct xadc *xadc = iio_priv(indio_dev);
	unsigned long scan_mask;
	int seq_mode;
	int ret;

	ret = xadc_update_adc_reg(xadc, XADC_REG_CONF1, XADC_CONF1_SEQ_MASK,
		XADC_CONF1_SEQ_DEFAULT);
	if (ret)
		goto err;

	scan_mask = *indio_dev->active_scan_mask;
	seq_mode = xadc_get_seq_mode(xadc, scan_mask);

	ret = xadc_write_adc_reg(xadc, XADC_REG_SEQ(0), scan_mask & 0xffff);
	if (ret)
		goto err;

	/*
	 * In simultaneous mode the upper and lower aux channels are samples at
	 * the same time. In this mode the upper 8 bits in the sequencer
	 * register are don't care and the lower 8 bits control two channels
	 * each. As such we must set the bit if either the channel in the lower
	 * group or the upper group is enabled.
	 */
	if (seq_mode == XADC_CONF1_SEQ_SIMULTANEOUS)
		scan_mask = ((scan_mask >> 8) | scan_mask) & 0xff0000;

	ret = xadc_write_adc_reg(xadc, XADC_REG_SEQ(1), scan_mask >> 16);
	if (ret)
		goto err;

	ret = xadc_power_adc_b(xadc, seq_mode);
	if (ret)
		goto err;

	ret = xadc_update_adc_reg(xadc, XADC_REG_CONF1, XADC_CONF1_SEQ_MASK,
		seq_mode);
	if (ret)
		goto err;

	return 0;
err:
	xadc_postdisable(indio_dev);
	return ret;
}

static const struct iio_buffer_setup_ops xadc_buffer_ops = {
	.preenable = &xadc_preenable,
	.postdisable = &xadc_postdisable,
};

static int xadc_read_samplerate(struct xadc *xadc)
{
	unsigned int div;
	uint16_t val16;
	int ret;

	ret = xadc_read_adc_reg(xadc, XADC_REG_CONF2, &val16);
	if (ret)
		return ret;

	div = (val16 & XADC_CONF2_DIV_MASK) >> XADC_CONF2_DIV_OFFSET;
	if (div < 2)
		div = 2;

	return xadc_get_dclk_rate(xadc) / div / 26;
}

static int xadc_read_raw(struct iio_dev *indio_dev,
	struct iio_chan_spec const *chan, int *val, int *val2, long info)
{
	struct xadc *xadc = iio_priv(indio_dev);
	unsigned int bits = chan->scan_type.realbits;
	uint16_t val16;
	int ret;

	switch (info) {
	case IIO_CHAN_INFO_RAW:
		if (iio_buffer_enabled(indio_dev))
			return -EBUSY;
		ret = xadc_read_adc_reg(xadc, chan->address, &val16);
		if (ret < 0)
			return ret;

		val16 >>= chan->scan_type.shift;
		if (chan->scan_type.sign == 'u')
			*val = val16;
		else
			*val = sign_extend32(val16, bits - 1);

		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_VOLTAGE:
			/* V = (val * 3.0) / 2**bits */
			switch (chan->address) {
			case XADC_REG_VCCINT:
			case XADC_REG_VCCAUX:
			case XADC_REG_VREFP:
			case XADC_REG_VREFN:
			case XADC_REG_VCCBRAM:
			case XADC_REG_VCCPINT:
			case XADC_REG_VCCPAUX:
			case XADC_REG_VCCO_DDR:
				*val = 3000;
				break;
			default:
				*val = 1000;
				break;
			}
			*val2 = chan->scan_type.realbits;
			return IIO_VAL_FRACTIONAL_LOG2;
		case IIO_TEMP:
			/* Temp in C = (val * 503.975) / 2**bits - 273.15 */
			*val = 503975;
			*val2 = bits;
			return IIO_VAL_FRACTIONAL_LOG2;
		default:
			return -EINVAL;
		}
	case IIO_CHAN_INFO_OFFSET:
		/* Only the temperature channel has an offset */
		*val = -((273150 << bits) / 503975);
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SAMP_FREQ:
		ret = xadc_read_samplerate(xadc);
		if (ret < 0)
			return ret;

		*val = ret;
		return IIO_VAL_INT;
	default:
		return -EINVAL;
	}
}

static int xadc_write_samplerate(struct xadc *xadc, int val)
{
	unsigned long clk_rate = xadc_get_dclk_rate(xadc);
	unsigned int div;

	if (!clk_rate)
		return -EINVAL;

	if (val <= 0)
		return -EINVAL;

	/* Max. 150 kSPS */
	if (val > XADC_MAX_SAMPLERATE)
		val = XADC_MAX_SAMPLERATE;

	val *= 26;

	/* Min 1MHz */
	if (val < 1000000)
		val = 1000000;

	/*
	 * We want to round down, but only if we do not exceed the 150 kSPS
	 * limit.
	 */
	div = clk_rate / val;
	if (clk_rate / div / 26 > XADC_MAX_SAMPLERATE)
		div++;
	if (div < 2)
		div = 2;
	else if (div > 0xff)
		div = 0xff;

	return xadc_update_adc_reg(xadc, XADC_REG_CONF2, XADC_CONF2_DIV_MASK,
		div << XADC_CONF2_DIV_OFFSET);
}

static int xadc_write_raw(struct iio_dev *indio_dev,
	struct iio_chan_spec const *chan, int val, int val2, long info)
{
	struct xadc *xadc = iio_priv(indio_dev);

	if (info != IIO_CHAN_INFO_SAMP_FREQ)
		return -EINVAL;

	return xadc_write_samplerate(xadc, val);
}

static const struct iio_event_spec xadc_temp_events[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_ENABLE) |
				BIT(IIO_EV_INFO_VALUE) |
				BIT(IIO_EV_INFO_HYSTERESIS),
	},
};

/* Separate values for upper and lower thresholds, but only a shared enabled */
static const struct iio_event_spec xadc_voltage_events[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_FALLING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_EITHER,
		.mask_separate = BIT(IIO_EV_INFO_ENABLE),
	},
};

#define XADC_CHAN_TEMP(_chan, _scan_index, _addr, _bits) { \
	.type = IIO_TEMP, \
	.indexed = 1, \
	.channel = (_chan), \
	.address = (_addr), \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | \
		BIT(IIO_CHAN_INFO_SCALE) | \
		BIT(IIO_CHAN_INFO_OFFSET), \
	.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_SAMP_FREQ), \
	.event_spec = xadc_temp_events, \
	.num_event_specs = ARRAY_SIZE(xadc_temp_events), \
	.scan_index = (_scan_index), \
	.scan_type = { \
		.sign = 'u', \
		.realbits = (_bits), \
		.storagebits = 16, \
		.shift = 16 - (_bits), \
		.endianness = IIO_CPU, \
	}, \
}

#define XADC_CHAN_VOLTAGE(_chan, _scan_index, _addr, _bits, _ext, _alarm) { \
	.type = IIO_VOLTAGE, \
	.indexed = 1, \
	.channel = (_chan), \
	.address = (_addr), \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | \
		BIT(IIO_CHAN_INFO_SCALE), \
	.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_SAMP_FREQ), \
	.event_spec = (_alarm) ? xadc_voltage_events : NULL, \
	.num_event_specs = (_alarm) ? ARRAY_SIZE(xadc_voltage_events) : 0, \
	.scan_index = (_scan_index), \
	.scan_type = { \
		.sign = ((_addr) == XADC_REG_VREFN) ? 's' : 'u', \
		.realbits = (_bits), \
		.storagebits = 16, \
		.shift = 16 - (_bits), \
		.endianness = IIO_CPU, \
	}, \
	.extend_name = _ext, \
}

/* 7 Series */
#define XADC_7S_CHAN_TEMP(_chan, _scan_index, _addr) \
	XADC_CHAN_TEMP(_chan, _scan_index, _addr, 12)
#define XADC_7S_CHAN_VOLTAGE(_chan, _scan_index, _addr, _ext, _alarm) \
	XADC_CHAN_VOLTAGE(_chan, _scan_index, _addr, 12, _ext, _alarm)

static const struct iio_chan_spec xadc_7s_channels[] = {
	XADC_7S_CHAN_TEMP(0, 8, XADC_REG_TEMP),
	XADC_7S_CHAN_VOLTAGE(0, 9, XADC_REG_VCCINT, "vccint", true),
	XADC_7S_CHAN_VOLTAGE(1, 10, XADC_REG_VCCAUX, "vccaux", true),
	XADC_7S_CHAN_VOLTAGE(2, 14, XADC_REG_VCCBRAM, "vccbram", true),
	XADC_7S_CHAN_VOLTAGE(3, 5, XADC_REG_VCCPINT, "vccpint", true),
	XADC_7S_CHAN_VOLTAGE(4, 6, XADC_REG_VCCPAUX, "vccpaux", true),
	XADC_7S_CHAN_VOLTAGE(5, 7, XADC_REG_VCCO_DDR, "vccoddr", true),
	XADC_7S_CHAN_VOLTAGE(6, 12, XADC_REG_VREFP, "vrefp", false),
	XADC_7S_CHAN_VOLTAGE(7, 13, XADC_REG_VREFN, "vrefn", false),
	XADC_7S_CHAN_VOLTAGE(8, 11, XADC_REG_VPVN, NULL, false),
	XADC_7S_CHAN_VOLTAGE(9, 16, XADC_REG_VAUX(0), NULL, false),
	XADC_7S_CHAN_VOLTAGE(10, 17, XADC_REG_VAUX(1), NULL, false),
	XADC_7S_CHAN_VOLTAGE(11, 18, XADC_REG_VAUX(2), NULL, false),
	XADC_7S_CHAN_VOLTAGE(12, 19, XADC_REG_VAUX(3), NULL, false),
	XADC_7S_CHAN_VOLTAGE(13, 20, XADC_REG_VAUX(4), NULL, false),
	XADC_7S_CHAN_VOLTAGE(14, 21, XADC_REG_VAUX(5), NULL, false),
	XADC_7S_CHAN_VOLTAGE(15, 22, XADC_REG_VAUX(6), NULL, false),
	XADC_7S_CHAN_VOLTAGE(16, 23, XADC_REG_VAUX(7), NULL, false),
	XADC_7S_CHAN_VOLTAGE(17, 24, XADC_REG_VAUX(8), NULL, false),
	XADC_7S_CHAN_VOLTAGE(18, 25, XADC_REG_VAUX(9), NULL, false),
	XADC_7S_CHAN_VOLTAGE(19, 26, XADC_REG_VAUX(10), NULL, false),
	XADC_7S_CHAN_VOLTAGE(20, 27, XADC_REG_VAUX(11), NULL, false),
	XADC_7S_CHAN_VOLTAGE(21, 28, XADC_REG_VAUX(12), NULL, false),
	XADC_7S_CHAN_VOLTAGE(22, 29, XADC_REG_VAUX(13), NULL, false),
	XADC_7S_CHAN_VOLTAGE(23, 30, XADC_REG_VAUX(14), NULL, false),
	XADC_7S_CHAN_VOLTAGE(24, 31, XADC_REG_VAUX(15), NULL, false),
};

/* UltraScale */
#define XADC_US_CHAN_TEMP(_chan, _scan_index, _addr) \
	XADC_CHAN_TEMP(_chan, _scan_index, _addr, 10)
#define XADC_US_CHAN_VOLTAGE(_chan, _scan_index, _addr, _ext, _alarm) \
	XADC_CHAN_VOLTAGE(_chan, _scan_index, _addr, 10, _ext, _alarm)

static const struct iio_chan_spec xadc_us_channels[] = {
	XADC_US_CHAN_TEMP(0, 8, XADC_REG_TEMP),
	XADC_US_CHAN_VOLTAGE(0, 9, XADC_REG_VCCINT, "vccint", true),
	XADC_US_CHAN_VOLTAGE(1, 10, XADC_REG_VCCAUX, "vccaux", true),
	XADC_US_CHAN_VOLTAGE(2, 14, XADC_REG_VCCBRAM, "vccbram", true),
	XADC_US_CHAN_VOLTAGE(3, 5, XADC_REG_VCCPINT, "vccpsintlp", true),
	XADC_US_CHAN_VOLTAGE(4, 6, XADC_REG_VCCPAUX, "vccpsintfp", true),
	XADC_US_CHAN_VOLTAGE(5, 7, XADC_REG_VCCO_DDR, "vccpsaux", true),
	XADC_US_CHAN_VOLTAGE(6, 12, XADC_REG_VREFP, "vrefp", false),
	XADC_US_CHAN_VOLTAGE(7, 13, XADC_REG_VREFN, "vrefn", false),
	XADC_US_CHAN_VOLTAGE(8, 11, XADC_REG_VPVN, NULL, false),
	XADC_US_CHAN_VOLTAGE(9, 16, XADC_REG_VAUX(0), NULL, false),
	XADC_US_CHAN_VOLTAGE(10, 17, XADC_REG_VAUX(1), NULL, false),
	XADC_US_CHAN_VOLTAGE(11, 18, XADC_REG_VAUX(2), NULL, false),
	XADC_US_CHAN_VOLTAGE(12, 19, XADC_REG_VAUX(3), NULL, false),
	XADC_US_CHAN_VOLTAGE(13, 20, XADC_REG_VAUX(4), NULL, false),
	XADC_US_CHAN_VOLTAGE(14, 21, XADC_REG_VAUX(5), NULL, false),
	XADC_US_CHAN_VOLTAGE(15, 22, XADC_REG_VAUX(6), NULL, false),
	XADC_US_CHAN_VOLTAGE(16, 23, XADC_REG_VAUX(7), NULL, false),
	XADC_US_CHAN_VOLTAGE(17, 24, XADC_REG_VAUX(8), NULL, false),
	XADC_US_CHAN_VOLTAGE(18, 25, XADC_REG_VAUX(9), NULL, false),
	XADC_US_CHAN_VOLTAGE(19, 26, XADC_REG_VAUX(10), NULL, false),
	XADC_US_CHAN_VOLTAGE(20, 27, XADC_REG_VAUX(11), NULL, false),
	XADC_US_CHAN_VOLTAGE(21, 28, XADC_REG_VAUX(12), NULL, false),
	XADC_US_CHAN_VOLTAGE(22, 29, XADC_REG_VAUX(13), NULL, false),
	XADC_US_CHAN_VOLTAGE(23, 30, XADC_REG_VAUX(14), NULL, false),
	XADC_US_CHAN_VOLTAGE(24, 31, XADC_REG_VAUX(15), NULL, false),
};

static const struct iio_info xadc_info = {
	.read_raw = &xadc_read_raw,
	.write_raw = &xadc_write_raw,
	.read_event_config = &xadc_read_event_config,
	.write_event_config = &xadc_write_event_config,
	.read_event_value = &xadc_read_event_value,
	.write_event_value = &xadc_write_event_value,
	.update_scan_mode = &xadc_update_scan_mode,
};

static const struct of_device_id xadc_of_match_table[] = {
	{
		.compatible = "xlnx,zynq-xadc-1.00.a",
		.data = &xadc_zynq_ops
	}, {
		.compatible = "xlnx,axi-xadc-1.00.a",
		.data = &xadc_7s_axi_ops
	}, {
		.compatible = "xlnx,system-management-wiz-1.3",
		.data = &xadc_us_axi_ops
	},
	{ },
};
MODULE_DEVICE_TABLE(of, xadc_of_match_table);

static int xadc_parse_dt(struct iio_dev *indio_dev, struct device_node *np,
	unsigned int *conf)
{
	struct device *dev = indio_dev->dev.parent;
	struct xadc *xadc = iio_priv(indio_dev);
	const struct iio_chan_spec *channel_templates;
	struct iio_chan_spec *channels, *chan;
	struct device_node *chan_node, *child;
	unsigned int max_channels;
	unsigned int num_channels;
	const char *external_mux;
	u32 ext_mux_chan;
	u32 reg;
	int ret;

	*conf = 0;

	ret = of_property_read_string(np, "xlnx,external-mux", &external_mux);
	if (ret < 0 || strcasecmp(external_mux, "none") == 0)
		xadc->external_mux_mode = XADC_EXTERNAL_MUX_NONE;
	else if (strcasecmp(external_mux, "single") == 0)
		xadc->external_mux_mode = XADC_EXTERNAL_MUX_SINGLE;
	else if (strcasecmp(external_mux, "dual") == 0)
		xadc->external_mux_mode = XADC_EXTERNAL_MUX_DUAL;
	else
		return -EINVAL;

	if (xadc->external_mux_mode != XADC_EXTERNAL_MUX_NONE) {
		ret = of_property_read_u32(np, "xlnx,external-mux-channel",
					&ext_mux_chan);
		if (ret < 0)
			return ret;

		if (xadc->external_mux_mode == XADC_EXTERNAL_MUX_SINGLE) {
			if (ext_mux_chan == 0)
				ext_mux_chan = XADC_REG_VPVN;
			else if (ext_mux_chan <= 16)
				ext_mux_chan = XADC_REG_VAUX(ext_mux_chan - 1);
			else
				return -EINVAL;
		} else {
			if (ext_mux_chan > 0 && ext_mux_chan <= 8)
				ext_mux_chan = XADC_REG_VAUX(ext_mux_chan - 1);
			else
				return -EINVAL;
		}

		*conf |= XADC_CONF0_MUX | XADC_CONF0_CHAN(ext_mux_chan);
	}
	if (xadc->ops->type == XADC_TYPE_S7) {
		channel_templates = xadc_7s_channels;
		max_channels = ARRAY_SIZE(xadc_7s_channels);
	} else {
		channel_templates = xadc_us_channels;
		max_channels = ARRAY_SIZE(xadc_us_channels);
	}
	channels = devm_kmemdup(dev, channel_templates,
				sizeof(channels[0]) * max_channels, GFP_KERNEL);
	if (!channels)
		return -ENOMEM;

	num_channels = 9;
	chan = &channels[9];

	chan_node = of_get_child_by_name(np, "xlnx,channels");
	if (chan_node) {
		for_each_child_of_node(chan_node, child) {
			if (num_channels >= max_channels) {
				of_node_put(child);
				break;
			}

			ret = of_property_read_u32(child, "reg", &reg);
			if (ret || reg > 16)
				continue;

			if (of_property_read_bool(child, "xlnx,bipolar"))
				chan->scan_type.sign = 's';

			if (reg == 0) {
				chan->scan_index = 11;
				chan->address = XADC_REG_VPVN;
			} else {
				chan->scan_index = 15 + reg;
				chan->address = XADC_REG_VAUX(reg - 1);
			}
			num_channels++;
			chan++;
		}
	}
	of_node_put(chan_node);

	indio_dev->num_channels = num_channels;
	indio_dev->channels = devm_krealloc(dev, channels,
					    sizeof(*channels) * num_channels,
					    GFP_KERNEL);
	/* If we can't resize the channels array, just use the original */
	if (!indio_dev->channels)
		indio_dev->channels = channels;

	return 0;
}

static const char * const xadc_type_names[] = {
	[XADC_TYPE_S7] = "xadc",
	[XADC_TYPE_US] = "xilinx-system-monitor",
};

static void xadc_clk_disable_unprepare(void *data)
{
	struct clk *clk = data;

	clk_disable_unprepare(clk);
}

static void xadc_cancel_delayed_work(void *data)
{
	struct delayed_work *work = data;

	cancel_delayed_work_sync(work);
}

static int xadc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct of_device_id *id;
	struct iio_dev *indio_dev;
	unsigned int bipolar_mask;
	unsigned int conf0;
	struct xadc *xadc;
	int ret;
	int irq;
	int i;

	if (!dev->of_node)
		return -ENODEV;

	id = of_match_node(xadc_of_match_table, dev->of_node);
	if (!id)
		return -EINVAL;

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		return -ENXIO;

	indio_dev = devm_iio_device_alloc(dev, sizeof(*xadc));
	if (!indio_dev)
		return -ENOMEM;

	xadc = iio_priv(indio_dev);
	xadc->ops = id->data;
	xadc->irq = irq;
	init_completion(&xadc->completion);
	mutex_init(&xadc->mutex);
	spin_lock_init(&xadc->lock);
	INIT_DELAYED_WORK(&xadc->zynq_unmask_work, xadc_zynq_unmask_worker);

	xadc->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(xadc->base))
		return PTR_ERR(xadc->base);

	indio_dev->name = xadc_type_names[xadc->ops->type];
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->info = &xadc_info;

	ret = xadc_parse_dt(indio_dev, dev->of_node, &conf0);
	if (ret)
		return ret;

	if (xadc->ops->flags & XADC_FLAGS_BUFFERED) {
		ret = devm_iio_triggered_buffer_setup(dev, indio_dev,
						      &iio_pollfunc_store_time,
						      &xadc_trigger_handler,
						      &xadc_buffer_ops);
		if (ret)
			return ret;

		xadc->convst_trigger = xadc_alloc_trigger(indio_dev, "convst");
		if (IS_ERR(xadc->convst_trigger))
			return PTR_ERR(xadc->convst_trigger);

		xadc->samplerate_trigger = xadc_alloc_trigger(indio_dev,
			"samplerate");
		if (IS_ERR(xadc->samplerate_trigger))
			return PTR_ERR(xadc->samplerate_trigger);
	}

	xadc->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(xadc->clk))
		return PTR_ERR(xadc->clk);

	ret = clk_prepare_enable(xadc->clk);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(dev,
				       xadc_clk_disable_unprepare, xadc->clk);
	if (ret)
		return ret;

	/*
	 * Make sure not to exceed the maximum samplerate since otherwise the
	 * resulting interrupt storm will soft-lock the system.
	 */
	if (xadc->ops->flags & XADC_FLAGS_BUFFERED) {
		ret = xadc_read_samplerate(xadc);
		if (ret < 0)
			return ret;

		if (ret > XADC_MAX_SAMPLERATE) {
			ret = xadc_write_samplerate(xadc, XADC_MAX_SAMPLERATE);
			if (ret < 0)
				return ret;
		}
	}

	ret = devm_request_irq(dev, xadc->irq, xadc->ops->interrupt_handler, 0,
			       dev_name(dev), indio_dev);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(dev, xadc_cancel_delayed_work,
				       &xadc->zynq_unmask_work);
	if (ret)
		return ret;

	ret = xadc->ops->setup(pdev, indio_dev, xadc->irq);
	if (ret)
		return ret;

	for (i = 0; i < 16; i++)
		xadc_read_adc_reg(xadc, XADC_REG_THRESHOLD(i),
			&xadc->threshold[i]);

	ret = xadc_write_adc_reg(xadc, XADC_REG_CONF0, conf0);
	if (ret)
		return ret;

	bipolar_mask = 0;
	for (i = 0; i < indio_dev->num_channels; i++) {
		if (indio_dev->channels[i].scan_type.sign == 's')
			bipolar_mask |= BIT(indio_dev->channels[i].scan_index);
	}

	ret = xadc_write_adc_reg(xadc, XADC_REG_INPUT_MODE(0), bipolar_mask);
	if (ret)
		return ret;

	ret = xadc_write_adc_reg(xadc, XADC_REG_INPUT_MODE(1),
		bipolar_mask >> 16);
	if (ret)
		return ret;

	/* Disable all alarms */
	ret = xadc_update_adc_reg(xadc, XADC_REG_CONF1, XADC_CONF1_ALARM_MASK,
				  XADC_CONF1_ALARM_MASK);
	if (ret)
		return ret;

	/* Set thresholds to min/max */
	for (i = 0; i < 16; i++) {
		/*
		 * Set max voltage threshold and both temperature thresholds to
		 * 0xffff, min voltage threshold to 0.
		 */
		if (i % 8 < 4 || i == 7)
			xadc->threshold[i] = 0xffff;
		else
			xadc->threshold[i] = 0;
		ret = xadc_write_adc_reg(xadc, XADC_REG_THRESHOLD(i),
			xadc->threshold[i]);
		if (ret)
			return ret;
	}

	/* Go to non-buffered mode */
	xadc_postdisable(indio_dev);

	return devm_iio_device_register(dev, indio_dev);
}

static struct platform_driver xadc_driver = {
	.probe = xadc_probe,
	.driver = {
		.name = "xadc",
		.of_match_table = xadc_of_match_table,
	},
};
module_platform_driver(xadc_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("Xilinx XADC IIO driver");
