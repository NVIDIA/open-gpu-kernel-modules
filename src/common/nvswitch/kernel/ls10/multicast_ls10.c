/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ls10/multicast_ls10.h"

#include "nvswitch/ls10/dev_route_ip.h"

// Source: IAS Table 44. Laguna NXbar TileCol Port Mapping
static const NVSWITCH_COLUMN_PORT_OFFSET_LS10 nvswitch_portmap_ls10[NVSWITCH_NUM_LINKS_LS10] = {
    // ports 0 - 10
    { 0,  0 }, { 0,  1 }, { 0,  2 }, { 0,  3 },
    { 0,  4 }, { 0,  5 }, { 0,  6 }, { 0,  7 },
    { 0,  8 }, { 0,  9 }, { 0, 10 },
    // ports 11 - 15
    { 2,  0 }, { 2,  3 }, { 2,  4 }, { 2,  5 },
    { 2,  8 },
    // ports 16 - 26
    { 4, 10 }, { 4,  9 }, { 4,  8 }, { 4,  7 },
    { 4,  6 }, { 4,  5 }, { 4,  4 }, { 4,  3 },
    { 4,  2 }, { 4,  1 }, { 4,  0 },
    // ports 27 - 31
    { 2,  9 }, { 2,  7 }, { 2,  6 }, { 2,  2 },
    { 2,  1 },
    // ports 32 - 42
    { 1,  0 }, { 1,  1 }, { 1,  2 }, { 1,  3 },
    { 1,  4 }, { 1,  5 }, { 1,  6 }, { 1,  7 },
    { 1,  8 }, { 1,  9 }, { 1, 10 },
    // ports 43 - 47
    { 3,  0 }, { 3,  3 }, { 3,  4 }, { 3,  5 },
    { 3,  8 },
    // ports 48 - 58
    { 5, 10 }, { 5,  9 }, { 5,  8 }, { 5,  7 },
    { 5,  6 }, { 5,  5 }, { 5,  4 }, { 5,  3 },
    { 5,  2 }, { 5,  1 }, { 5,  0 },
    // ports 59 - 63
    { 3,  9 }, { 3,  7 }, { 3,  6 },  { 3,  2 },
    { 3,  1 }
};

static NvlStatus
_nvswitch_get_column_port_offset_ls10
(
    NvU32 port,
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 *column_port_offset
)
{
    if (port >=  NVSWITCH_NUM_LINKS_LS10)
        return -NVL_BAD_ARGS;

    *column_port_offset = nvswitch_portmap_ls10[port];

    return NVL_SUCCESS;
}

#if defined(NVSWITCH_MC_TRACE)
static void
_nvswitch_mc_print_directive
(
    nvswitch_device *device,
    NVSWITCH_TCP_DIRECTIVE_LS10 *mc_directive
)
{
    if (!mc_directive)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: null directive pointer\n", __FUNCTION__);
        return;
    }

    NVSWITCH_PRINT(device, INFO, "TCP:      %4d ", mc_directive->tcp);

    // pretty-print null ports
    if (mc_directive->tcpEPort == NVSWITCH_MC_NULL_PORT_LS10)
    {
        NVSWITCH_PRINT(device, INFO, "EPort:       X         OPort: %4d",
                        mc_directive->tcpOPort);
    }
    else if (mc_directive->tcpOPort == NVSWITCH_MC_NULL_PORT_LS10)
    {
         NVSWITCH_PRINT(device, INFO, "EPort:    %4d         OPort:    X",
                         mc_directive->tcpEPort);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "EPort:    %4d         OPort: %4d",
                    mc_directive->tcpEPort,
                    mc_directive->tcpOPort);
    }

    NVSWITCH_PRINT(device, INFO, "EAltPath: %4d      OAltPath: %4d",
                    mc_directive->tcpEAltPath,
                    mc_directive->tcpOAltPath);
    NVSWITCH_PRINT(device, INFO, "EVCHop:   %4d      OVCHop:   %4d",
                    mc_directive->tcpEVCHop,
                    mc_directive->tcpOVCHop);
    NVSWITCH_PRINT(device, INFO, "portFlag: %4d continueRound: %4d lastRound: %4d ",
                    mc_directive->portFlag,
                    mc_directive->continueRound,
                    mc_directive->lastRound);
    NVSWITCH_PRINT(device, INFO, "\n");
}

static void
_nvswitch_mc_print_directives
(
    nvswitch_device *device,
    NVSWITCH_TCP_DIRECTIVE_LS10 *mcp_list,
    NvU32 entries_used,
    NvU8 *spray_group_ptrs,
    NvU32 num_spray_groups
)
{
    NvU32 i, spray_group_offset, round, spray_group_idx, cur_entry_idx, entries_printed;
    NvBool spray_group_done = NV_FALSE;

    if (num_spray_groups == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: No spray groups specified\n", __FUNCTION__);
        return;
    }

    if (num_spray_groups > NVSWITCH_MC_MAX_SPRAY_LS10)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Too many spray groups specified: %d\n",
                        __FUNCTION__, num_spray_groups);
        return;
    }

    if (entries_used > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Too many entries specified: %d\n",
                        __FUNCTION__, entries_used);
        return;
    }


    NVSWITCH_PRINT(device, INFO, "Total spray groups %d\n", num_spray_groups);

    entries_printed = 0;

    // loop through spray groups
    for (spray_group_idx = 0; spray_group_idx < num_spray_groups; spray_group_idx++)
    {
        spray_group_done = NV_FALSE;
        spray_group_offset = spray_group_ptrs[spray_group_idx];
        cur_entry_idx = spray_group_offset;
        round = 0;

        NVSWITCH_PRINT(device, INFO, "Spray group %d offset %d\n", spray_group_idx,
                        spray_group_offset);

        while (!spray_group_done)
        {
            if (entries_printed >= NVSWITCH_MC_TCP_LIST_SIZE_LS10)
            {
                NVSWITCH_PRINT(device, ERROR, "%s: Overflow of mcplist. Entries printed: %d\n",
                                __FUNCTION__, entries_printed);
                return;
            }

            NVSWITCH_PRINT(device, INFO, "Round %d, First mc_plist Index %d round size %d\n",
                            round, cur_entry_idx, mcp_list[cur_entry_idx].roundSize);

            for (i = 0; i < mcp_list[cur_entry_idx].roundSize; i++)
            {
                if ((i + cur_entry_idx) > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
                {
                    NVSWITCH_PRINT(device, ERROR, "%s: Overflow of mcplist. %d\n",
                    __FUNCTION__, i + cur_entry_idx);
                }

                _nvswitch_mc_print_directive(device, &mcp_list[i + cur_entry_idx]);
                entries_printed++;

                if (mcp_list[i + cur_entry_idx].lastRound)
                {
                    NVSWITCH_PRINT(device, INFO, "Last round of spray group found at offset %d\n",
                        i + cur_entry_idx);
                    spray_group_done = NV_TRUE;
                }
            }

            round++;
            cur_entry_idx += i;
        }

    }
}
#endif  // defined(NVSWITCH_MC_TRACE)


