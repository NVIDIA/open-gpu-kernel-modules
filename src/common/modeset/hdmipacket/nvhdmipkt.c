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
 * File:      nvhdmipkt.c
 *
 * Purpose:   Provide initialization functions for HDMI library
 */

#include <stddef.h>
#include "nvlimits.h"
#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"
#include "nvhdmipkt_internal.h"

#include "../timing/nvt_dsc_pps.h"

#include "class/cl9170.h"
#include "class/cl917d.h"
#include "class/cl9270.h"
#include "class/cl927d.h"
#include "class/cl9470.h"
#include "class/cl947d.h"
#include "class/cl9570.h"
#include "class/cl957d.h"
#include "class/clc370.h"
#include "class/clc37d.h"
#include "class/clc570.h"
#include "class/clc57d.h"
#include "class/clc670.h"
#include "class/clc67d.h"
#include "class/clc770.h"

#include "hdmi_spec.h"

// Class hierarchy structure
typedef struct tagNVHDMIPKT_CLASS_HIERARCHY
{
    NVHDMIPKT_CLASS_ID classId;
    NVHDMIPKT_CLASS_ID parentClassId;
    NvBool             isRootClass;
    void               (*initInterface)(NVHDMIPKT_CLASS*);
    NvBool             (*constructor)  (NVHDMIPKT_CLASS*);
    void               (*destructor)   (NVHDMIPKT_CLASS*);
    NvU32              displayClass;
    NvU32              coreDmaClass;
} NVHDMIPKT_CLASS_HIERARCHY;

/*************************************************************************************************
 *              hierarchy structure establishes the relationship between classes.                *
 *   If isRootClass=NV_TRUE, it is a root class, else it is a child of a class. classId          *
 *   also acts as an index, and hence the order of the structure below should be maintanied.     *
 *                                                                                               *
 *   ASSUMPTION: There are two huge assumptions while creating the class relationship and        *
 *   while traversing it. 1. That of the Class ID definitaion (NVHDMIPKT_CLASS_ID), which has    *
 *   to be strictly indexed, that is 0, 1, 2... and so on. And 2. that the structure             *
 *   CLASS_HIERARCHY (above) follow that indexing. That is NVHDMIPKT_0073_CLASS is value 0 and   *
 *   the first entry in CLASS_HIERARCHY, NVHDMIPKT_9171_CLASS is value 1 and hence the second    *
 *   entry in CLASS_HIERARCHY, so on and so forth.                                               *
 *                                                                                               *
 *   HOW TO ADD A NEW CLASS?                                                                     *
 *   1. Add an ID in NVHDMIPKT_CLASS_ID.                                                         *
 *   2. Add a source file nvhdmipkt_XXXX.c, and include it into makefiles. Makefiles of          *
 *      Mods, Windows, and Linux.                                                                *
 *   3. Provide initializeHdmiPktInterfaceXXXX, hdmiConstructorXXXX, and, hdmiDestructorXXXX.    *
 *   4. Add functions that needs to be overridden in  NVHDMIPKT_CLASS.                           *
 *   5. Add a relationship in hierarchy[] array. The new class can be a subclass or a root. In   *
 *      case of a root all the interfaces needs to be overridden in NVHDMIPKT_CLASS.             *
 ************************************************************************************************/
