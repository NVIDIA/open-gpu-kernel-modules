// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2017 Chelsio Communications.  All rights reserved.
 */

#include <linux/sort.h>
#include <linux/string.h>

#include "t4_regs.h"
#include "cxgb4.h"
#include "cxgb4_cudbg.h"
#include "cudbg_if.h"
#include "cudbg_lib_common.h"
#include "cudbg_entity.h"
#include "cudbg_lib.h"
#include "cudbg_zlib.h"

static const u32 t6_tp_pio_array[][IREG_NUM_ELEM] = {
	{0x7e40, 0x7e44, 0x020, 28}, /* t6_tp_pio_regs_20_to_3b */
	{0x7e40, 0x7e44, 0x040, 10}, /* t6_tp_pio_regs_40_to_49 */
	{0x7e40, 0x7e44, 0x050, 10}, /* t6_tp_pio_regs_50_to_59 */
	{0x7e40, 0x7e44, 0x060, 14}, /* t6_tp_pio_regs_60_to_6d */
	{0x7e40, 0x7e44, 0x06F, 1}, /* t6_tp_pio_regs_6f */
	{0x7e40, 0x7e44, 0x070, 6}, /* t6_tp_pio_regs_70_to_75 */
	{0x7e40, 0x7e44, 0x130, 18}, /* t6_tp_pio_regs_130_to_141 */
	{0x7e40, 0x7e44, 0x145, 19}, /* t6_tp_pio_regs_145_to_157 */
	{0x7e40, 0x7e44, 0x160, 1}, /* t6_tp_pio_regs_160 */
	{0x7e40, 0x7e44, 0x230, 25}, /* t6_tp_pio_regs_230_to_248 */
	{0x7e40, 0x7e44, 0x24a, 3}, /* t6_tp_pio_regs_24c */
	{0x7e40, 0x7e44, 0x8C0, 1} /* t6_tp_pio_regs_8c0 */
};

static const u32 t5_tp_pio_array[][IREG_NUM_ELEM] = {
	{0x7e40, 0x7e44, 0x020, 28}, /* t5_tp_pio_regs_20_to_3b */
	{0x7e40, 0x7e44, 0x040, 19}, /* t5_tp_pio_regs_40_to_52 */
	{0x7e40, 0x7e44, 0x054, 2}, /* t5_tp_pio_regs_54_to_55 */
	{0x7e40, 0x7e44, 0x060, 13}, /* t5_tp_pio_regs_60_to_6c */
	{0x7e40, 0x7e44, 0x06F, 1}, /* t5_tp_pio_regs_6f */
	{0x7e40, 0x7e44, 0x120, 4}, /* t5_tp_pio_regs_120_to_123 */
	{0x7e40, 0x7e44, 0x12b, 2}, /* t5_tp_pio_regs_12b_to_12c */
	{0x7e40, 0x7e44, 0x12f, 21}, /* t5_tp_pio_regs_12f_to_143 */
	{0x7e40, 0x7e44, 0x145, 19}, /* t5_tp_pio_regs_145_to_157 */
	{0x7e40, 0x7e44, 0x230, 25}, /* t5_tp_pio_regs_230_to_248 */
	{0x7e40, 0x7e44, 0x8C0, 1} /* t5_tp_pio_regs_8c0 */
};

static const u32 t6_tp_tm_pio_array[][IREG_NUM_ELEM] = {
	{0x7e18, 0x7e1c, 0x0, 12}
};

static const u32 t5_tp_tm_pio_array[][IREG_NUM_ELEM] = {
	{0x7e18, 0x7e1c, 0x0, 12}
};

static const u32 t6_tp_mib_index_array[6][IREG_NUM_ELEM] = {
	{0x7e50, 0x7e54, 0x0, 13},
	{0x7e50, 0x7e54, 0x10, 6},
	{0x7e50, 0x7e54, 0x18, 21},
	{0x7e50, 0x7e54, 0x30, 32},
	{0x7e50, 0x7e54, 0x50, 22},
	{0x7e50, 0x7e54, 0x68, 12}
};

static const u32 t5_tp_mib_index_array[9][IREG_NUM_ELEM] = {
	{0x7e50, 0x7e54, 0x0, 13},
	{0x7e50, 0x7e54, 0x10, 6},
	{0x7e50, 0x7e54, 0x18, 8},
	{0x7e50, 0x7e54, 0x20, 13},
	{0x7e50, 0x7e54, 0x30, 16},
	{0x7e50, 0x7e54, 0x40, 16},
	{0x7e50, 0x7e54, 0x50, 16},
	{0x7e50, 0x7e54, 0x60, 6},
	{0x7e50, 0x7e54, 0x68, 4}
};

static const u32 t5_sge_dbg_index_array[2][IREG_NUM_ELEM] = {
	{0x10cc, 0x10d0, 0x0, 16},
	{0x10cc, 0x10d4, 0x0, 16},
};

static const u32 t6_sge_qbase_index_array[] = {
	/* 1 addr reg SGE_QBASE_INDEX and 4 data reg SGE_QBASE_MAP[0-3] */
	0x1250, 0x1240, 0x1244, 0x1248, 0x124c,
};

static const u32 t5_pcie_pdbg_array[][IREG_NUM_ELEM] = {
	{0x5a04, 0x5a0c, 0x00, 0x20}, /* t5_pcie_pdbg_regs_00_to_20 */
	{0x5a04, 0x5a0c, 0x21, 0x20}, /* t5_pcie_pdbg_regs_21_to_40 */
	{0x5a04, 0x5a0c, 0x41, 0x10}, /* t5_pcie_pdbg_regs_41_to_50 */
};

static const u32 t5_pcie_cdbg_array[][IREG_NUM_ELEM] = {
	{0x5a10, 0x5a18, 0x00, 0x20}, /* t5_pcie_cdbg_regs_00_to_20 */
	{0x5a10, 0x5a18, 0x21, 0x18}, /* t5_pcie_cdbg_regs_21_to_37 */
};

static const u32 t5_pm_rx_array[][IREG_NUM_ELEM] = {
	{0x8FD0, 0x8FD4, 0x10000, 0x20}, /* t5_pm_rx_regs_10000_to_10020 */
	{0x8FD0, 0x8FD4, 0x10021, 0x0D}, /* t5_pm_rx_regs_10021_to_1002c */
};

static const u32 t5_pm_tx_array[][IREG_NUM_ELEM] = {
	{0x8FF0, 0x8FF4, 0x10000, 0x20}, /* t5_pm_tx_regs_10000_to_10020 */
	{0x8FF0, 0x8FF4, 0x10021, 0x1D}, /* t5_pm_tx_regs_10021_to_1003c */
};

static const u32 t5_pcie_config_array[][2] = {
	{0x0, 0x34},
	{0x3c, 0x40},
	{0x50, 0x64},
	{0x70, 0x80},
	{0x94, 0xa0},
	{0xb0, 0xb8},
	{0xd0, 0xd4},
	{0x100, 0x128},
	{0x140, 0x148},
	{0x150, 0x164},
	{0x170, 0x178},
	{0x180, 0x194},
	{0x1a0, 0x1b8},
	{0x1c0, 0x208},
};

static const u32 t6_ma_ireg_array[][IREG_NUM_ELEM] = {
	{0x78f8, 0x78fc, 0xa000, 23}, /* t6_ma_regs_a000_to_a016 */
	{0x78f8, 0x78fc, 0xa400, 30}, /* t6_ma_regs_a400_to_a41e */
	{0x78f8, 0x78fc, 0xa800, 20} /* t6_ma_regs_a800_to_a813 */
};

static const u32 t6_ma_ireg_array2[][IREG_NUM_ELEM] = {
	{0x78f8, 0x78fc, 0xe400, 17}, /* t6_ma_regs_e400_to_e600 */
	{0x78f8, 0x78fc, 0xe640, 13} /* t6_ma_regs_e640_to_e7c0 */
};

static const u32 t6_up_cim_reg_array[][IREG_NUM_ELEM + 1] = {
	{0x7b50, 0x7b54, 0x2000, 0x20, 0}, /* up_cim_2000_to_207c */
	{0x7b50, 0x7b54, 0x2080, 0x1d, 0}, /* up_cim_2080_to_20fc */
	{0x7b50, 0x7b54, 0x00, 0x20, 0}, /* up_cim_00_to_7c */
	{0x7b50, 0x7b54, 0x80, 0x20, 0}, /* up_cim_80_to_fc */
	{0x7b50, 0x7b54, 0x100, 0x11, 0}, /* up_cim_100_to_14c */
	{0x7b50, 0x7b54, 0x200, 0x10, 0}, /* up_cim_200_to_23c */
	{0x7b50, 0x7b54, 0x240, 0x2, 0}, /* up_cim_240_to_244 */
	{0x7b50, 0x7b54, 0x250, 0x2, 0}, /* up_cim_250_to_254 */
	{0x7b50, 0x7b54, 0x260, 0x2, 0}, /* up_cim_260_to_264 */
	{0x7b50, 0x7b54, 0x270, 0x2, 0}, /* up_cim_270_to_274 */
	{0x7b50, 0x7b54, 0x280, 0x20, 0}, /* up_cim_280_to_2fc */
	{0x7b50, 0x7b54, 0x300, 0x20, 0}, /* up_cim_300_to_37c */
	{0x7b50, 0x7b54, 0x380, 0x14, 0}, /* up_cim_380_to_3cc */
	{0x7b50, 0x7b54, 0x4900, 0x4, 0x4}, /* up_cim_4900_to_4c60 */
	{0x7b50, 0x7b54, 0x4904, 0x4, 0x4}, /* up_cim_4904_to_4c64 */
	{0x7b50, 0x7b54, 0x4908, 0x4, 0x4}, /* up_cim_4908_to_4c68 */
	{0x7b50, 0x7b54, 0x4910, 0x4, 0x4}, /* up_cim_4910_to_4c70 */
	{0x7b50, 0x7b54, 0x4914, 0x4, 0x4}, /* up_cim_4914_to_4c74 */
	{0x7b50, 0x7b54, 0x4920, 0x10, 0x10}, /* up_cim_4920_to_4a10 */
	{0x7b50, 0x7b54, 0x4924, 0x10, 0x10}, /* up_cim_4924_to_4a14 */
	{0x7b50, 0x7b54, 0x4928, 0x10, 0x10}, /* up_cim_4928_to_4a18 */
	{0x7b50, 0x7b54, 0x492c, 0x10, 0x10}, /* up_cim_492c_to_4a1c */
};

static const u32 t5_up_cim_reg_array[][IREG_NUM_ELEM + 1] = {
	{0x7b50, 0x7b54, 0x2000, 0x20, 0}, /* up_cim_2000_to_207c */
	{0x7b50, 0x7b54, 0x2080, 0x19, 0}, /* up_cim_2080_to_20ec */
	{0x7b50, 0x7b54, 0x00, 0x20, 0}, /* up_cim_00_to_7c */
	{0x7b50, 0x7b54, 0x80, 0x20, 0}, /* up_cim_80_to_fc */
	{0x7b50, 0x7b54, 0x100, 0x11, 0}, /* up_cim_100_to_14c */
	{0x7b50, 0x7b54, 0x200, 0x10, 0}, /* up_cim_200_to_23c */
	{0x7b50, 0x7b54, 0x240, 0x2, 0}, /* up_cim_240_to_244 */
	{0x7b50, 0x7b54, 0x250, 0x2, 0}, /* up_cim_250_to_254 */
	{0x7b50, 0x7b54, 0x260, 0x2, 0}, /* up_cim_260_to_264 */
	{0x7b50, 0x7b54, 0x270, 0x2, 0}, /* up_cim_270_to_274 */
	{0x7b50, 0x7b54, 0x280, 0x20, 0}, /* up_cim_280_to_2fc */
	{0x7b50, 0x7b54, 0x300, 0x20, 0}, /* up_cim_300_to_37c */
	{0x7b50, 0x7b54, 0x380, 0x14, 0}, /* up_cim_380_to_3cc */
};

static const u32 t6_hma_ireg_array[][IREG_NUM_ELEM] = {
	{0x51320, 0x51324, 0xa000, 32} /* t6_hma_regs_a000_to_a01f */
};

