/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file  flcnretval.h
 * @brief Defines various status codes that are convenient to relay status
 *        information in functions.
 */

#ifndef FLCNRETVAL_H
#define FLCNRETVAL_H

#include "nvtypes.h"
/*!
 * Defines a generic type that may be used to convey status information.  This
 * is very similar to the RM_STATUS type but smaller in width to save DMEM and
 * stack space.
 */
typedef NvU8 FLCN_STATUS;

#define FLCN_OK                                                            (0x00U)
#define FLCN_ERR_BINARY_NOT_STARTED                                        (0xFEU)
#define FLCN_ERROR                                                         (0xFFU)

#define FLCN_ERR_NOT_SUPPORTED                                             (0x01U)
#define FLCN_ERR_INVALID_ARGUMENT                                          (0x02U)
#define FLCN_ERR_ILLEGAL_OPERATION                                         (0x03U)
#define FLCN_ERR_TIMEOUT                                                   (0x04U)
#define FLCN_ERR_NO_FREE_MEM                                               (0x05U)
#define FLCN_ERR_HDCP_INVALID_SRM                                          (0x06U)
#define FLCN_ERR_HDCP_RECV_REVOKED                                         (0x07U)
#define FLCN_ERR_RPC_INVALID_INPUT                                         (0x08U)
#define FLCN_ERR_DMA_SUSPENDED                                             (0x09U)
#define FLCN_ERR_MUTEX_ACQUIRED                                            (0x10U)
#define FLCN_ERR_MUTEX_ID_NOT_AVAILABLE                                    (0x11U)
#define FLCN_ERR_OBJECT_NOT_FOUND                                          (0x12U)
#define FLCN_ERR_MSGBOX_TIMEOUT                                            (0x13U)
#define FLCN_ERR_INVALID_INDEX                                             (0x14U)
#define FLCN_ERR_INVALID_FUNCTION                                          (0x15U)
#define FLCN_ERR_INSUFFICIENT_PMB_PLM_PROTECTION                           (0x16U)
#define FLCN_ERR_DMA_NACK                                                  (0x17U)
#define FLCN_ERR_CHIP_NOT_SUPPORTED_FOR_PR                                 (0x18U)
#define FLCN_ERR_BAR0_PRIV_READ_ERROR                                      (0x19U)
#define FLCN_ERR_BAR0_PRIV_WRITE_ERROR                                     (0x1AU)
#define FLCN_ERR_HDCP22_ABORT_AUTHENTICATION                               (0x1BU)
#define FLCN_ERR_DPU_IS_BUSY                                               (0x1CU)
#define FLCN_ERR_DPU_TIMEOUT_FOR_HDCP_TYPE1_LOCK_REQUEST                   (0x1DU)
#define FLCN_ERR_HDCP_TYPE1_LOCK_FAILED                                    (0x1EU)
#define FLCN_ERR_HDCP_TYPE1_LOCK_UNKNOWN                                   (0x1FU)
#define FLCN_ERR_WAIT_FOR_BAR0_IDLE_FAILED                                 (0x20U)
#define FLCN_ERR_CSB_PRIV_READ_ERROR                                       (0x21U)
#define FLCN_ERR_CSB_PRIV_WRITE_ERROR                                      (0x22U)
#define FLCN_ERR_DMA_UNEXPECTED_DMAIDX                                     (0x23U)
#define FLCN_ERR_PRIV_SEC_VIOLATION                                        (0x24U)
#define FLCN_ERR_INVALID_VERSION                                           (0x25U)
#define FLCN_ERR_PR_SHARED_STRUCT_INIT_FAILED                              (0x26U)
#define FLCN_ERR_GPU_IN_DEBUG_MODE                                         (0x27U)
#define FLCN_ERR_HPD_UNPLUG                                                (0x28U)
#define FLCN_ERR_HDCP22_DELAY_ABORT_AUTHENTICATION                         (0x29U)
#define FLCN_ERR_SECUREBUS_REGISTER_READ_ERROR                             (0x2AU)
#define FLCN_ERR_SECUREBUS_REGISTER_WRITE_ERROR                            (0x2BU)
#define FLCN_ERR_HDCP22_FLUSH_TYPE_LOCK_ACTIVE                             (0x2CU)
#define FLCN_ERR_HDCP22_FLUSH_TYPE_IN_PROGRESS                             (0x2DU)
#define FLCN_ERR_FEATURE_NOT_ENABLED                                       (0x2EU)
#define FLCN_ERR_OUT_OF_RANGE                                              (0x2FU)