static const NVHDMIPKT_CLASS_HIERARCHY hierarchy[] =
{
    [NVHDMIPKT_0073_CLASS] = {// Index 0==NVHDMIPKT_0073_CLASS
        NVHDMIPKT_0073_CLASS,             // classId
        NVHDMIPKT_0073_CLASS,             // parentClassId
        NV_TRUE,                          // isRootClass
        initializeHdmiPktInterface0073,   // initInterface
        hdmiConstructor0073,              // constructor
        hdmiDestructor0073,               // destructor
        0,                                // displayClass
        0                                 // coreDmaClass
    },
    [NVHDMIPKT_9171_CLASS] = {// Index 1==NVHDMIPKT_9171_CLASS
        NVHDMIPKT_9171_CLASS,             // classId
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_TRUE,                          // isRootClass
        initializeHdmiPktInterface9171,   // initInterface
        hdmiConstructor9171,              // constructor
        hdmiDestructor9171,               // destructor
        NV9170_DISPLAY,                   // displayClass
        NV917D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_9271_CLASS] = {// Index 2==NVHDMIPKT_9271_CLASS
        NVHDMIPKT_9271_CLASS,             // classId
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterface9271,   // initInterface
        hdmiConstructor9271,              // constructor
        hdmiDestructor9271,               // destructor
        NV9270_DISPLAY,                   // displayClass
        NV927D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_9471_CLASS] = {// Index 3==NVHDMIPKT_9471_CLASS
        NVHDMIPKT_9471_CLASS,             // classId
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterface9471,   // initInterface
        hdmiConstructor9471,              // constructor
        hdmiDestructor9471,               // destructor
        NV9470_DISPLAY,                   // displayClass
        NV947D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_9571_CLASS] = {// Index 4==NVHDMIPKT_9571_CLASS
        NVHDMIPKT_9571_CLASS,             // classId
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterface9571,   // initInterface
        hdmiConstructor9571,              // constructor
        hdmiDestructor9571,               // destructor
        NV9570_DISPLAY,                   // displayClass
        NV957D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_C371_CLASS] = {// Index 5==NVHDMIPKT_C371_CLASS
        NVHDMIPKT_C371_CLASS,             // classId
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterfaceC371,   // initInterface
        hdmiConstructorC371,              // constructor
        hdmiDestructorC371,               // destructor
        NVC370_DISPLAY,                   // displayClass
        NVC37D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_C571_CLASS] = {// Index 6==NVHDMIPKT_C571_CLASS
     // Note that Turing (C57x) has a distinct displayClass and coreDmaClass,
     // but it inherits the _DISP_SF_USER class from Volta (C37x).  We call this
     // NVHDMIPKT_C571_CLASS, but reuse initInterface()/constructor()/destructor()
     // from C371.
        NVHDMIPKT_C571_CLASS,
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterfaceC371,   // initInterface
        hdmiConstructorC371,              // constructor
        hdmiDestructorC371,               // destructor
        NVC570_DISPLAY,                   // displayClass
        NVC57D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_C671_CLASS] = {// Index 7==NVHDMIPKT_C671_CLASS
        NVHDMIPKT_C671_CLASS,             // classId
        NVHDMIPKT_9171_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterfaceC671,   // initInterface
        hdmiConstructorC671,              // constructor
        hdmiDestructorC671,               // destructor
        NVC670_DISPLAY,                   // displayClass
        NVC67D_CORE_CHANNEL_DMA           // coreDmaClass
    },
    [NVHDMIPKT_C771_CLASS] = {// Index 8==NVHDMIPKT_C771_CLASS
        NVHDMIPKT_C771_CLASS,             // classId
        NVHDMIPKT_C671_CLASS,             // parentClassId
        NV_FALSE,                         // isRootClass
        initializeHdmiPktInterfaceC771,   // initInterface
        hdmiConstructorC771,              // constructor
        hdmiDestructorC771,               // destructor
        NVC770_DISPLAY,                   // displayClass
        NVC67D_CORE_CHANNEL_DMA           // coreDmaClass
    },
};

/********************************** HDMI Library interfaces *************************************/
/*
 * NvHdmiPkt_PacketCtrl
 */
NVHDMIPKT_RESULT
NvHdmiPkt_PacketCtrl(NvHdmiPkt_Handle  libHandle,
                     NvU32             subDevice,
                     NvU32             displayId,
                     NvU32             head,
                     NVHDMIPKT_TYPE    packetType,
                     NVHDMIPKT_TC      transmitControl)
{
    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);

    if (libHandle == NVHDMIPKT_INVALID_HANDLE)
    {
        return NVHDMIPKT_LIBRARY_INIT_FAIL;
    }

    return pClass->hdmiPacketCtrl(pClass,
                                  subDevice,
                                  displayId,
                                  head,
                                  packetType,
                                  transmitControl);
}

