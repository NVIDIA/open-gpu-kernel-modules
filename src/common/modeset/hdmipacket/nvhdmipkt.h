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
 *
 * File:      nvhdmipkt.h
 *
 * Purpose:   This file is a common header for all HDMI Library Clients
 */

#ifndef _NVHDMIPKT_H_
#define _NVHDMIPKT_H_

#include <nvlimits.h>

#include "nvmisc.h"


#ifdef __cplusplus
extern "C" {
#endif

/**************************** HDMI Library defines, enums and structs ***************************/
/************************************************************************************************
 * NOTE: NVHDMIPKT_RM_CALLS_INTERNAL define tells this library to make RM calls (allocate, free *
 *       control, etc.) internally and not through callbacks into the client.                   *
 ************************************************************************************************/
#if !defined(NVHDMIPKT_RM_CALLS_INTERNAL)
#  define NVHDMIPKT_RM_CALLS_INTERNAL 1
#endif

// NVHDMIPKT_RESULT: HDMI library return result enums
typedef enum
{
    NVHDMIPKT_SUCCESS                = 0,
    NVHDMIPKT_FAIL                   = 1,
    NVHDMIPKT_LIBRARY_INIT_FAIL      = 2,
    NVHDMIPKT_INVALID_ARG            = 3,
    NVHDMIPKT_TIMEOUT                = 4,
    NVHDMIPKT_ERR_GENERAL            = 5,
    NVHDMIPKT_INSUFFICIENT_BANDWIDTH = 6,
    NVHDMIPKT_RETRY                  = 7
} NVHDMIPKT_RESULT;

// NVHDMIPKT_TYPE: HDMI Packet Enums
typedef enum _NVHDMIPKT_TYPE
{
    NVHDMIPKT_TYPE_UNDEFINED                  = 0,  // Undefined Packet Type
    NVHDMIPKT_TYPE_GENERIC                    = 1,  // Generic packet, any Generic Packet
                                                    // (e.g Gamut Metadata packet)
    NVHDMIPKT_TYPE_AVI_INFOFRAME              = 2,  // Avi infoframe
    NVHDMIPKT_TYPE_GENERAL_CONTROL            = 3,  // GCP
    NVHDMIPKT_TYPE_VENDOR_SPECIFIC_INFOFRAME  = 4,  // VSI
    NVHDMIPKT_TYPE_AUDIO_INFOFRAME            = 5,  // Audio InfoFrame
    NVHDMIPKT_TYPE_EXTENDED_METADATA_PACKET   = 6,  // Extended Metadata Packet (HDMI 2.1)
    NVHDMIPKT_INVALID_PKT_TYPE                = 13
} NVHDMIPKT_TYPE;

// Hdmi packet TransmitControl defines. These definitions reflect the
// defines from ctrl and class defines for info frames.
#define NV_HDMI_PKT_TRANSMIT_CTRL_ENABLE                0:0
#define NV_HDMI_PKT_TRANSMIT_CTRL_ENABLE_DIS            0x00000000
#define NV_HDMI_PKT_TRANSMIT_CTRL_ENABLE_EN             0x00000001

#define NV_HDMI_PKT_TRANSMIT_CTRL_OTHER                 1:1
#define NV_HDMI_PKT_TRANSMIT_CTRL_OTHER_DIS             0x00000000
#define NV_HDMI_PKT_TRANSMIT_CTRL_OTHER_EN              0x00000001

#define NV_HDMI_PKT_TRANSMIT_CTRL_SINGLE                2:2
#define NV_HDMI_PKT_TRANSMIT_CTRL_SINGLE_DIS            0x00000000
#define NV_HDMI_PKT_TRANSMIT_CTRL_SINGLE_EN             0x00000001

#define NV_HDMI_PKT_TRANSMIT_CTRL_CHKSUM_HW             3:3
#define NV_HDMI_PKT_TRANSMIT_CTRL_CHKSUM_HW_DIS         0x00000000
#define NV_HDMI_PKT_TRANSMIT_CTRL_CHKSUM_HW_EN          0x00000001

#define NV_HDMI_PKT_TRANSMIT_CTRL_HBLANK                4:4
#define NV_HDMI_PKT_TRANSMIT_CTRL_HBLANK_DIS            0x00000000
#define NV_HDMI_PKT_TRANSMIT_CTRL_HBLANK_EN             0x00000001

#define NV_HDMI_PKT_TRANSMIT_CTRL_VIDEO_FMT             5:5
#define NV_HDMI_PKT_TRANSMIT_CTRL_VIDEO_FMT_SW_CTRL     0x00000000
#define NV_HDMI_PKT_TRANSMIT_CTRL_VIDEO_FMT_HW_CTRL     0x00000001

// NVHDMIPKT_TC: HDMI Packet Transmit Control
// NOTE: Client should use these defines below for transmit control, and avoid using the ones
//       above. Use only if client knows and wants fine control. And in that case the value
//       passed has to be explicitly typecasted to NVHDMIPKT_TC by the client.
typedef enum _NVHDMIPKT_TC
{
    NVHDMIPKT_TRANSMIT_CONTROL_DISABLE                     =
    (DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE,    _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _OTHER,     _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE,    _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _DIS)),

    NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_EVERY_FRAME          =
    (DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE,    _EN)   |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _OTHER,     _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE,    _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _EN)),

    NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_SINGLE_FRAME         =
    (DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE,    _EN)   |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _OTHER,     _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE,    _EN)   |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _EN)),

    NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_EVERY_OTHER_FRAME    =
    (DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE,    _EN)   |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _OTHER,     _EN)   |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE,    _DIS)  |
     DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _EN)),

    NVHDMIPKT_TRANSMIT_CONTROL_VIDEO_FMT_HW_CTRL           =
    (DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _VIDEO_FMT, _HW_CTRL)),

} NVHDMIPKT_TC;