// I2C Errors
#define FLCN_ERR_I2C_BUSY                                                  (0x30U)
#define FLCN_ERR_I2C_NACK_ADDRESS                                          (0x31U)
#define FLCN_ERR_I2C_NACK_BYTE                                             (0x32U)
#define FLCN_ERR_I2C_SIZE                                                  (0x33U)
#define FLCN_ERR_I2C_BUS_INVALID                                           (0x34U)

#define FLCN_ERR_INVALID_STATE                                             (0x35U)
#define FLCN_ERR_RECURSION_LIMIT_EXCEEDED                                  (0x36U)
#define FLCN_ERR_INVALID_CAST                                              (0x37U)

// AUX Errors
#define FLCN_ERR_AUX_ERROR                                                 (0x3AU)
#define FLCN_ERR_AUX_SEMA_ACQUIRED                                         (0x3BU)
#define FLCN_ERR_AUX_SEMA_INVALID_RELEASE                                  (0x3CU)

#define FLCN_ERR_MORE_PROCESSING_REQUIRED                                  (0x3EU)
#define FLCN_ERR_DMA_ALIGN                                                 (0x3FU)

// Power-Device Errors
#define FLCN_ERR_PWR_DEVICE_TAMPERED                                       (0x40U)

#define FLCN_ERR_ITERATION_END                                             (0x41U)

// Perf change sequence Errors
#define FLCN_ERR_LOCK_NOT_AVAILABLE                                        (0x42U)

#define FLCN_ERR_STATE_RESET_NEEDED                                        (0x43U)
#define FLCN_ERR_DMA_GENERIC                                               (0x44U)
#define FLCN_ERR_LS_CHK_UCODE_REVOKED                                      (0x45U)
#define FLCN_ERR_ACC_SEQ_MISMATCH                                          (0x46U)
#define FLCN_ERR_SSP_STACK_CHECK_FAILED                                    (0x47U)
#define FLCN_ERR_SE_TRNG_FAILED                                            (0x48U)
#define FLCN_ERR_PROD_MODE_NOT_YET_SUPPORTED                               (0x49U)

// SHA HW errors
#define FLCN_ERR_SHA_HW_CHECK_INT_STATUS                                   (0x4AU)
#define FLCN_ERR_SHA_HW_SOFTRESET_REQUIRED                                 (0x4BU)
#define FLCN_ERR_SHA_HW_SOFTRESET_FAILED                                   (0x4CU)
#define FLCN_ERR_SHA_HW_BUSY                                               (0x4DU)

//
// Add new generic error codes here, do not changes values of exiting error codes,
// because that will affect other binaries and their signatures
//
#define FLCN_ERR_CTXSW_ERROR                                               (0x4EU)
#define FLCN_ERR_SE_SNPKA_HW_CRYPTO_OPERATION_FAILED                       (0x4FU)

// VPR SEC2 task errors
#define FLCN_ERR_VPR_APP_INVALID_REQUEST_END_ADDR                          (0x51U)
#define FLCN_ERR_VPR_APP_INVALID_REQUEST_START_ADDR                        (0x52U)
#define FLCN_ERR_VPR_APP_SCRUB_VERIF_FAILED                                (0x53U)
#define FLCN_ERR_VPR_APP_MEMLOCK_ALREADY_SET                               (0x54U)
#define FLCN_ERR_VPR_APP_INVALID_INDEX                                     (0x55U)
#define FLCN_ERR_VPR_APP_UNEXPECTED_VPR_HANDOFF_FROM_SCRUBBER              (0x56U)
#define FLCN_ERR_VPR_APP_CBC_RANGE_CLASH                                   (0x57U)
#define FLCN_ERR_VPR_APP_NOT_SUPPORTED_BY_HW                               (0x58U)
#define FLCN_ERR_VPR_APP_NOT_SUPPORTED_BY_SW                               (0x59U)
#define FLCN_ERR_VPR_APP_DISPLAY_VERSION_NOT_SUPPORTED                     (0x5AU)
#define FLCN_ERR_VPR_APP_VPR_WPR_WRITE_FAILED                              (0x5BU)
#define FLCN_ERR_VPR_APP_NOTHING_TO_DO                                     (0x5CU)
#define FLCN_ERR_VPR_APP_DISPLAY_NOT_PRESENT                               (0x5DU)
#define FLCN_ERR_VPR_APP_PREVIOUS_CMD_FAILED_AS_MAX_VPR_IS_0               (0x5EU)
#define FLCN_ERR_VPR_APP_PLM_PROTECTION_NOT_RAISED                         (0x5FU)
#define FLCN_ERR_VPR_APP_PLM_PROTECTION_ALREADY_RAISED                     (0x60U)
#define FLCN_ERR_VPR_APP_DISP_FALCON_IS_NOT_IN_LS_MODE                     (0x61U)
#define FLCN_ERR_VPR_APP_VPR_IS_ALREADY_ENABLED                            (0x62U)
#define FLCN_ERR_VPR_APP_UNEXPECTEDLY_RUNNING_ON_RISCV                     (0x63U)
#define FLCN_ERR_INVALID_SR_CONFIGURATION                                  (0x64U)

