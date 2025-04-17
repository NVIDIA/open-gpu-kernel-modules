/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _LS10_H_
#define _LS10_H_



#include "export_nvswitch.h"
#include "common_nvswitch.h"

#include "nvswitch/ls10/dev_boot.h"
#include "ctrl_dev_nvswitch.h"


#define NVSWITCH_NUM_LINKS_LS10                 64
#define NVSWITCH_NUM_LANES_LS10                 2

#define NVSWITCH_LINKS_PER_MINION_LS10          4
#define NVSWITCH_LINKS_PER_NVLIPT_LS10          4
#define NVSWITCH_LINKS_PER_NVLW_LS10            4
#define NVSWITCH_LINKS_PER_NPG_LS10             4

#define NVSWITCH_NPORT_PER_NPG_LS10             NVSWITCH_LINKS_PER_NPG_LS10

#define NUM_PTOP_ENGINE_LS10                    1
#define NUM_FUSE_ENGINE_LS10                    1
#define NUM_GIN_ENGINE_LS10                     1
#define NUM_JTAG_ENGINE_LS10                    1

#define NUM_PMGR_ENGINE_LS10                    1
#define NUM_SAW_ENGINE_LS10                     1
#define NUM_ROM_ENGINE_LS10                     1
#define NUM_EXTDEV_ENGINE_LS10                  1
#define NUM_PTIMER_ENGINE_LS10                  1
#define NUM_SOE_ENGINE_LS10                     1
#define NUM_SMR_ENGINE_LS10                     2
#define NUM_SE_ENGINE_LS10                      1
#define NUM_THERM_ENGINE_LS10                   1
#define NUM_XAL_ENGINE_LS10                     1
#define NUM_XAL_FUNC_ENGINE_LS10                1
#define NUM_XTL_CONFIG_ENGINE_LS10              1
#define NUM_XPL_ENGINE_LS10                     1
#define NUM_XTL_ENGINE_LS10                     1
#define NUM_SYSCTRL_ENGINE_LS10                 1
#define NUM_UXL_ENGINE_LS10                     1
#define NUM_GPU_PTOP_ENGINE_LS10                1
#define NUM_PMC_ENGINE_LS10                     1
#define NUM_PBUS_ENGINE_LS10                    1
#define NUM_ROM2_ENGINE_LS10                    1
#define NUM_GPIO_ENGINE_LS10                    1
#define NUM_FSP_ENGINE_LS10                     1

#define NUM_CLKS_SYS_ENGINE_LS10                1
#define NUM_CLKS_SYSB_ENGINE_LS10               1
#define NUM_CLKS_P0_ENGINE_LS10                 4
#define NUM_CLKS_P0_BCAST_ENGINE_LS10           1
#define NUM_SAW_PM_ENGINE_LS10                  1
#define NUM_PCIE_PM_ENGINE_LS10                 1
#define NUM_PRT_PRI_HUB_ENGINE_LS10             16
#define NUM_PRT_PRI_RS_CTRL_ENGINE_LS10         16
#define NUM_PRT_PRI_HUB_BCAST_ENGINE_LS10       1
#define NUM_PRT_PRI_RS_CTRL_BCAST_ENGINE_LS10   1
#define NUM_SYS_PRI_HUB_ENGINE_LS10             1
#define NUM_SYS_PRI_RS_CTRL_ENGINE_LS10         1
#define NUM_SYSB_PRI_HUB_ENGINE_LS10            1
#define NUM_SYSB_PRI_RS_CTRL_ENGINE_LS10        1
#define NUM_PRI_MASTER_RS_ENGINE_LS10           1

#define NUM_NPG_ENGINE_LS10                     16
#define NUM_NPG_PERFMON_ENGINE_LS10             NUM_NPG_ENGINE_LS10
#define NUM_NPORT_ENGINE_LS10                   (NUM_NPG_ENGINE_LS10 * NVSWITCH_NPORT_PER_NPG_LS10)
#define NUM_NPORT_MULTICAST_ENGINE_LS10         NUM_NPG_ENGINE_LS10
#define NUM_NPORT_PERFMON_ENGINE_LS10           NUM_NPORT_ENGINE_LS10
#define NUM_NPORT_PERFMON_MULTICAST_ENGINE_LS10 NUM_NPG_ENGINE_LS10

#define NUM_NPG_BCAST_ENGINE_LS10               1
#define NUM_NPG_PERFMON_BCAST_ENGINE_LS10       NUM_NPG_BCAST_ENGINE_LS10
#define NUM_NPORT_BCAST_ENGINE_LS10             NVSWITCH_NPORT_PER_NPG_LS10
#define NUM_NPORT_MULTICAST_BCAST_ENGINE_LS10   NUM_NPG_BCAST_ENGINE_LS10
#define NUM_NPORT_PERFMON_BCAST_ENGINE_LS10     NUM_NPORT_BCAST_ENGINE_LS10
#define NUM_NPORT_PERFMON_MULTICAST_BCAST_ENGINE_LS10 NUM_NPG_BCAST_ENGINE_LS10

#define NUM_NVLW_ENGINE_LS10                            16
#define NUM_NVLIPT_ENGINE_LS10                          NUM_NVLW_ENGINE_LS10
#define NUM_MINION_ENGINE_LS10                          NUM_NVLW_ENGINE_LS10
#define NUM_PLL_ENGINE_LS10                             NUM_NVLW_ENGINE_LS10
#define NUM_CPR_ENGINE_LS10                             NUM_NVLW_ENGINE_LS10
#define NUM_NVLW_PERFMON_ENGINE_LS10                    NUM_NVLW_ENGINE_LS10
#define NUM_NVLIPT_SYS_PERFMON_ENGINE_LS10              NUM_NVLW_ENGINE_LS10
#define NUM_NVLDL_MULTICAST_ENGINE_LS10                 NUM_NVLW_ENGINE_LS10
#define NUM_NVLTLC_MULTICAST_ENGINE_LS10                NUM_NVLW_ENGINE_LS10
#define NUM_NVLIPT_LNK_MULTICAST_ENGINE_LS10            NUM_NVLW_ENGINE_LS10
#define NUM_SYS_PERFMON_MULTICAST_ENGINE_LS10           NUM_NVLW_ENGINE_LS10
#define NUM_TX_PERFMON_MULTICAST_ENGINE_LS10            NUM_NVLW_ENGINE_LS10
#define NUM_RX_PERFMON_MULTICAST_ENGINE_LS10            NUM_NVLW_ENGINE_LS10
#define NUM_NVLDL_ENGINE_LS10                           (NUM_NVLW_ENGINE_LS10 * NVSWITCH_LINKS_PER_NVLIPT_LS10)
#define NUM_NVLTLC_ENGINE_LS10                          NUM_NVLDL_ENGINE_LS10
#define NUM_NVLIPT_LNK_ENGINE_LS10                      NUM_NVLDL_ENGINE_LS10
#define NUM_SYS_PERFMON_ENGINE_LS10                     NUM_NVLDL_ENGINE_LS10
#define NUM_TX_PERFMON_ENGINE_LS10                      NUM_NVLDL_ENGINE_LS10
#define NUM_RX_PERFMON_ENGINE_LS10                      NUM_NVLDL_ENGINE_LS10