// build column-port array
static NvlStatus
_nvswitch_mc_build_ports_array
(
    nvswitch_device *device,
    NvU32 num_ports,
    NvU32 *spray_group,
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 *ports,
    NvU8 *vchop_array_sg,
    NvU8 vchop_map[NVSWITCH_MC_NUM_COLUMNS_LS10][NVSWITCH_MC_PORTS_PER_COLUMN_LS10]
)
{
    NvU64 ports_used = 0;
    NvU32 i, ret;
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 cpo;
    if ((spray_group == NULL) || (ports == NULL) || (num_ports == 0) ||
        (num_ports > NVSWITCH_MC_TCP_LIST_SIZE_LS10))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: invalid arguments\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(vchop_map, 0, sizeof(NvU8) *
                        NVSWITCH_MC_NUM_COLUMNS_LS10 * NVSWITCH_MC_PORTS_PER_COLUMN_LS10);

    for (i = 0; i < num_ports; i++)
    {
        // prevent duplicate ports
        if (ports_used & NVBIT64(spray_group[i]))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: duplicate port specified: %d\n", __FUNCTION__,
                      spray_group[i]);
            return -NVL_BAD_ARGS;
        }
        ports_used |= NVBIT64(spray_group[i]);

        ret = _nvswitch_get_column_port_offset_ls10(spray_group[i], &cpo);
        if (ret != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: error getting column-port offset\n", __FUNCTION__);
            return ret;
        }

        ports[i] = cpo;

        if (vchop_array_sg[i] > NVSWITCH_MC_VCHOP_FORCE1)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: vchop value out of range: %d\n", __FUNCTION__,
                           vchop_array_sg[i]);
            return -NVL_BAD_ARGS;
        }
        vchop_map[cpo.column][cpo.port_offset] = vchop_array_sg[i];
    }

    return NVL_SUCCESS;
}

//
// Determine whether the given column/offset pair matches the given absolute
// primary_replica port number.
//
static NvBool
_is_primary_replica
(
    NvU32 col,
    NvU32 offset,
    NvU32 primary_replica
)
{
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 cpo;

    if (primary_replica == NVSWITCH_MC_INVALID)
        return NV_FALSE;

    if (_nvswitch_get_column_port_offset_ls10(primary_replica, &cpo) != NVL_SUCCESS)
        return NV_FALSE;

    if ((cpo.column == col) && (cpo.port_offset == offset))
        return NV_TRUE;

    return NV_FALSE;
}

//
// Set the round flags to indicate the size of each multicast round.
// See IAS section "6.12. Consistent MC Semantics" for more info.
//
// Make sure rounds have following properties:
// - no more than 11 rounds
// - no more than 3 directives per round
// - no duplicate TCP# within same round
//
static NvlStatus
_nvswitch_mc_set_round_flags(
    nvswitch_device *device,
    NVSWITCH_TCP_DIRECTIVE_LS10 *port_list,
    NvU32 port_list_size)
{
    NvU32 cur_portlist_pos, round_size, round_start, round_end;
    NVSWITCH_TCP_DIRECTIVE_LS10 *cur_dir, *next_dir;
    NvU32 round_tcp_mask = 0;
    NvU32 round_count = 0;
    if ((port_list == NULL) || (port_list_size == 0))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: called with empty port list?\n",
                       __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    round_start = 0;
    round_end = 0;

    for (cur_portlist_pos = 0; cur_portlist_pos < port_list_size; cur_portlist_pos++)
    {
        cur_dir = &port_list[cur_portlist_pos];

        // special case: last element: end of round and last round
        if (cur_portlist_pos == port_list_size - 1)
        {
            cur_dir->continueRound = NV_FALSE;
            cur_dir->lastRound = NV_TRUE;

            round_end = cur_portlist_pos;
            round_size = round_end - round_start + 1;

            // set the round size in the first directive
            cur_dir = &port_list[round_start];
            cur_dir->roundSize = (NvU8)round_size;
            round_count++;
        }
        else
        {
            round_tcp_mask |= (1 << cur_dir->tcp);

            // if next tcp is tcp that is already in current round, then current directive is end of round
            // if its third directive in current round, end round regardless of tcp assignments
            next_dir = &port_list[cur_portlist_pos + 1];
            if ((round_end - round_start == 2) || ((round_tcp_mask & (1 << next_dir->tcp)) != 0))
            {
                cur_dir->continueRound = NV_FALSE;

                round_end = cur_portlist_pos;
                round_size = round_end - round_start + 1;

                // set the round size in the first directive
                cur_dir = &port_list[round_start];
                cur_dir->roundSize = (NvU8)round_size;

                // advance round_start
                round_start = cur_portlist_pos + 1;

                round_tcp_mask = 0;
                round_count++;
            }
        }
    }

    if (round_count > 11)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Round count exceeds 11: %d\n",
                       __FUNCTION__, round_count);
        return -NVL_BAD_ARGS;
    }

    return NVL_SUCCESS;
}

