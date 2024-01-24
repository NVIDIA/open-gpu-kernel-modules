/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _MULTICAST_LS10_H_
#define _MULTICAST_LS10_H_

#define NVSWITCH_MC_TCP_LIST_SIZE_LS10          NVSWITCH_NUM_LINKS_LS10 / 2
#define NVSWITCH_MC_MAX_SPRAY_LS10              16
#define NVSWITCH_MC_NUM_COLUMNS_LS10            6
#define NVSWITCH_MC_NUM_COLUMN_PAIRS_LS10       NVSWITCH_MC_NUM_COLUMNS_LS10 / 2
#define NVSWITCH_MC_PORTS_PER_COLUMN_LS10       11
#define NVSWITCH_MC_MIN_PORTS_PER_GROUP_LS10    1

#define PRIMARY_REPLICA_NONE                    0
#define PRIMARY_REPLICA_EVEN                    1
#define PRIMARY_REPLICA_ODD                     2

#define NVSWITCH_MC_INVALID                     0xFF

#define NVSWITCH_MC_NULL_PORT_LS10              0xF

//
// Debug and trace print toggles
// To enable tracing, define NVSWITCH_MC_TRACE
// To enable extensive debug messages uncomment the below define NVSWITCH_MC_DEBUG
//
// #if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
// #define NVSWITCH_MC_DEBUG                       1
// #endif

typedef struct {
    NvU32 column;
    NvU32 port_offset;
} NVSWITCH_COLUMN_PORT_OFFSET_LS10;

typedef struct {
    NvU8      tcp;           // Tile column pair
    NvU8      tcpEPort;      // Port index within even column
    NvU8      tcpEVCHop;     // VC selection
    NvU8      tcpOPort;      // Port index within odd column
    NvU8      tcpOVCHop;     // VC selection
    NvU8      roundSize;     // This is no longer part of the hardware structure. We retain it here
                             // because it is useful in various loops
    NvU8      primaryReplica;// This field is not in hardware. This code uses it to
                             // track which port should be primary, so that it can make a pass over
                             // the assembled tcp directive list and adjust portFlag and
                             // continueRound as needed to indicate primary replica
                             // valid values are:
                             //         PRIMARY_REPLICA_NONE (0b00): no primary replica in tcp
                             //         PRIMARY_REPLICA_EVEN (0b01): even (0) port is primary replica
                             //         PRIMARY_REPLICA_ODD  (0b10): odd  (1) port is primary replica
    NvBool    tcpEAltPath :1;// Alternative to select from odd column
    NvBool    tcpOAltPath :1;// Alternative to select from even column
    NvBool    lastRound   :1;// last TCP directive of the last round in this multicast string
                             // could be multiple strings in case of spray
    NvBool    continueRound:1;// dual meaning:
                             // 1) if lastRound = 1 and continueRound = 1, primary replica is in
                             // this TCP directive and portFlag = 0/1 selects even/odd port
                             // 2) if lastRound = 0 there are more TCP directives for this round.
    NvBool    portFlag    :1;// triple meaning:
                             // 1) if lastRound = 1 and continueRound = 1, primary replica is in
                             // this TCP directive and portFlag = 0/1 selects even/odd port
                             // 2) If the previous TCP directive was not used to select the even/odd
                             // port of its predecessor, and if portFlag of the previous TCP
                             // directive = 1, portFlag of this TCP directive = 0/1 selects
                             // the even/odd port of its predecessor
                             // 3) if the previous TCP directive's portFlag = 0, and if it was not
                             // used to select the even or odd port of its predecessor, this TCP
                             // directive's portFlag == 1, this TCP directive contains the
                             // primary replica, and the next TCP directive's portFlag = 0/1
                             // selects the even/odd port of this TCP directive
} NVSWITCH_TCP_DIRECTIVE_LS10;

typedef struct {
    NvU8        index;
    NvBool      use_extended_table;
    NvU8        mcpl_size;
    NvU8        num_spray_groups;
    NvU8        ext_ptr;
    NvBool      no_dyn_rsp;
    NvBool      ext_ptr_valid;
    NvBool      valid;
    NVSWITCH_TCP_DIRECTIVE_LS10 directives[NVSWITCH_MC_TCP_LIST_SIZE_LS10];
    NvU8        spray_group_ptrs[NVSWITCH_MC_MAX_SPRAY_LS10];
} NVSWITCH_MC_RID_ENTRY_LS10;

NvlStatus nvswitch_mc_build_mcp_list_ls10(nvswitch_device *device, NvU32 *port_list,
                                            NvU32 *ports_per_spray_string,
                                            NvU32 *pri_replica_offsets, NvBool *replica_valid_array,
                                            NvU8 *vchop_array,
                                            NVSWITCH_MC_RID_ENTRY_LS10 *table_entry,
                                            NvU32 *entries_used);

NvlStatus nvswitch_mc_unwind_directives_ls10(nvswitch_device *device,
                                             NVSWITCH_TCP_DIRECTIVE_LS10 directives[NVSWITCH_MC_TCP_LIST_SIZE_LS10],
                                             NvU32 ports[NVSWITCH_MC_MAX_PORTS],
                                             NvU8 vc_hop[NVSWITCH_MC_MAX_PORTS],
                                             NvU32 ports_per_spray_group[NVSWITCH_MC_MAX_SPRAYGROUPS],
                                             NvU32 replica_offset[NVSWITCH_MC_MAX_SPRAYGROUPS],
                                             NvBool replica_valid[NVSWITCH_MC_MAX_SPRAYGROUPS]);

NvlStatus nvswitch_mc_invalidate_mc_rid_entry_ls10(nvswitch_device *device, NvU32 port, NvU32 index,
                                                    NvBool use_extended_table, NvBool zero);

NvlStatus nvswitch_mc_program_mc_rid_entry_ls10(nvswitch_device *device, NvU32 port,
                                                NVSWITCH_MC_RID_ENTRY_LS10 *table_entry,
                                                NvU32 directive_list_size);

NvlStatus nvswitch_mc_read_mc_rid_entry_ls10(nvswitch_device *device, NvU32 port,
                                             NVSWITCH_MC_RID_ENTRY_LS10 *table_entry);
#endif //_MULTICAST_LS10_H_
