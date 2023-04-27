/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _PMGR_NVSWITCH_H_
#define _PMGR_NVSWITCH_H_

#include "ctrl_dev_nvswitch.h"

#define NVSWITCH_BITS_PER_BYTE       8

#define NVSWITCH_HIGH NV_TRUE
#define NVSWITCH_LOW  NV_FALSE

/*! Extract the first byte of a 10-bit address. */
#define NVSWITCH_GET_ADDRESS_10BIT_FIRST(a) ((NvU8)((((a) >> 8) & 0x6) | 0xF0))

/*! Extract the second byte of a 10-bit address. */
#define NVSWITCH_GET_ADDRESS_10BIT_SECOND(a) ((NvU8)(((a) >> 1) & 0xFF))

/*! Attaching read to read application interface */
#define NVSWITCH_I2C_READ(a,b) _nvswitch_i2c_i2cRead(device, a, b)

#define NVSWITCH_I2C_DELAY(a)    NVSWITCH_NSEC_DELAY(a)

#define NVSWITCH_MAX_I2C_PORTS       4

/*! bit 0 of address set indicates read cycle to follow */
#define NVSWITCH_I2C_READCYCLE                                              ((NvU8)0x01)

/*! Determine if an address is valid in the 7-bit address space. */
#define NVSWITCH_I2C_IS_7BIT_I2C_ADDRESS(a)                                ((a) <= 0xFF)

/*! Determine if an address is valid in the 10-bit address space. */
#define NVSWITCH_I2C_IS_10BIT_I2C_ADDRESS(a)                              ((a) <= 0x7FF)

// by-the-spec delay defaults (yields 100KHz)
#define NVSWITCH_I2C_PROFILE_STANDARD_tF              300
#define NVSWITCH_I2C_PROFILE_STANDARD_tR             1000
#define NVSWITCH_I2C_PROFILE_STANDARD_tSUDAT         1800    // actually, spec calls for (min) 250, but we've borrowed from tHDDAT
#define NVSWITCH_I2C_PROFILE_STANDARD_tHDDAT         1900    // actually, spec calls for (max) 3450, but we've loaned time to tSUDAT
#define NVSWITCH_I2C_PROFILE_STANDARD_tHIGH          4000
#define NVSWITCH_I2C_PROFILE_STANDARD_tSUSTO         4000
#define NVSWITCH_I2C_PROFILE_STANDARD_tHDSTA         4000
#define NVSWITCH_I2C_PROFILE_STANDARD_tSUSTA         4700
#define NVSWITCH_I2C_PROFILE_STANDARD_tBUF           4700
#define NVSWITCH_I2C_PROFILE_STANDARD_tLOW           4700    // NVSWITCH_I2C_PROFILE_STANDARD_tSUDAT + NVSWITCH_I2C_PROFILE_STANDARD_tR + NVSWITCH_I2C_PROFILE_STANDARD_tHDDAT
#define NVSWITCH_I2C_PROFILE_STANDARD_CYCLEPERIOD   10000    // NVSWITCH_I2C_PROFILE_STANDARD_tF + NVSWITCH_I2C_PROFILE_STANDARD_tLOW + NVSWITCH_I2C_PROFILE_STANDARD_tR + NVSWITCH_I2C_PROFILE_STANDARD_tHIGH

