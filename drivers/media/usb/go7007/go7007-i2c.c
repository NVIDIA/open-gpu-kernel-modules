// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2005-2006 Micronas USA Inc.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#include "go7007-priv.h"

/********************* Driver for on-board I2C adapter *********************/

/* #define GO7007_I2C_DEBUG */

#define SPI_I2C_ADDR_BASE		0x1400
#define STATUS_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x2)
#define I2C_CTRL_REG_ADDR		(SPI_I2C_ADDR_BASE + 0x6)
#define I2C_DEV_UP_ADDR_REG_ADDR	(SPI_I2C_ADDR_BASE + 0x7)
#define I2C_LO_ADDR_REG_ADDR		(SPI_I2C_ADDR_BASE + 0x8)
#define I2C_DATA_REG_ADDR		(SPI_I2C_ADDR_BASE + 0x9)
#define I2C_CLKFREQ_REG_ADDR		(SPI_I2C_ADDR_BASE + 0xa)

#define I2C_STATE_MASK			0x0007
#define I2C_READ_READY_MASK		0x0008

/* There is only one I2C port on the TW2804 that feeds all four GO7007 VIPs
 * on the Adlink PCI-MPG24, so access is shared between all of them. */
static DEFINE_MUTEX(adlink_mpg24_i2c_lock);

static int go7007_i2c_xfer(struct go7007 *go, u16 addr, int read,
		u16 command, int flags, u8 *data)
{
	int i, ret = -EIO;
	u16 val;

	if (go->status == STATUS_SHUTDOWN)
		return -ENODEV;

#ifdef GO7007_I2C_DEBUG
	if (read)
		dev_dbg(go->dev, "go7007-i2c: reading 0x%02x on 0x%02x\n",
			command, addr);
	else
		dev_dbg(go->dev,
			"go7007-i2c: writing 0x%02x to 0x%02x on 0x%02x\n",
			*data, command, addr);
#endif

	mutex_lock(&go->hw_lock);

	if (go->board_id == GO7007_BOARDID_ADLINK_MPG24) {
		/* Bridge the I2C port on this GO7007 to the shared bus */
		mutex_lock(&adlink_mpg24_i2c_lock);
		go7007_write_addr(go, 0x3c82, 0x0020);
	}

	/* Wait for I2C adapter to be ready */
	for (i = 0; i < 10; ++i) {
		if (go7007_read_addr(go, STATUS_REG_ADDR, &val) < 0)
			goto i2c_done;
		if (!(val & I2C_STATE_MASK))
			break;
		msleep(100);
	}
	if (i == 10) {
		dev_err(go->dev, "go7007-i2c: I2C adapter is hung\n");
		goto i2c_done;
	}

	/* Set target register (command) */
	go7007_write_addr(go, I2C_CTRL_REG_ADDR, flags);
	go7007_write_addr(go, I2C_LO_ADDR_REG_ADDR, command);

	/* If we're writing, send the data and target address and we're done */
	if (!read) {
		go7007_write_addr(go, I2C_DATA_REG_ADDR, *data);
		go7007_write_addr(go, I2C_DEV_UP_ADDR_REG_ADDR,
					(addr << 9) | (command >> 8));
		ret = 0;
		goto i2c_done;
	}

	/* Otherwise, we're reading.  First clear i2c_rx_data_rdy. */
	if (go7007_read_addr(go, I2C_DATA_REG_ADDR, &val) < 0)
		goto i2c_done;

	/* Send the target address plus read flag */
	go7007_write_addr(go, I2C_DEV_UP_ADDR_REG_ADDR,
			(addr << 9) | 0x0100 | (command >> 8));

	/* Wait for i2c_rx_data_rdy */
	for (i = 0; i < 10; ++i) {
		if (go7007_read_addr(go, STATUS_REG_ADDR, &val) < 0)
			goto i2c_done;
		if (val & I2C_READ_READY_MASK)
			break;
		msleep(100);
	}
	if (i == 10) {
		dev_err(go->dev, "go7007-i2c: I2C adapter is hung\n");
		goto i2c_done;
	}

	/* Retrieve the read byte */
	if (go7007_read_addr(go, I2C_DATA_REG_ADDR, &val) < 0)
		goto i2c_done;
	*data = val;
	ret = 0;

i2c_done:
	if (go->board_id == GO7007_BOARDID_ADLINK_MPG24) {
		/* Isolate the I2C port on this GO7007 from the shared bus */
		go7007_write_addr(go, 0x3c82, 0x0000);
		mutex_unlock(&adlink_mpg24_i2c_lock);
	}
	mutex_unlock(&go->hw_lock);
	return ret;
}

