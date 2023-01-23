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

#include "common_nvswitch.h"
#include "ls10/ls10.h"

#include "nvswitch/ls10/dev_nvs_top.h"
#include "nvswitch/ls10/nvlinkip_discovery.h"
#include "nvswitch/ls10/npgip_discovery.h"
#include "nvswitch/ls10/nxbar_discovery.h"
#include "nvswitch/ls10/dev_npg_ip.h"

#include <stddef.h>

#define VERBOSE_MMIO_DISCOVERY      1

#define MAKE_DISCOVERY_LS10(_engine)                \
    {                                               \
        #_engine,                                   \
        NUM_##_engine##_ENGINE_LS10,                \
        NV_SWPTOP_ENUM_DEVICE_##_engine,            \
        offsetof(ls10_device, eng##_engine)         \
    },

typedef struct
{
    const char *engname;
    NvU32 engcount_max;
    NvU32 discovery_id;
    size_t chip_device_engine_offset;
}
DISCOVERY_TABLE_TYPE_LS10;

#define DISCOVERY_TYPE_UNDEFINED    0
#define DISCOVERY_TYPE_DISCOVERY    1
#define DISCOVERY_TYPE_UNICAST      2
#define DISCOVERY_TYPE_BROADCAST    3

#define NVSWITCH_DISCOVERY_ENTRY_INVALID    0x0
#define NVSWITCH_DISCOVERY_ENTRY_ENUM       0x1
#define NVSWITCH_DISCOVERY_ENTRY_DATA1      0x2
#define NVSWITCH_DISCOVERY_ENTRY_DATA2      0x3

typedef struct
{
    void (*parse_entry)(nvswitch_device *device, NvU32 entry, NvU32 *entry_type, NvBool *entry_chain);
    void (*parse_enum)(nvswitch_device *device, NvU32 entry, NvU32 *entry_device, NvU32 *entry_id, NvU32 *entry_version);
    void (*handle_data1)(nvswitch_device *device, NvU32 entry, ENGINE_DISCOVERY_TYPE_LS10 *engine, NvU32 entry_device, NvU32 *discovery_list_size);
    void (*handle_data2)(nvswitch_device *device, NvU32 entry, ENGINE_DISCOVERY_TYPE_LS10 *engine, NvU32 entry_device);
}
NVSWITCH_DISCOVERY_HANDLERS_LS10;

#define DISCOVERY_DUMP_ENGINE_LS10(_engine)   \
    _discovery_dump_eng_ls10(device, #_engine, NVSWITCH_GET_CHIP_DEVICE_LS10(device)->eng##_engine, NUM_##_engine##_ENGINE_LS10);

#define DISCOVERY_DUMP_BC_ENGINE_LS10(_engine) \
    DISCOVERY_DUMP_ENGINE_LS10(_engine##_BCAST)

static void
_discovery_dump_eng_ls10
(
    nvswitch_device *device,
    const char *eng_name,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 count
)
{
    NvU32 i;

    if (VERBOSE_MMIO_DISCOVERY)
    {
        for (i = 0; i < count; i++)
        {
            if (engine[i].valid)
            {
                if (engine[i].disc_type == DISCOVERY_TYPE_DISCOVERY)
                {
                    NVSWITCH_PRINT(device, SETUP,
                        "%-24s[%2d]:V:%1x %s:%6x                      CL/ID:%2x/%2x\n",
                        eng_name, i,
                        engine[i].version,
                        "DI",
                        engine[i].info.top.discovery,
                        engine[i].info.top.cluster, engine[i].info.top.cluster_id);
                }
                else if (engine[i].disc_type == DISCOVERY_TYPE_UNICAST)
                {
                    NVSWITCH_PRINT(device, SETUP,
                        "%-24s[%2d]:V:%1x %s:%6x\n",
                        eng_name, i,
                        engine[i].version,
                        "UC",
                        engine[i].info.uc.uc_addr);
                }
                else if (engine[i].disc_type == DISCOVERY_TYPE_BROADCAST)
                {
                    NVSWITCH_PRINT(device, SETUP,
                        "%-24s[%2d]:V:%1x %s:%6x %s:%6x/%6x/%6x\n",
                        eng_name, i,
                        engine[i].version,
                        "BC",
                        engine[i].info.bc.bc_addr,
                        "MC",
                        engine[i].info.bc.mc_addr[0],
                        engine[i].info.bc.mc_addr[1],
                        engine[i].info.bc.mc_addr[2]);
                }
                else
                {
                    NVSWITCH_PRINT(device, SETUP,
                        "%-24s[%2d]:V:%1x UNDEFINED\n",
                        eng_name, i,
                        engine[i].version);
                }
            }
            else
            {
                NVSWITCH_PRINT(device, SETUP,
                    "%-24s[%2d]: INVALID\n",
                    eng_name, i);
            }
        }
    }
}

static NvlStatus
_nvswitch_device_discovery_ls10
(
    nvswitch_device *device,
    NvU32   discovery_offset,
    const DISCOVERY_TABLE_TYPE_LS10 *discovery_table,
    NvU32 discovery_table_size,
    NVSWITCH_DISCOVERY_HANDLERS_LS10 *discovery_handlers
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    ENGINE_DISCOVERY_TYPE_LS10 *engine = NULL;
    NvU32                   entry_type = NVSWITCH_DISCOVERY_ENTRY_INVALID;
    NvBool                  entry_chain = NV_FALSE;
    NvU32                   entry = 0;
    NvU32                   entry_device = 0;
    NvU32                   entry_id = 0;
    NvU32                   entry_version = 0;
    NvU32                   entry_count = 0;
    NvBool                  done = NV_FALSE;
    NvlStatus               retval = NVL_SUCCESS;
    NvU32                   i;

    //
    // Must be at least two entries.  We'll fix it up later when we find the length in the table
    //
    NvU32                   discovery_list_size = 2;

    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: NvSwitch Engine discovery table @%x\n",
            __FUNCTION__,
            discovery_offset);
    }

    while ((!done) && (entry_count < discovery_list_size))
    {
        entry = NVSWITCH_OFF_RD32(device, discovery_offset);
        discovery_handlers->parse_entry(device, entry, &entry_type, &entry_chain);

        switch (entry_type)
        {
            case NVSWITCH_DISCOVERY_ENTRY_ENUM:
                NVSWITCH_ASSERT(engine == NULL);
                discovery_handlers->parse_enum(device, entry, &entry_device, &entry_id, &entry_version);

                for(i = 0; i < discovery_table_size; i++)
                {
                    if (entry_device == discovery_table[i].discovery_id)
                    {
                        //
                        // chip_device_engine_offset is a byte offset within
                        // the ls10_device structure; at that offset is a table
                        // of ENGINE_DISCOVERY_TYPE_LS10 entries.
                        //
                        ENGINE_DISCOVERY_TYPE_LS10 *entry_base = (ENGINE_DISCOVERY_TYPE_LS10 *)
                            ((NvU8 *)chip_device + discovery_table[i].chip_device_engine_offset);

                        if (entry_id < discovery_table[i].engcount_max)
                        {
                            engine = entry_base + entry_id;
                            break;
                        }
                        else
                        {
                            NVSWITCH_PRINT(device, ERROR,
                                "%s:_ENUM: ERROR: %s[%d] out of engine range %d..%d\n",
                                __FUNCTION__,
                                discovery_table[i].engname,
                                entry_id,
                                0, discovery_table[i].engcount_max-1);
                        }
                    }
                }

                if (engine == NULL)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s:_ENUM: ERROR: device=%x id=%x version=%x not recognized!\n",
                        __FUNCTION__,
                        entry_device, entry_id, entry_version);
                }

                if (engine != NULL)
                {
                    if ((engine->valid == NV_TRUE) && 
                        (engine->disc_type != DISCOVERY_TYPE_DISCOVERY))
                    {
                        NVSWITCH_PRINT(device, WARN,
                            "%s:_ENUM: WARNING: device=%x id=%x previously discovered!\n",
                            __FUNCTION__,
                            entry_device, entry_id);
                    }
                    engine->valid = NV_TRUE;
                    engine->version  = entry_version;
                }

                break;

            case NVSWITCH_DISCOVERY_ENTRY_DATA1:
                discovery_handlers->handle_data1(device, entry, engine, entry_device, &discovery_list_size);
                break;

            case NVSWITCH_DISCOVERY_ENTRY_DATA2:
                if (engine == NULL)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s:DATA2:engine == NULL.  Skipping processing\n",
                        __FUNCTION__);
                }
                else
                {
                    discovery_handlers->handle_data2(device, entry, engine, entry_device);
                }
                break;

            default:
                NVSWITCH_PRINT(device, ERROR,
                    "%s:Unknown (%d)\n",
                    __FUNCTION__, entry_type);
                NVSWITCH_ASSERT(0);
                // Deliberate fallthrough
            case NVSWITCH_DISCOVERY_ENTRY_INVALID:
                // Invalid entry.  Just ignore it
                NVSWITCH_PRINT(device, SETUP,
                    "%s:_INVALID -- skip 0x%08x\n",
                    __FUNCTION__, entry);
                break;
        }

        if (!entry_chain)
        {
            // End of chain.  Close the active engine
            engine = NULL;
            entry_device  = 0;      // Mark invalid
            entry_id      = ~0;
            entry_version = ~0;
        }

        discovery_offset += sizeof(NvU32);
        entry_count++;
    }

    if (entry_chain)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:Discovery list incorrectly terminated: chain end(%d)\n",
            __FUNCTION__,
            entry_chain);
        NVSWITCH_ASSERT(!entry_chain);
    }

    return retval;
}

