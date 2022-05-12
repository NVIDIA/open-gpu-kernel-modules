/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
	Mantis PCI bridge driver

	Copyright (C) Manu Abraham (abraham.manu@gmail.com)

*/

#ifndef __MANTIS_CORE_H
#define __MANTIS_CORE_H

#include "mantis_common.h"


#define FE_TYPE_SAT	0
#define FE_TYPE_CAB	1
#define FE_TYPE_TER	2

#define FE_TYPE_TS204	0
#define FE_TYPE_TS188	1


struct vendorname {
	u8  *sub_vendor_name;
	u32 sub_vendor_id;
};

struct devicetype {
	u8  *sub_device_name;
	u32 sub_device_id;
	u8  device_type;
	u32 type_flags;
};


extern int mantis_dma_init(struct mantis_pci *mantis);
extern int mantis_dma_exit(struct mantis_pci *mantis);
extern void mantis_dma_start(struct mantis_pci *mantis);
extern void mantis_dma_stop(struct mantis_pci *mantis);
extern int mantis_i2c_init(struct mantis_pci *mantis);
extern int mantis_i2c_exit(struct mantis_pci *mantis);

#endif /* __MANTIS_CORE_H */
