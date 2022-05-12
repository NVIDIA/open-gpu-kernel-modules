/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GPIO_DRIVER_H
#define __LINUX_GPIO_DRIVER_H

#include <linux/device.h>
#include <linux/types.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/lockdep.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf-generic.h>

struct gpio_desc;
struct of_phandle_args;
struct device_node;
struct seq_file;
struct gpio_device;
struct module;
enum gpiod_flags;
enum gpio_lookup_flags;

struct gpio_chip;

#define GPIO_LINE_DIRECTION_IN	1
#define GPIO_LINE_DIRECTION_OUT	0

/**
 * struct gpio_irq_chip - GPIO interrupt controller
 */
struct gpio_irq_chip {
	/**
	 * @chip:
	 *
	 * GPIO IRQ chip implementation, provided by GPIO driver.
	 */
	struct irq_chip *chip;

	/**
	 * @domain:
	 *
	 * Interrupt translation domain; responsible for mapping between GPIO
	 * hwirq number and Linux IRQ number.
	 */
	struct irq_domain *domain;

	/**
	 * @domain_ops:
	 *
	 * Table of interrupt domain operations for this IRQ chip.
	 */
	const struct irq_domain_ops *domain_ops;

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
	/**
	 * @fwnode:
	 *
	 * Firmware node corresponding to this gpiochip/irqchip, necessary
	 * for hierarchical irqdomain support.
	 */
	struct fwnode_handle *fwnode;

	/**
	 * @parent_domain:
	 *
	 * If non-NULL, will be set as the parent of this GPIO interrupt
	 * controller's IRQ domain to establish a hierarchical interrupt
	 * domain. The presence of this will activate the hierarchical
	 * interrupt support.
	 */
	struct irq_domain *parent_domain;

	/**
	 * @child_to_parent_hwirq:
	 *
	 * This callback translates a child hardware IRQ offset to a parent
	 * hardware IRQ offset on a hierarchical interrupt chip. The child
	 * hardware IRQs correspond to the GPIO index 0..ngpio-1 (see the
	 * ngpio field of struct gpio_chip) and the corresponding parent
	 * hardware IRQ and type (such as IRQ_TYPE_*) shall be returned by
	 * the driver. The driver can calculate this from an offset or using
	 * a lookup table or whatever method is best for this chip. Return
	 * 0 on successful translation in the driver.
	 *
	 * If some ranges of hardware IRQs do not have a corresponding parent
	 * HWIRQ, return -EINVAL, but also make sure to fill in @valid_mask and
	 * @need_valid_mask to make these GPIO lines unavailable for
	 * translation.
	 */
	int (*child_to_parent_hwirq)(struct gpio_chip *gc,
				     unsigned int child_hwirq,
				     unsigned int child_type,
				     unsigned int *parent_hwirq,
				     unsigned int *parent_type);

	/**
	 * @populate_parent_alloc_arg :
	 *
	 * This optional callback allocates and populates the specific struct
	 * for the parent's IRQ domain. If this is not specified, then
	 * &gpiochip_populate_parent_fwspec_twocell will be used. A four-cell
	 * variant named &gpiochip_populate_parent_fwspec_fourcell is also
	 * available.
	 */
	void *(*populate_parent_alloc_arg)(struct gpio_chip *gc,
				       unsigned int parent_hwirq,
				       unsigned int parent_type);

	/**
	 * @child_offset_to_irq:
	 *
	 * This optional callback is used to translate the child's GPIO line
	 * offset on the GPIO chip to an IRQ number for the GPIO to_irq()
	 * callback. If this is not specified, then a default callback will be
	 * provided that returns the line offset.
	 */
	unsigned int (*child_offset_to_irq)(struct gpio_chip *gc,
					    unsigned int pin);

	/**
	 * @child_irq_domain_ops:
	 *
	 * The IRQ domain operations that will be used for this GPIO IRQ
	 * chip. If no operations are provided, then default callbacks will
	 * be populated to setup the IRQ hierarchy. Some drivers need to
	 * supply their own translate function.
	 */
	struct irq_domain_ops child_irq_domain_ops;
#endif

	/**
	 * @handler:
	 *
	 * The IRQ handler to use (often a predefined IRQ core function) for
	 * GPIO IRQs, provided by GPIO driver.
	 */
	irq_flow_handler_t handler;

