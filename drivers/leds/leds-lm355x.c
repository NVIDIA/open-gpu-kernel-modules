// SPDX-License-Identifier: GPL-2.0-only
/*
* Simple driver for Texas Instruments LM355x LED Flash driver chip
* Copyright (C) 2012 Texas Instruments
*/

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/regmap.h>
#include <linux/platform_data/leds-lm355x.h>

enum lm355x_type {
	CHIP_LM3554 = 0,
	CHIP_LM3556,
};

enum lm355x_regs {
	REG_FLAG = 0,
	REG_TORCH_CFG,
	REG_TORCH_CTRL,
	REG_STROBE_CFG,
	REG_FLASH_CTRL,
	REG_INDI_CFG,
	REG_INDI_CTRL,
	REG_OPMODE,
	REG_MAX,
};

/* operation mode */
enum lm355x_mode {
	MODE_SHDN = 0,
	MODE_INDIC,
	MODE_TORCH,
	MODE_FLASH
};

/* register map info. */
struct lm355x_reg_data {
	u8 regno;
	u8 mask;
	u8 shift;
};

struct lm355x_chip_data {
	struct device *dev;
	enum lm355x_type type;

	struct led_classdev cdev_flash;
	struct led_classdev cdev_torch;
	struct led_classdev cdev_indicator;

	struct lm355x_platform_data *pdata;
	struct regmap *regmap;
	struct mutex lock;

	unsigned int last_flag;
	struct lm355x_reg_data *regs;
};

/* specific indicator function for lm3556 */
enum lm3556_indic_pulse_time {
	PULSE_TIME_0_MS = 0,
	PULSE_TIME_32_MS,
	PULSE_TIME_64_MS,
	PULSE_TIME_92_MS,
	PULSE_TIME_128_MS,
	PULSE_TIME_160_MS,
	PULSE_TIME_196_MS,
	PULSE_TIME_224_MS,
	PULSE_TIME_256_MS,
	PULSE_TIME_288_MS,
	PULSE_TIME_320_MS,
	PULSE_TIME_352_MS,
	PULSE_TIME_384_MS,
	PULSE_TIME_416_MS,
	PULSE_TIME_448_MS,
	PULSE_TIME_480_MS,
};

enum lm3556_indic_n_blank {
	INDIC_N_BLANK_0 = 0,
	INDIC_N_BLANK_1,
	INDIC_N_BLANK_2,
	INDIC_N_BLANK_3,
	INDIC_N_BLANK_4,
	INDIC_N_BLANK_5,
	INDIC_N_BLANK_6,
	INDIC_N_BLANK_7,
	INDIC_N_BLANK_8,
	INDIC_N_BLANK_9,
	INDIC_N_BLANK_10,
	INDIC_N_BLANK_11,
	INDIC_N_BLANK_12,
	INDIC_N_BLANK_13,
	INDIC_N_BLANK_14,
	INDIC_N_BLANK_15,
};

enum lm3556_indic_period {
	INDIC_PERIOD_0 = 0,
	INDIC_PERIOD_1,
	INDIC_PERIOD_2,
	INDIC_PERIOD_3,
	INDIC_PERIOD_4,
	INDIC_PERIOD_5,
	INDIC_PERIOD_6,
	INDIC_PERIOD_7,
};

#define INDIC_PATTERN_SIZE 4

struct indicator {
	u8 blinking;
	u8 period_cnt;
};

/* indicator pattern data only for lm3556 */
static struct indicator indicator_pattern[INDIC_PATTERN_SIZE] = {
	[0] = {(INDIC_N_BLANK_1 << 4) | PULSE_TIME_32_MS, INDIC_PERIOD_1},
	[1] = {(INDIC_N_BLANK_15 << 4) | PULSE_TIME_32_MS, INDIC_PERIOD_2},
	[2] = {(INDIC_N_BLANK_10 << 4) | PULSE_TIME_32_MS, INDIC_PERIOD_4},
	[3] = {(INDIC_N_BLANK_5 << 4) | PULSE_TIME_32_MS, INDIC_PERIOD_7},
};