/*
 * NvHdmiPkt_PacketWrite
 */
NVHDMIPKT_RESULT
NvHdmiPkt_PacketWrite(NvHdmiPkt_Handle  libHandle,
                      NvU32             subDevice,
                      NvU32             displayId,
                      NvU32             head,
                      NVHDMIPKT_TYPE    packetType,
                      NVHDMIPKT_TC      transmitControl,
                      NvU32             packetLen,
                      NvU8 const *const pPacket)
{
    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);

    if (libHandle == NVHDMIPKT_INVALID_HANDLE)
    {
        return NVHDMIPKT_LIBRARY_INIT_FAIL;
    }

    if ((pPacket == NULL) || (packetLen == 0))
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    HDMI_PACKET_TYPE infoframeType = pPacket[0]; // header byte 0 is packet type
    // Lower bound check. Since actual infoframe size varies depending on the infoframe packet being sent, 
    // check all supported infoframe types and their expected sizes. This is not a strict == check becuase they may/may not need
    // additional checksum byte (library clients take care of adding checksum byte if needed)
    if (((infoframeType == hdmi_pktType_GeneralControl)                 && (packetLen < 6))                                             ||
        ((infoframeType == hdmi_pktType_GamutMetadata)                  && (packetLen < sizeof(NVT_GAMUT_METADATA)))                    ||
        ((infoframeType == hdmi_pktType_ExtendedMetadata)               && (packetLen < sizeof(NVT_EXTENDED_METADATA_PACKET_INFOFRAME)))||
        ((infoframeType == hdmi_pktType_VendorSpecInfoFrame)            && (packetLen < 8))                                             ||
        ((infoframeType == hdmi_pktType_AviInfoFrame)                   && (packetLen < 13))                                            ||
        ((infoframeType == hdmi_pktType_SrcProdDescInfoFrame)           && (packetLen < sizeof(NVT_SPD_INFOFRAME)))                     ||
        ((infoframeType == hdmi_pktType_DynamicRangeMasteringInfoFrame) && (packetLen < sizeof(NVT_HDR_INFOFRAME))))
        //  Unused: hdmi_pktType_AudioClkRegeneration
        //  Unused: hdmi_pktType_MpegSrcInfoFrame
    {
        NvHdmiPkt_Print(pClass, "WARNING - packet length too small for infoframe type %d check payload ", infoframeType);
    }

    return pClass->hdmiPacketWrite(pClass,
                                   subDevice,
                                   displayId,
                                   head,
                                   packetType,
                                   transmitControl,
                                   packetLen,
                                   pPacket);
}

NVHDMIPKT_RESULT
NvHdmi_AssessLinkCapabilities(NvHdmiPkt_Handle             libHandle,
                              NvU32                        subDevice,
                              NvU32                        displayId,
                              NVT_EDID_INFO         const * const pSinkEdid,
                              HDMI_SRC_CAPS               *pSrcCaps,
                              HDMI_SINK_CAPS              *pSinkCaps)
{
    if (libHandle == NVHDMIPKT_INVALID_HANDLE)
    {
        return NVHDMIPKT_LIBRARY_INIT_FAIL;
    }

    if (!pSinkEdid ||
        !pSrcCaps  ||
        !pSinkCaps)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);
    return pClass->hdmiAssessLinkCapabilities(pClass,
                                              subDevice,
                                              displayId,
                                              pSinkEdid,
                                              pSrcCaps,
                                              pSinkCaps);
}
/*
 * NvHdmi_QueryFRLConfig
 */
