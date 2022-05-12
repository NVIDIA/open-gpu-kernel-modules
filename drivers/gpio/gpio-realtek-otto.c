// SPDX-License-Identifier: GPL-2.0-only

#include <linux/gpio/driver.h>
#include <linux/irq.h>
#include <linux/minmax.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>

/*
 * Total register block size is 0x1C for one bank of four ports (A, B, C, D).
 * An optional second bank, with ports E, F, G, and H, may be present, starting
 * at register offset 0x1C.
 */

/*
 * Pin select: (0) "normal", (1) "dedicate peripheral"
 * Not used on RTL8380/RTL8390, peripheral selection is managed by control bits
 * in the peripheral registers.
 */
#define REALTEK_GPIO_REG_CNR		0x00
/* Clear bit (0) for input, set bit (1) for output */
#define REALTEK_GPIO_REG_DIR		0x08
#define REALTEK_GPIO_REG_DATA		0x0C
/* Read bit for IRQ status, write 1 to clear IRQ */
#define REALTEK_GPIO_REG_ISR		0x10
/* Two bits per GPIO in IMR registers */
#define REALTEK_GPIO_REG_IMR		0x14
#define REALTEK_GPIO_REG_IMR_AB		0x14
#define REALTEK_GPIO_REG_IMR_CD		0x18
#define REALTEK_GPIO_IMR_LINE_MASK	GENMASK(1, 0)
#define REALTEK_GPIO_IRQ_EDGE_FALLING	1
#define REALTEK_GPIO_IRQ_EDGE_RISING	2
#define REALTEK_GPIO_IRQ_EDGE_BOTH	3

#define REALTEK_GPIO_MAX		32
#define REALTEK_GPIO_PORTS_PER_BANK	4

/**
 * realtek_gpio_ctrl - Realtek Otto GPIO driver data
 *
 * @gc: Associated gpio_chip instance
 * @base: Base address of the register block for a GPIO bank
 * @lock: Lock for accessing the IRQ registers and values
 * @intr_mask: Mask for interrupts lines
 * @intr_type: Interrupt type selection
 *
 * Because the interrupt mask register (IMR) combines the function of IRQ type
 * selection and masking, two extra values are stored. @intr_mask is used to
 * mask/unmask the interrupts for a GPIO port, and @intr_type is used to store
 * the selected interrupt types. The logical AND of these values is written to
 * IMR on changes.
 */
struct realtek_gpio_ctrl {
	struct gpio_chip gc;
	void __iomem *base;
	raw_spinlock_t lock;
	u16 intr_mask[REALTEK_GPIO_PORTS_PER_BANK];
	u16 intr_type[REALTEK_GPIO_PORTS_PER_BANK];
};

/* Expand with more flags as devices with other quirks are added */
enum realtek_gpio_flags {
	/*
	 * Allow disabling interrupts, for cases where the port order is
	 * unknown. This may result in a port mismatch between ISR and IMR.
	 * An interrupt would appear to come from a different line than the
	 * line the IRQ handler was assigned to, causing uncaught interrupts.
	 */
	GPIO_INTERRUPTS_DISABLED = BIT(0),
};

static struct realtek_gpio_ctrl *irq_data_to_ctrl(struct irq_data *data)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(data);

	return container_of(gc, struct realtek_gpio_ctrl, gc);
}

/*
 * Normal port order register access
 *
 * Port information is stored with the first port at offset 0, followed by the
 * second, etc. Most registers store one bit per GPIO and use a u8 value per
 * port. The two interrupt mask registers store two bits per GPIO, so use u16
 * values.
 */
static void realtek_gpio_write_imr(struct realtek_gpio_ctrl *ctrl,
	unsigned int port, u16 irq_type, u16 irq_mask)
{
	iowrite16(irq_type & irq_mask, ctrl->base + REALTEK_GPIO_REG_IMR + 2 * port);
}

static void realtek_gpio_clear_isr(struct realtek_gpio_ctrl *ctrl,
	unsigned int port, u8 mask)
{
	iowrite8(mask, ctrl->base + REALTEK_GPIO_REG_ISR + port);
}

static u8 realtek_gpio_read_isr(struct realtek_gpio_ctrl *ctrl, unsigned int port)
{
	return ioread8(ctrl->base + REALTEK_GPIO_REG_ISR + port);
}

