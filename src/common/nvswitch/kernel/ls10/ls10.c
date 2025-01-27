/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink_export.h"
#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "regkey_nvswitch.h"
#include "haldef_nvswitch.h"
#include "nvlink_inband_msg.h"
#include "rmsoecmdif.h"

#include "ls10/ls10.h"
#include "lr10/lr10.h"
#include "ls10/clock_ls10.h"
#include "ls10/inforom_ls10.h"
#include "ls10/minion_ls10.h"
#include "ls10/pmgr_ls10.h"
#include "ls10/therm_ls10.h"
#include "ls10/smbpbi_ls10.h"
#include "ls10/cci_ls10.h"
#include "cci/cci_nvswitch.h"
#include "ls10/multicast_ls10.h"
#include "ls10/soe_ls10.h"
#include "ls10/gfw_ls10.h"

#include "nvswitch/ls10/dev_nvs_top.h"
#include "nvswitch/ls10/ptop_discovery_ip.h"
#include "nvswitch/ls10/dev_pri_masterstation_ip.h"
#include "nvswitch/ls10/dev_pri_hub_sys_ip.h"
#include "nvswitch/ls10/dev_nvlw_ip.h"
#include "nvswitch/ls10/dev_nvlsaw_ip.h"
#include "nvswitch/ls10/dev_nvlsaw_ip_addendum.h"
#include "nvswitch/ls10/dev_nvltlc_ip.h"
#include "nvswitch/ls10/dev_nvldl_ip.h"
#include "nvswitch/ls10/dev_nport_ip.h"
#include "nvswitch/ls10/dev_route_ip.h"
#include "nvswitch/ls10/dev_nport_ip_addendum.h"
#include "nvswitch/ls10/dev_route_ip_addendum.h"
#include "nvswitch/ls10/dev_ingress_ip.h"
#include "nvswitch/ls10/dev_egress_ip.h"
#include "nvswitch/ls10/dev_tstate_ip.h"
#include "nvswitch/ls10/dev_sourcetrack_ip.h"
#include "nvswitch/ls10/dev_cpr_ip.h"
#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"
#include "nvswitch/ls10/dev_minion_ip_addendum.h" 
#include "nvswitch/ls10/dev_multicasttstate_ip.h"
#include "nvswitch/ls10/dev_reductiontstate_ip.h"
#include "ls10/minion_nvlink_defines_public_ls10.h"
#include "nvswitch/ls10/dev_pmgr.h"
#include "nvswitch/ls10/dev_timer_ip.h"

#define NVSWITCH_IFR_MIN_BIOS_VER_LS10      0x9610170000ull
#define NVSWITCH_SMBPBI_MIN_BIOS_VER_LS10   0x9610220000ull

void *
nvswitch_alloc_chipdevice_ls10
(
    nvswitch_device *device
)
{
    void *chip_device;

    chip_device = nvswitch_os_malloc(sizeof(ls10_device));
    if (NULL != chip_device)
    {
        nvswitch_os_memset(chip_device, 0, sizeof(ls10_device));
    }

    device->chip_id = NV_PMC_BOOT_42_CHIP_ID_LS10;
    return(chip_device);
}

/*
 * @Brief : Initializes the PRI Ring
 *
 * @Description : An example of a function that we'd like to generate from SU.
 *
 * @paramin device    a reference to the device to initialize
 *
 * @returns             NVL_SUCCESS if the action succeeded
 */
NvlStatus
nvswitch_pri_ring_init_ls10
(
    nvswitch_device *device
)
{
    NvU32 checked_data;
    NvU32 command;
    NvBool keepPolling;
    NVSWITCH_TIMEOUT timeout;

    if (!IS_FMODEL(device))
    {
        // check if FSP successfully started
        nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            command = NVSWITCH_REG_RD32(device, _GFW_GLOBAL, _BOOT_PARTITION_PROGRESS);
            if (FLD_TEST_DRF(_GFW_GLOBAL, _BOOT_PARTITION_PROGRESS, _VALUE, _SUCCESS, command))
            {
                break;
            }

            nvswitch_os_sleep(1);
        }
        while (keepPolling);
        if (!FLD_TEST_DRF(_GFW_GLOBAL, _BOOT_PARTITION_PROGRESS, _VALUE, _SUCCESS, command))
        {
            NVSWITCH_RAW_ERROR_LOG_TYPE report = {0, { 0 }};
            NVSWITCH_RAW_ERROR_LOG_TYPE report_saw = {0, { 0 }};
            NvU32 report_idx = 0;
            NvU32 i;

            report.data[report_idx++] = command;
            NVSWITCH_PRINT(device, ERROR, "%s: -- _GFW_GLOBAL, _BOOT_PARTITION_PROGRESS (0x%x) != _SUCCESS --\n",
                __FUNCTION__, command);

            for (i = 0; i <= 15; i++)
            {
                command = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _SW_SCRATCH(i));
                report_saw.data[i] = command;
                NVSWITCH_PRINT(device, ERROR, "%s: -- NV_NVLSAW_SW_SCRATCH(%d) = 0x%08x\n",
                    __FUNCTION__, i, command);
            }

            for (i = 0; i < NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2__SIZE_1; i++)
            {
                command = NVSWITCH_REG_RD32(device, _PFSP, _FALCON_COMMON_SCRATCH_GROUP_2(i));
                report.data[report_idx++] = command;
                    NVSWITCH_PRINT(device, ERROR, "%s: -- NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(%d) = 0x%08x\n",
                    __FUNCTION__, i, command);
            }

            // Include useful scratch information for triage
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_FIRMWARE_INITIALIZATION_FAILURE,
                "Fatal, Firmware initialization failure (0x%x/0x%x, 0x%x, 0x%x, 0x%x/0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
                report.data[0], report.data[1], report.data[2], report.data[3], report.data[4],
                report_saw.data[0], report_saw.data[1], report_saw.data[12], report_saw.data[14], report_saw.data[15]);
            return -NVL_INITIALIZATION_TOTAL_FAILURE;
        }

        command = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRI_RING_INIT);
        if (FLD_TEST_DRF(_PPRIV_SYS, _PRI_RING_INIT, _STATUS, _ALIVE, command))
        {
            // _STATUS == ALIVE. Skipping
            return NVL_SUCCESS;
        }

        if (!FLD_TEST_DRF(_PPRIV_SYS, _PRI_RING_INIT, _STATUS, _ALIVE_IN_SAFE_MODE, command))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: -- Initial _STATUS (0x%x) != _ALIVE_IN_SAFE_MODE --\n",
                __FUNCTION__, DRF_VAL(_PPRIV_SYS, _PRI_RING_INIT, _STATUS, command));
            return -NVL_ERR_GENERIC;
        }

        // .Switch PRI Ring Init Sequence

        // *****

        // . [SW] Enumerate and start the PRI Ring

        NVSWITCH_ENG_WR32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRI_RING_INIT,
                               DRF_DEF(_PPRIV_SYS, _PRI_RING_INIT, _CMD, _ENUMERATE_AND_START));

        // . [SW] Wait for the command to complete

        if (IS_EMULATION(device))
        {
            nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
        }
        else
        {
            nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
        }

        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;
            command = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRI_RING_INIT);

            if ( FLD_TEST_DRF(_PPRIV_SYS,_PRI_RING_INIT,_CMD,_NONE,command) )
            {
                break;
            }
            if ( keepPolling == NV_FALSE )
            {
                NVSWITCH_PRINT(device, ERROR, "%s: -- Timeout waiting for _CMD == _NONE --\n", __FUNCTION__);
                return -NVL_ERR_GENERIC;
            }
        }
        while (keepPolling);

        // . [SW] Confirm PRI Ring initialized properly. Executing four reads to introduce a delay.

        if (IS_EMULATION(device))
        {
            nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
        }
        else
        {
            nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
        }

        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;
            command = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRI_RING_INIT);

            if ( FLD_TEST_DRF(_PPRIV_SYS, _PRI_RING_INIT, _STATUS, _ALIVE, command) )
            {
                break;
            }
            if ( keepPolling == NV_FALSE )
            {
                NVSWITCH_PRINT(device, ERROR, "%s: -- Timeout waiting for _STATUS == _ALIVE --\n", __FUNCTION__);
                return -NVL_ERR_GENERIC;
            }
        }
        while (keepPolling);

        // . [SW] PRI Ring Interrupt Status0 and Status1 should be clear unless there was an error.

        checked_data = NVSWITCH_ENG_RD32(device, PRI_MASTER_RS, , 0, _PPRIV_MASTER, _RING_INTERRUPT_STATUS0);
        if ( !FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0, _DISCONNECT_FAULT, 0x0, checked_data) )
        {
            NVSWITCH_PRINT(device, ERROR, "%s: _PPRIV_MASTER,_RING_INTERRUPT_STATUS0,_DISCONNECT_FAULT != 0x0\n", __FUNCTION__);
        }
        if ( !FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0, _GBL_WRITE_ERROR_FBP, 0x0, checked_data) )
        {
            NVSWITCH_PRINT(device, ERROR, "%s: _PPRIV_MASTER,_RING_INTERRUPT_STATUS0,_GBL_WRITE_ERROR_FBP != 0x0\n", __FUNCTION__);
        }
        if ( !FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0, _GBL_WRITE_ERROR_SYS, 0x0, checked_data) )
        {
            NVSWITCH_PRINT(device, ERROR, "%s: _PPRIV_MASTER,_RING_INTERRUPT_STATUS0,_GBL_WRITE_ERROR_SYS != 0x0\n", __FUNCTION__);
        }
        if ( !FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0, _OVERFLOW_FAULT, 0x0, checked_data) )
        {
            NVSWITCH_PRINT(device, ERROR, "%s: _PPRIV_MASTER,_RING_INTERRUPT_STATUS0,_OVERFLOW_FAULT != 0x0\n", __FUNCTION__);
        }
        if ( !FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0, _RING_START_CONN_FAULT, 0x0, checked_data) )
        {
            NVSWITCH_PRINT(device, ERROR, "%s: _PPRIV_MASTER,_RING_INTERRUPT_STATUS0,_RING_START_CONN_FAULT != 0x0\n", __FUNCTION__);
        }

        checked_data = NVSWITCH_ENG_RD32(device, PRI_MASTER_RS, , 0, _PPRIV_MASTER, _RING_INTERRUPT_STATUS1);
        if ( !FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS1, _GBL_WRITE_ERROR_GPC, 0x0, checked_data) )
        {
            NVSWITCH_PRINT(device, ERROR, "%s: _PPRIV_MASTER,_RING_INTERRUPT_STATUS1,_GBL_WRITE_ERROR_GPC != 0x0\n", __FUNCTION__);
        }

        // *****
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Destroys an NvSwitch hardware state
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 */
void
nvswitch_destroy_device_state_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    if (NVSWITCH_ENG_VALID_LS10(device, SOE, 0) && nvswitch_is_soe_supported(device))
    {
        nvswitch_soe_unregister_events(device);
        nvswitch_unload_soe_ls10(device);
    }

    if (chip_device != NULL)
    {
        if ((chip_device->latency_stats) != NULL)
        {
            nvswitch_os_free(chip_device->latency_stats);
        }

        if ((chip_device->ganged_link_table) != NULL)
        {
            nvswitch_os_free(chip_device->ganged_link_table);
        }

        nvswitch_free_chipdevice(device);
    }

    nvswitch_i2c_destroy(device);

    return;
}

NvlStatus
nvswitch_initialize_pmgr_ls10
(
    nvswitch_device *device
)
{
    // Init PMGR info
    nvswitch_init_pmgr_ls10(device);
    nvswitch_init_pmgr_devices_ls10(device);

    return NVL_SUCCESS;
}


NvlStatus
nvswitch_initialize_ip_wrappers_ls10
(
    nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;

    //
    // Now that software knows the devices and addresses, it must take all
    // the wrapper modules out of reset.
    //

    status = nvswitch_nvs_top_prod_ls10(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: TOP PROD initialization failed.\n",
            __FUNCTION__);
        return status;
    }

    status = nvswitch_apply_prod_nvlw_ls10(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NVLW PROD initialization failed.\n",
            __FUNCTION__);
        return status;
    }

    status = nvswitch_apply_prod_nxbar_ls10(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NXBAR PROD initialization failed.\n",
            __FUNCTION__);
        return status;
    }

    return status;
}

void
nvswitch_set_ganged_link_table_ls10
(
    nvswitch_device *device,
    NvU32            firstIndex,
    NvU64           *ganged_link_table,
    NvU32            numEntries
)
{
    NvU32 i;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return;
    }

    NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _ROUTE, _REG_TABLE_ADDRESS,
        DRF_NUM(_ROUTE, _REG_TABLE_ADDRESS, _INDEX, firstIndex) |
        DRF_NUM(_ROUTE, _REG_TABLE_ADDRESS, _AUTO_INCR, 1));

    for (i = 0; i < numEntries; i++)
    {
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _ROUTE, _REG_TABLE_DATA1,
            NvU64_HI32(ganged_link_table[i]));

        // HW will fill in the ECC
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _ROUTE, _REG_TABLE_DATA2,
            0);

        //
        // Writing DATA0 triggers the latched data to be written to the table
        // So write it last
        //
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _ROUTE, _REG_TABLE_DATA0,
            NvU64_LO32(ganged_link_table[i]));
    }
}

static NvlStatus
_nvswitch_init_ganged_link_routing_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32        gang_size;
    NvU64        gang_entry;
    NvU32        glt_entries = 16;
    NvU32        glt_size = (NV_ROUTE_REG_TABLE_ADDRESS_INDEX_GLTAB_DEPTH + 1);
    NvU64        *ganged_link_table = NULL;
    NvU32        i;
    NvU32        glt_index;

    //
    // The ganged link routing table is composed of 256 entries of 64-bits in
    // size.  Each entry is divided into 16 4-bit fields GLX(i), where GLX(x)
    // contains the distribution pattern for x ports.  Zero ports is not a
    // valid configuration, so GLX(0) corresponds with 16 ports.
    // Each GLX(i) column therefore should contain a uniform distribution
    // pattern for i ports.
    //
    // The ganged link routing table will be loaded with following values:
    // (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
    // (1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
    // (2,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2),
    // (3,0,1,0,3,3,3,3,3,3,3,3,3,3,3,3),
    //  :
    // (E,0,0,2,2,4,2,2,6,2,4,1,2,7,2,E),
    // (F,0,1,0,3,0,3,3,7,3,5,2,3,8,3,0)
    //
    // Refer table 22: Definition of size bits used with Ganged Link Number Table.
    //

    //Alloc memory for Ganged Link Table
    ganged_link_table = nvswitch_os_malloc(glt_size * sizeof(gang_entry));
    if (ganged_link_table == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to allocate memory for GLT!!\n");
        return -NVL_NO_MEM;
    }

    for (glt_index = 0; glt_index < glt_size; glt_index++)
    {
        gang_entry = 0;
        for (i = 0; i < glt_entries; i++)
        {
            gang_size = ((i==0) ? 16 : i);
            gang_entry |=
                DRF_NUM64(_ROUTE, _REG_TABLE_DATA0, _GLX(i), glt_index % gang_size);
        }

        ganged_link_table[glt_index] = gang_entry;
    }

    nvswitch_set_ganged_link_table_ls10(device, 0, ganged_link_table, glt_size);

    chip_device->ganged_link_table = ganged_link_table;

    return NVL_SUCCESS;
}

static void
_nvswitch_init_cmd_routing_ls10
(
    nvswitch_device *device
)
{
    NvU32 val;

    //Set Hash policy for the requests.
    val = DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN1, _SPRAY) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN2, _SPRAY) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN4, _SPRAY) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN7, _SPRAY);
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _ROUTE, _CMD_ROUTE_TABLE0, val);

    // Set Random policy for reponses.
    val = DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE2, _RFUN16, _RANDOM) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE2, _RFUN17, _RANDOM);
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _ROUTE, _CMD_ROUTE_TABLE2, val);
}

static NvlStatus
_nvswitch_init_portstat_counters_ls10
(
    nvswitch_device *device
)
{
    NvlStatus retval;
    NvU32 idx_channel;
    NVSWITCH_SET_LATENCY_BINS default_latency_bins;
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->latency_stats = nvswitch_os_malloc(sizeof(NVSWITCH_LATENCY_STATS_LS10));
    if (chip_device->latency_stats == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed allocate memory for latency stats\n",
            __FUNCTION__);
        return -NVL_NO_MEM;
    }

    nvswitch_os_memset(chip_device->latency_stats, 0, sizeof(NVSWITCH_LATENCY_STATS_LS10));

    //
    // These bin thresholds are values provided by Arch based off
    // switch latency expectations.
    //
    for (idx_channel=0; idx_channel < NVSWITCH_NUM_VCS_LS10; idx_channel++)
    {
        default_latency_bins.bin[idx_channel].lowThreshold = 120;    // 120ns
        default_latency_bins.bin[idx_channel].medThreshold = 200;    // 200ns
        default_latency_bins.bin[idx_channel].hiThreshold  = 1000;   // 1us
    }

    //
    // 6 hour sample interval
    // The 48-bit counters can theoretically rollover after ~12 hours of full
    // throttle traffic.
    //
    chip_device->latency_stats->sample_interval_msec = 6 * 60 * 60 * 1000;

    retval = nvswitch_ctrl_set_latency_bins(device, &default_latency_bins);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to set latency bins\n",
            __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return retval;
    }

    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _PORTSTAT_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_CONTROL, _RANGESELECT, _BITS13TO0));

    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _PORTSTAT_SOURCE_FILTER_0,
        DRF_NUM(_NPORT, _PORTSTAT_SOURCE_FILTER_0, _SRCFILTERBIT, 0xFFFFFFFF));

    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _PORTSTAT_SOURCE_FILTER_1,
        DRF_NUM(_NPORT, _PORTSTAT_SOURCE_FILTER_1, _SRCFILTERBIT, 0xFFFFFFFF));

    NVSWITCH_SAW_WR32_LS10(device, _NVLSAW, _GLBLLATENCYTIMERCTRL,
        DRF_DEF(_NVLSAW, _GLBLLATENCYTIMERCTRL, _ENABLE, _ENABLE));

    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _PORTSTAT_SNAP_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _DISABLE));

    // Start & Clear Residency Counters
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL,
        DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL,
        DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));

    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL,
        DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL,
        DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));

    // Start & Clear Stall/Busy Counters
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL,
        DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL,
        DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));

    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL,
        DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL,
        DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
        DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_initialize_route_ls10
(
    nvswitch_device *device
)
{
    NvlStatus retval;

    retval = _nvswitch_init_ganged_link_routing_ls10(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to initialize GLT\n",
            __FUNCTION__);
        goto nvswitch_initialize_route_exit;
    }

    _nvswitch_init_cmd_routing_ls10(device);

    // Initialize Portstat Counters
    retval = _nvswitch_init_portstat_counters_ls10(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to initialize portstat counters\n",
            __FUNCTION__);
        goto nvswitch_initialize_route_exit;
    }

    // TODO: Setup multicast/reductions

nvswitch_initialize_route_exit:
    return retval;
}

NvlStatus
nvswitch_ctrl_get_counters_ls10
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_GET_COUNTERS_PARAMS *ret
)
{
    nvlink_link *link;
    NvU8   i;
    NvU32  counterMask;
    NvU32  data;
    NvU32  val;
    NvU64  tx0TlCount;
    NvU64  tx1TlCount;
    NvU64  rx0TlCount;
    NvU64  rx1TlCount;
    NvU32  laneId;
    NvBool bLaneReversed;
    NvlStatus status;
    NvBool minion_enabled;

    ct_assert(NVSWITCH_NUM_LANES_LS10 <= NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE__SIZE);

    link = nvswitch_get_link(device, ret->linkId);
    if ((link == NULL) ||
        !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
    {
        return -NVL_BAD_ARGS;
    }

    minion_enabled = nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, link->linkNumber, MINION));

    counterMask = ret->counterMask;

    // Common usage allows one of these to stand for all of them
    if (counterMask & (NVSWITCH_NVLINK_COUNTER_TL_TX0 |
                       NVSWITCH_NVLINK_COUNTER_TL_TX1 |
                       NVSWITCH_NVLINK_COUNTER_TL_RX0 |
                       NVSWITCH_NVLINK_COUNTER_TL_RX1))
    {
        tx0TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_LO(0)),
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_HI(0)));
        if (NVBIT64(63) & tx0TlCount)
        {
            ret->bTx0TlCounterOverflow = NV_TRUE;
            tx0TlCount &= ~(NVBIT64(63));
        }

        tx1TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_LO(1)),
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_HI(1)));
        if (NVBIT64(63) & tx1TlCount)
        {
            ret->bTx1TlCounterOverflow = NV_TRUE;
            tx1TlCount &= ~(NVBIT64(63));
        }

        rx0TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_LO(0)),
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_HI(0)));
        if (NVBIT64(63) & rx0TlCount)
        {
            ret->bRx0TlCounterOverflow = NV_TRUE;
            rx0TlCount &= ~(NVBIT64(63));
        }

        rx1TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_LO(1)),
            NVSWITCH_LINK_OFFSET_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_HI(1)));
        if (NVBIT64(63) & rx1TlCount)
        {
            ret->bRx1TlCounterOverflow = NV_TRUE;
            rx1TlCount &= ~(NVBIT64(63));
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_TX0)] = tx0TlCount;
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_TX1)] = tx1TlCount;
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_RX0)] = rx0TlCount;
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_RX1)] = rx1TlCount;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT)
    {
        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_RX01, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _RX01, _FLIT_CRC_ERRORS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT)]
            = data;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_MASKED)
    {
        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_RX02, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _RX02, _MASKED_CRC_ERRORS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_MASKED)]
            = data;
    }
    data = 0x0;
    bLaneReversed = nvswitch_link_lane_reversed_ls10(device, link->linkNumber);

    for (laneId = 0; laneId < NVSWITCH_NUM_LANES_LS10; laneId++)
    {
        //
        // HW may reverse the lane ordering or it may be overridden by SW.
        // If so, invert the interpretation of the lane CRC errors.
        //
        i = (NvU8)((bLaneReversed) ? (NVSWITCH_NUM_LANES_LS10 - 1) - laneId : laneId);

        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_DB01, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(laneId))
        {
            val = BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(laneId));

            switch (i)
            {
                case 0:
                    ret->nvlinkCounters[val]
                        = DRF_VAL(_NVLSTAT, _DB01, _ERROR_COUNT_ERR_LANECRC_L0, data);
                    break;
                case 1:
                    ret->nvlinkCounters[val]
                        = DRF_VAL(_NVLSTAT, _DB01, _ERROR_COUNT_ERR_LANECRC_L1, data);
                    break;
            }
        }
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY)
    {
        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_TX09, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _TX09, _REPLAY_EVENTS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY)]
            = data;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY)
    {
        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_LNK1, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _LNK1, _ERROR_COUNT1_RECOVERY_EVENTS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY)]
            = data;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY)
    {
        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_RX00, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _RX00, _REPLAY_EVENTS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY)]
            = data;
    }

    if ((counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS) ||
        (counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL))
    {
        if (minion_enabled)
        {
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                NV_NVLSTAT_DB11, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        if (counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS)
        {
            ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS)]
                = DRF_VAL(_NVLSTAT_DB11, _COUNT_PHY_REFRESH, _PASS, data);
        }

        if (counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL)
        {
            ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL)]
                = DRF_VAL(_NVLSTAT_DB11, _COUNT_PHY_REFRESH, _FAIL, data);
        }
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_sw_info_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_SW_INFO_PARAMS *p
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU32 i;

    if (p->count > NVSWITCH_GET_SW_INFO_COUNT_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid args\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(p->info, 0, sizeof(NvU32)*NVSWITCH_GET_SW_INFO_COUNT_MAX);

    for (i = 0; i < p->count; i++)
    {
        switch (p->index[i])
        {
            case NVSWITCH_GET_SW_INFO_INDEX_INFOROM_NVL_SUPPORTED:
                p->info[i] = NV_TRUE;
                break;
            case NVSWITCH_GET_SW_INFO_INDEX_INFOROM_BBX_SUPPORTED:
                p->info[i] = NV_TRUE;
                break;
            default:
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Undefined NVSWITCH_GET_SW_INFO_INDEX 0x%x\n",
                    __FUNCTION__,
                    p->index[i]);
                retval = -NVL_BAD_ARGS;
                break;
        }
    }

    return retval;
}

