/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _REGKEY_NVSWITCH_H_
#define _REGKEY_NVSWITCH_H_


#include "export_nvswitch.h"

/*
 * NV_SWITCH_REGKEY_TXTRAIN_OPTIMIZATION_ALGORITHM - Select TXTRAIN optimization algorithm
 *
 * NVLink3.0 Allows for multiple optimization algorithms A0-A7
 * Documentation on details about each algorithm can be found in
 * the IAS section "4.4.3.3. Optimization Algorithms"
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL                                     "TxTrainControl"

#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_NOP                                 0x00000000
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_FOM_FORMAT                          2:0
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_FOM_FORMAT_NOP                      0x00000000
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_FOM_FORMAT_FOMA                     0x00000001
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_FOM_FORMAT_FOMB                     0x00000002
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM              10:3
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_NOP          0x00000000
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A0           0x00000001
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A1           0x00000002
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A2           0x00000004
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A3           0x00000008
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A4           0x00000010
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A5           0x00000020
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A6           0x00000040
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_A7           0x00000080
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM                15:11
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM_NOP            0x00000000
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM_B0             0x00000001
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM_B1             0x00000002
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM_B2             0x00000004
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM_B3             0x00000008
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_MANTISSA         19:16
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_MANTISSA_NOP     0x00000000
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_EXPONENT         23:20
#define NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_EXPONENT_NOP     0x00000000

/*
 * NV_SWITCH_REGKEY_EXTERNAL_FABRIC_MGMT - Toggle external fabric management.
 *
 * Switch driver currently uses nvlink core driver APIs which internally trigger
 * link initialization and training. However, nvlink core driver now exposes a
 * set of APIs for managing nvlink fabric externally (from user mode).
 *
 * When the regkey is enabled, switch driver will skip use of APIs which trigger
 * link initialization and training. In that case, link training needs to be
 * triggered externally.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_EXTERNAL_FABRIC_MGMT    "ExternalFabricMgmt"

#define NV_SWITCH_REGKEY_EXTERNAL_FABRIC_MGMT_DISABLE   0x0
#define NV_SWITCH_REGKEY_EXTERNAL_FABRIC_MGMT_ENABLE    0x1

/*
 * NV_SWITCH_REGKEY_CROSSBAR_DBI - Enable/disable crossbar DBI
 * DBI - Data bus inversion provides some small power savings.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_CROSSBAR_DBI           "CrossbarDBI"

#define NV_SWITCH_REGKEY_CROSSBAR_DBI_DISABLE   0x0
#define NV_SWITCH_REGKEY_CROSSBAR_DBI_ENABLE    0x1

/*
 * NV_SWITCH_REGKEY_LINK_DBI - Enable/disable link DBI
 * DBI - Data bus inversion provides some small power savings.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_LINK_DBI               "LinkDBI"

#define NV_SWITCH_REGKEY_LINK_DBI_DISABLE       0x0
#define NV_SWITCH_REGKEY_LINK_DBI_ENABLE        0x1

/*
 * NV_SWITCH_REGKEY_AC_COUPLING_MASK
 *
 * Value is a bitmask of which links are AC coupled and should be
 * configured with SETACMODE.
 * All links default to DC coupled.
 *
 * Mask  contains links  0-31
 * Mask2 contains links 32-63
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_AC_COUPLED_MASK     "ACCoupledMask"
#define NV_SWITCH_REGKEY_AC_COUPLED_MASK2    "ACCoupledMask2"

/*
 * NV_SWITCH_REGKEY_SWAP_CLK_OVERRIDE
 *
 * Value is a bitmask applied directly to _SWAP_CLK field.
 * bit 0: select source for RXCLK_0P/N - ports 0-7
 * bit 1: select source for RXCLK_1P/N - ports 16-17
 * bit 2: select source for RXCLK_2P/N - ports 8-15
 * bit 3: unconnected
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_SWAP_CLK_OVERRIDE  "SwapClkOverride"

#define NV_SWITCH_REGKEY_SWAP_CLK_OVERRIDE_FIELD    3:0

/*
 * NV_SWITCH_REGKEY_ENABLE_LINK_MASK - Mask of links to enable
 *
 * By default, all links are enabled
 *
 * [0]=1 - Enable link 0
 *  :
 * [31]=1 - Enable link 31
 *
 * Mask  contains links  0-31
 * Mask2 contains links 32-63
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_ENABLE_LINK_MASK      "LinkEnableMask"
#define NV_SWITCH_REGKEY_ENABLE_LINK_MASK2     "LinkEnableMask2"

/*
 * NV_SWITCH_REGKEY_BANDWIDTH_SHAPER
 *
 * Selects among various transaction fairness modes affecting bandwidth
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_BANDWIDTH_SHAPER   "BandwidthShaper"

#define NV_SWITCH_REGKEY_BANDWIDTH_SHAPER_PROD              0x0
#define NV_SWITCH_REGKEY_BANDWIDTH_SHAPER_XSD               0x1
#define NV_SWITCH_REGKEY_BANDWIDTH_SHAPER_BUCKET_BW         0x2
#define NV_SWITCH_REGKEY_BANDWIDTH_SHAPER_BUCKET_TX_FAIR    0x3

/*
 * NV_SWITCH_REGKEY_SSG_CONTROL
 *
 * Internal use only (supported only on MODS)
 * Allows SSG interface to tweak internal behavior for testing & debugging
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_SSG_CONTROL                            "SSGControl"
#define NV_SWITCH_REGKEY_SSG_CONTROL_BREAK_AFTER_UPHY_INIT      0:0
#define NV_SWITCH_REGKEY_SSG_CONTROL_BREAK_AFTER_UPHY_INIT_NO   (0x00000000)
#define NV_SWITCH_REGKEY_SSG_CONTROL_BREAK_AFTER_UPHY_INIT_YES  (0x00000001)
#define NV_SWITCH_REGKEY_SSG_CONTROL_BREAK_AFTER_DLPL_INIT      1:1
#define NV_SWITCH_REGKEY_SSG_CONTROL_BREAK_AFTER_DLPL_INIT_NO   (0x00000000)
#define NV_SWITCH_REGKEY_SSG_CONTROL_BREAK_AFTER_DLPL_INIT_YES  (0x00000001)

/*
 * NV_SWITCH_REGKEY_SKIP_BUFFER_READY
 *
 * Used to optionally skip the initialization of NVLTLC_TX_CTRL_BUFFER_READY,
 * NVLTLC_RX_CTRL_BUFFER_READY, and NPORT_CTRL_BUFFER_READY registers.
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY                      "SkipBufferReady"
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY_TLC                  0:0
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY_TLC_NO               (0x00000000)
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY_TLC_YES              (0x00000001)
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY_NPORT                1:1
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY_NPORT_NO             (0x00000000)
#define NV_SWITCH_REGKEY_SKIP_BUFFER_READY_NPORT_YES            (0x00000001)

/*
 * NV_SWITCH_REGKEY_SOE_DISABLE - Disables init and usage of SOE by the kernel driver
 *
 * The NVSwitch driver relies on SOE for some features, but can operate
 * without it, with reduced functionality.
 *
 * When the regkey is set to YES, the Nvswitch driver disregards SOE and will not
 * bootstrap it with the driver payload image. All interactions between
 * the driver and SOE are disabled.
 *
 * Driver unload doesn't idle already bootstrapped SOE. Hence it is
 * recommended to reset device in order disable SOE completely. The pre-OS image
 * will still be running even though SOE is disabled through the driver.
 *
 * If set to NO, the SOE will function as normal.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_SOE_DISABLE            "SoeDisable"
#define NV_SWITCH_REGKEY_SOE_DISABLE_NO         0x0
#define NV_SWITCH_REGKEY_SOE_DISABLE_YES        0x1

/*
 * NV_SWITCH_REGKEY_ENABLE_PM
 *
 * Used to optionally send the ENABLE_PM command to MINION on link training
 * and DISABLE_PM on link teardown.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_ENABLE_PM                              "EnablePM"
#define NV_SWITCH_REGKEY_ENABLE_PM_NO                           0x0
#define NV_SWITCH_REGKEY_ENABLE_PM_YES                          0x1

/*
 * NV_SWITCH_REGKEY_MINION_SET_UCODE*
 *
 * The following regkeys are used to override MINION image in the driver.
 *
 * The ucode image is overriden from .js file given along the regkey -nvswitch_set_minion_ucode.
 *
 * Private: Debug use only
 */

