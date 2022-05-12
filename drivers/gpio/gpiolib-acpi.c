// SPDX-License-Identifier: GPL-2.0
/*
 * ACPI helpers for GPIO API
 *
 * Copyright (C) 2012, Intel Corporation
 * Authors: Mathias Nyman <mathias.nyman@linux.intel.com>
 *          Mika Westerberg <mika.westerberg@linux.intel.com>
 */

#include <linux/dmi.h>
#include <linux/errno.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/gpio/machine.h>
#include <linux/export.h>
#include <linux/acpi.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/pinctrl/pinctrl.h>

#include "gpiolib.h"
#include "gpiolib-acpi.h"

static int run_edge_events_on_boot = -1;
module_param(run_edge_events_on_boot, int, 0444);
MODULE_PARM_DESC(run_edge_events_on_boot,
		 "Run edge _AEI event-handlers at boot: 0=no, 1=yes, -1=auto");

static char *ignore_wake;
module_param(ignore_wake, charp, 0444);
MODULE_PARM_DESC(ignore_wake,
		 "controller@pin combos on which to ignore the ACPI wake flag "
		 "ignore_wake=controller@pin[,controller@pin[,...]]");

struct acpi_gpiolib_dmi_quirk {
	bool no_edge_events_on_boot;
	char *ignore_wake;
};

/**
 * struct acpi_gpio_event - ACPI GPIO event handler data
 *
 * @node:	  list-entry of the events list of the struct acpi_gpio_chip
 * @handle:	  handle of ACPI method to execute when the IRQ triggers
 * @handler:	  handler function to pass to request_irq() when requesting the IRQ
 * @pin:	  GPIO pin number on the struct gpio_chip
 * @irq:	  Linux IRQ number for the event, for request_irq() / free_irq()
 * @irqflags:	  flags to pass to request_irq() when requesting the IRQ
 * @irq_is_wake:  If the ACPI flags indicate the IRQ is a wakeup source
 * @irq_requested:True if request_irq() has been done
 * @desc:	  struct gpio_desc for the GPIO pin for this event
 */
struct acpi_gpio_event {
	struct list_head node;
	acpi_handle handle;
	irq_handler_t handler;
	unsigned int pin;
	unsigned int irq;
	unsigned long irqflags;
	bool irq_is_wake;
	bool irq_requested;
	struct gpio_desc *desc;
};

struct acpi_gpio_connection {
	struct list_head node;
	unsigned int pin;
	struct gpio_desc *desc;
};

struct acpi_gpio_chip {
	/*
	 * ACPICA requires that the first field of the context parameter
	 * passed to acpi_install_address_space_handler() is large enough
	 * to hold struct acpi_connection_info.
	 */
	struct acpi_connection_info conn_info;
	struct list_head conns;
	struct mutex conn_lock;
	struct gpio_chip *chip;
	struct list_head events;
	struct list_head deferred_req_irqs_list_entry;
};

/*
 * For GPIO chips which call acpi_gpiochip_request_interrupts() before late_init
 * (so builtin drivers) we register the ACPI GpioInt IRQ handlers from a
 * late_initcall_sync() handler, so that other builtin drivers can register their
 * OpRegions before the event handlers can run. This list contains GPIO chips
 * for which the acpi_gpiochip_request_irqs() call has been deferred.
 */
static DEFINE_MUTEX(acpi_gpio_deferred_req_irqs_lock);
static LIST_HEAD(acpi_gpio_deferred_req_irqs_list);
static bool acpi_gpio_deferred_req_irqs_done;

static int acpi_gpiochip_find(struct gpio_chip *gc, void *data)
{
	if (!gc->parent)
		return false;

	return ACPI_HANDLE(gc->parent) == data;
}

/**
 * acpi_get_gpiod() - Translate ACPI GPIO pin to GPIO descriptor usable with GPIO API
 * @path:	ACPI GPIO controller full path name, (e.g. "\\_SB.GPO1")
 * @pin:	ACPI GPIO pin number (0-based, controller-relative)
 *
 * Return: GPIO descriptor to use with Linux generic GPIO API, or ERR_PTR
 * error value. Specifically returns %-EPROBE_DEFER if the referenced GPIO
 * controller does not have GPIO chip registered at the moment. This is to
 * support probe deferral.
 */
static struct gpio_desc *acpi_get_gpiod(char *path, int pin)
{
	struct gpio_chip *chip;
	acpi_handle handle;
	acpi_status status;

	status = acpi_get_handle(NULL, path, &handle);
	if (ACPI_FAILURE(status))
		return ERR_PTR(-ENODEV);

	chip = gpiochip_find(handle, acpi_gpiochip_find);
	if (!chip)
		return ERR_PTR(-EPROBE_DEFER);

	return gpiochip_get_desc(chip, pin);
}

static irqreturn_t acpi_gpio_irq_handler(int irq, void *data)
{
	struct acpi_gpio_event *event = data;

	acpi_evaluate_object(event->handle, NULL, NULL, NULL);

	return IRQ_HANDLED;
}

static irqreturn_t acpi_gpio_irq_handler_evt(int irq, void *data)
{
	struct acpi_gpio_event *event = data;

	acpi_execute_simple_method(event->handle, NULL, event->pin);

	return IRQ_HANDLED;
}

static void acpi_gpio_chip_dh(acpi_handle handle, void *data)
{
	/* The address of this function is used as a key. */
}

bool acpi_gpio_get_irq_resource(struct acpi_resource *ares,
				struct acpi_resource_gpio **agpio)
{
	struct acpi_resource_gpio *gpio;

	if (ares->type != ACPI_RESOURCE_TYPE_GPIO)
		return false;

	gpio = &ares->data.gpio;
	if (gpio->connection_type != ACPI_RESOURCE_GPIO_TYPE_INT)
		return false;

	*agpio = gpio;
	return true;
}
EXPORT_SYMBOL_GPL(acpi_gpio_get_irq_resource);

static void acpi_gpiochip_request_irq(struct acpi_gpio_chip *acpi_gpio,
				      struct acpi_gpio_event *event)
{
	int ret, value;

	ret = request_threaded_irq(event->irq, NULL, event->handler,
				   event->irqflags | IRQF_ONESHOT, "ACPI:Event", event);
	if (ret) {
		dev_err(acpi_gpio->chip->parent,
			"Failed to setup interrupt handler for %d\n",
			event->irq);
		return;
	}

	if (event->irq_is_wake)
		enable_irq_wake(event->irq);

