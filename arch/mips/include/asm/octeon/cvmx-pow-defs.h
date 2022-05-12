/***********************license start***************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2012 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
 ***********************license end**************************************/

#ifndef __CVMX_POW_DEFS_H__
#define __CVMX_POW_DEFS_H__

#define CVMX_POW_BIST_STAT (CVMX_ADD_IO_SEG(0x00016700000003F8ull))
#define CVMX_POW_DS_PC (CVMX_ADD_IO_SEG(0x0001670000000398ull))
#define CVMX_POW_ECC_ERR (CVMX_ADD_IO_SEG(0x0001670000000218ull))
#define CVMX_POW_INT_CTL (CVMX_ADD_IO_SEG(0x0001670000000220ull))
#define CVMX_POW_IQ_CNTX(offset) (CVMX_ADD_IO_SEG(0x0001670000000340ull) + ((offset) & 7) * 8)
#define CVMX_POW_IQ_COM_CNT (CVMX_ADD_IO_SEG(0x0001670000000388ull))
#define CVMX_POW_IQ_INT (CVMX_ADD_IO_SEG(0x0001670000000238ull))
#define CVMX_POW_IQ_INT_EN (CVMX_ADD_IO_SEG(0x0001670000000240ull))
#define CVMX_POW_IQ_THRX(offset) (CVMX_ADD_IO_SEG(0x00016700000003A0ull) + ((offset) & 7) * 8)
#define CVMX_POW_NOS_CNT (CVMX_ADD_IO_SEG(0x0001670000000228ull))
#define CVMX_POW_NW_TIM (CVMX_ADD_IO_SEG(0x0001670000000210ull))
#define CVMX_POW_PF_RST_MSK (CVMX_ADD_IO_SEG(0x0001670000000230ull))
#define CVMX_POW_PP_GRP_MSKX(offset) (CVMX_ADD_IO_SEG(0x0001670000000000ull) + ((offset) & 15) * 8)
#define CVMX_POW_QOS_RNDX(offset) (CVMX_ADD_IO_SEG(0x00016700000001C0ull) + ((offset) & 7) * 8)
#define CVMX_POW_QOS_THRX(offset) (CVMX_ADD_IO_SEG(0x0001670000000180ull) + ((offset) & 7) * 8)
#define CVMX_POW_TS_PC (CVMX_ADD_IO_SEG(0x0001670000000390ull))
#define CVMX_POW_WA_COM_PC (CVMX_ADD_IO_SEG(0x0001670000000380ull))
#define CVMX_POW_WA_PCX(offset) (CVMX_ADD_IO_SEG(0x0001670000000300ull) + ((offset) & 7) * 8)
#define CVMX_POW_WQ_INT (CVMX_ADD_IO_SEG(0x0001670000000200ull))
#define CVMX_POW_WQ_INT_CNTX(offset) (CVMX_ADD_IO_SEG(0x0001670000000100ull) + ((offset) & 15) * 8)
#define CVMX_POW_WQ_INT_PC (CVMX_ADD_IO_SEG(0x0001670000000208ull))
#define CVMX_POW_WQ_INT_THRX(offset) (CVMX_ADD_IO_SEG(0x0001670000000080ull) + ((offset) & 15) * 8)
#define CVMX_POW_WS_PCX(offset) (CVMX_ADD_IO_SEG(0x0001670000000280ull) + ((offset) & 15) * 8)

#define CVMX_SSO_WQ_INT (CVMX_ADD_IO_SEG(0x0001670000001000ull))
#define CVMX_SSO_WQ_IQ_DIS (CVMX_ADD_IO_SEG(0x0001670000001010ull))
#define CVMX_SSO_WQ_INT_PC (CVMX_ADD_IO_SEG(0x0001670000001020ull))
#define CVMX_SSO_PPX_GRP_MSK(offset) (CVMX_ADD_IO_SEG(0x0001670000006000ull) + ((offset) & 31) * 8)
#define CVMX_SSO_WQ_INT_THRX(offset) (CVMX_ADD_IO_SEG(0x0001670000007000ull) + ((offset) & 63) * 8)

