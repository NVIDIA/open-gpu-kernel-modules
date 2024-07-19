/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_HELPER_H_
#define _NVLINK_HELPER_H_

//
// fabric node id will be used as MSB 16 bits of the link token value to 
// generate a unique token for discovering connections
//
#define NVLINK_FABRIC_NODE_ID_MASK 0xFFFF
#define NVLINK_FABRIC_NODE_ID_POS  48

/**
 * Check if the device type is supported
 */
NvBool nvlink_core_is_supported_device_type(NvU32 devType);

/**
 * Get the link and sublink states for the endpoint
 */
void nvlink_core_get_endpoint_state(nvlink_link *link, nvlink_link_state *linkState);

/**
 * Get the nvlink_device * from the PCI DBDF
 */
void nvlink_core_get_device_by_devinfo(nvlink_device_info *devInfo, nvlink_device **dev);

/**
 * Get the nvlink_link * from the PCI DBDF and link#
 */
void nvlink_core_get_link_by_endpoint(nvlink_endpoint *endPoint, nvlink_link **link);

/**
 * Given the nvlink_link ptr, copy the endpoint details for the link
 */
void nvlink_core_copy_endpoint_info(nvlink_link *connLink, nvlink_endpoint *endPointInfo);

/**
 * Given the nvlink_device ptr, copy the device details
 */
void nvlink_core_copy_device_info(nvlink_device *tmpDev, nvlink_detailed_dev_info *devInfo);


/************************************************************************************************/
/****************************** NVLink initialization functions *********************************/
/************************************************************************************************/

/**
 * Kick-off INITPHASE5 on the given array of links
 */
