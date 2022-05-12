/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * include/linux/mfd/wm831x/core.h -- Core interface for WM831x
 *
 * Copyright 2009 Wolfson Microelectronics PLC.
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 */

#ifndef __MFD_WM831X_CORE_H__
#define __MFD_WM831X_CORE_H__

#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/list.h>
#include <linux/regmap.h>
#include <linux/mfd/wm831x/auxadc.h>
#include <linux/mfd/wm831x/pdata.h>
#include <linux/of.h>

/*
 * Register values.
 */
#define WM831X_RESET_ID                         0x00
#define WM831X_REVISION                         0x01
#define WM831X_PARENT_ID                        0x4000
#define WM831X_SYSVDD_CONTROL                   0x4001
#define WM831X_THERMAL_MONITORING               0x4002
#define WM831X_POWER_STATE                      0x4003
#define WM831X_WATCHDOG                         0x4004
#define WM831X_ON_PIN_CONTROL                   0x4005
#define WM831X_RESET_CONTROL                    0x4006
#define WM831X_CONTROL_INTERFACE                0x4007
#define WM831X_SECURITY_KEY                     0x4008
#define WM831X_SOFTWARE_SCRATCH                 0x4009
#define WM831X_OTP_CONTROL                      0x400A
#define WM831X_GPIO_LEVEL                       0x400C
#define WM831X_SYSTEM_STATUS                    0x400D
#define WM831X_ON_SOURCE                        0x400E
#define WM831X_OFF_SOURCE                       0x400F
#define WM831X_SYSTEM_INTERRUPTS                0x4010
#define WM831X_INTERRUPT_STATUS_1               0x4011
#define WM831X_INTERRUPT_STATUS_2               0x4012
#define WM831X_INTERRUPT_STATUS_3               0x4013
#define WM831X_INTERRUPT_STATUS_4               0x4014
#define WM831X_INTERRUPT_STATUS_5               0x4015
#define WM831X_IRQ_CONFIG                       0x4017
#define WM831X_SYSTEM_INTERRUPTS_MASK           0x4018
#define WM831X_INTERRUPT_STATUS_1_MASK          0x4019
#define WM831X_INTERRUPT_STATUS_2_MASK          0x401A
#define WM831X_INTERRUPT_STATUS_3_MASK          0x401B
#define WM831X_INTERRUPT_STATUS_4_MASK          0x401C
#define WM831X_INTERRUPT_STATUS_5_MASK          0x401D
#define WM831X_RTC_WRITE_COUNTER                0x4020
#define WM831X_RTC_TIME_1                       0x4021
#define WM831X_RTC_TIME_2                       0x4022
#define WM831X_RTC_ALARM_1                      0x4023
#define WM831X_RTC_ALARM_2                      0x4024
#define WM831X_RTC_CONTROL                      0x4025
#define WM831X_RTC_TRIM                         0x4026
#define WM831X_TOUCH_CONTROL_1                  0x4028
#define WM831X_TOUCH_CONTROL_2                  0x4029
#define WM831X_TOUCH_DATA_X                     0x402A
#define WM831X_TOUCH_DATA_Y                     0x402B
#define WM831X_TOUCH_DATA_Z                     0x402C
#define WM831X_AUXADC_DATA                      0x402D
#define WM831X_AUXADC_CONTROL                   0x402E
#define WM831X_AUXADC_SOURCE                    0x402F
#define WM831X_COMPARATOR_CONTROL               0x4030
#define WM831X_COMPARATOR_1                     0x4031
#define WM831X_COMPARATOR_2                     0x4032
#define WM831X_COMPARATOR_3                     0x4033
#define WM831X_COMPARATOR_4                     0x4034
#define WM831X_GPIO1_CONTROL                    0x4038
#define WM831X_GPIO2_CONTROL                    0x4039
#define WM831X_GPIO3_CONTROL                    0x403A
#define WM831X_GPIO4_CONTROL                    0x403B
#define WM831X_GPIO5_CONTROL                    0x403C
#define WM831X_GPIO6_CONTROL                    0x403D
#define WM831X_GPIO7_CONTROL                    0x403E
#define WM831X_GPIO8_CONTROL                    0x403F
#define WM831X_GPIO9_CONTROL                    0x4040
#define WM831X_GPIO10_CONTROL                   0x4041
#define WM831X_GPIO11_CONTROL                   0x4042
#define WM831X_GPIO12_CONTROL                   0x4043
#define WM831X_GPIO13_CONTROL                   0x4044
#define WM831X_GPIO14_CONTROL                   0x4045
#define WM831X_GPIO15_CONTROL                   0x4046
#define WM831X_GPIO16_CONTROL                   0x4047
#define WM831X_CHARGER_CONTROL_1                0x4048
#define WM831X_CHARGER_CONTROL_2                0x4049
#define WM831X_CHARGER_STATUS                   0x404A
#define WM831X_BACKUP_CHARGER_CONTROL           0x404B
#define WM831X_STATUS_LED_1                     0x404C
#define WM831X_STATUS_LED_2                     0x404D
#define WM831X_CURRENT_SINK_1                   0x404E
#define WM831X_CURRENT_SINK_2                   0x404F
#define WM831X_DCDC_ENABLE                      0x4050
#define WM831X_LDO_ENABLE                       0x4051
#define WM831X_DCDC_STATUS                      0x4052
#define WM831X_LDO_STATUS                       0x4053
#define WM831X_DCDC_UV_STATUS                   0x4054
#define WM831X_LDO_UV_STATUS                    0x4055
#define WM831X_DC1_CONTROL_1                    0x4056
#define WM831X_DC1_CONTROL_2                    0x4057
#define WM831X_DC1_ON_CONFIG                    0x4058
#define WM831X_DC1_SLEEP_CONTROL                0x4059
#define WM831X_DC1_DVS_CONTROL                  0x405A
#define WM831X_DC2_CONTROL_1                    0x405B
#define WM831X_DC2_CONTROL_2                    0x405C
#define WM831X_DC2_ON_CONFIG                    0x405D
#define WM831X_DC2_SLEEP_CONTROL                0x405E
#define WM831X_DC2_DVS_CONTROL                  0x405F
#define WM831X_DC3_CONTROL_1                    0x4060
#define WM831X_DC3_CONTROL_2                    0x4061
#define WM831X_DC3_ON_CONFIG                    0x4062
#define WM831X_DC3_SLEEP_CONTROL                0x4063
#define WM831X_DC4_CONTROL                      0x4064
#define WM831X_DC4_SLEEP_CONTROL                0x4065
#define WM832X_DC4_SLEEP_CONTROL                0x4067
#define WM831X_EPE1_CONTROL                     0x4066
#define WM831X_EPE2_CONTROL                     0x4067
#define WM831X_LDO1_CONTROL                     0x4068
#define WM831X_LDO1_ON_CONTROL                  0x4069
#define WM831X_LDO1_SLEEP_CONTROL               0x406A
#define WM831X_LDO2_CONTROL                     0x406B
#define WM831X_LDO2_ON_CONTROL                  0x406C
#define WM831X_LDO2_SLEEP_CONTROL               0x406D
#define WM831X_LDO3_CONTROL                     0x406E
#define WM831X_LDO3_ON_CONTROL                  0x406F
#define WM831X_LDO3_SLEEP_CONTROL               0x4070
#define WM831X_LDO4_CONTROL                     0x4071
#define WM831X_LDO4_ON_CONTROL                  0x4072
#define WM831X_LDO4_SLEEP_CONTROL               0x4073
#define WM831X_LDO5_CONTROL                     0x4074
#define WM831X_LDO5_ON_CONTROL                  0x4075
#define WM831X_LDO5_SLEEP_CONTROL               0x4076
#define WM831X_LDO6_CONTROL                     0x4077
#define WM831X_LDO6_ON_CONTROL                  0x4078
#define WM831X_LDO6_SLEEP_CONTROL               0x4079
#define WM831X_LDO7_CONTROL                     0x407A
#define WM831X_LDO7_ON_CONTROL                  0x407B
#define WM831X_LDO7_SLEEP_CONTROL               0x407C
#define WM831X_LDO8_CONTROL                     0x407D
#define WM831X_LDO8_ON_CONTROL                  0x407E
#define WM831X_LDO8_SLEEP_CONTROL               0x407F
#define WM831X_LDO9_CONTROL                     0x4080
#define WM831X_LDO9_ON_CONTROL                  0x4081
#define WM831X_LDO9_SLEEP_CONTROL               0x4082
#define WM831X_LDO10_CONTROL                    0x4083
#define WM831X_LDO10_ON_CONTROL                 0x4084
#define WM831X_LDO10_SLEEP_CONTROL              0x4085
#define WM831X_LDO11_ON_CONTROL                 0x4087
#define WM831X_LDO11_SLEEP_CONTROL              0x4088
#define WM831X_POWER_GOOD_SOURCE_1              0x408E
#define WM831X_POWER_GOOD_SOURCE_2              0x408F
#define WM831X_CLOCK_CONTROL_1                  0x4090
#define WM831X_CLOCK_CONTROL_2                  0x4091
#define WM831X_FLL_CONTROL_1                    0x4092
#define WM831X_FLL_CONTROL_2                    0x4093
#define WM831X_FLL_CONTROL_3                    0x4094
#define WM831X_FLL_CONTROL_4                    0x4095
#define WM831X_FLL_CONTROL_5                    0x4096
#define WM831X_UNIQUE_ID_1                      0x7800
#define WM831X_UNIQUE_ID_2                      0x7801
#define WM831X_UNIQUE_ID_3                      0x7802
#define WM831X_UNIQUE_ID_4                      0x7803
#define WM831X_UNIQUE_ID_5                      0x7804
#define WM831X_UNIQUE_ID_6                      0x7805
#define WM831X_UNIQUE_ID_7                      0x7806
#define WM831X_UNIQUE_ID_8                      0x7807
#define WM831X_FACTORY_OTP_ID                   0x7808
#define WM831X_FACTORY_OTP_1                    0x7809
#define WM831X_FACTORY_OTP_2                    0x780A
#define WM831X_FACTORY_OTP_3                    0x780B
#define WM831X_FACTORY_OTP_4                    0x780C
#define WM831X_FACTORY_OTP_5                    0x780D
#define WM831X_CUSTOMER_OTP_ID                  0x7810
#define WM831X_DC1_OTP_CONTROL                  0x7811
#define WM831X_DC2_OTP_CONTROL                  0x7812
#define WM831X_DC3_OTP_CONTROL                  0x7813
#define WM831X_LDO1_2_OTP_CONTROL               0x7814
#define WM831X_LDO3_4_OTP_CONTROL               0x7815
#define WM831X_LDO5_6_OTP_CONTROL               0x7816
#define WM831X_LDO7_8_OTP_CONTROL               0x7817
#define WM831X_LDO9_10_OTP_CONTROL              0x7818
#define WM831X_LDO11_EPE_CONTROL                0x7819
#define WM831X_GPIO1_OTP_CONTROL                0x781A
#define WM831X_GPIO2_OTP_CONTROL                0x781B
#define WM831X_GPIO3_OTP_CONTROL                0x781C
#define WM831X_GPIO4_OTP_CONTROL                0x781D
#define WM831X_GPIO5_OTP_CONTROL                0x781E
#define WM831X_GPIO6_OTP_CONTROL                0x781F
#define WM831X_DBE_CHECK_DATA                   0x7827