u32 cudbg_get_entity_length(struct adapter *adap, u32 entity)
{
	struct cudbg_tcam tcam_region = { 0 };
	u32 value, n = 0, len = 0;

	switch (entity) {
	case CUDBG_REG_DUMP:
		switch (CHELSIO_CHIP_VERSION(adap->params.chip)) {
		case CHELSIO_T4:
			len = T4_REGMAP_SIZE;
			break;
		case CHELSIO_T5:
		case CHELSIO_T6:
			len = T5_REGMAP_SIZE;
			break;
		default:
			break;
		}
		break;
	case CUDBG_DEV_LOG:
		len = adap->params.devlog.size;
		break;
	case CUDBG_CIM_LA:
		if (is_t6(adap->params.chip)) {
			len = adap->params.cim_la_size / 10 + 1;
			len *= 10 * sizeof(u32);
		} else {
			len = adap->params.cim_la_size / 8;
			len *= 8 * sizeof(u32);
		}
		len += sizeof(u32); /* for reading CIM LA configuration */
		break;
	case CUDBG_CIM_MA_LA:
		len = 2 * CIM_MALA_SIZE * 5 * sizeof(u32);
		break;
	case CUDBG_CIM_QCFG:
		len = sizeof(struct cudbg_cim_qcfg);
		break;
	case CUDBG_CIM_IBQ_TP0:
	case CUDBG_CIM_IBQ_TP1:
	case CUDBG_CIM_IBQ_ULP:
	case CUDBG_CIM_IBQ_SGE0:
	case CUDBG_CIM_IBQ_SGE1:
	case CUDBG_CIM_IBQ_NCSI:
		len = CIM_IBQ_SIZE * 4 * sizeof(u32);
		break;
	case CUDBG_CIM_OBQ_ULP0:
		len = cudbg_cim_obq_size(adap, 0);
		break;
	case CUDBG_CIM_OBQ_ULP1:
		len = cudbg_cim_obq_size(adap, 1);
		break;
	case CUDBG_CIM_OBQ_ULP2:
		len = cudbg_cim_obq_size(adap, 2);
		break;
	case CUDBG_CIM_OBQ_ULP3:
		len = cudbg_cim_obq_size(adap, 3);
		break;
	case CUDBG_CIM_OBQ_SGE:
		len = cudbg_cim_obq_size(adap, 4);
		break;
	case CUDBG_CIM_OBQ_NCSI:
		len = cudbg_cim_obq_size(adap, 5);
		break;
	case CUDBG_CIM_OBQ_RXQ0:
		len = cudbg_cim_obq_size(adap, 6);
		break;
	case CUDBG_CIM_OBQ_RXQ1:
		len = cudbg_cim_obq_size(adap, 7);
		break;
	case CUDBG_EDC0:
		value = t4_read_reg(adap, MA_TARGET_MEM_ENABLE_A);
		if (value & EDRAM0_ENABLE_F) {
			value = t4_read_reg(adap, MA_EDRAM0_BAR_A);
			len = EDRAM0_SIZE_G(value);
		}
		len = cudbg_mbytes_to_bytes(len);
		break;
	case CUDBG_EDC1:
		value = t4_read_reg(adap, MA_TARGET_MEM_ENABLE_A);
		if (value & EDRAM1_ENABLE_F) {
			value = t4_read_reg(adap, MA_EDRAM1_BAR_A);
			len = EDRAM1_SIZE_G(value);
		}
		len = cudbg_mbytes_to_bytes(len);
		break;
	case CUDBG_MC0:
		value = t4_read_reg(adap, MA_TARGET_MEM_ENABLE_A);
		if (value & EXT_MEM0_ENABLE_F) {
			value = t4_read_reg(adap, MA_EXT_MEMORY0_BAR_A);
			len = EXT_MEM0_SIZE_G(value);
		}
		len = cudbg_mbytes_to_bytes(len);
		break;
	case CUDBG_MC1:
		value = t4_read_reg(adap, MA_TARGET_MEM_ENABLE_A);
		if (value & EXT_MEM1_ENABLE_F) {
			value = t4_read_reg(adap, MA_EXT_MEMORY1_BAR_A);
			len = EXT_MEM1_SIZE_G(value);
		}
		len = cudbg_mbytes_to_bytes(len);
		break;
	case CUDBG_RSS:
		len = t4_chip_rss_size(adap) * sizeof(u16);
		break;
	case CUDBG_RSS_VF_CONF:
		len = adap->params.arch.vfcount *
		      sizeof(struct cudbg_rss_vf_conf);
		break;
	case CUDBG_PATH_MTU:
		len = NMTUS * sizeof(u16);
		break;
	case CUDBG_PM_STATS:
		len = sizeof(struct cudbg_pm_stats);
		break;
	case CUDBG_HW_SCHED:
		len = sizeof(struct cudbg_hw_sched);
		break;
	case CUDBG_TP_INDIRECT:
		switch (CHELSIO_CHIP_VERSION(adap->params.chip)) {
		case CHELSIO_T5:
			n = sizeof(t5_tp_pio_array) +
			    sizeof(t5_tp_tm_pio_array) +
			    sizeof(t5_tp_mib_index_array);
			break;
		case CHELSIO_T6:
			n = sizeof(t6_tp_pio_array) +
			    sizeof(t6_tp_tm_pio_array) +
			    sizeof(t6_tp_mib_index_array);
			break;
		default:
			break;
		}
		n = n / (IREG_NUM_ELEM * sizeof(u32));
		len = sizeof(struct ireg_buf) * n;
		break;
	case CUDBG_SGE_INDIRECT:
		len = sizeof(struct ireg_buf) * 2 +
		      sizeof(struct sge_qbase_reg_field);
		break;
	case CUDBG_ULPRX_LA:
		len = sizeof(struct cudbg_ulprx_la);
		break;
	case CUDBG_TP_LA:
		len = sizeof(struct cudbg_tp_la) + TPLA_SIZE * sizeof(u64);
		break;
	case CUDBG_MEMINFO:
		len = sizeof(struct cudbg_ver_hdr) +
		      sizeof(struct cudbg_meminfo);
		break;
	case CUDBG_CIM_PIF_LA:
		len = sizeof(struct cudbg_cim_pif_la);
		len += 2 * CIM_PIFLA_SIZE * 6 * sizeof(u32);
		break;
	case CUDBG_CLK:
		len = sizeof(struct cudbg_clk_info);
		break;
	case CUDBG_PCIE_INDIRECT:
		n = sizeof(t5_pcie_pdbg_array) / (IREG_NUM_ELEM * sizeof(u32));
		len = sizeof(struct ireg_buf) * n * 2;
		break;
	case CUDBG_PM_INDIRECT:
		n = sizeof(t5_pm_rx_array) / (IREG_NUM_ELEM * sizeof(u32));
		len = sizeof(struct ireg_buf) * n * 2;
		break;
	case CUDBG_TID_INFO:
		len = sizeof(struct cudbg_tid_info_region_rev1);
		break;
	case CUDBG_PCIE_CONFIG:
		len = sizeof(u32) * CUDBG_NUM_PCIE_CONFIG_REGS;
		break;
	case CUDBG_DUMP_CONTEXT:
		len = cudbg_dump_context_size(adap);
		break;
	case CUDBG_MPS_TCAM:
		len = sizeof(struct cudbg_mps_tcam) *
		      adap->params.arch.mps_tcam_size;
		break;
	case CUDBG_VPD_DATA:
		len = sizeof(struct cudbg_vpd_data);
		break;
	case CUDBG_LE_TCAM:
		cudbg_fill_le_tcam_info(adap, &tcam_region);
		len = sizeof(struct cudbg_tcam) +
		      sizeof(struct cudbg_tid_data) * tcam_region.max_tid;
		break;
	case CUDBG_CCTRL:
		len = sizeof(u16) * NMTUS * NCCTRL_WIN;
		break;
	case CUDBG_MA_INDIRECT:
		if (CHELSIO_CHIP_VERSION(adap->params.chip) > CHELSIO_T5) {
			n = sizeof(t6_ma_ireg_array) /
			    (IREG_NUM_ELEM * sizeof(u32));
			len = sizeof(struct ireg_buf) * n * 2;
		}
		break;
	case CUDBG_ULPTX_LA:
		len = sizeof(struct cudbg_ver_hdr) +
		      sizeof(struct cudbg_ulptx_la);
		break;
	case CUDBG_UP_CIM_INDIRECT:
		n = 0;
		if (is_t5(adap->params.chip))
			n = sizeof(t5_up_cim_reg_array) /
			    ((IREG_NUM_ELEM + 1) * sizeof(u32));
		else if (is_t6(adap->params.chip))
			n = sizeof(t6_up_cim_reg_array) /
			    ((IREG_NUM_ELEM + 1) * sizeof(u32));
		len = sizeof(struct ireg_buf) * n;
		break;
	case CUDBG_PBT_TABLE:
		len = sizeof(struct cudbg_pbt_tables);
		break;
	case CUDBG_MBOX_LOG:
		len = sizeof(struct cudbg_mbox_log) * adap->mbox_log->size;
		break;
	case CUDBG_HMA_INDIRECT:
		if (CHELSIO_CHIP_VERSION(adap->params.chip) > CHELSIO_T5) {
			n = sizeof(t6_hma_ireg_array) /
			    (IREG_NUM_ELEM * sizeof(u32));
			len = sizeof(struct ireg_buf) * n;
		}
		break;
	case CUDBG_HMA:
		value = t4_read_reg(adap, MA_TARGET_MEM_ENABLE_A);
		if (value & HMA_MUX_F) {
			/* In T6, there's no MC1.  So, HMA shares MC1
			 * address space.
			 */
			value = t4_read_reg(adap, MA_EXT_MEMORY1_BAR_A);
			len = EXT_MEM1_SIZE_G(value);
		}
		len = cudbg_mbytes_to_bytes(len);
		break;
	case CUDBG_QDESC:
		cudbg_fill_qdesc_num_and_size(adap, NULL, &len);
		break;
	default:
		break;
	}

	return len;
}

static int cudbg_do_compression(struct cudbg_init *pdbg_init,
				struct cudbg_buffer *pin_buff,
				struct cudbg_buffer *dbg_buff)
{
	struct cudbg_buffer temp_in_buff = { 0 };
	int bytes_left, bytes_read, bytes;
	u32 offset = dbg_buff->offset;
	int rc;

	temp_in_buff.offset = pin_buff->offset;
	temp_in_buff.data = pin_buff->data;
	temp_in_buff.size = pin_buff->size;

	bytes_left = pin_buff->size;
	bytes_read = 0;
	while (bytes_left > 0) {
		/* Do compression in smaller chunks */
		bytes = min_t(unsigned long, bytes_left,
			      (unsigned long)CUDBG_CHUNK_SIZE);
		temp_in_buff.data = (char *)pin_buff->data + bytes_read;
		temp_in_buff.size = bytes;
		rc = cudbg_compress_buff(pdbg_init, &temp_in_buff, dbg_buff);
		if (rc)
			return rc;
		bytes_left -= bytes;
		bytes_read += bytes;
	}

	pin_buff->size = dbg_buff->offset - offset;
	return 0;
}

static int cudbg_write_and_release_buff(struct cudbg_init *pdbg_init,
					struct cudbg_buffer *pin_buff,
					struct cudbg_buffer *dbg_buff)
{
	int rc = 0;

	if (pdbg_init->compress_type == CUDBG_COMPRESSION_NONE) {
		cudbg_update_buff(pin_buff, dbg_buff);
	} else {
		rc = cudbg_do_compression(pdbg_init, pin_buff, dbg_buff);
		if (rc)
			goto out;
	}

out:
	cudbg_put_buff(pdbg_init, pin_buff);
	return rc;
}

static int is_fw_attached(struct cudbg_init *pdbg_init)
{
	struct adapter *padap = pdbg_init->adap;

	if (!(padap->flags & CXGB4_FW_OK) || padap->use_bd)
		return 0;

	return 1;
}

/* This function will add additional padding bytes into debug_buffer to make it
 * 4 byte aligned.
 */
void cudbg_align_debug_buffer(struct cudbg_buffer *dbg_buff,
			      struct cudbg_entity_hdr *entity_hdr)
{
	u8 zero_buf[4] = {0};
	u8 padding, remain;

	remain = (dbg_buff->offset - entity_hdr->start_offset) % 4;
	padding = 4 - remain;
	if (remain) {
		memcpy(((u8 *)dbg_buff->data) + dbg_buff->offset, &zero_buf,
		       padding);
		dbg_buff->offset += padding;
		entity_hdr->num_pad = padding;
	}
	entity_hdr->size = dbg_buff->offset - entity_hdr->start_offset;
}

struct cudbg_entity_hdr *cudbg_get_entity_hdr(void *outbuf, int i)
{
	struct cudbg_hdr *cudbg_hdr = (struct cudbg_hdr *)outbuf;

	return (struct cudbg_entity_hdr *)
	       ((char *)outbuf + cudbg_hdr->hdr_len +
		(sizeof(struct cudbg_entity_hdr) * (i - 1)));
}

static int cudbg_read_vpd_reg(struct adapter *padap, u32 addr, u32 len,
			      void *dest)
{
	int vaddr, rc;

	vaddr = t4_eeprom_ptov(addr, padap->pf, EEPROMPFSIZE);
	if (vaddr < 0)
		return vaddr;

	rc = pci_read_vpd(padap->pdev, vaddr, len, dest);
	if (rc < 0)
		return rc;

	return 0;
}

static int cudbg_mem_desc_cmp(const void *a, const void *b)
{
	return ((const struct cudbg_mem_desc *)a)->base -
	       ((const struct cudbg_mem_desc *)b)->base;
}

int cudbg_fill_meminfo(struct adapter *padap,
		       struct cudbg_meminfo *meminfo_buff)
{
	struct cudbg_mem_desc *md;
	u32 lo, hi, used, alloc;
	int n, i;

	memset(meminfo_buff->avail, 0,
	       ARRAY_SIZE(meminfo_buff->avail) *
	       sizeof(struct cudbg_mem_desc));
	memset(meminfo_buff->mem, 0,
	       (ARRAY_SIZE(cudbg_region) + 3) * sizeof(struct cudbg_mem_desc));
	md  = meminfo_buff->mem;

	for (i = 0; i < ARRAY_SIZE(meminfo_buff->mem); i++) {
		meminfo_buff->mem[i].limit = 0;
		meminfo_buff->mem[i].idx = i;
	}

	/* Find and sort the populated memory ranges */
	i = 0;
	lo = t4_read_reg(padap, MA_TARGET_MEM_ENABLE_A);
	if (lo & EDRAM0_ENABLE_F) {
		hi = t4_read_reg(padap, MA_EDRAM0_BAR_A);
		meminfo_buff->avail[i].base =
			cudbg_mbytes_to_bytes(EDRAM0_BASE_G(hi));
		meminfo_buff->avail[i].limit =
			meminfo_buff->avail[i].base +
			cudbg_mbytes_to_bytes(EDRAM0_SIZE_G(hi));
		meminfo_buff->avail[i].idx = 0;
		i++;
	}

	if (lo & EDRAM1_ENABLE_F) {
		hi =  t4_read_reg(padap, MA_EDRAM1_BAR_A);
		meminfo_buff->avail[i].base =
			cudbg_mbytes_to_bytes(EDRAM1_BASE_G(hi));
		meminfo_buff->avail[i].limit =
			meminfo_buff->avail[i].base +
			cudbg_mbytes_to_bytes(EDRAM1_SIZE_G(hi));
		meminfo_buff->avail[i].idx = 1;
		i++;
	}

	if (is_t5(padap->params.chip)) {
		if (lo & EXT_MEM0_ENABLE_F) {
			hi = t4_read_reg(padap, MA_EXT_MEMORY0_BAR_A);
			meminfo_buff->avail[i].base =
				cudbg_mbytes_to_bytes(EXT_MEM_BASE_G(hi));
			meminfo_buff->avail[i].limit =
				meminfo_buff->avail[i].base +
				cudbg_mbytes_to_bytes(EXT_MEM_SIZE_G(hi));
			meminfo_buff->avail[i].idx = 3;
			i++;
		}

		if (lo & EXT_MEM1_ENABLE_F) {
			hi = t4_read_reg(padap, MA_EXT_MEMORY1_BAR_A);
			meminfo_buff->avail[i].base =
				cudbg_mbytes_to_bytes(EXT_MEM1_BASE_G(hi));
			meminfo_buff->avail[i].limit =
				meminfo_buff->avail[i].base +
				cudbg_mbytes_to_bytes(EXT_MEM1_SIZE_G(hi));
			meminfo_buff->avail[i].idx = 4;
			i++;
		}
	} else {
		if (lo & EXT_MEM_ENABLE_F) {
			hi = t4_read_reg(padap, MA_EXT_MEMORY_BAR_A);
			meminfo_buff->avail[i].base =
				cudbg_mbytes_to_bytes(EXT_MEM_BASE_G(hi));
			meminfo_buff->avail[i].limit =
				meminfo_buff->avail[i].base +
				cudbg_mbytes_to_bytes(EXT_MEM_SIZE_G(hi));
			meminfo_buff->avail[i].idx = 2;
			i++;
		}

		if (lo & HMA_MUX_F) {
			hi = t4_read_reg(padap, MA_EXT_MEMORY1_BAR_A);
			meminfo_buff->avail[i].base =
				cudbg_mbytes_to_bytes(EXT_MEM1_BASE_G(hi));
			meminfo_buff->avail[i].limit =
				meminfo_buff->avail[i].base +
				cudbg_mbytes_to_bytes(EXT_MEM1_SIZE_G(hi));
			meminfo_buff->avail[i].idx = 5;
			i++;
		}
	}

	if (!i) /* no memory available */
		return CUDBG_STATUS_ENTITY_NOT_FOUND;

	meminfo_buff->avail_c = i;
	sort(meminfo_buff->avail, i, sizeof(struct cudbg_mem_desc),
	     cudbg_mem_desc_cmp, NULL);
	(md++)->base = t4_read_reg(padap, SGE_DBQ_CTXT_BADDR_A);
	(md++)->base = t4_read_reg(padap, SGE_IMSG_CTXT_BADDR_A);
	(md++)->base = t4_read_reg(padap, SGE_FLM_CACHE_BADDR_A);
	(md++)->base = t4_read_reg(padap, TP_CMM_TCB_BASE_A);
	(md++)->base = t4_read_reg(padap, TP_CMM_MM_BASE_A);
	(md++)->base = t4_read_reg(padap, TP_CMM_TIMER_BASE_A);
	(md++)->base = t4_read_reg(padap, TP_CMM_MM_RX_FLST_BASE_A);
	(md++)->base = t4_read_reg(padap, TP_CMM_MM_TX_FLST_BASE_A);
	(md++)->base = t4_read_reg(padap, TP_CMM_MM_PS_FLST_BASE_A);

	/* the next few have explicit upper bounds */
	md->base = t4_read_reg(padap, TP_PMM_TX_BASE_A);
	md->limit = md->base - 1 +
		    t4_read_reg(padap, TP_PMM_TX_PAGE_SIZE_A) *
		    PMTXMAXPAGE_G(t4_read_reg(padap, TP_PMM_TX_MAX_PAGE_A));
	md++;

	md->base = t4_read_reg(padap, TP_PMM_RX_BASE_A);
	md->limit = md->base - 1 +
		    t4_read_reg(padap, TP_PMM_RX_PAGE_SIZE_A) *
		    PMRXMAXPAGE_G(t4_read_reg(padap, TP_PMM_RX_MAX_PAGE_A));
	md++;

