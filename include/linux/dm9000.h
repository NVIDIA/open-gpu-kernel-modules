/* SPDX-License-Identifier: GPL-2.0-only */
/* include/linux/dm9000.h
 *
 * Copyright (c) 2004 Simtec Electronics
 *   Ben Dooks <ben@simtec.co.uk>
 *
 * Header file for dm9000 platform data
*/

#ifndef __DM9000_PLATFORM_DATA
#define __DM9000_PLATFORM_DATA __FILE__

#include <linux/if_ether.h>

/* IO control flags */

#define DM9000_PLATF_8BITONLY	(0x0001)
#define DM9000_PLATF_16BITONLY	(0x0002)
#define DM9000_PLATF_32BITONLY	(0x0004)
#define DM9000_PLATF_EXT_PHY	(0x0008)
#define DM9000_PLATF_NO_EEPROM	(0x0010)
#define DM9000_PLATF_SIMPLE_PHY (0x0020)  /* Use NSR to find LinkStatus */

/* platform data for platform device structure's platform_data field */

struct dm9000_plat_data {
	unsigned int	flags;
	unsigned char	dev_addr[ETH_ALEN];

	/* allow replacement IO routines */

	void	(*inblk)(void __iomem *reg, void *data, int len);
	void	(*outblk)(void __iomem *reg, void *data, int len);
	void	(*dumpblk)(void __iomem *reg, int len);
};

#endif /* __DM9000_PLATFORM_DATA */