/*
 * R0 (0x00) - Reset ID
 */
#define WM831X_CHIP_ID_MASK                     0xFFFF  /* CHIP_ID - [15:0] */
#define WM831X_CHIP_ID_SHIFT                         0  /* CHIP_ID - [15:0] */
#define WM831X_CHIP_ID_WIDTH                        16  /* CHIP_ID - [15:0] */

/*
 * R1 (0x01) - Revision
 */
#define WM831X_PARENT_REV_MASK                  0xFF00  /* PARENT_REV - [15:8] */
#define WM831X_PARENT_REV_SHIFT                      8  /* PARENT_REV - [15:8] */
#define WM831X_PARENT_REV_WIDTH                      8  /* PARENT_REV - [15:8] */
#define WM831X_CHILD_REV_MASK                   0x00FF  /* CHILD_REV - [7:0] */
#define WM831X_CHILD_REV_SHIFT                       0  /* CHILD_REV - [7:0] */
#define WM831X_CHILD_REV_WIDTH                       8  /* CHILD_REV - [7:0] */

/*
 * R16384 (0x4000) - Parent ID
 */
#define WM831X_PARENT_ID_MASK                   0xFFFF  /* PARENT_ID - [15:0] */
#define WM831X_PARENT_ID_SHIFT                       0  /* PARENT_ID - [15:0] */
#define WM831X_PARENT_ID_WIDTH                      16  /* PARENT_ID - [15:0] */

