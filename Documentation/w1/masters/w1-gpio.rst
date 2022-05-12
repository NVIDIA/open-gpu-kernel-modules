=====================
Kernel driver w1-gpio
=====================

Author: Ville Syrjala <syrjala@sci.fi>


Description
-----------

GPIO 1-wire bus master driver. The driver uses the GPIO API to control the
wire and the GPIO pin can be specified using GPIO machine descriptor tables.
It is also possible to define the master using device tree, see
Documentation/devicetree/bindings/w1/w1-gpio.txt


Example (mach-at91)
-------------------

::

  #include <linux/gpio/machine.h>
  #include <linux/w1-gpio.h>

  static struct gpiod_lookup_table foo_w1_gpiod_table = {
	.dev_id = "w1-gpio",
	.table = {
		GPIO_LOOKUP_IDX("at91-gpio", AT91_PIN_PB20, NULL, 0,
			GPIO_ACTIVE_HIGH|GPIO_OPEN_DRAIN),
	},
  };

  static struct w1_gpio_platform_data foo_w1_gpio_pdata = {
	.ext_pullup_enable_pin	= -EINVAL,
  };

  static struct platform_device foo_w1_device = {
	.name			= "w1-gpio",
	.id			= -1,
	.dev.platform_data	= &foo_w1_gpio_pdata,
  };

  ...
	at91_set_GPIO_periph(foo_w1_gpio_pdata.pin, 1);
	at91_set_multi_drive(foo_w1_gpio_pdata.pin, 1);
	gpiod_add_lookup_table(&foo_w1_gpiod_table);
	platform_device_register(&foo_w1_device);
