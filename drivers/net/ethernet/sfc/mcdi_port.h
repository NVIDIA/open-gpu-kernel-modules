/* SPDX-License-Identifier: GPL-2.0-only */
/****************************************************************************
 * Driver for Solarflare network controllers and boards
 * Copyright 2008-2013 Solarflare Communications Inc.
 * Copyright 2019-2020 Xilinx Inc.
 */

#ifndef EFX_MCDI_PORT_H
#define EFX_MCDI_PORT_H

#include "net_driver.h"

u32 efx_mcdi_phy_get_caps(struct efx_nic *efx);
bool efx_mcdi_mac_check_fault(struct efx_nic *efx);
int efx_mcdi_port_probe(struct efx_nic *efx);
void efx_mcdi_port_remove(struct efx_nic *efx);

#endif /* EFX_MCDI_PORT_H */
