// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * QUICC Engine GPIOs
 *
 * Copyright (c) MontaVista Software, Inc. 2008.
 *
 * Author: Anton Vorontsov <avorontsov@ru.mvista.com>
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio/driver.h>
/* FIXME: needed for gpio_to_chip() get rid of this */
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <soc/fsl/qe/qe.h>

struct qe_gpio_chip {
	struct of_mm_gpio_chip mm_gc;
	spinlock_t lock;

	unsigned long pin_flags[QE_PIO_PINS];
#define QE_PIN_REQUESTED 0

	/* shadowed data register to clear/set bits safely */
	u32 cpdata;

	/* saved_regs used to restore dedicated functions */
	struct qe_pio_regs saved_regs;
};

static void qe_gpio_save_regs(struct of_mm_gpio_chip *mm_gc)
{
	struct qe_gpio_chip *qe_gc =
		container_of(mm_gc, struct qe_gpio_chip, mm_gc);
	struct qe_pio_regs __iomem *regs = mm_gc->regs;

	qe_gc->cpdata = ioread32be(&regs->cpdata);
	qe_gc->saved_regs.cpdata = qe_gc->cpdata;
	qe_gc->saved_regs.cpdir1 = ioread32be(&regs->cpdir1);
	qe_gc->saved_regs.cpdir2 = ioread32be(&regs->cpdir2);
	qe_gc->saved_regs.cppar1 = ioread32be(&regs->cppar1);
	qe_gc->saved_regs.cppar2 = ioread32be(&regs->cppar2);
	qe_gc->saved_regs.cpodr = ioread32be(&regs->cpodr);
}

static int qe_gpio_get(struct gpio_chip *gc, unsigned int gpio)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct qe_pio_regs __iomem *regs = mm_gc->regs;
	u32 pin_mask = 1 << (QE_PIO_PINS - 1 - gpio);

	return !!(ioread32be(&regs->cpdata) & pin_mask);
}

static void qe_gpio_set(struct gpio_chip *gc, unsigned int gpio, int val)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct qe_gpio_chip *qe_gc = gpiochip_get_data(gc);
	struct qe_pio_regs __iomem *regs = mm_gc->regs;
	unsigned long flags;
	u32 pin_mask = 1 << (QE_PIO_PINS - 1 - gpio);

	spin_lock_irqsave(&qe_gc->lock, flags);

	if (val)
		qe_gc->cpdata |= pin_mask;
	else
		qe_gc->cpdata &= ~pin_mask;

	iowrite32be(qe_gc->cpdata, &regs->cpdata);

	spin_unlock_irqrestore(&qe_gc->lock, flags);
}

static void qe_gpio_set_multiple(struct gpio_chip *gc,
				 unsigned long *mask, unsigned long *bits)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct qe_gpio_chip *qe_gc = gpiochip_get_data(gc);
	struct qe_pio_regs __iomem *regs = mm_gc->regs;
	unsigned long flags;
	int i;

	spin_lock_irqsave(&qe_gc->lock, flags);

	for (i = 0; i < gc->ngpio; i++) {
		if (*mask == 0)
			break;
		if (__test_and_clear_bit(i, mask)) {
			if (test_bit(i, bits))
				qe_gc->cpdata |= (1U << (QE_PIO_PINS - 1 - i));
			else
				qe_gc->cpdata &= ~(1U << (QE_PIO_PINS - 1 - i));
		}
	}

	iowrite32be(qe_gc->cpdata, &regs->cpdata);

	spin_unlock_irqrestore(&qe_gc->lock, flags);
}

static int qe_gpio_dir_in(struct gpio_chip *gc, unsigned int gpio)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct qe_gpio_chip *qe_gc = gpiochip_get_data(gc);
	unsigned long flags;

	spin_lock_irqsave(&qe_gc->lock, flags);

	__par_io_config_pin(mm_gc->regs, gpio, QE_PIO_DIR_IN, 0, 0, 0);

	spin_unlock_irqrestore(&qe_gc->lock, flags);

	return 0;
}

