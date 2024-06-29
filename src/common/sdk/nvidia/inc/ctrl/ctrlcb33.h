/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrlcb33.finn
//



#include "nvcfg_sdk.h"
#include "ctrl/ctrlxxxx.h"
/* NV_CONF_COMPUTE control commands and parameters */

#define NV_CONF_COMPUTE_CTRL_CMD(cat,idx)                   NVXXXX_CTRL_CMD(0xCB33, NVCB33_CTRL_##cat, idx)

#define NVCB33_CTRL_RESERVED          (0x00)
#define NVCB33_CTRL_CONF_COMPUTE      (0x01)

/*
 * NV_CONF_COMPUTE_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible return values:
 *   NV_OK
 */
#define NV_CONF_COMPUTE_CTRL_CMD_NULL (0xcb330000) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES
 *   This control call returns overall system and gpu capabilities
 *
 *   Final operating environment depends on a lot of factors:
 *      APM: Ampere Protected Memory
 *      HCC: Hopper Confidential Compute
 *   ---------------------------------------------------------------------------
 *   SrNo   CPU TEE   GPU TEE   GPU Platform  GPU mode    SW Status   System Env
 *   ---------------------------------------------------------------------------
 *     1    AMD SEV   APM/HCC   Silicon       Production  Production  Production
 *     2    AMD SEV   APM/HCC   Silicon       Production  Development Simulation
 *     3    <Any>     APM/HCC   <Any>         Debug       <Any>       Simulation
 *     4    Non SEV   APM/HCC   <Any>         <Any>       <Any>       Simulation
 *     5    <Any>     APM/HCC   FMOD/EMU/RTL  <Any>       <Any>       Simulation
 *   ---------------------------------------------------------------------------
 *
 *   Prameters:
 *      cpuCapability: [OUT]
 *          This indicates if cpu is capable of AMD SEV
 *      gpusCapability: [OUT]
 *          This indicates if all gpus in the system support APM/HCC.
 *          This field doesn't mean APM/HCC is enabled.
 *      environment: [OUT]
 *          System environment can be production or simulation
 *      ccFeature: [OUT]
 *          Specifies if all gpus in the system have APM/HCC feature enabled
 *          CC feature can be enabled/disabled using this control call:
 *          NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_CC_FEATURE
 *      devToolsMode: [OUT]
 *          Dev tools mode is used for debugging/profiling
 *          Dev tools mode is set at system level and implies that all GPUs in
 *          the system have this mode enabled/disabled
 *      multiGpuMode: [OUT]
 *          Specifies the mode in which a multi gpu system is operating
 *
 *      cpuCapability, gpusCapability & environment are determined by the
 *       driver and cannot be modified later on
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES     (0xcb330101) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x1" */

#define NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_NONE           0
#define NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_AMD_SEV        1
#define NV_CONF_COMPUTE_SYSTEM_CPU_CAPABILITY_INTEL_TDX      2

#define NV_CONF_COMPUTE_SYSTEM_GPUS_CAPABILITY_NONE          0
#define NV_CONF_COMPUTE_SYSTEM_GPUS_CAPABILITY_APM           1
#define NV_CONF_COMPUTE_SYSTEM_GPUS_CAPABILITY_HCC           2

#define NV_CONF_COMPUTE_SYSTEM_ENVIRONMENT_UNAVAILABLE       0
#define NV_CONF_COMPUTE_SYSTEM_ENVIRONMENT_SIM               1
#define NV_CONF_COMPUTE_SYSTEM_ENVIRONMENT_PROD              2

#define NV_CONF_COMPUTE_SYSTEM_FEATURE_DISABLED              0
#define NV_CONF_COMPUTE_SYSTEM_FEATURE_APM_ENABLED           1
#define NV_CONF_COMPUTE_SYSTEM_FEATURE_HCC_ENABLED           2

#define NV_CONF_COMPUTE_SYSTEM_DEVTOOLS_MODE_DISABLED        0
#define NV_CONF_COMPUTE_SYSTEM_DEVTOOLS_MODE_ENABLED         1

#define NV_CONF_COMPUTE_SYSTEM_MULTI_GPU_MODE_NONE           0
#define NV_CONF_COMPUTE_SYSTEM_MULTI_GPU_MODE_PROTECTED_PCIE 1

