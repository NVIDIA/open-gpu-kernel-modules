// SPDX-License-Identifier: GPL-2.0-only
/*
 * Elan Microelectronics touch panels with I2C interface
 *
 * Copyright (C) 2014 Elan Microelectronics Corporation.
 * Scott Liu <scott.liu@emc.com.tw>
 *
 * This code is partly based on hid-multitouch.c:
 *
 *  Copyright (c) 2010-2012 Stephane Chatty <chatty@enac.fr>
 *  Copyright (c) 2010-2012 Benjamin Tissoires <benjamin.tissoires@gmail.com>
 *  Copyright (c) 2010-2012 Ecole Nationale de l'Aviation Civile, France
 *
 * This code is partly based on i2c-hid.c:
 *
 * Copyright (c) 2012 Benjamin Tissoires <benjamin.tissoires@gmail.com>
 * Copyright (c) 2012 Ecole Nationale de l'Aviation Civile, France
 * Copyright (c) 2012 Red Hat, Inc
 */


#include <linux/bits.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/firmware.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/acpi.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/uuid.h>
#include <asm/unaligned.h>

/* Device, Driver information */
#define DEVICE_NAME	"elants_i2c"

/* Convert from rows or columns into resolution */
#define ELAN_TS_RESOLUTION(n, m)   (((n) - 1) * (m))

/* FW header data */
#define HEADER_SIZE		4
#define FW_HDR_TYPE		0
#define FW_HDR_COUNT		1
#define FW_HDR_LENGTH		2

/* Buffer mode Queue Header information */
#define QUEUE_HEADER_SINGLE	0x62
#define QUEUE_HEADER_NORMAL	0X63
#define QUEUE_HEADER_WAIT	0x64
#define QUEUE_HEADER_NORMAL2	0x66

/* Command header definition */
#define CMD_HEADER_WRITE	0x54
#define CMD_HEADER_READ		0x53
#define CMD_HEADER_6B_READ	0x5B
#define CMD_HEADER_ROM_READ	0x96
#define CMD_HEADER_RESP		0x52
#define CMD_HEADER_6B_RESP	0x9B
#define CMD_HEADER_ROM_RESP	0x95
#define CMD_HEADER_HELLO	0x55
#define CMD_HEADER_REK		0x66

/* FW position data */
#define PACKET_SIZE_OLD		40
#define PACKET_SIZE		55
#define MAX_CONTACT_NUM		10
#define FW_POS_HEADER		0
#define FW_POS_STATE		1
#define FW_POS_TOTAL		2
#define FW_POS_XY		3
#define FW_POS_TOOL_TYPE	33
#define FW_POS_CHECKSUM		34
#define FW_POS_WIDTH		35
#define FW_POS_PRESSURE		45

#define HEADER_REPORT_10_FINGER	0x62

/* Header (4 bytes) plus 3 full 10-finger packets */
#define MAX_PACKET_SIZE		169

#define BOOT_TIME_DELAY_MS	50

/* FW read command, 0x53 0x?? 0x0, 0x01 */
#define E_ELAN_INFO_FW_VER	0x00
#define E_ELAN_INFO_BC_VER	0x10
#define E_ELAN_INFO_X_RES	0x60
#define E_ELAN_INFO_Y_RES	0x63
#define E_ELAN_INFO_REK		0xD0
#define E_ELAN_INFO_TEST_VER	0xE0
#define E_ELAN_INFO_FW_ID	0xF0
#define E_INFO_OSR		0xD6
#define E_INFO_PHY_SCAN		0xD7
#define E_INFO_PHY_DRIVER	0xD8

/* FW write command, 0x54 0x?? 0x0, 0x01 */
#define E_POWER_STATE_SLEEP	0x50
#define E_POWER_STATE_RESUME	0x58

#define MAX_RETRIES		3
#define MAX_FW_UPDATE_RETRIES	30

#define ELAN_FW_PAGESIZE	132

/* calibration timeout definition */
#define ELAN_CALI_TIMEOUT_MSEC	12000

#define ELAN_POWERON_DELAY_USEC	500
#define ELAN_RESET_DELAY_MSEC	20

enum elants_chip_id {
	EKTH3500,
	EKTF3624,
};

enum elants_state {
	ELAN_STATE_NORMAL,
	ELAN_WAIT_QUEUE_HEADER,
	ELAN_WAIT_RECALIBRATION,
};

enum elants_iap_mode {
	ELAN_IAP_OPERATIONAL,
	ELAN_IAP_RECOVERY,
};

/* struct elants_data - represents state of Elan touchscreen device */
struct elants_data {
	struct i2c_client *client;
	struct input_dev *input;

	struct regulator *vcc33;
	struct regulator *vccio;
	struct gpio_desc *reset_gpio;

	u16 fw_version;
	u8 test_version;
	u8 solution_version;
	u8 bc_version;
	u8 iap_version;
	u16 hw_version;
	u8 major_res;
	unsigned int x_res;	/* resolution in units/mm */
	unsigned int y_res;
	unsigned int x_max;
	unsigned int y_max;
	unsigned int phy_x;
	unsigned int phy_y;
	struct touchscreen_properties prop;

	enum elants_state state;
	enum elants_chip_id chip_id;
	enum elants_iap_mode iap_mode;

	/* Guards against concurrent access to the device via sysfs */
	struct mutex sysfs_mutex;

	u8 cmd_resp[HEADER_SIZE];
	struct completion cmd_done;

	bool wake_irq_enabled;
	bool keep_power_in_suspend;

	/* Must be last to be used for DMA operations */
	u8 buf[MAX_PACKET_SIZE] ____cacheline_aligned;
};

static int elants_i2c_send(struct i2c_client *client,
			   const void *data, size_t size)
{
	int ret;

	ret = i2c_master_send(client, data, size);
	if (ret == size)
		return 0;

	if (ret >= 0)
		ret = -EIO;

	dev_err(&client->dev, "%s failed (%*ph): %d\n",
		__func__, (int)size, data, ret);

	return ret;
}

static int elants_i2c_read(struct i2c_client *client, void *data, size_t size)
{
	int ret;

	ret = i2c_master_recv(client, data, size);
	if (ret == size)
		return 0;

	if (ret >= 0)
		ret = -EIO;

	dev_err(&client->dev, "%s failed: %d\n", __func__, ret);

	return ret;
}