NVHDMIPKT_RESULT
NvHdmi_QueryFRLConfig(NvHdmiPkt_Handle                      libHandle,
                      HDMI_VIDEO_TRANSPORT_INFO     const * const pVidTransInfo,
                      HDMI_QUERY_FRL_CLIENT_CONTROL const * const pClientCtrl,
                      HDMI_SRC_CAPS                 const * const pSrcCaps,
                      HDMI_SINK_CAPS                const * const pSinkCaps,
                      HDMI_FRL_CONFIG                     *pFRLConfig)
{
    if (libHandle == NVHDMIPKT_INVALID_HANDLE)
    {
        return NVHDMIPKT_LIBRARY_INIT_FAIL;
    }

    if (!pVidTransInfo ||
        !pClientCtrl   ||
        !pSrcCaps      ||
        !pSinkCaps     ||
        !pFRLConfig)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    // if there is no FRL capability reported fail this call
    if ((pSrcCaps->linkMaxFRLRate == HDMI_FRL_DATA_RATE_NONE) ||
        (pSinkCaps->linkMaxFRLRate == HDMI_FRL_DATA_RATE_NONE))
    {
        return NVHDMIPKT_FAIL;
    }
    
    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);
    return pClass->hdmiQueryFRLConfig(pClass,
                                      pVidTransInfo,
                                      pClientCtrl,
                                      pSrcCaps,
                                      pSinkCaps,
                                      pFRLConfig);
}

/*
 * NvHdmi_SetFRLConfig
 */
NVHDMIPKT_RESULT
NvHdmi_SetFRLConfig(NvHdmiPkt_Handle                   libHandle,
                    NvU32                              subDevice,
                    NvU32                              displayId,
                    NvBool                             bFakeLt,
                    HDMI_FRL_CONFIG                   *pFRLConfig)
{
    if (libHandle == NVHDMIPKT_INVALID_HANDLE)
    {
        return NVHDMIPKT_LIBRARY_INIT_FAIL;
    }

    if (!pFRLConfig)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);
    return pClass->hdmiSetFRLConfig(pClass,
                                    subDevice,
                                    displayId,
                                    bFakeLt,
                                    pFRLConfig);

}

/*
 * NvHdmi_ClearFRLConfig
 */
NVHDMIPKT_RESULT
NvHdmi_ClearFRLConfig(NvHdmiPkt_Handle     libHandle,
                      NvU32                subDevice,
                      NvU32                displayId)
{
    if (libHandle == NVHDMIPKT_INVALID_HANDLE)
    {
        return NVHDMIPKT_LIBRARY_INIT_FAIL;
    }

    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);
    return pClass->hdmiClearFRLConfig(pClass,
                                      subDevice,
                                      displayId);
}

/*************************** HDMI Library internal helper functions *****************************/
/*
 * NvHdmiPkt_HwClass2HdmiClass
 * internal function; translates display/display-dma class to hdmi class
 */
static NVHDMIPKT_CLASS_ID
NvHdmiPkt_HwClass2HdmiClass(NvU32 const hwClass)
{
    NVHDMIPKT_CLASS_ID hdmiClassId = NVHDMIPKT_9571_CLASS;
    NvU32 i = 0;

    for (i = 0; i < NVHDMIPKT_INVALID_CLASS; i++)
    {
        if ((hierarchy[i].displayClass == hwClass) ||
            (hierarchy[i].coreDmaClass == hwClass))
        {
            hdmiClassId = hierarchy[i].classId;
            break;
        }
    }

    // Assign default class 73 to pre-Kepler families
    if (hwClass < NV9170_DISPLAY)
    {
        hdmiClassId = NVHDMIPKT_0073_CLASS;
    }

    return hdmiClassId;
}

/*
 * NvHdmiPkt_InitInterfaces
 * internal function; calls class init interface functions
 */
