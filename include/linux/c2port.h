/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  Silicon Labs C2 port Linux support
 *
 *  Copyright (c) 2007 Rodolfo Giometti <giometti@linux.it>
 *  Copyright (c) 2007 Eurotech S.p.A. <info@eurotech.it>
 */

#define C2PORT_NAME_LEN			32

struct device;

/*
 * C2 port basic structs
 */

/* Main struct */
struct c2port_ops;
struct c2port_device {
	unsigned int access:1;
	unsigned int flash_access:1;

	int id;
	char name[C2PORT_NAME_LEN];
	struct c2port_ops *ops;
	struct mutex mutex;		/* prevent races during read/write */

	struct device *dev;

	void *private_data;
};

/* Basic operations */
struct c2port_ops {
	/* Flash layout */
	unsigned short block_size;	/* flash block size in bytes */
	unsigned short blocks_num;	/* flash blocks number */

	/* Enable or disable the access to C2 port */
	void (*access)(struct c2port_device *dev, int status);

	/* Set C2D data line as input/output */
	void (*c2d_dir)(struct c2port_device *dev, int dir);

	/* Read/write C2D data line */
	int (*c2d_get)(struct c2port_device *dev);
	void (*c2d_set)(struct c2port_device *dev, int status);

	/* Write C2CK clock line */
	void (*c2ck_set)(struct c2port_device *dev, int status);
};

/*
 * Exported functions
 */

extern struct c2port_device *c2port_device_register(char *name,
					struct c2port_ops *ops, void *devdata);
extern void c2port_device_unregister(struct c2port_device *dev);