	event->irq_requested = true;

	/* Make sure we trigger the initial state of edge-triggered IRQs */
	if (run_edge_events_on_boot &&
	    (event->irqflags & (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING))) {
		value = gpiod_get_raw_value_cansleep(event->desc);
		if (((event->irqflags & IRQF_TRIGGER_RISING) && value == 1) ||
		    ((event->irqflags & IRQF_TRIGGER_FALLING) && value == 0))
			event->handler(event->irq, event);
	}
}

static void acpi_gpiochip_request_irqs(struct acpi_gpio_chip *acpi_gpio)
{
	struct acpi_gpio_event *event;

	list_for_each_entry(event, &acpi_gpio->events, node)
		acpi_gpiochip_request_irq(acpi_gpio, event);
}

static enum gpiod_flags
acpi_gpio_to_gpiod_flags(const struct acpi_resource_gpio *agpio, int polarity)
{
	/* GpioInt() implies input configuration */
	if (agpio->connection_type == ACPI_RESOURCE_GPIO_TYPE_INT)
		return GPIOD_IN;

	switch (agpio->io_restriction) {
	case ACPI_IO_RESTRICT_INPUT:
		return GPIOD_IN;
	case ACPI_IO_RESTRICT_OUTPUT:
		/*
		 * ACPI GPIO resources don't contain an initial value for the
		 * GPIO. Therefore we deduce that value from the pull field
		 * and the polarity instead. If the pin is pulled up we assume
		 * default to be high, if it is pulled down we assume default
		 * to be low, otherwise we leave pin untouched. For active low
		 * polarity values will be switched. See also
		 * Documentation/firmware-guide/acpi/gpio-properties.rst.
		 */
		switch (agpio->pin_config) {
		case ACPI_PIN_CONFIG_PULLUP:
			return polarity == GPIO_ACTIVE_LOW ? GPIOD_OUT_LOW : GPIOD_OUT_HIGH;
		case ACPI_PIN_CONFIG_PULLDOWN:
			return polarity == GPIO_ACTIVE_LOW ? GPIOD_OUT_HIGH : GPIOD_OUT_LOW;
		default:
			break;
		}
		break;
	default:
		break;
	}

	/*
	 * Assume that the BIOS has configured the direction and pull
	 * accordingly.
	 */
	return GPIOD_ASIS;
}

static struct gpio_desc *acpi_request_own_gpiod(struct gpio_chip *chip,
						struct acpi_resource_gpio *agpio,
						unsigned int index,
						const char *label)
{
	int polarity = GPIO_ACTIVE_HIGH;
	enum gpiod_flags flags = acpi_gpio_to_gpiod_flags(agpio, polarity);
	unsigned int pin = agpio->pin_table[index];
	struct gpio_desc *desc;
	int ret;

	desc = gpiochip_request_own_desc(chip, pin, label, polarity, flags);
	if (IS_ERR(desc))
		return desc;

	ret = gpio_set_debounce_timeout(desc, agpio->debounce_timeout);
	if (ret)
		gpiochip_free_own_desc(desc);

	return ret ? ERR_PTR(ret) : desc;
}

static bool acpi_gpio_in_ignore_list(const char *controller_in, int pin_in)
{
	const char *controller, *pin_str;
	int len, pin;
	char *endp;

	controller = ignore_wake;
	while (controller) {
		pin_str = strchr(controller, '@');
		if (!pin_str)
			goto err;

		len = pin_str - controller;
		if (len == strlen(controller_in) &&
		    strncmp(controller, controller_in, len) == 0) {
			pin = simple_strtoul(pin_str + 1, &endp, 10);
			if (*endp != 0 && *endp != ',')
				goto err;

			if (pin == pin_in)
				return true;
		}

		controller = strchr(controller, ',');
		if (controller)
			controller++;
	}

	return false;
err:
	pr_err_once("Error invalid value for gpiolib_acpi.ignore_wake: %s\n",
		    ignore_wake);
	return false;
}

static bool acpi_gpio_irq_is_wake(struct device *parent,
				  struct acpi_resource_gpio *agpio)
{
	int pin = agpio->pin_table[0];

	if (agpio->wake_capable != ACPI_WAKE_CAPABLE)
		return false;

	if (acpi_gpio_in_ignore_list(dev_name(parent), pin)) {
		dev_info(parent, "Ignoring wakeup on pin %d\n", pin);
		return false;
	}

	return true;
}

/* Always returns AE_OK so that we keep looping over the resources */
static acpi_status acpi_gpiochip_alloc_event(struct acpi_resource *ares,
					     void *context)
{
	struct acpi_gpio_chip *acpi_gpio = context;
	struct gpio_chip *chip = acpi_gpio->chip;
	struct acpi_resource_gpio *agpio;
	acpi_handle handle, evt_handle;
	struct acpi_gpio_event *event;
	irq_handler_t handler = NULL;
	struct gpio_desc *desc;
	int ret, pin, irq;

	if (!acpi_gpio_get_irq_resource(ares, &agpio))
		return AE_OK;

	handle = ACPI_HANDLE(chip->parent);
	pin = agpio->pin_table[0];

	if (pin <= 255) {
		char ev_name[5];
		sprintf(ev_name, "_%c%02hhX",
			agpio->triggering == ACPI_EDGE_SENSITIVE ? 'E' : 'L',
			pin);
		if (ACPI_SUCCESS(acpi_get_handle(handle, ev_name, &evt_handle)))
			handler = acpi_gpio_irq_handler;
	}
	if (!handler) {
		if (ACPI_SUCCESS(acpi_get_handle(handle, "_EVT", &evt_handle)))
			handler = acpi_gpio_irq_handler_evt;
	}
	if (!handler)
		return AE_OK;

	desc = acpi_request_own_gpiod(chip, agpio, 0, "ACPI:Event");
	if (IS_ERR(desc)) {
		dev_err(chip->parent,
			"Failed to request GPIO for pin 0x%04X, err %ld\n",
			pin, PTR_ERR(desc));
		return AE_OK;
	}

	ret = gpiochip_lock_as_irq(chip, pin);
	if (ret) {
		dev_err(chip->parent,
			"Failed to lock GPIO pin 0x%04X as interrupt, err %d\n",
			pin, ret);
		goto fail_free_desc;
	}

	irq = gpiod_to_irq(desc);
	if (irq < 0) {
		dev_err(chip->parent,
			"Failed to translate GPIO pin 0x%04X to IRQ, err %d\n",
			pin, irq);
		goto fail_unlock_irq;
	}

	event = kzalloc(sizeof(*event), GFP_KERNEL);
	if (!event)
		goto fail_unlock_irq;

	event->irqflags = IRQF_ONESHOT;
	if (agpio->triggering == ACPI_LEVEL_SENSITIVE) {
		if (agpio->polarity == ACPI_ACTIVE_HIGH)
			event->irqflags |= IRQF_TRIGGER_HIGH;
		else
			event->irqflags |= IRQF_TRIGGER_LOW;
	} else {
		switch (agpio->polarity) {
		case ACPI_ACTIVE_HIGH:
			event->irqflags |= IRQF_TRIGGER_RISING;
			break;
		case ACPI_ACTIVE_LOW:
			event->irqflags |= IRQF_TRIGGER_FALLING;
			break;
		default:
			event->irqflags |= IRQF_TRIGGER_RISING |
					   IRQF_TRIGGER_FALLING;
			break;
		}
	}

	event->handle = evt_handle;
	event->handler = handler;
	event->irq = irq;
	event->irq_is_wake = acpi_gpio_irq_is_wake(chip->parent, agpio);
	event->pin = pin;
	event->desc = desc;

	list_add_tail(&event->node, &acpi_gpio->events);

	return AE_OK;

fail_unlock_irq:
	gpiochip_unlock_as_irq(chip, pin);
fail_free_desc:
	gpiochip_free_own_desc(desc);

	return AE_OK;
}