	/**
	 * @default_type:
	 *
	 * Default IRQ triggering type applied during GPIO driver
	 * initialization, provided by GPIO driver.
	 */
	unsigned int default_type;

	/**
	 * @lock_key:
	 *
	 * Per GPIO IRQ chip lockdep class for IRQ lock.
	 */
	struct lock_class_key *lock_key;

	/**
	 * @request_key:
	 *
	 * Per GPIO IRQ chip lockdep class for IRQ request.
	 */
	struct lock_class_key *request_key;

	/**
	 * @parent_handler:
	 *
	 * The interrupt handler for the GPIO chip's parent interrupts, may be
	 * NULL if the parent interrupts are nested rather than cascaded.
	 */
	irq_flow_handler_t parent_handler;

	/**
	 * @parent_handler_data:
	 *
	 * Data associated, and passed to, the handler for the parent
	 * interrupt.
	 */
	void *parent_handler_data;

	/**
	 * @num_parents:
	 *
	 * The number of interrupt parents of a GPIO chip.
	 */
	unsigned int num_parents;

	/**
	 * @parents:
	 *
	 * A list of interrupt parents of a GPIO chip. This is owned by the
	 * driver, so the core will only reference this list, not modify it.
	 */
	unsigned int *parents;

	/**
	 * @map:
	 *
	 * A list of interrupt parents for each line of a GPIO chip.
	 */
	unsigned int *map;

	/**
	 * @threaded:
	 *
	 * True if set the interrupt handling uses nested threads.
	 */
	bool threaded;

	/**
	 * @init_hw: optional routine to initialize hardware before
	 * an IRQ chip will be added. This is quite useful when
	 * a particular driver wants to clear IRQ related registers
	 * in order to avoid undesired events.
	 */
	int (*init_hw)(struct gpio_chip *gc);

	/**
	 * @init_valid_mask: optional routine to initialize @valid_mask, to be
	 * used if not all GPIO lines are valid interrupts. Sometimes some
	 * lines just cannot fire interrupts, and this routine, when defined,
	 * is passed a bitmap in "valid_mask" and it will have ngpios
	 * bits from 0..(ngpios-1) set to "1" as in valid. The callback can
	 * then directly set some bits to "0" if they cannot be used for
	 * interrupts.
	 */
	void (*init_valid_mask)(struct gpio_chip *gc,
				unsigned long *valid_mask,
				unsigned int ngpios);

	/**
	 * @valid_mask:
	 *
	 * If not %NULL, holds bitmask of GPIOs which are valid to be included
	 * in IRQ domain of the chip.
	 */
	unsigned long *valid_mask;

	/**
	 * @first:
	 *
	 * Required for static IRQ allocation. If set, irq_domain_add_simple()
	 * will allocate and map all IRQs during initialization.
	 */
	unsigned int first;

	/**
	 * @irq_enable:
	 *
	 * Store old irq_chip irq_enable callback
	 */
	void		(*irq_enable)(struct irq_data *data);

	/**
	 * @irq_disable:
	 *
	 * Store old irq_chip irq_disable callback
	 */
	void		(*irq_disable)(struct irq_data *data);
	/**
	 * @irq_unmask:
	 *
	 * Store old irq_chip irq_unmask callback
	 */
	void		(*irq_unmask)(struct irq_data *data);

	/**
	 * @irq_mask:
	 *
	 * Store old irq_chip irq_mask callback
	 */
	void		(*irq_mask)(struct irq_data *data);
};