/*
 * R16389 (0x4005) - ON Pin Control
 */
#define WM831X_ON_PIN_SECACT_MASK               0x0300  /* ON_PIN_SECACT - [9:8] */
#define WM831X_ON_PIN_SECACT_SHIFT                   8  /* ON_PIN_SECACT - [9:8] */
#define WM831X_ON_PIN_SECACT_WIDTH                   2  /* ON_PIN_SECACT - [9:8] */
#define WM831X_ON_PIN_PRIMACT_MASK              0x0030  /* ON_PIN_PRIMACT - [5:4] */
#define WM831X_ON_PIN_PRIMACT_SHIFT                  4  /* ON_PIN_PRIMACT - [5:4] */
#define WM831X_ON_PIN_PRIMACT_WIDTH                  2  /* ON_PIN_PRIMACT - [5:4] */
#define WM831X_ON_PIN_STS                       0x0008  /* ON_PIN_STS */
#define WM831X_ON_PIN_STS_MASK                  0x0008  /* ON_PIN_STS */
#define WM831X_ON_PIN_STS_SHIFT                      3  /* ON_PIN_STS */
#define WM831X_ON_PIN_STS_WIDTH                      1  /* ON_PIN_STS */
#define WM831X_ON_PIN_TO_MASK                   0x0003  /* ON_PIN_TO - [1:0] */
#define WM831X_ON_PIN_TO_SHIFT                       0  /* ON_PIN_TO - [1:0] */
#define WM831X_ON_PIN_TO_WIDTH                       2  /* ON_PIN_TO - [1:0] */