// Clocks Errors
#define FLCN_ERR_CYCLE_DETECTED                                            (0x70U)
#define FLCN_ERR_INVALID_PATH                                              (0x71U)
#define FLCN_ERR_MISMATCHED_TARGET                                         (0x72U)
#define FLCN_ERR_FREQ_NOT_SUPPORTED                                        (0x73U)
#define FLCN_ERR_INVALID_SOURCE                                            (0x74U)
#define FLCN_ERR_NOT_INITIALIZED                                           (0x75U)
#define FLCN_ERR_CLOCK_NOT_SETTLED                                         (0x76U)

// HDCP2.2 Errors
#define FLCN_ERR_HDCP22_GETDKEY_FAILED                                     (0x80U)
#define FLCN_ERR_HDCP22_H_PRIME                                            (0x81U)
#define FLCN_ERR_HDCP22_CERT_RX                                            (0x82U)
#define FLCN_ERR_HDCP22_PAIRING                                            (0x83U)
#define FLCN_ERR_HDCP22_L_PRIME                                            (0x84U)
#define FLCN_ERR_HDCP22_V_PRIME                                            (0x85U)
#define FLCN_ERR_HDCP22_INVALID_RXIDLIST                                   (0x86U)
#define FLCN_ERR_HDCP22_M_PRIME                                            (0x87U)
#define FLCN_ERR_HDCP22_SEQ_ROLLOVER                                       (0x88U)
#define FLCN_ERR_HDCP22_RSA_HW                                             (0x89U)
#define FLCN_ERR_HDCP22_ECF_TIMESLOT_MISMATCH                              (0x90U)

// LibCCC Errors
#define FLCN_ERR_INIT_CRYPTO_DEVICE_FAILED                                 (0x91U)
#define FLCN_ERR_NVPKA_SELECT_ENGINE_FAILED                                (0x92U)
#define FLCN_ERR_NVPKA_ACQUIRE_MUTEX_FAILED                                (0x93U)
#define FLCN_ERR_NVPKA_MODULAR_EXP_LOCK_FAILED                             (0x94U)
#define FLCN_ERR_NVRNG_INIT_CRYPTO_DEVICE_FAILED                           (0x95U)
#define FLCN_ERR_NVRNG_SELECT_ENGINE_FAILED                                (0x96U)
#define FLCN_ERR_NVRNG_GENERATE_FAILED                                     (0x97U)
#define FLCN_ERR_NVPKA_ECDSA_INIT_FAILED                                   (0x98U)
#define FLCN_ERR_NVPKA_ECDSA_SIGN_FAILED                                   (0x99U)
#define FLCN_ERR_NVPKA_GET_ECC_KEY_PAIR_FAILED                             (0x9AU)

