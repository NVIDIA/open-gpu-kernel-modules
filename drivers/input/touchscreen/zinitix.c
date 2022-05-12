// SPDX-License-Identifier: GPL-2.0-only

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>

/* Register Map */

#define BT541_SWRESET_CMD			0x0000
#define BT541_WAKEUP_CMD			0x0001

#define BT541_IDLE_CMD				0x0004
#define BT541_SLEEP_CMD				0x0005

#define BT541_CLEAR_INT_STATUS_CMD		0x0003
#define BT541_CALIBRATE_CMD			0x0006
#define BT541_SAVE_STATUS_CMD			0x0007
#define BT541_SAVE_CALIBRATION_CMD		0x0008
#define BT541_RECALL_FACTORY_CMD		0x000f

#define BT541_THRESHOLD				0x0020

#define BT541_LARGE_PALM_REJECT_AREA_TH		0x003F

#define BT541_DEBUG_REG				0x0115 /* 0~7 */

#define BT541_TOUCH_MODE			0x0010
#define BT541_CHIP_REVISION			0x0011
#define BT541_FIRMWARE_VERSION			0x0012

#define ZINITIX_USB_DETECT			0x116

#define BT541_MINOR_FW_VERSION			0x0121

#define BT541_VENDOR_ID				0x001C
#define BT541_HW_ID				0x0014

#define BT541_DATA_VERSION_REG			0x0013
#define BT541_SUPPORTED_FINGER_NUM		0x0015
#define BT541_EEPROM_INFO			0x0018
#define BT541_INITIAL_TOUCH_MODE		0x0019

#define BT541_TOTAL_NUMBER_OF_X			0x0060
#define BT541_TOTAL_NUMBER_OF_Y			0x0061

#define BT541_DELAY_RAW_FOR_HOST		0x007f

#define BT541_BUTTON_SUPPORTED_NUM		0x00B0
#define BT541_BUTTON_SENSITIVITY		0x00B2
#define BT541_DUMMY_BUTTON_SENSITIVITY		0X00C8

#define BT541_X_RESOLUTION			0x00C0
#define BT541_Y_RESOLUTION			0x00C1

#define BT541_POINT_STATUS_REG			0x0080
#define BT541_ICON_STATUS_REG			0x00AA

#define BT541_POINT_COORD_REG			(BT541_POINT_STATUS_REG + 2)

#define BT541_AFE_FREQUENCY			0x0100
#define BT541_DND_N_COUNT			0x0122
#define BT541_DND_U_COUNT			0x0135

#define BT541_RAWDATA_REG			0x0200

#define BT541_EEPROM_INFO_REG			0x0018

#define BT541_INT_ENABLE_FLAG			0x00f0
#define BT541_PERIODICAL_INTERRUPT_INTERVAL	0x00f1

#define BT541_BTN_WIDTH				0x016d

#define BT541_CHECKSUM_RESULT			0x012c

#define BT541_INIT_FLASH			0x01d0
#define BT541_WRITE_FLASH			0x01d1
#define BT541_READ_FLASH			0x01d2

#define ZINITIX_INTERNAL_FLAG_02		0x011e
#define ZINITIX_INTERNAL_FLAG_03		0x011f

#define ZINITIX_I2C_CHECKSUM_WCNT		0x016a
#define ZINITIX_I2C_CHECKSUM_RESULT		0x016c

/* Interrupt & status register flags */

#define BIT_PT_CNT_CHANGE			BIT(0)
#define BIT_DOWN				BIT(1)
#define BIT_MOVE				BIT(2)
#define BIT_UP					BIT(3)
#define BIT_PALM				BIT(4)
#define BIT_PALM_REJECT				BIT(5)
#define BIT_RESERVED_0				BIT(6)
#define BIT_RESERVED_1				BIT(7)
#define BIT_WEIGHT_CHANGE			BIT(8)
#define BIT_PT_NO_CHANGE			BIT(9)
#define BIT_REJECT				BIT(10)
#define BIT_PT_EXIST				BIT(11)
#define BIT_RESERVED_2				BIT(12)
#define BIT_ERROR				BIT(13)
#define BIT_DEBUG				BIT(14)
#define BIT_ICON_EVENT				BIT(15)

