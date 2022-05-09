/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "lr10/lr10.h"

#include "nvswitch/lr10/dev_nvs_top.h"
#include "nvswitch/lr10/nvlinkip_discovery.h"
#include "nvswitch/lr10/npgip_discovery.h"
#include "nvswitch/lr10/nxbar_discovery.h"

#define VERBOSE_MMIO_DISCOVERY      0

#define MAKE_DISCOVERY_LR10(device, _chip, _engine) \
    {                                               \
        #_engine,                                   \
        NUM_##_engine##_ENGINE_##_chip,             \
        NV_SWPTOP_ENUM_DEVICE_##_engine,            \
        chip_device->eng##_engine                   \
    }

typedef struct
{
    const char *engname;
    NvU32 engcount_max;
    NvU32 discovery_id;
    ENGINE_DESCRIPTOR_TYPE_LR10    *engine;
}
DISCOVERY_TABLE_TYPE_LR10;

#define NVSWITCH_DISCOVERY_ENTRY_INVALID    0x0
#define NVSWITCH_DISCOVERY_ENTRY_ENUM       0x1
#define NVSWITCH_DISCOVERY_ENTRY_DATA1      0x2
#define NVSWITCH_DISCOVERY_ENTRY_DATA2      0x3

typedef struct
{
    void (*parse_entry)(nvswitch_device *device, NvU32 entry, NvU32 *entry_type, NvBool *entry_chain);
    void (*parse_enum)(nvswitch_device *device, NvU32 entry, NvU32 *entry_device, NvU32 *entry_id, NvU32 *entry_version);
    void (*handle_data1)(nvswitch_device *device, NvU32 entry, ENGINE_DESCRIPTOR_TYPE_LR10 *engine, NvU32 entry_device, NvU32 *discovery_list_size);
    void (*handle_data2)(nvswitch_device *device, NvU32 entry, ENGINE_DESCRIPTOR_TYPE_LR10 *engine, NvU32 entry_device);
}
NVSWITCH_DISCOVERY_HANDLERS_LR10;

#define DISCOVERY_DUMP_ENGINE_LR10(_device,  _engine, _bcast)   \
    _discovery_dump_eng_lr10(_device, #_engine, NVSWITCH_GET_CHIP_DEVICE_LR10(_device)->eng##_engine##_bcast, NUM_##_engine##_bcast##_ENGINE_LR10);

static void
_discovery_dump_eng_lr10
(
    nvswitch_device *device,
    const char *eng_name,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
_nvswitch_device_discovery_lr10
(
    nvswitch_device *device,
    NvU32   discovery_offset,
    DISCOVERY_TABLE_TYPE_LR10 *discovery_table,
    NvU32 discovery_table_size,
    NVSWITCH_DISCOVERY_HANDLERS_LR10 *discovery_handlers
)
{
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine = NULL;
    NvU32                   entry_type = NVSWITCH_DISCOVERY_ENTRY_INVALID;
    NvBool                  entry_chain = NV_FALSE;
    NvU32                   entry = 0;
    NvU32                   entry_device = 0;
    NvU32                   entry_id = 0;
    NvU32                   entry_version = 0;
    NvU32                   entry_count = 0;
    NvBool                  done = NV_FALSE;
    NvlStatus               retval = NVL_SUCCESS;

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

                {
                    NvU32 i;

                    for(i = 0; i < discovery_table_size; i++)
                    {
                        if (entry_device == discovery_table[i].discovery_id)
                        {
                            if (discovery_table[i].engine == NULL)
                            {
                                NVSWITCH_PRINT(device, ERROR,
                                    "%s:_ENUM: ERROR: %s:device=%x id=%x version=%x not supported!\n",
                                    __FUNCTION__,
                                    discovery_table[i].engname,
                                    entry_device, entry_id, entry_version);
                                NVSWITCH_ASSERT(0);
                                continue;
                            }

                            if (entry_id < discovery_table[i].engcount_max)
                            {
                                engine = &(discovery_table[i].engine[entry_id]);
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
_nvswitch_ptop_parse_entry_lr10
(
    nvswitch_device *device,
    NvU32   entry,
    NvU32   *entry_type,
    NvBool  *entry_chain
)
{
    NvU32 entry_type_lr10;

    entry_type_lr10 = DRF_VAL(_SWPTOP, _, ENTRY, entry);
    *entry_chain = FLD_TEST_DRF(_SWPTOP, _, CHAIN, _ENABLE, entry);

    switch (entry_type_lr10)
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
_nvswitch_ptop_parse_enum_lr10
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
_nvswitch_ptop_handle_data1_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
_nvswitch_ptop_handle_data2_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
nvswitch_nvlw_parse_entry_lr10
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
nvswitch_nvlw_parse_enum_lr10
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

    if (*entry_version != NV_NVLINKIP_DISCOVERY_COMMON_VERSION_NVLINK30)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:_NVLINKIP, _DISCOVERY_COMMON, _VERSION = %x but expected %x (_NVLINK30).\n",
            __FUNCTION__, *entry_version, NV_NVLINKIP_DISCOVERY_COMMON_VERSION_NVLINK30);
    }
}

void
nvswitch_nvlw_handle_data1_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
nvswitch_nvlw_handle_data2_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
_nvswitch_npg_parse_entry_lr10
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
_nvswitch_npg_parse_enum_lr10
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

    if (*entry_version != NV_NPG_DISCOVERY_ENUM_VERSION_2)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:_NPG_DISCOVERY_ENUM_VERSION = %x but expected %x (_VERSION_2).\n",
            __FUNCTION__, *entry_version, NV_NPG_DISCOVERY_ENUM_VERSION_2);
    }
}

static void
_nvswitch_npg_handle_data1_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
_nvswitch_npg_handle_data2_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
nvswitch_nxbar_parse_entry_lr10
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
nvswitch_nxbar_parse_enum_lr10
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

    if (*entry_version != NV_NXBAR_DISCOVERY_ENUM_VERSION_2)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s:_NXBAR_DISCOVERY_ENUM_VERSION = %x but expected %x (_VERSION_2).\n",
            __FUNCTION__, *entry_version, NV_NXBAR_DISCOVERY_ENUM_VERSION_2);
    }
}

