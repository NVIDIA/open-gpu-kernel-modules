/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  linux/drivers/mfd/mcp.h
 *
 *  Copyright (C) 2001 Russell King, All Rights Reserved.
 */
#ifndef MCP_H
#define MCP_H

#include <linux/device.h>

struct mcp_ops;

struct mcp {
	struct module	*owner;
	struct mcp_ops	*ops;
	spinlock_t	lock;
	int		use_count;
	unsigned int	sclk_rate;
	unsigned int	rw_timeout;
	struct device	attached_device;
};

struct mcp_ops {
	void		(*set_telecom_divisor)(struct mcp *, unsigned int);
	void		(*set_audio_divisor)(struct mcp *, unsigned int);
	void		(*reg_write)(struct mcp *, unsigned int, unsigned int);
	unsigned int	(*reg_read)(struct mcp *, unsigned int);
	void		(*enable)(struct mcp *);
	void		(*disable)(struct mcp *);
};

void mcp_set_telecom_divisor(struct mcp *, unsigned int);
void mcp_set_audio_divisor(struct mcp *, unsigned int);
void mcp_reg_write(struct mcp *, unsigned int, unsigned int);
unsigned int mcp_reg_read(struct mcp *, unsigned int);
void mcp_enable(struct mcp *);
void mcp_disable(struct mcp *);
#define mcp_get_sclk_rate(mcp)	((mcp)->sclk_rate)

struct mcp *mcp_host_alloc(struct device *, size_t);
int mcp_host_add(struct mcp *, void *);
void mcp_host_del(struct mcp *);
void mcp_host_free(struct mcp *);

struct mcp_driver {
	struct device_driver drv;
	int (*probe)(struct mcp *);
	void (*remove)(struct mcp *);
};

int mcp_driver_register(struct mcp_driver *);
void mcp_driver_unregister(struct mcp_driver *);

#define mcp_get_drvdata(mcp)	dev_get_drvdata(&(mcp)->attached_device)
#define mcp_set_drvdata(mcp,d)	dev_set_drvdata(&(mcp)->attached_device, d)

static inline void *mcp_priv(struct mcp *mcp)
{
	return mcp + 1;
}

#endif
