/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _IO_NVSWITCH_H_
#define _IO_NVSWITCH_H_

#include "nv_list.h"
#include "ctrl_dev_nvswitch.h"

// NVSWITCH_REG_* MMIO wrappers are to be used for absolute symbolic BAR0 offset
// register  references like SMC, CLOCK, BUS, and PRIV_MASTER.
//

#define NVSWITCH_REG_RD32(_d, _dev, _reg)               \
    (                                                   \
        NVSWITCH_PRINT(_d, MMIO,                        \
            "%s: MEM_RD: %s, %s (+%04x)\n",             \
            __FUNCTION__,                               \
            #_dev, #_reg, NV ## _dev ## _reg)           \
    ,                                                   \
        nvswitch_reg_read_32(_d, NV##_dev##_reg)        \
    );                                                  \
    ((void)(_d))

#define NVSWITCH_REG_WR32(_d, _dev, _reg, _data)        \
    NVSWITCH_PRINT(_d, MMIO,                            \
        "%s: MEM_WR: %s, %s (+%04x) 0x%08x\n",          \
        __FUNCTION__,                                   \
        #_dev, #_reg, NV ## _dev ## _reg, _data);       \
    nvswitch_reg_write_32(_d, NV##_dev##_reg, _data);   \
    ((void)(_d))

//
// NVSWITCH_OFF_* MMIO wrappers are used to access a fully formed BAR0 offset.
//

#define NVSWITCH_OFF_RD32(_d, _off)                 \
    nvswitch_reg_read_32(_d, _off);                 \
    ((void)(_d))

#define NVSWITCH_OFF_WR32(_d, _off, _data)          \
    nvswitch_reg_write_32(_d, _off, _data);         \
    ((void)(_d))

#define NVSWITCH_ENGINE_DESCRIPTOR_UC_SIZE      64
#define NVSWITCH_ENGINE_DESCRIPTOR_MC_SIZE      3

#define NVSWITCH_ENGINE_INSTANCE_INVALID        ((NvU32) (~0))

typedef struct engine_descriptor
{
    const char *eng_name;
    NvU32 eng_id;           // REGISTER_RW_ENGINE_*
    NvU32 eng_count;
    NvU32 uc_addr[NVSWITCH_ENGINE_DESCRIPTOR_UC_SIZE];
    NvU32 bc_addr;
    NvU32 mc_addr[NVSWITCH_ENGINE_DESCRIPTOR_MC_SIZE];
    NvU32 mc_addr_count;
} NVSWITCH_ENGINE_DESCRIPTOR_TYPE;

#define NVSWITCH_DECLARE_IO_DESCRIPTOR(_engine, _bcast)    \
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE     _engine;

#define NVSWITCH_BASE_ADDR_INVALID          ((NvU32) (~0))

//
// All IP-based (0-based register manuals) engines that ever existed on *ANY*
// architecture(s) must be listed here in order to use the common IO wrappers.
// New engines need to be added here as well as in the chip-specific lists in
// their respective headers that generate chip-specific handlers.
// Absolute BAR0 offset-based units are legacy units in which the unit's offset
// in BAR0 is included in the register definition in the manuals.  For these
// legacy units the discovered base is not used since it is already part of the
// register.  Legacy units (e.g. PSMC, CLOCK, BUS, and PRIV_MASTER) should use
// NVSWITCH_REG_RD/WR IO wrappers.
//

#define NVSWITCH_LIST_LS10_ONLY_ENGINES(_op)     \
    _op(GIN)                                \
    _op(XAL)                                \
    _op(XAL_FUNC)                           \
    _op(XPL)                                \
    _op(XTL)                                \
    _op(XTL_CONFIG)                         \
    _op(UXL)                                \
    _op(GPU_PTOP)                           \
    _op(PMC)                                \
    _op(PBUS)                               \
    _op(ROM2)                               \
    _op(GPIO)                               \
    _op(FSP)                                \
    _op(SYSCTRL)                            \
    _op(CLKS_SYS)                           \
    _op(CLKS_SYSB)                          \
    _op(CLKS_P0)                            \
    _op(SAW_PM)                             \
    _op(PCIE_PM)                            \
    _op(PRT_PRI_HUB)                        \
    _op(PRT_PRI_RS_CTRL)                    \
    _op(SYS_PRI_HUB)                        \
    _op(SYS_PRI_RS_CTRL)                    \
    _op(SYSB_PRI_HUB)                       \
    _op(SYSB_PRI_RS_CTRL)                   \
    _op(PRI_MASTER_RS)                      \
    _op(PTIMER)                             \
    _op(CPR)                                \
    _op(TILEOUT)                            \

#define NVSWITCH_LIST_ALL_ENGINES(_op)      \
    _op(XVE)                                \
    _op(SAW)                                \
    _op(SOE)                                \
    _op(SMR)                                \
                                            \
    _op(NPG)                                \
    _op(NPORT)                              \
                                            \
    _op(NVLW)                               \
    _op(MINION)                             \
    _op(NVLIPT)                             \
    _op(NVLIPT_LNK)                         \
    _op(NVLTLC)                             \
    _op(NVLDL)                              \
                                            \
    _op(NXBAR)                              \
    _op(TILE)                               \
                                            \
    _op(NPG_PERFMON)                        \
    _op(NPORT_PERFMON)                      \
                                            \
    _op(NVLW_PERFMON)                       \

#define ENGINE_ID_LIST(_eng)                \
    NVSWITCH_ENGINE_ID_##_eng,

//
// ENGINE_IDs are the complete list of all engines that are supported on
// *ANY* architecture(s) that may support them.  Any one architecture may or
// may not understand how to operate on any one specific engine.
// Architectures that share a common ENGINE_ID are not guaranteed to have
// compatible manuals.
//
typedef enum nvswitch_engine_id
{
    NVSWITCH_LIST_ALL_ENGINES(ENGINE_ID_LIST)
    NVSWITCH_LIST_LS10_ONLY_ENGINES(ENGINE_ID_LIST)
    NVSWITCH_ENGINE_ID_SIZE,
} NVSWITCH_ENGINE_ID;

//
// NVSWITCH_ENG_* MMIO wrappers are to be used for top level discovered
// devices like SAW, FUSE, PMGR, XVE, etc.
//

#define NVSWITCH_GET_ENG_DESC_TYPE              0
#define NVSWITCH_GET_ENG_DESC_TYPE_UNICAST      NVSWITCH_GET_ENG_DESC_TYPE
#define NVSWITCH_GET_ENG_DESC_TYPE_BCAST        1
#define NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST    2

#define NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx)                 \
    ((_d)->hal.nvswitch_get_eng_base(                               \
        _d,                                                         \
        NVSWITCH_ENGINE_ID_##_eng,                                  \
        NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                         \
        _engidx))

#define NVSWITCH_ENG_COUNT(_d, _eng, _bcast)                        \
    ((_d)->hal.nvswitch_get_eng_count(                              \
        _d,                                                         \
        NVSWITCH_ENGINE_ID_##_eng,                                  \
        NVSWITCH_GET_ENG_DESC_TYPE##_bcast))

#define NVSWITCH_ENG_IS_VALID(_d, _eng, _engidx)                    \
    (                                                               \
        NVSWITCH_GET_ENG(_d, _eng, , _engidx) != NVSWITCH_BASE_ADDR_INVALID \
    )

#define NVSWITCH_ENG_WR32(_d, _eng, _bcast, _engidx, _dev, _reg, _data) \
    {                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: MEM_WR %s[%d]: %s, %s (%06x+%04x) 0x%08x\n",       \
            __FUNCTION__,                                           \
            #_eng#_bcast, _engidx,                                  \
            #_dev, #_reg,                                           \
            NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx),            \
            NV ## _dev ## _reg, _data);                             \
                                                                    \
        ((_d)->hal.nvswitch_eng_wr(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                     \
            _engidx,                                                \
            NV ## _dev ## _reg, _data));                            \
    }

#define NVSWITCH_ENG_RD32(_d, _eng, _bcast, _engidx, _dev, _reg)    \
    (                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: MEM_RD %s[%d]: %s, %s (%06x+%04x)\n",              \
            __FUNCTION__,                                           \
            #_eng#_bcast, _engidx,                                  \
            #_dev, #_reg,                                           \
            NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx),            \
            NV ## _dev ## _reg)                                     \
    ,                                                               \
        ((_d)->hal.nvswitch_eng_rd(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                     \
            _engidx,                                                \
            NV ## _dev ## _reg))                                    \
    );                                                              \
    ((void)(_d))

#define NVSWITCH_ENG_WR32_IDX(_d, _eng, _bcast, _engidx, _dev, _reg, _idx, _data) \
    {                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: MEM_WR %s[%d]: %s, %s(%d) (%06x+%04x) 0x%08x\n",   \
            __FUNCTION__,                                           \
            #_eng#_bcast, _engidx,                                  \
            #_dev, #_reg, _idx,                                     \
            NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx),            \
            NV ## _dev ## _reg(_idx), _data);                       \
                                                                    \
        ((_d)->hal.nvswitch_eng_wr(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                     \
            _engidx,                                                \
            NV ## _dev ## _reg(_idx), _data));                      \
    }

#define NVSWITCH_ENG_RD32_IDX(_d, _eng, _bcast, _engidx, _dev, _reg, _idx)  \
    (                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: MEM_RD %s[%d]: %s, %s(%d) (%06x+%04x)\n",          \
            __FUNCTION__,                                           \
            #_eng#_bcast, _engidx,                                  \
            #_dev, #_reg, _idx,                                     \
            NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx),            \
            NV ## _dev ## _reg(_idx))                               \
    ,                                                               \
        ((_d)->hal.nvswitch_eng_rd(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                     \
            _engidx,                                                \
            NV ## _dev ## _reg(_idx)))                              \
    );                                                              \
    ((void)(_d))

#define NVSWITCH_ENG_OFF_WR32(_d, _eng, _bcast, _engidx, _offset, _data) \
    {                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: MEM_WR %s[%d]: 0x%x (%06x+%04x) 0x%08x\n",         \
            __FUNCTION__,                                           \
            #_eng#_bcast, _engidx,                                  \
            _offset,                                                \
            NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx),            \
            _offset, _data);                                        \
        ((_d)->hal.nvswitch_eng_wr(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                     \
            _engidx,                                                \
            _offset, _data));                                       \
    }

#define NVSWITCH_ENG_OFF_RD32(_d, _eng, _bcast, _engidx, _offset)   \
    (                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: MEM_RD %s[%d]: 0x%x (%06x+%04x)\n",                \
            __FUNCTION__,                                           \
            #_eng#_bcast, _engidx,                                  \
            _offset,                                                \
            NVSWITCH_GET_ENG(_d, _eng, _bcast, _engidx),            \
            _offset)                                                \
    ,                                                               \
        ((_d)->hal.nvswitch_eng_rd(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE##_bcast,                     \
            _engidx,                                                \
            _offset))                                               \
    )

//
// Per-link information
//

#define NVSWITCH_MAX_LINK_COUNT             64

#define NVSWITCH_MAX_SEED_BUFFER_SIZE         NVSWITCH_MAX_SEED_NUM + 1

//
// Storing list entries for sending to FM 
//
typedef struct
{
    NVListRec entry;
    NvU8      data[NVSWITCH_INBAND_DATA_SIZE];
    NvU32     dataSize;
} nvswitch_inband_data_list;

typedef struct
{
    // Temp entry being received
    nvswitch_inband_data_list *message;

    //
    // Persistent messages are stored even if the listener (e.g. FM) is
    // not present.
    //
    NVListRec persistent_list;

    // Stores messages if and only if the listener is present.
    NVListRec nonpersistent_list;
} nvswitch_inband_receive_data;

typedef struct
{
    NvBool valid;
    NvU32  link_clock_khz;

    NvBool fatal_error_occurred;
    NvBool ingress_packet_latched;
    NvBool egress_packet_latched;

    NvBool nea;    // Near end analog
    NvBool nedr;   // Near end digital
    NvBool nedw;

    NvU32  lane_rxdet_status_mask;

    NvBool bIsRepeaterMode;

    // Minion Inband Data structure
    nvswitch_inband_receive_data inbandData;

} NVSWITCH_LINK_TYPE;

//
// Per link register access routines
// LINK_* MMIO wrappers are used to reference per-link engine instances
//

#define NVSWITCH_LINK_COUNT(_d)                                         \
    (nvswitch_get_num_links(_d))

#define NVSWITCH_GET_LINK_ENG_INST(_d, _linknum, _eng)                  \
    nvswitch_get_link_eng_inst(_d, _linknum, NVSWITCH_ENGINE_ID_##_eng)

#define NVSWITCH_IS_LINK_ENG_VALID(_d, _linknum, _eng)                  \
    (                                                                   \
        (NVSWITCH_GET_ENG(_d, _eng, ,                                   \
            NVSWITCH_GET_LINK_ENG_INST(_d, _linknum, _eng))             \
            != NVSWITCH_BASE_ADDR_INVALID) &&                           \
        nvswitch_is_link_valid(_d, _linknum)                            \
    )

#define NVSWITCH_LINK_OFFSET(_d, _physlinknum, _eng, _dev, _reg)        \
    (                                                                   \
        NVSWITCH_ASSERT(NVSWITCH_IS_LINK_ENG_VALID(_d, _physlinknum, _eng)) \
        ,                                                               \
        NVSWITCH_PRINT(_d, MMIO,                                        \
            "%s: LINK_OFFSET link[%d] %s: %s,%s (+%04x)\n",             \
            __FUNCTION__,                                               \
            _physlinknum,                                               \
            #_eng, #_dev, #_reg, NV ## _dev ## _reg)                    \
        ,                                                               \
        NVSWITCH_GET_ENG(_d, _eng, ,                                    \
            NVSWITCH_GET_LINK_ENG_INST(_d, _physlinknum, _eng)) +       \
            NV##_dev##_reg                                              \
    )

#define NVSWITCH_LINK_WR32(_d, _physlinknum, _eng, _dev, _reg, _data)   \
    NVSWITCH_ASSERT(NVSWITCH_IS_LINK_ENG_VALID(_d, _physlinknum, _eng)); \
    NVSWITCH_PRINT(_d, MMIO,                                            \
        "%s: LINK_WR link[%d] %s: %s,%s (+%04x) 0x%08x\n",              \
        __FUNCTION__,                                                   \
        _physlinknum,                                                   \
        #_eng, #_dev, #_reg, NV ## _dev ## _reg, _data);                \
    ((_d)->hal.nvswitch_eng_wr(                                         \
            _d,                                                         \
            NVSWITCH_ENGINE_ID_##_eng,                                  \
            NVSWITCH_GET_ENG_DESC_TYPE_UNICAST,                         \
            NVSWITCH_GET_LINK_ENG_INST(_d, _physlinknum, _eng),         \
            NV ## _dev ## _reg, _data));                                \
    ((void)(_d))

#define NVSWITCH_LINK_RD32(_d, _physlinknum, _eng, _dev, _reg)      \
    (                                                               \
        NVSWITCH_ASSERT(NVSWITCH_IS_LINK_ENG_VALID(_d, _physlinknum, _eng)) \
        ,                                                           \
        NVSWITCH_PRINT(_d, MMIO,                                    \
            "%s: LINK_RD link[%d] %s: %s,%s (+%04x)\n",             \
            __FUNCTION__,                                           \
            _physlinknum,                                           \
            #_eng, #_dev, #_reg, NV ## _dev ## _reg)                \
        ,                                                           \
        ((_d)->hal.nvswitch_eng_rd(                                 \
            _d,                                                     \
            NVSWITCH_ENGINE_ID_##_eng,                              \
            NVSWITCH_GET_ENG_DESC_TYPE_UNICAST,                     \
            NVSWITCH_GET_LINK_ENG_INST(_d, _physlinknum, _eng),     \
            NV ## _dev ## _reg))                                    \
    );                                                              \
    ((void)(_d))

#define NVSWITCH_LINK_WR32_IDX(_d, _physlinknum, _eng, _dev, _reg, _idx, _data)    \
    NVSWITCH_LINK_WR32(_d, _physlinknum, _eng, _dev, _reg(_idx), _data);           \
    ((void)(_d))

#define NVSWITCH_LINK_RD32_IDX(_d, _physlinknum, _eng, _dev, _reg, _idx)   \
    NVSWITCH_LINK_RD32(_d, _physlinknum, _eng, _dev, _reg(_idx));          \
    ((void)(_d))

#endif //_IO_NVSWITCH_H_
