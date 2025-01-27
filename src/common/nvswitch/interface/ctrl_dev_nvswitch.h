/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file defines CTRL calls that are device specifics.
 *
 * This is a platform agnostic file and lists the CTRL calls used by all the
 * clients, Fabric Manager, MODS or NVSwitch GTEST etc.
 *
 * As Fabric Manager relies on driver ABI compatibility the CTRL calls listed in
 * this file contribute to the driver ABI version.
 *
 * Note: ctrl_dev_nvswitch.h and ctrl_dev_internal_nvswitch.h do not share any
 * data. This helps to keep the driver ABI stable.
 */

#ifndef _CTRL_DEVICE_NVSWITCH_H_
#define _CTRL_DEVICE_NVSWITCH_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "nvtypes.h"
#include "nvfixedtypes.h"
#include "nvmisc.h"
#include "ioctl_common_nvswitch.h"

/*
 * CTRL_NVSWITCH_GET_INFO
 *
 * Control for querying miscellaneous device information.
 *
 * This provides a single API to query for multiple pieces of miscellaneous
 * information via a single call.
 *
 * Parameters:
 *   count [IN]
 *      Count of queries. Max supported queries per-call are
 *      NVSWITCH_GET_INFO_COUNT_MAX
 *   index [IN]
 *      One of the NVSWITCH_GET_INFO_INDEX type value.
 *
 *   info [OUT]
 *      Data pertaining to the provided NVSWITCH_GET_INFO_INDEX type value.
 */

#define NVSWITCH_GET_INFO_COUNT_MAX 32

