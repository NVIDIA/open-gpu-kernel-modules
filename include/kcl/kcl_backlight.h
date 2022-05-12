/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Backlight Lowlevel Control Abstraction
 *
 * Copyright (C) 2003,2004 Hewlett-Packard Company
 *
 */
#ifndef AMDKCL_BACKLIGHT_H
#define AMDKCL_BACKLIGHT_H

#include <linux/backlight.h>
#ifndef HAVE_BACKLIGHT_DEVICE_SET_BRIGHTNESS
int backlight_device_set_brightness(struct backlight_device *bd,
				    unsigned long brightness);
#endif /* HAVE_BACKLIGHT_DEVICE_SET_BRIGHTNESS */
#endif