/**
 * acpi_gpiochip_request_interrupts() - Register isr for gpio chip ACPI events
 * @chip:      GPIO chip
 *
 * ACPI5 platforms can use GPIO signaled ACPI events. These GPIO interrupts are
 * handled by ACPI event methods which need to be called from the GPIO
 * chip's interrupt handler. acpi_gpiochip_request_interrupts() finds out which
 * GPIO pins have ACPI event methods and assigns interrupt handlers that calls
 * the ACPI event methods for those pins.
 */
void acpi_gpiochip_request_interrupts(struct gpio_chip *chip)
{
	struct acpi_gpio_chip *acpi_gpio;
	acpi_handle handle;
	acpi_status status;
	bool defer;

	if (!chip->parent || !chip->to_irq)
		return;

	handle = ACPI_HANDLE(chip->parent);
	if (!handle)
		return;

	status = acpi_get_data(handle, acpi_gpio_chip_dh, (void **)&acpi_gpio);
	if (ACPI_FAILURE(status))
		return;

	acpi_walk_resources(handle, "_AEI",
			    acpi_gpiochip_alloc_event, acpi_gpio);

	mutex_lock(&acpi_gpio_deferred_req_irqs_lock);
	defer = !acpi_gpio_deferred_req_irqs_done;
	if (defer)
		list_add(&acpi_gpio->deferred_req_irqs_list_entry,
			 &acpi_gpio_deferred_req_irqs_list);
	mutex_unlock(&acpi_gpio_deferred_req_irqs_lock);

	if (defer)
		return;

	acpi_gpiochip_request_irqs(acpi_gpio);
}
EXPORT_SYMBOL_GPL(acpi_gpiochip_request_interrupts);

/**
 * acpi_gpiochip_free_interrupts() - Free GPIO ACPI event interrupts.
 * @chip:      GPIO chip
 *
 * Free interrupts associated with GPIO ACPI event method for the given
 * GPIO chip.
 */
void acpi_gpiochip_free_interrupts(struct gpio_chip *chip)
{
	struct acpi_gpio_chip *acpi_gpio;
	struct acpi_gpio_event *event, *ep;
	acpi_handle handle;
	acpi_status status;

	if (!chip->parent || !chip->to_irq)
		return;

	handle = ACPI_HANDLE(chip->parent);
	if (!handle)
		return;

	status = acpi_get_data(handle, acpi_gpio_chip_dh, (void **)&acpi_gpio);
	if (ACPI_FAILURE(status))
		return;

	mutex_lock(&acpi_gpio_deferred_req_irqs_lock);
	if (!list_empty(&acpi_gpio->deferred_req_irqs_list_entry))
		list_del_init(&acpi_gpio->deferred_req_irqs_list_entry);
	mutex_unlock(&acpi_gpio_deferred_req_irqs_lock);

	list_for_each_entry_safe_reverse(event, ep, &acpi_gpio->events, node) {
		if (event->irq_requested) {
			if (event->irq_is_wake)
				disable_irq_wake(event->irq);

			free_irq(event->irq, event);
		}

		gpiochip_unlock_as_irq(chip, event->pin);
		gpiochip_free_own_desc(event->desc);
		list_del(&event->node);
		kfree(event);
	}
}
EXPORT_SYMBOL_GPL(acpi_gpiochip_free_interrupts);