static struct lm355x_reg_data lm3554_regs[REG_MAX] = {
	[REG_FLAG] = {0xD0, 0xBF, 0},
	[REG_TORCH_CFG] = {0xE0, 0x80, 7},
	[REG_TORCH_CTRL] = {0xA0, 0x38, 3},
	[REG_STROBE_CFG] = {0xE0, 0x04, 2},
	[REG_FLASH_CTRL] = {0xB0, 0x78, 3},
	[REG_INDI_CFG] = {0xE0, 0x08, 3},
	[REG_INDI_CTRL] = {0xA0, 0xC0, 6},
	[REG_OPMODE] = {0xA0, 0x03, 0},
};

static struct lm355x_reg_data lm3556_regs[REG_MAX] = {
	[REG_FLAG] = {0x0B, 0xFF, 0},
	[REG_TORCH_CFG] = {0x0A, 0x10, 4},
	[REG_TORCH_CTRL] = {0x09, 0x70, 4},
	[REG_STROBE_CFG] = {0x0A, 0x20, 5},
	[REG_FLASH_CTRL] = {0x09, 0x0F, 0},
	[REG_INDI_CFG] = {0xFF, 0xFF, 0},
	[REG_INDI_CTRL] = {0x09, 0x70, 4},
	[REG_OPMODE] = {0x0A, 0x03, 0},
};

static char lm355x_name[][I2C_NAME_SIZE] = {
	[CHIP_LM3554] = LM3554_NAME,
	[CHIP_LM3556] = LM3556_NAME,
};

/* chip initialize */
static int lm355x_chip_init(struct lm355x_chip_data *chip)
{
	int ret;
	unsigned int reg_val;
	struct lm355x_platform_data *pdata = chip->pdata;

	/* input and output pins configuration */
	switch (chip->type) {
	case CHIP_LM3554:
		reg_val = (u32)pdata->pin_tx2 | (u32)pdata->ntc_pin;
		ret = regmap_update_bits(chip->regmap, 0xE0, 0x28, reg_val);
		if (ret < 0)
			goto out;
		reg_val = (u32)pdata->pass_mode;
		ret = regmap_update_bits(chip->regmap, 0xA0, 0x04, reg_val);
		if (ret < 0)
			goto out;
		break;

	case CHIP_LM3556:
		reg_val = (u32)pdata->pin_tx2 | (u32)pdata->ntc_pin |
		          (u32)pdata->pass_mode;
		ret = regmap_update_bits(chip->regmap, 0x0A, 0xC4, reg_val);
		if (ret < 0)
			goto out;
		break;
	default:
		return -ENODATA;
	}

	return ret;
out:
	dev_err(chip->dev, "%s:i2c access fail to register\n", __func__);
	return ret;
}

/* chip control */
static int lm355x_control(struct lm355x_chip_data *chip,
			   u8 brightness, enum lm355x_mode opmode)
{
	int ret;
	unsigned int reg_val;
	struct lm355x_platform_data *pdata = chip->pdata;
	struct lm355x_reg_data *preg = chip->regs;

	ret = regmap_read(chip->regmap, preg[REG_FLAG].regno, &chip->last_flag);
	if (ret < 0)
		goto out;
	if (chip->last_flag & preg[REG_FLAG].mask)
		dev_info(chip->dev, "%s Last FLAG is 0x%x\n",
			 lm355x_name[chip->type],
			 chip->last_flag & preg[REG_FLAG].mask);
	/* brightness 0 means shutdown */
	if (!brightness)
		opmode = MODE_SHDN;

	switch (opmode) {
	case MODE_TORCH:
		ret =
		    regmap_update_bits(chip->regmap, preg[REG_TORCH_CTRL].regno,
				       preg[REG_TORCH_CTRL].mask,
				       (brightness - 1)
				       << preg[REG_TORCH_CTRL].shift);
		if (ret < 0)
			goto out;

		if (pdata->pin_tx1 != LM355x_PIN_TORCH_DISABLE) {
			ret =
			    regmap_update_bits(chip->regmap,
					       preg[REG_TORCH_CFG].regno,
					       preg[REG_TORCH_CFG].mask,
					       0x01 <<
					       preg[REG_TORCH_CFG].shift);
			if (ret < 0)
				goto out;
			opmode = MODE_SHDN;
			dev_info(chip->dev,
				 "torch brt is set - ext. torch pin mode\n");
		}
		break;

	case MODE_FLASH:

		ret =
		    regmap_update_bits(chip->regmap, preg[REG_FLASH_CTRL].regno,
				       preg[REG_FLASH_CTRL].mask,
				       (brightness - 1)
				       << preg[REG_FLASH_CTRL].shift);
		if (ret < 0)
			goto out;

		if (pdata->pin_strobe != LM355x_PIN_STROBE_DISABLE) {
			if (chip->type == CHIP_LM3554)
				reg_val = 0x00;
			else
				reg_val = 0x01;
			ret =
			    regmap_update_bits(chip->regmap,
					       preg[REG_STROBE_CFG].regno,
					       preg[REG_STROBE_CFG].mask,
					       reg_val <<
					       preg[REG_STROBE_CFG].shift);
			if (ret < 0)
				goto out;
			opmode = MODE_SHDN;
			dev_info(chip->dev,
				 "flash brt is set - ext. strobe pin mode\n");
		}
		break;

	case MODE_INDIC:
		ret =
		    regmap_update_bits(chip->regmap, preg[REG_INDI_CTRL].regno,
				       preg[REG_INDI_CTRL].mask,
				       (brightness - 1)
				       << preg[REG_INDI_CTRL].shift);
		if (ret < 0)
			goto out;

		if (pdata->pin_tx2 != LM355x_PIN_TX_DISABLE) {
			ret =
			    regmap_update_bits(chip->regmap,
					       preg[REG_INDI_CFG].regno,
					       preg[REG_INDI_CFG].mask,
					       0x01 <<
					       preg[REG_INDI_CFG].shift);
			if (ret < 0)
				goto out;
			opmode = MODE_SHDN;
		}
		break;
	case MODE_SHDN:
		break;
	default:
		return -EINVAL;
	}
	/* operation mode control */
	ret = regmap_update_bits(chip->regmap, preg[REG_OPMODE].regno,
				 preg[REG_OPMODE].mask,
				 opmode << preg[REG_OPMODE].shift);
	if (ret < 0)
		goto out;
	return ret;
out:
	dev_err(chip->dev, "%s:i2c access fail to register\n", __func__);
	return ret;
}