typedef struct NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS {
    NvU8 cpuCapability;
    NvU8 gpusCapability;
    NvU8 environment;
    NvU8 ccFeature;
    NvU8 devToolsMode;
    NvU8 multiGpuMode;
} NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE
 *   This control call can be used to determine if all GPUs are ready to accept
 *   work form clients.
 *
 *      bAcceptClientRequest: [OUT]
 *          NV_TRUE: all gpus accepting client work requests
 *          NV_FALSE: all gpus blocking client work requests
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE (0xcb330104) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x4" */

typedef struct NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS {
    NvBool bAcceptClientRequest;
} NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE
 *   This control call can be used to set gpu state to accept client requests
 *   or to block client requests
 *   This is a PRIVILEGED control call and can be set via admin tools
 *
 *      bAcceptClientRequest:[IN]
 *          NV_TRUE: set all gpus state to accept client work requests
 *          NV_FALSE: set all gpus state to block client work requests
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 */
#define NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE (0xcb330105) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x5" */

typedef struct NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS {
    NvBool bAcceptClientRequest;
} NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE
 *   This control call returns protected and unprotected vidmem size
 *
 *      hSubDevice: [IN]
 *          subdevice handle for the gpu whose vidmem size is requested
 *      protectedMemSizeInKb: [OUT]
 *          total protected memory size in kB
 *      unprotectedMemSizeInKb: [OUT]
 *          total unprotected memory size in kB
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE (0xcb330106) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x6" */

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS {
    NvHandle hSubDevice;
    NV_DECLARE_ALIGNED(NvU64 protectedMemSizeInKb, 8);
    NV_DECLARE_ALIGNED(NvU64 unprotectedMemSizeInKb, 8);
} NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE
 *   This control call updates protected and unprotected vidmem size.
 *   All memory is protected if APM/HCC is enabled. User can override
 *   unprotectedMemSizeInKb and that will adjust protectedMemSizeInKb accordingly.
 *   This is a PRIVILEGED control call and can be set via tools like nvidia-smi.
 *   Vidmem size can be updated after driver load and before any client FB
 *   allocations are made.
 *
 *      hSubDevice: [IN]
 *          subdevice handle for the gpu whose vidmem size is requested
 *      protectedMemSizeInKb: [OUT]
 *          total protected memory size in kB
 *      unprotectedMemSizeInKb: [IN/OUT]
 *          total unprotected memory size in kB
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE (0xcb330107) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x7" */

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS {
    NvHandle hSubDevice;
    NV_DECLARE_ALIGNED(NvU64 protectedMemSizeInKb, 8);
    NV_DECLARE_ALIGNED(NvU64 unprotectedMemSizeInKb, 8);
} NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_GET_NUM_SUPPORTED_CC_SECURE_CHANNELS
 *   This control call returns the max number of AES capable channels SEC2 and CE support.
 *
 *      hSubDevice: [IN]
 *          subdevice handle for the GPU queried
 *      numSupportedSec2CCSecureChannels: [OUT]
 *          Max number of AES capable channels SEC2 supports
 *      numSupportedCeCCSecureChannels: [OUT]
 *          Max number of channels CE supports with encrypt/decrypt
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GET_NUM_SUPPORTED_CC_SECURE_CHANNELS (0xcb330108) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x8" */

#define NV_CONF_COMPUTE_CTRL_CMD_GET_NUM_SUPPORTED_CC_SECURE_CHANNELS_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GET_NUM_SUPPORTED_CC_SECURE_CHANNELS_PARAMS {
    NvHandle hSubDevice;
    NvU32    numSupportedSec2CCSecureChannels;
    NvU32    numSupportedCeCCSecureChannels;
} NV_CONF_COMPUTE_CTRL_CMD_GET_NUM_SUPPORTED_CC_SECURE_CHANNELS_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE
 *   This control call returns the GPU session certificate for the specified GPU.
 *   The certificate size is the maximum of the certificate size of APM and CC.
 *
 *      hSubDevice: [IN]
 *          Subdevice handle for the GPU queried
 *      certChain: [OUT]
 *          Certificate chain for the GPU queried
 *      certChainSize: [OUT]
 *          Actual size of certChain data
 *      attestationCertChain: [OUT]
 *          Attestation certificate chain for the GPU queried
 *      attestationCertChainSize: [OUT]
 *          Actual size of attestationCertChain data
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE    (0xcb330109) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0x9" */