// Heavy Secure Errors
#define FLCN_ERR_HS_CHK_INVALID_INPUT                                      (0xA0U)
#define FLCN_ERR_HS_CHK_CHIP_NOT_SUPPORTED                                 (0xA1U)
#define FLCN_ERR_HS_CHK_UCODE_REVOKED                                      (0xA2U)
#define FLCN_ERR_HS_CHK_NOT_IN_LSMODE                                      (0xA3U)
#define FLCN_ERR_HS_CHK_INVALID_LS_PRIV_LEVEL                              (0xA4U)
#define FLCN_ERR_HS_CHK_INVALID_REGIONCFG                                  (0xA5U)
#define FLCN_ERR_HS_CHK_PRIV_SEC_DISABLED_ON_PROD                          (0xA6U)
#define FLCN_ERR_HS_CHK_SW_FUSING_ALLOWED_ON_PROD                          (0xA7U)
#define FLCN_ERR_HS_CHK_INTERNAL_SKU_ON_PROD                               (0xA8U)
#define FLCN_ERR_HS_CHK_DEVID_OVERRIDE_ENABLED_ON_PROD                     (0xA9U)
#define FLCN_ERR_HS_CHK_INCONSISTENT_PROD_MODES                            (0xAAU)
#define FLCN_ERR_HS_CHK_HUB_ENCRPTION_DISABLED                             (0xABU)
#define FLCN_ERR_HS_PR_ILLEGAL_LASSAHS_STATE_AT_HS_ENTRY                   (0xACU)
#define FLCN_ERR_HS_PR_ILLEGAL_LASSAHS_STATE_AT_MPK_DECRYPT                (0xADU)
#define FLCN_ERR_HS_PR_ILLEGAL_LASSAHS_STATE_AT_HS_EXIT                    (0xAEU)
#define FLCN_ERR_HS_PROD_MODE_NOT_YET_SUPPORTED                            (0xAFU)
#define FLCN_ERR_HS_DEV_VERSION_ON_PROD                                    (0xB0U)
#define FLCN_ERR_HS_PR_LASSAHS_LS_SIG_GRP_MISMATCH                         (0xB1U)
#define FLCN_ERR_HS_PR_LASSAHS_LS_SIG_GRP_OVERLAYS_CNT                     (0xB2U)
#define FLCN_ERR_HS_PR_LASSAHS_LS_SIG_GRP_INVALID_VA                       (0xB3U)
#define FLCN_ERR_HS_MUTEX_ACQUIRE_FAILED                                   (0xB4U)
#define FLCN_ERR_HS_MUTEX_RELEASE_FAILED                                   (0xB5U)
#define FLCN_ERR_HS_PR_MPK_DEC_NEEDS_NEWER_ACR_UDE_SCRUBBER                (0xB6U)
#define FLCN_ERR_HS_CHK_ENGID_MISMATCH                                     (0xB7U)
#define FLCN_ERR_HS_OPT_INTERNAL_SKU_CHECK_FAILED                          (0xB8U)
#define FLCN_ERR_HS_CHK_BOARD_MISMATCH                                     (0xB9U)
#define FLCN_ERR_HS_CHK_DISP_ENG_DISABLED                                  (0xBAU)
#define FLCN_ERR_HS_GEN_RANDOM                                             (0xBBU)
#define FLCN_ERR_HS_CHK_IMPROPERLY_FUSED_BOARD                             (0xBCU)
#define FLCN_ERR_HS_CHK_HDCP_DISABLED                                      (0xBDU)
#define FLCN_ERR_HS_CHK_HDCP_BLACKLISTED_SKU                               (0XBEU)
#define FLCN_ERR_HS_SECURE_ACTION_ARG_CHECK_FAILED                         (0xBFU)
#define FLCN_ERR_HS_CHK_RETURN_PC_AT_HS_ENTRY_IS_OF_HS                     (0xC0U)
#define FLCN_ERR_HS_CHK_HS_LIB_ENTRY_CALLED_BY_NON_HS                      (0xC1U)
#define FLCN_ERR_HS_DECODE_TRAP_ALREADY_IN_USE                             (0xC2U)
#define FLCN_ERR_HS_REGISTER_READ_WRITE_ERROR                              (0xC3U)
#define FLCN_ERR_HS_CHK_CPUCTL_ALIAS_FALSE                                 (0xC4U)
#define FLCN_ERR_HS_UPDATE_RESET_PLM_ERROR                                 (0xC5U)
#define FLCN_ERR_HS_RNG_CONFIG_ERROR                                       (0xC6U)
#define FLCN_ERR_HS_CHK_NOT_IN_HSMODE                                      (0xC7U)
#define FLCN_ERR_HS_CHK_GFW_CHAIN_OF_TRUST_BROKEN                          (0xC8U)
#define FLCN_ERR_HS_HDCP22_WRONG_SEQUENCE                                  (0xC9U)
#define FLCN_ERR_HS_HDCP22_INTEGRITY_ERROR                                 (0xCAU)
#define FLCN_ERR_HS_HDCP22_WRONG_TYPE                                      (0xCBU)
#define FLCN_ERR_HS_APM_NOT_ENABLED                                        (0xCCU)
#define FLCN_ERR_HS_APM_SMC_ENABLED                                        (0xCDU)
#define FLCN_ERR_HS_APM_FECS_NOT_HALTED                                    (0xCEU)
#define FLCN_ERR_HS_APM_SCRATCH_PLM_INVALID                                (0xCFU)
#define FLCN_ERR_HS_APM_SCRATCH_INIT_INVALID                               (0xD0U)
#define FLCN_ERR_HS_INVALID_KEY                                            (0xD1U)
#define FLCN_ERR_HS_SWKG_INVALID_SIGNATURE                                 (0xD2U)
#define FLCN_ERR_HS_SCP_LIB_DMHASH_FAILED                                  (0xD3U)
#define FLCN_ERR_HS_SCP_LIB_AESKDF_FAILED                                  (0xD4U)