static void
_nvswitch_ptop_parse_entry_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_type,
    NvBool  *entry_chain
)
{
    NvU32 entry_type_ls10;

    entry_type_ls10 = DRF_VAL(_SWPTOP, _, ENTRY, entry);
    *entry_chain = FLD_TEST_DRF(_SWPTOP, _, CHAIN, _ENABLE, entry);

    switch (entry_type_ls10)
    {
        case NV_SWPTOP_ENTRY_ENUM:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_ENUM;
            break;
        case NV_SWPTOP_ENTRY_DATA1:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA1;
            break;
        case NV_SWPTOP_ENTRY_DATA2:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA2;
            break;
        default:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_INVALID;
            break;
    }
}

static void
_nvswitch_ptop_parse_enum_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_device,
    NvU32   *entry_id,
    NvU32   *entry_version
)
{
    *entry_device  = DRF_VAL(_SWPTOP, _, ENUM_DEVICE, entry);
    *entry_id      = DRF_VAL(_SWPTOP, _, ENUM_ID, entry);
    *entry_version = DRF_VAL(_SWPTOP, _, ENUM_VERSION, entry);
}

static void
_nvswitch_ptop_handle_data1_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device,
    NvU32 *discovery_list_size
)
{
    if (NV_SWPTOP_ENUM_DEVICE_PTOP == entry_device)
    {
        *discovery_list_size = DRF_VAL(_SWPTOP, _DATA1, _PTOP_LENGTH, entry);
        return;
    }
    else
    {
        NVSWITCH_ASSERT(DRF_VAL(_SWPTOP, _DATA1, _RESERVED, entry) == 0);
    }

    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:DATA1:engine == NULL.  Skipping processing\n",
            __FUNCTION__);
        return;
    }

    engine->info.top.cluster    = DRF_VAL(_SWPTOP, _DATA1, _CLUSTER_TYPE, entry);
    engine->info.top.cluster_id = DRF_VAL(_SWPTOP, _DATA1, _CLUSTER_NUMBER, entry);
    engine->disc_type = DISCOVERY_TYPE_DISCOVERY;
}