//
// Set the port flags to indicate primary replica port location.
// See IAS section "6.12. Consistent MC Semantics" for more info.
//
static void
_nvswitch_mc_set_port_flags
(
    NVSWITCH_TCP_DIRECTIVE_LS10 *port_list,
    NvU32 port_list_size
)
{
    NvU32 cur_portlist_pos;
    NVSWITCH_TCP_DIRECTIVE_LS10 *cur_dir, *next_dir;

    if ((port_list == NULL) || (port_list_size == 0))
        return;

    for (cur_portlist_pos = 0; cur_portlist_pos < port_list_size; cur_portlist_pos++)
    {
        cur_dir = &port_list[cur_portlist_pos];

        if (cur_dir->primaryReplica != PRIMARY_REPLICA_NONE)
        {
            if (cur_dir->lastRound)
            {
                cur_dir->continueRound = NV_TRUE;

                if (cur_dir->primaryReplica == PRIMARY_REPLICA_EVEN)
                    cur_dir->portFlag = 0;
                if (cur_dir->primaryReplica == PRIMARY_REPLICA_ODD)
                    cur_dir->portFlag = 1;
            }
            else
            {
                // primary replica is in this directive, next directive specifies even or odd
                cur_dir->portFlag = 1;

                if (cur_portlist_pos + 1 >= port_list_size)
                {
                    NVSWITCH_ASSERT(0);
                    return;
                }

                next_dir = &port_list[cur_portlist_pos + 1];

                if (cur_dir->primaryReplica == PRIMARY_REPLICA_EVEN)
                    next_dir->portFlag = 0;
                if (cur_dir->primaryReplica == PRIMARY_REPLICA_ODD)
                    next_dir->portFlag = 1;
            }
        }
    }
}

//
// Build a list of TCP directives. This is the main conversion function which is used to build a
// TCP directive list for each spray group from a given column/port bitmap.
//
// @param device                [in]  pointer to the nvswitch device struct
// @param num_ports             [in]  number of ports
// @param ports                 [in]  ports array (pair of column and port offset)
// @param primary_replica       [in]  the primary replica port for this spray group, if specified
// @param vchop_map             [in]  array containing per-port vchop values in column/port format
// @param port_list             [out] array where the newly built directive list is written
// @param entries_used          [out] pointer to an int where the size of resulting list is written
//
static NvlStatus
_nvswitch_mc_build_portlist
(
    nvswitch_device *device,
    NvU32 num_ports,
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 *ports,
    NvU32 primary_replica,
    NvU8 vchop_map[NVSWITCH_MC_NUM_COLUMNS_LS10][NVSWITCH_MC_PORTS_PER_COLUMN_LS10],
    NVSWITCH_TCP_DIRECTIVE_LS10 *port_list,
    NvU32 *entries_used
)
{
    NvU8 last_portlist_pos;
    NvU8 i;
    NVSWITCH_TCP_DIRECTIVE_LS10 *cur_dir;

    if ((ports == NULL) || (port_list == NULL))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Invalid arguments\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // 6.15.2. Route Interface Description
    for (i = 0 ; i < num_ports ; ++i)
    {

        NvU8 cur_port = ports[i].port_offset;
        NvU32 cur_col = ports[i].column;

        // Populating either EVEN or ODD column but not both
        cur_dir = &port_list[i];
        cur_dir->primaryReplica = PRIMARY_REPLICA_NONE;
        cur_dir->tcp = cur_col/2; // tile-column pair numbering
        if (cur_col % 2 == 1)
        { // odd column port
            cur_dir->tcpOPort = cur_port;
            cur_dir->tcpEPort = NVSWITCH_MC_NULL_PORT_LS10;
            cur_dir->tcpOVCHop = vchop_map[cur_col][cur_port];
            if (_is_primary_replica(cur_col, cur_port, primary_replica))
            {
                cur_dir->primaryReplica = PRIMARY_REPLICA_ODD;

#ifdef NVSWITCH_MC_DEBUG
                NVSWITCH_PRINT(device, DEBUG, "%s: Odd column primary replica programmed: %d %d\n",
                       __FUNCTION__, primary_replica, i);
#endif
            }
        }
        else
        { // even column port
            cur_dir->tcpEPort = cur_port;
            cur_dir->tcpOPort = NVSWITCH_MC_NULL_PORT_LS10;
            cur_dir->tcpEVCHop = vchop_map[cur_col][cur_port];
            if (_is_primary_replica(cur_col, cur_port, primary_replica))
            {
                cur_dir->primaryReplica = PRIMARY_REPLICA_EVEN;
#ifdef NVSWITCH_MC_DEBUG
                NVSWITCH_PRINT(device, DEBUG, "%s: Even column primary replica programmed: %d %d\n",
                       __FUNCTION__, primary_replica, i);
#endif
            }
        }

        last_portlist_pos = i;
    }

    // set the lastRound flag for the last entry in the spray string
    cur_dir = &port_list[last_portlist_pos];
    cur_dir->lastRound = NV_TRUE;

    *entries_used = last_portlist_pos + 1;

#ifdef NVSWITCH_MC_DEBUG
    NVSWITCH_PRINT(device, INFO,
                    "%s: entries_used: %d, last_portlist_pos: %d\n",
                    __FUNCTION__, *entries_used, last_portlist_pos);
#endif

    return NVL_SUCCESS;
}

