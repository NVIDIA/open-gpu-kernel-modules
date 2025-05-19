/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_deviceimpl.cpp                                                 *
*   DP device implementation                                                *
*                                                                           *
\***************************************************************************/

#include "dp_connectorimpl.h"
#include "dp_deviceimpl.h"
#include "dp_auxdefs.h"
#include "dp_groupimpl.h"
#include "dp_printf.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
using namespace DisplayPort;

bool DeviceImpl::isMustDisconnect()
{
    //
    //     Device is must disconnect if we're trying to make an SST<->MST transition
    //
    if ((this->isActive()) && connector->linkAwaitingTransition)
    {
        return true;
    }

    return false;
}

DeviceImpl::~DeviceImpl()
{
    if (isDeviceHDCPDetectionAlive && deviceHDCPDetection)
    {
        delete deviceHDCPDetection;
        deviceHDCPDetection = nullptr;
    }

    if (vrrEnablement)
    {
        delete vrrEnablement;
        vrrEnablement = NULL;
    }

    // Unlink this node from its children
    for (unsigned int i = 0; i < sizeof(children)/sizeof(*children); i++)
        if (children[i])
            children[i]->parent = 0;

    // Unlink this node from its parent when it's there
    if (parent && (parent->children[this->address.tail()] == this))
        parent->children[this->address.tail()] = 0;

    devDoingDscDecompression = NULL;
}


DeviceImpl::DeviceImpl(DPCDHAL * hal, ConnectorImpl * connector, DeviceImpl * parent)
    : parent(parent),
      hal(hal),
      activeGroup(0),
      connector(connector),
      address(),
      bVirtualPeerDevice(false),
      plugged(false),
      friendlyAux(this),
      isHDCPCap(False),
      isDeviceHDCPDetectionAlive(false),
      deviceHDCPDetection(0),
      vrrEnablement(0),
      bIsFakedMuxDevice(false),
      bIsPreviouslyFakedMuxDevice(false),
      bisMarkedForDeletion(false),
      bIgnoreMsaCap(false),
      bIgnoreMsaCapCached(false),
      bSdpExtCapable(Indeterminate),
      bAsyncSDPCapable(Indeterminate),
      bDscPassThroughColorFormatWar(false),
      maxModeBwRequired(0)
{
    bandwidth.enum_path.dataValid = false;
    shadow.plugged = false;
    shadow.zombie = false;
    shadow.cableOk = true;
    shadow.hdcpCapDone = false;
    shadow.highestAssessedLC = connector->highestAssessedLC;
    dpMemZero(rawDscCaps, sizeof(rawDscCaps));
}

bool DeviceImpl::isZombie()
{
    // You can't be a zombie if nothing is attached
    if (!(this->isActive()))
        return false;

    if (!plugged)
        return true;

    if (isMustDisconnect())
        return true;

    if (!isMultistream())
    {
        if (connector->bMitigateZombie)
            return true;

        return !connector->willLinkSupportModeSST(connector->highestAssessedLC,
                                                  ((GroupImpl*)activeGroup)->lastModesetInfo);
    }
    else
    {
        return !this->payloadAllocated;
    }
}

bool DeviceImpl::isCableOk()
{
    if (hal->isDpcdOffline())
    {
        // Just say that the cable is ok since we do not have anything connected
        return true;
    }
    else
    {
        return ! (connector->highestAssessedLC.peakRate < connector->getMaxLinkConfig().peakRate &&
            connector->highestAssessedLC.lanes < connector->getMaxLinkConfig().lanes);
    }
}

bool DeviceImpl::isLogical()
{
    if (this->address.size() == 0)
        return false;

    DP_ASSERT((this->address.tail() <= LOGICAL_PORT_END) && "Invalid port number");

    // Logical port numbers of a branching unit are from Port 0x08 up to Port 0xF
    if (this->address.tail() >= LOGICAL_PORT_START)
        return true;

    return false;
}

bool DeviceImpl::isPendingNewDevice()
{
    if (shadow.plugged == plugged)
        return false;

    if (!plugged)
        return false;

    // Delay the newDevice event till all enabled heads are not detached.
    if (connector->policyModesetOrderMitigation && connector->modesetOrderMitigation)
        return false;

    return !connector->linkAwaitingTransition;
}

bool DeviceImpl::isPendingLostDevice()
{
    // marked for lazy exit..to be done now.
    if (complianceDeviceEdidReadTest && lazyExitNow)
        return true;

    if (isZombie())
        return false;

    if (shadow.plugged == plugged)
        return false;

    return !plugged;
}

bool DeviceImpl::isPendingZombie()
{
    if (isZombie() && !shadow.zombie)
        return true;
    else if (!isZombie() && shadow.zombie && plugged)
        return (connector->policyModesetOrderMitigation ? false : true);
    return false;
}

bool DeviceImpl::isPendingHDCPCapDone()
{
    if ((isHDCPCap != Indeterminate) && !shadow.hdcpCapDone)
        return true;
    else
        return false;
}

bool DeviceImpl::isPendingCableOk()
{
    return isCableOk() != shadow.cableOk;
}

bool DeviceImpl::isPendingBandwidthChange()
{
    return shadow.highestAssessedLC != connector->highestAssessedLC;
}

bool DeviceImpl::getI2cData(unsigned offset, NvU8 * buffer, unsigned sizeRequested, unsigned * sizeCompleted, bool bForceMot)
{
    unsigned dataCompleted, sizeRemaining;
    DisplayPort::AuxBus::status status;
    Type transactionType;

    if (!buffer || !sizeCompleted)
        return false;

    dataCompleted = 0;
    *sizeCompleted = 0;
    do
    {
        sizeRemaining = (sizeRequested - *sizeCompleted);
        if ((this->address.size() < 2) && (sizeRemaining > NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE))
        {

            //
            // SST case
            // if the transaction buffer is a multiple of 16 bytes (NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE).
            // Break it to 16 bytes boundary (HW default) and the first transaction sets the middle of
            // transaction bit (MOT). This will mark all the subsequent reads are all of a part of the
            // same transaction (I2C restart).
            //
            status = transaction(AuxBus::read, AuxBus::i2cMot, offset, buffer + *sizeCompleted,
                                 NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE, &dataCompleted);
        }
        else if (sizeRemaining > NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE)
        {

            //
            // MST case
            // For i2c transactions over MST devices, if the transaction buffer is divided into
            // 16 bytes chunks, then read index keeps getting reset for subsequent 16B fetch.
            // Refer Bug: 1233042.
            //
            status = transaction(AuxBus::read, AuxBus::i2cMot, offset, buffer + *sizeCompleted,
                                 sizeRemaining, &dataCompleted);
        }
        else
        {
            //
            // clear the MOT if it is a single transaction or the last bytes of
            // a large, multiple of 16 bytes buffer (end of transaction).
            // Note that for some customer specific needs they might force MOT bit
            // when it shouldn't be set. So check if client forced the MOT bit and honour that.
            //
            transactionType = bForceMot ? AuxBus::i2cMot : AuxBus::i2c;
            status = transaction(AuxBus::read, transactionType, offset, buffer + *sizeCompleted,
                                 sizeRemaining, &dataCompleted);
        }

        if (status != AuxBus::success)
        {
            DP_PRINTF(DP_ERROR, "DPDEV> %s: Failed read transaction", __FUNCTION__);
            break;
        }

        if (dataCompleted == 0)
        {
            // Successfully read 0 bytes? Break out
            break;
        }
        *sizeCompleted += dataCompleted;
    }
    while (*sizeCompleted < sizeRequested);

    return (status == AuxBus::success);
}

bool DeviceImpl::setI2cData(unsigned offset, NvU8 * buffer, unsigned sizeRequested, unsigned * sizeCompleted, bool bForceMot)
{
    unsigned dataCompleted, sizeRemaining;
    DisplayPort::AuxBus::status status;
    Type transactionType;

    if (!buffer || !sizeCompleted)
        return false;

    dataCompleted = 0;
    *sizeCompleted = 0;

    //
    // If the hop count is one, we're asking for DPCD to the root node.
    // If hop count is zero, this is a DP 1.1 target.
    // Hop Count Greater than or equal 2 is when we have a single or multiple branch
    // device/s. This signifies REMOTE_I2C_WRITE transaction case.
    // Here we should not divide the data to 16 byte boundary as if we
    // do, the branch device will not know that it needs to set MOT=1.
    // So we send the entire data up to a max payload of 255 Bytes.
    // Please refer Bug 1964453 for more information.
    //
    if ((this->address.size() >= 2) &&
        (sizeRequested > NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE))
    {
        status = transaction(AuxBus::write, AuxBus::i2cMot, offset, buffer,
                             sizeRequested, &dataCompleted);

        if (status != AuxBus::success)
        {
            DP_PRINTF(DP_ERROR, "DPDEV> %s: Failed write transaction", __FUNCTION__);
            return false;
        }
        *sizeCompleted = dataCompleted;
        DP_ASSERT(*sizeCompleted >= sizeRequested);
        return (status == AuxBus::success);
    }

    do
    {
        sizeRemaining = (sizeRequested - *sizeCompleted);
        if (sizeRemaining > NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE)
        {

            //
            // if the transaction buffer is a multiple of 16 bytes (NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE).
            // Break it to 16 bytes boundary (HW default) and the first transaction sets the middle of
            // transaction bit (MOT). This will mark all the subsequent writes are all of a part of the
            // same transaction (I2C restart).
            //
            status = transaction(AuxBus::write, AuxBus::i2cMot, offset, buffer + *sizeCompleted,
                                 NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE, &dataCompleted);
        }
        else
        {
            //
            // clear the MOT if it is a single transaction or the last bytes of
            // a large, multiple of 16 bytes buffer (end of transaction).
            // Note that for some customer specific needs they might force MOT bit
            // when it shouldn't be set. So check if client forced the MOT bit and honour that.
            //
            transactionType = bForceMot ? AuxBus::i2cMot : AuxBus::i2c;
            status = transaction(AuxBus::write, transactionType, offset, buffer + *sizeCompleted,
                                 sizeRemaining, &dataCompleted);
        }

        if (status != AuxBus::success)
        {
            DP_PRINTF(DP_ERROR, "DPDEV> Failed write transaction");
            break;
        }

        if (dataCompleted == 0)
        {
            // Successfully read 0 bytes? Break out
            break;
        }
        *sizeCompleted += dataCompleted;
    } while (*sizeCompleted < sizeRequested);

    return (status == AuxBus::success);
}

AuxBus::status DeviceImpl::getDpcdData(unsigned offset, NvU8 * buffer,
                                       unsigned sizeRequested,
                                       unsigned * sizeCompleted,
                                       unsigned  * pNakReason)
{
    if (isFakedMuxDevice())
    {
        DP_PRINTF(DP_INFO, "Device is faked, returning nack\n");
        return AuxBus::nack;
    }

    if (!buffer || !sizeCompleted)
    {
        // default param may be NULL
        if (pNakReason) *pNakReason = NakUndefined;
        return AuxBus::nack;
    }

    //
    // Remote DPCD doesn't work for Peer Device 4 i.e. DP-to-Legacy Dongle.
    // But if a virtual DP peer device with Protocol Converter functionality
    // populates the DPCD_Revision field of the LINK_ADDRESS Message reply
    // then allow DPCD transaction
    //
    if ((this->peerDevice == Dongle) && (this->dpcdRevisionMajor == 0))
    {
        if (pNakReason) *pNakReason = NakBadParam;
        return AuxBus::nack;
    }

    return (transaction(AuxBus::read, AuxBus::native, offset, buffer,
                        sizeRequested, sizeCompleted, pNakReason));
}

AuxBus::status DeviceImpl::setDpcdData(unsigned offset, NvU8 * buffer,
                                       unsigned sizeRequested,
                                       unsigned * sizeCompleted,
                                       unsigned  * pNakReason)
{
    if (isFakedMuxDevice())
    {
        DP_PRINTF(DP_INFO, "Device is faked, returning nack\n");
        return AuxBus::nack;
    }

    if (!buffer || !sizeCompleted)
    {
        // default param may be NULL
        if (pNakReason) *pNakReason = NakUndefined;
        return AuxBus::nack;
    }

    //
    // Remote DPCD doesn't work for Peer Device 4 i.e. DP-to-Legacy Dongle
    // But if a virtual DP peer device with Protocol Converter functionality
    // populates the DPCD_Revision field of the LINK_ADDRESS Message reply
    // then allow DPCD transaction
    //
    if ((this->peerDevice == Dongle) && (this->dpcdRevisionMajor == 0))
    {
        if (pNakReason) *pNakReason = NakBadParam;
        return AuxBus::nack;
    }

    return (transaction(AuxBus::write, AuxBus::native, offset, buffer,
                        sizeRequested, sizeCompleted, pNakReason));
}

AuxBus::status DeviceImpl::queryFecData(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags)
{
    if (!fecStatus || !fecErrorCount)
    {
        return AuxBus::nack;
    }

    return (fecTransaction(fecStatus, fecErrorCount, flags));
}

DscCaps DeviceImpl::getDscCaps()
{
    return dscCaps;
}

//
// This function returns the device itself or its parent device that is doing
// DSC decompression for it.
//
Device* DeviceImpl::getDevDoingDscDecompression()
{
    return devDoingDscDecompression;
}

bool DeviceImpl::getRawDscCaps(NvU8 *buffer, NvU32 bufferSize)
{
    if (bufferSize < sizeof(rawDscCaps))
        return false;

    dpMemCopy(buffer, &rawDscCaps, sizeof(rawDscCaps));
    return true;
}

bool DeviceImpl::setRawDscCaps(const NvU8 *buffer, NvU32 bufferSize)
{
    if (bufferSize < sizeof(rawDscCaps))
        return false;

    dpMemCopy(&rawDscCaps, buffer, sizeof(rawDscCaps));
    return parseDscCaps(&rawDscCaps[0], sizeof(rawDscCaps));
}

