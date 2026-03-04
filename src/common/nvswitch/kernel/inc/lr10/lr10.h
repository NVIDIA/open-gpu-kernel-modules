/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _LR10_H_
#define _LR10_H_

#include "nvlink.h"
#include "nvCpuUuid.h"

#include "export_nvswitch.h"
#include "common_nvswitch.h"
#include "pmgr_nvswitch.h"
#include "rom_nvswitch.h"
#include "error_nvswitch.h"

#include "ctrl_dev_nvswitch.h"

#include "nvswitch/lr10/dev_nvs_master.h"

//
// Re-direction to use new common link access wrappers
//

#define NVSWITCH_IS_LINK_ENG_VALID_LR10(_d, _eng, _linknum)  \
    NVSWITCH_IS_LINK_ENG_VALID(_d, _linknum, _eng)

#define NVSWITCH_LINK_OFFSET_LR10(_d, _physlinknum, _eng, _dev, _reg) \
    NVSWITCH_LINK_OFFSET(_d, _physlinknum, _eng, _dev, _reg)

#define NVSWITCH_LINK_WR32_LR10(_d, _physlinknum, _eng, _dev, _reg, _data)  \
    NVSWITCH_LINK_WR32(_d, _physlinknum, _eng, _dev, _reg, _data)

#define NVSWITCH_LINK_RD32_LR10(_d, _physlinknum, _eng, _dev, _reg) \
    NVSWITCH_LINK_RD32(_d, _physlinknum, _eng, _dev, _reg)

#define NVSWITCH_LINK_WR32_IDX_LR10(_d, _physlinknum, _eng, _dev, _reg, _idx, _data)  \
    NVSWITCH_LINK_WR32_IDX(_d, _physlinknum, _eng, _dev, _reg, _idx, _data)

#define NVSWITCH_LINK_RD32_IDX_LR10(_d, _physlinknum, _eng, _dev, _reg, _idx) \
    NVSWITCH_LINK_RD32_IDX(_d, _physlinknum, _eng, _dev, _reg, _idx)

//
// NVSWITCH_ENG_* MMIO wrappers are to be used for top level discovered
// devices like SAW, FUSE, PMGR, XVE, etc.
//

#define NVSWITCH_ENG_WR32_LR10(_d, _eng, _bcast, _engidx, _dev, _reg, _data) \
    NVSWITCH_ENG_WR32(_d, _eng, _bcast, _engidx, _dev, _reg, _data)

#define NVSWITCH_ENG_RD32_LR10(_d, _eng, _engidx, _dev, _reg)           \
    NVSWITCH_ENG_RD32(_d, _eng, , _engidx, _dev, _reg)

#define NVSWITCH_ENG_WR32_IDX_LR10(_d, _eng, _bcast, _engidx, _dev, _reg, _idx, _data) \
    NVSWITCH_ENG_WR32_IDX(_d, _eng, _bcast, _engidx, _dev, _reg, _idx, _data)

#define NVSWITCH_BCAST_WR32_LR10(_d, _eng, _dev, _reg, _data)           \
    NVSWITCH_ENG_WR32_LR10(_d, _eng, _BCAST, 0, _dev, _reg, _data)

#define NVSWITCH_BCAST_RD32_LR10(_d, _eng, _dev, _reg)           \
    NVSWITCH_ENG_RD32(_d, _eng, _BCAST, 0, bc, _dev, _reg)

