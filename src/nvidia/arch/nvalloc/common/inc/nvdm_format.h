/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef NVDM_FORMAT_H
#define NVDM_FORMAT_H

/**
 * NVDM (NVIDIA Data Model) overlaid on MCTP (Management Component Transport
 * Protocol) format definitions.
 *
 * This header consolidates NVDM type definitions from multiple RM subsystems
 * including FSP, SEC2, GSP, and others. NVDM is a vendor-defined message layer
 * overlaid on MCTP that provides a simple way for different subsystems to
 * communicate.
 */

//
// List of NVDM Message Types
//
// Values are allocated from a shared namespace across all subsystems.
// Add new types here and update the NVDM Message Type allocation page.
//
#define NVDM_TYPE_RESET                       0x4
#define NVDM_TYPE_HULK                        0x11
#define NVDM_TYPE_FIRMWARE_UPDATE             0x12
#define NVDM_TYPE_PRC                         0x13
#define NVDM_TYPE_COT                         0x14
#define NVDM_TYPE_FSP_RESPONSE                0x15
#define NVDM_TYPE_SEC2_RESPONSE               NVDM_TYPE_FSP_RESPONSE
#define NVDM_TYPE_CAPS_QUERY                  0x16
#define NVDM_TYPE_INFOROM                     0x17
#define NVDM_TYPE_SMBPBI                      0x18
#define NVDM_TYPE_ROMREAD                     0x1A
#define NVDM_TYPE_UEFI_RM                     0x1C
#define NVDM_TYPE_UEFI_XTL_DEBUG_INTR         0x1D
#define NVDM_TYPE_TNVL                        0x1F
#define NVDM_TYPE_CLOCK_BOOST                 0x20
#define NVDM_TYPE_FSP_GSP_COMM                0x21
#define NVDM_TYPE_SEC2_GSP_COMM               NVDM_TYPE_FSP_GSP_COMM
#define NVDM_TYPE_FSP_GSP_RM_METHOD_OF_COMMS  0x23

#define NVDM_TYPE_RM_RPC                0x25  // GSP-RM RPC messages
#define NVDM_TYPE_GMCAPI                0x26  // GSP-RM GMCAPI messages

#endif // NVDM_FORMAT_H
