/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_edid.h                                                         *
*    reading EDID from SST/MST Device                                       *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_EDID_H
#define INCLUDED_DP_EDID_H

#include "dp_buffer.h"
#include "dp_auxbus.h"
#include "dp_address.h"
#include "dp_messages.h"
#include "dp_messagecodings.h"
#include "dp_timer.h"

namespace DisplayPort
{
    class Edid;

    //
    //  Shared utility object for MST/SST edid reading.
    //    This object handles the retry, CRC validating,
    //    identification of EDID length, DDC ping, etc.
    //
    //   It's designed as an asynchronous state machine
    //   because of the way MST EDID reads are built.
    //
    class EdidAssembler
    {
    public:
        EdidAssembler(Edid * const edid, bool bPatchCrc = false);

        //
        // returns false - when existing data in Edid is invalid
        // returns seg - segment from which to read next block
        // returns offset - offset within block from which to start reading next block
        //
        bool readNextRequest(NvU8 & seg, NvU8 & offset);

        // returns false when Edid read is completed
        void postReply(const Buffer & buffer, unsigned sizeCompleted, bool success);
        void postReply(unsigned char * data, unsigned sizeCompleted, bool success);

        // returns true when it read all the required blocks
        bool readIsComplete();
        void reset();
    private:
        Edid * edid;
        Stream stream;

        NvU8 oldBlockChecksum;
        unsigned blocksRead;
        unsigned totalBlockCnt;
        unsigned retriesCount;
        bool bPatchCrc;
    };

    //
    //  EDID
    //
    class Edid
    {
    public:
        Edid();
        ~Edid();

        Buffer * getBuffer() const { return &buffer; }
        NvU8 getFirstPageChecksum();  // Get checksum byte
        NvU8 getLastPageChecksum();   // Get checksum byte for last block

        bool     verifyCRC();
        unsigned getEdidVersion();
        unsigned getBlockCount();
        const char * getName() const;
        unsigned getEdidSize() const;
        bool isChecksumValid() const;
        bool isJunkEdid() const;
        bool isFallbackEdid() const;
        void swap(Edid & right);
        void applyEdidWorkArounds(NvU32 warFlag, const DpMonitorDenylistData *pDenylistData);
        void patchCrc();
        void setForcedEdidChecksum(bool set)
        {
            this->forcedCheckSum = set;
        }

        void setFallbackFlag(bool set)
        {
            this->fallbackEdid = set;
        }

        void setPatchedChecksum(bool set)
        {
            this->patchedChecksum = set;
        }

        bool isPatchedChecksum() const
        {
            return this->patchedChecksum;
        }

        bool isValidHeader() const;

        unsigned getManufId() const
        {
            if (buffer.getLength() < 0xa)
                return 0;

            return ((buffer.data[0x9] << 8) | (buffer.data[0x8]));
        }

        unsigned getProductId() const
        {
            if (buffer.getLength() < 0xc)
                return 0;

            return ((buffer.data[0xb] << 8) | (buffer.data[0xa]));
        }

        unsigned getYearWeek() const
        {
            if (buffer.getLength() < 0x12)
                return 0;

            return ((buffer.data[0x11] << 8) | (buffer.data[0x10]));
        }

        typedef struct
        {
            bool extensionCountDisabled;
            bool dataForced;
            bool disableDpcdPowerOff;
            bool forceMaxLinkConfig;
            bool powerOnBeforeLt;
            bool skipRedundantLt;
            bool skipCableBWCheck;
            bool overrideOptimalLinkCfg;
            bool overrideMaxLaneCount;
            bool ignoreRedundantHotplug;
            bool delayAfterD3;
            bool keepLinkAlive;
            bool useLegacyAddress;
            bool bIgnoreDscCap;           // Ignore DSC even if sink reports DSC capability
            bool bDisableDownspread;
            bool bForceHeadShutdown;
            bool bDP2XPreferNonDSCForLowPClk;
            bool bDisableDscMaxBppLimit;
            bool bForceHeadShutdownOnModeTransition;
            bool bSkipCableIdCheck;
            bool bAllocateManualTimeslots;
            bool bSkipResetMSTMBeforeLt;
        }_WARFlags;

        _WARFlags WARFlags;