// by-the-spec delay defaults (yields 400KHz)
#define NVSWITCH_I2C_PROFILE_FAST_tF                  300
#define NVSWITCH_I2C_PROFILE_FAST_tR                  300
#define NVSWITCH_I2C_PROFILE_FAST_tSUDAT              200    // actually, spec calls for (min) 100, but we've borrowed from tHDDAT
#define NVSWITCH_I2C_PROFILE_FAST_tHDDAT              800    // actually, spec calls for (max) 900, but we've loaned time to tSUDAT
#define NVSWITCH_I2C_PROFILE_FAST_tHIGH               600
#define NVSWITCH_I2C_PROFILE_FAST_tSUSTO              600
#define NVSWITCH_I2C_PROFILE_FAST_tHDSTA              600
#define NVSWITCH_I2C_PROFILE_FAST_tSUSTA              600
#define NVSWITCH_I2C_PROFILE_FAST_tBUF               1300
#define NVSWITCH_I2C_PROFILE_FAST_tLOW               1300    // NVSWITCH_I2C_PROFILE_STANDARD_tSUDAT + NVSWITCH_I2C_PROFILE_STANDARD_tR + NVSWITCH_I2C_PROFILE_STANDARD_tHDDAT
#define NVSWITCH_I2C_PROFILE_FAST_CYCLEPERIOD        2500    // NVSWITCH_I2C_PROFILE_STANDARD_tF + NVSWITCH_I2C_PROFILE_STANDARD_tLOW + NVSWITCH_I2C_PROFILE_STANDARD_tR + NVSWITCH_I2C_PROFILE_STANDARD_tHIGH

/*!
 * The I2C specification does not specify any timeout conditions for clock
 * stretching, i.e. any device can hold down SCL as long as it likes so this
 * value needs to be adjusted on case by case basis.
 */
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_1200US  1200
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_1000KHZ    (NVSWITCH_I2C_SCL_CLK_TIMEOUT_100KHZ * 4)
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_400KHZ     (NVSWITCH_I2C_SCL_CLK_TIMEOUT_100KHZ * 4)
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_300KHZ     (NVSWITCH_I2C_SCL_CLK_TIMEOUT_100KHZ * 3)
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_200KHZ     (NVSWITCH_I2C_SCL_CLK_TIMEOUT_100KHZ * 2)
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_100KHZ     (NVSWITCH_I2C_SCL_CLK_TIMEOUT_1200US / 10)

/* A reasonable SCL timeout is five cycles at 20 KHz.  Full use should be rare
 * in devices, occurring when in the middle of a real-time task. That comes to
 * 25 clock cycles at 100 KHz, or 250 us. */
#define NVSWITCH_I2C_SCL_CLK_TIMEOUT_250US 250

/* We don't want I2C to deal with traffic slower than 20 KHz (50 us cycle).
 */
#define NVSWITCH_I2C_MAX_CYCLE_US 50

/* The longest HW I2C transaction: S BYTE*2 S BYTE*4 P, at 1 each for S/P, and
 * 9 for each byte (+ack). */
#define NVSWITCH_I2C_HW_MAX_CYCLES ((1 * 3) + (9 * 6))

/* We determine the HW operational timeout as the longest operation, plus two
 * long SCL clock stretches. */
#define I2C_HW_IDLE_TIMEOUT_NS (1000 * \
    ((NVSWITCH_I2C_MAX_CYCLE_US * NVSWITCH_I2C_HW_MAX_CYCLES) + (NVSWITCH_I2C_SCL_CLK_TIMEOUT_1200US * 2)))

//
// PMGR board configuration information
//