/* torch */

static int lm355x_torch_brightness_set(struct led_classdev *cdev,
					enum led_brightness brightness)
{
	struct lm355x_chip_data *chip =
	    container_of(cdev, struct lm355x_chip_data, cdev_torch);
	int ret;

	mutex_lock(&chip->lock);
	ret = lm355x_control(chip, brightness, MODE_TORCH);
	mutex_unlock(&chip->lock);
	return ret;
}

/* flash */

static int lm355x_strobe_brightness_set(struct led_classdev *cdev,
					 enum led_brightness brightness)
{
	struct lm355x_chip_data *chip =
	    container_of(cdev, struct lm355x_chip_data, cdev_flash);
	int ret;

	mutex_lock(&chip->lock);
	ret = lm355x_control(chip, brightness, MODE_FLASH);
	mutex_unlock(&chip->lock);
	return ret;
}

/* indicator */

static int lm355x_indicator_brightness_set(struct led_classdev *cdev,
					    enum led_brightness brightness)
{
	struct lm355x_chip_data *chip =
	    container_of(cdev, struct lm355x_chip_data, cdev_indicator);
	int ret;

	mutex_lock(&chip->lock);
	ret = lm355x_control(chip, brightness, MODE_INDIC);
	mutex_unlock(&chip->lock);
	return ret;
}

/* indicator pattern only for lm3556*/
static ssize_t pattern_store(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t size)
{
	ssize_t ret;
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct lm355x_chip_data *chip =
	    container_of(led_cdev, struct lm355x_chip_data, cdev_indicator);
	unsigned int state;

	ret = kstrtouint(buf, 10, &state);
	if (ret)
		goto out;
	if (state > INDIC_PATTERN_SIZE - 1)
		state = INDIC_PATTERN_SIZE - 1;

	ret = regmap_write(chip->regmap, 0x04,
			   indicator_pattern[state].blinking);
	if (ret < 0)
		goto out;

	ret = regmap_write(chip->regmap, 0x05,
			   indicator_pattern[state].period_cnt);
	if (ret < 0)
		goto out;

	return size;
out:
	dev_err(chip->dev, "%s:i2c access fail to register\n", __func__);
	return ret;
}

static DEVICE_ATTR_WO(pattern);

static struct attribute *lm355x_indicator_attrs[] = {
	&dev_attr_pattern.attr,
	NULL
};
ATTRIBUTE_GROUPS(lm355x_indicator);

static const struct regmap_config lm355x_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0xFF,
};