#define NUM_NVLW_BCAST_ENGINE_LS10                      1
#define NUM_NVLIPT_BCAST_ENGINE_LS10                    NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_MINION_BCAST_ENGINE_LS10                    NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_PLL_BCAST_ENGINE_LS10                       NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_CPR_BCAST_ENGINE_LS10                       NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_NVLW_PERFMON_BCAST_ENGINE_LS10              NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_NVLIPT_SYS_PERFMON_BCAST_ENGINE_LS10        NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_NVLDL_MULTICAST_BCAST_ENGINE_LS10           NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_NVLTLC_MULTICAST_BCAST_ENGINE_LS10          NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_NVLIPT_LNK_MULTICAST_BCAST_ENGINE_LS10      NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_SYS_PERFMON_MULTICAST_BCAST_ENGINE_LS10     NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_TX_PERFMON_MULTICAST_BCAST_ENGINE_LS10      NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_RX_PERFMON_MULTICAST_BCAST_ENGINE_LS10      NUM_NVLW_BCAST_ENGINE_LS10
#define NUM_NVLDL_BCAST_ENGINE_LS10                     NVSWITCH_LINKS_PER_NVLIPT_LS10
#define NUM_NVLTLC_BCAST_ENGINE_LS10                    NUM_NVLDL_BCAST_ENGINE_LS10
#define NUM_NVLIPT_LNK_BCAST_ENGINE_LS10                NUM_NVLDL_BCAST_ENGINE_LS10
#define NUM_SYS_PERFMON_BCAST_ENGINE_LS10               NUM_NVLDL_BCAST_ENGINE_LS10
#define NUM_TX_PERFMON_BCAST_ENGINE_LS10                NUM_NVLDL_BCAST_ENGINE_LS10
#define NUM_RX_PERFMON_BCAST_ENGINE_LS10                NUM_NVLDL_BCAST_ENGINE_LS10

#define NUM_NXBAR_ENGINE_LS10                           3
#define NUM_NXBAR_PERFMON_ENGINE_LS10                   NUM_NXBAR_ENGINE_LS10
#define NUM_TILE_MULTICAST_ENGINE_LS10                  NUM_NXBAR_ENGINE_LS10
#define NUM_TILE_PERFMON_MULTICAST_ENGINE_LS10          NUM_NXBAR_ENGINE_LS10
#define NUM_TILE_ENGINE_LS10                            (12 * NUM_NXBAR_ENGINE_LS10)
#define NUM_TILE_PERFMON_ENGINE_LS10                    NUM_TILE_ENGINE_LS10
#define NUM_TILEOUT_MULTICAST_ENGINE_LS10               NUM_NXBAR_ENGINE_LS10
#define NUM_TILEOUT_PERFMON_MULTICAST_ENGINE_LS10       NUM_NXBAR_ENGINE_LS10
#define NUM_TILEOUT_ENGINE_LS10                         NUM_TILE_ENGINE_LS10
#define NUM_TILEOUT_PERFMON_ENGINE_LS10                 NUM_TILE_ENGINE_LS10

#define NUM_NXBAR_BCAST_ENGINE_LS10                     1
#define NUM_NXBAR_PERFMON_BCAST_ENGINE_LS10             NUM_NXBAR_BCAST_ENGINE_LS10
#define NUM_TILE_MULTICAST_BCAST_ENGINE_LS10            NUM_NXBAR_BCAST_ENGINE_LS10
#define NUM_TILE_PERFMON_MULTICAST_BCAST_ENGINE_LS10    NUM_NXBAR_BCAST_ENGINE_LS10
#define NUM_TILE_BCAST_ENGINE_LS10                      12
#define NUM_TILE_PERFMON_BCAST_ENGINE_LS10              NUM_TILE_BCAST_ENGINE_LS10
#define NUM_TILEOUT_MULTICAST_BCAST_ENGINE_LS10         NUM_NXBAR_BCAST_ENGINE_LS10
#define NUM_TILEOUT_PERFMON_MULTICAST_BCAST_ENGINE_LS10 NUM_NXBAR_BCAST_ENGINE_LS10
#define NUM_TILEOUT_BCAST_ENGINE_LS10                   NUM_TILE_BCAST_ENGINE_LS10
#define NUM_TILEOUT_PERFMON_BCAST_ENGINE_LS10           NUM_TILE_BCAST_ENGINE_LS10
#define NUM_MAX_MCFLA_SLOTS_LS10                        128

#define NPORT_TO_LINK_LS10(_device, _npg, _nport)               \
    (                                                           \
        NVSWITCH_ASSERT((_npg < NUM_NPG_ENGINE_LS10))           \
    ,                                                           \
        NVSWITCH_ASSERT((_nport < NVSWITCH_NPORT_PER_NPG_LS10)) \
    ,                                                           \
        ((_npg) * NVSWITCH_NPORT_PER_NPG_LS10 + (_nport))       \
    )

#define NVSWITCH_NUM_LINKS_PER_NVLIPT_LS10              (NVSWITCH_NUM_LINKS_LS10/NUM_NVLIPT_ENGINE_LS10)

#define NVSWITCH_NVLIPT_GET_PUBLIC_ID_LS10(_physlinknum) \
    ((_physlinknum)/NVSWITCH_LINKS_PER_NVLIPT_LS10)

#define NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(_physlinknum) \
    ((_physlinknum)%NVSWITCH_NUM_LINKS_PER_NVLIPT_LS10)

#define NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(_nvlipt_idx)     \
    (NVBIT64(NVSWITCH_LINKS_PER_NVLIPT_LS10) - 1) << (_nvlipt_idx * NVSWITCH_LINKS_PER_NVLIPT_LS10);

#define DMA_ADDR_WIDTH_LS10     64

#define SOE_VBIOS_VERSION_MASK                        0xFF0000
#define SOE_VBIOS_REVLOCK_DISABLE_NPORT_FATAL_INTR    0x370000
#define SOE_VBIOS_REVLOCK_ISSUE_INGRESS_STOP          0x4C0000
#define SOE_VBIOS_REVLOCK_TNVL_PRELOCK_COMMAND        0x590000
#define SOE_VBIOS_REVLOCK_SOE_PRI_CHECKS              0x610000
#define SOE_VBIOS_REVLOCK_REPORT_EN                   0x6A0000
#define SOE_VBIOS_REVLOCK_ENGINE_COUNTERS             0x700000


// LS10 Saved LED state
#define ACCESS_LINK_LED_STATE CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED

// Access link LED states on LS10 Systems
#define ACCESS_LINK_LED_STATE_FAULT      0U
#define ACCESS_LINK_LED_STATE_OFF        1U
#define ACCESS_LINK_LED_STATE_INITIALIZE 2U
#define ACCESS_LINK_LED_STATE_UP_WARM    3U
#define ACCESS_LINK_LED_STATE_UP_ACTIVE  4U
#define ACCESS_LINK_NUM_LED_STATES       5U

//
// Helpful IO wrappers
//

#define NVSWITCH_NPORT_WR32_LS10(_d, _engidx, _dev, _reg, _data)        \
    NVSWITCH_ENG_WR32(_d, NPORT, , _engidx, _dev, _reg, _data)

#define NVSWITCH_NPORT_RD32_LS10(_d, _engidx, _dev, _reg)               \
    NVSWITCH_ENG_RD32(_d, NPORT, , _engidx, _dev, _reg)

#define NVSWITCH_MINION_WR32_LS10(_d, _engidx, _dev, _reg, _data)       \
    NVSWITCH_ENG_WR32(_d, MINION, , _engidx, _dev, _reg, _data)

#define NVSWITCH_MINION_RD32_LS10(_d, _engidx, _dev, _reg)              \
    NVSWITCH_ENG_RD32(_d, MINION, , _engidx, _dev, _reg)

#define NVSWITCH_MINION_WR32_BCAST_LS10(_d, _dev, _reg, _data)          \
    NVSWITCH_ENG_WR32(_d, MINION, _BCAST, 0, _dev, _reg, _data)

//
// Per-chip device information
//

#define DISCOVERY_TYPE_UNDEFINED    0
#define DISCOVERY_TYPE_DISCOVERY    1
#define DISCOVERY_TYPE_UNICAST      2
#define DISCOVERY_TYPE_BROADCAST    3

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
} ENGINE_DISCOVERY_TYPE_LS10;

#define NVSWITCH_DECLARE_ENGINE_UC_LS10(_engine)                                \
    ENGINE_DISCOVERY_TYPE_LS10  eng##_engine[NUM_##_engine##_ENGINE_LS10];

#define NVSWITCH_DECLARE_ENGINE_LS10(_engine)                                   \
    ENGINE_DISCOVERY_TYPE_LS10  eng##_engine[NUM_##_engine##_ENGINE_LS10];     \
    ENGINE_DISCOVERY_TYPE_LS10  eng##_engine##_BCAST[NUM_##_engine##_BCAST_ENGINE_LS10];

