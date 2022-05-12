====================
Kernel driver lp855x
====================

Backlight driver for LP855x ICs

Supported chips:

	Texas Instruments LP8550, LP8551, LP8552, LP8553, LP8555, LP8556 and
	LP8557

Author: Milo(Woogyom) Kim <milo.kim@ti.com>

Description
-----------

* Brightness control

  Brightness can be controlled by the pwm input or the i2c command.
  The lp855x driver supports both cases.

* Device attributes

  1) bl_ctl_mode

  Backlight control mode.

  Value: pwm based or register based

  2) chip_id

  The lp855x chip id.

  Value: lp8550/lp8551/lp8552/lp8553/lp8555/lp8556/lp8557

Platform data for lp855x
------------------------

For supporting platform specific data, the lp855x platform data can be used.

* name:
	Backlight driver name. If it is not defined, default name is set.
* device_control:
	Value of DEVICE CONTROL register.
* initial_brightness:
	Initial value of backlight brightness.
* period_ns:
	Platform specific PWM period value. unit is nano.
	Only valid when brightness is pwm input mode.
* size_program:
	Total size of lp855x_rom_data.
* rom_data:
	List of new eeprom/eprom registers.

Examples
========

1) lp8552 platform data: i2c register mode with new eeprom data::

    #define EEPROM_A5_ADDR	0xA5
    #define EEPROM_A5_VAL	0x4f	/* EN_VSYNC=0 */

    static struct lp855x_rom_data lp8552_eeprom_arr[] = {
	{EEPROM_A5_ADDR, EEPROM_A5_VAL},
    };

    static struct lp855x_platform_data lp8552_pdata = {
	.name = "lcd-bl",
	.device_control = I2C_CONFIG(LP8552),
	.initial_brightness = INITIAL_BRT,
	.size_program = ARRAY_SIZE(lp8552_eeprom_arr),
	.rom_data = lp8552_eeprom_arr,
    };

2) lp8556 platform data: pwm input mode with default rom data::

    static struct lp855x_platform_data lp8556_pdata = {
	.device_control = PWM_CONFIG(LP8556),
	.initial_brightness = INITIAL_BRT,
	.period_ns = 1000000,
    };
