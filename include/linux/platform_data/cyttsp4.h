/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Header file for:
 * Cypress TrueTouch(TM) Standard Product (TTSP) touchscreen drivers.
 * For use with Cypress Txx3xx parts.
 * Supported parts include:
 * CY8CTST341
 * CY8CTMA340
 *
 * Copyright (C) 2009, 2010, 2011 Cypress Semiconductor, Inc.
 * Copyright (C) 2012 Javier Martinez Canillas <javier@dowhile0.org>
 *
 * Contact Cypress Semiconductor at www.cypress.com (kev@cypress.com)
 */
#ifndef _CYTTSP4_H_
#define _CYTTSP4_H_

#define CYTTSP4_MT_NAME "cyttsp4_mt"
#define CYTTSP4_I2C_NAME "cyttsp4_i2c_adapter"
#define CYTTSP4_SPI_NAME "cyttsp4_spi_adapter"

#define CY_TOUCH_SETTINGS_MAX 32

struct touch_framework {
	const uint16_t  *abs;
	uint8_t         size;
	uint8_t         enable_vkeys;
} __packed;

struct cyttsp4_mt_platform_data {
	struct touch_framework *frmwrk;
	unsigned short flags;
	char const *inp_dev_name;
};

struct touch_settings {
	const uint8_t *data;
	uint32_t size;
	uint8_t tag;
} __packed;

struct cyttsp4_core_platform_data {
	int irq_gpio;
	int rst_gpio;
	int level_irq_udelay;
	int (*xres)(struct cyttsp4_core_platform_data *pdata,
		struct device *dev);
	int (*init)(struct cyttsp4_core_platform_data *pdata,
		int on, struct device *dev);
	int (*power)(struct cyttsp4_core_platform_data *pdata,
		int on, struct device *dev, atomic_t *ignore_irq);
	int (*irq_stat)(struct cyttsp4_core_platform_data *pdata,
		struct device *dev);
	struct touch_settings *sett[CY_TOUCH_SETTINGS_MAX];
};

struct cyttsp4_platform_data {
	struct cyttsp4_core_platform_data *core_pdata;
	struct cyttsp4_mt_platform_data *mt_pdata;
};

#endif /* _CYTTSP4_H_ */
