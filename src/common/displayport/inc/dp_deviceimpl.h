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

/******************************* DisplayPort*********************************\
*                                                                           *
* Module: dp_connector.cpp                                                  *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_DEVICEIMPL_H
#define INCLUDED_DP_DEVICEIMPL_H

#include "dp_connector.h"
#include "dp_internal.h"
#include "dp_edid.h"
#include "dp_list.h"
#include "dp_auxdefs.h"
#include "dp_vrr.h"

namespace DisplayPort
{
    #define PREDEFINED_DSC_MST_BPPX16 160;
    #define MAX_DSC_COMPRESSION_BPPX16 128;
    #define HDCP_BCAPS_DDC_OFFSET 0x40
    #define HDCP_BCAPS_DDC_EN_BIT 0x80
    #define HDCP_BCAPS_DP_EN_BIT  0x01
    #define HDCP_I2C_CLIENT_ADDR  0x74
    #define DEVICE_OUI_SIZE       3
    #define DSC_CAPS_SIZE         16

    struct GroupImpl;
    struct ConnectorImpl;
    class DeviceHDCPDetection;
    class VrrEnablement;

    struct DeviceImpl : public Device,
                        public AuxBus,
                        public ListElement
    {
        //
        //  Shadow state: This is the last state delivered to DD.
        //     see the ConnectorImpl::fireEvents() function for handling.
        //
        //   State is double buffered to allow for allow announces
        //   to happen at the end of the state updates.  We assume
        //   the DD can call any Connector API in response to the
        //   event.
        //
        struct Shadow
        {
            bool plugged;
            bool zombie;
            bool cableOk;
            bool mustDisconnect;
            bool hdcpCapDone;
            LinkConfiguration highestAssessedLC;
        } shadow;

        struct BandWidth
        {
            struct _Enum_Path
            {
                unsigned availableStreams, total, free, dfpLinkAvailable;
                bool     bPathFECCapable;
                bool     dataValid;                     // Is the cache valid?
                bool     availablePbnUpdated;
            } enum_path;

            struct Compound_Query_State
            {
                unsigned totalTimeSlots;                // Total timeslots available for allocation across this node

                unsigned timeslots_used_by_query;       // Timeslots accounted for.

                unsigned bandwidthAllocatedForIndex;    // Compound query is compromised of several
                                                        //   qeuery attaches.  These query attaches
                                                        //   may have more than one device associated.
                                                        //   this mask keeps track of which queryAttach's
                                                        //   have already had the stream "rounted" past
                                                        //   this node.
            } compound_query_state;

            LinkConfiguration lastHopLinkConfig;        // inferred from enum_path.total

        } bandwidth;

        enum rawEprState
        {
            software,
            hardware
        };

        void                resetCacheInferredLink();
        LinkConfiguration * inferLeafLink(unsigned * totalLinkSlots);
        void                inferPathConstraints();


        DeviceImpl      * parent;               // Upstream parent device
        DeviceImpl      * children[16];
        PortMap         portMap;

        Edid              rawEDID;
        Edid              processedEdid;
        Edid              ddcEdid;
        DPCDHAL         * hal;
        GroupImpl       * activeGroup;
        ConnectorImpl   * connector;
        ConnectorType     connectorType;
        Address           address;
        GUID              guid;
        GUID              guid2;
        bool              bVirtualPeerDevice;
        NvU8              peerDevice;
        NvU8              dpcdRevisionMajor;
        NvU8              dpcdRevisionMinor;
        bool              multistream;
        bool              videoSink, audioSink;
        bool              plugged;
        bool              bApplyPclkWarBug4949066;

        AuxRetry          friendlyAux;
        bool              payloadAllocated;             // did the allocate payload go through?

        unsigned char     BCAPS[HDCP_BCAPS_SIZE];       // Hdcp1.x bCaps raw data
        unsigned char     BKSV[HDCP_KSV_SIZE];          // Hdcp1.x bKsv raw data
        unsigned char     nvBCaps[HDCP_BCAPS_SIZE];     // NV generic HDCP BCAPS including 1.x, 2.2, ...
        NvU64             maxTmdsClkRate;


        bool            isPendingNewDevice();
        bool            isPendingLostDevice();
        bool            isPendingZombie();
        bool            isPendingCableOk();
        bool            isPendingBandwidthChange();
        bool            isPendingHDCPCapDone();

        TriState        isHDCPCap;
        bool            isDeviceHDCPDetectionAlive;
        DeviceHDCPDetection * deviceHDCPDetection;

        PCONCaps        pconCaps;

        // this flag signifies that the compliance device has requested EDID read test and may follow
        // hidden and lazy zombie policy.
        bool            complianceDeviceEdidReadTest;

        bool            lazyExitNow;

        // VRR Enablement structure
        VrrEnablement     *vrrEnablement;

        // DSC fields
        NvU8    rawDscCaps[16];
        DscCaps dscCaps;

        // Panel replay Caps
        PanelReplayCaps prCaps;
        // ALPM caps
        AlpmCaps alpmCaps;
        bool bIsFakedMuxDevice;
        bool bIsPreviouslyFakedMuxDevice;
        bool bisMarkedForDeletion;
        bool bIgnoreMsaCap;
        bool bIgnoreMsaCapCached;

        //
        // Device doing the DSC decompression for this device. This could be device itself
        // or its parent
        //
        DeviceImpl* devDoingDscDecompression;
        //
        // If DSC stream can be sent to this device or not. Either device itself or it's
        // parent can do DSC decompression
        //
        bool bDSCPossible;

        bool bFECSupported;
        bool bFECUncorrectedSupported;
        bool bFECCorrectedSupported;
        bool bFECBitSupported;
        bool bFECParityBlockSupported;
        bool bFECParitySupported;

        TriState bSdpExtCapable;
        TriState bAsyncSDPCapable;
        bool bMSAOverMSTCapable;
        bool bDscPassThroughColorFormatWar;

        NvU64 maxModeBwRequired;

        DeviceImpl(DPCDHAL * hal, ConnectorImpl * connector, DeviceImpl * parent);
        ~DeviceImpl();

        virtual bool isCableOk();
        virtual bool isLogical();
        virtual bool isZombie();

        virtual unsigned    getEDIDSize() const;
        virtual bool        getEDID(char * buffer, unsigned size) const;
        virtual unsigned    getRawEDIDSize() const;
        virtual bool        getRawEDID(char * buffer, unsigned size) const;

        virtual bool getPCONCaps(PCONCaps *pPCONCaps);

        virtual Group * getOwningGroup()
        {
            return (Group *)activeGroup;
        }

        bool isActive();

        void applyOUIOverrides();

        virtual Device * getParent()
        {
            return parent;
        }

        virtual Device * getChild(unsigned portNumber)
        {
            return children[portNumber];
        }

        virtual bool isMultistream()        // Sink supports multistream, remember we can have 1.1 targets
        {
            return address.size() != 0;
        }

        virtual bool isNativeDPCD()
        {
            return (address.size() < 2);
        }

        virtual bool isVideoSink()
        {
            return videoSink;
        }

        virtual bool isAudioSink()
        {
            return audioSink;
        }

        virtual bool isLoop()
        {
           // implementation is pending (bug 791059)
            return false;
        }

        virtual bool isRedundant()
        {
            // implementation is pending (bug 791059)
            return false;
        }

        virtual bool isMustDisconnect();

        virtual bool isPlugged()
        {
            return plugged;
        }

        virtual Address getTopologyAddress() const
        {
            return address;
        }

        virtual ConnectorType   getConnectorType()
        {
            return connectorType;
        }

        virtual bool isFallbackEdid()
        {
            return this->processedEdid.isFallbackEdid();
        }

        virtual GUID getGUID() const
        {
            return guid;
        }

        virtual PortMap getPortMap() const
        {
            return portMap;
        }

        virtual TriState hdcpAvailableHop();
        virtual TriState hdcpAvailable();

        virtual bool isMSAOverMSTCapable()
        {
            return bMSAOverMSTCapable;
        }

        virtual bool isFakedMuxDevice();
        virtual bool isPreviouslyFakedMuxDevice();

        bool bypassDpcdPowerOff()
        {
            return processedEdid.WARFlags.disableDpcdPowerOff;
        }

        bool powerOnMonitorBeforeLt()
        {
            return processedEdid.WARFlags.powerOnBeforeLt;
        }

        bool forceMaxLinkConfig()
        {
            return processedEdid.WARFlags.forceMaxLinkConfig;
        }

        bool skipRedundantLt()
        {
            return processedEdid.WARFlags.skipRedundantLt;
        }

        bool ignoreRedundantHotplug()
        {
            return processedEdid.WARFlags.ignoreRedundantHotplug;
        }

        bool isOptimalLinkConfigOverridden()
        {
            return processedEdid.WARFlags.overrideOptimalLinkCfg;
        }

        // Apply DPCD overrides if required
        void dpcdOverrides();

        bool getDpcdRevision(unsigned * major, unsigned * minor)
        {
            if (!major || !minor)
            {
                DP_ASSERT(0 && "Null pointers passed in.");
                return false;
            }

            *major = this->dpcdRevisionMajor;
            *minor = this->dpcdRevisionMinor;
            return true;
        }

        bool getIgnoreMSACap();

        AuxRetry::status setIgnoreMSAEnable(bool msaTimingParamIgnoreEn);

        bool isVirtualPeerDevice()
        {
            return bVirtualPeerDevice;
        }

        bool isBranchDevice()
        {
            return !isVideoSink() && !isAudioSink();
        }

        bool  isAtLeastVersion(unsigned major, unsigned minor)
        {
            if (dpcdRevisionMajor > major)
                return true;

            if (dpcdRevisionMajor < major)
                return false;

            return dpcdRevisionMinor >= minor;
        }

        NvU64 getMaxModeBwRequired()
        {
            return maxModeBwRequired;
        }

        virtual void queryGUID2();

        virtual bool getSDPExtnForColorimetrySupported();
        virtual bool getAsyncSDPSupported();

        virtual bool getPanelFwRevision(NvU16 *revision);

        virtual bool isPowerSuspended();

        virtual void setPanelPowerParams(bool bSinkPowerStateD0, bool bPanelPowerStateOn);

        virtual status transaction(Action action, Type type, int address,
                                   NvU8 * buffer, unsigned sizeRequested,
                                   unsigned * sizeCompleted,
                                   unsigned *pNakReason= NULL,
                                   NvU8 offset= 0, NvU8 nWriteTransactions= 0);
        virtual unsigned transactionSize();
        // default behaviour is querying first three registers for every lane --> flags = 0x7
        virtual status fecTransaction(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags = NV_DP_FEC_FLAGS_SELECT_ALL);
        virtual AuxBus        * getRawAuxChannel()  { return this; }
        virtual AuxRetry      * getAuxChannel()     { return &friendlyAux; }
        virtual AuxBus::status getDpcdData(unsigned offset, NvU8 * buffer,
                                           unsigned sizeRequested,
                                           unsigned * sizeCompleted,
                                           unsigned * pNakReason=NULL);
        virtual AuxBus::status setDpcdData(unsigned offset, NvU8 * buffer,
                                           unsigned sizeRequested,
                                           unsigned * sizeCompleted,
                                           unsigned * pNakReason=NULL);
        virtual AuxBus::status queryFecData(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags);

        virtual DscCaps getDscCaps();

        //
        // This function returns the device itself or its parent device that is doing
        // DSC decompression for it.
        //
        virtual Device* getDevDoingDscDecompression();
        virtual void    markDeviceForDeletion() {bisMarkedForDeletion = true;};
        virtual bool    isMarkedForDeletion() {return bisMarkedForDeletion;};
        virtual bool    getRawDscCaps(NvU8 *buffer, NvU32 bufferSize);
        virtual bool    setRawDscCaps(const NvU8 *buffer, NvU32 bufferSize);

        virtual AuxBus::status dscCrcControl(NvBool bEnable, gpuDscCrc *dataGpu, sinkDscCrc *dataSink);

        //
        // Parameter bForceMot in both getI2cData and setI2cData is used to forfully set
        // the MOT bit. It is needed for some special cases where the MOT bit shouldn't
        // be set but some customers need it to please their monitors.
        //
        virtual bool getI2cData(unsigned offset, NvU8 * buffer, unsigned sizeRequested, unsigned * sizeCompleted, bool bForceMot = false);
        virtual bool setI2cData(unsigned offset, NvU8 * buffer, unsigned sizeRequested, unsigned * sizeCompleted, bool bForceMot = false);
        virtual bool getRawEpr(unsigned * totalEpr, unsigned * freeEpr, rawEprState eprState);

        void switchToComplianceFallback();

        // VRR Display Enablement Functions
        bool startVrrEnablement(void);
        void resetVrrEnablement(void);
        bool isVrrMonitorEnabled(void);
        bool isVrrDriverEnabled(void);

        // Panel replay related functions
        bool isPanelReplaySupported(void);
        void getPanelReplayCaps(void);
        bool setPanelReplayConfig(panelReplayConfig prcfg);
        bool getPanelReplayConfig(panelReplayConfig *pPrcfg);
        bool getPanelReplayStatus(PanelReplayStatus *pPrStatus);
        NvBool isSelectiveUpdateSupported(void);
        NvBool isEarlyRegionTpSupported(void);
        NvBool enableAdaptiveSyncSdp(NvBool enable);
        SelectiveUpdateCaps getSelectiveUpdateCaps(void);
        NvBool isAdaptiveSyncSdpNotSupportedInPr(void);
        NvBool isdscDecodeNotSupportedInPr(void);
        NvBool isLinkOffSupportedAfterAsSdpInPr(void);
        void getAlpmCaps(void);
        NvBool setAlpmConfig(AlpmConfig alpmcfg);
        NvBool getAlpmStatus(AlpmStatus *pAlpmStatus);
        NvBool isAuxLessAlpmSupported(void);

        NvBool getDSCSupport();
        bool getFECSupport();
        NvBool isDSCPassThroughSupported();
        NvBool isDynamicPPSSupported();
        NvBool isDynamicDscToggleSupported();
        NvBool isDSCSupported();
        NvBool isDSCDecompressionSupported();
        NvBool isDSCPossible();
        bool isFECSupported();
        bool readAndParseDSCCaps();
        bool readAndParseBranchSpecificDSCCaps();
        bool parseDscCaps(const NvU8 *buffer, NvU32 bufferSize);
        bool parseBranchSpecificDscCaps(const NvU8 *buffer, NvU32 bufferSize);
        bool setDscEnable(bool enable);
        bool setDscEnableDPToHDMIPCON(bool bDscEnable, bool bEnablePassThroughForPCON);
        bool getDscEnable(bool *pEnable);
        unsigned getDscVersionMajor();
        unsigned getDscVersionMinor();
        unsigned getDscRcBufferSize();
        unsigned getDscRcBufferBlockSize();
        unsigned getDscMaxSlicesPerSink();
        unsigned getDscLineBufferBitDepth();
        NvBool   isDscBlockPredictionSupported();
        unsigned getDscMaxBitsPerPixel();
        NvBool   isDscRgbSupported();
        NvBool   isDscYCbCr444Supported();
        NvBool   isDscYCbCrSimple422Supported();
        NvBool   isDscYCbCr422NativeSupported();
        NvBool   isDscYCbCr420NativeSupported();
        unsigned getDscPeakThroughputMode0();
        unsigned getDscPeakThroughputModel();
        unsigned getDscMaxSliceWidth();
        unsigned getDscDecoderColorDepthSupportMask();
        void setDscDecompressionDevice(bool bDscCapBasedOnParent);
        virtual bool getDeviceSpecificData(NvU8 *oui, NvU8 *deviceIdString,
                                           NvU8 *hwRevision, NvU8 *swMajorRevision,
                                           NvU8 *swMinorRevision);
        virtual bool getParentSpecificData(NvU8 *oui, NvU8 *deviceIdString,
                                           NvU8 *hwRevision, NvU8 *swMajorRevision,
                                           NvU8 *swMinorRevision);

        virtual bool setModeList(DisplayPort::DpModesetParams *pModeList, unsigned numModes);
    };
    class DeviceHDCPDetection : public Object, MessageManager::Message::MessageEventSink, Timer::TimerCallback
    {
            DeviceImpl*              parent;
            RemoteDpcdReadMessage    remoteBKSVReadMessage;
            RemoteDpcdReadMessage    remoteBCapsReadMessage;
            RemoteDpcdReadMessage    remote22BCapsReadMessage;
            MessageManager *         messageManager;     // For transmit and receive
            Timer *                  timer;
            bool                     bksvReadCompleted;
            bool                     bCapsReadCompleted;
            bool                     isValidBKSV;
            bool                     isBCapsHDCP;
            unsigned                 retriesRemoteBKSVReadMessage;
            unsigned                 retriesRemoteBCapsReadMessage;
            unsigned                 retriesRemote22BCapsReadMessage;
            bool                     retryRemoteBKSVReadMessage;
            bool                     retryRemoteBCapsReadMessage;
            bool                     retryRemote22BCapsReadMessage;
            bool                     bBKSVReadMessagePending;
            bool                     bBCapsReadMessagePending;

        public:

            DeviceHDCPDetection(DeviceImpl * parent, MessageManager * messageManager, Timer * timer)
                : bksvReadCompleted(false),bCapsReadCompleted(false),isValidBKSV(false),
                  isBCapsHDCP(false), retriesRemoteBKSVReadMessage(0), retriesRemoteBCapsReadMessage(0),
                  retriesRemote22BCapsReadMessage(0), retryRemoteBKSVReadMessage(false),
                  retryRemoteBCapsReadMessage(false), retryRemote22BCapsReadMessage(false),
                  bBKSVReadMessagePending(false), bBCapsReadMessagePending(false)

            {
                this->parent = parent;
                this->messageManager = messageManager;
                this->timer = timer;
            }

            ~DeviceHDCPDetection();
            void expired(const void * tag);
            void start();
            void waivePendingHDCPCapDoneNotification();

            bool hdcpValidateKsv(const NvU8 *ksv, NvU32 Size);
            void handleRemoteDpcdReadDownReply(MessageManager::Message * from);
            void messageFailed(MessageManager::Message * from, NakData * nakData);
            void messageCompleted(MessageManager::Message * from);
    };
}

#endif //INCLUDED_DP_DEVICEIMPL_H

