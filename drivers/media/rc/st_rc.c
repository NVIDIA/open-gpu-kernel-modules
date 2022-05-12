// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2013 STMicroelectronics Limited
 * Author: Srinivas Kandagatla <srinivas.kandagatla@st.com>
 */
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <media/rc-core.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pm_wakeirq.h>

struct st_rc_device {
	struct device			*dev;
	int				irq;
	int				irq_wake;
	struct clk			*sys_clock;
	void __iomem			*base;	/* Register base address */
	void __iomem			*rx_base;/* RX Register base address */
	struct rc_dev			*rdev;
	bool				overclocking;
	int				sample_mult;
	int				sample_div;
	bool				rxuhfmode;
	struct	reset_control		*rstc;
};

/* Registers */
#define IRB_SAMPLE_RATE_COMM	0x64	/* sample freq divisor*/
#define IRB_CLOCK_SEL		0x70	/* clock select       */
#define IRB_CLOCK_SEL_STATUS	0x74	/* clock status       */
/* IRB IR/UHF receiver registers */
#define IRB_RX_ON               0x40	/* pulse time capture */
#define IRB_RX_SYS              0X44	/* sym period capture */
#define IRB_RX_INT_EN           0x48	/* IRQ enable (R/W)   */
#define IRB_RX_INT_STATUS       0x4c	/* IRQ status (R/W)   */
#define IRB_RX_EN               0x50	/* Receive enable     */
#define IRB_MAX_SYM_PERIOD      0x54	/* max sym value      */
#define IRB_RX_INT_CLEAR        0x58	/* overrun status     */
#define IRB_RX_STATUS           0x6c	/* receive status     */
#define IRB_RX_NOISE_SUPPR      0x5c	/* noise suppression  */
#define IRB_RX_POLARITY_INV     0x68	/* polarity inverter  */

/*
 * IRQ set: Enable full FIFO                 1  -> bit  3;
 *          Enable overrun IRQ               1  -> bit  2;
 *          Enable last symbol IRQ           1  -> bit  1:
 *          Enable RX interrupt              1  -> bit  0;
 */
#define IRB_RX_INTS		0x0f
#define IRB_RX_OVERRUN_INT	0x04
 /* maximum symbol period (microsecs),timeout to detect end of symbol train */
#define MAX_SYMB_TIME		0x5000
#define IRB_SAMPLE_FREQ		10000000
#define	IRB_FIFO_NOT_EMPTY	0xff00
#define IRB_OVERFLOW		0x4
#define IRB_TIMEOUT		0xffff
#define IR_ST_NAME "st-rc"

static void st_rc_send_lirc_timeout(struct rc_dev *rdev)
{
	struct ir_raw_event ev = { .timeout = true, .duration = rdev->timeout };
	ir_raw_event_store(rdev, &ev);
}

/*
 * RX graphical example to better understand the difference between ST IR block
 * output and standard definition used by LIRC (and most of the world!)
 *
 *           mark                                     mark
 *      |-IRB_RX_ON-|                            |-IRB_RX_ON-|
 *      ___  ___  ___                            ___  ___  ___             _
 *      | |  | |  | |                            | |  | |  | |             |
 *      | |  | |  | |         space 0            | |  | |  | |   space 1   |
 * _____| |__| |__| |____________________________| |__| |__| |_____________|
 *
 *      |--------------- IRB_RX_SYS -------------|------ IRB_RX_SYS -------|
 *
 *      |------------- encoding bit 0 -----------|---- encoding bit 1 -----|
 *
 * ST hardware returns mark (IRB_RX_ON) and total symbol time (IRB_RX_SYS), so
 * convert to standard mark/space we have to calculate space=(IRB_RX_SYS-mark)
 * The mark time represents the amount of time the carrier (usually 36-40kHz)
 * is detected.The above examples shows Pulse Width Modulation encoding where
 * bit 0 is represented by space>mark.
 */