static void
nvswitch_ctrl_clear_throughput_counters_ls10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvU32           counterMask
)
{
    NvU32 data;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLTLC, link->linkNumber))
    {
        return;
    }

    //
    // Common usage allows one of these to stand for all of them
    // If one field is defined: perform a clear on counters 0 & 1
    //

    if ((counterMask) & ( NVSWITCH_NVLINK_COUNTER_TL_TX0 |
                          NVSWITCH_NVLINK_COUNTER_TL_TX1 |
                          NVSWITCH_NVLINK_COUNTER_TL_RX0 |
                          NVSWITCH_NVLINK_COUNTER_TL_RX1 ))
    {
        // TX 0
        data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0(0));
        data = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _RESET, 0x1, data);
        NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0(0), data);

        // TX 1
        data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0(1));
        data = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _RESET, 0x1, data);
        NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0(1), data);

        // RX 0
        data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0(0));
        data = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _RESET, 0x1, data);
        NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0(0), data);

        // RX 1
        data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0(1));
        data = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _RESET, 0x1, data);
        NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0(1), data);
    }
}

static void
nvswitch_ctrl_clear_lp_counters_ls10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvU32           counterMask
)
{
    NvlStatus status;

    // Clears all LP counters
    if (counterMask & NVSWITCH_NVLINK_LP_COUNTERS_DL)
    {
        status = nvswitch_minion_send_command(device, link->linkNumber,
            NV_MINION_NVLINK_DL_CMD_COMMAND_DLSTAT_CLR_DLLPCNT, 0);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s : Failed to clear lp counts to MINION for link # %d\n",
                __FUNCTION__, link->linkNumber);
        }
    }
}

static NvlStatus
nvswitch_ctrl_clear_dl_error_counters_ls10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvU32            counterMask
)
{
    NvU32           data;

    if ((!counterMask) ||
        (!(counterMask & (NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_ECC_COUNTS |
                          NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY |
                          NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY))))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Link%d: No error count clear request, counterMask (0x%x). Returning!\n",
            __FUNCTION__, link->linkNumber, counterMask);
        return NVL_SUCCESS;
    }

    // With Minion initialized, send command to minion
    if (nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, link->linkNumber, MINION)))
    {
        return nvswitch_minion_clear_dl_error_counters_ls10(device, link->linkNumber);
    }

    // With Minion not-initialized, perform with the registers
    if (counterMask & (NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6 |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7 |
                       NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY      |
                       NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY    |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT    |
                       NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_MASKED  ))
    {
        data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_LANE_CRC, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_FLIT_CRC, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_TX, _ERROR_COUNT_CTRL, _CLEAR_REPLAY, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_TOP, _ERROR_COUNT_CTRL, _CLEAR_RECOVERY, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_RATES, _CLEAR, data);
        NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL, data);
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_ECC_COUNTS)
    {
        data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_LANE_CRC, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_RATES, _CLEAR, data);        
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_ECC_COUNTS, _CLEAR, data);
    }

    return NVL_SUCCESS;
}

static void
_nvswitch_portstat_reset_latency_counters_ls10
(
    nvswitch_device *device
)
{
    // Set SNAPONDEMAND from 0->1 to reset the counters
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _PORTSTAT_SNAP_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _ENABLE));

    // Set SNAPONDEMAND back to 0.
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _PORTSTAT_SNAP_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _DISABLE));
}

/*
 *  Disable interrupts comming from NPG & NVLW blocks.
 */
void
nvswitch_link_disable_interrupts_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    NvU32 localLinkIdx, instance;

    instance     = link / NVSWITCH_LINKS_PER_NVLIPT_LS10;
    localLinkIdx = link % NVSWITCH_LINKS_PER_NVLIPT_LS10;

    if (nvswitch_is_soe_supported(device))
    {
        nvswitch_soe_set_nport_interrupts_ls10(device, link, NV_FALSE);
    }
    else
    {
        NVSWITCH_NPORT_WR32_LS10(device, link, _NPORT, _ERR_CONTROL_COMMON_NPORT,
            DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _CORRECTABLEENABLE, 0x0) |
            DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _FATALENABLE,       0x0) |
            DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _NONFATALENABLE,    0x0));
    }

    NVSWITCH_ENG_WR32(device, NVLW, , instance, _NVLW, _LINK_INTR_0_MASK(localLinkIdx),
        DRF_NUM(_NVLW, _LINK_INTR_0_MASK, _FATAL,       0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_0_MASK, _NONFATAL,    0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_0_MASK, _CORRECTABLE, 0x0));

    NVSWITCH_ENG_WR32(device, NVLW, , instance, _NVLW, _LINK_INTR_1_MASK(localLinkIdx),
        DRF_NUM(_NVLW, _LINK_INTR_1_MASK, _FATAL,       0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_1_MASK, _NONFATAL,    0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_1_MASK, _CORRECTABLE, 0x0));

    NVSWITCH_ENG_WR32(device, NVLW, , instance, _NVLW, _LINK_INTR_2_MASK(localLinkIdx),
        DRF_NUM(_NVLW, _LINK_INTR_2_MASK, _FATAL,       0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_2_MASK, _NONFATAL,    0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_2_MASK, _CORRECTABLE, 0x0));
}

/*
 *  Reset NPG & NVLW interrupt state.
 */
static void
_nvswitch_link_reset_interrupts_ls10
(
    nvswitch_device *device,
    NvU32 link
)
{
    NvU32 regval;
    NvU32 eng_instance = link / NVSWITCH_LINKS_PER_NVLIPT_LS10;
    NvU32 localLinkNum = link % NVSWITCH_LINKS_PER_NVLIPT_LS10;

    if (nvswitch_is_soe_supported(device))
    {
        nvswitch_soe_set_nport_interrupts_ls10(device, link, NV_TRUE);
    }
    else
    {
        NVSWITCH_NPORT_WR32_LS10(device, link, _NPORT, _ERR_CONTROL_COMMON_NPORT,
            DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _CORRECTABLEENABLE, 0x1) |
            DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _FATALENABLE, 0x1) |
            DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _NONFATALENABLE, 0x1));
    }

    NVSWITCH_ENG_WR32(device, NVLW, , eng_instance, _NVLW, _LINK_INTR_0_MASK(localLinkNum),
        DRF_NUM(_NVLW, _LINK_INTR_0_MASK, _FATAL, 0x1) |
        DRF_NUM(_NVLW, _LINK_INTR_0_MASK, _NONFATAL, 0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_0_MASK, _CORRECTABLE, 0x0) |
        DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _INTR0,       0x1) |
        DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _INTR1,       0x0));

    NVSWITCH_ENG_WR32(device, NVLW, , eng_instance, _NVLW, _LINK_INTR_1_MASK(localLinkNum),
        DRF_NUM(_NVLW, _LINK_INTR_1_MASK, _FATAL, 0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_1_MASK, _NONFATAL, 0x1) |
        DRF_NUM(_NVLW, _LINK_INTR_1_MASK, _CORRECTABLE, 0x1) |
        DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _INTR0,       0x0) |
        DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _INTR1,       0x1));

    NVSWITCH_ENG_WR32(device, NVLW, , eng_instance, _NVLW, _LINK_INTR_2_MASK(localLinkNum),
        DRF_NUM(_NVLW, _LINK_INTR_2_MASK, _FATAL, 0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_2_MASK, _NONFATAL, 0x0) |
        DRF_NUM(_NVLW, _LINK_INTR_2_MASK, _CORRECTABLE, 0x0) |
        DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _INTR0,       0x0) |
        DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _INTR1,       0x0));

    // NVLIPT_LNK
    regval = NVSWITCH_LINK_RD32_LS10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _INTR_CONTROL_LINK);
    regval = regval |
             DRF_NUM(_NVLIPT_LNK, _INTR_CONTROL_LINK, _INT0_EN, 0x1) |
             DRF_NUM(_NVLIPT_LNK, _INTR_CONTROL_LINK, _INT1_EN, 0x1);
    NVSWITCH_LINK_WR32_LS10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _INTR_CONTROL_LINK, regval);

    // NVLIPT_LNK_INTR_1
    regval = NVSWITCH_LINK_RD32_LS10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _INTR_INT1_EN);
    regval = regval | DRF_NUM(_NVLIPT_LNK, _INTR_INT1_EN, _LINKSTATEREQUESTREADYSET, 0x1);
    NVSWITCH_LINK_WR32_LS10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _INTR_INT1_EN, regval);

    // Clear fatal error status
    device->link[link].fatal_error_occurred = NV_FALSE;
}

//
// Data collector which runs on a background thread, collecting latency stats.
//
// The latency counters have a maximum window period of about 12 hours
// (2^48 clk cycles). The counters reset after this period. So SW snaps
// the bins and records latencies every 6 hours. Setting SNAPONDEMAND from 0->1
// snaps the  latency counters and updates them to PRI registers for
// the SW to read. It then resets the counters to start collecting fresh latencies.
//

void
nvswitch_internal_latency_bin_log_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 idx_nport;
    NvU32 idx_vc;
    NvBool vc_valid;
    NvU64 lo, hi;
    NvU64 latency;
    NvU64 time_nsec;
    NvU32 link_type;    // Access or trunk link
    NvU64 last_visited_time_nsec;

    if (chip_device->latency_stats == NULL)
    {
        // Latency stat buffers not allocated yet
        return;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return;
    }

    time_nsec = nvswitch_os_get_platform_time();
    last_visited_time_nsec = chip_device->latency_stats->last_visited_time_nsec;

    // Update last visited time
    chip_device->latency_stats->last_visited_time_nsec = time_nsec;

    // Compare time stamp and reset the counters if the snap is missed
    if (!IS_RTLSIM(device) || !IS_FMODEL(device))
    {
        if ((last_visited_time_nsec != 0) &&
            ((time_nsec - last_visited_time_nsec) >
             chip_device->latency_stats->sample_interval_msec * NVSWITCH_INTERVAL_1MSEC_IN_NS))
        {
            NVSWITCH_PRINT(device, ERROR,
                "Latency metrics recording interval missed.  Resetting counters.\n");
            _nvswitch_portstat_reset_latency_counters_ls10(device);
            return;
        }
    }

    for (idx_nport=0; idx_nport < NVSWITCH_LINK_COUNT(device); idx_nport++)
    {
        if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, idx_nport))
        {
            continue;
        }

        // Setting SNAPONDEMAND from 0->1 snaps the latencies and resets the counters
        NVSWITCH_LINK_WR32_LS10(device, idx_nport, NPORT, _NPORT, _PORTSTAT_SNAP_CONTROL,
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _ENABLE));

        link_type = NVSWITCH_LINK_RD32_LS10(device, idx_nport, NPORT, _NPORT, _CTRL);
        for (idx_vc = 0; idx_vc < NVSWITCH_NUM_VCS_LS10; idx_vc++)
        {
            vc_valid = NV_FALSE;

            // VC's CREQ0(0) and RSP0(5) are relevant on access links.
            if (FLD_TEST_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _ACCESSLINK, link_type) &&
                ((idx_vc == NV_NPORT_VC_MAPPING_CREQ0) ||
                (idx_vc == NV_NPORT_VC_MAPPING_RSP0)))
            {
                vc_valid = NV_TRUE;
            }

            // VC's CREQ0(0), RSP0(5), CREQ1(6) and RSP1(7) are relevant on trunk links.
            if (FLD_TEST_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _TRUNKLINK, link_type) &&
                ((idx_vc == NV_NPORT_VC_MAPPING_CREQ0)  ||
                 (idx_vc == NV_NPORT_VC_MAPPING_RSP0)   ||
                 (idx_vc == NV_NPORT_VC_MAPPING_CREQ1)  ||
                 (idx_vc == NV_NPORT_VC_MAPPING_RSP1)))
            {
                vc_valid = NV_TRUE;
            }

            // If the VC is not being used, skip reading it
            if (!vc_valid)
            {
                continue;
            }

            lo = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _LOW, _0, idx_vc);
            hi = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _LOW, _1, idx_vc);
            latency = lo | (hi << 32);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].low += latency;

            lo = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _MEDIUM, _0, idx_vc);
            hi = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _MEDIUM, _1, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].medium += latency;
            latency = lo | (hi << 32);

            lo = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _HIGH, _0, idx_vc);
            hi = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _HIGH, _1, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].high += latency;
            latency = lo | (hi << 32);

            lo = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _PANIC, _0, idx_vc);
            hi = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _COUNT, _PANIC, _1, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].panic += latency;
            latency = lo | (hi << 32);

            lo = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _PACKET, _COUNT, _0, idx_vc);
            hi = NVSWITCH_NPORT_PORTSTAT_RD32_LS10(device, idx_nport, _PACKET, _COUNT, _1, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].count += latency;
            latency = lo | (hi << 32);

            // Note the time of this snap
            chip_device->latency_stats->latency[idx_vc].last_read_time_nsec = time_nsec;
            chip_device->latency_stats->latency[idx_vc].count++;
        }

        // Disable SNAPONDEMAND after fetching the latencies
        NVSWITCH_LINK_WR32_LS10(device, idx_nport, NPORT, _NPORT, _PORTSTAT_SNAP_CONTROL,
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _DISABLE));
    }
}

static NvlStatus
nvswitch_ctrl_set_ganged_link_table_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_GANGED_LINK_TABLE *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_init_npg_multicast_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_PRINT(device, WARN, "%s: Function not implemented\n", __FUNCTION__);
}

void
nvswitch_init_warm_reset_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_PRINT(device, WARN, "%s: Function not implemented\n", __FUNCTION__);
 }

//
// Helper funcction to query MINION to see if DL clocks are on
// return NV_TRUE if the clocks are on
//        NV_FALSE if the clocks are off
static
NvBool
_nvswitch_are_dl_clocks_on
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    NvU32 link_state;
    NvU32 stat_data;
    NvlStatus status = NVL_SUCCESS;
    nvlink_link * link= nvswitch_get_link(device, linkNumber);

    if (link == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: invalid link %d\n",
                       __FUNCTION__, linkNumber);
        return NV_FALSE;
    }

    status = nvswitch_minion_get_dl_status(device, linkNumber,
                NV_NVLSTAT_UC01, 0, &stat_data);
    if (status != NVL_SUCCESS)
    {
        return NV_FALSE;
    }

    link_state = DRF_VAL(_NVLSTAT, _UC01, _LINK_STATE, stat_data);
    switch(link_state)
    {
        case LINKSTATUS_RESET:
        case LINKSTATUS_UNINIT:
            return NV_FALSE;
        case LINKSTATUS_LANESHUTDOWN:
        case LINKSTATUS_ACTIVE_PENDING:
            return nvswitch_are_link_clocks_on_ls10(device, link,
                    NVSWITCH_PER_LINK_CLOCK_SET(RXCLK) | NVSWITCH_PER_LINK_CLOCK_SET(TXCLK));
    }

    return NV_TRUE;
}

static NvlStatus
_nvswitch_reset_and_drain_links_ls10
(
    nvswitch_device *device,
    NvU64 link_mask,
    NvBool bForced
)
{
    NvlStatus    status    = NVL_SUCCESS;
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    nvlink_link *link_info = NULL;
    NvU32        link;
    NvU32        data32;
    NvU32        retry_count = 3;
    NvU32 link_state_request;
    NvU32 link_state;
    NvU32 stat_data;
    NvU32 link_intr_subcode;
    NvBool bKeepPolling;
    NvBool bIsLinkInEmergencyShutdown;
    NvBool bAreDlClocksOn;
    NVSWITCH_TIMEOUT timeout;

    if (link_mask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Invalid link_mask 0\n",
                       __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // Check for inactive links
    FOR_EACH_INDEX_IN_MASK(64, link, link_mask)
    {
        if (!nvswitch_is_link_valid(device, link))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: link #%d invalid\n",
                           __FUNCTION__, link);
            continue;
        }

        if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, link))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: NPORT #%d invalid\n",
                           __FUNCTION__, link);  
            continue;
        }

        if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLW, link))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: NVLW #%d invalid\n",
                           __FUNCTION__, link);
            continue;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    FOR_EACH_INDEX_IN_MASK(64, link, link_mask)
    {
        link_info = nvswitch_get_link(device, link);
        if (link_info == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: invalid link %d\n",
                           __FUNCTION__, link);
            continue;
        }

        if (nvswitch_is_link_in_reset(device, link_info))
        {
            continue;
        }

        // Unregister links to make them unusable while reset is in progress.
        nvlink_lib_unregister_link(link_info);

        //
        // Step 1.0 : Check NXBAR error state. NXBAR errors are always fatal
        // errors and are assumed to require a full power-on reset to recover.
        // No incremental recovery is possible after a NXBAR error.
        //
        data32 = NVSWITCH_NPORT_RD32_LS10(device, link, _EGRESS, _ERR_STATUS_0);
        if (FLD_TEST_DRF(_EGRESS, _ERR_STATUS_0, _EGRESSBUFERR,            _CLEAR, data32) ||
            FLD_TEST_DRF(_EGRESS, _ERR_STATUS_0, _SEQIDERR,                _CLEAR, data32) ||
            FLD_TEST_DRF(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_LIMIT_ERR, _CLEAR, data32) ||
            FLD_TEST_DRF(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_DBE_ERR,   _CLEAR, data32) ||
            FLD_TEST_DRF(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_PARITY_ERR,    _CLEAR, data32))
        {
            NVSWITCH_PRINT(device, ERROR,
                           "%s: Fatal NXBAR error on link %d. Chip reset required\n",
                           __FUNCTION__, link);

            // Re-register links.
            status = nvlink_lib_register_link(device->nvlink_device, link_info);
            if (status != NVL_SUCCESS)
            {
                nvswitch_destroy_link(link_info);
            }

            continue;
        }

        //
        // Step 2.0 : Disable NPG & NVLW interrupts
        //
        nvswitch_link_disable_interrupts_ls10(device, link);

        //
        // Step 3.0 :
        // Prior to starting port reset, ensure the links is in emergency shutdown
        //
        // Forcibly shutdown links if requested
        //
        if (bForced)
        {
            nvswitch_execute_unilateral_link_shutdown_ls10(link_info);
        }
        else
        {
            bIsLinkInEmergencyShutdown = NV_FALSE;
            nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
            do
            {
                bKeepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

                status = nvswitch_minion_get_dl_status(device, link_info->linkNumber,
                            NV_NVLSTAT_UC01, 0, &stat_data);

                if (status != NVL_SUCCESS)
                {
                    continue;
                }

                link_state = DRF_VAL(_NVLSTAT, _UC01, _LINK_STATE, stat_data);

                bIsLinkInEmergencyShutdown = (link_state == LINKSTATUS_EMERGENCY_SHUTDOWN) ?
                                                NV_TRUE:NV_FALSE;

                if (bIsLinkInEmergencyShutdown == NV_TRUE)
                {
                    break;
                }
            }
            while(bKeepPolling);

            if (bIsLinkInEmergencyShutdown == NV_FALSE)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: link %d failed to enter emergency shutdown\n",
                    __FUNCTION__, link);
                    
                // Re-register links.
                status = nvlink_lib_register_link(device->nvlink_device, link_info);
                if (status != NVL_SUCCESS)
                {
                    nvswitch_destroy_link(link_info);
                }
                continue;
            }
        }

        nvswitch_corelib_clear_link_state_ls10(link_info);

        //
        // Step 4.0 : Send command to SOE to perform the following steps :
        // - Backup NPORT state before reset
        // - Assert debug_clear for the given port NPORT by writing to the
        //   DEBUG_CLEAR (0x144) register
        // - Assert NPortWarmReset[i] using the WARMRESET (0x140) register
        //
        nvswitch_soe_issue_nport_reset_ls10(device, link);

        //
        // Step 5.0 : Issue Minion request to perform the link reset sequence
        // We retry the Minion reset sequence 3 times, if we there is an error
        // while trying to reset the first few times. Refer Bug 3799577 for
        // more details.
        // 
        do
        {
            status = nvswitch_request_tl_link_state_ls10(link_info,
                     NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET, NV_TRUE);

            if (status == NVL_SUCCESS)
            {
                break;
            }
            else
            {

                link_state_request = NVSWITCH_LINK_RD32_LS10(device, link_info->linkNumber,
                                        NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_REQUEST);

                link_state = DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST, _STATUS,
                                        link_state_request);

                if (nvswitch_minion_get_dl_status(device, link_info->linkNumber,
                                  NV_NVLSTAT_MN00, 0, &stat_data) == NVL_SUCCESS)
                {
                    link_intr_subcode = DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_SUBCODE, stat_data);

                    if ((link_state == NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_STATUS_MINION_REQUEST_FAIL) &&
                        (link_intr_subcode == MINION_ALARM_BUSY))
                    {

                        //
                        // We retry the reset sequence when we see a MINION_REQUEST_FAIL
                        // or MINION_ALARM_BUSY
                        //
                    }
                    else
                    {
                        break;
                    }
                }
       
                retry_count--;
            }

        } while(retry_count);

        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: NvLink Reset has failed for link %d\n",
                __FUNCTION__, link);

            // Re-register links.
            status = nvlink_lib_register_link(device->nvlink_device, link_info);
            if (status != NVL_SUCCESS)
            {
                nvswitch_destroy_link(link_info);
            }
            continue;
        }

        //
        // Step 6.0 : Send command to SOE to perform the following steps :
        // - Clear the CONTAIN_AND_DRAIN (0x5c) status
        // - Assert NPORT INITIALIZATION and program the state tracking RAMS
        // - Restore NPORT state after reset
        //
        nvswitch_soe_restore_nport_state_ls10(device, link);

        // Step 7.0 : Re-program the routing table for DBEs
  
        // Step 8.0 : Reset NVLW and NPORT interrupt state
        _nvswitch_link_reset_interrupts_ls10(device, link);
  
        // Re-register links.
        status = nvlink_lib_register_link(device->nvlink_device, link_info);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to register link: 0x%x with the corelib\n",
                __FUNCTION__, link);
            nvswitch_destroy_link(link_info);
            continue;
        }

        // Reset NV_NPORT_SCRATCH_WARM_PORT_RESET_REQUIRED to 0x0
        NVSWITCH_LINK_WR32(device, link, NPORT, _NPORT, _SCRATCH_WARM, 0);

        //
        // Step 9.0: Launch ALI training to re-initialize and train the links
        // nvswitch_launch_ALI_link_training(device, link_info);
        //
        // Request active, but don't block. FM will come back and check
        // active link status by blocking on this TLREQ's completion
        //
        // CCI will re-train links
        if (!cciIsLinkManaged(device, link)) 
        {
            status = nvswitch_request_tl_link_state_ls10(link_info,
                        NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_ACTIVE,
                        NV_FALSE);

            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: TL link state request to active for ALI failed for link: 0x%x\n",
                    __FUNCTION__, link);
                continue;
            }
        }

        bAreDlClocksOn = NV_FALSE;
        nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
        do
        {
            bKeepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;
            bAreDlClocksOn = _nvswitch_are_dl_clocks_on(device, link);

            if (bAreDlClocksOn)
            {
                break;
            }
        }
        while(bKeepPolling);

        if (!bAreDlClocksOn)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: link: 0x%x doesn't have the TX/RX clocks on, skipping setting DL interrupts!\n",
                __FUNCTION__, link);
            continue;
        }

        nvswitch_set_dlpl_interrupts_ls10(link_info);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    chip_device->deferredLinkErrors[link].state.lastRetrainTime = nvswitch_os_get_platform_time();

    return NVL_SUCCESS;
}

