// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Gas Gauge driver for SBS Compliant Batteries
 *
 * Copyright (c) 2010, NVIDIA Corporation.
 */

#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/devm-helpers.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/of_device.h>
#include <linux/power/sbs-battery.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include <linux/stat.h>

enum {
	REG_MANUFACTURER_DATA,
	REG_BATTERY_MODE,
	REG_TEMPERATURE,
	REG_VOLTAGE,
	REG_CURRENT_NOW,
	REG_CURRENT_AVG,
	REG_MAX_ERR,
	REG_CAPACITY,
	REG_TIME_TO_EMPTY,
	REG_TIME_TO_FULL,
	REG_STATUS,
	REG_CAPACITY_LEVEL,
	REG_CYCLE_COUNT,
	REG_SERIAL_NUMBER,
	REG_REMAINING_CAPACITY,
	REG_REMAINING_CAPACITY_CHARGE,
	REG_FULL_CHARGE_CAPACITY,
	REG_FULL_CHARGE_CAPACITY_CHARGE,
	REG_DESIGN_CAPACITY,
	REG_DESIGN_CAPACITY_CHARGE,
	REG_DESIGN_VOLTAGE_MIN,
	REG_DESIGN_VOLTAGE_MAX,
	REG_CHEMISTRY,
	REG_MANUFACTURER,
	REG_MODEL_NAME,
	REG_CHARGE_CURRENT,
	REG_CHARGE_VOLTAGE,
};

#define REG_ADDR_SPEC_INFO		0x1A
#define SPEC_INFO_VERSION_MASK		GENMASK(7, 4)
#define SPEC_INFO_VERSION_SHIFT		4

#define SBS_VERSION_1_0			1
#define SBS_VERSION_1_1			2
#define SBS_VERSION_1_1_WITH_PEC	3

#define REG_ADDR_MANUFACTURE_DATE	0x1B

/* Battery Mode defines */
#define BATTERY_MODE_OFFSET		0x03
#define BATTERY_MODE_CAPACITY_MASK	BIT(15)
enum sbs_capacity_mode {
	CAPACITY_MODE_AMPS = 0,
	CAPACITY_MODE_WATTS = BATTERY_MODE_CAPACITY_MASK
};
#define BATTERY_MODE_CHARGER_MASK	(1<<14)

/* manufacturer access defines */
#define MANUFACTURER_ACCESS_STATUS	0x0006
#define MANUFACTURER_ACCESS_SLEEP	0x0011

/* battery status value bits */
#define BATTERY_INITIALIZED		0x80
#define BATTERY_DISCHARGING		0x40
#define BATTERY_FULL_CHARGED		0x20
#define BATTERY_FULL_DISCHARGED		0x10

/* min_value and max_value are only valid for numerical data */
#define SBS_DATA(_psp, _addr, _min_value, _max_value) { \
	.psp = _psp, \
	.addr = _addr, \
	.min_value = _min_value, \
	.max_value = _max_value, \
}

static const struct chip_data {
	enum power_supply_property psp;
	u8 addr;
	int min_value;
	int max_value;
} sbs_data[] = {
	[REG_MANUFACTURER_DATA] =
		SBS_DATA(POWER_SUPPLY_PROP_PRESENT, 0x00, 0, 65535),
	[REG_BATTERY_MODE] =
		SBS_DATA(-1, 0x03, 0, 65535),
	[REG_TEMPERATURE] =
		SBS_DATA(POWER_SUPPLY_PROP_TEMP, 0x08, 0, 65535),
	[REG_VOLTAGE] =
		SBS_DATA(POWER_SUPPLY_PROP_VOLTAGE_NOW, 0x09, 0, 20000),
	[REG_CURRENT_NOW] =
		SBS_DATA(POWER_SUPPLY_PROP_CURRENT_NOW, 0x0A, -32768, 32767),
	[REG_CURRENT_AVG] =
		SBS_DATA(POWER_SUPPLY_PROP_CURRENT_AVG, 0x0B, -32768, 32767),
	[REG_MAX_ERR] =
		SBS_DATA(POWER_SUPPLY_PROP_CAPACITY_ERROR_MARGIN, 0x0c, 0, 100),
	[REG_CAPACITY] =
		SBS_DATA(POWER_SUPPLY_PROP_CAPACITY, 0x0D, 0, 100),
	[REG_REMAINING_CAPACITY] =
		SBS_DATA(POWER_SUPPLY_PROP_ENERGY_NOW, 0x0F, 0, 65535),
	[REG_REMAINING_CAPACITY_CHARGE] =
		SBS_DATA(POWER_SUPPLY_PROP_CHARGE_NOW, 0x0F, 0, 65535),
	[REG_FULL_CHARGE_CAPACITY] =
		SBS_DATA(POWER_SUPPLY_PROP_ENERGY_FULL, 0x10, 0, 65535),
	[REG_FULL_CHARGE_CAPACITY_CHARGE] =
		SBS_DATA(POWER_SUPPLY_PROP_CHARGE_FULL, 0x10, 0, 65535),
	[REG_TIME_TO_EMPTY] =
		SBS_DATA(POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG, 0x12, 0, 65535),
	[REG_TIME_TO_FULL] =
		SBS_DATA(POWER_SUPPLY_PROP_TIME_TO_FULL_AVG, 0x13, 0, 65535),
	[REG_CHARGE_CURRENT] =
		SBS_DATA(POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX, 0x14, 0, 65535),
	[REG_CHARGE_VOLTAGE] =
		SBS_DATA(POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX, 0x15, 0, 65535),
	[REG_STATUS] =
		SBS_DATA(POWER_SUPPLY_PROP_STATUS, 0x16, 0, 65535),
	[REG_CAPACITY_LEVEL] =
		SBS_DATA(POWER_SUPPLY_PROP_CAPACITY_LEVEL, 0x16, 0, 65535),
	[REG_CYCLE_COUNT] =
		SBS_DATA(POWER_SUPPLY_PROP_CYCLE_COUNT, 0x17, 0, 65535),
	[REG_DESIGN_CAPACITY] =
		SBS_DATA(POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN, 0x18, 0, 65535),
	[REG_DESIGN_CAPACITY_CHARGE] =
		SBS_DATA(POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, 0x18, 0, 65535),
	[REG_DESIGN_VOLTAGE_MIN] =
		SBS_DATA(POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN, 0x19, 0, 65535),
	[REG_DESIGN_VOLTAGE_MAX] =
		SBS_DATA(POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN, 0x19, 0, 65535),
	[REG_SERIAL_NUMBER] =
		SBS_DATA(POWER_SUPPLY_PROP_SERIAL_NUMBER, 0x1C, 0, 65535),
	/* Properties of type `const char *' */
	[REG_MANUFACTURER] =
		SBS_DATA(POWER_SUPPLY_PROP_MANUFACTURER, 0x20, 0, 65535),
	[REG_MODEL_NAME] =
		SBS_DATA(POWER_SUPPLY_PROP_MODEL_NAME, 0x21, 0, 65535),
	[REG_CHEMISTRY] =
		SBS_DATA(POWER_SUPPLY_PROP_TECHNOLOGY, 0x22, 0, 65535)
};