/*
 * R16528 (0x4090) - Clock Control 1
 */
#define WM831X_CLKOUT_ENA                       0x8000  /* CLKOUT_ENA */
#define WM831X_CLKOUT_ENA_MASK                  0x8000  /* CLKOUT_ENA */
#define WM831X_CLKOUT_ENA_SHIFT                     15  /* CLKOUT_ENA */
#define WM831X_CLKOUT_ENA_WIDTH                      1  /* CLKOUT_ENA */
#define WM831X_CLKOUT_OD                        0x2000  /* CLKOUT_OD */
#define WM831X_CLKOUT_OD_MASK                   0x2000  /* CLKOUT_OD */
#define WM831X_CLKOUT_OD_SHIFT                      13  /* CLKOUT_OD */
#define WM831X_CLKOUT_OD_WIDTH                       1  /* CLKOUT_OD */
#define WM831X_CLKOUT_SLOT_MASK                 0x0700  /* CLKOUT_SLOT - [10:8] */
#define WM831X_CLKOUT_SLOT_SHIFT                     8  /* CLKOUT_SLOT - [10:8] */
#define WM831X_CLKOUT_SLOT_WIDTH                     3  /* CLKOUT_SLOT - [10:8] */
#define WM831X_CLKOUT_SLPSLOT_MASK              0x0070  /* CLKOUT_SLPSLOT - [6:4] */
#define WM831X_CLKOUT_SLPSLOT_SHIFT                  4  /* CLKOUT_SLPSLOT - [6:4] */
#define WM831X_CLKOUT_SLPSLOT_WIDTH                  3  /* CLKOUT_SLPSLOT - [6:4] */
#define WM831X_CLKOUT_SRC                       0x0001  /* CLKOUT_SRC */
#define WM831X_CLKOUT_SRC_MASK                  0x0001  /* CLKOUT_SRC */
#define WM831X_CLKOUT_SRC_SHIFT                      0  /* CLKOUT_SRC */
#define WM831X_CLKOUT_SRC_WIDTH                      1  /* CLKOUT_SRC */

/*
 * R16529 (0x4091) - Clock Control 2
 */