AuxBus::status DeviceImpl::transaction(Action action, Type type, int address,
                                       NvU8 * buffer, unsigned sizeRequested,
                                       unsigned * sizeCompleted,
                                       unsigned  * pNakReason,
                                       NvU8 offset, NvU8 nWriteTransactions)
{
    // In case of default implementation, the reason for transaction failure
    // must be stored somewhere
    unsigned defaultReason;
    if (!pNakReason) pNakReason = &defaultReason;
    // default failure reason is undefined
    *pNakReason = NakUndefined;

    if (type == AuxBus::i2c || type ==  AuxBus::i2cMot)
    {
        address >>= 1; // right shifted DDC Address (request identifier in spec)
    }

    // If the hop count is one, we're asking for DPCD to the root node.
    // If hop count is zero, this is a DP 1.1 target.
    if (this->address.size() >= 2)
    {
        NakData nak;

        if (connector == NULL || connector->messageManager == NULL)
        {
            return AuxBus::nack;
        }

        if (action == AuxBus::read && type == AuxBus::native)
        {
            RemoteDpcdReadMessage read;
            read.set(this->address.parent(), this->address.tail(), address, sizeRequested);
            if (!connector->messageManager->send(&read, nak)) {
                // Copy reason back to caller
                *pNakReason = nak.reason;
                // Translate the DPCD error codes
                if (nak.reason == NakDefer)
                    return AuxBus::defer;
                if (nak.reason == NakDpcdFail)
                    return AuxBus::nack;

                // This isn't quite right.  We're translating unknown messaging related
                // failure cases into defers.  This is done so that the client will retry the operation
                return AuxBus::defer;
            }

            *sizeCompleted = read.replyNumOfBytesReadDPCD();

            if (*sizeCompleted > sizeRequested) {
                DP_PRINTF(DP_ERROR, "DPDEV> DPCD Read return more data than requested.  Clamping buffer to requested size!");
                *sizeCompleted = sizeRequested;
            }

            dpMemCopy(buffer, read.replyGetData(), *sizeCompleted);

            return AuxBus::success;
        }
        else if ((action == AuxBus::read) && ((type == AuxBus::i2c) || (type == AuxBus::i2cMot)))
        {
            bool isNoStopBit = (type == AuxBus::i2cMot) ? 1:0;
            RemoteI2cReadMessage remoteI2cRead;
            I2cWriteTransaction i2cWriteTransactions[1];
            i2cWriteTransactions[0] = I2cWriteTransaction(address,
                                                          0,
                                                          &offset,
                                                          isNoStopBit);

            if (nWriteTransactions > 1)
            {
                DP_PRINTF(DP_ERROR, "DPDEV> Set function will fail for transactions > 1, please increase the array size!");
                return AuxBus::nack;
            }

            remoteI2cRead.set(this->address.parent(), // topology Address
                nWriteTransactions,                   // number of write transactions
                this->address.tail(),                 // port of Device
                i2cWriteTransactions,                 // list of write transactions
                address,                              // right shifted DDC Address (request identifier in spec)
                sizeRequested);                       // requested size

            if (!connector->messageManager->send(&remoteI2cRead, nak)) {
                // Copy reason back to caller
                *pNakReason = nak.reason;
                // Translate the DPCD error codes
                if (nak.reason == NakI2cNak)
                    return AuxBus::nack;

                // This isn't quite right.  We're translating unknown messaging related
                // failure cases into defers.  This is done so that the client will retry the operation
                return AuxBus::defer;
            }

            *sizeCompleted = remoteI2cRead.replyNumOfBytesReadI2C();

            if (*sizeCompleted > sizeRequested) {
                DP_PRINTF(DP_ERROR, "DPDEV> I2C Read return more data than requested.  Clamping buffer to requested size!");
                *sizeCompleted = sizeRequested;
            }

            dpMemCopy(buffer, remoteI2cRead.replyGetI2CData(sizeCompleted), *sizeCompleted);

            return AuxBus::success;
        }
        else if (action == AuxBus::write && type == AuxBus::native)
        {
            RemoteDpcdWriteMessage write;
            write.set(this->address.parent(), this->address.tail(), address, sizeRequested, buffer);

            if (!connector->messageManager->send(&write, nak)) {
                // Copy reason back to caller
                *pNakReason = nak.reason;
                // Translate the DPCD error codes
                if (nak.reason == NakDefer)
                    return AuxBus::defer;
                if (nak.reason == NakDpcdFail)
                    return AuxBus::nack;

                // This isn't quite right.  We're translating unknown messaging related
                // failure cases into defers.  This is done so that the client will retry the operation
                return AuxBus::defer;
            }

            *sizeCompleted = sizeRequested;

            return AuxBus::success;
        }
        else if ((action == AuxBus::write) && ((type == AuxBus::i2c) || (type == AuxBus::i2cMot)))
        {
            RemoteI2cWriteMessage remoteI2cWrite;

            remoteI2cWrite.set(this->address.parent(),  // topology Address
                this->address.tail(),                   // port of Device
                address,                               // right shifted DDC Address (request identifier in spec)
                sizeRequested,
                buffer);

            if (!connector->messageManager->send(&remoteI2cWrite, nak)) {
                // Copy reason back to caller
                *pNakReason = nak.reason;
                // Translate the DPCD error codes
                if (nak.reason == NakI2cNak)
                    return AuxBus::nack;

                // This isn't quite right.  We're translating unknown messaging related
                // failure cases into defers.  This is done so that the client will retry the operation
                return AuxBus::defer;
            }

            *sizeCompleted = sizeRequested;

            return AuxBus::success;
        }
       else
        {
            DP_ASSERT(0 && "Only aux native and i2c reads and writes supported");
            return AuxBus::nack;
        }
    }
    else
    {
        return this->connector->auxBus->transaction(action, type, address, buffer,
                                                    sizeRequested, sizeCompleted, pNakReason);
    }
}

unsigned DeviceImpl::transactionSize()
{
    //
    //  Remote (DP 1.2) sinks can read much larger chunks at once due to messaging.
    //
    if (this->address.size() >= 2)
        return 255;
    else
        return this->connector->auxBus->transactionSize();
}

static AuxBus::status _QueryFecStatus
(
    DeviceImpl  *bus,
    NvU8        *pStatus
)
{
    AuxBus::status status = AuxBus::success;

    NvU32     addr      = NV_DPCD14_FEC_STATUS;
    unsigned  size      = 1;

    unsigned     sizeCompleted = 0;
    unsigned     pNakReason    = 0;

    status = bus->getDpcdData(addr, pStatus, size, &sizeCompleted, &pNakReason);

    if (status != AuxBus::success)
    {
        DP_PRINTF(DP_ERROR, "DP> Error querying FEC status!");
        return AuxBus::nack;
    }
    return AuxBus::success;
}

static AuxBus::status _QueryFecErrorCount
(
    DeviceImpl  *bus,
    NvU16       *pErrorCount
)
{
    AuxBus::status status = AuxBus::success;
    NvU32     addr      = NV_DPCD14_FEC_ERROR_COUNT;
    unsigned  size      = 2;

    unsigned  sizeCompleted = 0;
    NvU8      cnt[2]    = {0, 0};
    unsigned  pNakReason    = 0;

    status = bus->getDpcdData(addr, &cnt[0], size, &sizeCompleted, &pNakReason);

    if (status != AuxBus::success)
    {
        DP_PRINTF(DP_ERROR, "DP> Error querying FEC error count!");
        return AuxBus::nack;
    }
    else
    {
        *pErrorCount = (((NvU16) cnt[1]) << (sizeof(NvU8) * 8)) | cnt[0];
    }
    return AuxBus::success;
}

static AuxBus::status _WriteFecConfiguration
(
    DeviceImpl  *bus,
    NvU8        configuration
)
{
    AuxBus::status status = AuxBus::success;

    NvU32     addr      = NV_DPCD14_FEC_CONFIGURATION;
    unsigned  size      = 1;

    unsigned  sizeCompleted = 0;
    unsigned  pNakReason    = 0;

    status = bus->setDpcdData(addr, &configuration, size, &sizeCompleted, &pNakReason);

    if (status != AuxBus::success)
    {
        DP_PRINTF(DP_ERROR, "DP> Error setting FEC configuration!");
        return AuxBus::nack;
    }
    return AuxBus::success;
}

AuxBus::status DeviceImpl::fecTransaction(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags)
{
    AuxBus::status status;
    // the capability needs to be checked first (bits 5:0 and 7 need to be set)
    NvU8 data, lane, counter, laneData, offset;
    if (!bFECSupported)
    {
        DP_PRINTF(DP_ERROR, "DP> FEC capability not correct!");
        return nack;
    }

    if (!bFECUncorrectedSupported)
    {
        // check if this counter is specified in the flags
        if (FLD_TEST_DRF(_DP, _UNCORRECTED, _ERROR, _YES, flags))
        {
            for (int i = 0; i < NV_DP_MAX_NUM_OF_LANES; i++)
            {
                for (int j = 0; j < NV_DP_ERROR_COUNTERS_PER_LANE; j++)
                {
                    // specific error value for error
                    fecErrorCount[i][j] = NV_DP_FEC_ERROR_COUNT_INVALID;
                }
            }
            DP_PRINTF(DP_ERROR, "DP> FEC capability not correct!");
            return success;
        }
    }
    if (!bFECCorrectedSupported)
    {
        // check if this counter is specified in the flags
        if (FLD_TEST_DRF(_DP, _CORRECTED, _ERROR, _YES, flags))
        {
            for (int i = 0; i < NV_DP_MAX_NUM_OF_LANES; i++)
            {
                for (int j = 0; j < NV_DP_ERROR_COUNTERS_PER_LANE; j++)
                {
                    // specific error value for error
                    fecErrorCount[i][j] = NV_DP_FEC_ERROR_COUNT_INVALID;
                }
            }
            DP_PRINTF(DP_ERROR, "DP> FEC capability not correct!");
            return success;
        }
    }
    if (!bFECBitSupported)
    {
        // check if this counter is specified in the flags
        if (FLD_TEST_DRF(_DP, _BIT, _ERROR, _YES, flags))
        {
            for (int i = 0; i < NV_DP_MAX_NUM_OF_LANES; i++)
            {
                for (int j = 0; j < NV_DP_ERROR_COUNTERS_PER_LANE; j++)
                {
                    // specific error value for error
                    fecErrorCount[i][j] = NV_DP_FEC_ERROR_COUNT_INVALID;
                }
            }
            DP_PRINTF(DP_ERROR, "DP> FEC capability not correct!");
            return success;
        }
    }
    if (!bFECParityBlockSupported)
    {
        // check if this counter is specified in the flags
        if (FLD_TEST_DRF(_DP, _PARITY_BLOCK, _ERROR, _YES, flags))
        {
            for (int i = 0; i < NV_DP_MAX_NUM_OF_LANES; i++)
            {
                for (int j = 0; j < NV_DP_ERROR_COUNTERS_PER_LANE; j++)
                {
                    // specific error value for error
                    fecErrorCount[i][j] = NV_DP_FEC_ERROR_COUNT_INVALID;
                }
            }
            DP_PRINTF(DP_ERROR, "DP> FEC capability not correct!");
            return success;
        }
    }
    if (!bFECParitySupported)
    {
        // check if this counter is specified in the flags
        if (FLD_TEST_DRF(_DP, _PARITY_BIT, _ERROR, _YES, flags))
        {
            for (int i = 0; i < NV_DP_MAX_NUM_OF_LANES; i++)
            {
                for (int j = 0; j < NV_DP_ERROR_COUNTERS_PER_LANE; j++)
                {
                    // specific error value for error
                    fecErrorCount[i][j] = NV_DP_FEC_ERROR_COUNT_INVALID;
                }
            }
            DP_PRINTF(DP_ERROR, "DP> FEC capability not correct!");
            return success;
        }
    }

    status = _QueryFecStatus(this, fecStatus);
    if(status != AuxBus::success)
    {
        return status;
    }
    // setting configuration for querying error counters for every lane
    for (lane = NV_DPCD14_FEC_CONFIGURATION_LANE_SELECT_LANE_0; lane < connector->activeLinkConfig.lanes; lane++)
    {
        // keeping FEC ready bit
        laneData =  DRF_DEF(_DPCD14, _FEC_CONFIGURATION, _FEC_READY, _YES);
        // selecting specific lane
        laneData |= DRF_NUM(_DPCD14, _FEC_CONFIGURATION, _LANE_SELECT, lane);
        // setting configuration for querying all the error counters for a specific lane
        for (counter = NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_UNCORRECTED_BLOCK_ERROR_COUNT;
             counter <= NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_PARITY_BIT_ERROR_COUNT; counter++)
        {
            // address function for the current register (in the matrix registers start from 0 and in the bit mask from 1)
            offset = counter - 1;
            // if flag for corresponding register is not set skip querying
            if ((flags & NVBIT(offset)) == 0) continue;
            // selecting specific counter
            data = laneData | DRF_NUM(_DPCD14, _FEC_CONFIGURATION, _FEC_ERROR_COUNT_SEL, counter) ;
            status = _WriteFecConfiguration(this, data);
            if (status != AuxBus::success)
            {
                return status;
            }
            // reading specific error counter register based on address function
            status = _QueryFecErrorCount(this, fecErrorCount[lane] + offset);
            if (status != AuxBus::success)
            {
                return status;
            }
        }
    }
    return AuxBus::success;
}

// Apply DPCD overrides if required
void DeviceImpl::dpcdOverrides()
{
    if (this->parent)
    {
        //
        // Device is behind a branch. SW can't perform overrides as branch will
        // handle link training the device not source. Also hal can only override
        // capability of sink, not the individual device behind the branch.
        //
        return;
    }
    if (processedEdid.WARFlags.overrideMaxLaneCount)
    {
        hal->overrideMaxLaneCount(processedEdid.WARData.maxLaneCount);
    }
    if (processedEdid.WARFlags.skipCableBWCheck)
    {
        hal->skipCableBWCheck(processedEdid.WARData.maxLaneAtHighRate,
                              processedEdid.WARData.maxLaneAtLowRate);
    }
    if (processedEdid.WARFlags.overrideOptimalLinkCfg)
    {
        LinkRate optimalLinkRate = 0;

        switch(processedEdid.WARData.optimalLinkRate)
        {
            case 0x6:
                optimalLinkRate = dp2LinkRate_1_62Gbps;
                break;
            case 0xa:
                optimalLinkRate = dp2LinkRate_2_70Gbps;
                break;
            case 0x14:
                optimalLinkRate = dp2LinkRate_5_40Gbps;
                break;
            case 0x1E:
                optimalLinkRate = dp2LinkRate_8_10Gbps;
                break;
            default:
                optimalLinkRate = dp2LinkRate_1_62Gbps;
                DP_PRINTF(DP_ERROR, "DP-DEV> Invalid link rate supplied. Falling back to RBR");
                break;
        }
        hal->overrideOptimalLinkCfg(optimalLinkRate, processedEdid.WARData.optimalLaneCount);
    }
}

void DeviceImpl::applyOUIOverrides()
{
    // For now we only need this for Synaptic branch.
    if ((this->peerDevice == DownstreamBranch) ||
        (this->peerDevice == UpstreamSourceOrSSTBranch))
    {
        NvU8 buffer[16] = {0};
        unsigned size = 13;                                         // Read 0x500 ~ 0x50C
        unsigned sizeCompleted = 0;
        unsigned nakReason = NakUndefined;

        //
        // Synaptic branch claims it supports MSA override, but some older firmware has problems
        // on their decoder. We need to disable the feature in that case.
        //
        if (AuxBus::success != this->getDpcdData(NV_DPCD_BRANCH_IEEE_OUI, &buffer[0],
                                                 size, &sizeCompleted, &nakReason))
            return;

        // Check Branch IEEE_OUI (0x500h~0x502h) is Synaptic IEEE_OUI (0x90, 0xCC, 0x24)
        if ((buffer[0] == 0x90) && (buffer[1] == 0xCC) && (buffer[2] == 0x24))
        {
            // Check if Device Identification String (0x503~0x506) is "SYNA"
            if ((buffer[3] == 0x53) && (buffer[4] == 0x59) && (buffer[5] == 0x4E) && (buffer[6] == 0x41))
            {
                // For Synaptic VMM5331 and VMM5320, it only support MSA-Over-MST for DP after Firmware 5.4.5
                if (buffer[7] == 0x53)
                {
                    //
                    // This flag will be checked only in DSC Pass through cases (MST).
                    // All Synaptics VMM53XX chips which support pass through can only support
                    // color formats that are listed in 0x69h even in pass through mode.
                    //
                    this->bDscPassThroughColorFormatWar = true;

                    if ((buffer[8] == 0x31) || (buffer[8] == 0x20))
                    {
                        this->bSdpExtCapable = False;

                        //
                        // Check firmware version
                        // 0x50A: FW/SW Major Revision.
                        // 0x50B: FW/SW Minor Revision.
                        // 0x50C: Build Number.
                        //
                        if ((buffer[10] >= 0x06) ||
                            ((buffer[10] == 0x05) && (buffer[11] >= 0x05)) ||
                            ((buffer[10] == 0x05) && (buffer[11] == 0x04) && (buffer[12] >= 0x05)))
                        {
                            this->bSdpExtCapable = True;
                        }
                    }
                }
            }
        }

    }
}

