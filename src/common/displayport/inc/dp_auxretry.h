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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_auxretry.h                                                     *
*   Adapter interface for friendlier AuxBus                                 *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_AUXRETRY_H
#define INCLUDED_DP_AUXRETRY_H

#include "dp_auxbus.h"
#include "dp_timeout.h"

namespace DisplayPort
{
    enum
    {
        minimumRetriesOnDefer = 7
    };

    class AuxRetry
    {
        AuxBus * aux;
    public:
        AuxRetry(AuxBus * aux = 0)
            : aux(aux)
        {
        }

        AuxBus * getDirect()
        {
            return aux;
        }

        enum status
        {
            ack,
            nack,
            unsupportedRegister,
            defer
        };

        //
        //  Perform an aux read transaction.
        //    - Automatically handles defers up to retry limit
        //    - Retries on partial read
        //
        virtual status readTransaction(int address, NvU8 * buffer, unsigned size, unsigned retries = minimumRetriesOnDefer);

        //
        //  Similar to readTransaction except that it supports reading
        //  larger spans than AuxBus::transactionSize()
        //
        virtual status read(int address, NvU8 * buffer, unsigned size, unsigned retries = minimumRetriesOnDefer);

        //
        //  Perform an aux write transaction.
        //    - Automatically handles defers up to retry limit
        //    - Retries on partial write
        //
        virtual status writeTransaction(int address, NvU8 * buffer, unsigned size, unsigned retries = minimumRetriesOnDefer);

        //
        //  Similar to writeTransaction except that it supports writin
        //  larger spans than AuxBus::transactionSize()
        //
        virtual status write(int address, NvU8 * buffer, unsigned size, unsigned retries = minimumRetriesOnDefer);
    };

    class AuxLogger : public AuxBus
    {
        AuxBus * bus;
        char hex[256];
        char hex_body[256];
        char hint[128];

    public:
        AuxLogger(AuxBus * bus) : bus(bus)
        {
        }

        const char * getAction(Action action)
        {
            if (action == read)
                return "rd ";
            else if (action == write)
                return "wr ";
            else if (action == writeStatusUpdateRequest)
                return "writeStatusUpdateRequest ";
            else
                DP_ASSERT(0);
            return "???";
        }

        const char * getType(Type typ)
        {
            if (typ == native)
                return "";
            else if (typ == i2c)
                return "i2c ";
            else if (typ == i2cMot)
                return "i2cMot ";
            else
                DP_ASSERT(0);
            return "???";
        }

        const char * getStatus(status stat)
        {
            if (stat == success)
                return "";
            else if (stat == nack)
                return "(nack) ";
            else if (stat == defer)
                return "(defer) ";
            else
                DP_ASSERT(0);
            return "???";
        }

        const char * getRequestId(unsigned requestIdentifier)
        {
            switch(requestIdentifier)
            {
                case 0x1:  return "LINK_ADDRESS";
                case 0x4:  return "CLEAR_PAT";
                case 0x10: return "ENUM_PATH";
                case 0x11: return "ALLOCATE";
                case 0x12: return "QUERY";
                case 0x20: return "DPCD_READ";
                case 0x21: return "DPCD_WRITE";
                case 0x22: return "I2C_READ";
                case 0x23: return "I2C_WRITE";
                case 0x24: return "POWER_UP_PHY";
                case 0x25: return "POWER_DOWN_PHY";
                case 0x38: return "HDCP_STATUS";
                default:   return "";
            }
        }

        virtual status transaction(Action action, Type type, int address,
                                   NvU8 * buffer, unsigned sizeRequested,
                                   unsigned * sizeCompleted, unsigned * pNakReason,
                                   NvU8 offset, NvU8 nWriteTransactions);

        virtual unsigned transactionSize()
        {
            return bus->transactionSize();
        }

        virtual status fecTransaction(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags)
        {
            return bus->fecTransaction(fecStatus, fecErrorCount, flags);
        }
    };
}

#endif //INCLUDED_DP_AUXRETRY_H
