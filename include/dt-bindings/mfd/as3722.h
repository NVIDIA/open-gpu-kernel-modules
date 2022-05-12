/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This header provides macros for ams AS3722 device bindings.
 *
 * Copyright (c) 2013, NVIDIA Corporation.
 *
 * Author: Laxman Dewangan <ldewangan@nvidia.com>
 *
 */

#ifndef __DT_BINDINGS_AS3722_H__
#define __DT_BINDINGS_AS3722_H__

/* External control pins */
#define AS3722_EXT_CONTROL_PIN_ENABLE1 1
#define AS3722_EXT_CONTROL_PIN_ENABLE2 2
#define AS3722_EXT_CONTROL_PIN_ENABLE3 3

/* Interrupt numbers for AS3722 */
#define AS3722_IRQ_LID			0
#define AS3722_IRQ_ACOK			1
#define AS3722_IRQ_ENABLE1		2
#define AS3722_IRQ_OCCUR_ALARM_SD0	3
#define AS3722_IRQ_ONKEY_LONG_PRESS	4
#define AS3722_IRQ_ONKEY		5
#define AS3722_IRQ_OVTMP		6
#define AS3722_IRQ_LOWBAT		7
#define AS3722_IRQ_SD0_LV		8
#define AS3722_IRQ_SD1_LV		9
#define AS3722_IRQ_SD2_LV		10
#define AS3722_IRQ_PWM1_OV_PROT		11
#define AS3722_IRQ_PWM2_OV_PROT		12
#define AS3722_IRQ_ENABLE2		13
#define AS3722_IRQ_SD6_LV		14
#define AS3722_IRQ_RTC_REP		15
#define AS3722_IRQ_RTC_ALARM		16
#define AS3722_IRQ_GPIO1		17
#define AS3722_IRQ_GPIO2		18
#define AS3722_IRQ_GPIO3		19
#define AS3722_IRQ_GPIO4		20
#define AS3722_IRQ_GPIO5		21
#define AS3722_IRQ_WATCHDOG		22
#define AS3722_IRQ_ENABLE3		23
#define AS3722_IRQ_TEMP_SD0_SHUTDOWN	24
#define AS3722_IRQ_TEMP_SD1_SHUTDOWN	25
#define AS3722_IRQ_TEMP_SD2_SHUTDOWN	26
#define AS3722_IRQ_TEMP_SD0_ALARM	27
#define AS3722_IRQ_TEMP_SD1_ALARM	28
#define AS3722_IRQ_TEMP_SD6_ALARM	29
#define AS3722_IRQ_OCCUR_ALARM_SD6	30
#define AS3722_IRQ_ADC			31

#endif /* __DT_BINDINGS_AS3722_H__ */