bool DeviceImpl::getAsyncSDPSupported()
{
    NvU8 byte = 0;
    unsigned size = 1;
    unsigned sizeCompleted;
    unsigned nakReason = NakUndefined;
    //
    // On faked mux devices, we cannot check if the device has
    // the capability as we don't have access to aux.
    //
    if (this->isFakedMuxDevice())
    {
        return false;
    }
    // If the capability is queried/set already.
    if (this->bAsyncSDPCapable != Indeterminate)
    {
        return (this->bAsyncSDPCapable == True);
    }
    // Check device capabilities first.
    if (AuxBus::success != this->getDpcdData(NV_DPCD_DOWN_STREAM_PORT, &byte,
                                             size, &sizeCompleted, &nakReason) ||
        (FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _MSA_TIMING_PAR_IGNORED, _NO, byte)))
    {
        this->bAsyncSDPCapable = False;
        return false;
    }
    if (AuxBus::success != this->getDpcdData(NV_DPCD14_DPRX_FEATURE_ENUM_LIST, &byte,
                                             size, &sizeCompleted, &nakReason) ||
        (FLD_TEST_DRF(_DPCD14, _DPRX_FEATURE_ENUM_LIST, _ADAPTIVE_SYNC_SDP_SUPPORTED, _NO, byte)))
    {
        this->bAsyncSDPCapable = False;
        return false;
    }
    if (this->isMultistream())
    {
        // For MST devices, check root branch capability.
        this->bAsyncSDPCapable = hal->getRootAsyncSDPSupported() ? True : False;
    }
    else
    {
        // For SST, it supports Async SDP once reaches here.
        this->bAsyncSDPCapable = True;
    }
    return (this->bAsyncSDPCapable == True);
}

bool DeviceImpl::getSDPExtnForColorimetrySupported()
{
    DeviceImpl *targetDevice = NULL;
    DeviceImpl *parentDevice = NULL;

    NvU8 byte = 0;
    unsigned size = 0;
    unsigned nakReason = NakUndefined;

    //
    // On fakeed mux devices, we cannot check if the device has
    // the capability as we don't have access to aux.
    //
    if (this->isFakedMuxDevice())
    {
        return false;
    }

    // If the capability is queried/set already.
    if (this->bSdpExtCapable != Indeterminate)
    {
        return (this->bSdpExtCapable == True);
    }

    if (!this->isMultistream())
    {
        // If the device is directly connected to the source read the DPCD directly
        this->bSdpExtCapable = hal->getSDPExtnForColorimetry() ? True : False;
        return (this->bSdpExtCapable == True);
    }

    // For MST devices
    switch (this->peerDevice)
    {
        case DownstreamBranch:
        case UpstreamSourceOrSSTBranch:
        {
            targetDevice = this;
            break;
        }
        case DownstreamSink:
        {
            //
            // When the device is type of DownstreamSink and with branch(es)
            // between GPU and it, query goes to the device and its parent
            //
            targetDevice = this;
            parentDevice = (DeviceImpl *)this->getParent();
            break;
        }
        case Dongle:
        {
            //
            // Bug 2527026: When the device is type of dongle and with branch(es)
            // between GPU and it, query goes to its parent.
            //
            targetDevice = (DeviceImpl *)this->getParent();
            break;
        }
        default:
        {
            DP_ASSERT(0 && "Unsupported Peer Type for SDP_EXT COLORIMETRY");
            return false;
            break;
        }
    }

    if (!targetDevice)
    {
        DP_ASSERT(0 && "targetDevice is invalid for SDP_EXT COLORIMETRY");
        return false;
    }

    // Send remote DPCD for devices behind the branch
    if ((AuxBus::success == targetDevice->getDpcdData(NV_DPCD_TRAINING_AUX_RD_INTERVAL,
                                                      &byte, sizeof byte, &size, &nakReason)) &&
       (FLD_TEST_DRF(_DPCD14, _TRAINING_AUX_RD_INTERVAL, _EXTENDED_RX_CAP, _YES, byte)))
    {
        byte = 0;
        size = 0;
        nakReason = NakUndefined;

        if (AuxBus::success == targetDevice->getDpcdData(NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST,
                                                     &byte, sizeof byte, &size, &nakReason))
        {
            this->bSdpExtCapable = FLD_TEST_DRF(_DPCD14,
                                                _EXTENDED_DPRX_FEATURE_ENUM_LIST,
                                                _VSC_SDP_EXT_FOR_COLORIMETRY,
                                                _YES, byte) ? True : False;
        }
    }

    if (parentDevice && (this->bSdpExtCapable == True))
    {
        //
        // Do not override bSdpExtCapable for the sink. Although result won't
        // change but we can keep the value for debug purpose.
        //
        return parentDevice->getSDPExtnForColorimetrySupported();
    }

    return (this->bSdpExtCapable == True);
}

bool DeviceImpl::getPanelFwRevision(NvU16 *revision)
{
    NvU8 fwRevisionMajor   = 0;
    NvU8 fwRevisionMinor   = 0;
    unsigned size          = 0;
    unsigned nakReason     = NakUndefined;

    if (!revision)
    {
        return false;
    }

    *revision = 0;

    //
    // On faked mux devices, we cannot check if the device has
    // the capability as we don't have access to aux.
    //
    if (this->isFakedMuxDevice())
    {
        return false;
    }

    if (AuxBus::success != this->getDpcdData(NV_DPCD14_FW_SW_REVISION_MAJOR,
                                             &fwRevisionMajor, sizeof(fwRevisionMajor), &size, &nakReason))
    {
        return false;
    }

    if (AuxBus::success != this->getDpcdData(NV_DPCD14_FW_SW_REVISION_MINOR,
                                             &fwRevisionMinor, sizeof(fwRevisionMinor), &size, &nakReason))
    {
        return false;
    }

    *revision = (fwRevisionMajor << 8) | fwRevisionMinor;

    return true;
}

bool DeviceImpl::isPowerSuspended()
{
    bool bPanelPowerOn, bDPCDPowerStateD0;
    if (connector->main->isEDP())
    {
        connector->main->getEdpPowerData(&bPanelPowerOn, &bDPCDPowerStateD0);
        return !bDPCDPowerStateD0;
    }
    return (connector->hal->getPowerState() == PowerStateD3);
}

void DeviceImpl::setPanelPowerParams(bool bSinkPowerStateD0, bool bPanelPowerStateOn)
{
    bool bPanelPowerOn, bDPCDPowerStateD0;
    GroupImpl * pGroupAttached = connector->getActiveGroupForSST();

    //
    // For single head dual SST mode, set the panel power params for the
    // secondary connector while updating the primary connector.
    //
    if (pGroupAttached &&
        connector->pCoupledConnector &&
        (pGroupAttached->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST) &&
        (pGroupAttached->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY))
    {
        return;
    }

    if (connector->main->isEDP())
    {
        connector->main->getEdpPowerData(&bPanelPowerOn, &bDPCDPowerStateD0);
    }
    else
    {
        bDPCDPowerStateD0 = (connector->hal->getPowerState() == PowerStateD0)?
                            true : false;
    }

    // Going to Suspend (D3)
    if (!bSinkPowerStateD0)
    {
        if (this->bypassDpcdPowerOff())
        {
            DP_PRINTF(DP_NOTICE, "DP-DEV> Bypassing 600h write for this display");
            return;
        }

        if (connector->main->isEDP())
        {
            /*
             * If it's an eDP panel, the setPowerState call below will turn on LCD_POWER
             * if it's already off. So only call the function when panel power is on
             * and DPCD_SET_POWER is set to _D0.
             */
             if (bPanelPowerOn && bDPCDPowerStateD0)
            {
                // monitor to be put to sleep
                if (connector->hal->setPowerState(PowerStateD3))
                    shadow.highestAssessedLC = connector->highestAssessedLC;
            }
        }
        else
        {

            if (connector->pCoupledConnector)
            {
                // Put secondary connctor to sleep
                connector->pCoupledConnector->hal->setPowerState(PowerStateD3);
            }

            // monitor to be put to sleep
            if (connector->hal->setPowerState(PowerStateD3))
            {
                shadow.highestAssessedLC = connector->highestAssessedLC;
            }
        }
        //
        // If bPanelPowerStateOn is false and this
        // is not a multistream device, then shut down the main link. Some eDP
        // panels are known to need this in order to actually shut down.
        //
        if (!isMultistream() && !bPanelPowerStateOn)
        {
            if (connector->pCoupledConnector)
            {
                // configure power state on secondary
                connector->pCoupledConnector->main->configurePowerState(false);
            }
            connector->main->configurePowerState(false);
        }
    }
    else
    {
        if (connector->main->isEDP() && !bPanelPowerOn)
        {
            // Turn on the eDP panel if required.
            connector->main->configurePowerState(true);
        }
        // monitor to be brought out of sleep
        if (connector->hal->setPowerState(PowerStateD0))
        {
            if (connector->pCoupledConnector)
            {
                // power up main link on secondary
                connector->pCoupledConnector->hal->setPowerState(PowerStateD0);
            }

            // Mark linkStatus as dirty as we need to read linkStatus again since we are resuming a power state D0, link might have lost.
            connector->hal->setDirtyLinkStatus(true);
            if (connector->pCoupledConnector)
            {
                connector->pCoupledConnector->hal->setDirtyLinkStatus(true);
            }

            if (connector->activeGroups.isEmpty())
            {
                return;
            }
            if ((!connector->isLinkActive()) ||
                (connector->main->isEDP() && !bPanelPowerOn) ||
                (connector->isLinkLost()) ||
                (!bDPCDPowerStateD0))
            {
                //
                // If link is inactive, lost, or the panel was off before, then
                // assess Link. Note that this'll detach head if required.
                //
                if (pGroupAttached &&
                    pGroupAttached->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)
                {
                    // Special handling for single head dual SST cases
                    connector->trainSingleHeadMultipleSSTLinkNotAlive(pGroupAttached);
                }
                else
                {
                    connector->assessLink();
                }
            }
        }
        else
            DP_ASSERT(0 && "Could not bring the monitor back from sleep.");
    }
}

void DeviceImpl::switchToComplianceFallback()
{
    Edid fallbackEdid;
    makeEdidFallback(fallbackEdid);
    this->processedEdid.resetData();
    this->processedEdid = fallbackEdid;
}

TriState DeviceImpl::hdcpAvailableHop()
{
    return this->isHDCPCap;
}

TriState DeviceImpl::hdcpAvailable()
{
    if (isNativeDPCD())
    {
        return this->hdcpAvailableHop();
    }
    return False;
}

void DeviceImpl::resetCacheInferredLink()
{
    this->bandwidth.enum_path.dataValid = false;
}

LinkConfiguration * DeviceImpl::inferLeafLink(unsigned * totalLinkSlots)
{
    // update the EPR data
    if (!bandwidth.enum_path.dataValid)
    {
        if (plugged)
        {
            NakData nack;
            for (unsigned retries = 0; retries < 7; retries++)
            {
                EnumPathResMessage epr(getTopologyAddress().parent(), getTopologyAddress().tail(), true);
                bool sendStatus = connector->messageManager->send(&epr, nack);
                if (!sendStatus)
                {
                    if (nack.reason == NakDefer || nack.reason == NakTimeout)
                        continue;

                    bandwidth.enum_path.total = 0;
                    bandwidth.enum_path.free = 0;
                    bandwidth.enum_path.availableStreams = 0;
                    break;
                }
                else
                {
                    bandwidth.enum_path.total = epr.reply.TotalPBN;
                    bandwidth.enum_path.free = epr.reply.FreePBN;
                    bandwidth.enum_path.bPathFECCapable = epr.reply.bFECCapability;
                    bandwidth.enum_path.availableStreams = epr.reply.availableStreams;

                    break;
                }
            }
        }
        else
        {
            bandwidth.enum_path.total = bandwidth.enum_path.free = 0;
        }

        bandwidth.enum_path.dataValid = true;
        bandwidth.lastHopLinkConfig = LinkConfiguration(bandwidth.enum_path.total);
        // Update FEC support of the device after EPR
        this->getFECSupport();
    }

    if (totalLinkSlots)
    {
        *totalLinkSlots = bandwidth.lastHopLinkConfig.slotsForPBN(bandwidth.enum_path.total, true /*epr aware*/);

        //
        // Override the totalLinkSlots returned to 63 only if peer device is
        // 2 (branch), since TS-0 will be used for MTP header.
        // Branch may return the total pbn corresponding to 64 timeslots.
        //
        if (*totalLinkSlots == 64 && peerDevice ==  DownstreamBranch)
        {
            *totalLinkSlots = 63;
        }
    }

    return &bandwidth.lastHopLinkConfig;
}

void DeviceImpl::inferPathConstraints()
{
    if (!bandwidth.enum_path.availablePbnUpdated)
    {
        if (plugged)
        {
            NakData nack;
            for (unsigned retries = 0; retries < 7; retries++)
            {
                // Marking the EPR as a path message in 2x
                EnumPathResMessage epr(getTopologyAddress().parent(), getTopologyAddress().tail(), false);
                bool sendStatus = connector->messageManager->send(&epr, nack);
                if (!sendStatus)
                {
                    if (nack.reason == NakDefer || nack.reason == NakTimeout)
                        continue;

                    bandwidth.enum_path.total = 0;
                    bandwidth.enum_path.free = 0;
                    bandwidth.enum_path.availableStreams = 0;
                    bandwidth.enum_path.dfpLinkAvailable = 0;
                    break;
                }
                else
                {
                    bandwidth.enum_path.total = epr.reply.TotalPBN;
                    bandwidth.enum_path.free = epr.reply.FreePBN;
                    bandwidth.enum_path.bPathFECCapable = epr.reply.bFECCapability;
                    bandwidth.enum_path.availableStreams = epr.reply.availableStreams;
                    // Include the new DFP available PBN only for 2x
                    bandwidth.enum_path.dfpLinkAvailable = epr.reply.DFPLinkAvailablePBN;

                    break;
                }
            }
        }
        else
        {
            bandwidth.enum_path.total = bandwidth.enum_path.free = bandwidth.enum_path.dfpLinkAvailable = 0;
        }

        bandwidth.enum_path.dataValid = true;
        bandwidth.enum_path.availablePbnUpdated = true;
        bandwidth.lastHopLinkConfig = LinkConfiguration(DP_MIN(bandwidth.enum_path.total, bandwidth.enum_path.dfpLinkAvailable));
        // Update FEC support of the device after EPR
        this->getFECSupport();
    }
    return;
}

bool DeviceImpl::isActive()
{
    DP_ASSERT(!activeGroup || activeGroup->isHeadAttached());
    return activeGroup != NULL;
}

bool DeviceImpl::getRawEpr(unsigned * totalEpr, unsigned * freeEpr, rawEprState eprState)
{
    DP_ASSERT((totalEpr && freeEpr) && "Invalid arguments passed to function getRawEpr()");
    bool status = true;
    *totalEpr = 0;
    *freeEpr = 0;

    // If request has come for main link/Native branch device
    // return main link PBNs as "0" & return
    if (isNativeDPCD())
        return status;

    // Cached/Software state is queried
    if (eprState == software)
    {
        *totalEpr = bandwidth.enum_path.total;
        *freeEpr = bandwidth.enum_path.free;

        return status;
    }

    // Hardware state is queried. Send a new EPR message to get the current state
    EnumPathResMessage rawEpr(getTopologyAddress().parent(), getTopologyAddress().tail(), true);
    NakData nack;
    for (unsigned retries = 0; retries < 7; retries++)
    {
        bool sendStatus = connector->messageManager->send(&rawEpr, nack);
        if (!sendStatus)
        {
            status = false;
            if (nack.reason == NakDefer)
                continue;

            DP_PRINTF(DP_ERROR, "DP-DEV> EPR message failed while getting RAW EPR");

            break;
        }
        else
        {
            *totalEpr = rawEpr.reply.TotalPBN;
            *freeEpr = rawEpr.reply.FreePBN;
            status = true;

            break;
        }
    }

    return status;
}