static int elants_i2c_execute_command(struct i2c_client *client,
				      const u8 *cmd, size_t cmd_size,
				      u8 *resp, size_t resp_size,
				      int retries, const char *cmd_name)
{
	struct i2c_msg msgs[2];
	int ret;
	u8 expected_response;

	switch (cmd[0]) {
	case CMD_HEADER_READ:
		expected_response = CMD_HEADER_RESP;
		break;

	case CMD_HEADER_6B_READ:
		expected_response = CMD_HEADER_6B_RESP;
		break;

	case CMD_HEADER_ROM_READ:
		expected_response = CMD_HEADER_ROM_RESP;
		break;

	default:
		dev_err(&client->dev, "(%s): invalid command: %*ph\n",
			cmd_name, (int)cmd_size, cmd);
		return -EINVAL;
	}

	for (;;) {
		msgs[0].addr = client->addr;
		msgs[0].flags = client->flags & I2C_M_TEN;
		msgs[0].len = cmd_size;
		msgs[0].buf = (u8 *)cmd;

		msgs[1].addr = client->addr;
		msgs[1].flags = (client->flags & I2C_M_TEN) | I2C_M_RD;
		msgs[1].flags |= I2C_M_RD;
		msgs[1].len = resp_size;
		msgs[1].buf = resp;

		ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
		if (ret < 0) {
			if (--retries > 0) {
				dev_dbg(&client->dev,
					"(%s) I2C transfer failed: %pe (retrying)\n",
					cmd_name, ERR_PTR(ret));
				continue;
			}

			dev_err(&client->dev,
				"(%s) I2C transfer failed: %pe\n",
				cmd_name, ERR_PTR(ret));
			return ret;
		}

		if (ret != ARRAY_SIZE(msgs) ||
		    resp[FW_HDR_TYPE] != expected_response) {
			if (--retries > 0) {
				dev_dbg(&client->dev,
					"(%s) unexpected response: %*ph (retrying)\n",
					cmd_name, ret, resp);
				continue;
			}

			dev_err(&client->dev,
				"(%s) unexpected response: %*ph\n",
				cmd_name, ret, resp);
			return -EIO;
		}

		return 0;
	}
}

static int elants_i2c_calibrate(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int ret, error;
	static const u8 w_flashkey[] = { CMD_HEADER_WRITE, 0xC0, 0xE1, 0x5A };
	static const u8 rek[] = { CMD_HEADER_WRITE, 0x29, 0x00, 0x01 };
	static const u8 rek_resp[] = { CMD_HEADER_REK, 0x66, 0x66, 0x66 };

	disable_irq(client->irq);

	ts->state = ELAN_WAIT_RECALIBRATION;
	reinit_completion(&ts->cmd_done);

	elants_i2c_send(client, w_flashkey, sizeof(w_flashkey));
	elants_i2c_send(client, rek, sizeof(rek));

	enable_irq(client->irq);

	ret = wait_for_completion_interruptible_timeout(&ts->cmd_done,
				msecs_to_jiffies(ELAN_CALI_TIMEOUT_MSEC));

	ts->state = ELAN_STATE_NORMAL;

	if (ret <= 0) {
		error = ret < 0 ? ret : -ETIMEDOUT;
		dev_err(&client->dev,
			"error while waiting for calibration to complete: %d\n",
			error);
		return error;
	}

	if (memcmp(rek_resp, ts->cmd_resp, sizeof(rek_resp))) {
		dev_err(&client->dev,
			"unexpected calibration response: %*ph\n",
			(int)sizeof(ts->cmd_resp), ts->cmd_resp);
		return -EINVAL;
	}

	return 0;
}

static int elants_i2c_sw_reset(struct i2c_client *client)
{
	const u8 soft_rst_cmd[] = { 0x77, 0x77, 0x77, 0x77 };
	int error;

	error = elants_i2c_send(client, soft_rst_cmd,
				sizeof(soft_rst_cmd));
	if (error) {
		dev_err(&client->dev, "software reset failed: %d\n", error);
		return error;
	}

	/*
	 * We should wait at least 10 msec (but no more than 40) before
	 * sending fastboot or IAP command to the device.
	 */
	msleep(30);

	return 0;
}

static u16 elants_i2c_parse_version(u8 *buf)
{
	return get_unaligned_be32(buf) >> 4;
}

static int elants_i2c_query_hw_version(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int retry_cnt = MAX_RETRIES;
	const u8 cmd[] = { CMD_HEADER_READ, E_ELAN_INFO_FW_ID, 0x00, 0x01 };
	u8 resp[HEADER_SIZE];
	int error;

	while (retry_cnt--) {
		error = elants_i2c_execute_command(client, cmd, sizeof(cmd),
						   resp, sizeof(resp), 1,
						   "read fw id");
		if (error)
			return error;

		ts->hw_version = elants_i2c_parse_version(resp);
		if (ts->hw_version != 0xffff)
			return 0;
	}

	dev_err(&client->dev, "Invalid fw id: %#04x\n", ts->hw_version);

	return -EINVAL;
}

static int elants_i2c_query_fw_version(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int retry_cnt = MAX_RETRIES;
	const u8 cmd[] = { CMD_HEADER_READ, E_ELAN_INFO_FW_VER, 0x00, 0x01 };
	u8 resp[HEADER_SIZE];
	int error;

	while (retry_cnt--) {
		error = elants_i2c_execute_command(client, cmd, sizeof(cmd),
						   resp, sizeof(resp), 1,
						   "read fw version");
		if (error)
			return error;

		ts->fw_version = elants_i2c_parse_version(resp);
		if (ts->fw_version != 0x0000 && ts->fw_version != 0xffff)
			return 0;

		dev_dbg(&client->dev, "(read fw version) resp %*phC\n",
			(int)sizeof(resp), resp);
	}

	dev_err(&client->dev, "Invalid fw ver: %#04x\n", ts->fw_version);

	return -EINVAL;
}

static int elants_i2c_query_test_version(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int error;
	u16 version;
	const u8 cmd[] = { CMD_HEADER_READ, E_ELAN_INFO_TEST_VER, 0x00, 0x01 };
	u8 resp[HEADER_SIZE];

	error = elants_i2c_execute_command(client, cmd, sizeof(cmd),
					   resp, sizeof(resp), MAX_RETRIES,
					   "read test version");
	if (error) {
		dev_err(&client->dev, "Failed to read test version\n");
		return error;
	}

	version = elants_i2c_parse_version(resp);
	ts->test_version = version >> 8;
	ts->solution_version = version & 0xff;

	return 0;
}

