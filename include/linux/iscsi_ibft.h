/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  Copyright 2007 Red Hat, Inc.
 *  by Peter Jones <pjones@redhat.com>
 *  Copyright 2007 IBM, Inc.
 *  by Konrad Rzeszutek <konradr@linux.vnet.ibm.com>
 *  Copyright 2008
 *  by Konrad Rzeszutek <ketuzsezr@darnok.org>
 *
 * This code exposes the iSCSI Boot Format Table to userland via sysfs.
 */

#ifndef ISCSI_IBFT_H
#define ISCSI_IBFT_H

#include <linux/acpi.h>

/*
 * Logical location of iSCSI Boot Format Table.
 * If the value is NULL there is no iBFT on the machine.
 */
extern struct acpi_table_ibft *ibft_addr;

/*
 * Routine used to find and reserve the iSCSI Boot Format Table. The
 * mapped address is set in the ibft_addr variable.
 */
#ifdef CONFIG_ISCSI_IBFT_FIND
unsigned long find_ibft_region(unsigned long *sizep);
#else
static inline unsigned long find_ibft_region(unsigned long *sizep)
{
	*sizep = 0;
	return 0;
}
#endif

#endif /* ISCSI_IBFT_H */