#define NVSWITCH_CLK_NVLINK_RD32_LR10(_d, _reg, _idx)                   \
    NVSWITCH_REG_RD32(_d, _PCLOCK, _NVSW_NVLINK##_reg(_idx))

#define NVSWITCH_CLK_NVLINK_WR32_LR10(_d, _reg, _idx, _data)            \
    if (IS_RTLSIM(_d) || IS_FMODEL(_d))                                 \
    {                                                                   \
        NVSWITCH_PRINT(_d, MMIO,                                        \
        "%s: Skip write NV_PCLOCK_NVSW_NVLINK%d %s (0x%06x) on FSF\n",  \
            __FUNCTION__,                                               \
            _idx, #_reg,                                                \
            NV_PCLOCK_NVSW_NVLINK##_reg(_idx));                         \
    }                                                                   \
    else                                                                \
    {                                                                   \
        NVSWITCH_REG_WR32(_d, _PCLOCK, _NVSW_NVLINK##_reg(_idx), _data);     \
    }

#define NVSWITCH_ENG_VALID_LR10(_d, _eng, _engidx)                      \
    (                                                                   \
        ((_engidx < NUM_##_eng##_ENGINE_LR10) &&                        \
        (NVSWITCH_GET_CHIP_DEVICE_LR10(_d)->eng##_eng[_engidx].valid)) ? \
        NV_TRUE : NV_FALSE                                              \
    )

#define NVSWITCH_SAW_RD32_LR10(_d, _dev, _reg)                          \
    NVSWITCH_ENG_RD32_LR10(_d, SAW, 0, _dev, _reg)

#define NVSWITCH_SAW_WR32_LR10(_d, _dev, _reg, _data)                   \
    NVSWITCH_ENG_WR32_LR10(_d, SAW, , 0, _dev, _reg, _data)

#define NVSWITCH_NPG_RD32_LR10(_d, _engidx, _dev, _reg)                 \
    NVSWITCH_ENG_RD32_LR10(_d, NPG, _engidx, _dev, _reg)

#define NVSWITCH_NPG_WR32_LR10(_d, _engidx, _dev, _reg, _data)          \
    NVSWITCH_ENG_WR32_LR10(_d, NPG, , _engidx, _dev, _reg, _data)

#define NVSWITCH_NPGPERF_WR32_LR10(_d, _engidx, _dev, _reg, _data)      \
    NVSWITCH_ENG_WR32_LR10(_d, NPG_PERFMON, , _engidx, _dev, _reg, _data)

#define NVSWITCH_NPORT_RD32_LR10(_d, _engidx, _dev, _reg)               \
    NVSWITCH_ENG_RD32_LR10(_d, NPORT, _engidx, _dev, _reg)

#define NVSWITCH_NPORT_WR32_LR10(_d, _engidx, _dev, _reg, _data)        \
    NVSWITCH_ENG_WR32_LR10(_d, NPORT, , _engidx, _dev, _reg, _data)

#define NVSWITCH_NPORT_MC_BCAST_WR32_LR10(_d, _dev, _reg, _data)        \
    NVSWITCH_BCAST_WR32_LR10(_d, NPORT, _dev, _reg, _data)

#define NVSWITCH_NVLIPT_RD32_LR10(_d, _engidx, _dev, _reg)                 \
    NVSWITCH_ENG_RD32_LR10(_d, NVLIPT, _engidx, _dev, _reg)

#define NVSWITCH_NVLIPT_WR32_LR10(_d, _engidx, _dev, _reg, _data)          \
    NVSWITCH_ENG_WR32_LR10(_d, NVLIPT, , _engidx, _dev, _reg, _data)

typedef struct
{
    NvBool valid;
    NvU32 initialized;
    NvU32 version;
    NvU32 disc_type;
    union
    {
        struct
        {
            NvU32 cluster;
            NvU32 cluster_id;
            NvU32 discovery;                // Used for top level only
        } top;
        struct
        {
            NvU32 uc_addr;
        } uc;
        struct
        {
            NvU32 bc_addr;
            NvU32 mc_addr[3];
        } bc;
    } info;
} ENGINE_DESCRIPTOR_TYPE_LR10;

#define NUM_PTOP_ENGINE_LR10                    1
#define NUM_CLKS_ENGINE_LR10                    1
#define NUM_FUSE_ENGINE_LR10                    1
#define NUM_JTAG_ENGINE_LR10                    1
#define NUM_PMGR_ENGINE_LR10                    1
#define NUM_SAW_ENGINE_LR10                     1
#define NUM_XP3G_ENGINE_LR10                    1
#define NUM_XVE_ENGINE_LR10                     1
#define NUM_ROM_ENGINE_LR10                     1
#define NUM_EXTDEV_ENGINE_LR10                  1
#define NUM_PRIVMAIN_ENGINE_LR10                1
#define NUM_PRIVLOC_ENGINE_LR10                 10
#define NUM_PTIMER_ENGINE_LR10                  1
#define NUM_SOE_ENGINE_LR10                     1
#define NUM_SMR_ENGINE_LR10                     2
#define NUM_I2C_ENGINE_LR10                     1
#define NUM_SE_ENGINE_LR10                      1
#define NUM_THERM_ENGINE_LR10                   1

#define NUM_NPG_ENGINE_LR10                     9
#define NUM_NPG_BCAST_ENGINE_LR10               1
#define NUM_NPG_PERFMON_ENGINE_LR10             9
#define NUM_NPG_PERFMON_BCAST_ENGINE_LR10       1
#define NUM_NPORT_ENGINE_LR10                   36
#define NUM_NPORT_BCAST_ENGINE_LR10             4
#define NUM_NPORT_MULTICAST_ENGINE_LR10         9
#define NUM_NPORT_MULTICAST_BCAST_ENGINE_LR10   1
#define NUM_NPORT_PERFMON_ENGINE_LR10           36
#define NUM_NPORT_PERFMON_BCAST_ENGINE_LR10     4
#define NUM_NPORT_PERFMON_MULTICAST_ENGINE_LR10 9
#define NUM_NPORT_PERFMON_MULTICAST_BCAST_ENGINE_LR10 1

#define NUM_NXBAR_ENGINE_LR10                   4
#define NUM_NXBAR_BCAST_ENGINE_LR10             1
#define NUM_NXBAR_PERFMON_ENGINE_LR10           4
#define NUM_NXBAR_PERFMON_BCAST_ENGINE_LR10     1
#define NUM_TILE_ENGINE_LR10                    16
#define NUM_TILE_BCAST_ENGINE_LR10              4
#define NUM_TILE_MULTICAST_ENGINE_LR10          4
#define NUM_TILE_MULTICAST_BCAST_ENGINE_LR10    1
#define NUM_TILE_PERFMON_ENGINE_LR10            16
#define NUM_TILE_PERFMON_BCAST_ENGINE_LR10      4
#define NUM_TILE_PERFMON_MULTICAST_ENGINE_LR10  4
#define NUM_TILE_PERFMON_MULTICAST_BCAST_ENGINE_LR10 1

//
// Tile Column consists of 4 Tile blocks and 9 Tileout blocks.
// There are 4 Tile Columns, one per each NXBAR.

#define NUM_NXBAR_TCS_LR10   NUM_NXBAR_ENGINE_LR10
#define NUM_NXBAR_TILEOUTS_PER_TC_LR10      9
#define NUM_NXBAR_TILES_PER_TC_LR10         4

#define TILE_TO_LINK(_device, _tc, _tile)                     \
    (                                                         \
        NVSWITCH_ASSERT((_tc < NUM_NXBAR_TCS_LR10))        \
    ,                                                         \
        NVSWITCH_ASSERT((_tile < NUM_NXBAR_TILES_PER_TC_LR10))  \
    ,                                                         \
        ((_tc) *  NUM_NXBAR_TILES_PER_TC_LR10 + (_tile))      \
    )

#define NV_NXBAR_TC_TILEOUT_ERR_FATAL_INTR_EN(i)  (NV_NXBAR_TC_TILEOUT0_ERR_FATAL_INTR_EN +  \
    i * (NV_NXBAR_TC_TILEOUT1_ERR_FATAL_INTR_EN - NV_NXBAR_TC_TILEOUT0_ERR_FATAL_INTR_EN))

#define  NV_NXBAR_TC_TILEOUT_ERR_STATUS(i)  (NV_NXBAR_TC_TILEOUT0_ERR_STATUS +  \
    i * (NV_NXBAR_TC_TILEOUT1_ERR_STATUS - NV_NXBAR_TC_TILEOUT0_ERR_STATUS))

#define NV_NXBAR_TC_TILEOUT_ERR_FIRST(i)  (NV_NXBAR_TC_TILEOUT0_ERR_FIRST +  \
    i * (NV_NXBAR_TC_TILEOUT1_ERR_FIRST - NV_NXBAR_TC_TILEOUT0_ERR_FIRST))

#define NV_NXBAR_TC_TILEOUT_ERR_CYA(i)  (NV_NXBAR_TC_TILEOUT0_ERR_CYA +  \
    i * (NV_NXBAR_TC_TILEOUT1_ERR_CYA - NV_NXBAR_TC_TILEOUT0_ERR_CYA))

#define NVSWITCH_NXBAR_RD32_LR10(_d, _engidx, _dev, _reg)  \
    NVSWITCH_ENG_RD32_LR10(_d, NXBAR, _engidx, _dev, _reg)

#define NVSWITCH_NXBAR_WR32_LR10(_d, _engidx, _dev, _reg, _data)  \
    NVSWITCH_ENG_WR32_LR10(_d, NXBAR, , _engidx, _dev, _reg, _data)

#define NVSWITCH_TILE_RD32_LR10(_d, _engidx, _dev, _reg)  \
    NVSWITCH_ENG_RD32_LR10(_d, TILE, _engidx, _dev, _reg)

#define NVSWITCH_TILE_WR32_LR10(_d, _engidx, _dev, _reg, _data)  \
    NVSWITCH_ENG_WR32_LR10(_d, TILE, , _engidx, _dev, _reg, _data)


#define NV_PPRIV_PRT_PRT_PRIV_ERROR_ADR(i) (NV_PPRIV_PRT_PRT0_PRIV_ERROR_ADR + \
    i * (NV_PPRIV_PRT_PRT1_PRIV_ERROR_ADR - NV_PPRIV_PRT_PRT0_PRIV_ERROR_ADR))

#define NV_PPRIV_PRT_PRT_PRIV_ERROR_WRDAT(i) (NV_PPRIV_PRT_PRT0_PRIV_ERROR_WRDAT + \
    i * (NV_PPRIV_PRT_PRT1_PRIV_ERROR_WRDAT - NV_PPRIV_PRT_PRT0_PRIV_ERROR_WRDAT))