//
// Helper that initializes a given directive list to some base values.
//
static NV_INLINE NvlStatus
nvswitch_init_portlist_ls10
(
    nvswitch_device *device,
    NVSWITCH_TCP_DIRECTIVE_LS10 *mcp_list,
    NvU32 mcp_list_size
)
{
    NvU32 i;

    if (mcp_list_size > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
    {
         NVSWITCH_PRINT(device, ERROR, "%s: mcp_list_size out of range (%d)\n",
                        __FUNCTION__, mcp_list_size);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(mcp_list, 0,
                        sizeof(NVSWITCH_TCP_DIRECTIVE_LS10) * mcp_list_size);

    //
    // initialize port list with invalid values
    // continueRound will be fixed up when processing round flags
    //
    for ( i = 0; i < mcp_list_size; i ++ )
    {
        mcp_list[i].tcp = NVSWITCH_MC_INVALID;
        mcp_list[i].continueRound = NV_TRUE;
        mcp_list[i].tcpEPort = NVSWITCH_MC_NULL_PORT_LS10;
        mcp_list[i].tcpOPort = NVSWITCH_MC_NULL_PORT_LS10;
    }

    return NVL_SUCCESS;
}


//
// Helper to traverse list of directives given in src and copy only valid entries to dst starting
// at dst_list_offset.
//
// This is used when building the final directive list from individual per-spray-group lists,
// ensuring that no invalid entries sneak in, as well as checking for a nontrivial corner case
// where a configuration of input spray groups can result in a directive list larger than the
// 32-entry space allowed in the table. This returns -NVL_MORE_PROCESSING_REQUIRED which is
// then propagated to the caller to adjust the input parameters and try again.
//
static NV_INLINE NvlStatus
_nvswitch_mc_copy_valid_entries_ls10
(
    nvswitch_device *device,
    NVSWITCH_TCP_DIRECTIVE_LS10 *dst,
    NVSWITCH_TCP_DIRECTIVE_LS10 *src,
    NvU32 num_valid_entries,
    NvU32 dst_list_offset
)
{
    NvU32 i;

    if (num_valid_entries + dst_list_offset > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
    {
        NVSWITCH_PRINT(device, ERROR,
                        "%s: Overflow of mcplist. num_valid_entries: %d, dst_list_offset: %d\n",
                        __FUNCTION__, num_valid_entries, dst_list_offset);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    for (i = 0; i < num_valid_entries; i++)
    {
        if (src[i].tcp == NVSWITCH_MC_INVALID)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: invalid entry at offset %d\n", __FUNCTION__, i);
            return -NVL_ERR_GENERIC;
        }

#ifdef NVSWITCH_MC_TRACE
            NVSWITCH_PRINT(device, INFO, "%s: copying entry from src[%d] to dst[%d]\n",
                            __FUNCTION__, i, dst_list_offset + i);
            _nvswitch_mc_print_directive(device, &src[i]);
#endif

        nvswitch_os_memcpy(&dst[dst_list_offset + i], &src[i], sizeof(NVSWITCH_TCP_DIRECTIVE_LS10));
    }

    return NVL_SUCCESS;
}


//
// Build multicast directive list using the inputs given.
//
// @param device                [in] pointer to the nvswitch device struct
// @param port_list             [in] array of ports for all spray groups
// @param ports_per_spray_group [in] array specifying the size of each spray group
// @param pri_replica_offsets   [in] array, offsets of primary replica ports for each spray group
// @param replica_valid_array   [in] array, specifies which pri_replica_offsets are valid
// @param vchop_array           [in] array of vchop values for each port given in port_list
// @param table_entry           [out] pointer to table entry where directive list will be written
// @param entries_used          [out] pointer, number of valid entries produced is written here
//
NvlStatus
nvswitch_mc_build_mcp_list_ls10
(
    nvswitch_device *device,
    NvU32 *port_list,
    NvU32 *ports_per_spray_group,
    NvU32 *pri_replica_offsets,
    NvBool *replica_valid_array,
    NvU8 *vchop_array,
    NVSWITCH_MC_RID_ENTRY_LS10 *table_entry,
    NvU32 *entries_used
)
{
    NvU32 i, spray_group_idx, spray_group_size, num_spray_groups, ret;
    NvU8 *spray_group_ptrs;
    NvU32 spray_group_offset = 0;
    NvU32 primary_replica_port = NVSWITCH_MC_INVALID;
    NvU32 dir_entries_used_sg = 0;
    NvU32 dir_entries_used = 0;
    NvU32 mcplist_offset = 0;
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 ports[NVSWITCH_NUM_LINKS_LS10];
    NvU8 vchop_map[NVSWITCH_MC_NUM_COLUMNS_LS10][NVSWITCH_MC_PORTS_PER_COLUMN_LS10];
    NVSWITCH_TCP_DIRECTIVE_LS10 tmp_mcp_list[NVSWITCH_MC_TCP_LIST_SIZE_LS10];
    NVSWITCH_TCP_DIRECTIVE_LS10 *mcp_list;

    NvU32 j;

    if ((device == NULL) || (port_list == NULL) || (ports_per_spray_group == NULL) ||
        (pri_replica_offsets == NULL) || (replica_valid_array == NULL) || (vchop_array == NULL) ||
        (table_entry == NULL) || (entries_used == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    num_spray_groups = table_entry->num_spray_groups;
    spray_group_ptrs = table_entry->spray_group_ptrs;
    mcp_list = table_entry->directives;

    if (num_spray_groups == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: No spray groups specified\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (num_spray_groups > NVSWITCH_MC_MAX_SPRAYGROUPS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Too many spray groups specified: %d\n",
                        __FUNCTION__, num_spray_groups);
        return -NVL_BAD_ARGS;
    }

    for (i = 0, j = 0; i < num_spray_groups; i++)
    {
        if (ports_per_spray_group[i] < NVSWITCH_MC_MIN_PORTS_PER_GROUP_LS10)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Too few ports in spray group %d\n",
                            __FUNCTION__, i);
            return -NVL_BAD_ARGS;
        }

        if (ports_per_spray_group[i] > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Too many ports in spray group %d\n",
                            __FUNCTION__, i);
            return -NVL_BAD_ARGS;
        }

        j += ports_per_spray_group[i];
    }

    if (j > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
    {
         NVSWITCH_PRINT(device, ERROR, "%s: Too many ports specified in total spray groups: %d\n",
                        __FUNCTION__, j);
        return -NVL_BAD_ARGS;
    }

    ret = nvswitch_init_portlist_ls10(device, mcp_list, NVSWITCH_MC_TCP_LIST_SIZE_LS10);
    if (ret != NVL_SUCCESS)
        return ret;

    // build spray strings for each spray group
    for ( spray_group_idx = 0; spray_group_idx < num_spray_groups; spray_group_idx++ )
    {
        spray_group_size = ports_per_spray_group[spray_group_idx];

#ifdef NVSWITCH_MC_DEBUG
        NVSWITCH_PRINT(device, INFO, "%s: processing spray group %d size %d of %d total groups\n",
                        __FUNCTION__, spray_group_idx, spray_group_size, num_spray_groups);
#endif

        ret = nvswitch_init_portlist_ls10(device, tmp_mcp_list, NVSWITCH_MC_TCP_LIST_SIZE_LS10);
        if (ret != NVL_SUCCESS)
            return ret;

        ret = _nvswitch_mc_build_ports_array(device, spray_group_size, &port_list[spray_group_offset],
                                             ports, &vchop_array[spray_group_offset], vchop_map);

        if (ret != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                            "%s: error building port-column array for spray group %d: %d\n",
                            __FUNCTION__, spray_group_idx, ret);
            return ret;
        }

        // Set the offset to this spray group in the mcp list.
        spray_group_ptrs[spray_group_idx] = (NvU8)dir_entries_used;

        // if primary replica is specified for this spray group, find the port number
        if (replica_valid_array[spray_group_idx])
        {
            if (pri_replica_offsets[spray_group_idx] >= spray_group_size)
            {
                NVSWITCH_PRINT(device, ERROR,
                            "%s: primary replica offset %d is out of range for spray group %d\n",
                            __FUNCTION__, pri_replica_offsets[spray_group_idx], spray_group_idx);
                return -NVL_BAD_ARGS;
            }

            for (i = 0; i < spray_group_size; i++)
            {
                if (pri_replica_offsets[spray_group_idx] == i)
                {
                    primary_replica_port = port_list[spray_group_offset + i];
#ifdef NVSWITCH_MC_DEBUG
                    NVSWITCH_PRINT(device, INFO, "Primary replica port in spray group %d is %d\n",
                                    spray_group_idx, primary_replica_port);
#endif
                }
            }
        }
#ifdef NVSWITCH_MC_DEBUG
        if (primary_replica_port == NVSWITCH_MC_INVALID)
            NVSWITCH_PRINT(device, INFO, "%s: No primary replica specified for spray group %d\n",
                           __FUNCTION__, spray_group_idx);
#endif

        // process columns into spray group of multicast directives
        mcplist_offset = dir_entries_used;

        if (mcplist_offset >= NVSWITCH_MC_TCP_LIST_SIZE_LS10)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Overflow: mcplist_offset is %d\n",
                            __FUNCTION__, mcplist_offset);
            return -NVL_ERR_GENERIC;
        }

#ifdef NVSWITCH_MC_DEBUG
        NVSWITCH_PRINT(device, INFO, "%s: building tmp mc portlist at mcp offset %d, size %d\n",
                        __FUNCTION__, mcplist_offset, spray_group_size);
#endif
        ret = _nvswitch_mc_build_portlist(device, spray_group_size, ports, primary_replica_port, vchop_map,
                                          tmp_mcp_list, &dir_entries_used_sg);

        if (ret != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: error building MC portlist\n", __FUNCTION__);
            return ret;
        }

#ifdef NVSWITCH_MC_DEBUG
        NVSWITCH_PRINT(device, INFO, "%s: entries used after building portlist: %d\n",
                       __FUNCTION__, dir_entries_used_sg);
#endif
        ret = _nvswitch_mc_set_round_flags(device, tmp_mcp_list, dir_entries_used_sg);
        if (ret != NVL_SUCCESS)
            return ret;

        _nvswitch_mc_set_port_flags(tmp_mcp_list, dir_entries_used_sg);

        //copy spray group entries into final portlist
        ret = _nvswitch_mc_copy_valid_entries_ls10(device, mcp_list, tmp_mcp_list,
                                                   dir_entries_used_sg, mcplist_offset);
        if (ret != NVL_SUCCESS)
            return ret;

        dir_entries_used += dir_entries_used_sg;

        // increment position in the input port list
        spray_group_offset += spray_group_size;
    }

    *entries_used = dir_entries_used;

#ifdef NVSWITCH_MC_TRACE
    _nvswitch_mc_print_directives(device, mcp_list, *entries_used, spray_group_ptrs,
                                  num_spray_groups);
#endif

    return NVL_SUCCESS;
}