	if (t4_read_reg(padap, LE_DB_CONFIG_A) & HASHEN_F) {
		if (CHELSIO_CHIP_VERSION(padap->params.chip) <= CHELSIO_T5) {
			hi = t4_read_reg(padap, LE_DB_TID_HASHBASE_A) / 4;
			md->base = t4_read_reg(padap, LE_DB_HASH_TID_BASE_A);
		} else {
			hi = t4_read_reg(padap, LE_DB_HASH_TID_BASE_A);
			md->base = t4_read_reg(padap,
					       LE_DB_HASH_TBL_BASE_ADDR_A);
		}
		md->limit = 0;
	} else {
		md->base = 0;
		md->idx = ARRAY_SIZE(cudbg_region);  /* hide it */
	}
	md++;

#define ulp_region(reg) do { \
	md->base = t4_read_reg(padap, ULP_ ## reg ## _LLIMIT_A);\
	(md++)->limit = t4_read_reg(padap, ULP_ ## reg ## _ULIMIT_A);\
} while (0)

	ulp_region(RX_ISCSI);
	ulp_region(RX_TDDP);
	ulp_region(TX_TPT);
	ulp_region(RX_STAG);
	ulp_region(RX_RQ);
	ulp_region(RX_RQUDP);
	ulp_region(RX_PBL);
	ulp_region(TX_PBL);
#undef ulp_region
	md->base = 0;
	md->idx = ARRAY_SIZE(cudbg_region);
	if (!is_t4(padap->params.chip)) {
		u32 fifo_size = t4_read_reg(padap, SGE_DBVFIFO_SIZE_A);
		u32 sge_ctrl = t4_read_reg(padap, SGE_CONTROL2_A);
		u32 size = 0;

		if (is_t5(padap->params.chip)) {
			if (sge_ctrl & VFIFO_ENABLE_F)
				size = DBVFIFO_SIZE_G(fifo_size);
		} else {
			size = T6_DBVFIFO_SIZE_G(fifo_size);
		}

		if (size) {
			md->base = BASEADDR_G(t4_read_reg(padap,
							  SGE_DBVFIFO_BADDR_A));
			md->limit = md->base + (size << 2) - 1;
		}
	}

	md++;

	md->base = t4_read_reg(padap, ULP_RX_CTX_BASE_A);
	md->limit = 0;
	md++;
	md->base = t4_read_reg(padap, ULP_TX_ERR_TABLE_BASE_A);
	md->limit = 0;
	md++;

	md->base = padap->vres.ocq.start;
	if (padap->vres.ocq.size)
		md->limit = md->base + padap->vres.ocq.size - 1;
	else
		md->idx = ARRAY_SIZE(cudbg_region);  /* hide it */
	md++;

	/* add any address-space holes, there can be up to 3 */
	for (n = 0; n < i - 1; n++)
		if (meminfo_buff->avail[n].limit <
		    meminfo_buff->avail[n + 1].base)
			(md++)->base = meminfo_buff->avail[n].limit;

	if (meminfo_buff->avail[n].limit)
		(md++)->base = meminfo_buff->avail[n].limit;

	n = md - meminfo_buff->mem;
	meminfo_buff->mem_c = n;

	sort(meminfo_buff->mem, n, sizeof(struct cudbg_mem_desc),
	     cudbg_mem_desc_cmp, NULL);

	lo = t4_read_reg(padap, CIM_SDRAM_BASE_ADDR_A);
	hi = t4_read_reg(padap, CIM_SDRAM_ADDR_SIZE_A) + lo - 1;
	meminfo_buff->up_ram_lo = lo;
	meminfo_buff->up_ram_hi = hi;

	lo = t4_read_reg(padap, CIM_EXTMEM2_BASE_ADDR_A);
	hi = t4_read_reg(padap, CIM_EXTMEM2_ADDR_SIZE_A) + lo - 1;
	meminfo_buff->up_extmem2_lo = lo;
	meminfo_buff->up_extmem2_hi = hi;

	lo = t4_read_reg(padap, TP_PMM_RX_MAX_PAGE_A);
	for (i = 0, meminfo_buff->free_rx_cnt = 0; i < 2; i++)
		meminfo_buff->free_rx_cnt +=
			FREERXPAGECOUNT_G(t4_read_reg(padap,
						      TP_FLM_FREE_RX_CNT_A));

	meminfo_buff->rx_pages_data[0] =  PMRXMAXPAGE_G(lo);
	meminfo_buff->rx_pages_data[1] =
		t4_read_reg(padap, TP_PMM_RX_PAGE_SIZE_A) >> 10;
	meminfo_buff->rx_pages_data[2] = (lo & PMRXNUMCHN_F) ? 2 : 1;

	lo = t4_read_reg(padap, TP_PMM_TX_MAX_PAGE_A);
	hi = t4_read_reg(padap, TP_PMM_TX_PAGE_SIZE_A);
	for (i = 0, meminfo_buff->free_tx_cnt = 0; i < 4; i++)
		meminfo_buff->free_tx_cnt +=
			FREETXPAGECOUNT_G(t4_read_reg(padap,
						      TP_FLM_FREE_TX_CNT_A));

	meminfo_buff->tx_pages_data[0] = PMTXMAXPAGE_G(lo);
	meminfo_buff->tx_pages_data[1] =
		hi >= (1 << 20) ? (hi >> 20) : (hi >> 10);
	meminfo_buff->tx_pages_data[2] =
		hi >= (1 << 20) ? 'M' : 'K';
	meminfo_buff->tx_pages_data[3] = 1 << PMTXNUMCHN_G(lo);

	meminfo_buff->p_structs = t4_read_reg(padap, TP_CMM_MM_MAX_PSTRUCT_A);
	meminfo_buff->p_structs_free_cnt =
		FREEPSTRUCTCOUNT_G(t4_read_reg(padap, TP_FLM_FREE_PS_CNT_A));

	for (i = 0; i < 4; i++) {
		if (CHELSIO_CHIP_VERSION(padap->params.chip) > CHELSIO_T5)
			lo = t4_read_reg(padap,
					 MPS_RX_MAC_BG_PG_CNT0_A + i * 4);
		else
			lo = t4_read_reg(padap, MPS_RX_PG_RSV0_A + i * 4);
		if (is_t5(padap->params.chip)) {
			used = T5_USED_G(lo);
			alloc = T5_ALLOC_G(lo);
		} else {
			used = USED_G(lo);
			alloc = ALLOC_G(lo);
		}
		meminfo_buff->port_used[i] = used;
		meminfo_buff->port_alloc[i] = alloc;
	}

	for (i = 0; i < padap->params.arch.nchan; i++) {
		if (CHELSIO_CHIP_VERSION(padap->params.chip) > CHELSIO_T5)
			lo = t4_read_reg(padap,
					 MPS_RX_LPBK_BG_PG_CNT0_A + i * 4);
		else
			lo = t4_read_reg(padap, MPS_RX_PG_RSV4_A + i * 4);
		if (is_t5(padap->params.chip)) {
			used = T5_USED_G(lo);
			alloc = T5_ALLOC_G(lo);
		} else {
			used = USED_G(lo);
			alloc = ALLOC_G(lo);
		}
		meminfo_buff->loopback_used[i] = used;
		meminfo_buff->loopback_alloc[i] = alloc;
	}

	return 0;
}

int cudbg_collect_reg_dump(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	u32 buf_size = 0;
	int rc = 0;

	if (is_t4(padap->params.chip))
		buf_size = T4_REGMAP_SIZE;
	else if (is_t5(padap->params.chip) || is_t6(padap->params.chip))
		buf_size = T5_REGMAP_SIZE;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, buf_size, &temp_buff);
	if (rc)
		return rc;
	t4_get_regs(padap, (void *)temp_buff.data, temp_buff.size);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_fw_devlog(struct cudbg_init *pdbg_init,
			    struct cudbg_buffer *dbg_buff,
			    struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct devlog_params *dparams;
	int rc = 0;

	rc = t4_init_devlog_params(padap);
	if (rc < 0) {
		cudbg_err->sys_err = rc;
		return rc;
	}

	dparams = &padap->params.devlog;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, dparams->size, &temp_buff);
	if (rc)
		return rc;

	/* Collect FW devlog */
	if (dparams->start != 0) {
		spin_lock(&padap->win0_lock);
		rc = t4_memory_rw(padap, padap->params.drv_memwin,
				  dparams->memtype, dparams->start,
				  dparams->size,
				  (__be32 *)(char *)temp_buff.data,
				  1);
		spin_unlock(&padap->win0_lock);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cim_la(struct cudbg_init *pdbg_init,
			 struct cudbg_buffer *dbg_buff,
			 struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int size, rc;
	u32 cfg = 0;

	if (is_t6(padap->params.chip)) {
		size = padap->params.cim_la_size / 10 + 1;
		size *= 10 * sizeof(u32);
	} else {
		size = padap->params.cim_la_size / 8;
		size *= 8 * sizeof(u32);
	}

	size += sizeof(cfg);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	rc = t4_cim_read(padap, UP_UP_DBG_LA_CFG_A, 1, &cfg);
	if (rc) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}

	memcpy((char *)temp_buff.data, &cfg, sizeof(cfg));
	rc = t4_cim_read_la(padap,
			    (u32 *)((char *)temp_buff.data + sizeof(cfg)),
			    NULL);
	if (rc < 0) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cim_ma_la(struct cudbg_init *pdbg_init,
			    struct cudbg_buffer *dbg_buff,
			    struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int size, rc;

	size = 2 * CIM_MALA_SIZE * 5 * sizeof(u32);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	t4_cim_read_ma_la(padap,
			  (u32 *)temp_buff.data,
			  (u32 *)((char *)temp_buff.data +
				  5 * CIM_MALA_SIZE));
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cim_qcfg(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_cim_qcfg *cim_qcfg_data;
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, sizeof(struct cudbg_cim_qcfg),
			    &temp_buff);
	if (rc)
		return rc;

	cim_qcfg_data = (struct cudbg_cim_qcfg *)temp_buff.data;
	cim_qcfg_data->chip = padap->params.chip;
	rc = t4_cim_read(padap, UP_IBQ_0_RDADDR_A,
			 ARRAY_SIZE(cim_qcfg_data->stat), cim_qcfg_data->stat);
	if (rc) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}

	rc = t4_cim_read(padap, UP_OBQ_0_REALADDR_A,
			 ARRAY_SIZE(cim_qcfg_data->obq_wr),
			 cim_qcfg_data->obq_wr);
	if (rc) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}

	t4_read_cimq_cfg(padap, cim_qcfg_data->base, cim_qcfg_data->size,
			 cim_qcfg_data->thres);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

static int cudbg_read_cim_ibq(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err, int qid)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int no_of_read_words, rc = 0;
	u32 qsize;

	/* collect CIM IBQ */
	qsize = CIM_IBQ_SIZE * 4 * sizeof(u32);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, qsize, &temp_buff);
	if (rc)
		return rc;

	/* t4_read_cim_ibq will return no. of read words or error */
	no_of_read_words = t4_read_cim_ibq(padap, qid,
					   (u32 *)temp_buff.data, qsize);
	/* no_of_read_words is less than or equal to 0 means error */
	if (no_of_read_words <= 0) {
		if (!no_of_read_words)
			rc = CUDBG_SYSTEM_ERROR;
		else
			rc = no_of_read_words;
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cim_ibq_tp0(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_ibq(pdbg_init, dbg_buff, cudbg_err, 0);
}

int cudbg_collect_cim_ibq_tp1(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_ibq(pdbg_init, dbg_buff, cudbg_err, 1);
}

int cudbg_collect_cim_ibq_ulp(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_ibq(pdbg_init, dbg_buff, cudbg_err, 2);
}

int cudbg_collect_cim_ibq_sge0(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_ibq(pdbg_init, dbg_buff, cudbg_err, 3);
}

int cudbg_collect_cim_ibq_sge1(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_ibq(pdbg_init, dbg_buff, cudbg_err, 4);
}

int cudbg_collect_cim_ibq_ncsi(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_ibq(pdbg_init, dbg_buff, cudbg_err, 5);
}

u32 cudbg_cim_obq_size(struct adapter *padap, int qid)
{
	u32 value;

	t4_write_reg(padap, CIM_QUEUE_CONFIG_REF_A, OBQSELECT_F |
		     QUENUMSELECT_V(qid));
	value = t4_read_reg(padap, CIM_QUEUE_CONFIG_CTRL_A);
	value = CIMQSIZE_G(value) * 64; /* size in number of words */
	return value * sizeof(u32);
}

static int cudbg_read_cim_obq(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err, int qid)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int no_of_read_words, rc = 0;
	u32 qsize;

	/* collect CIM OBQ */
	qsize =  cudbg_cim_obq_size(padap, qid);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, qsize, &temp_buff);
	if (rc)
		return rc;

	/* t4_read_cim_obq will return no. of read words or error */
	no_of_read_words = t4_read_cim_obq(padap, qid,
					   (u32 *)temp_buff.data, qsize);
	/* no_of_read_words is less than or equal to 0 means error */
	if (no_of_read_words <= 0) {
		if (!no_of_read_words)
			rc = CUDBG_SYSTEM_ERROR;
		else
			rc = no_of_read_words;
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cim_obq_ulp0(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 0);
}

int cudbg_collect_cim_obq_ulp1(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 1);
}

int cudbg_collect_cim_obq_ulp2(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 2);
}

int cudbg_collect_cim_obq_ulp3(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 3);
}

int cudbg_collect_cim_obq_sge(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 4);
}

int cudbg_collect_cim_obq_ncsi(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 5);
}

int cudbg_collect_obq_sge_rx_q0(struct cudbg_init *pdbg_init,
				struct cudbg_buffer *dbg_buff,
				struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 6);
}

int cudbg_collect_obq_sge_rx_q1(struct cudbg_init *pdbg_init,
				struct cudbg_buffer *dbg_buff,
				struct cudbg_error *cudbg_err)
{
	return cudbg_read_cim_obq(pdbg_init, dbg_buff, cudbg_err, 7);
}

static int cudbg_meminfo_get_mem_index(struct adapter *padap,
				       struct cudbg_meminfo *mem_info,
				       u8 mem_type, u8 *idx)
{
	u8 i, flag;

	switch (mem_type) {
	case MEM_EDC0:
		flag = EDC0_FLAG;
		break;
	case MEM_EDC1:
		flag = EDC1_FLAG;
		break;
	case MEM_MC0:
		/* Some T5 cards have both MC0 and MC1. */
		flag = is_t5(padap->params.chip) ? MC0_FLAG : MC_FLAG;
		break;
	case MEM_MC1:
		flag = MC1_FLAG;
		break;
	case MEM_HMA:
		flag = HMA_FLAG;
		break;
	default:
		return CUDBG_STATUS_ENTITY_NOT_FOUND;
	}

	for (i = 0; i < mem_info->avail_c; i++) {
		if (mem_info->avail[i].idx == flag) {
			*idx = i;
			return 0;
		}
	}

	return CUDBG_STATUS_ENTITY_NOT_FOUND;
}

/* Fetch the @region_name's start and end from @meminfo. */
static int cudbg_get_mem_region(struct adapter *padap,
				struct cudbg_meminfo *meminfo,
				u8 mem_type, const char *region_name,
				struct cudbg_mem_desc *mem_desc)
{
	u8 mc, found = 0;
	u32 idx = 0;
	int rc, i;

	rc = cudbg_meminfo_get_mem_index(padap, meminfo, mem_type, &mc);
	if (rc)
		return rc;

	i = match_string(cudbg_region, ARRAY_SIZE(cudbg_region), region_name);
	if (i < 0)
		return -EINVAL;

	idx = i;
	for (i = 0; i < meminfo->mem_c; i++) {
		if (meminfo->mem[i].idx >= ARRAY_SIZE(cudbg_region))
			continue; /* Skip holes */

		if (!(meminfo->mem[i].limit))
			meminfo->mem[i].limit =
				i < meminfo->mem_c - 1 ?
				meminfo->mem[i + 1].base - 1 : ~0;

		if (meminfo->mem[i].idx == idx) {
			/* Check if the region exists in @mem_type memory */
			if (meminfo->mem[i].base < meminfo->avail[mc].base &&
			    meminfo->mem[i].limit < meminfo->avail[mc].base)
				return -EINVAL;

			if (meminfo->mem[i].base > meminfo->avail[mc].limit)
				return -EINVAL;

			memcpy(mem_desc, &meminfo->mem[i],
			       sizeof(struct cudbg_mem_desc));
			found = 1;
			break;
		}
	}
	if (!found)
		return -EINVAL;

	return 0;
}

/* Fetch and update the start and end of the requested memory region w.r.t 0
 * in the corresponding EDC/MC/HMA.
 */
static int cudbg_get_mem_relative(struct adapter *padap,
				  struct cudbg_meminfo *meminfo,
				  u8 mem_type, u32 *out_base, u32 *out_end)
{
	u8 mc_idx;
	int rc;

	rc = cudbg_meminfo_get_mem_index(padap, meminfo, mem_type, &mc_idx);
	if (rc)
		return rc;

	if (*out_base < meminfo->avail[mc_idx].base)
		*out_base = 0;
	else
		*out_base -= meminfo->avail[mc_idx].base;

	if (*out_end > meminfo->avail[mc_idx].limit)
		*out_end = meminfo->avail[mc_idx].limit;
	else
		*out_end -= meminfo->avail[mc_idx].base;

	return 0;
}

/* Get TX and RX Payload region */
static int cudbg_get_payload_range(struct adapter *padap, u8 mem_type,
				   const char *region_name,
				   struct cudbg_region_info *payload)
{
	struct cudbg_mem_desc mem_desc = { 0 };
	struct cudbg_meminfo meminfo;
	int rc;

	rc = cudbg_fill_meminfo(padap, &meminfo);
	if (rc)
		return rc;

