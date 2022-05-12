/* SPDX-License-Identifier: GPL-2.0 */
/*
 * <linux/gpio.h>
 *
 * This is the LEGACY GPIO bulk include file, including legacy APIs. It is
 * used for GPIO drivers still referencing the global GPIO numberspace,
 * and should not be included in new code.
 *
 * If you're implementing a GPIO driver, only include <linux/gpio/driver.h>
 * If you're implementing a GPIO consumer, only include <linux/gpio/consumer.h>
 */
#ifndef __LINUX_GPIO_H
#define __LINUX_GPIO_H

#include <linux/errno.h>

/* see Documentation/driver-api/gpio/legacy.rst */

/* make these flag values available regardless of GPIO kconfig options */
#define GPIOF_DIR_OUT	(0 << 0)
#define GPIOF_DIR_IN	(1 << 0)

#define GPIOF_INIT_LOW	(0 << 1)
#define GPIOF_INIT_HIGH	(1 << 1)

#define GPIOF_IN		(GPIOF_DIR_IN)
#define GPIOF_OUT_INIT_LOW	(GPIOF_DIR_OUT | GPIOF_INIT_LOW)
#define GPIOF_OUT_INIT_HIGH	(GPIOF_DIR_OUT | GPIOF_INIT_HIGH)

/* Gpio pin is active-low */
#define GPIOF_ACTIVE_LOW        (1 << 2)

/* Gpio pin is open drain */
#define GPIOF_OPEN_DRAIN	(1 << 3)

/* Gpio pin is open source */
#define GPIOF_OPEN_SOURCE	(1 << 4)

#define GPIOF_EXPORT		(1 << 5)
#define GPIOF_EXPORT_CHANGEABLE	(1 << 6)
#define GPIOF_EXPORT_DIR_FIXED	(GPIOF_EXPORT)
#define GPIOF_EXPORT_DIR_CHANGEABLE (GPIOF_EXPORT | GPIOF_EXPORT_CHANGEABLE)

/**
 * struct gpio - a structure describing a GPIO with configuration
 * @gpio:	the GPIO number
 * @flags:	GPIO configuration as specified by GPIOF_*
 * @label:	a literal description string of this GPIO
 */
struct gpio {
	unsigned	gpio;
	unsigned long	flags;
	const char	*label;
};

#ifdef CONFIG_GPIOLIB

#ifdef CONFIG_ARCH_HAVE_CUSTOM_GPIO_H
#include <asm/gpio.h>
#else

#include <asm-generic/gpio.h>

static inline int gpio_get_value(unsigned int gpio)
{
	return __gpio_get_value(gpio);
}

static inline void gpio_set_value(unsigned int gpio, int value)
{
	__gpio_set_value(gpio, value);
}

static inline int gpio_cansleep(unsigned int gpio)
{
	return __gpio_cansleep(gpio);
}

static inline int gpio_to_irq(unsigned int gpio)
{
	return __gpio_to_irq(gpio);
}

static inline int irq_to_gpio(unsigned int irq)
{
	return -EINVAL;
}

#endif /* ! CONFIG_ARCH_HAVE_CUSTOM_GPIO_H */

/* CONFIG_GPIOLIB: bindings for managed devices that want to request gpios */

struct device;

int devm_gpio_request(struct device *dev, unsigned gpio, const char *label);
int devm_gpio_request_one(struct device *dev, unsigned gpio,
			  unsigned long flags, const char *label);
void devm_gpio_free(struct device *dev, unsigned int gpio);

#else /* ! CONFIG_GPIOLIB */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/bug.h>

struct device;
struct gpio_chip;

static inline bool gpio_is_valid(int number)
{
	return false;
}

static inline int gpio_request(unsigned gpio, const char *label)
{
	return -ENOSYS;
}

static inline int gpio_request_one(unsigned gpio,
					unsigned long flags, const char *label)
{
	return -ENOSYS;
}

static inline int gpio_request_array(const struct gpio *array, size_t num)
{
	return -ENOSYS;
}

static inline void gpio_free(unsigned gpio)
{
	might_sleep();

	/* GPIO can never have been requested */
	WARN_ON(1);
}

static inline void gpio_free_array(const struct gpio *array, size_t num)
{
	might_sleep();

	/* GPIO can never have been requested */
	WARN_ON(1);
}

static inline int gpio_direction_input(unsigned gpio)
{
	return -ENOSYS;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	return -ENOSYS;
}

static inline int gpio_set_debounce(unsigned gpio, unsigned debounce)
{
	return -ENOSYS;
}

static inline int gpio_get_value(unsigned gpio)
{
	/* GPIO can never have been requested or set as {in,out}put */
	WARN_ON(1);
	return 0;
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	/* GPIO can never have been requested or set as output */
	WARN_ON(1);
}

static inline int gpio_cansleep(unsigned gpio)
{
	/* GPIO can never have been requested or set as {in,out}put */
	WARN_ON(1);
	return 0;
}

static inline int gpio_get_value_cansleep(unsigned gpio)
{
	/* GPIO can never have been requested or set as {in,out}put */
	WARN_ON(1);
	return 0;
}

static inline void gpio_set_value_cansleep(unsigned gpio, int value)
{
	/* GPIO can never have been requested or set as output */
	WARN_ON(1);
}

static inline int gpio_export(unsigned gpio, bool direction_may_change)
{
	/* GPIO can never have been requested or set as {in,out}put */
	WARN_ON(1);
	return -EINVAL;
}

static inline int gpio_export_link(struct device *dev, const char *name,
				unsigned gpio)
{
	/* GPIO can never have been exported */
	WARN_ON(1);
	return -EINVAL;
}

static inline void gpio_unexport(unsigned gpio)
{
	/* GPIO can never have been exported */
	WARN_ON(1);
}

static inline int gpio_to_irq(unsigned gpio)
{
	/* GPIO can never have been requested or set as input */
	WARN_ON(1);
	return -EINVAL;
}

static inline int irq_to_gpio(unsigned irq)
{
	/* irq can never have been returned from gpio_to_irq() */
	WARN_ON(1);
	return -EINVAL;
}

static inline int devm_gpio_request(struct device *dev, unsigned gpio,
				    const char *label)
{
	WARN_ON(1);
	return -EINVAL;
}

static inline int devm_gpio_request_one(struct device *dev, unsigned gpio,
					unsigned long flags, const char *label)
{
	WARN_ON(1);
	return -EINVAL;
}

static inline void devm_gpio_free(struct device *dev, unsigned int gpio)
{
	WARN_ON(1);
}

#endif /* ! CONFIG_GPIOLIB */

#endif /* __LINUX_GPIO_H */