#define NV_PPRIV_PRT_PRT_PRIV_ERROR_INFO(i) (NV_PPRIV_PRT_PRT0_PRIV_ERROR_INFO + \
    i * (NV_PPRIV_PRT_PRT1_PRIV_ERROR_INFO - NV_PPRIV_PRT_PRT0_PRIV_ERROR_INFO))

#define NV_PPRIV_PRT_PRT_PRIV_ERROR_CODE(i) (NV_PPRIV_PRT_PRT0_PRIV_ERROR_CODE + \
    i * (NV_PPRIV_PRT_PRT1_PRIV_ERROR_CODE - NV_PPRIV_PRT_PRT0_PRIV_ERROR_CODE))

#define NUM_NVLW_ENGINE_LR10                            9
#define NUM_NVLW_BCAST_ENGINE_LR10                      1
#define NUM_NVLW_PERFMON_ENGINE_LR10                    9
#define NUM_NVLW_PERFMON_BCAST_ENGINE_LR10              1
#define NUM_MINION_ENGINE_LR10                          9
#define NUM_MINION_BCAST_ENGINE_LR10                    1
#define NUM_NVLIPT_ENGINE_LR10                          9
#define NUM_NVLIPT_BCAST_ENGINE_LR10                    1
#define NUM_NVLIPT_SYS_PERFMON_ENGINE_LR10              9
#define NUM_NVLIPT_SYS_PERFMON_BCAST_ENGINE_LR10        1
#define NUM_NVLTLC_ENGINE_LR10                          36
#define NUM_NVLTLC_BCAST_ENGINE_LR10                    4
#define NUM_NVLTLC_MULTICAST_ENGINE_LR10                9
#define NUM_NVLTLC_MULTICAST_BCAST_ENGINE_LR10          1
#define NUM_TX_PERFMON_ENGINE_LR10                      36
#define NUM_TX_PERFMON_BCAST_ENGINE_LR10                4
#define NUM_TX_PERFMON_MULTICAST_ENGINE_LR10            9
#define NUM_TX_PERFMON_MULTICAST_BCAST_ENGINE_LR10      1
#define NUM_RX_PERFMON_ENGINE_LR10                      36
#define NUM_RX_PERFMON_BCAST_ENGINE_LR10                4
#define NUM_RX_PERFMON_MULTICAST_ENGINE_LR10            9
#define NUM_RX_PERFMON_MULTICAST_BCAST_ENGINE_LR10      1
#define NUM_PLL_ENGINE_LR10                             9
#define NUM_PLL_BCAST_ENGINE_LR10                       1
#define NUM_NVLDL_ENGINE_LR10                           36
#define NUM_NVLDL_BCAST_ENGINE_LR10                     4
#define NUM_NVLDL_MULTICAST_ENGINE_LR10                 9
#define NUM_NVLDL_MULTICAST_BCAST_ENGINE_LR10           1
#define NUM_NVLIPT_LNK_ENGINE_LR10                      36
#define NUM_NVLIPT_LNK_BCAST_ENGINE_LR10                4
#define NUM_NVLIPT_LNK_MULTICAST_ENGINE_LR10            9
#define NUM_NVLIPT_LNK_MULTICAST_BCAST_ENGINE_LR10      1
#define NUM_SYS_PERFMON_ENGINE_LR10                     36
#define NUM_SYS_PERFMON_BCAST_ENGINE_LR10               4
#define NUM_SYS_PERFMON_MULTICAST_ENGINE_LR10           9
#define NUM_SYS_PERFMON_MULTICAST_BCAST_ENGINE_LR10     1
#define NVSWITCH_NUM_PRIV_PRT_LR10                      9