static int elants_i2c_query_bc_version(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	const u8 cmd[] = { CMD_HEADER_READ, E_ELAN_INFO_BC_VER, 0x00, 0x01 };
	u8 resp[HEADER_SIZE];
	u16 version;
	int error;

	error = elants_i2c_execute_command(client, cmd, sizeof(cmd),
					   resp, sizeof(resp), 1,
					   "read BC version");
	if (error)
		return error;

	version = elants_i2c_parse_version(resp);
	ts->bc_version = version >> 8;
	ts->iap_version = version & 0xff;

	return 0;
}

static int elants_i2c_query_ts_info_ektf(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int error;
	u8 resp[4];
	u16 phy_x, phy_y;
	const u8 get_xres_cmd[] = {
		CMD_HEADER_READ, E_ELAN_INFO_X_RES, 0x00, 0x00
	};
	const u8 get_yres_cmd[] = {
		CMD_HEADER_READ, E_ELAN_INFO_Y_RES, 0x00, 0x00
	};

	/* Get X/Y size in mm */
	error = elants_i2c_execute_command(client, get_xres_cmd,
					   sizeof(get_xres_cmd),
					   resp, sizeof(resp), 1,
					   "get X size");
	if (error)
		return error;

	phy_x = resp[2] | ((resp[3] & 0xF0) << 4);

	error = elants_i2c_execute_command(client, get_yres_cmd,
					   sizeof(get_yres_cmd),
					   resp, sizeof(resp), 1,
					   "get Y size");
	if (error)
		return error;

	phy_y = resp[2] | ((resp[3] & 0xF0) << 4);

	dev_dbg(&client->dev, "phy_x=%d, phy_y=%d\n", phy_x, phy_y);

	ts->phy_x = phy_x;
	ts->phy_y = phy_y;

	/* eKTF doesn't report max size, set it to default values */
	ts->x_max = 2240 - 1;
	ts->y_max = 1408 - 1;

	return 0;
}

