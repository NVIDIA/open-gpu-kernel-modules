/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Elan I2C/SMBus Touchpad driver
 *
 * Copyright (c) 2013 ELAN Microelectronics Corp.
 *
 * Author: 林政維 (Duson Lin) <dusonlin@emc.com.tw>
 *
 * Based on cyapa driver:
 * copyright (c) 2011-2012 Cypress Semiconductor, Inc.
 * copyright (c) 2011-2012 Google, Inc.
 *
 * Trademarks are the property of their respective owners.
 */

#ifndef _ELAN_I2C_H
#define _ELAN_I2C_H

#include <linux/types.h>

#define ETP_ENABLE_ABS		0x0001
#define ETP_ENABLE_CALIBRATE	0x0002
#define ETP_DISABLE_CALIBRATE	0x0000
#define ETP_DISABLE_POWER	0x0001
#define ETP_PRESSURE_OFFSET	25

#define ETP_CALIBRATE_MAX_LEN	3

#define ETP_FEATURE_REPORT_MK	BIT(0)

#define ETP_REPORT_ID		0x5D
#define ETP_TP_REPORT_ID	0x5E
#define ETP_TP_REPORT_ID2	0x5F
#define ETP_REPORT_ID2		0x60	/* High precision report */

#define ETP_REPORT_ID_OFFSET	2
#define ETP_TOUCH_INFO_OFFSET	3
#define ETP_FINGER_DATA_OFFSET	4
#define ETP_HOVER_INFO_OFFSET	30
#define ETP_MK_DATA_OFFSET	33	/* For high precision reports */

#define ETP_MAX_REPORT_LEN	39

#define ETP_MAX_FINGERS		5
#define ETP_FINGER_DATA_LEN	5

/* IAP Firmware handling */
#define ETP_PRODUCT_ID_FORMAT_STRING	"%d.0"
#define ETP_FW_NAME		"elan_i2c_" ETP_PRODUCT_ID_FORMAT_STRING ".bin"
#define ETP_IAP_START_ADDR	0x0083
#define ETP_FW_IAP_PAGE_ERR	(1 << 5)
#define ETP_FW_IAP_INTF_ERR	(1 << 4)
#define ETP_FW_PAGE_SIZE	64
#define ETP_FW_PAGE_SIZE_128	128
#define ETP_FW_PAGE_SIZE_512	512
#define ETP_FW_SIGNATURE_SIZE	6

#define ETP_PRODUCT_ID_DELBIN	0x00C2
#define ETP_PRODUCT_ID_VOXEL	0x00BF
#define ETP_PRODUCT_ID_MAGPIE	0x0120
#define ETP_PRODUCT_ID_BOBBA	0x0121

struct i2c_client;
struct completion;

enum tp_mode {
	IAP_MODE = 1,
	MAIN_MODE
};

struct elan_transport_ops {
	int (*initialize)(struct i2c_client *client);
	int (*sleep_control)(struct i2c_client *, bool sleep);
	int (*power_control)(struct i2c_client *, bool enable);
	int (*set_mode)(struct i2c_client *client, u8 mode);

	int (*calibrate)(struct i2c_client *client);
	int (*calibrate_result)(struct i2c_client *client, u8 *val);

	int (*get_baseline_data)(struct i2c_client *client,
				 bool max_baseline, u8 *value);

	int (*get_version)(struct i2c_client *client, u8 pattern, bool iap,
			   u8 *version);
	int (*get_sm_version)(struct i2c_client *client, u8 pattern,
			      u16 *ic_type, u8 *version, u8 *clickpad);
	int (*get_checksum)(struct i2c_client *client, bool iap, u16 *csum);
	int (*get_product_id)(struct i2c_client *client, u16 *id);

	int (*get_max)(struct i2c_client *client,
		       unsigned int *max_x, unsigned int *max_y);
	int (*get_resolution)(struct i2c_client *client,
			      u8 *hw_res_x, u8 *hw_res_y);
	int (*get_num_traces)(struct i2c_client *client,
			      unsigned int *x_tracenum,
			      unsigned int *y_tracenum);

	int (*iap_get_mode)(struct i2c_client *client, enum tp_mode *mode);
	int (*iap_reset)(struct i2c_client *client);

	int (*prepare_fw_update)(struct i2c_client *client, u16 ic_type,
				 u8 iap_version, u16 fw_page_size);
	int (*write_fw_block)(struct i2c_client *client, u16 fw_page_size,
			      const u8 *page, u16 checksum, int idx);
	int (*finish_fw_update)(struct i2c_client *client,
				struct completion *reset_done);

	int (*get_report_features)(struct i2c_client *client, u8 pattern,
				   unsigned int *features,
				   unsigned int *report_len);
	int (*get_report)(struct i2c_client *client, u8 *report,
			  unsigned int report_len);
	int (*get_pressure_adjustment)(struct i2c_client *client,
				       int *adjustment);
	int (*get_pattern)(struct i2c_client *client, u8 *pattern);
};

extern const struct elan_transport_ops elan_smbus_ops, elan_i2c_ops;

#endif /* _ELAN_I2C_H */