/* Set the rising and falling edge mask bits for a GPIO port pin */
static u16 realtek_gpio_imr_bits(unsigned int pin, u16 value)
{
	return (value & REALTEK_GPIO_IMR_LINE_MASK) << 2 * pin;
}

static void realtek_gpio_irq_ack(struct irq_data *data)
{
	struct realtek_gpio_ctrl *ctrl = irq_data_to_ctrl(data);
	irq_hw_number_t line = irqd_to_hwirq(data);
	unsigned int port = line / 8;
	unsigned int port_pin = line % 8;

	realtek_gpio_clear_isr(ctrl, port, BIT(port_pin));
}

static void realtek_gpio_irq_unmask(struct irq_data *data)
{
	struct realtek_gpio_ctrl *ctrl = irq_data_to_ctrl(data);
	unsigned int line = irqd_to_hwirq(data);
	unsigned int port = line / 8;
	unsigned int port_pin = line % 8;
	unsigned long flags;
	u16 m;

	raw_spin_lock_irqsave(&ctrl->lock, flags);
	m = ctrl->intr_mask[port];
	m |= realtek_gpio_imr_bits(port_pin, REALTEK_GPIO_IMR_LINE_MASK);
	ctrl->intr_mask[port] = m;
	realtek_gpio_write_imr(ctrl, port, ctrl->intr_type[port], m);
	raw_spin_unlock_irqrestore(&ctrl->lock, flags);
}

static void realtek_gpio_irq_mask(struct irq_data *data)
{
	struct realtek_gpio_ctrl *ctrl = irq_data_to_ctrl(data);
	unsigned int line = irqd_to_hwirq(data);
	unsigned int port = line / 8;
	unsigned int port_pin = line % 8;
	unsigned long flags;
	u16 m;

	raw_spin_lock_irqsave(&ctrl->lock, flags);
	m = ctrl->intr_mask[port];
	m &= ~realtek_gpio_imr_bits(port_pin, REALTEK_GPIO_IMR_LINE_MASK);
	ctrl->intr_mask[port] = m;
	realtek_gpio_write_imr(ctrl, port, ctrl->intr_type[port], m);
	raw_spin_unlock_irqrestore(&ctrl->lock, flags);
}

static int realtek_gpio_irq_set_type(struct irq_data *data, unsigned int flow_type)
{
	struct realtek_gpio_ctrl *ctrl = irq_data_to_ctrl(data);
	unsigned int line = irqd_to_hwirq(data);
	unsigned int port = line / 8;
	unsigned int port_pin = line % 8;
	unsigned long flags;
	u16 type, t;

	switch (flow_type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_FALLING:
		type = REALTEK_GPIO_IRQ_EDGE_FALLING;
		break;
	case IRQ_TYPE_EDGE_RISING:
		type = REALTEK_GPIO_IRQ_EDGE_RISING;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		type = REALTEK_GPIO_IRQ_EDGE_BOTH;
		break;
	default:
		return -EINVAL;
	}

	irq_set_handler_locked(data, handle_edge_irq);

	raw_spin_lock_irqsave(&ctrl->lock, flags);
	t = ctrl->intr_type[port];
	t &= ~realtek_gpio_imr_bits(port_pin, REALTEK_GPIO_IMR_LINE_MASK);
	t |= realtek_gpio_imr_bits(port_pin, type);
	ctrl->intr_type[port] = t;
	realtek_gpio_write_imr(ctrl, port, t, ctrl->intr_mask[port]);
	raw_spin_unlock_irqrestore(&ctrl->lock, flags);

	return 0;
}

static void realtek_gpio_irq_handler(struct irq_desc *desc)
{
	struct gpio_chip *gc = irq_desc_get_handler_data(desc);
	struct realtek_gpio_ctrl *ctrl = gpiochip_get_data(gc);
	struct irq_chip *irq_chip = irq_desc_get_chip(desc);
	unsigned int lines_done;
	unsigned int port_pin_count;
	unsigned int irq;
	unsigned long status;
	int offset;

	chained_irq_enter(irq_chip, desc);

	for (lines_done = 0; lines_done < gc->ngpio; lines_done += 8) {
		status = realtek_gpio_read_isr(ctrl, lines_done / 8);
		port_pin_count = min(gc->ngpio - lines_done, 8U);
		for_each_set_bit(offset, &status, port_pin_count) {
			irq = irq_find_mapping(gc->irq.domain, offset);
			generic_handle_irq(irq);
		}
	}

	chained_irq_exit(irq_chip, desc);
}

