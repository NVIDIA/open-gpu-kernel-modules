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

#ifndef _SMBPBI_PRIV_H_
#define _SMBPBI_PRIV_H_

/*!
 * This file contains NVIDIA private defines for the SMBPBI
 * interface.
 */

#include "oob/smbpbi.h"

// MSGBOX command word structure

//
// Reserving opcodes above 0xC0 for internal/private functionationality.
//
// These opcodes should not be included in any documentation we release outside
// of NVIDIA!
//

//
// Only for internal use (should not be written to the command register). Used
// for internal tracking when commands are redirected to the RM from the PMU.
//
#define NV_MSGBOX_CMD_ERR_MORE_PROCESSING_REQUIRED              0x000000F0

//
// Alternative encodings of the command word.
// These were distinguished by a non-zero value in the 29:29 bit.
// Bit 29 is now reserved and must be 0 i.e. only standard requests will be processed
// and debug requests would fail.
//
#define NV_MSGBOX_CMD_ENCODING                               29:29
#define NV_MSGBOX_CMD_ENCODING_STANDARD                 0x00000000

#endif // _SMBPBI_PRIV_H_