/* module initialize */
static int lm355x_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	struct lm355x_platform_data *pdata = dev_get_platdata(&client->dev);
	struct lm355x_chip_data *chip;

	int err;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "i2c functionality check fail.\n");
		return -EOPNOTSUPP;
	}

	if (pdata == NULL) {
		dev_err(&client->dev, "needs Platform Data.\n");
		return -ENODATA;
	}

	chip = devm_kzalloc(&client->dev,
			    sizeof(struct lm355x_chip_data), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->dev = &client->dev;
	chip->type = id->driver_data;
	switch (id->driver_data) {
	case CHIP_LM3554:
		chip->regs = lm3554_regs;
		break;
	case CHIP_LM3556:
		chip->regs = lm3556_regs;
		break;
	default:
		return -ENOSYS;
	}
	chip->pdata = pdata;

	chip->regmap = devm_regmap_init_i2c(client, &lm355x_regmap);
	if (IS_ERR(chip->regmap)) {
		err = PTR_ERR(chip->regmap);
		dev_err(&client->dev,
			"Failed to allocate register map: %d\n", err);
		return err;
	}

	mutex_init(&chip->lock);
	i2c_set_clientdata(client, chip);

	err = lm355x_chip_init(chip);
	if (err < 0)
		goto err_out;

	/* flash */
	chip->cdev_flash.name = "flash";
	chip->cdev_flash.max_brightness = 16;
	chip->cdev_flash.brightness_set_blocking = lm355x_strobe_brightness_set;
	chip->cdev_flash.default_trigger = "flash";
	err = led_classdev_register(&client->dev, &chip->cdev_flash);
	if (err < 0)
		goto err_out;
	/* torch */
	chip->cdev_torch.name = "torch";
	chip->cdev_torch.max_brightness = 8;
	chip->cdev_torch.brightness_set_blocking = lm355x_torch_brightness_set;
	chip->cdev_torch.default_trigger = "torch";
	err = led_classdev_register(&client->dev, &chip->cdev_torch);
	if (err < 0)
		goto err_create_torch_file;
	/* indicator */
	chip->cdev_indicator.name = "indicator";
	if (id->driver_data == CHIP_LM3554)
		chip->cdev_indicator.max_brightness = 4;
	else
		chip->cdev_indicator.max_brightness = 8;
	chip->cdev_indicator.brightness_set_blocking =
					lm355x_indicator_brightness_set;
	/* indicator pattern control only for LM3556 */
	if (id->driver_data == CHIP_LM3556)
		chip->cdev_indicator.groups = lm355x_indicator_groups;
	err = led_classdev_register(&client->dev, &chip->cdev_indicator);
	if (err < 0)
		goto err_create_indicator_file;

	dev_info(&client->dev, "%s is initialized\n",
		 lm355x_name[id->driver_data]);
	return 0;

err_create_indicator_file:
	led_classdev_unregister(&chip->cdev_torch);
err_create_torch_file:
	led_classdev_unregister(&chip->cdev_flash);
err_out:
	return err;
}

static int lm355x_remove(struct i2c_client *client)
{
	struct lm355x_chip_data *chip = i2c_get_clientdata(client);
	struct lm355x_reg_data *preg = chip->regs;

	regmap_write(chip->regmap, preg[REG_OPMODE].regno, 0);
	led_classdev_unregister(&chip->cdev_indicator);
	led_classdev_unregister(&chip->cdev_torch);
	led_classdev_unregister(&chip->cdev_flash);
	dev_info(&client->dev, "%s is removed\n", lm355x_name[chip->type]);

	return 0;
}

static const struct i2c_device_id lm355x_id[] = {
	{LM3554_NAME, CHIP_LM3554},
	{LM3556_NAME, CHIP_LM3556},
	{}
};

MODULE_DEVICE_TABLE(i2c, lm355x_id);

static struct i2c_driver lm355x_i2c_driver = {
	.driver = {
		   .name = LM355x_NAME,
		   .pm = NULL,
		   },
	.probe = lm355x_probe,
	.remove = lm355x_remove,
	.id_table = lm355x_id,
};

module_i2c_driver(lm355x_i2c_driver);

MODULE_DESCRIPTION("Texas Instruments Flash Lighting driver for LM355x");
MODULE_AUTHOR("Daniel Jeong <daniel.jeong@ti.com>");
MODULE_AUTHOR("G.Shark Jeong <gshark.jeong@gmail.com>");
MODULE_LICENSE("GPL v2");