static void
NvHdmiPkt_InitInterfaces(NVHDMIPKT_CLASS_ID const thisClassId,
                         NVHDMIPKT_CLASS*   const pClass)
{
    // Recurse to the root first, and then call each initInterface() method
    // from root to child.
    if (!hierarchy[thisClassId].isRootClass)
    {
        NvHdmiPkt_InitInterfaces(hierarchy[thisClassId].parentClassId, pClass);
    }
    hierarchy[thisClassId].initInterface(pClass);
}

static void
NvHdmiPkt_CallDestructors(NVHDMIPKT_CLASS_ID const thisClassId,
                          NVHDMIPKT_CLASS*   const pClass)
{
    // Destructor calls are made from this to root class.
    hierarchy[thisClassId].destructor(pClass);
    if (!hierarchy[thisClassId].isRootClass)
    {
        NvHdmiPkt_CallDestructors(hierarchy[thisClassId].parentClassId, pClass);
    }
}

/*
 * NvHdmiPkt_CallConstructors
 * internal function; calls class constructors and returns boolean success/failure
 */
static NvBool
NvHdmiPkt_CallConstructors(NVHDMIPKT_CLASS_ID const thisClassId,
                           NVHDMIPKT_CLASS*   const pClass)
{
    // Recurse to the root first, and then call each constructor
    // from root to child.
    if (!hierarchy[thisClassId].isRootClass)
    {
        if (!NvHdmiPkt_CallConstructors(hierarchy[thisClassId].parentClassId, pClass))
        {
            return NV_FALSE;
        }
    }

    if (!hierarchy[thisClassId].constructor(pClass))
    {
        if (!hierarchy[thisClassId].isRootClass)
        {
            // Backtrack on constructor failure
            NvHdmiPkt_CallDestructors(hierarchy[thisClassId].parentClassId, pClass);
        }

        return NV_FALSE;
    }

    return NV_TRUE;
}

/******************************** HDMI Library Init functions ***********************************/
/*
 * NvHdmiPkt_InitializeLibrary
 */