static void
_nvswitch_ptop_handle_data2_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device
)
{
    NvU32 data2_type = DRF_VAL(_SWPTOP, _DATA2, _TYPE, entry);
    NvU32 data2_addr = DRF_VAL(_SWPTOP, _DATA2, _ADDR, entry);

    switch(data2_type)
    {
        case NV_SWPTOP_DATA2_TYPE_DISCOVERY:
            // Parse sub-discovery table
            engine->disc_type = DISCOVERY_TYPE_DISCOVERY;
            engine->info.top.discovery = data2_addr*sizeof(NvU32);
            break;
        case NV_SWPTOP_DATA2_TYPE_UNICAST:
            engine->disc_type = DISCOVERY_TYPE_UNICAST;
            engine->info.uc.uc_addr = data2_addr*sizeof(NvU32);
            break;
        case NV_SWPTOP_DATA2_TYPE_BROADCAST:
            engine->disc_type = DISCOVERY_TYPE_BROADCAST;
            engine->info.bc.bc_addr = data2_addr*sizeof(NvU32);
            break;
        case NV_SWPTOP_DATA2_TYPE_MULTICAST0:
        case NV_SWPTOP_DATA2_TYPE_MULTICAST1:
        case NV_SWPTOP_DATA2_TYPE_MULTICAST2:
            {
                NvU32 mc_idx = data2_type - NV_SWPTOP_DATA2_TYPE_MULTICAST0;
                engine->disc_type = DISCOVERY_TYPE_BROADCAST;
                engine->info.bc.mc_addr[mc_idx] = data2_addr*sizeof(NvU32);
            }
            break;
        case NV_SWPTOP_DATA2_TYPE_INVALID:
            NVSWITCH_PRINT(device, SETUP,
                "%s:_DATA2: %s=%6x\n",
                __FUNCTION__,
                "_INVALID", data2_addr);
            engine->disc_type = DISCOVERY_TYPE_UNDEFINED;
            break;
        default:
            NVSWITCH_PRINT(device, SETUP,
                "%s:_DATA2: Unknown type 0x%x (0x%08x)!\n",
                __FUNCTION__, data2_type, entry);
            engine->disc_type = DISCOVERY_TYPE_UNDEFINED;
            NVSWITCH_ASSERT(0);
            break;
    }
}

void
nvswitch_nvlw_parse_entry_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_type,
    NvBool  *entry_chain
)
{
    NvU32 entry_type_nvlw;

    entry_type_nvlw = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _ENTRY, entry);
    *entry_chain = FLD_TEST_DRF(_NVLINKIP, _DISCOVERY_COMMON, _CHAIN, _ENABLE, entry);

    switch (entry_type_nvlw)
    {
        case NV_NVLINKIP_DISCOVERY_COMMON_ENTRY_ENUM:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_ENUM;
            break;
        case NV_NVLINKIP_DISCOVERY_COMMON_ENTRY_DATA1:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA1;
            break;
        case NV_NVLINKIP_DISCOVERY_COMMON_ENTRY_DATA2:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA2;
            break;
        default:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_INVALID;
            break;
    }
}

void
nvswitch_nvlw_parse_enum_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_device,
    NvU32   *entry_id,
    NvU32   *entry_version
)
{
    NvU32 entry_reserved;

    *entry_device  = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _DEVICE, entry);
    *entry_id      = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _ID, entry);
    *entry_version = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _VERSION, entry);

    entry_reserved = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _RESERVED, entry);
    NVSWITCH_ASSERT(entry_reserved == 0);

    if (*entry_version != NV_NVLINKIP_DISCOVERY_COMMON_VERSION_NVLINK40)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:_NVLINKIP, _DISCOVERY_COMMON, _VERSION = %x but expected %x (_NVLINK40).\n",
            __FUNCTION__, *entry_version, NV_NVLINKIP_DISCOVERY_COMMON_VERSION_NVLINK40);
    }
}

void
nvswitch_nvlw_handle_data1_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device,
    NvU32 *discovery_list_size
)
{
    if ((NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_IOCTRL == entry_device) ||
        (NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_SIOCTRL == entry_device) ||
        (NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_TIOCTRL == entry_device) ||
        (NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_NVLW == entry_device))
    {
        *discovery_list_size = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _DATA1_IOCTRL_LENGTH, entry);
    }

    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:DATA1:engine == NULL.  Skipping processing\n",
            __FUNCTION__);
        return;
    }

    if ((NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_IOCTRL != entry_device) &&
        (NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_SIOCTRL != entry_device) &&
        (NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_TIOCTRL != entry_device) &&
        (NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_NVLW != entry_device))
    {
        // Nothing specific needed to handle
        if (0 != DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _DATA1_RESERVED, entry))
        {
            NVSWITCH_PRINT(device, WARN,
                "%s:WARNING:IOCTRL _RESERVED field != 0 (entry %x -> %x)\n",
                __FUNCTION__,
                entry, DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _DATA1_RESERVED, entry));
        }
    }

    if (0 != DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _DATA1_RESERVED2, entry))
    {
        NVSWITCH_PRINT(device, WARN,
            "%s:WARNING:IOCTRL _RESERVED2 field != 0 (entry %x -> %x)\n",
            __FUNCTION__,
            entry, DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON, _DATA1_RESERVED2, entry));
    }
}