unsigned DeviceImpl::getEDIDSize() const
{
    // Return DDC EDID size only if we got a valid EDID there
    if (this->connector->isAcpiInitDone() && ddcEdid.isValidHeader())
    {
        return ddcEdid.getEdidSize();
    }
    else
    {
        return processedEdid.getEdidSize();
    }
}

bool DeviceImpl::getEDID(char * buffer, unsigned size) const
{
    //
    // Return DDC EDID only if we got a valid EDID there
    // This has priority on regular EDID read from panel
    //
    if (this->connector->isAcpiInitDone() && ddcEdid.isValidHeader())
    {
        if (size < ddcEdid.getEdidSize())
            goto panelEdid;

        dpMemCopy(buffer, ddcEdid.getBuffer()->getData(), ddcEdid.getEdidSize());
        return true;
    }

panelEdid:
    // No EDID read from SBIOS. Return panel EDID now.
    if (size < processedEdid.getEdidSize())
        return false;

    dpMemCopy(buffer, processedEdid.getBuffer()->getData(), processedEdid.getEdidSize());
    return true;
}

unsigned DeviceImpl::getRawEDIDSize() const
{
    // Return DDC EDID size only if we got a valid EDID there
    if (this->connector->isAcpiInitDone() && ddcEdid.isValidHeader())
    {
        return ddcEdid.getEdidSize();
    }
    else
    {
        return rawEDID.getEdidSize();
    }
}

bool DeviceImpl::getRawEDID(char * buffer, unsigned size) const
{
    //
    // Return DDC EDID only if we got a valid EDID there
    // This has priority on regular EDID read from panel
    //
    if (this->connector->isAcpiInitDone() && ddcEdid.isValidHeader())
    {
        if (size >= ddcEdid.getEdidSize())
        {
            dpMemCopy(buffer, ddcEdid.getBuffer()->getData(), ddcEdid.getEdidSize());
            return true;
        }
    }

    // No EDID read from SBIOS. Return panel EDID now.
    if (size < rawEDID.getEdidSize())
        return false;

    dpMemCopy(buffer, rawEDID.getBuffer()->getData(), rawEDID.getEdidSize());
    return true;
}

bool DeviceImpl::startVrrEnablement()
{
    bool ret = false;

    if (vrrEnablement)
    {
        ret = vrrEnablement->start();
    }

    return ret;
}

void DeviceImpl::resetVrrEnablement()
{
    if (vrrEnablement)
    {
        vrrEnablement->reset();
    }
}

bool DeviceImpl::isVrrMonitorEnabled()
{
    bool ret = false;

    if (vrrEnablement)
    {
        ret = vrrEnablement->isMonitorEnabled();
    }

    return ret;
}

bool DeviceImpl::isVrrDriverEnabled()
{
    bool ret = false;

    if (vrrEnablement)
    {
        ret = vrrEnablement->isDriverEnabled();
    }

    return ret;
}

NvBool DeviceImpl::getDSCSupport()
{
    NvU8 byte          = 0;
    unsigned size      = 0;
    unsigned nakReason = NakUndefined;
    Address::StringBuffer sb;
    DP_USED(sb);

    dscCaps.bDSCSupported = false;

    if(AuxBus::success == this->getDpcdData(NV_DPCD14_DSC_SUPPORT,
        &byte, sizeof(byte), &size, &nakReason))
    {
        if (FLD_TEST_DRF(_DPCD14, _DSC_SUPPORT, _DECOMPRESSION, _YES, byte))
        {
            dscCaps.bDSCDecompressionSupported = true;
        }
        if (FLD_TEST_DRF(_DPCD20, _DSC_SUPPORT, _PASS_THROUGH, _YES, byte))
        {
            dscCaps.bDSCPassThroughSupported = true;
        }
    }

    else
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> DSC Support AUX READ failed for %s!", address.toString(sb));
    }

    if (dscCaps.bDSCDecompressionSupported || dscCaps.bDSCPassThroughSupported)
    {
        dscCaps.bDSCSupported = true;
    }

    return dscCaps.bDSCSupported;
}

bool DeviceImpl::isPanelReplaySupported()
{
    return prCaps.bPanelReplaySupported;
}

NvBool DeviceImpl::isSelectiveUpdateSupported()
{
    return prCaps.bSelUpdateSupported;
}

NvBool DeviceImpl::isEarlyRegionTpSupported()
{
    return prCaps.bEarlyRegionTpSupported;
}

NvBool DeviceImpl::isAdaptiveSyncSdpNotSupportedInPr()
{
    return prCaps.bAdaptiveSyncSdpNotSupportedInPr;
}

NvBool DeviceImpl::isdscDecodeNotSupportedInPr()
{
    return prCaps.bDscDecodeNotSupportedInPr;
}

NvBool DeviceImpl::isLinkOffSupportedAfterAsSdpInPr()
{
    return prCaps.bLinkOffSupportAfterAsSdpSent;
}

SelectiveUpdateCaps DeviceImpl::getSelectiveUpdateCaps()
{
    return prCaps.suCaps;
}

NvBool DeviceImpl::isAuxLessAlpmSupported()
{
    return alpmCaps.bAuxLessAlpmSupported;
}

void  DeviceImpl::getPanelReplayCaps()
{
    NvU8 buffer[10]    = {0U};
    unsigned size;
    unsigned sizeCompleted;
    unsigned nakReason = NakUndefined;

    size = 1U;
    if (AuxBus::success == this->getDpcdData(NV_DPCD20_PANEL_REPLAY_CAPABILITY,
        &buffer[0], size, &sizeCompleted, &nakReason))
    {
        prCaps.bPanelReplaySupported =
            FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY, _SUPPORTED,
            _YES, buffer[0]);
        prCaps.bSelUpdateSupported =
            FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY, _SEL_UPDATE,
            _YES, buffer[0]);
        prCaps.bEarlyRegionTpSupported =
            FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY, _EARLY_TRANSPORT,
            _YES, buffer[0]);

        size = 1U;
        // Check Additional Panel replay caps register
        if (AuxBus::success == this->getDpcdData(NV_DPCD20_PANEL_REPLAY_CAPABILITY_ADD,
            &buffer[1], size, &sizeCompleted, &nakReason))
        {
            prCaps.bDscDecodeNotSupportedInPr =
                FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY_ADD,
                _DSC_DECODE_NOT_SUPPORTED_IN_PR, _YES, buffer[1]);
            prCaps.bAdaptiveSyncSdpNotSupportedInPr =
                FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY_ADD,
                _ASYNC_VIDEOTIMING_NOT_SUPPORTED_IN_PR, _YES, buffer[1]);
            prCaps.bLinkOffSupportAfterAsSdpSent =
                FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY_ADD,
                _LINK_OFF_SUPPORTED_IN_PR_AFTER_ADAPT_SYNC_SDP, _YES, buffer[1]);
            prCaps.suCaps.bDscCrcOfMultipleSuSupported =
                FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY_ADD,
                _DSC_CRC_MULTIPLE_SUS_SUPPORTED, _YES, buffer[1]);
            prCaps.suCaps.bSelUpdateGranularityNeeded =
                FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY_ADD,
                _PR_SEL_UPDATE_GRANULARITY_NEEDED, _YES, buffer[1]);

            if (prCaps.suCaps.bSelUpdateGranularityNeeded)
            {
                size = 3U;
                if (AuxBus::success ==
                    this->getDpcdData(NV_DPCD20_PANEL_REPLAY_SU_X_GRANULARITY_CAPABILITY,
                    &buffer[2], size, &sizeCompleted, &nakReason))
                {
                    prCaps.suCaps.selUpdateXGranularityCap = DRF_VAL(_DPCD20_PANEL, _REPLAY_SU_X_GRANULARITY,
                    _CAPABILITY_VAL, buffer[2]);

                    prCaps.suCaps.selUpdateXGranularityCap1 = DRF_VAL(_DPCD20_PANEL, _REPLAY_SU_X,
                    _GRANULARITY_CAPABILITY1_VAL, buffer[3]);

                    prCaps.suCaps.selUpdateYGranularityCap = DRF_VAL(_DPCD20_PANEL, _REPLAY_SU_Y,
                    _GRANULARITY_CAPABILITY_VAL, buffer[4]);
                }
                else
                {
                    DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read to DPCD offset 0xB2-0xB4 failed!");
                }
            }

            prCaps.suCaps.bSuYGranularityExtendedCap =
                FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CAPABILITY_ADD,
                _SU_Y_GRANULARITY_EXT_CAP_SUPPORTED, _YES, buffer[1]);

            if (prCaps.suCaps.bSuYGranularityExtendedCap)
            {
                size = 2U;
                if (AuxBus::success ==
                    this->getDpcdData(NV_DPCD20_PANEL_REPLAY_SU_Y_GRANULARITY_EXTENDED_CAPABILITY,
                    &buffer[5], size, &sizeCompleted, &nakReason))
                {
                    prCaps.suCaps.selUpdateYGranularityExtCap = DRF_VAL(_DPCD20_PANEL, _REPLAY_SU_Y,
                    _GRANULARITY_EXTENDED_CAPABILITY_VAL, buffer[5]);

                    prCaps.suCaps.selUpdateYGranularityExtCap1 = DRF_VAL(_DPCD20_PANEL, _REPLAY_SU_Y,
                    _GRANULARITY_EXTENDED_CAPABILITY1_VAL, buffer[6]);
                }
                else
                {
                    DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read to DPCD offset 0xB5 failed!");
                }
            }
        }
        else
        {
            DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read to DPCD offset 0xB1 failed!");
        }
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read to DPCD offset 0xB0 failed!");
    }
}

bool DeviceImpl::setPanelReplayConfig(panelReplayConfig prcfg)
{
    NvU8 config = 0U;
    unsigned size = 0U;
    unsigned nakReason = NakUndefined;

    if (prcfg.enablePanelReplay)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_PR_MODE, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_PR_MODE, _NO, config);
    }

    if (prcfg.bEnableCrcWithPr)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_CRC, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_CRC, _NO, config);
    }

    if (prcfg.bHpdOnAdaptiveSyncSdpMissing)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_ADAPTIVE_SYNC_SDP_MISSING, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_ADAPTIVE_SYNC_SDP_MISSING, _NO, config);
    }

    if (prcfg.bHpdOnSdpUncorrectableError)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_SDP_UNCORRECTABLE_ERROR, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_SDP_UNCORRECTABLE_ERROR, _NO, config);
    }

    if (prcfg.bHpdOnRfbStorageErrors)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_RFB_STORAGE_ERRORS, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_RFB_STORAGE_ERRORS, _NO, config);
    }

    if (prcfg.bHpdOnRfbActiveFrameCrcError)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_RFB_ACTIVE_FRAME_CRC_ERROR, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _HPD_RFB_ACTIVE_FRAME_CRC_ERROR, _NO, config);
    }

    if (prcfg.bEnableSelectiveUpdate)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_SELECTIVE_UPDATE, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_SELECTIVE_UPDATE, _NO, config);
    }

    if (prcfg.bSuRegionEarlyTpEnable)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_SU_REGION_EARLY_TRANSPORT, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_SU_REGION_EARLY_TRANSPORT, _NO, config);
    }

    if (AuxBus::success !=
        this->setDpcdData(NV_DPCD20_PANEL_REPLAY_CONFIGURATION,
        &config, sizeof(config), &size, &nakReason))
    {
        return false;
    }

    config = 0U;
    if (prcfg.bSinkRrUnlockGranted)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SINK_RR_UNLOCK_GRANTED, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SINK_RR_UNLOCK_GRANTED, _NO, config);
    }

    if (prcfg.bSelUpdateYExtValEnable)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SU_Y_GRANULARITY_EXTENDED_VALUE, _YES, config);

        config = FLD_SET_DRF_NUM(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SU_Y_GRANULARITY_EXTENDED_VALUE_SELECTION, prcfg.selUpdateYExtVal, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SU_Y_GRANULARITY_EXTENDED_VALUE, _NO, config);
    }

    if (prcfg.bSuRegionScanLineIndicate)
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SU_REGION_SCANLINE_CAPTURE_INDICATION, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
            _SU_REGION_SCANLINE_CAPTURE_INDICATION, _NO, config);
    }

    if (AuxBus::success !=
        this->setDpcdData(NV_DPCD20_PANEL_REPLAY_CONFIGURATION2,
        &config, sizeof(config), &size, &nakReason))
    {
        return false;
    }

    config = 0U;
    config = FLD_SET_DRF_NUM(_DPCD20_PANEL, _REPLAY_CONFIGURATION3,
        _AS_SDP_SETUP_CONFIG_PR_ACTIVE_TIME, prcfg.asSdpSetUpTimePrActive, config);

    if (AuxBus::success !=
        this->setDpcdData(NV_DPCD20_PANEL_REPLAY_CONFIGURATION3,
        &config, sizeof(config), &size, &nakReason))
    {
        return false;
    }

    return true;
}

bool DeviceImpl::getPanelReplayConfig(panelReplayConfig *pPrcfg)
{
    NvU8 config = 0U;
    unsigned size = 0U;
    unsigned nakReason = NakUndefined;

    if (AuxBus::success !=
        this->getDpcdData(NV_DPCD20_PANEL_REPLAY_CONFIGURATION,
        &config, sizeof(config), &size, &nakReason))
    {
        return false;
    }

    pPrcfg->enablePanelReplay = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _ENABLE_PR_MODE, _YES, config);

    pPrcfg->bEnableCrcWithPr = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _ENABLE_CRC, _YES, config);

    pPrcfg->bHpdOnAdaptiveSyncSdpMissing = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _HPD_ADAPTIVE_SYNC_SDP_MISSING, _YES, config);

    pPrcfg->bHpdOnSdpUncorrectableError = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _HPD_SDP_UNCORRECTABLE_ERROR, _YES, config);

    pPrcfg->bHpdOnRfbStorageErrors = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _HPD_RFB_STORAGE_ERRORS, _YES, config);

    pPrcfg->bHpdOnRfbActiveFrameCrcError = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _HPD_RFB_ACTIVE_FRAME_CRC_ERROR, _YES, config);


    pPrcfg->bEnableSelectiveUpdate = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
        _ENABLE_SELECTIVE_UPDATE, _YES, config);

    pPrcfg->bSuRegionEarlyTpEnable = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION,
            _ENABLE_SU_REGION_EARLY_TRANSPORT, _YES, config);

   if (AuxBus::success !=
        this->getDpcdData(NV_DPCD20_PANEL_REPLAY_CONFIGURATION2,
        &config, sizeof(config), &size, &nakReason))
    {
        return false;
    }

    pPrcfg->bSinkRrUnlockGranted = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
        _SINK_RR_UNLOCK_GRANTED, _YES, config);

    pPrcfg->bSelUpdateYExtValEnable =  FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
        _SU_Y_GRANULARITY_EXTENDED_VALUE, _YES, config);

    pPrcfg->selUpdateYExtVal = DRF_VAL(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
        _SU_Y_GRANULARITY_EXTENDED_VALUE_SELECTION, config);

    pPrcfg->bSuRegionScanLineIndicate = FLD_TEST_DRF(_DPCD20_PANEL, _REPLAY_CONFIGURATION2,
        _SU_REGION_SCANLINE_CAPTURE_INDICATION, _YES, config);

    if (AuxBus::success !=
        this->getDpcdData(NV_DPCD20_PANEL_REPLAY_CONFIGURATION3,
        &config, sizeof(config), &size, &nakReason))
    {
        return false;
    }

    pPrcfg->asSdpSetUpTimePrActive = DRF_VAL(_DPCD20_PANEL, _REPLAY_CONFIGURATION3,
        _AS_SDP_SETUP_CONFIG_PR_ACTIVE_TIME, config);

    return true;
}

