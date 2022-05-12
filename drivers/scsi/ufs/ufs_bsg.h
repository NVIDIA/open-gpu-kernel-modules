/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2018 Western Digital Corporation
 */
#ifndef UFS_BSG_H
#define UFS_BSG_H

#include <linux/bsg-lib.h>
#include <scsi/scsi.h>
#include <scsi/scsi_host.h>

#include "ufshcd.h"
#include "ufs.h"

#ifdef CONFIG_SCSI_UFS_BSG
void ufs_bsg_remove(struct ufs_hba *hba);
int ufs_bsg_probe(struct ufs_hba *hba);
#else
static inline void ufs_bsg_remove(struct ufs_hba *hba) {}
static inline int ufs_bsg_probe(struct ufs_hba *hba) {return 0; }
#endif

#endif /* UFS_BSG_H */
