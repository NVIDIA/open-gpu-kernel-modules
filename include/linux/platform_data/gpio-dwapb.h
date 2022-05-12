/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright(c) 2014 Intel Corporation.
 */

#ifndef GPIO_DW_APB_H
#define GPIO_DW_APB_H

#define DWAPB_MAX_GPIOS		32

struct dwapb_port_property {
	struct fwnode_handle *fwnode;
	unsigned int	idx;
	unsigned int	ngpio;
	unsigned int	gpio_base;
	int		irq[DWAPB_MAX_GPIOS];
	bool		irq_shared;
};

struct dwapb_platform_data {
	struct dwapb_port_property *properties;
	unsigned int nports;
};

#endif