        typedef struct
        {
            unsigned maxLaneCount;      // Max lane count value to override
            unsigned maxLaneAtHighRate; // Max lane count supported at HBR
            unsigned maxLaneAtLowRate;  // Max lane count supported at RBR
            unsigned optimalLinkRate;   // Optimal link rate value to override
            unsigned optimalLaneCount;  // Optimal lane count value to override
        }_WARData;

        _WARData WARData;

        void resetData()
        {
            buffer.reset();
            checkSumValid = false;
            forcedCheckSum = false;
            fallbackEdid = false;
            // clear the WARFlags
            _WARFlags temp = {0};
            WARFlags = temp;
        }

        bool operator== (const Edid & other)
        {
            return (buffer == other.buffer);
        }

        bool operator!= (const Edid & other)
        {
            return !(buffer == other.buffer);
        }

    private:
        void     validateCheckSum();

        mutable Buffer buffer;
        bool checkSumValid;
        bool forcedCheckSum;
        bool fallbackEdid;
        bool patchedChecksum;
    };

    //
    //  SST EDID Read API
    //
    bool EdidReadSST(Edid & edid, AuxBus * aux, Timer * timer, bool pendingTestRequestEdidRead = false, bool bBypassAssembler = false, MainLink *main = NULL);

    enum EDID_DDC
    {
        EDID_DDC_NONE = 0x00,
        EDID_DDC_ADR0 = 0xA0,
        EDID_DDC_ADR1 = 0xA2,
        EDID_DDC_ADR2 = 0xA6,
        EDID_SEG_SELECTOR_OFFSET = 0x60,
    };
    EDID_DDC sstDDCPing(AuxBus  & dpAux);

    //
    //  MST EDID Read API
    //

    class EdidReadMultistream : public Object, protected MessageManager::Message::MessageEventSink, Timer::TimerCallback
    {
    public:
        class EdidReadMultistreamEventSink // Connector will inherit from this
        {
        public:
            virtual void mstEdidCompleted(EdidReadMultistream * from) = 0;
            virtual void mstEdidReadFailed(EdidReadMultistream * from) = 0;
        };

        EdidReadMultistream(Timer * timer, MessageManager * manager, EdidReadMultistream::EdidReadMultistreamEventSink * sink, Address topologyAddress)
           : topologyAddress(topologyAddress), manager(manager), edidReaderManager(&edid), ddcIndex(0),
             retries(0), timer(timer), sink(sink)
        {
            startReadingEdid();
        }

        Edid edid;
        Address topologyAddress;
        ~EdidReadMultistream();

    private:
        void startReadingEdid();

        MessageManager * manager;
        RemoteI2cReadMessage remoteI2cRead;
        EdidAssembler edidReaderManager;    // come up another word besides edidReaderManager eg Manager
        NvU8 DDCAddress;
        NvU8 ddcIndex;
        unsigned retries;
        Timer * timer;

        void readNextBlock(NvU8 seg, NvU8 offset);
        void failedToReadEdid();
        void expired(const void * tag);

        EdidReadMultistreamEventSink * sink;

        virtual void messageFailed(MessageManager::Message * from, NakData * nakData);
        virtual void messageCompleted(MessageManager::Message * from);
        void edidAttemptDone(bool succeeded);
    };

    //
    //  Useful defines
    //
    enum
    {
        EDID_BLOCK_SIZE = 0x80,
        EDID_SEGMENT_SIZE = 2*EDID_BLOCK_SIZE,
        EDID_POLICY_BLOCK_READ_MAX_RETRY_COUNT = 3,
        // DID EDID CTS v1.3 d12 currently outlines that Source shall support up to 16 blocks of EDID data.
        EDID_MAX_BLOCK_COUNT = 16,
    };

    static const NvU8 ddcAddrList[] = {EDID_DDC_ADR0, EDID_DDC_ADR1, EDID_DDC_ADR2};
    const NvU8 ddcAddrListSize = sizeof(ddcAddrList)/sizeof(NvU8);

    // HDMI 1.4 Section 8.5: HDMI Sink can have up to 100ms to get EDID ready.
    const NvU8 EDID_READ_RETRY_TIMEOUT_MS = 100;
    const NvU8 EDID_MAX_AUX_RETRIES = 10;
    const NvU8 EDID_AUX_WAIT_TIME = 1;
    NvU8 getEDIDBlockChecksum(const Buffer &);

    void makeEdidFallback(Edid & edid, NvU32 fallbackFormatSupported = 0);
    void makeEdidFallbackVGA(Edid & edid);

}

#endif //INCLUDED_DP_EDID_H