#define SUB_BIT_EXIST				BIT(0)
#define SUB_BIT_DOWN				BIT(1)
#define SUB_BIT_MOVE				BIT(2)
#define SUB_BIT_UP				BIT(3)
#define SUB_BIT_UPDATE				BIT(4)
#define SUB_BIT_WAIT				BIT(5)

#define DEFAULT_TOUCH_POINT_MODE		2
#define MAX_SUPPORTED_FINGER_NUM		5

#define CHIP_ON_DELAY				15 // ms
#define FIRMWARE_ON_DELAY			40 // ms

struct point_coord {
	__le16	x;
	__le16	y;
	u8	width;
	u8	sub_status;
	// currently unused, but needed as padding:
	u8	minor_width;
	u8	angle;
};

struct touch_event {
	__le16	status;
	u8	finger_cnt;
	u8	time_stamp;
	struct point_coord point_coord[MAX_SUPPORTED_FINGER_NUM];
};

struct bt541_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct touchscreen_properties prop;
	struct regulator_bulk_data supplies[2];
	u32 zinitix_mode;
};

static int zinitix_read_data(struct i2c_client *client,
			     u16 reg, void *values, size_t length)
{
	__le16 reg_le = cpu_to_le16(reg);
	int ret;

	/* A single i2c_transfer() transaction does not work here. */
	ret = i2c_master_send(client, (u8 *)&reg_le, sizeof(reg_le));
	if (ret != sizeof(reg_le))
		return ret < 0 ? ret : -EIO;

	ret = i2c_master_recv(client, (u8 *)values, length);
	if (ret != length)
		return ret < 0 ? ret : -EIO;

	return 0;
}

static int zinitix_write_u16(struct i2c_client *client, u16 reg, u16 value)
{
	__le16 packet[2] = {cpu_to_le16(reg), cpu_to_le16(value)};
	int ret;

	ret = i2c_master_send(client, (u8 *)packet, sizeof(packet));
	if (ret != sizeof(packet))
		return ret < 0 ? ret : -EIO;

	return 0;
}

static int zinitix_write_cmd(struct i2c_client *client, u16 reg)
{
	__le16 reg_le = cpu_to_le16(reg);
	int ret;

	ret = i2c_master_send(client, (u8 *)&reg_le, sizeof(reg_le));
	if (ret != sizeof(reg_le))
		return ret < 0 ? ret : -EIO;

	return 0;
}

static int zinitix_init_touch(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;
	int i;
	int error;

	error = zinitix_write_cmd(client, BT541_SWRESET_CMD);
	if (error) {
		dev_err(&client->dev, "Failed to write reset command\n");
		return error;
	}

	error = zinitix_write_u16(client, BT541_INT_ENABLE_FLAG, 0x0);
	if (error) {
		dev_err(&client->dev,
			"Failed to reset interrupt enable flag\n");
		return error;
	}

	/* initialize */
	error = zinitix_write_u16(client, BT541_X_RESOLUTION,
				  bt541->prop.max_x);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_Y_RESOLUTION,
				  bt541->prop.max_y);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_SUPPORTED_FINGER_NUM,
				  MAX_SUPPORTED_FINGER_NUM);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_INITIAL_TOUCH_MODE,
				  bt541->zinitix_mode);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_TOUCH_MODE,
				  bt541->zinitix_mode);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_INT_ENABLE_FLAG,
				  BIT_PT_CNT_CHANGE | BIT_DOWN | BIT_MOVE |
					BIT_UP);
	if (error)
		return error;

	/* clear queue */
	for (i = 0; i < 10; i++) {
		zinitix_write_cmd(client, BT541_CLEAR_INT_STATUS_CMD);
		udelay(10);
	}

	return 0;
}

static int zinitix_init_regulators(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;
	int error;

	bt541->supplies[0].supply = "vdd";
	bt541->supplies[1].supply = "vddo";
	error = devm_regulator_bulk_get(&client->dev,
					ARRAY_SIZE(bt541->supplies),
					bt541->supplies);
	if (error < 0) {
		dev_err(&client->dev, "Failed to get regulators: %d\n", error);
		return error;
	}

	return 0;
}