/*
 * Overrides MINION image data with g_os_ucode_data_nvswitch_minion it fetches from js file.
 */
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_DATA                     "MinionSetUCodeData"

/*
 * Overrides MINION header with g_os_ucode_header_nvswitch_minion it fetches from js file.
 */
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_HDR                      "MinionSetUCodeHdr"

/*
 * Overrides MINION ucode data size with g_os_ucode_data_nvswitch_minion_size it fetches from js file.
 */
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_DATA_SIZE                "MinionSetUCodeDataSize"

/*
 * Overrides MINION ucode data size with g_os_ucode_data_nvswitch_minion_size it fetches from js file.
 */
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_HDR_SIZE                "MinionSetUCodeHdrSize"

/*
 * NV_SWITCH_REGKEY_CHIPLIB_FORCED_LINK_CONFIG_MASK
 *
 * Internal use only
 * This notifies the driver that we are using a chiplib forced link config
 * to initialize and train the links.
 * Mask  contains links  0-31
 * Mask2 contains links 32-63
 *
 * This is intended for sim platforms only where MINION is not available
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_CHIPLIB_FORCED_LINK_CONFIG_MASK             "ChiplibForcedLinkConfigMask"
#define NV_SWITCH_REGKEY_CHIPLIB_FORCED_LINK_CONFIG_MASK2            "ChiplibForcedLinkConfigMask2"

/*
 * Initiates DMA selftest on SOE during init. Default is disable.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_SOE_DMA_SELFTEST                       "SoeDmaSelfTest"
#define NV_SWITCH_REGKEY_SOE_DMA_SELFTEST_DISABLE                0x00
#define NV_SWITCH_REGKEY_SOE_DMA_SELFTEST_ENABLE                 0x01

/*
 * CCI Control
 *  
 * This regkey controls enablement of CCI on LS10 systems.
 *
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_CCI_CONTROL                            "CCIControl"
#define NV_SWITCH_REGKEY_CCI_CONTROL_DEFAULT                    0x1
#define NV_SWITCH_REGKEY_CCI_CONTROL_ENABLE                     0:0
#define NV_SWITCH_REGKEY_CCI_CONTROL_ENABLE_FALSE               0x00
#define NV_SWITCH_REGKEY_CCI_CONTROL_ENABLE_TRUE                0x01

/*
 * CCI Link Train Disable Mask 
 *
 * These regkeys will disable link training for CCI managed links.
 *
 * Mask  contains links  0-31
 * Mask2 contains links 32-63
 *
 * Private: Will only be applied on debug/develop drivers and MODS
 */