static int elants_i2c_query_ts_info_ekth(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int error;
	u8 resp[17];
	u16 phy_x, phy_y, rows, cols, osr;
	const u8 get_resolution_cmd[] = {
		CMD_HEADER_6B_READ, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	const u8 get_osr_cmd[] = {
		CMD_HEADER_READ, E_INFO_OSR, 0x00, 0x01
	};
	const u8 get_physical_scan_cmd[] = {
		CMD_HEADER_READ, E_INFO_PHY_SCAN, 0x00, 0x01
	};
	const u8 get_physical_drive_cmd[] = {
		CMD_HEADER_READ, E_INFO_PHY_DRIVER, 0x00, 0x01
	};

	/* Get trace number */
	error = elants_i2c_execute_command(client,
					   get_resolution_cmd,
					   sizeof(get_resolution_cmd),
					   resp, sizeof(resp), 1,
					   "get resolution");
	if (error)
		return error;

	rows = resp[2] + resp[6] + resp[10];
	cols = resp[3] + resp[7] + resp[11];

	/* Get report resolution value of ABS_MT_TOUCH_MAJOR */
	ts->major_res = resp[16];

	/* Process mm_to_pixel information */
	error = elants_i2c_execute_command(client,
					   get_osr_cmd, sizeof(get_osr_cmd),
					   resp, sizeof(resp), 1, "get osr");
	if (error)
		return error;

	osr = resp[3];

	error = elants_i2c_execute_command(client,
					   get_physical_scan_cmd,
					   sizeof(get_physical_scan_cmd),
					   resp, sizeof(resp), 1,
					   "get physical scan");
	if (error)
		return error;

	phy_x = get_unaligned_be16(&resp[2]);

	error = elants_i2c_execute_command(client,
					   get_physical_drive_cmd,
					   sizeof(get_physical_drive_cmd),
					   resp, sizeof(resp), 1,
					   "get physical drive");
	if (error)
		return error;

	phy_y = get_unaligned_be16(&resp[2]);

	dev_dbg(&client->dev, "phy_x=%d, phy_y=%d\n", phy_x, phy_y);

	if (rows == 0 || cols == 0 || osr == 0) {
		dev_warn(&client->dev,
			 "invalid trace number data: %d, %d, %d\n",
			 rows, cols, osr);
	} else {
		/* translate trace number to TS resolution */
		ts->x_max = ELAN_TS_RESOLUTION(rows, osr);
		ts->x_res = DIV_ROUND_CLOSEST(ts->x_max, phy_x);
		ts->y_max = ELAN_TS_RESOLUTION(cols, osr);
		ts->y_res = DIV_ROUND_CLOSEST(ts->y_max, phy_y);
		ts->phy_x = phy_x;
		ts->phy_y = phy_y;
	}

	return 0;
}

static int elants_i2c_fastboot(struct i2c_client *client)
{
	const u8 boot_cmd[] = { 0x4D, 0x61, 0x69, 0x6E };
	int error;

	error = elants_i2c_send(client, boot_cmd, sizeof(boot_cmd));
	if (error) {
		dev_err(&client->dev, "boot failed: %d\n", error);
		return error;
	}

	dev_dbg(&client->dev, "boot success -- 0x%x\n", client->addr);
	return 0;
}

static int elants_i2c_initialize(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	int error, error2, retry_cnt;
	const u8 hello_packet[] = { 0x55, 0x55, 0x55, 0x55 };
	const u8 recov_packet[] = { 0x55, 0x55, 0x80, 0x80 };
	u8 buf[HEADER_SIZE];

	for (retry_cnt = 0; retry_cnt < MAX_RETRIES; retry_cnt++) {
		error = elants_i2c_sw_reset(client);
		if (error) {
			/* Continue initializing if it's the last try */
			if (retry_cnt < MAX_RETRIES - 1)
				continue;
		}

		error = elants_i2c_fastboot(client);
		if (error) {
			/* Continue initializing if it's the last try */
			if (retry_cnt < MAX_RETRIES - 1)
				continue;
		}

		/* Wait for Hello packet */
		msleep(BOOT_TIME_DELAY_MS);

		error = elants_i2c_read(client, buf, sizeof(buf));
		if (error) {
			dev_err(&client->dev,
				"failed to read 'hello' packet: %d\n", error);
		} else if (!memcmp(buf, hello_packet, sizeof(hello_packet))) {
			ts->iap_mode = ELAN_IAP_OPERATIONAL;
			break;
		} else if (!memcmp(buf, recov_packet, sizeof(recov_packet))) {
			/*
			 * Setting error code will mark device
			 * in recovery mode below.
			 */
			error = -EIO;
			break;
		} else {
			error = -EINVAL;
			dev_err(&client->dev,
				"invalid 'hello' packet: %*ph\n",
				(int)sizeof(buf), buf);
		}
	}

	/* hw version is available even if device in recovery state */
	error2 = elants_i2c_query_hw_version(ts);
	if (!error2)
		error2 = elants_i2c_query_bc_version(ts);
	if (!error)
		error = error2;

	if (!error)
		error = elants_i2c_query_fw_version(ts);
	if (!error)
		error = elants_i2c_query_test_version(ts);

	switch (ts->chip_id) {
	case EKTH3500:
		if (!error)
			error = elants_i2c_query_ts_info_ekth(ts);
		break;
	case EKTF3624:
		if (!error)
			error = elants_i2c_query_ts_info_ektf(ts);
		break;
	default:
		BUG();
	}

	if (error)
		ts->iap_mode = ELAN_IAP_RECOVERY;

	return 0;
}

/*
 * Firmware update interface.
 */

static int elants_i2c_fw_write_page(struct i2c_client *client,
				    const void *page)
{
	const u8 ack_ok[] = { 0xaa, 0xaa };
	u8 buf[2];
	int retry;
	int error;

	for (retry = 0; retry < MAX_FW_UPDATE_RETRIES; retry++) {
		error = elants_i2c_send(client, page, ELAN_FW_PAGESIZE);
		if (error) {
			dev_err(&client->dev,
				"IAP Write Page failed: %d\n", error);
			continue;
		}

		error = elants_i2c_read(client, buf, 2);
		if (error) {
			dev_err(&client->dev,
				"IAP Ack read failed: %d\n", error);
			return error;
		}

		if (!memcmp(buf, ack_ok, sizeof(ack_ok)))
			return 0;

		error = -EIO;
		dev_err(&client->dev,
			"IAP Get Ack Error [%02x:%02x]\n",
			buf[0], buf[1]);
	}

	return error;
}

static int elants_i2c_validate_remark_id(struct elants_data *ts,
					 const struct firmware *fw)
{
	struct i2c_client *client = ts->client;
	int error;
	const u8 cmd[] = { CMD_HEADER_ROM_READ, 0x80, 0x1F, 0x00, 0x00, 0x21 };
	u8 resp[6] = { 0 };
	u16 ts_remark_id = 0;
	u16 fw_remark_id = 0;

	/* Compare TS Remark ID and FW Remark ID */
	error = elants_i2c_execute_command(client, cmd, sizeof(cmd),
					   resp, sizeof(resp),
					   1, "read Remark ID");
	if (error)
		return error;

	ts_remark_id = get_unaligned_be16(&resp[3]);

	fw_remark_id = get_unaligned_le16(&fw->data[fw->size - 4]);

	if (fw_remark_id != ts_remark_id) {
		dev_err(&client->dev,
			"Remark ID Mismatched: ts_remark_id=0x%04x, fw_remark_id=0x%04x.\n",
			ts_remark_id, fw_remark_id);
		return -EINVAL;
	}

	return 0;
}

static int elants_i2c_do_update_firmware(struct i2c_client *client,
					 const struct firmware *fw,
					 bool force)
{
	struct elants_data *ts = i2c_get_clientdata(client);
	const u8 enter_iap[] = { 0x45, 0x49, 0x41, 0x50 };
	const u8 enter_iap2[] = { 0x54, 0x00, 0x12, 0x34 };
	const u8 iap_ack[] = { 0x55, 0xaa, 0x33, 0xcc };
	const u8 close_idle[] = { 0x54, 0x2c, 0x01, 0x01 };
	u8 buf[HEADER_SIZE];
	u16 send_id;
	int page, n_fw_pages;
	int error;
	bool check_remark_id = ts->iap_version >= 0x60;

	/* Recovery mode detection! */
	if (force) {
		dev_dbg(&client->dev, "Recovery mode procedure\n");

		if (check_remark_id) {
			error = elants_i2c_validate_remark_id(ts, fw);
			if (error)
				return error;
		}

		error = elants_i2c_send(client, enter_iap2, sizeof(enter_iap2));
		if (error) {
			dev_err(&client->dev, "failed to enter IAP mode: %d\n",
				error);
			return error;
		}
	} else {
		/* Start IAP Procedure */
		dev_dbg(&client->dev, "Normal IAP procedure\n");

		/* Close idle mode */
		error = elants_i2c_send(client, close_idle, sizeof(close_idle));
		if (error)
			dev_err(&client->dev, "Failed close idle: %d\n", error);
		msleep(60);

		elants_i2c_sw_reset(client);
		msleep(20);

		if (check_remark_id) {
			error = elants_i2c_validate_remark_id(ts, fw);
			if (error)
				return error;
		}

		error = elants_i2c_send(client, enter_iap, sizeof(enter_iap));
		if (error) {
			dev_err(&client->dev, "failed to enter IAP mode: %d\n",
				error);
			return error;
		}
	}

	msleep(20);

	/* check IAP state */
	error = elants_i2c_read(client, buf, 4);
	if (error) {
		dev_err(&client->dev,
			"failed to read IAP acknowledgement: %d\n",
			error);
		return error;
	}

	if (memcmp(buf, iap_ack, sizeof(iap_ack))) {
		dev_err(&client->dev,
			"failed to enter IAP: %*ph (expected %*ph)\n",
			(int)sizeof(buf), buf, (int)sizeof(iap_ack), iap_ack);
		return -EIO;
	}

	dev_info(&client->dev, "successfully entered IAP mode");

	send_id = client->addr;
	error = elants_i2c_send(client, &send_id, 1);
	if (error) {
		dev_err(&client->dev, "sending dummy byte failed: %d\n",
			error);
		return error;
	}

	/* Clear the last page of Master */
	error = elants_i2c_send(client, fw->data, ELAN_FW_PAGESIZE);
	if (error) {
		dev_err(&client->dev, "clearing of the last page failed: %d\n",
			error);
		return error;
	}

	error = elants_i2c_read(client, buf, 2);
	if (error) {
		dev_err(&client->dev,
			"failed to read ACK for clearing the last page: %d\n",
			error);
		return error;
	}

	n_fw_pages = fw->size / ELAN_FW_PAGESIZE;
	dev_dbg(&client->dev, "IAP Pages = %d\n", n_fw_pages);

	for (page = 0; page < n_fw_pages; page++) {
		error = elants_i2c_fw_write_page(client,
					fw->data + page * ELAN_FW_PAGESIZE);
		if (error) {
			dev_err(&client->dev,
				"failed to write FW page %d: %d\n",
				page, error);
			return error;
		}
	}

	/* Old iap needs to wait 200ms for WDT and rest is for hello packets */
	msleep(300);

	dev_info(&client->dev, "firmware update completed\n");
	return 0;
}

static int elants_i2c_fw_update(struct elants_data *ts)
{
	struct i2c_client *client = ts->client;
	const struct firmware *fw;
	char *fw_name;
	int error;

	fw_name = kasprintf(GFP_KERNEL, "elants_i2c_%04x.bin", ts->hw_version);
	if (!fw_name)
		return -ENOMEM;

	dev_info(&client->dev, "requesting fw name = %s\n", fw_name);
	error = request_firmware(&fw, fw_name, &client->dev);
	kfree(fw_name);
	if (error) {
		dev_err(&client->dev, "failed to request firmware: %d\n",
			error);
		return error;
	}

	if (fw->size % ELAN_FW_PAGESIZE) {
		dev_err(&client->dev, "invalid firmware length: %zu\n",
			fw->size);
		error = -EINVAL;
		goto out;
	}

	disable_irq(client->irq);

	error = elants_i2c_do_update_firmware(client, fw,
					ts->iap_mode == ELAN_IAP_RECOVERY);
	if (error) {
		dev_err(&client->dev, "firmware update failed: %d\n", error);
		ts->iap_mode = ELAN_IAP_RECOVERY;
		goto out_enable_irq;
	}

	error = elants_i2c_initialize(ts);
	if (error) {
		dev_err(&client->dev,
			"failed to initialize device after firmware update: %d\n",
			error);
		ts->iap_mode = ELAN_IAP_RECOVERY;
		goto out_enable_irq;
	}

	ts->iap_mode = ELAN_IAP_OPERATIONAL;

out_enable_irq:
	ts->state = ELAN_STATE_NORMAL;
	enable_irq(client->irq);
	msleep(100);

	if (!error)
		elants_i2c_calibrate(ts);
out:
	release_firmware(fw);
	return error;
}

/*
 * Event reporting.
 */

static void elants_i2c_mt_event(struct elants_data *ts, u8 *buf,
				size_t packet_size)
{
	struct input_dev *input = ts->input;
	unsigned int n_fingers;
	unsigned int tool_type;
	u16 finger_state;
	int i;

	n_fingers = buf[FW_POS_STATE + 1] & 0x0f;
	finger_state = ((buf[FW_POS_STATE + 1] & 0x30) << 4) |
			buf[FW_POS_STATE];

	dev_dbg(&ts->client->dev,
		"n_fingers: %u, state: %04x\n",  n_fingers, finger_state);

	/* Note: all fingers have the same tool type */
	tool_type = buf[FW_POS_TOOL_TYPE] & BIT(0) ?
			MT_TOOL_FINGER : MT_TOOL_PALM;

	for (i = 0; i < MAX_CONTACT_NUM && n_fingers; i++) {
		if (finger_state & 1) {
			unsigned int x, y, p, w;
			u8 *pos;

			pos = &buf[FW_POS_XY + i * 3];
			x = (((u16)pos[0] & 0xf0) << 4) | pos[1];
			y = (((u16)pos[0] & 0x0f) << 8) | pos[2];

			/*
			 * eKTF3624 may have use "old" touch-report format,
			 * depending on a device and TS firmware version.
			 * For example, ASUS Transformer devices use the "old"
			 * format, while ASUS Nexus 7 uses the "new" formant.
			 */
			if (packet_size == PACKET_SIZE_OLD &&
			    ts->chip_id == EKTF3624) {
				w = buf[FW_POS_WIDTH + i / 2];
				w >>= 4 * (~i & 1);
				w |= w << 4;
				w |= !w;
				p = w;
			} else {
				p = buf[FW_POS_PRESSURE + i];
				w = buf[FW_POS_WIDTH + i];
			}

			dev_dbg(&ts->client->dev, "i=%d x=%d y=%d p=%d w=%d\n",
				i, x, y, p, w);

			input_mt_slot(input, i);
			input_mt_report_slot_state(input, tool_type, true);
			touchscreen_report_pos(input, &ts->prop, x, y, true);
			input_event(input, EV_ABS, ABS_MT_PRESSURE, p);
			input_event(input, EV_ABS, ABS_MT_TOUCH_MAJOR, w);

			n_fingers--;
		}

		finger_state >>= 1;
	}

	input_mt_sync_frame(input);
	input_sync(input);
}

static u8 elants_i2c_calculate_checksum(u8 *buf)
{
	u8 checksum = 0;
	u8 i;

	for (i = 0; i < FW_POS_CHECKSUM; i++)
		checksum += buf[i];

	return checksum;
}

static void elants_i2c_event(struct elants_data *ts, u8 *buf,
			     size_t packet_size)
{
	u8 checksum = elants_i2c_calculate_checksum(buf);

	if (unlikely(buf[FW_POS_CHECKSUM] != checksum))
		dev_warn(&ts->client->dev,
			 "%s: invalid checksum for packet %02x: %02x vs. %02x\n",
			 __func__, buf[FW_POS_HEADER],
			 checksum, buf[FW_POS_CHECKSUM]);
	else if (unlikely(buf[FW_POS_HEADER] != HEADER_REPORT_10_FINGER))
		dev_warn(&ts->client->dev,
			 "%s: unknown packet type: %02x\n",
			 __func__, buf[FW_POS_HEADER]);
	else
		elants_i2c_mt_event(ts, buf, packet_size);
}

static irqreturn_t elants_i2c_irq(int irq, void *_dev)
{
	const u8 wait_packet[] = { 0x64, 0x64, 0x64, 0x64 };
	struct elants_data *ts = _dev;
	struct i2c_client *client = ts->client;
	int report_count, report_len;
	int i;
	int len;

	len = i2c_master_recv_dmasafe(client, ts->buf, sizeof(ts->buf));
	if (len < 0) {
		dev_err(&client->dev, "%s: failed to read data: %d\n",
			__func__, len);
		goto out;
	}

	dev_dbg(&client->dev, "%s: packet %*ph\n",
		__func__, HEADER_SIZE, ts->buf);

	switch (ts->state) {
	case ELAN_WAIT_RECALIBRATION:
		if (ts->buf[FW_HDR_TYPE] == CMD_HEADER_REK) {
			memcpy(ts->cmd_resp, ts->buf, sizeof(ts->cmd_resp));
			complete(&ts->cmd_done);
			ts->state = ELAN_STATE_NORMAL;
		}
		break;

	case ELAN_WAIT_QUEUE_HEADER:
		if (ts->buf[FW_HDR_TYPE] != QUEUE_HEADER_NORMAL)
			break;

		ts->state = ELAN_STATE_NORMAL;
		fallthrough;

	case ELAN_STATE_NORMAL:

		switch (ts->buf[FW_HDR_TYPE]) {
		case CMD_HEADER_HELLO:
		case CMD_HEADER_RESP:
			break;

		case QUEUE_HEADER_WAIT:
			if (memcmp(ts->buf, wait_packet, sizeof(wait_packet))) {
				dev_err(&client->dev,
					"invalid wait packet %*ph\n",
					HEADER_SIZE, ts->buf);
			} else {
				ts->state = ELAN_WAIT_QUEUE_HEADER;
				udelay(30);
			}
			break;

		case QUEUE_HEADER_SINGLE:
			elants_i2c_event(ts, &ts->buf[HEADER_SIZE],
					 ts->buf[FW_HDR_LENGTH]);
			break;

		case QUEUE_HEADER_NORMAL2: /* CMD_HEADER_REK */
			/*
			 * Depending on firmware version, eKTF3624 touchscreens
			 * may utilize one of these opcodes for the touch events:
			 * 0x63 (NORMAL) and 0x66 (NORMAL2).  The 0x63 is used by
			 * older firmware version and differs from 0x66 such that
			 * touch pressure value needs to be adjusted.  The 0x66
			 * opcode of newer firmware is equal to 0x63 of eKTH3500.
			 */
			if (ts->chip_id != EKTF3624)
				break;

			fallthrough;

		case QUEUE_HEADER_NORMAL:
			report_count = ts->buf[FW_HDR_COUNT];
			if (report_count == 0 || report_count > 3) {
				dev_err(&client->dev,
					"bad report count: %*ph\n",
					HEADER_SIZE, ts->buf);
				break;
			}

			report_len = ts->buf[FW_HDR_LENGTH] / report_count;

			if (report_len == PACKET_SIZE_OLD &&
			    ts->chip_id == EKTF3624) {
				dev_dbg_once(&client->dev,
					     "using old report format\n");
			} else if (report_len != PACKET_SIZE) {
				dev_err(&client->dev,
					"mismatching report length: %*ph\n",
					HEADER_SIZE, ts->buf);
				break;
			}

			for (i = 0; i < report_count; i++) {
				u8 *buf = ts->buf + HEADER_SIZE +
							i * report_len;
				elants_i2c_event(ts, buf, report_len);
			}
			break;

		default:
			dev_err(&client->dev, "unknown packet %*ph\n",
				HEADER_SIZE, ts->buf);
			break;
		}
		break;
	}

out:
	return IRQ_HANDLED;
}

/*
 * sysfs interface
 */
static ssize_t calibrate_store(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct elants_data *ts = i2c_get_clientdata(client);
	int error;

	error = mutex_lock_interruptible(&ts->sysfs_mutex);
	if (error)
		return error;

	error = elants_i2c_calibrate(ts);

	mutex_unlock(&ts->sysfs_mutex);
	return error ?: count;
}

static ssize_t write_update_fw(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct elants_data *ts = i2c_get_clientdata(client);
	int error;

	error = mutex_lock_interruptible(&ts->sysfs_mutex);
	if (error)
		return error;

	error = elants_i2c_fw_update(ts);
	dev_dbg(dev, "firmware update result: %d\n", error);

	mutex_unlock(&ts->sysfs_mutex);
	return error ?: count;
}

static ssize_t show_iap_mode(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct elants_data *ts = i2c_get_clientdata(client);

	return sprintf(buf, "%s\n",
		       ts->iap_mode == ELAN_IAP_OPERATIONAL ?
				"Normal" : "Recovery");
}

static ssize_t show_calibration_count(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	const u8 cmd[] = { CMD_HEADER_READ, E_ELAN_INFO_REK, 0x00, 0x01 };
	u8 resp[HEADER_SIZE];
	u16 rek_count;
	int error;

	error = elants_i2c_execute_command(client, cmd, sizeof(cmd),
					   resp, sizeof(resp), 1,
					   "read ReK status");
	if (error)
		return sprintf(buf, "%d\n", error);

	rek_count = get_unaligned_be16(&resp[2]);
	return sprintf(buf, "0x%04x\n", rek_count);
}

static DEVICE_ATTR_WO(calibrate);
static DEVICE_ATTR(iap_mode, S_IRUGO, show_iap_mode, NULL);
static DEVICE_ATTR(calibration_count, S_IRUGO, show_calibration_count, NULL);
static DEVICE_ATTR(update_fw, S_IWUSR, NULL, write_update_fw);

struct elants_version_attribute {
	struct device_attribute dattr;
	size_t field_offset;
	size_t field_size;
};

#define __ELANTS_FIELD_SIZE(_field)					\
	sizeof(((struct elants_data *)NULL)->_field)
#define __ELANTS_VERIFY_SIZE(_field)					\
	(BUILD_BUG_ON_ZERO(__ELANTS_FIELD_SIZE(_field) > 2) +		\
	 __ELANTS_FIELD_SIZE(_field))
#define ELANTS_VERSION_ATTR(_field)					\
	struct elants_version_attribute elants_ver_attr_##_field = {	\
		.dattr = __ATTR(_field, S_IRUGO,			\
				elants_version_attribute_show, NULL),	\
		.field_offset = offsetof(struct elants_data, _field),	\
		.field_size = __ELANTS_VERIFY_SIZE(_field),		\
	}