static const enum power_supply_property sbs_properties[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_CAPACITY_LEVEL,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CYCLE_COUNT,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_AVG,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_CAPACITY_ERROR_MARGIN,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG,
	POWER_SUPPLY_PROP_TIME_TO_FULL_AVG,
	POWER_SUPPLY_PROP_SERIAL_NUMBER,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_ENERGY_NOW,
	POWER_SUPPLY_PROP_ENERGY_FULL,
	POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_NOW,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_MANUFACTURE_YEAR,
	POWER_SUPPLY_PROP_MANUFACTURE_MONTH,
	POWER_SUPPLY_PROP_MANUFACTURE_DAY,
	/* Properties of type `const char *' */
	POWER_SUPPLY_PROP_MANUFACTURER,
	POWER_SUPPLY_PROP_MODEL_NAME
};

/* Supports special manufacturer commands from TI BQ20Z65 and BQ20Z75 IC. */
#define SBS_FLAGS_TI_BQ20ZX5		BIT(0)

struct sbs_info {
	struct i2c_client		*client;
	struct power_supply		*power_supply;
	bool				is_present;
	struct gpio_desc		*gpio_detect;
	bool				charger_broadcasts;
	int				last_state;
	int				poll_time;
	u32				i2c_retry_count;
	u32				poll_retry_count;
	struct delayed_work		work;
	struct mutex			mode_lock;
	u32				flags;
};

static char model_name[I2C_SMBUS_BLOCK_MAX + 1];
static char manufacturer[I2C_SMBUS_BLOCK_MAX + 1];
static char chemistry[I2C_SMBUS_BLOCK_MAX + 1];
static bool force_load;

static int sbs_read_word_data(struct i2c_client *client, u8 address);
static int sbs_write_word_data(struct i2c_client *client, u8 address, u16 value);

static void sbs_disable_charger_broadcasts(struct sbs_info *chip)
{
	int val = sbs_read_word_data(chip->client, BATTERY_MODE_OFFSET);
	if (val < 0)
		goto exit;

	val |= BATTERY_MODE_CHARGER_MASK;

	val = sbs_write_word_data(chip->client, BATTERY_MODE_OFFSET, val);

exit:
	if (val < 0)
		dev_err(&chip->client->dev,
			"Failed to disable charger broadcasting: %d\n", val);
	else
		dev_dbg(&chip->client->dev, "%s\n", __func__);
}