void
nvswitch_nvlw_handle_data2_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device
)
{
    NvU32 data2_type = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON_DATA2, _TYPE, entry);
    NvU32 data2_addr = DRF_VAL(_NVLINKIP, _DISCOVERY_COMMON_DATA2, _ADDR, entry);

    switch(data2_type)
    {

        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_DISCOVERY:
            // Parse sub-discovery table

            //
            // Currently _DISCOVERY is not used in the second
            // level discovery.
            //
            NVSWITCH_ASSERT(0);

            break;

        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_UNICAST:
            engine->disc_type = DISCOVERY_TYPE_UNICAST;
            engine->info.uc.uc_addr = data2_addr*sizeof(NvU32);
            break;

        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_BROADCAST:
            engine->disc_type = DISCOVERY_TYPE_BROADCAST;
            engine->info.bc.bc_addr = data2_addr*sizeof(NvU32);
            break;

        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_MULTICAST0:
        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_MULTICAST1:
        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_MULTICAST2:
            {
                NvU32 mc_idx = data2_type - NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_MULTICAST0;
                engine->disc_type = DISCOVERY_TYPE_BROADCAST;
                engine->info.bc.mc_addr[mc_idx] = data2_addr*sizeof(NvU32);
            }
            break;

        case NV_NVLINKIP_DISCOVERY_COMMON_DATA2_TYPE_INVALID:
            NVSWITCH_PRINT(device, ERROR,
                "%s:_DATA2: %s=%6x\n",
                __FUNCTION__,
                "_INVALID", data2_addr);
            engine->disc_type = DISCOVERY_TYPE_UNDEFINED;
            break;

        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s:_DATA2: Unknown!\n",
                __FUNCTION__);
            NVSWITCH_ASSERT(0);
            break;
    }
}

static void
_nvswitch_npg_parse_entry_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_type,
    NvBool  *entry_chain
)
{
    NvU32 entry_type_npg;

    entry_type_npg = DRF_VAL(_NPG, _DISCOVERY, _ENTRY, entry);
    *entry_chain = FLD_TEST_DRF(_NPG, _DISCOVERY, _CHAIN, _ENABLE, entry);

    switch (entry_type_npg)
    {
        case NV_NPG_DISCOVERY_ENTRY_ENUM:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_ENUM;
            break;
        case NV_NPG_DISCOVERY_ENTRY_DATA1:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA1;
            break;
        case NV_NPG_DISCOVERY_ENTRY_DATA2:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA2;
            break;
        default:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_INVALID;
            break;
    }
}

static void
_nvswitch_npg_parse_enum_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_device,
    NvU32   *entry_id,
    NvU32   *entry_version
)
{
    *entry_device  = DRF_VAL(_NPG, _DISCOVERY, _ENUM_DEVICE, entry);
    *entry_id      = DRF_VAL(_NPG, _DISCOVERY, _ENUM_ID, entry);
    *entry_version = DRF_VAL(_NPG, _DISCOVERY, _ENUM_VERSION, entry);
    NVSWITCH_ASSERT(DRF_VAL(_NPG, _DISCOVERY, _ENUM_RESERVED, entry) == 0);

    if (*entry_version != NV_NPG_DISCOVERY_ENUM_VERSION_3)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:_NPG_DISCOVERY_ENUM_VERSION = %x but expected %x (_VERSION_3).\n",
            __FUNCTION__, *entry_version, NV_NPG_DISCOVERY_ENUM_VERSION_3);
    }
}

static void
_nvswitch_npg_handle_data1_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device,
    NvU32 *discovery_list_size
)
{
    if (NV_NPG_DISCOVERY_ENUM_DEVICE_NPG == entry_device)
    {
        *discovery_list_size = DRF_VAL(_NPG, _DISCOVERY, _DATA1_NPG_LENGTH, entry);
    }
    else
    {
        NVSWITCH_ASSERT(0 == DRF_VAL(_NPG, _DISCOVERY, _DATA1_RESERVED, entry));
    }

    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s:DATA1:engine == NULL.  Skipping processing\n",
            __FUNCTION__);
        return;
    }
}

static void
_nvswitch_npg_handle_data2_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device
)
{
    NvU32 data2_type = DRF_VAL(_NPG, _DISCOVERY_DATA2, _TYPE, entry);
    NvU32 data2_addr = DRF_VAL(_NPG, _DISCOVERY_DATA2, _ADDR, entry);

    switch(data2_type)
    {
        case NV_NPG_DISCOVERY_DATA2_TYPE_DISCOVERY:
            // Parse sub-discovery table

            //
            // Currently _DISCOVERY is not used in the second
            // level discovery.
            //
            NVSWITCH_ASSERT(0);

            break;

        case NV_NPG_DISCOVERY_DATA2_TYPE_UNICAST:
            engine->disc_type = DISCOVERY_TYPE_UNICAST;
            engine->info.uc.uc_addr = data2_addr*sizeof(NvU32);
            break;

        case NV_NPG_DISCOVERY_DATA2_TYPE_BROADCAST:
            engine->disc_type = DISCOVERY_TYPE_BROADCAST;
            engine->info.bc.bc_addr = data2_addr*sizeof(NvU32);
            break;

        case NV_NPG_DISCOVERY_DATA2_TYPE_MULTICAST0:
        case NV_NPG_DISCOVERY_DATA2_TYPE_MULTICAST1:
        case NV_NPG_DISCOVERY_DATA2_TYPE_MULTICAST2:
            {
                NvU32 mc_idx = data2_type - NV_NPG_DISCOVERY_DATA2_TYPE_MULTICAST0;
                engine->disc_type = DISCOVERY_TYPE_BROADCAST;
                engine->info.bc.mc_addr[mc_idx] = data2_addr*sizeof(NvU32);
            }
            break;

        case NV_NPG_DISCOVERY_DATA2_TYPE_INVALID:
            NVSWITCH_PRINT(device, SETUP,
                "%s:_DATA2: %s=%6x\n",
                __FUNCTION__,
                "_INVALID", data2_addr);
            engine->disc_type = DISCOVERY_TYPE_UNDEFINED;
            break;

        default:
            NVSWITCH_PRINT(device, SETUP,
                "%s:_DATA2: Unknown!\n",
                __FUNCTION__);
            NVSWITCH_ASSERT(0);
            break;
    }
}

