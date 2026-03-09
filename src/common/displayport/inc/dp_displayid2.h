/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_did2.h                                                         *
*    reading DisplayID2x from SST/MST Device                                *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_DID2_H
#define INCLUDED_DP_DID2_H

#include "dp_buffer.h"
#include "dp_auxbus.h"
#include "dp_timer.h"
#include "dp_messages.h"
#include "dp_messagecodings.h"

#define NV_DISPLAYID2_BLOCK_SIZE                    0x80
#define NV_DISPLAYID2_MAX_RETRIES                   0x03

#define NV_DISPLAYID2_DDC_ADDRESS                   0xA4
#define NV_DISPLAYID2_DDC_SEG_SELECTOR_OFFSET       0x60

#define NV_DISPLAYID2_MAX_BLOCK_COUNT                 16

#define NV_DISPLAYID2_IDX_STRUCTURE_VERSION           0x00
#define NV_DISPLAYID2_STRUCTURE_VERSION_V20           0x20
#define NV_DISPLAYID2_IDX_BYTES_IN_SECTION            0x01
#define NV_DISPLAYID2_SECTION_SIZE_MIN                0x05
#define NV_DISPLAYID2_SECTION_SIZE_MAX                0x100

#define NV_DISPLAYID2_BYTES_IN_SECTION_MIN            0x00
#define NV_DISPLAYID2_BYTES_IN_SECTION_MAX            0xFB

#define NV_DISPLAYID2_IDX_EXTENSION_COUNT             0x03

#define NV_DISPLAYID2_EXTENSION_COUNT_RESERVED        0x00
#define NV_DISPLAYID2_EXTENSION_COUNT_MIN             0x00
#define NV_DISPLAYID2_EXTENSION_COUNT_MAX             0xFF

#define IS_VALID_DISPLAYID2_VERSION(pSection)                         \
        (pSection[NV_DISPLAYID2_IDX_STRUCTURE_VERSION] ==             \
         NV_DISPLAYID2_STRUCTURE_VERSION_V20)

#define GET_DISPLAYID2_SECTION_LENGTH(pSection)                       \
        (NV_DISPLAYID2_SECTION_SIZE_MIN +                             \
         pSection[NV_DISPLAYID2_IDX_BYTES_IN_SECTION])

#define GET_DISPLAYID2_EXTENSION_COUNT(pSection)                      \
        (pSection[NV_DISPLAYID2_IDX_EXTENSION_COUNT])

namespace DisplayPort
{
    class DisplayID2x
    {
    public:
        DisplayID2x();
        ~DisplayID2x();

        NvU32       getDisplayId2xSize() const { return displayId2xSize; }
        void        setDisplayId2xSize(NvU32 length) { displayId2xSize = length; }

        NvU8        getBlockCount() const { return blockCount; }
        void        setBlockCount(NvU8 count) { blockCount = count; }
        Buffer *    getBuffer() const { return &buffer; }

        bool operator== (const DisplayID2x & other)
        {
            return (buffer == other.buffer);
        }

        bool operator!= (const DisplayID2x & other)
        {
            return !(buffer == other.buffer);
        }

    private:
        mutable Buffer buffer;
        NvU8    blockCount;
        NvU32   displayId2xSize;

    };

    bool DisplayId2ReadSST(DisplayID2x & displayId2x, AuxBus * aux, Timer * timer, MainLink *main = NULL);
    bool DisplayId2ReadMST(DisplayID2x & displayId2x, AuxBus * aux, Timer * timer, MainLink *main = NULL);

    NvU8 getDID2BlockChecksum(NvU8 * buffer, NvU8 size);

    const NvU8 NV_DISPLAYID2_READ_RETRY_MS = 100;
    const NvU8 NV_DISPLAYID2_MAX_AUX_RETRIES = 10;
    const NvU8 NV_DISPLAYID2_AUX_WAIT_TIME = 1;

    //
    //  MST DID2x Read API
    //

    class DID2ReadMultistream : public Object, protected MessageManager::Message::MessageEventSink, Timer::TimerCallback
    {
    public:
        class DID2ReadMultistreamEventSink // Connector will inherit from this
        {
        public:
            virtual void mstDid2Completed(DID2ReadMultistream * from) = 0;
            virtual void mstDid2ReadFailed(DID2ReadMultistream * from) = 0;
        };

        DID2ReadMultistream(Timer * timer, MessageManager * manager, DID2ReadMultistream::DID2ReadMultistreamEventSink * sink, Address topologyAddress)
           : topologyAddress(topologyAddress), manager(manager),
             retries(0), timer(timer), sink(sink), stream(displayId2x.getBuffer())
        {
            startReadingDid2();
        }

        DisplayID2x     displayId2x;

        Address         topologyAddress;

        ~DID2ReadMultistream();

    private:
        void startReadingDid2();

        MessageManager * manager;
        RemoteI2cReadMessage remoteI2cRead;
        unsigned retries;
        Timer * timer;

        void readNextBlock();
        bool readNextRequest();
        void failedToReadDid2();
        void expired(const void * tag);

        DID2ReadMultistreamEventSink * sink;

        virtual void messageFailed(MessageManager::Message * from, NakData * nakData);
        virtual void messageCompleted(MessageManager::Message * from);
        void        did2AttemptDone(bool succeeded);

        Stream  stream;

        // Number of times the sections are read. Note the section might not be aligned to 128 bytes.
        NvU8    loop;

        // Number of bytes already read but not yet processed.
        NvU32   remainingBytes;

        // Pointer to the current section of the DisplayID2x
        NvU8    *pSection;

        // Length of the current section of the DisplayID2x
        NvU8    sectionLength;

        // Number of extension blocks. Only valid for the base section.
        NvU8    extensionCount;

        // Size of the DisplayID2x blocks that are already completed.
        NvU32   displayId2xSize;

        bool    retried;
    };

}
#endif