//
// Implement reset and drain sequence for ls10
//
NvlStatus
nvswitch_reset_and_drain_links_ls10
(
    nvswitch_device *device,
    NvU64 link_mask,
    NvBool bForced
)
{
    NvlStatus    status    = NVL_SUCCESS;
    
    NvU32        link;
    
    // CCI will call reset and drain separately
    FOR_EACH_INDEX_IN_MASK(64, link, link_mask)
    {
        if (cciIsLinkManaged(device, link))
        {
            link_mask = link_mask & ~NVBIT64(link);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    status = _nvswitch_reset_and_drain_links_ls10(device, link_mask, bForced);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_set_nport_port_config_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_SWITCH_PORT_CONFIG *p
)
{
    NvU32   val;
    NvlStatus status = NVL_SUCCESS;

    if (p->requesterLinkID >= NVBIT(
        DRF_SIZE(NV_NPORT_REQLINKID_REQROUTINGID) +
        DRF_SIZE(NV_NPORT_REQLINKID_REQROUTINGID_UPPER)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid requester RID 0x%x\n",
            __FUNCTION__, p->requesterLinkID);
        return -NVL_BAD_ARGS;
    }

    if (p->requesterLanID > DRF_MASK(NV_NPORT_REQLINKID_REQROUTINGLAN))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid requester RLAN 0x%x\n",
            __FUNCTION__, p->requesterLanID);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _NPORT, _CTRL);
    switch (p->type)
    {
        case CONNECT_ACCESS_GPU:
        case CONNECT_ACCESS_CPU:
        case CONNECT_ACCESS_SWITCH:
            val = FLD_SET_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _ACCESSLINK, val);
            break;
        case CONNECT_TRUNK_SWITCH:
            val = FLD_SET_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _TRUNKLINK, val);
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: invalid type #%d\n",
                __FUNCTION__, p->type);
            return -NVL_BAD_ARGS;
    }

    // _ENDPOINT_COUNT deprecated on LS10

    NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _CTRL, val);

    NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _REQLINKID,
        DRF_NUM(_NPORT, _REQLINKID, _REQROUTINGID, p->requesterLinkID) |
        DRF_NUM(_NPORT, _REQLINKID, _REQROUTINGID_UPPER,
            p->requesterLinkID >> DRF_SIZE(NV_NPORT_REQLINKID_REQROUTINGID)) |
        DRF_NUM(_NPORT, _REQLINKID, _REQROUTINGLAN, p->requesterLanID));

    if (p->type == CONNECT_TRUNK_SWITCH)
    {
        if (!nvswitch_is_soe_supported(device))
        {
            // Set trunk specific settings (TPROD) on PRE-SILION

            // NPORT
            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _NPORT, _CTRL);
            val = FLD_SET_DRF(_NPORT, _CTRL, _EGDRAINENB, _DISABLE, val);
            val = FLD_SET_DRF(_NPORT, _CTRL, _ENEGRESSDBI, _ENABLE, val);
            val = FLD_SET_DRF(_NPORT, _CTRL, _ENROUTEDBI, _ENABLE, val);
            val = FLD_SET_DRF(_NPORT, _CTRL, _RTDRAINENB, _DISABLE, val);
            val = FLD_SET_DRF(_NPORT, _CTRL, _SPARE, _INIT, val);
            val = FLD_SET_DRF(_NPORT, _CTRL, _TRUNKLINKENB, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _CTRL, val);

            // EGRESS
            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _EGRESS, _CTRL);
            val = FLD_SET_DRF(_EGRESS, _CTRL, _CTO_ENB, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _EGRESS, _CTRL, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _EGRESS, _ERR_CONTAIN_EN_0);
            val = FLD_SET_DRF(_EGRESS, _ERR_CONTAIN_EN_0, _CREDIT_TIME_OUT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_CONTAIN_EN_0, _HWRSPERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_CONTAIN_EN_0, _INVALIDVCSET_ERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_CONTAIN_EN_0, _REQTGTIDMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_CONTAIN_EN_0, _RSPREQIDMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_CONTAIN_EN_0, _URRSPERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _EGRESS, _ERR_CONTAIN_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _EGRESS, _ERR_FATAL_REPORT_EN_0);
            val = FLD_SET_DRF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _CREDIT_TIME_OUT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _HWRSPERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _INVALIDVCSET_ERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _REQTGTIDMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _RSPREQIDMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _URRSPERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _EGRESS, _ERR_FATAL_REPORT_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _EGRESS, _ERR_LOG_EN_0);
            val = FLD_SET_DRF(_EGRESS, _ERR_LOG_EN_0, _CREDIT_TIME_OUT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_LOG_EN_0, _HWRSPERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_LOG_EN_0, _INVALIDVCSET_ERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_LOG_EN_0, _REQTGTIDMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_LOG_EN_0, _RSPREQIDMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_EGRESS, _ERR_LOG_EN_0, _URRSPERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _EGRESS, _ERR_LOG_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0);
            val = FLD_SET_DRF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _PRIVRSPERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0, val);

            // INGRESS
            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _INGRESS, _ERR_CONTAIN_EN_0);
            val = FLD_SET_DRF(_INGRESS, _ERR_CONTAIN_EN_0, _EXTAREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_CONTAIN_EN_0, _EXTBREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_CONTAIN_EN_0, _INVALIDVCSET, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_CONTAIN_EN_0, _MCREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_CONTAIN_EN_0, _REMAPTAB_ECC_DBE_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _INGRESS, _ERR_CONTAIN_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _INGRESS, _ERR_FATAL_REPORT_EN_0);
            val = FLD_SET_DRF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _EXTAREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _EXTBREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _INVALIDVCSET, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _MCREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _REMAPTAB_ECC_DBE_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _INGRESS, _ERR_FATAL_REPORT_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _INGRESS, _ERR_LOG_EN_0);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_0, _EXTAREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_0, _EXTBREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_0, _INVALIDVCSET, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_0, _MCREMAPTAB_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_0, _REMAPTAB_ECC_DBE_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _INGRESS, _ERR_LOG_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _INGRESS, _ERR_LOG_EN_1);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _EXTAREMAPTAB_ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _EXTAREMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _EXTBREMAPTAB_ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _EXTBREMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _MCCMDTOUCADDRERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _MCREMAPTAB_ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _MCREMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_LOG_EN_1, _READMCREFLECTMEMERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _INGRESS, _ERR_LOG_EN_1, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ACLFAIL, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ADDRBOUNDSERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_ACLFAIL, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_ADDRBOUNDSERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_INDEX_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_REQCONTEXTMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_ACLFAIL, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_ADDRBOUNDSERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_INDEX_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_REQCONTEXTMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_ACLFAIL, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_ADDRBOUNDSERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_INDEX_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_REQCONTEXTMISMATCHERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REQCONTEXTMISMATCHERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _INGRESS, _ERR_NON_FATAL_REPORT_EN_1);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTAREMAPTAB_ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTAREMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTBREMAPTAB_ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTBREMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCCMDTOUCADDRERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREMAPTAB_ADDRTYPEERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREMAPTAB_ECC_LIMIT_ERR, __TPROD, val);
            val = FLD_SET_DRF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _READMCREFLECTMEMERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _INGRESS, _ERR_NON_FATAL_REPORT_EN_1, val);

            // SOURCETRACK
            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_CONTAIN_EN_0);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _DUP_CREQ_TCEN0_TAG_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _INVALID_TCEN0_RSP_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _INVALID_TCEN1_RSP_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _SOURCETRACK_TIME_OUT_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_CONTAIN_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_FATAL_REPORT_EN_0);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _DUP_CREQ_TCEN0_TAG_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _INVALID_TCEN0_RSP_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _INVALID_TCEN1_RSP_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _SOURCETRACK_TIME_OUT_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_FATAL_REPORT_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_LOG_EN_0);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_LOG_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_LOG_EN_0, _DUP_CREQ_TCEN0_TAG_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_LOG_EN_0, _INVALID_TCEN0_RSP_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_LOG_EN_0, _INVALID_TCEN1_RSP_ERR, __TPROD, val);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_LOG_EN_0, _SOURCETRACK_TIME_OUT_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_LOG_EN_0, val);

            val = NVSWITCH_LINK_RD32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0);
            val = FLD_SET_DRF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, __TPROD, val);
            NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, val);
        }
        else
        {
            // Set trunk specific settings (TPROD) in SOE
            status = nvswitch_set_nport_tprod_state_ls10(device, p->portNum);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Failed to set NPORT TPROD state\n",
                    __FUNCTION__);
            }
        }
    }
    else
    {
        // PROD setting assumes ACCESS link
    }

    NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _SRC_PORT_TYPE0, NvU64_LO32(p->trunkSrcMask));
    NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _SRC_PORT_TYPE1, NvU64_HI32(p->trunkSrcMask));

    return status;
}

/*
 * @brief Returns the ingress requester link id.
 *
 * @param[in] device            nvswitch device
 * @param[in] params            NVSWITCH_GET_INGRESS_REQLINKID_PARAMS
 *
 * @returns                     NVL_SUCCESS if action succeeded,
 *                              -NVL_ERR_INVALID_STATE invalid link
 */
NvlStatus
nvswitch_ctrl_get_ingress_reqlinkid_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_REQLINKID_PARAMS *params
)
{
    NvU32 regval;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, params->portNum))
    {
        return -NVL_BAD_ARGS;
    }

    regval = NVSWITCH_NPORT_RD32_LS10(device, params->portNum, _NPORT, _REQLINKID);
    params->requesterLinkID = DRF_VAL(_NPORT, _REQLINKID, _REQROUTINGID, regval) |
        (DRF_VAL(_NPORT, _REQLINKID, _REQROUTINGID_UPPER, regval) <<
            DRF_SIZE(NV_NPORT_REQLINKID_REQROUTINGID));

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_get_internal_latency_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_INTERNAL_LATENCY *pLatency
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 vc_selector = pLatency->vc_selector;
    NvU32 idx_nport;

    // Validate VC selector
    if (vc_selector >= NVSWITCH_NUM_VCS_LS10)
    {
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    nvswitch_os_memset(pLatency, 0, sizeof(*pLatency));
    pLatency->vc_selector = vc_selector;

    // Snap up-to-the moment stats
    nvswitch_internal_latency_bin_log(device);

    for (idx_nport=0; idx_nport < NVSWITCH_LINK_COUNT(device); idx_nport++)
    {
        if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, idx_nport))
        {
            continue;
        }

        pLatency->egressHistogram[idx_nport].low =
            chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].low;
        pLatency->egressHistogram[idx_nport].medium =
            chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].medium;
        pLatency->egressHistogram[idx_nport].high =
           chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].high;
        pLatency->egressHistogram[idx_nport].panic =
           chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].panic;
        pLatency->egressHistogram[idx_nport].count =
           chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].count;
    }

    pLatency->elapsed_time_msec =
      (chip_device->latency_stats->latency[vc_selector].last_read_time_nsec -
       chip_device->latency_stats->latency[vc_selector].start_time_nsec)/1000000ULL;

    chip_device->latency_stats->latency[vc_selector].start_time_nsec =
        chip_device->latency_stats->latency[vc_selector].last_read_time_nsec;

    chip_device->latency_stats->latency[vc_selector].count = 0;

    // Clear accum_latency[]
    for (idx_nport = 0; idx_nport < NVSWITCH_LINK_COUNT(device); idx_nport++)
    {
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].low = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].medium = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].high = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].panic = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].count = 0;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_latency_bins_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_LATENCY_BINS *pLatency
)
{
    NvU32 vc_selector;
    const NvU32 freq_mhz = 1330;
    const NvU32 switchpll_hz = freq_mhz * 1000000ULL; // TODO: Verify this against POR clocks
    const NvU32 min_threshold = 10;   // Must be > zero to avoid div by zero
    const NvU32 max_threshold = 10000;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Quick input validation and ns to register value conversion
    for (vc_selector = 0; vc_selector < NVSWITCH_NUM_VCS_LS10; vc_selector++)
    {
        if ((pLatency->bin[vc_selector].lowThreshold > max_threshold)                           ||
            (pLatency->bin[vc_selector].lowThreshold < min_threshold)                           ||
            (pLatency->bin[vc_selector].medThreshold > max_threshold)                           ||
            (pLatency->bin[vc_selector].medThreshold < min_threshold)                           ||
            (pLatency->bin[vc_selector].hiThreshold  > max_threshold)                           ||
            (pLatency->bin[vc_selector].hiThreshold  < min_threshold)                           ||
            (pLatency->bin[vc_selector].lowThreshold > pLatency->bin[vc_selector].medThreshold) ||
            (pLatency->bin[vc_selector].medThreshold > pLatency->bin[vc_selector].hiThreshold))
        {
            return -NVL_BAD_ARGS;
        }

        pLatency->bin[vc_selector].lowThreshold =
            switchpll_hz / (1000000000 / pLatency->bin[vc_selector].lowThreshold);
        pLatency->bin[vc_selector].medThreshold =
            switchpll_hz / (1000000000 / pLatency->bin[vc_selector].medThreshold);
        pLatency->bin[vc_selector].hiThreshold =
            switchpll_hz / (1000000000 / pLatency->bin[vc_selector].hiThreshold);

        NVSWITCH_PORTSTAT_BCAST_WR32_LS10(device, _LIMIT, _LOW,    vc_selector, pLatency->bin[vc_selector].lowThreshold);
        NVSWITCH_PORTSTAT_BCAST_WR32_LS10(device, _LIMIT, _MEDIUM, vc_selector, pLatency->bin[vc_selector].medThreshold);
        NVSWITCH_PORTSTAT_BCAST_WR32_LS10(device, _LIMIT, _HIGH,   vc_selector, pLatency->bin[vc_selector].hiThreshold);
    }

    return NVL_SUCCESS;
}

//
// MODS-only IOCTLS
//

/*
 * REGISTER_READ/_WRITE
 * Provides direct access to the MMIO space for trusted clients like MODS.
 * This API should not be exposed to unsecure clients.
 */

/*
 * _nvswitch_get_engine_base
 * Used by REGISTER_READ/WRITE API.  Looks up an engine based on device/instance
 * and returns the base address in BAR0.
 *
 * register_rw_engine   [in] REGISTER_RW_ENGINE_*
 * instance             [in] physical instance of device
 * bcast                [in] FALSE: find unicast base address
 *                           TRUE:  find broadcast base address
 * base_addr            [out] base address in BAR0 of requested device
 *
 * Returns              NVL_SUCCESS: Device base address successfully found
 *                      else device lookup failed
 */

static NvlStatus
_nvswitch_get_engine_base_ls10
(
    nvswitch_device *device,
    NvU32   register_rw_engine,     // REGISTER_RW_ENGINE_*
    NvU32   instance,               // device instance
    NvBool  bcast,
    NvU32   *base_addr
)
{
    NvU32 base = 0;
    ENGINE_DISCOVERY_TYPE_LS10  *engine = NULL;
    NvlStatus retval = NVL_SUCCESS;
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    // Find the engine descriptor matching the request
    engine = NULL;

    switch (register_rw_engine)
    {
        case REGISTER_RW_ENGINE_RAW:
            // Special case raw IO
            if ((instance != 0) ||
                (bcast != NV_FALSE))
            {
                retval = -NVL_BAD_ARGS;
            }
        break;

        case REGISTER_RW_ENGINE_FUSE:
        case REGISTER_RW_ENGINE_JTAG:
        case REGISTER_RW_ENGINE_PMGR:
            //
            // Legacy devices are always single-instance, unicast-only.
            // These manuals are BAR0 offset-based, not IP-based.  Treat them
            // the same as RAW.
            //
            if ((instance != 0) ||
                (bcast != NV_FALSE))
            {
                retval = -NVL_BAD_ARGS;
            }
            register_rw_engine = REGISTER_RW_ENGINE_RAW;
        break;

        case REGISTER_RW_ENGINE_SAW:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, SAW, instance))
                {
                    engine = &chip_device->engSAW[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_SOE:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, SOE, instance))
                {
                    engine = &chip_device->engSOE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_SE:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, SE, instance))
                {
                    engine = &chip_device->engSE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_CLKS_SYS:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, CLKS_SYS, instance))
                {
                    engine = &chip_device->engCLKS_SYS[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_CLKS_SYSB:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, CLKS_SYSB, instance))
                {
                    engine = &chip_device->engCLKS_SYSB[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_CLKS_P0:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, CLKS_P0_BCAST, instance))
                {
                    engine = &chip_device->engCLKS_P0_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, CLKS_P0, instance))
                {
                    engine = &chip_device->engCLKS_P0[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_XPL:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, XPL, instance))
                {
                    engine = &chip_device->engXPL[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_XTL:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, XTL, instance))
                {
                    engine = &chip_device->engXTL[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLW:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLW_BCAST, instance))
                {
                    engine = &chip_device->engNVLW_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLW, instance))
                {
                    engine = &chip_device->engNVLW[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_MINION:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, MINION_BCAST, instance))
                {
                    engine = &chip_device->engMINION_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, MINION, instance))
                {
                    engine = &chip_device->engMINION[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLIPT:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLIPT_BCAST, instance))
                {
                    engine = &chip_device->engNVLIPT_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLIPT, instance))
                {
                    engine = &chip_device->engNVLIPT[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLTLC:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLTLC_BCAST, instance))
                {
                    engine = &chip_device->engNVLTLC_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLTLC, instance))
                {
                    engine = &chip_device->engNVLTLC[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLTLC_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLTLC_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNVLTLC_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLTLC_MULTICAST, instance))
                {
                    engine = &chip_device->engNVLTLC_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NPG:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NPG_BCAST, instance))
                {
                    engine = &chip_device->engNPG_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NPG, instance))
                {
                    engine = &chip_device->engNPG[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NPORT:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NPORT_BCAST, instance))
                {
                    engine = &chip_device->engNPORT_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NPORT, instance))
                {
                    engine = &chip_device->engNPORT[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NPORT_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NPORT_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNPORT_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NPORT_MULTICAST, instance))
                {
                    engine = &chip_device->engNPORT_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLIPT_LNK:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLIPT_LNK_BCAST, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLIPT_LNK, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLIPT_LNK_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLIPT_LNK_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLIPT_LNK_MULTICAST, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLDL:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLDL_BCAST, instance))
                {
                    engine = &chip_device->engNVLDL_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLDL, instance))
                {
                    engine = &chip_device->engNVLDL[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLDL_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLDL_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNVLDL_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NVLDL_MULTICAST, instance))
                {
                    engine = &chip_device->engNVLDL_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NXBAR:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NXBAR_BCAST, instance))
                {
                    engine = &chip_device->engNXBAR_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, NXBAR, instance))
                {
                    engine = &chip_device->engNXBAR[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_TILE:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILE_BCAST, instance))
                {
                    engine = &chip_device->engTILE_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILE, instance))
                {
                    engine = &chip_device->engTILE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_TILE_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILE_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engTILE_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILE_MULTICAST, instance))
                {
                    engine = &chip_device->engTILE_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_TILEOUT:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILEOUT_BCAST, instance))
                {
                    engine = &chip_device->engTILEOUT_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILEOUT, instance))
                {
                    engine = &chip_device->engTILEOUT[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_TILEOUT_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILEOUT_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engTILEOUT_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LS10(device, TILEOUT_MULTICAST, instance))
                {
                    engine = &chip_device->engTILEOUT_MULTICAST[instance];
                }
            }
        break;

        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: unknown REGISTER_RW_ENGINE 0x%x\n",
                __FUNCTION__,
                register_rw_engine);
            engine = NULL;
        break;
    }

    if (register_rw_engine == REGISTER_RW_ENGINE_RAW)
    {
        // Raw IO -- client provides full BAR0 offset
        base = 0;
    }
    else
    {
        // Check engine descriptor was found and valid
        if (engine == NULL)
        {
            retval = -NVL_BAD_ARGS;
            NVSWITCH_PRINT(device, ERROR,
                "%s: invalid REGISTER_RW_ENGINE/instance 0x%x(%d)\n",
                __FUNCTION__,
                register_rw_engine,
                instance);
        }
        else if (!engine->valid)
        {
            retval = -NVL_UNBOUND_DEVICE;
            NVSWITCH_PRINT(device, ERROR,
                "%s: REGISTER_RW_ENGINE/instance 0x%x(%d) disabled or invalid\n",
                __FUNCTION__,
                register_rw_engine,
                instance);
        }
        else
        {
            if (bcast && (engine->disc_type == DISCOVERY_TYPE_BROADCAST))
            {
                //
                // Caveat emptor: A read of a broadcast register is
                // implementation-specific.
                //
                base = engine->info.bc.bc_addr;
            }
            else if ((!bcast) && (engine->disc_type == DISCOVERY_TYPE_UNICAST))
            {
                base = engine->info.uc.uc_addr;
            }

            if (base == 0)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: REGISTER_RW_ENGINE/instance 0x%x(%d) has %s base address 0!\n",
                    __FUNCTION__,
                    register_rw_engine,
                    instance,
                    (bcast ? "BCAST" : "UNICAST" ));
                retval = -NVL_IO_ERROR;
            }
        }
    }

    *base_addr = base;
    return retval;
}