static irqreturn_t st_rc_rx_interrupt(int irq, void *data)
{
	unsigned long timeout;
	unsigned int symbol, mark = 0;
	struct st_rc_device *dev = data;
	int last_symbol = 0;
	u32 status, int_status;
	struct ir_raw_event ev = {};

	if (dev->irq_wake)
		pm_wakeup_event(dev->dev, 0);

	/* FIXME: is 10ms good enough ? */
	timeout = jiffies +  msecs_to_jiffies(10);
	do {
		status  = readl(dev->rx_base + IRB_RX_STATUS);
		if (!(status & (IRB_FIFO_NOT_EMPTY | IRB_OVERFLOW)))
			break;

		int_status = readl(dev->rx_base + IRB_RX_INT_STATUS);
		if (unlikely(int_status & IRB_RX_OVERRUN_INT)) {
			/* discard the entire collection in case of errors!  */
			ir_raw_event_reset(dev->rdev);
			dev_info(dev->dev, "IR RX overrun\n");
			writel(IRB_RX_OVERRUN_INT,
					dev->rx_base + IRB_RX_INT_CLEAR);
			continue;
		}

		symbol = readl(dev->rx_base + IRB_RX_SYS);
		mark = readl(dev->rx_base + IRB_RX_ON);

		if (symbol == IRB_TIMEOUT)
			last_symbol = 1;

		 /* Ignore any noise */
		if ((mark > 2) && (symbol > 1)) {
			symbol -= mark;
			if (dev->overclocking) { /* adjustments to timings */
				symbol *= dev->sample_mult;
				symbol /= dev->sample_div;
				mark *= dev->sample_mult;
				mark /= dev->sample_div;
			}

			ev.duration = mark;
			ev.pulse = true;
			ir_raw_event_store(dev->rdev, &ev);

			if (!last_symbol) {
				ev.duration = symbol;
				ev.pulse = false;
				ir_raw_event_store(dev->rdev, &ev);
			} else  {
				st_rc_send_lirc_timeout(dev->rdev);
			}

		}
		last_symbol = 0;
	} while (time_is_after_jiffies(timeout));

	writel(IRB_RX_INTS, dev->rx_base + IRB_RX_INT_CLEAR);

	/* Empty software fifo */
	ir_raw_event_handle(dev->rdev);
	return IRQ_HANDLED;
}

static void st_rc_hardware_init(struct st_rc_device *dev)
{
	int baseclock, freqdiff;
	unsigned int rx_max_symbol_per = MAX_SYMB_TIME;
	unsigned int rx_sampling_freq_div;

	/* Enable the IP */
	reset_control_deassert(dev->rstc);

	clk_prepare_enable(dev->sys_clock);
	baseclock = clk_get_rate(dev->sys_clock);

	/* IRB input pins are inverted internally from high to low. */
	writel(1, dev->rx_base + IRB_RX_POLARITY_INV);

	rx_sampling_freq_div = baseclock / IRB_SAMPLE_FREQ;
	writel(rx_sampling_freq_div, dev->base + IRB_SAMPLE_RATE_COMM);

	freqdiff = baseclock - (rx_sampling_freq_div * IRB_SAMPLE_FREQ);
	if (freqdiff) { /* over clocking, workout the adjustment factors */
		dev->overclocking = true;
		dev->sample_mult = 1000;
		dev->sample_div = baseclock / (10000 * rx_sampling_freq_div);
		rx_max_symbol_per = (rx_max_symbol_per * 1000)/dev->sample_div;
	}

	writel(rx_max_symbol_per, dev->rx_base + IRB_MAX_SYM_PERIOD);
}

static int st_rc_remove(struct platform_device *pdev)
{
	struct st_rc_device *rc_dev = platform_get_drvdata(pdev);

	dev_pm_clear_wake_irq(&pdev->dev);
	device_init_wakeup(&pdev->dev, false);
	clk_disable_unprepare(rc_dev->sys_clock);
	rc_unregister_device(rc_dev->rdev);
	return 0;
}

static int st_rc_open(struct rc_dev *rdev)
{
	struct st_rc_device *dev = rdev->priv;
	unsigned long flags;
	local_irq_save(flags);
	/* enable interrupts and receiver */
	writel(IRB_RX_INTS, dev->rx_base + IRB_RX_INT_EN);
	writel(0x01, dev->rx_base + IRB_RX_EN);
	local_irq_restore(flags);

	return 0;
}

static void st_rc_close(struct rc_dev *rdev)
{
	struct st_rc_device *dev = rdev->priv;
	/* disable interrupts and receiver */
	writel(0x00, dev->rx_base + IRB_RX_EN);
	writel(0x00, dev->rx_base + IRB_RX_INT_EN);
}

