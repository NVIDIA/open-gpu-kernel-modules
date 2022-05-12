// SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
/* Copyright(c) 2020 Intel Corporation */
#include "adf_accel_devices.h"
#include "adf_gen4_hw_data.h"

static u64 build_csr_ring_base_addr(dma_addr_t addr, u32 size)
{
	return BUILD_RING_BASE_ADDR(addr, size);
}

static u32 read_csr_ring_head(void __iomem *csr_base_addr, u32 bank, u32 ring)
{
	return READ_CSR_RING_HEAD(csr_base_addr, bank, ring);
}

static void write_csr_ring_head(void __iomem *csr_base_addr, u32 bank, u32 ring,
				u32 value)
{
	WRITE_CSR_RING_HEAD(csr_base_addr, bank, ring, value);
}

static u32 read_csr_ring_tail(void __iomem *csr_base_addr, u32 bank, u32 ring)
{
	return READ_CSR_RING_TAIL(csr_base_addr, bank, ring);
}

static void write_csr_ring_tail(void __iomem *csr_base_addr, u32 bank, u32 ring,
				u32 value)
{
	WRITE_CSR_RING_TAIL(csr_base_addr, bank, ring, value);
}

static u32 read_csr_e_stat(void __iomem *csr_base_addr, u32 bank)
{
	return READ_CSR_E_STAT(csr_base_addr, bank);
}

static void write_csr_ring_config(void __iomem *csr_base_addr, u32 bank, u32 ring,
				  u32 value)
{
	WRITE_CSR_RING_CONFIG(csr_base_addr, bank, ring, value);
}

static void write_csr_ring_base(void __iomem *csr_base_addr, u32 bank, u32 ring,
				dma_addr_t addr)
{
	WRITE_CSR_RING_BASE(csr_base_addr, bank, ring, addr);
}

static void write_csr_int_flag(void __iomem *csr_base_addr, u32 bank,
			       u32 value)
{
	WRITE_CSR_INT_FLAG(csr_base_addr, bank, value);
}

static void write_csr_int_srcsel(void __iomem *csr_base_addr, u32 bank)
{
	WRITE_CSR_INT_SRCSEL(csr_base_addr, bank);
}

static void write_csr_int_col_en(void __iomem *csr_base_addr, u32 bank, u32 value)
{
	WRITE_CSR_INT_COL_EN(csr_base_addr, bank, value);
}

static void write_csr_int_col_ctl(void __iomem *csr_base_addr, u32 bank,
				  u32 value)
{
	WRITE_CSR_INT_COL_CTL(csr_base_addr, bank, value);
}

static void write_csr_int_flag_and_col(void __iomem *csr_base_addr, u32 bank,
				       u32 value)
{
	WRITE_CSR_INT_FLAG_AND_COL(csr_base_addr, bank, value);
}

static void write_csr_ring_srv_arb_en(void __iomem *csr_base_addr, u32 bank,
				      u32 value)
{
	WRITE_CSR_RING_SRV_ARB_EN(csr_base_addr, bank, value);
}

void adf_gen4_init_hw_csr_ops(struct adf_hw_csr_ops *csr_ops)
{
	csr_ops->build_csr_ring_base_addr = build_csr_ring_base_addr;
	csr_ops->read_csr_ring_head = read_csr_ring_head;
	csr_ops->write_csr_ring_head = write_csr_ring_head;
	csr_ops->read_csr_ring_tail = read_csr_ring_tail;
	csr_ops->write_csr_ring_tail = write_csr_ring_tail;
	csr_ops->read_csr_e_stat = read_csr_e_stat;
	csr_ops->write_csr_ring_config = write_csr_ring_config;
	csr_ops->write_csr_ring_base = write_csr_ring_base;
	csr_ops->write_csr_int_flag = write_csr_int_flag;
	csr_ops->write_csr_int_srcsel = write_csr_int_srcsel;
	csr_ops->write_csr_int_col_en = write_csr_int_col_en;
	csr_ops->write_csr_int_col_ctl = write_csr_int_col_ctl;
	csr_ops->write_csr_int_flag_and_col = write_csr_int_flag_and_col;
	csr_ops->write_csr_ring_srv_arb_en = write_csr_ring_srv_arb_en;
}
EXPORT_SYMBOL_GPL(adf_gen4_init_hw_csr_ops);

static inline void adf_gen4_unpack_ssm_wdtimer(u64 value, u32 *upper,
					       u32 *lower)
{
	*lower = lower_32_bits(value);
	*upper = upper_32_bits(value);
}

void adf_gen4_set_ssm_wdtimer(struct adf_accel_dev *accel_dev)
{
	struct adf_hw_device_data *hw_data = accel_dev->hw_device;
	u64 timer_val_pke = ADF_SSM_WDT_PKE_DEFAULT_VALUE;
	u64 timer_val = ADF_SSM_WDT_DEFAULT_VALUE;
	u32 ssm_wdt_pke_high = 0;
	u32 ssm_wdt_pke_low = 0;
	u32 ssm_wdt_high = 0;
	u32 ssm_wdt_low = 0;
	void __iomem *pmisc_addr;
	struct adf_bar *pmisc;
	int pmisc_id;

	pmisc_id = hw_data->get_misc_bar_id(hw_data);
	pmisc = &GET_BARS(accel_dev)[pmisc_id];
	pmisc_addr = pmisc->virt_addr;

	/* Convert 64bit WDT timer value into 32bit values for
	 * mmio write to 32bit CSRs.
	 */
	adf_gen4_unpack_ssm_wdtimer(timer_val, &ssm_wdt_high, &ssm_wdt_low);
	adf_gen4_unpack_ssm_wdtimer(timer_val_pke, &ssm_wdt_pke_high,
				    &ssm_wdt_pke_low);

	/* Enable WDT for sym and dc */
	ADF_CSR_WR(pmisc_addr, ADF_SSMWDTL_OFFSET, ssm_wdt_low);
	ADF_CSR_WR(pmisc_addr, ADF_SSMWDTH_OFFSET, ssm_wdt_high);
	/* Enable WDT for pke */
	ADF_CSR_WR(pmisc_addr, ADF_SSMWDTPKEL_OFFSET, ssm_wdt_pke_low);
	ADF_CSR_WR(pmisc_addr, ADF_SSMWDTPKEH_OFFSET, ssm_wdt_pke_high);
}
EXPORT_SYMBOL_GPL(adf_gen4_set_ssm_wdtimer);