void
nvswitch_nxbar_parse_entry_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_type,
    NvBool  *entry_chain
)
{
    NvU32 entry_type_nxbar;

    entry_type_nxbar = DRF_VAL(_NXBAR, _DISCOVERY, _ENTRY, entry);
    *entry_chain = FLD_TEST_DRF(_NXBAR, _DISCOVERY, _CHAIN, _ENABLE, entry);

    switch (entry_type_nxbar)
    {
        case NV_NXBAR_DISCOVERY_ENTRY_ENUM:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_ENUM;
            break;
        case NV_NXBAR_DISCOVERY_ENTRY_DATA1:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA1;
            break;
        case NV_NXBAR_DISCOVERY_ENTRY_DATA2:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_DATA2;
            break;
        default:
            *entry_type = NVSWITCH_DISCOVERY_ENTRY_INVALID;
            break;
    }
}

void
nvswitch_nxbar_parse_enum_ls10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_device,
    NvU32   *entry_id,
    NvU32   *entry_version
)
{
    NvU32 entry_reserved;

    *entry_device  = DRF_VAL(_NXBAR, _DISCOVERY, _ENUM_DEVICE, entry);
    *entry_id      = DRF_VAL(_NXBAR, _DISCOVERY, _ENUM_ID, entry);
    *entry_version = DRF_VAL(_NXBAR, _DISCOVERY, _ENUM_VERSION, entry);

    entry_reserved = DRF_VAL(_NXBAR, _DISCOVERY, _ENUM_RESERVED, entry);
    NVSWITCH_ASSERT(entry_reserved == 0);

    if (*entry_version != NV_NXBAR_DISCOVERY_ENUM_VERSION_3)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:_NXBAR_DISCOVERY_ENUM_VERSION = %x but expected %x (_VERSION_3).\n",
            __FUNCTION__, *entry_version, NV_NXBAR_DISCOVERY_ENUM_VERSION_3);
    }
}

void
nvswitch_nxbar_handle_data1_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device,
    NvU32 *discovery_list_size
)
{
    if (NV_NXBAR_DISCOVERY_ENUM_DEVICE_NXBAR == entry_device)
    {
        *discovery_list_size = DRF_VAL(_NXBAR, _DISCOVERY, _DATA1_NXBAR_LENGTH, entry);
    }

    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:DATA1:engine == NULL.  Skipping processing\n",
            __FUNCTION__);
        return;
    }

    if (NV_NXBAR_DISCOVERY_ENUM_DEVICE_NXBAR != entry_device)
    {
        NVSWITCH_ASSERT(DRF_VAL(_NXBAR, _DISCOVERY, _DATA1_RESERVED, entry) == 0);
    }
}

void
nvswitch_nxbar_handle_data2_ls10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DISCOVERY_TYPE_LS10 *engine,
    NvU32 entry_device
)
{
    NvU32 data2_type = DRF_VAL(_NXBAR, _DISCOVERY_DATA2, _TYPE, entry);
    NvU32 data2_addr = DRF_VAL(_NXBAR, _DISCOVERY_DATA2, _ADDR, entry);

    switch(data2_type)
    {
        case NV_NXBAR_DISCOVERY_DATA2_TYPE_DISCOVERY:
            // Parse sub-discovery table

            //
            // Currently _DISCOVERY is not used in the second
            // level discovery.
            //
            NVSWITCH_ASSERT(0);

            break;

        case NV_NXBAR_DISCOVERY_DATA2_TYPE_UNICAST:
            engine->disc_type = DISCOVERY_TYPE_UNICAST;
            engine->info.uc.uc_addr = data2_addr*sizeof(NvU32);
            break;

        case NV_NXBAR_DISCOVERY_DATA2_TYPE_BROADCAST:
            engine->disc_type = DISCOVERY_TYPE_BROADCAST;
            engine->info.bc.bc_addr = data2_addr*sizeof(NvU32);
            engine->info.bc.mc_addr[0] = 0;
            engine->info.bc.mc_addr[1] = 0;
            engine->info.bc.mc_addr[2] = 0;
            break;

        case NV_NXBAR_DISCOVERY_DATA2_TYPE_MULTICAST0:
        case NV_NXBAR_DISCOVERY_DATA2_TYPE_MULTICAST1:
        case NV_NXBAR_DISCOVERY_DATA2_TYPE_MULTICAST2:
            {
                NvU32 mc_idx = data2_type - NV_NXBAR_DISCOVERY_DATA2_TYPE_MULTICAST0;
                engine->info.bc.mc_addr[mc_idx] = data2_addr*sizeof(NvU32);
                NVSWITCH_PRINT(device, ERROR,
                    "%s:_DATA2: NXBAR MULTICAST%d=0x%x but should be unused!\n",
                    __FUNCTION__, mc_idx, engine->info.bc.mc_addr[mc_idx]);
                NVSWITCH_ASSERT(0);
            }
            break;

        case NV_NXBAR_DISCOVERY_DATA2_TYPE_INVALID:
            NVSWITCH_PRINT(device, ERROR,
                "%s:_DATA2: %s=%6x\n",
                __FUNCTION__,
                "_INVALID", data2_addr);
            engine->disc_type = DISCOVERY_TYPE_UNDEFINED;
            break;

        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s:_DATA2: Unknown!\n",
                __FUNCTION__);
            NVSWITCH_ASSERT(0);
            break;
    }
}