/*
 * CTRL_NVSWITCH_REGISTER_READ
 *
 * This provides direct access to the MMIO space for trusted clients like
 * MODS.
 * This API should not be exposed to unsecure clients.
 */

static NvlStatus
nvswitch_ctrl_register_read_ls10
(
    nvswitch_device *device,
    NVSWITCH_REGISTER_READ *p
)
{
    NvU32 base;
    NvU32 data;
    NvlStatus retval = NVL_SUCCESS;

    retval = _nvswitch_get_engine_base_ls10(device, p->engine, p->instance, NV_FALSE, &base);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Make sure target offset isn't out-of-range
    if ((base + p->offset) >= device->nvlink_device->pciInfo.bars[0].barSize)
    {
        return -NVL_IO_ERROR;
    }

    //
    // Some legacy device manuals are not 0-based (IP style).
    //
    data = NVSWITCH_OFF_RD32(device, base + p->offset);
    p->val = data;

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_REGISTER_WRITE
 *
 * This provides direct access to the MMIO space for trusted clients like
 * MODS.
 * This API should not be exposed to unsecure clients.
 */

static NvlStatus
nvswitch_ctrl_register_write_ls10
(
    nvswitch_device *device,
    NVSWITCH_REGISTER_WRITE *p
)
{
    NvU32 base;
    NvlStatus retval = NVL_SUCCESS;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    retval = _nvswitch_get_engine_base_ls10(device, p->engine, p->instance, p->bcast, &base);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Make sure target offset isn't out-of-range
    if ((base + p->offset) >= device->nvlink_device->pciInfo.bars[0].barSize)
    {
        return -NVL_IO_ERROR;
    }

    //
    // Some legacy device manuals are not 0-based (IP style).
    //
    NVSWITCH_OFF_WR32(device, base + p->offset, p->val);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_get_nvlink_ecc_errors_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ECC_ERRORS_PARAMS *params
)
{
    NvU32 statData;
    NvU8 i, j;
    NvlStatus status;
    NvBool bLaneReversed;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    nvswitch_os_memset(params->errorLink, 0, sizeof(params->errorLink));

    FOR_EACH_INDEX_IN_MASK(64, i, params->linkMask)
    {
        nvlink_link         *link;
        NVSWITCH_LANE_ERROR *errorLane;
        NvU8                offset;
        NvBool              minion_enabled;
        NvU32               sublinkWidth;

        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber) ||
            (i >= NVSWITCH_LINK_COUNT(device)))
        {
            return -NVL_BAD_ARGS;
        }

        sublinkWidth = device->hal.nvswitch_get_sublink_width(device, i);

        minion_enabled = nvswitch_is_minion_initialized(device,
            NVSWITCH_GET_LINK_ENG_INST(device, link->linkNumber, MINION));

        bLaneReversed = nvswitch_link_lane_reversed_ls10(device, link->linkNumber);

        for (j = 0; j < NVSWITCH_NUM_LANES_LS10; j++)
        {
            if (minion_enabled && (j < sublinkWidth))
            {
                status = nvswitch_minion_get_dl_status(device, i,
                                        (NV_NVLSTAT_RX12 + j), 0, &statData);

                if (status != NVL_SUCCESS)
                {
                    return status;
                }
                offset = bLaneReversed ? ((sublinkWidth - 1) - j) : j;
                errorLane                = &params->errorLink[i].errorLane[offset];
                errorLane->valid         = NV_TRUE;
            }
            else
            {
                // MINION disabled
                statData                 = 0;
                offset                   = j;
                errorLane                = &params->errorLink[i].errorLane[offset];
                errorLane->valid         = NV_FALSE;
            }

            errorLane->eccErrorValue = DRF_VAL(_NVLSTAT, _RX12, _ECC_CORRECTED_ERR_L0_VALUE, statData);
            errorLane->overflowed    = DRF_VAL(_NVLSTAT, _RX12, _ECC_CORRECTED_ERR_L0_OVER, statData);
        }

        if (minion_enabled)
        {
            status = nvswitch_minion_get_dl_status(device, i,
                                    NV_NVLSTAT_RX11, 0, &statData);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
        }
        else 
        {
            statData = 0;
        }

        params->errorLink[i].eccDecFailed           = DRF_VAL(_NVLSTAT, _RX11, _ECC_DEC_FAILED_VALUE, statData);
        params->errorLink[i].eccDecFailedOverflowed = DRF_VAL(_NVLSTAT, _RX11, _ECC_DEC_FAILED_OVER, statData);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvU32
nvswitch_get_num_links_ls10
(
    nvswitch_device *device
)
{
    return NVSWITCH_NUM_LINKS_LS10;
}

static NvU8
nvswitch_get_num_links_per_nvlipt_ls10
(
    nvswitch_device *device
)
{
    return NVSWITCH_LINKS_PER_NVLIPT_LS10;
}


NvlStatus
nvswitch_ctrl_get_fom_values_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_FOM_VALUES_PARAMS *p
)
{
    NvlStatus status;
    NvU32     statData;
    nvlink_link *link;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    link = nvswitch_get_link(device, p->linkId);
    if (link == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: link #%d invalid\n",
            __FUNCTION__, p->linkId);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_link_in_reset(device, link))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: link #%d is in reset\n",
            __FUNCTION__, p->linkId);
        return -NVL_ERR_INVALID_STATE;
    }

    status = nvswitch_minion_get_dl_status(device, p->linkId,
                                        NV_NVLSTAT_TR16, 0, &statData);
    p->figureOfMeritValues[0] = (NvU16) (statData & 0xFFFF);
    p->figureOfMeritValues[1] = (NvU16) ((statData >> 16) & 0xFFFF);

    status = nvswitch_minion_get_dl_status(device, p->linkId,
                                        NV_NVLSTAT_TR17, 0, &statData);
    p->figureOfMeritValues[2] = (NvU16) (statData & 0xFFFF);
    p->figureOfMeritValues[3] = (NvU16) ((statData >> 16) & 0xFFFF);

    p->numLanes = nvswitch_get_sublink_width(device, p->linkId);

    return status;
}

void
nvswitch_set_fatal_error_ls10
(
    nvswitch_device *device,
    NvBool           device_fatal,
    NvU32            link_id
)
{
    NvU32 reg;

    NVSWITCH_ASSERT(link_id < nvswitch_get_num_links(device));

    device->link[link_id].fatal_error_occurred = NV_TRUE;

    if (device_fatal)
    {
        reg = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH);
        reg = FLD_SET_DRF_NUM(_NVLSAW, _DRIVER_ATTACH_DETACH, _DEVICE_RESET_REQUIRED,
                              1, reg);

        NVSWITCH_SAW_WR32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH, reg);
    }
    else
    {
        reg = NVSWITCH_LINK_RD32(device, link_id, NPORT, _NPORT, _SCRATCH_WARM);
        reg = FLD_SET_DRF_NUM(_NPORT, _SCRATCH_WARM, _PORT_RESET_REQUIRED,
                              1, reg);

        NVSWITCH_LINK_WR32(device, link_id, NPORT, _NPORT, _SCRATCH_WARM, reg);
    }
}

static NvU32
nvswitch_get_latency_sample_interval_msec_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    return chip_device->latency_stats->sample_interval_msec;
}

static NvU32
nvswitch_get_device_dma_width_ls10
(
    nvswitch_device *device
)
{
    return DMA_ADDR_WIDTH_LS10;
}

static NvU32
nvswitch_get_link_ip_version_ls10
(
    nvswitch_device *device,
    NvU32            link_id
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 nvldl_instance;

    nvldl_instance = NVSWITCH_GET_LINK_ENG_INST(device, link_id, NVLDL);
    if (NVSWITCH_ENG_IS_VALID(device, NVLDL, nvldl_instance))
    {
        return chip_device->engNVLDL[nvldl_instance].version;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NVLink[0x%x] NVLDL instance invalid\n",
            __FUNCTION__, link_id);
        return 0;
    }
}

static NvBool
nvswitch_is_soe_supported_ls10
(
    nvswitch_device *device
)
{
    if (IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO, "SOE is not yet supported on fmodel\n");
        return NV_FALSE;
    }

    if (device->regkeys.soe_disable == NV_SWITCH_REGKEY_SOE_DISABLE_YES)
    {
        NVSWITCH_PRINT(device, WARN, "SOE can not be disabled via regkey.\n");
    }

    return NV_TRUE;
}

NvlStatus
_nvswitch_get_bios_version
(
    nvswitch_device *device,
    NvU64           *pVersion
)
{
    NVSWITCH_GET_BIOS_INFO_PARAMS p = { 0 };
    NvlStatus status;

    if (pVersion == NULL)
    {
        return NVL_BAD_ARGS;
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &p);
    if (status == NVL_SUCCESS)
    {
        *pVersion = p.version;
    }

    return status;
}

/*
 * @Brief : Checks if Inforom is supported
 *
 */
NvBool
nvswitch_is_inforom_supported_ls10
(
    nvswitch_device *device
)
{
    NvU64       version;
    NvlStatus   status;

    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "INFOROM is not supported on non-silicon platform\n");
        return NV_FALSE;
    }

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "INFOROM is not supported since SOE is not supported\n");
        return NV_FALSE; 
    }

    status = _nvswitch_get_bios_version(device, &version);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Error getting BIOS version\n",
            __FUNCTION__);
        return NV_FALSE;
    }

    if (version >= NVSWITCH_IFR_MIN_BIOS_VER_LS10)
    {
        return NV_TRUE;
    }
    else
    {
        NVSWITCH_PRINT(device, WARN,
            "INFOROM is not supported on this NVSwitch BIOS version.\n");
        return NV_FALSE;
    }
}

/*
 * @Brief : Checks if Spi is supported
 *
 * Stubbing SOE Spi support on ls10.
 *
 */
NvBool
nvswitch_is_spi_supported_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_PRINT(device, INFO,
        "SPI is not supported on LS10\n");

    return NV_FALSE;
}

NvBool
nvswitch_is_bios_supported_ls10
(
    nvswitch_device *device
)
{
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "BIOS is not supported on non-silicon platforms\n");
        return NV_FALSE;
    }

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "BIOS is not supported since SOE is not supported\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

NvlStatus
nvswitch_get_bios_size_ls10
(
    nvswitch_device *device,
    NvU32 *pSize
)
{
    *pSize = SOE_CORE_BIOS_SIZE_LS10;

    return NVL_SUCCESS;
}

/*
 * @Brief : Check if SMBPBI is supported
 *
 */
NvBool
nvswitch_is_smbpbi_supported_ls10
(
    nvswitch_device *device
)
{
    NvU64       version;
    NvlStatus   status;

    if (!nvswitch_is_smbpbi_supported_lr10(device))
    {
        return NV_FALSE;
    }

    if (nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "SMBPBI is not supported when TNVL mode is enabled\n");
        return NV_FALSE; 
    }

    status = _nvswitch_get_bios_version(device, &version);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Error getting BIOS version\n",
            __FUNCTION__);
        return NV_FALSE;
    }

    if (version >= NVSWITCH_SMBPBI_MIN_BIOS_VER_LS10)
    {
        return NV_TRUE;
    }
    else
    {
        NVSWITCH_PRINT(device, WARN,
            "SMBPBI is not supported on NVSwitch BIOS version %llx.\n", version);
        return NV_FALSE;
    }
}

/*
 * @Brief : Additional setup needed after blacklisted device initialization
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 */
void
nvswitch_post_init_blacklist_device_setup_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_PRINT(device, WARN, "%s: Function not implemented\n", __FUNCTION__);
    return;
}

/*
* @brief: This function retrieves the NVLIPT public ID for a given global link idx
* @params[in]  device        reference to current nvswitch device
* @params[in]  linkId        link to retrieve NVLIPT public ID from
* @params[out] publicId      Public ID of NVLIPT owning linkId
*/
NvlStatus nvswitch_get_link_public_id_ls10
(
    nvswitch_device *device,
    NvU32 linkId,
    NvU32 *publicId
)
{
    if (!device->hal.nvswitch_is_link_valid(device, linkId) ||
        (publicId == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    *publicId = NVSWITCH_NVLIPT_GET_PUBLIC_ID_LS10(linkId);


    return (NVSWITCH_ENG_VALID_LS10(device, NVLIPT, *publicId)) ?
                NVL_SUCCESS : -NVL_BAD_ARGS;
}

/*
* @brief: This function retrieves the internal link idx for a given global link idx
* @params[in]  device        reference to current nvswitch device
* @params[in]  linkId        link to retrieve NVLIPT public ID from
* @params[out] localLinkIdx  Internal link index of linkId
*/
NvlStatus nvswitch_get_link_local_idx_ls10
(
    nvswitch_device *device,
    NvU32 linkId,
    NvU32 *localLinkIdx
)
{
    if (!device->hal.nvswitch_is_link_valid(device, linkId) ||
        (localLinkIdx == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    *localLinkIdx = NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(linkId);

    return NVL_SUCCESS;
}

NvlStatus nvswitch_ctrl_get_fatal_error_scope_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS *pParams
)
{
    NvU32 linkId;
    NvU32 reg = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH);
    pParams->device = FLD_TEST_DRF_NUM(_NVLSAW, _DRIVER_ATTACH_DETACH, _DEVICE_RESET_REQUIRED,
                                       1, reg);

    for (linkId = 0; linkId < NVSWITCH_MAX_PORTS; linkId++)
    {
        if (!nvswitch_is_link_valid(device, linkId))
        {
            pParams->port[linkId] = NV_FALSE;
            continue;
        }

        reg = NVSWITCH_LINK_RD32(device, linkId, NPORT, _NPORT, _SCRATCH_WARM);
        pParams->port[linkId] = FLD_TEST_DRF_NUM(_NPORT, _SCRATCH_WARM,
                                                 _PORT_RESET_REQUIRED, 1, reg);
    }

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_SET_REMAP_POLICY
 */

NvlStatus
nvswitch_get_remap_table_selector_ls10
(
    nvswitch_device *device,
    NVSWITCH_TABLE_SELECT_REMAP table_selector,
    NvU32 *remap_ram_sel
)
{
    NvU32 ram_sel = 0;

    switch (table_selector)
    {
        case NVSWITCH_TABLE_SELECT_REMAP_PRIMARY:
            ram_sel = NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSNORMREMAPRAM;
            break;
        case NVSWITCH_TABLE_SELECT_REMAP_EXTA:
            ram_sel = NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSEXTAREMAPRAM;
            break;
        case NVSWITCH_TABLE_SELECT_REMAP_EXTB:
            ram_sel = NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSEXTBREMAPRAM;
            break;
        case NVSWITCH_TABLE_SELECT_REMAP_MULTICAST:
            ram_sel = NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECT_MULTICAST_REMAPRAM;
            break;
        default:
            NVSWITCH_PRINT(device, ERROR, "%s: invalid remap table selector (0x%x)\n",
                __FUNCTION__, table_selector);
            return -NVL_ERR_NOT_SUPPORTED;
            break;
    }

    if (remap_ram_sel)
    {
        *remap_ram_sel = ram_sel;
    }

    return NVL_SUCCESS;
}

NvU32
nvswitch_get_ingress_ram_size_ls10
(
    nvswitch_device *device,
    NvU32 ingress_ram_selector      // NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECT*
)
{
    NvU32 ram_size = 0;

    switch (ingress_ram_selector)
    {
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSNORMREMAPRAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_NORMREMAPTAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSEXTAREMAPRAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_EXTAREMAPTAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSEXTBREMAPRAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_EXTBREMAPTAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRIDROUTERAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_RID_TAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRLANROUTERAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_RLAN_TAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECT_MULTICAST_REMAPRAM:
            ram_size = NV_INGRESS_MCREMAPTABADDR_RAM_ADDRESS_MCREMAPTAB_DEPTH + 1;
            break;
        default:
            NVSWITCH_PRINT(device, ERROR, "%s: Unsupported ingress RAM selector (0x%x)\n",
                __FUNCTION__, ingress_ram_selector);
            break;
    }

    return ram_size;
}

static void
_nvswitch_set_remap_policy_ls10
(
    nvswitch_device *device,
    NvU32 portNum,
    NvU32 remap_ram_sel,
    NvU32 firstIndex,
    NvU32 numEntries,
    NVSWITCH_REMAP_POLICY_ENTRY *remap_policy
)
{
    NvU32 i;
    NvU32 remap_address;
    NvU32 address_base;
    NvU32 address_limit;
    NvU32 rfunc;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return;
    }

    NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, firstIndex) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, remap_ram_sel) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, _ENABLE));

    for (i = 0; i < numEntries; i++)
    {
        // Set each field if enabled, else set it to 0.
        remap_address = DRF_VAL64(_INGRESS, _REMAP, _ADDR_PHYS_LS10, remap_policy[i].address);
        address_base = DRF_VAL64(_INGRESS, _REMAP, _ADR_BASE_PHYS_LS10, remap_policy[i].addressBase);
        address_limit = DRF_VAL64(_INGRESS, _REMAP, _ADR_LIMIT_PHYS_LS10, remap_policy[i].addressLimit);
        rfunc = remap_policy[i].flags &
            (
                NVSWITCH_REMAP_POLICY_FLAGS_REMAP_ADDR |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_CHECK |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_REPLACE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADR_BASE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE
            );
        // Handle re-used RFUNC[5] conflict between Limerock and Laguna Seca
        if (rfunc & NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE)
        {
            //
            // RFUNC[5] Limerock functionality was deprecated and replaced with
            // a new function in Laguna Seca.  So fix RFUNC if needed.
            //

            rfunc &= ~NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE;
            rfunc |= NVBIT(5);
        }

        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA1,
            DRF_NUM(_INGRESS, _REMAPTABDATA1, _REQCTXT_MSK, remap_policy[i].reqCtxMask) |
            DRF_NUM(_INGRESS, _REMAPTABDATA1, _REQCTXT_CHK, remap_policy[i].reqCtxChk));
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA2,
            DRF_NUM(_INGRESS, _REMAPTABDATA2, _REQCTXT_REP, remap_policy[i].reqCtxRep));
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA3,
            DRF_NUM(_INGRESS, _REMAPTABDATA3, _ADR_BASE, address_base) |
            DRF_NUM(_INGRESS, _REMAPTABDATA3, _ADR_LIMIT, address_limit));
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA4,
            DRF_NUM(_INGRESS, _REMAPTABDATA4, _TGTID, remap_policy[i].targetId) |
            DRF_NUM(_INGRESS, _REMAPTABDATA4, _RFUNC, rfunc));
        // Get the upper bits of address_base/_limit
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA5,
            DRF_NUM(_INGRESS, _REMAPTABDATA5, _ADR_BASE,
                (address_base >> DRF_SIZE(NV_INGRESS_REMAPTABDATA3_ADR_BASE))) |
            DRF_NUM(_INGRESS, _REMAPTABDATA5, _ADR_LIMIT,
                (address_limit >> DRF_SIZE(NV_INGRESS_REMAPTABDATA3_ADR_LIMIT))));

        // Write last and auto-increment
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA0,
            DRF_NUM(_INGRESS, _REMAPTABDATA0, _RMAP_ADDR, remap_address) |
            DRF_NUM(_INGRESS, _REMAPTABDATA0, _IRL_SEL, remap_policy[i].irlSelect) |
            DRF_NUM(_INGRESS, _REMAPTABDATA0, _ACLVALID, remap_policy[i].entryValid));
    }
}

static void
_nvswitch_set_mc_remap_policy_ls10
(
    nvswitch_device *device,
    NvU32 portNum,
    NvU32 firstIndex,
    NvU32 numEntries,
    NVSWITCH_REMAP_POLICY_ENTRY *remap_policy
)
{
    NvU32 i;
    NvU32 remap_address;
    NvU32 address_base;
    NvU32 address_limit;
    NvU32 rfunc;
    NvU32 reflective;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return;
    }

    NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABADDR,
        DRF_NUM(_INGRESS, _MCREMAPTABADDR, _RAM_ADDRESS, firstIndex) |
        DRF_DEF(_INGRESS, _MCREMAPTABADDR, _AUTO_INCR, _ENABLE));

    for (i = 0; i < numEntries; i++)
    {
        // Set each field if enabled, else set it to 0.
        remap_address = DRF_VAL64(_INGRESS, _REMAP, _ADDR_PHYS_LS10, remap_policy[i].address);
        address_base = DRF_VAL64(_INGRESS, _REMAP, _ADR_BASE_PHYS_LS10, remap_policy[i].addressBase);
        address_limit = DRF_VAL64(_INGRESS, _REMAP, _ADR_LIMIT_PHYS_LS10, remap_policy[i].addressLimit);
        rfunc = remap_policy[i].flags &
            (
                NVSWITCH_REMAP_POLICY_FLAGS_REMAP_ADDR |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_CHECK |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_REPLACE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADR_BASE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE
            );
        // Handle re-used RFUNC[5] conflict between Limerock and Laguna Seca
        if (rfunc & NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE)
        {
            //
            // RFUNC[5] Limerock functionality was deprecated and replaced with
            // a new function in Laguna Seca.  So fix RFUNC if needed.
            //

            rfunc &= ~NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE;
            rfunc |= NVBIT(5);
        }
        reflective = (remap_policy[i].flags & NVSWITCH_REMAP_POLICY_FLAGS_REFLECTIVE ? 1 : 0);

        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABDATA1,
            DRF_NUM(_INGRESS, _MCREMAPTABDATA1, _REQCTXT_MSK, remap_policy[i].reqCtxMask) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA1, _REQCTXT_CHK, remap_policy[i].reqCtxChk));
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABDATA2,
            DRF_NUM(_INGRESS, _MCREMAPTABDATA2, _REQCTXT_REP, remap_policy[i].reqCtxRep));
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABDATA3,
            DRF_NUM(_INGRESS, _MCREMAPTABDATA3, _ADR_BASE, address_base) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA3, _ADR_LIMIT, address_limit));
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABDATA4,
            DRF_NUM(_INGRESS, _MCREMAPTABDATA4, _MCID, remap_policy[i].targetId) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA4, _RFUNC, rfunc) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA4, _ENB_REFLECT_MEM, reflective));
        // Get the upper bits of address_base/_limit
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABDATA5,
            DRF_NUM(_INGRESS, _MCREMAPTABDATA5, _ADR_BASE,
                (address_base >> DRF_SIZE(NV_INGRESS_MCREMAPTABDATA3_ADR_BASE))) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA5, _ADR_LIMIT,
                (address_limit >> DRF_SIZE(NV_INGRESS_MCREMAPTABDATA3_ADR_LIMIT))));

        // Write last and auto-increment
        NVSWITCH_LINK_WR32_LS10(device, portNum, NPORT, _INGRESS, _MCREMAPTABDATA0,
            DRF_NUM(_INGRESS, _MCREMAPTABDATA0, _RMAP_ADDR, remap_address) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA0, _IRL_SEL, remap_policy[i].irlSelect) |
            DRF_NUM(_INGRESS, _MCREMAPTABDATA0, _ACLVALID, remap_policy[i].entryValid));
    }
}

