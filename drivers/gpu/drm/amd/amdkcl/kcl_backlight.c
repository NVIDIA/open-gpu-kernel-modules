// SPDX-License-Identifier: GPL-2.0-only
/*
 * Backlight Lowlevel Control Abstraction
 *
 * Copyright (C) 2003,2004 Hewlett-Packard Company
 *
 */
#include <kcl/kcl_backlight.h>

#ifndef HAVE_BACKLIGHT_DEVICE_SET_BRIGHTNESS
amdkcl_dummy_symbol(backlight_device_set_brightness, int, return 0,
	     struct backlight_device *bd, unsigned long brightness)
#endif