static ssize_t elants_version_attribute_show(struct device *dev,
					     struct device_attribute *dattr,
					     char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct elants_data *ts = i2c_get_clientdata(client);
	struct elants_version_attribute *attr =
		container_of(dattr, struct elants_version_attribute, dattr);
	u8 *field = (u8 *)((char *)ts + attr->field_offset);
	unsigned int fmt_size;
	unsigned int val;

	if (attr->field_size == 1) {
		val = *field;
		fmt_size = 2; /* 2 HEX digits */
	} else {
		val = *(u16 *)field;
		fmt_size = 4; /* 4 HEX digits */
	}

	return sprintf(buf, "%0*x\n", fmt_size, val);
}

static ELANTS_VERSION_ATTR(fw_version);
static ELANTS_VERSION_ATTR(hw_version);
static ELANTS_VERSION_ATTR(test_version);
static ELANTS_VERSION_ATTR(solution_version);
static ELANTS_VERSION_ATTR(bc_version);
static ELANTS_VERSION_ATTR(iap_version);

static struct attribute *elants_attributes[] = {
	&dev_attr_calibrate.attr,
	&dev_attr_update_fw.attr,
	&dev_attr_iap_mode.attr,
	&dev_attr_calibration_count.attr,

	&elants_ver_attr_fw_version.dattr.attr,
	&elants_ver_attr_hw_version.dattr.attr,
	&elants_ver_attr_test_version.dattr.attr,
	&elants_ver_attr_solution_version.dattr.attr,
	&elants_ver_attr_bc_version.dattr.attr,
	&elants_ver_attr_iap_version.dattr.attr,
	NULL
};