static int qe_gpio_dir_out(struct gpio_chip *gc, unsigned int gpio, int val)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct qe_gpio_chip *qe_gc = gpiochip_get_data(gc);
	unsigned long flags;

	qe_gpio_set(gc, gpio, val);

	spin_lock_irqsave(&qe_gc->lock, flags);

	__par_io_config_pin(mm_gc->regs, gpio, QE_PIO_DIR_OUT, 0, 0, 0);

	spin_unlock_irqrestore(&qe_gc->lock, flags);

	return 0;
}

struct qe_pin {
	/*
	 * The qe_gpio_chip name is unfortunate, we should change that to
	 * something like qe_pio_controller. Someday.
	 */
	struct qe_gpio_chip *controller;
	int num;
};

/**
 * qe_pin_request - Request a QE pin
 * @np:		device node to get a pin from
 * @index:	index of a pin in the device tree
 * Context:	non-atomic
 *
 * This function return qe_pin so that you could use it with the rest of
 * the QE Pin Multiplexing API.
 */
struct qe_pin *qe_pin_request(struct device_node *np, int index)
{
	struct qe_pin *qe_pin;
	struct gpio_chip *gc;
	struct qe_gpio_chip *qe_gc;
	int err;
	unsigned long flags;

	qe_pin = kzalloc(sizeof(*qe_pin), GFP_KERNEL);
	if (!qe_pin) {
		pr_debug("%s: can't allocate memory\n", __func__);
		return ERR_PTR(-ENOMEM);
	}

	err = of_get_gpio(np, index);
	if (err < 0)
		goto err0;
	gc = gpio_to_chip(err);
	if (WARN_ON(!gc)) {
		err = -ENODEV;
		goto err0;
	}

	if (!of_device_is_compatible(gc->of_node, "fsl,mpc8323-qe-pario-bank")) {
		pr_debug("%s: tried to get a non-qe pin\n", __func__);
		err = -EINVAL;
		goto err0;
	}

	qe_gc = gpiochip_get_data(gc);

	spin_lock_irqsave(&qe_gc->lock, flags);

	err -= gc->base;
	if (test_and_set_bit(QE_PIN_REQUESTED, &qe_gc->pin_flags[err]) == 0) {
		qe_pin->controller = qe_gc;
		qe_pin->num = err;
		err = 0;
	} else {
		err = -EBUSY;
	}

	spin_unlock_irqrestore(&qe_gc->lock, flags);

	if (!err)
		return qe_pin;
err0:
	kfree(qe_pin);
	pr_debug("%s failed with status %d\n", __func__, err);
	return ERR_PTR(err);
}
EXPORT_SYMBOL(qe_pin_request);

/**
 * qe_pin_free - Free a pin
 * @qe_pin:	pointer to the qe_pin structure
 * Context:	any
 *
 * This function frees the qe_pin structure and makes a pin available
 * for further qe_pin_request() calls.
 */
void qe_pin_free(struct qe_pin *qe_pin)
{
	struct qe_gpio_chip *qe_gc = qe_pin->controller;
	unsigned long flags;
	const int pin = qe_pin->num;

	spin_lock_irqsave(&qe_gc->lock, flags);
	test_and_clear_bit(QE_PIN_REQUESTED, &qe_gc->pin_flags[pin]);
	spin_unlock_irqrestore(&qe_gc->lock, flags);

	kfree(qe_pin);
}
EXPORT_SYMBOL(qe_pin_free);

/**
 * qe_pin_set_dedicated - Revert a pin to a dedicated peripheral function mode
 * @qe_pin:	pointer to the qe_pin structure
 * Context:	any
 *
 * This function resets a pin to a dedicated peripheral function that
 * has been set up by the firmware.
 */
