/* SPDX-License-Identifier: GPL-2.0-only */
/****************************************************************************
 * Driver for Solarflare network controllers and boards
 * Copyright 2005-2006 Fen Systems Ltd.
 * Copyright 2006-2012 Solarflare Communications Inc.
 */

#ifndef EFX_SELFTEST_H
#define EFX_SELFTEST_H

#include "net_driver.h"

/*
 * Self tests
 */

struct efx_loopback_self_tests {
	int tx_sent[EFX_MAX_TXQ_PER_CHANNEL];
	int tx_done[EFX_MAX_TXQ_PER_CHANNEL];
	int rx_good;
	int rx_bad;
};

#define EFX_MAX_PHY_TESTS 20

/* Efx self test results
 * For fields which are not counters, 1 indicates success and -1
 * indicates failure; 0 indicates test could not be run.
 */
struct efx_self_tests {
	/* online tests */
	int phy_alive;
	int nvram;
	int interrupt;
	int eventq_dma[EFX_MAX_CHANNELS];
	int eventq_int[EFX_MAX_CHANNELS];
	/* offline tests */
	int memory;
	int registers;
	int phy_ext[EFX_MAX_PHY_TESTS];
	struct efx_loopback_self_tests loopback[LOOPBACK_TEST_MAX + 1];
};

void efx_loopback_rx_packet(struct efx_nic *efx, const char *buf_ptr,
			    int pkt_len);
int efx_selftest(struct efx_nic *efx, struct efx_self_tests *tests,
		 unsigned flags);
void efx_selftest_async_init(struct efx_nic *efx);
void efx_selftest_async_start(struct efx_nic *efx);
void efx_selftest_async_cancel(struct efx_nic *efx);

#endif /* EFX_SELFTEST_H */