bool DeviceImpl::getPanelReplayStatus(PanelReplayStatus *pPrStatus)
{
    NvU8 state = 0U;
    unsigned size = 0U;
    unsigned nakReason = NakUndefined;

    if (pPrStatus == NULL)
    {
        DP_ASSERT(0);
        return false;
    }

    if (AuxBus::success == this->getDpcdData(NV_DPCD20_PANEL_REPLAY_AND_FRAME_LOCK_STATUS,
        &state, sizeof(state), &size, &nakReason))
    {
        switch (DRF_VAL(_DPCD20, _PANEL_REPLAY_AND_FRAME_LOCK_STATUS, _PR_STATUS, state))
        {
            case NV_DPCD20_PANEL_REPLAY_AND_FRAME_LOCK_STATUS_PR_STATUS_STATE_0:
            pPrStatus->prState = PanelReplay_Inactive;
            break;

            case NV_DPCD20_PANEL_REPLAY_AND_FRAME_LOCK_STATUS_PR_STATUS_STATE_1:
            pPrStatus->prState = PanelReplay_CaptureAndDisplay;
            break;

            case NV_DPCD20_PANEL_REPLAY_AND_FRAME_LOCK_STATUS_PR_STATUS_STATE_2:
            pPrStatus->prState = PanelReplay_DisplayFromRfb;
            break;

            default:
            pPrStatus->prState = PanelReplay_Undefined;
            break;
        }
        return true;
    }

    return false;
}

void DeviceImpl::getAlpmCaps()
{
    NvU8     byte      = 0U;
    unsigned size      = 0U;
    unsigned nakReason = NakUndefined;

    if (AuxBus::success ==
        this->getDpcdData(NV_DPCD20_RECEIVER_ALPM_CAPABILITIES,
        &byte, sizeof(byte), &size, &nakReason))
    {
        alpmCaps.bFwStandbySupported =
           FLD_TEST_DRF(_DPCD20_RECEIVER, _ALPM_CAPABILITIES,
           _FW_STANDBY_SUPPORT, _YES, byte);
        alpmCaps.bAuxLessAlpmSupported =
           FLD_TEST_DRF(_DPCD20_RECEIVER, _ALPM_CAPABILITIES,
           _AUX_LESS_ALPM_CAP, _YES, byte);
        alpmCaps.bAuxLessAlpmPhySleepSupported =
            FLD_TEST_DRF(_DPCD20_RECEIVER, _ALPM_CAPABILITIES,
            _AUX_LESS_ALPM_ML_PHY_SLEEP_SUPPORT, _YES, byte);
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read to DPCD offset 0x2E failed!");
    }
}

NvBool DeviceImpl::setAlpmConfig(AlpmConfig alpmCfg)
{
    NvU8     config = 0U;
    unsigned size = 0U;
    unsigned nakReason = NakUndefined;

    if (alpmCfg.bEnableAlpm)
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _ENABLE_ALPM, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _ENABLE_ALPM, _NO, config);
    }

    if (alpmCfg.bHpdOnAlpmLockError)
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _IRQ_HPD_ON_ALPM_LOCK_ERROR, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _IRQ_HPD_ON_ALPM_LOCK_ERROR, _NO, config);
    }

    if (alpmCfg.bSelectedAlpmMode)
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _ALPM_SELECTED_MODE, _AUX_LESS_ALPM, config);
    }

    if (alpmCfg.bAcdsPeriodDuration)
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _ACDS_PERIOD_DURATION, _YES, config);
    }
    else
    {
        config = FLD_SET_DRF(_DPCD20_RECEIVER, _ALPM_CONFIGURATION,
            _ACDS_PERIOD_DURATION, _NO, config);
    }

    if (AuxBus::success ==
        this->setDpcdData(NV_DPCD20_RECEIVER_ALPM_CONFIGURATION,
        &config, sizeof(config), &size, &nakReason))
    {
        return true;
    }

    return false;
}

NvBool DeviceImpl::getAlpmStatus(AlpmStatus *pAlpmStatus)
{
    NvU8 byte = 0U;
    unsigned size = 0U;
    unsigned nakReason = NakUndefined;

    if (pAlpmStatus == NULL)
    {
        DP_ASSERT(0);
        return false;
    }

    if (AuxBus::success == this->getDpcdData(NV_DPCD20_RECEIVER_ALPM_STATUS,
        &byte, sizeof(byte), &size, &nakReason))
    {
        pAlpmStatus->bAuxlessAlpmLockTimeout =
            FLD_TEST_DRF(_DPCD20_RECEIVER, _ALPM_STATUS,
            _AUX_LESS_ALPM_LOCK_TIMEOUT_ERR, _YES, byte);
        pAlpmStatus->bAuxlessAlpmPhySleepDetected =
            FLD_TEST_DRF(_DPCD20_RECEIVER, _ALPM_STATUS,
            _AUX_LESS_ALPM_ML_PHY_SLEEP_DETECTED, _YES, byte);
        return true;
    }

    return false;
}

NvBool DeviceImpl::enableAdaptiveSyncSdp(NvBool bEnable)
{
    NvU8     byte = 0U;
    unsigned size = 0U;
    unsigned nakReason = NakUndefined;
    AuxBus::status status;

    status = this->getDpcdData(NV_DPCD_DOWNSPREAD_CTRL,
                               &byte, sizeof byte, &size, &nakReason);
    if (status == AuxBus::success)
    {
        if (bEnable)
        {
            byte = FLD_SET_DRF(_DPCD, _DOWNSPREAD_CTRL,
                _FIXED_VTOTAL_AS_SDP_EN_IN_PR_ACTIVE, _YES, byte);
        }
        else
        {
            byte = FLD_SET_DRF(_DPCD, _DOWNSPREAD_CTRL,
                _FIXED_VTOTAL_AS_SDP_EN_IN_PR_ACTIVE, _NO, byte);
        }

        status = this->setDpcdData(NV_DPCD_DOWNSPREAD_CTRL,
                                   &byte, sizeof byte, &size, &nakReason);
        if (status != AuxBus::success)
        {
            DP_PRINTF(DP_ERROR, "DP-DEV> Aux Write to DPCD offset 0x107 failed!");
            return status;
        }
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read to DPCD offset 0x107 failed!");
        return status;
    }
    return true;
}

bool DeviceImpl::getFECSupport()
{
    NvU8 byte          = 0;
    unsigned size      = 0;
    unsigned nakReason = NakUndefined;

    if(this->address.size() > 1)
    {
        bFECSupported = this->bandwidth.enum_path.bPathFECCapable;
    }

    else if (AuxBus::success == this->getDpcdData(NV_DPCD14_FEC_CAPABILITY,
        &byte, sizeof(byte), &size, &nakReason))
    {
        bFECSupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY, _FEC_CAPABLE, _YES, byte);
        bFECUncorrectedSupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY, _UNCORRECTED_BLOCK_ERROR_COUNT_CAPABLE, _YES, byte);
        bFECCorrectedSupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY, _CORRECTED_BLOCK_ERROR_COUNT_CAPABLE, _YES, byte);
        bFECBitSupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY, _BIT_ERROR_COUNT_CAPABLE, _YES, byte);
        bFECParityBlockSupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY, _PARITY_BLOCK_ERROR_COUNT_CAPABLE, _YES, byte);
        bFECParitySupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY, _PARITY_ERROR_COUNT_CAPABLE, _YES, byte);
    }

    return bFECSupported;
}

NvBool DeviceImpl::isDSCSupported()
{
    return dscCaps.bDSCSupported;
}

NvBool DeviceImpl::isDSCDecompressionSupported()
{
    return dscCaps.bDSCDecompressionSupported;
}

NvBool DeviceImpl::isDSCPassThroughSupported()
{
    return dscCaps.bDSCPassThroughSupported;
}

NvBool DeviceImpl::isDynamicPPSSupported()
{
    return dscCaps.bDynamicPPSSupported;
}

NvBool DeviceImpl::isDynamicDscToggleSupported()
{
    return dscCaps.bDynamicDscToggleSupported;
}

NvBool DeviceImpl::isDSCPossible()
{
    return this->bDSCPossible;
}

bool DeviceImpl::isFECSupported()
{
    return bFECSupported;
}

bool DeviceImpl::parseDscCaps(const NvU8 *buffer, NvU32 bufferSize)
{

    if (bufferSize < 16)
    {
        DP_PRINTF(DP_ERROR, "DSC caps buffer must be greater than or equal to 16");
        return false;
    }

    if (FLD_TEST_DRF(_DPCD20, _DSC_SUPPORT, _PASS_THROUGH, _YES, buffer[0x0]))
    {
        dscCaps.bDSCPassThroughSupported = true;
    }

    if (FLD_TEST_DRF(_DPCD20, _DSC_SUPPORT, _DYNAMIC_PPS_COMPRESSED_TO_COMPRESSED, _YES, buffer[0x0]))
    {
        dscCaps.bDynamicPPSSupported = true;
    }

    if (FLD_TEST_DRF(_DPCD20, _DSC_SUPPORT, _DYNAMIC_PPS_UNCOMPRESSED_TO_FROM_COMPRESSED, _YES, buffer[0x0]))
    {
        dscCaps.bDynamicDscToggleSupported = true;
    }

    dscCaps.versionMajor = DRF_VAL(_DPCD14, _DSC_ALGORITHM_REVISION, _MAJOR, buffer[0x1]);
    dscCaps.versionMinor = DRF_VAL(_DPCD14, _DSC_ALGORITHM_REVISION, _MINOR, buffer[0x1]);

    dscCaps.rcBufferBlockSize = DRF_VAL(_DPCD14, _DSC_RC_BUFFER_BLOCK, _SIZE, buffer[0x2]);

    dscCaps.rcBuffersize      = DRF_VAL(_DPCD14, _DSC_RC_BUFFER, _SIZE, buffer[0x3]);

    dscCaps.sliceCountSupportedMask = (((buffer[0xD]) << 8) | buffer[0x4]);
    if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_2, _SLICES_PER_SINK_24, _YES, buffer[0xD]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_24;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_2, _SLICES_PER_SINK_20, _YES, buffer[0xD]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_20;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_2, _SLICES_PER_SINK_16, _YES, buffer[0xD]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_16;

    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_12, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_12;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_10, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_10;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_8, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_8;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_6, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_6;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_4, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_4;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_2, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_2;
    else if(FLD_TEST_DRF(_DPCD14, _DSC_SLICE_CAPABILITIES_1, _SLICES_PER_SINK_1, _YES, buffer[0x4]))
        dscCaps.maxSlicesPerSink = DSC_SLICES_PER_SINK_1;

    if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _8, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 8;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _9, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 9;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _10, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 10;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _11, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 11;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _12, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 12;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _13, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 13;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _14, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 14;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _15, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 15;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_LINE_BUFFER, _BIT_DEPTH, _16, buffer[0x5]))
    {
        dscCaps.lineBufferBitDepth = 16;
    }

    if(FLD_TEST_DRF(_DPCD14, _DSC_BLOCK_PREDICTION, _SUPPORT, _YES, buffer[0x6]))
        dscCaps.bDscBlockPredictionSupport = true;

    unsigned maxBitsPerPixelLSB  = DRF_VAL(_DPCD14, _DSC_MAXIMUM_BITS_PER_PIXEL_1, _LSB, buffer[0x7]);
    unsigned maxBitsPerPixelMSB = DRF_VAL(_DPCD14, _DSC_MAXIMUM_BITS_PER_PIXEL_2, _MSB, buffer[0x8]);

    dscCaps.maxBitsPerPixelX16  = (maxBitsPerPixelMSB << 8) | maxBitsPerPixelLSB;

    if(FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_FORMAT_CAPABILITIES, _RGB, _YES, buffer[0x9]))
        dscCaps.dscDecoderColorFormatCaps.bRgb = true;
    if(FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_FORMAT_CAPABILITIES, _YCbCr_444, _YES, buffer[0x9]))
        dscCaps.dscDecoderColorFormatCaps.bYCbCr444 = true;
    if(FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_FORMAT_CAPABILITIES, _YCbCr_SIMPLE_422, _YES, buffer[0x9]))
        dscCaps.dscDecoderColorFormatCaps.bYCbCrSimple422 = true;
    if(FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_FORMAT_CAPABILITIES, _YCbCr_NATIVE_422, _YES, buffer[0x9]))
        dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422 = true;
    if(FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_FORMAT_CAPABILITIES, _YCbCr_NATIVE_420, _YES, buffer[0x9]))
        dscCaps.dscDecoderColorFormatCaps.bYCbCrNative420 = true;

    if (FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_DEPTH_CAPABILITIES, _12_BITS_PER_COLOR, _YES, buffer[0xa]))
        dscCaps.dscDecoderColorDepthMask |= DSC_BITS_PER_COLOR_MASK_12;
    if (FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_DEPTH_CAPABILITIES, _10_BITS_PER_COLOR, _YES, buffer[0xa]))
        dscCaps.dscDecoderColorDepthMask |= DSC_BITS_PER_COLOR_MASK_10;
    if (FLD_TEST_DRF(_DPCD14, _DSC_DECODER_COLOR_DEPTH_CAPABILITIES, _8_BITS_PER_COLOR, _YES, buffer[0xa]))
        dscCaps.dscDecoderColorDepthMask |= DSC_BITS_PER_COLOR_MASK_8;

    dscCaps.dscPeakThroughputMode0    = DRF_VAL(_DPCD14, _DSC_PEAK_THROUGHPUT, _MODE0, buffer[0xb]);
    dscCaps.dscPeakThroughputMode1    = DRF_VAL(_DPCD14, _DSC_PEAK_THROUGHPUT, _MODE1, buffer[0xb]);

    unsigned numOfPixels              = DRF_VAL(_DPCD14, _DSC_MAXIMUM_SLICE_WIDTH, _MAX, buffer[0xc]);
    dscCaps.dscMaxSliceWidth          = numOfPixels * 320;

    if (FLD_TEST_DRF(_DPCD14, _DSC_BITS_PER_PIXEL_INCREMENT, _SUPPORTED, _1_16, buffer[0xf]))
    {
        dscCaps.dscBitsPerPixelIncrement = BITS_PER_PIXEL_PRECISION_1_16;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_BITS_PER_PIXEL_INCREMENT, _SUPPORTED, _1_8, buffer[0xf]))
    {
        dscCaps.dscBitsPerPixelIncrement = BITS_PER_PIXEL_PRECISION_1_8;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_BITS_PER_PIXEL_INCREMENT, _SUPPORTED, _1_4, buffer[0xf]))
    {
        dscCaps.dscBitsPerPixelIncrement = BITS_PER_PIXEL_PRECISION_1_4;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_BITS_PER_PIXEL_INCREMENT, _SUPPORTED, _1_2, buffer[0xf]))
    {
        dscCaps.dscBitsPerPixelIncrement = BITS_PER_PIXEL_PRECISION_1_2;
    }
    else if (FLD_TEST_DRF(_DPCD14, _DSC_BITS_PER_PIXEL_INCREMENT, _SUPPORTED, _1, buffer[0xf]))
    {
        dscCaps.dscBitsPerPixelIncrement = BITS_PER_PIXEL_PRECISION_1;
    }

    return true;
}