union cvmx_pow_bist_stat {
	uint64_t u64;
	struct cvmx_pow_bist_stat_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t pp:16;
		uint64_t reserved_0_15:16;
#else
		uint64_t reserved_0_15:16;
		uint64_t pp:16;
		uint64_t reserved_32_63:32;
#endif
	} s;
	struct cvmx_pow_bist_stat_cn30xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_17_63:47;
		uint64_t pp:1;
		uint64_t reserved_9_15:7;
		uint64_t cam:1;
		uint64_t nbt1:1;
		uint64_t nbt0:1;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t nbr1:1;
		uint64_t nbr0:1;
		uint64_t pend:1;
		uint64_t adr:1;
#else
		uint64_t adr:1;
		uint64_t pend:1;
		uint64_t nbr0:1;
		uint64_t nbr1:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt0:1;
		uint64_t nbt1:1;
		uint64_t cam:1;
		uint64_t reserved_9_15:7;
		uint64_t pp:1;
		uint64_t reserved_17_63:47;
#endif
	} cn30xx;
	struct cvmx_pow_bist_stat_cn31xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_18_63:46;
		uint64_t pp:2;
		uint64_t reserved_9_15:7;
		uint64_t cam:1;
		uint64_t nbt1:1;
		uint64_t nbt0:1;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t nbr1:1;
		uint64_t nbr0:1;
		uint64_t pend:1;
		uint64_t adr:1;
#else
		uint64_t adr:1;
		uint64_t pend:1;
		uint64_t nbr0:1;
		uint64_t nbr1:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt0:1;
		uint64_t nbt1:1;
		uint64_t cam:1;
		uint64_t reserved_9_15:7;
		uint64_t pp:2;
		uint64_t reserved_18_63:46;
#endif
	} cn31xx;
	struct cvmx_pow_bist_stat_cn38xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t pp:16;
		uint64_t reserved_10_15:6;
		uint64_t cam:1;
		uint64_t nbt:1;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t nbr1:1;
		uint64_t nbr0:1;
		uint64_t pend1:1;
		uint64_t pend0:1;
		uint64_t adr1:1;
		uint64_t adr0:1;
#else
		uint64_t adr0:1;
		uint64_t adr1:1;
		uint64_t pend0:1;
		uint64_t pend1:1;
		uint64_t nbr0:1;
		uint64_t nbr1:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt:1;
		uint64_t cam:1;
		uint64_t reserved_10_15:6;
		uint64_t pp:16;
		uint64_t reserved_32_63:32;
#endif
	} cn38xx;
	struct cvmx_pow_bist_stat_cn52xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_20_63:44;
		uint64_t pp:4;
		uint64_t reserved_9_15:7;
		uint64_t cam:1;
		uint64_t nbt1:1;
		uint64_t nbt0:1;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t nbr1:1;
		uint64_t nbr0:1;
		uint64_t pend:1;
		uint64_t adr:1;
#else
		uint64_t adr:1;
		uint64_t pend:1;
		uint64_t nbr0:1;
		uint64_t nbr1:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt0:1;
		uint64_t nbt1:1;
		uint64_t cam:1;
		uint64_t reserved_9_15:7;
		uint64_t pp:4;
		uint64_t reserved_20_63:44;
#endif
	} cn52xx;
	struct cvmx_pow_bist_stat_cn56xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_28_63:36;
		uint64_t pp:12;
		uint64_t reserved_10_15:6;
		uint64_t cam:1;
		uint64_t nbt:1;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t nbr1:1;
		uint64_t nbr0:1;
		uint64_t pend1:1;
		uint64_t pend0:1;
		uint64_t adr1:1;
		uint64_t adr0:1;
#else
		uint64_t adr0:1;
		uint64_t adr1:1;
		uint64_t pend0:1;
		uint64_t pend1:1;
		uint64_t nbr0:1;
		uint64_t nbr1:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt:1;
		uint64_t cam:1;
		uint64_t reserved_10_15:6;
		uint64_t pp:12;
		uint64_t reserved_28_63:36;
#endif
	} cn56xx;
	struct cvmx_pow_bist_stat_cn61xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_20_63:44;
		uint64_t pp:4;
		uint64_t reserved_12_15:4;
		uint64_t cam:1;
		uint64_t nbr:3;
		uint64_t nbt:4;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t pend:1;
		uint64_t adr:1;