#define NVSWITCH_NPORT_PER_NPG          (NUM_NPORT_ENGINE_LR10/NUM_NPG_ENGINE_LR10)
#define NPORT_TO_LINK(_device, _npg, _nport)                 \
    (                                                        \
        NVSWITCH_ASSERT((_npg < NUM_NPG_ENGINE_LR10))     \
    ,                                                        \
        NVSWITCH_ASSERT((_nport < NVSWITCH_NPORT_PER_NPG))\
    ,                                                        \
        ((_npg) * NVSWITCH_NPORT_PER_NPG + (_nport))         \
    )
#define NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(_nvlipt_idx)     \
    (NVBIT64(NVSWITCH_LINKS_PER_NVLIPT) - 1) << (_nvlipt_idx * NVSWITCH_LINKS_PER_NVLIPT);

#define NVSWITCH_NUM_LINKS_LR10         (NUM_NPORT_ENGINE_LR10)
#define NVSWITCH_NUM_LANES_LR10         4

#define NVSWITCH_LINKS_PER_NVLW         (NVSWITCH_NUM_LINKS_LR10/NUM_NVLW_ENGINE_LR10)
#define NVSWITCH_LINKS_PER_MINION       (NVSWITCH_NUM_LINKS_LR10/NUM_MINION_ENGINE_LR10)
#define NVSWITCH_LINKS_PER_NVLIPT       (NVSWITCH_NUM_LINKS_LR10/NUM_NVLIPT_ENGINE_LR10)
#define NVSWITCH_LINKS_PER_NPG          (NVSWITCH_NUM_LINKS_LR10/NUM_NPG_ENGINE_LR10)

#define NVSWITCH_DECLARE_ENGINE_UC_LR10(_engine)                                \
    ENGINE_DESCRIPTOR_TYPE_LR10  eng##_engine[NUM_##_engine##_ENGINE_LR10];

#define NVSWITCH_DECLARE_ENGINE_LR10(_engine)                                   \
    ENGINE_DESCRIPTOR_TYPE_LR10  eng##_engine[NUM_##_engine##_ENGINE_LR10];   \
    ENGINE_DESCRIPTOR_TYPE_LR10  eng##_engine##_BCAST[NUM_##_engine##_BCAST_ENGINE_LR10];

#define NVSWITCH_NVLIPT_GET_PUBLIC_ID_LR10(_physlinknum) \
    ((_physlinknum)/NVSWITCH_LINKS_PER_NVLIPT)

#define NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(_physlinknum) \
    ((_physlinknum)%NVSWITCH_LINKS_PER_NVLIPT)

#define DISCOVERY_TYPE_UNDEFINED    0
#define DISCOVERY_TYPE_DISCOVERY    1
#define DISCOVERY_TYPE_UNICAST      2
#define DISCOVERY_TYPE_BROADCAST    3

//
// These field #defines describe which physical fabric address bits are
// relevant to the specific remap table address check/remap operation.
//
#define NV_INGRESS_REMAP_ADDR_PHYS_LR10         46:36

#define NV_INGRESS_REMAP_ADR_OFFSET_PHYS_LR10   35:20
#define NV_INGRESS_REMAP_ADR_BASE_PHYS_LR10     35:20
#define NV_INGRESS_REMAP_ADR_LIMIT_PHYS_LR10    35:20

typedef NVSWITCH_LINK_TYPE  NVSWITCH_LINK_TYPE_LR10;

//
// NPORT Portstat information
//