//
// BAR0/CSB Priv Read/Write Error Handling Defines
// These need to be defined by HW - NV Bug 200198584
//
#define FLCN_BAR0_PRIV_PRI_ERROR_MASK                     0xFFF00000U
#define FLCN_BAR0_PRIV_PRI_ERROR_CODE                     0xBAD00000U
#define FLCN_BAR0_PRIV_PRI_RETURN_VAL                     0x00BADBADU

#define FLCN_CSB_PRIV_PRI_ERROR_MASK                      0xFFFF0000U
#define FLCN_CSB_PRIV_PRI_ERROR_CODE                      0xBADF0000U

//
// Macro to check FALCON return status
//
#define CHECK_FLCN_STATUS(expr) do {                    \
        flcnStatus = (expr);                            \
        if (flcnStatus != FLCN_OK)                      \
        {                                               \
            goto ErrorExit;                             \
        }                                               \
    } while (NV_FALSE)

// Warnings.
#define FLCN_WARN_NOTHING_TO_DO                                            (0xD0U)
#define FLCN_WARN_NOT_QUERIED                                              (0xD1U)
#define FLCN_WARN_DATA_NOT_AVAILABLE                                       (0xD2U)

// Queue handling Errors
#define FLCN_ERR_QUEUE_MGMT_INVALID_UNIT_ID                                (0xE0U)
#define FLCN_ERR_QUEUE_MGMT_HEAP_MIRROR_ERR                                (0xE1U)
#define FLCN_ERR_QUEUE_TASK_INVALID_EVENT_TYPE                             (0xE2U)
#define FLCN_ERR_QUEUE_TASK_INVALID_UNIT_ID                                (0xE3U)
#define FLCN_ERR_QUEUE_TASK_INVALID_CMD_TYPE                               (0xE4U)
#define FLCN_ERR_QUEUE_TASK_SLOT_NOT_AVAILABLE                             (0xE5U)

// Posted write errors
#define FLCN_ERR_POSTED_WRITE_FAILURE                                      (0xF0U)
#define FLCN_ERR_POSTED_WRITE_INTERRUPTS_ENABLED                           (0xF1U)
#define FLCN_ERR_POSTED_WRITE_PRI_CLUSTER_COUNT_MISMATCH                   (0xF2U)
#define FLCN_ERR_POSTED_WRITE_INCORRECT_PARAMS                             (0xF3U)

// Lane Margining errors
#define FLCN_ERR_LM_INVALID_RECEIVER_NUMBER                                (0xF5U)

// APM errors
#define FLCN_ERR_APM_NOT_FUSED_FOR_EK                                      (0xF6U)
#define FLCN_ERR_APM_BROM_SIGN_FAIL                                        (0xF7U)
// Booter Reload on SEC2-RTOS errors
#define FLCN_ERR_AUTH_GSP_RM_HANDOFF_FAILED                                (0xF8U)
#define FLCN_ERR_INVALID_WPRMETA_MAGIC_OR_REVISION                         (0xF9U)

// Arithmetic errors
#define FLCN_ERR_ARITHMETIC_OVERFLOW                                       (0xFAU)

// Pri errros
#define FLCN_ERR_OUTSTANDING_PRI_ERROR                                     (0xFBU)

#endif // FLCNRETVAL_H