static NvU32
_nvswitch_col_offset_to_port_ls10
(
    NvU32 col,
    NvU32 offset
)
{
    NvU32 i;
    NVSWITCH_COLUMN_PORT_OFFSET_LS10 cpo;

    if ((col > NVSWITCH_MC_NUM_COLUMNS_LS10) || (offset > NVSWITCH_MC_PORTS_PER_COLUMN_LS10))
        return NVSWITCH_MC_INVALID;

    for (i = 0; i < NVSWITCH_NUM_LINKS_LS10; i++)
    {
        cpo = nvswitch_portmap_ls10[i];

        if ((cpo.column == col) && (cpo.port_offset == offset))
            return i;
    }

    return NVSWITCH_MC_INVALID;
}

NvlStatus
nvswitch_mc_unwind_directives_ls10
(
    nvswitch_device *device,
    NVSWITCH_TCP_DIRECTIVE_LS10 directives[NVSWITCH_MC_TCP_LIST_SIZE_LS10],
    NvU32 ports[NVSWITCH_MC_MAX_PORTS],
    NvU8 vc_hop[NVSWITCH_MC_MAX_PORTS],
    NvU32 ports_per_spray_group[NVSWITCH_MC_MAX_SPRAYGROUPS],
    NvU32 replica_offset[NVSWITCH_MC_MAX_SPRAYGROUPS],
    NvBool replica_valid[NVSWITCH_MC_MAX_SPRAYGROUPS]
)
{
    NvU32 ret = NVL_SUCCESS;
    NvU32 i, port_idx, cur_sg, ports_in_cur_sg, port, primary_replica;
    NVSWITCH_TCP_DIRECTIVE_LS10 cur_dir;

    cur_sg = 0;
    port_idx = 0;
    ports_in_cur_sg = 0;

    for (i = 0; i < NVSWITCH_MC_TCP_LIST_SIZE_LS10; i++)
    {
        cur_dir = directives[i];
        if (cur_dir.tcp == NVSWITCH_MC_INVALID)
        {
#ifdef NVSWITCH_MC_DEBUG
            NVSWITCH_PRINT(device, INFO, "%s: reached end of directive list (element %d)\n",
                            __FUNCTION__, i);
#endif
            break;
        }

        //
        // Find primary replica.
        // For more info, see: IAS 6.12. Consistent MC Semantics
        //

        primary_replica = PRIMARY_REPLICA_NONE;

        //
        // If lastRound = 1 and continueRound = 1, primary replica is in
        // this TCP directive and portFlag = 0/1 selects even/odd port.
        //
        if ((cur_dir.lastRound) && (cur_dir.continueRound))
        {
            if (cur_dir.portFlag)
                primary_replica = PRIMARY_REPLICA_ODD;
            else
                primary_replica = PRIMARY_REPLICA_EVEN;

        }
        //
        // If directive's portFlag == 1, this TCP directive contains the
        // primary replica, and the next TCP directive's portFlag = 0/1
        // selects the even/odd port of this TCP directive.
        //
        else if ((i < (NVSWITCH_MC_TCP_LIST_SIZE_LS10 - 1)) && (cur_dir.portFlag == 1))
        {
            // The next directive's portFlags specify even or odd
            if (directives[i + 1].portFlag)
                primary_replica = PRIMARY_REPLICA_ODD;
            else
                primary_replica = PRIMARY_REPLICA_EVEN;
        }

#ifdef NVSWITCH_MC_TRACE
        if (primary_replica == PRIMARY_REPLICA_ODD) {
            NVSWITCH_PRINT(device, INFO, "%s: Odd primary replica detected: %d\n",
                       __FUNCTION__, i);
        }
        if (primary_replica == PRIMARY_REPLICA_EVEN) {
            NVSWITCH_PRINT(device, INFO, "%s: Even primary replica detected: %d\n",
                       __FUNCTION__, i);
        }
#endif

        if (cur_dir.tcpEPort != NVSWITCH_MC_NULL_PORT_LS10)
        {
            ports_in_cur_sg++;

            if (cur_dir.tcpEAltPath)
            {
                port = _nvswitch_col_offset_to_port_ls10(cur_dir.tcp * 2 + 1, cur_dir.tcpEPort);
            }
            else
            {
                port = _nvswitch_col_offset_to_port_ls10(cur_dir.tcp * 2, cur_dir.tcpEPort);
            }

            if (port == NVSWITCH_MC_INVALID)
            {
                // if we get here, there's a bug when converting from col/offset to port number
                NVSWITCH_ASSERT(0);
                return -NVL_ERR_GENERIC;
            }

            if (port_idx >= NVSWITCH_MC_MAX_PORTS)
            {
                // if we get here, there's a bug when incrementing the port index
                NVSWITCH_ASSERT(0);
                return -NVL_ERR_GENERIC;
            }

            vc_hop[port_idx] = cur_dir.tcpEVCHop;
            ports[port_idx] = port;

            if (primary_replica == PRIMARY_REPLICA_EVEN)
            {
                replica_offset[cur_sg] = port_idx;
                replica_valid[cur_sg] = NV_TRUE;
#ifdef NVSWITCH_MC_TRACE
                NVSWITCH_PRINT(device, INFO, "%s: primary replica is port %d, offset %d in sg %d\n",
                                __FUNCTION__, port, port_idx, cur_sg);
#endif
            }

            port_idx++;
        }

        if (cur_dir.tcpOPort != NVSWITCH_MC_NULL_PORT_LS10)
        {
            ports_in_cur_sg++;

            if (cur_dir.tcpOAltPath)
            {
                port = _nvswitch_col_offset_to_port_ls10(cur_dir.tcp * 2, cur_dir.tcpOPort);
            }
            else
            {
                port = _nvswitch_col_offset_to_port_ls10(cur_dir.tcp * 2 + 1, cur_dir.tcpOPort);
            }

            if (port == NVSWITCH_MC_INVALID)
            {
                // if we get here, there's a bug when converting from col/offset to port number
                NVSWITCH_ASSERT(0);
                return -NVL_ERR_GENERIC;
            }

            if (port_idx >= NVSWITCH_MC_MAX_PORTS)
            {
                // if we get here, there's a bug when incrementing the port index
                NVSWITCH_ASSERT(0);
                return -NVL_ERR_GENERIC;
            }

            vc_hop[port_idx] = cur_dir.tcpOVCHop;
            ports[port_idx] = port;

            if (primary_replica == PRIMARY_REPLICA_ODD)
            {
                replica_offset[cur_sg] = port_idx;
                replica_valid[cur_sg] = NV_TRUE;
#ifdef NVSWITCH_MC_TRACE
                NVSWITCH_PRINT(device, INFO, "%s: primary replica is port %d, offset %d in sg %d\n",
                                __FUNCTION__, port, port_idx, cur_sg);
#endif
            }

            port_idx++;
        }

        if (cur_dir.lastRound)
        {
#ifdef NVSWITCH_MC_TRACE
            NVSWITCH_PRINT(device, INFO, "%s: reached end of spray group %d, %d total ports\n",
                            __FUNCTION__, cur_sg, ports_in_cur_sg);
#endif
            ports_per_spray_group[cur_sg] = ports_in_cur_sg;
            ports_in_cur_sg = 0;
            cur_sg++;
        }
    }

    return ret;
}