static int sbs_update_presence(struct sbs_info *chip, bool is_present)
{
	struct i2c_client *client = chip->client;
	int retries = chip->i2c_retry_count;
	s32 ret = 0;
	u8 version;

	if (chip->is_present == is_present)
		return 0;

	if (!is_present) {
		chip->is_present = false;
		/* Disable PEC when no device is present */
		client->flags &= ~I2C_CLIENT_PEC;
		return 0;
	}

	/* Check if device supports packet error checking and use it */
	while (retries > 0) {
		ret = i2c_smbus_read_word_data(client, REG_ADDR_SPEC_INFO);
		if (ret >= 0)
			break;

		/*
		 * Some batteries trigger the detection pin before the
		 * I2C bus is properly connected. This works around the
		 * issue.
		 */
		msleep(100);

		retries--;
	}

	if (ret < 0) {
		dev_dbg(&client->dev, "failed to read spec info: %d\n", ret);

		/* fallback to old behaviour */
		client->flags &= ~I2C_CLIENT_PEC;
		chip->is_present = true;

		return ret;
	}

	version = (ret & SPEC_INFO_VERSION_MASK) >> SPEC_INFO_VERSION_SHIFT;

	if (version == SBS_VERSION_1_1_WITH_PEC)
		client->flags |= I2C_CLIENT_PEC;
	else
		client->flags &= ~I2C_CLIENT_PEC;

	if (of_device_is_compatible(client->dev.parent->of_node, "google,cros-ec-i2c-tunnel")
	    && client->flags & I2C_CLIENT_PEC) {
		dev_info(&client->dev, "Disabling PEC because of broken Cros-EC implementation\n");
		client->flags &= ~I2C_CLIENT_PEC;
	}

	dev_dbg(&client->dev, "PEC: %s\n", (client->flags & I2C_CLIENT_PEC) ?
		"enabled" : "disabled");

	if (!chip->is_present && is_present && !chip->charger_broadcasts)
		sbs_disable_charger_broadcasts(chip);

	chip->is_present = true;

	return 0;
}

static int sbs_read_word_data(struct i2c_client *client, u8 address)
{
	struct sbs_info *chip = i2c_get_clientdata(client);
	int retries = chip->i2c_retry_count;
	s32 ret = 0;

	while (retries > 0) {
		ret = i2c_smbus_read_word_data(client, address);
		if (ret >= 0)
			break;
		retries--;
	}

	if (ret < 0) {
		dev_dbg(&client->dev,
			"%s: i2c read at address 0x%x failed\n",
			__func__, address);
		return ret;
	}

	return ret;
}

static int sbs_read_string_data_fallback(struct i2c_client *client, u8 address, char *values)
{
	struct sbs_info *chip = i2c_get_clientdata(client);
	s32 ret = 0, block_length = 0;
	int retries_length, retries_block;
	u8 block_buffer[I2C_SMBUS_BLOCK_MAX + 1];

	retries_length = chip->i2c_retry_count;
	retries_block = chip->i2c_retry_count;

	dev_warn_once(&client->dev, "I2C adapter does not support I2C_FUNC_SMBUS_READ_BLOCK_DATA.\n"
				    "Fallback method does not support PEC.\n");

	/* Adapter needs to support these two functions */
	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_BYTE_DATA |
				     I2C_FUNC_SMBUS_I2C_BLOCK)){
		return -ENODEV;
	}

	/* Get the length of block data */
	while (retries_length > 0) {
		ret = i2c_smbus_read_byte_data(client, address);
		if (ret >= 0)
			break;
		retries_length--;
	}

	if (ret < 0) {
		dev_dbg(&client->dev,
			"%s: i2c read at address 0x%x failed\n",
			__func__, address);
		return ret;
	}

	/* block_length does not include NULL terminator */
	block_length = ret;
	if (block_length > I2C_SMBUS_BLOCK_MAX) {
		dev_err(&client->dev,
			"%s: Returned block_length is longer than 0x%x\n",
			__func__, I2C_SMBUS_BLOCK_MAX);
		return -EINVAL;
	}

	/* Get the block data */
	while (retries_block > 0) {
		ret = i2c_smbus_read_i2c_block_data(
				client, address,
				block_length + 1, block_buffer);
		if (ret >= 0)
			break;
		retries_block--;
	}

	if (ret < 0) {
		dev_dbg(&client->dev,
			"%s: i2c read at address 0x%x failed\n",
			__func__, address);
		return ret;
	}

	/* block_buffer[0] == block_length */
	memcpy(values, block_buffer + 1, block_length);
	values[block_length] = '\0';

	return ret;
}

static int sbs_read_string_data(struct i2c_client *client, u8 address, char *values)
{
	struct sbs_info *chip = i2c_get_clientdata(client);
	int retries = chip->i2c_retry_count;
	int ret = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_READ_BLOCK_DATA)) {
		bool pec = client->flags & I2C_CLIENT_PEC;
		client->flags &= ~I2C_CLIENT_PEC;
		ret = sbs_read_string_data_fallback(client, address, values);
		if (pec)
			client->flags |= I2C_CLIENT_PEC;
		return ret;
	}

	while (retries > 0) {
		ret = i2c_smbus_read_block_data(client, address, values);
		if (ret >= 0)
			break;
		retries--;
	}

	if (ret < 0) {
		dev_dbg(&client->dev, "failed to read block 0x%x: %d\n", address, ret);
		return ret;
	}

	/* add string termination */
	values[ret] = '\0';
	return ret;
}

static int sbs_write_word_data(struct i2c_client *client, u8 address,
	u16 value)
{
	struct sbs_info *chip = i2c_get_clientdata(client);
	int retries = chip->i2c_retry_count;
	s32 ret = 0;

	while (retries > 0) {
		ret = i2c_smbus_write_word_data(client, address, value);
		if (ret >= 0)
			break;
		retries--;
	}

	if (ret < 0) {
		dev_dbg(&client->dev,
			"%s: i2c write to address 0x%x failed\n",
			__func__, address);
		return ret;
	}

	return 0;
}

