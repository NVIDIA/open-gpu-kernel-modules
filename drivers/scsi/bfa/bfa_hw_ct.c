// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2005-2014 Brocade Communications Systems, Inc.
 * Copyright (c) 2014- QLogic Corporation.
 * All rights reserved
 * www.qlogic.com
 *
 * Linux driver for QLogic BR-series Fibre Channel Host Bus Adapter.
 */

#include "bfad_drv.h"
#include "bfa_modules.h"
#include "bfi_reg.h"

BFA_TRC_FILE(HAL, IOCFC_CT);

/*
 * Dummy interrupt handler for handling spurious interrupt during chip-reinit.
 */
static void
bfa_hwct_msix_dummy(struct bfa_s *bfa, int vec)
{
}

void
bfa_hwct_reginit(struct bfa_s *bfa)
{
	struct bfa_iocfc_regs_s	*bfa_regs = &bfa->iocfc.bfa_regs;
	void __iomem *kva = bfa_ioc_bar0(&bfa->ioc);
	int	fn = bfa_ioc_pcifn(&bfa->ioc);

	if (fn == 0) {
		bfa_regs->intr_status = (kva + HOSTFN0_INT_STATUS);
		bfa_regs->intr_mask   = (kva + HOSTFN0_INT_MSK);
	} else {
		bfa_regs->intr_status = (kva + HOSTFN1_INT_STATUS);
		bfa_regs->intr_mask   = (kva + HOSTFN1_INT_MSK);
	}
}

void
bfa_hwct2_reginit(struct bfa_s *bfa)
{
	struct bfa_iocfc_regs_s *bfa_regs = &bfa->iocfc.bfa_regs;
	void __iomem	*kva = bfa_ioc_bar0(&bfa->ioc);

	bfa_regs->intr_status = (kva + CT2_HOSTFN_INT_STATUS);
	bfa_regs->intr_mask   = (kva + CT2_HOSTFN_INTR_MASK);
}

void
bfa_hwct_reqq_ack(struct bfa_s *bfa, int reqq)
{
	u32	r32;

	r32 = readl(bfa->iocfc.bfa_regs.cpe_q_ctrl[reqq]);
	writel(r32, bfa->iocfc.bfa_regs.cpe_q_ctrl[reqq]);
}

/*
 * Actions to respond RME Interrupt for Catapult ASIC:
 * - Write 1 to Interrupt Status register (INTx only - done in bfa_intx())
 * - Acknowledge by writing to RME Queue Control register
 * - Update CI
 */
void
bfa_hwct_rspq_ack(struct bfa_s *bfa, int rspq, u32 ci)
{
	u32	r32;

	r32 = readl(bfa->iocfc.bfa_regs.rme_q_ctrl[rspq]);
	writel(r32, bfa->iocfc.bfa_regs.rme_q_ctrl[rspq]);

	bfa_rspq_ci(bfa, rspq) = ci;
	writel(ci, bfa->iocfc.bfa_regs.rme_q_ci[rspq]);
}

/*
 * Actions to respond RME Interrupt for Catapult2 ASIC:
 * - Write 1 to Interrupt Status register (INTx only - done in bfa_intx())
 * - Update CI
 */
void
bfa_hwct2_rspq_ack(struct bfa_s *bfa, int rspq, u32 ci)
{
	bfa_rspq_ci(bfa, rspq) = ci;
	writel(ci, bfa->iocfc.bfa_regs.rme_q_ci[rspq]);
}

void
bfa_hwct_msix_getvecs(struct bfa_s *bfa, u32 *msix_vecs_bmap,
		 u32 *num_vecs, u32 *max_vec_bit)
{
	*msix_vecs_bmap = (1 << BFI_MSIX_CT_MAX) - 1;
	*max_vec_bit = (1 << (BFI_MSIX_CT_MAX - 1));
	*num_vecs = BFI_MSIX_CT_MAX;
}

/*
 * Setup MSI-X vector for catapult
 */
void
bfa_hwct_msix_init(struct bfa_s *bfa, int nvecs)
{
	WARN_ON((nvecs != 1) && (nvecs != BFI_MSIX_CT_MAX));
	bfa_trc(bfa, nvecs);

	bfa->msix.nvecs = nvecs;
	bfa_hwct_msix_uninstall(bfa);
}

void
bfa_hwct_msix_ctrl_install(struct bfa_s *bfa)
{
	if (bfa->msix.nvecs == 0)
		return;

	if (bfa->msix.nvecs == 1)
		bfa->msix.handler[BFI_MSIX_LPU_ERR_CT] = bfa_msix_all;
	else
		bfa->msix.handler[BFI_MSIX_LPU_ERR_CT] = bfa_msix_lpu_err;
}

void
bfa_hwct_msix_queue_install(struct bfa_s *bfa)
{
	int i;

	if (bfa->msix.nvecs == 0)
		return;

	if (bfa->msix.nvecs == 1) {
		for (i = BFI_MSIX_CPE_QMIN_CT; i < BFI_MSIX_CT_MAX; i++)
			bfa->msix.handler[i] = bfa_msix_all;
		return;
	}

	for (i = BFI_MSIX_CPE_QMIN_CT; i <= BFI_MSIX_CPE_QMAX_CT; i++)
		bfa->msix.handler[i] = bfa_msix_reqq;

	for (i = BFI_MSIX_RME_QMIN_CT; i <= BFI_MSIX_RME_QMAX_CT; i++)
		bfa->msix.handler[i] = bfa_msix_rspq;
}

void
bfa_hwct_msix_uninstall(struct bfa_s *bfa)
{
	int i;

	for (i = 0; i < BFI_MSIX_CT_MAX; i++)
		bfa->msix.handler[i] = bfa_hwct_msix_dummy;
}

/*
 * Enable MSI-X vectors
 */
void
bfa_hwct_isr_mode_set(struct bfa_s *bfa, bfa_boolean_t msix)
{
	bfa_trc(bfa, 0);
	bfa_ioc_isr_mode_set(&bfa->ioc, msix);
}

void
bfa_hwct_msix_get_rme_range(struct bfa_s *bfa, u32 *start, u32 *end)
{
	*start = BFI_MSIX_RME_QMIN_CT;
	*end = BFI_MSIX_RME_QMAX_CT;
}
