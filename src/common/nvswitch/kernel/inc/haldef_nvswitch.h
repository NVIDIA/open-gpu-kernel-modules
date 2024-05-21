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

#ifndef _HALDEF_NVSWITCH_H_
#define _HALDEF_NVSWITCH_H_

#include "ctrl_dev_nvswitch.h"

#include "inforom/ifrstruct.h"
#include "inforom/omsdef.h"

//
// List of functions halified in the NVSwitch Driver
//
// Note: All hal fns must be implemented for each chip.
//       There is no automatic stubbing here.
//
// This 'xmacro' list is fed into generator macros which then use the
// _FUNCTION_LIST to generate HAL declarations, function prototypes, and HAL
// construction.  Xmacros are a useful way to maintain consistency between
// parallel lists.
// The components of the _FUNCTION_LIST are similar to a function prototype
// declaration, with the addition of an '_arch' parameter suffixed on to it
// which is used on some _FUNCTION_LIST expansions to generate arch-specific
// information.
// The format of each line is:
//     _op(return type, function name, (parameter list), _arch)
//

#define NVSWITCH_HAL_FUNCTION_LIST_FEATURE_0(_op, _arch) \
    _op(NvBool,    nvswitch_is_cci_supported,               (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_cci_setup_gpio_pins,            (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_cci_get_cci_link_mode,          (nvswitch_device *device, NvU32 linkNumber, NvU64 *mode), _arch)  \
    _op(NvlStatus, nvswitch_cci_discovery,                  (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_cci_get_xcvrs_present,          (nvswitch_device *device, NvU32 *pMaskPresent), _arch)  \
    _op(void,      nvswitch_cci_get_xcvrs_present_change,   (nvswitch_device *device, NvU32 *pMaskPresentChange), _arch)  \
    _op(NvlStatus, nvswitch_cci_reset,                      (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_cci_reset_links,                (nvswitch_device *device, NvU64 linkMask), _arch)  \
    _op(NvlStatus, nvswitch_cci_ports_cpld_read,            (nvswitch_device *device, NvU8 reg, NvU8 *pData), _arch)  \
    _op(NvlStatus, nvswitch_cci_ports_cpld_write,           (nvswitch_device *device, NvU8 reg, NvU8 Data), _arch)  \
    _op(void,      nvswitch_cci_update_link_state_led,      (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_cci_set_xcvr_led_state,         (nvswitch_device *device, NvU32 client, NvU32 osfp, NvBool bSetLocate), _arch) \
    _op(NvlStatus, nvswitch_cci_get_xcvr_led_state,         (nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 *pLedState), _arch) \
    _op(NvlStatus, nvswitch_cci_setup_onboard,              (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_cci_setup_module_path,          (nvswitch_device *device, NvU32 client, NvU32 osfp), _arch)  \
    _op(NvlStatus, nvswitch_cci_module_access_cmd,          (nvswitch_device *device, NvU32 client, NvU32 osfp, NvU32 addr, NvU32 length, \
                                                                                  NvU8 *pValArray, NvBool bRead, NvBool bBlk), _arch)  \
    _op(NvlStatus, nvswitch_cci_apply_control_set_values,   (nvswitch_device *device, NvU32 client, NvU32 moduleMask), _arch)  \
    _op(NvlStatus, nvswitch_cci_reset_and_drain_links,      (nvswitch_device *device, NvU64 link_mask, NvBool bForced), _arch)  \
    _op(NvlStatus, nvswitch_cci_cmis_cage_bezel_marking,    (nvswitch_device *device, NvU8 cageIndex, char *pBezelMarking), _arch)  \
    _op(NvlStatus, nvswitch_cci_get_grading_values,         (nvswitch_device *device, NvU32 client, NvU32 linkId, NvU8 *laneMask, NVSWITCH_CCI_GRADING_VALUES *pGrading), _arch) \
    _op(NvlStatus, nvswitch_cci_get_xcvr_mask,              (nvswitch_device *device, NvU32 *pMaskAll, NvU32 *pMaskPresent), _arch) \
    _op(void,      nvswitch_cci_set_xcvr_present,           (nvswitch_device *device, NvU32 maskPresent), _arch) \
    _op(void,      nvswitch_cci_destroy,                    (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_soe_heartbeat,         (nvswitch_device *device, NVSWITCH_GET_SOE_HEARTBEAT_PARAMS *p), _arch) \
    _op(void,      nvswitch_update_link_state_led,          (nvswitch_device *device), _arch) \
    _op(void,      nvswitch_led_shutdown,                   (nvswitch_device *device), _arch) \


#define NVSWITCH_HAL_FUNCTION_LIST(_op, _arch)                                          \
    _op(NvlStatus, nvswitch_initialize_device_state, (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_destroy_device_state,    (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_determine_platform,      (nvswitch_device *device), _arch)  \
    _op(NvU32,     nvswitch_get_num_links,           (nvswitch_device *device), _arch)  \
    _op(NvU8,      nvswitch_get_num_links_per_nvlipt,(nvswitch_device *device), _arch)  \
    _op(NvBool,    nvswitch_is_link_valid,           (nvswitch_device *device, NvU32 link_id), _arch)  \
    _op(void,      nvswitch_set_fatal_error,         (nvswitch_device *device, NvBool device_fatal, NvU32 link_id), _arch)  \
    _op(NvU32,     nvswitch_get_swap_clk_default,    (nvswitch_device *device), _arch)  \
    _op(NvU32,     nvswitch_get_latency_sample_interval_msec, (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_internal_latency_bin_log,(nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_ecc_writeback_task,      (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_monitor_thermal_alert,   (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_hw_counter_shutdown,     (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_get_rom_info,            (nvswitch_device *device, NVSWITCH_EEPROM_TYPE *eeprom), _arch)  \
    _op(void,      nvswitch_lib_enable_interrupts,   (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_lib_disable_interrupts,  (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_lib_check_interrupts,    (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_lib_service_interrupts,  (nvswitch_device *device), _arch)  \
    _op(NvU64,     nvswitch_hw_counter_read_counter, (nvswitch_device *device), _arch)  \
    _op(NvBool,    nvswitch_is_link_in_use,          (nvswitch_device *device, NvU32 link_id), _arch)  \
    _op(NvlStatus, nvswitch_reset_and_drain_links,   (nvswitch_device *device, NvU64 link_mask, NvBool bForced), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_info,           (nvswitch_device *device, NVSWITCH_GET_INFO *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_nvlink_status,  (nvswitch_device *device, NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_counters,       (nvswitch_device *device, NVSWITCH_NVLINK_GET_COUNTERS_PARAMS *ret), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_switch_port_config,    (nvswitch_device *device, NVSWITCH_SET_SWITCH_PORT_CONFIG *p), _arch)  \
    _op(NvlStatus, nvswitch_set_nport_port_config,   (nvswitch_device *device, NVSWITCH_SET_SWITCH_PORT_CONFIG *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_ingress_request_table, (nvswitch_device *device, NVSWITCH_GET_INGRESS_REQUEST_TABLE_PARAMS *params), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_ingress_request_table, (nvswitch_device *device, NVSWITCH_SET_INGRESS_REQUEST_TABLE *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_ingress_request_valid, (nvswitch_device *device, NVSWITCH_SET_INGRESS_REQUEST_VALID *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_ingress_response_table, (nvswitch_device *device, NVSWITCH_GET_INGRESS_RESPONSE_TABLE_PARAMS *params), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_ingress_response_table, (nvswitch_device *device, NVSWITCH_SET_INGRESS_RESPONSE_TABLE *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_ganged_link_table,      (nvswitch_device *device, NVSWITCH_SET_GANGED_LINK_TABLE *p), _arch)  \
    _op(void,      nvswitch_init_npg_multicast,     (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_init_warm_reset,        (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_remap_policy,  (nvswitch_device *device, NVSWITCH_SET_REMAP_POLICY *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_remap_policy,  (nvswitch_device *device, NVSWITCH_GET_REMAP_POLICY_PARAMS *params), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_remap_policy_valid, (nvswitch_device *device, NVSWITCH_SET_REMAP_POLICY_VALID *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_routing_id,    (nvswitch_device *device, NVSWITCH_SET_ROUTING_ID *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_routing_id,    (nvswitch_device *device, NVSWITCH_GET_ROUTING_ID_PARAMS *params), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_routing_id_valid, (nvswitch_device *device, NVSWITCH_SET_ROUTING_ID_VALID *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_routing_lan,   (nvswitch_device *device, NVSWITCH_SET_ROUTING_LAN *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_routing_lan,   (nvswitch_device *device, NVSWITCH_GET_ROUTING_LAN_PARAMS *params), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_routing_lan_valid, (nvswitch_device *device, NVSWITCH_SET_ROUTING_LAN_VALID *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_internal_latency, (nvswitch_device *device, NVSWITCH_GET_INTERNAL_LATENCY *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_latency_bins,  (nvswitch_device *device, NVSWITCH_SET_LATENCY_BINS *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_ingress_reqlinkid, (nvswitch_device *device, NVSWITCH_GET_INGRESS_REQLINKID_PARAMS *params), _arch)  \
    _op(NvU32,     nvswitch_i2c_get_port_info,      (nvswitch_device *device, NvU32 port), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_register_read,     (nvswitch_device *device, NVSWITCH_REGISTER_READ *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_register_write,    (nvswitch_device *device, NVSWITCH_REGISTER_WRITE *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_i2c_indexed,       (nvswitch_device *device, NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_therm_read_temperature, (nvswitch_device *device, NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS *info), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_therm_get_temperature_limit, (nvswitch_device *device, NVSWITCH_CTRL_GET_TEMPERATURE_LIMIT_PARAMS *info), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_throughput_counters, (nvswitch_device *device, NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_corelib_add_link,       (nvlink_link *link), _arch)  \
    _op(NvlStatus, nvswitch_corelib_remove_link,    (nvlink_link *link), _arch)  \
    _op(NvlStatus, nvswitch_corelib_set_dl_link_mode, (nvlink_link *link, NvU64 mode, NvU32 flags), _arch)  \
    _op(NvlStatus, nvswitch_corelib_get_dl_link_mode, (nvlink_link *link, NvU64 *mode), _arch)  \
    _op(NvlStatus, nvswitch_corelib_set_tl_link_mode, (nvlink_link *link, NvU64 mode, NvU32 flags), _arch)  \
    _op(NvlStatus, nvswitch_corelib_get_tl_link_mode, (nvlink_link *link, NvU64 *mode), _arch)  \
    _op(NvlStatus, nvswitch_corelib_set_tx_mode,    (nvlink_link *link, NvU64 mode, NvU32 flags), _arch)  \
    _op(NvlStatus, nvswitch_corelib_get_tx_mode,    (nvlink_link *link, NvU64 *mode, NvU32 *subMode), _arch)  \
    _op(NvlStatus, nvswitch_corelib_set_rx_mode,    (nvlink_link *link, NvU64 mode, NvU32 flags), _arch)  \
    _op(NvlStatus, nvswitch_corelib_get_rx_mode,    (nvlink_link *link, NvU64 *mode, NvU32 *subMode), _arch)  \
    _op(NvlStatus, nvswitch_corelib_set_rx_detect,  (nvlink_link *link, NvU32 flags), _arch)  \
    _op(NvlStatus, nvswitch_corelib_get_rx_detect,  (nvlink_link *link), _arch)  \
    _op(void,      nvswitch_corelib_training_complete, (nvlink_link *link), _arch)  \
    _op(NvU32,     nvswitch_get_device_dma_width,   (nvswitch_device *device), _arch)  \
    _op(NvU32,     nvswitch_get_link_ip_version,    (nvswitch_device *device, NvU32 link_id), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_fom_values,    (nvswitch_device *device, NVSWITCH_GET_FOM_VALUES_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_deassert_link_reset,    (nvswitch_device *device, nvlink_link *link), _arch)  \
    _op(NvBool,    nvswitch_is_soe_supported,       (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_init_soe, (nvswitch_device *device), _arch)  \
    _op(NvBool,    nvswitch_is_inforom_supported,   (nvswitch_device *device), _arch)  \
    _op(NvBool,    nvswitch_is_spi_supported,       (nvswitch_device *device), _arch)  \
    _op(NvBool,    nvswitch_is_bios_supported,      (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_get_bios_size,         (nvswitch_device *device, NvU32 *pSize),  _arch)  \
    _op(NvBool,    nvswitch_is_smbpbi_supported,   (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_post_init_device_setup, (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_post_init_blacklist_device_setup, (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_setup_system_registers, (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_setup_link_system_registers, (nvswitch_device *device, nvlink_link *link), _arch)  \
    _op(void,      nvswitch_load_link_disable_settings, (nvswitch_device *device, nvlink_link *link), _arch)  \
    _op(NvlStatus, nvswitch_read_vbios_link_entries, (nvswitch_device *device, NvU32 tblPtr,NvU32 expected_link_entriesCount,NVLINK_CONFIG_DATA_LINKENTRY *link_entries, NvU32 *identified_link_entriesCount), _arch)  \
    _op(NvlStatus, nvswitch_vbios_read_structure, (nvswitch_device *device, void *structure, NvU32 offset, NvU32 *ppacked_size, const char *format), _arch)  \
    _op(NvlStatus, nvswitch_get_nvlink_ecc_errors,  (nvswitch_device *device, NVSWITCH_GET_NVLINK_ECC_ERRORS_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_inforom_ecc_log_error_event, (nvswitch_device *device, INFOROM_ECC_OBJECT *pEccGeneric, INFOROM_NVS_ECC_ERROR_EVENT *error_event), _arch)  \
    _op(void,      nvswitch_oms_set_device_disable, (INFOROM_OMS_STATE *pOmsState, NvBool bForceDeviceDisable), _arch)  \
    _op(NvBool,    nvswitch_oms_get_device_disable, (INFOROM_OMS_STATE *pOmsState), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_log_error_event, (nvswitch_device *device, void *pNvlGeneric, void *error_event, NvBool *bDirty), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_update_link_correctable_error_info, (nvswitch_device *device, void *pNvlGeneric, void *pData, NvU8 linkId, NvU8 nvliptInstance, NvU8 localLinkIdx, void *pErrorCounts, NvBool *bDirty), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_get_max_correctable_error_rate,  (nvswitch_device *device, NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_get_errors,  (nvswitch_device *device, NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_setL1Threshold, (nvswitch_device *device, void *pNvlGeneric, NvU32 word1, NvU32 word2), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_getL1Threshold, (nvswitch_device *device, void *pNvlGeneric, NvU32 *word1, NvU32 *word2), _arch)  \
    _op(NvlStatus, nvswitch_inforom_nvl_setup_nvlink_state, (nvswitch_device *device, INFOROM_NVLINK_STATE *pNvlinkState, NvU8 version), _arch)  \
    _op(NvlStatus, nvswitch_inforom_ecc_get_errors,  (nvswitch_device *device, NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *p), _arch)  \
    _op(void,      nvswitch_load_uuid,              (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_i2c_set_hw_speed_mode,  (nvswitch_device *device, NvU32 port, NvU32 speedMode), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_bios_info,     (nvswitch_device *device, NVSWITCH_GET_BIOS_INFO_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_inforom_version,  (nvswitch_device *device, NVSWITCH_GET_INFOROM_VERSION_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_read_oob_blacklist_state, (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_write_fabric_state,     (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_initialize_oms_state,   (nvswitch_device *device, INFOROM_OMS_STATE *pOmsState), _arch)  \
    _op(NvlStatus, nvswitch_oms_inforom_flush,      (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_inforom_ecc_get_total_errors,   (nvswitch_device *device, INFOROM_ECC_OBJECT *pEccGeneric, NvU64 *corCount, NvU64 *uncCount), _arch)  \
    _op(NvlStatus, nvswitch_inforom_load_obd, (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_bbx_add_sxid, (nvswitch_device *device, NvU32 exceptionType, NvU32 data0, NvU32 data1, NvU32 data2), _arch)  \
    _op(NvlStatus, nvswitch_bbx_unload, (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_bbx_load, (nvswitch_device *device, NvU64 time_ns, NvU8 osType, NvU32 osVersion), _arch)  \
    _op(NvlStatus, nvswitch_bbx_get_sxid, (nvswitch_device *device, NVSWITCH_GET_SXIDS_PARAMS * params), _arch)  \
    _op(NvlStatus, nvswitch_bbx_get_data, (nvswitch_device *device, NvU8 dataType, void * params), _arch)  \
    _op(NvlStatus, nvswitch_smbpbi_alloc,           (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_smbpbi_post_init_hal,   (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_smbpbi_destroy_hal,     (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_smbpbi_send_unload,     (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_smbpbi_dem_load,        (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_smbpbi_dem_flush,       (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_smbpbi_get_dem_num_messages,    (nvswitch_device *device, NvU8 *pMsgCount), _arch)  \
    _op(void,      nvswitch_smbpbi_log_message,     (nvswitch_device *device, NvU32 num, NvU32 msglen, NvU8 *osErrorString), _arch)  \
    _op(NvlStatus, nvswitch_smbpbi_send_init_data,  (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_set_minion_initialized, (nvswitch_device *device, NvU32 idx_minion, NvBool initialized), _arch)  \
    _op(NvBool,    nvswitch_is_minion_initialized,  (nvswitch_device *device, NvU32 idx_minion), _arch)  \
    _op(NvlStatus, nvswitch_get_link_public_id, (nvswitch_device *device, NvU32 linkId, NvU32 *publicId), _arch)  \
    _op(NvlStatus, nvswitch_get_link_local_idx, (nvswitch_device *device, NvU32 linkId, NvU32 *localLinkIdx), _arch)  \
    _op(NvlStatus, nvswitch_set_training_error_info, (nvswitch_device *device, NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS *pLinkTrainingErrorInfoParams), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_fatal_error_scope, (nvswitch_device *device, NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS *pParams), _arch)  \
    _op(void,      nvswitch_init_scratch,       (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_device_discovery,   (nvswitch_device *device, NvU32 discovery_offset), _arch)  \
    _op(void,      nvswitch_filter_discovery,   (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_process_discovery,  (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_init_minion,        (nvswitch_device *device), _arch)  \
    _op(NvU32,     nvswitch_get_eng_base,   (nvswitch_device *device, NVSWITCH_ENGINE_ID eng_id, NvU32 eng_bcast, NvU32 eng_instance), _arch)  \
    _op(NvU32,     nvswitch_get_eng_count,  (nvswitch_device *device, NVSWITCH_ENGINE_ID eng_id, NvU32 eng_bcast), _arch)  \
    _op(NvU32,     nvswitch_eng_rd,         (nvswitch_device *device, NVSWITCH_ENGINE_ID eng_id, NvU32 eng_bcast, NvU32 eng_instance, NvU32 offset), _arch)  \
    _op(void,      nvswitch_eng_wr,         (nvswitch_device *device, NVSWITCH_ENGINE_ID eng_id, NvU32 eng_bcast, NvU32 eng_instance, NvU32 offset, NvU32 data), _arch)  \
    _op(NvU32,     nvswitch_get_link_eng_inst,  (nvswitch_device *device, NvU32 link_id, NVSWITCH_ENGINE_ID eng_id), _arch)  \
    _op(void *,    nvswitch_alloc_chipdevice,   (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_init_thermal,       (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_init_pll_config,    (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_init_pll,           (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_init_clock_gating,  (nvswitch_device *device), _arch)  \
    _op(NvU32,     nvswitch_read_physical_id,   (nvswitch_device *device), _arch)  \
    _op(NvU32,     nvswitch_get_caps_nvlink_version,    (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_initialize_interrupt_tree,  (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_init_dlpl_interrupts,       (nvlink_link *link), _arch)  \
    _op(NvlStatus, nvswitch_initialize_pmgr,    (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_initialize_ip_wrappers,     (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_initialize_route,   (nvswitch_device *device), _arch)  \
    _op(void,      nvswitch_soe_unregister_events,      (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_soe_register_event_callbacks,  (nvswitch_device *device), _arch)  \
    _op(NVSWITCH_BIOS_NVLINK_CONFIG *, nvswitch_get_bios_nvlink_config, (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_minion_send_command, (nvswitch_device *device, NvU32 linkNumber, NvU32 command, NvU32 scratch0), _arch)  \
    _op(NvlStatus, nvswitch_init_nport,      (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_init_nxbar,      (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_clear_nport_rams,       (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_pri_ring_init,          (nvswitch_device *device), _arch)  \
    _op(NvlStatus, nvswitch_get_remap_table_selector,   (nvswitch_device *device, NVSWITCH_TABLE_SELECT_REMAP table_selector, NvU32 *remap_ram_sel), _arch)  \
    _op(NvU32,     nvswitch_get_ingress_ram_size,   (nvswitch_device *device, NvU32 ingress_ram_selector), _arch)  \
    _op(NvlStatus, nvswitch_minion_get_dl_status,   (nvswitch_device *device, NvU32 linkId, NvU32 statusIdx, NvU32 statusArgs, NvU32 *statusData), _arch)  \
    _op(void,      nvswitch_corelib_get_uphy_load, (nvlink_link *link, NvBool *bUnlocked), _arch) \
    _op(NvBool,    nvswitch_is_i2c_supported, (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_poll_sublink_state, (nvswitch_device *device, nvlink_link *link), _arch)\
    _op(void,      nvswitch_setup_link_loopback_mode, (nvswitch_device *device, NvU32 linkNumber), _arch)\
    _op(void,      nvswitch_reset_persistent_link_hw_state, (nvswitch_device *device, NvU32 linkNumber), _arch)\
    _op(void,      nvswitch_store_topology_information, (nvswitch_device *device, nvlink_link *link), _arch) \
    _op(void,      nvswitch_init_lpwr_regs, (nvlink_link *link), _arch) \
    _op(void,      nvswitch_program_l1_scratch_reg, (nvswitch_device *device, NvU32 linkNumber), _arch) \
    _op(NvlStatus, nvswitch_set_training_mode, (nvswitch_device *device), _arch) \
    _op(NvU32,     nvswitch_get_sublink_width, (nvswitch_device *device, NvU32 linkNumber), _arch) \
    _op(NvBool,    nvswitch_i2c_is_device_access_allowed, (nvswitch_device *device, NvU32 port, NvU8 addr, NvBool bIsRead), _arch) \
    _op(NvlStatus, nvswitch_parse_bios_image, (nvswitch_device *device), _arch) \
    _op(NvBool,    nvswitch_is_link_in_reset, (nvswitch_device *device, nvlink_link *link), _arch) \
    _op(void,      nvswitch_init_buffer_ready, (nvswitch_device *device, nvlink_link * link, NvBool bNportBufferReady), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_nvlink_lp_counters, (nvswitch_device *device, NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS *p), _arch) \
    _op(void,      nvswitch_apply_recal_settings, (nvswitch_device *device, nvlink_link *), _arch) \
    _op(NvlStatus, nvswitch_service_nvldl_fatal_link, (nvswitch_device *device, NvU32 nvliptInstance, NvU32 link), _arch) \
    _op(NvlStatus, nvswitch_service_minion_link, (nvswitch_device *device, NvU32 link_id), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_sw_info,  (nvswitch_device *device, NVSWITCH_GET_SW_INFO_PARAMS *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_err_info, (nvswitch_device *device, NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS *ret), _arch) \
    _op(NvlStatus, nvswitch_ctrl_clear_counters, (nvswitch_device *device, NVSWITCH_NVLINK_CLEAR_COUNTERS_PARAMS *ret), _arch) \
    _op(NvlStatus, nvswitch_ctrl_set_nvlink_error_threshold, (nvswitch_device *device, NVSWITCH_SET_NVLINK_ERROR_THRESHOLD_PARAMS *pParams), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_nvlink_error_threshold, (nvswitch_device *device, NVSWITCH_GET_NVLINK_ERROR_THRESHOLD_PARAMS *pParams), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_board_part_number, (nvswitch_device *device, NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_therm_read_voltage, (nvswitch_device *device, NVSWITCH_CTRL_GET_VOLTAGE_PARAMS *info), _arch) \
    _op(void,      nvswitch_soe_init_l2_state,       (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_ctrl_therm_read_power, (nvswitch_device *device, NVSWITCH_GET_POWER_PARAMS *info), _arch) \
    _op(NvlStatus, nvswitch_get_board_id, (nvswitch_device *device, NvU16 *boardId), _arch) \
    _op(NvBool,    nvswitch_does_link_need_termination_enabled, (nvswitch_device *device, nvlink_link *link), _arch) \
    _op(NvlStatus, nvswitch_link_termination_setup, (nvswitch_device *device, nvlink_link *link), _arch) \
    _op(NvlStatus, nvswitch_check_io_sanity, (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_link_l1_capability, (nvswitch_device *device, NvU32 linkNum, NvBool *isL1Capable), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_link_l1_threshold, (nvswitch_device *device, NvU32 linkNum, NvU32 *lpThreshold), _arch) \
    _op(NvlStatus, nvswitch_ctrl_set_link_l1_threshold, (nvlink_link *link, NvU32 lpEntryThreshold), _arch) \
    _op(void,      nvswitch_fsp_update_cmdq_head_tail, (nvswitch_device  *device, NvU32 queueHead, NvU32 queueTail), _arch) \
    _op(void,      nvswitch_fsp_get_cmdq_head_tail, (nvswitch_device  *device, NvU32 *pQueueHead, NvU32 *pQueueTail), _arch) \
    _op(void,      nvswitch_fsp_update_msgq_head_tail, (nvswitch_device *device, NvU32 msgqHead, NvU32 msgqTail), _arch) \
    _op(void,      nvswitch_fsp_get_msgq_head_tail, (nvswitch_device *device, NvU32 *pMsgqHead, NvU32 *pMsgqTail), _arch) \
    _op(NvU32,     nvswitch_fsp_get_channel_size, (nvswitch_device *device), _arch) \
    _op(NvU8,      nvswitch_fsp_nvdm_to_seid, (nvswitch_device *device, NvU8 nvdmType), _arch) \
    _op(NvU32,     nvswitch_fsp_create_mctp_header, (nvswitch_device *device, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq), _arch) \
    _op(NvU32,     nvswitch_fsp_create_nvdm_header, (nvswitch_device *device, NvU32 nvdmType), _arch) \
    _op(NvlStatus, nvswitch_fsp_validate_mctp_payload_header, (nvswitch_device *device, NvU8 *pBuffer, NvU32 size), _arch) \
    _op(NvlStatus, nvswitch_fsp_process_nvdm_msg, (nvswitch_device  *device, NvU8 *pBuffer, NvU32 size), _arch) \
    _op(NvlStatus, nvswitch_fsp_process_cmd_response, (nvswitch_device *device, NvU8 *pBuffer, NvU32 size), _arch) \
    _op(NvlStatus, nvswitch_fsp_config_ememc, (nvswitch_device *device, NvU32 offset, NvBool bAincw, NvBool bAincr), _arch) \
    _op(NvlStatus, nvswitch_fsp_write_to_emem, (nvswitch_device *device, NvU8 *pBuffer, NvU32 size), _arch) \
    _op(NvlStatus, nvswitch_fsp_read_from_emem, (nvswitch_device *device, NvU8 *pBuffer, NvU32 size), _arch) \
    _op(NvlStatus, nvswitch_fsp_error_code_to_nvlstatus_map, (nvswitch_device *device, NvU32 errorCode), _arch) \
    _op(NvlStatus, nvswitch_fsp_get_packet_info, (nvswitch_device *device, NvU8 *pBuffer, NvU32 size, NvU8 *pPacketState, NvU8 *pTag), _arch) \
    _op(NvlStatus, nvswitch_fsprpc_get_caps, (nvswitch_device *device, NVSWITCH_FSPRPC_GET_CAPS_PARAMS *params), _arch) \
    _op(NvlStatus, nvswitch_soe_issue_ingress_stop, (nvswitch_device *device, NvU32 nport, NvBool bStop), _arch) \
    _op(NvlStatus, nvswitch_detect_tnvl_mode, (nvswitch_device *device), _arch) \
    _op(NvBool,    nvswitch_is_tnvl_mode_enabled, (nvswitch_device *device), _arch) \
    _op(NvBool,    nvswitch_is_tnvl_mode_locked, (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_tnvl_get_attestation_certificate_chain, (nvswitch_device *device, NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS *params), _arch) \
    _op(NvlStatus, nvswitch_tnvl_get_attestation_report, (nvswitch_device *device, NVSWITCH_GET_ATTESTATION_REPORT_PARAMS *params), _arch) \
    _op(NvlStatus, nvswitch_tnvl_send_fsp_lock_config, (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_tnvl_get_status, (nvswitch_device *device, NVSWITCH_GET_TNVL_STATUS_PARAMS *params), _arch) \
    NVSWITCH_HAL_FUNCTION_LIST_FEATURE_0(_op, _arch) \

#define NVSWITCH_HAL_FUNCTION_LIST_LS10(_op, _arch) \
    _op(NvlStatus, nvswitch_launch_ALI, (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_launch_ALI_link_training, (nvswitch_device *device, nvlink_link *link, NvBool bSync), _arch) \
    _op(NvlStatus, nvswitch_ctrl_inband_send_data, (nvswitch_device *device, NVSWITCH_INBAND_SEND_DATA_PARAMS *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_inband_read_data, (nvswitch_device *device, NVSWITCH_INBAND_READ_DATA_PARAMS *p), _arch) \
    _op(void,      nvswitch_send_inband_nack, (nvswitch_device *device, NvU32 *msghdr, NvU32  linkId), _arch) \
    _op(NvU32,     nvswitch_get_max_persistent_message_count, (nvswitch_device *device), _arch) \
    _op(NvlStatus, nvswitch_ctrl_set_mc_rid_table,  (nvswitch_device *device, NVSWITCH_SET_MC_RID_TABLE_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_get_mc_rid_table,  (nvswitch_device *device, NVSWITCH_GET_MC_RID_TABLE_PARAMS *p), _arch)  \
    _op(NvlStatus, nvswitch_ctrl_set_residency_bins, (nvswitch_device *device, NVSWITCH_SET_RESIDENCY_BINS *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_residency_bins, (nvswitch_device *device, NVSWITCH_GET_RESIDENCY_BINS *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_rb_stall_busy, (nvswitch_device *device, NVSWITCH_GET_RB_STALL_BUSY *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_get_multicast_id_error_vector, (nvswitch_device *device, NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR *p), _arch) \
    _op(NvlStatus, nvswitch_ctrl_clear_multicast_id_error_vector, (nvswitch_device *device, NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR *p), _arch) \
    _op(NvlStatus, nvswitch_reset_and_train_link, (nvswitch_device *device, nvlink_link *link), _arch) \

//
// Declare HAL function pointer table
//
// This macro takes the xmacro _FUNCTION_LIST and uses some components in it to
// automatically generate the HAL structure declaration in a form:
//     NvU32    (*function_foo1)(nvswitch_device device);
//     void     (*function_foo2)(nvswitch_device device, NvU32 parameter1);
//     NvlStatus (*function_foo3)(nvswitch_device device, NvU32 parameter1, void *parameter2);
//

#define DECLARE_HAL_FUNCTIONS(_return, _function, _params, _arch)   \
    _return (*_function)_params;

typedef struct nvswitch_hal_functions
{
    NVSWITCH_HAL_FUNCTION_LIST(DECLARE_HAL_FUNCTIONS, HAL)
    NVSWITCH_HAL_FUNCTION_LIST_LS10(DECLARE_HAL_FUNCTIONS, HAL)

} nvswitch_hal;

//
// Fill in HAL function pointer table
//
// This macro takes the xmacro _FUNCTION_LIST and uses some components in it to
// automatically generate all the HAL function fill-in assignments for a given
// architecture.
//

#define CREATE_HAL_FUNCTIONS(_return, _function, _params, _arch)    \
    device->hal._function = _function##_##_arch;                    \

#define NVSWITCH_INIT_HAL(device, arch)                             \
    NVSWITCH_HAL_FUNCTION_LIST(CREATE_HAL_FUNCTIONS, arch)          \

#define NVSWITCH_INIT_HAL_LS10(device, arch)                         \
    NVSWITCH_HAL_FUNCTION_LIST_LS10(CREATE_HAL_FUNCTIONS, arch)      \

//
// Declare HAL function dispatch functions
//
// This macro takes the xmacro _FUNCTION_LIST and uses some components in it to
// automatically generate the function prototypes for the dispatcher functions
// that dereference the correct arch HAL function.
//

#define DECLARE_HAL_DISPATCHERS(_return, _function, _params, _arch) \
    _return _function _params;

NVSWITCH_HAL_FUNCTION_LIST(DECLARE_HAL_DISPATCHERS, unused_argument)
NVSWITCH_HAL_FUNCTION_LIST_LS10(DECLARE_HAL_DISPATCHERS, unused_argument)

// HAL functions
void nvswitch_setup_hal_lr10(nvswitch_device *device);
void nvswitch_setup_hal_ls10(nvswitch_device *device);

#endif //_HALDEF_NVSWITCH_H_