/**
 * struct gpio_chip - abstract a GPIO controller
 * @label: a functional name for the GPIO device, such as a part
 *	number or the name of the SoC IP-block implementing it.
 * @gpiodev: the internal state holder, opaque struct
 * @parent: optional parent device providing the GPIOs
 * @owner: helps prevent removal of modules exporting active GPIOs
 * @request: optional hook for chip-specific activation, such as
 *	enabling module power and clock; may sleep
 * @free: optional hook for chip-specific deactivation, such as
 *	disabling module power and clock; may sleep
 * @get_direction: returns direction for signal "offset", 0=out, 1=in,
 *	(same as GPIO_LINE_DIRECTION_OUT / GPIO_LINE_DIRECTION_IN),
 *	or negative error. It is recommended to always implement this
 *	function, even on input-only or output-only gpio chips.
 * @direction_input: configures signal "offset" as input, or returns error
 *	This can be omitted on input-only or output-only gpio chips.
 * @direction_output: configures signal "offset" as output, or returns error
 *	This can be omitted on input-only or output-only gpio chips.
 * @get: returns value for signal "offset", 0=low, 1=high, or negative error
 * @get_multiple: reads values for multiple signals defined by "mask" and
 *	stores them in "bits", returns 0 on success or negative error
 * @set: assigns output value for signal "offset"
 * @set_multiple: assigns output values for multiple signals defined by "mask"
 * @set_config: optional hook for all kinds of settings. Uses the same
 *	packed config format as generic pinconf.
 * @to_irq: optional hook supporting non-static gpio_to_irq() mappings;
 *	implementation may not sleep
 * @dbg_show: optional routine to show contents in debugfs; default code
 *	will be used when this is omitted, but custom code can show extra
 *	state (such as pullup/pulldown configuration).
 * @init_valid_mask: optional routine to initialize @valid_mask, to be used if
 *	not all GPIOs are valid.
 * @add_pin_ranges: optional routine to initialize pin ranges, to be used when
 *	requires special mapping of the pins that provides GPIO functionality.
 *	It is called after adding GPIO chip and before adding IRQ chip.
 * @base: identifies the first GPIO number handled by this chip;
 *	or, if negative during registration, requests dynamic ID allocation.
 *	DEPRECATION: providing anything non-negative and nailing the base
 *	offset of GPIO chips is deprecated. Please pass -1 as base to
 *	let gpiolib select the chip base in all possible cases. We want to
 *	get rid of the static GPIO number space in the long run.
 * @ngpio: the number of GPIOs handled by this controller; the last GPIO
 *	handled is (base + ngpio - 1).
 * @names: if set, must be an array of strings to use as alternative
 *      names for the GPIOs in this chip. Any entry in the array
 *      may be NULL if there is no alias for the GPIO, however the
 *      array must be @ngpio entries long.  A name can include a single printk
 *      format specifier for an unsigned int.  It is substituted by the actual
 *      number of the gpio.
 * @can_sleep: flag must be set iff get()/set() methods sleep, as they
 *	must while accessing GPIO expander chips over I2C or SPI. This
 *	implies that if the chip supports IRQs, these IRQs need to be threaded
 *	as the chip access may sleep when e.g. reading out the IRQ status
 *	registers.
 * @read_reg: reader function for generic GPIO
 * @write_reg: writer function for generic GPIO
 * @be_bits: if the generic GPIO has big endian bit order (bit 31 is representing
 *	line 0, bit 30 is line 1 ... bit 0 is line 31) this is set to true by the
 *	generic GPIO core. It is for internal housekeeping only.
 * @reg_dat: data (in) register for generic GPIO
 * @reg_set: output set register (out=high) for generic GPIO
 * @reg_clr: output clear register (out=low) for generic GPIO
 * @reg_dir_out: direction out setting register for generic GPIO
 * @reg_dir_in: direction in setting register for generic GPIO
 * @bgpio_dir_unreadable: indicates that the direction register(s) cannot
 *	be read and we need to rely on out internal state tracking.
 * @bgpio_bits: number of register bits used for a generic GPIO i.e.
 *	<register width> * 8
 * @bgpio_lock: used to lock chip->bgpio_data. Also, this is needed to keep
 *	shadowed and real data registers writes together.
 * @bgpio_data:	shadowed data register for generic GPIO to clear/set bits
 *	safely.
 * @bgpio_dir: shadowed direction register for generic GPIO to clear/set
 *	direction safely. A "1" in this word means the line is set as
 *	output.
 *
 * A gpio_chip can help platforms abstract various sources of GPIOs so
 * they can all be accessed through a common programming interface.
 * Example sources would be SOC controllers, FPGAs, multifunction
 * chips, dedicated GPIO expanders, and so on.
 *
 * Each chip controls a number of signals, identified in method calls
 * by "offset" values in the range 0..(@ngpio - 1).  When those signals
 * are referenced through calls like gpio_get_value(gpio), the offset
 * is calculated by subtracting @base from the gpio number.
 */