NvlStatus nvlink_core_initphase5(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Kick-off INITPHASE1 on the given array of links
 */
NvlStatus nvlink_core_initphase1(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Kick-off INITRXTERM on the given array of links
 */
NvlStatus nvlink_core_rx_init_term(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Kick-off receiver detect on the given array of links
 */
NvlStatus nvlink_core_set_rx_detect(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Get receiver detect status on the given array of links
 */
NvlStatus nvlink_core_get_rx_detect(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Get Enable TX common mode on the given array of links
 */
NvlStatus nvlink_core_enable_common_mode(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Get Disable TX common mode on the given array of links
 */
NvlStatus nvlink_core_disable_common_mode(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Perform RX calibration on the given array of links
 */
NvlStatus nvlink_core_calibrate_links(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Enable data on the given array of links
 */
NvlStatus nvlink_core_enable_data(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/**
 * Transition to SWCFG on the given array of links
 */
NvlStatus nvlink_core_link_init_async(nvlink_link **links, NvU32 numLinks);

/**
 * Poll on SAFE/SWCFG on the given link
 */
NvlStatus nvlink_core_wait_for_link_init(nvlink_link *link);

/**
 * Initialize all the endpoints from OFF to SWCFG state
 */
void nvlink_core_init_links_from_off_to_swcfg(nvlink_link **pLinks,
                                              NvU32         numLinks,
                                              NvU32         flags);

/*
 * Initialize all the endpoints from OFF to SWCFG state for Non-ALI sequence
 * Used for nvlink 4.0+
 */
void nvlink_core_init_links_from_off_to_swcfg_non_ALI(nvlink_link **pLinks,
                                                     NvU32         numLinks,
                                                     NvU32         flags);
/**
 * Send INITNEGOTIATE command on the given array of links
 */
NvlStatus nvlink_core_initnegotiate(nvlink_link **links, NvU32 numLinks, NvU32 flags);

/*
 * Initialize all the endpoints from OFF to ACTIVE state for ALI sequence
 * Used for nvlink 4.0+
 */
NvlStatus nvlink_core_train_intranode_conns_from_off_to_active_ALI(nvlink_link **pLinks,
                                                                   NvU32         numLinks);
/************************************************************************************************/
/*************************** NVLink topology discovery functions ********************************/
/************************************************************************************************/

/**
 * Generate a discovery token for the given link
 */
NvU64 nvlink_core_get_link_discovery_token(nvlink_link *link);

/**
 * Write the dicovery token for the given link
 */
NvlStatus nvlink_core_write_link_discovery_token(nvlink_link *link, NvU64 token);

/**
 * Read the dicovery token for the given link
 */
NvU64 nvlink_core_read_link_discovery_token(nvlink_link *link);

/**
 * Detect the connection by correlating the tokens
 */
void nvlink_core_correlate_conn_by_token(nvlink_link *srcLink, NvU64 writeToken, NvBool skipReadToken);

/**
 * For a given end of a link, returns the other end its connected to.
 */
void nvlink_core_discover_and_get_remote_end(nvlink_link  *end,
                                             nvlink_link **remote_end,
                                             NvU32         flags,
                                             NvBool        bForceDiscovery);


/************************************************************************************************/
/********************************** NVLink training functions ***********************************/
/************************************************************************************************/

/**
 * Train the internode connection link from SWCFG to ACTIVE
 */
NvlStatus nvlink_core_train_internode_conns_from_swcfg_to_active(nvlink_internode_conn **conns,
                                                                 NvU32                   connCount,
                                                                 NvU32                  *isMasterEnd,
                                                                 NvU32                   flags);

/**
 *  Train the internode connection sublink to enter high speed
 */
NvlStatus nvlink_core_train_internode_conn_sublink_from_safe_to_hs(nvlink_internode_conn *conn,
                                                                   NvU32                  flags);

/**
 * Train a given set of intranode connections from L2 to ACTIVE state
 */
NvlStatus nvlink_core_train_intranode_conns_from_from_L2_to_active(nvlink_intranode_conn **conns,
                                                                   NvU32                   connCount,
                                                                   NvU32                   flags);

/**
 * Train intranode connections associated with a list of links to HS using ALT sequence
 */
NvlStatus nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(nvlink_intranode_conn **conns,
                                                                     NvU32                   connCount,
                                                                     NvU32                   flags);



/**
 * Train a single intranode connection associated with a list of links to HS using legacy
 * pre-Ampere sequence
 */
NvlStatus nvlink_core_train_intranode_conns_from_swcfg_to_active_legacy(nvlink_intranode_conn **conns,
                                                                        NvU32                   connCount,
                                                                        NvU32                   flags);

/**
 * Train intranode connections associated with a list of links to HS using non-ALI sequence
 * for nvlink 4.0+
 */
NvlStatus nvlink_core_train_intranode_conns_from_swcfg_to_active_non_ALI(nvlink_intranode_conn **conns,
                                                                        NvU32                   connCount,
                                                                        NvU32                   flags);

/**
 * Check to make sure that links are in active and ready for ALI training for nvlink 4.0+
 */
NvlStatus nvlink_core_train_check_link_ready_ALI(nvlink_link **links,
                                                 NvU32         linkCount);

/**
 * Initiate ALI training for nvlink 4.0+
 */
NvlStatus nvlink_core_train_from_off_to_active_ALI(nvlink_link **links,
                                                   NvU32        linkCount);


/************************************************************************************************/
/********************************** NVLink shutdown functions ***********************************/
/************************************************************************************************/

/**
 * [CLEAN SHUTDOWN]
 *
 * Shutdown given intranode connections from active to L2 state
 */
NvlStatus nvlink_core_powerdown_intranode_conns_from_active_to_L2(nvlink_intranode_conn **conns,
                                                                  NvU32                   connCount,
                                                                  NvU32                   flags);

/**
 * [PSEUDO-CLEAN SHUTDOWN]
 *
 * Shutdown the given array of intranode connections from ACTIVE to OFF state
 */
NvlStatus nvlink_core_powerdown_intranode_conns_from_active_to_off(nvlink_intranode_conn **conns,
                                                                   NvU32                   connCount,
                                                                   NvU32                   flags);

/**
 * Power down the given array of intranode connections from ACTIVE to SWCFG state
 */
NvlStatus nvlink_core_powerdown_intranode_conns_from_active_to_swcfg(nvlink_intranode_conn **conns,
                                                                     NvU32                   connCount,
                                                                     NvU32                   flags);

/**
 * Reset the given array of intranode connections
 */
NvlStatus nvlink_core_reset_intranode_conns(nvlink_intranode_conn **conns,
                                            NvU32                   connCount,
                                            NvU32                   flags);

/**
 * Check to make sure that links are in active and ready for ALI training for nvlink 4.0+
 */
NvlStatus nvlink_core_powerdown_floorswept_conns_to_off(nvlink_link **links,
                                       NvU32          numLinks,
                                       NvU32          numIoctrls,
                                       NvU32          numLinksPerIoctrl,
                                       NvU32          numActiveLinksPerIoctrl);


/************************************************************************************************/
/**************************** NVLink connection management functions ****************************/
/************************************************************************************************/

/**
 * For a given link, return the associated internode connection
 */
void nvlink_core_get_internode_conn(nvlink_link            *localLink,
                                    nvlink_internode_conn **conn);

/**
 * Add a new internode connection to the list of connections
 */
NvlStatus nvlink_core_add_internode_conn(nvlink_link                 *localLink,
                                         nvlink_remote_endpoint_info *remoteEndPoint);

/**
 * For a given link, delete the associated internode connection
 */
void nvlink_core_remove_internode_conn(nvlink_link *localLink);

/**
 * For a given link, return the associated intranode connection
 */
void nvlink_core_get_intranode_conn(nvlink_link            *endpoint,
                                    nvlink_intranode_conn **conn);

/**
 * Add a new intranode connection to the list of intranode connections
 */
NvlStatus nvlink_core_add_intranode_conn(nvlink_link *end0, nvlink_link *end1);

/**
 * Remove the connection from the list of intranode connections
 */
void nvlink_core_remove_intranode_conn(nvlink_intranode_conn *conn);

/**
 * Check if the given intranode connection is in the specified mode
 */
NvlStatus nvlink_core_check_intranode_conn_state(nvlink_intranode_conn *conn,
                                                NvU64                  linkMode);

/**
 * Copy the intranode connection's remote endpoint information into the nvlink_conn_info
 * structure passed in
 */
void nvlink_core_copy_intranode_conn_info(nvlink_link      *remote_end,
                                          nvlink_conn_info *conn_info);

/**
 * Copy the internode connection's remote endpoint information into the nvlink_conn_info
 * structure passed in
 */
void nvlink_core_copy_internode_conn_info(nvlink_remote_endpoint_info *remote_end,
                                          nvlink_conn_info            *conn_info);


/************************************************************************************************/
/******************************* NVLink link management functions *******************************/
/************************************************************************************************/

/**
 * For the given link, check whether the link state is at the requested state
 */
NvBool nvlink_core_check_link_state(nvlink_link *link, NvU64 linkState);

/**
 * For the given link, check whether the tx sublink state is at the requested state
 */
NvBool nvlink_core_check_tx_sublink_state(nvlink_link *link, NvU64 txSublinkState);

/**
 * For the given link, check whether the rx sublink state is at the requested state
 */
NvBool nvlink_core_check_rx_sublink_state(nvlink_link *link, NvU64 rxSublinkState);

/**
 * Poll for the link to reach the particular state upto the given timeout. The link
 * state transition is considered failed once timeout occurs
 */
NvlStatus nvlink_core_poll_link_state(nvlink_link *link,
                                      NvU64        linkState,
                                      NvU32        timeout);
/**
 * Poll for a given timeout period for a sublink to reach the particular state. The
 * sublink state transition is considered failed once timeout occurs
 */
NvlStatus nvlink_core_poll_sublink_state(nvlink_link *localTxSubLink,
                                         NvU64        localTxSubLinkState,
                                         NvU32        localTxSubLinkSubtate,
                                         nvlink_link *remoteRxSubLink,
                                         NvU64        remoteRxSubLinkState,
                                         NvU32        remoteRxSubLinkSubstate,
                                         NvU32        timeout);

/**
 * Poll for the tx sublink to reach the specified state upto the given timeout. The
 * sublink state transition is considered failed once timeout occurs
 */
NvlStatus nvlink_core_poll_tx_sublink_state(nvlink_link *link,
                                            NvU64        txSublinkState,
                                            NvU32        txSublinkSubState,
                                            NvU32        timeout);

/**
 * Poll for the rx sublink to reach the specified state upto the given timeout. The
 * sublink state transition is considered failed once timeout occurs
 */
NvlStatus nvlink_core_poll_rx_sublink_state(nvlink_link *link,
                                            NvU64        rxSublinkState,
                                            NvU32        rxSublinkSubState,
                                            NvU32        timeout);

/************************************************************************************************/
/****************** Nvlink print functions for devices/links/connections ************************/
/************************************************************************************************/

void nvlink_core_print_link_state(nvlink_link *link);
void nvlink_core_print_intranode_conn(nvlink_intranode_conn *conn);


#endif //_NVLINK_HELPER_H_