//
// LR10 supports CREQ0(0), DNGRD(1), ATR(2), ATSD(3), PROBE(4), RSP0(5), CREQ1(6), and RSP1(7) VCs.
// But DNGRD(1), ATR(2), ATSD(3), and PROBE(4) will be never used as PowerPC ATS support is not a POR for LR10 HW.
//
#define NVSWITCH_NUM_VCS_LR10    8

typedef struct
{
    NvU32 count;
    NvU32 low;
    NvU32 medium;
    NvU32 high;
    NvU32 panic;
}
NVSWITCH_LATENCY_BINS_LR10;

typedef struct
{
    NvU32                       count;
    NvU64                       start_time_nsec;
    NvU64                       last_read_time_nsec;
    NVSWITCH_LATENCY_BINS_LR10  accum_latency[NVSWITCH_NUM_LINKS_LR10];
}
NVSWITCH_LATENCY_VC_LR10;

typedef struct
{
    NvU32 sample_interval_msec;
    NvU64 last_visited_time_nsec;
    NVSWITCH_LATENCY_VC_LR10 latency[NVSWITCH_NUM_VCS_LR10];
} NVSWITCH_LATENCY_STATS_LR10;

#define NV_NPORT_PORTSTAT_LR10(_block, _reg, _vc, _index)    (NV_NPORT_PORTSTAT ## _block ## _reg ## _0 ## _index +  \
    _vc * (NV_NPORT_PORTSTAT ## _block ## _reg ## _1 ## _index - NV_NPORT_PORTSTAT ## _block ## _reg ## _0 ## _index))

#define NVSWITCH_NPORT_PORTSTAT_RD32_LR10(_d, _engidx, _block, _reg, _vc)               \
    (                                                                                   \
          NVSWITCH_ASSERT(NVSWITCH_IS_LINK_ENG_VALID_LR10(_d, NPORT, _engidx))          \
          ,                                                                             \
          NVSWITCH_PRINT(_d, MMIO,                                                      \
              "%s: MEM_RD NPORT_PORTSTAT[%d]: %s,%s (%06x+%04x)\n",                     \
              __FUNCTION__,                                                             \
              _engidx,                                                                  \
              #_block, #_reg,                                                           \
              NVSWITCH_GET_ENG(_d, NPORT, , _engidx),                                   \
              NV_NPORT_PORTSTAT_LR10(_block, _reg, _vc, _0))                            \
          ,                                                                             \
          nvswitch_reg_read_32(_d,                                                      \
              NVSWITCH_GET_ENG(_d, NPORT, , _engidx) +                                  \
              NV_NPORT_PORTSTAT_LR10(_block, _reg, _vc, _0))                            \
    );                                                                                  \
    ((void)(_d))

#define NVSWITCH_PORTSTAT_BCAST_WR32_LR10(_d, _block, _reg, _idx, _data)                \
    {                                                                                   \
        NVSWITCH_PRINT(_d, MMIO,                                                        \
              "%s: BCAST_WR NPORT_PORTSTAT: %s,%s (%06x+%04x) 0x%08x\n",                \
              __FUNCTION__,                                                             \
              #_block, #_reg,                                                           \
              NVSWITCH_GET_ENG(_d, NPORT, _BCAST, 0),                                   \
              NV_NPORT_PORTSTAT_LR10(_block, _reg, _idx, ), _data);                     \
              NVSWITCH_OFF_WR32(_d,                                                     \
                  NVSWITCH_GET_ENG(_d, NPORT, _BCAST, 0) +                              \
                  NV_NPORT_PORTSTAT_LR10(_block, _reg, _idx, ), _data);                 \
    }

//
// Per-chip device information
//

//
// The chip-specific engine list is used to generate the code to collect
// discovered unit information and coalesce it into the data structures used by
// the common IO library (see io_nvswitch.h).
//
// The PTOP discovery table presents the information on wrappers and sub-units
// in a hierarchical manner.  The top level discovery contains information
// about top level UNICAST units and IP wrappers like NPG, NVLW, and NXBAR.
// Individual units within an IP wrapper are described in discovery sub-tables.
// Each IP wrapper may have MULTICAST descriptors to allow addressing sub-units
// within a wrapper and a cluster of IP wrappers will also have a BCAST
// discovery tables, which have MULTICAST descriptors within them.
// In order to collect all the useful unit information into a single container,
// we need to pick where to find each piece within the parsed discovery table.
// Top level IP wrappers like NPG have a BCAST range to broadcast reads/writes,
// but IP sub-units like NPORT have a MULTICAST range within the BCAST IP
// wrapper to broadcast to all the sub-units in all the IP wrappers.
// So in the lists below top level IP wrappers (NPG, NVLW, and NXBAR) point
// to the _BCAST IP wrapper, but sub-unit point to the _MULTICAST range inside
// the BCAST unit (_MULTICAST_BCAST).
//
// All IP-based (0-based register manuals) engines need to be listed here to
// generate chip-specific handlers as well as in the global common list of all
// engines that have ever existed on *ANY* architecture(s) in order for them
// use common IO wrappers.
//

#define NVSWITCH_LIST_LR10_ENGINES(_op)         \
    _op(XVE, )                                  \
    _op(SAW, )                                  \
    _op(SOE, )                                  \
    _op(SMR, )                                  \
    _op(NPG, _BCAST)                            \
    _op(NPORT, _MULTICAST_BCAST)                \
                                                \
    _op(NVLW, _BCAST)                           \
    _op(MINION, _BCAST)                         \
    _op(NVLIPT, _BCAST)                         \
    _op(NVLIPT_LNK, _MULTICAST_BCAST)           \
    _op(NVLTLC, _MULTICAST_BCAST)               \
    _op(NVLDL, _MULTICAST_BCAST)                \
                                                \
    _op(NXBAR, _BCAST)                          \
    _op(TILE, _MULTICAST_BCAST)                 \
                                                \
    _op(NPG_PERFMON, _BCAST)                    \
    _op(NPORT_PERFMON, _MULTICAST_BCAST)        \
                                                \
    _op(NVLW_PERFMON, _BCAST)                   \

typedef struct
{
    struct
    {
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE common[NVSWITCH_ENGINE_ID_SIZE];
    } io;

    NVSWITCH_DECLARE_ENGINE_UC_LR10(PTOP)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(CLKS)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(FUSE)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(JTAG)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(PMGR)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(SAW)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(XP3G)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(XVE)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(ROM)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(EXTDEV)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(PRIVMAIN)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(PRIVLOC)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(PTIMER)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(SOE)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(SMR)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(I2C)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(SE)
    NVSWITCH_DECLARE_ENGINE_UC_LR10(THERM)

    NVSWITCH_DECLARE_ENGINE_LR10(NVLW)
    NVSWITCH_DECLARE_ENGINE_LR10(NXBAR)
    NVSWITCH_DECLARE_ENGINE_LR10(NPG)

    NVSWITCH_DECLARE_ENGINE_LR10(MINION)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLIPT)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLTLC)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLTLC_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLIPT_SYS_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(TX_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(RX_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(TX_PERFMON_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(RX_PERFMON_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(PLL)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLW_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLDL)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLDL_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLIPT_LNK)
    NVSWITCH_DECLARE_ENGINE_LR10(NVLIPT_LNK_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(SYS_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(SYS_PERFMON_MULTICAST)

    NVSWITCH_DECLARE_ENGINE_LR10(NPG_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(NPORT)
    NVSWITCH_DECLARE_ENGINE_LR10(NPORT_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(NPORT_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(NPORT_PERFMON_MULTICAST)

    NVSWITCH_DECLARE_ENGINE_LR10(NXBAR_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(TILE)
    NVSWITCH_DECLARE_ENGINE_LR10(TILE_MULTICAST)
    NVSWITCH_DECLARE_ENGINE_LR10(TILE_PERFMON)
    NVSWITCH_DECLARE_ENGINE_LR10(TILE_PERFMON_MULTICAST)

    // VBIOS configuration Data
    NVSWITCH_BIOS_NVLINK_CONFIG bios_config;

    // GPIO
    const NVSWITCH_GPIO_INFO   *gpio_pin;
    NvU32                       gpio_pin_size;

    // Interrupts
    NvU32                               intr_enable_legacy;
    NvU32                               intr_enable_corr;
    NvU32                               intr_enable_fatal;
    NvU32                               intr_enable_nonfatal;
    NvU32                               intr_minion_dest;

    //
    // Book-keep interrupt masks to restore them after reset.
    // Note: There is no need to book-keep interrupt masks for NVLink units like
    // DL, MINION, TLC etc. because NVLink init routines would setup them.
    //
    struct
    {
        NVSWITCH_INTERRUPT_MASK route;
        NVSWITCH_INTERRUPT_MASK ingress;
        NVSWITCH_INTERRUPT_MASK egress;
        NVSWITCH_INTERRUPT_MASK tstate;
        NVSWITCH_INTERRUPT_MASK sourcetrack;
        NVSWITCH_INTERRUPT_MASK tile;
        NVSWITCH_INTERRUPT_MASK tileout;
    } intr_mask;

    // Latency statistics
    NVSWITCH_LATENCY_STATS_LR10         *latency_stats;

    // External TDIODE info
    NVSWITCH_TDIODE_INFO_TYPE           tdiode;

    // Ganged Link table
    NvU64 *ganged_link_table;

    //
    // Mask of links on the LR10 device connected to a disabled
    // remote link
    //
    NvU64 disabledRemoteEndLinkMask;

    //
    // Bool indicating if disabledRemoteEndLinkMask
    // has been cached previously
    //
    NvBool bDisabledRemoteEndLinkMaskCached;
} lr10_device;

#define NVSWITCH_NUM_DEVICES_PER_DELTA_LR10 6

typedef struct {
    NvU32 switchPhysicalId;
    NvU64 accessLinkMask;
    NvU64 trunkLinkMask;
} lr10_links_connected_to_disabled_remote_end;

#define NVSWITCH_GET_CHIP_DEVICE_LR10(_device)                  \
    (                                                           \
        ((_device)->chip_id == NV_PSMC_BOOT_42_CHIP_ID_LR10) ?  \
            ((lr10_device *) _device->chip_device) :            \
            NULL                                                \
    )

//
// Internal function declarations
//
NvlStatus nvswitch_device_discovery_lr10(nvswitch_device *device, NvU32 discovery_offset);
void nvswitch_filter_discovery_lr10(nvswitch_device *device);
NvlStatus nvswitch_process_discovery_lr10(nvswitch_device *device);
nvswitch_device *nvswitch_get_device_by_pci_info_lr10(nvlink_pci_info *info);
NvlStatus nvswitch_ring_master_cmd_lr10(nvswitch_device *device, NvU32 cmd);
void nvswitch_initialize_interrupt_tree_lr10(nvswitch_device *device);
void nvswitch_lib_enable_interrupts_lr10(nvswitch_device *device);
void nvswitch_lib_disable_interrupts_lr10(nvswitch_device *device);
NvlStatus nvswitch_lib_service_interrupts_lr10(nvswitch_device *device);
NvlStatus nvswitch_lib_check_interrupts_lr10(nvswitch_device *device);
void nvswitch_set_ganged_link_table_lr10(nvswitch_device *device, NvU32 firstIndex, NvU64 *ganged_link_table, NvU32 numEntries);
NvlStatus nvswitch_pmgr_init_config_lr10(nvswitch_device *device);
NvlStatus nvswitch_minion_service_falcon_interrupts_lr10(nvswitch_device *device, NvU32 instance);
NvlStatus nvswitch_ctrl_i2c_indexed_lr10(nvswitch_device *device,
                    NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams);
NvU32 nvswitch_i2c_get_port_info_lr10(nvswitch_device *device, NvU32 port);
void nvswitch_translate_error_lr10(NVSWITCH_ERROR_TYPE         *error_entry,
                                   NVSWITCH_NVLINK_ARCH_ERROR  *arch_error,
                                   NVSWITCH_NVLINK_HW_ERROR    *hw_error);
NvlStatus nvswitch_corelib_add_link_lr10(nvlink_link *link);
NvlStatus nvswitch_corelib_remove_link_lr10(nvlink_link *link);
NvlStatus nvswitch_corelib_set_dl_link_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_dl_link_mode_lr10(nvlink_link *link, NvU64 *mode);
NvlStatus nvswitch_corelib_set_tl_link_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_tl_link_mode_lr10(nvlink_link *link, NvU64 *mode);
NvlStatus nvswitch_corelib_set_tx_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_tx_mode_lr10(nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus nvswitch_corelib_set_rx_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_rx_mode_lr10(nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus nvswitch_corelib_set_rx_detect_lr10(nvlink_link *link, NvU32 flags);
NvlStatus nvswitch_corelib_get_rx_detect_lr10(nvlink_link *link);
void      nvswitch_corelib_training_complete_lr10(nvlink_link *link);
NvBool    nvswitch_link_lane_reversed_lr10(nvswitch_device *device, NvU32 linkId);
NvBool    nvswitch_is_link_in_reset_lr10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_wait_for_tl_request_ready_lr10(nvlink_link *link);
NvlStatus nvswitch_request_tl_link_state_lr10(nvlink_link *link, NvU32 tlLinkState, NvBool bSync);
void      nvswitch_execute_unilateral_link_shutdown_lr10(nvlink_link *link);
NvlStatus nvswitch_get_link_public_id_lr10(nvswitch_device *device, NvU32 linkId, NvU32 *publicId);
NvlStatus nvswitch_get_link_local_idx_lr10(nvswitch_device *device, NvU32 linkId, NvU32 *localLinkIdx);
NvlStatus nvswitch_set_training_error_info_lr10(nvswitch_device *device,
                                                NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS *pLinkTrainingErrorInfoParams);
NvlStatus nvswitch_read_vbios_link_entries_lr10(nvswitch_device *device, NvU32 tblPtr,NvU32 entriesCount,NVLINK_CONFIG_DATA_LINKENTRY *link_entries, NvU32 *identified_entriesCount);
NvlStatus nvswitch_ctrl_get_fatal_error_scope_lr10(nvswitch_device *device, NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS *pParams);
void      nvswitch_init_scratch_lr10(nvswitch_device *device);
void      nvswitch_init_dlpl_interrupts_lr10(nvlink_link *link);
NvlStatus nvswitch_init_nport_lr10(nvswitch_device *device);
NvlStatus nvswitch_poll_sublink_state_lr10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_setup_link_loopback_mode_lr10(nvswitch_device *device, NvU32 linkNumber);
void nvswitch_reset_persistent_link_hw_state_lr10(nvswitch_device *device, NvU32 linkNumber);
void nvswitch_store_topology_information_lr10(nvswitch_device *device, nvlink_link *link);
void nvswitch_init_lpwr_regs_lr10(nvlink_link *link);
void nvswitch_program_l1_scratch_reg_lr10(nvswitch_device *device, NvU32 linkNumber);
NvlStatus nvswitch_set_training_mode_lr10(nvswitch_device *device);
NvBool nvswitch_i2c_is_device_access_allowed_lr10(nvswitch_device *device, NvU32 port, NvU8 addr, NvBool bIsRead);
NvU32     nvswitch_get_sublink_width_lr10(nvswitch_device *device,NvU32 linkNumber);
NvlStatus nvswitch_parse_bios_image_lr10(nvswitch_device *device);
NvlStatus nvswitch_ctrl_get_throughput_counters_lr10(nvswitch_device *device, NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *p);
void nvswitch_corelib_get_uphy_load_lr10(nvlink_link *link, NvBool *bUnlocked);
void      nvswitch_init_buffer_ready_lr10(nvswitch_device *device, nvlink_link *link, NvBool bNportBufferReady);
NvlStatus nvswitch_ctrl_get_nvlink_lp_counters_lr10(nvswitch_device *device, NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS *params);
NvlStatus nvswitch_service_nvldl_fatal_link_lr10(nvswitch_device *device, NvU32 nvliptInstance, NvU32 link);
NvlStatus nvswitch_ctrl_inband_send_data_lr10(nvswitch_device *device, NVSWITCH_INBAND_SEND_DATA_PARAMS *p);
NvlStatus nvswitch_ctrl_inband_read_data_lr10(nvswitch_device *device, NVSWITCH_INBAND_READ_DATA_PARAMS *p);
void      nvswitch_send_inband_nack_lr10(nvswitch_device *device, NvU32 *msghdr, NvU32  linkId);
NvU32     nvswitch_get_max_persistent_message_count_lr10(nvswitch_device *device);
NvlStatus nvswitch_launch_ALI_link_training_lr10(nvswitch_device *device, nvlink_link *link, NvBool bSync);
NvlStatus nvswitch_service_minion_link_lr10(nvswitch_device *device, NvU32 nvliptInstance);
void      nvswitch_apply_recal_settings_lr10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_ctrl_get_sw_info_lr10(nvswitch_device *device, NVSWITCH_GET_SW_INFO_PARAMS *p);
void      nvswitch_setup_link_system_registers_lr10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_load_link_disable_settings_lr10(nvswitch_device *device, nvlink_link *link);
NvBool    nvswitch_is_smbpbi_supported_lr10(nvswitch_device *device);
NvlStatus nvswitch_ctrl_get_board_part_number_lr10(nvswitch_device *device, NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR *p);
NvlStatus nvswitch_ctrl_get_link_l1_capability_lr10(nvswitch_device *device, NvU32 linkId, NvBool *isL1Capable);
NvlStatus nvswitch_ctrl_get_link_l1_threshold_lr10(nvswitch_device *device, NvU32 linkNum, NvU32 *lpThreshold);
NvlStatus nvswitch_ctrl_set_link_l1_threshold_lr10(nvlink_link *link, NvU32 lpEntryThreshold);
NvlStatus nvswitch_get_board_id_lr10(nvswitch_device *device, NvU16 *boardId);

NvlStatus nvswitch_ctrl_get_soe_heartbeat_lr10(nvswitch_device *device, NVSWITCH_GET_SOE_HEARTBEAT_PARAMS *p);
void      nvswitch_update_link_state_led_lr10(nvswitch_device *device);
void      nvswitch_led_shutdown_lr10(nvswitch_device *device);

NvlStatus nvswitch_ctrl_set_mc_rid_table_lr10(nvswitch_device *device, NVSWITCH_SET_MC_RID_TABLE_PARAMS *p);
NvlStatus nvswitch_ctrl_get_mc_rid_table_lr10(nvswitch_device *device, NVSWITCH_GET_MC_RID_TABLE_PARAMS *p);
NvlStatus nvswitch_launch_ALI_lr10(nvswitch_device *device);
NvlStatus nvswitch_reset_and_train_link_lr10(nvswitch_device *device, nvlink_link *link);

NvlStatus nvswitch_ctrl_get_bios_info_lr10(nvswitch_device *device, NVSWITCH_GET_BIOS_INFO_PARAMS *p);
NvBool    nvswitch_does_link_need_termination_enabled_lr10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_link_termination_setup_lr10(nvswitch_device *device, nvlink_link* link);
void      nvswitch_fsp_update_cmdq_head_tail_lr10(nvswitch_device  *device, NvU32 queueHead, NvU32 queueTail);
void      nvswitch_fsp_get_cmdq_head_tail_lr10(nvswitch_device  *device, NvU32 *pQueueHead, NvU32 *pQueueTail);
void      nvswitch_fsp_update_msgq_head_tail_lr10(nvswitch_device *device, NvU32 msgqHead, NvU32 msgqTail);
void      nvswitch_fsp_get_msgq_head_tail_lr10(nvswitch_device *device, NvU32 *pMsgqHead, NvU32 *pMsgqTail);
NvU32     nvswitch_fsp_get_channel_size_lr10(nvswitch_device *device);
NvU8      nvswitch_fsp_nvdm_to_seid_lr10(nvswitch_device *device, NvU8 nvdmType);
NvU32     nvswitch_fsp_create_mctp_header_lr10(nvswitch_device *device, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq);
NvU32     nvswitch_fsp_create_nvdm_header_lr10(nvswitch_device *device, NvU32 nvdmType);
NvlStatus nvswitch_fsp_get_packet_info_lr10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size, NvU8 *pPacketState, NvU8 *pTag);
NvlStatus nvswitch_fsp_validate_mctp_payload_header_lr10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_process_nvdm_msg_lr10(nvswitch_device  *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_process_cmd_response_lr10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_config_ememc_lr10(nvswitch_device *device, NvU32 offset, NvBool bAincw, NvBool bAincr);
NvlStatus nvswitch_fsp_write_to_emem_lr10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_read_from_emem_lr10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_error_code_to_nvlstatus_map_lr10(nvswitch_device *device, NvU32 errorCode);
NvlStatus nvswitch_tnvl_get_attestation_certificate_chain_lr10(nvswitch_device *device, NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS *params);
NvlStatus nvswitch_tnvl_get_attestation_report_lr10(nvswitch_device *device, NVSWITCH_GET_ATTESTATION_REPORT_PARAMS *params);
NvlStatus nvswitch_tnvl_get_status_lr10(nvswitch_device *device, NVSWITCH_GET_TNVL_STATUS_PARAMS *params);
void      nvswitch_tnvl_disable_interrupts_lr10(nvswitch_device *device);
#endif //_LR10_H_
