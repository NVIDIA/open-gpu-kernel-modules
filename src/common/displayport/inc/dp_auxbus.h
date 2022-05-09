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
* Module: dp_auxbus.h                                                       *
*    Interface for low level access to the aux bus.                         *
*    This is the synchronous version of the interface.                      *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_AUXBUS_H
#define INCLUDED_DP_AUXBUS_H

namespace DisplayPort
{
    class AuxBus : virtual public Object
    {
    public:
        enum status
        {
            success,
            defer,
            nack,
            unSupported,
        };

        enum Action
        {
            read,
            write,
            writeStatusUpdateRequest,    // I2C only
        };

        enum Type
        {
            native,
            i2c,
            i2cMot
        };

        virtual status transaction(Action action, Type type, int address,
                                   NvU8 * buffer, unsigned sizeRequested,
                                   unsigned * sizeCompleted,
                                   unsigned * pNakReason = NULL,
                                   NvU8 offset = 0, NvU8 nWriteTransactions = 0) = 0;

        virtual unsigned transactionSize() = 0;              
        virtual status fecTransaction(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags) { return nack; }          
        virtual void setDevicePlugged(bool) {}
        virtual ~AuxBus() {}
    };

    //
    //  Wraps an auxbus interface with one that prints all the input and output
    //
    AuxBus * CreateAuxLogger(AuxBus * auxBus);
}

#endif //INCLUDED_DP_AUXBUS_H