//
// Invalidate an MCRID table entry.
//
// @param device                [in] pointer to the nvswitch device struct
// @param port                  [in] port for which to invalidate the table entry
// @param index                 [in] index into the MCRID table
// @param use_extended_table    [in] specifies whether to use the extended table, or main table
// @param zero                  [in] specifies whether to zero the entry as well as invalidate
//
NvlStatus
nvswitch_mc_invalidate_mc_rid_entry_ls10
(
    nvswitch_device *device,
    NvU32 port,
    NvU32 index,
    NvBool use_extended_table,
    NvBool zero
)
{
    NvU32 reg, i;

    if ((device == NULL) || (!nvswitch_is_link_valid(device, port)))
        return -NVL_BAD_ARGS;

    if (use_extended_table && (index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDEXTTAB_DEPTH))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for extended table\n",
                        __FUNCTION__, index);
        return -NVL_BAD_ARGS;
    }

    if (index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDTAB_DEPTH)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for main table\n",
                       __FUNCTION__, index);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (use_extended_table)
        reg = FLD_SET_DRF(_ROUTE, _RIDTABADDR, _RAM_SEL, _SELECTSEXTMCRIDROUTERAM, 0);
    else
        reg = FLD_SET_DRF(_ROUTE, _RIDTABADDR, _RAM_SEL, _SELECTSMCRIDROUTERAM, 0);

    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABADDR, _INDEX, index, reg);
    NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABADDR, reg);

    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _VALID, 0, 0);

    if (!zero)
    {
        NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA0, reg);
        return NVL_SUCCESS;
    }

    for (i = 0; i < 4; i++)
    {
        NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA2, 0);
    }

    for (i = 0; i < 32; i++)
    {
        NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA1, 0);
    }

    NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA0, 0);

    return NVL_SUCCESS;
}

