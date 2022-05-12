/* SPDX-License-Identifier: BSD-3-Clause-Clear */
/*
 * Copyright (c) 2018-2019 The Linux Foundation. All rights reserved.
 */
#ifndef ATH11K_AHB_H
#define ATH11K_AHB_H

#include "core.h"

#define ATH11K_AHB_RECOVERY_TIMEOUT (3 * HZ)
struct ath11k_base;

struct ath11k_ahb {
	struct rproc *tgt_rproc;
};

static inline struct ath11k_ahb *ath11k_ahb_priv(struct ath11k_base *ab)
{
	return (struct ath11k_ahb *)ab->drv_priv;
}
#endif