	rc = cudbg_get_mem_region(padap, &meminfo, mem_type, region_name,
				  &mem_desc);
	if (rc) {
		payload->exist = false;
		return 0;
	}

	payload->exist = true;
	payload->start = mem_desc.base;
	payload->end = mem_desc.limit;

	return cudbg_get_mem_relative(padap, &meminfo, mem_type,
				      &payload->start, &payload->end);
}

static int cudbg_memory_read(struct cudbg_init *pdbg_init, int win,
			     int mtype, u32 addr, u32 len, void *hbuf)
{
	u32 win_pf, memoffset, mem_aperture, mem_base;
	struct adapter *adap = pdbg_init->adap;
	u32 pos, offset, resid;
	u32 *res_buf;
	u64 *buf;
	int ret;

	/* Argument sanity checks ...
	 */
	if (addr & 0x3 || (uintptr_t)hbuf & 0x3)
		return -EINVAL;

	buf = (u64 *)hbuf;

	/* Try to do 64-bit reads.  Residual will be handled later. */
	resid = len & 0x7;
	len -= resid;

	ret = t4_memory_rw_init(adap, win, mtype, &memoffset, &mem_base,
				&mem_aperture);
	if (ret)
		return ret;

	addr = addr + memoffset;
	win_pf = is_t4(adap->params.chip) ? 0 : PFNUM_V(adap->pf);

	pos = addr & ~(mem_aperture - 1);
	offset = addr - pos;

	/* Set up initial PCI-E Memory Window to cover the start of our
	 * transfer.
	 */
	t4_memory_update_win(adap, win, pos | win_pf);

	/* Transfer data from the adapter */
	while (len > 0) {
		*buf++ = le64_to_cpu((__force __le64)
				     t4_read_reg64(adap, mem_base + offset));
		offset += sizeof(u64);
		len -= sizeof(u64);

		/* If we've reached the end of our current window aperture,
		 * move the PCI-E Memory Window on to the next.
		 */
		if (offset == mem_aperture) {
			pos += mem_aperture;
			offset = 0;
			t4_memory_update_win(adap, win, pos | win_pf);
		}
	}

	res_buf = (u32 *)buf;
	/* Read residual in 32-bit multiples */
	while (resid > sizeof(u32)) {
		*res_buf++ = le32_to_cpu((__force __le32)
					 t4_read_reg(adap, mem_base + offset));
		offset += sizeof(u32);
		resid -= sizeof(u32);

		/* If we've reached the end of our current window aperture,
		 * move the PCI-E Memory Window on to the next.
		 */
		if (offset == mem_aperture) {
			pos += mem_aperture;
			offset = 0;
			t4_memory_update_win(adap, win, pos | win_pf);
		}
	}

	/* Transfer residual < 32-bits */
	if (resid)
		t4_memory_rw_residual(adap, resid, mem_base + offset,
				      (u8 *)res_buf, T4_MEMORY_READ);

	return 0;
}

#define CUDBG_YIELD_ITERATION 256

static int cudbg_read_fw_mem(struct cudbg_init *pdbg_init,
			     struct cudbg_buffer *dbg_buff, u8 mem_type,
			     unsigned long tot_len,
			     struct cudbg_error *cudbg_err)
{
	static const char * const region_name[] = { "Tx payload:",
						    "Rx payload:" };
	unsigned long bytes, bytes_left, bytes_read = 0;
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_region_info payload[2];
	u32 yield_count = 0;
	int rc = 0;
	u8 i;

	/* Get TX/RX Payload region range if they exist */
	memset(payload, 0, sizeof(payload));
	for (i = 0; i < ARRAY_SIZE(region_name); i++) {
		rc = cudbg_get_payload_range(padap, mem_type, region_name[i],
					     &payload[i]);
		if (rc)
			return rc;

		if (payload[i].exist) {
			/* Align start and end to avoid wrap around */
			payload[i].start = roundup(payload[i].start,
						   CUDBG_CHUNK_SIZE);
			payload[i].end = rounddown(payload[i].end,
						   CUDBG_CHUNK_SIZE);
		}
	}

	bytes_left = tot_len;
	while (bytes_left > 0) {
		/* As MC size is huge and read through PIO access, this
		 * loop will hold cpu for a longer time. OS may think that
		 * the process is hanged and will generate CPU stall traces.
		 * So yield the cpu regularly.
		 */
		yield_count++;
		if (!(yield_count % CUDBG_YIELD_ITERATION))
			schedule();

		bytes = min_t(unsigned long, bytes_left,
			      (unsigned long)CUDBG_CHUNK_SIZE);
		rc = cudbg_get_buff(pdbg_init, dbg_buff, bytes, &temp_buff);
		if (rc)
			return rc;

		for (i = 0; i < ARRAY_SIZE(payload); i++)
			if (payload[i].exist &&
			    bytes_read >= payload[i].start &&
			    bytes_read + bytes <= payload[i].end)
				/* TX and RX Payload regions can't overlap */
				goto skip_read;

		spin_lock(&padap->win0_lock);
		rc = cudbg_memory_read(pdbg_init, MEMWIN_NIC, mem_type,
				       bytes_read, bytes, temp_buff.data);
		spin_unlock(&padap->win0_lock);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}

skip_read:
		bytes_left -= bytes;
		bytes_read += bytes;
		rc = cudbg_write_and_release_buff(pdbg_init, &temp_buff,
						  dbg_buff);
		if (rc) {
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
	}
	return rc;
}

static void cudbg_t4_fwcache(struct cudbg_init *pdbg_init,
			     struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	int rc;

	if (is_fw_attached(pdbg_init)) {
		/* Flush uP dcache before reading edcX/mcX  */
		rc = t4_fwcache(padap, FW_PARAM_DEV_FWCACHE_FLUSH);
		if (rc)
			cudbg_err->sys_warn = rc;
	}
}

static int cudbg_mem_region_size(struct cudbg_init *pdbg_init,
				 struct cudbg_error *cudbg_err,
				 u8 mem_type, unsigned long *region_size)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_meminfo mem_info;
	u8 mc_idx;
	int rc;

	memset(&mem_info, 0, sizeof(struct cudbg_meminfo));
	rc = cudbg_fill_meminfo(padap, &mem_info);
	if (rc) {
		cudbg_err->sys_err = rc;
		return rc;
	}

	cudbg_t4_fwcache(pdbg_init, cudbg_err);
	rc = cudbg_meminfo_get_mem_index(padap, &mem_info, mem_type, &mc_idx);
	if (rc) {
		cudbg_err->sys_err = rc;
		return rc;
	}

	if (region_size)
		*region_size = mem_info.avail[mc_idx].limit -
			       mem_info.avail[mc_idx].base;

	return 0;
}

static int cudbg_collect_mem_region(struct cudbg_init *pdbg_init,
				    struct cudbg_buffer *dbg_buff,
				    struct cudbg_error *cudbg_err,
				    u8 mem_type)
{
	unsigned long size = 0;
	int rc;

	rc = cudbg_mem_region_size(pdbg_init, cudbg_err, mem_type, &size);
	if (rc)
		return rc;

	return cudbg_read_fw_mem(pdbg_init, dbg_buff, mem_type, size,
				 cudbg_err);
}

int cudbg_collect_edc0_meminfo(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_collect_mem_region(pdbg_init, dbg_buff, cudbg_err,
					MEM_EDC0);
}

int cudbg_collect_edc1_meminfo(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	return cudbg_collect_mem_region(pdbg_init, dbg_buff, cudbg_err,
					MEM_EDC1);
}

int cudbg_collect_mc0_meminfo(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_collect_mem_region(pdbg_init, dbg_buff, cudbg_err,
					MEM_MC0);
}

int cudbg_collect_mc1_meminfo(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_collect_mem_region(pdbg_init, dbg_buff, cudbg_err,
					MEM_MC1);
}

int cudbg_collect_hma_meminfo(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	return cudbg_collect_mem_region(pdbg_init, dbg_buff, cudbg_err,
					MEM_HMA);
}