//
// Program an MCRID table entry.
//
// @param device                [in] pointer to the nvswitch device struct
// @param port                  [in] port for which to write the table entry
// @param table_entry           [in] pointer to the table entry to write
// @param directive_list_size   [in] size of the directive list contained in table_entry
//
NvlStatus
nvswitch_mc_program_mc_rid_entry_ls10
(
    nvswitch_device *device,
    NvU32 port,
    NVSWITCH_MC_RID_ENTRY_LS10 *table_entry,
    NvU32 directive_list_size
)
{
    NvU32 i, reg;
    NVSWITCH_TCP_DIRECTIVE_LS10 *cur_dir;

    if ((device == NULL) || (!nvswitch_is_link_valid(device, port)) || (table_entry == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    if (table_entry->use_extended_table &&
        (table_entry->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDEXTTAB_DEPTH))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for extended table\n",
                        __FUNCTION__, table_entry->index);
        return -NVL_BAD_ARGS;
    }

    if (table_entry->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDTAB_DEPTH)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for main table\n",
                       __FUNCTION__, table_entry->index);
        return -NVL_BAD_ARGS;
    }

    if (directive_list_size > NVSWITCH_MC_TCP_LIST_SIZE_LS10)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: directive_list_size out of range\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if ((table_entry->num_spray_groups > NVSWITCH_MC_MAX_SPRAY_LS10) ||
        (table_entry->num_spray_groups == 0))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: num_spray_groups out of range\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if ((table_entry->mcpl_size > NVSWITCH_NUM_LINKS_LS10) ||
        (table_entry->mcpl_size == 0))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: mcpl_size out of range\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (table_entry->ext_ptr_valid &&
        (table_entry->ext_ptr > NV_ROUTE_RIDTABADDR_INDEX_MCRIDEXTTAB_DEPTH))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: extended_ptr out of range\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (table_entry->use_extended_table)
        reg = FLD_SET_DRF(_ROUTE, _RIDTABADDR, _RAM_SEL, _SELECTSEXTMCRIDROUTERAM, 0);
    else
        reg = FLD_SET_DRF(_ROUTE, _RIDTABADDR, _RAM_SEL, _SELECTSMCRIDROUTERAM, 0);

    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABADDR, _INDEX, table_entry->index, reg);
    NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABADDR, reg);

    //
    // Write register 2. Each time this register is written it causes the
    // mcpl_str_ptr index to increment by 4 (4 entries are written at a time).
    //
    i = 0;
    while (i < table_entry->num_spray_groups)
    {

#ifdef NVSWITCH_MC_DEBUG
        NVSWITCH_PRINT(device, INFO, "%s: writing offset %d for spray group %d\n",
                                __FUNCTION__, table_entry->spray_group_ptrs[i], i);
#endif
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR0,
                              table_entry->spray_group_ptrs[i], 0);
        i++;

        if (i < table_entry->num_spray_groups)
        {
#ifdef NVSWITCH_MC_DEBUG
            NVSWITCH_PRINT(device, INFO, "%s: writing offset %d for spray group %d\n",
                                __FUNCTION__, table_entry->spray_group_ptrs[i], i);
#endif
            reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR1,
                                  table_entry->spray_group_ptrs[i], reg);
            i++;
        }

        if (i < table_entry->num_spray_groups)
        {
#ifdef NVSWITCH_MC_DEBUG
            NVSWITCH_PRINT(device, INFO, "%s: writing offset %d for spray group %d\n",
                                __FUNCTION__, table_entry->spray_group_ptrs[i], i);
#endif
            reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR2,
                                  table_entry->spray_group_ptrs[i], reg);
            i++;
        }

        if (i < table_entry->num_spray_groups)
        {
#ifdef NVSWITCH_MC_DEBUG
            NVSWITCH_PRINT(device, INFO, "%s: writing offset %d for spray group %d\n",
                                __FUNCTION__, table_entry->spray_group_ptrs[i], i);
#endif
            reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR3,
                                  table_entry->spray_group_ptrs[i], reg);
            i++;
        }

        NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA2, reg);
    }


    //
    // Write register 1. Each time this register is written it causes the mcpl_directive
    // index to increment by 1.
    //

    for (i = 0; i < directive_list_size; i++)
    {
        cur_dir = &table_entry->directives[i];

        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_E_PORT, cur_dir->tcpEPort, 0);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_E_ALTPATH, cur_dir->tcpEAltPath, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_E_REQ_VCHOP, cur_dir->tcpEVCHop, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_O_PORT, cur_dir->tcpOPort, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_O_ALTPATH, cur_dir->tcpOAltPath, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_O_REQ_VCHOP, cur_dir->tcpOVCHop, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_TCP, cur_dir->tcp, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_PORT_FLAG, cur_dir->portFlag, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_RND_CONTINUE, cur_dir->continueRound, reg);
        reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA1, _MCPL_LAST_RND, cur_dir->lastRound, reg);

        NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA1, reg);
    }

    //
    // Write register 0.
    //
    // Due to size limitations in HW, _MCPL_SIZE must be adjusted by one here.
    // From the reference manuals:
    //
    // The number of expected responses at this switch hop for this MCID is MCPL_SIZE+1.
    //
    // For _MCPL_SPRAY_SIZE the value 0 represents 16. This requires no adjustment
    // when writing, since 16 is truncated to 0 due to field width.
    //
    // The input parameters for both of these are guaranteed to be nonzero values.
    //
    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _MCPL_SIZE, table_entry->mcpl_size - 1, 0);
    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _MCPL_SPRAY_SIZE, table_entry->num_spray_groups,
                          reg);
    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _MCPL_RID_EXT_PTR, table_entry->ext_ptr, reg);
    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _MCPL_RID_EXT_PTR_VAL, table_entry->ext_ptr_valid,
                          reg);
    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _VALID, 1, reg);

    if (!table_entry->use_extended_table)
         reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABDATA0, _MCPL_NO_DYN_RSP, table_entry->no_dyn_rsp, reg);

    NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABDATA0, reg);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_mc_read_mc_rid_entry_ls10