static int sbs_status_correct(struct i2c_client *client, int *intval)
{
	int ret;

	ret = sbs_read_word_data(client, sbs_data[REG_CURRENT_NOW].addr);
	if (ret < 0)
		return ret;

	ret = (s16)ret;

	/* Not drawing current -> not charging (i.e. idle) */
	if (*intval != POWER_SUPPLY_STATUS_FULL && ret == 0)
		*intval = POWER_SUPPLY_STATUS_NOT_CHARGING;

	if (*intval == POWER_SUPPLY_STATUS_FULL) {
		/* Drawing or providing current when full */
		if (ret > 0)
			*intval = POWER_SUPPLY_STATUS_CHARGING;
		else if (ret < 0)
			*intval = POWER_SUPPLY_STATUS_DISCHARGING;
	}

	return 0;
}

static bool sbs_bat_needs_calibration(struct i2c_client *client)
{
	int ret;

	ret = sbs_read_word_data(client, sbs_data[REG_BATTERY_MODE].addr);
	if (ret < 0)
		return false;

	return !!(ret & BIT(7));
}

static int sbs_get_ti_battery_presence_and_health(
	struct i2c_client *client, enum power_supply_property psp,
	union power_supply_propval *val)
{
	s32 ret;

	/*
	 * Write to ManufacturerAccess with ManufacturerAccess command
	 * and then read the status.
	 */
	ret = sbs_write_word_data(client, sbs_data[REG_MANUFACTURER_DATA].addr,
				  MANUFACTURER_ACCESS_STATUS);
	if (ret < 0) {
		if (psp == POWER_SUPPLY_PROP_PRESENT)
			val->intval = 0; /* battery removed */
		return ret;
	}

	ret = sbs_read_word_data(client, sbs_data[REG_MANUFACTURER_DATA].addr);
	if (ret < 0) {
		if (psp == POWER_SUPPLY_PROP_PRESENT)
			val->intval = 0; /* battery removed */
		return ret;
	}

	if (ret < sbs_data[REG_MANUFACTURER_DATA].min_value ||
	    ret > sbs_data[REG_MANUFACTURER_DATA].max_value) {
		val->intval = 0;
		return 0;
	}

	/* Mask the upper nibble of 2nd byte and
	 * lower byte of response then
	 * shift the result by 8 to get status*/
	ret &= 0x0F00;
	ret >>= 8;
	if (psp == POWER_SUPPLY_PROP_PRESENT) {
		if (ret == 0x0F)
			/* battery removed */
			val->intval = 0;
		else
			val->intval = 1;
	} else if (psp == POWER_SUPPLY_PROP_HEALTH) {
		if (ret == 0x09)
			val->intval = POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;
		else if (ret == 0x0B)
			val->intval = POWER_SUPPLY_HEALTH_OVERHEAT;
		else if (ret == 0x0C)
			val->intval = POWER_SUPPLY_HEALTH_DEAD;
		else if (sbs_bat_needs_calibration(client))
			val->intval = POWER_SUPPLY_HEALTH_CALIBRATION_REQUIRED;
		else
			val->intval = POWER_SUPPLY_HEALTH_GOOD;
	}

	return 0;
}

static int sbs_get_battery_presence_and_health(
	struct i2c_client *client, enum power_supply_property psp,
	union power_supply_propval *val)
{
	struct sbs_info *chip = i2c_get_clientdata(client);
	int ret;

	if (chip->flags & SBS_FLAGS_TI_BQ20ZX5)
		return sbs_get_ti_battery_presence_and_health(client, psp, val);

	/* Dummy command; if it succeeds, battery is present. */
	ret = sbs_read_word_data(client, sbs_data[REG_STATUS].addr);

	if (ret < 0) { /* battery not present*/
		if (psp == POWER_SUPPLY_PROP_PRESENT) {
			val->intval = 0;
			return 0;
		}
		return ret;
	}

	if (psp == POWER_SUPPLY_PROP_PRESENT)
		val->intval = 1; /* battery present */
	else { /* POWER_SUPPLY_PROP_HEALTH */
		if (sbs_bat_needs_calibration(client)) {
			val->intval = POWER_SUPPLY_HEALTH_CALIBRATION_REQUIRED;
		} else {
			/* SBS spec doesn't have a general health command. */
			val->intval = POWER_SUPPLY_HEALTH_UNKNOWN;
		}
	}

	return 0;
}

static int sbs_get_battery_property(struct i2c_client *client,
	int reg_offset, enum power_supply_property psp,
	union power_supply_propval *val)
{
	struct sbs_info *chip = i2c_get_clientdata(client);
	s32 ret;

	ret = sbs_read_word_data(client, sbs_data[reg_offset].addr);
	if (ret < 0)
		return ret;

	/* returned values are 16 bit */
	if (sbs_data[reg_offset].min_value < 0)
		ret = (s16)ret;

