/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_sst_edid.c                                                     *
*    Implementation Single Stream EDID reads                                *
*                                                                           *
\***************************************************************************/

#include "dp_buffer.h"
#include "dp_auxbus.h"
#include "dp_internal.h"
#include "dp_edid.h"

using namespace DisplayPort;

/*
* seg -> 256 segment of EDID
* offset -> offset within segment
*/
static bool readNextBlock(AuxBus * auxBus, NvU8 seg, NvU8 offset, Buffer & buffer, unsigned & totalRead, unsigned DDCAddress, Timer * timer)
{
    AuxBus::Type type = AuxBus::i2cMot;
    AuxBus::status auxStatus;

    unsigned retries = 0;
    unsigned sizeRequested;
    unsigned sizeCompleted;
    unsigned transactionSize =  auxBus->transactionSize();
    totalRead = 0;

    DP_ASSERT(auxBus);
    DP_ASSERT(transactionSize > 0);

    // ASSERT if edidOffset offset wasn't increased in block len sizes
    DP_ASSERT(offset == 0 || offset == EDID_BLOCK_SIZE);

    sizeRequested = transactionSize;
    if (!buffer.resize(EDID_BLOCK_SIZE))
    {
        return false;
    }

    DP_ASSERT(sizeof(seg) == 1);
    DP_ASSERT(sizeof(offset) == 1);

    // only set segment if it is required
    if (seg)
    {
        // start EDID read by specifying appropriate Edid segment id
        for (unsigned retry = 0; retry < EDID_MAX_AUX_RETRIES; retry++)
        {
            auxStatus = auxBus->transaction(AuxBus::write, AuxBus::i2cMot, EDID_SEG_SELECTOR_OFFSET >> 1,
                &seg, sizeof(seg), &sizeCompleted);
            if (auxStatus == AuxBus::success)
                break;

            // If deferred due to timeout
            if (auxStatus == AuxBus::defer)
            {
                // Wait for sometime between retries
                timer->sleep(EDID_AUX_WAIT_TIME);
                continue;
            }

            return false;
        }
    }

    auxStatus = AuxBus::nack;

    for (retries = 0; totalRead < EDID_BLOCK_SIZE;)
    {
        //
        // For retry, rewrite the Offset for the internal read pointer
        // except when the previous Read auxstatus was an Aux::defer
        // since in that case, the offset was never incremented by sink
        //
        if ((auxStatus != AuxBus::success) && (auxStatus != AuxBus::defer))
        {
            // start from this offset, need to verify with display with multiple edid blocks
            for (unsigned retry = 0; retry < EDID_MAX_AUX_RETRIES; retry++)
            {
                auxStatus = auxBus->transaction(AuxBus::write, AuxBus::i2cMot, DDCAddress >> 1,
                    (NvU8*)(&offset), sizeof(offset), &sizeCompleted);
                if (auxStatus == AuxBus::success)
                    break;
                // If deferred due to timeout
                if (auxStatus == AuxBus::defer)
                {
                    // Wait for sometime between retries
                    timer->sleep(EDID_AUX_WAIT_TIME);
                    continue;
                }

                return false;
            }
            // if retries exceed EDID_MAX_AUX_RETRIES, give up
            if (auxStatus != AuxBus::success)
            {
                return false;
            }
        }
        // need to change to I2C (not MOT) to read just one last part of EDID block
        if (totalRead + transactionSize >= EDID_BLOCK_SIZE)
            type = AuxBus::i2c;

        sizeRequested = DP_MIN(transactionSize, EDID_BLOCK_SIZE - totalRead);
        auxStatus = auxBus->transaction(AuxBus::read, type, DDCAddress >> 1,
            &(buffer.data[totalRead]), sizeRequested, &sizeCompleted);

        if (AuxBus::success != auxStatus || (sizeRequested && (sizeCompleted == 0)))
        {
            if (retries >= EDID_MAX_AUX_RETRIES)
                return false;

            DP_LOG(("DisplayPort: %s: Retrying at totalRead 0x%08x (replyType %x, size %x)",
                __FUNCTION__, totalRead, auxStatus, sizeRequested));

            // Wait for sometime between retries
            timer->sleep(EDID_AUX_WAIT_TIME);
            retries++;

            continue;
        }

        // Assert when size mismatches and it is not last block
        if ((sizeRequested != sizeCompleted) &&
            (totalRead + transactionSize < EDID_BLOCK_SIZE))
        {
            DP_LOG(("DisplayPort: %s: dpAux returned edid block smaller than expected. Read from totalRead 0x%08x (replyType %x, size %x)",
                __FUNCTION__, totalRead, auxStatus, sizeRequested));
            DP_ASSERT(0);
        }

        retries     = 0;                // reset the number of retries
        totalRead   += sizeCompleted;
        offset      += (NvU8)sizeCompleted;
    }

    return true;
}