// RM client handles. Used when client chooses that hdmi library make RM calls.
// NOTE: NVHDMIPKT_RM_CALLS_INTERNAL macro should be define to use it.
typedef struct tagNVHDMIPKT_RM_CLIENT_HANDLES
{
    NvU32 hClient;
    NvU32 hDevice;
    NvU32 hSubDevices[NV_MAX_SUBDEVICES];
    NvU32 hDisplay;
} NVHDMIPKT_RM_CLIENT_HANDLES;

/****************************** HDMI Library callbacks into client ******************************/
typedef void* NvHdmiPkt_CBHandle;

/************************************************************************************************
 * [rmGetMemoryMap, rmFreeMemoryMap, rmDispControl,] acquireMutex and releaseMutex are mandatory*
 * callbacks, to be implemented by the client. Callbacks in [] above are mandatory only for     *
 * Windows.                                                                                     *
 * Linux need not implement those, if they plan to use NVHDMIPKT_RM_CALLS_INTERNAL define.      *
 *                                                                                              *
 * rmGetMemoryMap and rmFreeMemoryMap are RM calls to allocate the DISP_SF_USER class.          *
 * And mutex callbacks keep hemi packet operations atomic.                                      *
 ************************************************************************************************/
typedef struct _tagNVHDMIPKT_CALLBACK
{
    // MANDATORY callbacks.
    NvBool
    (*rmGetMemoryMap)  (NvHdmiPkt_CBHandle handle,
                        NvU32              dispSfUserClassId,
                        NvU32              dispSfUserSize,
                        NvU32              subDevice,
                        NvU32*             pMemHandle,
                        void**             ppBaseMem);

    void
    (*rmFreeMemoryMap) (NvHdmiPkt_CBHandle handle,
                        NvU32              subDevice,
                        NvU32              memHandle,
                        void*              pMem);

    NvBool
    (*rmDispControl2)   (NvHdmiPkt_CBHandle handle,
                         NvU32              subDevice,
                         NvU32              cmd,
                         void*              pParams,
                         NvU32              paramSize);


    void
    (*acquireMutex)    (NvHdmiPkt_CBHandle handle);

    void
    (*releaseMutex)    (NvHdmiPkt_CBHandle handle);

    // OPTIONAL callbacks
    /* time in microseconds (us) */
    NvBool
    (*setTimeout)      (NvHdmiPkt_CBHandle handle,
                        NvU32              us_timeout);

    /* ChecTimeout returns true when timer times out */
    NvBool
    (*checkTimeout)    (NvHdmiPkt_CBHandle handle);

    // callbacks to allocate memory on heap to reduce stack usage
    void*
    (*malloc)          (NvHdmiPkt_CBHandle handle,
                        NvLength           numBytes);

    void
    (*free)            (NvHdmiPkt_CBHandle handle,
                        void              *pMem);

    void
    (*print)           (NvHdmiPkt_CBHandle handle,
                        const char*        fmtstring,
                        ...)
#if defined(__GNUC__)
    __attribute__ ((format (printf, 2, 3)))
#endif
                        ;

    void
    (*assert)          (NvHdmiPkt_CBHandle handle,
                        NvBool             expression);
} NVHDMIPKT_CALLBACK;

/*********************** HDMI Library interface to write hdmi ctrl/packet ***********************/
typedef void* NvHdmiPkt_Handle;
#define NVHDMIPKT_INVALID_HANDLE ((NvHdmiPkt_Handle)0)

