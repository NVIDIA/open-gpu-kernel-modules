/*
 * SPDX-FileCopyrightText: Copyright (c) <year> NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _IFR_DEM_H_
#define _IFR_DEM_H_

#define INFOROM_DEM_OBJECT_V1_00_PACKED_SIZE 4144
#define INFOROM_DEM_OBJECT_V1_00_FIFO_SIZE 4096

struct INFOROM_DEM_OBJECT_V1_00
{
    INFOROM_OBJECT_HEADER_V1_00 header;
    inforom_U032    seqNumber;
    inforom_U016    writeOffset;
    inforom_U016    readOffset;
    inforom_X008    fifoBuffer[INFOROM_DEM_OBJECT_V1_00_FIFO_SIZE];
    inforom_U032    reserved[8];
};

#define INFOROM_DEM_OBJECT_V1_00_FMT INFOROM_OBJECT_HEADER_V1_00_FMT "d2w4096x8d"
typedef struct INFOROM_DEM_OBJECT_V1_00 INFOROM_DEM_OBJECT_V1_00;

#endif // _IFR_DEM_H_