NvlStatus
nvswitch_ctrl_set_remap_policy_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_REMAP_POLICY *p
)
{
    NvU32 i;
    NvU32 rfunc;
    NvU32 remap_ram_sel = ~0;
    NvU32 ram_size;
    NvlStatus retval = NVL_SUCCESS;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    //
    // This function is used to read both normal and multicast REMAP table,
    // so guarantee table definitions are identical.
    //
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA0_RMAP_ADDR) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA0_RMAP_ADDR));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA0_IRL_SEL) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA0_IRL_SEL));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA1_REQCTXT_MSK) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA1_REQCTXT_MSK));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA1_REQCTXT_CHK) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA1_REQCTXT_CHK));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA2_REQCTXT_REP) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA2_REQCTXT_REP));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA3_ADR_BASE) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA3_ADR_BASE));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA3_ADR_LIMIT) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA3_ADR_LIMIT));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA4_RFUNC) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA4_RFUNC));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA5_ADR_BASE) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA5_ADR_BASE));
    ct_assert(DRF_SIZE(NV_INGRESS_REMAPTABDATA5_ADR_LIMIT) == DRF_SIZE(NV_INGRESS_MCREMAPTABDATA5_ADR_LIMIT));

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "NPORT port #%d not valid\n",
            p->portNum);
        return -NVL_BAD_ARGS;
    }

    retval = nvswitch_get_remap_table_selector(device, p->tableSelect, &remap_ram_sel);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Remap table #%d not supported\n",
            p->tableSelect);
        return retval;
    }
    ram_size = nvswitch_get_ingress_ram_size(device, remap_ram_sel);

    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_REMAP_POLICY_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "remapPolicy[%d..%d] overflows range %d..%d or size %d.\n",
            p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_REMAP_POLICY_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    for (i = 0; i < p->numEntries; i++)
    {
        if (p->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
        {
            if (p->remapPolicy[i].targetId &
                ~DRF_MASK(NV_INGRESS_MCREMAPTABDATA4_MCID))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "remapPolicy[%d].targetId 0x%x out of valid MCID range (0x%x..0x%x)\n",
                    i, p->remapPolicy[i].targetId,
                    0, DRF_MASK(NV_INGRESS_MCREMAPTABDATA4_MCID));
                return -NVL_BAD_ARGS;
            }
        }
        else
        {
            if (p->remapPolicy[i].targetId &
                ~DRF_MASK(NV_INGRESS_REMAPTABDATA4_TGTID))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "remapPolicy[%d].targetId 0x%x out of valid TGTID range (0x%x..0x%x)\n",
                    i, p->remapPolicy[i].targetId,
                    0, DRF_MASK(NV_INGRESS_REMAPTABDATA4_TGTID));
                return -NVL_BAD_ARGS;
            }
        }

        if (p->remapPolicy[i].irlSelect &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA0_IRL_SEL))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].irlSelect 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].irlSelect,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA0_IRL_SEL));
            return -NVL_BAD_ARGS;
        }

        rfunc = p->remapPolicy[i].flags &
            (
                NVSWITCH_REMAP_POLICY_FLAGS_REMAP_ADDR |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_CHECK |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_REPLACE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADR_BASE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE |
                NVSWITCH_REMAP_POLICY_FLAGS_REFLECTIVE
            );
        if (rfunc != p->remapPolicy[i].flags)
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].flags 0x%x has undefined flags (0x%x)\n",
                i, p->remapPolicy[i].flags,
                p->remapPolicy[i].flags ^ rfunc);
            return -NVL_BAD_ARGS;
        }
        if ((rfunc & NVSWITCH_REMAP_POLICY_FLAGS_REFLECTIVE) &&
            (p->tableSelect != NVSWITCH_TABLE_SELECT_REMAP_MULTICAST))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].flags: REFLECTIVE mapping only supported for MC REMAP\n",
                i);
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 51:39 are used
        if (p->remapPolicy[i].address &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADDR_PHYS_LS10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].address 0x%llx & ~0x%llx != 0\n",
                i, p->remapPolicy[i].address,
                DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADDR_PHYS_LS10));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].reqCtxMask &
           ~DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_MSK))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].reqCtxMask 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].reqCtxMask,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_MSK));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].reqCtxChk &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_CHK))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].reqCtxChk 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].reqCtxChk,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_CHK));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].reqCtxRep &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA2_REQCTXT_REP))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].reqCtxRep 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].reqCtxRep,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA2_REQCTXT_REP));
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 38:21 are used
        if (p->remapPolicy[i].addressBase &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_BASE_PHYS_LS10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].addressBase 0x%llx & ~0x%llx != 0\n",
                i, p->remapPolicy[i].addressBase,
                DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_BASE_PHYS_LS10));
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 38:21 are used
        if (p->remapPolicy[i].addressLimit &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_LIMIT_PHYS_LS10))
        {
            NVSWITCH_PRINT(device, ERROR,
                 "remapPolicy[%d].addressLimit 0x%llx & ~0x%llx != 0\n",
                 i, p->remapPolicy[i].addressLimit,
                 DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_LIMIT_PHYS_LS10));
            return -NVL_BAD_ARGS;
        }

        // Validate base & limit describe a region
        if (p->remapPolicy[i].addressBase > p->remapPolicy[i].addressLimit)
        {
            NVSWITCH_PRINT(device, ERROR,
                 "remapPolicy[%d].addressBase/Limit invalid: 0x%llx > 0x%llx\n",
                 i, p->remapPolicy[i].addressBase, p->remapPolicy[i].addressLimit);
            return -NVL_BAD_ARGS;
        }

        // Validate limit - base doesn't overflow 64G
        if ((p->remapPolicy[i].addressLimit - p->remapPolicy[i].addressBase) &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_OFFSET_PHYS_LS10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].addressLimit 0x%llx - addressBase 0x%llx overflows 64GB\n",
                i, p->remapPolicy[i].addressLimit, p->remapPolicy[i].addressBase);
            return -NVL_BAD_ARGS;
        }

        // AddressOffset is deprecated in LS10 and later
        if (p->remapPolicy[i].addressOffset != 0)
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].addressOffset deprecated\n",
                i);
            return -NVL_BAD_ARGS;
        }
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (p->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
    {
        _nvswitch_set_mc_remap_policy_ls10(device, p->portNum, p->firstIndex, p->numEntries, p->remapPolicy);
    }
    else
    {
        // Stop traffic on the port
        retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_TRUE);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to stop traffic on nport %d\n", p->portNum);
            return retval;
        }

        _nvswitch_set_remap_policy_ls10(device, p->portNum, remap_ram_sel, p->firstIndex, p->numEntries, p->remapPolicy);

        // Allow traffic on the port
        retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_FALSE);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to restart traffic on nport %d\n", p->portNum);
            return retval;
        }
    }

    return retval;
}

/*
 * CTRL_NVSWITCH_GET_REMAP_POLICY
 */

#define NVSWITCH_NUM_REMAP_POLICY_REGS_LS10 6

NvlStatus
nvswitch_ctrl_get_remap_policy_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_REMAP_POLICY_PARAMS *params
)
{
    NVSWITCH_REMAP_POLICY_ENTRY *remap_policy;
    NvU32 remap_policy_data[NVSWITCH_NUM_REMAP_POLICY_REGS_LS10]; // 6 word/REMAP table entry
    NvU32 table_index;
    NvU32 remap_count;
    NvU32 remap_address;
    NvU32 address_base;
    NvU32 address_limit;
    NvU32 remap_ram_sel;
    NvU32 ram_size;
    NvlStatus retval;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, params->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "NPORT port #%d not valid\n",
            params->portNum);
        return -NVL_BAD_ARGS;
    }

    retval = nvswitch_get_remap_table_selector(device, params->tableSelect, &remap_ram_sel);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Remap table #%d not supported\n",
            params->tableSelect);
        return retval;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, remap_ram_sel);
    if ((params->firstIndex >= ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: remapPolicy first index %d out of range[%d..%d].\n",
            __FUNCTION__, params->firstIndex, 0, ram_size - 1);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    nvswitch_os_memset(params->entry, 0, (NVSWITCH_REMAP_POLICY_ENTRIES_MAX *
        sizeof(NVSWITCH_REMAP_POLICY_ENTRY)));

    table_index = params->firstIndex;
    remap_policy = params->entry;
    remap_count = 0;

    /* set table offset */
    if (params->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
    {
        NVSWITCH_LINK_WR32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABADDR,
            DRF_NUM(_INGRESS, _MCREMAPTABADDR, _RAM_ADDRESS, params->firstIndex) |
            DRF_DEF(_INGRESS, _MCREMAPTABADDR, _AUTO_INCR, _ENABLE));
    }
    else
    {
        NVSWITCH_LINK_WR32_LS10(device, params->portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
            DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, params->firstIndex) |
            DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, remap_ram_sel) |
            DRF_DEF(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, _ENABLE));
    }

    while (remap_count < NVSWITCH_REMAP_POLICY_ENTRIES_MAX &&
        table_index < ram_size)
    {
        if (params->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
        {
            remap_policy_data[0] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABDATA0);
            remap_policy_data[1] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABDATA1);
            remap_policy_data[2] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABDATA2);
            remap_policy_data[3] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABDATA3);
            remap_policy_data[4] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABDATA4);
            remap_policy_data[5] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _MCREMAPTABDATA5);
        }
        else
        {
            remap_policy_data[0] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA0);
            remap_policy_data[1] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA1);
            remap_policy_data[2] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA2);
            remap_policy_data[3] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA3);
            remap_policy_data[4] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA4);
            remap_policy_data[5] = NVSWITCH_LINK_RD32_LS10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA5);
        }

        /* add to remap_entries list if nonzero */
        if (remap_policy_data[0] || remap_policy_data[1] || remap_policy_data[2] ||
            remap_policy_data[3] || remap_policy_data[4] || remap_policy_data[5])
        {
            remap_policy[remap_count].irlSelect =
                DRF_VAL(_INGRESS, _REMAPTABDATA0, _IRL_SEL, remap_policy_data[0]);

            remap_policy[remap_count].entryValid =
                DRF_VAL(_INGRESS, _REMAPTABDATA0, _ACLVALID, remap_policy_data[0]);

            remap_address =
                DRF_VAL(_INGRESS, _REMAPTABDATA0, _RMAP_ADDR, remap_policy_data[0]);

            remap_policy[remap_count].address =
                DRF_NUM64(_INGRESS, _REMAP, _ADDR_PHYS_LS10, remap_address);

            remap_policy[remap_count].reqCtxMask =
                DRF_VAL(_INGRESS, _REMAPTABDATA1, _REQCTXT_MSK, remap_policy_data[1]);

            remap_policy[remap_count].reqCtxChk =
                DRF_VAL(_INGRESS, _REMAPTABDATA1, _REQCTXT_CHK, remap_policy_data[1]);

            remap_policy[remap_count].reqCtxRep =
                DRF_VAL(_INGRESS, _REMAPTABDATA2, _REQCTXT_REP, remap_policy_data[2]);

            remap_policy[remap_count].addressOffset = 0;

            address_base =
                DRF_VAL(_INGRESS, _REMAPTABDATA3, _ADR_BASE, remap_policy_data[3]) |
                (DRF_VAL(_INGRESS, _REMAPTABDATA5, _ADR_BASE, remap_policy_data[5]) <<
                    DRF_SIZE(NV_INGRESS_REMAPTABDATA3_ADR_BASE));

            remap_policy[remap_count].addressBase =
                DRF_NUM64(_INGRESS, _REMAP, _ADR_BASE_PHYS_LS10, address_base);

            address_limit =
                DRF_VAL(_INGRESS, _REMAPTABDATA3, _ADR_LIMIT, remap_policy_data[3]) |
                (DRF_VAL(_INGRESS, _REMAPTABDATA5, _ADR_LIMIT, remap_policy_data[5]) <<
                    DRF_SIZE(NV_INGRESS_REMAPTABDATA3_ADR_LIMIT));

            remap_policy[remap_count].addressLimit =
                DRF_NUM64(_INGRESS, _REMAP, _ADR_LIMIT_PHYS_LS10, address_limit);

            if (params->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
            {
                remap_policy[remap_count].targetId =
                    DRF_VAL(_INGRESS, _MCREMAPTABDATA4, _MCID, remap_policy_data[4]);
            }
            else
            {
                remap_policy[remap_count].targetId =
                    DRF_VAL(_INGRESS, _REMAPTABDATA4, _TGTID, remap_policy_data[4]);
            }

            remap_policy[remap_count].flags =
                DRF_VAL(_INGRESS, _REMAPTABDATA4, _RFUNC, remap_policy_data[4]);
            // Handle re-used RFUNC[5] conflict between Limerock and Laguna Seca
            if (remap_policy[remap_count].flags & NVBIT(5))
            {
                remap_policy[remap_count].flags &= ~NVBIT(5);
                remap_policy[remap_count].flags |= NVSWITCH_REMAP_POLICY_FLAGS_ADDR_TYPE;
            }
            if (params->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
            {
                if (FLD_TEST_DRF_NUM(_INGRESS, _MCREMAPTABDATA4, _ENB_REFLECT_MEM, 1, remap_policy_data[4]))
                {
                    remap_policy[remap_count].flags |= NVSWITCH_REMAP_POLICY_FLAGS_REFLECTIVE;
                }
            }

            remap_count++;
        }

        table_index++;
    }

    params->nextIndex = table_index;
    params->numEntries = remap_count;

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_SET_REMAP_POLICY_VALID
 */
NvlStatus
nvswitch_ctrl_set_remap_policy_valid_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_REMAP_POLICY_VALID *p
)
{
    NvU32 remap_ram;
    NvU32 ram_address = p->firstIndex;
    NvU32 remap_policy_data[NVSWITCH_NUM_REMAP_POLICY_REGS_LS10]; // 6 word/REMAP table entry
    NvU32 i;
    NvU32 remap_ram_sel;
    NvU32 ram_size;
    NvlStatus retval;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    retval = nvswitch_get_remap_table_selector(device, p->tableSelect, &remap_ram_sel);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Remap table #%d not supported\n",
            p->tableSelect);
        return retval;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, remap_ram_sel);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_REMAP_POLICY_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: remapPolicy[%d..%d] overflows range %d..%d or size %d.\n",
            __FUNCTION__, p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_REMAP_POLICY_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Stop traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to stop traffic on nport %d\n", p->portNum);
        return retval;
    }

    if (p->tableSelect == NVSWITCH_TABLE_SELECT_REMAP_MULTICAST)
    {
        for (i = 0; i < p->numEntries; i++)
        {
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABADDR,
                DRF_NUM(_INGRESS, _MCREMAPTABADDR, _RAM_ADDRESS, ram_address++) |
                DRF_DEF(_INGRESS, _MCREMAPTABADDR, _AUTO_INCR, _DISABLE));

            remap_policy_data[0] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA0);
            remap_policy_data[1] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA1);
            remap_policy_data[2] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA2);
            remap_policy_data[3] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA3);
            remap_policy_data[4] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA4);
            remap_policy_data[5] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA5);

            // Set valid bit in REMAPTABDATA0.
            remap_policy_data[0] = FLD_SET_DRF_NUM(_INGRESS, _MCREMAPTABDATA0, _ACLVALID, p->entryValid[i], remap_policy_data[0]);

            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA5, remap_policy_data[5]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA4, remap_policy_data[4]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA3, remap_policy_data[3]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA2, remap_policy_data[2]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA1, remap_policy_data[1]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _MCREMAPTABDATA0, remap_policy_data[0]);
        }
    }
    else
    {
        // Select REMAP POLICY RAM and disable Auto Increment.
        remap_ram =
            DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, remap_ram_sel) |
            DRF_DEF(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, _DISABLE);

        for (i = 0; i < p->numEntries; i++)
        {
            /* set the ram address */
            remap_ram = FLD_SET_DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, ram_address++, remap_ram);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REQRSPMAPADDR, remap_ram);

            remap_policy_data[0] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA0);
            remap_policy_data[1] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA1);
            remap_policy_data[2] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA2);
            remap_policy_data[3] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA3);
            remap_policy_data[4] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA4);
            remap_policy_data[5] = NVSWITCH_LINK_RD32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA5);

            // Set valid bit in REMAPTABDATA0.
            remap_policy_data[0] = FLD_SET_DRF_NUM(_INGRESS, _REMAPTABDATA0, _ACLVALID, p->entryValid[i], remap_policy_data[0]);

            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA5, remap_policy_data[5]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA4, remap_policy_data[4]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA3, remap_policy_data[3]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA2, remap_policy_data[2]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA1, remap_policy_data[1]);
            NVSWITCH_LINK_WR32_LS10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA0, remap_policy_data[0]);
        }
    }

    // Allow traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to restart traffic on nport %d\n", p->portNum);
        return retval;
    }

    return NVL_SUCCESS;
}

NvlStatus nvswitch_ctrl_set_mc_rid_table_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_MC_RID_TABLE_PARAMS *p
)
{
    NvlStatus ret;
    NVSWITCH_MC_RID_ENTRY_LS10 table_entry;
    NvU32 entries_used = 0;

    if (!nvswitch_is_link_valid(device, p->portNum))
        return -NVL_BAD_ARGS;

    // check if link is invalid or repeater
    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: NPORT invalid for port %d\n",
                       __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    // range check index
    if (p->extendedTable && (p->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDEXTTAB_DEPTH))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for extended table\n",
                        __FUNCTION__, p->index);
        return -NVL_BAD_ARGS;
    }

    if (p->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDTAB_DEPTH)
    {
         NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for main table\n",
                        __FUNCTION__, p->index);
         return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // if !entryValid, zero the table and return
    if (!p->entryValid)
        return nvswitch_mc_invalidate_mc_rid_entry_ls10(device, p->portNum, p->index,
                                                        p->extendedTable, NV_TRUE);

    // range check mcSize
    if ((p->mcSize == 0) || (p->mcSize > NVSWITCH_NUM_LINKS_LS10))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: mcSize %d is invalid\n", __FUNCTION__, p->mcSize);
        return -NVL_BAD_ARGS;
    }

    // extended table cannot have an extended ptr
    if (p->extendedTable && p->extendedValid)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: extendedTable cannot have an extendedValid ptr\n",
                        __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // set up table entry fields
    table_entry.index = (NvU8)p->index;
    table_entry.use_extended_table = p->extendedTable;
    table_entry.mcpl_size = (NvU8)p->mcSize;
    table_entry.num_spray_groups = (NvU8)p->numSprayGroups;
    table_entry.ext_ptr = (NvU8)p->extendedPtr;
    table_entry.no_dyn_rsp = p->noDynRsp;
    table_entry.ext_ptr_valid = p->extendedValid;
    table_entry.valid = p->entryValid;

    // build the directive list, remaining range checks are performed inside
    ret = nvswitch_mc_build_mcp_list_ls10(device, p->ports, p->portsPerSprayGroup, p->replicaOffset,
                                          p->replicaValid, p->vcHop, &table_entry, &entries_used);

    NVSWITCH_PRINT(device, INFO, "nvswitch_mc_build_mcp_list_ls10() returned %d, entries used: %d\n",
                   ret, entries_used);

    if (ret != NVL_SUCCESS)
        return ret;

    // program the table
    ret = nvswitch_mc_program_mc_rid_entry_ls10(device, p->portNum, &table_entry, entries_used);

    return ret;
}

NvlStatus nvswitch_ctrl_get_mc_rid_table_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_MC_RID_TABLE_PARAMS *p
)
{
    NvU32 ret;
    NVSWITCH_MC_RID_ENTRY_LS10 table_entry;
    NvU32 port = p->portNum;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NPORT, port))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: NPORT invalid for port %d\n",
                       __FUNCTION__, port);
        return -NVL_BAD_ARGS;
    }

    // range check index
    if (p->extendedTable && (p->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDEXTTAB_DEPTH))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for extended table\n",
                        __FUNCTION__, p->index);
        return -NVL_BAD_ARGS;
    }

    if (p->index > NV_ROUTE_RIDTABADDR_INDEX_MCRIDTAB_DEPTH)
    {
         NVSWITCH_PRINT(device, ERROR, "%s: index %d out of range for main table\n",
                        __FUNCTION__, p->index);
         return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    nvswitch_os_memset(&table_entry, 0, sizeof(NVSWITCH_MC_RID_ENTRY_LS10));

    table_entry.index = (NvU8)p->index;
    table_entry.use_extended_table = p->extendedTable;

    ret = nvswitch_mc_read_mc_rid_entry_ls10(device, port, &table_entry);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_mc_read_mc_rid_entry_ls10() returned %d\n",
                        __FUNCTION__, ret);
        return ret;
    }

    nvswitch_os_memset(p, 0, sizeof(NVSWITCH_GET_MC_RID_TABLE_PARAMS));

    p->portNum = port;
    p->index = table_entry.index;
    p->extendedTable = table_entry.use_extended_table;

    ret = nvswitch_mc_unwind_directives_ls10(device, table_entry.directives, p->ports,
                                                p->vcHop, p->portsPerSprayGroup, p->replicaOffset,
                                                p->replicaValid);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_mc_unwind_directives_ls10() returned %d\n",
                        __FUNCTION__, ret);
        return ret;
    }

    p->mcSize = table_entry.mcpl_size;
    p->numSprayGroups = table_entry.num_spray_groups;
    p->extendedPtr = table_entry.ext_ptr;
    p->noDynRsp = table_entry.no_dyn_rsp;
    p->extendedValid = table_entry.ext_ptr_valid;
    p->entryValid = table_entry.valid;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_write_fabric_state_ls10
(
    nvswitch_device *device
)
{
    NvU32 reg;

    if (device == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Called with invalid argument\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // bump the sequence number for each write
    device->fabric_state_sequence_number++;

    reg = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH);

    reg = FLD_SET_DRF_NUM(_NVLSAW, _DRIVER_ATTACH_DETACH, _DEVICE_BLACKLIST_REASON,
                          device->device_blacklist_reason, reg);
    reg = FLD_SET_DRF_NUM(_NVLSAW, _DRIVER_ATTACH_DETACH, _DEVICE_FABRIC_STATE,
                          device->device_fabric_state, reg);
    reg = FLD_SET_DRF_NUM(_NVLSAW, _DRIVER_ATTACH_DETACH, _DRIVER_FABRIC_STATE,
                          device->driver_fabric_state, reg);
    reg = FLD_SET_DRF_NUM(_NVLSAW, _DRIVER_ATTACH_DETACH, _EVENT_MESSAGE_COUNT,
                          device->fabric_state_sequence_number, reg);

    NVSWITCH_SAW_WR32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH, reg);

    return NVL_SUCCESS;
}