static const struct attribute_group elants_attribute_group = {
	.attrs = elants_attributes,
};

static int elants_i2c_power_on(struct elants_data *ts)
{
	int error;

	/*
	 * If we do not have reset gpio assume platform firmware
	 * controls regulators and does power them on for us.
	 */
	if (IS_ERR_OR_NULL(ts->reset_gpio))
		return 0;

	gpiod_set_value_cansleep(ts->reset_gpio, 1);

	error = regulator_enable(ts->vcc33);
	if (error) {
		dev_err(&ts->client->dev,
			"failed to enable vcc33 regulator: %d\n",
			error);
		goto release_reset_gpio;
	}

	error = regulator_enable(ts->vccio);
	if (error) {
		dev_err(&ts->client->dev,
			"failed to enable vccio regulator: %d\n",
			error);
		regulator_disable(ts->vcc33);
		goto release_reset_gpio;
	}

	/*
	 * We need to wait a bit after powering on controller before
	 * we are allowed to release reset GPIO.
	 */
	udelay(ELAN_POWERON_DELAY_USEC);

release_reset_gpio:
	gpiod_set_value_cansleep(ts->reset_gpio, 0);
	if (error)
		return error;

	msleep(ELAN_RESET_DELAY_MSEC);

	return 0;
}

static void elants_i2c_power_off(void *_data)
{
	struct elants_data *ts = _data;

	if (!IS_ERR_OR_NULL(ts->reset_gpio)) {
		/*
		 * Activate reset gpio to prevent leakage through the
		 * pin once we shut off power to the controller.
		 */
		gpiod_set_value_cansleep(ts->reset_gpio, 1);
		regulator_disable(ts->vccio);
		regulator_disable(ts->vcc33);
	}
}

