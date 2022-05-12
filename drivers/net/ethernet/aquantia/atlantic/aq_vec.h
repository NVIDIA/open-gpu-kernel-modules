/* SPDX-License-Identifier: GPL-2.0-only */
/* Atlantic Network Driver
 *
 * Copyright (C) 2014-2019 aQuantia Corporation
 * Copyright (C) 2019-2020 Marvell International Ltd.
 */

/* File aq_vec.h: Definition of common structures for vector of Rx and Tx rings.
 * Declaration of functions for Rx and Tx rings.
 */

#ifndef AQ_VEC_H
#define AQ_VEC_H

#include "aq_common.h"
#include <linux/irqreturn.h>

struct aq_hw_s;
struct aq_hw_ops;
struct aq_nic_s;
struct aq_nic_cfg_s;
struct aq_ring_stats_rx_s;
struct aq_ring_stats_tx_s;

irqreturn_t aq_vec_isr(int irq, void *private);
irqreturn_t aq_vec_isr_legacy(int irq, void *private);
struct aq_vec_s *aq_vec_alloc(struct aq_nic_s *aq_nic, unsigned int idx,
			      struct aq_nic_cfg_s *aq_nic_cfg);
int aq_vec_ring_alloc(struct aq_vec_s *self, struct aq_nic_s *aq_nic,
		      unsigned int idx, struct aq_nic_cfg_s *aq_nic_cfg);
int aq_vec_init(struct aq_vec_s *self, const struct aq_hw_ops *aq_hw_ops,
		struct aq_hw_s *aq_hw);
void aq_vec_deinit(struct aq_vec_s *self);
void aq_vec_free(struct aq_vec_s *self);
void aq_vec_ring_free(struct aq_vec_s *self);
int aq_vec_start(struct aq_vec_s *self);
void aq_vec_stop(struct aq_vec_s *self);
cpumask_t *aq_vec_get_affinity_mask(struct aq_vec_s *self);
bool aq_vec_is_valid_tc(struct aq_vec_s *self, const unsigned int tc);
unsigned int aq_vec_get_sw_stats(struct aq_vec_s *self, const unsigned int tc, u64 *data);

#endif /* AQ_VEC_H */