#else
		uint64_t adr:1;
		uint64_t pend:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt:4;
		uint64_t nbr:3;
		uint64_t cam:1;
		uint64_t reserved_12_15:4;
		uint64_t pp:4;
		uint64_t reserved_20_63:44;
#endif
	} cn61xx;
	struct cvmx_pow_bist_stat_cn63xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_22_63:42;
		uint64_t pp:6;
		uint64_t reserved_12_15:4;
		uint64_t cam:1;
		uint64_t nbr:3;
		uint64_t nbt:4;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t pend:1;
		uint64_t adr:1;
#else
		uint64_t adr:1;
		uint64_t pend:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt:4;
		uint64_t nbr:3;
		uint64_t cam:1;
		uint64_t reserved_12_15:4;
		uint64_t pp:6;
		uint64_t reserved_22_63:42;
#endif
	} cn63xx;
	struct cvmx_pow_bist_stat_cn66xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_26_63:38;
		uint64_t pp:10;
		uint64_t reserved_12_15:4;
		uint64_t cam:1;
		uint64_t nbr:3;
		uint64_t nbt:4;
		uint64_t index:1;
		uint64_t fidx:1;
		uint64_t pend:1;
		uint64_t adr:1;
#else
		uint64_t adr:1;
		uint64_t pend:1;
		uint64_t fidx:1;
		uint64_t index:1;
		uint64_t nbt:4;
		uint64_t nbr:3;
		uint64_t cam:1;
		uint64_t reserved_12_15:4;
		uint64_t pp:10;
		uint64_t reserved_26_63:38;
#endif
	} cn66xx;
};

union cvmx_pow_ds_pc {
	uint64_t u64;
	struct cvmx_pow_ds_pc_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t ds_pc:32;
#else
		uint64_t ds_pc:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_ecc_err {
	uint64_t u64;
	struct cvmx_pow_ecc_err_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_45_63:19;
		uint64_t iop_ie:13;
		uint64_t reserved_29_31:3;
		uint64_t iop:13;
		uint64_t reserved_14_15:2;
		uint64_t rpe_ie:1;
		uint64_t rpe:1;
		uint64_t reserved_9_11:3;
		uint64_t syn:5;
		uint64_t dbe_ie:1;
		uint64_t sbe_ie:1;
		uint64_t dbe:1;
		uint64_t sbe:1;
#else
		uint64_t sbe:1;
		uint64_t dbe:1;
		uint64_t sbe_ie:1;
		uint64_t dbe_ie:1;
		uint64_t syn:5;
		uint64_t reserved_9_11:3;
		uint64_t rpe:1;
		uint64_t rpe_ie:1;
		uint64_t reserved_14_15:2;
		uint64_t iop:13;
		uint64_t reserved_29_31:3;
		uint64_t iop_ie:13;
		uint64_t reserved_45_63:19;
#endif
	} s;
	struct cvmx_pow_ecc_err_cn31xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_14_63:50;
		uint64_t rpe_ie:1;
		uint64_t rpe:1;
		uint64_t reserved_9_11:3;
		uint64_t syn:5;
		uint64_t dbe_ie:1;
		uint64_t sbe_ie:1;
		uint64_t dbe:1;
		uint64_t sbe:1;
#else
		uint64_t sbe:1;
		uint64_t dbe:1;
		uint64_t sbe_ie:1;
		uint64_t dbe_ie:1;
		uint64_t syn:5;
		uint64_t reserved_9_11:3;
		uint64_t rpe:1;
		uint64_t rpe_ie:1;
		uint64_t reserved_14_63:50;
#endif
	} cn31xx;
};

union cvmx_pow_int_ctl {
	uint64_t u64;
	struct cvmx_pow_int_ctl_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_6_63:58;
		uint64_t pfr_dis:1;
		uint64_t nbr_thr:5;
#else
		uint64_t nbr_thr:5;
		uint64_t pfr_dis:1;
		uint64_t reserved_6_63:58;
#endif
	} s;
};