	if (ret >= sbs_data[reg_offset].min_value &&
	    ret <= sbs_data[reg_offset].max_value) {
		val->intval = ret;
		if (psp == POWER_SUPPLY_PROP_CAPACITY_LEVEL) {
			if (!(ret & BATTERY_INITIALIZED))
				val->intval =
					POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN;
			else if (ret & BATTERY_FULL_CHARGED)
				val->intval =
					POWER_SUPPLY_CAPACITY_LEVEL_FULL;
			else if (ret & BATTERY_FULL_DISCHARGED)
				val->intval =
					POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL;
			else
				val->intval =
					POWER_SUPPLY_CAPACITY_LEVEL_NORMAL;
			return 0;
		} else if (psp != POWER_SUPPLY_PROP_STATUS) {
			return 0;
		}

		if (ret & BATTERY_FULL_CHARGED)
			val->intval = POWER_SUPPLY_STATUS_FULL;
		else if (ret & BATTERY_DISCHARGING)
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		else
			val->intval = POWER_SUPPLY_STATUS_CHARGING;

		sbs_status_correct(client, &val->intval);

		if (chip->poll_time == 0)
			chip->last_state = val->intval;
		else if (chip->last_state != val->intval) {
			cancel_delayed_work_sync(&chip->work);
			power_supply_changed(chip->power_supply);
			chip->poll_time = 0;
		}
	} else {
		if (psp == POWER_SUPPLY_PROP_STATUS)
			val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
		else if (psp == POWER_SUPPLY_PROP_CAPACITY)
			/* sbs spec says that this can be >100 %
			 * even if max value is 100 %
			 */
			val->intval = min(ret, 100);
		else
			val->intval = 0;
	}

	return 0;
}

static int sbs_get_battery_string_property(struct i2c_client *client,
	int reg_offset, enum power_supply_property psp, char *val)
{
	s32 ret;

	ret = sbs_read_string_data(client, sbs_data[reg_offset].addr, val);

	if (ret < 0)
		return ret;

	return 0;
}

static void  sbs_unit_adjustment(struct i2c_client *client,
	enum power_supply_property psp, union power_supply_propval *val)
{
#define BASE_UNIT_CONVERSION		1000
#define BATTERY_MODE_CAP_MULT_WATT	(10 * BASE_UNIT_CONVERSION)
#define TIME_UNIT_CONVERSION		60
#define TEMP_KELVIN_TO_CELSIUS		2731
	switch (psp) {
	case POWER_SUPPLY_PROP_ENERGY_NOW:
	case POWER_SUPPLY_PROP_ENERGY_FULL:
	case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
		/* sbs provides energy in units of 10mWh.
		 * Convert to µWh
		 */
		val->intval *= BATTERY_MODE_CAP_MULT_WATT;
		break;

	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
	case POWER_SUPPLY_PROP_CURRENT_NOW:
	case POWER_SUPPLY_PROP_CURRENT_AVG:
	case POWER_SUPPLY_PROP_CHARGE_NOW:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX:
	case POWER_SUPPLY_PROP_CHARGE_FULL:
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
		val->intval *= BASE_UNIT_CONVERSION;
		break;

	case POWER_SUPPLY_PROP_TEMP:
		/* sbs provides battery temperature in 0.1K
		 * so convert it to 0.1°C
		 */
		val->intval -= TEMP_KELVIN_TO_CELSIUS;
		break;

	case POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG:
	case POWER_SUPPLY_PROP_TIME_TO_FULL_AVG:
		/* sbs provides time to empty and time to full in minutes.
		 * Convert to seconds
		 */
		val->intval *= TIME_UNIT_CONVERSION;
		break;

	default:
		dev_dbg(&client->dev,
			"%s: no need for unit conversion %d\n", __func__, psp);
	}
}

static enum sbs_capacity_mode sbs_set_capacity_mode(struct i2c_client *client,
	enum sbs_capacity_mode mode)
{
	int ret, original_val;

	original_val = sbs_read_word_data(client, BATTERY_MODE_OFFSET);
	if (original_val < 0)
		return original_val;

	if ((original_val & BATTERY_MODE_CAPACITY_MASK) == mode)
		return mode;

	if (mode == CAPACITY_MODE_AMPS)
		ret = original_val & ~BATTERY_MODE_CAPACITY_MASK;
	else
		ret = original_val | BATTERY_MODE_CAPACITY_MASK;

	ret = sbs_write_word_data(client, BATTERY_MODE_OFFSET, ret);
	if (ret < 0)
		return ret;

	usleep_range(1000, 2000);

	return original_val & BATTERY_MODE_CAPACITY_MASK;
}

static int sbs_get_battery_capacity(struct i2c_client *client,
	int reg_offset, enum power_supply_property psp,
	union power_supply_propval *val)
{
	s32 ret;
	enum sbs_capacity_mode mode = CAPACITY_MODE_WATTS;

	if (power_supply_is_amp_property(psp))
		mode = CAPACITY_MODE_AMPS;

	mode = sbs_set_capacity_mode(client, mode);
	if ((int)mode < 0)
		return mode;

	ret = sbs_read_word_data(client, sbs_data[reg_offset].addr);
	if (ret < 0)
		return ret;

	val->intval = ret;

	ret = sbs_set_capacity_mode(client, mode);
	if (ret < 0)
		return ret;

	return 0;
}

static char sbs_serial[5];
static int sbs_get_battery_serial_number(struct i2c_client *client,
	union power_supply_propval *val)
{
	int ret;