#define NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK                       "CCILinkTrainDisableMask"
#define NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK2                      "CCILinkTrainDisableMask2"
#define NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK_DEFAULT               0x0
#define NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK2_DEFAULT              0x0

/*
 * CCI Max Onboard Attempts 
 *  
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_CCI_MAX_ONBOARD_ATTEMPTS               "CCIMaxOnboardAttempts"
#define NV_SWITCH_REGKEY_CCI_MAX_ONBOARD_ATTEMPTS_DEFAULT       0x5

/*
 * CCI Error Log Enable 
 *
 * This regkey will enable emission of CCI module onboarding SXids errors. 
 *  
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_CCI_ERROR_LOG_ENABLE                   "CCIErrorLogEnable"
#define NV_SWITCH_REGKEY_CCI_ERROR_LOG_ENABLE_DEFAULT           0x1

/*
 * Disables logging of latency counters
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_LATENCY_COUNTER_LOGGING                 "LatencyCounterLogging"
#define NV_SWITCH_REGKEY_LATENCY_COUNTER_LOGGING_DISABLE         0x00
#define NV_SWITCH_REGKEY_LATENCY_COUNTER_LOGGING_ENABLE          0x01

/*
 * Knob to change NVLink link speed
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_SPEED_CONTROL                          "SpeedControl"
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED                    4:0
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_DEFAULT            0x00
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_16G                0x01
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_20G                0x03
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_25G                0x08
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_25_78125G          0x08
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_32G                0x0E
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_40G                0x0F
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_50G                0x10
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_53_12500G          0x11
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_100_00000G         0x12
#define NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_106_25000G         0x13

/*
 * Enable/Disable periodic flush to inforom. Default is disabled.
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_INFOROM_BBX_ENABLE_PERIODIC_FLUSHING           "InforomBbxPeriodicFlush"
#define NV_SWITCH_REGKEY_INFOROM_BBX_ENABLE_PERIODIC_FLUSHING_DISABLE   0x00
#define NV_SWITCH_REGKEY_INFOROM_BBX_ENABLE_PERIODIC_FLUSHING_ENABLE    0x01

/*
 * The rate at which the lifetime data about the NVSwitch is written into the BBX object in seconds.
 * This is gated by NV_SWITCH_REGKEY_INFOROM_BBX_ENABLE_PERIODIC_FLUSHING
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_PERIODICITY          "InforomBbxWritePeriodicity"
#define NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_PERIODICITY_DEFAULT  600 // 600 seconds (10 min)

/*
 * The minimum duration the driver must run before writing to the BlackBox Recorder (BBX) object
 * on driver exit (in seconds).
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_MIN_DURATION             "InforomBbxWriteMinDuration"
#define NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_MIN_DURATION_DEFAULT     30 // 30 seconds

/*
 * Change ATO timer value
 *
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_ATO_CONTROL                            "ATOControl"
#define NV_SWITCH_REGKEY_ATO_CONTROL_DEFAULT                    0x0
#define NV_SWITCH_REGKEY_ATO_CONTROL_TIMEOUT                    19:0
#define NV_SWITCH_REGKEY_ATO_CONTROL_TIMEOUT_DEFAULT            0x00
#define NV_SWITCH_REGKEY_ATO_CONTROL_DISABLE                    20:20
#define NV_SWITCH_REGKEY_ATO_CONTROL_DISABLE_FALSE              0x00
#define NV_SWITCH_REGKEY_ATO_CONTROL_DISABLE_TRUE               0x01

/*
 * Change STO timer value
 *
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_STO_CONTROL                            "STOControl"
#define NV_SWITCH_REGKEY_STO_CONTROL_DEFAULT                    0x0
#define NV_SWITCH_REGKEY_STO_CONTROL_TIMEOUT                    19:0
#define NV_SWITCH_REGKEY_STO_CONTROL_TIMEOUT_DEFAULT            0x00
#define NV_SWITCH_REGKEY_STO_CONTROL_DISABLE                    20:20
#define NV_SWITCH_REGKEY_STO_CONTROL_DISABLE_FALSE              0x00
#define NV_SWITCH_REGKEY_STO_CONTROL_DISABLE_TRUE               0x01

/*
 * NV_SWITCH_REGKEY_MINION_DISABLE - Disables init and usage of MINION by the kernel driver
 *
 * The NVSwitch driver relies on MINION for some features, but can operate
 * without it and is required for Bug 2848340.
 *
 * When the regkey is set to YES, the Nvswitch driver disregards MINION and will not
 * bootstrap it. All interactions between the driver and MINION are disabled.
 *
 * If set to NO, the MINION will function as normal.
 *
 * Private: Debug use only
 */