#define WM831X_XTAL_INH                         0x8000  /* XTAL_INH */
#define WM831X_XTAL_INH_MASK                    0x8000  /* XTAL_INH */
#define WM831X_XTAL_INH_SHIFT                       15  /* XTAL_INH */
#define WM831X_XTAL_INH_WIDTH                        1  /* XTAL_INH */
#define WM831X_XTAL_ENA                         0x2000  /* XTAL_ENA */
#define WM831X_XTAL_ENA_MASK                    0x2000  /* XTAL_ENA */
#define WM831X_XTAL_ENA_SHIFT                       13  /* XTAL_ENA */
#define WM831X_XTAL_ENA_WIDTH                        1  /* XTAL_ENA */
#define WM831X_XTAL_BKUPENA                     0x1000  /* XTAL_BKUPENA */
#define WM831X_XTAL_BKUPENA_MASK                0x1000  /* XTAL_BKUPENA */
#define WM831X_XTAL_BKUPENA_SHIFT                   12  /* XTAL_BKUPENA */
#define WM831X_XTAL_BKUPENA_WIDTH                    1  /* XTAL_BKUPENA */
#define WM831X_FLL_AUTO                         0x0080  /* FLL_AUTO */
#define WM831X_FLL_AUTO_MASK                    0x0080  /* FLL_AUTO */
#define WM831X_FLL_AUTO_SHIFT                        7  /* FLL_AUTO */
#define WM831X_FLL_AUTO_WIDTH                        1  /* FLL_AUTO */
#define WM831X_FLL_AUTO_FREQ_MASK               0x0007  /* FLL_AUTO_FREQ - [2:0] */
#define WM831X_FLL_AUTO_FREQ_SHIFT                   0  /* FLL_AUTO_FREQ - [2:0] */
#define WM831X_FLL_AUTO_FREQ_WIDTH                   3  /* FLL_AUTO_FREQ - [2:0] */

/*
 * R16530 (0x4092) - FLL Control 1
 */
#define WM831X_FLL_FRAC                         0x0004  /* FLL_FRAC */
#define WM831X_FLL_FRAC_MASK                    0x0004  /* FLL_FRAC */
#define WM831X_FLL_FRAC_SHIFT                        2  /* FLL_FRAC */
#define WM831X_FLL_FRAC_WIDTH                        1  /* FLL_FRAC */
#define WM831X_FLL_OSC_ENA                      0x0002  /* FLL_OSC_ENA */
#define WM831X_FLL_OSC_ENA_MASK                 0x0002  /* FLL_OSC_ENA */
#define WM831X_FLL_OSC_ENA_SHIFT                     1  /* FLL_OSC_ENA */
#define WM831X_FLL_OSC_ENA_WIDTH                     1  /* FLL_OSC_ENA */
#define WM831X_FLL_ENA                          0x0001  /* FLL_ENA */
#define WM831X_FLL_ENA_MASK                     0x0001  /* FLL_ENA */
#define WM831X_FLL_ENA_SHIFT                         0  /* FLL_ENA */
#define WM831X_FLL_ENA_WIDTH                         1  /* FLL_ENA */

/*
 * R16531 (0x4093) - FLL Control 2
 */
#define WM831X_FLL_OUTDIV_MASK                  0x3F00  /* FLL_OUTDIV - [13:8] */
#define WM831X_FLL_OUTDIV_SHIFT                      8  /* FLL_OUTDIV - [13:8] */
#define WM831X_FLL_OUTDIV_WIDTH                      6  /* FLL_OUTDIV - [13:8] */
#define WM831X_FLL_CTRL_RATE_MASK               0x0070  /* FLL_CTRL_RATE - [6:4] */
#define WM831X_FLL_CTRL_RATE_SHIFT                   4  /* FLL_CTRL_RATE - [6:4] */
#define WM831X_FLL_CTRL_RATE_WIDTH                   3  /* FLL_CTRL_RATE - [6:4] */
#define WM831X_FLL_FRATIO_MASK                  0x0007  /* FLL_FRATIO - [2:0] */
#define WM831X_FLL_FRATIO_SHIFT                      0  /* FLL_FRATIO - [2:0] */
#define WM831X_FLL_FRATIO_WIDTH                      3  /* FLL_FRATIO - [2:0] */

/*
 * R16532 (0x4094) - FLL Control 3
 */
#define WM831X_FLL_K_MASK                       0xFFFF  /* FLL_K - [15:0] */
#define WM831X_FLL_K_SHIFT                           0  /* FLL_K - [15:0] */
#define WM831X_FLL_K_WIDTH                          16  /* FLL_K - [15:0] */

/*
 * R16533 (0x4095) - FLL Control 4
 */
#define WM831X_FLL_N_MASK                       0x7FE0  /* FLL_N - [14:5] */
#define WM831X_FLL_N_SHIFT                           5  /* FLL_N - [14:5] */
#define WM831X_FLL_N_WIDTH                          10  /* FLL_N - [14:5] */
#define WM831X_FLL_GAIN_MASK                    0x000F  /* FLL_GAIN - [3:0] */
#define WM831X_FLL_GAIN_SHIFT                        0  /* FLL_GAIN - [3:0] */
#define WM831X_FLL_GAIN_WIDTH                        4  /* FLL_GAIN - [3:0] */