static NVSWITCH_ENGINE_DESCRIPTOR_TYPE *
_nvswitch_get_eng_descriptor_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine = NULL;

    if (eng_id >= NVSWITCH_ENGINE_ID_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Engine_ID 0x%x out of range 0..0x%x\n",
            __FUNCTION__,
            eng_id, NVSWITCH_ENGINE_ID_SIZE-1);
        return NULL;
    }

    engine = &(chip_device->io.common[eng_id]);
    if (eng_id != engine->eng_id)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Requested Engine_ID 0x%x does not equal found Engine_ID 0x%x (%s)\n",
            __FUNCTION__,
            eng_id, engine->eng_id, engine->eng_name);
    }
    NVSWITCH_ASSERT(eng_id == engine->eng_id);

    return engine;
}

NvU32
nvswitch_get_eng_base_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance
)
{
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine;
    NvU32 base_addr = NVSWITCH_BASE_ADDR_INVALID;

    engine = _nvswitch_get_eng_descriptor_ls10(device, eng_id);
    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s not found\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        return NVSWITCH_BASE_ADDR_INVALID;
    }

    if ((eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) &&
        (eng_instance < engine->eng_count))
    {
        base_addr = engine->uc_addr[eng_instance];
    }
    else if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST)
    {
        base_addr = engine->bc_addr;
    }
    else if ((eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) &&
        (eng_instance < engine->mc_addr_count))
    {
        base_addr = engine->mc_addr[eng_instance];
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown address space type 0x%x (not UC, BC, or MC)\n",
            __FUNCTION__,
            eng_bcast);
    }

    // The NPORT engine can be marked as invalid when it is in Repeater Mode
    if (base_addr == NVSWITCH_BASE_ADDR_INVALID)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: ID 0x%x[%d] %s invalid address\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
    }

    return base_addr;
}

NvU32
nvswitch_get_eng_count_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast
)
{
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine;
    NvU32 eng_count = 0;

    engine = _nvswitch_get_eng_descriptor_ls10(device, eng_id);
    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x %s not found\n",
            __FUNCTION__,
            eng_id,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        return 0;
    }

    if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST)
    {
        eng_count = engine->eng_count;
    }
    else if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST)
    {
        if (engine->bc_addr == NVSWITCH_BASE_ADDR_INVALID)
        {
            eng_count = 0;
        }
        else
        {
            eng_count = 1;
        }
    }
    else if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST)
    {
        eng_count = engine->mc_addr_count;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown address space type 0x%x (not UC, BC, or MC)\n",
            __FUNCTION__,
            eng_bcast);
    }

    return eng_count;
}

NvU32
nvswitch_eng_rd_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance,
    NvU32 offset
)
{
    NvU32 base_addr = NVSWITCH_BASE_ADDR_INVALID;
    NvU32 data;

    base_addr = nvswitch_get_eng_base_ls10(device, eng_id, eng_bcast, eng_instance);
    if (base_addr == NVSWITCH_BASE_ADDR_INVALID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s invalid address\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        NVSWITCH_ASSERT(base_addr != NVSWITCH_BASE_ADDR_INVALID);
        return 0xBADFBADF;
    }

    data = nvswitch_reg_read_32(device, base_addr + offset);

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    {
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine = _nvswitch_get_eng_descriptor_ls10(device, eng_id);

        NVSWITCH_PRINT(device, MMIO,
            "%s: ENG_RD %s(0x%x)[%d] @0x%08x+0x%06x = 0x%08x\n",
            __FUNCTION__,
            engine->eng_name, engine->eng_id,
            eng_instance,
            base_addr, offset,
            data);
    }
#endif  //defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

    return data;
}

void
nvswitch_eng_wr_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance,
    NvU32 offset,
    NvU32 data
)
{
    NvU32 base_addr = NVSWITCH_BASE_ADDR_INVALID;

    base_addr = nvswitch_get_eng_base_ls10(device, eng_id, eng_bcast, eng_instance);
    if (base_addr == NVSWITCH_BASE_ADDR_INVALID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s invalid address\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        NVSWITCH_ASSERT(base_addr != NVSWITCH_BASE_ADDR_INVALID);
        return;
    }

    if (nvswitch_is_tnvl_mode_enabled(device))
    {
        nvswitch_tnvl_eng_wr_32_ls10(device, eng_id, eng_bcast, eng_instance, base_addr, offset, data);
    }
    else
    {
        nvswitch_reg_write_32(device, base_addr + offset, data);
    }

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    {
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine = _nvswitch_get_eng_descriptor_ls10(device, eng_id);

        NVSWITCH_PRINT(device, MMIO,
            "%s: ENG_WR %s(0x%x)[%d] @0x%08x+0x%06x = 0x%08x\n",
            __FUNCTION__,
            engine->eng_name, engine->eng_id,
            eng_instance,
            base_addr, offset,
            data);
    }
#endif  //defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
}

void
nvswitch_reg_write_32_ls10
(
    nvswitch_device *device,
    NvU32 offset,
    NvU32 data
)
{
    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: register write failed at offset 0x%x\n",
            __FUNCTION__, offset);
        return;
    }

    if (nvswitch_is_tnvl_mode_enabled(device))
    {
        nvswitch_tnvl_reg_wr_32_ls10(device, offset, data);
    }
    else
    {
        // Write the register
        nvswitch_os_mem_write32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + offset, data);
    }
}

NvU32
nvswitch_get_link_eng_inst_ls10
(
    nvswitch_device *device,
    NvU32 link_id,
    NVSWITCH_ENGINE_ID eng_id
)
{
    NvU32   eng_instance = NVSWITCH_ENGINE_INSTANCE_INVALID;

    if (link_id >= NVSWITCH_LINK_COUNT(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link ID 0x%x out-of-range [0x0..0x%x]\n",
            __FUNCTION__,
            link_id, NVSWITCH_LINK_COUNT(device)-1);
        return NVSWITCH_ENGINE_INSTANCE_INVALID;
    }

    switch (eng_id)
    {
        case NVSWITCH_ENGINE_ID_NPG:
            eng_instance = link_id / NVSWITCH_LINKS_PER_NPG_LS10;
            break;
        case NVSWITCH_ENGINE_ID_NVLIPT:
            eng_instance = link_id / NVSWITCH_LINKS_PER_NVLIPT_LS10;
            break;
        case NVSWITCH_ENGINE_ID_NVLW:
        case NVSWITCH_ENGINE_ID_NVLW_PERFMON:
            eng_instance = link_id / NVSWITCH_LINKS_PER_NVLW_LS10;
            break;
        case NVSWITCH_ENGINE_ID_MINION:
            eng_instance = link_id / NVSWITCH_LINKS_PER_MINION_LS10;
            break;
        case NVSWITCH_ENGINE_ID_NPORT:
        case NVSWITCH_ENGINE_ID_NVLTLC:
        case NVSWITCH_ENGINE_ID_NVLDL:
        case NVSWITCH_ENGINE_ID_NVLIPT_LNK:
        case NVSWITCH_ENGINE_ID_NPORT_PERFMON:
            eng_instance = link_id;
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: link ID 0x%x has no association with EngID 0x%x\n",
                __FUNCTION__,
                link_id, eng_id);
            eng_instance = NVSWITCH_ENGINE_INSTANCE_INVALID;
            break;
    }

    return eng_instance;
}

NvU32
nvswitch_get_caps_nvlink_version_ls10
(
    nvswitch_device *device
)
{
    ct_assert(NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_4_0 ==
                NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_4_0);
    return NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_4_0;
}


NVSWITCH_BIOS_NVLINK_CONFIG *
nvswitch_get_bios_nvlink_config_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    return &chip_device->bios_config;
}


static void
_nvswitch_init_nport_ecc_control_ls10
(
    nvswitch_device *device
)
{
// Moving this L2 register access to SOE. Refer bug #3747687 
#if 0 
    // Set ingress ECC error limits
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER,
        DRF_NUM(_INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set egress ECC error limits
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER,
        DRF_NUM(_EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER_LIMIT, 1);

    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER,
        DRF_NUM(_EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set route ECC error limits
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER,
        DRF_NUM(_ROUTE, _ERR_NVS_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set tstate ECC error limits
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
        DRF_NUM(_TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT, 1);

    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
        DRF_NUM(_TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set sourcetrack ECC error limits to _PROD value
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _SOURCETRACK, _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
        DRF_NUM(_SOURCETRACK, _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _SOURCETRACK, _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT, 1);

    // Enable ECC/parity
    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _INGRESS, _ERR_ECC_CTRL,
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _NCISOC_HDR_ECC_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _REMAPTAB_ECC_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _RIDTAB_ECC_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _RLANTAB_ECC_ENABLE, __PROD));

    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _EGRESS, _ERR_ECC_CTRL,
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NXBAR_ECC_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NXBAR_PARITY_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _RAM_OUT_ECC_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NCISOC_ECC_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, __PROD));

    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _ROUTE, _ERR_ECC_CTRL,
        DRF_DEF(_ROUTE, _ERR_ECC_CTRL, _GLT_ECC_ENABLE, __PROD) |
        DRF_DEF(_ROUTE, _ERR_ECC_CTRL, _NVS_ECC_ENABLE, __PROD));

    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _TSTATE, _ERR_ECC_CTRL,
        DRF_DEF(_TSTATE, _ERR_ECC_CTRL, _CRUMBSTORE_ECC_ENABLE, __PROD) |
        DRF_DEF(_TSTATE, _ERR_ECC_CTRL, _TAGPOOL_ECC_ENABLE, __PROD));

    NVSWITCH_ENG_WR32(device, NPORT, _BCAST, 0, _SOURCETRACK, _ERR_ECC_CTRL,
        DRF_DEF(_SOURCETRACK, _ERR_ECC_CTRL, _CREQ_TCEN0_CRUMBSTORE_ECC_ENABLE, __PROD));
#endif // 0
}

NvlStatus
nvswitch_init_nport_ls10
(
    nvswitch_device *device
)
{
    NvU32 data32, timeout;
    NvU32 idx_nport;
    NvU32 num_nports;

    num_nports = NVSWITCH_ENG_COUNT(device, NPORT, );

    for (idx_nport = 0; idx_nport < num_nports; idx_nport++)
    {
        // Find the first valid nport
        if (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_nport))
        {
            break;
        }
    }

    // This is a valid case, since all NPORTs can be in Repeater mode.
    if (idx_nport == num_nports)
    {
        NVSWITCH_PRINT(device, INFO, "%s: No valid nports found! Skipping.\n", __FUNCTION__);
        return NVL_SUCCESS;
    }

    _nvswitch_init_nport_ecc_control_ls10(device);

// Moving this L2 register access to SOE. Refer bug #3747687 
#if 0 
    if (DRF_VAL(_SWITCH_REGKEY, _ATO_CONTROL, _DISABLE, device->regkeys.ato_control) ==
        NV_SWITCH_REGKEY_ATO_CONTROL_DISABLE_TRUE)
    {
        // ATO Disable
        data32 = NVSWITCH_NPORT_RD32_LS10(device, idx_nport, _TSTATE, _TAGSTATECONTROL);
        data32 = FLD_SET_DRF(_TSTATE, _TAGSTATECONTROL, _ATO_ENB, _OFF, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _TSTATE, _TAGSTATECONTROL, data32);
    }
    else
    {
        // ATO Enable
        data32 = NVSWITCH_NPORT_RD32_LS10(device, idx_nport, _TSTATE, _TAGSTATECONTROL);
        data32 = FLD_SET_DRF(_TSTATE, _TAGSTATECONTROL, _ATO_ENB, _ON, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _TSTATE, _TAGSTATECONTROL, data32);

        // ATO Timeout value
        timeout = DRF_VAL(_SWITCH_REGKEY, _ATO_CONTROL, _TIMEOUT, device->regkeys.ato_control);
        if (timeout != NV_SWITCH_REGKEY_ATO_CONTROL_TIMEOUT_DEFAULT)
        {
            NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _TSTATE, _ATO_TIMER_LIMIT,
                DRF_NUM(_TSTATE, _ATO_TIMER_LIMIT, _LIMIT, timeout));
        }
    }
#endif // 0
    if (DRF_VAL(_SWITCH_REGKEY, _STO_CONTROL, _DISABLE, device->regkeys.sto_control) ==
        NV_SWITCH_REGKEY_STO_CONTROL_DISABLE_TRUE)
    {
        // STO Disable
        data32 = NVSWITCH_NPORT_RD32_LS10(device, idx_nport, _SOURCETRACK, _CTRL);
        data32 = FLD_SET_DRF(_SOURCETRACK, _CTRL, _STO_ENB, _DISABLED, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _SOURCETRACK, _CTRL, data32);
    }
    else
    {
        // STO Enable
        data32 = NVSWITCH_NPORT_RD32_LS10(device, idx_nport, _SOURCETRACK, _CTRL);
        data32 = FLD_SET_DRF(_SOURCETRACK, _CTRL, _STO_ENB, _ENABLED, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _SOURCETRACK, _CTRL, data32);

        // STO Timeout value
        timeout = DRF_VAL(_SWITCH_REGKEY, _STO_CONTROL, _TIMEOUT, device->regkeys.sto_control);
        if (timeout != NV_SWITCH_REGKEY_STO_CONTROL_TIMEOUT_DEFAULT)
        {
            NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _SOURCETRACK, _MULTISEC_TIMER0,
                DRF_NUM(_SOURCETRACK, _MULTISEC_TIMER0, _TIMERVAL0, timeout));
        }
    }

    NVSWITCH_NPORT_MC_BCAST_WR32_LS10(device, _NPORT, _CONTAIN_AND_DRAIN,
        DRF_DEF(_NPORT, _CONTAIN_AND_DRAIN, _CLEAR, _ENABLE));

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_init_nxbar_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_PRINT(device, WARN, "%s: Function not implemented\n", __FUNCTION__);
    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_clear_nport_rams_ls10
(
    nvswitch_device *device
)
{
    NvU32 idx_nport;
    NvU64 nport_mask = 0;
    NvU32 zero_init_mask;
    NvU32 val;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    // Build the mask of available NPORTs
    for (idx_nport = 0; idx_nport < NVSWITCH_ENG_COUNT(device, NPORT, ); idx_nport++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_nport))
        {
            nport_mask |= NVBIT64(idx_nport);
        }
    }

    // Start the HW zero init
    zero_init_mask =
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_0, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _LINKTABLEINIT, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _REMAPTABINIT,  _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _RIDTABINIT,    _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _RLANTABINIT,   _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _MCREMAPTABINIT, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _MCTAGSTATEINIT, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _RDTAGSTATEINIT, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _MCREDSGTINIT,  _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _MCREDBUFINIT,  _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _MCRIDINIT,     _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _EXTMCRIDINIT,  _HWINIT);

    NVSWITCH_BCAST_WR32_LS10(device, NPORT, _NPORT, _INITIALIZATION,
        zero_init_mask);

    nvswitch_timeout_create(25 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Check each enabled NPORT that is still pending until all are done
        for (idx_nport = 0; idx_nport < NVSWITCH_ENG_COUNT(device, NPORT, ); idx_nport++)
        {
            if (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_nport) && (nport_mask & NVBIT64(idx_nport)))
            {
                val = NVSWITCH_ENG_RD32_LS10(device, NPORT, idx_nport, _NPORT, _INITIALIZATION);
                if (val == zero_init_mask)
                {
                    nport_mask &= ~NVBIT64(idx_nport);
                }
            }
        }

        if (nport_mask == 0)
        {
            break;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if (nport_mask != 0)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Timeout waiting for NV_NPORT_INITIALIZATION (0x%llx)\n",
            __FUNCTION__, nport_mask);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_SET_RESIDENCY_BINS
 */
static NvlStatus
nvswitch_ctrl_set_residency_bins_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_RESIDENCY_BINS *p
)
{
    NvU64 threshold;
    NvU64 max_threshold;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (p->bin.lowThreshold > p->bin.hiThreshold )
    {
        NVSWITCH_PRINT(device, ERROR,
            "SET_RESIDENCY_BINS: Low threshold (%d) > Hi threshold (%d)\n",
            p->bin.lowThreshold, p->bin.hiThreshold);
        return -NVL_BAD_ARGS;
    }

    if (p->table_select == NVSWITCH_TABLE_SELECT_MULTICAST)
    {
        max_threshold = DRF_MASK(NV_MULTICASTTSTATE_STAT_RESIDENCY_BIN_CTRL_HIGH_LIMIT);

        threshold = (NvU64) p->bin.hiThreshold * 1333 / 1000;
        if (threshold > max_threshold)
        {
            NVSWITCH_PRINT(device, ERROR,
                "SET_RESIDENCY_BINS: Threshold overflow.  %u > %llu max\n",
                p->bin.hiThreshold, max_threshold * 1000 / 1333);
            return -NVL_BAD_ARGS;
        }
        NVSWITCH_NPORT_BCAST_WR32_LS10(device, _MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH,
            DRF_NUM(_MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH, _LIMIT, (NvU32)threshold));

        threshold = (NvU64)p->bin.lowThreshold * 1333 / 1000;
        NVSWITCH_NPORT_BCAST_WR32_LS10(device, _MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW,
            DRF_NUM(_MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW, _LIMIT, (NvU32)threshold));
    }
    else if (p->table_select == NVSWITCH_TABLE_SELECT_REDUCTION)
    {
        max_threshold = DRF_MASK(NV_REDUCTIONTSTATE_STAT_RESIDENCY_BIN_CTRL_HIGH_LIMIT);

        threshold = (NvU64) p->bin.hiThreshold * 1333 / 1000;
        if (threshold > max_threshold)
        {
            NVSWITCH_PRINT(device, ERROR,
                "SET_RESIDENCY_BINS: Threshold overflow.  %u > %llu max\n",
                p->bin.hiThreshold, max_threshold * 1000 / 1333);
            return -NVL_BAD_ARGS;
        }
        NVSWITCH_NPORT_BCAST_WR32_LS10(device, _REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH,
            DRF_NUM(_REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH, _LIMIT, (NvU32)threshold));

        threshold = (NvU64)p->bin.lowThreshold * 1333 / 1000;
        NVSWITCH_NPORT_BCAST_WR32_LS10(device, _REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW,
            DRF_NUM(_REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW, _LIMIT, (NvU32)threshold));
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "SET_RESIDENCY_BINS: Invalid table %d\n", p->table_select);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return NVL_SUCCESS;
}

#define NVSWITCH_RESIDENCY_BIN_SIZE                                 \
    ((NV_MULTICASTTSTATE_STAT_RESIDENCY_COUNT_CTRL_INDEX_MAX + 1) / \
     NV_MULTICASTTSTATE_STAT_RESIDENCY_COUNT_CTRL_INDEX_MCID_STRIDE)

/*
 * CTRL_NVSWITCH_GET_RESIDENCY_BINS
 */
static NvlStatus
nvswitch_ctrl_get_residency_bins_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_RESIDENCY_BINS *p
)
{
    NvU64 val;
    NvU64 val_hi;
    NvU32 i;
    NvU64 threshold;

    ct_assert(
        NV_MULTICASTTSTATE_STAT_RESIDENCY_COUNT_CTRL_INDEX_MCID_STRIDE ==
        NV_REDUCTIONTSTATE_STAT_RESIDENCY_COUNT_CTRL_INDEX_MCID_STRIDE);
    ct_assert(
        NV_MULTICASTTSTATE_STAT_RESIDENCY_COUNT_CTRL_INDEX_MAX ==
        NV_REDUCTIONTSTATE_STAT_RESIDENCY_COUNT_CTRL_INDEX_MAX);

    ct_assert(NVSWITCH_RESIDENCY_BIN_SIZE == NVSWITCH_RESIDENCY_SIZE);

    if (!nvswitch_is_link_valid(device, p->link))
    {
        NVSWITCH_PRINT(device, ERROR,
            "GET_RESIDENCY_BINS: Invalid link %d\n", p->link);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (p->table_select == NVSWITCH_TABLE_SELECT_MULTICAST)
    {
        // Snap the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL,
            DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));

        // Read high/low thresholds and convery clocks to nsec
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW);
        threshold = DRF_VAL(_MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW, _LIMIT, val);
        p->bin.lowThreshold = threshold * 1000 / 1333;

        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH);
        threshold = DRF_VAL(_MULTICASTTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH, _LIMIT, val);
        p->bin.hiThreshold = threshold * 1000 / 1333;

        NVSWITCH_NPORT_WR32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_CTRL,
            DRF_NUM(_MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_CTRL, _INDEX, 0) |
            DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_CTRL, _AUTOINCR, _ON));
        for (i = 0; i < NVSWITCH_RESIDENCY_BIN_SIZE; i++)
        {
            // Low
            val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            p->residency[i].low = (val_hi << 32) | val;

            // Medium
            val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            p->residency[i].medium = (val_hi << 32) | val;

            // High
            val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            p->residency[i].high = (val_hi << 32) | val;
        }

        // Reset the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL,
            DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_MULTICASTTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));

    }
    else if (p->table_select == NVSWITCH_TABLE_SELECT_REDUCTION)
    {
        // Snap the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL,
            DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));

        // Read high/low thresholds and convery clocks to nsec
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW);
        threshold = DRF_VAL(_REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_LOW, _LIMIT, val);
        p->bin.lowThreshold = threshold * 1000 / 1333;

        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH);
        threshold = DRF_VAL(_REDUCTIONTSTATE, _STAT_RESIDENCY_BIN_CTRL_HIGH, _LIMIT, val);
        p->bin.hiThreshold = threshold * 1000 / 1333;

        NVSWITCH_NPORT_WR32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_CTRL,
            DRF_NUM(_REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_CTRL, _INDEX, 0) |
            DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_CTRL, _AUTOINCR, _ON));
        for (i = 0; i < NVSWITCH_RESIDENCY_BIN_SIZE; i++)
        {
            // Low
            val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            p->residency[i].low = (val_hi << 32) | val;

            // Medium
            val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            p->residency[i].medium = (val_hi << 32) | val;

            // High
            val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_COUNT_DATA);
            p->residency[i].high = (val_hi << 32) | val;
        }

        // Reset the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL,
            DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_REDUCTIONTSTATE, _STAT_RESIDENCY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "GET_RESIDENCY_BINS: Invalid table %d\n", p->table_select);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_GET_RB_STALL_BUSY
 */