union cvmx_pow_iq_cntx {
	uint64_t u64;
	struct cvmx_pow_iq_cntx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t iq_cnt:32;
#else
		uint64_t iq_cnt:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_iq_com_cnt {
	uint64_t u64;
	struct cvmx_pow_iq_com_cnt_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t iq_cnt:32;
#else
		uint64_t iq_cnt:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_iq_int {
	uint64_t u64;
	struct cvmx_pow_iq_int_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_8_63:56;
		uint64_t iq_int:8;
#else
		uint64_t iq_int:8;
		uint64_t reserved_8_63:56;
#endif
	} s;
};

union cvmx_pow_iq_int_en {
	uint64_t u64;
	struct cvmx_pow_iq_int_en_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_8_63:56;
		uint64_t int_en:8;
#else
		uint64_t int_en:8;
		uint64_t reserved_8_63:56;
#endif
	} s;
};

union cvmx_pow_iq_thrx {
	uint64_t u64;
	struct cvmx_pow_iq_thrx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t iq_thr:32;
#else
		uint64_t iq_thr:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_nos_cnt {
	uint64_t u64;
	struct cvmx_pow_nos_cnt_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_12_63:52;
		uint64_t nos_cnt:12;
#else
		uint64_t nos_cnt:12;
		uint64_t reserved_12_63:52;
#endif
	} s;
	struct cvmx_pow_nos_cnt_cn30xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_7_63:57;
		uint64_t nos_cnt:7;
#else
		uint64_t nos_cnt:7;
		uint64_t reserved_7_63:57;
#endif
	} cn30xx;
	struct cvmx_pow_nos_cnt_cn31xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_9_63:55;
		uint64_t nos_cnt:9;
#else
		uint64_t nos_cnt:9;
		uint64_t reserved_9_63:55;
#endif
	} cn31xx;
	struct cvmx_pow_nos_cnt_cn52xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_10_63:54;
		uint64_t nos_cnt:10;
#else
		uint64_t nos_cnt:10;
		uint64_t reserved_10_63:54;
#endif
	} cn52xx;
	struct cvmx_pow_nos_cnt_cn63xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_11_63:53;
		uint64_t nos_cnt:11;
#else
		uint64_t nos_cnt:11;
		uint64_t reserved_11_63:53;
#endif
	} cn63xx;
};

union cvmx_pow_nw_tim {
	uint64_t u64;
	struct cvmx_pow_nw_tim_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_10_63:54;
		uint64_t nw_tim:10;
#else
		uint64_t nw_tim:10;
		uint64_t reserved_10_63:54;
#endif
	} s;
};

union cvmx_pow_pf_rst_msk {
	uint64_t u64;
	struct cvmx_pow_pf_rst_msk_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_8_63:56;
		uint64_t rst_msk:8;
#else
		uint64_t rst_msk:8;
		uint64_t reserved_8_63:56;
#endif
	} s;
};

union cvmx_pow_pp_grp_mskx {
	uint64_t u64;
	struct cvmx_pow_pp_grp_mskx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_48_63:16;
		uint64_t qos7_pri:4;
		uint64_t qos6_pri:4;
		uint64_t qos5_pri:4;
		uint64_t qos4_pri:4;
		uint64_t qos3_pri:4;
		uint64_t qos2_pri:4;
		uint64_t qos1_pri:4;
		uint64_t qos0_pri:4;
		uint64_t grp_msk:16;
#else
		uint64_t grp_msk:16;
		uint64_t qos0_pri:4;
		uint64_t qos1_pri:4;
		uint64_t qos2_pri:4;
		uint64_t qos3_pri:4;
		uint64_t qos4_pri:4;
		uint64_t qos5_pri:4;
		uint64_t qos6_pri:4;
		uint64_t qos7_pri:4;
		uint64_t reserved_48_63:16;
#endif
	} s;
	struct cvmx_pow_pp_grp_mskx_cn30xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_16_63:48;
		uint64_t grp_msk:16;
#else
		uint64_t grp_msk:16;
		uint64_t reserved_16_63:48;
#endif
	} cn30xx;
};