static int go7007_smbus_xfer(struct i2c_adapter *adapter, u16 addr,
		unsigned short flags, char read_write,
		u8 command, int size, union i2c_smbus_data *data)
{
	struct go7007 *go = i2c_get_adapdata(adapter);

	if (size != I2C_SMBUS_BYTE_DATA)
		return -EIO;
	return go7007_i2c_xfer(go, addr, read_write == I2C_SMBUS_READ, command,
			flags & I2C_CLIENT_SCCB ? 0x10 : 0x00, &data->byte);
}

/* VERY LIMITED I2C master xfer function -- only needed because the
 * SMBus functions only support 8-bit commands and the SAA7135 uses
 * 16-bit commands.  The I2C interface on the GO7007, as limited as
 * it is, does support this mode. */

static int go7007_i2c_master_xfer(struct i2c_adapter *adapter,
					struct i2c_msg msgs[], int num)
{
	struct go7007 *go = i2c_get_adapdata(adapter);
	int i;

	for (i = 0; i < num; ++i) {
		/* We can only do two things here -- write three bytes, or
		 * write two bytes and read one byte. */
		if (msgs[i].len == 2) {
			if (i + 1 == num || msgs[i].addr != msgs[i + 1].addr ||
					(msgs[i].flags & I2C_M_RD) ||
					!(msgs[i + 1].flags & I2C_M_RD) ||
					msgs[i + 1].len != 1)
				return -EIO;
			if (go7007_i2c_xfer(go, msgs[i].addr, 1,
					(msgs[i].buf[0] << 8) | msgs[i].buf[1],
					0x01, &msgs[i + 1].buf[0]) < 0)
				return -EIO;
			++i;
		} else if (msgs[i].len == 3) {
			if (msgs[i].flags & I2C_M_RD)
				return -EIO;
			if (msgs[i].len != 3)
				return -EIO;
			if (go7007_i2c_xfer(go, msgs[i].addr, 0,
					(msgs[i].buf[0] << 8) | msgs[i].buf[1],
					0x01, &msgs[i].buf[2]) < 0)
				return -EIO;
		} else
			return -EIO;
	}

	return num;
}

static u32 go7007_functionality(struct i2c_adapter *adapter)
{
	return I2C_FUNC_SMBUS_BYTE_DATA;
}

static const struct i2c_algorithm go7007_algo = {
	.smbus_xfer	= go7007_smbus_xfer,
	.master_xfer	= go7007_i2c_master_xfer,
	.functionality	= go7007_functionality,
};

static struct i2c_adapter go7007_adap_templ = {
	.owner			= THIS_MODULE,
	.name			= "WIS GO7007SB",
	.algo			= &go7007_algo,
};

int go7007_i2c_init(struct go7007 *go)
{
	memcpy(&go->i2c_adapter, &go7007_adap_templ,
			sizeof(go7007_adap_templ));
	go->i2c_adapter.dev.parent = go->dev;
	i2c_set_adapdata(&go->i2c_adapter, go);
	if (i2c_add_adapter(&go->i2c_adapter) < 0) {
		dev_err(go->dev,
			"go7007-i2c: error: i2c_add_adapter failed\n");
		return -1;
	}
	return 0;
}