bool DeviceImpl::parseBranchSpecificDscCaps(const NvU8 *buffer, NvU32 bufferSize)
{
    if (bufferSize < 3)
    {
        DP_PRINTF(DP_ERROR, "Branch DSC caps buffer must be greater than or equal to 3");
        return false;
    }

    dscCaps.branchDSCOverallThroughputMode0 = DRF_VAL(_DPCD14, _BRANCH_DSC_OVERALL_THROUGHPUT_MODE_0, _VALUE, buffer[0x0]);
    if (dscCaps.branchDSCOverallThroughputMode0 == 1)
    {
        dscCaps.branchDSCOverallThroughputMode0 = 680;
    }
    else if (dscCaps.branchDSCOverallThroughputMode0 >= 2)
    {
        dscCaps.branchDSCOverallThroughputMode0 = 600 + dscCaps.branchDSCOverallThroughputMode0 * 50;
    }

    dscCaps.branchDSCOverallThroughputMode1 = DRF_VAL(_DPCD14, _BRANCH_DSC_OVERALL_THROUGHPUT_MODE_1, _VALUE, buffer[0x1]);
    if (dscCaps.branchDSCOverallThroughputMode1 == 1)
    {
        dscCaps.branchDSCOverallThroughputMode1 = 680;
    }
    else if (dscCaps.branchDSCOverallThroughputMode1 >= 2)
    {
        dscCaps.branchDSCOverallThroughputMode1 = 600 + dscCaps.branchDSCOverallThroughputMode1 * 50;
    }

    dscCaps.branchDSCMaximumLineBufferWidth = DRF_VAL(_DPCD14, _BRANCH_DSC_MAXIMUM_LINE_BUFFER_WIDTH, _VALUE, buffer[0x2]);
    if (dscCaps.branchDSCMaximumLineBufferWidth != 0)
    {
        if (dscCaps.branchDSCMaximumLineBufferWidth >= 16)
        {
            dscCaps.branchDSCMaximumLineBufferWidth = dscCaps.branchDSCMaximumLineBufferWidth * 320;
        }
        else
        {
            dscCaps.branchDSCMaximumLineBufferWidth = 0;
            DP_PRINTF(DP_WARNING, "Value of branch DSC maximum line buffer width is invalid, so setting it to 0.");
        }
    }
    return true;
}

bool DeviceImpl::readAndParseDSCCaps()
{
    // Allocate a buffer of 16 bytes to read DSC caps

    unsigned sizeCompleted  = 0;
    unsigned nakReason      = NakUndefined;
    Address::StringBuffer sb;
    DP_USED(sb);

    if(AuxBus::success != this->getDpcdData(NV_DPCD14_DSC_SUPPORT,
        &rawDscCaps[0], sizeof(rawDscCaps), &sizeCompleted, &nakReason))
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Error querying DSC Caps on %s!", this->address.toString(sb));
        return false;
    }

    return parseDscCaps(&rawDscCaps[0], sizeof(rawDscCaps));
}

bool DeviceImpl::readAndParseBranchSpecificDSCCaps()
{
    unsigned sizeCompleted = 0;
    unsigned nakReason = NakUndefined;
    NvU8 rawBranchSpecificDscCaps[3];

    if(AuxBus::success != this->getDpcdData(NV_DPCD14_BRANCH_DSC_OVERALL_THROUGHPUT_MODE_0,
                                            &rawBranchSpecificDscCaps[0],
                                            sizeof(rawBranchSpecificDscCaps),
                                            &sizeCompleted, &nakReason))
    {
        return false;
    }

    return parseBranchSpecificDscCaps(&rawBranchSpecificDscCaps[0], sizeof(rawBranchSpecificDscCaps));
}

void DeviceImpl::queryGUID2()
{
    unsigned sizeCompleted  = 0;
    unsigned nakReason      = NakUndefined;
    Address::StringBuffer sb;
    DP_USED(sb);

    if(AuxBus::success == this->getDpcdData(NV_DPCD14_GUID_2, &this->guid2.data[0],
                                            DPCD_GUID_SIZE, &sizeCompleted, &nakReason))
    {
        if (!(this->guid2.isGuidZero()))
        {
            this->bVirtualPeerDevice = true;
        }
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Error querying GUID2 on %s!", this->address.toString(sb));
    }
}

bool DeviceImpl::getDscEnable(bool *pEnable)
{
    AuxBus::status  status = AuxBus::success;
    unsigned        sizeCompleted = 0;
    unsigned        pNakReason    = 0;
    NvU8            byte = 0;

    if (!pEnable ||
        !this->isDSCPossible() ||
        !this->devDoingDscDecompression ||
        !this->devDoingDscDecompression->plugged)
    {
        return false;
    }

    status = this->devDoingDscDecompression->getDpcdData(NV_DPCD14_DSC_ENABLE,
                                                         &byte,
                                                         sizeof byte,
                                                         &sizeCompleted,
                                                         &pNakReason);

    if (status != AuxBus::success)
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Error querying DSC Enable State!");
        return false;
    }

    *pEnable = FLD_TEST_DRF(_DPCD14, _DSC_ENABLE, _DECOMPRESSION, _YES, byte);
    return true;
}

void DeviceImpl::setDscDecompressionDevice(bool bDscCapBasedOnParent)
{
    // Decide if DSC stream can be sent to new device
    this->bDSCPossible = false;
    this->devDoingDscDecompression = NULL;

    if (this->multistream)
    {
        if ((this->peerDevice == Dongle) &&
            (this->dpcdRevisionMajor != 0) &&
            !bDscCapBasedOnParent)
        {
            // For Peer Type 4 device with LAM DPCD rev != 0.0, check only the device's own DSC capability.
            if (this->isDSCSupported())
            {
                this->bDSCPossible = true;
                this->devDoingDscDecompression = this;
            }
        }
        else
        {
            //
            // Check the device's own and its parent's DSC capability.
            // - Sink device will do DSC cecompression when
            //       1. Sink device is capable of DSC decompression
            //       2. Sink is on a logical port (8-15)
            //
            //       OR
            //
            //       1. Sink device is capable of DSC decompression
            //       2. Parent of sink is a Virtual Peer device
            //       3. Parent of sink supports DSC Pass through
            //
            // - Sink device's parent will do DSC decompression
            //       1. Above conditions are not true.
            //       2. Parent of sink supports DSC decompression.
            //
            if (this->isDSCSupported())
            {
                if (this->isVideoSink() && this->getParent() != NULL)
                {
                    if (this->isLogical())
                    {
                        this->devDoingDscDecompression = this;
                        this->bDSCPossible = true;
                    }
                    else if (this->parent->isVirtualPeerDevice() &&
                             this->parent->isDSCPassThroughSupported())
                    {
                        //
                        // This condition takes care of DSC capable sink devices
                        // connected behind a DSC Pass through capable branch
                        //
                        this->devDoingDscDecompression = this;
                        this->bDSCPossible = true;
                    }
                    else if (this->parent->isDSCDecompressionSupported())
                    {
                        //
                        // This condition takes care of DSC capable sink devices
                        // connected behind a branch device that is not capable
                        // of DSC pass through but can do DSC decompression.
                        //
                        this->bDSCPossible = true;
                        this->devDoingDscDecompression = this->parent;
                    }
                }
                else
                {
                    if (this->isDSCDecompressionSupported())
                    {
                        // This condition takes care of branch device capable of DSC decoding.
                        this->devDoingDscDecompression = this;
                        this->bDSCPossible = true;
                    }
                }
            }
            else if (this->parent && this->parent->isDSCDecompressionSupported() &&
                     !(this->isLogical()))
            {
                //
                // This condition takes care of sink devices not capable of DSC
                // but parent is capable of DSC decompression. We need to skip this
                // if sink is at logical port.
                //
                this->bDSCPossible = true;
                this->devDoingDscDecompression = this->parent;
            }
        }
    }
    else
    {
        if (this->isDSCDecompressionSupported())
        {
            this->bDSCPossible = true;
            this->devDoingDscDecompression = this;
        }
    }
}

bool DeviceImpl::setDscEnable(bool enable)
{
    NvU8 dscEnableByte = 0;
    NvU8 dscPassthroughByte = 0;
    unsigned size        = 0;
    unsigned nakReason   = NakUndefined;
    bool bCurrDscEnable  = false;
    bool bDscPassThrough = false;
    AuxBus::status dscEnableStatus      = AuxBus::success;
    AuxBus::status dscPassThroughStatus = AuxBus::success;
    Address::StringBuffer buffer;
    DP_USED(buffer);

    if (!this->isDSCPossible() || !this->devDoingDscDecompression ||
        !this->devDoingDscDecompression->plugged)
    {
        return false;
    }

    if ((this->devDoingDscDecompression == this) && !this->isLogical() && !(this->peerDevice == Dongle) && this->parent != NULL)
    {
        //
        // If the device has a parent, that means the sink is on a MST link and
        // and on a MST link if DSC is possible on the path and devDoingDscDecompression
        // is the sink itself and sink is not on a logical port, then the parent should be
        // DSC Pass through capable.
        //
        bDscPassThrough = true;
    }

    //
    // Get Current DSC Enable State
    // Ideally we don't need to check the current state but Synaptics DSC device,
    // which was used for inital DSC code developement did not follow spec and so
    // we have added this code. Overwriting the same value should not have any
    // impact as per the spec. Will remove this check once all DSC devices follow spec.
    //
    if (!getDscEnable(&bCurrDscEnable))
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Not able to get DSC Enable State!");
        return false;
    }

    if(enable)
    {
        if(bDscPassThrough)
        {
            dscPassthroughByte = FLD_SET_DRF(_DPCD20, _DSC_ENABLE, _PASS_THROUGH, _YES, dscPassthroughByte);
            DP_PRINTF(DP_NOTICE, "DP-DEV> Enabling DSC Pass through on branch device - %s",
                      this->parent->getTopologyAddress().toString(buffer));
        }

        if (!bCurrDscEnable)
        {
            dscEnableByte = FLD_SET_DRF(_DPCD14, _DSC_ENABLE, _DECOMPRESSION, _YES, dscEnableByte);
            DP_PRINTF(DP_NOTICE, "DP-DEV> Enabling DSC decompression on device - %s",
                      this->devDoingDscDecompression->getTopologyAddress().toString(buffer));
        }
        else
        {
            DP_PRINTF(DP_NOTICE, "DP-DEV> DSC decompression is already enabled on device - %s",
                      this->devDoingDscDecompression->getTopologyAddress().toString(buffer));
        }
    }
    else
    {
        if(bDscPassThrough)
        {
            dscPassthroughByte = FLD_SET_DRF(_DPCD20, _DSC_ENABLE, _PASS_THROUGH, _NO, dscPassthroughByte);
            DP_PRINTF(DP_NOTICE, "DP-DEV> Disabling DSC Pass through on branch device - %s",
                      this->parent->getTopologyAddress().toString(buffer));
        }

        if (bCurrDscEnable)
        {
            dscEnableByte = FLD_SET_DRF(_DPCD14, _DSC_ENABLE, _DECOMPRESSION, _NO, dscEnableByte);
            DP_PRINTF(DP_NOTICE, "DP-DEV> Disabling DSC decompression on device - %s",
                      this->devDoingDscDecompression->getTopologyAddress().toString(buffer));
        }
        else
        {
            DP_PRINTF(DP_NOTICE, "DP-DEV> DSC decompression is already disabled on device - %s",
                      this->devDoingDscDecompression->getTopologyAddress().toString(buffer));
        }
    }

    if (bDscPassThrough)
    {
        dscPassThroughStatus = this->parent->setDpcdData(NV_DPCD14_DSC_ENABLE,
                                   &dscPassthroughByte, sizeof dscPassthroughByte, &size, &nakReason);
        if (dscPassThroughStatus != AuxBus::success)
        {
            DP_PRINTF(DP_ERROR, "DP-DEV> Setting DSC Passthrough on parent branch %s failed",
                      this->parent->getTopologyAddress().toString(buffer));
        }
    }

    if (enable != bCurrDscEnable)
    {
        dscEnableStatus = this->devDoingDscDecompression->setDpcdData(NV_DPCD14_DSC_ENABLE,
                              &dscEnableByte, sizeof dscEnableByte, &size, &nakReason);
        if (dscEnableStatus != AuxBus::success)
        {
            DP_PRINTF(DP_ERROR, "DP-DEV> Setting DSC Enable on sink %s failed",
                      this->devDoingDscDecompression->getTopologyAddress().toString(buffer));

        }
    }

    if ((dscPassThroughStatus != AuxBus::success) || (dscEnableStatus != AuxBus::success))
    {
        return false;
    }
    else
    {
        return true;
    }
}



bool DeviceImpl::setDscEnableDPToHDMIPCON(bool bDscEnable, bool bEnablePassThroughForPCON)
{
    NvU8 dscEnableByte              = 0;
    unsigned size                   = 0;
    unsigned nakReason              = NakUndefined;
    AuxBus::status dscEnableStatus  = AuxBus::success;
    Address::StringBuffer buffer;
    DP_USED(buffer);

    if (!this->isDSCPossible())
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> DSC is not supported on DP to HDMI PCON - %s",
                  this->getTopologyAddress().toString(buffer));
        return false;
    }

    if (bDscEnable)
    {
        if(bEnablePassThroughForPCON)
        {
            dscEnableByte = FLD_SET_DRF(_DPCD20, _DSC_ENABLE, _PASS_THROUGH, _YES, dscEnableByte);
            DP_PRINTF(DP_NOTICE, "DP-DEV> Enabling DSC Pass through on DP to HDMI PCON device - %s",
                      this->getTopologyAddress().toString(buffer));
        }
        else
        {
            dscEnableByte = FLD_SET_DRF(_DPCD14, _DSC_ENABLE, _DECOMPRESSION, _YES, dscEnableByte);
            DP_PRINTF(DP_NOTICE, "DP-DEV> Enabling DSC decompression on DP to HDMI PCON device - %s",
                      this->getTopologyAddress().toString(buffer));
        }

    }

    dscEnableStatus = this->setDpcdData(NV_DPCD14_DSC_ENABLE,
                              &dscEnableByte, sizeof dscEnableByte, &size, &nakReason);

    if (dscEnableStatus != AuxBus::success)
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Setting DSC Enable on DP to HDMI PCON %s failed",
                  this->getTopologyAddress().toString(buffer));
        return false;

    }

    return true;
}

unsigned DeviceImpl::getDscVersionMajor()
{
    return dscCaps.versionMajor;
}

unsigned DeviceImpl::getDscVersionMinor()
{
    return dscCaps.versionMinor;
}

unsigned DeviceImpl::getDscRcBufferSize()
{
    return dscCaps.rcBuffersize;
}

unsigned DeviceImpl::getDscRcBufferBlockSize()
{
    return dscCaps.rcBufferBlockSize;
}

unsigned DeviceImpl::getDscMaxSlicesPerSink()
{
    return dscCaps.maxSlicesPerSink;
}

unsigned DeviceImpl::getDscLineBufferBitDepth()
{
    return dscCaps.lineBufferBitDepth;
}