static NvlStatus
nvswitch_ctrl_get_rb_stall_busy_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_RB_STALL_BUSY *p
)
{
    NvU64 val;
    NvU64 val_hi;

    if (!nvswitch_is_link_valid(device, p->link))
    {
        NVSWITCH_PRINT(device, ERROR,
            "GET_RB_STALL_BUSY: Invalid link %d\n", p->link);
        return -NVL_BAD_ARGS;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (p->table_select == NVSWITCH_TABLE_SELECT_MULTICAST)
    {
        // Snap the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL,
            DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));

        //
        // VC0
        // 

        // Total time
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_WINDOW_0_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_WINDOW_0_HIGH);
        p->vc0.time = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Busy
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_BUSY_TIMER_0_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_BUSY_TIMER_0_HIGH);
        p->vc0.busy = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Stall
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_STALL_TIMER_0_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_STALL_TIMER_0_HIGH);
        p->vc0.stall = ((val_hi << 32) | val) * 1000 / 1333;     // in ns

        //
        // VC1
        // 

        // Total time
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_WINDOW_1_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_WINDOW_1_HIGH);
        p->vc1.time = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Busy
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_BUSY_TIMER_1_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_BUSY_TIMER_1_HIGH);
        p->vc1.busy = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Stall
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_STALL_TIMER_1_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_STALL_TIMER_1_HIGH);
        p->vc1.stall = ((val_hi << 32) | val) * 1000 / 1333;     // in ns

        // Reset the busy/stall counters
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL,
            DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_MULTICASTTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));
    }
    else if (p->table_select == NVSWITCH_TABLE_SELECT_REDUCTION)
    {
        // Snap the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL,
            DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _ENABLE));
        //
        // VC0
        // 

        // Total time
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_WINDOW_0_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_WINDOW_0_HIGH);
        p->vc0.time = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Busy
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_BUSY_TIMER_0_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_BUSY_TIMER_0_HIGH);
        p->vc0.busy = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Stall
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_STALL_TIMER_0_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_STALL_TIMER_0_HIGH);
        p->vc0.stall = ((val_hi << 32) | val) * 1000 / 1333;     // in ns

        //
        // VC1
        // 

        // Total time
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_WINDOW_1_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_WINDOW_1_HIGH);
        p->vc1.time = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Busy
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_BUSY_TIMER_1_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_BUSY_TIMER_1_HIGH);
        p->vc1.busy = ((val_hi << 32) | val) * 1000 / 1333;      // in ns

        // Stall
        val = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_STALL_TIMER_1_LOW);
        val_hi = NVSWITCH_NPORT_RD32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_STALL_TIMER_1_HIGH);
        p->vc1.stall = ((val_hi << 32) | val) * 1000 / 1333;     // in ns

        // Reset the histogram
        NVSWITCH_NPORT_WR32_LS10(device, p->link, _REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL,
            DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _ENABLE_TIMER, _ENABLE) |
            DRF_DEF(_REDUCTIONTSTATE, _STAT_STALL_BUSY_CONTROL, _SNAP_ON_DEMAND, _DISABLE));
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "GET_RB_STALL_BUSY: Invalid table %d\n", p->table_select);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR
 */
static NvlStatus
nvswitch_ctrl_get_multicast_id_error_vector_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR *p
)
{
    NvU32 i;

    ct_assert(NV_NPORT_MCID_ERROR_VECTOR__SIZE_1 == (NVSWITCH_MC_ID_ERROR_VECTOR_COUNT / 32));

    if (!NVSWITCH_IS_LINK_ENG_VALID(device, p->link, NPORT))
    {
        NVSWITCH_PRINT(device, ERROR,
            "GET_MULTICAST_ID_ERROR_VECTOR: Invalid link %d\n", p->link);
        return -NVL_BAD_ARGS;
    }

    for (i = 0; i < NV_NPORT_MCID_ERROR_VECTOR__SIZE_1; i++)
    {
        p->error_vector[i] = NVSWITCH_LINK_RD32(device, p->link, NPORT, _NPORT, 
            _MCID_ERROR_VECTOR(i));
    }

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR
 */
static NvlStatus
nvswitch_ctrl_clear_multicast_id_error_vector_ls10
(
    nvswitch_device *device,
    NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR *p
)
{
    NvU32 i;

    ct_assert(NV_NPORT_MCID_ERROR_VECTOR__SIZE_1 == (NVSWITCH_MC_ID_ERROR_VECTOR_COUNT / 32));

    if (!NVSWITCH_IS_LINK_ENG_VALID(device, p->link, NPORT))
    {
        NVSWITCH_PRINT(device, ERROR,
            "CLEAR_MULTICAST_ID_ERROR_VECTOR: Invalid link %d\n", p->link);
        return -NVL_BAD_ARGS;
    }

    for (i = 0; i < NV_NPORT_MCID_ERROR_VECTOR__SIZE_1; i++)
    {
        NVSWITCH_LINK_WR32(device, p->link, NPORT, _NPORT, 
            _MCID_ERROR_VECTOR(i), p->error_vector[i]);
    }

    return NVL_SUCCESS;
}

void
nvswitch_load_uuid_ls10
(
    nvswitch_device *device
)
{
    NvU32 regData[4];

    //
    // Read 128-bit UUID from secure scratch registers which must be
    // populated by firmware.
    //
    regData[0] = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _SECURE_SCRATCH_WARM_GROUP_1(0));
    regData[1] = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _SECURE_SCRATCH_WARM_GROUP_1(1));
    regData[2] = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _SECURE_SCRATCH_WARM_GROUP_1(2));
    regData[3] = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _SECURE_SCRATCH_WARM_GROUP_1(3));

    nvswitch_os_memcpy(&device->uuid.uuid, (NvU8 *)regData, NV_UUID_LEN);
}

NvlStatus
nvswitch_launch_ALI_ls10
(
    nvswitch_device *device
)
{
    NvU64 enabledLinkMask;
    NvU64 forcedConfgLinkMask = 0;
    NvBool bEnableAli = NV_FALSE;
    NvU64 i           = 0;
    nvlink_link *link;

    enabledLinkMask   = nvswitch_get_enabled_link_mask(device);
    forcedConfgLinkMask = ((NvU64)device->regkeys.chiplib_forced_config_link_mask) +
                ((NvU64)device->regkeys.chiplib_forced_config_link_mask2 << 32);

    //
    // Currently, we don't support a mix of forced/auto config links
    // return early
    //
    if (forcedConfgLinkMask != 0)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

#ifdef INCLUDE_NVLINK_LIB
    bEnableAli = device->nvlink_device->enableALI;
#endif

    if (!bEnableAli)
    {
        NVSWITCH_PRINT(device, INFO,
                "%s: ALI not supported on the given device\n",
                __FUNCTION__);
        return NVL_ERR_GENERIC;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, enabledLinkMask)
    {
        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLIPT_LNK, link->linkNumber) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        if (cciIsLinkManaged(device, i))
        {
            continue;
        }
        nvswitch_launch_ALI_link_training(device, link, NV_FALSE);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_set_training_mode_ls10
(
    nvswitch_device *device
)
{
    NvU64 enabledLinkMask, forcedConfgLinkMask;

    NvU32 regVal;
    NvU64 i = 0;
    nvlink_link *link;

    enabledLinkMask     = nvswitch_get_enabled_link_mask(device);
    forcedConfgLinkMask = ((NvU64)device->regkeys.chiplib_forced_config_link_mask) +
                ((NvU64)device->regkeys.chiplib_forced_config_link_mask2 << 32);

    //
    // Currently, we don't support a mix of forced/auto config links
    // return early
    //
    if (forcedConfgLinkMask != 0)
    {
        NVSWITCH_PRINT(device, INFO,
                "%s: Forced-config set, skipping setting up link training selection\n",
                __FUNCTION__);
        return NVL_SUCCESS;
    }

    if (device->regkeys.link_training_mode == NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_ALI)
    {
        //
        // If ALI is force enabled then check to make sure ALI is supported
        // and write to the SYSTEM_CTRL register to force it to enabled
        //
        FOR_EACH_INDEX_IN_MASK(64, i, enabledLinkMask)
        {
            NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

            link = nvswitch_get_link(device, i);

            if ((link == NULL) ||
                !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLIPT_LNK, link->linkNumber) ||
                (i >= NVSWITCH_NVLINK_MAX_LINKS))
            {
                continue;
            }

            regVal = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                        _CTRL_CAP_LOCAL_LINK_CHANNEL);

            if (!FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CAP_LOCAL_LINK_CHANNEL, _ALI_SUPPORT, _SUPPORTED, regVal))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: ALI training not supported! Regkey forcing ALI will be ignored\n",__FUNCTION__);
                return -NVL_ERR_NOT_SUPPORTED;
            }

            NVSWITCH_PRINT(device, INFO,
                "%s: ALI training set on link: 0x%llx\n",
                __FUNCTION__, i);

            regVal = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                        _CTRL_SYSTEM_LINK_CHANNEL_CTRL2);

            regVal = FLD_SET_DRF(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2, _ALI_ENABLE, _ENABLE, regVal);
            NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                        _CTRL_SYSTEM_LINK_CHANNEL_CTRL2, regVal);

        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    else if (device->regkeys.link_training_mode == NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_NON_ALI)
    {
        // If non-ALI is force enabled then disable ALI
        FOR_EACH_INDEX_IN_MASK(64, i, enabledLinkMask)
        {
            NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

            link = nvswitch_get_link(device, i);

            if ((link == NULL) ||
                !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLIPT_LNK, link->linkNumber) ||
                (i >= NVSWITCH_NVLINK_MAX_LINKS))
            {
                continue;
            }

            regVal = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                        _CTRL_SYSTEM_LINK_CHANNEL_CTRL2);

            regVal = FLD_SET_DRF(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2, _ALI_ENABLE, _DISABLE, regVal);
            NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                        _CTRL_SYSTEM_LINK_CHANNEL_CTRL2, regVal);

        }
        FOR_EACH_INDEX_IN_MASK_END;

    }
    else
    {
        // Else sanity check the SYSTEM register settings
        FOR_EACH_INDEX_IN_MASK(64, i, enabledLinkMask)
        {
            NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

            link = nvswitch_get_link(device, i);

            if ((link == NULL) ||
                !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLIPT_LNK, link->linkNumber) ||
                (i >= NVSWITCH_NVLINK_MAX_LINKS))
            {
                continue;
            }

            regVal = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                        _CTRL_SYSTEM_LINK_CHANNEL_CTRL2);

            if (FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2, _ALI_ENABLE, _ENABLE, regVal))
            {

                regVal = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                            _CTRL_CAP_LOCAL_LINK_CHANNEL);

                if (!FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CAP_LOCAL_LINK_CHANNEL, _ALI_SUPPORT, _SUPPORTED, regVal))
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: ALI training not supported! Non-ALI will be used as the default.\n",__FUNCTION__);
#ifdef INCLUDE_NVLINK_LIB
                    device->nvlink_device->enableALI = NV_FALSE;
#endif
                    return NVL_SUCCESS;
                }
#ifdef INCLUDE_NVLINK_LIB
                device->nvlink_device->enableALI = NV_TRUE;
#endif
            }
            else
            {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: ALI training not enabled! Non-ALI will be used as the default.\n",__FUNCTION__);
#ifdef INCLUDE_NVLINK_LIB
                    device->nvlink_device->enableALI = NV_FALSE;
#endif
                    return NVL_SUCCESS;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }

    return NVL_SUCCESS;
}

static void
_nvswitch_get_nvlink_power_state_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret
)
{
    nvlink_link *link;
    NvU32 linkState;
    NvU32 linkPowerState;
    NvU8 i;

    // Determine power state for each enabled link
    FOR_EACH_INDEX_IN_MASK(64, i, ret->enabledLinkMask)
    {
        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        linkState = ret->linkInfo[i].linkState;

        switch (linkState)
        {
            case NVSWITCH_NVLINK_STATUS_LINK_STATE_ACTIVE:
                linkPowerState = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _PWRM_CTRL);

                if (FLD_TEST_DRF(_NVLIPT_LNK, _PWRM_CTRL, _L1_CURRENT_STATE, _L1, linkPowerState))
                {
                    linkPowerState = NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_L1;
                }
                else
                {
                    linkPowerState = NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_L0;
                }
                break;

            default:
                linkPowerState  = NVSWITCH_NVLINK_STATUS_LINK_POWER_STATE_INVALID;
                break;
        }

        ret->linkInfo[i].linkPowerState = linkPowerState;
    }
    FOR_EACH_INDEX_IN_MASK_END;
}

NvlStatus
nvswitch_ctrl_get_nvlink_status_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret
)
{
    NvlStatus retval = NVL_SUCCESS;

    retval = nvswitch_ctrl_get_nvlink_status_lr10(device, ret);

    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    _nvswitch_get_nvlink_power_state_ls10(device, ret);

    return retval;
}

NvlStatus
nvswitch_parse_bios_image_ls10
(
    nvswitch_device *device
)
{
    return nvswitch_parse_bios_image_lr10(device);
}

static NvlStatus
nvswitch_split_and_send_inband_data_ls10
(
    nvswitch_device      *device,
    NvU32                linkId,
    nvswitch_inband_send_data *inBandData
)
{
    NvlStatus status = NVL_SUCCESS;
    NvU32 i;
    NvU32 bytes;
    NvU32 maxSplitSize = NVLINK_INBAND_MAX_XFER_SIZE;
    NvU32 totalBytesToSend = inBandData->bufferSize;
    NvU32 numChunks = NV_ALIGN_UP(inBandData->bufferSize, maxSplitSize) /
                       maxSplitSize;

    inBandData->hdr.data = NVLINK_INBAND_DRV_HDR_TYPE_START;
    bytes = NV_MIN(totalBytesToSend, maxSplitSize);

    NVSWITCH_ASSERT(numChunks != 0);

    for (i = 0; i < numChunks; i++)
    {
        inBandData->bufferSize = bytes;
        // Last chunk
        if (i == (numChunks - 1))
        {
            //
            // A chunk can have both _START and _END set at the same time, if it
            // is the only chunk being sent.
            //
            inBandData->hdr.data |= NVLINK_INBAND_DRV_HDR_TYPE_END; 
            inBandData->hdr.data &= ~NVLINK_INBAND_DRV_HDR_TYPE_MID; // clear
        }

        status = nvswitch_minion_send_inband_data_ls10(device, linkId, inBandData);
        if (status != NVL_SUCCESS)
            return status;

        inBandData->sendBuffer += bytes;
        totalBytesToSend -= bytes;

        bytes = NV_MIN(totalBytesToSend, maxSplitSize);
        inBandData->hdr.data = NVLINK_INBAND_DRV_HDR_TYPE_MID;
    }

    return NVL_SUCCESS;
}

void
nvswitch_send_inband_nack_ls10
(
    nvswitch_device *device,
    NvU32 *hdr,
    NvU32  linkId
)
{
    NvlStatus status;
    nvswitch_inband_send_data inBandData;
    nvlink_inband_msg_header_t *msghdr = (nvlink_inband_msg_header_t *)hdr;

    msghdr->status = NV_ERR_FABRIC_MANAGER_NOT_PRESENT;
    switch (msghdr->type)
    {
         case NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ_V2:
         case NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ:
               msghdr->type   = NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP;
               break;
         default:
               NVSWITCH_PRINT(device, ERROR, "%s:Wrong HDR type for NACK\n",
                             __FUNCTION__);
               return;
    }
    msghdr->length = 0;

    inBandData.sendBuffer = (NvU8 *)msghdr;
    inBandData.bufferSize = sizeof(nvlink_inband_msg_header_t);

    status = nvswitch_split_and_send_inband_data_ls10(device, linkId, &inBandData);

    if (status != NVL_SUCCESS)
    {
         NVSWITCH_PRINT(device, ERROR, "%s:Sending NACK failed\n",
                         __FUNCTION__);
    }
}

NvU32
nvswitch_get_max_persistent_message_count_ls10
(
    nvswitch_device *device
)
{
    return NUM_MAX_MCFLA_SLOTS_LS10;
}

/*
 * CTRL_NVSWITCH_INBAND_SEND_DATA
 */
NvlStatus
nvswitch_ctrl_inband_send_data_ls10
(
    nvswitch_device *device,
    NVSWITCH_INBAND_SEND_DATA_PARAMS *p
)
{
    NvlStatus status;
    nvswitch_inband_send_data inBandData;

    ct_assert(NVLINK_INBAND_MAX_MSG_SIZE == NVSWITCH_INBAND_DATA_SIZE);

    if (p->dataSize == 0 || p->dataSize > NVSWITCH_INBAND_DATA_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR, "Bad Inband data, got buffer of 0. Skipping Inband Send\n");
        return -NVL_BAD_ARGS;
    }

    if (!device->hal.nvswitch_is_link_valid(device, p->linkId))
    {
        NVSWITCH_PRINT(device, ERROR, "Bad linkId %d is wrong\n", p->linkId);
        return -NVL_BAD_ARGS;
    }

    inBandData.sendBuffer = p->buffer;
    inBandData.bufferSize = p->dataSize;

    status = nvswitch_split_and_send_inband_data_ls10(device, p->linkId, &inBandData);

    if (status != NVL_SUCCESS)
    {
         return status;
    }

    p->dataSent = p->dataSize;

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_INBAND_READ_DATA
 */
NvlStatus
nvswitch_ctrl_inband_read_data_ls10
(
    nvswitch_device *device,
    NVSWITCH_INBAND_READ_DATA_PARAMS *p
)
{
    if (!device->hal.nvswitch_is_link_valid(device, p->linkId))
    {
        NVSWITCH_PRINT(device, ERROR, "Bad linkId %d is wrong\n", p->linkId);
        return -NVL_BAD_ARGS;
    }

    return nvswitch_inband_read_data(device, p->buffer, p->linkId, &p->dataSize);
}

/*
 * CTRL_NVSWITCH_GET_BOARD_PART_NUMBER
 */