/*!
* @return: true => EDID read is success, false => read is failure
*/
static bool sstReadEdid(AuxBus * auxBus, Edid & edid, unsigned DDCAddr, Timer * timer, bool pendingTestRequestEdidRead)
{
    //
    // If there is pending test request for edid read,
    // ask edidReaderManager to take whatever posted,
    // instead of discarding bytes read by a failed read.
    // Because cert devices may need to see the checksum of these bytes,
    // even if they seem corrupted.
    //
    EdidAssembler edidReaderManager(&edid, pendingTestRequestEdidRead);
    NvU32 retryCount = 0;
    Buffer buffer;
    if (!buffer.resize(EDID_BLOCK_SIZE))
    {
        return false;
    }

    DP_ASSERT(auxBus);

    do
    {
        NvU8 seg = 0;
        NvU8 offset = 0;
        unsigned totalRead = 0;
        edidReaderManager.reset();

        // start by reading first EDID block, posting it and analyzing for next request
        do
        {
            bool success = readNextBlock(auxBus, seg, offset, buffer, totalRead, DDCAddr, timer);
            edidReaderManager.postReply(buffer, totalRead, success);
        }
        while (edidReaderManager.readNextRequest(seg, offset));
        if (!edid.isPatchedChecksum())
            break;
    } while (retryCount++ < EDID_POLICY_BLOCK_READ_MAX_RETRY_COUNT);

    //
    // EDID read is successful when
    //  1. read was done to the end (i.e. no corruption, no blocks exceeding retry count)
    //  2. EDID CRC is correct
    //
    return edidReaderManager.readIsComplete();
}

EDID_DDC DisplayPort::sstDDCPing(AuxBus  & dpAux)
{
    unsigned sizeRequested = 0, sizeCompleted;
    AuxBus::status auxStatus = AuxBus::nack;
    NvU8 offset = 0;
    unsigned ddcAddrIdx;

    for (ddcAddrIdx = 0; ddcAddrIdx < ddcAddrListSize; ddcAddrIdx++)
    {
        //
        //  Don't use an I2C write.  Some devices erroneously ACK on the write
        //
        auxStatus = dpAux.transaction(AuxBus::read, AuxBus::i2c, ddcAddrList[ddcAddrIdx] >> 1,
            &offset, sizeRequested, &sizeCompleted);

        if (AuxBus::success == auxStatus)
            return (EDID_DDC)ddcAddrList[ddcAddrIdx];
    }

    return EDID_DDC_NONE;

}

bool DisplayPort::EdidReadSST(Edid & edid, AuxBus * auxBus, Timer* timer,
                              bool pendingTestRequestEdidRead, bool bBypassAssembler,
                              MainLink * main)
{
    Edid previousEdid;
    Buffer *buffer;
    bool status;
    bool firstTrial = true;
    NvU64 startTime, elapsedTime;
    for (unsigned i = 0; i < ddcAddrListSize; i++)
    {
        startTime = timer->getTimeUs();
        elapsedTime = 0;
        do
        {
            //
            // Client asks to use RM control code to fetch EDID.
            //
            if (bBypassAssembler && main)
            {
                unsigned blockCnt;
                buffer = edid.getBuffer();
                if (!buffer->resize(EDID_BLOCK_SIZE))
                {
                    return false;
                }
                status = main->fetchEdidByRmCtrl(buffer->getData(), buffer->getLength());

                if (status)
                {
                    blockCnt = edid.getBlockCount();

                    // If read successfully, check if there are two or more blocks.
                    if (blockCnt != 1)
                    {
                        if (!buffer->resize(EDID_BLOCK_SIZE * blockCnt))
                        {
                            return false;
                        }
                        status = main->fetchEdidByRmCtrl(buffer->getData(), buffer->getLength());
                    }
                }
                if (!status)
                {
                    //
                    // If fetchEdidByRmCtrl fails for some reasons:
                    // Try to read again using DPLib read function.
                    // One reason client to request read from RM is to making sure
                    // the EDID is overridden (regkey or others). So call the RM
                    // control call to apply the EDID overrides.
                    //
                    status = sstReadEdid(auxBus, edid, ddcAddrList[i], timer,
                                         pendingTestRequestEdidRead);
                    if (status)
                    {
                        main->applyEdidOverrideByRmCtrl(buffer->getData(),
                                                        buffer->getLength());
                    }
                    else
                    {
                        DP_LOG(("EDID> Failed to read EDID from RM and DPLib"));
                    }
                }
            }
            else
            {
                //
                // If there is pending test request for edid read, make sure we get the raw bytes without check.
                // Because cert devices may need to see the checksum of whatever is read for edid, even if they seem corrupted.
                //
                status = sstReadEdid(auxBus, edid, ddcAddrList[i], timer, pendingTestRequestEdidRead);

            }

            if (status)
            {
                if (edid.verifyCRC())
                {
                    return true;
                }
                else
                {
                    if (firstTrial) // first failure?
                    {
                        previousEdid.swap(edid);
                        firstTrial = false;
                    }
                    else
                    {
                        if (previousEdid == edid)
                        {
                            // we got the same invalid checksum again; we will assume it is valid.
                            edid.setForcedEdidChecksum(true);
                            return true;
                        }
                    }
                }
            }
            elapsedTime = timer->getTimeUs() - startTime;
            timer->sleep(1);
        } while (elapsedTime < (EDID_READ_RETRY_TIMEOUT_MS * 1000));
    }

    DP_LOG(("EDID> Failed to ping sst DDC addresses"));

    return false;
}