static int zinitix_send_power_on_sequence(struct bt541_ts_data *bt541)
{
	int error;
	struct i2c_client *client = bt541->client;

	error = zinitix_write_u16(client, 0xc000, 0x0001);
	if (error) {
		dev_err(&client->dev,
			"Failed to send power sequence(vendor cmd enable)\n");
		return error;
	}
	udelay(10);

	error = zinitix_write_cmd(client, 0xc004);
	if (error) {
		dev_err(&client->dev,
			"Failed to send power sequence (intn clear)\n");
		return error;
	}
	udelay(10);

	error = zinitix_write_u16(client, 0xc002, 0x0001);
	if (error) {
		dev_err(&client->dev,
			"Failed to send power sequence (nvm init)\n");
		return error;
	}
	mdelay(2);

	error = zinitix_write_u16(client, 0xc001, 0x0001);
	if (error) {
		dev_err(&client->dev,
			"Failed to send power sequence (program start)\n");
		return error;
	}
	msleep(FIRMWARE_ON_DELAY);

	return 0;
}

static void zinitix_report_finger(struct bt541_ts_data *bt541, int slot,
				  const struct point_coord *p)
{
	input_mt_slot(bt541->input_dev, slot);
	input_mt_report_slot_state(bt541->input_dev, MT_TOOL_FINGER, true);
	touchscreen_report_pos(bt541->input_dev, &bt541->prop,
			       le16_to_cpu(p->x), le16_to_cpu(p->y), true);
	input_report_abs(bt541->input_dev, ABS_MT_TOUCH_MAJOR, p->width);
}

static irqreturn_t zinitix_ts_irq_handler(int irq, void *bt541_handler)
{
	struct bt541_ts_data *bt541 = bt541_handler;
	struct i2c_client *client = bt541->client;
	struct touch_event touch_event;
	int error;
	int i;

	memset(&touch_event, 0, sizeof(struct touch_event));

	error = zinitix_read_data(bt541->client, BT541_POINT_STATUS_REG,
				  &touch_event, sizeof(struct touch_event));
	if (error) {
		dev_err(&client->dev, "Failed to read in touchpoint struct\n");
		goto out;
	}

	for (i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++)
		if (touch_event.point_coord[i].sub_status & SUB_BIT_EXIST)
			zinitix_report_finger(bt541, i,
					      &touch_event.point_coord[i]);

	input_mt_sync_frame(bt541->input_dev);
	input_sync(bt541->input_dev);

out:
	zinitix_write_cmd(bt541->client, BT541_CLEAR_INT_STATUS_CMD);
	return IRQ_HANDLED;
}

static int zinitix_start(struct bt541_ts_data *bt541)
{
	int error;

	error = regulator_bulk_enable(ARRAY_SIZE(bt541->supplies),
				      bt541->supplies);
	if (error) {
		dev_err(&bt541->client->dev,
			"Failed to enable regulators: %d\n", error);
		return error;
	}

	msleep(CHIP_ON_DELAY);

	error = zinitix_send_power_on_sequence(bt541);
	if (error) {
		dev_err(&bt541->client->dev,
			"Error while sending power-on sequence: %d\n", error);
		return error;
	}

	error = zinitix_init_touch(bt541);
	if (error) {
		dev_err(&bt541->client->dev,
			"Error while configuring touch IC\n");
		return error;
	}

	enable_irq(bt541->client->irq);

	return 0;
}

static int zinitix_stop(struct bt541_ts_data *bt541)
{
	int error;

	disable_irq(bt541->client->irq);

	error = regulator_bulk_disable(ARRAY_SIZE(bt541->supplies),
				       bt541->supplies);
	if (error) {
		dev_err(&bt541->client->dev,
			"Failed to disable regulators: %d\n", error);
		return error;
	}

	return 0;
}

static int zinitix_input_open(struct input_dev *dev)
{
	struct bt541_ts_data *bt541 = input_get_drvdata(dev);

	return zinitix_start(bt541);
}

static void zinitix_input_close(struct input_dev *dev)
{
	struct bt541_ts_data *bt541 = input_get_drvdata(dev);

	zinitix_stop(bt541);
}