#define NVSWITCH_LIST_LS10_ENGINE_UC(_op)       \
    _op(PTOP)                                   \
    _op(FUSE)                                   \
    _op(GIN)                                    \
    _op(JTAG)                                   \
    _op(PMGR)                                   \
    _op(SAW)                                    \
    _op(ROM)                                    \
    _op(EXTDEV)                                 \
    _op(PTIMER)                                 \
    _op(SOE)                                    \
    _op(SMR)                                    \
    _op(SE)                                     \
    _op(THERM)                                  \
    _op(XAL)                                    \
    _op(XAL_FUNC)                               \
    _op(XTL_CONFIG)                             \
    _op(XPL)                                    \
    _op(XTL)                                    \
    _op(UXL)                                    \
    _op(GPU_PTOP)                               \
    _op(PMC)                                    \
    _op(PBUS)                                   \
    _op(ROM2)                                   \
    _op(GPIO)                                   \
    _op(FSP)                                    \
    _op(CLKS_SYS)                               \
    _op(CLKS_SYSB)                              \
    _op(CLKS_P0)                                \
    _op(CLKS_P0_BCAST)                          \
    _op(SAW_PM)                                 \
    _op(PCIE_PM)                                \
    _op(SYS_PRI_HUB)                            \
    _op(SYS_PRI_RS_CTRL)                        \
    _op(SYSB_PRI_HUB)                           \
    _op(SYSB_PRI_RS_CTRL)                       \
    _op(PRI_MASTER_RS)                          \

#define NVSWITCH_LIST_PRI_HUB_LS10_ENGINE(_op)  \
    _op(PRT_PRI_HUB)                            \
    _op(PRT_PRI_RS_CTRL)                        \
    _op(PRT_PRI_HUB_BCAST)                      \
    _op(PRT_PRI_RS_CTRL_BCAST)                  \

#define NVSWITCH_LIST_NPG_LS10_ENGINE(_op)      \
    _op(NPG)                                    \
    _op(NPG_PERFMON)                            \
    _op(NPORT)                                  \
    _op(NPORT_MULTICAST)                        \
    _op(NPORT_PERFMON)                          \
    _op(NPORT_PERFMON_MULTICAST)

#define NVSWITCH_LIST_NVLW_LS10_ENGINE(_op)     \
    _op(NVLW)                                   \
    _op(NVLIPT)                                 \
    _op(MINION)                                 \
    _op(CPR)                                    \
    _op(NVLW_PERFMON)                           \
    _op(NVLIPT_SYS_PERFMON)                     \
    _op(NVLDL_MULTICAST)                        \
    _op(NVLTLC_MULTICAST)                       \
    _op(NVLIPT_LNK_MULTICAST)                   \
    _op(SYS_PERFMON_MULTICAST)                  \
    _op(TX_PERFMON_MULTICAST)                   \
    _op(RX_PERFMON_MULTICAST)                   \
    _op(NVLDL)                                  \
    _op(NVLTLC)                                 \
    _op(NVLIPT_LNK)                             \
    _op(SYS_PERFMON)                            \
    _op(TX_PERFMON)                             \
    _op(RX_PERFMON)

#define NVSWITCH_LIST_NXBAR_LS10_ENGINE(_op)    \
    _op(NXBAR)                                  \
    _op(NXBAR_PERFMON)                          \
    _op(TILE_MULTICAST)                         \
    _op(TILE_PERFMON_MULTICAST)                 \
    _op(TILE)                                   \
    _op(TILE_PERFMON)                           \
    _op(TILEOUT_MULTICAST)                      \
    _op(TILEOUT_PERFMON_MULTICAST)              \
    _op(TILEOUT)                                \
    _op(TILEOUT_PERFMON)

#define NVSWITCH_LIST_LS10_ENGINE(_op)          \
    NVSWITCH_LIST_NPG_LS10_ENGINE(_op)          \
    NVSWITCH_LIST_NVLW_LS10_ENGINE(_op)         \
    NVSWITCH_LIST_NXBAR_LS10_ENGINE(_op)

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

#define NVSWITCH_LIST_LS10_ENGINES(_op)         \
    _op(GIN, )                                  \
    _op(XAL, )                                  \
    _op(XPL, )                                  \
    _op(XTL, )                                  \
    _op(XTL_CONFIG, )                           \
    _op(SAW, )                                  \
    _op(SOE, )                                  \
    _op(SMR, )                                  \
                                                \
    _op(PRT_PRI_HUB, _BCAST)                    \
    _op(PRT_PRI_RS_CTRL, _BCAST)                \
    _op(SYS_PRI_HUB, )                          \
    _op(SYS_PRI_RS_CTRL, )                      \
    _op(SYSB_PRI_HUB, )                         \
    _op(SYSB_PRI_RS_CTRL, )                     \
    _op(PRI_MASTER_RS, )                        \
    _op(PTIMER, )                               \
    _op(CLKS_SYS, )                             \
    _op(CLKS_SYSB, )                            \
    _op(CLKS_P0, _BCAST)                        \
                                                \
    _op(NPG, _BCAST)                            \
    _op(NPORT, _MULTICAST_BCAST)                \
                                                \
    _op(NVLW, _BCAST)                           \
    _op(MINION, _BCAST)                         \
    _op(NVLIPT, _BCAST)                         \
    _op(CPR, _BCAST)                            \
    _op(NVLIPT_LNK, _MULTICAST_BCAST)           \
    _op(NVLTLC, _MULTICAST_BCAST)               \
    _op(NVLDL, _MULTICAST_BCAST)                \
                                                \
    _op(NXBAR, _BCAST)                          \
    _op(TILE, _MULTICAST_BCAST)                 \
    _op(TILEOUT, _MULTICAST_BCAST)              \
                                                \
    _op(NPG_PERFMON, _BCAST)                    \
    _op(NPORT_PERFMON, _MULTICAST_BCAST)        \
                                                \
    _op(NVLW_PERFMON, _BCAST)                   \

//
// These field #defines describe which physical fabric address bits are
// relevant to the specific remap table address check/remap operation.
//

#define NV_INGRESS_REMAP_ADDR_PHYS_LS10         51:39       /* LR10: 46:36 */

#define NV_INGRESS_REMAP_ADR_OFFSET_PHYS_LS10   38:21       /* LR10: 35:20 */
#define NV_INGRESS_REMAP_ADR_BASE_PHYS_LS10     38:21       /* LR10: 35:20 */
#define NV_INGRESS_REMAP_ADR_LIMIT_PHYS_LS10    38:21       /* LR10: 35:20 */

//
// Multicast REMAP table is not indexed through the same _RAM_SEL mechanism as
// other REMAP tables, but we want to be able to use the same set of APIs for
// all the REMAP tables, so define a special RAM_SEL value for MCREMAP that
// does not conflict with the existing definitions.
//
#define NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECT_MULTICAST_REMAPRAM (DRF_MASK(NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS) + 1)

//
// NPORT Portstat information
//

//
// LS10 supports CREQ0(0), DNGRD(1), ATR(2), ATSD(3), PROBE(4), RSP0(5), CREQ1(6), and RSP1(7) VCs.
// But DNGRD(1), ATR(2), ATSD(3), and PROBE(4) will be never used.
//
#define NVSWITCH_NUM_VCS_LS10    8

typedef struct
{
    NvU32 count;
    NvU32 low;
    NvU32 medium;
    NvU32 high;
    NvU32 panic;
}
NVSWITCH_LATENCY_BINS_LS10;

typedef struct
{
    NvU32                       count;
    NvU64                       start_time_nsec;
    NvU64                       last_read_time_nsec;
    NVSWITCH_LATENCY_BINS_LS10  accum_latency[NVSWITCH_NUM_LINKS_LS10];
}
NVSWITCH_LATENCY_VC_LS10;

typedef struct
{
    NvU32 sample_interval_msec;
    NvU64 last_visited_time_nsec;
    NVSWITCH_LATENCY_VC_LS10 latency[NVSWITCH_NUM_VCS_LS10];
} NVSWITCH_LATENCY_STATS_LS10;