int acpi_dev_add_driver_gpios(struct acpi_device *adev,
			      const struct acpi_gpio_mapping *gpios)
{
	if (adev && gpios) {
		adev->driver_gpios = gpios;
		return 0;
	}
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(acpi_dev_add_driver_gpios);

void acpi_dev_remove_driver_gpios(struct acpi_device *adev)
{
	if (adev)
		adev->driver_gpios = NULL;
}
EXPORT_SYMBOL_GPL(acpi_dev_remove_driver_gpios);

static void devm_acpi_dev_release_driver_gpios(struct device *dev, void *res)
{
	acpi_dev_remove_driver_gpios(ACPI_COMPANION(dev));
}

int devm_acpi_dev_add_driver_gpios(struct device *dev,
				   const struct acpi_gpio_mapping *gpios)
{
	void *res;
	int ret;

	res = devres_alloc(devm_acpi_dev_release_driver_gpios, 0, GFP_KERNEL);
	if (!res)
		return -ENOMEM;

	ret = acpi_dev_add_driver_gpios(ACPI_COMPANION(dev), gpios);
	if (ret) {
		devres_free(res);
		return ret;
	}
	devres_add(dev, res);
	return 0;
}
EXPORT_SYMBOL_GPL(devm_acpi_dev_add_driver_gpios);

void devm_acpi_dev_remove_driver_gpios(struct device *dev)
{
	WARN_ON(devres_release(dev, devm_acpi_dev_release_driver_gpios, NULL, NULL));
}
EXPORT_SYMBOL_GPL(devm_acpi_dev_remove_driver_gpios);

static bool acpi_get_driver_gpio_data(struct acpi_device *adev,
				      const char *name, int index,
				      struct fwnode_reference_args *args,
				      unsigned int *quirks)
{
	const struct acpi_gpio_mapping *gm;

	if (!adev->driver_gpios)
		return false;

	for (gm = adev->driver_gpios; gm->name; gm++)
		if (!strcmp(name, gm->name) && gm->data && index < gm->size) {
			const struct acpi_gpio_params *par = gm->data + index;

			args->fwnode = acpi_fwnode_handle(adev);
			args->args[0] = par->crs_entry_index;
			args->args[1] = par->line_index;
			args->args[2] = par->active_low;
			args->nargs = 3;

			*quirks = gm->quirks;
			return true;
		}

	return false;
}

static int
__acpi_gpio_update_gpiod_flags(enum gpiod_flags *flags, enum gpiod_flags update)
{
	const enum gpiod_flags mask =
		GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT |
		GPIOD_FLAGS_BIT_DIR_VAL;
	int ret = 0;

	/*
	 * Check if the BIOS has IoRestriction with explicitly set direction
	 * and update @flags accordingly. Otherwise use whatever caller asked
	 * for.
	 */
	if (update & GPIOD_FLAGS_BIT_DIR_SET) {
		enum gpiod_flags diff = *flags ^ update;

		/*
		 * Check if caller supplied incompatible GPIO initialization
		 * flags.
		 *
		 * Return %-EINVAL to notify that firmware has different
		 * settings and we are going to use them.
		 */
		if (((*flags & GPIOD_FLAGS_BIT_DIR_SET) && (diff & GPIOD_FLAGS_BIT_DIR_OUT)) ||
		    ((*flags & GPIOD_FLAGS_BIT_DIR_OUT) && (diff & GPIOD_FLAGS_BIT_DIR_VAL)))
			ret = -EINVAL;
		*flags = (*flags & ~mask) | (update & mask);
	}
	return ret;
}

int
acpi_gpio_update_gpiod_flags(enum gpiod_flags *flags, struct acpi_gpio_info *info)
{
	struct device *dev = &info->adev->dev;
	enum gpiod_flags old = *flags;
	int ret;

	ret = __acpi_gpio_update_gpiod_flags(&old, info->flags);
	if (info->quirks & ACPI_GPIO_QUIRK_NO_IO_RESTRICTION) {
		if (ret)
			dev_warn(dev, FW_BUG "GPIO not in correct mode, fixing\n");
	} else {
		if (ret)
			dev_dbg(dev, "Override GPIO initialization flags\n");
		*flags = old;
	}

	return ret;
}

int acpi_gpio_update_gpiod_lookup_flags(unsigned long *lookupflags,
					struct acpi_gpio_info *info)
{
	switch (info->pin_config) {
	case ACPI_PIN_CONFIG_PULLUP:
		*lookupflags |= GPIO_PULL_UP;
		break;
	case ACPI_PIN_CONFIG_PULLDOWN:
		*lookupflags |= GPIO_PULL_DOWN;
		break;
	default:
		break;
	}

	if (info->polarity == GPIO_ACTIVE_LOW)
		*lookupflags |= GPIO_ACTIVE_LOW;

	return 0;
}

struct acpi_gpio_lookup {
	struct acpi_gpio_info info;
	int index;
	u16 pin_index;
	bool active_low;
	struct gpio_desc *desc;
	int n;
};

static int acpi_populate_gpio_lookup(struct acpi_resource *ares, void *data)
{
	struct acpi_gpio_lookup *lookup = data;

	if (ares->type != ACPI_RESOURCE_TYPE_GPIO)
		return 1;

	if (!lookup->desc) {
		const struct acpi_resource_gpio *agpio = &ares->data.gpio;
		bool gpioint = agpio->connection_type == ACPI_RESOURCE_GPIO_TYPE_INT;
		struct gpio_desc *desc;
		u16 pin_index;

		if (lookup->info.quirks & ACPI_GPIO_QUIRK_ONLY_GPIOIO && gpioint)
			lookup->index++;

		if (lookup->n++ != lookup->index)
			return 1;

		pin_index = lookup->pin_index;
		if (pin_index >= agpio->pin_table_length)
			return 1;

		if (lookup->info.quirks & ACPI_GPIO_QUIRK_ABSOLUTE_NUMBER)
			desc = gpio_to_desc(agpio->pin_table[pin_index]);
		else
			desc = acpi_get_gpiod(agpio->resource_source.string_ptr,
					      agpio->pin_table[pin_index]);
		lookup->desc = desc;
		lookup->info.pin_config = agpio->pin_config;
		lookup->info.debounce = agpio->debounce_timeout;
		lookup->info.gpioint = gpioint;

		/*
		 * Polarity and triggering are only specified for GpioInt
		 * resource.
		 * Note: we expect here:
		 * - ACPI_ACTIVE_LOW == GPIO_ACTIVE_LOW
		 * - ACPI_ACTIVE_HIGH == GPIO_ACTIVE_HIGH
		 */
		if (lookup->info.gpioint) {
			lookup->info.polarity = agpio->polarity;
			lookup->info.triggering = agpio->triggering;
		} else {
			lookup->info.polarity = lookup->active_low;
		}

		lookup->info.flags = acpi_gpio_to_gpiod_flags(agpio, lookup->info.polarity);
	}

	return 1;
}

static int acpi_gpio_resource_lookup(struct acpi_gpio_lookup *lookup,
				     struct acpi_gpio_info *info)
{
	struct acpi_device *adev = lookup->info.adev;
	struct list_head res_list;
	int ret;

	INIT_LIST_HEAD(&res_list);

	ret = acpi_dev_get_resources(adev, &res_list,
				     acpi_populate_gpio_lookup,
				     lookup);
	if (ret < 0)
		return ret;

	acpi_dev_free_resource_list(&res_list);

	if (!lookup->desc)
		return -ENOENT;

	if (info)
		*info = lookup->info;
	return 0;
}

static int acpi_gpio_property_lookup(struct fwnode_handle *fwnode,
				     const char *propname, int index,
				     struct acpi_gpio_lookup *lookup)
{
	struct fwnode_reference_args args;
	unsigned int quirks = 0;
	int ret;

	memset(&args, 0, sizeof(args));
	ret = __acpi_node_get_property_reference(fwnode, propname, index, 3,
						 &args);
	if (ret) {
		struct acpi_device *adev = to_acpi_device_node(fwnode);

		if (!adev)
			return ret;

		if (!acpi_get_driver_gpio_data(adev, propname, index, &args,
					       &quirks))
			return ret;
	}
	/*
	 * The property was found and resolved, so need to lookup the GPIO based
	 * on returned args.
	 */
	if (!to_acpi_device_node(args.fwnode))
		return -EINVAL;
	if (args.nargs != 3)
		return -EPROTO;

	lookup->index = args.args[0];
	lookup->pin_index = args.args[1];
	lookup->active_low = !!args.args[2];

	lookup->info.adev = to_acpi_device_node(args.fwnode);
	lookup->info.quirks = quirks;

	return 0;
}

/**
 * acpi_get_gpiod_by_index() - get a GPIO descriptor from device resources
 * @adev: pointer to a ACPI device to get GPIO from
 * @propname: Property name of the GPIO (optional)
 * @index: index of GpioIo/GpioInt resource (starting from %0)
 * @info: info pointer to fill in (optional)
 *
 * Function goes through ACPI resources for @adev and based on @index looks
 * up a GpioIo/GpioInt resource, translates it to the Linux GPIO descriptor,
 * and returns it. @index matches GpioIo/GpioInt resources only so if there
 * are total %3 GPIO resources, the index goes from %0 to %2.
 *
 * If @propname is specified the GPIO is looked using device property. In
 * that case @index is used to select the GPIO entry in the property value
 * (in case of multiple).
 *
 * If the GPIO cannot be translated or there is an error, an ERR_PTR is
 * returned.
 *
 * Note: if the GPIO resource has multiple entries in the pin list, this
 * function only returns the first.
 */
static struct gpio_desc *acpi_get_gpiod_by_index(struct acpi_device *adev,
					  const char *propname, int index,
					  struct acpi_gpio_info *info)
{
	struct acpi_gpio_lookup lookup;
	int ret;

	if (!adev)
		return ERR_PTR(-ENODEV);

	memset(&lookup, 0, sizeof(lookup));
	lookup.index = index;

	if (propname) {
		dev_dbg(&adev->dev, "GPIO: looking up %s\n", propname);

		ret = acpi_gpio_property_lookup(acpi_fwnode_handle(adev),
						propname, index, &lookup);
		if (ret)
			return ERR_PTR(ret);

		dev_dbg(&adev->dev, "GPIO: _DSD returned %s %d %u %u\n",
			dev_name(&lookup.info.adev->dev), lookup.index,
			lookup.pin_index, lookup.active_low);
	} else {
		dev_dbg(&adev->dev, "GPIO: looking up %d in _CRS\n", index);
		lookup.info.adev = adev;
	}

	ret = acpi_gpio_resource_lookup(&lookup, info);
	return ret ? ERR_PTR(ret) : lookup.desc;
}

static bool acpi_can_fallback_to_crs(struct acpi_device *adev,
				     const char *con_id)
{
	/* Never allow fallback if the device has properties */
	if (acpi_dev_has_props(adev) || adev->driver_gpios)
		return false;

	return con_id == NULL;
}

struct gpio_desc *acpi_find_gpio(struct device *dev,
				 const char *con_id,
				 unsigned int idx,
				 enum gpiod_flags *dflags,
				 unsigned long *lookupflags)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);
	struct acpi_gpio_info info;
	struct gpio_desc *desc;
	char propname[32];
	int i;

	/* Try first from _DSD */
	for (i = 0; i < ARRAY_SIZE(gpio_suffixes); i++) {
		if (con_id) {
			snprintf(propname, sizeof(propname), "%s-%s",
				 con_id, gpio_suffixes[i]);
		} else {
			snprintf(propname, sizeof(propname), "%s",
				 gpio_suffixes[i]);
		}

		desc = acpi_get_gpiod_by_index(adev, propname, idx, &info);
		if (!IS_ERR(desc))
			break;
		if (PTR_ERR(desc) == -EPROBE_DEFER)
			return ERR_CAST(desc);
	}

	/* Then from plain _CRS GPIOs */
	if (IS_ERR(desc)) {
		if (!acpi_can_fallback_to_crs(adev, con_id))
			return ERR_PTR(-ENOENT);

		desc = acpi_get_gpiod_by_index(adev, NULL, idx, &info);
		if (IS_ERR(desc))
			return desc;
	}

	if (info.gpioint &&
	    (*dflags == GPIOD_OUT_LOW || *dflags == GPIOD_OUT_HIGH)) {
		dev_dbg(dev, "refusing GpioInt() entry when doing GPIOD_OUT_* lookup\n");
		return ERR_PTR(-ENOENT);
	}

	acpi_gpio_update_gpiod_flags(dflags, &info);
	acpi_gpio_update_gpiod_lookup_flags(lookupflags, &info);
	return desc;
}

