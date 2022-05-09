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
/*****************************************************************************
*
* Description:
* RM Event Tracer
*
******************************************************************************/


#ifndef TRACER_H
#define TRACER_H


#define RMTRACE_EVENT_FLAG_ENABLED      NVBIT(0)      // whether the event is enabled
#define RMTRACE_EVENT_FLAG_FREQ         NVBIT(1)      // an event frequently happens.  In ETW, freq events use an independent Provider 'NVRM_FREQ'
#define RMTRACE_EVENT_FLAG_FLUSHABLE    NVBIT(2)      // an event may cause a flush.  'Flush' is enabled by regkey 'RMEnableEventTracer'

//
// Function Progress Value
//
// RMTRACE_FUNC_PROG_ENTER and RMTRACE_FUNC_PROG_EXIT are used to indicate enter and leave state of a specific events, so that the time interval
// of this event can be computed.
//
// RMTRACE_FUNC_PROG_STEP can be combined with RMTRACE_FUNC_PROG_ENTER and RMTRACE_FUNC_PROG_EXIT, used in ETW_Event_Generic_Marker. Below is a
// sample how to use it.
//
// void AAA()
// {
//      RMTRACE_MARKER_PROBE("AAA Function", pGpu->gpuId, data, RMTRACE_FUNC_PROG_ENTER);
//      ...
//      RMTRACE_MARKER_PROBE("About To Enter XXX", pGpu->gpuId, data, RMTRACE_FUNC_PROG_STEP);
//      XXX();
//      ...
//      BBB();
//      ...
//      RMTRACE_MARKER_PROBE("AAA Function", pGpu->gpuId, data, RMTRACE_FUNC_PROG_EXIT);
// }
//
//
// void BBB()
// {
//      RMTRACE_MARKER_PROBE("BBB Function", pGpu->gpuId, data, RMTRACE_FUNC_PROG_ENTER);
//      ...
//      CCC();
//      ...
//      RMTRACE_MARKER_PROBE("BBB Function", pGpu->gpuId, data, RMTRACE_FUNC_PROG_EXIT);
// }
//
// With a tool (like EtwTool), we can generate below message automatically
//
// AAA Function (Enter)
//      (0.1234ms)
//   About to Enter XXX
//      (0.0012ms)
//      BBB Function (Enter)
//      BBB Function (Leave) - 0.23ms
// AAA Function (Leave) -- 0.4111ms
//

#define RMTRACE_FUNC_PROG_ENTER          0x0000
#define RMTRACE_FUNC_PROG_EXIT           0x00FF
#define RMTRACE_FUNC_PROG_STEP           0x007F
#define RMTRACE_UNKNOWN_GPUID            0xFFFFFFFF
#define RMTRACE_UNUSED_PARAM             0
#define RMTRACE_MAX_PRINT_BUFFER_SIZE    128

//
// Empty macros
//

#define RMTRACE_INIT_NEW()
#define RMTRACE_DESTROY_NEW()
#define RMTRACE_SET_PTIMER_LOG(enable)
#define RMTRACE_IS_PTIMER_LOG_ENABLED()                                             \
    NV_FALSE