union cvmx_pow_qos_rndx {
	uint64_t u64;
	struct cvmx_pow_qos_rndx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t rnd_p3:8;
		uint64_t rnd_p2:8;
		uint64_t rnd_p1:8;
		uint64_t rnd:8;
#else
		uint64_t rnd:8;
		uint64_t rnd_p1:8;
		uint64_t rnd_p2:8;
		uint64_t rnd_p3:8;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_qos_thrx {
	uint64_t u64;
	struct cvmx_pow_qos_thrx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_60_63:4;
		uint64_t des_cnt:12;
		uint64_t buf_cnt:12;
		uint64_t free_cnt:12;
		uint64_t reserved_23_23:1;
		uint64_t max_thr:11;
		uint64_t reserved_11_11:1;
		uint64_t min_thr:11;
#else
		uint64_t min_thr:11;
		uint64_t reserved_11_11:1;
		uint64_t max_thr:11;
		uint64_t reserved_23_23:1;
		uint64_t free_cnt:12;
		uint64_t buf_cnt:12;
		uint64_t des_cnt:12;
		uint64_t reserved_60_63:4;
#endif
	} s;
	struct cvmx_pow_qos_thrx_cn30xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_55_63:9;
		uint64_t des_cnt:7;
		uint64_t reserved_43_47:5;
		uint64_t buf_cnt:7;
		uint64_t reserved_31_35:5;
		uint64_t free_cnt:7;
		uint64_t reserved_18_23:6;
		uint64_t max_thr:6;
		uint64_t reserved_6_11:6;
		uint64_t min_thr:6;
#else
		uint64_t min_thr:6;
		uint64_t reserved_6_11:6;
		uint64_t max_thr:6;
		uint64_t reserved_18_23:6;
		uint64_t free_cnt:7;
		uint64_t reserved_31_35:5;
		uint64_t buf_cnt:7;
		uint64_t reserved_43_47:5;
		uint64_t des_cnt:7;
		uint64_t reserved_55_63:9;
#endif
	} cn30xx;
	struct cvmx_pow_qos_thrx_cn31xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_57_63:7;
		uint64_t des_cnt:9;
		uint64_t reserved_45_47:3;
		uint64_t buf_cnt:9;
		uint64_t reserved_33_35:3;
		uint64_t free_cnt:9;
		uint64_t reserved_20_23:4;
		uint64_t max_thr:8;
		uint64_t reserved_8_11:4;
		uint64_t min_thr:8;
#else
		uint64_t min_thr:8;
		uint64_t reserved_8_11:4;
		uint64_t max_thr:8;
		uint64_t reserved_20_23:4;
		uint64_t free_cnt:9;
		uint64_t reserved_33_35:3;
		uint64_t buf_cnt:9;
		uint64_t reserved_45_47:3;
		uint64_t des_cnt:9;
		uint64_t reserved_57_63:7;
#endif
	} cn31xx;
	struct cvmx_pow_qos_thrx_cn52xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_58_63:6;
		uint64_t des_cnt:10;
		uint64_t reserved_46_47:2;
		uint64_t buf_cnt:10;
		uint64_t reserved_34_35:2;
		uint64_t free_cnt:10;
		uint64_t reserved_21_23:3;
		uint64_t max_thr:9;
		uint64_t reserved_9_11:3;
		uint64_t min_thr:9;
#else
		uint64_t min_thr:9;
		uint64_t reserved_9_11:3;
		uint64_t max_thr:9;
		uint64_t reserved_21_23:3;
		uint64_t free_cnt:10;
		uint64_t reserved_34_35:2;
		uint64_t buf_cnt:10;
		uint64_t reserved_46_47:2;
		uint64_t des_cnt:10;
		uint64_t reserved_58_63:6;
#endif
	} cn52xx;
	struct cvmx_pow_qos_thrx_cn63xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_59_63:5;
		uint64_t des_cnt:11;
		uint64_t reserved_47_47:1;
		uint64_t buf_cnt:11;
		uint64_t reserved_35_35:1;
		uint64_t free_cnt:11;
		uint64_t reserved_22_23:2;
		uint64_t max_thr:10;
		uint64_t reserved_10_11:2;
		uint64_t min_thr:10;
#else
		uint64_t min_thr:10;
		uint64_t reserved_10_11:2;
		uint64_t max_thr:10;
		uint64_t reserved_22_23:2;
		uint64_t free_cnt:11;
		uint64_t reserved_35_35:1;
		uint64_t buf_cnt:11;
		uint64_t reserved_47_47:1;
		uint64_t des_cnt:11;
		uint64_t reserved_59_63:5;