#define NV_NPORT_PORTSTAT_LS10(_block, _reg, _vc, _hi_lo)    (NV_NPORT_PORTSTAT ## _block ## _reg ## _0 ## _hi_lo +  \
    _vc * (NV_NPORT_PORTSTAT ## _block ## _reg ## _1 ## _hi_lo - NV_NPORT_PORTSTAT ## _block ## _reg ## _0 ## _hi_lo))

#define NVSWITCH_NPORT_PORTSTAT_RD32_LS10(_d, _engidx, _block, _reg, _hi_lo, _vc)   \
    (                                                                               \
          NVSWITCH_ASSERT(NVSWITCH_IS_LINK_ENG_VALID_LS10(_d, NPORT, _engidx))      \
          ,                                                                         \
          NVSWITCH_PRINT(_d, MMIO,                                                  \
              "%s: MEM_RD NPORT_PORTSTAT[%d]: %s,%s,_%s,%s (%06x+%04x)\n",          \
              __FUNCTION__,                                                         \
              _engidx,                                                              \
              #_block, #_reg, #_vc, #_hi_lo,                                        \
              NVSWITCH_GET_ENG(_d, NPORT, , _engidx),                               \
              NV_NPORT_PORTSTAT_LS10(_block, _reg, _vc, _hi_lo))                    \
          ,                                                                         \
          nvswitch_reg_read_32(_d,                                                  \
              NVSWITCH_GET_ENG(_d, NPORT, , _engidx) +                              \
              NV_NPORT_PORTSTAT_LS10(_block, _reg, _vc, _hi_lo))                    \
    );                                                                              \
    ((void)(_d))

#define NVSWITCH_PORTSTAT_BCAST_WR32_LS10(_d, _block, _reg, _idx, _data)            \
    {                                                                               \
         NVSWITCH_PRINT(_d, MMIO,                                                   \
              "%s: BCAST_WR NPORT_PORTSTAT: %s,%s (%06x+%04x) 0x%08x\n",            \
              __FUNCTION__,                                                         \
              #_block, #_reg,                                                       \
              NVSWITCH_GET_ENG(_d, NPORT, _BCAST, 0),                               \
              NV_NPORT_PORTSTAT_LS10(_block, _reg, _idx, ), _data);                 \
          NVSWITCH_OFF_WR32(_d,                                                     \
              NVSWITCH_GET_ENG(_d, NPORT, _BCAST, 0) +                              \
              NV_NPORT_PORTSTAT_LS10(_block, _reg, _idx, ), _data);                 \
    }

#define NVSWITCH_DEFERRED_LINK_STATE_CHECK_INTERVAL_NS ((device->bModeContinuousALI ? 15 : 30) *\
                                                        NVSWITCH_INTERVAL_1SEC_IN_NS)
#define NVSWITCH_DEFERRED_FAULT_UP_CHECK_INTERVAL_NS   (12 * NVSWITCH_INTERVAL_1MSEC_IN_NS)

// Struct used for passing around error masks in error handling functions
typedef struct
{
    NvBool bPending;
    NvU32  regData;
} MINION_LINK_INTR;

typedef struct
{
    NvU32 dl;
    NvU32 tlcRx0;
    NvU32 tlcRx0Injected;
    NvU32 tlcRx1;
    NvU32 tlcRx1Injected;
    NvU32 liptLnk;
    NvU32 liptLnkInjected;
    MINION_LINK_INTR minionLinkIntr;
} NVLINK_LINK_ERROR_INFO_ERR_MASKS, *PNVLINK_LINK_ERROR_INFO_ERR_MASKS;

typedef struct
{
    NvBool bLinkErrorsCallBackEnabled;
    NvBool bLinkStateCallBackEnabled;
    NvU64  lastRetrainTime;
    NvU64  lastLinkUpTime;
} NVLINK_LINK_ERROR_REPORTING_STATE;

typedef struct
{
    NVLINK_LINK_ERROR_INFO_ERR_MASKS fatalIntrMask;
    NVLINK_LINK_ERROR_INFO_ERR_MASKS nonFatalIntrMask;
} NVLINK_LINK_ERROR_REPORTING_DATA;

typedef struct
{
    NVLINK_LINK_ERROR_REPORTING_STATE state;
    NVLINK_LINK_ERROR_REPORTING_DATA  data;
} NVLINK_LINK_ERROR_REPORTING;

typedef struct
{
    struct
    {
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE common[NVSWITCH_ENGINE_ID_SIZE];
    } io;

    NVSWITCH_LIST_LS10_ENGINE_UC(NVSWITCH_DECLARE_ENGINE_UC_LS10)
    NVSWITCH_LIST_PRI_HUB_LS10_ENGINE(NVSWITCH_DECLARE_ENGINE_UC_LS10)
    NVSWITCH_LIST_LS10_ENGINE(NVSWITCH_DECLARE_ENGINE_LS10)

    // Interrupts
    NvU32       intr_minion_dest;

    // VBIOS configuration Data
    NVSWITCH_BIOS_NVLINK_CONFIG bios_config;

    // GPIO
    const NVSWITCH_GPIO_INFO   *gpio_pin;
    NvU32                       gpio_pin_size;

    // Latency statistics
    NVSWITCH_LATENCY_STATS_LS10         *latency_stats;

    // External TDIODE info
    NVSWITCH_TDIODE_INFO_TYPE           tdiode;

    //
    // Book-keep interrupt masks to restore them after reset.
    // Note: There is no need to book-keep interrupt masks for NVLink units like
    // DL, MINION, TLC etc. because NVLink init routines would setup them.
    //
    struct
    {
        NVSWITCH_INTERRUPT_MASK route;
        NVSWITCH_INTERRUPT_MASK ingress[2];
        NVSWITCH_INTERRUPT_MASK egress[2];
        NVSWITCH_INTERRUPT_MASK tstate;
        NVSWITCH_INTERRUPT_MASK sourcetrack;
        NVSWITCH_INTERRUPT_MASK mc_tstate;
        NVSWITCH_INTERRUPT_MASK red_tstate;
        NVSWITCH_INTERRUPT_MASK tile;
        NVSWITCH_INTERRUPT_MASK tileout;
    } intr_mask;

    // Ganged Link table
    NvU64 *ganged_link_table;

    //NVSWITCH Minion core
    NvU32 minionEngArch;

    NvBool riscvManifestBoot;

    // Nvlink error reporting management
    NVLINK_LINK_ERROR_REPORTING deferredLinkErrors[NVSWITCH_NUM_LINKS_LS10];
    NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS deferredLinkErrorsArgs[NVSWITCH_NUM_LINKS_LS10];

} ls10_device;

//
// Helpful IO wrappers
//

#define NVSWITCH_GET_CHIP_DEVICE_LS10(_device)                  \
    (                                                           \
        ((_device)->chip_id == NV_PMC_BOOT_42_CHIP_ID_LS10) ?   \
            ((ls10_device *) _device->chip_device) :            \
            NULL                                                \
    )