/**
 * acpi_node_get_gpiod() - get a GPIO descriptor from ACPI resources
 * @fwnode: pointer to an ACPI firmware node to get the GPIO information from
 * @propname: Property name of the GPIO
 * @index: index of GpioIo/GpioInt resource (starting from %0)
 * @info: info pointer to fill in (optional)
 *
 * If @fwnode is an ACPI device object, call acpi_get_gpiod_by_index() for it.
 * Otherwise (i.e. it is a data-only non-device object), use the property-based
 * GPIO lookup to get to the GPIO resource with the relevant information and use
 * that to obtain the GPIO descriptor to return.
 *
 * If the GPIO cannot be translated or there is an error an ERR_PTR is
 * returned.
 */
struct gpio_desc *acpi_node_get_gpiod(struct fwnode_handle *fwnode,
				      const char *propname, int index,
				      struct acpi_gpio_info *info)
{
	struct acpi_gpio_lookup lookup;
	struct acpi_device *adev;
	int ret;

	adev = to_acpi_device_node(fwnode);
	if (adev)
		return acpi_get_gpiod_by_index(adev, propname, index, info);

	if (!is_acpi_data_node(fwnode))
		return ERR_PTR(-ENODEV);

	if (!propname)
		return ERR_PTR(-EINVAL);

	memset(&lookup, 0, sizeof(lookup));
	lookup.index = index;

	ret = acpi_gpio_property_lookup(fwnode, propname, index, &lookup);
	if (ret)
		return ERR_PTR(ret);

	ret = acpi_gpio_resource_lookup(&lookup, info);
	return ret ? ERR_PTR(ret) : lookup.desc;
}

