// SPDX-License-Identifier: GPL-2.0-only
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <asm/adc.h>
#include <mach/hp6xx.h>

#define MODNAME "hp680_ts_input"

#define HP680_TS_ABS_X_MIN	40
#define HP680_TS_ABS_X_MAX	950
#define HP680_TS_ABS_Y_MIN	80
#define HP680_TS_ABS_Y_MAX	910

#define	PHDR	0xa400012e
#define SCPDR	0xa4000136

static void do_softint(struct work_struct *work);

static struct input_dev *hp680_ts_dev;
static DECLARE_DELAYED_WORK(work, do_softint);

static void do_softint(struct work_struct *work)
{
	int absx = 0, absy = 0;
	u8 scpdr;
	int touched = 0;

	if (__raw_readb(PHDR) & PHDR_TS_PEN_DOWN) {
		scpdr = __raw_readb(SCPDR);
		scpdr |= SCPDR_TS_SCAN_ENABLE;
		scpdr &= ~SCPDR_TS_SCAN_Y;
		__raw_writeb(scpdr, SCPDR);
		udelay(30);

		absy = adc_single(ADC_CHANNEL_TS_Y);

		scpdr = __raw_readb(SCPDR);
		scpdr |= SCPDR_TS_SCAN_Y;
		scpdr &= ~SCPDR_TS_SCAN_X;
		__raw_writeb(scpdr, SCPDR);
		udelay(30);

		absx = adc_single(ADC_CHANNEL_TS_X);

		scpdr = __raw_readb(SCPDR);
		scpdr |= SCPDR_TS_SCAN_X;
		scpdr &= ~SCPDR_TS_SCAN_ENABLE;
		__raw_writeb(scpdr, SCPDR);
		udelay(100);
		touched = __raw_readb(PHDR) & PHDR_TS_PEN_DOWN;
	}

	if (touched) {
		input_report_key(hp680_ts_dev, BTN_TOUCH, 1);
		input_report_abs(hp680_ts_dev, ABS_X, absx);
		input_report_abs(hp680_ts_dev, ABS_Y, absy);
	} else {
		input_report_key(hp680_ts_dev, BTN_TOUCH, 0);
	}

	input_sync(hp680_ts_dev);
	enable_irq(HP680_TS_IRQ);
}

static irqreturn_t hp680_ts_interrupt(int irq, void *dev)
{
	disable_irq_nosync(irq);
	schedule_delayed_work(&work, HZ / 20);

	return IRQ_HANDLED;
}

static int __init hp680_ts_init(void)
{
	int err;

	hp680_ts_dev = input_allocate_device();
	if (!hp680_ts_dev)
		return -ENOMEM;

	hp680_ts_dev->evbit[0] = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
	hp680_ts_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

	input_set_abs_params(hp680_ts_dev, ABS_X,
		HP680_TS_ABS_X_MIN, HP680_TS_ABS_X_MAX, 0, 0);
	input_set_abs_params(hp680_ts_dev, ABS_Y,
		HP680_TS_ABS_Y_MIN, HP680_TS_ABS_Y_MAX, 0, 0);

	hp680_ts_dev->name = "HP Jornada touchscreen";
	hp680_ts_dev->phys = "hp680_ts/input0";

	if (request_irq(HP680_TS_IRQ, hp680_ts_interrupt,
			0, MODNAME, NULL) < 0) {
		printk(KERN_ERR "hp680_touchscreen.c: Can't allocate irq %d\n",
		       HP680_TS_IRQ);
		err = -EBUSY;
		goto fail1;
	}

	err = input_register_device(hp680_ts_dev);
	if (err)
		goto fail2;

	return 0;

 fail2:	free_irq(HP680_TS_IRQ, NULL);
	cancel_delayed_work_sync(&work);
 fail1:	input_free_device(hp680_ts_dev);
	return err;
}

static void __exit hp680_ts_exit(void)
{
	free_irq(HP680_TS_IRQ, NULL);
	cancel_delayed_work_sync(&work);
	input_unregister_device(hp680_ts_dev);
}

module_init(hp680_ts_init);
module_exit(hp680_ts_exit);

MODULE_AUTHOR("Andriy Skulysh, askulysh@image.kiev.ua");
MODULE_DESCRIPTION("HP Jornada 680 touchscreen driver");
MODULE_LICENSE("GPL");
