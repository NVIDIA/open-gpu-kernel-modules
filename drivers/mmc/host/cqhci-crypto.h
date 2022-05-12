/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * CQHCI crypto engine (inline encryption) support
 *
 * Copyright 2020 Google LLC
 */

#ifndef LINUX_MMC_CQHCI_CRYPTO_H
#define LINUX_MMC_CQHCI_CRYPTO_H

#include <linux/mmc/host.h>

#include "cqhci.h"

#ifdef CONFIG_MMC_CRYPTO

int cqhci_crypto_init(struct cqhci_host *host);

/*
 * Returns the crypto bits that should be set in bits 64-127 of the
 * task descriptor.
 */
static inline u64 cqhci_crypto_prep_task_desc(struct mmc_request *mrq)
{
	if (!mrq->crypto_enabled)
		return 0;

	return CQHCI_CRYPTO_ENABLE_BIT |
	       CQHCI_CRYPTO_KEYSLOT(mrq->crypto_key_slot) |
	       mrq->data_unit_num;
}

#else /* CONFIG_MMC_CRYPTO */

static inline int cqhci_crypto_init(struct cqhci_host *host)
{
	return 0;
}

static inline u64 cqhci_crypto_prep_task_desc(struct mmc_request *mrq)
{
	return 0;
}

#endif /* !CONFIG_MMC_CRYPTO */

#endif /* LINUX_MMC_CQHCI_CRYPTO_H */
