/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFIFR_H_
#define _SOEIFIFR_H_

#include "flcnifcmn.h"

#define INFOROM_FS_FILE_NAME_SIZE       3
#define INFOROM_BBX_OBJ_XID_ENTRIES     10

enum
{
    RM_SOE_IFR_READ,
    RM_SOE_IFR_WRITE,
    RM_SOE_IFR_BBX_INITIALIZE,
    RM_SOE_IFR_BBX_SHUTDOWN,
    RM_SOE_IFR_BBX_SXID_ADD,
    RM_SOE_IFR_BBX_SXID_GET,
    RM_SOE_IFR_BBX_DATA_GET,
};

enum
{
    RM_SOE_IFR_BBX_GET_NONE,
    RM_SOE_IFR_BBX_GET_SXID,
    RM_SOE_IFR_BBX_GET_SYS_INFO,
    RM_SOE_IFR_BBX_GET_TIME_INFO,
    RM_SOE_IFR_BBX_GET_TEMP_DATA,
    RM_SOE_IFR_BBX_GET_TEMP_SAMPLES,
};

typedef struct
{
    NvU8        cmdType;
    RM_FLCN_U64 dmaHandle;
    NvU32       offset;
    NvU32       sizeInBytes;
    char        fileName[INFOROM_FS_FILE_NAME_SIZE];
} RM_SOE_IFR_CMD_PARAMS;

typedef struct
{
    NvU8        cmdType;
    RM_FLCN_U64 time;
    NvU8        osType;
    NvU32       osVersion;
} RM_SOE_IFR_CMD_BBX_INIT_PARAMS;

typedef struct
{
    NvU8        cmdType;
} RM_SOE_IFR_CMD_BBX_SHUTDOWN_PARAMS;

typedef struct
{
    NvU8        cmdType;
    NvU32       exceptionType;
    NvU32       data[3];
} RM_SOE_IFR_CMD_BBX_SXID_ADD_PARAMS;

typedef struct
{
    NvU8 cmdType;
    NvU32 sizeInBytes;
    RM_FLCN_U64 dmaHandle;
} RM_SOE_IFR_CMD_BBX_SXID_GET_PARAMS;

typedef struct
{
    NvU8 cmdType;
    NvU32 sizeInBytes;
    RM_FLCN_U64 dmaHandle;
    NvU8 dataType;
} RM_SOE_IFR_CMD_BBX_GET_DATA_PARAMS;

typedef union
{
	NvU8	cmdType;
	RM_SOE_IFR_CMD_PARAMS params;
    RM_SOE_IFR_CMD_BBX_INIT_PARAMS bbxInit;
    RM_SOE_IFR_CMD_BBX_SXID_ADD_PARAMS bbxSxidAdd;
    RM_SOE_IFR_CMD_BBX_SXID_GET_PARAMS bbxSxidGet;
    RM_SOE_IFR_CMD_BBX_GET_DATA_PARAMS bbxDataGet;
} RM_SOE_IFR_CMD;

// entry of getSxid
typedef struct
{
    NvU32 sxid;
    NvU32 timestamp;
} RM_SOE_BBX_SXID_ENTRY;

// SXID data array return to getSxid
typedef struct
{
    NvU32 sxidCount;
    RM_SOE_BBX_SXID_ENTRY sxidFirst[INFOROM_BBX_OBJ_XID_ENTRIES];
    RM_SOE_BBX_SXID_ENTRY sxidLast[INFOROM_BBX_OBJ_XID_ENTRIES];
} RM_SOE_BBX_GET_SXID_DATA;

// NVSwitch system version information returning with the command GET_SYS_INFO
typedef struct
{
    NvU32 driverLo;             //Driver Version Low 32 bits
    NvU16 driverHi;             //Driver Version High 16 bits
    NvU32 vbiosVersion;         //VBIOS Version 
    NvU8 vbiosVersionOem;       //VBIOS OEM Version byte
    NvU8  osType;               //OS Type (UNIX/WIN/WIN2K/WIN9x/OTHER)
    NvU32 osVersion;            //OS Version (Build|MINOR|MAJOR)
} RM_SOE_BBX_GET_SYS_INFO_DATA;