#define NV_SWITCH_REGKEY_MINION_DISABLE            "MinionDisable"
#define NV_SWITCH_REGKEY_MINION_DISABLE_NO         0x0
#define NV_SWITCH_REGKEY_MINION_DISABLE_YES        0x1

/*
 * NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET - Selects the core on which Minion will run
 *
 * When the regkey is set to FALCON, the Nvswitch driver will run MINION on Falcon core.
 *
 * If set to RISCV, the MINION will run on RISCV core in Non-Manifest Mode.
 * If set to RISCV_MANIFEST, the MINION will run on RISCV core in Manifest Mode.
 *
 * In the default option, RISCV_BCR_CTRL register will be used to get the default core.
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET                "MinionSetUcodeTarget"
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET_DEFAULT        0x0
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET_FALCON         0x1
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET_RISCV          0x2
#define NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET_RISCV_MANIFEST 0x3

/*
 * NV_SWITCH_REGKEY_MINION_SET_SIMMODE - Selects simmode settings to send to MINION
 *
 * Regkey is set to either SLOW, MEDIUM or FAST depending on the environment and timing
 * needed by MINION to setup alarms during the training sequence
 *
 * In the default option, no SIMMODE is selected
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_MINION_SET_SIMMODE          "MinionSetSimmode"
#define NV_SWITCH_REGKEY_MINION_SET_SIMMODE_DEFAULT     0x0
#define NV_SWITCH_REGKEY_MINION_SET_SIMMODE_FAST        0x1
#define NV_SWITCH_REGKEY_MINION_SET_SIMMODE_MEDIUM      0x2
#define NV_SWITCH_REGKEY_MINION_SET_SIMMODE_SLOW        0x3

/*
 * NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS - Selects SMF settings to send to MINION
 *
 * Regkey is set to either SLOW, MEDIUM or FAST depending on the environment and timing
 * needed by MINION to setup alarms during the training sequence
 *
 * In the default option, no SMF settings are selected
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS        "MinionSmfSettings"
#define NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_DEFAULT        0x0
#define NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_FAST           0x1
#define NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_MEDIUM         0x2
#define NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_SLOW           0x3
#define NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_MEDIUM_SERIAL  0x4

/*
 * NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES - Selects uphy tables to send to MINION
 *
 * Regkey is set to either SHORT or FAST depending on the environment and timing
 * needed by MINION to setup alarms during the training sequence
 *
 * In the default option, no UPHY table is selected
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES        "MinionSelectUphyTables"
#define NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES_DEFAULT     0x0
#define NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES_SHORT       0x1
#define NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES_FAST        0x2

/*
 * NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS - Programs the L1_RECAL fields
 *
 * Regkey is used to program the the following:
 *
 * MIN_RECAL_TIME_MANTISSA
 * MIN_RECAL_TIME_EXPONENT
 * MAX_RECAL_PERIOD_MANTISSA
 * MAX_RECAL_PERIOD_EXPONENT
 *
 * In the default option, no L1_RECAL fields are programmed
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS                               "LinkRecalSettings"
#define NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP                           0x0
#define NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_MIN_RECAL_TIME_MANTISSA       3:0
#define NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_MIN_RECAL_TIME_EXPONENT       7:4
#define NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_MAX_RECAL_PERIOD_MANTISSA    11:8
#define NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_MAX_RECAL_PERIOD_EXPONENT    15:12

/*
 * Used to disable private internal-use only regkeys from release build drivers
 */