	ret = sbs_read_word_data(client, sbs_data[REG_SERIAL_NUMBER].addr);
	if (ret < 0)
		return ret;

	sprintf(sbs_serial, "%04x", ret);
	val->strval = sbs_serial;

	return 0;
}

static int sbs_get_property_index(struct i2c_client *client,
	enum power_supply_property psp)
{
	int count;
	for (count = 0; count < ARRAY_SIZE(sbs_data); count++)
		if (psp == sbs_data[count].psp)
			return count;

	dev_warn(&client->dev,
		"%s: Invalid Property - %d\n", __func__, psp);

	return -EINVAL;
}

static int sbs_get_chemistry(struct i2c_client *client,
		union power_supply_propval *val)
{
	enum power_supply_property psp = POWER_SUPPLY_PROP_TECHNOLOGY;
	int ret;

	ret = sbs_get_property_index(client, psp);
	if (ret < 0)
		return ret;

	ret = sbs_get_battery_string_property(client, ret, psp,
					      chemistry);
	if (ret < 0)
		return ret;

	if (!strncasecmp(chemistry, "LION", 4))
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
	else if (!strncasecmp(chemistry, "LiP", 3))
		val->intval = POWER_SUPPLY_TECHNOLOGY_LIPO;
	else if (!strncasecmp(chemistry, "NiCd", 4))
		val->intval = POWER_SUPPLY_TECHNOLOGY_NiCd;
	else if (!strncasecmp(chemistry, "NiMH", 4))
		val->intval = POWER_SUPPLY_TECHNOLOGY_NiMH;
	else
		val->intval = POWER_SUPPLY_TECHNOLOGY_UNKNOWN;

	if (val->intval == POWER_SUPPLY_TECHNOLOGY_UNKNOWN)
		dev_warn(&client->dev, "Unknown chemistry: %s\n", chemistry);

	return 0;
}

static int sbs_get_battery_manufacture_date(struct i2c_client *client,
	enum power_supply_property psp,
	union power_supply_propval *val)
{
	int ret;
	u16 day, month, year;

	ret = sbs_read_word_data(client, REG_ADDR_MANUFACTURE_DATE);
	if (ret < 0)
		return ret;

	day   = ret   & GENMASK(4,  0);
	month = (ret  & GENMASK(8,  5)) >> 5;
	year  = ((ret & GENMASK(15, 9)) >> 9) + 1980;