#define MAKE_DISCOVERY_NVLINK_UC_LS10(_eng)         \
    {                                               \
        #_eng,                                      \
        NUM_##_eng##_ENGINE_LS10,                   \
        NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_##_eng, \
        offsetof(ls10_device, eng##_eng)            \
    },

#define MAKE_DISCOVERY_NVLINK_BC_LS10(_eng)         \
    {                                               \
        #_eng "_BCAST",                             \
        NUM_##_eng##_BCAST_ENGINE_LS10,             \
        NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_##_eng, \
        offsetof(ls10_device, eng##_eng##_BCAST)    \
    },

#define MAKE_DISCOVERY_NPG_UC_LS10(_eng)            \
    {                                               \
        #_eng,                                      \
        NUM_##_eng##_ENGINE_LS10,                   \
        NV_NPG_DISCOVERY_ENUM_DEVICE_##_eng,        \
        offsetof(ls10_device, eng##_eng)            \
    },

#define MAKE_DISCOVERY_NPG_BC_LS10(_eng)            \
    {                                               \
        #_eng "_BCAST",                             \
        NUM_##_eng##_BCAST_ENGINE_LS10,             \
        NV_NPG_DISCOVERY_ENUM_DEVICE_##_eng,        \
        offsetof(ls10_device, eng##_eng##_BCAST)    \
    },

#define MAKE_DISCOVERY_NXBAR_UC_LS10(_eng)          \
    {                                               \
        #_eng,                                      \
        NUM_##_eng##_ENGINE_LS10,                   \
        NV_NXBAR_DISCOVERY_ENUM_DEVICE_##_eng,      \
        offsetof(ls10_device, eng##_eng)            \
    },

#define MAKE_DISCOVERY_NXBAR_BC_LS10(_eng)          \
    {                                               \
        #_eng "_BCAST",                             \
        NUM_##_eng##_BCAST_ENGINE_LS10,             \
        NV_NXBAR_DISCOVERY_ENUM_DEVICE_##_eng,      \
        offsetof(ls10_device, eng##_eng##_BCAST)    \
    },

static
NVSWITCH_DISCOVERY_HANDLERS_LS10 discovery_handlers_ptop_ls10 =
{
    &_nvswitch_ptop_parse_entry_ls10,
    &_nvswitch_ptop_parse_enum_ls10,
    &_nvswitch_ptop_handle_data1_ls10,
    &_nvswitch_ptop_handle_data2_ls10
};

static
NVSWITCH_DISCOVERY_HANDLERS_LS10 discovery_handlers_npg_ls10 =
{
    &_nvswitch_npg_parse_entry_ls10,
    &_nvswitch_npg_parse_enum_ls10,
    &_nvswitch_npg_handle_data1_ls10,
    &_nvswitch_npg_handle_data2_ls10
};

static
NVSWITCH_DISCOVERY_HANDLERS_LS10 discovery_handlers_nvlw_ls10 =
{
    &nvswitch_nvlw_parse_entry_ls10,
    &nvswitch_nvlw_parse_enum_ls10,
    &nvswitch_nvlw_handle_data1_ls10,
    &nvswitch_nvlw_handle_data2_ls10
};

static
NVSWITCH_DISCOVERY_HANDLERS_LS10 discovery_handlers_nxbar_ls10 =
{
    &nvswitch_nxbar_parse_entry_ls10,
    &nvswitch_nxbar_parse_enum_ls10,
    &nvswitch_nxbar_handle_data1_ls10,
    &nvswitch_nxbar_handle_data2_ls10
};

//
// Parse top level PTOP engine discovery information to identify MMIO, interrupt, and
// reset information
//

NvlStatus
nvswitch_device_discovery_ls10
(
    nvswitch_device *device,
    NvU32   discovery_offset
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    //
    // The top level discovery table in PTOP describes all the top level
    // unicast units, and also points to the sub-discovery tables of the IP
    // wrappers.  These IP wrapper sub-discovery tables are then examined in
    // succession to build the MMIO map of units.  Floorswept units will have
    // discovery entries that are marked invalid.
    //
    static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_ls10[] =
    {
        NVSWITCH_LIST_LS10_ENGINE_UC(MAKE_DISCOVERY_LS10)
        NVSWITCH_LIST_PRI_HUB_LS10_ENGINE(MAKE_DISCOVERY_LS10)
        MAKE_DISCOVERY_LS10(NPG)
        MAKE_DISCOVERY_LS10(NPG_BCAST)
        MAKE_DISCOVERY_LS10(NVLW)
        MAKE_DISCOVERY_LS10(NVLW_BCAST)
        MAKE_DISCOVERY_LS10(NXBAR)
        MAKE_DISCOVERY_LS10(NXBAR_BCAST)
    };
    NvU32 discovery_table_ls10_size = NV_ARRAY_ELEMENTS(discovery_table_ls10);
    NvU32 i;
    NvlStatus   status;

    status = _nvswitch_device_discovery_ls10(
        device, discovery_offset, discovery_table_ls10, discovery_table_ls10_size, 
        &discovery_handlers_ptop_ls10);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "MMIO discovery failed\n");
        return status;
    }

    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "PTOP Discovery\n");

        NVSWITCH_LIST_LS10_ENGINE_UC(DISCOVERY_DUMP_ENGINE_LS10)
        NVSWITCH_LIST_PRI_HUB_LS10_ENGINE(DISCOVERY_DUMP_ENGINE_LS10)
        DISCOVERY_DUMP_ENGINE_LS10(NPG)
        DISCOVERY_DUMP_ENGINE_LS10(NPG_BCAST)
        DISCOVERY_DUMP_ENGINE_LS10(NVLW)
        DISCOVERY_DUMP_ENGINE_LS10(NVLW_BCAST)
        DISCOVERY_DUMP_ENGINE_LS10(NXBAR)
        DISCOVERY_DUMP_ENGINE_LS10(NXBAR_BCAST)
    }

    for (i = 0; i < NUM_NVLW_ENGINE_LS10; i++)
    {
        if (chip_device->engNVLW[i].valid && 
            (chip_device->engNVLW[i].info.top.discovery != 0))
        {
            static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_nvlw[] =
            {
                NVSWITCH_LIST_NVLW_LS10_ENGINE(MAKE_DISCOVERY_NVLINK_UC_LS10)
            };
            NvU32 discovery_table_nvlw_size = NV_ARRAY_ELEMENTS(discovery_table_nvlw);

            status = _nvswitch_device_discovery_ls10(
                device, chip_device->engNVLW[i].info.top.discovery, discovery_table_nvlw, 
                discovery_table_nvlw_size, &discovery_handlers_nvlw_ls10);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "NVLW[%d] discovery failed\n", i);
                return status;
            }
        }
    }
    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVLW[0..%d] Discovery\n",
             NUM_NVLW_ENGINE_LS10-1);
        NVSWITCH_LIST_NVLW_LS10_ENGINE(DISCOVERY_DUMP_ENGINE_LS10)
    }

    for (i = 0; i < NUM_NVLW_BCAST_ENGINE_LS10; i++)
    {
        if (chip_device->engNVLW_BCAST[i].valid && 
            (chip_device->engNVLW_BCAST[i].info.top.discovery != 0))
        {
            static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_nvlw[] =
            {
                NVSWITCH_LIST_NVLW_LS10_ENGINE(MAKE_DISCOVERY_NVLINK_BC_LS10)
            };
            NvU32 discovery_table_nvlw_size = NV_ARRAY_ELEMENTS(discovery_table_nvlw);

            status = _nvswitch_device_discovery_ls10(
                device, chip_device->engNVLW_BCAST[i].info.top.discovery, discovery_table_nvlw, 
                discovery_table_nvlw_size, &discovery_handlers_nvlw_ls10);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "NVLW_BCAST[%d] discovery failed\n", i);
                return status;
            }
        }
    }
    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVLW_BCAST[0..%d] Discovery\n",
             NUM_NVLW_BCAST_ENGINE_LS10-1);
        NVSWITCH_LIST_NVLW_LS10_ENGINE(DISCOVERY_DUMP_BC_ENGINE_LS10)
    }

    for (i = 0; i < NUM_NPG_ENGINE_LS10; i++)
    {
        if (chip_device->engNPG[i].valid && 
            (chip_device->engNPG[i].info.top.discovery != 0))
        {
            static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_npg[] =
            {
                NVSWITCH_LIST_NPG_LS10_ENGINE(MAKE_DISCOVERY_NPG_UC_LS10)
            };
            NvU32 discovery_table_npg_size = NV_ARRAY_ELEMENTS(discovery_table_npg);

            status = _nvswitch_device_discovery_ls10(
                device, chip_device->engNPG[i].info.top.discovery, discovery_table_npg, 
                discovery_table_npg_size, &discovery_handlers_npg_ls10);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "NPG[%d] discovery failed\n", i);
                return status;
            }
        }
    }
    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NPG[0..%d] Discovery\n",
             NUM_NPG_ENGINE_LS10-1);
        NVSWITCH_LIST_NPG_LS10_ENGINE(DISCOVERY_DUMP_ENGINE_LS10)
    }

    for (i = 0; i < NUM_NPG_BCAST_ENGINE_LS10; i++)
    {
        if (chip_device->engNPG_BCAST[i].valid && 
            (chip_device->engNPG_BCAST[i].info.top.discovery != 0))
        {
            static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_npg[] =
            {
                NVSWITCH_LIST_NPG_LS10_ENGINE(MAKE_DISCOVERY_NPG_BC_LS10)
            };
            NvU32 discovery_table_npg_size = NV_ARRAY_ELEMENTS(discovery_table_npg);

            status = _nvswitch_device_discovery_ls10(
                device, chip_device->engNPG_BCAST[i].info.top.discovery, discovery_table_npg, 
                discovery_table_npg_size, &discovery_handlers_npg_ls10);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "NPG_BCAST[%d] discovery failed\n", i);
                return status;
            }
        }
    }
    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NPG_BCAST[%d] Discovery\n",
            NUM_NPG_BCAST_ENGINE_LS10-1);
        NVSWITCH_LIST_NPG_LS10_ENGINE(DISCOVERY_DUMP_BC_ENGINE_LS10)
    }

    for (i = 0; i < NUM_NXBAR_ENGINE_LS10; i++)
    {
        if (chip_device->engNXBAR[i].valid && 
            (chip_device->engNXBAR[i].info.top.discovery != 0))
        {
            static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_nxbar[] =
            {
                NVSWITCH_LIST_NXBAR_LS10_ENGINE(MAKE_DISCOVERY_NXBAR_UC_LS10)
            };
            NvU32 discovery_table_nxbar_size = NV_ARRAY_ELEMENTS(discovery_table_nxbar);

            status = _nvswitch_device_discovery_ls10(
                device, chip_device->engNXBAR[i].info.top.discovery, 
                discovery_table_nxbar, discovery_table_nxbar_size, 
                &discovery_handlers_nxbar_ls10);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "NXBAR[%d] discovery failed\n", i);
                return status;
            }
        }
    }
    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NXBAR[0..%d] Discovery\n",
            NUM_NXBAR_ENGINE_LS10-1);
        NVSWITCH_LIST_NXBAR_LS10_ENGINE(DISCOVERY_DUMP_ENGINE_LS10)
    }

    for (i = 0; i < NUM_NXBAR_BCAST_ENGINE_LS10; i++)
    {
        if (chip_device->engNXBAR_BCAST[i].valid && 
            (chip_device->engNXBAR_BCAST[i].info.top.discovery != 0))
        {
            static const DISCOVERY_TABLE_TYPE_LS10 discovery_table_nxbar[] =
            {
                NVSWITCH_LIST_NXBAR_LS10_ENGINE(MAKE_DISCOVERY_NXBAR_BC_LS10)
            };
            NvU32 discovery_table_nxbar_size = NV_ARRAY_ELEMENTS(discovery_table_nxbar);

            status = _nvswitch_device_discovery_ls10(
                device, chip_device->engNXBAR_BCAST[i].info.top.discovery, 
                discovery_table_nxbar, discovery_table_nxbar_size, 
                &discovery_handlers_nxbar_ls10);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "NXBAR_BCAST[%d] discovery failed\n", i);
                return status;
            }
        }
    }
    if (VERBOSE_MMIO_DISCOVERY)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NXBAR_BCAST[0..%d] Discovery\n",
            NUM_NXBAR_BCAST_ENGINE_LS10-1);
        NVSWITCH_LIST_NXBAR_LS10_ENGINE(DISCOVERY_DUMP_BC_ENGINE_LS10)
    }

    return status;
}