// NVSwitch time information returning with the command GET_TIME_INFO
typedef struct
{
    NvU32 timeStart;            //Timestamp (EPOCH) when the driver was loaded on the GPU for the first time
    NvU32 timeEnd;              //Timestamp (EPOCH) when the data was last flushed
    NvU32 timeRun;              //Amount of time (in seconds) driver was loaded, and GPU has run
    NvU32 time24Hours;          //Timestamp (EPOCH) of when the first 24 operational hours is hit
    NvU32 time100Hours;         //Timestamp (EPOCH) of when the first 100 operational hours is hit
} RM_SOE_BBX_GET_TIME_INFO_DATA;

#define RM_SOE_BBX_TEMP_DAY_ENTRIES               5
#define RM_SOE_BBX_TEMP_WEEK_ENTRIES              5
#define RM_SOE_BBX_TEMP_MNT_ENTRIES               5
#define RM_SOE_BBX_TEMP_ALL_ENTRIES               5
#define RM_SOE_BBX_TEMP_SUM_HOUR_ENTRIES          23
#define RM_SOE_BBX_TEMP_SUM_DAY_ENTRIES           5
#define RM_SOE_BBX_TEMP_SUM_MNT_ENTRIES           3
#define RM_SOE_BBX_TEMP_HISTOGRAM_THLD_ENTRIES    20
#define RM_SOE_BBX_TEMP_HISTOGRAM_TIME_ENTRIES    21
#define RM_SOE_BBX_TEMP_HOURLY_MAX_ENTRIES        168
#define RM_SOE_BBX_TEMP_COMPRESS_BUFFER_ENTRIES   1096
#define RM_SOE_BBX_NUM_COMPRESSION_PERIODS        8

// NVSwitch Temperature Entry
typedef struct
{
    NvU16 value;                //Temperature (SFXP 9.7 format in Celsius)
    NvU32 timestamp;            //Timestamp (EPOCH) of when the entry is recorded
} RM_SOE_BBX_TEMP_ENTRY;

// NVSwitch Temperature Data returning with the command GET_TEMP_DATA
typedef struct
{
    NvU32                 tempMaxDayIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMaxDay[RM_SOE_BBX_TEMP_DAY_ENTRIES];
    NvU32                 tempMaxWeekIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMaxWeek[RM_SOE_BBX_TEMP_WEEK_ENTRIES];
    NvU32                 tempMaxMntIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMaxMnt[RM_SOE_BBX_TEMP_MNT_ENTRIES];
    NvU32                 tempMaxAllIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMaxAll[RM_SOE_BBX_TEMP_ALL_ENTRIES];
    NvU32                 tempMinDayIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMinDay[RM_SOE_BBX_TEMP_DAY_ENTRIES];
    NvU32                 tempMinWeekIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMinWeek[RM_SOE_BBX_TEMP_WEEK_ENTRIES];
    NvU32                 tempMinMntIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMinMnt[RM_SOE_BBX_TEMP_MNT_ENTRIES];
    NvU32                 tempMinAllIdx;
    RM_SOE_BBX_TEMP_ENTRY tempMinAll[RM_SOE_BBX_TEMP_ALL_ENTRIES];
    NvU32                 tempSumDelta;
    NvU32                 tempSumHour[RM_SOE_BBX_TEMP_SUM_HOUR_ENTRIES];
    NvU32                 tempSumDay[RM_SOE_BBX_TEMP_SUM_DAY_ENTRIES];
    NvU32                 tempSumMnt[RM_SOE_BBX_TEMP_SUM_MNT_ENTRIES];
    NvU32                 tempHistogramThld[RM_SOE_BBX_TEMP_HISTOGRAM_THLD_ENTRIES];
    NvU32                 tempHistogramTime[RM_SOE_BBX_TEMP_HISTOGRAM_TIME_ENTRIES];
    RM_SOE_BBX_TEMP_ENTRY tempHourlyMaxSample[RM_SOE_BBX_TEMP_HOURLY_MAX_ENTRIES];
} RM_SOE_BBX_GET_TEMP_DATA;

// NVSwitch Temperature Compressed Samples returning with the command GET_TEMP_SAMPLES
typedef struct
{
    NvU32                 compressionPeriodIdx;
    NvU32                 compressionPeriod[RM_SOE_BBX_NUM_COMPRESSION_PERIODS];
    RM_SOE_BBX_TEMP_ENTRY tempCompressionBuffer[RM_SOE_BBX_TEMP_COMPRESS_BUFFER_ENTRIES];
} RM_SOE_BBX_GET_TEMP_SAMPLES;

#endif // _SOEIFIFR_H_