static int realtek_gpio_irq_init(struct gpio_chip *gc)
{
	struct realtek_gpio_ctrl *ctrl = gpiochip_get_data(gc);
	unsigned int port;

	for (port = 0; (port * 8) < gc->ngpio; port++) {
		realtek_gpio_write_imr(ctrl, port, 0, 0);
		realtek_gpio_clear_isr(ctrl, port, GENMASK(7, 0));
	}

	return 0;
}

static struct irq_chip realtek_gpio_irq_chip = {
	.name = "realtek-otto-gpio",
	.irq_ack = realtek_gpio_irq_ack,
	.irq_mask = realtek_gpio_irq_mask,
	.irq_unmask = realtek_gpio_irq_unmask,
	.irq_set_type = realtek_gpio_irq_set_type,
};

static const struct of_device_id realtek_gpio_of_match[] = {
	{
		.compatible = "realtek,otto-gpio",
		.data = (void *)GPIO_INTERRUPTS_DISABLED,
	},
	{
		.compatible = "realtek,rtl8380-gpio",
	},
	{
		.compatible = "realtek,rtl8390-gpio",
	},
	{}
};
MODULE_DEVICE_TABLE(of, realtek_gpio_of_match);

static int realtek_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	unsigned int dev_flags;
	struct gpio_irq_chip *girq;
	struct realtek_gpio_ctrl *ctrl;
	u32 ngpios;
	int err, irq;

	ctrl = devm_kzalloc(dev, sizeof(*ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	dev_flags = (unsigned int) device_get_match_data(dev);

	ngpios = REALTEK_GPIO_MAX;
	device_property_read_u32(dev, "ngpios", &ngpios);

	if (ngpios > REALTEK_GPIO_MAX) {
		dev_err(&pdev->dev, "invalid ngpios (max. %d)\n",
			REALTEK_GPIO_MAX);
		return -EINVAL;
	}

	ctrl->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(ctrl->base))
		return PTR_ERR(ctrl->base);

	raw_spin_lock_init(&ctrl->lock);

	err = bgpio_init(&ctrl->gc, dev, 4,
		ctrl->base + REALTEK_GPIO_REG_DATA, NULL, NULL,
		ctrl->base + REALTEK_GPIO_REG_DIR, NULL,
		BGPIOF_BIG_ENDIAN_BYTE_ORDER);
	if (err) {
		dev_err(dev, "unable to init generic GPIO");
		return err;
	}

	ctrl->gc.ngpio = ngpios;
	ctrl->gc.owner = THIS_MODULE;

	irq = platform_get_irq_optional(pdev, 0);
	if (!(dev_flags & GPIO_INTERRUPTS_DISABLED) && irq > 0) {
		girq = &ctrl->gc.irq;
		girq->chip = &realtek_gpio_irq_chip;
		girq->default_type = IRQ_TYPE_NONE;
		girq->handler = handle_bad_irq;
		girq->parent_handler = realtek_gpio_irq_handler;
		girq->num_parents = 1;
		girq->parents = devm_kcalloc(dev, girq->num_parents,
					sizeof(*girq->parents),	GFP_KERNEL);
		if (!girq->parents)
			return -ENOMEM;
		girq->parents[0] = irq;
		girq->init_hw = realtek_gpio_irq_init;
	}

	return devm_gpiochip_add_data(dev, &ctrl->gc, ctrl);
}

static struct platform_driver realtek_gpio_driver = {
	.driver = {
		.name = "realtek-otto-gpio",
		.of_match_table	= realtek_gpio_of_match,
	},
	.probe = realtek_gpio_probe,
};
module_platform_driver(realtek_gpio_driver);

MODULE_DESCRIPTION("Realtek Otto GPIO support");
MODULE_AUTHOR("Sander Vanheule <sander@svanheule.net>");
MODULE_LICENSE("GPL v2");