#endif
	} cn63xx;
};

union cvmx_pow_ts_pc {
	uint64_t u64;
	struct cvmx_pow_ts_pc_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t ts_pc:32;
#else
		uint64_t ts_pc:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_wa_com_pc {
	uint64_t u64;
	struct cvmx_pow_wa_com_pc_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t wa_pc:32;
#else
		uint64_t wa_pc:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_wa_pcx {
	uint64_t u64;
	struct cvmx_pow_wa_pcx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t wa_pc:32;
#else
		uint64_t wa_pc:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_wq_int {
	uint64_t u64;
	struct cvmx_pow_wq_int_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t iq_dis:16;
		uint64_t wq_int:16;
#else
		uint64_t wq_int:16;
		uint64_t iq_dis:16;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_pow_wq_int_cntx {
	uint64_t u64;
	struct cvmx_pow_wq_int_cntx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_28_63:36;
		uint64_t tc_cnt:4;
		uint64_t ds_cnt:12;
		uint64_t iq_cnt:12;
#else
		uint64_t iq_cnt:12;
		uint64_t ds_cnt:12;
		uint64_t tc_cnt:4;
		uint64_t reserved_28_63:36;
#endif
	} s;
	struct cvmx_pow_wq_int_cntx_cn30xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_28_63:36;
		uint64_t tc_cnt:4;
		uint64_t reserved_19_23:5;
		uint64_t ds_cnt:7;
		uint64_t reserved_7_11:5;
		uint64_t iq_cnt:7;
#else
		uint64_t iq_cnt:7;
		uint64_t reserved_7_11:5;
		uint64_t ds_cnt:7;
		uint64_t reserved_19_23:5;
		uint64_t tc_cnt:4;
		uint64_t reserved_28_63:36;
#endif
	} cn30xx;
	struct cvmx_pow_wq_int_cntx_cn31xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_28_63:36;
		uint64_t tc_cnt:4;
		uint64_t reserved_21_23:3;
		uint64_t ds_cnt:9;
		uint64_t reserved_9_11:3;
		uint64_t iq_cnt:9;
#else
		uint64_t iq_cnt:9;
		uint64_t reserved_9_11:3;
		uint64_t ds_cnt:9;
		uint64_t reserved_21_23:3;
		uint64_t tc_cnt:4;
		uint64_t reserved_28_63:36;
#endif
	} cn31xx;
	struct cvmx_pow_wq_int_cntx_cn52xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_28_63:36;
		uint64_t tc_cnt:4;
		uint64_t reserved_22_23:2;
		uint64_t ds_cnt:10;
		uint64_t reserved_10_11:2;
		uint64_t iq_cnt:10;
#else
		uint64_t iq_cnt:10;
		uint64_t reserved_10_11:2;
		uint64_t ds_cnt:10;
		uint64_t reserved_22_23:2;
		uint64_t tc_cnt:4;
		uint64_t reserved_28_63:36;
#endif
	} cn52xx;
	struct cvmx_pow_wq_int_cntx_cn63xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_28_63:36;
		uint64_t tc_cnt:4;
		uint64_t reserved_23_23:1;
		uint64_t ds_cnt:11;
		uint64_t reserved_11_11:1;
		uint64_t iq_cnt:11;
#else
		uint64_t iq_cnt:11;
		uint64_t reserved_11_11:1;
		uint64_t ds_cnt:11;
		uint64_t reserved_23_23:1;
		uint64_t tc_cnt:4;
		uint64_t reserved_28_63:36;
#endif
	} cn63xx;
};

union cvmx_pow_wq_int_pc {
	uint64_t u64;
	struct cvmx_pow_wq_int_pc_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_60_63:4;
		uint64_t pc:28;
		uint64_t reserved_28_31:4;
		uint64_t pc_thr:20;
		uint64_t reserved_0_7:8;
#else
		uint64_t reserved_0_7:8;
		uint64_t pc_thr:20;
		uint64_t reserved_28_31:4;
		uint64_t pc:28;
		uint64_t reserved_60_63:4;
#endif
	} s;
};