/**
 * acpi_dev_gpio_irq_get_by() - Find GpioInt and translate it to Linux IRQ number
 * @adev: pointer to a ACPI device to get IRQ from
 * @name: optional name of GpioInt resource
 * @index: index of GpioInt resource (starting from %0)
 *
 * If the device has one or more GpioInt resources, this function can be
 * used to translate from the GPIO offset in the resource to the Linux IRQ
 * number.
 *
 * The function is idempotent, though each time it runs it will configure GPIO
 * pin direction according to the flags in GpioInt resource.
 *
 * The function takes optional @name parameter. If the resource has a property
 * name, then only those will be taken into account.
 *
 * Return: Linux IRQ number (> %0) on success, negative errno on failure.
 */
int acpi_dev_gpio_irq_get_by(struct acpi_device *adev, const char *name, int index)
{
	int idx, i;
	unsigned int irq_flags;
	int ret;

	for (i = 0, idx = 0; idx <= index; i++) {
		struct acpi_gpio_info info;
		struct gpio_desc *desc;

		desc = acpi_get_gpiod_by_index(adev, name, i, &info);

		/* Ignore -EPROBE_DEFER, it only matters if idx matches */
		if (IS_ERR(desc) && PTR_ERR(desc) != -EPROBE_DEFER)
			return PTR_ERR(desc);

		if (info.gpioint && idx++ == index) {
			unsigned long lflags = GPIO_LOOKUP_FLAGS_DEFAULT;
			enum gpiod_flags dflags = GPIOD_ASIS;
			char label[32];
			int irq;

			if (IS_ERR(desc))
				return PTR_ERR(desc);

			irq = gpiod_to_irq(desc);
			if (irq < 0)
				return irq;

			acpi_gpio_update_gpiod_flags(&dflags, &info);
			acpi_gpio_update_gpiod_lookup_flags(&lflags, &info);

			snprintf(label, sizeof(label), "GpioInt() %d", index);
			ret = gpiod_configure_flags(desc, label, lflags, dflags);
			if (ret < 0)
				return ret;

			ret = gpio_set_debounce_timeout(desc, info.debounce);
			if (ret)
				return ret;

			irq_flags = acpi_dev_get_irq_type(info.triggering,
							  info.polarity);

			/* Set type if specified and different than the current one */
			if (irq_flags != IRQ_TYPE_NONE &&
			    irq_flags != irq_get_trigger_type(irq))
				irq_set_irq_type(irq, irq_flags);

			return irq;
		}

	}
	return -ENOENT;
}
EXPORT_SYMBOL_GPL(acpi_dev_gpio_irq_get_by);

static acpi_status
acpi_gpio_adr_space_handler(u32 function, acpi_physical_address address,
			    u32 bits, u64 *value, void *handler_context,
			    void *region_context)
{
	struct acpi_gpio_chip *achip = region_context;
	struct gpio_chip *chip = achip->chip;
	struct acpi_resource_gpio *agpio;
	struct acpi_resource *ares;
	u16 pin_index = address;
	acpi_status status;
	int length;
	int i;

	status = acpi_buffer_to_resource(achip->conn_info.connection,
					 achip->conn_info.length, &ares);
	if (ACPI_FAILURE(status))
		return status;

	if (WARN_ON(ares->type != ACPI_RESOURCE_TYPE_GPIO)) {
		ACPI_FREE(ares);
		return AE_BAD_PARAMETER;
	}

	agpio = &ares->data.gpio;

	if (WARN_ON(agpio->io_restriction == ACPI_IO_RESTRICT_INPUT &&
	    function == ACPI_WRITE)) {
		ACPI_FREE(ares);
		return AE_BAD_PARAMETER;
	}

	length = min_t(u16, agpio->pin_table_length, pin_index + bits);
	for (i = pin_index; i < length; ++i) {
		int pin = agpio->pin_table[i];
		struct acpi_gpio_connection *conn;
		struct gpio_desc *desc;
		bool found;

		mutex_lock(&achip->conn_lock);

		found = false;
		list_for_each_entry(conn, &achip->conns, node) {
			if (conn->pin == pin) {
				found = true;
				desc = conn->desc;
				break;
			}
		}

		/*
		 * The same GPIO can be shared between operation region and
		 * event but only if the access here is ACPI_READ. In that
		 * case we "borrow" the event GPIO instead.
		 */
		if (!found && agpio->shareable == ACPI_SHARED &&
		     function == ACPI_READ) {
			struct acpi_gpio_event *event;

			list_for_each_entry(event, &achip->events, node) {
				if (event->pin == pin) {
					desc = event->desc;
					found = true;
					break;
				}
			}
		}

		if (!found) {
			desc = acpi_request_own_gpiod(chip, agpio, i, "ACPI:OpRegion");
			if (IS_ERR(desc)) {
				mutex_unlock(&achip->conn_lock);
				status = AE_ERROR;
				goto out;
			}

			conn = kzalloc(sizeof(*conn), GFP_KERNEL);
			if (!conn) {
				gpiochip_free_own_desc(desc);
				mutex_unlock(&achip->conn_lock);
				status = AE_NO_MEMORY;
				goto out;
			}

			conn->pin = pin;
			conn->desc = desc;
			list_add_tail(&conn->node, &achip->conns);
		}

		mutex_unlock(&achip->conn_lock);

		if (function == ACPI_WRITE)
			gpiod_set_raw_value_cansleep(desc, !!(*value & BIT(i)));
		else
			*value |= (u64)gpiod_get_raw_value_cansleep(desc) << i;
	}

out:
	ACPI_FREE(ares);
	return status;
}

static void acpi_gpiochip_request_regions(struct acpi_gpio_chip *achip)
{
	struct gpio_chip *chip = achip->chip;
	acpi_handle handle = ACPI_HANDLE(chip->parent);
	acpi_status status;

	INIT_LIST_HEAD(&achip->conns);
	mutex_init(&achip->conn_lock);
	status = acpi_install_address_space_handler(handle, ACPI_ADR_SPACE_GPIO,
						    acpi_gpio_adr_space_handler,
						    NULL, achip);
	if (ACPI_FAILURE(status))
		dev_err(chip->parent,
		        "Failed to install GPIO OpRegion handler\n");
}

static void acpi_gpiochip_free_regions(struct acpi_gpio_chip *achip)
{
	struct gpio_chip *chip = achip->chip;
	acpi_handle handle = ACPI_HANDLE(chip->parent);
	struct acpi_gpio_connection *conn, *tmp;
	acpi_status status;

	status = acpi_remove_address_space_handler(handle, ACPI_ADR_SPACE_GPIO,
						   acpi_gpio_adr_space_handler);
	if (ACPI_FAILURE(status)) {
		dev_err(chip->parent,
			"Failed to remove GPIO OpRegion handler\n");
		return;
	}

	list_for_each_entry_safe_reverse(conn, tmp, &achip->conns, node) {
		gpiochip_free_own_desc(conn->desc);
		list_del(&conn->node);
		kfree(conn);
	}
}