struct gpio_chip {
	const char		*label;
	struct gpio_device	*gpiodev;
	struct device		*parent;
	struct module		*owner;

	int			(*request)(struct gpio_chip *gc,
						unsigned int offset);
	void			(*free)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*get_direction)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*direction_input)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*direction_output)(struct gpio_chip *gc,
						unsigned int offset, int value);
	int			(*get)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*get_multiple)(struct gpio_chip *gc,
						unsigned long *mask,
						unsigned long *bits);
	void			(*set)(struct gpio_chip *gc,
						unsigned int offset, int value);
	void			(*set_multiple)(struct gpio_chip *gc,
						unsigned long *mask,
						unsigned long *bits);
	int			(*set_config)(struct gpio_chip *gc,
					      unsigned int offset,
					      unsigned long config);
	int			(*to_irq)(struct gpio_chip *gc,
						unsigned int offset);

	void			(*dbg_show)(struct seq_file *s,
						struct gpio_chip *gc);

	int			(*init_valid_mask)(struct gpio_chip *gc,
						   unsigned long *valid_mask,
						   unsigned int ngpios);

	int			(*add_pin_ranges)(struct gpio_chip *gc);

	int			base;
	u16			ngpio;
	const char		*const *names;
	bool			can_sleep;

#if IS_ENABLED(CONFIG_GPIO_GENERIC)
	unsigned long (*read_reg)(void __iomem *reg);
	void (*write_reg)(void __iomem *reg, unsigned long data);
	bool be_bits;
	void __iomem *reg_dat;
	void __iomem *reg_set;
	void __iomem *reg_clr;
	void __iomem *reg_dir_out;
	void __iomem *reg_dir_in;
	bool bgpio_dir_unreadable;
	int bgpio_bits;
	spinlock_t bgpio_lock;
	unsigned long bgpio_data;
	unsigned long bgpio_dir;
#endif /* CONFIG_GPIO_GENERIC */

#ifdef CONFIG_GPIOLIB_IRQCHIP
	/*
	 * With CONFIG_GPIOLIB_IRQCHIP we get an irqchip inside the gpiolib
	 * to handle IRQs for most practical cases.
	 */

	/**
	 * @irq:
	 *
	 * Integrates interrupt chip functionality with the GPIO chip. Can be
	 * used to handle IRQs for most practical cases.
	 */
	struct gpio_irq_chip irq;
#endif /* CONFIG_GPIOLIB_IRQCHIP */

	/**
	 * @valid_mask:
	 *
	 * If not %NULL, holds bitmask of GPIOs which are valid to be used
	 * from the chip.
	 */
	unsigned long *valid_mask;

#if defined(CONFIG_OF_GPIO)
	/*
	 * If CONFIG_OF_GPIO is enabled, then all GPIO controllers described in
	 * the device tree automatically may have an OF translation
	 */

	/**
	 * @of_node:
	 *
	 * Pointer to a device tree node representing this GPIO controller.
	 */
	struct device_node *of_node;

	/**
	 * @of_gpio_n_cells:
	 *
	 * Number of cells used to form the GPIO specifier.
	 */
	unsigned int of_gpio_n_cells;

	/**
	 * @of_xlate:
	 *
	 * Callback to translate a device tree GPIO specifier into a chip-
	 * relative GPIO number and flags.
	 */
	int (*of_xlate)(struct gpio_chip *gc,
			const struct of_phandle_args *gpiospec, u32 *flags);
#endif /* CONFIG_OF_GPIO */
};

extern const char *gpiochip_is_requested(struct gpio_chip *gc,
			unsigned int offset);

/**
 * for_each_requested_gpio_in_range - iterates over requested GPIOs in a given range
 * @chip:	the chip to query
 * @i:		loop variable
 * @base:	first GPIO in the range
 * @size:	amount of GPIOs to check starting from @base
 * @label:	label of current GPIO
 */
#define for_each_requested_gpio_in_range(chip, i, base, size, label)			\
	for (i = 0; i < size; i++)							\
		if ((label = gpiochip_is_requested(chip, base + i)) == NULL) {} else

/* Iterates over all requested GPIO of the given @chip */
#define for_each_requested_gpio(chip, i, label)						\
	for_each_requested_gpio_in_range(chip, i, 0, chip->ngpio, label)