#ifdef CONFIG_ACPI
static const struct acpi_device_id i2c_hid_ids[] = {
	{"ACPI0C50", 0 },
	{"PNP0C50", 0 },
	{ },
};

static const guid_t i2c_hid_guid =
	GUID_INIT(0x3CDFF6F7, 0x4267, 0x4555,
		  0xAD, 0x05, 0xB3, 0x0A, 0x3D, 0x89, 0x38, 0xDE);

static bool elants_acpi_is_hid_device(struct device *dev)
{
	acpi_handle handle = ACPI_HANDLE(dev);
	union acpi_object *obj;

	if (acpi_match_device_ids(ACPI_COMPANION(dev), i2c_hid_ids))
		return false;

	obj = acpi_evaluate_dsm_typed(handle, &i2c_hid_guid, 1, 1, NULL, ACPI_TYPE_INTEGER);
	if (obj) {
		ACPI_FREE(obj);
		return true;
	}

	return false;
}
#else
static bool elants_acpi_is_hid_device(struct device *dev)
{
	return false;
}
#endif

static int elants_i2c_probe(struct i2c_client *client,
			    const struct i2c_device_id *id)
{
	union i2c_smbus_data dummy;
	struct elants_data *ts;
	unsigned long irqflags;
	int error;

	/* Don't bind to i2c-hid compatible devices, these are handled by the i2c-hid drv. */
	if (elants_acpi_is_hid_device(&client->dev)) {
		dev_warn(&client->dev, "This device appears to be an I2C-HID device, not binding\n");
		return -ENODEV;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C check functionality error\n");
		return -ENXIO;
	}

	ts = devm_kzalloc(&client->dev, sizeof(struct elants_data), GFP_KERNEL);
	if (!ts)
		return -ENOMEM;

	mutex_init(&ts->sysfs_mutex);
	init_completion(&ts->cmd_done);

	ts->client = client;
	ts->chip_id = (enum elants_chip_id)id->driver_data;
	i2c_set_clientdata(client, ts);

	ts->vcc33 = devm_regulator_get(&client->dev, "vcc33");
	if (IS_ERR(ts->vcc33)) {
		error = PTR_ERR(ts->vcc33);
		if (error != -EPROBE_DEFER)
			dev_err(&client->dev,
				"Failed to get 'vcc33' regulator: %d\n",
				error);
		return error;
	}

	ts->vccio = devm_regulator_get(&client->dev, "vccio");
	if (IS_ERR(ts->vccio)) {
		error = PTR_ERR(ts->vccio);
		if (error != -EPROBE_DEFER)
			dev_err(&client->dev,
				"Failed to get 'vccio' regulator: %d\n",
				error);
		return error;
	}

	ts->reset_gpio = devm_gpiod_get(&client->dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ts->reset_gpio)) {
		error = PTR_ERR(ts->reset_gpio);

		if (error == -EPROBE_DEFER)
			return error;

		if (error != -ENOENT && error != -ENOSYS) {
			dev_err(&client->dev,
				"failed to get reset gpio: %d\n",
				error);
			return error;
		}

		ts->keep_power_in_suspend = true;
	}

	error = elants_i2c_power_on(ts);
	if (error)
		return error;

	error = devm_add_action(&client->dev, elants_i2c_power_off, ts);
	if (error) {
		dev_err(&client->dev,
			"failed to install power off action: %d\n", error);
		elants_i2c_power_off(ts);
		return error;
	}

	/* Make sure there is something at this address */
	if (i2c_smbus_xfer(client->adapter, client->addr, 0,
			   I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &dummy) < 0) {
		dev_err(&client->dev, "nothing at this address\n");
		return -ENXIO;
	}

	error = elants_i2c_initialize(ts);
	if (error) {
		dev_err(&client->dev, "failed to initialize: %d\n", error);
		return error;
	}

	ts->input = devm_input_allocate_device(&client->dev);
	if (!ts->input) {
		dev_err(&client->dev, "Failed to allocate input device\n");
		return -ENOMEM;
	}

	ts->input->name = "Elan Touchscreen";
	ts->input->id.bustype = BUS_I2C;

	/* Multitouch input params setup */

	input_set_abs_params(ts->input, ABS_MT_POSITION_X, 0, ts->x_max, 0, 0);
	input_set_abs_params(ts->input, ABS_MT_POSITION_Y, 0, ts->y_max, 0, 0);
	input_set_abs_params(ts->input, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(ts->input, ABS_MT_PRESSURE, 0, 255, 0, 0);
	input_set_abs_params(ts->input, ABS_MT_TOOL_TYPE,
			     0, MT_TOOL_PALM, 0, 0);

	touchscreen_parse_properties(ts->input, true, &ts->prop);

	if (ts->chip_id == EKTF3624 && ts->phy_x && ts->phy_y) {
		/* calculate resolution from size */
		ts->x_res = DIV_ROUND_CLOSEST(ts->prop.max_x, ts->phy_x);
		ts->y_res = DIV_ROUND_CLOSEST(ts->prop.max_y, ts->phy_y);
	}

	input_abs_set_res(ts->input, ABS_MT_POSITION_X, ts->x_res);
	input_abs_set_res(ts->input, ABS_MT_POSITION_Y, ts->y_res);
	input_abs_set_res(ts->input, ABS_MT_TOUCH_MAJOR, ts->major_res);

	error = input_mt_init_slots(ts->input, MAX_CONTACT_NUM,
				    INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (error) {
		dev_err(&client->dev,
			"failed to initialize MT slots: %d\n", error);
		return error;
	}

	error = input_register_device(ts->input);
	if (error) {
		dev_err(&client->dev,
			"unable to register input device: %d\n", error);
		return error;
	}

	/*
	 * Platform code (ACPI, DTS) should normally set up interrupt
	 * for us, but in case it did not let's fall back to using falling
	 * edge to be compatible with older Chromebooks.
	 */
	irqflags = irq_get_trigger_type(client->irq);
	if (!irqflags)
		irqflags = IRQF_TRIGGER_FALLING;

	error = devm_request_threaded_irq(&client->dev, client->irq,
					  NULL, elants_i2c_irq,
					  irqflags | IRQF_ONESHOT,
					  client->name, ts);
	if (error) {
		dev_err(&client->dev, "Failed to register interrupt\n");
		return error;
	}

	/*
	 * Systems using device tree should set up wakeup via DTS,
	 * the rest will configure device as wakeup source by default.
	 */
	if (!client->dev.of_node)
		device_init_wakeup(&client->dev, true);

	error = devm_device_add_group(&client->dev, &elants_attribute_group);
	if (error) {
		dev_err(&client->dev, "failed to create sysfs attributes: %d\n",
			error);
		return error;
	}

	return 0;
}

static int __maybe_unused elants_i2c_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct elants_data *ts = i2c_get_clientdata(client);
	const u8 set_sleep_cmd[] = {
		CMD_HEADER_WRITE, E_POWER_STATE_SLEEP, 0x00, 0x01
	};
	int retry_cnt;
	int error;

	/* Command not support in IAP recovery mode */
	if (ts->iap_mode != ELAN_IAP_OPERATIONAL)
		return -EBUSY;

	disable_irq(client->irq);

	if (device_may_wakeup(dev)) {
		/*
		 * The device will automatically enter idle mode
		 * that has reduced power consumption.
		 */
		ts->wake_irq_enabled = (enable_irq_wake(client->irq) == 0);
	} else if (ts->keep_power_in_suspend) {
		for (retry_cnt = 0; retry_cnt < MAX_RETRIES; retry_cnt++) {
			error = elants_i2c_send(client, set_sleep_cmd,
						sizeof(set_sleep_cmd));
			if (!error)
				break;

			dev_err(&client->dev,
				"suspend command failed: %d\n", error);
		}
	} else {
		elants_i2c_power_off(ts);
	}

	return 0;
}

