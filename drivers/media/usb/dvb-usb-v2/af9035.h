/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Afatech AF9035 DVB USB driver
 *
 * Copyright (C) 2009 Antti Palosaari <crope@iki.fi>
 * Copyright (C) 2012 Antti Palosaari <crope@iki.fi>
 */

#ifndef AF9035_H
#define AF9035_H

#include <linux/platform_device.h>
#include "dvb_usb.h"
#include "af9033.h"
#include "tua9001.h"
#include "fc0011.h"
#include "fc0012.h"
#include "mxl5007t.h"
#include "tda18218.h"
#include "fc2580.h"
#include "it913x.h"
#include "si2168.h"
#include "si2157.h"

struct reg_val {
	u32 reg;
	u8  val;
};

struct reg_val_mask {
	u32 reg;
	u8  val;
	u8  mask;
};

struct usb_req {
	u8  cmd;
	u8  mbox;
	u8  wlen;
	u8  *wbuf;
	u8  rlen;
	u8  *rbuf;
};

struct state {
#define BUF_LEN 64
	u8 buf[BUF_LEN];
	u8 seq; /* packet sequence number */
	u8 prechip_version;
	u8 chip_version;
	u16 chip_type;
	u8 eeprom[256];
	bool no_eeprom;
	u8 ir_mode;
	u8 ir_type;
	u8 dual_mode:1;
	u8 no_read:1;
	u8 af9033_i2c_addr[2];
	u8 it930x_addresses;
	struct af9033_config af9033_config[2];
	struct af9033_ops ops;
	#define AF9035_I2C_CLIENT_MAX 4
	struct i2c_client *i2c_client[AF9035_I2C_CLIENT_MAX];
	struct i2c_adapter *i2c_adapter_demod;
	struct platform_device *platform_device_tuner[2];
};

struct address_table {
	u8 frontend_i2c_addr;
	u8 tuner_i2c_addr;
	u8 tuner_if_port;
};

static const struct address_table it930x_addresses_table[] = {
	{ 0x67, 0x63, 1 },
	{ 0x64, 0x60, 0 },
};

static const u32 clock_lut_af9035[] = {
	20480000, /*      FPGA */
	16384000, /* 16.38 MHz */
	20480000, /* 20.48 MHz */
	36000000, /* 36.00 MHz */
	30000000, /* 30.00 MHz */
	26000000, /* 26.00 MHz */
	28000000, /* 28.00 MHz */
	32000000, /* 32.00 MHz */
	34000000, /* 34.00 MHz */
	24000000, /* 24.00 MHz */
	22000000, /* 22.00 MHz */
	12000000, /* 12.00 MHz */
};

static const u32 clock_lut_it9135[] = {
	12000000, /* 12.00 MHz */
	20480000, /* 20.48 MHz */
	36000000, /* 36.00 MHz */
	30000000, /* 30.00 MHz */
	26000000, /* 26.00 MHz */
	28000000, /* 28.00 MHz */
	32000000, /* 32.00 MHz */
	34000000, /* 34.00 MHz */
	24000000, /* 24.00 MHz */
	22000000, /* 22.00 MHz */
};

#define AF9035_FIRMWARE_AF9035 "dvb-usb-af9035-02.fw"
#define AF9035_FIRMWARE_IT9135_V1 "dvb-usb-it9135-01.fw"
#define AF9035_FIRMWARE_IT9135_V2 "dvb-usb-it9135-02.fw"
#define AF9035_FIRMWARE_IT9303 "dvb-usb-it9303-01.fw"

/*
 * eeprom is memory mapped as read only. Writing that memory mapped address
 * will not corrupt eeprom.
 *
 * TS mode:
 * 0  TS
 * 1  DCA + PIP
 * 3  PIP
 * 5  DCA + PIP (AF9035 only)
 * n  DCA
 *
 * Values 0, 3 and 5 are seen to this day. 0 for single TS and 3/5 for dual TS.
 */

#define EEPROM_BASE_AF9035        0x42f5
#define EEPROM_BASE_IT9135        0x4994
#define EEPROM_SHIFT                0x10

#define EEPROM_IR_MODE              0x18
#define EEPROM_TS_MODE              0x31
#define EEPROM_2ND_DEMOD_ADDR       0x32
#define EEPROM_IR_TYPE              0x34
#define EEPROM_1_IF_L               0x38
#define EEPROM_1_IF_H               0x39
#define EEPROM_1_TUNER_ID           0x3c
#define EEPROM_2_IF_L               0x48
#define EEPROM_2_IF_H               0x49
#define EEPROM_2_TUNER_ID           0x4c

/* USB commands */
#define CMD_MEM_RD                  0x00
#define CMD_MEM_WR                  0x01
#define CMD_I2C_RD                  0x02
#define CMD_I2C_WR                  0x03
#define CMD_IR_GET                  0x18
#define CMD_FW_DL                   0x21
#define CMD_FW_QUERYINFO            0x22
#define CMD_FW_BOOT                 0x23
#define CMD_FW_DL_BEGIN             0x24
#define CMD_FW_DL_END               0x25
#define CMD_FW_SCATTER_WR           0x29
#define CMD_GENERIC_I2C_RD          0x2a
#define CMD_GENERIC_I2C_WR          0x2b

#endif