	switch (psp) {
	case POWER_SUPPLY_PROP_MANUFACTURE_YEAR:
		val->intval = year;
		break;
	case POWER_SUPPLY_PROP_MANUFACTURE_MONTH:
		val->intval = month;
		break;
	case POWER_SUPPLY_PROP_MANUFACTURE_DAY:
		val->intval = day;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sbs_get_property(struct power_supply *psy,
	enum power_supply_property psp,
	union power_supply_propval *val)
{
	int ret = 0;
	struct sbs_info *chip = power_supply_get_drvdata(psy);
	struct i2c_client *client = chip->client;

	if (chip->gpio_detect) {
		ret = gpiod_get_value_cansleep(chip->gpio_detect);
		if (ret < 0)
			return ret;
		if (psp == POWER_SUPPLY_PROP_PRESENT) {
			val->intval = ret;
			sbs_update_presence(chip, ret);
			return 0;
		}
		if (ret == 0)
			return -ENODATA;
	}

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_HEALTH:
		ret = sbs_get_battery_presence_and_health(client, psp, val);

		/* this can only be true if no gpio is used */
		if (psp == POWER_SUPPLY_PROP_PRESENT)
			return 0;
		break;

	case POWER_SUPPLY_PROP_TECHNOLOGY:
		ret = sbs_get_chemistry(client, val);
		if (ret < 0)
			break;

		goto done; /* don't trigger power_supply_changed()! */

	case POWER_SUPPLY_PROP_ENERGY_NOW:
	case POWER_SUPPLY_PROP_ENERGY_FULL:
	case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
	case POWER_SUPPLY_PROP_CHARGE_NOW:
	case POWER_SUPPLY_PROP_CHARGE_FULL:
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
		ret = sbs_get_property_index(client, psp);
		if (ret < 0)
			break;

		/* sbs_get_battery_capacity() will change the battery mode
		 * temporarily to read the requested attribute. Ensure we stay
		 * in the desired mode for the duration of the attribute read.
		 */
		mutex_lock(&chip->mode_lock);
		ret = sbs_get_battery_capacity(client, ret, psp, val);
		mutex_unlock(&chip->mode_lock);
		break;

	case POWER_SUPPLY_PROP_SERIAL_NUMBER:
		ret = sbs_get_battery_serial_number(client, val);
		break;

	case POWER_SUPPLY_PROP_STATUS:
	case POWER_SUPPLY_PROP_CAPACITY_LEVEL:
	case POWER_SUPPLY_PROP_CYCLE_COUNT:
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
	case POWER_SUPPLY_PROP_CURRENT_NOW:
	case POWER_SUPPLY_PROP_CURRENT_AVG:
	case POWER_SUPPLY_PROP_TEMP:
	case POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG:
	case POWER_SUPPLY_PROP_TIME_TO_FULL_AVG:
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX:
	case POWER_SUPPLY_PROP_CAPACITY:
	case POWER_SUPPLY_PROP_CAPACITY_ERROR_MARGIN:
		ret = sbs_get_property_index(client, psp);
		if (ret < 0)
			break;

		ret = sbs_get_battery_property(client, ret, psp, val);
		break;

	case POWER_SUPPLY_PROP_MODEL_NAME:
		ret = sbs_get_property_index(client, psp);
		if (ret < 0)
			break;

		ret = sbs_get_battery_string_property(client, ret, psp,
						      model_name);
		val->strval = model_name;
		break;

	case POWER_SUPPLY_PROP_MANUFACTURER:
		ret = sbs_get_property_index(client, psp);
		if (ret < 0)
			break;

		ret = sbs_get_battery_string_property(client, ret, psp,
						      manufacturer);
		val->strval = manufacturer;
		break;

	case POWER_SUPPLY_PROP_MANUFACTURE_YEAR:
	case POWER_SUPPLY_PROP_MANUFACTURE_MONTH:
	case POWER_SUPPLY_PROP_MANUFACTURE_DAY:
		ret = sbs_get_battery_manufacture_date(client, psp, val);
		break;

	default:
		dev_err(&client->dev,
			"%s: INVALID property\n", __func__);
		return -EINVAL;
	}

	if (!chip->gpio_detect && chip->is_present != (ret >= 0)) {
		bool old_present = chip->is_present;
		union power_supply_propval val;
		int err = sbs_get_battery_presence_and_health(
				client, POWER_SUPPLY_PROP_PRESENT, &val);

		sbs_update_presence(chip, !err && val.intval);

		if (old_present != chip->is_present)
			power_supply_changed(chip->power_supply);
	}

done:
	if (!ret) {
		/* Convert units to match requirements for power supply class */
		sbs_unit_adjustment(client, psp, val);
		dev_dbg(&client->dev,
			"%s: property = %d, value = %x\n", __func__,
			psp, val->intval);
	} else if (!chip->is_present)  {
		/* battery not present, so return NODATA for properties */
		ret = -ENODATA;
	}
	return ret;
}

static void sbs_supply_changed(struct sbs_info *chip)
{
	struct power_supply *battery = chip->power_supply;
	int ret;

	ret = gpiod_get_value_cansleep(chip->gpio_detect);
	if (ret < 0)
		return;
	sbs_update_presence(chip, ret);
	power_supply_changed(battery);
}

static irqreturn_t sbs_irq(int irq, void *devid)
{
	sbs_supply_changed(devid);
	return IRQ_HANDLED;
}

static void sbs_alert(struct i2c_client *client, enum i2c_alert_protocol prot,
	unsigned int data)
{
	sbs_supply_changed(i2c_get_clientdata(client));
}

static void sbs_external_power_changed(struct power_supply *psy)
{
	struct sbs_info *chip = power_supply_get_drvdata(psy);

	/* cancel outstanding work */
	cancel_delayed_work_sync(&chip->work);

	schedule_delayed_work(&chip->work, HZ);
	chip->poll_time = chip->poll_retry_count;
}

static void sbs_delayed_work(struct work_struct *work)
{
	struct sbs_info *chip;
	s32 ret;

	chip = container_of(work, struct sbs_info, work.work);

	ret = sbs_read_word_data(chip->client, sbs_data[REG_STATUS].addr);
	/* if the read failed, give up on this work */
	if (ret < 0) {
		chip->poll_time = 0;
		return;
	}

	if (ret & BATTERY_FULL_CHARGED)
		ret = POWER_SUPPLY_STATUS_FULL;
	else if (ret & BATTERY_DISCHARGING)
		ret = POWER_SUPPLY_STATUS_DISCHARGING;
	else
		ret = POWER_SUPPLY_STATUS_CHARGING;

	sbs_status_correct(chip->client, &ret);

	if (chip->last_state != ret) {
		chip->poll_time = 0;
		power_supply_changed(chip->power_supply);
		return;
	}
	if (chip->poll_time > 0) {
		schedule_delayed_work(&chip->work, HZ);
		chip->poll_time--;
		return;
	}
}

static const struct power_supply_desc sbs_default_desc = {
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties = sbs_properties,
	.num_properties = ARRAY_SIZE(sbs_properties),
	.get_property = sbs_get_property,
	.external_power_changed = sbs_external_power_changed,
};

static int sbs_probe(struct i2c_client *client)
{
	struct sbs_info *chip;
	struct power_supply_desc *sbs_desc;
	struct sbs_platform_data *pdata = client->dev.platform_data;
	struct power_supply_config psy_cfg = {};
	int rc;
	int irq;

	sbs_desc = devm_kmemdup(&client->dev, &sbs_default_desc,
			sizeof(*sbs_desc), GFP_KERNEL);
	if (!sbs_desc)
		return -ENOMEM;

	sbs_desc->name = devm_kasprintf(&client->dev, GFP_KERNEL, "sbs-%s",
			dev_name(&client->dev));
	if (!sbs_desc->name)
		return -ENOMEM;

	chip = devm_kzalloc(&client->dev, sizeof(struct sbs_info), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->flags = (u32)(uintptr_t)device_get_match_data(&client->dev);
	chip->client = client;
	psy_cfg.of_node = client->dev.of_node;
	psy_cfg.drv_data = chip;
	chip->last_state = POWER_SUPPLY_STATUS_UNKNOWN;
	mutex_init(&chip->mode_lock);

	/* use pdata if available, fall back to DT properties,
	 * or hardcoded defaults if not
	 */
	rc = device_property_read_u32(&client->dev, "sbs,i2c-retry-count",
				      &chip->i2c_retry_count);
	if (rc)
		chip->i2c_retry_count = 0;

	rc = device_property_read_u32(&client->dev, "sbs,poll-retry-count",
				      &chip->poll_retry_count);
	if (rc)
		chip->poll_retry_count = 0;

	if (pdata) {
		chip->poll_retry_count = pdata->poll_retry_count;
		chip->i2c_retry_count  = pdata->i2c_retry_count;
	}
	chip->i2c_retry_count = chip->i2c_retry_count + 1;

	chip->charger_broadcasts = !device_property_read_bool(&client->dev,
					"sbs,disable-charger-broadcasts");

	chip->gpio_detect = devm_gpiod_get_optional(&client->dev,
			"sbs,battery-detect", GPIOD_IN);
	if (IS_ERR(chip->gpio_detect))
		return dev_err_probe(&client->dev, PTR_ERR(chip->gpio_detect),
				     "Failed to get gpio\n");

	i2c_set_clientdata(client, chip);

	if (!chip->gpio_detect)
		goto skip_gpio;

	irq = gpiod_to_irq(chip->gpio_detect);
	if (irq <= 0) {
		dev_warn(&client->dev, "Failed to get gpio as irq: %d\n", irq);
		goto skip_gpio;
	}

	rc = devm_request_threaded_irq(&client->dev, irq, NULL, sbs_irq,
		IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
		dev_name(&client->dev), chip);
	if (rc) {
		dev_warn(&client->dev, "Failed to request irq: %d\n", rc);
		goto skip_gpio;
	}

skip_gpio:
	/*
	 * Before we register, we might need to make sure we can actually talk
	 * to the battery.
	 */
	if (!(force_load || chip->gpio_detect)) {
		union power_supply_propval val;

		rc = sbs_get_battery_presence_and_health(
				client, POWER_SUPPLY_PROP_PRESENT, &val);
		if (rc < 0 || !val.intval)
			return dev_err_probe(&client->dev, -ENODEV,
					     "Failed to get present status\n");
	}

	rc = devm_delayed_work_autocancel(&client->dev, &chip->work,
					  sbs_delayed_work);
	if (rc)
		return rc;

	chip->power_supply = devm_power_supply_register(&client->dev, sbs_desc,
						   &psy_cfg);
	if (IS_ERR(chip->power_supply))
		return dev_err_probe(&client->dev, PTR_ERR(chip->power_supply),
				     "Failed to register power supply\n");

	dev_info(&client->dev,
		"%s: battery gas gauge device registered\n", client->name);

	return 0;
}

#if defined CONFIG_PM_SLEEP

static int sbs_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct sbs_info *chip = i2c_get_clientdata(client);
	int ret;

	if (chip->poll_time > 0)
		cancel_delayed_work_sync(&chip->work);

	if (chip->flags & SBS_FLAGS_TI_BQ20ZX5) {
		/* Write to manufacturer access with sleep command. */
		ret = sbs_write_word_data(client,
					  sbs_data[REG_MANUFACTURER_DATA].addr,
					  MANUFACTURER_ACCESS_SLEEP);
		if (chip->is_present && ret < 0)
			return ret;
	}

	return 0;
}

static SIMPLE_DEV_PM_OPS(sbs_pm_ops, sbs_suspend, NULL);
#define SBS_PM_OPS (&sbs_pm_ops)

#else
#define SBS_PM_OPS NULL
#endif

static const struct i2c_device_id sbs_id[] = {
	{ "bq20z65", 0 },
	{ "bq20z75", 0 },
	{ "sbs-battery", 1 },
	{}
};
MODULE_DEVICE_TABLE(i2c, sbs_id);

static const struct of_device_id sbs_dt_ids[] = {
	{ .compatible = "sbs,sbs-battery" },
	{
		.compatible = "ti,bq20z65",
		.data = (void *)SBS_FLAGS_TI_BQ20ZX5,
	},
	{
		.compatible = "ti,bq20z75",
		.data = (void *)SBS_FLAGS_TI_BQ20ZX5,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, sbs_dt_ids);

static struct i2c_driver sbs_battery_driver = {
	.probe_new	= sbs_probe,
	.alert		= sbs_alert,
	.id_table	= sbs_id,
	.driver = {
		.name	= "sbs-battery",
		.of_match_table = sbs_dt_ids,
		.pm	= SBS_PM_OPS,
	},
};
module_i2c_driver(sbs_battery_driver);

MODULE_DESCRIPTION("SBS battery monitor driver");
MODULE_LICENSE("GPL");

module_param(force_load, bool, 0444);
MODULE_PARM_DESC(force_load,
		 "Attempt to load the driver even if no battery is connected");