/*
 * R16534 (0x4096) - FLL Control 5
 */
#define WM831X_FLL_CLK_REF_DIV_MASK             0x0018  /* FLL_CLK_REF_DIV - [4:3] */
#define WM831X_FLL_CLK_REF_DIV_SHIFT                 3  /* FLL_CLK_REF_DIV - [4:3] */
#define WM831X_FLL_CLK_REF_DIV_WIDTH                 2  /* FLL_CLK_REF_DIV - [4:3] */
#define WM831X_FLL_CLK_SRC_MASK                 0x0003  /* FLL_CLK_SRC - [1:0] */
#define WM831X_FLL_CLK_SRC_SHIFT                     0  /* FLL_CLK_SRC - [1:0] */
#define WM831X_FLL_CLK_SRC_WIDTH                     2  /* FLL_CLK_SRC - [1:0] */

struct regulator_dev;
struct irq_domain;

#define WM831X_NUM_IRQ_REGS 5
#define WM831X_NUM_GPIO_REGS 16

enum wm831x_parent {
	WM8310 = 0x8310,
	WM8311 = 0x8311,
	WM8312 = 0x8312,
	WM8320 = 0x8320,
	WM8321 = 0x8321,
	WM8325 = 0x8325,
	WM8326 = 0x8326,
};

struct wm831x;

typedef int (*wm831x_auxadc_read_fn)(struct wm831x *wm831x,
				     enum wm831x_auxadc input);

struct wm831x {
	struct mutex io_lock;

	struct device *dev;

	struct regmap *regmap;

	struct wm831x_pdata pdata;
	enum wm831x_parent type;

	int irq;  /* Our chip IRQ */
	struct mutex irq_lock;
	struct irq_domain *irq_domain;
	int irq_masks_cur[WM831X_NUM_IRQ_REGS];   /* Currently active value */
	int irq_masks_cache[WM831X_NUM_IRQ_REGS]; /* Cached hardware value */

	bool soft_shutdown;

	/* Chip revision based flags */
	unsigned has_gpio_ena:1;         /* Has GPIO enable bit */
	unsigned has_cs_sts:1;           /* Has current sink status bit */
	unsigned charger_irq_wake:1;     /* Are charger IRQs a wake source? */

	int num_gpio;

	/* Used by the interrupt controller code to post writes */
	int gpio_update[WM831X_NUM_GPIO_REGS];
	bool gpio_level_high[WM831X_NUM_GPIO_REGS];
	bool gpio_level_low[WM831X_NUM_GPIO_REGS];

	struct mutex auxadc_lock;
	struct list_head auxadc_pending;
	u16 auxadc_active;
	wm831x_auxadc_read_fn auxadc_read;

	/* The WM831x has a security key blocking access to certain
	 * registers.  The mutex is taken by the accessors for locking
	 * and unlocking the security key, locked is used to fail
	 * writes if the lock is held.
	 */
	struct mutex key_lock;
	unsigned int locked:1;
};

/* Device I/O API */
int wm831x_reg_read(struct wm831x *wm831x, unsigned short reg);
int wm831x_reg_write(struct wm831x *wm831x, unsigned short reg,
		 unsigned short val);
void wm831x_reg_lock(struct wm831x *wm831x);
int wm831x_reg_unlock(struct wm831x *wm831x);
int wm831x_set_bits(struct wm831x *wm831x, unsigned short reg,
		    unsigned short mask, unsigned short val);
int wm831x_bulk_read(struct wm831x *wm831x, unsigned short reg,
		     int count, u16 *buf);

int wm831x_device_init(struct wm831x *wm831x, int irq);
int wm831x_device_suspend(struct wm831x *wm831x);
void wm831x_device_shutdown(struct wm831x *wm831x);
int wm831x_irq_init(struct wm831x *wm831x, int irq);
void wm831x_irq_exit(struct wm831x *wm831x);
void wm831x_auxadc_init(struct wm831x *wm831x);

static inline int wm831x_irq(struct wm831x *wm831x, int irq)
{
	return irq_create_mapping(wm831x->irq_domain, irq);
}

extern struct regmap_config wm831x_regmap_config;

extern const struct of_device_id wm831x_of_match[];

#endif