int cudbg_collect_rss(struct cudbg_init *pdbg_init,
		      struct cudbg_buffer *dbg_buff,
		      struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int rc, nentries;

	nentries = t4_chip_rss_size(padap);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, nentries * sizeof(u16),
			    &temp_buff);
	if (rc)
		return rc;

	rc = t4_read_rss(padap, (u16 *)temp_buff.data);
	if (rc) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_rss_vf_config(struct cudbg_init *pdbg_init,
				struct cudbg_buffer *dbg_buff,
				struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_rss_vf_conf *vfconf;
	int vf, rc, vf_count;

	vf_count = padap->params.arch.vfcount;
	rc = cudbg_get_buff(pdbg_init, dbg_buff,
			    vf_count * sizeof(struct cudbg_rss_vf_conf),
			    &temp_buff);
	if (rc)
		return rc;

	vfconf = (struct cudbg_rss_vf_conf *)temp_buff.data;
	for (vf = 0; vf < vf_count; vf++)
		t4_read_rss_vf_config(padap, vf, &vfconf[vf].rss_vf_vfl,
				      &vfconf[vf].rss_vf_vfh, true);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_path_mtu(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, NMTUS * sizeof(u16),
			    &temp_buff);
	if (rc)
		return rc;

	t4_read_mtu_tbl(padap, (u16 *)temp_buff.data, NULL);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_pm_stats(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_pm_stats *pm_stats_buff;
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, sizeof(struct cudbg_pm_stats),
			    &temp_buff);
	if (rc)
		return rc;

	pm_stats_buff = (struct cudbg_pm_stats *)temp_buff.data;
	t4_pmtx_get_stats(padap, pm_stats_buff->tx_cnt, pm_stats_buff->tx_cyc);
	t4_pmrx_get_stats(padap, pm_stats_buff->rx_cnt, pm_stats_buff->rx_cyc);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_hw_sched(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_hw_sched *hw_sched_buff;
	int i, rc = 0;

	if (!padap->params.vpd.cclk)
		return CUDBG_STATUS_CCLK_NOT_DEFINED;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, sizeof(struct cudbg_hw_sched),
			    &temp_buff);

	if (rc)
		return rc;

	hw_sched_buff = (struct cudbg_hw_sched *)temp_buff.data;
	hw_sched_buff->map = t4_read_reg(padap, TP_TX_MOD_QUEUE_REQ_MAP_A);
	hw_sched_buff->mode = TIMERMODE_G(t4_read_reg(padap, TP_MOD_CONFIG_A));
	t4_read_pace_tbl(padap, hw_sched_buff->pace_tab);
	for (i = 0; i < NTX_SCHED; ++i)
		t4_get_tx_sched(padap, i, &hw_sched_buff->kbps[i],
				&hw_sched_buff->ipg[i], true);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_tp_indirect(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct ireg_buf *ch_tp_pio;
	int i, rc, n = 0;
	u32 size;

	if (is_t5(padap->params.chip))
		n = sizeof(t5_tp_pio_array) +
		    sizeof(t5_tp_tm_pio_array) +
		    sizeof(t5_tp_mib_index_array);
	else
		n = sizeof(t6_tp_pio_array) +
		    sizeof(t6_tp_tm_pio_array) +
		    sizeof(t6_tp_mib_index_array);

	n = n / (IREG_NUM_ELEM * sizeof(u32));
	size = sizeof(struct ireg_buf) * n;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	ch_tp_pio = (struct ireg_buf *)temp_buff.data;

	/* TP_PIO */
	if (is_t5(padap->params.chip))
		n = sizeof(t5_tp_pio_array) / (IREG_NUM_ELEM * sizeof(u32));
	else if (is_t6(padap->params.chip))
		n = sizeof(t6_tp_pio_array) / (IREG_NUM_ELEM * sizeof(u32));

	for (i = 0; i < n; i++) {
		struct ireg_field *tp_pio = &ch_tp_pio->tp_pio;
		u32 *buff = ch_tp_pio->outbuf;

		if (is_t5(padap->params.chip)) {
			tp_pio->ireg_addr = t5_tp_pio_array[i][0];
			tp_pio->ireg_data = t5_tp_pio_array[i][1];
			tp_pio->ireg_local_offset = t5_tp_pio_array[i][2];
			tp_pio->ireg_offset_range = t5_tp_pio_array[i][3];
		} else if (is_t6(padap->params.chip)) {
			tp_pio->ireg_addr = t6_tp_pio_array[i][0];
			tp_pio->ireg_data = t6_tp_pio_array[i][1];
			tp_pio->ireg_local_offset = t6_tp_pio_array[i][2];
			tp_pio->ireg_offset_range = t6_tp_pio_array[i][3];
		}
		t4_tp_pio_read(padap, buff, tp_pio->ireg_offset_range,
			       tp_pio->ireg_local_offset, true);
		ch_tp_pio++;
	}

	/* TP_TM_PIO */
	if (is_t5(padap->params.chip))
		n = sizeof(t5_tp_tm_pio_array) / (IREG_NUM_ELEM * sizeof(u32));
	else if (is_t6(padap->params.chip))
		n = sizeof(t6_tp_tm_pio_array) / (IREG_NUM_ELEM * sizeof(u32));

	for (i = 0; i < n; i++) {
		struct ireg_field *tp_pio = &ch_tp_pio->tp_pio;
		u32 *buff = ch_tp_pio->outbuf;

		if (is_t5(padap->params.chip)) {
			tp_pio->ireg_addr = t5_tp_tm_pio_array[i][0];
			tp_pio->ireg_data = t5_tp_tm_pio_array[i][1];
			tp_pio->ireg_local_offset = t5_tp_tm_pio_array[i][2];
			tp_pio->ireg_offset_range = t5_tp_tm_pio_array[i][3];
		} else if (is_t6(padap->params.chip)) {
			tp_pio->ireg_addr = t6_tp_tm_pio_array[i][0];
			tp_pio->ireg_data = t6_tp_tm_pio_array[i][1];
			tp_pio->ireg_local_offset = t6_tp_tm_pio_array[i][2];
			tp_pio->ireg_offset_range = t6_tp_tm_pio_array[i][3];
		}
		t4_tp_tm_pio_read(padap, buff, tp_pio->ireg_offset_range,
				  tp_pio->ireg_local_offset, true);
		ch_tp_pio++;
	}

	/* TP_MIB_INDEX */
	if (is_t5(padap->params.chip))
		n = sizeof(t5_tp_mib_index_array) /
		    (IREG_NUM_ELEM * sizeof(u32));
	else if (is_t6(padap->params.chip))
		n = sizeof(t6_tp_mib_index_array) /
		    (IREG_NUM_ELEM * sizeof(u32));

	for (i = 0; i < n ; i++) {
		struct ireg_field *tp_pio = &ch_tp_pio->tp_pio;
		u32 *buff = ch_tp_pio->outbuf;

		if (is_t5(padap->params.chip)) {
			tp_pio->ireg_addr = t5_tp_mib_index_array[i][0];
			tp_pio->ireg_data = t5_tp_mib_index_array[i][1];
			tp_pio->ireg_local_offset =
				t5_tp_mib_index_array[i][2];
			tp_pio->ireg_offset_range =
				t5_tp_mib_index_array[i][3];
		} else if (is_t6(padap->params.chip)) {
			tp_pio->ireg_addr = t6_tp_mib_index_array[i][0];
			tp_pio->ireg_data = t6_tp_mib_index_array[i][1];
			tp_pio->ireg_local_offset =
				t6_tp_mib_index_array[i][2];
			tp_pio->ireg_offset_range =
				t6_tp_mib_index_array[i][3];
		}
		t4_tp_mib_read(padap, buff, tp_pio->ireg_offset_range,
			       tp_pio->ireg_local_offset, true);
		ch_tp_pio++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

static void cudbg_read_sge_qbase_indirect_reg(struct adapter *padap,
					      struct sge_qbase_reg_field *qbase,
					      u32 func, bool is_pf)
{
	u32 *buff, i;

	if (is_pf) {
		buff = qbase->pf_data_value[func];
	} else {
		buff = qbase->vf_data_value[func];
		/* In SGE_QBASE_INDEX,
		 * Entries 0->7 are PF0->7, Entries 8->263 are VFID0->256.
		 */
		func += 8;
	}

	t4_write_reg(padap, qbase->reg_addr, func);
	for (i = 0; i < SGE_QBASE_DATA_REG_NUM; i++, buff++)
		*buff = t4_read_reg(padap, qbase->reg_data[i]);
}

int cudbg_collect_sge_indirect(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct sge_qbase_reg_field *sge_qbase;
	struct ireg_buf *ch_sge_dbg;
	u8 padap_running = 0;
	int i, rc;
	u32 size;

	/* Accessing SGE_QBASE_MAP[0-3] and SGE_QBASE_INDEX regs can
	 * lead to SGE missing doorbells under heavy traffic. So, only
	 * collect them when adapter is idle.
	 */
	for_each_port(padap, i) {
		padap_running = netif_running(padap->port[i]);
		if (padap_running)
			break;
	}

	size = sizeof(*ch_sge_dbg) * 2;
	if (!padap_running)
		size += sizeof(*sge_qbase);

	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	ch_sge_dbg = (struct ireg_buf *)temp_buff.data;
	for (i = 0; i < 2; i++) {
		struct ireg_field *sge_pio = &ch_sge_dbg->tp_pio;
		u32 *buff = ch_sge_dbg->outbuf;

		sge_pio->ireg_addr = t5_sge_dbg_index_array[i][0];
		sge_pio->ireg_data = t5_sge_dbg_index_array[i][1];
		sge_pio->ireg_local_offset = t5_sge_dbg_index_array[i][2];
		sge_pio->ireg_offset_range = t5_sge_dbg_index_array[i][3];
		t4_read_indirect(padap,
				 sge_pio->ireg_addr,
				 sge_pio->ireg_data,
				 buff,
				 sge_pio->ireg_offset_range,
				 sge_pio->ireg_local_offset);
		ch_sge_dbg++;
	}

	if (CHELSIO_CHIP_VERSION(padap->params.chip) > CHELSIO_T5 &&
	    !padap_running) {
		sge_qbase = (struct sge_qbase_reg_field *)ch_sge_dbg;
		/* 1 addr reg SGE_QBASE_INDEX and 4 data reg
		 * SGE_QBASE_MAP[0-3]
		 */
		sge_qbase->reg_addr = t6_sge_qbase_index_array[0];
		for (i = 0; i < SGE_QBASE_DATA_REG_NUM; i++)
			sge_qbase->reg_data[i] =
				t6_sge_qbase_index_array[i + 1];

		for (i = 0; i <= PCIE_FW_MASTER_M; i++)
			cudbg_read_sge_qbase_indirect_reg(padap, sge_qbase,
							  i, true);

		for (i = 0; i < padap->params.arch.vfcount; i++)
			cudbg_read_sge_qbase_indirect_reg(padap, sge_qbase,
							  i, false);

		sge_qbase->vfcount = padap->params.arch.vfcount;
	}

	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_ulprx_la(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_ulprx_la *ulprx_la_buff;
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, sizeof(struct cudbg_ulprx_la),
			    &temp_buff);
	if (rc)
		return rc;

	ulprx_la_buff = (struct cudbg_ulprx_la *)temp_buff.data;
	t4_ulprx_read_la(padap, (u32 *)ulprx_la_buff->data);
	ulprx_la_buff->size = ULPRX_LA_SIZE;
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_tp_la(struct cudbg_init *pdbg_init,
			struct cudbg_buffer *dbg_buff,
			struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_tp_la *tp_la_buff;
	int size, rc;

	size = sizeof(struct cudbg_tp_la) + TPLA_SIZE *  sizeof(u64);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	tp_la_buff = (struct cudbg_tp_la *)temp_buff.data;
	tp_la_buff->mode = DBGLAMODE_G(t4_read_reg(padap, TP_DBG_LA_CONFIG_A));
	t4_tp_read_la(padap, (u64 *)tp_la_buff->data, NULL);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_meminfo(struct cudbg_init *pdbg_init,
			  struct cudbg_buffer *dbg_buff,
			  struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_meminfo *meminfo_buff;
	struct cudbg_ver_hdr *ver_hdr;
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff,
			    sizeof(struct cudbg_ver_hdr) +
			    sizeof(struct cudbg_meminfo),
			    &temp_buff);
	if (rc)
		return rc;

	ver_hdr = (struct cudbg_ver_hdr *)temp_buff.data;
	ver_hdr->signature = CUDBG_ENTITY_SIGNATURE;
	ver_hdr->revision = CUDBG_MEMINFO_REV;
	ver_hdr->size = sizeof(struct cudbg_meminfo);

	meminfo_buff = (struct cudbg_meminfo *)(temp_buff.data +
						sizeof(*ver_hdr));
	rc = cudbg_fill_meminfo(padap, meminfo_buff);
	if (rc) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}

	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cim_pif_la(struct cudbg_init *pdbg_init,
			     struct cudbg_buffer *dbg_buff,
			     struct cudbg_error *cudbg_err)
{
	struct cudbg_cim_pif_la *cim_pif_la_buff;
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	int size, rc;

	size = sizeof(struct cudbg_cim_pif_la) +
	       2 * CIM_PIFLA_SIZE * 6 * sizeof(u32);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	cim_pif_la_buff = (struct cudbg_cim_pif_la *)temp_buff.data;
	cim_pif_la_buff->size = CIM_PIFLA_SIZE;
	t4_cim_read_pif_la(padap, (u32 *)cim_pif_la_buff->data,
			   (u32 *)cim_pif_la_buff->data + 6 * CIM_PIFLA_SIZE,
			   NULL, NULL);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_clk_info(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_clk_info *clk_info_buff;
	u64 tp_tick_us;
	int rc;

	if (!padap->params.vpd.cclk)
		return CUDBG_STATUS_CCLK_NOT_DEFINED;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, sizeof(struct cudbg_clk_info),
			    &temp_buff);
	if (rc)
		return rc;

	clk_info_buff = (struct cudbg_clk_info *)temp_buff.data;
	clk_info_buff->cclk_ps = 1000000000 / padap->params.vpd.cclk; /* psec */
	clk_info_buff->res = t4_read_reg(padap, TP_TIMER_RESOLUTION_A);
	clk_info_buff->tre = TIMERRESOLUTION_G(clk_info_buff->res);
	clk_info_buff->dack_re = DELAYEDACKRESOLUTION_G(clk_info_buff->res);
	tp_tick_us = (clk_info_buff->cclk_ps << clk_info_buff->tre) / 1000000;

	clk_info_buff->dack_timer =
		(clk_info_buff->cclk_ps << clk_info_buff->dack_re) / 1000000 *
		t4_read_reg(padap, TP_DACK_TIMER_A);
	clk_info_buff->retransmit_min =
		tp_tick_us * t4_read_reg(padap, TP_RXT_MIN_A);
	clk_info_buff->retransmit_max =
		tp_tick_us * t4_read_reg(padap, TP_RXT_MAX_A);
	clk_info_buff->persist_timer_min =
		tp_tick_us * t4_read_reg(padap, TP_PERS_MIN_A);
	clk_info_buff->persist_timer_max =
		tp_tick_us * t4_read_reg(padap, TP_PERS_MAX_A);
	clk_info_buff->keepalive_idle_timer =
		tp_tick_us * t4_read_reg(padap, TP_KEEP_IDLE_A);
	clk_info_buff->keepalive_interval =
		tp_tick_us * t4_read_reg(padap, TP_KEEP_INTVL_A);
	clk_info_buff->initial_srtt =
		tp_tick_us * INITSRTT_G(t4_read_reg(padap, TP_INIT_SRTT_A));
	clk_info_buff->finwait2_timer =
		tp_tick_us * t4_read_reg(padap, TP_FINWAIT2_TIMER_A);

	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_pcie_indirect(struct cudbg_init *pdbg_init,
				struct cudbg_buffer *dbg_buff,
				struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct ireg_buf *ch_pcie;
	int i, rc, n;
	u32 size;

	n = sizeof(t5_pcie_pdbg_array) / (IREG_NUM_ELEM * sizeof(u32));
	size = sizeof(struct ireg_buf) * n * 2;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	ch_pcie = (struct ireg_buf *)temp_buff.data;
	/* PCIE_PDBG */
	for (i = 0; i < n; i++) {
		struct ireg_field *pcie_pio = &ch_pcie->tp_pio;
		u32 *buff = ch_pcie->outbuf;

		pcie_pio->ireg_addr = t5_pcie_pdbg_array[i][0];
		pcie_pio->ireg_data = t5_pcie_pdbg_array[i][1];
		pcie_pio->ireg_local_offset = t5_pcie_pdbg_array[i][2];
		pcie_pio->ireg_offset_range = t5_pcie_pdbg_array[i][3];
		t4_read_indirect(padap,
				 pcie_pio->ireg_addr,
				 pcie_pio->ireg_data,
				 buff,
				 pcie_pio->ireg_offset_range,
				 pcie_pio->ireg_local_offset);
		ch_pcie++;
	}

	/* PCIE_CDBG */
	n = sizeof(t5_pcie_cdbg_array) / (IREG_NUM_ELEM * sizeof(u32));
	for (i = 0; i < n; i++) {
		struct ireg_field *pcie_pio = &ch_pcie->tp_pio;
		u32 *buff = ch_pcie->outbuf;

		pcie_pio->ireg_addr = t5_pcie_cdbg_array[i][0];
		pcie_pio->ireg_data = t5_pcie_cdbg_array[i][1];
		pcie_pio->ireg_local_offset = t5_pcie_cdbg_array[i][2];
		pcie_pio->ireg_offset_range = t5_pcie_cdbg_array[i][3];
		t4_read_indirect(padap,
				 pcie_pio->ireg_addr,
				 pcie_pio->ireg_data,
				 buff,
				 pcie_pio->ireg_offset_range,
				 pcie_pio->ireg_local_offset);
		ch_pcie++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_pm_indirect(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct ireg_buf *ch_pm;
	int i, rc, n;
	u32 size;

	n = sizeof(t5_pm_rx_array) / (IREG_NUM_ELEM * sizeof(u32));
	size = sizeof(struct ireg_buf) * n * 2;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	ch_pm = (struct ireg_buf *)temp_buff.data;
	/* PM_RX */
	for (i = 0; i < n; i++) {
		struct ireg_field *pm_pio = &ch_pm->tp_pio;
		u32 *buff = ch_pm->outbuf;

		pm_pio->ireg_addr = t5_pm_rx_array[i][0];
		pm_pio->ireg_data = t5_pm_rx_array[i][1];
		pm_pio->ireg_local_offset = t5_pm_rx_array[i][2];
		pm_pio->ireg_offset_range = t5_pm_rx_array[i][3];
		t4_read_indirect(padap,
				 pm_pio->ireg_addr,
				 pm_pio->ireg_data,
				 buff,
				 pm_pio->ireg_offset_range,
				 pm_pio->ireg_local_offset);
		ch_pm++;
	}

	/* PM_TX */
	n = sizeof(t5_pm_tx_array) / (IREG_NUM_ELEM * sizeof(u32));
	for (i = 0; i < n; i++) {
		struct ireg_field *pm_pio = &ch_pm->tp_pio;
		u32 *buff = ch_pm->outbuf;

		pm_pio->ireg_addr = t5_pm_tx_array[i][0];
		pm_pio->ireg_data = t5_pm_tx_array[i][1];
		pm_pio->ireg_local_offset = t5_pm_tx_array[i][2];
		pm_pio->ireg_offset_range = t5_pm_tx_array[i][3];
		t4_read_indirect(padap,
				 pm_pio->ireg_addr,
				 pm_pio->ireg_data,
				 buff,
				 pm_pio->ireg_offset_range,
				 pm_pio->ireg_local_offset);
		ch_pm++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_tid(struct cudbg_init *pdbg_init,
		      struct cudbg_buffer *dbg_buff,
		      struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_tid_info_region_rev1 *tid1;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_tid_info_region *tid;
	u32 para[2], val[2];
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff,
			    sizeof(struct cudbg_tid_info_region_rev1),
			    &temp_buff);
	if (rc)
		return rc;

	tid1 = (struct cudbg_tid_info_region_rev1 *)temp_buff.data;
	tid = &tid1->tid;
	tid1->ver_hdr.signature = CUDBG_ENTITY_SIGNATURE;
	tid1->ver_hdr.revision = CUDBG_TID_INFO_REV;
	tid1->ver_hdr.size = sizeof(struct cudbg_tid_info_region_rev1) -
			     sizeof(struct cudbg_ver_hdr);

	/* If firmware is not attached/alive, use backdoor register
	 * access to collect dump.
	 */
	if (!is_fw_attached(pdbg_init))
		goto fill_tid;

#define FW_PARAM_PFVF_A(param) \
	(FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_PFVF) | \
	 FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_PFVF_##param) | \
	 FW_PARAMS_PARAM_Y_V(0) | \
	 FW_PARAMS_PARAM_Z_V(0))

	para[0] = FW_PARAM_PFVF_A(ETHOFLD_START);
	para[1] = FW_PARAM_PFVF_A(ETHOFLD_END);
	rc = t4_query_params(padap, padap->mbox, padap->pf, 0, 2, para, val);
	if (rc <  0) {
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}
	tid->uotid_base = val[0];
	tid->nuotids = val[1] - val[0] + 1;

	if (is_t5(padap->params.chip)) {
		tid->sb = t4_read_reg(padap, LE_DB_SERVER_INDEX_A) / 4;
	} else if (is_t6(padap->params.chip)) {
		tid1->tid_start =
			t4_read_reg(padap, LE_DB_ACTIVE_TABLE_START_INDEX_A);
		tid->sb = t4_read_reg(padap, LE_DB_SRVR_START_INDEX_A);

		para[0] = FW_PARAM_PFVF_A(HPFILTER_START);
		para[1] = FW_PARAM_PFVF_A(HPFILTER_END);
		rc = t4_query_params(padap, padap->mbox, padap->pf, 0, 2,
				     para, val);
		if (rc < 0) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
		tid->hpftid_base = val[0];
		tid->nhpftids = val[1] - val[0] + 1;
	}

#undef FW_PARAM_PFVF_A

fill_tid:
	tid->ntids = padap->tids.ntids;
	tid->nstids = padap->tids.nstids;
	tid->stid_base = padap->tids.stid_base;
	tid->hash_base = padap->tids.hash_base;

	tid->natids = padap->tids.natids;
	tid->nftids = padap->tids.nftids;
	tid->ftid_base = padap->tids.ftid_base;
	tid->aftid_base = padap->tids.aftid_base;
	tid->aftid_end = padap->tids.aftid_end;

	tid->sftid_base = padap->tids.sftid_base;
	tid->nsftids = padap->tids.nsftids;

	tid->flags = padap->flags;
	tid->le_db_conf = t4_read_reg(padap, LE_DB_CONFIG_A);
	tid->ip_users = t4_read_reg(padap, LE_DB_ACT_CNT_IPV4_A);
	tid->ipv6_users = t4_read_reg(padap, LE_DB_ACT_CNT_IPV6_A);

	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_pcie_config(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	u32 size, *value, j;
	int i, rc, n;

	size = sizeof(u32) * CUDBG_NUM_PCIE_CONFIG_REGS;
	n = sizeof(t5_pcie_config_array) / (2 * sizeof(u32));
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	value = (u32 *)temp_buff.data;
	for (i = 0; i < n; i++) {
		for (j = t5_pcie_config_array[i][0];
		     j <= t5_pcie_config_array[i][1]; j += 4) {
			t4_hw_pci_read_cfg4(padap, j, value);
			value++;
		}
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

static int cudbg_sge_ctxt_check_valid(u32 *buf, int type)
{
	int index, bit, bit_pos = 0;

	switch (type) {
	case CTXT_EGRESS:
		bit_pos = 176;
		break;
	case CTXT_INGRESS:
		bit_pos = 141;
		break;
	case CTXT_FLM:
		bit_pos = 89;
		break;
	}
	index = bit_pos / 32;
	bit =  bit_pos % 32;
	return buf[index] & (1U << bit);
}

static int cudbg_get_ctxt_region_info(struct adapter *padap,
				      struct cudbg_region_info *ctx_info,
				      u8 *mem_type)
{
	struct cudbg_mem_desc mem_desc;
	struct cudbg_meminfo meminfo;
	u32 i, j, value, found;
	u8 flq;
	int rc;

	rc = cudbg_fill_meminfo(padap, &meminfo);
	if (rc)
		return rc;

	/* Get EGRESS and INGRESS context region size */
	for (i = CTXT_EGRESS; i <= CTXT_INGRESS; i++) {
		found = 0;
		memset(&mem_desc, 0, sizeof(struct cudbg_mem_desc));
		for (j = 0; j < ARRAY_SIZE(meminfo.avail); j++) {
			rc = cudbg_get_mem_region(padap, &meminfo, j,
						  cudbg_region[i],
						  &mem_desc);
			if (!rc) {
				found = 1;
				rc = cudbg_get_mem_relative(padap, &meminfo, j,
							    &mem_desc.base,
							    &mem_desc.limit);
				if (rc) {
					ctx_info[i].exist = false;
					break;
				}
				ctx_info[i].exist = true;
				ctx_info[i].start = mem_desc.base;
				ctx_info[i].end = mem_desc.limit;
				mem_type[i] = j;
				break;
			}
		}
		if (!found)
			ctx_info[i].exist = false;
	}

	/* Get FLM and CNM max qid. */
	value = t4_read_reg(padap, SGE_FLM_CFG_A);

	/* Get number of data freelist queues */
	flq = HDRSTARTFLQ_G(value);
	ctx_info[CTXT_FLM].exist = true;
	ctx_info[CTXT_FLM].end = (CUDBG_MAX_FL_QIDS >> flq) * SGE_CTXT_SIZE;

	/* The number of CONM contexts are same as number of freelist
	 * queues.
	 */
	ctx_info[CTXT_CNM].exist = true;
	ctx_info[CTXT_CNM].end = ctx_info[CTXT_FLM].end;

	return 0;
}

int cudbg_dump_context_size(struct adapter *padap)
{
	struct cudbg_region_info region_info[CTXT_CNM + 1] = { {0} };
	u8 mem_type[CTXT_INGRESS + 1] = { 0 };
	u32 i, size = 0;
	int rc;

	/* Get max valid qid for each type of queue */
	rc = cudbg_get_ctxt_region_info(padap, region_info, mem_type);
	if (rc)
		return rc;

	for (i = 0; i < CTXT_CNM; i++) {
		if (!region_info[i].exist) {
			if (i == CTXT_EGRESS || i == CTXT_INGRESS)
				size += CUDBG_LOWMEM_MAX_CTXT_QIDS *
					SGE_CTXT_SIZE;
			continue;
		}

		size += (region_info[i].end - region_info[i].start + 1) /
			SGE_CTXT_SIZE;
	}
	return size * sizeof(struct cudbg_ch_cntxt);
}

static void cudbg_read_sge_ctxt(struct cudbg_init *pdbg_init, u32 cid,
				enum ctxt_type ctype, u32 *data)
{
	struct adapter *padap = pdbg_init->adap;
	int rc = -1;

	/* Under heavy traffic, the SGE Queue contexts registers will be
	 * frequently accessed by firmware.
	 *
	 * To avoid conflicts with firmware, always ask firmware to fetch
	 * the SGE Queue contexts via mailbox. On failure, fallback to
	 * accessing hardware registers directly.
	 */
	if (is_fw_attached(pdbg_init))
		rc = t4_sge_ctxt_rd(padap, padap->mbox, cid, ctype, data);
	if (rc)
		t4_sge_ctxt_rd_bd(padap, cid, ctype, data);
}

static void cudbg_get_sge_ctxt_fw(struct cudbg_init *pdbg_init, u32 max_qid,
				  u8 ctxt_type,
				  struct cudbg_ch_cntxt **out_buff)
{
	struct cudbg_ch_cntxt *buff = *out_buff;
	int rc;
	u32 j;

	for (j = 0; j < max_qid; j++) {
		cudbg_read_sge_ctxt(pdbg_init, j, ctxt_type, buff->data);
		rc = cudbg_sge_ctxt_check_valid(buff->data, ctxt_type);
		if (!rc)
			continue;

		buff->cntxt_type = ctxt_type;
		buff->cntxt_id = j;
		buff++;
		if (ctxt_type == CTXT_FLM) {
			cudbg_read_sge_ctxt(pdbg_init, j, CTXT_CNM, buff->data);
			buff->cntxt_type = CTXT_CNM;
			buff->cntxt_id = j;
			buff++;
		}
	}

	*out_buff = buff;
}

int cudbg_collect_dump_context(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	struct cudbg_region_info region_info[CTXT_CNM + 1] = { {0} };
	struct adapter *padap = pdbg_init->adap;
	u32 j, size, max_ctx_size, max_ctx_qid;
	u8 mem_type[CTXT_INGRESS + 1] = { 0 };
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_ch_cntxt *buff;
	u8 *ctx_buf;
	u8 i, k;
	int rc;

	/* Get max valid qid for each type of queue */
	rc = cudbg_get_ctxt_region_info(padap, region_info, mem_type);
	if (rc)
		return rc;

	rc = cudbg_dump_context_size(padap);
	if (rc <= 0)
		return CUDBG_STATUS_ENTITY_NOT_FOUND;

	size = rc;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	/* Get buffer with enough space to read the biggest context
	 * region in memory.
	 */
	max_ctx_size = max(region_info[CTXT_EGRESS].end -
			   region_info[CTXT_EGRESS].start + 1,
			   region_info[CTXT_INGRESS].end -
			   region_info[CTXT_INGRESS].start + 1);

	ctx_buf = kvzalloc(max_ctx_size, GFP_KERNEL);
	if (!ctx_buf) {
		cudbg_put_buff(pdbg_init, &temp_buff);
		return -ENOMEM;
	}

	buff = (struct cudbg_ch_cntxt *)temp_buff.data;

	/* Collect EGRESS and INGRESS context data.
	 * In case of failures, fallback to collecting via FW or
	 * backdoor access.
	 */
	for (i = CTXT_EGRESS; i <= CTXT_INGRESS; i++) {
		if (!region_info[i].exist) {
			max_ctx_qid = CUDBG_LOWMEM_MAX_CTXT_QIDS;
			cudbg_get_sge_ctxt_fw(pdbg_init, max_ctx_qid, i,
					      &buff);
			continue;
		}

		max_ctx_size = region_info[i].end - region_info[i].start + 1;
		max_ctx_qid = max_ctx_size / SGE_CTXT_SIZE;

		/* If firmware is not attached/alive, use backdoor register
		 * access to collect dump.
		 */
		if (is_fw_attached(pdbg_init)) {
			t4_sge_ctxt_flush(padap, padap->mbox, i);

			rc = t4_memory_rw(padap, MEMWIN_NIC, mem_type[i],
					  region_info[i].start, max_ctx_size,
					  (__be32 *)ctx_buf, 1);
		}

		if (rc || !is_fw_attached(pdbg_init)) {
			max_ctx_qid = CUDBG_LOWMEM_MAX_CTXT_QIDS;
			cudbg_get_sge_ctxt_fw(pdbg_init, max_ctx_qid, i,
					      &buff);
			continue;
		}

		for (j = 0; j < max_ctx_qid; j++) {
			__be64 *dst_off;
			u64 *src_off;

			src_off = (u64 *)(ctx_buf + j * SGE_CTXT_SIZE);
			dst_off = (__be64 *)buff->data;

			/* The data is stored in 64-bit cpu order.  Convert it
			 * to big endian before parsing.
			 */
			for (k = 0; k < SGE_CTXT_SIZE / sizeof(u64); k++)
				dst_off[k] = cpu_to_be64(src_off[k]);

			rc = cudbg_sge_ctxt_check_valid(buff->data, i);
			if (!rc)
				continue;

			buff->cntxt_type = i;
			buff->cntxt_id = j;
			buff++;
		}
	}

	kvfree(ctx_buf);

	/* Collect FREELIST and CONGESTION MANAGER contexts */
	max_ctx_size = region_info[CTXT_FLM].end -
		       region_info[CTXT_FLM].start + 1;
	max_ctx_qid = max_ctx_size / SGE_CTXT_SIZE;
	/* Since FLM and CONM are 1-to-1 mapped, the below function
	 * will fetch both FLM and CONM contexts.
	 */
	cudbg_get_sge_ctxt_fw(pdbg_init, max_ctx_qid, CTXT_FLM, &buff);

	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

static inline void cudbg_tcamxy2valmask(u64 x, u64 y, u8 *addr, u64 *mask)
{
	*mask = x | y;
	y = (__force u64)cpu_to_be64(y);
	memcpy(addr, (char *)&y + 2, ETH_ALEN);
}

static void cudbg_mps_rpl_backdoor(struct adapter *padap,
				   struct fw_ldst_mps_rplc *mps_rplc)
{
	if (is_t5(padap->params.chip)) {
		mps_rplc->rplc255_224 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP3_A));
		mps_rplc->rplc223_192 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP2_A));
		mps_rplc->rplc191_160 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP1_A));
		mps_rplc->rplc159_128 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP0_A));
	} else {
		mps_rplc->rplc255_224 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP7_A));
		mps_rplc->rplc223_192 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP6_A));
		mps_rplc->rplc191_160 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP5_A));
		mps_rplc->rplc159_128 = htonl(t4_read_reg(padap,
							  MPS_VF_RPLCT_MAP4_A));
	}
	mps_rplc->rplc127_96 = htonl(t4_read_reg(padap, MPS_VF_RPLCT_MAP3_A));
	mps_rplc->rplc95_64 = htonl(t4_read_reg(padap, MPS_VF_RPLCT_MAP2_A));
	mps_rplc->rplc63_32 = htonl(t4_read_reg(padap, MPS_VF_RPLCT_MAP1_A));
	mps_rplc->rplc31_0 = htonl(t4_read_reg(padap, MPS_VF_RPLCT_MAP0_A));
}