#define NV_SWITCH_REGKEY_PRIVATE                1
#define NV_SWITCH_REGKEY_PUBLIC                 0

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
#define NV_SWITCH_REGKEY_PRIVATE_ALLOWED       1
#else
#define NV_SWITCH_REGKEY_PRIVATE_ALLOWED       0
#endif  //defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

/*
 * NV_SWITCH_REGKEY_LINK_TRAINING_SELECT - Select the Link training to be done
 *
 * For LS10, links can be trained via non-ALI or ALI training. This regkey will
 * allow for overriding System Defaults and can force either training method
 * when desired.
 */
#define NV_SWITCH_REGKEY_LINK_TRAINING_SELECT           "LinkTrainingMode"
#define NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_DEFAULT   0x0
#define NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_NON_ALI   0x1
#define NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_ALI       0x2
/*
 * NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL - Enable access to all I2C Ports/Devices
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL                "I2cAccessControl"
#define NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_DEFAULT        0x0
#define NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_ENABLE         0x1
#define NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_DISABLE        0x0

/*
 * NV_SWITCH_REGKEY_FORCE_KERNEL_I2C - Used to force Kernel I2C path
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_FORCE_KERNEL_I2C                "ForceKernelI2c"
#define NV_SWITCH_REGKEY_FORCE_KERNEL_I2C_DEFAULT        0x0
#define NV_SWITCH_REGKEY_FORCE_KERNEL_I2C_ENABLE         0x1
#define NV_SWITCH_REGKEY_FORCE_KERNEL_I2C_DISABLE        0x0

/*
 * NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT - Configure the CRC bit error rate for the short interrupt
 * 
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT                  "CRCBitErrorRateShort"
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_DEFAULT          0x0
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN    2:0
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP    3:3
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN    6:4
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP    11:8

/*
 * NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG - Configure the CRC bit error rate for the long interrupt
 * 
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG                       "CRCBitErrorRateLong"
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_DEFAULT               0x000
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_BUG_3365481_CASE_1    0x803
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_BUG_3365481_CASE_2    0x703
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_BUG_3365481_CASE_5    0x34D
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_BUG_3365481_CASE_6    0x00F
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_MAN         2:0
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_EXP         3:3
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_MAN         6:4
#define NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_EXP         12:8

/*
 * NV_SWITCH_REGKEY_SET_LP_THRESHOLD - Sets the LP Threshold Value
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_SET_LP_THRESHOLD                "LPThreshold"
#define NV_SWITCH_REGKEY_SET_LP_THRESHOLD_DEFAULT        0x0

/*
 * NV_SWITCH_REGKEY_MINION_INTERRUPTS - Enable/disable MINION interrupts
 * 
 * Used for bug #3572329.  To be removed once fmodel conflict is resolved.
 *
 * Public: Available in release drivers
 */
