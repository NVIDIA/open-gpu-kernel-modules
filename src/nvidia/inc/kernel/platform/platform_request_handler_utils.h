/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _PLATFORM_REQUEST_HANDLER_UTILS_H_
#define _PLATFORM_REQUEST_HANDLER_UTILS_H_

//_PCONTROL
//_PCONTROL response from driver to SBIOS
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RES                               3:0
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RES_CHANGE_EVENT                   (0)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RES_VPSTATE_INFO                   (1)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RES_VPSTATE_SET                    (2)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RES_VPSTATE_UPDATE                 (3)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED                          7:4
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED_BITS                      (0)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_LAST_VPSTATE_LIMIT               15:8
#define NV_PB_PFM_REQ_HNDLR_PCTRL_INDEX_PSTATE                     15:8
#define NV_PB_PFM_REQ_HNDLR_PCTRL_SLOW_EXT_VPSTATE                 23:16
#define NV_PB_PFM_REQ_HNDLR_PCTRL_FAST_VPSTATE                     31:24
#define NV_PB_PFM_REQ_HNDLR_PCTRL_MAPPING_VPSTATE                  31:24

//_PCONTROL request from SBIOS to driver
#define NV_PB_PFM_REQ_HNDLR_PCTRL_REQ                               3:0
#define NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_NO_ACTION                      (0)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_VPSTATE_INFO                   (1)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_VPSTATE_SET                    (2)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_VPSTATE_UPDATE                 (3)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED1                         7:4
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED1_BITS                     (0)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_INDEX_PSTATE                     15:8
#define NV_PB_PFM_REQ_HNDLR_PCTRL_MAX_VPSTATE_LEVEL                15:8
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED2                        30:16
#define NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED2_BITS                     (0)
#define NV_PB_PFM_REQ_HNDLR_PCTRL_BIT_31                           31:31
#define NV_PB_PFM_REQ_HNDLR_PCTRL_BIT_31_ZERO                        (0)

/*
 * NV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO
 *
 * GPU EDPpeak Limit information for platform
 *
 *    ulVersion
 *     (Major(16 bits):Minor(16 bits), current v1.0)
 *     Little endian format 0x00, 0x00, 0x01, 0x00
 *    limitLast
 *     last requested platform limit
 *    limitMin
 *     Minimum allowed limit value on EDPp policy on both AC and DC
 *    limitRated
 *      Rated/default allowed limit value on EDPp policy on AC
 *    limitMax
 *     Maximum allowed limit value on EDPp policy on AC
 *    limitCurr
 *     Current resultant limit effective on EDPp policy on AC and DC
 *    limitBattRated
 *     Default/rated allowed limit on EDPp policy on DC
 *    limitBattMax
 *     Maximum allowed limit on EDPp policy on DC
 *    rsvd
 *      Reserved
 */
typedef struct NV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO_V1 {
    NvU32 ulVersion;
    NvU32 limitLast;
    NvU32 limitMin;
    NvU32 limitRated;
    NvU32 limitMax;
    NvU32 limitCurr;
    NvU32 limitBattRated;
    NvU32 limitBattMax;
    NvU32 rsvd;
} NV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO_V1, *PNV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO_V1;

/*
 * NV0000_CTRL_PFM_REQ_HNDLR_PSHAREDATA
 *
 * This structure represents a block of PSHARE data from SBIOS
 *
 *   status
 *     settings per spec
 *   ulVersion
 *     (Major(16 bits):Minor(16 bits), current v1.0)
 *     Little endian format 0x00, 0x00, 0x01, 0x00
 *   tGpu
 *     GPU temperature (not provided by system)
 *
 *    ctgp
 *    Configurable TGP limit
 */
typedef struct NV0000_CTRL_PFM_REQ_HNDLR_PSHAREDATA {
// Header to sensor structure
    NvU32   status;
    NvU32   ulVersion;

    NvU32   tGpu;
    NvU32   ctgp;
} NV0000_CTRL_PFM_REQ_HNDLR_PSHAREDATA, *PNV0000_CTRL_PFM_REQ_HNDLR_PSHAREDATA;