#define RMTRACE_RMAPI(id, cmd)
#define RMTRACE_RMLOCK(id)
#define RMTRACE_DISP1(id, gpuId, param1)
#define RMTRACE_DISP2(id, gpuId, param1, param2)
#define RMTRACE_DISP3(id, gpuId, param1, param2, param3)
#define RMTRACE_DISP4(id, gpuId, param1, param2, param3, param4)
#define RMTRACE_DISP5(id, gpuId, param1, param2, param3, param4, param5)
#define RMTRACE_DISP6(id, gpuId, param1, param2, param3, param4, param5, param6)
#define RMTRACE_DISP_EDID(gpuId, publicId, connectedId, data, size)
#define RMTRACE_DISP_BRIGHTNESS_ENTRY(dispId, flags, blType, pwmInfoProvider, pwmInfoEntries, SBEnable, lmnProvider, lmnEntryCount, blPwmInfoSize, blPwmInfo)
#define RMTRACE_DISP_ERROR(id, gpuId, param1, param2, status)
#define RMTRACE_DISP_EXCEPTION(gpuId, param1, param2, param3, param4, param5)
#define RMTRACE_GPIO(id, _function, _state, _gpioPin, param)
#define RMTRACE_GPIO_LIST(id, count, list)
#define RMTRACE_I2C(id, gpuId, portId, address, indexSize, pIndex, dataSize, pData, status)
#define RMTRACE_I2C_SET_ACQUIRED(gpuId, portId, acquirer, status, curTime)
#define RMTRACE_I2C_ENUM_PORTS(gpuId, count, ports)
#define RMTRACE_GPU(id, gpuId, param1, param2, param3, param4, param5, param6, param7)
#define RMTRACE_RMJOURNAL(id, gpuId, type, group, key, count, firstTime, lastTime)
#define RMTRACE_POWER(id, gpuId, state, head, forcePerf, fastBootPowerState)
#define RMTRACE_PERF(id, gpuId, param1, param2, param3, param4, param5, param6, param7)
#define RMTRACE_THERM2(id, gpuId, param1, param2)
#define RMTRACE_THERM3(id, gpuId, param1, param2, param3)
#define RMTRACE_THERM6(id, gpuId, param1, param2, param3, param4, param5, param6)
#define RMTRACE_TIMEOUT(id, gpuId)
#define RMTRACE_VBIOS(id, gpuId, param1, param2, param3, param4, param5, param6, param7)
#define RMTRACE_VBIOS_ERROR(id, gpuId, param1, param2, param3, param4, param5, param6, param7)
#define RMTRACE_NVLOG(id, pData, dataSize)
#define RMTRACE_SBIOS(id, gpuId, param1, param2, param3, param4, param5, param6, param7)
#define RMTRACE_USBC0(id, gpuId)
#define RMTRACE_USBC1(id, gpuId, param1)
#define RMTRACE_USBC2(id, gpuId, param1, param2)
#define RMTRACE_USBC7(id, gpuId, param1, param2, param3, param4, param5, param6, param7)
#define RMTRACE_RMGENERAL(id, param1, param2, param3)
#define RMTRACE_NVTELEMETRY(id, gpuId, param1, param2, param3)
#define RMTRACE_NOCAT(id, gpuId, type, group, key, count, timeStamp)
#define RMTRACE_PRINT


#ifndef RMTRACE_FLAG_ENABLED
#define RMTRACE_FLAG_ENABLED        (0)
#endif

//
// Empty macros
//
#define RMTRACE_INIT()
#define RMTRACE_DESTROY()
#define RMTRACE_ENABLE(eventEventMask)
#define RMTRACE_PROBE(module, event)

#define RMTRACE_PROBE1(module, event, dataType, data, dataSize)

#define RMTRACE_PROBE2(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2)

#define RMTRACE_PROBE3(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2,     \
                       dataType3, data3, dataSize3)

#define RMTRACE_PROBE4(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2,     \
                       dataType3, data3, dataSize3, dataType4, data4, dataSize4)

#define RMTRACE_PROBE5(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2,     \
                       dataType3, data3, dataSize3, dataType4, data4, dataSize4,                    \
                       dataType5, data5, dataSize5)

#define RMTRACE_PROBE6(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2,     \
                       dataType3, data3, dataSize3, dataType4, data4, dataSize4,                    \
                       dataType5, data5, dataSize5, dataType6, data6, dataSize6)

#define RMTRACE_PROBE7(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2,     \
                       dataType3, data3, dataSize3, dataType4, data4, dataSize4,                    \
                       dataType5, data5, dataSize5, dataType6, data6, dataSize6,                    \
                       dataType7, data7, dataSize7)
#define RMTRACE_PROBE10(module, event, dataType1, data1, dataSize1, dataType2, data2, dataSize2,    \
                       dataType3, data3, dataSize3, dataType4, data4, dataSize4,                    \
                       dataType5, data5, dataSize5, dataType6, data6, dataSize6,                    \
                       dataType7, data7, dataSize7, dataType8, data8, dataSize8,                    \
                       dataType9, data9, dataSize9, dataType10, data10, dataSize10)
#define RMTRACE_PROBE2_PRIMTYPE(module, event, type0, val0, type1, val1)
#define RMTRACE_PROBE3_PRIMTYPE(module, event, type0, val0, type1, val1, type2, val2)
#define RMTRACE_PROBE4_PRIMTYPE(module, event, type0, val0, type1, val1, type2, val2, type3, val3)
#define RMTRACE_PROBE5_PRIMTYPE(module, event, type0, val0, type1, val1, type2, val2, type3, val3,                  \
                                type4, val4)
#define RMTRACE_PROBE7_PRIMTYPE(module, event, type0, val0, type1, val1, type2, val2, type3, val3,                  \
                                type4, val4, type5, val5, type6, val6)
#define RMTRACE_PROBE10_PRIMTYPE(module, event, type0, val0, type1, val1, type2, val2, type3, val3,                 \
                                 type4, val4, type5, val5, type6, val6, type7, val7, type8, val8,                   \
                                 type9, val9)
#define RMTRACE_MARKER_PROBE(name, gpuId, payload, id)


#endif /* TRACER_H */