union cvmx_pow_wq_int_thrx {
	uint64_t u64;
	struct cvmx_pow_wq_int_thrx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_29_63:35;
		uint64_t tc_en:1;
		uint64_t tc_thr:4;
		uint64_t reserved_23_23:1;
		uint64_t ds_thr:11;
		uint64_t reserved_11_11:1;
		uint64_t iq_thr:11;
#else
		uint64_t iq_thr:11;
		uint64_t reserved_11_11:1;
		uint64_t ds_thr:11;
		uint64_t reserved_23_23:1;
		uint64_t tc_thr:4;
		uint64_t tc_en:1;
		uint64_t reserved_29_63:35;
#endif
	} s;
	struct cvmx_pow_wq_int_thrx_cn30xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_29_63:35;
		uint64_t tc_en:1;
		uint64_t tc_thr:4;
		uint64_t reserved_18_23:6;
		uint64_t ds_thr:6;
		uint64_t reserved_6_11:6;
		uint64_t iq_thr:6;
#else
		uint64_t iq_thr:6;
		uint64_t reserved_6_11:6;
		uint64_t ds_thr:6;
		uint64_t reserved_18_23:6;
		uint64_t tc_thr:4;
		uint64_t tc_en:1;
		uint64_t reserved_29_63:35;
#endif
	} cn30xx;
	struct cvmx_pow_wq_int_thrx_cn31xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_29_63:35;
		uint64_t tc_en:1;
		uint64_t tc_thr:4;
		uint64_t reserved_20_23:4;
		uint64_t ds_thr:8;
		uint64_t reserved_8_11:4;
		uint64_t iq_thr:8;
#else
		uint64_t iq_thr:8;
		uint64_t reserved_8_11:4;
		uint64_t ds_thr:8;
		uint64_t reserved_20_23:4;
		uint64_t tc_thr:4;
		uint64_t tc_en:1;
		uint64_t reserved_29_63:35;
#endif
	} cn31xx;
	struct cvmx_pow_wq_int_thrx_cn52xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_29_63:35;
		uint64_t tc_en:1;
		uint64_t tc_thr:4;
		uint64_t reserved_21_23:3;
		uint64_t ds_thr:9;
		uint64_t reserved_9_11:3;
		uint64_t iq_thr:9;
#else
		uint64_t iq_thr:9;
		uint64_t reserved_9_11:3;
		uint64_t ds_thr:9;
		uint64_t reserved_21_23:3;
		uint64_t tc_thr:4;
		uint64_t tc_en:1;
		uint64_t reserved_29_63:35;
#endif
	} cn52xx;
	struct cvmx_pow_wq_int_thrx_cn63xx {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_29_63:35;
		uint64_t tc_en:1;
		uint64_t tc_thr:4;
		uint64_t reserved_22_23:2;
		uint64_t ds_thr:10;
		uint64_t reserved_10_11:2;
		uint64_t iq_thr:10;
#else
		uint64_t iq_thr:10;
		uint64_t reserved_10_11:2;
		uint64_t ds_thr:10;
		uint64_t reserved_22_23:2;
		uint64_t tc_thr:4;
		uint64_t tc_en:1;
		uint64_t reserved_29_63:35;
#endif
	} cn63xx;
};

union cvmx_pow_ws_pcx {
	uint64_t u64;
	struct cvmx_pow_ws_pcx_s {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_32_63:32;
		uint64_t ws_pc:32;
#else
		uint64_t ws_pc:32;
		uint64_t reserved_32_63:32;
#endif
	} s;
};

union cvmx_sso_wq_int_thrx {
	uint64_t u64;
	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		uint64_t reserved_33_63:31;
		uint64_t tc_en:1;
		uint64_t tc_thr:4;
		uint64_t reserved_26_27:2;
		uint64_t ds_thr:12;
		uint64_t reserved_12_13:2;
		uint64_t iq_thr:12;
#else
		uint64_t iq_thr:12;
		uint64_t reserved_12_13:2;
		uint64_t ds_thr:12;
		uint64_t reserved_26_27:2;
		uint64_t tc_thr:4;
		uint64_t tc_en:1;
		uint64_t reserved_33_63:31;
#endif
	} s;
};

#endif