/*
 * Begin defines for access to ACPI calls, these
 *    are used in the RM, so we'd like to get them
 *    in, even though the CTRL call is not ready.
 * NV0000_CTRL_CMD_CALL_PFM_REQ_HNDLR_ACPI
 *
 * This command is used to send ACPI commands for PlatformRequestHandler compliant SBIOS
 *   to the RM to be executed by system BIOS.  Results of those SBIOS
 *   command are returned through this interface.
 *
 * cmd
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_TYPE
 *   Gets system configuration Information.
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SUPPORT
 *   Gets bit mask of supported functions
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PCONTROL
 *   Sets GPU power control features.
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHARESTATUS
 *   Gets System PShare Status
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETPPL
 *   Execute ACPI GETPPL command.
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETPPL
 *   Execute ACPI SETPPL command.
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHAREPARAMS
 *   Get sensor information and capabilities.
 * input
 *   Used for single DWORD (32 bit) values as input to
 *   the requested ACPI call.
 *
 */
typedef struct _NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CTRL
{
    NvU32   cmd;
    NvU32   input;
} NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CTRL;

/*
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_OUTPUT
 *
 * This structure represents the output from a single ACPI call.
 *
 *   result
 *     The ACPI return code for the operation attempted.
 *   pBuffer
 *     This field returns a pointer the buffer of data requested.
  *   bufferSz
 *     This field returns the size of returned data in above pBuffer.
 */
typedef struct _NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_OUTPUT
{
    NvU32   result[2];
    NvU8   *pBuffer;
    NvU16   bufferSz;
} NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_OUTPUT;


/*
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS
 *
 * This structure represents a parameter block of data to describe an
 *   ACPI request, and return it's output.
 *
 * ctrl
 *     The ACPI request for the operation attempted.
  *   output
 *     Output structure from the executed ACPI command.
 */
typedef struct _NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS
{
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CTRL      ctrl;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_OUTPUT    output;
}   NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS;

/*
 * NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX
 *
 * Used for PPL and TRL calls, which have up to three input DWORDs and three
 * output DWORDs.
 */
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX_MAX_SZ                               (3U)

typedef struct _NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX
{
    NvU32   pfmreqhndlrFunc;
    NvU16   inSize;
    NvU16   outSize;
    NvU32   input[NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX_MAX_SZ];
    NvU32   output[NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX_MAX_SZ];
}   NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX;

#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_TYPE                                        8:0
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SUPPORT                      (GPS_FUNC_SUPPORT)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PCONTROL                    (GPS_FUNC_PCONTROL)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHARESTATUS            (GPS_FUNC_PSHARESTATUS)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETPPL                        (GPS_FUNC_GETPPL)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETPPL                        (GPS_FUNC_SETPPL)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETTRL                        (GPS_FUNC_GETTRL)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETTRL                        (GPS_FUNC_SETTRL)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETPPM                        (GPS_FUNC_GETPPM)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETPPM                        (GPS_FUNC_SETPPM)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHAREPARAMS            (GPS_FUNC_PSHAREPARAMS)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETEDPPLIMITINFO    (GPS_FUNC_SETEDPPLIMITINFO)
#define NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETEDPPLIMIT            (GPS_FUNC_GETEDPPLIMIT)

// PFM_REQ_HNDLR_SUPPORT output
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_SUPPORT_AVAIL                                   0:0
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_PCONTROL_AVAIL                                28:28
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_PSHARESTATUS_AVAIL                            32:32
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_GETPPL_AVAIL                                  36:36
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_SETPPL_AVAIL                                  37:37
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_GETTRL_AVAIL                                  38:38
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_SETTRL_AVAIL                                  39:39
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_GETPPM_AVAIL                                  40:40
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_SETPPM_AVAIL                                  41:41
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_PSHAREPARAMS_AVAIL                            42:42
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_INFOEDPPLIMIT_AVAIL                           43:43
#define NV0000_CTRL_PFM_REQ_HNDLR_SUPPORTED_SETEDPPLIMIT_AVAIL                            44:44

// PFM_REQ_HNDLR_PCONTROL
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_REQ_TYPE                                         3:0
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_REQ_TYPE_EVENT_RESP                           (0x00)   // input only
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_REQ_TYPE_EVENT_DONE                           (0x00)   // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_REQ_TYPE_VPSTATE_INFO                         (0x01)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_REQ_TYPE_NEW_VP_STATE                         (0x02)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_REQ_TYPE_UPDATE_VP_STATE                      (0x03)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_VPSTATE_LIMIT                                   15:8   // input request type 0, 2, or 3; output request type 2
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_PSTATE_IDX                                      15:8   // input request type 1;          output request type 1
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_SLOWEST_VPSTATE                                23:16   // input only
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_FASTEST_VPSTATE                                31:24   // input request type 0, 2, or 3
#define NV0000_CTRL_PFM_REQ_HNDLR_PCONTROL_VPSTATE_MAPPING                                31:24   // input request type 1