#define NVSWITCH_DESCRIBE_I2C_DEVICE(_port, _addr, _type, _rdWrAccessMask)        \
    {NVSWITCH_I2C_PORT ## _port, _addr, NVSWITCH_I2C_DEVICE ## _type, _rdWrAccessMask}

#define NVSWITCH_DESCRIBE_GPIO_PIN(_pin, _func, _hw_select, _misc_io)    \
    {_pin, NVSWITCH_GPIO_ENTRY_FUNCTION ## _func, _hw_select,   \
        NVSWITCH_GPIO_ENTRY_MISC_IO_ ## _misc_io}

/*! Structure containing a description of the I2C bus as needed by the software
 *  bit-banging implementation.
 */
typedef struct
{
    NvU32 sclOut;      // Bit number for SCL Output
    NvU32 sdaOut;      // Bit number for SDA Output

    NvU32 sclIn;       // Bit number for SCL Input
    NvU32 sdaIn;       // Bit number for SDA Input

    NvU32 port;        // Port number of the driving lines
    NvU32 curLine;     // Required for isLineHighFunction

    NvU32 regCache;    // Keeps the cache value of registers.
    //
    // The following timings are used as stand-ins for I2C spec timings, so
    // that different speed modes may share the same code.
    //
    NvU16 tF;
    NvU16 tR;
    NvU16 tSuDat;
    NvU16 tHdDat;
    NvU16 tHigh;
    NvU16 tSuSto;
    NvU16 tHdSta;
    NvU16 tSuSta;
    NvU16 tBuf;
    NvU16 tLow;
} NVSWITCH_I2C_SW_BUS;

/*! @brief Internal Command structure for HW I2C to perform I2C transaction */
typedef struct
{
    NvU32   port;
    NvU32   bRead;
    NvU32   cntl;
    NvU32   data;
    NvU32   bytesRemaining;
    NvS32   status;
    NvU8   *pMessage;
    NvBool  bBlockProtocol;
} NVSWITCH_I2C_HW_CMD, *PNVSWITCH_I2C_HW_CMD;

typedef enum {
    i2cProfile_Standard,
    i2cProfile_Fast,
    i2cProfile_End
} NVSWITCH_I2CPROFILE;

typedef enum
{
    pmgrReg_i2cAddr,
    pmgrReg_i2cCntl,
    pmgrReg_i2cTiming,
    pmgrReg_i2cOverride,
    pmgrReg_i2cPoll,
    pmgrReg_i2cData,
    pmgrReg_unsupported
} NVSWITCH_PMGRREG_TYPE;


// I2C Speed limits
#define NVSWITCH_I2C_SPEED_LIMIT_NONE                 NV_U16_MAX  //Close enough to not having a speed limit.
#define NVSWITCH_I2C_SPEED_1000KHZ                    1000
#define NVSWITCH_I2C_SPEED_400KHZ                     400
#define NVSWITCH_I2C_SPEED_300KHZ                     300
#define NVSWITCH_I2C_SPEED_200KHZ                     200
#define NVSWITCH_I2C_SPEED_100KHZ                     100

enum
{
    i2cSpeedLimit_dcb = 0,
    i2cSpeedLimit_ctrl,

    // Always leave as last element!
    NVSWITCH_I2C_SPEED_LIMIT_MAX_DEVICES
};


// Timing for I2C cycles (allows for possibility of tweaking timing)
typedef struct __NVSWITCH_NVSWITCH_I2CTIMING
{
    NvU32 tR;            // at 100KHz, normally 1000ns
    NvU32 tF;            // at 100KHz, normally  300ns
    NvU32 tHIGH;         // at 100KHz, normally 4000ns
    NvU32 tSUDAT;        // at 100KHz, normally  250ns (min), but we borrow time from tHDDAT to improve clock phase
    NvU32 tHDDAT;        // at 100KHz, normally 3450ns (max), but we loan time to tSUDAT to improve clock phase
    NvU32 tSUSTO;        // at 100KHz, normally 4000ns
    NvU32 tHDSTA;        // at 100KHz, normally 4000ns
    NvU32 tBUF;          // at 100KHz, normally 4700ns

    NvU32 tLOW;          // computed to be:  tSUDAT + tR + tHDDAT

    NvU32 speed;         // Port speed

} NVSWITCH_I2CTIMING;

#define NV_NVSWITCH_I2C_DEVICE_WRITE_ACCESS_LEVEL              2:0
#define NV_NVSWITCH_I2C_DEVICE_READ_ACCESS_LEVEL               5:3
#define NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_PUBLIC             0x00000000
#define NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_PRIVILEGED         0x00000001
#define NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_INTERNAL           0x00000002
#define NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_INACCESSIBLE       0x00000003
#define NV_NVSWITCH_I2C_DEVICE_READ_ACCESS_LEVEL_PUBLIC        NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_PUBLIC
#define NV_NVSWITCH_I2C_DEVICE_READ_ACCESS_LEVEL_PRIVILEGED    NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_PRIVILEGED
#define NV_NVSWITCH_I2C_DEVICE_READ_ACCESS_LEVEL_INTERNAL      NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_INTERNAL
#define NV_NVSWITCH_I2C_DEVICE_READ_ACCESS_LEVEL_INACCESSIBLE  NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_INACCESSIBLE
#define NV_NVSWITCH_I2C_DEVICE_WRITE_ACCESS_LEVEL_PUBLIC       NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_PUBLIC
#define NV_NVSWITCH_I2C_DEVICE_WRITE_ACCESS_LEVEL_PRIVILEGED   NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_PRIVILEGED
#define NV_NVSWITCH_I2C_DEVICE_WRITE_ACCESS_LEVEL_INTERNAL     NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_INTERNAL
#define NV_NVSIWTCH_I2C_DEVICE_WRITE_ACCESS_LEVEL_INACCESSIVLE NV_NVSWITCH_I2C_DEVICE_ACCESS_LEVEL_INACCESSIBLE

typedef struct NVSWITCH_I2C_DEVICE_DESCRIPTOR
{
    NVSWITCH_I2C_PORT_TYPE      i2cPortLogical;     //<! Logical I2C port where the device sits
    NvU32                       i2cAddress;         //<! I2C slave address
    NVSWITCH_I2C_DEVICE_TYPE    i2cDeviceType;
    NvU8                        i2cRdWrAccessMask;
} NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE;


typedef struct NVSWITCH_OBJI2C   *PNVSWITCH_OBJI2C;

#define NVSWITCH_I2C_SPEED_MODE_100KHZ  0
#define NVSWITCH_I2C_SPEED_MODE_200KHZ  1
#define NVSWITCH_I2C_SPEED_MODE_300KHZ  2
#define NVSWITCH_I2C_SPEED_MODE_400KHZ  3
#define NVSWITCH_I2C_SPEED_MODE_1000KHZ 4

typedef struct _nvswitch_tag_i2c_port
{
    // Timing for I2C cycles (allows for possibility of tweaking timing)
    NVSWITCH_I2CTIMING Timing;

    NVSWITCH_I2C_HW_CMD  hwCmd;

    NvU32 defaultSpeedMode;
} NVSWITCH_I2CPORT, *PNVSWITCH_I2CPORT;


struct NVSWITCH_OBJI2C
{
    //
    // Addresses of I2C ports
    //
    // Note: The index of array is logical port number NOT physical
    //
    NVSWITCH_I2CPORT Ports[NVSWITCH_MAX_I2C_PORTS];

    //
    // Private data
    //

    // I2C Mutex/Synchronization state
    NvU32         I2CAcquired;

    NvU32 PortInfo[NVSWITCH_MAX_I2C_PORTS];
#define NV_I2C_PORTINFO_DEFINED                             0:0
#define NV_I2C_PORTINFO_DEFINED_ABSENT               0x00000000
#define NV_I2C_PORTINFO_DEFINED_PRESENT              0x00000001
#define NV_I2C_PORTINFO_ACCESS_ALLOWED                      1:1
#define NV_I2C_PORTINFO_ACCESS_ALLOWED_FALSE         0x00000000
#define NV_I2C_PORTINFO_ACCESS_ALLOWED_TRUE          0x00000001

    NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE           *device_list;
    NvU32                                     device_list_size;

    // I2C device allow list
    NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE        *i2c_allow_list;
    NvU32                                  i2c_allow_list_size;

    // For I2C via SOE support
    NvBool      soeI2CSupported;
    NvBool      kernelI2CSupported;
    void        *pCpuAddr;
    NvU64       dmaHandle;
};

//
// Thermal
//

#define     NVSWITCH_THERM_METHOD_UNKNOWN   0x00
#define     NVSWITCH_THERM_METHOD_I2C       0x01
#define     NVSWITCH_THERM_METHOD_MCU       0x02

typedef struct nvswitch_tdiode_info_type
{
    NvU32   method;
    struct NVSWITCH_I2C_DEVICE_DESCRIPTOR *method_i2c_info;

    NvS32   A;
    NvS32   B;
    NvS32   offset;
} NVSWITCH_TDIODE_INFO_TYPE;

void nvswitch_i2c_destroy(nvswitch_device *device);
void nvswitch_i2c_init(nvswitch_device *device);

#endif //_PMGR_NVSWITCH_H_