(
    nvswitch_device *device,
    NvU32 port,
    NVSWITCH_MC_RID_ENTRY_LS10 *table_entry
)
{
    NvU32 i, reg;

    if ((device == NULL) || (table_entry == NULL))
        return -NVL_BAD_ARGS;

    if (table_entry->use_extended_table &&
        (table_entry->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDEXTTAB_DEPTH))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for extended table\n",
                        __FUNCTION__, table_entry->index);
        return -NVL_BAD_ARGS;
    }

    if (table_entry->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDTAB_DEPTH)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for main table\n",
                       __FUNCTION__, table_entry->index);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // set the address
    if (table_entry->use_extended_table)
        reg = FLD_SET_DRF(_ROUTE, _RIDTABADDR, _RAM_SEL, _SELECTSEXTMCRIDROUTERAM, 0);
    else
        reg = FLD_SET_DRF(_ROUTE, _RIDTABADDR, _RAM_SEL, _SELECTSMCRIDROUTERAM, 0);

    reg = FLD_SET_DRF_NUM(_ROUTE, _RIDTABADDR, _INDEX, table_entry->index, reg);
    NVSWITCH_NPORT_WR32_LS10(device, port, _ROUTE, _RIDTABADDR, reg);

    // read in the entry
    reg = NVSWITCH_NPORT_RD32_LS10(device, port, _ROUTE, _RIDTABDATA0);

    // parse DATA0
    table_entry->valid = DRF_VAL(_ROUTE, _RIDTABDATA0, _VALID, reg);

    // if the entry is invalid, we're done
    if (!table_entry->valid)
    {
        return NVL_SUCCESS;
    }

    //
    // Due to size limitations in HW, _MCPL_SIZE must be adjusted by one here.
    // From the reference manuals:
    //
    // The number of expected responses at this switch hop for this MCID is MCPL_SIZE+1.
    //
    // For _MCPL_SPRAY_SIZE, the value 0 represents 16, so we need to adjust for that here.
    //
    table_entry->mcpl_size = DRF_VAL(_ROUTE, _RIDTABDATA0, _MCPL_SIZE, reg) + 1;
    table_entry->num_spray_groups = DRF_VAL(_ROUTE, _RIDTABDATA0, _MCPL_SPRAY_SIZE, reg);

    if (table_entry->num_spray_groups == 0)
        table_entry->num_spray_groups = 16;

    if (!table_entry->use_extended_table)
    {
        table_entry->ext_ptr = DRF_VAL(_ROUTE, _RIDTABDATA0, _MCPL_RID_EXT_PTR, reg);
        table_entry->ext_ptr_valid = DRF_VAL(_ROUTE, _RIDTABDATA0, _MCPL_RID_EXT_PTR_VAL, reg);
    }

    table_entry->no_dyn_rsp = DRF_VAL(_ROUTE, _RIDTABDATA0, _MCPL_NO_DYN_RSP, reg);

    // DATA1 contains the directives

    for (i = 0; i < NVSWITCH_MC_TCP_LIST_SIZE_LS10; i++)
    {
        reg = NVSWITCH_NPORT_RD32_LS10(device, port, _ROUTE, _RIDTABDATA1);

        table_entry->directives[i].tcpEPort = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_E_PORT, reg);
        table_entry->directives[i].tcpEAltPath = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_E_ALTPATH, reg);
        table_entry->directives[i].tcpEVCHop = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_E_REQ_VCHOP, reg);
        table_entry->directives[i].tcpOPort = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_O_PORT, reg);
        table_entry->directives[i].tcpOAltPath = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_O_ALTPATH, reg);
        table_entry->directives[i].tcpOVCHop = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_O_REQ_VCHOP, reg);
        table_entry->directives[i].tcp = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_TCP, reg);
        table_entry->directives[i].portFlag = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_PORT_FLAG, reg);
        table_entry->directives[i].continueRound = DRF_VAL(_ROUTE, _RIDTABDATA1,
                                                           _MCPL_RND_CONTINUE, reg);
        table_entry->directives[i].lastRound = DRF_VAL(_ROUTE, _RIDTABDATA1, _MCPL_LAST_RND, reg);
    }

    // DATA2 contains the spray group pointers. This register loads the next 4 pointers on each read.
    i = 0;
    while (i < table_entry->num_spray_groups)
    {
        reg = NVSWITCH_NPORT_RD32_LS10(device, port, _ROUTE, _RIDTABDATA2);

        table_entry->spray_group_ptrs[i] = DRF_VAL(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR0, reg);
        i++;

        if (i < table_entry->num_spray_groups)
        {
            table_entry->spray_group_ptrs[i] = DRF_VAL(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR1, reg);
            i++;
        }

        if (i < table_entry->num_spray_groups)
        {
            table_entry->spray_group_ptrs[i] = DRF_VAL(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR2, reg);
            i++;
        }

        if (i < table_entry->num_spray_groups)
        {
            table_entry->spray_group_ptrs[i] = DRF_VAL(_ROUTE, _RIDTABDATA2, _MCPL_STR_PTR3, reg);
            i++;
        }
    }

    return NVL_SUCCESS;
}
