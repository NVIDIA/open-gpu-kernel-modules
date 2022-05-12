// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2011 Google, Inc.
 */

#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/usb/otg.h>
#include <linux/usb/ulpi.h>

#define ULPI_VIEW_WAKEUP	(1 << 31)
#define ULPI_VIEW_RUN		(1 << 30)
#define ULPI_VIEW_WRITE		(1 << 29)
#define ULPI_VIEW_READ		(0 << 29)
#define ULPI_VIEW_ADDR(x)	(((x) & 0xff) << 16)
#define ULPI_VIEW_DATA_READ(x)	(((x) >> 8) & 0xff)
#define ULPI_VIEW_DATA_WRITE(x)	((x) & 0xff)

static int ulpi_viewport_wait(void __iomem *view, u32 mask)
{
	u32 val;

	return readl_poll_timeout_atomic(view, val, !(val & mask), 1, 2000);
}

static int ulpi_viewport_read(struct usb_phy *otg, u32 reg)
{
	int ret;
	void __iomem *view = otg->io_priv;

	writel(ULPI_VIEW_WAKEUP | ULPI_VIEW_WRITE, view);
	ret = ulpi_viewport_wait(view, ULPI_VIEW_WAKEUP);
	if (ret)
		return ret;

	writel(ULPI_VIEW_RUN | ULPI_VIEW_READ | ULPI_VIEW_ADDR(reg), view);
	ret = ulpi_viewport_wait(view, ULPI_VIEW_RUN);
	if (ret)
		return ret;

	return ULPI_VIEW_DATA_READ(readl(view));
}

static int ulpi_viewport_write(struct usb_phy *otg, u32 val, u32 reg)
{
	int ret;
	void __iomem *view = otg->io_priv;

	writel(ULPI_VIEW_WAKEUP | ULPI_VIEW_WRITE, view);
	ret = ulpi_viewport_wait(view, ULPI_VIEW_WAKEUP);
	if (ret)
		return ret;

	writel(ULPI_VIEW_RUN | ULPI_VIEW_WRITE | ULPI_VIEW_DATA_WRITE(val) |
						 ULPI_VIEW_ADDR(reg), view);

	return ulpi_viewport_wait(view, ULPI_VIEW_RUN);
}

struct usb_phy_io_ops ulpi_viewport_access_ops = {
	.read	= ulpi_viewport_read,
	.write	= ulpi_viewport_write,
};
EXPORT_SYMBOL_GPL(ulpi_viewport_access_ops);