static struct gpio_desc *
acpi_gpiochip_parse_own_gpio(struct acpi_gpio_chip *achip,
			     struct fwnode_handle *fwnode,
			     const char **name,
			     unsigned long *lflags,
			     enum gpiod_flags *dflags)
{
	struct gpio_chip *chip = achip->chip;
	struct gpio_desc *desc;
	u32 gpios[2];
	int ret;

	*lflags = GPIO_LOOKUP_FLAGS_DEFAULT;
	*dflags = GPIOD_ASIS;
	*name = NULL;

	ret = fwnode_property_read_u32_array(fwnode, "gpios", gpios,
					     ARRAY_SIZE(gpios));
	if (ret < 0)
		return ERR_PTR(ret);

	desc = gpiochip_get_desc(chip, gpios[0]);
	if (IS_ERR(desc))
		return desc;

	if (gpios[1])
		*lflags |= GPIO_ACTIVE_LOW;

	if (fwnode_property_present(fwnode, "input"))
		*dflags |= GPIOD_IN;
	else if (fwnode_property_present(fwnode, "output-low"))
		*dflags |= GPIOD_OUT_LOW;
	else if (fwnode_property_present(fwnode, "output-high"))
		*dflags |= GPIOD_OUT_HIGH;
	else
		return ERR_PTR(-EINVAL);

	fwnode_property_read_string(fwnode, "line-name", name);

	return desc;
}

static void acpi_gpiochip_scan_gpios(struct acpi_gpio_chip *achip)
{
	struct gpio_chip *chip = achip->chip;
	struct fwnode_handle *fwnode;

	device_for_each_child_node(chip->parent, fwnode) {
		unsigned long lflags;
		enum gpiod_flags dflags;
		struct gpio_desc *desc;
		const char *name;
		int ret;

		if (!fwnode_property_present(fwnode, "gpio-hog"))
			continue;

		desc = acpi_gpiochip_parse_own_gpio(achip, fwnode, &name,
						    &lflags, &dflags);
		if (IS_ERR(desc))
			continue;

		ret = gpiod_hog(desc, name, lflags, dflags);
		if (ret) {
			dev_err(chip->parent, "Failed to hog GPIO\n");
			fwnode_handle_put(fwnode);
			return;
		}
	}
}

void acpi_gpiochip_add(struct gpio_chip *chip)
{
	struct acpi_gpio_chip *acpi_gpio;
	acpi_handle handle;
	acpi_status status;

	if (!chip || !chip->parent)
		return;

	handle = ACPI_HANDLE(chip->parent);
	if (!handle)
		return;

	acpi_gpio = kzalloc(sizeof(*acpi_gpio), GFP_KERNEL);
	if (!acpi_gpio) {
		dev_err(chip->parent,
			"Failed to allocate memory for ACPI GPIO chip\n");
		return;
	}

	acpi_gpio->chip = chip;
	INIT_LIST_HEAD(&acpi_gpio->events);
	INIT_LIST_HEAD(&acpi_gpio->deferred_req_irqs_list_entry);

	status = acpi_attach_data(handle, acpi_gpio_chip_dh, acpi_gpio);
	if (ACPI_FAILURE(status)) {
		dev_err(chip->parent, "Failed to attach ACPI GPIO chip\n");
		kfree(acpi_gpio);
		return;
	}

	acpi_gpiochip_request_regions(acpi_gpio);
	acpi_gpiochip_scan_gpios(acpi_gpio);
	acpi_walk_dep_device_list(handle);
}

void acpi_gpiochip_remove(struct gpio_chip *chip)
{
	struct acpi_gpio_chip *acpi_gpio;
	acpi_handle handle;
	acpi_status status;

	if (!chip || !chip->parent)
		return;

	handle = ACPI_HANDLE(chip->parent);
	if (!handle)
		return;

	status = acpi_get_data(handle, acpi_gpio_chip_dh, (void **)&acpi_gpio);
	if (ACPI_FAILURE(status)) {
		dev_warn(chip->parent, "Failed to retrieve ACPI GPIO chip\n");
		return;
	}

	acpi_gpiochip_free_regions(acpi_gpio);

	acpi_detach_data(handle, acpi_gpio_chip_dh);
	kfree(acpi_gpio);
}

void acpi_gpio_dev_init(struct gpio_chip *gc, struct gpio_device *gdev)
{
	/* Set default fwnode to parent's one if present */
	if (gc->parent)
		ACPI_COMPANION_SET(&gdev->dev, ACPI_COMPANION(gc->parent));
}

static int acpi_gpio_package_count(const union acpi_object *obj)
{
	const union acpi_object *element = obj->package.elements;
	const union acpi_object *end = element + obj->package.count;
	unsigned int count = 0;

	while (element < end) {
		switch (element->type) {
		case ACPI_TYPE_LOCAL_REFERENCE:
			element += 3;
			fallthrough;
		case ACPI_TYPE_INTEGER:
			element++;
			count++;
			break;

		default:
			return -EPROTO;
		}
	}

	return count;
}

static int acpi_find_gpio_count(struct acpi_resource *ares, void *data)
{
	unsigned int *count = data;

	if (ares->type == ACPI_RESOURCE_TYPE_GPIO)
		*count += ares->data.gpio.pin_table_length;

	return 1;
}

/**
 * acpi_gpio_count - count the GPIOs associated with a device / function
 * @dev:	GPIO consumer, can be %NULL for system-global GPIOs
 * @con_id:	function within the GPIO consumer
 *
 * Return:
 * The number of GPIOs associated with a device / function or %-ENOENT,
 * if no GPIO has been assigned to the requested function.
 */