static int zinitix_init_input_dev(struct bt541_ts_data *bt541)
{
	struct input_dev *input_dev;
	int error;

	input_dev = devm_input_allocate_device(&bt541->client->dev);
	if (!input_dev) {
		dev_err(&bt541->client->dev,
			"Failed to allocate input device.");
		return -ENOMEM;
	}

	input_set_drvdata(input_dev, bt541);
	bt541->input_dev = input_dev;

	input_dev->name = "Zinitix Capacitive TouchScreen";
	input_dev->phys = "input/ts";
	input_dev->id.bustype = BUS_I2C;
	input_dev->open = zinitix_input_open;
	input_dev->close = zinitix_input_close;

	input_set_capability(input_dev, EV_ABS, ABS_MT_POSITION_X);
	input_set_capability(input_dev, EV_ABS, ABS_MT_POSITION_Y);
	input_set_abs_params(input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);

	touchscreen_parse_properties(input_dev, true, &bt541->prop);
	if (!bt541->prop.max_x || !bt541->prop.max_y) {
		dev_err(&bt541->client->dev,
			"Touchscreen-size-x and/or touchscreen-size-y not set in dts\n");
		return -EINVAL;
	}

	error = input_mt_init_slots(input_dev, MAX_SUPPORTED_FINGER_NUM,
				    INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (error) {
		dev_err(&bt541->client->dev,
			"Failed to initialize MT slots: %d", error);
		return error;
	}

	error = input_register_device(input_dev);
	if (error) {
		dev_err(&bt541->client->dev,
			"Failed to register input device: %d", error);
		return error;
	}

	return 0;
}

static int zinitix_ts_probe(struct i2c_client *client)
{
	struct bt541_ts_data *bt541;
	int error;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev,
			"Failed to assert adapter's support for plain I2C.\n");
		return -ENXIO;
	}

	bt541 = devm_kzalloc(&client->dev, sizeof(*bt541), GFP_KERNEL);
	if (!bt541)
		return -ENOMEM;

	bt541->client = client;
	i2c_set_clientdata(client, bt541);

	error = zinitix_init_regulators(bt541);
	if (error) {
		dev_err(&client->dev,
			"Failed to initialize regulators: %d\n", error);
		return error;
	}

	error = zinitix_init_input_dev(bt541);
	if (error) {
		dev_err(&client->dev,
			"Failed to initialize input device: %d\n", error);
		return error;
	}

	error = device_property_read_u32(&client->dev, "zinitix,mode",
					 &bt541->zinitix_mode);
	if (error < 0) {
		/* fall back to mode 2 */
		bt541->zinitix_mode = DEFAULT_TOUCH_POINT_MODE;
	}

	if (bt541->zinitix_mode != 2) {
		/*
		 * If there are devices that don't support mode 2, support
		 * for other modes (0, 1) will be needed.
		 */
		dev_err(&client->dev,
			"Malformed zinitix,mode property, must be 2 (supplied: %d)\n",
			bt541->zinitix_mode);
		return -EINVAL;
	}

	error = devm_request_threaded_irq(&client->dev, client->irq,
					  NULL, zinitix_ts_irq_handler,
					  IRQF_ONESHOT | IRQF_NO_AUTOEN,
					  client->name, bt541);
	if (error) {
		dev_err(&client->dev, "Failed to request IRQ: %d\n", error);
		return error;
	}

	return 0;
}

static int __maybe_unused zinitix_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bt541_ts_data *bt541 = i2c_get_clientdata(client);

	mutex_lock(&bt541->input_dev->mutex);

	if (input_device_enabled(bt541->input_dev))
		zinitix_stop(bt541);

	mutex_unlock(&bt541->input_dev->mutex);

	return 0;
}

static int __maybe_unused zinitix_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bt541_ts_data *bt541 = i2c_get_clientdata(client);
	int ret = 0;

	mutex_lock(&bt541->input_dev->mutex);

	if (input_device_enabled(bt541->input_dev))
		ret = zinitix_start(bt541);

	mutex_unlock(&bt541->input_dev->mutex);

	return ret;
}

static SIMPLE_DEV_PM_OPS(zinitix_pm_ops, zinitix_suspend, zinitix_resume);

#ifdef CONFIG_OF
static const struct of_device_id zinitix_of_match[] = {
	{ .compatible = "zinitix,bt541" },
	{ }
};
MODULE_DEVICE_TABLE(of, zinitix_of_match);
#endif

static struct i2c_driver zinitix_ts_driver = {
	.probe_new = zinitix_ts_probe,
	.driver = {
		.name = "Zinitix-TS",
		.pm = &zinitix_pm_ops,
		.of_match_table = of_match_ptr(zinitix_of_match),
	},
};
module_i2c_driver(zinitix_ts_driver);

MODULE_AUTHOR("Michael Srba <Michael.Srba@seznam.cz>");
MODULE_DESCRIPTION("Zinitix touchscreen driver");
MODULE_LICENSE("GPL v2");