void
nvswitch_nxbar_handle_data1_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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
nvswitch_nxbar_handle_data2_lr10
(
    nvswitch_device *device,
    NvU32 entry,
    ENGINE_DESCRIPTOR_TYPE_LR10 *engine,
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

#define MAKE_DISCOVERY_NVLINK_LR10(_eng, _bcast)    \
    {                                               \
        #_eng#_bcast,                               \
        NUM_##_eng##_bcast##_ENGINE_LR10,           \
        NV_NVLINKIP_DISCOVERY_COMMON_DEVICE_##_eng, \
        chip_device->eng##_eng##_bcast              \
    }

#define MAKE_DISCOVERY_NPG_LR10(_eng, _bcast)       \
    {                                               \
        #_eng#_bcast,                               \
        NUM_##_eng##_bcast##_ENGINE_LR10,           \
        NV_NPG_DISCOVERY_ENUM_DEVICE_##_eng,        \
        chip_device->eng##_eng##_bcast              \
    }

#define MAKE_DISCOVERY_NXBAR_LR10(_eng, _bcast)     \
    {                                               \
        #_eng#_bcast,                               \
        NUM_##_eng##_bcast##_ENGINE_LR10,           \
        NV_NXBAR_DISCOVERY_ENUM_DEVICE_##_eng,      \
        chip_device->eng##_eng##_bcast              \
    }

static
NVSWITCH_DISCOVERY_HANDLERS_LR10 discovery_handlers_ptop_lr10 =
{
    &_nvswitch_ptop_parse_entry_lr10,
    &_nvswitch_ptop_parse_enum_lr10,
    &_nvswitch_ptop_handle_data1_lr10,
    &_nvswitch_ptop_handle_data2_lr10
};

static
NVSWITCH_DISCOVERY_HANDLERS_LR10 discovery_handlers_npg_lr10 =
{
    &_nvswitch_npg_parse_entry_lr10,
    &_nvswitch_npg_parse_enum_lr10,
    &_nvswitch_npg_handle_data1_lr10,
    &_nvswitch_npg_handle_data2_lr10
};

static
NVSWITCH_DISCOVERY_HANDLERS_LR10 discovery_handlers_nvlw_lr10 =
{
    &nvswitch_nvlw_parse_entry_lr10,
    &nvswitch_nvlw_parse_enum_lr10,
    &nvswitch_nvlw_handle_data1_lr10,
    &nvswitch_nvlw_handle_data2_lr10
};

static
NVSWITCH_DISCOVERY_HANDLERS_LR10 discovery_handlers_nxbar_lr10 =
{
    &nvswitch_nxbar_parse_entry_lr10,
    &nvswitch_nxbar_parse_enum_lr10,
    &nvswitch_nxbar_handle_data1_lr10,
    &nvswitch_nxbar_handle_data2_lr10
};

//
// Parse top level PTOP engine discovery information to identify MMIO, interrupt, and
// reset information
//

NvlStatus
nvswitch_device_discovery_lr10
(
    nvswitch_device *device,
    NvU32   discovery_offset
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    DISCOVERY_TABLE_TYPE_LR10 discovery_table_lr10[] =
    {
        MAKE_DISCOVERY_LR10(device, LR10, PTOP),
        MAKE_DISCOVERY_LR10(device, LR10, NPG),
        MAKE_DISCOVERY_LR10(device, LR10, NPG_BCAST),
        MAKE_DISCOVERY_LR10(device, LR10, CLKS),
        MAKE_DISCOVERY_LR10(device, LR10, FUSE),
        MAKE_DISCOVERY_LR10(device, LR10, JTAG),
        MAKE_DISCOVERY_LR10(device, LR10, PMGR),
        MAKE_DISCOVERY_LR10(device, LR10, SAW),
        MAKE_DISCOVERY_LR10(device, LR10, XP3G),
        MAKE_DISCOVERY_LR10(device, LR10, XVE),
        MAKE_DISCOVERY_LR10(device, LR10, ROM),
        MAKE_DISCOVERY_LR10(device, LR10, EXTDEV),
        MAKE_DISCOVERY_LR10(device, LR10, PRIVMAIN),
        MAKE_DISCOVERY_LR10(device, LR10, PRIVLOC),
        MAKE_DISCOVERY_LR10(device, LR10, PTIMER),
        MAKE_DISCOVERY_LR10(device, LR10, SOE),
        MAKE_DISCOVERY_LR10(device, LR10, SMR),
        MAKE_DISCOVERY_LR10(device, LR10, I2C),
        MAKE_DISCOVERY_LR10(device, LR10, SE),
        MAKE_DISCOVERY_LR10(device, LR10, NVLW),
        MAKE_DISCOVERY_LR10(device, LR10, NVLW_BCAST),
        MAKE_DISCOVERY_LR10(device, LR10, NXBAR),
        MAKE_DISCOVERY_LR10(device, LR10, NXBAR_BCAST),
        MAKE_DISCOVERY_LR10(device, LR10, THERM)
    };
    NvU32 discovery_table_lr10_size = NV_ARRAY_ELEMENTS(discovery_table_lr10);
    NvU32 i;
    NvlStatus   status;

    status = _nvswitch_device_discovery_lr10(
        device, discovery_offset, discovery_table_lr10, discovery_table_lr10_size, 
        &discovery_handlers_ptop_lr10);
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

        DISCOVERY_DUMP_ENGINE_LR10(device, PTOP, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPG, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPG, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, CLKS, );
        DISCOVERY_DUMP_ENGINE_LR10(device, FUSE, );
        DISCOVERY_DUMP_ENGINE_LR10(device, JTAG, );
        DISCOVERY_DUMP_ENGINE_LR10(device, PMGR, );
        DISCOVERY_DUMP_ENGINE_LR10(device, SAW, );
        DISCOVERY_DUMP_ENGINE_LR10(device, XP3G, );
        DISCOVERY_DUMP_ENGINE_LR10(device, XVE, );
        DISCOVERY_DUMP_ENGINE_LR10(device, ROM, );
        DISCOVERY_DUMP_ENGINE_LR10(device, EXTDEV, );
        DISCOVERY_DUMP_ENGINE_LR10(device, PRIVMAIN, );
        DISCOVERY_DUMP_ENGINE_LR10(device, PRIVLOC, );
        DISCOVERY_DUMP_ENGINE_LR10(device, PTIMER, );
        DISCOVERY_DUMP_ENGINE_LR10(device, SOE, );
        DISCOVERY_DUMP_ENGINE_LR10(device, SMR, );
        DISCOVERY_DUMP_ENGINE_LR10(device, I2C, );
        DISCOVERY_DUMP_ENGINE_LR10(device, SE, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLW, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLW, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NXBAR, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NXBAR, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, THERM, );
    }

    for (i = 0; i < NUM_NVLW_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NVLW, i) && 
            (chip_device->engNVLW[i].info.top.discovery != 0))
        {
            DISCOVERY_TABLE_TYPE_LR10 discovery_table_nvlw[] =
            {
                MAKE_DISCOVERY_NVLINK_LR10(MINION, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT, ),
                MAKE_DISCOVERY_NVLINK_LR10(TX_PERFMON, ),
                MAKE_DISCOVERY_NVLINK_LR10(RX_PERFMON, ),
                MAKE_DISCOVERY_NVLINK_LR10(TX_PERFMON_MULTICAST, ),
                MAKE_DISCOVERY_NVLINK_LR10(RX_PERFMON_MULTICAST, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLTLC, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLTLC_MULTICAST, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT_SYS_PERFMON, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLW, ),
                MAKE_DISCOVERY_NVLINK_LR10(PLL, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLW_PERFMON, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLDL_MULTICAST, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT_LNK_MULTICAST, ),
                MAKE_DISCOVERY_NVLINK_LR10(SYS_PERFMON_MULTICAST, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLDL, ),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT_LNK, ),
                MAKE_DISCOVERY_NVLINK_LR10(SYS_PERFMON, )
            };
            NvU32 discovery_table_nvlw_size = NV_ARRAY_ELEMENTS(discovery_table_nvlw);

            status = _nvswitch_device_discovery_lr10(
                device, chip_device->engNVLW[i].info.top.discovery, discovery_table_nvlw, 
                discovery_table_nvlw_size, &discovery_handlers_nvlw_lr10);
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
             NUM_NVLW_ENGINE_LR10-1);
        DISCOVERY_DUMP_ENGINE_LR10(device, MINION, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLTLC, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLTLC_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT_SYS_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, TX_PERFMON_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, RX_PERFMON_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, TX_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, RX_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLW, );
        DISCOVERY_DUMP_ENGINE_LR10(device, PLL, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLW_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLDL_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT_LNK_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, SYS_PERFMON_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLDL, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT_LNK, );
        DISCOVERY_DUMP_ENGINE_LR10(device, SYS_PERFMON, );
    }

    for (i = 0; i < NUM_NVLW_BCAST_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NVLW_BCAST, i) && 
            (chip_device->engNVLW_BCAST[i].info.top.discovery != 0))
        {
            DISCOVERY_TABLE_TYPE_LR10 discovery_table_nvlw[] =
            {
                MAKE_DISCOVERY_NVLINK_LR10(MINION, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLTLC, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLTLC_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT_SYS_PERFMON, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(TX_PERFMON_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(RX_PERFMON_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(TX_PERFMON, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(RX_PERFMON, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLW, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(PLL, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLW_PERFMON, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLDL_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT_LNK_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(SYS_PERFMON_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLDL, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(NVLIPT_LNK, _BCAST),
                MAKE_DISCOVERY_NVLINK_LR10(SYS_PERFMON, _BCAST)
            };
            NvU32 discovery_table_nvlw_size = NV_ARRAY_ELEMENTS(discovery_table_nvlw);

            status = _nvswitch_device_discovery_lr10(
                device, chip_device->engNVLW_BCAST[i].info.top.discovery, discovery_table_nvlw, 
                discovery_table_nvlw_size, &discovery_handlers_nvlw_lr10);
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
             NUM_NVLW_BCAST_ENGINE_LR10-1);
        DISCOVERY_DUMP_ENGINE_LR10(device, MINION, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLTLC, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLTLC_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT_SYS_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, TX_PERFMON_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, RX_PERFMON_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, TX_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, RX_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLW, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, PLL, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLW_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLDL_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT_LNK_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, SYS_PERFMON_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLDL, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NVLIPT_LNK, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, SYS_PERFMON, _BCAST);
    }

    for (i = 0; i < NUM_NPG_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NPG, i) && 
            (chip_device->engNPG[i].info.top.discovery != 0))
        {
            DISCOVERY_TABLE_TYPE_LR10 discovery_table_npg[] =
            {
                MAKE_DISCOVERY_NPG_LR10(NPG, ),
                MAKE_DISCOVERY_NPG_LR10(NPORT, ),
                MAKE_DISCOVERY_NPG_LR10(NPORT_MULTICAST, ),
                MAKE_DISCOVERY_NPG_LR10(NPG_PERFMON, ),
                MAKE_DISCOVERY_NPG_LR10(NPORT_PERFMON, ),
                MAKE_DISCOVERY_NPG_LR10(NPORT_PERFMON_MULTICAST, )
            };
            NvU32 discovery_table_npg_size = NV_ARRAY_ELEMENTS(discovery_table_npg);

            status = _nvswitch_device_discovery_lr10(
                device, chip_device->engNPG[i].info.top.discovery, discovery_table_npg, 
                discovery_table_npg_size, &discovery_handlers_npg_lr10);
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
             NUM_NPG_ENGINE_LR10-1);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPG, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPG_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT_PERFMON_MULTICAST, );
    }

    for (i = 0; i < NUM_NPG_BCAST_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NPG_BCAST, i) && 
            (chip_device->engNPG_BCAST[i].info.top.discovery != 0))
        {
            DISCOVERY_TABLE_TYPE_LR10 discovery_table_npg[] =
            {
                MAKE_DISCOVERY_NPG_LR10(NPG, _BCAST),
                MAKE_DISCOVERY_NPG_LR10(NPORT, _BCAST),
                MAKE_DISCOVERY_NPG_LR10(NPORT_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NPG_LR10(NPG_PERFMON, _BCAST),
                MAKE_DISCOVERY_NPG_LR10(NPORT_PERFMON, _BCAST),
                MAKE_DISCOVERY_NPG_LR10(NPORT_PERFMON_MULTICAST, _BCAST)
            };
            NvU32 discovery_table_npg_size = NV_ARRAY_ELEMENTS(discovery_table_npg);

            status = _nvswitch_device_discovery_lr10(
                device, chip_device->engNPG_BCAST[i].info.top.discovery, discovery_table_npg, 
                discovery_table_npg_size, &discovery_handlers_npg_lr10);
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
            NUM_NPG_BCAST_ENGINE_LR10-1);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPG, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPG_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NPORT_PERFMON_MULTICAST, _BCAST);
    }

    for (i = 0; i < NUM_NXBAR_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NXBAR, i) && 
            (chip_device->engNXBAR[i].info.top.discovery != 0))
        {
            DISCOVERY_TABLE_TYPE_LR10 discovery_table_nxbar[] =
            {
                MAKE_DISCOVERY_NXBAR_LR10(NXBAR, ),
                MAKE_DISCOVERY_NXBAR_LR10(TILE, ),
                MAKE_DISCOVERY_NXBAR_LR10(TILE_MULTICAST, ),
                MAKE_DISCOVERY_NXBAR_LR10(NXBAR_PERFMON, ),
                MAKE_DISCOVERY_NXBAR_LR10(TILE_PERFMON, ),
                MAKE_DISCOVERY_NXBAR_LR10(TILE_PERFMON_MULTICAST, )
            };
            NvU32 discovery_table_nxbar_size = NV_ARRAY_ELEMENTS(discovery_table_nxbar);

            status = _nvswitch_device_discovery_lr10(
                device, chip_device->engNXBAR[i].info.top.discovery, 
                discovery_table_nxbar, discovery_table_nxbar_size, 
                &discovery_handlers_nxbar_lr10);
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
            NUM_NXBAR_ENGINE_LR10-1);
        DISCOVERY_DUMP_ENGINE_LR10(device, NXBAR, );
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE, );
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE_MULTICAST, );
        DISCOVERY_DUMP_ENGINE_LR10(device, NXBAR_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE_PERFMON, );
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE_PERFMON_MULTICAST, );
    }

    for (i = 0; i < NUM_NXBAR_BCAST_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NXBAR_BCAST, i) && 
            (chip_device->engNXBAR_BCAST[i].info.top.discovery != 0))
        {
            DISCOVERY_TABLE_TYPE_LR10 discovery_table_nxbar[] =
            {
                MAKE_DISCOVERY_NXBAR_LR10(NXBAR, _BCAST),
                MAKE_DISCOVERY_NXBAR_LR10(TILE, _BCAST),
                MAKE_DISCOVERY_NXBAR_LR10(TILE_MULTICAST, _BCAST),
                MAKE_DISCOVERY_NXBAR_LR10(NXBAR_PERFMON, _BCAST),
                MAKE_DISCOVERY_NXBAR_LR10(TILE_PERFMON, _BCAST),
                MAKE_DISCOVERY_NXBAR_LR10(TILE_PERFMON_MULTICAST, _BCAST)
            };
            NvU32 discovery_table_nxbar_size = NV_ARRAY_ELEMENTS(discovery_table_nxbar);

            status = _nvswitch_device_discovery_lr10(
                device, chip_device->engNXBAR_BCAST[i].info.top.discovery, 
                discovery_table_nxbar, discovery_table_nxbar_size, 
                &discovery_handlers_nxbar_lr10);
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
            NUM_NXBAR_BCAST_ENGINE_LR10-1);
        DISCOVERY_DUMP_ENGINE_LR10(device, NXBAR, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE_MULTICAST, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, NXBAR_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE_PERFMON, _BCAST);
        DISCOVERY_DUMP_ENGINE_LR10(device, TILE_PERFMON_MULTICAST, _BCAST);
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
nvswitch_filter_discovery_lr10
(
    nvswitch_device *device
)
{
    return;
}

