/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 SiFive, Inc.
 * Zong Li
 */

#ifndef __SIFIVE_CLK_FU540_PRCI_H
#define __SIFIVE_CLK_FU540_PRCI_H

#include "sifive-prci.h"

#define NUM_CLOCK_FU540	4

extern struct __prci_clock __prci_init_clocks_fu540[NUM_CLOCK_FU540];

#endif /* __SIFIVE_CLK_FU540_PRCI_H */