//
// Filter engine discovery information to handle platform-specific differences.
//
// Emulation and RTL sims have devices that show up in the discovery table but
// are actually tied off and not present.  On GPU the engine enables and
// floorsweeping info are used to disable devices that are not present.
// But a similar mechanism does not exist in NvSwitch.
// So instead we invalidate the devices that are known to be not-present on a
// given platform.
//

void
nvswitch_filter_discovery_ls10
(
    nvswitch_device *device
)
{
}

#define NVSWITCH_PROCESS_DISCOVERY(_current, _engine, _multicast)           \
    {                                                                       \
        NvU32 i;                                                            \
        ct_assert(NUM_##_engine##_ENGINE_LS10 <= NVSWITCH_ENGINE_DESCRIPTOR_UC_SIZE); \
                                                                            \
        _current->eng_name = #_engine;                                      \
        _current->eng_id = NVSWITCH_ENGINE_ID_##_engine;                    \
        _current->eng_count = NUM_##_engine##_ENGINE_LS10;                  \
                                                                            \
        for (i = 0; i < NUM_##_engine##_ENGINE_LS10; i++)                   \
        {                                                                   \
            if (chip_device->eng##_engine[i].valid)                         \
            {                                                               \
                _current->uc_addr[i] =                                      \
                    chip_device->eng##_engine[i].info.uc.uc_addr;           \
            }                                                               \
        }                                                                   \
                                                                            \
        if (chip_device->eng##_engine##_multicast[0].valid)                 \
        {                                                                   \
            _current->bc_addr =                                             \
                chip_device->eng##_engine##_multicast[0].info.bc.bc_addr;   \
        }                                                                   \
                                                                            \
        _current->mc_addr_count = 0;                                        \
    }                                                                       \

#define NVSWITCH_PROCESS_COMMON(_engine, _multicast)                        \
    {                                                                       \
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE *current;                           \
        ct_assert(NVSWITCH_ENGINE_ID_##_engine < NVSWITCH_ENGINE_ID_SIZE);  \
                                                                            \
        current = &chip_device->io.common[NVSWITCH_ENGINE_ID_##_engine];    \
        NVSWITCH_PROCESS_DISCOVERY(current, _engine, _multicast)            \
    }

//
// Process engine discovery information to associate engines
//

NvlStatus
nvswitch_process_discovery_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32       i, j;
    NvU64       localMinionLinkMask;
    NvlStatus   retval = NVL_SUCCESS;
    NvU64       link_enable_mask;

    //
    // Process per-link information
    //
    for (i = 0; i < NVSWITCH_NUM_LINKS_LS10; i++)
    {
        device->link[i].valid = 
            NVSWITCH_ENG_VALID_LS10(device, NVLTLC, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLTLC)) &&
            NVSWITCH_ENG_VALID_LS10(device, NVLDL, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLDL)) &&
            NVSWITCH_ENG_VALID_LS10(device, NVLIPT_LNK, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT_LNK)) &&
            NVSWITCH_ENG_VALID_LS10(device, NVLW, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLW)) &&
            NVSWITCH_ENG_VALID_LS10(device, MINION, NVSWITCH_GET_LINK_ENG_INST(device, i, MINION)) &&
            NVSWITCH_ENG_VALID_LS10(device, NVLIPT, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT));
    }

    //
    // Process common engine information
    //

    // Mark all entries as invalid
    for (i = 0; i < NVSWITCH_ENGINE_ID_SIZE; i++)
    {
        chip_device->io.common[i].eng_name = "";
        chip_device->io.common[i].eng_id = NVSWITCH_ENGINE_ID_SIZE; // Out of range
        chip_device->io.common[i].eng_count = 0;
        for (j = 0; j < NVSWITCH_ENGINE_DESCRIPTOR_UC_SIZE; j++)
        {
            chip_device->io.common[i].uc_addr[j] = NVSWITCH_BASE_ADDR_INVALID;
        }
        chip_device->io.common[i].bc_addr = NVSWITCH_BASE_ADDR_INVALID;
        for (j = 0; j < NVSWITCH_ENGINE_DESCRIPTOR_MC_SIZE; j++)
        {
            chip_device->io.common[i].mc_addr[j] = NVSWITCH_BASE_ADDR_INVALID;
        }
        chip_device->io.common[i].mc_addr_count = 0;
    }

    NVSWITCH_LIST_LS10_ENGINES(NVSWITCH_PROCESS_COMMON)

    //
    // Disable engines requested by regkey "LinkEnableMask".
    // All the links are enabled by default.
    //
    link_enable_mask = ((NvU64)device->regkeys.link_enable_mask2 << 32 |
        (NvU64)device->regkeys.link_enable_mask);

    for (i = 0; i < NVSWITCH_NUM_LINKS_LS10; i++)
    {
        if ((NVBIT64(i) & link_enable_mask) == 0)
        {
            NVSWITCH_PRINT(device, SETUP,
                "%s: Disable link #%d\n",
                __FUNCTION__, i);
            nvswitch_link_disable_interrupts_ls10(device, i);
            device->link[i].valid                  = NV_FALSE;
            chip_device->engNPORT[i].valid         = NV_FALSE;
            chip_device->engNPORT_PERFMON[i].valid = NV_FALSE;
            chip_device->engNVLTLC[i].valid        = NV_FALSE;
        }
    }

    //
    // Go through all MINION engines and mark them
    // as invalid if all links associated with that MINION
    // are invalid
    //
    for (i =0; i < NUM_MINION_ENGINE_LS10; i++)
    {

        //
        // Get the local link mask asscotiated with this minion
        // In this case there is a 1-1 association between NVLIPT 
        // and MINION instances
        //
        localMinionLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(i);

        //
        // If there are no links from localMinionLinkMask in
        // link_enable_mask then mark the MINION instance as invalid
        //
        if ((localMinionLinkMask & link_enable_mask) == 0)
        {
            chip_device->engMINION[i].valid = NV_FALSE;
        }
    }

    return retval;
}

