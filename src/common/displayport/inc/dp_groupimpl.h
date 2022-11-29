/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_groupimpl.h                                                    *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_GROUPIMPL_H
#define INCLUDED_DP_GROUPIMPL_H

#include "dp_connector.h"
#include "dp_deviceimpl.h"
#include "dp_linkedlist.h"
#include "dp_watermark.h"
#include "dp_auxdefs.h"

namespace DisplayPort
{

    struct GroupImpl : public Group, ListElement, Timer::TimerCallback
    {
        ConnectorImpl * parent;
        LinkedList<Device> members;
        List            elements;
        unsigned        headIndex;
        unsigned        streamIndex;
        bool            streamValidationDone;
        bool            headInFirmware;             // Set if this is a firmware run mode.  If set lastModesetInfo is NOT valid
        bool            bIsHeadShutdownNeeded;      // Set if head shutdown is requested during modeset
        bool            hdcpEnabled;
        bool            hdcpPreviousStatus;
        bool            bWaitForDeAllocACT;
        bool            bDeferredPayloadAlloc;
        ModesetInfo     lastModesetInfo;
        DSC_MODE        dscModeRequest;             // DSC mode requested during NAB
        DSC_MODE        dscModeActive;              // DSC mode currently active, set in NAE
        DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID singleHeadMultiStreamID;
        DP_SINGLE_HEAD_MULTI_STREAM_MODE        singleHeadMultiStreamMode;
        DP_COLORFORMAT  colorFormat;

        struct
        {
            unsigned          PBN;
            int               count;
            int               begin;
            bool              hardwareDirty;        // Does the configureStream need to be called again?
            Watermark         watermarks;           // Cached watermark calculations
        } timeslot;

        GroupImpl(ConnectorImpl * parent, bool isFirmwareGroup = false)
            : parent(parent),
              streamValidationDone(true),
              headInFirmware(false),
              bIsHeadShutdownNeeded(true),
              hdcpEnabled(false),
              hdcpPreviousStatus(false), 
              bWaitForDeAllocACT(false),
              dscModeRequest(DSC_MODE_NONE),
              dscModeActive(DSC_MODE_NONE),
              singleHeadMultiStreamID(DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY),
              singleHeadMultiStreamMode(DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE),
              headAttached(false)
        {
            timeslot.count = 0;
        }

        ~GroupImpl()
        {
        }

        virtual void insert(Device * dev);
        virtual void remove(Device * dev);
        void update(Device * dev, bool allocationState);        // send the allocatepayload/deallocatepayload message
        bool contains(Device * dev) { return members.contains(dev); }
        virtual Device * enumDevices(Device * previousDevice);

        void updateVbiosScratchRegister(Device * lastDevice);   // Update the VBIOS scratch register with last lit display
        
        //
        //  Timer callback tags.
        //   (we pass the address of these variables as context to ::expired)
        //
        char tagHDCPReauthentication;
        char tagStreamValidation;

        unsigned authRetries;                       // Retry counter for the authentication.

        virtual void expired(const void * tag);
        virtual bool hdcpGetEncrypted();
        virtual void destroy();
        void cancelHdcpCallbacks();

        bool            isHeadAttached() { return headAttached; }
        void            setHeadAttached(bool attached);

    private:
        bool            headAttached;               // True if modeset started (during NAB). Sets back to False during NDE
    };
}

#endif //INCLUDED_DP_GROUPIMPL_H