#define NVSWITCH_ENG_VALID_LS10(_d, _eng, _engidx)                      \
    (                                                                   \
        ((_engidx < NUM_##_eng##_ENGINE_LS10) &&                        \
        (NVSWITCH_GET_CHIP_DEVICE_LS10(_d)->eng##_eng[_engidx].valid)) ? \
        NV_TRUE : NV_FALSE                                              \
    )

#define NVSWITCH_ENG_WR32_LS10(_d, _eng, _bcast, _engidx, _dev, _reg, _data) \
    NVSWITCH_ENG_WR32(_d, _eng, _bcast, _engidx, _dev, _reg, _data)

#define NVSWITCH_ENG_RD32_LS10(_d, _eng, _engidx, _dev, _reg)       \
    NVSWITCH_ENG_RD32(_d, _eng, , _engidx, _dev, _reg)

#define NVSWITCH_BCAST_WR32_LS10(_d, _eng, _dev, _reg, _data)       \
    NVSWITCH_ENG_WR32(_d, _eng, _BCAST, 0, _dev, _reg, _data)

#define NVSWITCH_BCAST_RD32_LS10(_d, _eng, _dev, _reg)              \
    NVSWITCH_ENG_RD32(_d, _eng, _BCAST, 0, _dev, _reg)

#define NVSWITCH_SOE_WR32_LS10(_d, _instance, _dev, _reg, _data)    \
    NVSWITCH_ENG_WR32(_d, SOE, , _instance, _dev, _reg, _data)

#define NVSWITCH_SOE_RD32_LS10(_d, _instance, _dev, _reg)           \
    NVSWITCH_ENG_RD32(_d, SOE, , _instance, _dev, _reg)

#define NVSWITCH_NPORT_BCAST_WR32_LS10(_d, _dev, _reg, _data)       \
    NVSWITCH_ENG_WR32(_d, NPORT, _BCAST, 0, _dev, _reg, _data)

#define NVSWITCH_SAW_WR32_LS10(_d, _dev, _reg, _data)               \
    NVSWITCH_ENG_WR32(_d, SAW, , 0, _dev, _reg, _data)

#define NVSWITCH_SAW_RD32_LS10(_d, _dev, _reg)                      \
    NVSWITCH_ENG_RD32(_d, SAW, , 0, _dev, _reg)

#define NVSWITCH_NPORT_MC_BCAST_WR32_LS10(_d, _dev, _reg, _data)    \
    NVSWITCH_BCAST_WR32_LS10(_d, NPORT, _dev, _reg, _data)

//
// Tile Column consists of 12 Tile blocks and 11 (really 12) Tileout blocks.
//

#define NUM_NXBAR_TILES_PER_TC_LS10         12
#define NUM_NXBAR_TILEOUTS_PER_TC_LS10      12

#define TILE_INDEX_LS10(_device, _nxbar, _tile)                 \
    (                                                           \
        NVSWITCH_ASSERT((_nxbar < NUM_NXBAR_ENGINE_LS10))       \
    ,                                                           \
        NVSWITCH_ASSERT((_tile < NUM_NXBAR_TILES_PER_TC_LS10))  \
    ,                                                           \
        ((_nxbar) *  NUM_NXBAR_TILES_PER_TC_LS10 + (_tile))     \
    )

#define NVSWITCH_TILE_RD32(_d, _engidx, _dev, _reg)  \
    NVSWITCH_ENG_RD32(_d, TILE, , _engidx, _dev, _reg)

#define NVSWITCH_TILE_WR32(_d, _engidx, _dev, _reg, _data)  \
    NVSWITCH_ENG_WR32(_d, TILE, , _engidx, _dev, _reg, _data)

#define NVSWITCH_TILEOUT_RD32(_d, _engidx, _dev, _reg)  \
    NVSWITCH_ENG_RD32(_d, TILEOUT, , _engidx, _dev, _reg)

#define NVSWITCH_TILEOUT_WR32(_d, _engidx, _dev, _reg, _data)  \
    NVSWITCH_ENG_WR32(_d, TILEOUT, , _engidx, _dev, _reg, _data)

//
// Per link register access routines
// LINK_* MMIO wrappers are used to reference per-link engine instances
//

#define NVSWITCH_IS_LINK_ENG_VALID_LS10(_d, _eng, _linknum)             \
    NVSWITCH_IS_LINK_ENG_VALID(_d, _linknum, _eng)

#define NVSWITCH_LINK_OFFSET_LS10(_d, _physlinknum, _eng, _dev, _reg)   \
    NVSWITCH_LINK_OFFSET(_d, _physlinknum, _eng, _dev, _reg)

#define NVSWITCH_LINK_WR32_LS10(_d, _physlinknum, _eng, _dev, _reg, _data) \
    NVSWITCH_LINK_WR32(_d, _physlinknum, _eng, _dev, _reg, _data)

#define NVSWITCH_LINK_RD32_LS10(_d, _physlinknum, _eng, _dev, _reg)     \
    NVSWITCH_LINK_RD32(_d, _physlinknum, _eng, _dev, _reg)

#define NVSWITCH_LINK_WR32_IDX_LS10(_d, _physlinknum, _eng, _dev, _reg, _idx, _data) \
    NVSWITCH_LINK_WR32_IDX(_d, _physlinknum, _eng, _dev, _reg, _idx, _data)

#define NVSWITCH_LINK_RD32_IDX_LS10(_d, _physlinknum, _eng, _dev, _reg, _idx) \
    NVSWITCH_LINK_RD32_IDX(_d, _physlinknum, _eng, _dev, _reg, _idx)

#define NVSWITCH_MINION_LINK_WR32_LS10(_d, _physlinknum, _dev, _reg, _data)   \
    NVSWITCH_LINK_WR32(_d, _physlinknum, MINION, _dev, _reg, _data)

#define NVSWITCH_MINION_LINK_RD32_LS10(_d, _physlinknum, _dev, _reg) \
    NVSWITCH_LINK_RD32(_d, _physlinknum, MINION, _dev, _reg)

//
// MINION
//

typedef const struct
{
    NvU32 osCodeOffset;
    NvU32 osCodeSize;
    NvU32 osDataOffset;
    NvU32 osDataSize;
    NvU32 numApps;
    NvU32 appCodeStart;
    NvU32 appDataStart;
    NvU32 codeOffset;
    NvU32 codeSize;
    NvU32 dataOffset;
    NvU32 dataSize;
} FALCON_UCODE_HDR_INFO_LS10, *PFALCON_UCODE_HDR_INFO_LS10;

typedef const struct
{
      //
      // Version 1
      // Version 2
      // Vesrion 3 = for Partition boot
      // Vesrion 4 = for eb riscv boot
      //
      NvU32  version;                         // structure version
      NvU32  bootloaderOffset;
      NvU32  bootloaderSize;
      NvU32  bootloaderParamOffset;
      NvU32  bootloaderParamSize;
      NvU32  riscvElfOffset;
      NvU32  riscvElfSize;
      NvU32  appVersion;                      // Changelist number associated with the image
      //
      // Manifest contains information about Monitor and it is
      // input to BR
      //
      NvU32  manifestOffset;
      NvU32  manifestSize;
      //
      // Monitor Data offset within RISCV image and size
      //
      NvU32  monitorDataOffset;
      NvU32  monitorDataSize;
      //
      // Monitor Code offset withtin RISCV image and size
      //
      NvU32  monitorCodeOffset;
      NvU32  monitorCodeSize;
      NvU32  bIsMonitorEnabled;
      //
      // Swbrom Code offset within RISCV image and size
      //
      NvU32  swbromCodeOffset;
      NvU32  swbromCodeSize;
      //
      // Swbrom Data offset within RISCV image and size
      //
      NvU32  swbromDataOffset;
      NvU32  swbromDataSize;
} RISCV_UCODE_HDR_INFO_LS10, *PRISCV_UCODE_HDR_INFO_LS10;

//
// defines used by internal ls10 functions to get
// specific clock status
//
#define NVSWITCH_PER_LINK_CLOCK_RXCLK     0
#define NVSWITCH_PER_LINK_CLOCK_TXCLK     1
#define NVSWITCH_PER_LINK_CLOCK_NCISOCCLK 2
#define NVSWITCH_PER_LINK_CLOCK_NUM       3
#define NVSWITCH_PER_LINK_CLOCK_SET(_name) BIT(NVSWITCH_PER_LINK_CLOCK_##_name)
//
// HAL functions shared by LR10 and used by LS10
//

#define nvswitch_is_link_valid_ls10                 nvswitch_is_link_valid_lr10
#define nvswitch_is_link_in_use_ls10                nvswitch_is_link_in_use_lr10

#define nvswitch_deassert_link_reset_ls10           nvswitch_deassert_link_reset_lr10
#define nvswitch_determine_platform_ls10            nvswitch_determine_platform_lr10
#define nvswitch_get_swap_clk_default_ls10          nvswitch_get_swap_clk_default_lr10
#define nvswitch_post_init_device_setup_ls10        nvswitch_post_init_device_setup_lr10
#define nvswitch_set_training_error_info_ls10       nvswitch_set_training_error_info_lr10
#define nvswitch_init_scratch_ls10                  nvswitch_init_scratch_lr10
#define nvswitch_hw_counter_shutdown_ls10           nvswitch_hw_counter_shutdown_lr10
#define nvswitch_hw_counter_read_counter_ls10       nvswitch_hw_counter_read_counter_lr10

#define nvswitch_ecc_writeback_task_ls10            nvswitch_ecc_writeback_task_lr10
#define nvswitch_ctrl_get_routing_id_ls10           nvswitch_ctrl_get_routing_id_lr10
#define nvswitch_ctrl_set_routing_id_valid_ls10     nvswitch_ctrl_set_routing_id_valid_lr10
#define nvswitch_ctrl_set_routing_id_ls10           nvswitch_ctrl_set_routing_id_lr10
#define nvswitch_ctrl_set_routing_lan_ls10          nvswitch_ctrl_set_routing_lan_lr10
#define nvswitch_ctrl_get_routing_lan_ls10          nvswitch_ctrl_get_routing_lan_lr10
#define nvswitch_ctrl_set_routing_lan_valid_ls10    nvswitch_ctrl_set_routing_lan_valid_lr10
#define nvswitch_ctrl_set_ingress_request_table_ls10    nvswitch_ctrl_set_ingress_request_table_lr10
#define nvswitch_ctrl_get_ingress_request_table_ls10    nvswitch_ctrl_get_ingress_request_table_lr10
#define nvswitch_ctrl_set_ingress_request_valid_ls10    nvswitch_ctrl_set_ingress_request_valid_lr10
#define nvswitch_ctrl_get_ingress_response_table_ls10   nvswitch_ctrl_get_ingress_response_table_lr10
#define nvswitch_ctrl_set_ingress_response_table_ls10   nvswitch_ctrl_set_ingress_response_table_lr10

#define nvswitch_ctrl_get_info_ls10                 nvswitch_ctrl_get_info_lr10

#define nvswitch_ctrl_set_switch_port_config_ls10   nvswitch_ctrl_set_switch_port_config_lr10
#define nvswitch_ctrl_get_throughput_counters_ls10  nvswitch_ctrl_get_throughput_counters_lr10

#define nvswitch_save_nvlink_seed_data_from_minion_to_inforom_ls10  nvswitch_save_nvlink_seed_data_from_minion_to_inforom_lr10
#define nvswitch_store_seed_data_from_inforom_to_corelib_ls10       nvswitch_store_seed_data_from_inforom_to_corelib_lr10
#define nvswitch_corelib_clear_link_state_ls10  nvswitch_corelib_clear_link_state_lr10

#define nvswitch_read_oob_blacklist_state_ls10      nvswitch_read_oob_blacklist_state_lr10

#define nvswitch_corelib_add_link_ls10              nvswitch_corelib_add_link_lr10
#define nvswitch_corelib_remove_link_ls10           nvswitch_corelib_remove_link_lr10
#define nvswitch_corelib_set_tl_link_mode_ls10      nvswitch_corelib_set_tl_link_mode_lr10
#define nvswitch_corelib_set_rx_mode_ls10           nvswitch_corelib_set_rx_mode_lr10
#define nvswitch_corelib_set_rx_detect_ls10         nvswitch_corelib_set_rx_detect_lr10
#define nvswitch_corelib_write_discovery_token_ls10 nvswitch_corelib_write_discovery_token_lr10
#define nvswitch_corelib_read_discovery_token_ls10  nvswitch_corelib_read_discovery_token_lr10

#define nvswitch_inforom_ecc_log_error_event_ls10   nvswitch_inforom_ecc_log_error_event_lr10
#define nvswitch_inforom_ecc_get_errors_ls10        nvswitch_inforom_ecc_get_errors_lr10
#define nvswitch_inforom_bbx_get_sxid_ls10          nvswitch_inforom_bbx_get_sxid_lr10

#define nvswitch_vbios_read_structure_ls10          nvswitch_vbios_read_structure_lr10

#define nvswitch_setup_system_registers_ls10        nvswitch_setup_system_registers_lr10

#define nvswitch_minion_get_initoptimize_status_ls10 nvswitch_minion_get_initoptimize_status_lr10

#define nvswitch_poll_sublink_state_ls10             nvswitch_poll_sublink_state_lr10
#define nvswitch_setup_link_loopback_mode_ls10       nvswitch_setup_link_loopback_mode_lr10

#define nvswitch_link_lane_reversed_ls10             nvswitch_link_lane_reversed_lr10

#define nvswitch_i2c_set_hw_speed_mode_ls10         nvswitch_i2c_set_hw_speed_mode_lr10

#define nvswitch_ctrl_get_err_info_ls10             nvswitch_ctrl_get_err_info_lr10

NvlStatus nvswitch_ctrl_get_err_info_lr10(nvswitch_device *device, NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS *ret);

NvBool    nvswitch_is_link_valid_lr10(nvswitch_device *device, NvU32 link_id);
NvBool    nvswitch_is_link_in_use_lr10(nvswitch_device *device, NvU32 link_id);

NvlStatus nvswitch_initialize_device_state_lr10(nvswitch_device *device);
NvlStatus nvswitch_deassert_link_reset_lr10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_determine_platform_lr10(nvswitch_device *device);
NvU32     nvswitch_get_swap_clk_default_lr10(nvswitch_device *device);
NvlStatus nvswitch_post_init_device_setup_lr10(nvswitch_device *device);
NvlStatus nvswitch_set_training_error_info_lr10(nvswitch_device *device, NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS *pLinkTrainingErrorInfoParams);
void      nvswitch_init_scratch_lr10(nvswitch_device *device);
void      nvswitch_hw_counter_shutdown_lr10(nvswitch_device *device);
NvU64     nvswitch_hw_counter_read_counter_lr10(nvswitch_device *device);

void      nvswitch_ecc_writeback_task_lr10(nvswitch_device *device);
NvlStatus nvswitch_ctrl_get_routing_id_lr10(nvswitch_device *device, NVSWITCH_GET_ROUTING_ID_PARAMS *params);
NvlStatus nvswitch_ctrl_set_routing_id_valid_lr10(nvswitch_device *device, NVSWITCH_SET_ROUTING_ID_VALID *p);
NvlStatus nvswitch_ctrl_set_routing_id_lr10(nvswitch_device *device, NVSWITCH_SET_ROUTING_ID *p);
NvlStatus nvswitch_ctrl_set_routing_lan_lr10(nvswitch_device *device, NVSWITCH_SET_ROUTING_LAN *p);
NvlStatus nvswitch_ctrl_get_routing_lan_lr10(nvswitch_device *device, NVSWITCH_GET_ROUTING_LAN_PARAMS *params);
NvlStatus nvswitch_ctrl_set_routing_lan_valid_lr10(nvswitch_device *device, NVSWITCH_SET_ROUTING_LAN_VALID *p);
NvlStatus nvswitch_ctrl_set_ingress_request_table_lr10(nvswitch_device *device, NVSWITCH_SET_INGRESS_REQUEST_TABLE *p);
NvlStatus nvswitch_ctrl_get_ingress_request_table_lr10(nvswitch_device *device, NVSWITCH_GET_INGRESS_REQUEST_TABLE_PARAMS *params);
NvlStatus nvswitch_ctrl_set_ingress_request_valid_lr10(nvswitch_device *device, NVSWITCH_SET_INGRESS_REQUEST_VALID *p);
NvlStatus nvswitch_ctrl_get_ingress_response_table_lr10(nvswitch_device *device, NVSWITCH_GET_INGRESS_RESPONSE_TABLE_PARAMS *params);
NvlStatus nvswitch_ctrl_set_ingress_response_table_lr10(nvswitch_device *device, NVSWITCH_SET_INGRESS_RESPONSE_TABLE *p);

NvlStatus nvswitch_ctrl_get_nvlink_status_lr10(nvswitch_device *device, NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret);
NvlStatus nvswitch_ctrl_get_nvlink_status_ls10(nvswitch_device *device, NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret);

NvlStatus nvswitch_ctrl_get_info_lr10(nvswitch_device *device, NVSWITCH_GET_INFO *p);

NvlStatus nvswitch_ctrl_set_switch_port_config_lr10(nvswitch_device *device, NVSWITCH_SET_SWITCH_PORT_CONFIG *p);
NvlStatus nvswitch_ctrl_get_throughput_counters_lr10(nvswitch_device *device, NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *p);
void      nvswitch_save_nvlink_seed_data_from_minion_to_inforom_lr10(nvswitch_device *device, NvU32 linkId);
void      nvswitch_store_seed_data_from_inforom_to_corelib_lr10(nvswitch_device *device);
NvlStatus nvswitch_read_oob_blacklist_state_lr10(nvswitch_device *device);

NvlStatus nvswitch_corelib_add_link_lr10(nvlink_link *link);
NvlStatus nvswitch_corelib_remove_link_lr10(nvlink_link *link);
NvlStatus nvswitch_corelib_get_dl_link_mode_lr10(nvlink_link *link, NvU64 *mode);
NvlStatus nvswitch_corelib_set_tl_link_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_tx_mode_lr10(nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus nvswitch_corelib_set_rx_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_rx_mode_lr10(nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus nvswitch_corelib_set_rx_detect_lr10(nvlink_link *link, NvU32 flags);
NvlStatus nvswitch_corelib_write_discovery_token_lr10(nvlink_link *link, NvU64 token);
NvlStatus nvswitch_corelib_read_discovery_token_lr10(nvlink_link *link, NvU64 *token);
NvlStatus nvswitch_corelib_set_dl_link_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_set_tx_mode_lr10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_get_tl_link_mode_lr10(nvlink_link *link, NvU64 *mode);
void      nvswitch_init_buffer_ready_lr10(nvswitch_device *device, nvlink_link *link, NvBool bNportBufferReady);

NvlStatus nvswitch_inforom_ecc_log_error_event_lr10(nvswitch_device *device, INFOROM_ECC_OBJECT *pEccGeneric, INFOROM_NVS_ECC_ERROR_EVENT *err_event);
NvlStatus nvswitch_inforom_ecc_get_errors_lr10(nvswitch_device *device, NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *params);
NvlStatus nvswitch_inforom_bbx_get_sxid_lr10(nvswitch_device *device, NVSWITCH_GET_SXIDS_PARAMS *params);

void      nvswitch_init_dlpl_interrupts_lr10(nvlink_link *link);

NvlStatus nvswitch_vbios_read_structure_lr10(nvswitch_device *device, void *structure, NvU32 offset, NvU32 *ppacked_size, const char *format);

NvlStatus nvswitch_setup_system_registers_lr10(nvswitch_device *device);

NvlStatus nvswitch_minion_get_initoptimize_status_lr10(nvswitch_device *device, NvU32 linkId);

NvlStatus nvswitch_poll_sublink_state_lr10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_setup_link_loopback_mode_lr10(nvswitch_device *device, NvU32 linkNumber);

NvBool    nvswitch_link_lane_reversed_lr10(nvswitch_device *device, NvU32 linkId);
void nvswitch_store_topology_information_lr10(nvswitch_device *device, nvlink_link *link);

NvlStatus nvswitch_request_tl_link_state_lr10(nvlink_link *link, NvU32 tlLinkState, NvBool bSync);
NvlStatus nvswitch_wait_for_tl_request_ready_lr10(nvlink_link *link);

NvlStatus nvswitch_parse_bios_image_lr10(nvswitch_device *device);
NvU32 nvswitch_i2c_get_port_info_ls10(nvswitch_device *device, NvU32 port);
NvU32 nvswitch_i2c_get_port_info_lr10(nvswitch_device *device, NvU32 port);
void      nvswitch_i2c_set_hw_speed_mode_lr10(nvswitch_device *device, NvU32 port, NvU32 speedMode);
NvlStatus nvswitch_ctrl_i2c_indexed_lr10(nvswitch_device *device, NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams);
void   nvswitch_corelib_clear_link_state_lr10(nvlink_link *link);

//
// Internal function declarations
//

NvlStatus nvswitch_corelib_set_dl_link_mode_ls10(nvlink_link *link, NvU64 mode, NvU32 flags);
NvlStatus nvswitch_corelib_set_tx_mode_ls10(nvlink_link *link, NvU64 mode, NvU32 flags);
void nvswitch_init_lpwr_regs_ls10(nvlink_link *link);
void nvswitch_program_l1_scratch_reg_ls10(nvswitch_device *device, NvU32 linkNumber);

NvlStatus nvswitch_minion_service_falcon_interrupts_ls10(nvswitch_device *device, NvU32 instance);

NvlStatus nvswitch_device_discovery_ls10(nvswitch_device *device, NvU32 discovery_offset);
void nvswitch_filter_discovery_ls10(nvswitch_device *device);
NvlStatus nvswitch_process_discovery_ls10(nvswitch_device *device);
void nvswitch_lib_enable_interrupts_ls10(nvswitch_device *device);
void nvswitch_lib_disable_interrupts_ls10(nvswitch_device *device);
NvlStatus nvswitch_lib_service_interrupts_ls10(nvswitch_device *device);
NvlStatus nvswitch_lib_check_interrupts_ls10(nvswitch_device *device);
void      nvswitch_initialize_interrupt_tree_ls10(nvswitch_device *device);
void      nvswitch_corelib_training_complete_ls10(nvlink_link *link);
NvlStatus nvswitch_init_nport_ls10(nvswitch_device *device);
NvlStatus nvswitch_corelib_get_rx_detect_ls10(nvlink_link *link);
void nvswitch_reset_persistent_link_hw_state_ls10(nvswitch_device *device, NvU32 linkNumber);
NvlStatus nvswitch_minion_get_rxdet_status_ls10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_minion_restore_seed_data_ls10(nvswitch_device *device, NvU32 linkId, NvU32 *seedData);
NvlStatus nvswitch_minion_set_sim_mode_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_minion_set_smf_settings_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_minion_select_uphy_tables_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_set_training_mode_ls10(nvswitch_device *device);
NvlStatus nvswitch_corelib_get_tl_link_mode_ls10(nvlink_link *link, NvU64 *mode);
NvU32     nvswitch_get_sublink_width_ls10(nvswitch_device *device,NvU32 linkNumber);
NvlStatus nvswitch_parse_bios_image_ls10(nvswitch_device *device);
NvBool    nvswitch_is_link_in_reset_ls10(nvswitch_device *device, nvlink_link *link);
void nvswitch_corelib_get_uphy_load_ls10(nvlink_link *link, NvBool *bUnlocked);
NvlStatus nvswitch_ctrl_get_nvlink_lp_counters_ls10(nvswitch_device *device, NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS *params);
void      nvswitch_init_buffer_ready_ls10(nvswitch_device *device, nvlink_link *link, NvBool bNportBufferReady);
void      nvswitch_apply_recal_settings_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_corelib_get_dl_link_mode_ls10(nvlink_link *link, NvU64 *mode);
NvlStatus nvswitch_corelib_get_tx_mode_ls10(nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus nvswitch_corelib_get_rx_mode_ls10(nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus nvswitch_ctrl_get_sw_info_ls10(nvswitch_device *device, NVSWITCH_GET_SW_INFO_PARAMS *p);
NvlStatus nvswitch_launch_ALI_link_training_ls10(nvswitch_device *device, nvlink_link *link, NvBool bSync);
NvlStatus nvswitch_service_nvldl_fatal_link_ls10(nvswitch_device *device, NvU32 nvliptInstance, NvU32 link);
NvlStatus nvswitch_ctrl_inband_send_data_ls10(nvswitch_device *device, NVSWITCH_INBAND_SEND_DATA_PARAMS *p);
NvlStatus nvswitch_ctrl_inband_read_data_ls10(nvswitch_device *device, NVSWITCH_INBAND_READ_DATA_PARAMS *p);
void      nvswitch_send_inband_nack_ls10(nvswitch_device *device, NvU32 *msghdr, NvU32  linkId);
NvU32     nvswitch_get_max_persistent_message_count_ls10(nvswitch_device *device);
NvlStatus nvswitch_service_minion_link_ls10(nvswitch_device *device, NvU32 nvliptInstance);
void      nvswitch_apply_recal_settings_ls10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_store_topology_information_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_ctrl_i2c_indexed_ls10(nvswitch_device *device, NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams);
NvBool    nvswitch_i2c_is_device_access_allowed_ls10(nvswitch_device *device, NvU32 port, NvU8 addr, NvBool bIsRead);
NvlStatus nvswitch_minion_get_ali_debug_registers_ls10(nvswitch_device *device, nvlink_link *link, NVSWITCH_MINION_ALI_DEBUG_REGISTERS *params);
void      nvswitch_execute_unilateral_link_shutdown_ls10(nvlink_link *link);
void      nvswitch_setup_link_system_registers_ls10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_load_link_disable_settings_ls10(nvswitch_device *device, nvlink_link *link);
void      nvswitch_link_disable_interrupts_ls10(nvswitch_device *device, NvU32 link);
void      nvswitch_init_dlpl_interrupts_ls10(nvlink_link *link);
void      nvswitch_set_dlpl_interrupts_ls10(nvlink_link *link);
void      nvswitch_service_minion_all_links_ls10(nvswitch_device *device);
NvlStatus nvswitch_ctrl_get_board_part_number_ls10(nvswitch_device *device, NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR *p);
void      nvswitch_create_deferred_link_state_check_task_ls10(nvswitch_device *device, NvU32 nvlipt_instance, NvU32 link);
NvlStatus nvswitch_request_tl_link_state_ls10(nvlink_link *link, NvU32 tlLinkState, NvBool bSync);
NvlStatus nvswitch_ctrl_get_link_l1_capability_ls10(nvswitch_device *device, NvU32 linkId, NvBool *isL1Capable);
NvlStatus nvswitch_ctrl_get_link_l1_threshold_ls10(nvswitch_device *device, NvU32 linkNum, NvU32 *lpThreshold);
NvlStatus nvswitch_ctrl_set_link_l1_threshold_ls10(nvlink_link *link, NvU32 lpEntryThreshold);
NvlStatus nvswitch_get_board_id_ls10(nvswitch_device *device, NvU16 *boardId);

//
// SU generated functions
//

NvlStatus nvswitch_nvs_top_prod_ls10(nvswitch_device *device);
NvlStatus nvswitch_apply_prod_nvlw_ls10(nvswitch_device *device);
NvlStatus nvswitch_apply_prod_nxbar_ls10(nvswitch_device *device);

NvlStatus nvswitch_launch_ALI_ls10(nvswitch_device *device);

NvlStatus nvswitch_ctrl_set_mc_rid_table_ls10(nvswitch_device *device, NVSWITCH_SET_MC_RID_TABLE_PARAMS *p);
NvlStatus nvswitch_ctrl_get_mc_rid_table_ls10(nvswitch_device *device, NVSWITCH_GET_MC_RID_TABLE_PARAMS *p);

void      nvswitch_service_minion_all_links_ls10(nvswitch_device *device);

NvBool    nvswitch_is_inforom_supported_ls10(nvswitch_device *device);
void      nvswitch_set_error_rate_threshold_ls10(nvlink_link *link, NvBool bIsDefault);
void      nvswitch_configure_error_rate_threshold_interrupt_ls10(nvlink_link *link, NvBool bEnable);
NvlStatus nvswitch_reset_and_train_link_ls10(nvswitch_device *device, nvlink_link *link);
NvBool    nvswitch_are_link_clocks_on_ls10(nvswitch_device *device, nvlink_link *link, NvU32 clocksMask);
NvBool    nvswitch_does_link_need_termination_enabled_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_link_termination_setup_ls10(nvswitch_device *device, nvlink_link* link);
void      nvswitch_get_error_rate_threshold_ls10(nvlink_link *link);
void      nvswitch_fsp_update_cmdq_head_tail_ls10(nvswitch_device  *device, NvU32 queueHead, NvU32 queueTail);
void      nvswitch_fsp_get_cmdq_head_tail_ls10(nvswitch_device  *device, NvU32 *pQueueHead, NvU32 *pQueueTail);
void      nvswitch_fsp_update_msgq_head_tail_ls10(nvswitch_device *device, NvU32 msgqHead, NvU32 msgqTail);
void      nvswitch_fsp_get_msgq_head_tail_ls10(nvswitch_device *device, NvU32 *pMsgqHead, NvU32 *pMsgqTail);
NvU32     nvswitch_fsp_get_channel_size_ls10(nvswitch_device *device);
NvU8      nvswitch_fsp_nvdm_to_seid_ls10(nvswitch_device *device, NvU8 nvdmType);
NvU32     nvswitch_fsp_create_mctp_header_ls10(nvswitch_device *device, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq);
NvU32     nvswitch_fsp_create_nvdm_header_ls10(nvswitch_device *device, NvU32 nvdmType);
NvlStatus nvswitch_fsp_get_packet_info_ls10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size, NvU8 *pPacketState, NvU8 *pTag);
NvlStatus nvswitch_fsp_validate_mctp_payload_header_ls10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_process_nvdm_msg_ls10(nvswitch_device  *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_process_cmd_response_ls10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_config_ememc_ls10(nvswitch_device *device, NvU32 offset, NvBool bAincw, NvBool bAincr);
NvlStatus nvswitch_fsp_write_to_emem_ls10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_read_from_emem_ls10(nvswitch_device *device, NvU8 *pBuffer, NvU32 size);
NvlStatus nvswitch_fsp_error_code_to_nvlstatus_map_ls10(nvswitch_device *device, NvU32 errorCode);
NvlStatus nvswitch_fsprpc_get_caps_ls10(nvswitch_device *device, NVSWITCH_FSPRPC_GET_CAPS_PARAMS *params);
NvlStatus nvswitch_detect_tnvl_mode_ls10(nvswitch_device *device);
NvBool    nvswitch_is_tnvl_mode_enabled_ls10(nvswitch_device *device);
NvBool    nvswitch_is_tnvl_mode_locked_ls10(nvswitch_device *device);
NvlStatus nvswitch_tnvl_get_attestation_certificate_chain_ls10(nvswitch_device *device, NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS *params);
NvlStatus nvswitch_tnvl_get_attestation_report_ls10(nvswitch_device *device, NVSWITCH_GET_ATTESTATION_REPORT_PARAMS *params);
NvlStatus nvswitch_tnvl_send_fsp_lock_config_ls10(nvswitch_device *device);
NvlStatus nvswitch_tnvl_get_status_ls10(nvswitch_device *device, NVSWITCH_GET_TNVL_STATUS_PARAMS *params);
void      nvswitch_tnvl_eng_wr_32_ls10(nvswitch_device *device, NVSWITCH_ENGINE_ID eng_id, NvU32 eng_bcast, NvU32 eng_instance, NvU32 base_addr, NvU32 offset, NvU32 data);
NvlStatus nvswitch_send_tnvl_prelock_cmd_ls10(nvswitch_device *device);
void      nvswitch_tnvl_disable_interrupts_ls10(nvswitch_device *device);
void      nvswitch_tnvl_reg_wr_32_ls10(nvswitch_device *device, NvU32 offset, NvU32 data);
NvlStatus nvswitch_ctrl_get_soe_heartbeat_ls10(nvswitch_device *device, NVSWITCH_GET_SOE_HEARTBEAT_PARAMS *p);
NvlStatus nvswitch_cci_enable_iobist_ls10(nvswitch_device *device, NvU32 linkNumber, NvBool bEnable);
NvlStatus nvswitch_cci_initialization_sequence_ls10(nvswitch_device *device, NvU32 linkNumber);
NvlStatus nvswitch_cci_deinitialization_sequence_ls10(nvswitch_device *device, NvU32 linkNumber);
void      nvswitch_update_link_state_led_ls10(nvswitch_device *device);
void      nvswitch_led_shutdown_ls10(nvswitch_device *device);

#endif //_LS10_H_