NvBool DeviceImpl::isDscBlockPredictionSupported()
{
    return dscCaps.bDscBlockPredictionSupport;
}

unsigned DeviceImpl::getDscMaxBitsPerPixel()
{
    return dscCaps.maxBitsPerPixelX16;
}

NvBool DeviceImpl::isDscRgbSupported()
{
    return dscCaps.dscDecoderColorFormatCaps.bRgb;
}

NvBool DeviceImpl::isDscYCbCr444Supported()
{
    return dscCaps.dscDecoderColorFormatCaps.bYCbCr444;
}

NvBool DeviceImpl::isDscYCbCrSimple422Supported()
{
    return dscCaps.dscDecoderColorFormatCaps.bYCbCrSimple422;
}

NvBool DeviceImpl::isDscYCbCr422NativeSupported()
{
    return dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422;
}

NvBool DeviceImpl::isDscYCbCr420NativeSupported()
{
    return dscCaps.dscDecoderColorFormatCaps.bYCbCrNative420;
}

unsigned DeviceImpl::getDscPeakThroughputMode0()
{
    return dscCaps.dscPeakThroughputMode0;
}

unsigned DeviceImpl::getDscPeakThroughputModel()
{
    return dscCaps.dscPeakThroughputMode1;
}

unsigned DeviceImpl::getDscMaxSliceWidth()
{
    return dscCaps.dscMaxSliceWidth;
}

unsigned DeviceImpl::getDscDecoderColorDepthSupportMask()
{
    return dscCaps.dscDecoderColorDepthMask;
}

bool DeviceImpl::isFakedMuxDevice()
{
    return connector->main->isDynamicMuxCapable() && bIsFakedMuxDevice;
}

bool DeviceImpl::isPreviouslyFakedMuxDevice()
{
    return connector->main->isDynamicMuxCapable() && bIsPreviouslyFakedMuxDevice;
}

static AuxBus::status _QueryCrcSink
(
    DeviceImpl  *bus,
    NvU16       *sinkCrc0,
    NvU16       *sinkCrc1,
    NvU16       *sinkCrc2
)
{
    AuxBus::status status = AuxBus::success;
    // no sink op needs to be done if registers are NULL
    if (sinkCrc0 == NULL) return status;
    NvU32     addr      = NV_DPCD14_DSC_CRC_0;
    unsigned  size      = 2;
    NvU8      cnt[2]    = {0, 0};

    unsigned  sizeCompleted = 0;
    unsigned  nakReason     = 0;

    status = bus->getDpcdData(addr, &cnt[0], size, &sizeCompleted, &nakReason);

    if (status != AuxBus::success)
    {
        return status;
    }
    *sinkCrc0 = (((NvU16) cnt[1]) << (sizeof(NvU8) * 8)) | cnt[0];

    addr      = NV_DPCD14_DSC_CRC_1;
    size      = 2;

    status = bus->getDpcdData(addr, &cnt[0], size, &sizeCompleted, &nakReason);

    if (status != AuxBus::success)
    {
        return status;
    }
    *sinkCrc1 = (((NvU16) cnt[1]) << (sizeof(NvU8) * 8)) | cnt[0];

    addr      = NV_DPCD14_DSC_CRC_2;
    size      = 2;

    status = bus->getDpcdData(addr, &cnt[0], size, &sizeCompleted, &nakReason);

    if (status != AuxBus::success)
    {
        return status;
    }
    *sinkCrc2 = (((NvU16) cnt[1]) << (sizeof(NvU8) * 8)) | cnt[0];
    return status;
}

AuxBus::status DeviceImpl::dscCrcControl(NvBool bEnable, gpuDscCrc *gpuData, sinkDscCrc *sinkData)
{
    // GPU part
    if (this->connector->main->dscCrcTransaction(bEnable, gpuData, (NvU16*) &(activeGroup->headIndex)) != true)
    {
        return AuxBus::nack;
    }

    // sink part
    if (!sinkData)
    {
        return AuxBus::success;
    }
    return _QueryCrcSink(this, &(sinkData->sinkCrc0), &(sinkData->sinkCrc1), &(sinkData->sinkCrc2));
}

bool DeviceImpl::getPCONCaps(PCONCaps *pPCONCaps)
{
    AuxBus::status  status          = AuxBus::success;
    NvU32           addr            = NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT(0);
    NvU8            data[4]         = {0};
    unsigned        sizeCompleted   = 0;
    unsigned        nakReason       = 0;
    NvU8            pConType        = 0;

    status = this->getDpcdData(addr, &data[0], sizeof(data), &sizeCompleted, &nakReason);
    if (status == AuxBus::success)
    {
        pConType = DRF_VAL(_DPCD, _DETAILED_CAP_INFO, _DWNSTRM_PORT_TX_TYPE, data[0]);
        if (pConType == NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_HDMI)
        {
            this->connectorType = connectorHDMI;
            pPCONCaps->maxTmdsClkRate = data[1] * 2500000;

            pPCONCaps->bSourceControlModeSupported =
                    FLD_TEST_DRF(_DPCD, _DETAILED_CAP_INFO, _SRC_CONTROL_MODE_SUPPORT, _YES, data[2]);
            pPCONCaps->bConcurrentLTSupported =
                    FLD_TEST_DRF(_DPCD, _DETAILED_CAP_INFO, _CONCURRENT_LT_SUPPORT, _YES, data[2]);

            switch (DRF_VAL(_DPCD, _DETAILED_CAP_INFO, _MAX_FRL_LINK_BW_SUPPORT, data[2]))
            {
                case NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_9G:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 9;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_18G:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 18;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_24G:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 24;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_32G:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 32;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_40G:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 40;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_48G:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 48;
                    break;
                default:
                    pPCONCaps->maxHdmiLinkBandwidthGbps = 0;
                    break;
            }

            switch (DRF_VAL(_DPCD, _DETAILED_CAP_INFO, _MAX_BITS_PER_COMPONENT_DEF, data[2]))
            {
                case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_10BPC:
                    pPCONCaps->maxBpc = 10;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_12BPC:
                    pPCONCaps->maxBpc = 12;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_16BPC:
                    pPCONCaps->maxBpc = 16;
                    break;
                case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_8BPC:
                default:
                    pPCONCaps->maxBpc = 8;
                    break;
            }

            DP_PRINTF(DP_NOTICE, "DP2HDMI PCON caps - Max TMDS Clk: %u LinkBWGbps: %u MaxBpc: %u",
                      pPCONCaps->maxTmdsClkRate, pPCONCaps->maxHdmiLinkBandwidthGbps, pPCONCaps->maxBpc);
        }
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP-DEV> Error - DPCD Read for detailed port capabilities (0x80) failed.");
        return false;
    }
    return true;
}

bool DeviceImpl::getIgnoreMSACap()
{
    NvU8 byte = 0;
    unsigned size = 0;
    unsigned nakReason = NakUndefined;
    AuxBus::status status;

    if (bIgnoreMsaCapCached)
    {
        return bIgnoreMsaCap;
    }

    if (this->isMultistream())
    {
        status = this->getDpcdData(NV_DPCD_DOWN_STREAM_PORT,
                                   &byte, sizeof byte, &size, &nakReason);
        if (status == AuxBus::success)
        {
            if(FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _MSA_TIMING_PAR_IGNORED, _YES, byte))
            {
                if (this->parent && this->parent->isVirtualPeerDevice())
                {
                    byte = 0;
                    size = 0;
                    nakReason = NakUndefined;

                    status = this->parent->getDpcdData(NV_DPCD_DOWN_STREAM_PORT,
                                                       &byte, sizeof byte, &size, &nakReason);
                    if (status == AuxBus::success)
                    {
                        if(FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _MSA_TIMING_PAR_IGNORED, _YES, byte))
                        {
                            bIgnoreMsaCap = true;
                        }
                        else
                        {
                            bIgnoreMsaCap = false;
                        }
                        bIgnoreMsaCapCached = true;
                    }
                    else
                    {
                        DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read from DPCD offset 0x7 failed!");
                        return false;
                    }
                }
                else
                {
                    bIgnoreMsaCap = true;
                    bIgnoreMsaCapCached = true;
                }
            }
            else
            {
                bIgnoreMsaCap = false;
                bIgnoreMsaCapCached = true;
            }
        }
        else
        {
            DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read from DPCD offset 0x7 failed!");
            return false;
        }
    }
    else
    {
        bIgnoreMsaCap = hal->getMsaTimingparIgnored();
        bIgnoreMsaCapCached = true;
    }

    return bIgnoreMsaCap;
}

AuxRetry::status  DeviceImpl::setIgnoreMSAEnable(bool msaTimingParamIgnoreEn)
{
    NvU8 byte = 0;
    unsigned size = 0;
    unsigned nakReason = NakUndefined;
    AuxBus::status status;

    if (this->isMultistream())
    {
        status = this->getDpcdData(NV_DPCD_DOWNSPREAD_CTRL,
                                   &byte, sizeof byte, &size, &nakReason);
        if (status == AuxBus::success)
        {
            if (msaTimingParamIgnoreEn)
            {
                byte = FLD_SET_DRF(_DPCD, _DOWNSPREAD_CTRL, _MSA_TIMING_PAR_IGNORED, _TRUE, byte);
            }
            else
            {
                byte = FLD_SET_DRF(_DPCD, _DOWNSPREAD_CTRL, _MSA_TIMING_PAR_IGNORED, _FALSE, byte);
            }

            status = this->setDpcdData(NV_DPCD_DOWNSPREAD_CTRL,
                                       &byte, sizeof byte, &size, &nakReason);
            if (status == AuxBus::success)
            {
                return AuxRetry::ack;
            }
            else
            {
                DP_PRINTF(DP_ERROR, "DP-DEV> Aux Write to DPCD offset 0x107 failed!");
                return AuxRetry::nack;
            }
        }
        else
        {
            DP_PRINTF(DP_ERROR, "DP-DEV> Aux Read from DPCD offset 0x107 failed!");
            return AuxRetry::nack;
        }
    }
    else
    {
        return hal->setIgnoreMSATimingParamters(msaTimingParamIgnoreEn);
    }
}

bool DeviceImpl::getDeviceSpecificData(NvU8 *oui, NvU8 *devIdString,
                                       NvU8 *hwRevision, NvU8 *swMajorRevision,
                                       NvU8 *swMinorRevision)
{
    NvU8 buffer[16] = {0};
    unsigned size = 13U;
    unsigned sizeCompleted = 0U;
    unsigned nakReason = NakUndefined;
    unsigned i = 0U;
    unsigned address;

    if (oui == NULL || devIdString == NULL || hwRevision == NULL ||
        swMajorRevision == NULL || swMinorRevision == NULL)
    {
        return false;
    }

    if (!this->audioSink && !this->videoSink)
    {
        address = NV_DPCD_BRANCH_IEEE_OUI;
    }
    else
    {
        address = NV_DPCD_SINK_IEEE_OUI;
    }

    if (AuxBus::success != this->getDpcdData(address, &buffer[0],
                                             size, &sizeCompleted, &nakReason))
    {
        return false;
    }

    // 0x400-0x402 for sink device and 0x500-0x502 for branch device gives OUI.
    for (i = 0U; i < DEVICE_OUI_SIZE; i++)
    {
        oui[i] = buffer[i];
    }

    //
    // 0x403-0x408 for sink device and 0x503-0x508 for branch device provides
    // device Identification string.
    //
    for (unsigned j = 0U; j < NV_DPCD_SOURCE_DEV_ID_STRING__SIZE; i++, j++)
    {
        devIdString[j] = buffer[i];
    }

    // 0x409 for sink and 0x509 for branch provides HW revision.
    // 0x40A-0x40B for sink and 0x50A-0x50B for branch provides SW/Firmware revision.
    *hwRevision = buffer[9];
    *swMajorRevision = buffer[10];
    *swMinorRevision = buffer[11];

    return true;
}

bool DeviceImpl::getParentSpecificData(NvU8 *oui, NvU8 *devIdString,
                                       NvU8 *hwRevision, NvU8 *swMajorRevision,
                                       NvU8 *swMinorRevision)
{
    if (this->parent == NULL)
    {
        return false;
    }

    return this->parent->getDeviceSpecificData(oui, devIdString, hwRevision,
                                               swMajorRevision, swMinorRevision);
}

bool DeviceImpl::setModeList(DisplayPort::DpModesetParams *modeList, unsigned numModes)
{
    // Create a dummy group for compoundQuery
    GroupImpl g(connector);
    g.insert(this);

    maxModeBwRequired = 0;

    for (unsigned modeItr = 0; modeItr < numModes; modeItr++)
    {
        connector->beginCompoundQuery();
        DscParams dscParams = DscParams();
        dscParams.bCheckWithDsc = true;

        DpModesetParams &modesetParams = modeList[modeItr];
        NvU64 bpp = modesetParams.modesetInfo.depth;
        DP_IMP_ERROR dpImpError = DP_IMP_ERROR_NONE;

        if (connector->compoundQueryAttach((Group *)&g, modesetParams, &dscParams, &dpImpError))
        {
            if (dscParams.bEnableDsc)
            {
                bpp = divide_ceil(dscParams.bitsPerPixelX16, 16);
            }

            NvU64 modeBwRequired = modesetParams.modesetInfo.pixelClockHz * bpp;
            if (maxModeBwRequired < modeBwRequired)
            {
                maxModeBwRequired = modeBwRequired;
            }
        }

        connector->endCompoundQuery();
    }

    DP_PRINTF(DP_INFO, "Computed Max mode BW: %d Mbps", maxModeBwRequired / (1000 * 1000));

    connector->updateDpTunnelBwAllocation();

    return true;
}

void
DeviceHDCPDetection::start()
{
    if (parent->isNativeDPCD())
    {
        if (!parent->isMultistream())
        {
            goto NativeDPCDHDCPCAPRead;
        }
        else
        {
            parent->isHDCPCap = False;
            waivePendingHDCPCapDoneNotification();
            return;
        }

NativeDPCDHDCPCAPRead:

        BCaps bCaps = {0};

        parent->hal->getBCaps(bCaps, parent->BCAPS);
        *(parent->nvBCaps) = *(parent->BCAPS);

        if (bCaps.HDCPCapable)
        {
            NvU8 tempBKSV[HDCP_KSV_SIZE] = {0};
            if (parent->hal->getBKSV(tempBKSV))
            {
                if (hdcpValidateKsv(tempBKSV, HDCP_KSV_SIZE))
                {
                    for (unsigned i=0; i<HDCP_KSV_SIZE; i++)
                        parent->BKSV[i] = tempBKSV[i];
                }
            }
            parent->isHDCPCap = True;
            waivePendingHDCPCapDoneNotification();
            return;
        }
        else
        {
            unsigned char hdcp22BCAPS[HDCP22_BCAPS_SIZE];

            // Check if hdcp2.x only device and probe hdcp22Bcaps.
            parent->hal->getHdcp22BCaps(bCaps, hdcp22BCAPS);
            if (bCaps.HDCPCapable)
            {
                parent->nvBCaps[0] = FLD_SET_DRF_NUM(_DPCD, _HDCP_BCAPS_OFFSET,
                                                   _HDCP_CAPABLE, bCaps.HDCPCapable,
                                                   parent->nvBCaps[0]) |
                                   FLD_SET_DRF_NUM(_DPCD, _HDCP_BCAPS_OFFSET, _HDCP_REPEATER,
                                                   bCaps.repeater, parent->nvBCaps[0]);

                //
                // No need to validate 1.x bksv here and hdcp22 authentication would
                // validate certificate with bksv in uproc.
                //
                parent->isHDCPCap = True;
                waivePendingHDCPCapDoneNotification();
                return;
            }
        }

        parent->isHDCPCap = False;
        waivePendingHDCPCapDoneNotification();
    }
    else
    {
        parent->isHDCPCap = False;
        waivePendingHDCPCapDoneNotification();
    }
}