/* add/remove chips */
extern int gpiochip_add_data_with_key(struct gpio_chip *gc, void *data,
				      struct lock_class_key *lock_key,
				      struct lock_class_key *request_key);

/**
 * gpiochip_add_data() - register a gpio_chip
 * @gc: the chip to register, with gc->base initialized
 * @data: driver-private data associated with this chip
 *
 * Context: potentially before irqs will work
 *
 * When gpiochip_add_data() is called very early during boot, so that GPIOs
 * can be freely used, the gc->parent device must be registered before
 * the gpio framework's arch_initcall().  Otherwise sysfs initialization
 * for GPIOs will fail rudely.
 *
 * gpiochip_add_data() must only be called after gpiolib initialization,
 * i.e. after core_initcall().
 *
 * If gc->base is negative, this requests dynamic assignment of
 * a range of valid GPIOs.
 *
 * Returns:
 * A negative errno if the chip can't be registered, such as because the
 * gc->base is invalid or already associated with a different chip.
 * Otherwise it returns zero as a success code.
 */
#ifdef CONFIG_LOCKDEP
#define gpiochip_add_data(gc, data) ({		\
		static struct lock_class_key lock_key;	\
		static struct lock_class_key request_key;	  \
		gpiochip_add_data_with_key(gc, data, &lock_key, \
					   &request_key);	  \
	})
#define devm_gpiochip_add_data(dev, gc, data) ({ \
		static struct lock_class_key lock_key;	\
		static struct lock_class_key request_key;	  \
		devm_gpiochip_add_data_with_key(dev, gc, data, &lock_key, \
					   &request_key);	  \
	})
#else
#define gpiochip_add_data(gc, data) gpiochip_add_data_with_key(gc, data, NULL, NULL)
#define devm_gpiochip_add_data(dev, gc, data) \
	devm_gpiochip_add_data_with_key(dev, gc, data, NULL, NULL)
#endif /* CONFIG_LOCKDEP */

static inline int gpiochip_add(struct gpio_chip *gc)
{
	return gpiochip_add_data(gc, NULL);
}
extern void gpiochip_remove(struct gpio_chip *gc);
extern int devm_gpiochip_add_data_with_key(struct device *dev, struct gpio_chip *gc, void *data,
					   struct lock_class_key *lock_key,
					   struct lock_class_key *request_key);

extern struct gpio_chip *gpiochip_find(void *data,
			      int (*match)(struct gpio_chip *gc, void *data));

bool gpiochip_line_is_irq(struct gpio_chip *gc, unsigned int offset);
int gpiochip_reqres_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_relres_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_disable_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_enable_irq(struct gpio_chip *gc, unsigned int offset);

/* Line status inquiry for drivers */
bool gpiochip_line_is_open_drain(struct gpio_chip *gc, unsigned int offset);
bool gpiochip_line_is_open_source(struct gpio_chip *gc, unsigned int offset);

/* Sleep persistence inquiry for drivers */
bool gpiochip_line_is_persistent(struct gpio_chip *gc, unsigned int offset);
bool gpiochip_line_is_valid(const struct gpio_chip *gc, unsigned int offset);

/* get driver data */
void *gpiochip_get_data(struct gpio_chip *gc);

struct bgpio_pdata {
	const char *label;
	int base;
	int ngpio;
};

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY

void *gpiochip_populate_parent_fwspec_twocell(struct gpio_chip *gc,
					     unsigned int parent_hwirq,
					     unsigned int parent_type);
void *gpiochip_populate_parent_fwspec_fourcell(struct gpio_chip *gc,
					      unsigned int parent_hwirq,
					      unsigned int parent_type);

#else

static inline void *gpiochip_populate_parent_fwspec_twocell(struct gpio_chip *gc,
						    unsigned int parent_hwirq,
						    unsigned int parent_type)
{
	return NULL;
}

static inline void *gpiochip_populate_parent_fwspec_fourcell(struct gpio_chip *gc,
						     unsigned int parent_hwirq,
						     unsigned int parent_type)
{
	return NULL;
}

#endif /* CONFIG_IRQ_DOMAIN_HIERARCHY */

int bgpio_init(struct gpio_chip *gc, struct device *dev,
	       unsigned long sz, void __iomem *dat, void __iomem *set,
	       void __iomem *clr, void __iomem *dirout, void __iomem *dirin,
	       unsigned long flags);

