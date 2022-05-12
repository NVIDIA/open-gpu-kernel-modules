/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * tlv320aic32x4.h  --  TLV320AIC32X4 Soc Audio driver platform data
 *
 * Copyright 2011 Vista Silicon S.L.
 *
 * Author: Javier Martin <javier.martin@vista-silicon.com>
 */

#ifndef _AIC32X4_PDATA_H
#define _AIC32X4_PDATA_H

#define AIC32X4_PWR_MICBIAS_2075_LDOIN		0x00000001
#define AIC32X4_PWR_AVDD_DVDD_WEAK_DISABLE	0x00000002
#define AIC32X4_PWR_AIC32X4_LDO_ENABLE		0x00000004
#define AIC32X4_PWR_CMMODE_LDOIN_RANGE_18_36	0x00000008
#define AIC32X4_PWR_CMMODE_HP_LDOIN_POWERED	0x00000010

#define AIC32X4_MICPGA_ROUTE_LMIC_IN2R_10K	0x00000001
#define AIC32X4_MICPGA_ROUTE_RMIC_IN1L_10K	0x00000002

/* GPIO API */
#define AIC32X4_MFPX_DEFAULT_VALUE	0xff

#define AIC32X4_MFP1_DIN_DISABLED	0
#define AIC32X4_MFP1_DIN_ENABLED	0x2
#define AIC32X4_MFP1_GPIO_IN		0x4

#define AIC32X4_MFP2_GPIO_OUT_LOW	0x0
#define AIC32X4_MFP2_GPIO_OUT_HIGH	0x1

#define AIC32X4_MFP_GPIO_ENABLED	0x4

#define AIC32X4_MFP5_GPIO_DISABLED	0x0
#define AIC32X4_MFP5_GPIO_INPUT		0x8
#define AIC32X4_MFP5_GPIO_OUTPUT	0xc
#define AIC32X4_MFP5_GPIO_OUT_LOW	0x0
#define AIC32X4_MFP5_GPIO_OUT_HIGH	0x1

struct aic32x4_setup_data {
	unsigned int gpio_func[5];
};

struct aic32x4_pdata {
	struct aic32x4_setup_data *setup;
	u32 power_cfg;
	u32 micpga_routing;
	bool swapdacs;
	int rstn_gpio;
};

#endif