NvlStatus
nvswitch_ctrl_get_board_part_number_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR *p
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_OBD_OBJECT_V2_XX *pOBDObj;
    int byteIdx;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!pInforom->OBD.bValid)
    {
        NVSWITCH_PRINT(device, ERROR, "OBD data is not available\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pOBDObj = &pInforom->OBD.object.v2;

    if (sizeof(p->data) != sizeof(pOBDObj->productPartNumber)/sizeof(inforom_U008))
    {
        NVSWITCH_PRINT(device, ERROR,
                       "board part number available size %lu is not same as the request size %lu\n",
                       sizeof(pOBDObj->productPartNumber)/sizeof(inforom_U008), sizeof(p->data));
        return -NVL_ERR_GENERIC;
    }

    nvswitch_os_memset(p, 0, sizeof(NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR));

    /* Copy board type data */
    for (byteIdx = 0; byteIdx < NVSWITCH_BOARD_PART_NUMBER_SIZE_IN_BYTES; byteIdx++)
    {
        p->data[byteIdx] =(NvU8)(pOBDObj->productPartNumber[byteIdx] & 0xFF);
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_nvlink_lp_counters_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS *params
)
{
    NvU32 counterValidMaskOut;
    NvU32 counterValidMask;
    NvU32 cntIdx;
    NV_STATUS status;
    NvU32 statData;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, params->linkId))
    {
        return -NVL_BAD_ARGS;
    }

    counterValidMaskOut = 0;
    counterValidMask = params->counterValidMask;

    cntIdx = CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_NVHS;
    if (counterValidMask & NVBIT32(cntIdx))
    {
        status = nvswitch_minion_get_dl_status(device, params->linkId,
                                NV_NVLSTAT_TX01, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
        params->counterValues[cntIdx] = DRF_VAL(_NVLSTAT_TX01, _COUNT_TX_STATE,
                                                _NVHS_VALUE, statData);
        counterValidMaskOut |= NVBIT32(cntIdx);
    }

    cntIdx = CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_OTHER;
    if (counterValidMask & NVBIT32(cntIdx))
    {
        status = nvswitch_minion_get_dl_status(device, params->linkId,
                                NV_NVLSTAT_TX02, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
        params->counterValues[cntIdx] = DRF_VAL(_NVLSTAT_TX02, _COUNT_TX_STATE,
                                                _OTHER_VALUE, statData);
        counterValidMaskOut |= NVBIT32(cntIdx);
    }

    cntIdx = CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_NUM_TX_LP_ENTER;
    if (counterValidMask & NVBIT32(cntIdx))
    {
        status = nvswitch_minion_get_dl_status(device, params->linkId,
                                NV_NVLSTAT_TX06, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
        params->counterValues[cntIdx] = DRF_VAL(_NVLSTAT_TX06, _NUM_LCL,
                                                _LP_ENTER_VALUE, statData);
        counterValidMaskOut |= NVBIT32(cntIdx);
    }

    cntIdx = CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_NUM_TX_LP_EXIT;
    if (counterValidMask & NVBIT32(cntIdx))
    {
        status = nvswitch_minion_get_dl_status(device, params->linkId,
                                NV_NVLSTAT_TX05, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
        params->counterValues[cntIdx] = DRF_VAL(_NVLSTAT_TX05, _NUM_LCL,
                                                _LP_EXIT_VALUE, statData);
        counterValidMaskOut |= NVBIT32(cntIdx);
    }

    cntIdx = CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS_COUNT_TX_SLEEP;
    if (counterValidMask & NVBIT32(cntIdx))
    {
        status = nvswitch_minion_get_dl_status(device, params->linkId,
                                NV_NVLSTAT_TX10, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
        params->counterValues[cntIdx] = DRF_VAL(_NVLSTAT_TX10, _COUNT_TX_STATE,
                                                _SLEEP_VALUE, statData);
        counterValidMaskOut |= NVBIT32(cntIdx);
    }
    
    params->counterValidMask = counterValidMaskOut;

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_clear_counters_ls10
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_CLEAR_COUNTERS_PARAMS *ret
)
{
    nvlink_link *link;
    NvU8 i;
    NvU32 counterMask;
    NvlStatus status = NVL_SUCCESS;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    counterMask = ret->counterMask;

    FOR_EACH_INDEX_IN_MASK(64, i, ret->linkMask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            continue;
        }

        counterMask = ret->counterMask;

        if ((counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS) ||
            (counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL))
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                NV_MINION_NVLINK_DL_CMD_COMMAND_DLSTAT_CLR_MINION_MISCCNT, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR, "%s : Failed to clear misc count to MINION for link # %d\n",
                    __FUNCTION__, link->linkNumber);
            }
            counterMask &=
                ~(NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS | NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL);
        }

        nvswitch_ctrl_clear_throughput_counters_ls10(device, link, counterMask);
        nvswitch_ctrl_clear_lp_counters_ls10(device, link, counterMask);
        status = nvswitch_ctrl_clear_dl_error_counters_ls10(device, link, counterMask);

        // Return early with failure on clearing through minion
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failure on clearing link counter mask 0x%x on link %d\n",
                __FUNCTION__, counterMask, link->linkNumber);
            break;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

NvlStatus
nvswitch_ctrl_set_nvlink_error_threshold_ls10
(
    nvswitch_device *device,
    NVSWITCH_SET_NVLINK_ERROR_THRESHOLD_PARAMS *pParams
)
{
    nvlink_link *link;
    NvU8 i;

    FOR_EACH_INDEX_IN_MASK(64, i, pParams->link_mask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            continue;
        }

        if (pParams->errorThreshold[link->linkNumber].flags & NVSWITCH_NVLINK_ERROR_THRESHOLD_RESET)
        {
            link->errorThreshold.bUserConfig = NV_FALSE;

            // Disable the interrupt
            nvswitch_configure_error_rate_threshold_interrupt_ls10(link, NV_FALSE);

            // Set to default value
            nvswitch_set_error_rate_threshold_ls10(link, NV_TRUE);

            // Enable the interrupt
            nvswitch_configure_error_rate_threshold_interrupt_ls10(link, NV_TRUE);
        }
        else
        {
            link->errorThreshold.thresholdMan =
                pParams->errorThreshold[link->linkNumber].thresholdMan;
            link->errorThreshold.thresholdExp =
                pParams->errorThreshold[link->linkNumber].thresholdExp;
            link->errorThreshold.timescaleMan =
                pParams->errorThreshold[link->linkNumber].timescaleMan;
            link->errorThreshold.timescaleExp =
                pParams->errorThreshold[link->linkNumber].timescaleExp;
            link->errorThreshold.bInterruptEn =
                pParams->errorThreshold[link->linkNumber].bInterruptEn;
            link->errorThreshold.bUserConfig = NV_TRUE;

            // Disable the interrupt
            nvswitch_configure_error_rate_threshold_interrupt_ls10(link, NV_FALSE);

            // Set the Error threshold
            nvswitch_set_error_rate_threshold_ls10(link, NV_FALSE);

            // Configure the interrupt
            nvswitch_configure_error_rate_threshold_interrupt_ls10(link,
                                            pParams->errorThreshold[link->linkNumber].bInterruptEn);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_nvlink_error_threshold_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_THRESHOLD_PARAMS *pParams
)
{
    nvlink_link *link;
    NvU8 i;

    FOR_EACH_INDEX_IN_MASK(64, i, pParams->link_mask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            continue;
        }

        // Get the Error threshold
        nvswitch_get_error_rate_threshold_ls10(link);

        pParams->errorThreshold[link->linkNumber].thresholdMan =
            link->errorThreshold.thresholdMan;
        pParams->errorThreshold[link->linkNumber].thresholdExp =
            link->errorThreshold.thresholdExp;
        pParams->errorThreshold[link->linkNumber].timescaleMan =
            link->errorThreshold.timescaleMan;
        pParams->errorThreshold[link->linkNumber].timescaleExp =
            link->errorThreshold.timescaleExp;
        pParams->errorThreshold[link->linkNumber].bInterruptEn =
            link->errorThreshold.bInterruptEn;
        pParams->errorThreshold[link->linkNumber].bInterruptTrigerred =
            link->errorThreshold.bInterruptTrigerred;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_get_board_id_ls10
(
    nvswitch_device *device,
    NvU16 *pBoardId
)
{
    NvlStatus ret;
    NvU32 biosOemVersionBytes;

    if (pBoardId == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    // Check if bios valid
    ret = nvswitch_lib_get_bios_version(device, NULL);
    if (ret != NVL_SUCCESS)
    {
        return ret;
    }

    biosOemVersionBytes = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW_SW, 
                                                 _OEM_BIOS_VERSION);
    *pBoardId = DRF_VAL(_NVLSAW_SW, _OEM_BIOS_VERSION, _BOARD_ID, biosOemVersionBytes);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_check_io_sanity_ls10
(
    nvswitch_device *device
)
{
    NvBool keepPolling;
    NVSWITCH_TIMEOUT timeout;
    NvU32   val;
    NvBool error = NV_FALSE;
    NvU32 sxid;
    const char *sxid_desc = NULL;

    //
    // NOTE: MMIO discovery has not been performed so only constant BAR0 offset
    // addressing can be performed.
    //

    // BAR0 offset 0 should always contain valid data -- unless it doesn't
    val = NVSWITCH_OFF_RD32(device, 0);
    if (val == 0)
    {
        error = NV_TRUE;
        sxid = NVSWITCH_ERR_HW_HOST_FIRMWARE_RECOVERY_MODE;
        sxid_desc = "Firmware recovery mode";
    }
    else if ((val == 0xFFFFFFFF) || ((val & 0xFFFF0000) == 0xBADF0000))
    {
        error = NV_TRUE;
        sxid = NVSWITCH_ERR_HW_HOST_IO_FAILURE;
        sxid_desc = "IO failure";
    }
    else if (!IS_FMODEL(device))
    {
        // check if FSP successfully started
        nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            val = NVSWITCH_REG_RD32(device, _GFW_GLOBAL, _BOOT_PARTITION_PROGRESS);
            if (FLD_TEST_DRF(_GFW_GLOBAL, _BOOT_PARTITION_PROGRESS, _VALUE, _SUCCESS, val))
            {
                break;
            }

            nvswitch_os_sleep(1);
        }
        while (keepPolling);
        if (!FLD_TEST_DRF(_GFW_GLOBAL, _BOOT_PARTITION_PROGRESS, _VALUE, _SUCCESS, val))
        {
            error = NV_TRUE;
            sxid = NVSWITCH_ERR_HW_HOST_FIRMWARE_INITIALIZATION_FAILURE;
            sxid_desc = "Firmware initialization failure";
        }
    }

    if (error)
    {
        NVSWITCH_RAW_ERROR_LOG_TYPE report = { 0, { 0 } };
        NVSWITCH_RAW_ERROR_LOG_TYPE report_saw = {0, { 0 }};
        NvU32 report_idx = 0;
        NvU32 i;

        val = NVSWITCH_REG_RD32(device, _GFW_GLOBAL, _BOOT_PARTITION_PROGRESS);
        report.data[report_idx++] = val;
        NVSWITCH_PRINT(device, ERROR, "%s: -- _GFW_GLOBAL, _BOOT_PARTITION_PROGRESS (0x%x) != _SUCCESS --\n",
            __FUNCTION__, val);

        for (i = 0; i <= 15; i++)
        {
            val = NVSWITCH_OFF_RD32(device,
                NV_PTOP_UNICAST_SW_DEVICE_BASE_SAW_0 + NV_NVLSAW_SW_SCRATCH(i));
            report_saw.data[i] = val;
            NVSWITCH_PRINT(device, ERROR, "%s: -- NV_NVLSAW_SW_SCRATCH(%d) = 0x%08x\n",
                __FUNCTION__, i, val);
        }

        for (i = 0; i < NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2__SIZE_1; i++)
        {
            val = NVSWITCH_REG_RD32(device, _PFSP, _FALCON_COMMON_SCRATCH_GROUP_2(i));
            report.data[report_idx++] = val;
                NVSWITCH_PRINT(device, ERROR, "%s: -- NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(%d) = 0x%08x\n",
                __FUNCTION__, i, val);
        }

        // Include useful scratch information for triage
        NVSWITCH_PRINT_SXID_NO_BBX(device, sxid,
            "Fatal, %s (0x%x/0x%x, 0x%x, 0x%x, 0x%x/0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", sxid_desc,
            report.data[0], report.data[1], report.data[2], report.data[3], report.data[4],
            report_saw.data[0], report_saw.data[1], report_saw.data[12], report_saw.data[14], report_saw.data[15]);

        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return NVL_SUCCESS;
}

/*
 * @brief: This function returns the current value of the SOE heartbeat gpio
 * @params[in]   device       reference to current nvswitch device
 * @params[in]   p            NVSWITCH_GET_SOE_HEARTBEAT_PARAMS
 */
NvlStatus
nvswitch_ctrl_get_soe_heartbeat_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_SOE_HEARTBEAT_PARAMS *p
)
{
    NvU32 gpioVal = 0;
    NvU64 hi = 0;
    NvU64 lo = 0;
    NvU64 test = 0;

    if (!nvswitch_is_cci_supported(device))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Read status of heartbeat gpio
    gpioVal = NVSWITCH_REG_RD32(device, _GPIO, _OUTPUT_CNTL(3));

    // Record timestamp of gpio read from PTIMER
    do
    {
        hi = NVSWITCH_ENG_RD32(device, PTIMER, , 0, _PTIMER, _TIME_1);
        lo = NVSWITCH_ENG_RD32(device, PTIMER, , 0, _PTIMER, _TIME_0);
        test = NVSWITCH_ENG_RD32(device, PTIMER, , 0, _PTIMER, _TIME_1);
    }
    while (hi != test);
    p->timestampNs = (hi << 32) | lo;

    if (FLD_TEST_DRF(_GPIO, _OUTPUT_CNTL, _IO_OUTPUT, _1, gpioVal))
    {
        p->gpioVal = 1;
    }
    else if (FLD_TEST_DRF(_GPIO, _OUTPUT_CNTL, _IO_OUTPUT, _0, gpioVal))
    {
        p->gpioVal = 0;
    }

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_cci_reset_and_drain_links_ls10
(
    nvswitch_device *device,
    NvU64 link_mask,
    NvBool bForced
)
{
    NvU32 link;

    FOR_EACH_INDEX_IN_MASK(64, link, link_mask)
    {
        if (!cciIsLinkManaged(device, link))
        {
            link_mask = link_mask & ~NVBIT64(link);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return _nvswitch_reset_and_drain_links_ls10(device, link_mask, bForced);
}

/*
 * @brief Set the next LED state
 *        The HW will reflect this state on the next iteration of link 
 *        state update.   
 */
static void
_nvswitch_set_next_led_state_ls10
(
    nvswitch_device *device,
    NvU8             nextLedState
)
{
    device->next_led_state = nextLedState;
}

/*
 *  Returns the CPLD register value assigned to a particular LED state
 *  confluence page ID: 1011518154
 */
static NvU8
_nvswitch_get_led_state_regval_ls10
(
    nvswitch_device *device,
    NvU8 ledState
)
{
    switch (ledState)
    {
        case ACCESS_LINK_LED_STATE_OFF:
        {
            return CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_OFF;
        }
        case ACCESS_LINK_LED_STATE_UP_WARM:
        {
            return CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_GREEN;
        } 
        case ACCESS_LINK_LED_STATE_INITIALIZE:
        {
            return CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_3HZ_AMBER;
        } 
        case ACCESS_LINK_LED_STATE_UP_ACTIVE:
        {
            return CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_3HZ_GREEN;
        }
        case ACCESS_LINK_LED_STATE_FAULT:
        {
            return CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_6HZ_AMBER;
        }
        default:
        {
            NVSWITCH_ASSERT(0);
            return CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_OFF;
        }
    }
}

/*
 * @brief Set HW LED state using CPLD write
 *
 */
static NvlStatus
_nvswitch_set_led_state_ls10
(
    nvswitch_device *device
)
{
    NvlStatus retval;
    NvU8 ledState;
    NvU8 nextLedState;
    NvU8 regVal = 0;

    nextLedState = device->next_led_state;
    ledState = REF_VAL(ACCESS_LINK_LED_STATE, nextLedState);

    regVal = FLD_SET_REF_NUM(CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED,
                             _nvswitch_get_led_state_regval_ls10(device, ledState),
                             regVal);

    // Set state for LED
    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_ACCESS_LINK_LED_CTL, regVal);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // save HW state
    device->current_led_state = REF_NUM(ACCESS_LINK_LED_STATE, ledState);

    return NVL_SUCCESS;
}

static NvBool
_nvswitch_check_for_link_traffic
(
    nvswitch_device *device,
    NvU64 linkMask
)
{
    NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *pCounterParams = NULL;
    NvU64 *pCounterValues;
    NvU64 tpCounterPreviousSum;
    NvU64 tpCounterCurrentSum;
    NvBool bTraffic = NV_FALSE;
    NvU8 linkNum;

    pCounterParams = nvswitch_os_malloc(sizeof(*pCounterParams));
    if (pCounterParams == NULL)
        goto out;

    pCounterParams->counterMask = NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_TX |
                                  NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_RX;
    pCounterParams->linkMask = linkMask;
    if (nvswitch_ctrl_get_throughput_counters(device,
        pCounterParams) != NVL_SUCCESS)
    {
        goto out;
    }

    // Sum TX/RX traffic for each link
    FOR_EACH_INDEX_IN_MASK(64, linkNum, linkMask)
    {
        pCounterValues = pCounterParams->counters[linkNum].values;

        tpCounterPreviousSum = device->tp_counter_previous_sum[linkNum];

        // Sum taken to save space as it is unlikely to overflow before system is reset
        tpCounterCurrentSum = pCounterValues[NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_TX] +
                              pCounterValues[NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_RX];

        device->tp_counter_previous_sum[linkNum] = tpCounterCurrentSum;

        if (tpCounterCurrentSum > tpCounterPreviousSum)
        {
            bTraffic = NV_TRUE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

out:
    nvswitch_os_free(pCounterParams);
    return bTraffic;
}

static NvU8
_nvswitch_resolve_led_state_ls10
(
    nvswitch_device *device,
    NvU8  ledState0,
    NvU8  ledState1
)
{
    // Used to resolve link state discrepancies between partner links
    ct_assert(ACCESS_LINK_LED_STATE_FAULT < ACCESS_LINK_LED_STATE_OFF);
    ct_assert(ACCESS_LINK_LED_STATE_OFF < ACCESS_LINK_LED_STATE_INITIALIZE);
    ct_assert(ACCESS_LINK_LED_STATE_INITIALIZE < ACCESS_LINK_LED_STATE_UP_WARM);

    return (ledState0 < ledState1 ? ledState0 : ledState1);
}

static NvU8
_nvswitch_get_next_led_state_link_ls10
(
    nvswitch_device *device,
    NvU8            currentLedState,   
    NvU8            linkNum
)
{
    nvlink_link *link;
    NvU64 linkState;

    link = nvswitch_get_link(device, linkNum);

    if ((link == NULL) ||
        (device->hal.nvswitch_corelib_get_dl_link_mode(link, &linkState) != NVL_SUCCESS))
    {
        return ACCESS_LINK_LED_STATE_OFF;
    }

    switch (linkState)
    {
        case NVLINK_LINKSTATE_OFF:
        {
            return ACCESS_LINK_LED_STATE_OFF;
        }
        case NVLINK_LINKSTATE_HS:
        case NVLINK_LINKSTATE_RECOVERY:
        case NVLINK_LINKSTATE_SLEEP:
        {
            return ACCESS_LINK_LED_STATE_UP_WARM;
        }
        case NVLINK_LINKSTATE_FAULT:
        {
            return ACCESS_LINK_LED_STATE_FAULT;
        }
        default:
        {
            if (currentLedState == ACCESS_LINK_LED_STATE_INITIALIZE)
            {
                return ACCESS_LINK_LED_STATE_INITIALIZE;
            }
            return ACCESS_LINK_LED_STATE_OFF;
        }
    }
}

static NvU8
_nvswitch_get_next_led_state_links_ls10
(
    nvswitch_device *device,
    NvU8             currentLedState,   
    NvU64            linkMask
)
{
    NvU8  linkNum;
    NvU8  ledState;
    NvU8  nextLedState;

    nextLedState = ACCESS_LINK_NUM_LED_STATES;

    NVSWITCH_ASSERT(linkMask != 0);
    FOR_EACH_INDEX_IN_MASK(64, linkNum, linkMask)
    {
        ledState = _nvswitch_get_next_led_state_link_ls10(device, currentLedState, linkNum);
        nextLedState = _nvswitch_resolve_led_state_ls10(device, nextLedState, ledState);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (nextLedState == ACCESS_LINK_LED_STATE_UP_WARM)
    {
        // Only tells us that one of the links has activity
        if (_nvswitch_check_for_link_traffic(device, linkMask))
        {
            nextLedState = ACCESS_LINK_LED_STATE_UP_ACTIVE;
        }
    }

    return nextLedState;
}

static NvU8
_nvswitch_get_next_led_state_ls10
(
    nvswitch_device *device
)
{
    NvU8  linkNum;
    NvU8  ledNextState = 0;
    NvU8  currentLedState;
    NvU64 enabledLinkMask;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);

    FOR_EACH_INDEX_IN_MASK(64, linkNum, enabledLinkMask)
    {
        if (cciIsLinkManaged(device, linkNum))
        {
            enabledLinkMask = enabledLinkMask & ~NVBIT64(linkNum);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    currentLedState = device->current_led_state;
    currentLedState = REF_VAL(ACCESS_LINK_LED_STATE, currentLedState);

    ledNextState = FLD_SET_REF_NUM(ACCESS_LINK_LED_STATE,
                                   _nvswitch_get_next_led_state_links_ls10(device,
                                                                         currentLedState,
                                                                         enabledLinkMask),
                                    ledNextState);

    return ledNextState;
}

void
nvswitch_update_link_state_led_ls10
(
    nvswitch_device *device
)
{
    NvU8  currentLedState;
    NvU8  nextLedState;

    currentLedState = device->current_led_state;

    currentLedState = REF_VAL(ACCESS_LINK_LED_STATE, currentLedState);
    nextLedState = _nvswitch_get_next_led_state_ls10(device);

    // This is the next state that the LED will be set to
    _nvswitch_set_next_led_state_ls10(device, nextLedState);

    // Only update HW if required
    if (currentLedState != nextLedState)
    {
        _nvswitch_set_led_state_ls10(device);
    }
}

void
nvswitch_led_shutdown_ls10
(
    nvswitch_device *device
)
{
    NvU8 ledState = 0;
    ledState = FLD_SET_REF_NUM(ACCESS_LINK_LED_STATE,
                                ACCESS_LINK_LED_STATE_OFF, ledState);

    // This is the next state that the LED will be set to
    _nvswitch_set_next_led_state_ls10(device, ledState);
    _nvswitch_set_led_state_ls10(device);
}

NvlStatus
nvswitch_read_vbios_link_entries_ls10
(
    nvswitch_device *device,
    NvU32            tblPtr,
    NvU32            expected_link_entriesCount,
    NVLINK_CONFIG_DATA_LINKENTRY  *link_entries,
    NvU32            *identified_link_entriesCount
)
{
    NV_STATUS status = NV_ERR_INVALID_PARAMETER;
    NvU32 i;
    NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_30 vbios_link_entry;
    *identified_link_entriesCount = 0;

    for (i = 0; i < expected_link_entriesCount; i++)
    {
        if (!device->bIsNvlinkVbiosTableVersion2)
        {
            status = device->hal.nvswitch_vbios_read_structure(device,
                                                &vbios_link_entry,
                                                tblPtr, (NvU32 *)0,
                                                NVLINK_CONFIG_DATA_LINKENTRY_FMT_30);
        }
        else
        {
            status = device->hal.nvswitch_vbios_read_structure(device,
                                                &vbios_link_entry,
                                                tblPtr, (NvU32 *)0,
                                                NVLINK_CONFIG_DATA_LINKENTRY_FMT_20);
        } 
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Error on reading nvlink entry\n",
                __FUNCTION__);
            return status;
        }
        link_entries[i].nvLinkparam0 = (NvU8)vbios_link_entry.nvLinkparam0;
        link_entries[i].nvLinkparam1 = (NvU8)vbios_link_entry.nvLinkparam1;
        link_entries[i].nvLinkparam2 = (NvU8)vbios_link_entry.nvLinkparam2;
        link_entries[i].nvLinkparam3 = (NvU8)vbios_link_entry.nvLinkparam3;
        link_entries[i].nvLinkparam4 = (NvU8)vbios_link_entry.nvLinkparam4;
        link_entries[i].nvLinkparam5 = (NvU8)vbios_link_entry.nvLinkparam5;
        link_entries[i].nvLinkparam6 = (NvU8)vbios_link_entry.nvLinkparam6;
        if (!device->bIsNvlinkVbiosTableVersion2)
        {
            link_entries[i].nvLinkparam7 = (NvU8)vbios_link_entry.nvLinkparam7;
            link_entries[i].nvLinkparam8 = (NvU8)vbios_link_entry.nvLinkparam8;
            link_entries[i].nvLinkparam9 = (NvU8)vbios_link_entry.nvLinkparam9;
        }
        if (!device->bIsNvlinkVbiosTableVersion2)
            tblPtr += (sizeof(NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_30)/sizeof(NvU32));
        else
            tblPtr += (sizeof(NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_20)/sizeof(NvU32));


        NVSWITCH_PRINT(device, NOISY,
            "<<<---- NvLink ID 0x%x ---->>>\n", i);
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 0 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam0, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam0));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 1 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam1, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam1));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 2 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam2, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam2));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 3 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam3, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam3));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 4 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam4, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam4));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 5 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam5, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam5));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 6 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam6, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam6));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 7 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam7, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam7));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 8 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam8, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam8));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 9 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam9, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam9));
        NVSWITCH_PRINT(device, NOISY,
            "<<<---- NvLink ID 0x%x ---->>>\n\n", i);
    }
    *identified_link_entriesCount = i;
    return status;
}

NvlStatus
nvswitch_ctrl_get_bios_info_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_BIOS_INFO_PARAMS *p
)
{
    NvU32 biosVersionBytes;
    NvU32 biosOemVersionBytes;
    NvU32 biosMagic = 0x9610;

    //
    // Example: 96.10.09.00.00 is the formatted version string
    //          |         |  |
    //          |         |  |__ BIOS OEM version byte
    //          |         |
    //          |_________|_____ BIOS version bytes
    //
    biosVersionBytes = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW_SW, _BIOS_VERSION);
    biosOemVersionBytes = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW_SW, _OEM_BIOS_VERSION);

    //
    // LS10 is built out of core96 and the BIOS version will always begin with
    // 96.10.xx.xx.xx
    //
    if ((biosVersionBytes >> 16) != biosMagic)
    {
        NVSWITCH_PRINT(device, ERROR,
                "BIOS version not found in scratch register\n");
        return -NVL_ERR_INVALID_STATE;
    }

    p->version = (((NvU64)biosVersionBytes) << 8) | (biosOemVersionBytes & 0xff);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_inforom_version_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_INFOROM_VERSION_PARAMS *p
)
{
    struct inforom *pInforom = device->pInforom;

    if ((pInforom == NULL) || (!pInforom->IMG.bValid))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (NV_ARRAY_ELEMENTS(pInforom->IMG.object.version) <
        NVSWITCH_INFOROM_VERSION_LEN)
    {
        NVSWITCH_PRINT(device, ERROR,
                       "Inforom IMG object struct smaller than expected\n");
        return -NVL_ERR_INVALID_STATE;
    }

    nvswitch_inforom_string_copy(pInforom->IMG.object.version, p->version,
                                 NVSWITCH_INFOROM_VERSION_LEN);

    return NVL_SUCCESS;
}

/*
 * @Brief : Initializes an NvSwitch hardware state
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 *
 * @returns                 NVL_SUCCESS if the action succeeded
 *                          -NVL_BAD_ARGS if bad arguments provided
 *                          -NVL_PCI_ERROR if bar info unable to be retrieved
 */
NvlStatus
nvswitch_initialize_device_state_ls10
(
    nvswitch_device *device
)
{
    device->bModeContinuousALI = NV_TRUE;
    return nvswitch_initialize_device_state_lr10(device);
}

//
// This function auto creates the ls10 HAL connectivity from the NVSWITCH_INIT_HAL
// macro in haldef_nvswitch.h
//
// Note: All hal fns must be implemented for each chip.
//       There is no automatic stubbing here.
//
void nvswitch_setup_hal_ls10(nvswitch_device *device)
{
    device->chip_arch = NVSWITCH_GET_INFO_INDEX_ARCH_LS10;
    device->chip_impl = NVSWITCH_GET_INFO_INDEX_IMPL_LS10;

    NVSWITCH_INIT_HAL(device, ls10);
    NVSWITCH_INIT_HAL_LS10(device, ls10);                             
}