#define NVSWITCH_PROCESS_DISCOVERY(_current, _engine, _multicast)           \
    {                                                                       \
        NvU32 i;                                                            \
        ct_assert(NUM_##_engine##_ENGINE_LR10 <= NVSWITCH_ENGINE_DESCRIPTOR_UC_SIZE); \
                                                                            \
        _current->eng_name = #_engine;                                      \
        _current->eng_id = NVSWITCH_ENGINE_ID_##_engine;                    \
        _current->eng_count = NUM_##_engine##_ENGINE_LR10;                  \
                                                                            \
        for (i = 0; i < NUM_##_engine##_ENGINE_LR10; i++)                   \
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
nvswitch_process_discovery_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32       i, j;
    NvlStatus   retval = NVL_SUCCESS;
    NvU64       link_enable_mask;

    //
    // Process per-link information
    //
    for (i = 0; i < NVSWITCH_NUM_LINKS_LR10; i++)
    {
        device->link[i].valid = 
            NVSWITCH_ENG_VALID_LR10(device, NPORT, NVSWITCH_GET_LINK_ENG_INST(device, i, NPORT)) &&
            NVSWITCH_ENG_VALID_LR10(device, NVLTLC, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLTLC)) &&
            NVSWITCH_ENG_VALID_LR10(device, NVLDL, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLDL)) &&
            NVSWITCH_ENG_VALID_LR10(device, NVLIPT_LNK, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT_LNK)) &&
            NVSWITCH_ENG_VALID_LR10(device, NVLW, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLW)) &&
            NVSWITCH_ENG_VALID_LR10(device, MINION, NVSWITCH_GET_LINK_ENG_INST(device, i, MINION)) &&
            NVSWITCH_ENG_VALID_LR10(device, NVLIPT, NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT));
    }

    //
    // Disable engines requested by regkey "LinkEnableMask".
    // All the links are enabled by default.
    //
    link_enable_mask = ((NvU64)device->regkeys.link_enable_mask2 << 32 |
        (NvU64)device->regkeys.link_enable_mask);

    for (i = 0; i < NVSWITCH_NUM_LINKS_LR10; i++)
    {
        if ((NVBIT64(i) & link_enable_mask) == 0)
        {
            NVSWITCH_PRINT(device, SETUP,
                "%s: Disable link #%d\n",
                __FUNCTION__, i);
            device->link[i].valid                  = NV_FALSE;
            chip_device->engNPORT[i].valid         = NV_FALSE;
            chip_device->engNPORT_PERFMON[i].valid = NV_FALSE;
            chip_device->engNVLTLC[i].valid        = NV_FALSE;
            chip_device->engTX_PERFMON[i].valid    = NV_FALSE;
            chip_device->engRX_PERFMON[i].valid    = NV_FALSE;
        }
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

    NVSWITCH_LIST_LR10_ENGINES(NVSWITCH_PROCESS_COMMON)

    return retval;
}