static int __maybe_unused elants_i2c_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct elants_data *ts = i2c_get_clientdata(client);
	const u8 set_active_cmd[] = {
		CMD_HEADER_WRITE, E_POWER_STATE_RESUME, 0x00, 0x01
	};
	int retry_cnt;
	int error;

	if (device_may_wakeup(dev)) {
		if (ts->wake_irq_enabled)
			disable_irq_wake(client->irq);
		elants_i2c_sw_reset(client);
	} else if (ts->keep_power_in_suspend) {
		for (retry_cnt = 0; retry_cnt < MAX_RETRIES; retry_cnt++) {
			error = elants_i2c_send(client, set_active_cmd,
						sizeof(set_active_cmd));
			if (!error)
				break;

			dev_err(&client->dev,
				"resume command failed: %d\n", error);
		}
	} else {
		elants_i2c_power_on(ts);
		elants_i2c_initialize(ts);
	}

	ts->state = ELAN_STATE_NORMAL;
	enable_irq(client->irq);

	return 0;
}

static SIMPLE_DEV_PM_OPS(elants_i2c_pm_ops,
			 elants_i2c_suspend, elants_i2c_resume);

static const struct i2c_device_id elants_i2c_id[] = {
	{ DEVICE_NAME, EKTH3500 },
	{ "ekth3500", EKTH3500 },
	{ "ektf3624", EKTF3624 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, elants_i2c_id);

#ifdef CONFIG_ACPI
static const struct acpi_device_id elants_acpi_id[] = {
	{ "ELAN0001", EKTH3500 },
	{ }
};
MODULE_DEVICE_TABLE(acpi, elants_acpi_id);
#endif

#ifdef CONFIG_OF
static const struct of_device_id elants_of_match[] = {
	{ .compatible = "elan,ekth3500" },
	{ .compatible = "elan,ektf3624" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, elants_of_match);
#endif

static struct i2c_driver elants_i2c_driver = {
	.probe = elants_i2c_probe,
	.id_table = elants_i2c_id,
	.driver = {
		.name = DEVICE_NAME,
		.pm = &elants_i2c_pm_ops,
		.acpi_match_table = ACPI_PTR(elants_acpi_id),
		.of_match_table = of_match_ptr(elants_of_match),
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
	},
};
module_i2c_driver(elants_i2c_driver);

MODULE_AUTHOR("Scott Liu <scott.liu@emc.com.tw>");
MODULE_DESCRIPTION("Elan I2c Touchscreen driver");
MODULE_LICENSE("GPL");