void
DeviceHDCPDetection::messageCompleted
(
    MessageManager::Message *from
)
{
    if ((from == &remoteBKSVReadMessage)    ||
        (from == &remoteBCapsReadMessage)   ||
        (from == &remote22BCapsReadMessage))
    {
        handleRemoteDpcdReadDownReply(from);
    }
}

void
DeviceHDCPDetection::handleRemoteDpcdReadDownReply
(
    MessageManager::Message *from
)
{
    NvU8 i2cBcaps;
    unsigned dataCompleted;
    unsigned defaultReason;
    Address::StringBuffer sb;
    DP_USED(sb);

    if (from == &remoteBKSVReadMessage)
    {
        bksvReadCompleted = true;
        bBKSVReadMessagePending = false;
        DP_PRINTF(DP_NOTICE, "DP-QM> REMOTE_DPCD_READ(BKSV) {%p} at '%s' completed",
                  (MessageManager::Message *)&remoteBKSVReadMessage,
                  parent->address.toString(sb));

        if (remoteBKSVReadMessage.replyNumOfBytesReadDPCD() != HDCP_KSV_SIZE)
        {
            DP_ASSERT(0 && "Incomplete BKSV in remote DPCD read message");
            parent->isHDCPCap = False;

            // Destruct only when no message is pending
            if (!(bBKSVReadMessagePending || bBCapsReadMessagePending))
            {
                parent->isDeviceHDCPDetectionAlive = false;
                delete this;
            }
            return;
        }

        DP_ASSERT(remoteBKSVReadMessage.replyPortNumber() == parent->address.tail());
        if (hdcpValidateKsv(remoteBKSVReadMessage.replyGetData(), HDCP_KSV_SIZE))
        {
            isValidBKSV = true;
            for (unsigned i=0; i<HDCP_KSV_SIZE; i++)
                parent->BKSV[i] = (remoteBKSVReadMessage.replyGetData())[i];

            DP_PRINTF(DP_NOTICE, "DP-QM> Device at '%s' is with valid BKSV.",
                  parent->address.toString(sb));
        }
    }
    else if (from == &remoteBCapsReadMessage)
    {
        bCapsReadCompleted = true;
        bBCapsReadMessagePending = false;
        DP_PRINTF(DP_NOTICE, "DP-QM> REMOTE_DPCD_READ(BCaps) {%p} at '%s' completed",
                  (MessageManager::Message *)&remoteBCapsReadMessage,
                  parent->address.toString(sb));

        if (remoteBCapsReadMessage.replyNumOfBytesReadDPCD() != HDCP_BCAPS_SIZE)
        {
            DP_ASSERT(0 && "Incomplete BCaps in remote DPCD read message");
            parent->isHDCPCap = False;

            // Destruct only when no message is pending
            if (!(bBKSVReadMessagePending || bBCapsReadMessagePending))
            {
                parent->isDeviceHDCPDetectionAlive = false;
                delete this;
            }
            return;
        }

        DP_ASSERT(remoteBCapsReadMessage.replyPortNumber() == parent->address.tail());
        if (!!(*remoteBCapsReadMessage.replyGetData() & 0x1))
        {
            *(parent->nvBCaps) = *(parent->BCAPS) = *remoteBCapsReadMessage.replyGetData();
            isBCapsHDCP = true;

            DP_PRINTF(DP_NOTICE, "DP-QM> Device at '%s' is with valid BCAPS : %x",
                  parent->address.toString(sb), *remoteBCapsReadMessage.replyGetData());
        }
        else
        {
            if (isValidBKSV)
            {
                DP_PRINTF(DP_WARNING, "DP-QM> Device at '%s' is with valid BKSV but Invalid BCAPS : %x",
                      parent->address.toString(sb), *remoteBCapsReadMessage.replyGetData());

                // Read the BCAPS DDC offset
                parent->transaction(AuxBus::read, AuxBus::i2cMot, HDCP_I2C_CLIENT_ADDR, &i2cBcaps,
                                 1, &dataCompleted, &defaultReason, HDCP_BCAPS_DDC_OFFSET, 1);

                DP_PRINTF(DP_NOTICE, "DP-QM> Device at '%s' is with DDC BACPS: %x",
                      parent->address.toString(sb), i2cBcaps);

                // If the Reserved Bit is SET, Device supports HDCP
                if (i2cBcaps & HDCP_BCAPS_DDC_EN_BIT)
                {
                    isBCapsHDCP = true;
                    // Set the HDCP cap BCAPS according to DP protocol
                    *(parent->BCAPS) |= HDCP_BCAPS_DP_EN_BIT;
                    *(parent->nvBCaps) = *(parent->BCAPS);
                }
            }
            else
            {
                DP_PRINTF(DP_NOTICE, "DP-QM> Device at '%s' is without valid BKSV and BCAPS, thus try 22BCAPS", parent->address.toString(sb));

                Address parentAddress = parent->address.parent();
                remote22BCapsReadMessage.setMessagePriority(NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT);
                remote22BCapsReadMessage.set(parentAddress, parent->address.tail(), NV_DPCD_HDCP22_BCAPS_OFFSET, HDCP22_BCAPS_SIZE);
                bCapsReadCompleted = false;
                bBCapsReadMessagePending = true;
                messageManager->post(&remote22BCapsReadMessage, this);
            }
        }
    }
    else if (from == &remote22BCapsReadMessage)
    {
        bCapsReadCompleted = true;
        bBCapsReadMessagePending = false;
        DP_PRINTF(DP_NOTICE, "DP-QM> REMOTE_DPCD_READ(22BCaps) {%p} at '%s' completed",
                  (MessageManager::Message *)&remote22BCapsReadMessage,
                  parent->address.toString(sb));

        if (remote22BCapsReadMessage.replyNumOfBytesReadDPCD() != HDCP22_BCAPS_SIZE)
        {
            DP_ASSERT(0 && "Incomplete 22BCaps in remote DPCD read message");
            parent->isHDCPCap = False;

            // Destruct only when no message is pending
            if (!(bBKSVReadMessagePending || bBCapsReadMessagePending))
            {
                parent->isDeviceHDCPDetectionAlive = false;
                delete this;
            }
            return;
        }

        DP_ASSERT(remote22BCapsReadMessage.replyPortNumber() == parent->address.tail());
        if (!!(*remote22BCapsReadMessage.replyGetData() & 0x2))
        {
            unsigned char hdcp22BCAPS;

            hdcp22BCAPS = *remote22BCapsReadMessage.replyGetData();

            parent->nvBCaps[0] = FLD_SET_DRF_NUM(_DPCD, _HDCP_BCAPS_OFFSET,
                                               _HDCP_CAPABLE, (hdcp22BCAPS & 0x2) ? 1 : 0,
                                               parent->nvBCaps[0]) |
                               FLD_SET_DRF_NUM(_DPCD, _HDCP_BCAPS_OFFSET, _HDCP_REPEATER,
                                               (hdcp22BCAPS & 0x1) ? 1 : 0, parent->nvBCaps[0]);

            // hdcp22 will validate certificate's bksv directly.
            isBCapsHDCP = isValidBKSV = true;

            DP_PRINTF(DP_NOTICE, "DP-QM> Device at '%s' is with valid 22BCAPS : %x",
                  parent->address.toString(sb), *remote22BCapsReadMessage.replyGetData());
        }
    }

    if (bCapsReadCompleted && bksvReadCompleted)
    {
        // Complete remote HDCP probe and check if can power down again.
        if (parent->connector)
        {
            parent->connector->decPendingRemoteHdcpDetection();
            parent->connector->isNoActiveStreamAndPowerdown();
        }

        if (isValidBKSV && isBCapsHDCP)
        {
            parent->isHDCPCap = True;
        }
        else
        {
            parent->isHDCPCap = False;
        }

        // Destruct only when no message is pending
        if (!(bBKSVReadMessagePending || bBCapsReadMessagePending))
        {
            parent->isDeviceHDCPDetectionAlive = false;
            delete this;
        }
    }
    else
    {
        parent->isHDCPCap = Indeterminate;
    }
}

bool
DeviceHDCPDetection::hdcpValidateKsv
(
    const NvU8 *ksv,
    NvU32 Size
)
{

    if (HDCP_KSV_SIZE <= Size)
    {
        NvU32 i, j;
        NvU32 count_ones = 0;
        for (i=0; i < HDCP_KSV_SIZE; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (ksv[i] & (1 <<(j)))
                {
                    count_ones++;
                }
            }
        }

        if (count_ones == 20)
        {
            return true;
        }
    }
    return false;
}

void
DeviceHDCPDetection::messageFailed
(
    MessageManager::Message *from,
    NakData *nakData
)
{
    if (from == &remoteBKSVReadMessage)
    {
        if ((retriesRemoteBKSVReadMessage < DPCD_REMOTE_DPCD_READ_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesRemoteBKSVReadMessage++;
            retryRemoteBKSVReadMessage = bBKSVReadMessagePending = true;
            timer->queueCallback(this, "BKSV", DPCD_REMOTE_DPCD_READ_MESSAGE_COOLDOWN_BKSV);
            return;
        }
        //
        // If message failed is called after all retries have expired or due
        // to any other reason then reset the bBKSVReadMessagePending flag
        //
        bBKSVReadMessagePending = false;
    }

    if (from == &remoteBCapsReadMessage)
    {
        if ((retriesRemoteBCapsReadMessage < DPCD_REMOTE_DPCD_READ_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesRemoteBCapsReadMessage++;
            retryRemoteBCapsReadMessage = bBCapsReadMessagePending = true;
            timer->queueCallback(this, "BCaps", DPCD_REMOTE_DPCD_READ_MESSAGE_COOLDOWN_BKSV);
            return;
        }
        //
        // If message failed is called after all retries have expired or due
        // to any other reason then reset the bBCapsReadMessagePending flag
        //
        bBCapsReadMessagePending = false;
    }

    if (from == &remote22BCapsReadMessage)
    {
        if ((retriesRemote22BCapsReadMessage < DPCD_REMOTE_DPCD_READ_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesRemote22BCapsReadMessage++;
            retryRemote22BCapsReadMessage = bBCapsReadMessagePending = true;
            timer->queueCallback(this, "22BCaps", DPCD_REMOTE_DPCD_READ_MESSAGE_COOLDOWN_BKSV);
            return;
        }
        //
        // If message failed is called after all retries have expired or due to
        // any other reason then reset the bBCapsReadMessagePending flag
        //
        bBCapsReadMessagePending = false;
    }

    parent->isHDCPCap = False;
    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_ERROR, "DP-QM> Message %s {%p} at '%s' failed. Device marked as not HDCP support.",
              from == &remoteBKSVReadMessage ? "REMOTE_DPCD_READ(BKSV)" :
              from == &remoteBCapsReadMessage ? "REMOTE_DPC_READ(BCaps)" :
              from == &remote22BCapsReadMessage ? "REMOTE_DPC_READ(22BCaps)" : "???",
              from, parent->address.toString(sb));

    // Destruct only when no message is pending
    if (!(bBKSVReadMessagePending || bBCapsReadMessagePending))
    {
        parent->isDeviceHDCPDetectionAlive = false;

        // Complete remote HDCP probe and check if can power down again.
        if (parent->connector)
        {
            parent->connector->decPendingRemoteHdcpDetection();
            parent->connector->isNoActiveStreamAndPowerdown();
        }

        delete this;
    }
}

void
DeviceHDCPDetection::expired
(
    const void *tag
)
{
    // Clear stale HDCP states when monitor instance is already destroyed
    if (!parent->plugged)
    {
        if (retryRemoteBKSVReadMessage)
        {
            retryRemoteBKSVReadMessage = false;
            bBKSVReadMessagePending = false;
        }
        else if (retryRemoteBCapsReadMessage)
        {
            retryRemoteBCapsReadMessage = false;
            bBCapsReadMessagePending = false;
        }
        else if (retryRemote22BCapsReadMessage)
        {
            retryRemote22BCapsReadMessage = false;
            bBCapsReadMessagePending = false;
        }

        if (!(bBKSVReadMessagePending || bBCapsReadMessagePending))
        {
            parent->isDeviceHDCPDetectionAlive = false;
            delete this;
        }
        return;
    }

    if (retryRemoteBKSVReadMessage)
    {
        Address parentAddress = parent->address.parent();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-QM> Requeing REMOTE_DPCD_READ_MESSAGE(BKSV) to %s", parentAddress.toString(sb));

        retryRemoteBKSVReadMessage = false;
        remoteBKSVReadMessage.set(parentAddress, parent->address.tail(), NV_DPCD_HDCP_BKSV_OFFSET, HDCP_KSV_SIZE);
        DP_PRINTF(DP_NOTICE, "DP-QM> Get BKSV (remotely) for '%s' sent REMOTE_DPCD_READ {%p}", parent->address.toString(sb), &remoteBKSVReadMessage);

        bBKSVReadMessagePending = true;
        messageManager->post(&remoteBKSVReadMessage, this);
    }

    if (retryRemoteBCapsReadMessage)
    {
        Address parentAddress = parent->address.parent();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-QM> Requeing REMOTE_DPCD_READ_MESSAGE(BCAPS) to %s", parentAddress.toString(sb));

        retryRemoteBCapsReadMessage = false;
        remoteBCapsReadMessage.set(parentAddress, parent->address.tail(), NV_DPCD_HDCP_BCAPS_OFFSET, HDCP_BCAPS_SIZE);
        DP_PRINTF(DP_NOTICE, "DP-QM> Get BCaps (remotely) for '%s' sent REMOTE_DPCD_READ {%p}", parent->address.toString(sb), &remoteBCapsReadMessage);

        bBCapsReadMessagePending = true;
        messageManager->post(&remoteBCapsReadMessage, this);
    }

    if (retryRemote22BCapsReadMessage)
    {
        Address parentAddress = parent->address.parent();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-QM> Requeing REMOTE_DPCD_READ_MESSAGE(22BCAPS) to %s", parentAddress.toString(sb));

        retryRemote22BCapsReadMessage = false;
        remote22BCapsReadMessage.set(parentAddress, parent->address.tail(), NV_DPCD_HDCP22_BCAPS_OFFSET, HDCP22_BCAPS_SIZE);
        DP_PRINTF(DP_NOTICE, "DP-QM> Get 22BCaps (remotely) for '%s' sent REMOTE_DPCD_READ {%p}", parent->address.toString(sb), &remote22BCapsReadMessage);

        bBCapsReadMessagePending = true;
        messageManager->post(&remote22BCapsReadMessage, this);
    }

}

DeviceHDCPDetection::~DeviceHDCPDetection()
{
    parent->isDeviceHDCPDetectionAlive = false;

    // Clear all pending callbacks/messages
    if (this->timer)
    {
        this->timer->cancelCallbacks(this);
    }

    if (this->messageManager)
    {
        this->messageManager->cancelAll(&remoteBKSVReadMessage);
        this->messageManager->cancelAll(&remoteBCapsReadMessage);
        this->messageManager->cancelAll(&remote22BCapsReadMessage);
    }
}

void
DeviceHDCPDetection::waivePendingHDCPCapDoneNotification()
{
    // Waive the pendingHDCPCapDone notification
    parent->shadow.hdcpCapDone = true;
    parent->isDeviceHDCPDetectionAlive = false;
    delete this;
}