#define NV_SWITCH_REGKEY_MINION_INTERRUPTS               "MINIONIntr"
#define NV_SWITCH_REGKEY_MINION_INTERRUPTS_DEFAULT       0x0
#define NV_SWITCH_REGKEY_MINION_INTERRUPTS_ENABLE        0x1
#define NV_SWITCH_REGKEY_MINION_INTERRUPTS_DISABLE       0x2

/*
 * NV_SWITCH_REGKEY_SURPRESS_LINK_ERRORS_FOR_GPU_RESET - surpresses error prints/notifs
 *
 * When set, Heartbeat timeout, Short Error Rate and Fault Up interrupts won't be
 * logged
 *
 * Public: Available in release drivers
 */

#define NV_SWITCH_REGKEY_SURPRESS_LINK_ERRORS_FOR_GPU_RESET    "SurpressLinkErrorsForGpuReset"
#define NV_SWITCH_REGKEY_SURPRESS_LINK_ERRORS_FOR_GPU_RESET_DISABLE   0x0
#define NV_SWITCH_REGKEY_SURPRESS_LINK_ERRORS_FOR_GPU_RESET_ENABLE    0x1

/*
 * NV_SWITCH_REGKEY_BLOCK_CODE_MODE - Indicates the Forward Error Correction Mode
 *
 * Forward Error Correction Mode (Pre-HS).
 * DEFAULT = System Default
 * OFF = 0x0
 * ECC96_ENABLED = 0x1
 * ECC88_ENABLED = 0x2
 */
#define NV_SWITCH_REGKEY_BLOCK_CODE_MODE                   "BlockCodeMode"
#define NV_SWITCH_REGKEY_BLOCK_CODE_MODE_DEFAULT           0x0
#define NV_SWITCH_REGKEY_BLOCK_CODE_MODE_OFF               0x0
#define NV_SWITCH_REGKEY_BLOCK_CODE_MODE_ECC96_ENABLED     0x1
#define NV_SWITCH_REGKEY_BLOCK_CODE_MODE_ECC88_ENABLED     0x2

/*
 * NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE - Indicates the reference clock mode for 
 * the system w.r.t. this link.
 *
 * DEFAULT = System Default
 * COMMON = Common reference clock. Spread Spectrum (SS) may or may not be enabled.
 * NON_COMMON_NO_SS = Non-common reference clock without SS enabled.
 * NON_COMMON_SS = Non-common reference clock with SS enabled.
 */
#define NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE                   "ReferenceClockMode"
#define NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_DEFAULT           0x0
#define NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_COMMON            0x0
#define NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_RESERVED          0x1
#define NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_NON_COMMON_NO_SS  0x2
#define NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_NON_COMMON_SS     0x3

/*
 * NV_SWITCH_REGKEY_DBG_LEVEL - Adjusts the amount of debug prints that will be generated
 *
 * Prints will be generated at the specified level and above. Eg. Debug level 0 will 
 *  enable all prints  
 *
 * Private: Debug use only
 */
#define NV_SWITCH_REGKEY_DBG_LEVEL                              "DebugLevel"
#define NV_SWITCH_REGKEY_DBG_LEVEL_DEFAULT                      NV_SWITCH_REGKEY_DBG_LEVEL_INFO
#define NV_SWITCH_REGKEY_DBG_LEVEL_MMIO                         0x0
#define NV_SWITCH_REGKEY_DBG_LEVEL_NOISY                        0x1
#define NV_SWITCH_REGKEY_DBG_LEVEL_INFO                         0x2
#define NV_SWITCH_REGKEY_DBG_LEVEL_SETUP                        0x3
#define NV_SWITCH_REGKEY_DBG_LEVEL_WARN                         0x4
#define NV_SWITCH_REGKEY_DBG_LEVEL_ERROR                        0x5

#endif //_REGKEY_NVSWITCH_H_