void qe_pin_set_dedicated(struct qe_pin *qe_pin)
{
	struct qe_gpio_chip *qe_gc = qe_pin->controller;
	struct qe_pio_regs __iomem *regs = qe_gc->mm_gc.regs;
	struct qe_pio_regs *sregs = &qe_gc->saved_regs;
	int pin = qe_pin->num;
	u32 mask1 = 1 << (QE_PIO_PINS - (pin + 1));
	u32 mask2 = 0x3 << (QE_PIO_PINS - (pin % (QE_PIO_PINS / 2) + 1) * 2);
	bool second_reg = pin > (QE_PIO_PINS / 2) - 1;
	unsigned long flags;

	spin_lock_irqsave(&qe_gc->lock, flags);

	if (second_reg) {
		qe_clrsetbits_be32(&regs->cpdir2, mask2,
				   sregs->cpdir2 & mask2);
		qe_clrsetbits_be32(&regs->cppar2, mask2,
				   sregs->cppar2 & mask2);
	} else {
		qe_clrsetbits_be32(&regs->cpdir1, mask2,
				   sregs->cpdir1 & mask2);
		qe_clrsetbits_be32(&regs->cppar1, mask2,
				   sregs->cppar1 & mask2);
	}

	if (sregs->cpdata & mask1)
		qe_gc->cpdata |= mask1;
	else
		qe_gc->cpdata &= ~mask1;

	iowrite32be(qe_gc->cpdata, &regs->cpdata);
	qe_clrsetbits_be32(&regs->cpodr, mask1, sregs->cpodr & mask1);

	spin_unlock_irqrestore(&qe_gc->lock, flags);
}
EXPORT_SYMBOL(qe_pin_set_dedicated);

/**
 * qe_pin_set_gpio - Set a pin to the GPIO mode
 * @qe_pin:	pointer to the qe_pin structure
 * Context:	any
 *
 * This function sets a pin to the GPIO mode.
 */
void qe_pin_set_gpio(struct qe_pin *qe_pin)
{
	struct qe_gpio_chip *qe_gc = qe_pin->controller;
	struct qe_pio_regs __iomem *regs = qe_gc->mm_gc.regs;
	unsigned long flags;

	spin_lock_irqsave(&qe_gc->lock, flags);

	/* Let's make it input by default, GPIO API is able to change that. */
	__par_io_config_pin(regs, qe_pin->num, QE_PIO_DIR_IN, 0, 0, 0);

	spin_unlock_irqrestore(&qe_gc->lock, flags);
}
EXPORT_SYMBOL(qe_pin_set_gpio);

static int __init qe_add_gpiochips(void)
{
	struct device_node *np;

	for_each_compatible_node(np, NULL, "fsl,mpc8323-qe-pario-bank") {
		int ret;
		struct qe_gpio_chip *qe_gc;
		struct of_mm_gpio_chip *mm_gc;
		struct gpio_chip *gc;

		qe_gc = kzalloc(sizeof(*qe_gc), GFP_KERNEL);
		if (!qe_gc) {
			ret = -ENOMEM;
			goto err;
		}

		spin_lock_init(&qe_gc->lock);

		mm_gc = &qe_gc->mm_gc;
		gc = &mm_gc->gc;

		mm_gc->save_regs = qe_gpio_save_regs;
		gc->ngpio = QE_PIO_PINS;
		gc->direction_input = qe_gpio_dir_in;
		gc->direction_output = qe_gpio_dir_out;
		gc->get = qe_gpio_get;
		gc->set = qe_gpio_set;
		gc->set_multiple = qe_gpio_set_multiple;

		ret = of_mm_gpiochip_add_data(np, mm_gc, qe_gc);
		if (ret)
			goto err;
		continue;
err:
		pr_err("%pOF: registration failed with status %d\n",
		       np, ret);
		kfree(qe_gc);
		/* try others anyway */
	}
	return 0;
}
arch_initcall(qe_add_gpiochips);