static int st_rc_probe(struct platform_device *pdev)
{
	int ret = -EINVAL;
	struct rc_dev *rdev;
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct st_rc_device *rc_dev;
	struct device_node *np = pdev->dev.of_node;
	const char *rx_mode;

	rc_dev = devm_kzalloc(dev, sizeof(struct st_rc_device), GFP_KERNEL);

	if (!rc_dev)
		return -ENOMEM;

	rdev = rc_allocate_device(RC_DRIVER_IR_RAW);

	if (!rdev)
		return -ENOMEM;

	if (np && !of_property_read_string(np, "rx-mode", &rx_mode)) {

		if (!strcmp(rx_mode, "uhf")) {
			rc_dev->rxuhfmode = true;
		} else if (!strcmp(rx_mode, "infrared")) {
			rc_dev->rxuhfmode = false;
		} else {
			dev_err(dev, "Unsupported rx mode [%s]\n", rx_mode);
			goto err;
		}

	} else {
		goto err;
	}

	rc_dev->sys_clock = devm_clk_get(dev, NULL);
	if (IS_ERR(rc_dev->sys_clock)) {
		dev_err(dev, "System clock not found\n");
		ret = PTR_ERR(rc_dev->sys_clock);
		goto err;
	}

	rc_dev->irq = platform_get_irq(pdev, 0);
	if (rc_dev->irq < 0) {
		ret = rc_dev->irq;
		goto err;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	rc_dev->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(rc_dev->base)) {
		ret = PTR_ERR(rc_dev->base);
		goto err;
	}

	if (rc_dev->rxuhfmode)
		rc_dev->rx_base = rc_dev->base + 0x40;
	else
		rc_dev->rx_base = rc_dev->base;

	rc_dev->rstc = reset_control_get_optional_exclusive(dev, NULL);
	if (IS_ERR(rc_dev->rstc)) {
		ret = PTR_ERR(rc_dev->rstc);
		goto err;
	}

	rc_dev->dev = dev;
	platform_set_drvdata(pdev, rc_dev);
	st_rc_hardware_init(rc_dev);

	rdev->allowed_protocols = RC_PROTO_BIT_ALL_IR_DECODER;
	/* rx sampling rate is 10Mhz */
	rdev->rx_resolution = 100;
	rdev->timeout = MAX_SYMB_TIME;
	rdev->priv = rc_dev;
	rdev->open = st_rc_open;
	rdev->close = st_rc_close;
	rdev->driver_name = IR_ST_NAME;
	rdev->map_name = RC_MAP_EMPTY;
	rdev->device_name = "ST Remote Control Receiver";

	ret = rc_register_device(rdev);
	if (ret < 0)
		goto clkerr;

	rc_dev->rdev = rdev;
	if (devm_request_irq(dev, rc_dev->irq, st_rc_rx_interrupt,
			     0, IR_ST_NAME, rc_dev) < 0) {
		dev_err(dev, "IRQ %d register failed\n", rc_dev->irq);
		ret = -EINVAL;
		goto rcerr;
	}

	/* enable wake via this device */
	device_init_wakeup(dev, true);
	dev_pm_set_wake_irq(dev, rc_dev->irq);

	/*
	 * for LIRC_MODE_MODE2 or LIRC_MODE_PULSE or LIRC_MODE_RAW
	 * lircd expects a long space first before a signal train to sync.
	 */
	st_rc_send_lirc_timeout(rdev);

	dev_info(dev, "setup in %s mode\n", rc_dev->rxuhfmode ? "UHF" : "IR");

	return ret;
rcerr:
	rc_unregister_device(rdev);
	rdev = NULL;
clkerr:
	clk_disable_unprepare(rc_dev->sys_clock);
err:
	rc_free_device(rdev);
	dev_err(dev, "Unable to register device (%d)\n", ret);
	return ret;
}

#ifdef CONFIG_PM_SLEEP
static int st_rc_suspend(struct device *dev)
{
	struct st_rc_device *rc_dev = dev_get_drvdata(dev);

	if (device_may_wakeup(dev)) {
		if (!enable_irq_wake(rc_dev->irq))
			rc_dev->irq_wake = 1;
		else
			return -EINVAL;
	} else {
		pinctrl_pm_select_sleep_state(dev);
		writel(0x00, rc_dev->rx_base + IRB_RX_EN);
		writel(0x00, rc_dev->rx_base + IRB_RX_INT_EN);
		clk_disable_unprepare(rc_dev->sys_clock);
		reset_control_assert(rc_dev->rstc);
	}

	return 0;
}

static int st_rc_resume(struct device *dev)
{
	struct st_rc_device *rc_dev = dev_get_drvdata(dev);
	struct rc_dev	*rdev = rc_dev->rdev;

	if (rc_dev->irq_wake) {
		disable_irq_wake(rc_dev->irq);
		rc_dev->irq_wake = 0;
	} else {
		pinctrl_pm_select_default_state(dev);
		st_rc_hardware_init(rc_dev);
		if (rdev->users) {
			writel(IRB_RX_INTS, rc_dev->rx_base + IRB_RX_INT_EN);
			writel(0x01, rc_dev->rx_base + IRB_RX_EN);
		}
	}

	return 0;
}

#endif

static SIMPLE_DEV_PM_OPS(st_rc_pm_ops, st_rc_suspend, st_rc_resume);

#ifdef CONFIG_OF
static const struct of_device_id st_rc_match[] = {
	{ .compatible = "st,comms-irb", },
	{},
};

MODULE_DEVICE_TABLE(of, st_rc_match);
#endif

static struct platform_driver st_rc_driver = {
	.driver = {
		.name = IR_ST_NAME,
		.of_match_table = of_match_ptr(st_rc_match),
		.pm     = &st_rc_pm_ops,
	},
	.probe = st_rc_probe,
	.remove = st_rc_remove,
};

module_platform_driver(st_rc_driver);

MODULE_DESCRIPTION("RC Transceiver driver for STMicroelectronics platforms");
MODULE_AUTHOR("STMicroelectronics (R&D) Ltd");
MODULE_LICENSE("GPL");