typedef enum nvswitch_get_info_index
{
    NVSWITCH_GET_INFO_INDEX_ARCH = 0x0,
    NVSWITCH_GET_INFO_INDEX_IMPL,
    NVSWITCH_GET_INFO_INDEX_CHIPID,
    NVSWITCH_GET_INFO_INDEX_REVISION_MAJOR,
    NVSWITCH_GET_INFO_INDEX_REVISION_MINOR,
    NVSWITCH_GET_INFO_INDEX_REVISION_MINOR_EXT,
    NVSWITCH_GET_INFO_INDEX_PLATFORM,
    NVSWITCH_GET_INFO_INDEX_DEVICE_ID,

    NVSWITCH_GET_INFO_INDEX_NUM_PORTS = 0x100,
    NVSWITCH_GET_INFO_INDEX_ENABLED_PORTS_MASK_31_0,
    NVSWITCH_GET_INFO_INDEX_ENABLED_PORTS_MASK_63_32,
    NVSWITCH_GET_INFO_INDEX_NUM_VCS,
    NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_TABLE_SIZE,
    NVSWITCH_GET_INFO_INDEX_ROUTING_ID_TABLE_SIZE,
    NVSWITCH_GET_INFO_INDEX_ROUTING_LAN_TABLE_SIZE,
    NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_EXTA_TABLE_SIZE,
    NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_EXTB_TABLE_SIZE,
    NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_MULTICAST_TABLE_SIZE,

    NVSWITCH_GET_INFO_INDEX_FREQ_KHZ = 0x200,
    NVSWITCH_GET_INFO_INDEX_VCOFREQ_KHZ,
    NVSWITCH_GET_INFO_INDEX_VOLTAGE_MVOLT,
    NVSWITCH_GET_INFO_INDEX_PHYSICAL_ID,

    NVSWITCH_GET_INFO_INDEX_PCI_DOMAIN = 0x300,
    NVSWITCH_GET_INFO_INDEX_PCI_BUS,
    NVSWITCH_GET_INFO_INDEX_PCI_DEVICE,
    NVSWITCH_GET_INFO_INDEX_PCI_FUNCTION,
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_GET_INFO_INDEX;

#define NVSWITCH_GET_INFO_INDEX_ARCH_LR10     0x02
#define NVSWITCH_GET_INFO_INDEX_IMPL_LR10     0x01

#define NVSWITCH_GET_INFO_INDEX_ARCH_LS10     0x03
#define NVSWITCH_GET_INFO_INDEX_IMPL_LS10     0x01

#define NVSWITCH_GET_INFO_INDEX_PLATFORM_UNKNOWN    0x00
#define NVSWITCH_GET_INFO_INDEX_PLATFORM_RTLSIM     0x01
#define NVSWITCH_GET_INFO_INDEX_PLATFORM_FMODEL     0x02
#define NVSWITCH_GET_INFO_INDEX_PLATFORM_EMULATION  0x03
#define NVSWITCH_GET_INFO_INDEX_PLATFORM_SILICON    0x04

typedef struct nvswitch_get_info
{
    NvU32 count;
    NvU32 index[NVSWITCH_GET_INFO_COUNT_MAX];
    NvU32 info[NVSWITCH_GET_INFO_COUNT_MAX];

} NVSWITCH_GET_INFO;

/*
 * CTRL_NVSWITCH_SET_INGRESS_REQUEST_TABLE
 *
 * Control for programming ingress request tables.
 * This interface is only supported on SV10 architecture.  All others will
 * return an error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the ingress request table from which table entries
 *      should be programmed.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports
 *      programming NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX at a time.
 *   entries [IN]
 *      The entries (entry format is architecture dependent).
 */

#define NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX 256

/* TODO: document the entry format in detail */
typedef struct nvswitch_ingress_request_entry
{
    NvU32  vcModeValid7_0;
    NvU32  vcModeValid15_8;
    NvU32  vcModeValid17_16;
    NvU32  mappedAddress;
    NvU32  routePolicy;
    NvBool entryValid;

} NVSWITCH_INGRESS_REQUEST_ENTRY;

typedef struct nvswitch_set_ingress_request_table
{
    NvU32                          portNum;
    NvU32                          firstIndex;
    NvU32                          numEntries;
    NVSWITCH_INGRESS_REQUEST_ENTRY entries[NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX];

} NVSWITCH_SET_INGRESS_REQUEST_TABLE;

/*
 * CTRL_NVSWITCH_GET_INGRESS_REQUEST_TABLE
 *
 * Control for reading ingress request tables. A sparse list of nonzero entries
 * and their table indices is returned.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the ingress request table from which table entries
 *      should be read.
 *   nextIndex [OUT]
 *      The table index of the next entry to read. Set to INGRESS_MAP_TABLE_SIZE
 *      when the end of the table has been reached.
 *   numEntries [OUT]
 *      Number of entries returned. Currently, the call supports returning up to
 *      NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX entries at a time.
 *   entries [OUT]
 *      Ingress request entries along with their table indices.
 *      Entry format is architecture dependent.
 */

typedef struct nvswitch_ingress_request_idx_entry
{
    NvU32                          idx;
    NVSWITCH_INGRESS_REQUEST_ENTRY entry;

} NVSWITCH_INGRESS_REQUEST_IDX_ENTRY;

typedef struct nvswitch_get_ingress_request_table_params
{
    NvU32                               portNum;
    NvU32                               firstIndex;
    NvU32                               nextIndex;
    NvU32                               numEntries;
    NVSWITCH_INGRESS_REQUEST_IDX_ENTRY  entries[NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX];

} NVSWITCH_GET_INGRESS_REQUEST_TABLE_PARAMS;

/*
 * CTRL_NVSWITCH_SET_INGRESS_REQUEST_VALID
 *
 * Control for toggling the existing ingress request table entries' validity.
 * This interface is only supported on SV10 architecture.  All others will
 * return an error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the ingress request table from which table entries
 *      should be programmed.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports
 *      programming NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX at a time.
 *   entryValid [IN]
 *      If true, an existing entry is marked valid, else will be marked invalid.
 */

typedef struct nvswitch_set_ingress_request_valid
{
    NvU32  portNum;
    NvU32  firstIndex;
    NvU32  numEntries;
    NvBool entryValid[NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX];

} NVSWITCH_SET_INGRESS_REQUEST_VALID;

/*
 * CTRL_NVSWITCH_SET_INGRESS_RESPONSE_TABLE
 *
 * Control for programming ingress response tables.
 * This interface is only supported on SV10 architecture.  All others will
 * return an error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the ingress request table from which table entries
 *      should be programmed.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports
 *      programming NVSWITCH_INGRESS_REQUEST_ENTRIES_MAX at a time.
 *   entries [IN]
 *      The entries (entry format is architecture dependent).
 */

#define NVSWITCH_INGRESS_RESPONSE_ENTRIES_MAX 256

/* TODO: document the entry format in detail */
typedef struct nvswitch_ingress_response_entry
{
    NvU32  vcModeValid7_0;
    NvU32  vcModeValid15_8;
    NvU32  vcModeValid17_16;
    NvU32  routePolicy;
    NvBool entryValid;

} NVSWITCH_INGRESS_RESPONSE_ENTRY;

typedef struct nvswitch_set_ingress_response_table
{
    NvU32                           portNum;
    NvU32                           firstIndex;
    NvU32                           numEntries;
    NVSWITCH_INGRESS_RESPONSE_ENTRY entries[NVSWITCH_INGRESS_RESPONSE_ENTRIES_MAX];

} NVSWITCH_SET_INGRESS_RESPONSE_TABLE;

/*
 * CTRL_NVSWITCH_SET_REMAP_POLICY
 *
 * Control to load remap policy table
 * This interface is not supported on SV10 architecture.  SV10 will return an
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   tableSelect [IN]
 *      Remap table selector
 *   firstIndex [IN]
 *      A starting index of the remap table from which table entries
 *      should be programmed.  Valid range should be queried using
 *      NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_TABLE_SIZE.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports
 *      programming NVSWITCH_REMAP_POLICY_ENTRIES_MAX at a time.
 *   remapPolicy [IN]
 *      The entries (see NVSWITCH_REMAP_POLICY_ENTRY).
 */

#define NVSWITCH_REMAP_POLICY_ENTRIES_MAX 64

#define NVSWITCH_REMAP_POLICY_FLAGS_REMAP_ADDR      NVBIT(0)
#define NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_CHECK   NVBIT(1)
#define NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_REPLACE NVBIT(2)
#define NVSWITCH_REMAP_POLICY_FLAGS_ADR_BASE        NVBIT(4)
#define NVSWITCH_REMAP_POLICY_FLAGS_ADR_OFFSET      NVBIT(5)    /* Apply address offset */
#define NVSWITCH_REMAP_POLICY_FLAGS_REFLECTIVE      NVBIT(30)   /* Reflective mapping */
#define NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE       NVBIT(31)   /* Enforce address type checking */

typedef struct nvswitch_remap_policy_entry
{
    NvBool entryValid;
    NvU32  targetId;                            /* Unique endpoint ID */

    NvU32  irlSelect;                           /* Injection rate limiter (0=none/1=IRL1/2=IRL2) */

    NvU32  flags;                               /* See NVSWITCH_REMAP_POLICY_FLAGS_* */

    NV_DECLARE_ALIGNED(NvU64 address, 8);       /* 47-bit remap address. Bits 46:36 are used. */

                                                /* reqContext fields are used when */
                                                /* routing function _REQCTXT_CHECK or _REPLACE */
                                                /* is set. */
    NvU32  reqCtxMask;                          /* Used to mask packet request ctxt before */
                                                /* checking. */

    NvU32  reqCtxChk;                           /* Post-mask packet request ctxt check value. */
                                                /* Packets that fail compare are converted to */
                                                /* UR response and looped back. */

    NvU32  reqCtxRep;                           /* Replaces packet request context when */
                                                /* _REQCTXT_REPLACE is set. */

    NV_DECLARE_ALIGNED(NvU64 addressOffset, 8); /* offset - base is added to packet address if */
                                                /* routing function _ADR_OFFSET & _ADR_BASE are */
                                                /* set. 64GB offset 1MB aligned on LR10. */

    NV_DECLARE_ALIGNED(NvU64 addressBase,  8);  /* If routing function _ADR_BASE is set, limits */
    NV_DECLARE_ALIGNED(NvU64 addressLimit, 8);  /* application of _ADR_OFFSET to packet */
                                                /* addresses that pass base/limit bounds check. */
                                                /* Maximum 64GB size 1MB aligned on LR10. */


} NVSWITCH_REMAP_POLICY_ENTRY;

typedef enum nvswitch_table_select_remap
{
    NVSWITCH_TABLE_SELECT_REMAP_PRIMARY = 0,
    NVSWITCH_TABLE_SELECT_REMAP_EXTA,
    NVSWITCH_TABLE_SELECT_REMAP_EXTB,
    NVSWITCH_TABLE_SELECT_REMAP_MULTICAST,
    NVSWITCH_TABLE_SELECT_REMAP_MAX
} NVSWITCH_TABLE_SELECT_REMAP;

typedef struct nvswitch_set_remap_policy
{
    NvU32                       portNum;
    NVSWITCH_TABLE_SELECT_REMAP tableSelect;
    NvU32                       firstIndex;
    NvU32                       numEntries;
    NVSWITCH_REMAP_POLICY_ENTRY remapPolicy[NVSWITCH_REMAP_POLICY_ENTRIES_MAX];

} NVSWITCH_SET_REMAP_POLICY;

/*
 * CTRL_NVSWITCH_GET_REMAP_POLICY
 *
 * Control to get remap policy table
 * This interface is not supported on SV10 architecture. SV10 will return unsupported
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   tableSelect [IN]
 *      Remap table selector
 *   firstIndex [IN]
 *      A starting index of the remap policy table from which table entries
 *      should be read.
 *   numEntries [OUT]
 *      Number of entries returned. This call returns
 *      NVSWITCH_REMAP_POLICY_ENTRIES_MAX entries at a time.
 *   nextIndex [OUT]
 *      The table index of the next entry to read. Set to INGRESS_REMAPTAB_SIZE
 *      when the end of the table has been reached.
 *   entries [OUT]
 *      The entries (see NVSWITCH_REMAP_POLICY_ENTRY).
 */


typedef struct nvswitch_get_remap_policy_params
{
    NvU32                             portNum;
    NVSWITCH_TABLE_SELECT_REMAP       tableSelect;
    NvU32                             firstIndex;
    NvU32                             numEntries;
    NvU32                             nextIndex;
    NVSWITCH_REMAP_POLICY_ENTRY       entry[NVSWITCH_REMAP_POLICY_ENTRIES_MAX];

} NVSWITCH_GET_REMAP_POLICY_PARAMS;

/*
 * CTRL_NVSWITCH_SET_REMAP_POLICY_VALID
 *
 * Control to set remap policy tables valid/invalid
 * This interface is not supported on SV10 architecture.  SV10 will return unsupported
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   tableSelect [IN]
 *      Remap table selector
 *   firstIndex [IN]
 *      A starting index of the remap policy table from which table entries
 *      should be programmed.
 *   numEntries [IN]
 *      Number of entries to be programmed. The call supports
 *      programming of maximum NVSWITCH_REMAP_POLICY_ENTRIES_MAX at a time.
 *   entryValid [IN]
 *      If true, an existing entry is marked valid, else will be marked invalid.
 */

typedef struct nvswitch_set_remap_policy_valid
{
    NvU32                      portNum;
    NVSWITCH_TABLE_SELECT_REMAP tableSelect;
    NvU32                      firstIndex;
    NvU32                      numEntries;
    NvBool                     entryValid[NVSWITCH_REMAP_POLICY_ENTRIES_MAX];

} NVSWITCH_SET_REMAP_POLICY_VALID;

/*
 * CTRL_NVSWITCH_SET_ROUTING_ID
 *
 * Control to load Routing ID table
 * The routing ID table configures the VC and routing policy as well as the
 * valid set if ganged link routes.
 * This interface is not supported on SV10 architecture.  SV10 will return an
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the routing ID table from which table entries
 *      should be programmed.  Valid range should be queried using
 *      NVSWITCH_GET_INFO_INDEX_ROUTING_ID_TABLE_SIZE.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports programming
 *      maximum of NVSWITCH_ROUTING_ID_ENTRIES_MAX entries at a time.
 *   routingId [IN]
 *      The entries (see NVSWITCH_ROUTING_ID_ENTRY).
 */

#define NVSWITCH_ROUTING_ID_DEST_PORT_LIST_MAX  16
#define NVSWITCH_ROUTING_ID_VC_MODE_MAX          4
#define NVSWITCH_ROUTING_ID_ENTRIES_MAX         64

typedef enum nvswitch_routing_id_vcmap
{
    NVSWITCH_ROUTING_ID_VCMAP_SAME = 0x0,
    NVSWITCH_ROUTING_ID_VCMAP_INVERT,
    NVSWITCH_ROUTING_ID_VCMAP_ZERO,
    NVSWITCH_ROUTING_ID_VCMAP_ONE
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_ROUTING_ID_VCMAP;

typedef struct nvswitch_routing_id_dest_port_list
{
    NvU32 vcMap;      /* NVSWITCH_ROUTING_ID_VCMAP_* */
    NvU32 destPortNum;

} NVSWITCH_ROUTING_ID_DEST_PORT_LIST;

typedef struct nvswitch_routing_id_entry
{
    NvBool                              entryValid;
    NvBool                              useRoutingLan;
    NvBool                              enableIrlErrResponse;
    NvU32                               numEntries;
    NVSWITCH_ROUTING_ID_DEST_PORT_LIST  portList[NVSWITCH_ROUTING_ID_DEST_PORT_LIST_MAX];

} NVSWITCH_ROUTING_ID_ENTRY;

typedef struct nvswitch_set_routing_id
{
    NvU32                       portNum;
    NvU32                       firstIndex;
    NvU32                       numEntries;
    NVSWITCH_ROUTING_ID_ENTRY   routingId[NVSWITCH_ROUTING_ID_ENTRIES_MAX];

} NVSWITCH_SET_ROUTING_ID;

/*
 * CTRL_NVSWITCH_GET_ROUTING_ID
 *
 * Control to get routing ID table
 * This interface is not supported on SV10 architecture. SV10 will return unsupported
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the routing id table from which table entries
 *      should be read.
 *   numEntries [OUT]
 *      Number of entries returned. The call returns only
 *      NVSWITCH_ROUTING_ID_ENTRIES_MAX entries at a time.
 *   nextIndex [OUT]
 *      The table index of the next entry to read. Set to INGRESS_RIDTAB_SIZE
 *      when the end of the table has been reached.
 *   entries [OUT]
 *      The entries (see NVSWITCH_ROUTING_ID_IDX_ENTRY).
 */

typedef struct nvswitch_routing_id_idx_entry
{
    NvU32                               idx;
    NVSWITCH_ROUTING_ID_ENTRY          entry;

} NVSWITCH_ROUTING_ID_IDX_ENTRY;

typedef struct nvswitch_get_routing_id_params
{
    NvU32                             portNum;
    NvU32                             firstIndex;
    NvU32                             numEntries;
    NvU32                             nextIndex;
    NVSWITCH_ROUTING_ID_IDX_ENTRY     entries[NVSWITCH_ROUTING_ID_ENTRIES_MAX];

} NVSWITCH_GET_ROUTING_ID_PARAMS;

/*
 * CTRL_NVSWITCH_SET_ROUTING_ID_VALID
 *
 * Control to set routing ID tables valid/invalid
 * This interface is not supported on SV10 architecture.  SV10 will return unsupported
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the routing lan table from which table entries
 *      should be programmed.
 *   numEntries [IN]
 *      Number of entries to be programmed. This call supports programming
 *      maximum entries of NVSWITCH_ROUTING_ID_ENTRIES_MAX at a time.
 *   entryValid [IN]
 *      If true, an existing entry is marked valid, else will be marked invalid.
 */

typedef struct nvswitch_set_routing_id_valid
{
    NvU32                      portNum;
    NvU32                      firstIndex;
    NvU32                      numEntries;
    NvBool                     entryValid[NVSWITCH_ROUTING_ID_ENTRIES_MAX];

} NVSWITCH_SET_ROUTING_ID_VALID;

/*
 * CTRL_NVSWITCH_SET_ROUTING_LAN
 *
 * Control to load routing LAN table
 * This interface is not supported on SV10 architecture.  SV10 will return an
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the ingress request table from which table entries
 *      should be programmed.  Valid range should be queried using
 *      NVSWITCH_GET_INFO_INDEX_ROUTING_LAN_TABLE_SIZE.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports
 *      programming NVSWITCH_ROUTING_LAN_ENTRIES_MAX at a time.
 *   routingLan [IN]
 *      The entries (see NVSWITCH_ROUTING_LAN_ENTRY).
 */

#define NVSWITCH_ROUTING_LAN_GROUP_SEL_MAX  16
#define NVSWITCH_ROUTING_LAN_GROUP_SIZE_MAX 16
#define NVSWITCH_ROUTING_LAN_ENTRIES_MAX    64

typedef struct nvswitch_routing_lan_port_select
{
    NvU32  groupSelect;                 /* Port list group selector */
    NvU32  groupSize;                   /* Valid range: 1..16 */

} NVSWITCH_ROUTING_LAN_PORT_SELECT;

typedef struct nvswitch_routing_lan_entry
{
    NvBool                              entryValid;
    NvU32                               numEntries;
    NVSWITCH_ROUTING_LAN_PORT_SELECT    portList[NVSWITCH_ROUTING_LAN_GROUP_SEL_MAX];

} NVSWITCH_ROUTING_LAN_ENTRY;

typedef struct nvswitch_set_routing_lan
{
    NvU32                      portNum;
    NvU32                      firstIndex;
    NvU32                      numEntries;
    NVSWITCH_ROUTING_LAN_ENTRY routingLan[NVSWITCH_ROUTING_LAN_ENTRIES_MAX];

} NVSWITCH_SET_ROUTING_LAN;

/*
 * CTRL_NVSWITCH_GET_ROUTING_LAN
 *
 * Control to get routing LAN table
 * This interface is not supported on SV10 architecture. SV10 will return unsupported
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the routing lan table from which table entries
 *      should be read.
 *   numEntries [OUT]
 *      Number of entries returned. Currently, the call supports
 *      NVSWITCH_ROUTING_LAN_ENTRIES_MAX at a time.
 *   nextIndex [OUT]
 *      The table index of the next entry to read. Set to INGRESS_RLANTAB_SIZE
 *      when the end of the table has been reached.
 *   entries [OUT]
 *      The entries (see NVSWITCH_ROUTING_LAN_IDX_ENTRY).
 */

typedef struct nvswitch_routing_lan_idx_entry
{
    NvU32                               idx;
    NVSWITCH_ROUTING_LAN_ENTRY          entry;

} NVSWITCH_ROUTING_LAN_IDX_ENTRY;

typedef struct nvswitch_get_routing_lan_params
{
    NvU32                             portNum;
    NvU32                             firstIndex;
    NvU32                             numEntries;
    NvU32                             nextIndex;
    NVSWITCH_ROUTING_LAN_IDX_ENTRY    entries[NVSWITCH_ROUTING_LAN_ENTRIES_MAX];

} NVSWITCH_GET_ROUTING_LAN_PARAMS;

/*
 * CTRL_NVSWITCH_SET_ROUTING_LAN_VALID
 *
 * Control to set routing LAN tables valid/invalid
 * This interface is not supported on SV10 architecture.  SV10 will return unsupported
 * error. Architecture can be queried using _GET_INFO_INDEX_ARCH. 
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the routing lan table from which table entries
 *      should be programmed.
 *   numEntries [IN]
 *      Number of entries to be programmed. Currently, the call supports
 *      programming NVSWITCH_ROUTING_LAN_ENTRIES_MAX at a time.
 *   entryValid [IN]
 *      If true, an existing entry is marked valid, else will be marked invalid.
 */

typedef struct nvswitch_set_routing_lan_valid
{
    NvU32                      portNum;
    NvU32                      firstIndex;
    NvU32                      numEntries;
    NvBool                     entryValid[NVSWITCH_ROUTING_LAN_ENTRIES_MAX];

} NVSWITCH_SET_ROUTING_LAN_VALID;

/*
 * CTRL_NVSWITCH_GET_INGRESS_RESPONSE_TABLE
 *
 * Control for reading ingress response tables. A sparse list of nonzero entries
 * and their table indices is returned.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   firstIndex [IN]
 *      A starting index of the ingress response table from which table entries
 *      should be read.
 *   nextIndex [OUT]
 *      The table index of the next entry to read. Set to INGRESS_MAP_TABLE_SIZE
 *      when the end of the table has been reached.
 *   numEntries [OUT]
 *      Number of entries returned. Currently, the call supports returning up to
 *      NVSWITCH_INGRESS_RESPONSE_ENTRIES_MAX entries at a time.
 *   entries [OUT]
 *      Ingress response entries along with their table indices.
 *      Entry format is architecture dependent.
 */

typedef struct nvswitch_ingress_response_idx_entry
{
    NvU32                               idx;
    NVSWITCH_INGRESS_RESPONSE_ENTRY     entry;

} NVSWITCH_INGRESS_RESPONSE_IDX_ENTRY;

typedef struct nvswitch_get_ingress_response_table_params
{
    NvU32                               portNum;
    NvU32                               firstIndex;
    NvU32                               nextIndex;
    NvU32                               numEntries;
    NVSWITCH_INGRESS_RESPONSE_IDX_ENTRY entries[NVSWITCH_INGRESS_RESPONSE_ENTRIES_MAX];

} NVSWITCH_GET_INGRESS_RESPONSE_TABLE_PARAMS;

/*
 * CTRL_NVSWITCH_GET_VOLTAGE
 *
 * Zero(0) indicates that a measurement is not available on the current platform.
 *
 */

typedef struct
{
    NvU32 vdd_mv;
    NvU32 dvdd_mv;
    NvU32 hvdd_mv;
} NVSWITCH_CTRL_GET_VOLTAGE_PARAMS;

/*
 * CTRL_NVSWITCH_GET_POWER
 *
 *    Zero(0) indicates that a measurement is not available 
 *         on the current platform.
 */
typedef struct
{
    NvU32 vdd_w;
    NvU32 dvdd_w;
    NvU32 hvdd_w;
} NVSWITCH_GET_POWER_PARAMS;

/*
 * CTRL_NVSWITCH_GET_ERRORS
 *
 * Control to query error information.
 *
 * Parameters:
 *   errorType [IN]
 *      Allows to query specific class of errors. See NVSWITCH_ERROR_SEVERITY_xxx.
 *
 *   errorIndex [IN/OUT]
 *      On input: The index of the first error of the specified 'errorType' at which to start
 *                reading out of the driver.
 *
 *      On output: The index of the first error that wasn't reported through the 'error' array
 *                 in this call to CTRL_NVSWITCH_GET_ERRORS. Specific to the specified 'errorType'.
 *
 *   nextErrorIndex[OUT]
 *      The index that will be assigned to the next error to occur for the specified 'errorType'.
 *      Users of the GET_ERRORS control call may set 'errorIndex' to this field on initialization
 *      to bypass errors that have already occurred without making multiple control calls.
 *
 *   errorCount [OUT]
 *      Number of errors returned by the call. Currently, errorCount is limited
 *      by NVSWITCH_ERROR_COUNT_SIZE. In order to query all the errors, a
 *      client needs to keep calling the control till errorCount is zero.
 *   error [OUT]
 *      The error entires.
 */

typedef enum nvswitch_error_severity_type
{
    NVSWITCH_ERROR_SEVERITY_NONFATAL = 0,
    NVSWITCH_ERROR_SEVERITY_FATAL,
    NVSWITCH_ERROR_SEVERITY_MAX
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_ERROR_SEVERITY_TYPE;

typedef enum nvswitch_error_src_type
{
    NVSWITCH_ERROR_SRC_NONE = 0,
    NVSWITCH_ERROR_SRC_HW
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_ERROR_SRC_TYPE;

typedef enum nvswitch_err_type
{
    NVSWITCH_ERR_NO_ERROR                                                = 0x0,

    /*
     * These error enumerations are derived from the error bits defined in each
     * hardware manual.
     *
     * NVSwitch errors values should start from 10000 (decimal) to be
     * distinguishable from GPU errors.
     */

    /* HOST */
    NVSWITCH_ERR_HW_HOST                                               = 10000,
    NVSWITCH_ERR_HW_HOST_PRIV_ERROR                                    = 10001,
    NVSWITCH_ERR_HW_HOST_PRIV_TIMEOUT                                  = 10002,
    NVSWITCH_ERR_HW_HOST_UNHANDLED_INTERRUPT                           = 10003,
    NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_START                           = 10004,
    NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_END                             = 10005,
    NVSWITCH_ERR_HW_HOST_THERMAL_SHUTDOWN                              = 10006,
    NVSWITCH_ERR_HW_HOST_IO_FAILURE                                    = 10007,
    NVSWITCH_ERR_HW_HOST_FIRMWARE_INITIALIZATION_FAILURE               = 10008,
    NVSWITCH_ERR_HW_HOST_FIRMWARE_RECOVERY_MODE                        = 10009,
    NVSWITCH_ERR_HW_HOST_TNVL_ERROR                                    = 10010,
    NVSWITCH_ERR_HW_HOST_LAST,


    /* NPORT: Ingress errors */
    NVSWITCH_ERR_HW_NPORT_INGRESS                                      = 11000,
    NVSWITCH_ERR_HW_NPORT_INGRESS_CMDDECODEERR                         = 11001,
    NVSWITCH_ERR_HW_NPORT_INGRESS_BDFMISMATCHERR                       = 11002,
    NVSWITCH_ERR_HW_NPORT_INGRESS_BUBBLEDETECT                         = 11003,
    NVSWITCH_ERR_HW_NPORT_INGRESS_ACLFAIL                              = 11004,
    NVSWITCH_ERR_HW_NPORT_INGRESS_PKTPOISONSET                         = 11005,
    NVSWITCH_ERR_HW_NPORT_INGRESS_ECCSOFTLIMITERR                      = 11006,
    NVSWITCH_ERR_HW_NPORT_INGRESS_ECCHDRDOUBLEBITERR                   = 11007,
    NVSWITCH_ERR_HW_NPORT_INGRESS_INVALIDCMD                           = 11008,
    NVSWITCH_ERR_HW_NPORT_INGRESS_INVALIDVCSET                         = 11009,
    NVSWITCH_ERR_HW_NPORT_INGRESS_ERRORINFO                            = 11010,
    NVSWITCH_ERR_HW_NPORT_INGRESS_REQCONTEXTMISMATCHERR                = 11011,
    NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_HDR_ECC_LIMIT_ERR             = 11012,
    NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR               = 11013,
    NVSWITCH_ERR_HW_NPORT_INGRESS_ADDRBOUNDSERR                        = 11014,
    NVSWITCH_ERR_HW_NPORT_INGRESS_RIDTABCFGERR                         = 11015,
    NVSWITCH_ERR_HW_NPORT_INGRESS_RLANTABCFGERR                        = 11016,
    NVSWITCH_ERR_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR                 = 11017,
    NVSWITCH_ERR_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR                   = 11018,
    NVSWITCH_ERR_HW_NPORT_INGRESS_RLANTAB_ECC_DBE_ERR                  = 11019,
    NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_PARITY_ERR                    = 11020,
    NVSWITCH_ERR_HW_NPORT_INGRESS_REMAPTAB_ECC_LIMIT_ERR               = 11021,
    NVSWITCH_ERR_HW_NPORT_INGRESS_RIDTAB_ECC_LIMIT_ERR                 = 11022,
    NVSWITCH_ERR_HW_NPORT_INGRESS_RLANTAB_ECC_LIMIT_ERR                = 11023,
    NVSWITCH_ERR_HW_NPORT_INGRESS_ADDRTYPEERR                          = 11024,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_INDEX_ERR               = 11025,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_INDEX_ERR               = 11026,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_INDEX_ERR                 = 11027,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_DBE_ERR             = 11028,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_DBE_ERR             = 11029,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ECC_DBE_ERR               = 11030,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_REQCONTEXTMISMATCHERR   = 11031,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_REQCONTEXTMISMATCHERR   = 11032,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_REQCONTEXTMISMATCHERR     = 11033,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ACLFAIL                 = 11034,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ACLFAIL                 = 11035,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ACLFAIL                   = 11036,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ADDRBOUNDSERR           = 11037,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ADDRBOUNDSERR           = 11038,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ADDRBOUNDSERR             = 11039,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_LIMIT_ERR           = 11040,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_LIMIT_ERR           = 11041,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ECC_LIMIT_ERR             = 11042,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCCMDTOUCADDRERR                     = 11043,
    NVSWITCH_ERR_HW_NPORT_INGRESS_READMCREFLECTMEMERR                  = 11044,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ADDRTYPEERR             = 11045,
    NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ADDRTYPEERR             = 11046,
    NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ADDRTYPEERR               = 11047,
    NVSWITCH_ERR_HW_NPORT_INGRESS_LAST, /* NOTE: Must be last */

    /* NPORT: Egress errors */
    NVSWITCH_ERR_HW_NPORT_EGRESS                                       = 12000,
    NVSWITCH_ERR_HW_NPORT_EGRESS_EGRESSBUFERR                          = 12001,
    NVSWITCH_ERR_HW_NPORT_EGRESS_PKTROUTEERR                           = 12002,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ECCSINGLEBITLIMITERR0                 = 12003,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ECCHDRDOUBLEBITERR0                   = 12004,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ECCDATADOUBLEBITERR0                  = 12005,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ECCSINGLEBITLIMITERR1                 = 12006,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ECCHDRDOUBLEBITERR1                   = 12007,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ECCDATADOUBLEBITERR1                  = 12008,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NCISOCHDRCREDITOVFL                   = 12009,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NCISOCDATACREDITOVFL                  = 12010,
    NVSWITCH_ERR_HW_NPORT_EGRESS_ADDRMATCHERR                          = 12011,
    NVSWITCH_ERR_HW_NPORT_EGRESS_TAGCOUNTERR                           = 12012,
    NVSWITCH_ERR_HW_NPORT_EGRESS_FLUSHRSPERR                           = 12013,
    NVSWITCH_ERR_HW_NPORT_EGRESS_DROPNPURRSPERR                        = 12014,
    NVSWITCH_ERR_HW_NPORT_EGRESS_POISONERR                             = 12015,
    NVSWITCH_ERR_HW_NPORT_EGRESS_PACKET_HEADER                         = 12016,
    NVSWITCH_ERR_HW_NPORT_EGRESS_BUFFER_DATA                           = 12017,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NCISOC_CREDITS                        = 12018,
    NVSWITCH_ERR_HW_NPORT_EGRESS_TAG_DATA                              = 12019,
    NVSWITCH_ERR_HW_NPORT_EGRESS_SEQIDERR                              = 12020,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR               = 12021,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_ECC_DBE_ERR                 = 12022,
    NVSWITCH_ERR_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR             = 12023,
    NVSWITCH_ERR_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR               = 12024,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NCISOCCREDITOVFL                      = 12025,
    NVSWITCH_ERR_HW_NPORT_EGRESS_REQTGTIDMISMATCHERR                   = 12026,
    NVSWITCH_ERR_HW_NPORT_EGRESS_RSPREQIDMISMATCHERR                   = 12027,
    NVSWITCH_ERR_HW_NPORT_EGRESS_PRIVRSPERR                            = 12028,
    NVSWITCH_ERR_HW_NPORT_EGRESS_HWRSPERR                              = 12029,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR                  = 12030,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR              = 12031,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_FLITTYPE_MISMATCH_ERR           = 12032,
    NVSWITCH_ERR_HW_NPORT_EGRESS_CREDIT_TIME_OUT_ERR                   = 12033,
    NVSWITCH_ERR_HW_NPORT_EGRESS_INVALIDVCSET_ERR                      = 12034,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_SIDEBAND_PD_PARITY_ERR          = 12035,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR     = 12036,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_DBE_ERR       = 12037,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_LIMIT_ERR          = 12038,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_DBE_ERR            = 12039,
    NVSWITCH_ERR_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_LIMIT_ERR             = 12040,
    NVSWITCH_ERR_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_DBE_ERR               = 12041,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDSGT_ECC_LIMIT_ERR                = 12042,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDSGT_ECC_DBE_ERR                  = 12043,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDBUF_ECC_LIMIT_ERR                = 12044,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDBUF_ECC_DBE_ERR                  = 12045,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_LIMIT_ERR           = 12046,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_DBE_ERR             = 12047,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_PARITY_ERR        = 12048,
    NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_REDUCTION_FLITTYPE_MISMATCH_ERR = 12049,
    NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSP_CNT_ERR                         = 12050,
    NVSWITCH_ERR_HW_NPORT_EGRESS_RBRSP_CNT_ERR                         = 12051,
    NVSWITCH_ERR_HW_NPORT_EGRESS_LAST, /* NOTE: Must be last */

    /* NPORT: Fstate errors */
    NVSWITCH_ERR_HW_NPORT_FSTATE                                       = 13000,
    NVSWITCH_ERR_HW_NPORT_FSTATE_TAGPOOLBUFERR                         = 13001,
    NVSWITCH_ERR_HW_NPORT_FSTATE_CRUMBSTOREBUFERR                      = 13002,
    NVSWITCH_ERR_HW_NPORT_FSTATE_SINGLEBITECCLIMITERR_CRUMBSTORE       = 13003,
    NVSWITCH_ERR_HW_NPORT_FSTATE_UNCORRECTABLEECCERR_CRUMBSTORE        = 13004,
    NVSWITCH_ERR_HW_NPORT_FSTATE_SINGLEBITECCLIMITERR_TAGSTORE         = 13005,
    NVSWITCH_ERR_HW_NPORT_FSTATE_UNCORRECTABLEECCERR_TAGSTORE          = 13006,
    NVSWITCH_ERR_HW_NPORT_FSTATE_SINGLEBITECCLIMITERR_FLUSHREQSTORE    = 13007,
    NVSWITCH_ERR_HW_NPORT_FSTATE_UNCORRECTABLEECCERR_FLUSHREQSTORE     = 13008,
    NVSWITCH_ERR_HW_NPORT_FSTATE_LAST, /* NOTE: Must be last */

    /* NPORT: Tstate errors */
    NVSWITCH_ERR_HW_NPORT_TSTATE                                       = 14000,
    NVSWITCH_ERR_HW_NPORT_TSTATE_TAGPOOLBUFERR                         = 14001,
    NVSWITCH_ERR_HW_NPORT_TSTATE_CRUMBSTOREBUFERR                      = 14002,
    NVSWITCH_ERR_HW_NPORT_TSTATE_SINGLEBITECCLIMITERR_CRUMBSTORE       = 14003,
    NVSWITCH_ERR_HW_NPORT_TSTATE_UNCORRECTABLEECCERR_CRUMBSTORE        = 14004,
    NVSWITCH_ERR_HW_NPORT_TSTATE_SINGLEBITECCLIMITERR_TAGSTORE         = 14005,
    NVSWITCH_ERR_HW_NPORT_TSTATE_UNCORRECTABLEECCERR_TAGSTORE          = 14006,
    NVSWITCH_ERR_HW_NPORT_TSTATE_TAGPOOL_ECC_LIMIT_ERR                 = 14007,
    NVSWITCH_ERR_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR                   = 14008,
    NVSWITCH_ERR_HW_NPORT_TSTATE_CRUMBSTORE_ECC_LIMIT_ERR              = 14009,
    NVSWITCH_ERR_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR                = 14010,
    NVSWITCH_ERR_HW_NPORT_TSTATE_COL_CRUMBSTOREBUFERR                  = 14011,
    NVSWITCH_ERR_HW_NPORT_TSTATE_COL_CRUMBSTORE_ECC_LIMIT_ERR          = 14012,
    NVSWITCH_ERR_HW_NPORT_TSTATE_COL_CRUMBSTORE_ECC_DBE_ERR            = 14013,
    NVSWITCH_ERR_HW_NPORT_TSTATE_TD_TID_RAMBUFERR                      = 14014,
    NVSWITCH_ERR_HW_NPORT_TSTATE_TD_TID_RAM_ECC_LIMIT_ERR              = 14015,
    NVSWITCH_ERR_HW_NPORT_TSTATE_TD_TID_RAM_ECC_DBE_ERR                = 14016,
    NVSWITCH_ERR_HW_NPORT_TSTATE_ATO_ERR                               = 14017,
    NVSWITCH_ERR_HW_NPORT_TSTATE_CAMRSP_ERR                            = 14018,
    NVSWITCH_ERR_HW_NPORT_TSTATE_LAST, /* NOTE: Must be last */

    /* NPORT: Route errors */
    NVSWITCH_ERR_HW_NPORT_ROUTE                                        = 15000,
    NVSWITCH_ERR_HW_NPORT_ROUTE_ROUTEBUFERR                            = 15001,
    NVSWITCH_ERR_HW_NPORT_ROUTE_NOPORTDEFINEDERR                       = 15002,
    NVSWITCH_ERR_HW_NPORT_ROUTE_INVALIDROUTEPOLICYERR                  = 15003,
    NVSWITCH_ERR_HW_NPORT_ROUTE_ECCLIMITERR                            = 15004,
    NVSWITCH_ERR_HW_NPORT_ROUTE_UNCORRECTABLEECCERR                    = 15005,
    NVSWITCH_ERR_HW_NPORT_ROUTE_TRANSDONERESVERR                       = 15006,
    NVSWITCH_ERR_HW_NPORT_ROUTE_PACKET_HEADER                          = 15007,
    NVSWITCH_ERR_HW_NPORT_ROUTE_GLT_ECC_LIMIT_ERR                      = 15008,
    NVSWITCH_ERR_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR                        = 15009,
    NVSWITCH_ERR_HW_NPORT_ROUTE_PDCTRLPARERR                           = 15010,
    NVSWITCH_ERR_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR                      = 15011,
    NVSWITCH_ERR_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR                        = 15012,
    NVSWITCH_ERR_HW_NPORT_ROUTE_CDTPARERR                              = 15013,
    NVSWITCH_ERR_HW_NPORT_ROUTE_MCRID_ECC_LIMIT_ERR                    = 15014,
    NVSWITCH_ERR_HW_NPORT_ROUTE_MCRID_ECC_DBE_ERR                      = 15015,
    NVSWITCH_ERR_HW_NPORT_ROUTE_EXTMCRID_ECC_LIMIT_ERR                 = 15016,
    NVSWITCH_ERR_HW_NPORT_ROUTE_EXTMCRID_ECC_DBE_ERR                   = 15017,
    NVSWITCH_ERR_HW_NPORT_ROUTE_RAM_ECC_LIMIT_ERR                      = 15018,
    NVSWITCH_ERR_HW_NPORT_ROUTE_RAM_ECC_DBE_ERR                        = 15019,
    NVSWITCH_ERR_HW_NPORT_ROUTE_INVALID_MCRID_ERR                      = 15020,
    NVSWITCH_ERR_HW_NPORT_ROUTE_LAST, /* NOTE: Must be last */

    /* NPORT: Nport errors */
    NVSWITCH_ERR_HW_NPORT                                              = 16000,
    NVSWITCH_ERR_HW_NPORT_DATAPOISONED                                 = 16001,
    NVSWITCH_ERR_HW_NPORT_UCINTERNAL                                   = 16002,
    NVSWITCH_ERR_HW_NPORT_CINTERNAL                                    = 16003,
    NVSWITCH_ERR_HW_NPORT_LAST, /* NOTE: Must be last */

    /* NVLCTRL: NVCTRL errors */
    NVSWITCH_ERR_HW_NVLCTRL                                            = 17000,
    NVSWITCH_ERR_HW_NVLCTRL_INGRESSECCSOFTLIMITERR                     = 17001,
    NVSWITCH_ERR_HW_NVLCTRL_INGRESSECCHDRDOUBLEBITERR                  = 17002,
    NVSWITCH_ERR_HW_NVLCTRL_INGRESSECCDATADOUBLEBITERR                 = 17003,
    NVSWITCH_ERR_HW_NVLCTRL_INGRESSBUFFERERR                           = 17004,
    NVSWITCH_ERR_HW_NVLCTRL_EGRESSECCSOFTLIMITERR                      = 17005,
    NVSWITCH_ERR_HW_NVLCTRL_EGRESSECCHDRDOUBLEBITERR                   = 17006,
    NVSWITCH_ERR_HW_NVLCTRL_EGRESSECCDATADOUBLEBITERR                  = 17007,
    NVSWITCH_ERR_HW_NVLCTRL_EGRESSBUFFERERR                            = 17008,
    NVSWITCH_ERR_HW_NVLCTRL_LAST, /* NOTE: Must be last */

    /* Nport: Nvlipt errors */
    NVSWITCH_ERR_HW_NVLIPT                                             = 18000,
    NVSWITCH_ERR_HW_NVLIPT_DLPROTOCOL                                  = 18001,
    NVSWITCH_ERR_HW_NVLIPT_DATAPOISONED                                = 18002,
    NVSWITCH_ERR_HW_NVLIPT_FLOWCONTROL                                 = 18003,
    NVSWITCH_ERR_HW_NVLIPT_RESPONSETIMEOUT                             = 18004,
    NVSWITCH_ERR_HW_NVLIPT_TARGETERROR                                 = 18005,
    NVSWITCH_ERR_HW_NVLIPT_UNEXPECTEDRESPONSE                          = 18006,
    NVSWITCH_ERR_HW_NVLIPT_RECEIVEROVERFLOW                            = 18007,
    NVSWITCH_ERR_HW_NVLIPT_MALFORMEDPACKET                             = 18008,
    NVSWITCH_ERR_HW_NVLIPT_STOMPEDPACKETRECEIVED                       = 18009,
    NVSWITCH_ERR_HW_NVLIPT_UNSUPPORTEDREQUEST                          = 18010,
    NVSWITCH_ERR_HW_NVLIPT_UCINTERNAL                                  = 18011,
    NVSWITCH_ERR_HW_NVLIPT_PHYRECEIVER                                 = 18012,
    NVSWITCH_ERR_HW_NVLIPT_BADAN0PKT                                   = 18013,
    NVSWITCH_ERR_HW_NVLIPT_REPLAYTIMEOUT                               = 18014,
    NVSWITCH_ERR_HW_NVLIPT_ADVISORYERROR                               = 18015,
    NVSWITCH_ERR_HW_NVLIPT_CINTERNAL                                   = 18016,
    NVSWITCH_ERR_HW_NVLIPT_HEADEROVERFLOW                              = 18017,
    NVSWITCH_ERR_HW_NVLIPT_RSTSEQ_PHYARB_TIMEOUT                       = 18018,
    NVSWITCH_ERR_HW_NVLIPT_RSTSEQ_PLL_TIMEOUT                          = 18019,
    NVSWITCH_ERR_HW_NVLIPT_CLKCTL_ILLEGAL_REQUEST                      = 18020,
    NVSWITCH_ERR_HW_NVLIPT_LAST, /* NOTE: Must be last */

    /* Nport: Nvltlc TX/RX errors */
    NVSWITCH_ERR_HW_NVLTLC                                             = 19000,
    NVSWITCH_ERR_HW_NVLTLC_TXHDRCREDITOVFERR                           = 19001,
    NVSWITCH_ERR_HW_NVLTLC_TXDATACREDITOVFERR                          = 19002,
    NVSWITCH_ERR_HW_NVLTLC_TXDLCREDITOVFERR                            = 19003,
    NVSWITCH_ERR_HW_NVLTLC_TXDLCREDITPARITYERR                         = 19004,
    NVSWITCH_ERR_HW_NVLTLC_TXRAMHDRPARITYERR                           = 19005,
    NVSWITCH_ERR_HW_NVLTLC_TXRAMDATAPARITYERR                          = 19006,
    NVSWITCH_ERR_HW_NVLTLC_TXUNSUPVCOVFERR                             = 19007,
    NVSWITCH_ERR_HW_NVLTLC_TXSTOMPDET                                  = 19008,
    NVSWITCH_ERR_HW_NVLTLC_TXPOISONDET                                 = 19009,
    NVSWITCH_ERR_HW_NVLTLC_TARGETERR                                   = 19010,
    NVSWITCH_ERR_HW_NVLTLC_TX_PACKET_HEADER                            = 19011,
    NVSWITCH_ERR_HW_NVLTLC_UNSUPPORTEDREQUESTERR                       = 19012,
    NVSWITCH_ERR_HW_NVLTLC_RXDLHDRPARITYERR                            = 19013,
    NVSWITCH_ERR_HW_NVLTLC_RXDLDATAPARITYERR                           = 19014,
    NVSWITCH_ERR_HW_NVLTLC_RXDLCTRLPARITYERR                           = 19015,
    NVSWITCH_ERR_HW_NVLTLC_RXRAMDATAPARITYERR                          = 19016,
    NVSWITCH_ERR_HW_NVLTLC_RXRAMHDRPARITYERR                           = 19017,
    NVSWITCH_ERR_HW_NVLTLC_RXINVALIDAEERR                              = 19018,
    NVSWITCH_ERR_HW_NVLTLC_RXINVALIDBEERR                              = 19019,
    NVSWITCH_ERR_HW_NVLTLC_RXINVALIDADDRALIGNERR                       = 19020,
    NVSWITCH_ERR_HW_NVLTLC_RXPKTLENERR                                 = 19021,
    NVSWITCH_ERR_HW_NVLTLC_RSVCMDENCERR                                = 19022,
    NVSWITCH_ERR_HW_NVLTLC_RSVDATLENENCERR                             = 19023,
    NVSWITCH_ERR_HW_NVLTLC_RSVADDRTYPEERR                              = 19024,
    NVSWITCH_ERR_HW_NVLTLC_RSVRSPSTATUSERR                             = 19025,
    NVSWITCH_ERR_HW_NVLTLC_RSVPKTSTATUSERR                             = 19026,
    NVSWITCH_ERR_HW_NVLTLC_RSVCACHEATTRPROBEREQERR                     = 19027,
    NVSWITCH_ERR_HW_NVLTLC_RSVCACHEATTRPROBERSPERR                     = 19028,
    NVSWITCH_ERR_HW_NVLTLC_DATLENGTATOMICREQMAXERR                     = 19029,
    NVSWITCH_ERR_HW_NVLTLC_DATLENGTRMWREQMAXERR                        = 19030,
    NVSWITCH_ERR_HW_NVLTLC_DATLENLTATRRSPMINERR                        = 19031,
    NVSWITCH_ERR_HW_NVLTLC_INVALIDCACHEATTRPOERR                       = 19032,
    NVSWITCH_ERR_HW_NVLTLC_INVALIDCRERR                                = 19033,
    NVSWITCH_ERR_HW_NVLTLC_RXRESPSTATUSTARGETERR                       = 19034,
    NVSWITCH_ERR_HW_NVLTLC_RXRESPSTATUSUNSUPPORTEDREQUESTERR           = 19035,
    NVSWITCH_ERR_HW_NVLTLC_RXHDROVFERR                                 = 19036,
    NVSWITCH_ERR_HW_NVLTLC_RXDATAOVFERR                                = 19037,
    NVSWITCH_ERR_HW_NVLTLC_STOMPDETERR                                 = 19038,
    NVSWITCH_ERR_HW_NVLTLC_RXPOISONERR                                 = 19039,
    NVSWITCH_ERR_HW_NVLTLC_CORRECTABLEINTERNALERR                      = 19040,
    NVSWITCH_ERR_HW_NVLTLC_RXUNSUPVCOVFERR                             = 19041,
    NVSWITCH_ERR_HW_NVLTLC_RXUNSUPNVLINKCREDITRELERR                   = 19042,
    NVSWITCH_ERR_HW_NVLTLC_RXUNSUPNCISOCCREDITRELERR                   = 19043,
    NVSWITCH_ERR_HW_NVLTLC_RX_PACKET_HEADER                            = 19044,
    NVSWITCH_ERR_HW_NVLTLC_RX_ERR_HEADER                               = 19045,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_NCISOC_PARITY_ERR                    = 19046,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_NCISOC_HDR_ECC_DBE_ERR               = 19047,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_NCISOC_DAT_ECC_DBE_ERR               = 19048,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_NCISOC_ECC_LIMIT_ERR                 = 19049,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_TXRSPSTATUS_HW_ERR                   = 19050,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_TXRSPSTATUS_UR_ERR                   = 19051,
    NVSWITCH_ERR_HW_NVLTLC_TX_SYS_TXRSPSTATUS_PRIV_ERR                 = 19052,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_NCISOC_PARITY_ERR                    = 19053,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_HDR_RAM_ECC_DBE_ERR                  = 19054,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_HDR_RAM_ECC_LIMIT_ERR                = 19055,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_DAT0_RAM_ECC_DBE_ERR                 = 19056,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_DAT0_RAM_ECC_LIMIT_ERR               = 19057,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_DAT1_RAM_ECC_DBE_ERR                 = 19058,
    NVSWITCH_ERR_HW_NVLTLC_RX_SYS_DAT1_RAM_ECC_LIMIT_ERR               = 19059,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_CREQ_RAM_HDR_ECC_DBE_ERR             = 19060,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_CREQ_RAM_DAT_ECC_DBE_ERR             = 19061,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_CREQ_RAM_ECC_LIMIT_ERR               = 19062,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_RSP_RAM_HDR_ECC_DBE_ERR              = 19063,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_RSP_RAM_DAT_ECC_DBE_ERR              = 19064,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_RSP_RAM_ECC_LIMIT_ERR                = 19065,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_COM_RAM_HDR_ECC_DBE_ERR              = 19066,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_COM_RAM_DAT_ECC_DBE_ERR              = 19067,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_COM_RAM_ECC_LIMIT_ERR                = 19068,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_RSP1_RAM_HDR_ECC_DBE_ERR             = 19069,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_RSP1_RAM_DAT_ECC_DBE_ERR             = 19070,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_RSP1_RAM_ECC_LIMIT_ERR               = 19071,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC0                      = 19072,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC1                      = 19073,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC2                      = 19074,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC3                      = 19075,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC4                      = 19076,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC5                      = 19077,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC6                      = 19078,
    NVSWITCH_ERR_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC7                      = 19079,
    NVSWITCH_ERR_HW_NVLTLC_RX_LNK_RXRSPSTATUS_HW_ERR                   = 19080,
    NVSWITCH_ERR_HW_NVLTLC_RX_LNK_RXRSPSTATUS_UR_ERR                   = 19081,
    NVSWITCH_ERR_HW_NVLTLC_RX_LNK_RXRSPSTATUS_PRIV_ERR                 = 19082,
    NVSWITCH_ERR_HW_NVLTLC_RX_LNK_INVALID_COLLAPSED_RESPONSE_ERR       = 19083,
    NVSWITCH_ERR_HW_NVLTLC_RX_LNK_AN1_HEARTBEAT_TIMEOUT_ERR            = 19084,
    NVSWITCH_ERR_HW_NVLTLC_LAST, /* NOTE: Must be last */

    /* DLPL: errors ( SL1 errors too) */
    NVSWITCH_ERR_HW_DLPL                                               = 20000,
    NVSWITCH_ERR_HW_DLPL_TX_REPLAY                                     = 20001,
    NVSWITCH_ERR_HW_DLPL_TX_RECOVERY_SHORT                             = 20002,
    NVSWITCH_ERR_HW_DLPL_TX_RECOVERY_LONG                              = 20003,
    NVSWITCH_ERR_HW_DLPL_TX_FAULT_RAM                                  = 20004,
    NVSWITCH_ERR_HW_DLPL_TX_FAULT_INTERFACE                            = 20005,
    NVSWITCH_ERR_HW_DLPL_TX_FAULT_SUBLINK_CHANGE                       = 20006,
    NVSWITCH_ERR_HW_DLPL_RX_FAULT_SUBLINK_CHANGE                       = 20007,
    NVSWITCH_ERR_HW_DLPL_RX_FAULT_DL_PROTOCOL                          = 20008,
    NVSWITCH_ERR_HW_DLPL_RX_SHORT_ERROR_RATE                           = 20009,
    NVSWITCH_ERR_HW_DLPL_RX_LONG_ERROR_RATE                            = 20010,
    NVSWITCH_ERR_HW_DLPL_RX_ILA_TRIGGER                                = 20011,
    NVSWITCH_ERR_HW_DLPL_RX_CRC_COUNTER                                = 20012,
    NVSWITCH_ERR_HW_DLPL_LTSSM_FAULT                                   = 20013,
    NVSWITCH_ERR_HW_DLPL_LTSSM_PROTOCOL                                = 20014,
    NVSWITCH_ERR_HW_DLPL_MINION_REQUEST                                = 20015,
    NVSWITCH_ERR_HW_DLPL_FIFO_DRAIN_ERR                                = 20016,
    NVSWITCH_ERR_HW_DLPL_CONST_DET_ERR                                 = 20017,
    NVSWITCH_ERR_HW_DLPL_OFF2SAFE_LINK_DET_ERR                         = 20018,
    NVSWITCH_ERR_HW_DLPL_SAFE2NO_LINK_DET_ERR                          = 20019,
    NVSWITCH_ERR_HW_DLPL_SCRAM_LOCK_ERR                                = 20020,
    NVSWITCH_ERR_HW_DLPL_SYM_LOCK_ERR                                  = 20021,
    NVSWITCH_ERR_HW_DLPL_SYM_ALIGN_END_ERR                             = 20022,
    NVSWITCH_ERR_HW_DLPL_FIFO_SKEW_ERR                                 = 20023,
    NVSWITCH_ERR_HW_DLPL_TRAIN2SAFE_LINK_DET_ERR                       = 20024,
    NVSWITCH_ERR_HW_DLPL_HS2SAFE_LINK_DET_ERR                          = 20025,
    NVSWITCH_ERR_HW_DLPL_FENCE_ERR                                     = 20026,
    NVSWITCH_ERR_HW_DLPL_SAFE_NO_LD_ERR                                = 20027,
    NVSWITCH_ERR_HW_DLPL_E2SAFE_LD_ERR                                 = 20028,
    NVSWITCH_ERR_HW_DLPL_RC_RXPWR_ERR                                  = 20029,
    NVSWITCH_ERR_HW_DLPL_RC_TXPWR_ERR                                  = 20030,
    NVSWITCH_ERR_HW_DLPL_RC_DEADLINE_ERR                               = 20031,
    NVSWITCH_ERR_HW_DLPL_TX_HS2LP_ERR                                  = 20032,
    NVSWITCH_ERR_HW_DLPL_RX_HS2LP_ERR                                  = 20033,
    NVSWITCH_ERR_HW_DLPL_LTSSM_FAULT_UP                                = 20034,
    NVSWITCH_ERR_HW_DLPL_LTSSM_FAULT_DOWN                              = 20035,
    NVSWITCH_ERR_HW_DLPL_PHY_A                                         = 20036,
    NVSWITCH_ERR_HW_DLPL_TX_PL_ERROR                                   = 20037,
    NVSWITCH_ERR_HW_DLPL_RX_PL_ERROR                                   = 20038,
    NVSWITCH_ERR_HW_DLPL_LAST, /* NOTE: Must be last */

    /* AFS: errors */
    NVSWITCH_ERR_HW_AFS                                                = 21000,
    NVSWITCH_ERR_HW_AFS_UC_INGRESS_CREDIT_OVERFLOW                     = 21001,
    NVSWITCH_ERR_HW_AFS_UC_INGRESS_CREDIT_UNDERFLOW                    = 21002,
    NVSWITCH_ERR_HW_AFS_UC_EGRESS_CREDIT_OVERFLOW                      = 21003,
    NVSWITCH_ERR_HW_AFS_UC_EGRESS_CREDIT_UNDERFLOW                     = 21004,
    NVSWITCH_ERR_HW_AFS_UC_INGRESS_NON_BURSTY_PKT_DETECTED             = 21005,
    NVSWITCH_ERR_HW_AFS_UC_INGRESS_NON_STICKY_PKT_DETECTED             = 21006,
    NVSWITCH_ERR_HW_AFS_UC_INGRESS_BURST_GT_17_DATA_VC_DETECTED        = 21007,
    NVSWITCH_ERR_HW_AFS_UC_INGRESS_BURST_GT_1_NONDATA_VC_DETECTED      = 21008,
    NVSWITCH_ERR_HW_AFS_UC_INVALID_DST                                 = 21009,
    NVSWITCH_ERR_HW_AFS_UC_PKT_MISROUTE                                = 21010,
    NVSWITCH_ERR_HW_AFS_LAST, /* NOTE: Must be last */

    /* MINION: errors */
    NVSWITCH_ERR_HW_MINION                                             = 22000,
    NVSWITCH_ERR_HW_MINION_UCODE_IMEM                                  = 22001,
    NVSWITCH_ERR_HW_MINION_UCODE_DMEM                                  = 22002,
    NVSWITCH_ERR_HW_MINION_HALT                                        = 22003,
    NVSWITCH_ERR_HW_MINION_BOOT_ERROR                                  = 22004,
    NVSWITCH_ERR_HW_MINION_TIMEOUT                                     = 22005,
    NVSWITCH_ERR_HW_MINION_DLCMD_FAULT                                 = 22006,
    NVSWITCH_ERR_HW_MINION_DLCMD_TIMEOUT                               = 22007,
    NVSWITCH_ERR_HW_MINION_DLCMD_FAIL                                  = 22008,
    NVSWITCH_ERR_HW_MINION_FATAL_INTR                                  = 22009,
    NVSWITCH_ERR_HW_MINION_WATCHDOG                                    = 22010,
    NVSWITCH_ERR_HW_MINION_EXTERR                                      = 22011,
    NVSWITCH_ERR_HW_MINION_FATAL_LINK_INTR                             = 22012,
    NVSWITCH_ERR_HW_MINION_NONFATAL                                    = 22013,
    NVSWITCH_ERR_HW_MINION_LAST, /* NOTE: Must be last */

    /* NXBAR errors */
    NVSWITCH_ERR_HW_NXBAR                                              = 23000,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_BUFFER_OVERFLOW                 = 23001,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_BUFFER_UNDERFLOW                = 23002,
    NVSWITCH_ERR_HW_NXBAR_TILE_EGRESS_CREDIT_OVERFLOW                  = 23003,
    NVSWITCH_ERR_HW_NXBAR_TILE_EGRESS_CREDIT_UNDERFLOW                 = 23004,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_NON_BURSTY_PKT                  = 23005,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_NON_STICKY_PKT                  = 23006,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_BURST_GT_9_DATA_VC              = 23007,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_PKT_INVALID_DST                 = 23008,
    NVSWITCH_ERR_HW_NXBAR_TILE_INGRESS_PKT_PARITY_ERROR                = 23009,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_INGRESS_BUFFER_OVERFLOW              = 23010,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_INGRESS_BUFFER_UNDERFLOW             = 23011,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_EGRESS_CREDIT_OVERFLOW               = 23012,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_EGRESS_CREDIT_UNDERFLOW              = 23013,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_INGRESS_NON_BURSTY_PKT               = 23014,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_INGRESS_NON_STICKY_PKT               = 23015,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_INGRESS_BURST_GT_9_DATA_VC           = 23016,
    NVSWITCH_ERR_HW_NXBAR_TILEOUT_EGRESS_CDT_PARITY_ERROR              = 23017,
    NVSWITCH_ERR_HW_NXBAR_LAST, /* NOTE: Must be last */

    /* NPORT: SOURCETRACK errors */
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK                                         = 24000,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR     = 24001,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_TD_CRUMBSTORE_ECC_LIMIT_ERR  = 24002,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR     = 24003,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR       = 24004,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_TD_CRUMBSTORE_ECC_DBE_ERR    = 24005,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR       = 24006,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_SOURCETRACK_TIME_OUT_ERR                = 24007,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_DUP_CREQ_TCEN0_TAG_ERR                  = 24008,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_INVALID_TCEN0_RSP_ERR                   = 24009,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_INVALID_TCEN1_RSP_ERR                   = 24010,
    NVSWITCH_ERR_HW_NPORT_SOURCETRACK_LAST, /* NOTE: Must be last */

    /* NVLIPT_LNK errors */
    NVSWITCH_ERR_HW_NVLIPT_LNK                                         = 25000,
    NVSWITCH_ERR_HW_NVLIPT_LNK_ILLEGALLINKSTATEREQUEST                 = 25001,
    NVSWITCH_ERR_HW_NVLIPT_LNK_FAILEDMINIONREQUEST                     = 25002,
    NVSWITCH_ERR_HW_NVLIPT_LNK_RESERVEDREQUESTVALUE                    = 25003,
    NVSWITCH_ERR_HW_NVLIPT_LNK_LINKSTATEWRITEWHILEBUSY                 = 25004,
    NVSWITCH_ERR_HW_NVLIPT_LNK_LINK_STATE_REQUEST_TIMEOUT              = 25005,
    NVSWITCH_ERR_HW_NVLIPT_LNK_WRITE_TO_LOCKED_SYSTEM_REG_ERR          = 25006,
    NVSWITCH_ERR_HW_NVLIPT_LNK_SLEEPWHILEACTIVELINK                    = 25007,
    NVSWITCH_ERR_HW_NVLIPT_LNK_RSTSEQ_PHYCTL_TIMEOUT                   = 25008,
    NVSWITCH_ERR_HW_NVLIPT_LNK_RSTSEQ_CLKCTL_TIMEOUT                   = 25009,
    NVSWITCH_ERR_HW_NVLIPT_LNK_ALI_TRAINING_FAIL                       = 25010,
    NVSWITCH_ERR_HW_NVLIPT_LNK_LAST, /* Note: Must be last */

    /* SOE errors */
    NVSWITCH_ERR_HW_SOE                                                = 26000,
    NVSWITCH_ERR_HW_SOE_RESET                                          = 26001,
    NVSWITCH_ERR_HW_SOE_BOOTSTRAP                                      = 26002,
    NVSWITCH_ERR_HW_SOE_COMMAND_QUEUE                                  = 26003,
    NVSWITCH_ERR_HW_SOE_TIMEOUT                                        = 26004,
    NVSWITCH_ERR_HW_SOE_SHUTDOWN                                       = 26005,
    NVSWITCH_ERR_HW_SOE_HALT                                           = 26006,
    NVSWITCH_ERR_HW_SOE_EXTERR                                         = 26007,
    NVSWITCH_ERR_HW_SOE_WATCHDOG                                       = 26008,
    NVSWITCH_ERR_HW_SOE_LAST, /* Note: Must be last */

    /* CCI errors */
    NVSWITCH_ERR_HW_CCI                                                = 27000,
    NVSWITCH_ERR_HW_CCI_RESET                                          = 27001,
    NVSWITCH_ERR_HW_CCI_INIT                                           = 27002,
    NVSWITCH_ERR_HW_CCI_TIMEOUT                                        = 27003,
    NVSWITCH_ERR_HW_CCI_SHUTDOWN                                       = 27004,
    NVSWITCH_ERR_HW_CCI_MODULE                                         = 27005,
    NVSWITCH_ERR_HW_CCI_MODULE_BOOT                                    = 27006,
    NVSWITCH_ERR_HW_CCI_MODULE_RECOVERY                                = 27007,
    NVSWITCH_ERR_HW_CCI_LAST, /* Note: Must be last */

    /* OSFP Therm Errors */
    NVSWITCH_ERR_HW_OSFP_THERM                                         = 28000,
    NVSWITCH_ERR_HW_OSFP_THERM_WARN_ACTIVATED                          = 28001,
    NVSWITCH_ERR_HW_OSFP_THERM_WARN_DEACTIVATED                        = 28002,
    NVSWITCH_ERR_HW_OSFP_THERM_OVERT_ACTIVATED                         = 28003,
    NVSWITCH_ERR_HW_OSFP_THERM_OVERT_DEACTIVATED                       = 28004,
    NVSWITCH_ERR_HW_OSFP_THERM_HEARTBEAT_SHUTDOWN                      = 28005,
    NVSWITCH_ERR_HW_OSFP_THERM_LAST, /* Note: Must be last */

    /* NPORT: Multicast Tstate errors */
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE                              = 29000,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_LIMIT_ERR        = 29001,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_DBE_ERR          = 29002,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_LIMIT_ERR     = 29003,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_DBE_ERR       = 29004,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_BUF_OVERWRITE_ERR = 29005,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_MCTO_ERR          = 29006,
    NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_LAST, /* Note: Must be last */

    /* NPORT: Reduction Tstate errors */
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE                              = 30000,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_LIMIT_ERR        = 30001,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_DBE_ERR          = 30002,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_LIMIT_ERR     = 30003,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_DBE_ERR       = 30004,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_BUF_OVERWRITE_ERR = 30005,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_RTO_ERR           = 30006,
    NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_LAST, /* Note: Must be last */

    /* Please update nvswitch_translate_hw_errors with a newly added error class. */
    NVSWITCH_ERR_LAST
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_ERR_TYPE;

typedef enum nvswitch_pri_error_instance
{
    NVSWITCH_PBUS_PRI_SQUASH = 0, 
    NVSWITCH_PBUS_PRI_FECSERR,
    NVSWITCH_PBUS_PRI_TIMEOUT,
    NVSWITCH_PPRIV_WRITE_SYS,
    NVSWITCH_PPRIV_WRITE_PRT
} NVSWITCH_PRI_ERROR_INSTANCE;

#define NVSWITCH_ERROR_MAX_DESCRPTION_LEN 70
#define NVSWITCH_RAW_ERROR_DATA_SIZE      16

typedef struct nvswitch_error
{
    NvU32  error_value;                 /* NVSWITCH_ERR_* */
    NvU32  error_src;                   /* NVSWITCH_ERROR_SRC_* */
    NvU32  instance;                    /* Used for link# or subengine instance */
    NvU32  subinstance;                 /* Used for lane# or similar */
    NV_DECLARE_ALIGNED(NvU64 time, 8);  /* Platform time (nsec) */
    NvBool error_resolved;              /* If an error is correctable, set to true. */
    NvU32  error_data[NVSWITCH_RAW_ERROR_DATA_SIZE]; /* Data from NVSWITCH_REPORT_DATA */
    NvU32  error_data_size;             /* Size of error_data */
    NvU8   error_description[NVSWITCH_ERROR_MAX_DESCRPTION_LEN]; /* Short description of error */
} NVSWITCH_ERROR;

#define NVSWITCH_ERROR_COUNT_SIZE 64

typedef struct nvswitch_get_errors
{
    NvU32          errorType;
    NvU64          errorIndex;
    NvU64          nextErrorIndex;
    NvU32          errorCount;
    NVSWITCH_ERROR error[NVSWITCH_ERROR_COUNT_SIZE];
} NVSWITCH_GET_ERRORS_PARAMS;

/*
* CTRL_NVSWITCH_GET_PORT_EVENTS
*
* Control for querying port up and port down event information
*
* Parameters:
*
*   portEventIndex [IN/OUT]
*      On input: The index of the first port event at which to start reading out of the driver.
*
*      On output: The index of the first port event that wasn't reported through the 'port event' array
*                 in this call to CTRL_NVSWITCH_GET_PORT_EVENTS.
*
*   nextPortEventIndex[OUT]
*      The index that will be assigned to the next port event to occur.
*      Users of the GET_PORT_EVENTS control call may set 'portEventIndex' to this field on initialization
*      to bypass port events that have already occurred without making multiple control calls.
*
*   portEventCount [OUT]
*      Number of port events returned by the call. Currently, portEventCount is limited
*      by NVSWITCH_PORT_EVENT_COUNT_SIZE. In order to query all the port events, a
*      client needs to keep calling the control till portEventCount is zero.
*
*   bOverflow [OUT]
*       True when the port event log is overflowed and no longer contains all the port
*       events that have occurred, false otherwise.
*
*   portEvent [OUT]
*      The port event entires.
*/
#define NVSWITCH_PORT_EVENT_COUNT_SIZE 64
#define NVSWITCH_PORT_EVENT_TYPE_UP 0
#define NVSWITCH_PORT_EVENT_TYPE_DOWN 1
#define NVSWITCH_PORT_EVENT_TYPE_INVALID 2

typedef struct 
{
    NvU8  port_event_type;              /* NVSWITCH_PORT_EVENT_TYPE* */
    NvU32  link_id;                     /* Link # that event occurred on */
    NV_DECLARE_ALIGNED(NvU64 time, 8);  /* Platform time (nsec) when event occurred */
} NVSWITCH_PORT_EVENT;

typedef struct nvswitch_get_port_events
{
    NvU64          portEventIndex;
    NvU64          nextPortEventIndex;
    NvU32          portEventCount;
    NvBool         bOverflow;
    NVSWITCH_PORT_EVENT portEvent[NVSWITCH_PORT_EVENT_COUNT_SIZE];
} NVSWITCH_GET_PORT_EVENTS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_INTERNAL_LATENCY
 *
 * Control for querying latency bins.
 *
 * Parameters:
 *   vc_selector [IN]
 *      A valid VC number returned by NVSWITCH_GET_INFO.
 *
 *   elapsed_time_msec [OUT]
 *      Elapsed time since the latency bins were queried.
 *   egressHistogram [OUT]
 *      Latency bin data/histogram format. The data will be available for the
 *      enabled/supported ports returned by NVSWITCH_GET_INFO.
 */

#define NVSWITCH_MAX_PORTS 64

/* TODO: describe the format */
typedef struct nvswitch_internal_latency_bins
{
    NV_DECLARE_ALIGNED(NvU64 low,    8);
    NV_DECLARE_ALIGNED(NvU64 medium, 8);
    NV_DECLARE_ALIGNED(NvU64 high,   8);
    NV_DECLARE_ALIGNED(NvU64 panic,  8);
    NV_DECLARE_ALIGNED(NvU64 count,  8);
} NVSWITCH_INTERNAL_LATENCY_BINS;

typedef struct nvswitch_get_internal_latency
{
    NvU32                          vc_selector;
    NV_DECLARE_ALIGNED(NvU64 elapsed_time_msec, 8);
    NVSWITCH_INTERNAL_LATENCY_BINS egressHistogram[NVSWITCH_MAX_PORTS];
} NVSWITCH_GET_INTERNAL_LATENCY;

/*
 * CTRL_NVSWITCH_SET_LATENCY_BINS
 *
 * Control for setting latency bins.
 *
 * Parameters:
 *   NVSWITCH_LATENCY_BIN [IN]
 *     Latency bin thresholds. The thresholds would be only applied to the
 *     enabled ports and the supported VCs by those ports.
 *     NVSWITCH_GET_INFO can be used to query enabled ports and supported VCs.
 */

#define NVSWITCH_MAX_VCS 8

/* TODO: describe the format */
typedef struct nvswitch_latency_bin
{
    NvU32   lowThreshold;       /* in nsec */
    NvU32   medThreshold;       /* in nsec */
    NvU32   hiThreshold;        /* in nsec */

} NVSWITCH_LATENCY_BIN;

typedef struct nvswitch_set_latency_bins
{
    NVSWITCH_LATENCY_BIN bin[NVSWITCH_MAX_VCS];

} NVSWITCH_SET_LATENCY_BINS;

/*
 * CTRL_NVSWITCH_SET_SWITCH_PORT_CONFIG
 *
 * Control for setting device port configurations.
 *
 * Parameters:
 *    portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO.
 *   type [IN]
 *      A connection type. See NVSWITCH_CONNECTION_TYPE.
 *   requesterLinkID [IN]
 *      An unique port ID in the fabric.
 *   requesterLan [IN]
 *      A Lan Id.
 *   count [IN]
 *      Endpoint Count
 *   acCoupled [IN]
 *      Set true, if the port is AC coupled.
 *   enableVC1 [IN]
 *      Set true, if VC1 should be enabled for the port.
 *   trunkSrcMask [IN]
 *      Mask of source trunk ports.
 */

typedef enum nvswitch_connection_type
{
    CONNECT_ACCESS_GPU = 0,
    CONNECT_ACCESS_CPU,
    CONNECT_TRUNK_SWITCH,
    CONNECT_ACCESS_SWITCH
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_CONNECTION_TYPE;

typedef enum nvswitch_connection_count
{
    CONNECT_COUNT_512 = 0,
    CONNECT_COUNT_1024,
    CONNECT_COUNT_2048
    /* See enum modification guidelines at the top of this file */
} NVSWITCH_CONNECTION_COUNT;

typedef struct nvswitch_set_switch_port_config
{
    NvU32  portNum;
    NvU32  type;
    NvU32  requesterLinkID;
    NvU32  requesterLanID;
    NvU32  count;
    NvBool acCoupled;
    NvBool enableVC1;
    NvU64  trunkSrcMask;

} NVSWITCH_SET_SWITCH_PORT_CONFIG;

/*
 * CTRL_NVSWITCH_SET_GANGED_LINK_TABLE
 *
 * Control for setting ganged link tables.
 * This interface is only supported on architectures that report
 * _GET_INFO_INDEX_ARCH == SV10.  All others will return an error.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link/port mask returned by the port masks returned by
 *      NVSWITCH_GET_INFO.
 *   entries [IN]
 *      The Ganged link entires. (TODO: Describe format)
 */

#define NVSWITCH_GANGED_LINK_TABLE_ENTRIES_MAX 256

typedef struct nvswitch_set_ganged_link_table
{
    NvU32 link_mask;
    NvU32 entries[NVSWITCH_GANGED_LINK_TABLE_ENTRIES_MAX];

} NVSWITCH_SET_GANGED_LINK_TABLE;

/*
 * CTRL_NVSWITCH_GET_NVLIPT_COUNTER
 *
 * Control for querying NVLIPT counters.
 *
 * Parameters:
 *    liptCounter [OUT]
 *      Port's TX/RX traffic data. The data will be available for the
 *      enabled/supported ports returned by NVSWITCH_GET_INFO.
 */

typedef struct nvswitch_nvlipt_counter
{
    NV_DECLARE_ALIGNED(NvU64 txCounter0, 8);
    NV_DECLARE_ALIGNED(NvU64 txCounter1, 8);
    NV_DECLARE_ALIGNED(NvU64 rxCounter0, 8);
    NV_DECLARE_ALIGNED(NvU64 rxCounter1, 8);

} NVSWITCH_NVLIPT_COUNTER;

typedef struct nvswitch_get_nvlipt_counters
{
    NVSWITCH_NVLIPT_COUNTER liptCounter[NVSWITCH_MAX_PORTS];

} NVSWITCH_GET_NVLIPT_COUNTERS;

/*
 * CTRL_NVSWITCH_SET_NVLIPT_COUNTER_CONFIG
 *
 * Control to set NVLIPT counter configuration.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link/port mask returned by the port masks returned by
 *      NVSWITCH_GET_INFO.
 *    tx0/tx1/rx0/rx1 [IN]
 *      TX/RX link configurations.
 */

/* TODO: describe format */
typedef struct nvlipt_counter_config
{
    NvU32 ctrl_0;
    NvU32 ctrl_1;
    NvU32 req_filter;
    NvU32 rsp_filter;
    NvU32 misc_filter;
    NV_DECLARE_ALIGNED(NvU64 addr_filter, 8);
    NV_DECLARE_ALIGNED(NvU64 addr_mask,   8);

} NVLIPT_COUNTER_CONFIG;

typedef struct nvswitch_set_nvlipt_counter_config
{
    NV_DECLARE_ALIGNED(NvU64 link_mask, 8);
    NVLIPT_COUNTER_CONFIG tx0;
    NVLIPT_COUNTER_CONFIG tx1;
    NVLIPT_COUNTER_CONFIG rx0;
    NVLIPT_COUNTER_CONFIG rx1;

} NVSWITCH_SET_NVLIPT_COUNTER_CONFIG;

/*
 * CTRL_NVSWITCH_GET_NVLIPT_COUNTER_CONFIG
 *
 * Control to query NVLIPT counter configuration.
 *
 * Parameters:
 *    link [IN]
 *      A valid link/port returned by the port masks returned by
 *      NVSWITCH_GET_INFO.
 *
 *    tx0/tx1/rx0/rx1 [OUT]
 *      TX/RX link configurations for the provide port.
 */

typedef struct nvswitch_get_nvlipt_counter_config
{
    NvU32                 link;
    NVLIPT_COUNTER_CONFIG tx0;
    NVLIPT_COUNTER_CONFIG tx1;
    NVLIPT_COUNTER_CONFIG rx0;
    NVLIPT_COUNTER_CONFIG rx1;

} NVSWITCH_GET_NVLIPT_COUNTER_CONFIG;

/*
 * CTRL_NVSWITCH_GET_INGRESS_REQLINKID
 *
 * Control to query the ingress requestor link id.
 *
 * Parameters:
 *    portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *
 *    requesterLinkID [OUT]
 *      Ingress requestor link id for the provided port.
 */

typedef struct nvswitch_get_ingress_reqlinkid_params
{
    NvU32       portNum;
    NvU32       requesterLinkID;

} NVSWITCH_GET_INGRESS_REQLINKID_PARAMS;

/*
 * CTRL_NVSWITCH_UNREGISTER_LINK
 *
 * Control to unregister the request link (port). This ensures that the black-
 * listed link will not be initialized or trained by the driver.
 *
 * Parameters:
 *    portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 */

typedef struct nvswitch_unregister_link_params
{
    NvU32       portNum;

} NVSWITCH_UNREGISTER_LINK_PARAMS;

/*
 * CTRL_RESET_AND_DRAIN_LINKS
 *
 * Control to reset and drain the links. Resets NVLinks and ensures to drain
 * backed up traffic.
 *
 * Parameters:
 *    linkMask [IN]
 *      A mask of link(s) to be reset.
 *      For SV10, the linkMask must contain at least a link-pair (even-odd links).
 *
 * Returns:
 *     NVL_SUCCESS if there were no errors
 *    -NVL_BAD_PARAMS if input parameters are wrong.
 *    -NVL_ERR_INVALID_STATE if other errors are present and a full-chip reset is required.
 *    -NVL_INITIALIZATION_TOTAL_FAILURE if NPORT initialization failed and a retry is required.
 */

typedef struct nvswitch_reset_and_drain_links_params
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);

} NVSWITCH_RESET_AND_DRAIN_LINKS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_NVLINK_STATUS
 *
 *   enabledLinkMask
 *     This field specifies the mask of available links on this subdevice.
 *   linkInfo
 *     This structure stores the per-link status of different NVLink
 *     parameters. The link is identified using an index.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

/*
 * NVSWITCH_NVLINK_DEVICE_INFO
 *
 * This structure stores information about the device to which this link is
 * associated
 *
 *   deviceIdFlags
 *      Bitmask that specifies which IDs are valid for the device
 *      Refer NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_* for possible values
 *      If NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI is set, PCI
 *      information is valid
 *      If NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_UUID is set, UUID is
 *      valid
 *   domain, bus, device, function, pciDeviceId
 *      PCI information for the device
 *   deviceType
 *      Type of the device
 *      See NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_* for possible values
 *   deviceUUID
 *      This field specifies the device UUID of the device. Useful for
 *      identifying the device (or version)
 */

typedef struct
{
    // ID Flags
    NvU32  deviceIdFlags;

    // PCI Information
    NvU32  domain;
    NvU16  bus;
    NvU16  device;
    NvU16  function;
    NvU32  pciDeviceId;

    // Device Type
    NV_DECLARE_ALIGNED(NvU64 deviceType, 8);

    // Device UUID
    NvU8   deviceUUID[16];
} NVSWITCH_NVLINK_DEVICE_INFO;

#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS        31:0
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_NONE   (0x00000000)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI    (0x00000001)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_UUID   (0x00000002)

#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_EBRIDGE    (0x00000000)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_NPU        (0x00000001)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU        (0x00000002)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH     (0x00000003)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_TEGRA      (0x00000004)
#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE       (0x000000FF)

#define NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_UUID_INVALID    (0xFFFFFFFF)

/*
 * NVSWITCH_NVLINK_CAPS_*
 *
 *   SUPPORTED
 *     Set if NVLink is present and supported on this subdevice, NV_FALSE
 *     otherwise. This field is used for *global* caps only and NOT for
 *     per-link caps
 *   P2P_SUPPORTED
 *     Set if P2P over NVLink is supported on this subdevice, NV_FALSE
 *     otherwise.
 *   SYSMEM_ACCESS
 *     Set if sysmem can be accessed over NVLink on this subdevice, NV_FALSE
 *     otherwise.
 *   PEER_ATOMICS
 *     Set if P2P atomics are supported over NVLink on this subdevice, NV_FALSE
 *     otherwise.
 *   SYSMEM_ATOMICS
 *     Set if sysmem atomic transcations are supported over NVLink on this
 *     subdevice, NV_FALSE otherwise.
 *   PEX_TUNNELING
 *     Set if PEX tunneling over NVLink is supported on this subdevice,
 *     NV_FALSE otherwise.
 *   SLI_BRIDGE
 *     GLOBAL: Set if SLI over NVLink is supported on this subdevice, NV_FALSE
 *     otherwise.
 *     LINK:   Set if SLI over NVLink is supported on a link, NV_FALSE
 *     otherwise.
 *   SLI_BRIDGE_SENSABLE
 *     GLOBAL: Set if the subdevice is capable of sensing SLI bridges, NV_FALSE
 *     otherwise.
 *     LINK:   Set if the link is capable of sensing an SLI bridge, NV_FALSE
 *     otherwise.
 *   POWER_STATE_L0
 *     Set if L0 is a supported power state on this subdevice/link, NV_FALSE
 *     otherwise.
 *   POWER_STATE_L1
 *     Set if L1 is a supported power state on this subdevice/link, NV_FALSE
 *     otherwise.
 *   POWER_STATE_L2
 *     Set if L2 is a supported power state on this subdevice/link, NV_FALSE
 *     otherwise.
 *   POWER_STATE_L3
 *     Set if L3 is a supported power state on this subdevice/link, NV_FALSE
 *     otherwise.
 *   VALID
 *     Set if this link is supported on this subdevice, NV_FALSE otherwise.
 *     This field is used for *per-link* caps only and NOT for global caps.
 *
 */

/* caps format is byte_index:bit_mask */
#define NVSWITCH_NVLINK_CAPS_SUPPORTED                          0:0x01
#define NVSWITCH_NVLINK_CAPS_P2P_SUPPORTED                      0:0x02
#define NVSWITCH_NVLINK_CAPS_SYSMEM_ACCESS                      0:0x04
#define NVSWITCH_NVLINK_CAPS_P2P_ATOMICS                        0:0x08
#define NVSWITCH_NVLINK_CAPS_SYSMEM_ATOMICS                     0:0x10
#define NVSWITCH_NVLINK_CAPS_PEX_TUNNELING                      0:0x20
#define NVSWITCH_NVLINK_CAPS_SLI_BRIDGE                         0:0x40
#define NVSWITCH_NVLINK_CAPS_SLI_BRIDGE_SENSABLE                0:0x80
#define NVSWITCH_NVLINK_CAPS_POWER_STATE_L0                     1:0x01
#define NVSWITCH_NVLINK_CAPS_POWER_STATE_L1                     1:0x02
#define NVSWITCH_NVLINK_CAPS_POWER_STATE_L2                     1:0x04
#define NVSWITCH_NVLINK_CAPS_POWER_STATE_L3                     1:0x08
#define NVSWITCH_NVLINK_CAPS_VALID                              1:0x10

/*
 * Size in bytes of nvlink caps table.  This value should be one greater
 * than the largest byte_index value above.
 */
#define NVSWITCH_NVLINK_CAPS_TBL_SIZE                           2

#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_INVALID      (0x00000000)
#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_1_0          (0x00000001)
#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_2_0          (0x00000002)
#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_2_2          (0x00000004)
#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_3_0          (0x00000005)
#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_3_1          (0x00000006)
#define NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_4_0          (0x00000007)

#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_INVALID         (0x00000000)
#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_1_0             (0x00000001)
#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_2_0             (0x00000002)
#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_2_2             (0x00000004)
#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_3_0             (0x00000005)
#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_3_1             (0x00000006)
#define NVSWITCH_NVLINK_CAPS_NCI_VERSION_4_0             (0x00000007)


/*
 * NVSWITCH_NVLINK_LINK_STATUS_INFO
 *
 * This structure stores the per-link status of different NVLink parameters.
 *
 *   capsTbl
 *     This is bit field for getting different global caps. The individual
 *     bitfields are specified by NVSWITCH_NVLINK_CAPS_*
 *   phyType
 *     This field specifies the type of PHY (NVHS or GRS) being used for this
 *     link.
 *   subLinkWidth
 *     This field specifies the no. of lanes per sublink.
 *   linkState
 *     This field specifies the current state of the link. See
 *     NVSWITCH_GET_NVLINK_STATUS_LINK_STATE_* for possible values.
 *   linkPowerState
 *     This field specifies the current power state of the link. See
 *     NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_* for possible values.
 *   rxSublinkStatus
 *     This field specifies the current state of RX sublink. See
 *     NVSWITCH_GET_NVLINK_STATUS_SUBLINK_RX_STATE_* for possible values.
 *   txSublinkStatus
 *     This field specifies the current state of TX sublink. See
 *     NVSWITCH_GET_NVLINK_STATUS_SUBLINK_TX_STATE_* for possible values.
 *   nvlinkVersion
 *     This field specifies the NVLink version supported by the link.
 *   nciVersion
 *     This field specifies the NCI version supported by the link.
 *   phyVersion
 *     This field specifies the version of PHY being used by the link.
 *   nvlinkCommonClockSpeed
 *     This field gives the value of nvlink common clock.
 *   nvlinkRefClkSpeed
 *     This field gives the value of nvlink refclk clock.
 *   nvlinkRefClkType
 *     This field specifies whether refclk is taken from NVHS reflck or PEX
 *     refclk for the current GPU. See NVSWITCH_NVLINK_REFCLK_TYPE_INVALID*
 *     for possible values.
 *   nvlinkLinkClock
 *     This field gives the actual clock/speed at which links is running.
 *   connected
 *     This field specifies if any device is connected on the other end of the
 *     link
 *   loopProperty
 *     This field specifies if the link is a loopback/loopout link. See
 *     NVSWITCH_NVLINK_STATUS_LOOP_PROPERTY_* for possible values.
 *   laneRxdetStatusMask
 *     This field reports the per-lane RX Detect status provided by MINION.
 *   remoteDeviceLinkNumber
 *     This field specifies the link number on the remote end of the link
 *   remoteDeviceInfo
 *     This field stores the device information for the remote end of the link
 *
 */

typedef struct
{
    // Top level capablilites
    NvU32   capsTbl;

    NvU8    phyType;
    NvU8    subLinkWidth;

    // Link and sublink states
    NvU32   linkState;
    NvU32   linkPowerState;
    NvU8    rxSublinkStatus;
    NvU8    txSublinkStatus;

    // Indicates that lane reveral is in effect on this link.
    NvBool  bLaneReversal;

    NvU8    nvlinkVersion;
    NvU8    nciVersion;
    NvU8    phyVersion;

    // Clock information

    // These are being deprecated, please use HW Consistent terminology below
    NvU32   nvlinkLinkClockKHz;
    NvU32   nvlinkCommonClockSpeedKHz;
    NvU32   nvlinkRefClkSpeedKHz;
    NvU32   nvlinkCommonClockSpeedMhz;

    // HW consistent terminology
    NvU32   nvlinkLineRateMbps;
    NvU32   nvlinkLinkDataRateKiBps;
    NvU32   nvlinkLinkClockMhz;
    NvU32   nvlinkRefClkSpeedMhz;
    NvU8    nvlinkRefClkType;

    // Connection information
    NvBool  connected;
    NvU8    loopProperty;
    NvU8    remoteDeviceLinkNumber;
    NvU8    localDeviceLinkNumber;

    //
    // Added as part of NvLink 3.0
    // Note: SID has link info appended to it when provided by minion
    //
    NV_DECLARE_ALIGNED(NvU64 remoteLinkSid, 8);
    NV_DECLARE_ALIGNED(NvU64 localLinkSid,  8);

    // LR10+ only
    NvU32   laneRxdetStatusMask;

    // LS10+ only
    NvBool  bIsRepeaterMode;

    NVSWITCH_NVLINK_DEVICE_INFO remoteDeviceInfo;
    NVSWITCH_NVLINK_DEVICE_INFO localDeviceInfo;
} NVSWITCH_NVLINK_LINK_STATUS_INFO;

/* NVLink link states */
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_INIT               (0x00000000)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_HWPCFG             (0x0000000c)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_HWCFG              (0x00000001)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_SWCFG              (0x00000002)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_ACTIVE             (0x00000003)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_FAULT              (0x00000004)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_SLEEP              (0x00000005)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_RECOVERY           (0x00000006)
#define NVSWITCH_NVLINK_STATUS_LINK_STATE_INVALID            (0xFFFFFFFF)

/* NVLink link power states */
#define NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_L0           (0x00000000)
#define NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_L1           (0x00000001)
#define NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_INVALID      (0xFFFFFFFF)

/* NVLink Tx sublink states */
#define NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_HIGH_SPEED_1 (0x00000000)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_SINGLE_LANE  (0x00000004)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_TRAINING     (0x00000005)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_SAFE_MODE    (0x00000006)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_OFF          (0x00000007)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_INVALID      (0x000000FF)

/* NVLink Rx sublink states */
#define NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_HIGH_SPEED_1 (0x00000000)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_SINGLE_LANE  (0x00000004)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_TRAINING     (0x00000005)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_SAFE_MODE    (0x00000006)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_OFF          (0x00000007)
#define NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_INVALID      (0x000000FF)

#define NVSWITCH_NVLINK_STATUS_PHY_NVHS                      (0x00000001)
#define NVSWITCH_NVLINK_STATUS_PHY_GRS                       (0x00000002)
#define NVSWITCH_NVLINK_STATUS_PHY_INVALID                   (0x000000FF)

/* Version information */
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_1_0            (0x00000001)
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_2_0            (0x00000002)
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_2_2            (0x00000004)
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_3_0            (0x00000005)
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_3_1            (0x00000006)
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_4_0            (0x00000007)
#define NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_INVALID        (0x000000FF)

#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_1_0               (0x00000001)
#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_2_0               (0x00000002)
#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_2_2               (0x00000004)
#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_3_0               (0x00000005)
#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_3_1               (0x00000006)
#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_4_0               (0x00000007)
#define NVSWITCH_NVLINK_STATUS_NCI_VERSION_INVALID           (0x000000FF)

#define NVSWITCH_NVLINK_STATUS_NVHS_VERSION_1_0              (0x00000001)
#define NVSWITCH_NVLINK_STATUS_NVHS_VERSION_INVALID          (0x000000FF)

#define NVSWITCH_NVLINK_STATUS_GRS_VERSION_1_0               (0x00000001)
#define NVSWITCH_NVLINK_STATUS_GRS_VERSION_INVALID           (0x000000FF)

/* Connection properties */
#define NVSWITCH_NVLINK_STATUS_CONNECTED_TRUE                (0x00000001)
#define NVSWITCH_NVLINK_STATUS_CONNECTED_FALSE               (0x00000000)

#define NVSWITCH_NVLINK_STATUS_LOOP_PROPERTY_LOOPBACK        (0x00000001)
#define NVSWITCH_NVLINK_STATUS_LOOP_PROPERTY_LOOPOUT         (0x00000002)
#define NVSWITCH_NVLINK_STATUS_LOOP_PROPERTY_NONE            (0x00000000)

#define NVSWITCH_NVLINK_STATUS_REMOTE_LINK_NUMBER_INVALID    (0x000000FF)

#define NVSWITCH_NVLINK_MAX_LINKS                            64

/* NVLink REFCLK types */
#define NVSWITCH_NVLINK_REFCLK_TYPE_INVALID                  (0x00)
#define NVSWITCH_NVLINK_REFCLK_TYPE_NVHS                     (0x01)
#define NVSWITCH_NVLINK_REFCLK_TYPE_PEX                      (0x02)

typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 enabledLinkMask, 8);
    NVSWITCH_NVLINK_LINK_STATUS_INFO linkInfo[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_GET_NVLINK_STATUS_PARAMS;

/* List of supported capability type */
#define NVSWITCH_CAP_FABRIC_MANAGEMENT 0

/*
 * Max supported capabilities count.
 */
#define NVSWITCH_CAP_COUNT 1

/*
 * CTRL_NVSWITCH_ACQUIRE_CAPABILITY
 *
 * Upon success, user mode would acquire the requested capability
 * to perform privilege operations. This IOCTL will acquire one
 * capability at a time.
 *
 * Parameters:
 *   capDescriptor [IN]
 *      The OS file descriptor or handle representing the capability.
 *   cap [IN]
 *      The requested capability. One of the NVSWITCH_CAP_*.
 */
typedef struct
{
    /* input parameters */
    NV_DECLARE_ALIGNED(NvU64 capDescriptor, 8);
    NvU32 cap;


} NVSWITCH_ACQUIRE_CAPABILITY_PARAMS;

/*
 * CTRL_NVSWITCH_GET_TEMPERATURE
 *
 * Control to query temperature of Nvswitch sensors.
 *
 * The Temperatures are returned in FXP 24.8(NvTemp) format.
 *
 * Parameters:
 *   channelMask [IN]
 *      Mask of all the thermal channels queried.
 *   temperature [OUT]
 *     Temperature of the channel.
 *   status [OUT]
 *     Return status of the channel.
 */

#define  NVSWITCH_NUM_MAX_CHANNELS  16

typedef struct
{
    NvU32  channelMask;
    NvTemp temperature[NVSWITCH_NUM_MAX_CHANNELS];
    NvS32  status[NVSWITCH_NUM_MAX_CHANNELS];
} NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS;

#define NVSWITCH_CTRL_THERMAL_EVENT_ID_WARN 0
#define NVSWITCH_CTRL_THERMAL_EVENT_ID_OVERT 1

typedef struct
{
    NvU32  thermalEventId;
    NvTemp temperatureLimit;
} NVSWITCH_CTRL_GET_TEMPERATURE_LIMIT_PARAMS;

/*
 * Limerock thermal channels
 */
#define NVSWITCH_THERM_CHANNEL_LR10_TSENSE_MAX         0x00
#define NVSWITCH_THERM_CHANNEL_LR10_TSENSE_OFFSET_MAX  0x01
#define NVSWITCH_THERM_CHANNEL_LR10_TDIODE             0x02
#define NVSWITCH_THERM_CHANNEL_LR10_TDIODE_OFFSET      0x03
#define NVSWITCH_NUM_CHANNELS_LR10                        4

/*
 * Laguna Seca thermal channels
 */
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_MAX         0x00
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_OFFSET_MAX  0x01
#define NVSWITCH_THERM_CHANNEL_LS10_TDIODE             0x02
#define NVSWITCH_THERM_CHANNEL_LS10_TDIODE_OFFSET      0x03
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_0           0x04
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_1           0x05
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_2           0x06
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_3           0x07
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_4           0x08
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_5           0x09
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_6           0x0A
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_7           0x0B
#define NVSWITCH_THERM_CHANNEL_LS10_TSENSE_8           0x0C
#define NVSWITCH_NUM_CHANNELS_LS10                       13

/*
 * CTRL_NVSWITCH_GET_THROUGHPUT_COUNTERS
 *
 * Control for querying NVLINK throughput counters.
 *
 * Parameters:
 *    counterMask [IN]
 *      A mask of counter types.
 *      One of the NVSWITCH_THROUGHPUT_COUNTERS_TYPE_* macros
 *    linkMask [IN]
 *      A mask of desired link(s)
 *    counters [OUT]
 *      Fetched counter values
 */

/* NVLink throughput counter types */

/* Nvlink throughput counters reading data flits in TX */
#define NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_TX       (0x00000001)

/* Nvlink throughput counters reading data flits in RX */
#define NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_RX       (0x00000002)

/* Nvlink throughput counters reading all flits in TX */
#define NVSWITCH_THROUGHPUT_COUNTERS_TYPE_RAW_TX        (0x00000004)

/* Nvlink throughput counters reading all flits in RX */
#define NVSWITCH_THROUGHPUT_COUNTERS_TYPE_RAW_RX        (0x00000008)

#define NVSWITCH_THROUGHPUT_COUNTERS_TYPE_MAX           4

typedef struct nvswitch_throughput_values
{
    NvU64 values[NVSWITCH_THROUGHPUT_COUNTERS_TYPE_MAX];

} NVSWITCH_THROUGHPUT_COUNTER_VALUES;

typedef struct nvswitch_get_throughput_counters
{
    NvU16 counterMask;
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NVSWITCH_THROUGHPUT_COUNTER_VALUES counters[NVSWITCH_MAX_PORTS];

} NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_BIOS_INFO
 *
 * Control call to get VBIOS information.
 *
 * Parameters:
 *     version [OUT]
 *       Vbios version in hex value.
 */
typedef struct nvswitch_get_bios_info
{
    NvU64 version;
} NVSWITCH_GET_BIOS_INFO_PARAMS;

#define NVSWITCH_INFOROM_VERSION_LEN 16
/*
 * CTRL_NVSWITCH_GET_INFOROM_VERSION
 *
 * Control call to get INFOROM information.
 *
 * Parameters:
 *     version [OUT]
 *       Inforom version in char value.
 */
typedef struct nvswitch_get_inforom_version
{
    NvU8 version[NVSWITCH_INFOROM_VERSION_LEN];
} NVSWITCH_GET_INFOROM_VERSION_PARAMS;

/*
 * CTRL_NVSWITCH_BLACKLIST_DEVICE
 *
 * Control to Blacklist a device.  A blacklisted device will have
 * interrupts disabled, and opens/ioctls will fail.  If a device is 
 * blacklisted OOB then the setting is persistent.  If a device is 
 * blacklisted by the OS (such as module parameter) then the setting 
 * persists for the OS until the config file is changed and the driver 
 * reloaded. If a device is blacklisted by ioctl then the setting does
 * not persist across driver unload/reload.
 *
 * See BLACKLIST_REASON enum definition in interface/ioctl_common_nvswitch.h
 *
 * Parameters:
 *    deviceReason [IN]
 *      The reason the device is blacklisted
 */
typedef struct nvswitch_blacklist_device
{
    NVSWITCH_DEVICE_BLACKLIST_REASON deviceReason;
} NVSWITCH_BLACKLIST_DEVICE_PARAMS;

/*
 * CTRL_NVSWITCH_SET_FM_DRIVER_STATE
 *
 * Control to set the FM driver state for a device (heartbeat).
 *
 * Driver Fabric State is intended to reflect the state of the driver and
 * fabric manager.  Once FM sets the Driver State to CONFIGURED, it is
 * expected the FM will send heartbeat updates.  If the heartbeat is not
 * received before the session timeout, then the driver reports status
 * as MANAGER_TIMEOUT.  See also control device ioctl CTRL_NVSWITCH_GET_DEVICES_V2.
 *
 * See DRIVER_FABRIC_STATE enum definition in interface/ioctl_common_nvswitch.h
 *
 * Parameters:
 *    driverState [IN]
 *      The driver state for the device
 */
typedef struct nvswitch_set_fm_driver_state
{
    NVSWITCH_DRIVER_FABRIC_STATE driverState;
} NVSWITCH_SET_FM_DRIVER_STATE_PARAMS;

/*
 * CTRL_NVSWITCH_SET_DEVICE_FABRIC_STATE
 *
 * Control to set the device fabric state
 *
 * Device Fabric State reflects the fabric state of the nvswitch device.
 * FM sets the Device Fabric State to CONFIGURED once FM is managing the
 * device.
 *
 * See DEVICE_FABRIC_STATE enum definition in interface/ioctl_common_nvswitch.h
 *
 * Parameters:
 *    deviceState [IN]
 *      The device fabric state
 */
typedef struct nvswitch_set_device_fabric_state
{
    NVSWITCH_DEVICE_FABRIC_STATE deviceState;
} NVSWITCH_SET_DEVICE_FABRIC_STATE_PARAMS;

/*
 * CTRL_NVSWITCH_SET_FM_HEARTBEAT_TIMEOUT
 *
 * Control to set the FM session heartbeat timeout for a device
 *
 * If a device is managed by FM, and if a heartbeat is not received
 * by the FM_HEARTBEAT_TIMEOUT, then the driver reports Driver
 * Fabric State as MANAGER_TIMEOUT.
 *
 * NVSWITCH_DEFAULT_FM_HEARTBEAT_TIMEOUT_MSEC is the default timeout
 *
 * Parameters:
 *    fmTimeout [IN]
 *      The FM timeout value for the device, in milliseconds
 */
typedef struct nvswitch_set_fm_heartbeat_timeout
{
    NvU32 fmTimeout;
} NVSWITCH_SET_FM_HEARTBEAT_TIMEOUT_PARAMS;
#define NVSWITCH_DEFAULT_FM_HEARTBEAT_TIMEOUT_MSEC (10*1000)

/*
 * CTRL_NVSWITCH_SET_LINK_ERROR_STATE_INFO
 *
 * Control to set bitmask info of the
 * link training error
 *
 * Parameters:
 *    attemptedTrainingMask0 [IN]
 *      Bitmask of links that have been
 *      attempted to train.
 *    trainingErrorMask0     [IN]
 *      Bitmaks of links that have an error
 *      during training.
 */
typedef struct nvswitch_set_training_error_info
{
    NvU64 attemptedTrainingMask0;
    NvU64 trainingErrorMask0;
} NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS;

#define NVSWITCH_DEVICE_EVENT_FATAL           0
#define NVSWITCH_DEVICE_EVENT_NONFATAL        1
#define NVSWITCH_DEVICE_EVENT_PORT_UP         2
#define NVSWITCH_DEVICE_EVENT_PORT_DOWN       3
#define NVSWITCH_DEVICE_EVENT_FABRIC_STATE    4
#define NVSWITCH_DEVICE_EVENT_INBAND_DATA     5
#define NVSWITCH_DEVICE_EVENT_COUNT           6
#define NVSWITCH_REGISTER_EVENTS_MAX_EVENT_IDS (500)

/*
 * CTRL_NVSWITCH_REGISTER_EVENTS
 *
 * Control to register event IDs with an OS descriptor
 *
 * This control allows for clients to register one or more event IDs
 * with an OS descriptor. After registering event IDs, clients may poll
 * the OS descriptor for the registered event.
 *
 * Subsequent calls to register_event will overwrite currently registered
 * event IDs. This allows the client to switch event polling as and when required.
 * Explicit unregister_events control call isn't necessary when the
 * client wishes to change the event types currently being monitored.
 *
 * On Linux, only a single event ID can be registered to each
 * OS descriptor at a time. Calling this control with
 * numEvents > 1 on Linux will cause an error to be returned.
 *
 * On Windows, the osDescriptor field should be a valid
 * Windows EVENT handle.
 *
 * osDescriptor is unused on other operating systems.
 *
 * Parameters:
 *    eventIds [IN]
 *      A buffer of event IDs to register for
 *    numEvents [IN]
 *      Number of valid elements in eventIds
 *    osDescriptor [IN]
 *      OS event handle (Windows only)
 */
typedef struct nvswitch_register_events
{
    NvU32 eventIds[NVSWITCH_REGISTER_EVENTS_MAX_EVENT_IDS];
    NvU32 numEvents;
    void *osDescriptor;
} NVSWITCH_REGISTER_EVENTS_PARAMS;

/*
 * CTRL_NVSWITCH_UNREGISTER_EVENTS
 *
 * Control to unregister all event IDs from an OS descriptor
 *
 * This control unregisters all registered event IDs associated
 * with an OS descriptor.
 *
 * On Windows, the osDescriptor field should be a valid
 * Windows EVENT handle.
 *
 * osDescriptor is unused on other operating systems.
 *
 * Parameters:
 *    osDescriptor [IN]
 *      OS event handle (Windows only)
 */
typedef struct nvswitch_unregister_events
{
    void *osDescriptor;
} NVSWITCH_UNREGISTER_EVENTS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_FATAL_ERROR_SCOPE
 *
 * Control to query if a fatal error is occurred on a port or device
 *
 * Parameters:
 *    device [OUT]
 *      Set to NV_TRUE if the nvswitch device encountered a fatal error
 *    port [OUT]
 *      An array of booleans indicating which ports
 *      encountered a fatal error
 */
typedef struct nvswitch_get_fatal_error_scope_params
{
    NvBool device;
    NvBool port[NVSWITCH_MAX_PORTS];
} NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS;

/*
 * CTRL_NVSWITCH_SET_MC_RID_TABLE
 *
 * Control for programming an ingress multicast RID table entry.
 * This interface is only supported on LS10 architecture.  All others will
 * return an error. Architecture can be queried using _GET_INFO_INDEX_ARCH.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   index [IN]
 *      Index within the multicast RID table to be programmed. This is
 *      equivalent to MCID.
 *   extendedTable [IN]
 *      boolean: Set the requested entry in the extended table
 *      else set the requested entry in the main table
 *   ports [IN]
 *      The list of ports. For each multicast request, the address hash
 *      selects the multicast port string, and hardware multicasts to ports
 *      in that string.
 *   vcHop [IN]
 *      Array of VC hop values for each port.
 *   mcSize [IN]
 *      Number of ports in the multicast group (must be a nonzero value).
 *      Must be the number of ports in the main table, plus the extended table
 *      if that is used.
 *      Must be the same for all spray groups.
 *      Caller is responsible for ensuring the above conditions, as the driver
 *      provides only minimal range checking.
 *   numSprayGroups [IN]
 *      Number of groups to spray over. This must be a nonzero value.
 *   portsPerSprayGroup [IN]
 *      Array, number of ports contained in each spray group.
 *      Note these must all be the same size unless an extended entry
 *      is used,
 *      _and_ numSprayGroups is the same for both the main entry and extended
 *      entry,
 *      _and_ the sum of ports in the main and extended groups equals
 *      mcSize for each spray group.
 *      FM is responsible for providing the correct value. Driver provides only
 *      minimal range checking.
 *   replicaOffset [IN]
 *      Array, offsets within each spray group to the primary replica port for the group.
 *      The caller should specify mcSize primaryReplicas.
 *   replicaValid [IN]
 *      boolean:  Array, set the primary replica according to the replicaOffset array.
 *      else let hardware choose a default primary replica port
 *   extendedPtr [IN]
 *      pointer to the extended table to append to the multicast table entry
 *      can only be valid in the main table entries
 *   extendedValid [IN]
 *      boolean: Use the extended index to append to the main table string.
 *      else the main string specifies the complete operation for its MCID
 *   noDynRsp [IN]
 *      boolean: no dynamic alt selection on MC responses. This field has no meaning in
 *      the extended table
 *   entryValid
 *      boolean: flag this entry in the MC RID table as valid
 */

#define NVSWITCH_MC_MAX_PORTS           64
#define NVSWITCH_MC_MAX_SPRAYGROUPS     16

#define NVSWITCH_MC_VCHOP_PASS          0
#define NVSWITCH_MC_VCHOP_INVERT        1
#define NVSWITCH_MC_VCHOP_FORCE0        2
#define NVSWITCH_MC_VCHOP_FORCE1        3

typedef struct nvswitch_set_mc_rid_table_params
{
    NvU32                           portNum;
    NvU32                           index;
    NvBool                          extendedTable;
    NvU32                           ports[NVSWITCH_MC_MAX_PORTS];
    NvU8                            vcHop[NVSWITCH_MC_MAX_PORTS];
    NvU32                           mcSize;
    NvU32                           numSprayGroups;
    NvU32                           portsPerSprayGroup[NVSWITCH_MC_MAX_SPRAYGROUPS];
    NvU32                           replicaOffset[NVSWITCH_MC_MAX_SPRAYGROUPS];
    NvBool                          replicaValid[NVSWITCH_MC_MAX_SPRAYGROUPS];
    NvU32                           extendedPtr;
    NvBool                          extendedValid;
    NvBool                          noDynRsp;
    NvBool                          entryValid;
} NVSWITCH_SET_MC_RID_TABLE_PARAMS;

/*
 * CTRL_NVSWITCH_GET_MC_RID_TABLE
 *
 * Control for reading an ingress multicast RID table entry.
 * This interface is only supported on LS10 architecture.  All others will
 * return an error. Architecture can be queried using _GET_INFO_INDEX_ARCH.
 *
 * Parameters:
 *   portNum [IN]
 *      A valid port number present in the port masks returned by
 *      NVSWITCH_GET_INFO
 *   index [IN]
 *      Index within the multicast RID table to be retrieved. This is
 *      equivalent to MCID.
 *   extendedTable [IN]
 *      boolean: Get the requested entry from the extended table.
 *      Else get the requested entry from the main table.
 *   ports [OUT]
 *      The list of ports. Port order within spray groups is not guaranteed
 *      to be preserved.
 *   vcHop [OUT]
 *      Array containing VC hop values for each entry in the ports array.
 *   mcSize [OUT]
 *      Number of ports in the multicast group.
 *   numSprayGroups [OUT]
 *      Number of groups to spray over.
 *   portsPerSprayGroup [OUT]
 *      Array, each element contains the number of ports within each corresponding
 *      spray group.
 *   replicaOffset [OUT]
 *      Array, offsets within each spray group to the primary replica port
 *      for the group.
 *   replicaValid [OUT]
 *      boolean:  Array, specifies whether each entry in the replicaOffset
 *      array is valid.
 *   extendedPtr [OUT]
 *      Pointer to the extended table appended to the main table entry.
 *      Only valid for main table entries.
 *   extendedValid [OUT]
 *      boolean: Whether the extendedPtr is valid.
 *   noDynRsp [IN]
 *      boolean: no dynamic alt selection on MC responses.
 *      This field has no meaning in the extended table.
 *   entryValid
 *      boolean: Whether this entry in the MC RID table is valid
 */

typedef struct nvswitch_get_mc_rid_table_params
{
    NvU32                           portNum;
    NvU32                           index;
    NvBool                          extendedTable;
    NvU32                           ports[NVSWITCH_MC_MAX_PORTS];
    NvU8                            vcHop[NVSWITCH_MC_MAX_PORTS];
    NvU32                           mcSize;
    NvU32                           numSprayGroups;
    NvU32                           portsPerSprayGroup[NVSWITCH_MC_MAX_SPRAYGROUPS];
    NvU32                           replicaOffset[NVSWITCH_MC_MAX_SPRAYGROUPS];
    NvBool                          replicaValid[NVSWITCH_MC_MAX_SPRAYGROUPS];
    NvU32                           extendedPtr;
    NvBool                          extendedValid;
    NvBool                          noDynRsp;
    NvBool                          entryValid;
} NVSWITCH_GET_MC_RID_TABLE_PARAMS;

#define NVSWITCH_I2C_SMBUS_CMD_QUICK      0
#define NVSWITCH_I2C_SMBUS_CMD_BYTE       1
#define NVSWITCH_I2C_SMBUS_CMD_BYTE_DATA  2
#define NVSWITCH_I2C_SMBUS_CMD_WORD_DATA  3

/*
 * NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW
 *
 * This structure provides data for the SMBUS Byte command.
 *
 * message [IN/OUT]
 *    8 Bit data message to read or write.
 */
typedef struct
{
    NvU8   message;
} NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW;

/*
 * NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_BYTE_DATA_RW
 *
 * This structure provides data for the SMBUS Byte Data command.
 *
 * cmd [IN]
 *   SMBUS input command.
 * message [IN/OUT]
 *    8 Bit data message to read or write.
 */
typedef struct
{
    NvU8   cmd;
    NvU8   message;
} NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_BYTE_DATA_RW;

/*
 * NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_WORD_DATA_RW
 *
 * This structure provides data for the SMBUS Word Data command.
 *
 * cmd [IN]
 *   SMBUS input command.
 * message [IN/OUT]
 *    16 Bit data message to read or write.
 */
typedef struct
{
    NvU8   cmd;
    NvU16  message;
} NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_WORD_DATA_RW;

typedef union
{
    NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW smbusByte;
    NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_BYTE_DATA_RW smbusByteData;
    NVSWITCH_I2C_TRANSACTION_DATA_SMBUS_WORD_DATA_RW smbusWordData;
} NVSWITCH_I2C_TRANSACTION_DATA;

/*
 * CTRL_NVSWITCH_I2C_SMBUS_COMMAND
 *
 * Control to issue SMBUS I2C transaction to an I2C device
 *
 * Parameters:
 *    deviceAddr [IN]
 *       The I2C Slave address to issue a transaction to. This is the unshifted,
 *       normal 7-bit address. For example, the input would be address 0x50 for
 *       device 0xA0.
 *    port [IN]
 *       The logical port/bus in which the I2C transaction is requested.
 *    cmdType [IN]
 *       The desired SMBUS command type. See NVSWITCH_I2C_SMBUS_CMD_*.
 *    bRead [IN]
 *       This field must be specified to indicate whether the
 *       command is a write (FALSE) or a read (TRUE).
 *    transactionData [IN/OUT]
 *       The NVSWITCH_I2C_TRANSACTION_DATA union to be filled out/read back
 *       depending on the SMBUS command type.
 */
typedef struct nvswitch_i2c_smbus_command_params
{
    NvU16  deviceAddr;
    NvU32  port;
    NvU8   cmdType;
    NvBool bRead;
    NVSWITCH_I2C_TRANSACTION_DATA transactionData;
} NVSWITCH_I2C_SMBUS_COMMAND_PARAMS;

/*
 * APIs for getting NVLink counters
 */

// These are the bitmask definitions for different counter types

#define NVSWITCH_NVLINK_COUNTER_INVALID                      0x00000000

#define NVSWITCH_NVLINK_COUNTER_TL_TX0                       0x00000001
#define NVSWITCH_NVLINK_COUNTER_TL_TX1                       0x00000002
#define NVSWITCH_NVLINK_COUNTER_TL_RX0                       0x00000004
#define NVSWITCH_NVLINK_COUNTER_TL_RX1                       0x00000008

#define NVSWITCH_NVLINK_LP_COUNTERS_DL                       0x00000010

#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT           0x00010000

#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(i)      (1 << (i + 17))
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE__SIZE     8
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0        0x00020000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1        0x00040000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2        0x00080000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3        0x00100000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4        0x00200000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5        0x00400000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6        0x00800000
#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7        0x01000000

#define NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY             0x02000000
#define NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY           0x04000000

#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY             0x08000000

#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_MASKED         0x10000000

#define NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_ECC_COUNTS         0x20000000

#define NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS             0x40000000
#define NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL             0x80000000

/*
 * Note that COUNTER_MAX_TYPES will need to be updated each time adding
 * a new counter type exceeds the existing value.
 *
 */
#define NVSWITCH_NVLINK_COUNTER_MAX_TYPES                    32

/*
 * CTRL_NVSWITCH_GET_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * [in] linkId
 *  This parameter specifies the TL link id/no for which we want to get
 *  counters for.
 *
 * [in]  counterMask
 *  This parameter specifies the input mask for desired counter types.
 *
 * [out] bTx0TlCounterOverflow
 *  This boolean is set to NV_TRUE if TX Counter 0 has rolled over.
 *
 * [out] bTx1TlCounterOverflow
 *  This boolean is set to NV_TRUE if TX Counter 1 has rolled over.
 *
 * [out] bRx0TlCounterOverflow
 *  This boolean is set to NV_TRUE if RX Counter 0 has rolled over.
 *
 * [out] bRx1TlCounterOverflow
 *  This boolean is set to NV_TRUE if RX Counter 1 has rolled over.
 *
 * [out] nvlinkCounters
 *  This array contains the error counts for each error type as requested from
 *  the counterMask. The array indexes correspond to the mask bits one-to-one.
 */

typedef struct
{
    NvU8   linkId;
    NvU32  counterMask;
    NvBool bTx0TlCounterOverflow;
    NvBool bTx1TlCounterOverflow;
    NvBool bRx0TlCounterOverflow;
    NvBool bRx1TlCounterOverflow;
    NV_DECLARE_ALIGNED(NvU64 nvlinkCounters[NVSWITCH_NVLINK_COUNTER_MAX_TYPES], 8);
} NVSWITCH_NVLINK_GET_COUNTERS_PARAMS;

/*
 * Structure to store the ECC error data.
 * valid
 *     Is the lane valid or not
 * eccErrorValue
 *     Value of the Error.
 * overflowed
 *     If the error overflowed or not
 */
typedef struct
{
    NvBool valid;
    NvU32  eccErrorValue;
    NvBool overflowed;
} NVSWITCH_LANE_ERROR;

/*
 * Structure to store ECC error data for Links
 * errorLane array index corresponds to the lane number.
 *
 * errorLane[]
 *    Stores the ECC error data per lane.
 */
typedef struct
{
    NVSWITCH_LANE_ERROR       errorLane[NVSWITCH_NVLINK_MAX_LANES];
    NvU32                     eccDecFailed;
    NvBool                    eccDecFailedOverflowed;
} NVSWITCH_LINK_ECC_ERROR;

/*
 * CTRL_GET_NVLINK_ECC_ERRORS
 *
 * Control to get the values of ECC ERRORS
 *
 * Parameters:
 *    linkMask [IN]
 *      Links on which the ECC error data requested
 *      A valid link/port mask returned by the port masks returned by
 *      NVSWITCH_GET_INFO
 *    errorLink[] [OUT]
 *      Stores the ECC error related information for each link.
 *      errorLink array index corresponds to the link Number.
 */

typedef struct nvswitch_get_nvlink_ecc_errors
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NVSWITCH_LINK_ECC_ERROR   errorLink[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_GET_NVLINK_ECC_ERRORS_PARAMS;

/*
 * CTRL_NVSWITCH_CCI_CMIS_PRESENCE
 *
 * Control to get module presence bitmasks
 *
 * Parameters:
 *    cagesMask [OUT]
 *      Bitmask representing the CMIS module cages present 
 *      (associated with) the selected ASIC device
 *    modulesMask [OUT]  
 *      Bitmask representing the CMIS modules currently present 
 *      (plugged in) on the selected ASIC device
 */

typedef struct nvswitch_cci_cmis_presence_params
{
    NvU32 cagesMask;
    NvU32 modulesMask;
    NvU32 reserved0;
    NvU32 reserved1;
    NvU32 reserved2;
    NvU32 reserved3;
} NVSWITCH_CCI_CMIS_PRESENCE_PARAMS;

#define NVSWITCH_CCI_CMIS_NVLINK_MAPPING_ENCODED_VALUE(i)           (7 + (i<<3)):(i<<3)
#define NVSWITCH_CCI_CMIS_NVLINK_MAPPING_ENCODED_VALUE_LINK_ID      5:0
#define NVSWITCH_CCI_CMIS_NVLINK_MAPPING_GET_OSFP_LANE_MASK(laneMask, linkId, eVal)                   \
    do {                                                                                              \
        NvU8 _byte, _lane;                                                                            \
                                                                                                      \
        laneMask = 0;                                                                                 \
        for (_lane = 0; _lane < 8; _lane++)                                                           \
        {                                                                                             \
            _byte = REF_VAL64(NVSWITCH_CCI_CMIS_NVLINK_MAPPING_ENCODED_VALUE(_lane), eVal);           \
            if (REF_VAL64(NVSWITCH_CCI_CMIS_NVLINK_MAPPING_ENCODED_VALUE_LINK_ID, _byte) == linkId)   \
            {                                                                                         \
                laneMask |= NVBIT(_lane);                                                             \
            }                                                                                         \
        }                                                                                             \
    } while (0);

/*
 * CTRL_NVSWITCH_CCI_CMIS_NVLINK_MAPPING
 *
 * Control to get cage to NVLink link mappings
 *
 * Provided macros should be used to extract fields from
 * encoded value.
 *
 * Parameters:
 *    cageIndex [IN]
 *      Target cage index (>=0 and <= 31) on the 
 *      selected ASIC device.
 *    linkMask [OUT]
 *      Mask of Nvlinks mapped to the given cage
 *    encodedValue [OUT]  
 *      Value that encodes the following:
 *      -Link Ids to OSFP lane number
 */
 
typedef struct nvswitch_cci_cmis_nvlink_mapping_params
{
    NvU8 cageIndex;
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);    
    NV_DECLARE_ALIGNED(NvU64 encodedValue, 8);
    NvU32 reserved0;
    NvU32 reserved1;
    NvU32 reserved2;
    NvU32 reserved3;
} NVSWITCH_CCI_CMIS_NVLINK_MAPPING_PARAMS;

#define NVSWITCH_CCI_CMIS_MEMORY_ACCESS_BUF_SIZE (128)

/*
 * CTRL_NVSWITCH_CCI_CMIS_MEMORY_ACCESS_READ
 *
 * Control for direct memory accesses to cages
 *
 * Parameters:
 *    cageIndex [IN]
 *      Target cage index (>=0 and <= 31) on the 
 *      selected ASIC device
 *    bank [IN]
 *      Target bank in module (if address >= 0x80)
 *    page [IN]
 *      Target page in module (if address >= 0x80)
 *    address [IN]
 *      Target byte address (offset) in module
 *    count [IN]
 *      Number of bytes to read (>=0 and <= 0x7F)
 *    bSequenceLock [IN] 
 *      Allows clients to own the module for
 *      CMIS read/write sequences. This must
 *      be set to TRUE for all accesses within 
 *      the sequence. Setting it to FALSE will
 *      release the lock. The lock will expire
 *      and be released if the module is left idle 
 *      for greater than 10 seconds.
 *    data[] [OUT]  
 *      128 byte data buffer
 */

typedef struct nvswitch_cci_cmis_memory_access_read_params
{
    NvU8 cageIndex;
    NvU8 bank;
    NvU8 page;
    NvU8 address;
    NvU8 count;
    NvU8 data[NVSWITCH_CCI_CMIS_MEMORY_ACCESS_BUF_SIZE];
    NvBool bSequenceLock;
    NvU8  reserved0[3];
    NvU32 reserved1;
    NvU32 reserved2;
    NvU32 reserved3;
} NVSWITCH_CCI_CMIS_MEMORY_ACCESS_READ_PARAMS;

/*
 * CTRL_NVSWITCH_CCI_CMIS_MEMORY_ACCESS_WRITE
 *
 * Control for direct memory accesses to cages
 *
 * Parameters:
 *    cageIndex [IN]
 *      Target cage index (>=0 and <= 31) on the 
 *      selected ASIC device
 *    bank [IN]
 *      Target bank in module (if address >= 0x80)
 *    page [IN]
 *      Target page in module (if address >= 0x80)
 *    address [IN]
 *      Target byte address (offset) in module
 *    count [IN]
 *      Number of bytes to write (>=0 and <= 0x7F)
 *    bSequenceLock [IN] 
 *      Allows clients to own the module for
 *      CMIS read/write sequences. This must
 *      be set to TRUE for all accesses within 
 *      the sequence. Setting it to FALSE will
 *      release the lock. The lock will expire
 *      and be released if the module is left idle 
 *      for greater than 10 seconds.
 *    data[] [IN]  
 *      128 byte data buffer
 */

typedef struct nvswitch_cci_cmis_memory_access_write_params
{
    NvU8 cageIndex;
    NvU8 bank;
    NvU8 page;
    NvU8 address;
    NvU8 count;
    NvU8 data[NVSWITCH_CCI_CMIS_MEMORY_ACCESS_BUF_SIZE];
    NvBool bSequenceLock;
    NvU8  reserved0[3];
    NvU32 reserved1;
    NvU32 reserved2;
    NvU32 reserved3;    
} NVSWITCH_CCI_CMIS_MEMORY_ACCESS_WRITE_PARAMS;

#define NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING_LEN    31

/*
 * CTRL_NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING
 *
 * Control to get bezel information for a cage.
 *
 * Parameters:
 *    cageIndex [IN]
 *      Target cage index (>=0 and <= 31) on the 
 *      selected ASIC device.
 *    bezelMarking [OUT]
 *              
 */

typedef struct nvswitch_cci_cmis_cage_bezel_marking_params
{
    NvU8 cageIndex;
    char bezelMarking[NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING_LEN + 1];
    NvU32 reserved0;
    NvU32 reserved1;
    NvU32 reserved2;
    NvU32 reserved3;
} NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING_PARAMS;

#define NVSWITCH_CCI_XVCR_LANES     0x8

/*
 *
 * Structure to store cci grading values
 *
 *
 * This API is not supported on SV10.
 *
 * Parameters:
 *   tx_init
 *     TX-Input Initial Tuning grading.
 *   rx_init
 *     RX-Input Initial Tuning grading.
 *   tx_maint
 *     TX-Input Maintenance grading.
 *   rx_maint
 *     RX-Input Maintenance grading.
 */
typedef struct nvswitch_cci_grading_values
{
    NvU8  tx_init[NVSWITCH_CCI_XVCR_LANES];
    NvU8  rx_init[NVSWITCH_CCI_XVCR_LANES];
    NvU8  tx_maint[NVSWITCH_CCI_XVCR_LANES];
    NvU8  rx_maint[NVSWITCH_CCI_XVCR_LANES];
} NVSWITCH_CCI_GRADING_VALUES;

/*
 * CTRL_NVSWITCH_CCI_GET_GRADING_VALUES
 *
 * Control to get cci xvcr grading values
 *
 *
 * This API is not supported on SV10.
 *
 * Parameters:
 *   link [IN]
 *     Link number
 *   laneMask [OUT]
 *     Lane mask of valid indexes in the grading data
 *   grading [OUT]
 *     xvcr grading values
 */
typedef struct nvswitch_cci_get_grading_values_params
{
    NvU32 linkId;
    NvU8  laneMask;
    NVSWITCH_CCI_GRADING_VALUES grading;
} NVSWITCH_CCI_GET_GRADING_VALUES_PARAMS;

#define NVSWITCH_NVLINK_MAX_CORRECTABLE_ERROR_DAYS      5
#define NVSWITCH_NVLINK_MAX_CORRECTABLE_ERROR_MONTHS    3

typedef struct
{
    NvU32 lastUpdated;
    NvU32 flitCrcErrorsPerMinute;
    NvU32 laneCrcErrorsPerMinute[NVSWITCH_NVLINK_MAX_LANES];
} NVSWITCH_NVLINK_CORRECTABLE_ERROR_RATES;

#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_NVHS      0
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_RESERVED  1
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_OTHER     2
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_NUM_TX_LP_ENTER    3
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_NUM_TX_LP_EXIT     4
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_SLEEP     5
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_MAX_COUNTERS       6
/*
 * CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS
 *
 * Reads NVLINK low power counters for given linkId
 *
 * Parameters:
 *    linkId [IN]
 *      ID of the link to be queried
 *    counterValidMask [IN,OUT]
 *      Mask of valid counters
 *    counterValues [OUT]
 *      Low power counter values returned
 */
typedef struct nvswitch_get_nvlink_lp_counters_params
{
      NvU32 linkId;
      NvU32 counterValidMask;
      NvU32 counterValues[CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_MAX_COUNTERS];
} NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES
 *
 * This command queries recent correctable error rates for the given link.
 *
 * The error rates specify the maximum number of errors per minute recorded
 * for the given link within a 24-hour period for daily maximums or a 30-day
 * period for monthly maximums.
 *
 * Parameters:
 *    linkId [in]
 *      NVLink link ID
 *    dailyMaxCorrectableErrorRates[] [OUT]
 *      NVLink daily max correctable error rate array
 *    monthlyMaxCorrectableErrorRates[] [OUT]
 *      NVLink monthly max correctable error rate array
 */
 
typedef struct
{
    NvU8   linkId;
    NVSWITCH_NVLINK_CORRECTABLE_ERROR_RATES dailyMaxCorrectableErrorRates[NVSWITCH_NVLINK_MAX_CORRECTABLE_ERROR_DAYS];
    NVSWITCH_NVLINK_CORRECTABLE_ERROR_RATES monthlyMaxCorrectableErrorRates[NVSWITCH_NVLINK_MAX_CORRECTABLE_ERROR_MONTHS];
} NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS;

#define NVSWITCH_NVLINK_ERROR_READ_SIZE            128 //could not read the maximum size (721) of entries in one call

typedef enum
{
    NVSWITCH_NVLINK_NO_ERROR                                    = 0,

    //DL RX Fatal Counts
    NVSWITCH_NVLINK_ERR_DL_RX_FAULT_DL_PROTOCOL_FATAL           = 1000,
    NVSWITCH_NVLINK_ERR_DL_RX_FAULT_SUBLINK_CHANGE_FATAL,

    //DL RX Correctable Accumulated Counts
    NVSWITCH_NVLINK_ERR_DL_RX_FLIT_CRC_CORR, 
    NVSWITCH_NVLINK_ERR_DL_RX_LANE0_CRC_CORR,
    NVSWITCH_NVLINK_ERR_DL_RX_LANE1_CRC_CORR,
    NVSWITCH_NVLINK_ERR_DL_RX_LANE2_CRC_CORR,
    NVSWITCH_NVLINK_ERR_DL_RX_LANE3_CRC_CORR,
    NVSWITCH_NVLINK_ERR_DL_RX_LINK_REPLAY_EVENTS_CORR,

    //DL TX Fatal Counts
    NVSWITCH_NVLINK_ERR_DL_TX_FAULT_RAM_FATAL,
    NVSWITCH_NVLINK_ERR_DL_TX_FAULT_INTERFACE_FATAL,
    NVSWITCH_NVLINK_ERR_DL_TX_FAULT_SUBLINK_CHANGE_FATAL,

    //DL TX Correctable Accumulated Counts
    NVSWITCH_NVLINK_ERR_DL_TX_LINK_REPLAY_EVENTS_CORR,

    //DL NA Fatal Counts
    NVSWITCH_NVLINK_ERR_DL_LTSSM_FAULT_UP_FATAL,
    NVSWITCH_NVLINK_ERR_DL_LTSSM_FAULT_DOWN_FATAL,

    //DL NA Correctable Accumulated Counts
    NVSWITCH_NVLINK_ERR_DL_LINK_RECOVERY_EVENTS_CORR,

    //TLC RX Fatal Counts
    NVSWITCH_NVLINK_ERR_TLC_RX_DL_HDR_PARITY_ERR_FATAL          = 2000,
    NVSWITCH_NVLINK_ERR_TLC_RX_DL_DATA_PARITY_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_DL_CTRL_PARITY_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_AE_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_BE_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_ADDR_ALIGN_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_PKTLEN_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_PACKET_STATUS_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_CACHE_ATTR_PROBE_REQ_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_CACHE_ATTR_PROBE_RSP_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_DATLEN_GT_RMW_REQ_MAX_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_DATLEN_LT_ATR_RSP_MIN_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_CR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_COLLAPSED_RESPONSE_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_HDR_OVERFLOW_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_DATA_OVERFLOW_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_STOMP_DETECTED_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_CMD_ENC_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_DAT_LEN_ENC_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_PO_FOR_CACHE_ATTR_FATAL,

    //TLC RX Non-Fatal Counts
    NVSWITCH_NVLINK_ERR_TLC_RX_RSP_STATUS_HW_ERR_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSP_STATUS_UR_ERR_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_RSP_STATUS_PRIV_ERR_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_POISON_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_AN1_HEARTBEAT_TIMEOUT_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_ILLEGAL_PRI_WRITE_NONFATAL,
    
    //TLC RX Fatal Counts addendum
    NVSWITCH_NVLINK_ERR_TLC_RX_HDR_RAM_ECC_DBE_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_DAT0_RAM_ECC_DBE_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_RX_DAT1_RAM_ECC_DBE_FATAL,

    //TLC TX Fatal Counts
    NVSWITCH_NVLINK_ERR_TLC_TX_DL_CREDIT_PARITY_ERR_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_NCISOC_HDR_ECC_DBE_FATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_NCISOC_PARITY_ERR_FATAL,

    //TLC TX Non-Fatal Counts
    NVSWITCH_NVLINK_ERR_TLC_TX_ILLEGAL_PRI_WRITE_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC0_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC1_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC2_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC3_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC4_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC5_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC6_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC7_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_POISON_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_RSP_STATUS_HW_ERR_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_RSP_STATUS_UR_ERR_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_RSP_STATUS_PRIV_ERR_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_CREQ_DAT_RAM_ECC_DBE_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_RSP_DAT_RAM_ECC_DBE_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_COM_DAT_RAM_ECC_DBE_NONFATAL,
    NVSWITCH_NVLINK_ERR_TLC_TX_RSP1_DAT_RAM_ECC_DBE_FATAL,
    
    //NVLIPT Fatal Counts
    NVSWITCH_NVLINK_ERR_NVLIPT_SLEEP_WHILE_ACTIVE_LINK_FATAL    = 3000,
    NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_PHYCTL_TIMEOUT_FATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_CLKCTL_TIMEOUT_FATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_CLKCTL_ILLEGAL_REQUEST_FATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_PLL_TIMEOUT_FATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_PHYARB_TIMEOUT_FATAL,

    //NVLIPT Non-Fatal Counts
    NVSWITCH_NVLINK_ERR_NVLIPT_ILLEGAL_LINK_STATE_REQUEST_NONFATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_FAILED_MINION_REQUEST_NONFATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_RESERVED_REQUEST_VALUE_NONFATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_LINK_STATE_WRITE_WHILE_BUSY_NONFATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL,
    NVSWITCH_NVLINK_ERR_NVLIPT_LINK_STATE_REQUEST_TIMEOUT_NONFATAL,
} NVSWITCH_NVLINK_ERROR_TYPE;

typedef struct
{
    NvU8  instance;
    NvU32 error; //NVSWITCH_NVLINK_ERROR_TYPE
    NvU32 timeStamp;
    NvU64 count;
} NVSWITCH_NVLINK_ERROR_ENTRY;

/*
 * CTRL_NVSWITCH_GET_NVLINK_ERROR_COUNTS
 *
 * Control to get the NVLINK errors from inforom cache 
 *
 * Parameters:
 *    errorIndex [IN/OUT]
 *      On input: The index of the first NVLink error to retrieve from inforom cache
 *      On output: The index of the first error to retrieve after the previous call.
 *    errorCount [OUT]
 *      Number of errors returned by the call. Currently, errorCount is limited
 *      by NVSWITCH_NVLINK_ERROR_READ_SIZE. In order to query all the errors, a
 *      client needs to keep calling the control till errorCount is zero.
 *    errorLog[] [OUT]
 *      NVLINK error array
 */

typedef struct
{
    NvU32 errorIndex;
    NvU32 errorCount;
    NVSWITCH_NVLINK_ERROR_ENTRY errorLog[NVSWITCH_NVLINK_ERROR_READ_SIZE];
} NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS;

#define NVSWITCH_ECC_ERRORS_MAX_READ_COUNT    128

typedef struct
{
    NvU32  sxid;
    NvU8   linkId;
    NvU32  lastErrorTimestamp;
    NvBool bAddressValid;
    NvU32  address;
    NvU32  correctedCount;
    NvU32  uncorrectedCount;
} NVSWITCH_ECC_ERROR_ENTRY;

/*
 * CTRL_NVSWITCH_GET_ECC_ERROR_COUNTS
 *
 * Control to get the ECC error counts and logs from inforom 
 *
 * Parameters:
 *    uncorrectedTotal [out]
 *      uncorrected ECC errors count
 *    correctedTotal [out]
 *      corrected ECC errors count
 *    errorCount [out]
 *      recorded error log count in the array
 *    errorLog[] [OUT]
 *      ECC errors array
 */

typedef struct
{
    NvU64 uncorrectedTotal;
    NvU64 correctedTotal;
    NvU32 errorCount;
    NVSWITCH_ECC_ERROR_ENTRY errorLog[NVSWITCH_ECC_ERRORS_MAX_READ_COUNT];
} NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS;

#define NVSWITCH_SXID_ENTRIES_NUM    10

typedef struct
{
    NvU32 sxid;
    NvU32 timestamp;
} NVSWITCH_SXID_ENTRY;

/*
 * CTRL_NVSWITCH_GET_SXIDS
 *
 * Control to get the NVSwitch SXID errors from inforom cache 
 *
 * Parameters:
 *    sxidCount [OUT]
 *      The total SXID error number
 *    sxidFirst [OUT]
 *      The array of the first NVSWITCH_SXID_ENTRIES_NUM (10) SXIDs
 *    sxidLast [OUT]
 *      The array of the last NVSWITCH_SXID_ENTRIES_NUM (10) SXIDs
 */

typedef struct
{
    NvU32 sxidCount;
    NVSWITCH_SXID_ENTRY sxidFirst[NVSWITCH_SXID_ENTRIES_NUM];
    NVSWITCH_SXID_ENTRY sxidLast[NVSWITCH_SXID_ENTRIES_NUM];
} NVSWITCH_GET_SXIDS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_SYS_INFO
 *
 * Control to get the NVSwitch system version information from inforom cache 
 *
 * Parameters:
 *    driverLo [OUT]
 *      The driver version low 32 bits. Example: driverLo = 54531 (Driver 545.31)
 *    driverHi [OUT]
 *      The driver version high 16 bits
 *    vbiosVersion [OUT]
 *      The vbios version number. Example: vbiosVersion=0x96104100 (release 96.10.41.00)
 *    vbiosVersionOem [OUT]
 *      The vbios OEM version byte.
 *    osType [OUT]
 *      The OS type. Example:  osType=0x05 (UNIX)
 *    osVersion [OUT]
 *      The OS version number. [BUILD[31:16]|MINOR[15:8]|MAJOR[7:0]]
 */

typedef struct
{
    NvU32 driverLo;
    NvU16 driverHi;
    NvU32 vbiosVersion;
    NvU8  vbiosVersionOem;
    NvU8  osType;
    NvU32 osVersion;
} NVSWITCH_GET_SYS_INFO_PARAMS;

/*
 * CTRL_NVSWITCH_GET_TIME_INFO
 *
 * Control to get the NVSwitch time information from inforom cache 
 *
 * Parameters:
 *    timeStart [OUT]
 *      The timestamp (EPOCH) when driver load onto the NVSwitch for the 1st time
 *    timeEnd [OUT]
 *      The timestamp (EPOCH) when the data was last flushed
 *    timeRun [OUT]
 *      The amount of time (in seconds) driver was loaded/running
 *    time24Hours [OUT]
 *      The timestamp (EPOCH) when the first 24 operational hours is hit
 *    time100Hours [OUT]
 *      The timestamp (EPOCH) when the first 100 operational hours is hit
 */

typedef struct
{
    NvU32 timeStart;
    NvU32 timeEnd;
    NvU32 timeRun;
    NvU32 time24Hours;
    NvU32 time100Hours;
} NVSWITCH_GET_TIME_INFO_PARAMS;

#define NVSWITCH_TEMP_DAY_ENTRIES               5
#define NVSWITCH_TEMP_WEEK_ENTRIES              5
#define NVSWITCH_TEMP_MNT_ENTRIES               5
#define NVSWITCH_TEMP_ALL_ENTRIES               5
#define NVSWITCH_TEMP_SUM_HOUR_ENTRIES          23
#define NVSWITCH_TEMP_SUM_DAY_ENTRIES           5
#define NVSWITCH_TEMP_SUM_MNT_ENTRIES           3
#define NVSWITCH_TEMP_HISTOGRAM_THLD_ENTRIES    20
#define NVSWITCH_TEMP_HISTOGRAM_TIME_ENTRIES    21
#define NVSWITCH_TEMP_HOURLY_MAX_ENTRIES        168

/*
 * NVSWITCH_TEMP_ENTRY
 *
 * This structure represents the NVSwitch TEMP with its timestamp.
 *
 *   value
 *     This parameter specifies the NVSwitch Temperature
 *     (SFXP 9.7 format in Celsius).
 *
 *   timestamp
 *     This parameter specifies the timestamp (EPOCH) of the entry.
 */
typedef struct
{
    NvU16 value;
    NvU32 timestamp;
} NVSWITCH_TEMP_ENTRY;

/*
 * CTRL_NVSWITCH_GET_TEMP_DATA
 *
 * Control to get the NVSwitch device historical temperature information from inforom cache 
 *
 * Parameters:
 *    tempMaxDayIdx [OUT]
 *      The current index to the maximum day temperature array
 *    tempMaxDay[] [OUT]
 *      The maximum temperature array for last NVSWITCH_TEMP_DAY_ENTRIES days
 *    tempMaxWeekIdx [OUT]
 *      The current index to the maximum week temperature array
 *    tempMaxWeek[] [OUT]
 *      The maximum temperature array for last NVSWITCH_TEMP_WEEK_ENTRIES weeks
 *    tempMaxMntIdx [OUT]
 *      The current index to the maximum month temperature array
 *    tempMaxMnt[] [OUT]
 *      The maximum temperature array for last NVSWITCH_TEMP_MNT_ENTRIES months
 *    tempMaxAllIdx [OUT]
 *      The current index to the maximum temperature array
 *    tempMaxAll[] [OUT]
 *      The maximum temperature array for the device 
 *    tempMinDayIdx [OUT]
 *      The current index to the minimum day temperature array
 *    tempMinDay[] [OUT]
 *      The minimum temperature array for last NVSWITCH_TEMP_DAY_ENTRIES days
 *    tempMinWeekIdx [OUT]
 *      The current index to the minimum week temperature array
 *    tempMinWeek[] [OUT]
 *      The minimum temperature array for last NVSWITCH_TEMP_WEEK_ENTRIES weeks
 *    tempMinMntIdx [OUT]
 *      The current index to the minimum month temperature array
 *    tempMinMnt[] [OUT]
 *      The minimum temperature array for last NVSWITCH_TEMP_MNT_ENTRIES months
 *    tempMinAllIdx [OUT]
 *      The current index to the minimum temperature array
 *    tempMinAll[] [OUT]
 *      The minimum temperature array for the device
 *    tempSumDelta [OUT]
 *      The total sum of temperature change in 0.1C granularity
 *    tempSumHour[] [OUT]
 *      The moving average of temperature per hour, for last NVSWITCH_TEMP_SUM_HOUR_ENTRIES hours
 *    tempSumDay[] [OUT]
 *      The moving average of temperature per day, for last NVSWITCH_TEMP_SUM_DAY_ENTRIES days
 *    tempSumMnt[] [OUT]
 *      The moving average of temperature per month, for last NVSWITCH_TEMP_SUM_MNT_ENTRIES months
 *    tempHistogramThld[] [OUT]
 *      The histogram of temperature crossing various thresholds (5/10/15/.../95/100)
 *    tempHistogramTime[] [OUT]
 *      The histogram of time was in various temperature ranges (0..5/5..10/.../100..)
 *    tempHourlyMaxSample[] [OUT]
 *      The maximum hourly temperature array for the device
 */

typedef struct
{
    NvU32               tempMaxDayIdx;
    NVSWITCH_TEMP_ENTRY tempMaxDay[NVSWITCH_TEMP_DAY_ENTRIES];
    NvU32               tempMaxWeekIdx;
    NVSWITCH_TEMP_ENTRY tempMaxWeek[NVSWITCH_TEMP_WEEK_ENTRIES];
    NvU32               tempMaxMntIdx;
    NVSWITCH_TEMP_ENTRY tempMaxMnt[NVSWITCH_TEMP_MNT_ENTRIES];
    NvU32               tempMaxAllIdx;
    NVSWITCH_TEMP_ENTRY tempMaxAll[NVSWITCH_TEMP_ALL_ENTRIES];
    NvU32               tempMinDayIdx;
    NVSWITCH_TEMP_ENTRY tempMinDay[NVSWITCH_TEMP_DAY_ENTRIES];
    NvU32               tempMinWeekIdx;
    NVSWITCH_TEMP_ENTRY tempMinWeek[NVSWITCH_TEMP_WEEK_ENTRIES];
    NvU32               tempMinMntIdx;
    NVSWITCH_TEMP_ENTRY tempMinMnt[NVSWITCH_TEMP_MNT_ENTRIES];
    NvU32               tempMinAllIdx;
    NVSWITCH_TEMP_ENTRY tempMinAll[NVSWITCH_TEMP_ALL_ENTRIES];
    NvU32               tempSumDelta;
    NvU32               tempSumHour[NVSWITCH_TEMP_SUM_HOUR_ENTRIES];
    NvU32               tempSumDay[NVSWITCH_TEMP_SUM_DAY_ENTRIES];
    NvU32               tempSumMnt[NVSWITCH_TEMP_SUM_MNT_ENTRIES];
    NvU32               tempHistogramThld[NVSWITCH_TEMP_HISTOGRAM_THLD_ENTRIES];
    NvU32               tempHistogramTime[NVSWITCH_TEMP_HISTOGRAM_TIME_ENTRIES];
    NVSWITCH_TEMP_ENTRY tempHourlyMaxSample[NVSWITCH_TEMP_HOURLY_MAX_ENTRIES];
} NVSWITCH_GET_TEMP_DATA_PARAMS;

#define NVSWITCH_TEMP_COMPRESS_BUFFER_ENTRIES   1096
#define NVSWITCH_NUM_COMPRESSION_PERIODS        8

/*
 * CTRL_NVSWITCH_GET_TEMP_DATA
 *
 * Control to get the NVSwitch device temperature information from inforom cache 
 *
 * Parameters:
 *    compressionPeriodIdx [OUT]
 *      The current index to the sample period array
 *    compressionPeriod[] [OUT]
 *      The samples period array (seconds)
 *    tempCompressionBuffer[] [OUT]
 *      The temperature array sampling at a specific period in compressionPeriod[]
 */

typedef struct
{
    NvU32               compressionPeriodIdx;
    NvU32               compressionPeriod[NVSWITCH_NUM_COMPRESSION_PERIODS];
    NVSWITCH_TEMP_ENTRY tempCompressionBuffer[NVSWITCH_TEMP_COMPRESS_BUFFER_ENTRIES];
} NVSWITCH_GET_TEMP_SAMPLES_PARAMS;

/*
 * CTRL_NVSWITCH_GET_FOM_VALUES
 *   This command gives the FOM values to MODS
 *
 *  [in] linkId
 *    Link number on which the FOM values are requested
 *  [out] numLanes
 *    This field specifies the no. of lanes per link
 *  [out] figureOfMetritValues
 *    This field contains the FOM values per lane
 */

typedef struct nvswitch_get_fom_values_params
{
    NvU32 linkId;
    NvU8  numLanes;
    NvU16 figureOfMeritValues[NVSWITCH_NVLINK_MAX_LANES];
} NVSWITCH_GET_FOM_VALUES_PARAMS;

/*
 * CTRL_NVSWITCH_SET_RESIDENCY_BINS
 *
 * Control for setting residency bins.
 *
 * Parameters:
 *  [in] table_select
 *      Which table to return.
 *  [in] NVSWITCH_RESIDENCY_BIN
 *     Residency thresholds. The thresholds would be only applied to the
 *     enabled ports.
 *     NVSWITCH_GET_INFO can be used to query enabled ports.
 */

typedef struct nvswitch_residency_bin
{
    NvU32   lowThreshold;       /* in nsec */
    NvU32   hiThreshold;        /* in nsec */

} NVSWITCH_RESIDENCY_THRESHOLDS;

#define NVSWITCH_TABLE_SELECT_MULTICAST     0
#define NVSWITCH_TABLE_SELECT_REDUCTION     1

typedef struct nvswitch_set_residency_bins
{
    NvU32 table_select;     // NVSWITCH_TABLE_SELECT_MULTICAST/_REDUCTION
    NVSWITCH_RESIDENCY_THRESHOLDS bin;

} NVSWITCH_SET_RESIDENCY_BINS;

/*
 * CTRL_NVSWITCH_GET_RESIDENCY_BINS
 *
 * Control for querying multicast & reduction residency histogram.
 *
 * Parameters:
 *  [in] linkId
 *    Link number on which the residency histogram is requested
 *  [in] table_select
 *      Which table to return.
 *
 *  [in] bin
 *     Residency thresholds.
 *  [out] residency
 *      Residency data/histogram format. The data will be available for the
 *      enabled/supported ports returned by NVSWITCH_GET_INFO.
 */

typedef struct nvswitch_residency_bins
{
    NV_DECLARE_ALIGNED(NvU64 low,    8);
    NV_DECLARE_ALIGNED(NvU64 medium, 8);
    NV_DECLARE_ALIGNED(NvU64 high,   8);
} NVSWITCH_RESIDENCY_BINS;

#define NVSWITCH_RESIDENCY_SIZE     128

typedef struct nvswitch_get_residency_bins
{
    NvU32 link;
    NvU32 table_select;     // NVSWITCH_TABLE_SELECT_MULTICAST/_REDUCTION
    NVSWITCH_RESIDENCY_THRESHOLDS bin;
    NVSWITCH_RESIDENCY_BINS residency[NVSWITCH_RESIDENCY_SIZE];
} NVSWITCH_GET_RESIDENCY_BINS;

/*
 * CTRL_NVSWITCH_GET_RB_STALL_BUSY
 *
 * Control for querying reduction buffer stall/busy counters.
 *
 * Parameters:
 *  [in] linkId
 *    Link number on which the stall/busy counters are requested
 *  [in] table_select
 *      Which table to return.
 *
 *  [out] stall_busy
 *      Reduction buffer stall/busy counters. The data will be available for the
 *      enabled/supported ports returned by NVSWITCH_GET_INFO.
 */

typedef struct nvswitch_stall_busy
{
    NV_DECLARE_ALIGNED(NvU64 time,  8); // in ns
    NV_DECLARE_ALIGNED(NvU64 stall, 8);
    NV_DECLARE_ALIGNED(NvU64 busy,  8);
} NVSWITCH_STALL_BUSY;

typedef struct nvswitch_get_rd_stall_busy
{
    NvU32 link;
    NvU32 table_select;         // NVSWITCH_TABLE_SELECT_MULTICAST/_REDUCTION
    NVSWITCH_STALL_BUSY vc0;
    NVSWITCH_STALL_BUSY vc1;
} NVSWITCH_GET_RB_STALL_BUSY;

/*
 * CTRL_NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR
 *
 * Control for querying multicast ID error vector
 *
 * Parameters:
 *  [in] link
 *    Link number on which the error vector is requested
 *
 *  [out] error_vector[]
 *      Bit vector of multicast IDs that are in error.
 */

#define NVSWITCH_MC_ID_ERROR_VECTOR_COUNT   128

typedef struct nvswitch_get_multicast_id_error_vector
{
    NvU32 link;
    NvU32 error_vector[NVSWITCH_MC_ID_ERROR_VECTOR_COUNT / 32];
} NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR;

/*
 * CTRL_NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR
 *
 * Control for clearing multicast ID error vector
 *
 * Parameters:
 *  [in] link
 *    Link number on which the error vector clear is requested
 *
 *  [in] error_vector[]
 *      Bit vector of multicast IDs to clear error.
 */

typedef struct nvswitch_clear_multicast_id_error_vector
{
    NvU32 link;
    NvU32 error_vector[NVSWITCH_MC_ID_ERROR_VECTOR_COUNT / 32];
} NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR;

/*
 * NVSWITCH_NVLINK_ERR_INFO
 *   Error information per link
 *
 * Parameters:
 *   TLErrlog
 *     Returns the error mask for NVLINK TL errors
 *     Used in Pascal
 *
 *   TLIntrEn
 *     Returns the intr enable mask for NVLINK TL errors
 *     Used in Pascal
 *
 *   TLCTxErrStatus0
 *     Returns the TLC Tx Error Mask 0
 *     Used in Volta
 *
 *   TLCRxErrStatus0
 *     Returns the TLC Rx Error Mask 0
 *     Used in Volta
 *
 *   TLCRxErrStatus1
 *     Returns the TLC Rx Error Mask 1
 *     Used in Volta
 *
 *   TLCTxErrLogEn0
 *     Returns the TLC Tx Error Log En 0
 *     Used in Volta
 *
 *   TLCRxErrLogEn0
 *     Returns the TLC Rx Error Log En 0
 *     Used in Volta
 *
 *   TLCRxErrLogEn1
 *     Returns the TLC Rx Error Log En 1
 *     Used in Volta
 *
 *   MIFTxErrStatus0
 *     Returns the MIF Rx Error Mask 0
 *     Used in Volta
 *
 *   MIFRxErrStatus0
 *     Returns the MIF Tx Error Mask 0
 *     Used in Volta
 *
 *   DLSpeedStatusTx
 *     Returns the NVLINK DL speed status for sublink Tx
 *
 *   DLSpeedStatusRx
 *     Returns the NVLINK DL speed status for sublink Rx
 *
 *   bExcessErrorDL
 *     Returns true for excessive error rate interrupt from DL
 */
typedef struct
{
    NvU32   TLErrlog;
    NvU32   TLIntrEn;
    NvU32   TLCTxErrStatus0;
    NvU32   TLCRxErrStatus0;
    NvU32   TLCRxErrStatus1;
    NvU32   TLCTxErrLogEn0;
    NvU32   TLCRxErrLogEn0;
    NvU32   TLCRxErrLogEn1;
    NvU32   MIFTxErrStatus0;
    NvU32   MIFRxErrStatus0;
    NvU32   DLSpeedStatusTx;
    NvU32   DLSpeedStatusRx;
    NvBool  bExcessErrorDL;
} NVSWITCH_NVLINK_ERR_INFO;

/*
 * CTRL_NVSWITCH_GET_ERR_INFO
 *     This command is used to query the NVLINK error information
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

/*
 *   NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS
 *
 *   linkMask
 *     Returns the mask of links enabled
 *
 *   linkErrInfo
 *     Returns the error information for all the links
 */
typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NVSWITCH_NVLINK_ERR_INFO linkErrInfo[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS;

#define NVSWITCH_INBAND_DATA_SIZE 5120

/*
 * CTRL_NVSWITCH_INBAND_SEND_DATA
 * 
 * Control call used for sending data over inband.
 *
 * Parameters:
 *
 *    dataSize[IN]
 *      Valid data in the buffer
 *
 *    linkId[IN]
 *      Link number on which the data needs to be sent
 *
 *    buffer[IN]
 *      Data which needs to be sent on the other side
 *
 *    dataSent [OUT]
 *      Bytes of data which were sent to the other side
 */
typedef struct nvswitch_inband_send_data_params
{
    /* input parameters */
    NvU32 dataSize;
    NvU32 linkId;
    NvU8  buffer[NVSWITCH_INBAND_DATA_SIZE];

    /* output parameters */
    NvU32 dataSent;
} NVSWITCH_INBAND_SEND_DATA_PARAMS;

/*
 * CTRL_NVSWITCH_INBAND_READ_DATA
 * 
 * Control call used for reading data received over inband
 *
 * Parameters:
 *
 *    linkId[IN]
 *      Link number on which the data needs to be read.
 *
 *    dataSize[OUT]
 *      Valid data in the buffer
 *
 *    buffer[OUT]
 *      Data which needs to be read from the other side
 */
typedef struct nvswitch_inband_read_data_params
{
    /* input parameters */
    NvU32 linkId;

    /* output parameters */
    NvU32 dataSize;
    NvU8  buffer[NVSWITCH_INBAND_DATA_SIZE];
} NVSWITCH_INBAND_READ_DATA_PARAMS;

/*
 * CTRL_NVSWITCH_INBAND_FLUSH_DATA
 * 
 * Flushing all the pending data for the corresponding link.
 * Messages would be stored in a queue. If flush is send all the
 * pending messages which are there for that linkId will be deleted.
 *
 * Parameters:
 *
 *    linkMask[IN]
 *      Mask of Links on which the data needs to be flushed.
 */
typedef struct nvswitch_inband_flush_data_params
{
    /* input parameters */
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);

    /* output parameters */
} NVSWITCH_INBAND_FLUSH_DATA_PARAMS;

/*
 * CTRL_NVSWITCH_INBAND_PENDING_DATA_STATS
 * 
 * Control call to check which links have pending data
 *
 * Parameters:
 *
 *    linkMask[OUT]
 *      Mask of the links which has data on it.
 */
typedef struct nvswitch_inband_pending_data_stats_params
{
    /* output parameters */
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
} NVSWITCH_INBAND_PENDING_DATA_STATS_PARAMS;

/*
 * CTRL_NVSWITCH_GET_BOARD_PART_NUMBER
 *
 * Control for querying the board part number
 *
 * Parameters:
 *  [out] data[]
 *      Byte vector of the board part number.
 */

#define NVSWITCH_BOARD_PART_NUMBER_SIZE_IN_BYTES        20

typedef struct nvswitch_get_board_part_number_vector
{
    NvU8 data[NVSWITCH_BOARD_PART_NUMBER_SIZE_IN_BYTES];
} NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR;

#define NVSWITCH_GET_SW_INFO_COUNT_MAX 32

typedef enum nvswitch_get_sw_info_index
{
    NVSWITCH_GET_SW_INFO_INDEX_INFOROM_NVL_SUPPORTED = 0x0,
    NVSWITCH_GET_SW_INFO_INDEX_INFOROM_BBX_SUPPORTED
} NVSWITCH_GET_SW_INFO_INDEX;

typedef struct nvswitch_get_sw_info_params
{
    NvU32 count;
    NvU32 index[NVSWITCH_GET_SW_INFO_COUNT_MAX];
    NvU32 info[NVSWITCH_GET_SW_INFO_COUNT_MAX];
} NVSWITCH_GET_SW_INFO_PARAMS;

/*
 * CTRL_NVSWITCH_CLEAR_COUNTERS
 *  This command clears/resets the counters for the specified types.
 *
 * [in] linkMask
 *  This parameter specifies for which links we want to clear the 
 *  counters.
 *
 * [in] counterMask
 *  This parameter specifies the input mask for desired counters to be
 *  cleared. Note that all counters cannot be cleared.
 *
 *  NOTE: Bug# 2098529: On Turing all DL errors and LP counters are cleared
 *        together. They cannot be cleared individually per error type. RM
 *        would possibly move to a new API on Ampere and beyond
 */  

typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NvU32  counterMask; 
} NVSWITCH_NVLINK_CLEAR_COUNTERS_PARAMS;

/*
 * NVSWITCH_CTRL_I2C_DEVICE_INFO
 *
 * This structure describes the basic I2C Device information.
 *
 *   type
 *     This field return the type of device NVSWITCH_I2C_DEVICE_<xyz>
 *   i2cAddress
 *     This field contains the 7 bit/10 bit address of the I2C device.
 *   i2cLogicalPort
 *     This field contains the Logical port of the I2C device.
 */

typedef enum
{
    NVSWITCH_I2C_PORT_I2CA      = 0,
    NVSWITCH_I2C_PORT_I2CB,
    NVSWITCH_I2C_PORT_I2CC,
    NVSWITCH_I2C_PORT_I2CD
} NVSWITCH_I2C_PORT_TYPE;

typedef enum
{
    NVSWITCH_I2C_DEVICE_UNKNOWN             = 0,

    // OSFP Devices
    NVSWITCH_I2C_DEVICE_CMIS4_MODULE       = 0xB0,
    NVSWITCH_I2C_DEVICE_CMIS4_MUX_PCA9847  = 0xB1,

    NVSWITCH_I2C_DEVICE_SKIP                = 0xFF

} NVSWITCH_I2C_DEVICE_TYPE;

typedef struct
{
    NVSWITCH_I2C_DEVICE_TYPE  type;
    NvU32  i2cAddress;
    NVSWITCH_I2C_PORT_TYPE  i2cPortLogical;
} NVSWITCH_CTRL_I2C_DEVICE_INFO;

/* Maximum number of I2C devices in DCB */
#define NVSWITCH_CTRL_I2C_MAX_DEVICES             32

/*
 * CTRL_NVSWITCH_I2C_TABLE_GET_DEV_INFO
 *
 * RM Control to get I2C device info from the DCB I2C Devices Table.
 *
 *   i2cDevCount
 *     The value of this parameter will give the number of valid
 *     I2C devices returned in structure.
 *
 *   i2cDevInfo[]
 *     For each device the control call will report the device info
 *
 */
typedef struct
{
    NvU8   i2cDevCount;
    NVSWITCH_CTRL_I2C_DEVICE_INFO i2cDevInfo[NVSWITCH_CTRL_I2C_MAX_DEVICES];
} NVSWITCH_CTRL_I2C_GET_DEV_INFO_PARAMS;

//! Maximum size of index.
#define NVSWITCH_CTRL_I2C_INDEX_LENGTH_MAX                      4

/*! Set if the command should begin with a START.  For a transactional
 *  interface (highly recommended), this should always be _SEND.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_START                          0:0
#define NVSWITCH_CTRL_I2C_FLAGS_START_NONE                       0
#define NVSWITCH_CTRL_I2C_FLAGS_START_SEND                       1

/*!
 *  Indicate whether to send a repeated start between the index and
 *  message phrases.
 *
 *  This flag will send a restart between each index and message.  This should
 *  be set for reads, but rarely (if ever) for writes.
 *
 *  A RESTART is required when switching directions; this is called a combined
 *  format.  These are typically used in indexed read commands, where an index
 *  is written to the device to indicate what register(s) to read, and then
 *  the register is read.  Almost always, indexed writes do not require a
 *  restart, though some devices will accept them.  However, this flag should
 *  be used for writes in the rare case where a restart should be sent between
 *  the last index and the message.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_RESTART                        1:1
#define NVSWITCH_CTRL_I2C_FLAGS_RESTART_NONE                     0
#define NVSWITCH_CTRL_I2C_FLAGS_RESTART_SEND                     1

/*! Set if the command should conclude with a STOP.  For a transactional
 *  interface (highly recommended), this should always be _SEND.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_STOP                           2:2
#define NVSWITCH_CTRL_I2C_FLAGS_STOP_NONE                        0
#define NVSWITCH_CTRL_I2C_FLAGS_STOP_SEND                        1

/*! The slave addressing mode: 7-bit (most common) or 10-bit.  It is possible
 *  but not recommended to send no address at all using _NONE.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_ADDRESS_MODE                   4:3
#define NVSWITCH_CTRL_I2C_FLAGS_ADDRESS_MODE_NO_ADDRESS          0
#define NVSWITCH_CTRL_I2C_FLAGS_ADDRESS_MODE_7BIT                1
#define NVSWITCH_CTRL_I2C_FLAGS_ADDRESS_MODE_10BIT               2

//! The length of the index.  If length is 0, no index will be sent.
#define NVSWITCH_CTRL_I2C_FLAGS_INDEX_LENGTH                   7:5
#define NVSWITCH_CTRL_I2C_FLAGS_INDEX_LENGTH_ZERO                0
#define NVSWITCH_CTRL_I2C_FLAGS_INDEX_LENGTH_ONE                 1
#define NVSWITCH_CTRL_I2C_FLAGS_INDEX_LENGTH_TWO                 2
#define NVSWITCH_CTRL_I2C_FLAGS_INDEX_LENGTH_THREE               3
#define NVSWITCH_CTRL_I2C_FLAGS_INDEX_LENGTH_MAXIMUM             NVSWITCH_CTRL_I2C_INDEX_LENGTH_MAX

/*! The flavor to use: software bit-bang or hardware controller.  The hardware
 *  controller is faster, but is not necessarily available or capable.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_FLAVOR                         8:8
#define NVSWITCH_CTRL_I2C_FLAGS_FLAVOR_HW                        0
#define NVSWITCH_CTRL_I2C_FLAGS_FLAVOR_SW                        1

/*! The target speed at which to drive the transaction at.
 *
 *  Note: The lib reserves the right to lower the speed mode if the I2C master
 *  implementation cannot handle the speed given.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE                    11:9
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_DEFAULT      0x00000000
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_100KHZ       0x00000003
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_200KHZ       0x00000004
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_300KHZ       0x00000005
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_400KHZ       0x00000006
#define NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_1000KHZ      0x00000007

/*
 * NVSWITCH_CTRL_I2C_FLAGS_TRANSACTION_MODE
 *   A client uses this field to specify a transaction mode.
 *   Possible values are:
 *     NVSWITCH_CTRL_I2C_FLAGS_TRANSACTION_MODE_NORMAL
 *       The default, this value indicates to use the normal I2C transaction
 *       mode which will involve read/write operations depending on client's
 *       needs.
 *     NVSWITCH_CTRL_I2C_FLAGS_TRANSACTION_MODE_PING
 *       This value specifies that the device only needs to be pinged. No need
 *       of performing a complete read/write transaction. This will address
 *       the device to be pinged but not send any data. On receiving an ACK,
 *       we will get a confirmation on the device's availability.
 *       PING requires that:
 *          _START   = _SEND
 *          _RESTART = _NONE
 *          _STOP    = _SEND
 *          _ADDRESS_MODE != _NO_ADDRESS
 *          _INDEX_LENGTH = _ZERO
 *          messageLength = 0
 */
#define NVSWITCH_CTRL_I2C_FLAGS_TRANSACTION_MODE                          12:12
#define NVSWITCH_CTRL_I2C_FLAGS_TRANSACTION_MODE_NORMAL             (0x00000000)
#define NVSWITCH_CTRL_I2C_FLAGS_TRANSACTION_MODE_PING               (0x00000001)

/*!
 * Block Reads/Writes: There are two different protocols for reading/writing >2
 * byte sets of data to/from a slave device.  The SMBus specification section
 * 5.5.7 defines "Block Reads/Writes" in which the first byte of the payload
 * specifies the size of the data to be read/written s.t. payload_size =
 * data_size + 1.  However, many other devices depend on the master to already
 * know the size of the data being accessed (i.e. SW written with knowledge of
 * the device's I2C register spec) and skip this overhead.  This second behavior
 * is actually the default behavior of all the lib's I2C interfaces.
 *
 * Setting this bit will enable the block protocol for reads and writes for size
 * >2.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_BLOCK_PROTOCOL               17:17
#define NVSWITCH_CTRL_I2C_FLAGS_BLOCK_PROTOCOL_DISABLED 0x00000000
#define NVSWITCH_CTRL_I2C_FLAGS_BLOCK_PROTOCOL_ENABLED  0x00000001

/*!
 * NVSWITCH_CTRL_I2C_FLAGS_RESERVED
 *   A client must leave this field as 0, as it is reserved for future use.
 */
#define NVSWITCH_CTRL_I2C_FLAGS_RESERVED                    31:18

#define NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX                256

typedef enum
{
    NVSWITCH_I2C_ACQUIRER_NONE = 0,
    NVSWITCH_I2C_ACQUIRER_UNKNOWN,
    NVSWITCH_I2C_ACQUIRER_IOCTL,          // e.g. MODS                  
    NVSWITCH_I2C_ACQUIRER_EXTERNAL,       // e.g. Linux Direct
    NVSWITCH_I2C_ACQUIRER_CCI_INITIALIZE, // CCI Init/Startup
    NVSWITCH_I2C_ACQUIRER_CCI_TRAIN,      // Cable training
    NVSWITCH_I2C_ACQUIRER_CCI_UX,         // User interface e.g. LEDs
    NVSWITCH_I2C_ACQUIRER_CCI_SERVICE,    // e.g. ISR
    NVSWITCH_I2C_ACQUIRER_CCI_SMBPBI,     // OOB path

} NVSWITCH_I2C_ACQUIRER;

/*
 * CTRL_NVSWITCH_I2C_INDEXED
 *
 * Perform a basic I2C transaction synchronously.
 *
 *   portId
 *     This field must be specified by the client to indicate the logical
 *     port/bus for which the transaction is requested.
 *
 *   bIsRead
 *     This field must be specified by the client to indicate whether the
 *     command is a write (FALSE) or a read (TRUE).
 *
 *   flags
 *     This parameter specifies optional flags used to control certain modal
 *     features such as target speed and addressing mode.  The currently
 *     defined fields are described previously; see NVSWITCH_CTRL_I2C_FLAGS_*.
 *
 *   acquirer
 *     The ID of the client that is trying to take control of the I2C module.
 *
 *   address
 *     The address of the I2C slave.  The address should be shifted left by
 *     one.  For example, the I2C address 0x50, often used for reading EDIDs,
 *     would be stored here as 0xA0.  This matches the position within the
 *     byte sent by the master, as the last bit is reserved to specify the
 *     read or write direction.
 *
 *   index
 *     This parameter, required of the client if index is one or more,
 *     specifies the index to be written.  The buffer should be arranged such
 *     that index[0] will be the first byte sent.
 *
 *   messageLength
 *     This parameter, required of the client, specifies the number of bytes to
 *     read or write from the slave after the index is written.
 *
 *   message
 *     This parameter, required of the client, specifies the data to be written
 *     to the slave.  The buffer should be arranged such that message[0] will
 *     be the first byte read or written.  If the transaction is a read, then
 *     it will follow the combined format described in the I2C specification.
 *     If the transaction is a write, the message will immediately follow the
 *     index without a restart.
 *
 */
typedef struct
{
    NvU8  port;
    NvU8  bIsRead;
    NvU16 address;
    NvU32 flags;
    NvU32 acquirer;

    NvU8 index[NVSWITCH_CTRL_I2C_INDEX_LENGTH_MAX];

    NvU32 messageLength;
    NvU8  message[NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX];
} NVSWITCH_CTRL_I2C_INDEXED_PARAMS;

/*
 * Structure to store register values required to debug ALI training failures
 *
 * dlstatMn00
 *     DLSTAT MN00 register value (subcode and code)
 * dlstatUc01
 *     DLSTAT UC01 register value
 * dlstatLinkIntr
 *     NV_MINION_NVLINK_LINK_INTR (subcode, code and state)
 */
typedef struct nvswitch_minion_ali_debug_registers
{
    NvU32 dlstatMn00;
    NvU32 dlstatUc01;
    NvU32 dlstatLinkIntr;
} NVSWITCH_MINION_ALI_DEBUG_REGISTERS;

/*
 * CTRL_NVSWITCH_CCI_GET_PORTS_CPLD_INFO
 *
 * Retrieve information about the Ports CPLD
 * 
 * Parameters:
 *
 *  versionMajor[OUT]
 *      Major number of CPLD version
 *  versionMinor[OUT]
 *      Minor number of CPLD version    
 */
typedef struct nvswitch_cci_get_ports_cpld_info_params
{
    NvU8 versionMajor;
    NvU8 versionMinor;
} NVSWITCH_CCI_GET_PORTS_CPLD_INFO_PARAMS;

#define NVSWITCH_CCI_FW_FLAGS_PRESENT        0:0
#define NVSWITCH_CCI_FW_FLAGS_PRESENT_NO       0
#define NVSWITCH_CCI_FW_FLAGS_PRESENT_YES      1
#define NVSWITCH_CCI_FW_FLAGS_ACTIVE         1:1
#define NVSWITCH_CCI_FW_FLAGS_ACTIVE_NO        0
#define NVSWITCH_CCI_FW_FLAGS_ACTIVE_YES       1
#define NVSWITCH_CCI_FW_FLAGS_COMMITED       2:2
#define NVSWITCH_CCI_FW_FLAGS_COMMITED_NO      0
#define NVSWITCH_CCI_FW_FLAGS_COMMITED_YES     1
#define NVSWITCH_CCI_FW_FLAGS_EMPTY          3:3
#define NVSWITCH_CCI_FW_FLAGS_EMPTY_NO         0
#define NVSWITCH_CCI_FW_FLAGS_EMPTY_YES        1

#define NVSWITCH_CCI_FW_IMAGE_A         0x0
#define NVSWITCH_CCI_FW_IMAGE_B         0x1
#define NVSWITCH_CCI_FW_IMAGE_FACTORY   0x2
#define NVSWITCH_CCI_FW_IMAGE_COUNT     0x3

/*
 * Structure to store FW revision parameters
 *
 * Parameters:
 *   status
 *     FW status flags
 *   image
 *     Firmware Image A/B/Factory.
 *   major
 *     FW major revision.
 *   minor
 *     FW minor revision.
 *   build
 *     FW build number.
 */
typedef struct nvswitch_cci_get_fw_revisions
{
    NvU8 flags;
    NvU8 major;
    NvU8 minor;
    NvU16 build;
} NVSWITCH_CCI_GET_FW_REVISIONS;

/*
 * CTRL_NVSWITCH_CCI_GET_FW_REVISIONS
 *
 * Control to get cci firmware revisions of the transreciever.
 *
 * This API is not supported on SV10.
 *
 * Parameters:
 *   link [IN]
 *     Link number
 *   revisions [OUT]
 *     Stores the CCI FW revision params
 */
typedef struct nvswitch_cci_get_fw_revision_params
{
    NvU32 linkId;
    NVSWITCH_CCI_GET_FW_REVISIONS revisions[NVSWITCH_CCI_FW_IMAGE_COUNT];
} NVSWITCH_CCI_GET_FW_REVISION_PARAMS;

/*
 * CTRL_NVSWITCH_CCI_SET_LOCATE_LED
 *
 * Control to turn on/off the LOCATE LED on a module cage.
 * This command will override the current LED state. 
 *
 * Parameters:
 *   cageIndex [IN]
 *      Target cage index (>=0 and <= 31) on the 
 *      selected ASIC device.
 *   portNum [IN]
 *      Target port (0 or 1) on the seleted
 *      cage.
 *   bSetLocateOn [IN]
 *      Turn on/off LED. NV_TRUE == ON, NV_FALSE == OFF
 */
typedef struct nvswitch_cci_set_locate_led_params
{
    NvU8 cageIndex;
    NvU8 portNum;
    NvBool bSetLocateOn;
} NVSWITCH_CCI_SET_LOCATE_LED_PARAMS;

/*
 * CTRL_NVSWITCH_GET_SOE_HEARTBEAT
 *
 * Retrieve SOE Heartbeat status
 * 
 * Parameters:
 *
 *  timestampNs[OUT]
 *      PTIMER timestamp of the SOE Heartbeat GPIO reading
 *  gpioVal[OUT]
 *      Current SOE Heartbeat GPIO value
 */
typedef struct nvswitch_get_soe_heartbeat_params
{
    NvU64 timestampNs;
    NvU32 gpioVal;
} NVSWITCH_GET_SOE_HEARTBEAT_PARAMS;

/*
 * CTRL_NVSWITCH_SET_CONTINUOUS_ALI
 *
 * Enable/disable continuous ALI for all CCI managed links.
 * This also enables/disables hot plug. 
 *
 * Continuous ALI is enabled by default.
 *
 * Parameters:
 *    bEnable [IN]
 */

typedef struct 
{
    NvBool  bEnable;
} NVSWITCH_SET_CONTINUOUS_ALI_PARAMS;

/*
 * CTRL_NVSWITCH_REQUEST_ALI
 *
 * Request link training for links specified in the mask.
 *  NVswitch must be in non-continuous ALI mode. Reset
 *  and drain will always be performed along with ALI. ALI 
 *  requests for links that are currently being trained will be 
 *  dropped. This is an asynchronous request.
 *
 * Parameters:
 *    linkMaskTrain [IN]
 */

typedef struct 
{
    // TODO: not used, remove later
    NvU64  linkMaskForcedResetAndDrain;
    NvU64  linkMaskTrain;
} NVSWITCH_REQUEST_ALI_PARAMS;

/*
 * CTRL_NVSWITCH_REGISTER_READ/WRITE
 *
 * This provides direct access to the MMIO space.
 */

typedef struct
{
    NvU32   engine;     // REGISTER_RW_ENGINE_*
    NvU32   instance;   // engine instance
    NvU32   offset;     // Register offset within device/instance
    NvU32   val;        // out: register value read
} NVSWITCH_REGISTER_READ;

typedef struct
{
    NvU32   engine;     // REGISTER_RW_ENGINE_*
    NvU32   instance;   // engine instance
    NvBool  bcast;      // Unicast or broadcast
    NvU32   offset;     // Register offset within engine/instance
    NvU32   val;        // in: register value to write
} NVSWITCH_REGISTER_WRITE;


typedef struct
{
    NvU8 thresholdMan;
    NvU8 thresholdExp;
    NvU8 timescaleMan;
    NvU8 timescaleExp;
    NvBool bInterruptEn;
    NvBool bInterruptTrigerred;
    NvU32 flags;
} NVSWITCH_NVLINK_ERROR_THRESHOLD_VALUES;

#define NVSWITCH_NVLINK_ERROR_THRESHOLD_RESET   0x1

/*
 * CTRL_NVSWITCH_SET_NVLINK_ERROR_THRESHOLD
 *
 * Set the Nvlink Error Rate Threshold.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link mask for which we need to set the Error Threshold
 *
 *    errorThreshold [IN]
 *      Threshold values, interrupt enable/disable and flags
 */

typedef struct 
{
    NV_DECLARE_ALIGNED(NvU64 link_mask, 8);
    NVSWITCH_NVLINK_ERROR_THRESHOLD_VALUES errorThreshold[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_SET_NVLINK_ERROR_THRESHOLD_PARAMS;

/*
 * CTRL_NVSWITCH_GET_NVLINK_ERROR_THRESHOLD
 *
 * Control to query NVLIPT counter configuration.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link mask for which we need to get the Error Threshold
 *
 *    errorThreshold [OUT]
 *      Threshold values, interrupt enable/disable and flags
 */

typedef struct 
{
    NV_DECLARE_ALIGNED(NvU64 link_mask, 8);
    NVSWITCH_NVLINK_ERROR_THRESHOLD_VALUES errorThreshold[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_GET_NVLINK_ERROR_THRESHOLD_PARAMS;

/*
 * CTRL_NVSWITCH_GET_NVLINK_L1_CAPABILITY
 *
 * Control to query NvLink L1 Threshold capability.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link mask for which we need to get the L1 Threshold
 *
 *    l1Capable [OUT]
 *      An array of links that are capable of supporting L1 Thresholds
 */
typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NvBool l1Capable[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_GET_NVLINK_L1_CAPABILITY_PARAMS;

/*
 * CTRL_NVSWITCH_GET_NVLINK_L1_THRESHOLD
 *
 * Control to query NvLink L1 Thresholds.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link mask for which we need to get the L1 Threshold
 *
 *    l1Threshold [OUT]
 *      L1 Threshold values in units of 100us
 */
typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NvU32 l1Threshold[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_GET_NVLINK_L1_THRESHOLD_PARAMS;

#define NVSWITCH_SET_NVLINK_L1_THRESHOLD_MIN     0x1
#define NVSWITCH_SET_NVLINK_L1_THRESHOLD_MAX     0x1fff
#define NVSWITCH_SET_NVLINK_L1_THRESHOLD_DEFAULT 0xffffffff

/*
 * CTRL_NVSWITCH_SET_NVLINK_L1_THRESHOLD
 *
 * Control to set NvLink L1 Thresholds.
 *
 * Parameters:
 *    linkMask [IN]
 *      A valid link mask for which we need to get the L1 Threshold
 *
 *    l1Threshold [IN]
 *      L1 Threshold values in units of 100us
 */
typedef struct
{
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NvU32 l1Threshold[NVSWITCH_NVLINK_MAX_LINKS];
} NVSWITCH_SET_NVLINK_L1_THRESHOLD_PARAMS;

/*
 * CTRL_NVSWITCH_FSPRPC_GET_CAPS
 *
 * Control to query FSP capabilities
 *
 * Parameters:
 *    commandNvdmType [OUT]
 *      NVDM type of the command RPC
 *    responseNvdmType [OUT]
 *      NVDM type of the RPC response
 *    errorCode [OUT]
 *      Error code of the RPC
 *    pRspPayload [OUT]
 *      Payload of the response
 */
typedef struct
{
    NvU32 commandNvdmType;
    NvU32 responseNvdmType;
    NvU32 errorCode;
} NVSWITCH_FSPRPC_GET_CAPS_PARAMS;

typedef enum nvswitch_device_tnvl_mode
{
    NVSWITCH_DEVICE_TNVL_MODE_DISABLED = 0,       // TNVL mode is disabled
    NVSWITCH_DEVICE_TNVL_MODE_ENABLED,            // TNVL mode is enabled 
    NVSWITCH_DEVICE_TNVL_MODE_FAILURE,            // TNVL mode is enabled but in failure state
    NVSWITCH_DEVICE_TNVL_MODE_LOCKED,             // TNVL mode is enabled and locked
    NVSWITCH_DEVICE_TNVL_MODE_COUNT
} NVSWITCH_DEVICE_TNVL_MODE;

/*
 * CTRL_NVSWITCH_SET_DEVICE_TNVL_LOCK
 *
 * Control to set Trusted NVLink(TNVL) lock
 *
 * FM sets the TNVL lock after Fabric State is CONFIGURED
 *
 * Parameters:
 *    tnvlStatus [OUT]
 *      TNVL mode status of the device
 */
typedef struct nvswitch_set_device_tnvl_lock_params
{
    NVSWITCH_DEVICE_TNVL_MODE tnvlStatus;
} NVSWITCH_SET_DEVICE_TNVL_LOCK_PARAMS;

/*
 * CTRL_NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN
 *   
 * Control to query NvSwitch session attestation certificate chain
 *
 * Parameters:
 *
 *    attestationCertChain: [OUT]
 *      Attestation certificate chain for the NvSwitch queried
 *
 *    attestationCertChainSize: [OUT]
 *      Actual size of attestation cert chain data
 */

#define NVSWITCH_ATTESTATION_CERT_CHAIN_MAX_SIZE 0x1400

typedef struct nvswitch_get_attestation_certificate_chain_params
{
    NvU8     attestationCertChain[NVSWITCH_ATTESTATION_CERT_CHAIN_MAX_SIZE];
    NvU32    attestationCertChainSize;
} NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS;

/*
 * CTRL_NVSWITCH_GET_ATTESTATION_REPORT
 *
 * Control to query NvSwitch attestation report.
 *
 * Parameters:
 *    nonce: [IN]
 *        nonce
 *    attestationReport: [OUT]
 *        Attestation report of the NvSwitch queried
 *    attestationReportSize: [OUT]
 *        Actual size of the report
 */

#define NVSWITCH_NONCE_SIZE                     0x20
#define NVSWITCH_ATTESTATION_REPORT_MAX_SIZE    0x2000

typedef struct nvswitch_get_attestation_report_params
{
    NvU8     nonce[NVSWITCH_NONCE_SIZE];
    NvU8     attestationReport[NVSWITCH_ATTESTATION_REPORT_MAX_SIZE];
    NvU32    attestationReportSize;
} NVSWITCH_GET_ATTESTATION_REPORT_PARAMS;

/* 
 * CTRL_NVSWITCH_GET_TNVL_STATUS
 *
 * Control to query Trusted NVLink(TNVL) status
 *
 * Parameters :
 *    status: [OUT]
 *        TNVL mode status
 */
typedef struct nvswitch_get_tnvl_status_params
{
    NVSWITCH_DEVICE_TNVL_MODE status;
} NVSWITCH_GET_TNVL_STATUS_PARAMS;

#define REGISTER_RW_ENGINE_RAW                       0x00

#define REGISTER_RW_ENGINE_CLKS                      0x10
#define REGISTER_RW_ENGINE_FUSE                      0x11
#define REGISTER_RW_ENGINE_JTAG                      0x12
#define REGISTER_RW_ENGINE_PMGR                      0x13
#define REGISTER_RW_ENGINE_SAW                       0x14
#define REGISTER_RW_ENGINE_XP3G                      0x15
#define REGISTER_RW_ENGINE_XVE                       0x16
#define REGISTER_RW_ENGINE_SOE                       0x17
#define REGISTER_RW_ENGINE_SMR                       0x18
#define REGISTER_RW_ENGINE_SE                        0x19
#define REGISTER_RW_ENGINE_CLKS_SYS                  0x1A
#define REGISTER_RW_ENGINE_CLKS_SYSB                 0x1B
#define REGISTER_RW_ENGINE_CLKS_P0                   0x1C
#define REGISTER_RW_ENGINE_XPL                       0x1D
#define REGISTER_RW_ENGINE_XTL                       0x1E

#define REGISTER_RW_ENGINE_SIOCTRL                   0x20
#define REGISTER_RW_ENGINE_MINION                    0x21
#define REGISTER_RW_ENGINE_NVLIPT                    0x22
#define REGISTER_RW_ENGINE_NVLTLC                    0x23
#define REGISTER_RW_ENGINE_NVLTLC_MULTICAST          0x24
#define REGISTER_RW_ENGINE_DLPL                      0x25
#define REGISTER_RW_ENGINE_NVLW                      0x26
#define REGISTER_RW_ENGINE_NVLIPT_LNK                0x27
#define REGISTER_RW_ENGINE_NVLIPT_LNK_MULTICAST      0x28
#define REGISTER_RW_ENGINE_NVLDL                     0x29
#define REGISTER_RW_ENGINE_NVLDL_MULTICAST           0x2a
#define REGISTER_RW_ENGINE_PLL                       0x2b

#define REGISTER_RW_ENGINE_NPG                       0x30
#define REGISTER_RW_ENGINE_NPORT                     0x31
#define REGISTER_RW_ENGINE_NPORT_MULTICAST           0x32

#define REGISTER_RW_ENGINE_SWX                       0x40
#define REGISTER_RW_ENGINE_AFS                       0x41
#define REGISTER_RW_ENGINE_NXBAR                     0x42
#define REGISTER_RW_ENGINE_TILE                      0x43
#define REGISTER_RW_ENGINE_TILE_MULTICAST            0x44
#define REGISTER_RW_ENGINE_TILEOUT                   0x45
#define REGISTER_RW_ENGINE_TILEOUT_MULTICAST         0x46

/*
 * CTRL call command list.
 *
 * Linux driver supports only 8-bit commands.
 *
 * See struct control call command  modification guidelines at the top
 * of this file.
 */
#define CTRL_NVSWITCH_GET_INFO                              0x01
#define CTRL_NVSWITCH_SET_SWITCH_PORT_CONFIG                0x02
#define CTRL_NVSWITCH_SET_INGRESS_REQUEST_TABLE             0x03
#define CTRL_NVSWITCH_SET_INGRESS_REQUEST_VALID             0x04
#define CTRL_NVSWITCH_SET_INGRESS_RESPONSE_TABLE            0x05
#define CTRL_NVSWITCH_SET_GANGED_LINK_TABLE                 0x06
#define CTRL_NVSWITCH_GET_INTERNAL_LATENCY                  0x07
#define CTRL_NVSWITCH_SET_LATENCY_BINS                      0x08
#define CTRL_NVSWITCH_GET_NVLIPT_COUNTERS                   0x09
#define CTRL_NVSWITCH_SET_NVLIPT_COUNTER_CONFIG             0x0A
#define CTRL_NVSWITCH_GET_NVLIPT_COUNTER_CONFIG             0x0B
#define CTRL_NVSWITCH_GET_ERRORS                            0x0C
#define CTRL_NVSWITCH_SET_REMAP_POLICY                      0x0D
#define CTRL_NVSWITCH_SET_ROUTING_ID                        0x0E
#define CTRL_NVSWITCH_SET_ROUTING_LAN                       0x0F
#define CTRL_NVSWITCH_GET_INGRESS_REQUEST_TABLE             0x10
#define CTRL_NVSWITCH_GET_INGRESS_RESPONSE_TABLE            0x11
#define CTRL_NVSWITCH_GET_INGRESS_REQLINKID                 0x12
#define CTRL_NVSWITCH_UNREGISTER_LINK                       0x13
#define CTRL_NVSWITCH_RESET_AND_DRAIN_LINKS                 0x14
#define CTRL_NVSWITCH_GET_ROUTING_LAN                       0x15
#define CTRL_NVSWITCH_SET_ROUTING_LAN_VALID                 0x16
#define CTRL_NVSWITCH_GET_NVLINK_STATUS                     0x17
#define CTRL_NVSWITCH_ACQUIRE_CAPABILITY                    0x18
#define CTRL_NVSWITCH_GET_ROUTING_ID                        0x19
#define CTRL_NVSWITCH_SET_ROUTING_ID_VALID                  0x1A
#define CTRL_NVSWITCH_GET_TEMPERATURE                       0x1B
#define CTRL_NVSWITCH_GET_REMAP_POLICY                      0x1C
#define CTRL_NVSWITCH_SET_REMAP_POLICY_VALID                0x1D
#define CTRL_NVSWITCH_GET_THROUGHPUT_COUNTERS               0x1E
#define CTRL_NVSWITCH_GET_BIOS_INFO                         0x1F
#define CTRL_NVSWITCH_BLACKLIST_DEVICE                      0x20
#define CTRL_NVSWITCH_SET_FM_DRIVER_STATE                   0x21
#define CTRL_NVSWITCH_SET_DEVICE_FABRIC_STATE               0x22
#define CTRL_NVSWITCH_SET_FM_HEARTBEAT_TIMEOUT              0x23
#define CTRL_NVSWITCH_REGISTER_EVENTS                       0x24
#define CTRL_NVSWITCH_UNREGISTER_EVENTS                     0x25
#define CTRL_NVSWITCH_SET_TRAINING_ERROR_INFO               0x26
#define CTRL_NVSWITCH_GET_FATAL_ERROR_SCOPE                 0x27
#define CTRL_NVSWITCH_SET_MC_RID_TABLE                      0x28
#define CTRL_NVSWITCH_GET_MC_RID_TABLE                      0x29
#define CTRL_NVSWITCH_GET_COUNTERS                          0x2A
#define CTRL_NVSWITCH_GET_NVLINK_ECC_ERRORS                 0x2B
#define CTRL_NVSWITCH_I2C_SMBUS_COMMAND                     0x2C
#define CTRL_NVSWITCH_GET_TEMPERATURE_LIMIT                 0x2D
#define CTRL_NVSWITCH_GET_NVLINK_MAX_ERROR_RATES            0x2E
#define CTRL_NVSWITCH_GET_NVLINK_ERROR_COUNTS               0x2F
#define CTRL_NVSWITCH_GET_ECC_ERROR_COUNTS                  0x30
#define CTRL_NVSWITCH_GET_SXIDS                             0x31
#define CTRL_NVSWITCH_GET_FOM_VALUES                        0x32
#define CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS                0x33
#define CTRL_NVSWITCH_SET_RESIDENCY_BINS                    0x34
#define CTRL_NVSWITCH_GET_RESIDENCY_BINS                    0x35
#define CTRL_NVSWITCH_GET_RB_STALL_BUSY                     0x36
#define CTRL_NVSWITCH_CCI_CMIS_PRESENCE                     0x37
#define CTRL_NVSWITCH_CCI_CMIS_NVLINK_MAPPING               0x38
#define CTRL_NVSWITCH_CCI_CMIS_MEMORY_ACCESS_READ           0x39
#define CTRL_NVSWITCH_CCI_CMIS_MEMORY_ACCESS_WRITE          0x3A
#define CTRL_NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING           0x3B
#define CTRL_NVSWITCH_CCI_GET_GRADING_VALUES                0x3C
#define CTRL_NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR         0x3D
#define CTRL_NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR       0x3E
#define CTRL_NVSWITCH_INBAND_SEND_DATA                      0x43
#define CTRL_NVSWITCH_INBAND_READ_DATA                      0x44
#define CTRL_NVSWITCH_INBAND_FLUSH_DATA                     0x45
#define CTRL_NVSWITCH_INBAND_PENDING_DATA_STATS             0x46
#define CTRL_NVSWITCH_GET_SW_INFO                           0x47
#define CTRL_NVSWITCH_RESERVED_6                            0x48
#define CTRL_NVSWITCH_RESERVED_7                            0x49
#define CTRL_NVSWITCH_CCI_GET_PORTS_CPLD_INFO               0x4A
#define CTRL_NVSWITCH_CCI_GET_FW_REVISIONS                  0x4B
#define CTRL_NVSWITCH_CCI_SET_LOCATE_LED                    0x4C
#define CTRL_NVSWITCH_REGISTER_READ                         0x4D
#define CTRL_NVSWITCH_REGISTER_WRITE                        0x4E
#define CTRL_NVSWITCH_GET_INFOROM_VERSION                   0x4F
#define CTRL_NVSWITCH_GET_ERR_INFO                          0x50
#define CTRL_NVSWITCH_CLEAR_COUNTERS                        0x51
#define CTRL_NVSWITCH_SET_NVLINK_ERROR_THRESHOLD            0x52
#define CTRL_NVSWITCH_GET_NVLINK_ERROR_THRESHOLD            0x53
#define CTRL_NVSWITCH_GET_VOLTAGE                           0x54
#define CTRL_NVSWITCH_GET_SOE_HEARTBEAT                     0x55
#define CTRL_NVSWITCH_GET_BOARD_PART_NUMBER                 0x56
#define CTRL_NVSWITCH_GET_POWER                             0x57
#define CTRL_NVSWITCH_GET_PORT_EVENTS                       0x58
#define CTRL_NVSWITCH_GET_SYS_INFO                          0x59
#define CTRL_NVSWITCH_GET_TIME_INFO                         0x60
#define CTRL_NVSWITCH_GET_TEMP_DATA                         0x61
#define CTRL_NVSWITCH_GET_TEMP_SAMPLES                      0x62
#define CTRL_NVSWITCH_SET_CONTINUOUS_ALI                    0x63
#define CTRL_NVSWITCH_REQUEST_ALI                           0x64
#define CTRL_NVSWITCH_GET_NVLINK_L1_CAPABILITY              0x65
#define CTRL_NVSWITCH_GET_NVLINK_L1_THRESHOLD               0x66
#define CTRL_NVSWITCH_SET_NVLINK_L1_THRESHOLD               0x67
#define CTRL_NVSWITCH_FSPRPC_GET_CAPS                       0x68
#define CTRL_NVSWITCH_SET_DEVICE_TNVL_LOCK                  0x69
#define CTRL_NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN     0x6A
#define CTRL_NVSWITCH_GET_ATTESTATION_REPORT                0x6B
#define CTRL_NVSWITCH_GET_TNVL_STATUS                       0x6C

#ifdef __cplusplus
}
#endif

#endif // _CTRL_DEVICE_NVSWITCH_H_