// PFM_REQ_HNDLR_PSHARESTATUS
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_UPDATE_LIMIT                                 0:0   // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_UPDATE_LIMIT_NOT_PENDING                     (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_UPDATE_LIMIT_PENDING                         (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_DO_NOT_USE                                  19:1
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLAT_USER_CONFIG_TGP_MODE_SUPPORT          20:20    // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLAT_USER_CONFIG_TGP_MODE_SUPPORT_DISABLE    (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLAT_USER_CONFIG_TGP_MODE_SUPPORT_ENABLE     (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_EDPPEAK_LIMIT_UPDATE                       21:21    // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_EDPPEAK_LIMIT_UPDATE_FALSE                   (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_EDPPEAK_LIMIT_UPDATE_TRUE                    (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_USER_CONFIG_TGP_MODE                       22:22    // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_USER_CONFIG_TGP_MODE_DISABLE                 (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_USER_CONFIG_TGP_MODE_ENABLE                  (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLATFORM_GETEDPPEAKLIMIT_SET               25:25    // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLATFORM_GETEDPPEAKLIMIT_SET_FALSE          (0U)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLATFORM_GETEDPPEAKLIMIT_SET_TRUE           (1U)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLATFORM_SETEDPPEAKLIMITINFO_SET           26:26    // output only
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLATFORM_SETEDPPEAKLIMITINFO_SET_FALSE      (0U)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARESTATUS_PLATFORM_SETEDPPEAKLIMITINFO_SET_TRUE       (1U)

// Shared by GETPPL, SETPPL
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_COUNT                                            (3)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_VERSION_IDX                                      (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARG0_VERSION_MINOR                                   15:0   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARG0_VERSION_MAJOR                                  31:16   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARG0_VERSION_MAJOR_V1                                 (1)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_LIMIT1_IDX                                       (1)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_LIMIT2_IDX                                       (2)   // input & output

// Shared by GETTRL, SETTRL
#define NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_COUNT                                            (2)
#define NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_VERSION_IDX                                      (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARG0_VERSION_MINOR                                   15:0   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARG0_VERSION_MAJOR                                  31:16   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARG0_VERSION_MAJOR_V1                                 (1)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_FREQ_MHZ_IDX                                     (1)   // input & output

// Shared by GETPPM, SETPPM
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_COUNT                                            (2)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_VERSION_IDX                                      (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARG0_VERSION_MINOR                                   15:0   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARG0_VERSION_MAJOR                                  31:16   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARG0_VERSION_MAJOR_V1                                 (1)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_IDX                                              (1)   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_INDEX                                            7:0   // output
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_AVAILABLE_MASK                                  15:8   // output

// Shared by INFOEDPPLIMIT and SETEDPPLIMIT
#define NV0000_CTRL_PFM_REQ_HNDLR_EDPP_VERSION_V10                                   (0x10000U)   // input & output

//
// PFM_REQ_HNDLR_PSHARE_PARAMS
// status bits
//
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_QUERY_TYPE                           3:0   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_QUERY_TYPE_CURRENT_INFO           (0x00)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_QUERY_TYPE_SUPPORTED_FIELDS       (0x01)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_QUERY_TYPE_CURRENT_LIMITS         (0x02)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_TGPU                                 8:8   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_TGPU_FALSE                           (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_TGPU_TRUE                            (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_DO_NOT_USE                          14:9   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_CTGP                               15:15   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_CTGP_FALSE                           (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_CTGP_TRUE                            (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_PPMD                               16:16   // input & output
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_PPMD_FALSE                           (0)
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_PPMD_TRUE                            (1)

//
// A mapping from ACPI DSM and SW support dsm version.
// The SW might emulate 1x based on other parameters
//
#define NV0000_CTRL_PFM_REQ_HNDLR_ACPI_REVISION_SW_1X                              (0x00000100)
#define NV0000_CTRL_PFM_REQ_HNDLR_ACPI_REVISION_SW_2X                              (0x00000200)


#endif // _PLATFORM_REQUEST_HANDLER_UTILS_H_