#define NV_CONF_COMPUTE_CERT_CHAIN_MAX_SIZE             0x1000
#define NV_CONF_COMPUTE_ATTESTATION_CERT_CHAIN_MAX_SIZE 0x1400

#define NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS {
    NvHandle hSubDevice;
    NvU8     certChain[NV_CONF_COMPUTE_CERT_CHAIN_MAX_SIZE];
    NvU32    certChainSize;
    NvU8     attestationCertChain[NV_CONF_COMPUTE_ATTESTATION_CERT_CHAIN_MAX_SIZE];
    NvU32    attestationCertChainSize;
} NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS;

 /*
 * NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION
 *   This control call returns the GPU attestation report for the specified GPU.
 *   The attestation report size is the maximum of the attestation report size of APM and CC.
 *
 *      hSubDevice: [IN]
 *          Subdevice handle for the GPU queried
 *      nonce: [IN]
 *          spdm supports 32 bytes on nonce
 *      attestationReport: [OUT]
 *          Attestation report of the GPU queried
 *      attestationReportSize: [OUT]
 *          Actual size of the report
 *      isCecAttestationReportPresent : [OUT]
 *          Indicates if the next 2 feilds are valid
 *      cecAttestationReport: [OUT]
 *          Cec attestation report for the gpu queried
 *      cecAttestationReportSize: [OUT]
 *          Actual size of the cec attestation report
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT (0xcb33010a) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0xA" */

#define NV_CONF_COMPUTE_GPU_ATTESTATION_REPORT_MAX_SIZE     0x2000
#define NV_CONF_COMPUTE_GPU_CEC_ATTESTATION_REPORT_MAX_SIZE 0x1000
#define NV_CONF_COMPUTE_NONCE_SIZE                          0x20

#define NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS {
    NvHandle hSubDevice;
    NvU8     nonce[NV_CONF_COMPUTE_NONCE_SIZE];
    NvU8     attestationReport[NV_CONF_COMPUTE_GPU_ATTESTATION_REPORT_MAX_SIZE];
    NvU32    attestationReportSize;
    NvBool   isCecAttestationReportPresent;
    NvU8     cecAttestationReport[NV_CONF_COMPUTE_GPU_CEC_ATTESTATION_REPORT_MAX_SIZE];
    NvU32    cecAttestationReportSize;
} NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS
 *   This control call returns the max number of Conf Compute capable channels SEC2 and CE support.
 *
 *      hSubDevice: [IN]
 *          subdevice handle for the GPU queried
 *      maxSec2Channels: [OUT]
 *          Max number of conf compute capable channels SEC2 supports
 *      maxCeChannels: [OUT]
 *          Max number of channels CE supports with encrypt/decrypt
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS (0xcb33010b) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0xB" */

#define NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS {
    NvHandle hSubDevice;
    NvU32    maxSec2Channels;
    NvU32    maxCeChannels;
} NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE
 *   This control call returns if key rotation is enabled.
 *
 *      hSubDevice: [IN]
 *          subdevice handle for the GPU queried
 *      keyRotationState: [OUT]
 *          NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_* value
 *
 * Possible return values:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE    (0xcb33010c) /* finn: Evaluated from "(FINN_NV_CONFIDENTIAL_COMPUTE_CONF_COMPUTE_INTERFACE_ID << 8) | 0xC" */

#define NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_DISABLED     0       // key rotation is disabled
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_KERN_ENABLED 1       // key rotation enabled for kernel keys
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_USER_ENABLED 2       // key rotation enabled for user keys
#define NV_CONF_COMPUTE_CTRL_CMD_GPU_KEY_ROTATION_BOTH_ENABLED 3       // key rotation enabled for both keys

#define NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE_PARAMS {
    NvHandle hSubDevice;
    NvU32    keyRotationState;
} NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE_PARAMS;

/* _ctrlcb33_h_ */