static int cudbg_collect_tcam_index(struct cudbg_init *pdbg_init,
				    struct cudbg_mps_tcam *tcam, u32 idx)
{
	struct adapter *padap = pdbg_init->adap;
	u64 tcamy, tcamx, val;
	u32 ctl, data2;
	int rc = 0;

	if (CHELSIO_CHIP_VERSION(padap->params.chip) >= CHELSIO_T6) {
		/* CtlReqID   - 1: use Host Driver Requester ID
		 * CtlCmdType - 0: Read, 1: Write
		 * CtlTcamSel - 0: TCAM0, 1: TCAM1
		 * CtlXYBitSel- 0: Y bit, 1: X bit
		 */

		/* Read tcamy */
		ctl = CTLREQID_V(1) | CTLCMDTYPE_V(0) | CTLXYBITSEL_V(0);
		if (idx < 256)
			ctl |= CTLTCAMINDEX_V(idx) | CTLTCAMSEL_V(0);
		else
			ctl |= CTLTCAMINDEX_V(idx - 256) | CTLTCAMSEL_V(1);

		t4_write_reg(padap, MPS_CLS_TCAM_DATA2_CTL_A, ctl);
		val = t4_read_reg(padap, MPS_CLS_TCAM_RDATA1_REQ_ID1_A);
		tcamy = DMACH_G(val) << 32;
		tcamy |= t4_read_reg(padap, MPS_CLS_TCAM_RDATA0_REQ_ID1_A);
		data2 = t4_read_reg(padap, MPS_CLS_TCAM_RDATA2_REQ_ID1_A);
		tcam->lookup_type = DATALKPTYPE_G(data2);

		/* 0 - Outer header, 1 - Inner header
		 * [71:48] bit locations are overloaded for
		 * outer vs. inner lookup types.
		 */
		if (tcam->lookup_type && tcam->lookup_type != DATALKPTYPE_M) {
			/* Inner header VNI */
			tcam->vniy = (data2 & DATAVIDH2_F) | DATAVIDH1_G(data2);
			tcam->vniy = (tcam->vniy << 16) | VIDL_G(val);
			tcam->dip_hit = data2 & DATADIPHIT_F;
		} else {
			tcam->vlan_vld = data2 & DATAVIDH2_F;
			tcam->ivlan = VIDL_G(val);
		}

		tcam->port_num = DATAPORTNUM_G(data2);

		/* Read tcamx. Change the control param */
		ctl |= CTLXYBITSEL_V(1);
		t4_write_reg(padap, MPS_CLS_TCAM_DATA2_CTL_A, ctl);
		val = t4_read_reg(padap, MPS_CLS_TCAM_RDATA1_REQ_ID1_A);
		tcamx = DMACH_G(val) << 32;
		tcamx |= t4_read_reg(padap, MPS_CLS_TCAM_RDATA0_REQ_ID1_A);
		data2 = t4_read_reg(padap, MPS_CLS_TCAM_RDATA2_REQ_ID1_A);
		if (tcam->lookup_type && tcam->lookup_type != DATALKPTYPE_M) {
			/* Inner header VNI mask */
			tcam->vnix = (data2 & DATAVIDH2_F) | DATAVIDH1_G(data2);
			tcam->vnix = (tcam->vnix << 16) | VIDL_G(val);
		}
	} else {
		tcamy = t4_read_reg64(padap, MPS_CLS_TCAM_Y_L(idx));
		tcamx = t4_read_reg64(padap, MPS_CLS_TCAM_X_L(idx));
	}

	/* If no entry, return */
	if (tcamx & tcamy)
		return rc;

	tcam->cls_lo = t4_read_reg(padap, MPS_CLS_SRAM_L(idx));
	tcam->cls_hi = t4_read_reg(padap, MPS_CLS_SRAM_H(idx));

	if (is_t5(padap->params.chip))
		tcam->repli = (tcam->cls_lo & REPLICATE_F);
	else if (is_t6(padap->params.chip))
		tcam->repli = (tcam->cls_lo & T6_REPLICATE_F);

	if (tcam->repli) {
		struct fw_ldst_cmd ldst_cmd;
		struct fw_ldst_mps_rplc mps_rplc;

		memset(&ldst_cmd, 0, sizeof(ldst_cmd));
		ldst_cmd.op_to_addrspace =
			htonl(FW_CMD_OP_V(FW_LDST_CMD) |
			      FW_CMD_REQUEST_F | FW_CMD_READ_F |
			      FW_LDST_CMD_ADDRSPACE_V(FW_LDST_ADDRSPC_MPS));
		ldst_cmd.cycles_to_len16 = htonl(FW_LEN16(ldst_cmd));
		ldst_cmd.u.mps.rplc.fid_idx =
			htons(FW_LDST_CMD_FID_V(FW_LDST_MPS_RPLC) |
			      FW_LDST_CMD_IDX_V(idx));

		/* If firmware is not attached/alive, use backdoor register
		 * access to collect dump.
		 */
		if (is_fw_attached(pdbg_init))
			rc = t4_wr_mbox(padap, padap->mbox, &ldst_cmd,
					sizeof(ldst_cmd), &ldst_cmd);

		if (rc || !is_fw_attached(pdbg_init)) {
			cudbg_mps_rpl_backdoor(padap, &mps_rplc);
			/* Ignore error since we collected directly from
			 * reading registers.
			 */
			rc = 0;
		} else {
			mps_rplc = ldst_cmd.u.mps.rplc;
		}

		tcam->rplc[0] = ntohl(mps_rplc.rplc31_0);
		tcam->rplc[1] = ntohl(mps_rplc.rplc63_32);
		tcam->rplc[2] = ntohl(mps_rplc.rplc95_64);
		tcam->rplc[3] = ntohl(mps_rplc.rplc127_96);
		if (padap->params.arch.mps_rplc_size > CUDBG_MAX_RPLC_SIZE) {
			tcam->rplc[4] = ntohl(mps_rplc.rplc159_128);
			tcam->rplc[5] = ntohl(mps_rplc.rplc191_160);
			tcam->rplc[6] = ntohl(mps_rplc.rplc223_192);
			tcam->rplc[7] = ntohl(mps_rplc.rplc255_224);
		}
	}
	cudbg_tcamxy2valmask(tcamx, tcamy, tcam->addr, &tcam->mask);
	tcam->idx = idx;
	tcam->rplc_size = padap->params.arch.mps_rplc_size;
	return rc;
}

