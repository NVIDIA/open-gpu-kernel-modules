/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Driver for the ov7660 sensor
 *
 * Copyright (C) 2009 Erik Andrén
 * Copyright (C) 2007 Ilyes Gouta. Based on the m5603x Linux Driver Project.
 * Copyright (C) 2005 m5603x Linux Driver Project <m5602@x3ng.com.br>
 *
 * Portions of code to USB interface and ALi driver software,
 * Copyright (c) 2006 Willem Duinker
 * v4l2 interface modeled after the V4L2 driver
 * for SN9C10x PC Camera Controllers
 */

#ifndef M5602_OV7660_H_
#define M5602_OV7660_H_

#include "m5602_sensor.h"

#define OV7660_GAIN		0x00
#define OV7660_BLUE_GAIN	0x01
#define OV7660_RED_GAIN		0x02
#define OV7660_VREF		0x03
#define OV7660_COM1		0x04
#define OV7660_BAVE		0x05
#define OV7660_GEAVE		0x06
#define OV7660_AECHH		0x07
#define OV7660_RAVE		0x08
#define OV7660_COM2		0x09
#define OV7660_PID		0x0a
#define OV7660_VER		0x0b
#define OV7660_COM3		0x0c
#define OV7660_COM4		0x0d
#define OV7660_COM5		0x0e
#define OV7660_COM6		0x0f
#define OV7660_AECH		0x10
#define OV7660_CLKRC		0x11
#define OV7660_COM7		0x12
#define OV7660_COM8		0x13
#define OV7660_COM9		0x14
#define OV7660_COM10		0x15
#define OV7660_RSVD16		0x16
#define OV7660_HSTART		0x17
#define OV7660_HSTOP		0x18
#define OV7660_VSTART		0x19
#define OV7660_VSTOP		0x1a
#define OV7660_PSHFT		0x1b
#define OV7660_MIDH		0x1c
#define OV7660_MIDL		0x1d
#define OV7660_MVFP		0x1e
#define OV7660_LAEC		0x1f
#define OV7660_BOS		0x20
#define OV7660_GBOS		0x21
#define OV7660_GROS		0x22
#define OV7660_ROS		0x23
#define OV7660_AEW		0x24
#define OV7660_AEB		0x25
#define OV7660_VPT		0x26
#define OV7660_BBIAS		0x27
#define OV7660_GbBIAS		0x28
#define OV7660_RSVD29		0x29
#define OV7660_RBIAS		0x2c
#define OV7660_HREF		0x32
#define OV7660_ADC		0x37
#define OV7660_OFON		0x39
#define OV7660_TSLB		0x3a
#define OV7660_COM12		0x3c
#define OV7660_COM13		0x3d
#define OV7660_LCC1		0x62
#define OV7660_LCC2		0x63
#define OV7660_LCC3		0x64
#define OV7660_LCC4		0x65
#define OV7660_LCC5		0x66
#define OV7660_HV		0x69
#define OV7660_RSVDA1		0xa1

#define OV7660_DEFAULT_GAIN		0x0e
#define OV7660_DEFAULT_RED_GAIN		0x80
#define OV7660_DEFAULT_BLUE_GAIN	0x80
#define OV7660_DEFAULT_SATURATION	0x00
#define OV7660_DEFAULT_EXPOSURE		0x20

/* Kernel module parameters */
extern int force_sensor;
extern bool dump_sensor;

int ov7660_probe(struct sd *sd);
int ov7660_init(struct sd *sd);
int ov7660_init(struct sd *sd);
int ov7660_init_controls(struct sd *sd);
int ov7660_start(struct sd *sd);
int ov7660_stop(struct sd *sd);
void ov7660_disconnect(struct sd *sd);

static const struct m5602_sensor ov7660 = {
	.name = "ov7660",
	.i2c_slave_id = 0x42,
	.i2c_regW = 1,
	.probe = ov7660_probe,
	.init = ov7660_init,
	.init_controls = ov7660_init_controls,
	.start = ov7660_start,
	.stop = ov7660_stop,
	.disconnect = ov7660_disconnect,
};
#endif