#define BGPIOF_BIG_ENDIAN		BIT(0)
#define BGPIOF_UNREADABLE_REG_SET	BIT(1) /* reg_set is unreadable */
#define BGPIOF_UNREADABLE_REG_DIR	BIT(2) /* reg_dir is unreadable */
#define BGPIOF_BIG_ENDIAN_BYTE_ORDER	BIT(3)
#define BGPIOF_READ_OUTPUT_REG_SET	BIT(4) /* reg_set stores output value */
#define BGPIOF_NO_OUTPUT		BIT(5) /* only input */
#define BGPIOF_NO_SET_ON_INPUT		BIT(6)

int gpiochip_irq_map(struct irq_domain *d, unsigned int irq,
		     irq_hw_number_t hwirq);
void gpiochip_irq_unmap(struct irq_domain *d, unsigned int irq);

int gpiochip_irq_domain_activate(struct irq_domain *domain,
				 struct irq_data *data, bool reserve);
void gpiochip_irq_domain_deactivate(struct irq_domain *domain,
				    struct irq_data *data);

bool gpiochip_irqchip_irq_valid(const struct gpio_chip *gc,
				unsigned int offset);

#ifdef CONFIG_GPIOLIB_IRQCHIP
int gpiochip_irqchip_add_domain(struct gpio_chip *gc,
				struct irq_domain *domain);
#else
static inline int gpiochip_irqchip_add_domain(struct gpio_chip *gc,
					      struct irq_domain *domain)
{
	WARN_ON(1);
	return -EINVAL;
}
#endif

int gpiochip_generic_request(struct gpio_chip *gc, unsigned int offset);
void gpiochip_generic_free(struct gpio_chip *gc, unsigned int offset);
int gpiochip_generic_config(struct gpio_chip *gc, unsigned int offset,
			    unsigned long config);

/**
 * struct gpio_pin_range - pin range controlled by a gpio chip
 * @node: list for maintaining set of pin ranges, used internally
 * @pctldev: pinctrl device which handles corresponding pins
 * @range: actual range of pins controlled by a gpio controller
 */
struct gpio_pin_range {
	struct list_head node;
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range range;
};

#ifdef CONFIG_PINCTRL

int gpiochip_add_pin_range(struct gpio_chip *gc, const char *pinctl_name,
			   unsigned int gpio_offset, unsigned int pin_offset,
			   unsigned int npins);
int gpiochip_add_pingroup_range(struct gpio_chip *gc,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group);
void gpiochip_remove_pin_ranges(struct gpio_chip *gc);

#else /* ! CONFIG_PINCTRL */

static inline int
gpiochip_add_pin_range(struct gpio_chip *gc, const char *pinctl_name,
		       unsigned int gpio_offset, unsigned int pin_offset,
		       unsigned int npins)
{
	return 0;
}
static inline int
gpiochip_add_pingroup_range(struct gpio_chip *gc,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group)
{
	return 0;
}

static inline void
gpiochip_remove_pin_ranges(struct gpio_chip *gc)
{
}

#endif /* CONFIG_PINCTRL */

struct gpio_desc *gpiochip_request_own_desc(struct gpio_chip *gc,
					    unsigned int hwnum,
					    const char *label,
					    enum gpio_lookup_flags lflags,
					    enum gpiod_flags dflags);
void gpiochip_free_own_desc(struct gpio_desc *desc);

#ifdef CONFIG_GPIOLIB

/* lock/unlock as IRQ */
int gpiochip_lock_as_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_unlock_as_irq(struct gpio_chip *gc, unsigned int offset);


struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc);

#else /* CONFIG_GPIOLIB */

static inline struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc)
{
	/* GPIO can never have been requested */
	WARN_ON(1);
	return ERR_PTR(-ENODEV);
}

static inline int gpiochip_lock_as_irq(struct gpio_chip *gc,
				       unsigned int offset)
{
	WARN_ON(1);
	return -EINVAL;
}

static inline void gpiochip_unlock_as_irq(struct gpio_chip *gc,
					  unsigned int offset)
{
	WARN_ON(1);
}
#endif /* CONFIG_GPIOLIB */

#endif /* __LINUX_GPIO_DRIVER_H */