NvHdmiPkt_Handle 
NvHdmiPkt_InitializeLibrary(NvU32                              const hwClass,
                            NvU32                              const numSubDevices,
                            NvHdmiPkt_CBHandle                 const cbHandle,
                            const NVHDMIPKT_CALLBACK*          const pCallbacks,
                            NvU32                              const sfUserHandle,
                            const NVHDMIPKT_RM_CLIENT_HANDLES* const pClientHandles)
{
    NVHDMIPKT_CLASS* pClass = 0;
    NvU32 i = 0;
    NvBool result = NV_FALSE;
    NVHDMIPKT_CLASS_ID thisClassId = NVHDMIPKT_INVALID_CLASS;

    // Argument validations
    if (pCallbacks == 0 || numSubDevices == 0)
    {
        goto NvHdmiPkt_InitializeLibrary_exit;
    }

    // Validating RM handles/callbacks
#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (sfUserHandle == 0 || pClientHandles == 0)
    {
        goto NvHdmiPkt_InitializeLibrary_exit;
    }
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    if (pCallbacks->rmGetMemoryMap  == 0 ||
        pCallbacks->rmFreeMemoryMap == 0 ||
        pCallbacks->rmDispControl2 == 0)
    {
        goto NvHdmiPkt_InitializeLibrary_exit;
    }
#endif // NVHDMIPKT_RM_CALLS_INTERNAL

    // Mandatory mutex callbacks.
    if (pCallbacks->acquireMutex == 0 || pCallbacks->releaseMutex == 0)
    {
        goto NvHdmiPkt_InitializeLibrary_exit;
    }

    // Mandatory memory allocation callbacks.
    if (pCallbacks->malloc == 0 || pCallbacks->free == 0)
    {
        goto NvHdmiPkt_InitializeLibrary_exit;
    }

    pClass = pCallbacks->malloc(cbHandle, sizeof(NVHDMIPKT_CLASS));
    if (!pClass)
    {
        goto NvHdmiPkt_InitializeLibrary_exit;
    }

    // 0. Get the hdmi class ID
    thisClassId = NvHdmiPkt_HwClass2HdmiClass(hwClass);

    // Init data
    NVMISC_MEMSET(pClass, 0, sizeof(NVHDMIPKT_CLASS));

    for (i = 0; i < NV_MAX_SUBDEVICES; i++)
    {
        pClass->memMap[i].subDevice = NVHDMIPKT_INVALID_SUBDEV;
    }

    pClass->numSubDevices = numSubDevices;
    pClass->cbHandle = cbHandle;
    pClass->thisId = thisClassId;

    // RM handles/callbacks
#if NVHDMIPKT_RM_CALLS_INTERNAL
    pClass->isRMCallInternal         = NV_TRUE;
    pClass->sfUserHandle             = sfUserHandle;
    pClass->clientHandles.hClient    = pClientHandles->hClient;
    pClass->clientHandles.hDevice    = pClientHandles->hDevice;
    pClass->clientHandles.hDisplay   = pClientHandles->hDisplay;

    for (i = 0; i < NV_MAX_SUBDEVICES; i++)
    {
        pClass->clientHandles.hSubDevices[i] = pClientHandles->hSubDevices[i];
    }
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    pClass->isRMCallInternal         = NV_FALSE;
    pClass->callback.rmGetMemoryMap  = pCallbacks->rmGetMemoryMap;
    pClass->callback.rmFreeMemoryMap = pCallbacks->rmFreeMemoryMap;
    pClass->callback.rmDispControl2  = pCallbacks->rmDispControl2;
#endif // NVHDMIPKT_RM_CALLS_INTERNAL

    pClass->callback.acquireMutex    = pCallbacks->acquireMutex;
    pClass->callback.releaseMutex    = pCallbacks->releaseMutex;

    pClass->callback.malloc          = pCallbacks->malloc;
    pClass->callback.free            = pCallbacks->free;

#if !defined (NVHDMIPKT_DONT_USE_TIMER)
    pClass->callback.setTimeout      = pCallbacks->setTimeout;
    pClass->callback.checkTimeout    = pCallbacks->checkTimeout;
#endif

#if defined (DEBUG)
    pClass->callback.print           = pCallbacks->print;
    pClass->callback.assert          = pCallbacks->assert;
#endif

    // 1. Init interfaces
    NvHdmiPkt_InitInterfaces(thisClassId, pClass);

    // 2. Constructor calls
    result = NvHdmiPkt_CallConstructors(thisClassId, pClass);

NvHdmiPkt_InitializeLibrary_exit:
    if (result)
    {
        NvHdmiPkt_Print(pClass, "Initialize Success.");
    }
    else
    {
        if (pClass)
        {
            NvHdmiPkt_Print(pClass, "Initialize Failed.");
        }
        if (pCallbacks && pCallbacks->free)
        {
            pCallbacks->free(cbHandle, pClass);
        }
    }

    return (result == NV_TRUE) ? toHdmiPktHandle(pClass) : NVHDMIPKT_INVALID_HANDLE;
}

/*
 * NvHdmiPkt_DestroyLibrary
 */
void
NvHdmiPkt_DestroyLibrary(NvHdmiPkt_Handle libHandle)
{
    NVHDMIPKT_CLASS* pClass = fromHdmiPktHandle(libHandle);
    NVHDMIPKT_CLASS_ID currClassId = NVHDMIPKT_0073_CLASS;

    if (pClass != 0)
    {
        NvHdmiPkt_Print(pClass, "Destroy.");
        NvHdmiPkt_CBHandle cbHandle = pClass->cbHandle;
        void  (*freeCb)   (NvHdmiPkt_CBHandle handle,
                           void              *pMem) = pClass->callback.free;

        currClassId = pClass->thisId;
        NvHdmiPkt_CallDestructors(currClassId, pClass);

        freeCb(cbHandle, pClass);
    }
}