int cudbg_collect_mps_tcam(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	u32 size = 0, i, n, total_size = 0;
	struct cudbg_mps_tcam *tcam;
	int rc;

	n = padap->params.arch.mps_tcam_size;
	size = sizeof(struct cudbg_mps_tcam) * n;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	tcam = (struct cudbg_mps_tcam *)temp_buff.data;
	for (i = 0; i < n; i++) {
		rc = cudbg_collect_tcam_index(pdbg_init, tcam, i);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
		total_size += sizeof(struct cudbg_mps_tcam);
		tcam++;
	}

	if (!total_size) {
		rc = CUDBG_SYSTEM_ERROR;
		cudbg_err->sys_err = rc;
		cudbg_put_buff(pdbg_init, &temp_buff);
		return rc;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_vpd_data(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	char vpd_str[CUDBG_VPD_VER_LEN + 1];
	struct cudbg_vpd_data *vpd_data;
	struct vpd_params vpd = { 0 };
	u32 vpd_vers, fw_vers;
	int rc;

	rc = t4_get_raw_vpd_params(padap, &vpd);
	if (rc)
		return rc;

	rc = t4_get_fw_version(padap, &fw_vers);
	if (rc)
		return rc;

	rc = cudbg_read_vpd_reg(padap, CUDBG_VPD_VER_ADDR, CUDBG_VPD_VER_LEN,
				vpd_str);
	if (rc)
		return rc;

	vpd_str[CUDBG_VPD_VER_LEN] = '\0';
	rc = kstrtouint(vpd_str, 0, &vpd_vers);
	if (rc)
		return rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff, sizeof(struct cudbg_vpd_data),
			    &temp_buff);
	if (rc)
		return rc;

	vpd_data = (struct cudbg_vpd_data *)temp_buff.data;
	memcpy(vpd_data->sn, vpd.sn, SERNUM_LEN + 1);
	memcpy(vpd_data->bn, vpd.pn, PN_LEN + 1);
	memcpy(vpd_data->na, vpd.na, MACADDR_LEN + 1);
	memcpy(vpd_data->mn, vpd.id, ID_LEN + 1);
	vpd_data->scfg_vers = t4_read_reg(padap, PCIE_STATIC_SPARE2_A);
	vpd_data->vpd_vers = vpd_vers;
	vpd_data->fw_major = FW_HDR_FW_VER_MAJOR_G(fw_vers);
	vpd_data->fw_minor = FW_HDR_FW_VER_MINOR_G(fw_vers);
	vpd_data->fw_micro = FW_HDR_FW_VER_MICRO_G(fw_vers);
	vpd_data->fw_build = FW_HDR_FW_VER_BUILD_G(fw_vers);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

static int cudbg_read_tid(struct cudbg_init *pdbg_init, u32 tid,
			  struct cudbg_tid_data *tid_data)
{
	struct adapter *padap = pdbg_init->adap;
	int i, cmd_retry = 8;
	u32 val;

	/* Fill REQ_DATA regs with 0's */
	for (i = 0; i < NUM_LE_DB_DBGI_REQ_DATA_INSTANCES; i++)
		t4_write_reg(padap, LE_DB_DBGI_REQ_DATA_A + (i << 2), 0);

	/* Write DBIG command */
	val = DBGICMD_V(4) | DBGITID_V(tid);
	t4_write_reg(padap, LE_DB_DBGI_REQ_TCAM_CMD_A, val);
	tid_data->dbig_cmd = val;

	val = DBGICMDSTRT_F | DBGICMDMODE_V(1); /* LE mode */
	t4_write_reg(padap, LE_DB_DBGI_CONFIG_A, val);
	tid_data->dbig_conf = val;

	/* Poll the DBGICMDBUSY bit */
	val = 1;
	while (val) {
		val = t4_read_reg(padap, LE_DB_DBGI_CONFIG_A);
		val = val & DBGICMDBUSY_F;
		cmd_retry--;
		if (!cmd_retry)
			return CUDBG_SYSTEM_ERROR;
	}

	/* Check RESP status */
	val = t4_read_reg(padap, LE_DB_DBGI_RSP_STATUS_A);
	tid_data->dbig_rsp_stat = val;
	if (!(val & 1))
		return CUDBG_SYSTEM_ERROR;

	/* Read RESP data */
	for (i = 0; i < NUM_LE_DB_DBGI_RSP_DATA_INSTANCES; i++)
		tid_data->data[i] = t4_read_reg(padap,
						LE_DB_DBGI_RSP_DATA_A +
						(i << 2));
	tid_data->tid = tid;
	return 0;
}

static int cudbg_get_le_type(u32 tid, struct cudbg_tcam tcam_region)
{
	int type = LE_ET_UNKNOWN;

	if (tid < tcam_region.server_start)
		type = LE_ET_TCAM_CON;
	else if (tid < tcam_region.filter_start)
		type = LE_ET_TCAM_SERVER;
	else if (tid < tcam_region.clip_start)
		type = LE_ET_TCAM_FILTER;
	else if (tid < tcam_region.routing_start)
		type = LE_ET_TCAM_CLIP;
	else if (tid < tcam_region.tid_hash_base)
		type = LE_ET_TCAM_ROUTING;
	else if (tid < tcam_region.max_tid)
		type = LE_ET_HASH_CON;
	else
		type = LE_ET_INVALID_TID;

	return type;
}

static int cudbg_is_ipv6_entry(struct cudbg_tid_data *tid_data,
			       struct cudbg_tcam tcam_region)
{
	int ipv6 = 0;
	int le_type;

	le_type = cudbg_get_le_type(tid_data->tid, tcam_region);
	if (tid_data->tid & 1)
		return 0;

	if (le_type == LE_ET_HASH_CON) {
		ipv6 = tid_data->data[16] & 0x8000;
	} else if (le_type == LE_ET_TCAM_CON) {
		ipv6 = tid_data->data[16] & 0x8000;
		if (ipv6)
			ipv6 = tid_data->data[9] == 0x00C00000;
	} else {
		ipv6 = 0;
	}
	return ipv6;
}

void cudbg_fill_le_tcam_info(struct adapter *padap,
			     struct cudbg_tcam *tcam_region)
{
	u32 value;

	/* Get the LE regions */
	value = t4_read_reg(padap, LE_DB_TID_HASHBASE_A); /* hash base index */
	tcam_region->tid_hash_base = value;

	/* Get routing table index */
	value = t4_read_reg(padap, LE_DB_ROUTING_TABLE_INDEX_A);
	tcam_region->routing_start = value;

	/* Get clip table index. For T6 there is separate CLIP TCAM */
	if (is_t6(padap->params.chip))
		value = t4_read_reg(padap, LE_DB_CLCAM_TID_BASE_A);
	else
		value = t4_read_reg(padap, LE_DB_CLIP_TABLE_INDEX_A);
	tcam_region->clip_start = value;

	/* Get filter table index */
	value = t4_read_reg(padap, LE_DB_FILTER_TABLE_INDEX_A);
	tcam_region->filter_start = value;

	/* Get server table index */
	value = t4_read_reg(padap, LE_DB_SERVER_INDEX_A);
	tcam_region->server_start = value;

	/* Check whether hash is enabled and calculate the max tids */
	value = t4_read_reg(padap, LE_DB_CONFIG_A);
	if ((value >> HASHEN_S) & 1) {
		value = t4_read_reg(padap, LE_DB_HASH_CONFIG_A);
		if (CHELSIO_CHIP_VERSION(padap->params.chip) > CHELSIO_T5) {
			tcam_region->max_tid = (value & 0xFFFFF) +
					       tcam_region->tid_hash_base;
		} else {
			value = HASHTIDSIZE_G(value);
			value = 1 << value;
			tcam_region->max_tid = value +
					       tcam_region->tid_hash_base;
		}
	} else { /* hash not enabled */
		if (is_t6(padap->params.chip))
			tcam_region->max_tid = (value & ASLIPCOMPEN_F) ?
					       CUDBG_MAX_TID_COMP_EN :
					       CUDBG_MAX_TID_COMP_DIS;
		else
			tcam_region->max_tid = CUDBG_MAX_TCAM_TID;
	}

	if (is_t6(padap->params.chip))
		tcam_region->max_tid += CUDBG_T6_CLIP;
}

int cudbg_collect_le_tcam(struct cudbg_init *pdbg_init,
			  struct cudbg_buffer *dbg_buff,
			  struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_tcam tcam_region = { 0 };
	struct cudbg_tid_data *tid_data;
	u32 bytes = 0;
	int rc, size;
	u32 i;

	cudbg_fill_le_tcam_info(padap, &tcam_region);

	size = sizeof(struct cudbg_tid_data) * tcam_region.max_tid;
	size += sizeof(struct cudbg_tcam);
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	memcpy(temp_buff.data, &tcam_region, sizeof(struct cudbg_tcam));
	bytes = sizeof(struct cudbg_tcam);
	tid_data = (struct cudbg_tid_data *)(temp_buff.data + bytes);
	/* read all tid */
	for (i = 0; i < tcam_region.max_tid; ) {
		rc = cudbg_read_tid(pdbg_init, i, tid_data);
		if (rc) {
			cudbg_err->sys_warn = CUDBG_STATUS_PARTIAL_DATA;
			/* Update tcam header and exit */
			tcam_region.max_tid = i;
			memcpy(temp_buff.data, &tcam_region,
			       sizeof(struct cudbg_tcam));
			goto out;
		}

		if (cudbg_is_ipv6_entry(tid_data, tcam_region)) {
			/* T6 CLIP TCAM: ipv6 takes 4 entries */
			if (is_t6(padap->params.chip) &&
			    i >= tcam_region.clip_start &&
			    i < tcam_region.clip_start + CUDBG_T6_CLIP)
				i += 4;
			else /* Main TCAM: ipv6 takes two tids */
				i += 2;
		} else {
			i++;
		}

		tid_data++;
		bytes += sizeof(struct cudbg_tid_data);
	}

out:
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_cctrl(struct cudbg_init *pdbg_init,
			struct cudbg_buffer *dbg_buff,
			struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	u32 size;
	int rc;

	size = sizeof(u16) * NMTUS * NCCTRL_WIN;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	t4_read_cong_tbl(padap, (void *)temp_buff.data);
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_ma_indirect(struct cudbg_init *pdbg_init,
			      struct cudbg_buffer *dbg_buff,
			      struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct ireg_buf *ma_indr;
	int i, rc, n;
	u32 size, j;

	if (CHELSIO_CHIP_VERSION(padap->params.chip) < CHELSIO_T6)
		return CUDBG_STATUS_ENTITY_NOT_FOUND;

	n = sizeof(t6_ma_ireg_array) / (IREG_NUM_ELEM * sizeof(u32));
	size = sizeof(struct ireg_buf) * n * 2;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	ma_indr = (struct ireg_buf *)temp_buff.data;
	for (i = 0; i < n; i++) {
		struct ireg_field *ma_fli = &ma_indr->tp_pio;
		u32 *buff = ma_indr->outbuf;

		ma_fli->ireg_addr = t6_ma_ireg_array[i][0];
		ma_fli->ireg_data = t6_ma_ireg_array[i][1];
		ma_fli->ireg_local_offset = t6_ma_ireg_array[i][2];
		ma_fli->ireg_offset_range = t6_ma_ireg_array[i][3];
		t4_read_indirect(padap, ma_fli->ireg_addr, ma_fli->ireg_data,
				 buff, ma_fli->ireg_offset_range,
				 ma_fli->ireg_local_offset);
		ma_indr++;
	}

	n = sizeof(t6_ma_ireg_array2) / (IREG_NUM_ELEM * sizeof(u32));
	for (i = 0; i < n; i++) {
		struct ireg_field *ma_fli = &ma_indr->tp_pio;
		u32 *buff = ma_indr->outbuf;

		ma_fli->ireg_addr = t6_ma_ireg_array2[i][0];
		ma_fli->ireg_data = t6_ma_ireg_array2[i][1];
		ma_fli->ireg_local_offset = t6_ma_ireg_array2[i][2];
		for (j = 0; j < t6_ma_ireg_array2[i][3]; j++) {
			t4_read_indirect(padap, ma_fli->ireg_addr,
					 ma_fli->ireg_data, buff, 1,
					 ma_fli->ireg_local_offset);
			buff++;
			ma_fli->ireg_local_offset += 0x20;
		}
		ma_indr++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_ulptx_la(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_ulptx_la *ulptx_la_buff;
	struct cudbg_ver_hdr *ver_hdr;
	u32 i, j;
	int rc;

	rc = cudbg_get_buff(pdbg_init, dbg_buff,
			    sizeof(struct cudbg_ver_hdr) +
			    sizeof(struct cudbg_ulptx_la),
			    &temp_buff);
	if (rc)
		return rc;

	ver_hdr = (struct cudbg_ver_hdr *)temp_buff.data;
	ver_hdr->signature = CUDBG_ENTITY_SIGNATURE;
	ver_hdr->revision = CUDBG_ULPTX_LA_REV;
	ver_hdr->size = sizeof(struct cudbg_ulptx_la);

	ulptx_la_buff = (struct cudbg_ulptx_la *)(temp_buff.data +
						  sizeof(*ver_hdr));
	for (i = 0; i < CUDBG_NUM_ULPTX; i++) {
		ulptx_la_buff->rdptr[i] = t4_read_reg(padap,
						      ULP_TX_LA_RDPTR_0_A +
						      0x10 * i);
		ulptx_la_buff->wrptr[i] = t4_read_reg(padap,
						      ULP_TX_LA_WRPTR_0_A +
						      0x10 * i);
		ulptx_la_buff->rddata[i] = t4_read_reg(padap,
						       ULP_TX_LA_RDDATA_0_A +
						       0x10 * i);
		for (j = 0; j < CUDBG_NUM_ULPTX_READ; j++)
			ulptx_la_buff->rd_data[i][j] =
				t4_read_reg(padap,
					    ULP_TX_LA_RDDATA_0_A + 0x10 * i);
	}

	for (i = 0; i < CUDBG_NUM_ULPTX_ASIC_READ; i++) {
		t4_write_reg(padap, ULP_TX_ASIC_DEBUG_CTRL_A, 0x1);
		ulptx_la_buff->rdptr_asic[i] =
				t4_read_reg(padap, ULP_TX_ASIC_DEBUG_CTRL_A);
		ulptx_la_buff->rddata_asic[i][0] =
				t4_read_reg(padap, ULP_TX_ASIC_DEBUG_0_A);
		ulptx_la_buff->rddata_asic[i][1] =
				t4_read_reg(padap, ULP_TX_ASIC_DEBUG_1_A);
		ulptx_la_buff->rddata_asic[i][2] =
				t4_read_reg(padap, ULP_TX_ASIC_DEBUG_2_A);
		ulptx_la_buff->rddata_asic[i][3] =
				t4_read_reg(padap, ULP_TX_ASIC_DEBUG_3_A);
		ulptx_la_buff->rddata_asic[i][4] =
				t4_read_reg(padap, ULP_TX_ASIC_DEBUG_4_A);
		ulptx_la_buff->rddata_asic[i][5] =
				t4_read_reg(padap, PM_RX_BASE_ADDR);
	}

	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_up_cim_indirect(struct cudbg_init *pdbg_init,
				  struct cudbg_buffer *dbg_buff,
				  struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	u32 local_offset, local_range;
	struct ireg_buf *up_cim;
	u32 size, j, iter;
	u32 instance = 0;
	int i, rc, n;

	if (is_t5(padap->params.chip))
		n = sizeof(t5_up_cim_reg_array) /
		    ((IREG_NUM_ELEM + 1) * sizeof(u32));
	else if (is_t6(padap->params.chip))
		n = sizeof(t6_up_cim_reg_array) /
		    ((IREG_NUM_ELEM + 1) * sizeof(u32));
	else
		return CUDBG_STATUS_NOT_IMPLEMENTED;

	size = sizeof(struct ireg_buf) * n;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	up_cim = (struct ireg_buf *)temp_buff.data;
	for (i = 0; i < n; i++) {
		struct ireg_field *up_cim_reg = &up_cim->tp_pio;
		u32 *buff = up_cim->outbuf;

		if (is_t5(padap->params.chip)) {
			up_cim_reg->ireg_addr = t5_up_cim_reg_array[i][0];
			up_cim_reg->ireg_data = t5_up_cim_reg_array[i][1];
			up_cim_reg->ireg_local_offset =
						t5_up_cim_reg_array[i][2];
			up_cim_reg->ireg_offset_range =
						t5_up_cim_reg_array[i][3];
			instance = t5_up_cim_reg_array[i][4];
		} else if (is_t6(padap->params.chip)) {
			up_cim_reg->ireg_addr = t6_up_cim_reg_array[i][0];
			up_cim_reg->ireg_data = t6_up_cim_reg_array[i][1];
			up_cim_reg->ireg_local_offset =
						t6_up_cim_reg_array[i][2];
			up_cim_reg->ireg_offset_range =
						t6_up_cim_reg_array[i][3];
			instance = t6_up_cim_reg_array[i][4];
		}

		switch (instance) {
		case NUM_CIM_CTL_TSCH_CHANNEL_INSTANCES:
			iter = up_cim_reg->ireg_offset_range;
			local_offset = 0x120;
			local_range = 1;
			break;
		case NUM_CIM_CTL_TSCH_CHANNEL_TSCH_CLASS_INSTANCES:
			iter = up_cim_reg->ireg_offset_range;
			local_offset = 0x10;
			local_range = 1;
			break;
		default:
			iter = 1;
			local_offset = 0;
			local_range = up_cim_reg->ireg_offset_range;
			break;
		}

		for (j = 0; j < iter; j++, buff++) {
			rc = t4_cim_read(padap,
					 up_cim_reg->ireg_local_offset +
					 (j * local_offset), local_range, buff);
			if (rc) {
				cudbg_put_buff(pdbg_init, &temp_buff);
				return rc;
			}
		}
		up_cim++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_pbt_tables(struct cudbg_init *pdbg_init,
			     struct cudbg_buffer *dbg_buff,
			     struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_pbt_tables *pbt;
	int i, rc;
	u32 addr;

	rc = cudbg_get_buff(pdbg_init, dbg_buff,
			    sizeof(struct cudbg_pbt_tables),
			    &temp_buff);
	if (rc)
		return rc;

	pbt = (struct cudbg_pbt_tables *)temp_buff.data;
	/* PBT dynamic entries */
	addr = CUDBG_CHAC_PBT_ADDR;
	for (i = 0; i < CUDBG_PBT_DYNAMIC_ENTRIES; i++) {
		rc = t4_cim_read(padap, addr + (i * 4), 1,
				 &pbt->pbt_dynamic[i]);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
	}

	/* PBT static entries */
	/* static entries start when bit 6 is set */
	addr = CUDBG_CHAC_PBT_ADDR + (1 << 6);
	for (i = 0; i < CUDBG_PBT_STATIC_ENTRIES; i++) {
		rc = t4_cim_read(padap, addr + (i * 4), 1,
				 &pbt->pbt_static[i]);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
	}

	/* LRF entries */
	addr = CUDBG_CHAC_PBT_LRF;
	for (i = 0; i < CUDBG_LRF_ENTRIES; i++) {
		rc = t4_cim_read(padap, addr + (i * 4), 1,
				 &pbt->lrf_table[i]);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
	}

	/* PBT data entries */
	addr = CUDBG_CHAC_PBT_DATA;
	for (i = 0; i < CUDBG_PBT_DATA_ENTRIES; i++) {
		rc = t4_cim_read(padap, addr + (i * 4), 1,
				 &pbt->pbt_data[i]);
		if (rc) {
			cudbg_err->sys_err = rc;
			cudbg_put_buff(pdbg_init, &temp_buff);
			return rc;
		}
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_mbox_log(struct cudbg_init *pdbg_init,
			   struct cudbg_buffer *dbg_buff,
			   struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_mbox_log *mboxlog = NULL;
	struct cudbg_buffer temp_buff = { 0 };
	struct mbox_cmd_log *log = NULL;
	struct mbox_cmd *entry;
	unsigned int entry_idx;
	u16 mbox_cmds;
	int i, k, rc;
	u64 flit;
	u32 size;

	log = padap->mbox_log;
	mbox_cmds = padap->mbox_log->size;
	size = sizeof(struct cudbg_mbox_log) * mbox_cmds;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	mboxlog = (struct cudbg_mbox_log *)temp_buff.data;
	for (k = 0; k < mbox_cmds; k++) {
		entry_idx = log->cursor + k;
		if (entry_idx >= log->size)
			entry_idx -= log->size;

		entry = mbox_cmd_log_entry(log, entry_idx);
		/* skip over unused entries */
		if (entry->timestamp == 0)
			continue;

		memcpy(&mboxlog->entry, entry, sizeof(struct mbox_cmd));
		for (i = 0; i < MBOX_LEN / 8; i++) {
			flit = entry->cmd[i];
			mboxlog->hi[i] = (u32)(flit >> 32);
			mboxlog->lo[i] = (u32)flit;
		}
		mboxlog++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

int cudbg_collect_hma_indirect(struct cudbg_init *pdbg_init,
			       struct cudbg_buffer *dbg_buff,
			       struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct ireg_buf *hma_indr;
	int i, rc, n;
	u32 size;

	if (CHELSIO_CHIP_VERSION(padap->params.chip) < CHELSIO_T6)
		return CUDBG_STATUS_ENTITY_NOT_FOUND;

	n = sizeof(t6_hma_ireg_array) / (IREG_NUM_ELEM * sizeof(u32));
	size = sizeof(struct ireg_buf) * n;
	rc = cudbg_get_buff(pdbg_init, dbg_buff, size, &temp_buff);
	if (rc)
		return rc;

	hma_indr = (struct ireg_buf *)temp_buff.data;
	for (i = 0; i < n; i++) {
		struct ireg_field *hma_fli = &hma_indr->tp_pio;
		u32 *buff = hma_indr->outbuf;

		hma_fli->ireg_addr = t6_hma_ireg_array[i][0];
		hma_fli->ireg_data = t6_hma_ireg_array[i][1];
		hma_fli->ireg_local_offset = t6_hma_ireg_array[i][2];
		hma_fli->ireg_offset_range = t6_hma_ireg_array[i][3];
		t4_read_indirect(padap, hma_fli->ireg_addr, hma_fli->ireg_data,
				 buff, hma_fli->ireg_offset_range,
				 hma_fli->ireg_local_offset);
		hma_indr++;
	}
	return cudbg_write_and_release_buff(pdbg_init, &temp_buff, dbg_buff);
}

void cudbg_fill_qdesc_num_and_size(const struct adapter *padap,
				   u32 *num, u32 *size)
{
	u32 tot_entries = 0, tot_size = 0;

	/* NIC TXQ, RXQ, FLQ, and CTRLQ */
	tot_entries += MAX_ETH_QSETS * 3;
	tot_entries += MAX_CTRL_QUEUES;

	tot_size += MAX_ETH_QSETS * MAX_TXQ_ENTRIES * MAX_TXQ_DESC_SIZE;
	tot_size += MAX_ETH_QSETS * MAX_RSPQ_ENTRIES * MAX_RXQ_DESC_SIZE;
	tot_size += MAX_ETH_QSETS * MAX_RX_BUFFERS * MAX_FL_DESC_SIZE;
	tot_size += MAX_CTRL_QUEUES * MAX_CTRL_TXQ_ENTRIES *
		    MAX_CTRL_TXQ_DESC_SIZE;

	/* FW_EVTQ and INTRQ */
	tot_entries += INGQ_EXTRAS;
	tot_size += INGQ_EXTRAS * MAX_RSPQ_ENTRIES * MAX_RXQ_DESC_SIZE;

	/* PTP_TXQ */
	tot_entries += 1;
	tot_size += MAX_TXQ_ENTRIES * MAX_TXQ_DESC_SIZE;

	/* ULD TXQ, RXQ, and FLQ */
	tot_entries += CXGB4_TX_MAX * MAX_OFLD_QSETS;
	tot_entries += CXGB4_ULD_MAX * MAX_ULD_QSETS * 2;

	tot_size += CXGB4_TX_MAX * MAX_OFLD_QSETS * MAX_TXQ_ENTRIES *
		    MAX_TXQ_DESC_SIZE;
	tot_size += CXGB4_ULD_MAX * MAX_ULD_QSETS * MAX_RSPQ_ENTRIES *
		    MAX_RXQ_DESC_SIZE;
	tot_size += CXGB4_ULD_MAX * MAX_ULD_QSETS * MAX_RX_BUFFERS *
		    MAX_FL_DESC_SIZE;

	/* ULD CIQ */
	tot_entries += CXGB4_ULD_MAX * MAX_ULD_QSETS;
	tot_size += CXGB4_ULD_MAX * MAX_ULD_QSETS * SGE_MAX_IQ_SIZE *
		    MAX_RXQ_DESC_SIZE;

	/* ETHOFLD TXQ, RXQ, and FLQ */
	tot_entries += MAX_OFLD_QSETS * 3;
	tot_size += MAX_OFLD_QSETS * MAX_TXQ_ENTRIES * MAX_TXQ_DESC_SIZE;

	tot_size += sizeof(struct cudbg_ver_hdr) +
		    sizeof(struct cudbg_qdesc_info) +
		    sizeof(struct cudbg_qdesc_entry) * tot_entries;

	if (num)
		*num = tot_entries;

	if (size)
		*size = tot_size;
}

int cudbg_collect_qdesc(struct cudbg_init *pdbg_init,
			struct cudbg_buffer *dbg_buff,
			struct cudbg_error *cudbg_err)
{
	u32 num_queues = 0, tot_entries = 0, size = 0;
	struct adapter *padap = pdbg_init->adap;
	struct cudbg_buffer temp_buff = { 0 };
	struct cudbg_qdesc_entry *qdesc_entry;
	struct cudbg_qdesc_info *qdesc_info;
	struct cudbg_ver_hdr *ver_hdr;
	struct sge *s = &padap->sge;
	u32 i, j, cur_off, tot_len;
	u8 *data;
	int rc;

	cudbg_fill_qdesc_num_and_size(padap, &tot_entries, &size);
	size = min_t(u32, size, CUDBG_DUMP_BUFF_SIZE);
	tot_len = size;
	data = kvzalloc(size, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	ver_hdr = (struct cudbg_ver_hdr *)data;
	ver_hdr->signature = CUDBG_ENTITY_SIGNATURE;
	ver_hdr->revision = CUDBG_QDESC_REV;
	ver_hdr->size = sizeof(struct cudbg_qdesc_info);
	size -= sizeof(*ver_hdr);

	qdesc_info = (struct cudbg_qdesc_info *)(data +
						 sizeof(*ver_hdr));
	size -= sizeof(*qdesc_info);
	qdesc_entry = (struct cudbg_qdesc_entry *)qdesc_info->data;

#define QDESC_GET(q, desc, type, label) do { \
	if (size <= 0) { \
		goto label; \
	} \
	if (desc) { \
		cudbg_fill_qdesc_##q(q, type, qdesc_entry); \
		size -= sizeof(*qdesc_entry) + qdesc_entry->data_size; \
		num_queues++; \
		qdesc_entry = cudbg_next_qdesc(qdesc_entry); \
	} \
} while (0)

#define QDESC_GET_TXQ(q, type, label) do { \
	struct sge_txq *txq = (struct sge_txq *)q; \
	QDESC_GET(txq, txq->desc, type, label); \
} while (0)

#define QDESC_GET_RXQ(q, type, label) do { \
	struct sge_rspq *rxq = (struct sge_rspq *)q; \
	QDESC_GET(rxq, rxq->desc, type, label); \
} while (0)

#define QDESC_GET_FLQ(q, type, label) do { \
	struct sge_fl *flq = (struct sge_fl *)q; \
	QDESC_GET(flq, flq->desc, type, label); \
} while (0)

	/* NIC TXQ */
	for (i = 0; i < s->ethqsets; i++)
		QDESC_GET_TXQ(&s->ethtxq[i].q, CUDBG_QTYPE_NIC_TXQ, out);

	/* NIC RXQ */
	for (i = 0; i < s->ethqsets; i++)
		QDESC_GET_RXQ(&s->ethrxq[i].rspq, CUDBG_QTYPE_NIC_RXQ, out);

	/* NIC FLQ */
	for (i = 0; i < s->ethqsets; i++)
		QDESC_GET_FLQ(&s->ethrxq[i].fl, CUDBG_QTYPE_NIC_FLQ, out);

	/* NIC CTRLQ */
	for (i = 0; i < padap->params.nports; i++)
		QDESC_GET_TXQ(&s->ctrlq[i].q, CUDBG_QTYPE_CTRLQ, out);

	/* FW_EVTQ */
	QDESC_GET_RXQ(&s->fw_evtq, CUDBG_QTYPE_FWEVTQ, out);

	/* INTRQ */
	QDESC_GET_RXQ(&s->intrq, CUDBG_QTYPE_INTRQ, out);

	/* PTP_TXQ */
	QDESC_GET_TXQ(&s->ptptxq.q, CUDBG_QTYPE_PTP_TXQ, out);

	/* ULD Queues */
	mutex_lock(&uld_mutex);

	if (s->uld_txq_info) {
		struct sge_uld_txq_info *utxq;

		/* ULD TXQ */
		for (j = 0; j < CXGB4_TX_MAX; j++) {
			if (!s->uld_txq_info[j])
				continue;

			utxq = s->uld_txq_info[j];
			for (i = 0; i < utxq->ntxq; i++)
				QDESC_GET_TXQ(&utxq->uldtxq[i].q,
					      cudbg_uld_txq_to_qtype(j),
					      out_unlock);
		}
	}

	if (s->uld_rxq_info) {
		struct sge_uld_rxq_info *urxq;
		u32 base;

		/* ULD RXQ */
		for (j = 0; j < CXGB4_ULD_MAX; j++) {
			if (!s->uld_rxq_info[j])
				continue;

			urxq = s->uld_rxq_info[j];
			for (i = 0; i < urxq->nrxq; i++)
				QDESC_GET_RXQ(&urxq->uldrxq[i].rspq,
					      cudbg_uld_rxq_to_qtype(j),
					      out_unlock);
		}

		/* ULD FLQ */
		for (j = 0; j < CXGB4_ULD_MAX; j++) {
			if (!s->uld_rxq_info[j])
				continue;

			urxq = s->uld_rxq_info[j];
			for (i = 0; i < urxq->nrxq; i++)
				QDESC_GET_FLQ(&urxq->uldrxq[i].fl,
					      cudbg_uld_flq_to_qtype(j),
					      out_unlock);
		}

		/* ULD CIQ */
		for (j = 0; j < CXGB4_ULD_MAX; j++) {
			if (!s->uld_rxq_info[j])
				continue;

			urxq = s->uld_rxq_info[j];
			base = urxq->nrxq;
			for (i = 0; i < urxq->nciq; i++)
				QDESC_GET_RXQ(&urxq->uldrxq[base + i].rspq,
					      cudbg_uld_ciq_to_qtype(j),
					      out_unlock);
		}
	}

	/* ETHOFLD TXQ */
	if (s->eohw_txq)
		for (i = 0; i < s->eoqsets; i++)
			QDESC_GET_TXQ(&s->eohw_txq[i].q,
				      CUDBG_QTYPE_ETHOFLD_TXQ, out);

	/* ETHOFLD RXQ and FLQ */
	if (s->eohw_rxq) {
		for (i = 0; i < s->eoqsets; i++)
			QDESC_GET_RXQ(&s->eohw_rxq[i].rspq,
				      CUDBG_QTYPE_ETHOFLD_RXQ, out);

		for (i = 0; i < s->eoqsets; i++)
			QDESC_GET_FLQ(&s->eohw_rxq[i].fl,
				      CUDBG_QTYPE_ETHOFLD_FLQ, out);
	}

out_unlock:
	mutex_unlock(&uld_mutex);

out:
	qdesc_info->qdesc_entry_size = sizeof(*qdesc_entry);
	qdesc_info->num_queues = num_queues;
	cur_off = 0;
	while (tot_len) {
		u32 chunk_size = min_t(u32, tot_len, CUDBG_CHUNK_SIZE);

		rc = cudbg_get_buff(pdbg_init, dbg_buff, chunk_size,
				    &temp_buff);
		if (rc) {
			cudbg_err->sys_warn = CUDBG_STATUS_PARTIAL_DATA;
			goto out_free;
		}

		memcpy(temp_buff.data, data + cur_off, chunk_size);
		tot_len -= chunk_size;
		cur_off += chunk_size;
		rc = cudbg_write_and_release_buff(pdbg_init, &temp_buff,
						  dbg_buff);
		if (rc) {
			cudbg_put_buff(pdbg_init, &temp_buff);
			cudbg_err->sys_warn = CUDBG_STATUS_PARTIAL_DATA;
			goto out_free;
		}
	}

out_free:
	kvfree(data);

#undef QDESC_GET_FLQ
#undef QDESC_GET_RXQ
#undef QDESC_GET_TXQ
#undef QDESC_GET

	return rc;
}

int cudbg_collect_flash(struct cudbg_init *pdbg_init,
			struct cudbg_buffer *dbg_buff,
			struct cudbg_error *cudbg_err)
{
	struct adapter *padap = pdbg_init->adap;
	u32 count = padap->params.sf_size, n;
	struct cudbg_buffer temp_buff = {0};
	u32 addr, i;
	int rc;

	addr = FLASH_EXP_ROM_START;

	for (i = 0; i < count; i += SF_PAGE_SIZE) {
		n = min_t(u32, count - i, SF_PAGE_SIZE);

		rc = cudbg_get_buff(pdbg_init, dbg_buff, n, &temp_buff);
		if (rc) {
			cudbg_err->sys_warn = CUDBG_STATUS_PARTIAL_DATA;
			goto out;
		}
		rc = t4_read_flash(padap, addr, n, (u32 *)temp_buff.data, 0);
		if (rc)
			goto out;

		addr += (n * 4);
		rc = cudbg_write_and_release_buff(pdbg_init, &temp_buff,
						  dbg_buff);
		if (rc) {
			cudbg_err->sys_warn = CUDBG_STATUS_PARTIAL_DATA;
			goto out;
		}
	}

out:
	return rc;
}