int acpi_gpio_count(struct device *dev, const char *con_id)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);
	const union acpi_object *obj;
	const struct acpi_gpio_mapping *gm;
	int count = -ENOENT;
	int ret;
	char propname[32];
	unsigned int i;

	/* Try first from _DSD */
	for (i = 0; i < ARRAY_SIZE(gpio_suffixes); i++) {
		if (con_id)
			snprintf(propname, sizeof(propname), "%s-%s",
				 con_id, gpio_suffixes[i]);
		else
			snprintf(propname, sizeof(propname), "%s",
				 gpio_suffixes[i]);

		ret = acpi_dev_get_property(adev, propname, ACPI_TYPE_ANY,
					    &obj);
		if (ret == 0) {
			if (obj->type == ACPI_TYPE_LOCAL_REFERENCE)
				count = 1;
			else if (obj->type == ACPI_TYPE_PACKAGE)
				count = acpi_gpio_package_count(obj);
		} else if (adev->driver_gpios) {
			for (gm = adev->driver_gpios; gm->name; gm++)
				if (strcmp(propname, gm->name) == 0) {
					count = gm->size;
					break;
				}
		}
		if (count > 0)
			break;
	}

	/* Then from plain _CRS GPIOs */
	if (count < 0) {
		struct list_head resource_list;
		unsigned int crs_count = 0;

		if (!acpi_can_fallback_to_crs(adev, con_id))
			return count;

		INIT_LIST_HEAD(&resource_list);
		acpi_dev_get_resources(adev, &resource_list,
				       acpi_find_gpio_count, &crs_count);
		acpi_dev_free_resource_list(&resource_list);
		if (crs_count > 0)
			count = crs_count;
	}
	return count ? count : -ENOENT;
}

/* Run deferred acpi_gpiochip_request_irqs() */
static int __init acpi_gpio_handle_deferred_request_irqs(void)
{
	struct acpi_gpio_chip *acpi_gpio, *tmp;

	mutex_lock(&acpi_gpio_deferred_req_irqs_lock);
	list_for_each_entry_safe(acpi_gpio, tmp,
				 &acpi_gpio_deferred_req_irqs_list,
				 deferred_req_irqs_list_entry)
		acpi_gpiochip_request_irqs(acpi_gpio);

	acpi_gpio_deferred_req_irqs_done = true;
	mutex_unlock(&acpi_gpio_deferred_req_irqs_lock);

	return 0;
}
/* We must use _sync so that this runs after the first deferred_probe run */
late_initcall_sync(acpi_gpio_handle_deferred_request_irqs);

static const struct dmi_system_id gpiolib_acpi_quirks[] __initconst = {
	{
		/*
		 * The Minix Neo Z83-4 has a micro-USB-B id-pin handler for
		 * a non existing micro-USB-B connector which puts the HDMI
		 * DDC pins in GPIO mode, breaking HDMI support.
		 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "MINIX"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Z83-4"),
		},
		.driver_data = &(struct acpi_gpiolib_dmi_quirk) {
			.no_edge_events_on_boot = true,
		},
	},
	{
		/*
		 * The Terra Pad 1061 has a micro-USB-B id-pin handler, which
		 * instead of controlling the actual micro-USB-B turns the 5V
		 * boost for its USB-A connector off. The actual micro-USB-B
		 * connector is wired for charging only.
		 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Wortmann_AG"),
			DMI_MATCH(DMI_PRODUCT_NAME, "TERRA_PAD_1061"),
		},
		.driver_data = &(struct acpi_gpiolib_dmi_quirk) {
			.no_edge_events_on_boot = true,
		},
	},
	{
		/*
		 * The Dell Venue 10 Pro 5055, with Bay Trail SoC + TI PMIC uses an
		 * external embedded-controller connected via I2C + an ACPI GPIO
		 * event handler on INT33FFC:02 pin 12, causing spurious wakeups.
		 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Venue 10 Pro 5055"),
		},
		.driver_data = &(struct acpi_gpiolib_dmi_quirk) {
			.ignore_wake = "INT33FC:02@12",
		},
	},
	{
		/*
		 * HP X2 10 models with Cherry Trail SoC + TI PMIC use an
		 * external embedded-controller connected via I2C + an ACPI GPIO
		 * event handler on INT33FF:01 pin 0, causing spurious wakeups.
		 * When suspending by closing the LID, the power to the USB
		 * keyboard is turned off, causing INT0002 ACPI events to
		 * trigger once the XHCI controller notices the keyboard is
		 * gone. So INT0002 events cause spurious wakeups too. Ignoring
		 * EC wakes breaks wakeup when opening the lid, the user needs
		 * to press the power-button to wakeup the system. The
		 * alternative is suspend simply not working, which is worse.
		 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "HP"),
			DMI_MATCH(DMI_PRODUCT_NAME, "HP x2 Detachable 10-p0XX"),
		},
		.driver_data = &(struct acpi_gpiolib_dmi_quirk) {
			.ignore_wake = "INT33FF:01@0,INT0002:00@2",
		},
	},
	{
		/*
		 * HP X2 10 models with Bay Trail SoC + AXP288 PMIC use an
		 * external embedded-controller connected via I2C + an ACPI GPIO
		 * event handler on INT33FC:02 pin 28, causing spurious wakeups.
		 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Hewlett-Packard"),
			DMI_MATCH(DMI_PRODUCT_NAME, "HP Pavilion x2 Detachable"),
			DMI_MATCH(DMI_BOARD_NAME, "815D"),
		},
		.driver_data = &(struct acpi_gpiolib_dmi_quirk) {
			.ignore_wake = "INT33FC:02@28",
		},
	},
	{
		/*
		 * HP X2 10 models with Cherry Trail SoC + AXP288 PMIC use an
		 * external embedded-controller connected via I2C + an ACPI GPIO
		 * event handler on INT33FF:01 pin 0, causing spurious wakeups.
		 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "HP"),
			DMI_MATCH(DMI_PRODUCT_NAME, "HP Pavilion x2 Detachable"),
			DMI_MATCH(DMI_BOARD_NAME, "813E"),
		},
		.driver_data = &(struct acpi_gpiolib_dmi_quirk) {
			.ignore_wake = "INT33FF:01@0",
		},
	},
	{} /* Terminating entry */
};

static int __init acpi_gpio_setup_params(void)
{
	const struct acpi_gpiolib_dmi_quirk *quirk = NULL;
	const struct dmi_system_id *id;

	id = dmi_first_match(gpiolib_acpi_quirks);
	if (id)
		quirk = id->driver_data;

	if (run_edge_events_on_boot < 0) {
		if (quirk && quirk->no_edge_events_on_boot)
			run_edge_events_on_boot = 0;
		else
			run_edge_events_on_boot = 1;
	}

	if (ignore_wake == NULL && quirk && quirk->ignore_wake)
		ignore_wake = quirk->ignore_wake;

	return 0;
}

/* Directly after dmi_setup() which runs as core_initcall() */
postcore_initcall(acpi_gpio_setup_params);
