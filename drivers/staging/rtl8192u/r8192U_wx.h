/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This is part of rtl8180 OpenSource driver - v 0.3
 * Copyright (C) Andrea Merello 2004  <andrea.merello@gmail.com>
 *
 * Parts of this driver are based on the GPL part of the official realtek driver
 * Parts of this driver are based on the rtl8180 driver skeleton from Patric
 * Schenke & Andres Salomon
 * Parts of this driver are based on the Intel Pro Wireless 2100 GPL driver
 *
 * We want to thank the Authors of such projects and the Ndiswrapper project
 * Authors.
 */

/* this file (will) contains wireless extension handlers */

#ifndef R8180_WX_H
#define R8180_WX_H

extern const struct iw_handler_def r8192_wx_handlers_def;
/* Enable  the rtl819x_core.c to share this function, david 2008.9.22 */
struct iw_statistics *r8192_get_wireless_stats(struct net_device *dev);

#endif