/************************************************************************************************
 * NvHdmiPkt_PacketCtrl - Returns HDMI NVHDMIPKT_RESULT.                                        *
 *                                                                                              *
 * Parameters:                                                                                  *
 * libHandle - Hdmi library handle, provided on initializing the library.                       *
 * subDevice - Sub Device ID.                                                                   *
 * displayId - Display ID.                                                                      *
 * head      - Head number.                                                                     *
 * packetType      - One of the NVHDMIPKT_TYPE types.                                           *
 * transmitControl - Packet transmit control setting.                                           *
 ************************************************************************************************/
NVHDMIPKT_RESULT
NvHdmiPkt_PacketCtrl (NvHdmiPkt_Handle   libHandle,
                      NvU32              subDevice,
                      NvU32              displayId,
                      NvU32              head,
                      NVHDMIPKT_TYPE     packetType,
                      NVHDMIPKT_TC       transmitControl);

/************************************************************************************************
 * NvHdmiPkt_PacketWrite - Returns HDMI NVHDMIPKT_RESULT.                                       *
 *                                                                                              *
 * Parameters:                                                                                  *
 * libHandle - Hdmi library handle, provided on initializing the library.                       *
 * subDevice - Sub Device ID.                                                                   *
 * displayId - Display ID.                                                                      *
 * head      - Head number.                                                                     *
 * packetType      - One of the NVHDMIPKT_TYPE types.                                           *
 * transmitControl - Packet transmit control setting.                                           *
 * packetLen       - Length of the packet in bytes to be transmitted.                           *
 * pPacket         - Pointer to packet data.                                                    *
 ************************************************************************************************/
NVHDMIPKT_RESULT
NvHdmiPkt_PacketWrite(NvHdmiPkt_Handle   libHandle,
                      NvU32              subDevice,
                      NvU32              displayId,
                      NvU32              head,
                      NVHDMIPKT_TYPE     packetType,
                      NVHDMIPKT_TC       transmitControl,
                      NvU32              packetLen,
                      NvU8 const *const  pPacket);

/***************************** Interface to initialize HDMI Library *****************************/

/************************************************************************************************
 * NvHdmiPkt_InitializeLibrary - Returns NvHdmiPkt_Handle. This handle is used to call          *
 *                               library interfaces. If handle returned is invalid -            *
 *                               NVHDMIPKT_INVALID_HANDLE -, there was a problem in             *
 *                               initialization and the library won't work.                     *
 *                                                                                              *
 * Parameters:                                                                                  *
 * hwClass          - Depending on HW, apply display class or display dma class. Either will do.*
 *                    Eg. for GK104- NV9170_DISPLAY or NV917D_CORE_CHANNEL_DMA.                 *
 * numSubDevices    - Number of sub devices.                                                    *
 *                                                                                              *
 * cbHandle         - Callback handle. Client cookie for callbacks made to client.              *
 * pCallback        - Callbacks. Struct NVHDMIPKT_CALLBACK.                                     *
 *                                                                                              *
 * Below mentioned sfUserHandle and clientHandles parameters are used only when not providing   *
 * rmGetMemoryMap, rmFreeMemoryMap and rmDispControl callbacks. This is meant for Linux.        *
 * And is controlled by NVHDMIPKT_RM_CALLS_INTERNAL macro.                                      *
 * NOTE: And Clients not using NVHDMIPKT_RM_CALLS_INTERNAL, need to set both sfUserHandle and   *
 *       clientHandles to 0.                                                                    *
 *                                                                                              *
 * sfUserHandle     - SF_USER handle; this is the base handle. Subsequent subdevice handles are *
 *                    derived incrementally from this handle.                                   *
 * pClientHandles   - RM handles for client, device, subdevices and displayCommon.              *
 *                                                                                              *
 ************************************************************************************************/
NvHdmiPkt_Handle
NvHdmiPkt_InitializeLibrary(NvU32                              const hwClass,
                            NvU32                              const numSubDevices,
                            NvHdmiPkt_CBHandle                 const cbHandle,
                            const NVHDMIPKT_CALLBACK*          const pCallback,
                            NvU32                              const sfUserHandle,
                            const NVHDMIPKT_RM_CLIENT_HANDLES* const pClientHandles);

/************************************************************************************************
 * NvHdmiPkt_DestroyLibrary                                                                     *
 *                                                                                              *
 * When done with the HDMI Library call NvHdmiPkt_DestroyLibrary. It is like a destructor.      *
 * This destructor frees up resources acquired during initialize.                               *
 *                                                                                              *
 ************************************************************************************************/
void
NvHdmiPkt_DestroyLibrary(NvHdmiPkt_Handle libHandle);

#ifdef __cplusplus
}
#endif
#endif // _NVHDMIPKT_H_
