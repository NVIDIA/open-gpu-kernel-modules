/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _PRINTF_H_
#define _PRINTF_H_

/*
 * RM PRINTF definitions.
 *
 * Provides RM internal definitions built on the generic nvprintf utilities
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "nvport/nvport.h"
#include "utils/nvprintf.h"
#include "nvlog/nvlog.h"

// TODO Bug 5078337: Move these away from kernel/core
#include "kernel/diagnostics/xid_context.h"

#define DBG_FILE_LINE_FUNCTION  NV_FILE_STR, __LINE__, NV_FUNCTION_STR

/**
 * @todo bug 1583359 - Move to NvPort compiler specifics
 */
#if defined(__GNUC__) || defined(__clang__)
#define NV_RETURN_ADDRESS()    __builtin_return_address(0)
#else
#define NV_RETURN_ADDRESS()    _ReturnAddress()
#endif


//******************************************************************************
//                                  BREAKPOINTS
//******************************************************************************

// NV_DBG_BREAKPOINT_ALLOWED can be overridden through CFLAGS
#if !defined(NV_DBG_BREAKPOINT_ALLOWED)
#if defined(DEBUG) || defined(ASSERT_BUILD) || defined(NV_MODS) || defined(QA_BUILD) || (defined(NVRM) && NVOS_IS_LIBOS)
#define NV_DBG_BREAKPOINT_ALLOWED 1
#else
#define NV_DBG_BREAKPOINT_ALLOWED 0
#endif
#endif // !defined(NV_DBG_BREAKPOINT_ALLOWED)

#define NV_DEBUG_BREAK_FLAGS_RC                        0:0
#define NV_DEBUG_BREAK_FLAGS_RC_DISABLE                (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_RC_ENABLE                 (0x00000001)
#define NV_DEBUG_BREAK_FLAGS_ASSERT                    1:1
#define NV_DEBUG_BREAK_FLAGS_ASSERT_DISABLE            (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_ASSERT_ENABLE             (0x00000001)
#define NV_DEBUG_BREAK_FLAGS_DBG_BREAK                 2:2
#define NV_DEBUG_BREAK_FLAGS_DBG_BREAK_DISABLE         (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_DBG_BREAK_ENABLE          (0x00000001)
#define NV_DEBUG_BREAK_FLAGS_GPU_TIMEOUT               3:3
#define NV_DEBUG_BREAK_FLAGS_GPU_TIMEOUT_DISABLE       (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_GPU_TIMEOUT_ENABLE        (0x00000001)
#define NV_DEBUG_BREAK_FLAGS_POOL_TAGS                 4:4
#define NV_DEBUG_BREAK_FLAGS_POOL_TAGS_DISABLE         (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_POOL_TAGS_ENABLE          (0x00000001)
#define NV_DEBUG_BREAK_FLAGS_POWER_ON                  5:5
#define NV_DEBUG_BREAK_FLAGS_POWER_ON_DISABLE          (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_POWER_ON_ENABLE           (0x00000001)
#define NV_DEBUG_BREAK_FLAGS_SMU_ERROR                 6:6
#define NV_DEBUG_BREAK_FLAGS_SMU_ERROR_DISABLE         (0x0)
#define NV_DEBUG_BREAK_FLAGS_SMU_ERROR_ENABLE          (0x1)
#define NV_DEBUG_BREAK_FLAGS_CRASH                     7:7
#define NV_DEBUG_BREAK_FLAGS_CRASH_DISABLE             (0x00000000)
#define NV_DEBUG_BREAK_FLAGS_CRASH_ENABLE              (0x00000001)

#define NV_DEBUG_BREAK_ATTRIBUTES                      7:0
#define NV_DEBUG_BREAK_ATTRIBUTES_NONE                 (0x00000000)
#define NV_DEBUG_BREAK_ATTRIBUTES_RC                   (0x00000001)
#define NV_DEBUG_BREAK_ATTRIBUTES_ASSERT               (0x00000002)
#define NV_DEBUG_BREAK_ATTRIBUTES_DBG_BREAK            (0x00000004)
#define NV_DEBUG_BREAK_ATTRIBUTES_GPU_TIMEOUT          (0x00000008)
#define NV_DEBUG_BREAK_ATTRIBUTES_POOL_TAGS            (0x00000010)
#define NV_DEBUG_BREAK_ATTRIBUTES_POWER_ON             (0x00000020)
#define NV_DEBUG_BREAK_ATTRIBUTES_SMU_ERROR            (0x00000040)
#define NV_DEBUG_BREAK_ATTRIBUTES_CRASH                (0x00000080)

// Checks RMINFO and OS config to see if triggering a breakpoint is ever allowed
NvBool nvDbgBreakpointEnabled(void);
// Flushes the logs before a breakpoint, so we can see all the prints.
void osFlushLog(void);

#define DBG_ROUTINE()                                                          \
    do                                                                         \
    {                                                                          \
        if (nvDbgBreakpointEnabled())                                          \
            PORT_BREAKPOINT_ALWAYS();                                          \
    } while (0)

#define REL_DBG_BREAKPOINT() \
    REL_DBG_BREAKPOINT_MSG("NVRM-RC: Nvidia GPU Error Detected\n")

#if NV_DBG_BREAKPOINT_ALLOWED

#if !NVOS_IS_LIBOS

#define DBG_BREAKPOINT_EX(PGPU, LEVEL)                                          \
    do                                                                          \
    {                                                                           \
        NV_PRINTF(LEVEL_ERROR, "bp @ " NV_FILE_FMT ":%d\n", NV_FILE, __LINE__); \
        osFlushLog();                                                           \
        DBG_ROUTINE();                                                          \
    } while (0)

#else // !NVOS_IS_LIBOS

#define DBG_BREAKPOINT_EX(PGPU, LEVEL)                                         \
    do                                                                         \
    {                                                                          \
        NV_ASSERT_FAILED("DBG_BREAKPOINT");                                    \
    } while (0)

#endif // !NVOS_IS_LIBOS

#define DBG_BREAKPOINT() DBG_BREAKPOINT_EX(NULL, 0)

#define REL_DBG_BREAKPOINT_MSG(msg)                                            \
    do                                                                         \
    {                                                                          \
        PORT_DBG_PRINT_STRING_LITERAL(msg);                                    \
        DBG_BREAKPOINT();                                                      \
    } while (0)

#else // !NV_DBG_BREAKPOINT_ALLOWED

#define DBG_BREAKPOINT()
#define DBG_BREAKPOINT_EX(PGPU, LEVEL)

#define REL_DBG_BREAKPOINT_MSG(msg)                                            \
    do                                                                         \
    {                                                                          \
        PORT_DBG_PRINT_STRING_LITERAL(msg);                                    \
        DBG_ROUTINE();                                                         \
    } while (0)


#endif // NV_DBG_BREAKPOINT_ALLOWED

#define DBG_BREAKPOINT_REASON(reason) DBG_BREAKPOINT()

#define DBG_BREAKPOINT_ERROR_INFO(errorCategory, errorInfo) DBG_BREAKPOINT()

//******************************************************************************
//                                  PRINTS
//******************************************************************************

#include "utils/nvprintf.h"

#define MAX_ERROR_STRING 512
#ifndef NVPORT_CHECK_PRINTF_ARGUMENTS
#define NVPORT_CHECK_PRINTF_ARGUMENTS(x,c)
#endif
//
// Prototypes
//
NvBool nvDbgInit(void);
void nvDbgDestroy(void);
void nvDbg_Printf  (const char *file, int line, const char *function, int debuglevel, const char *s, ...) NVPORT_CHECK_PRINTF_ARGUMENTS(5, 6);

//
// Like libc's vsnprintf(), nvDbg_vPrintf() invalidates its va_list argument. The va_list argument
// may not be reused after nvDbg_vPrintf() returns.  If the va_list is needed after the
// nvDbg_vPrintf() call, create a copy of the va_list using va_copy().
// The caller controls the lifetime of the va_list argument, and should free it using va_end.
//
void nvDbg_vPrintf (const char *file, int line, const char *function, int debuglevel, const char *s, va_list args);
void nvDbg_PrintBuf(const char *file, int line, const char *function, int debgulevel, NvU8 buffer[], NvU32 bufsize);

int nvDbgVsnprintf(char *dest, NvU32 destSize, const char *fmt, va_list args);
int nvDbgSnprintf (char *dest, NvU32 destSize, const char *fmt, ...);

struct OBJGPU;
void nvDbgInitRmMsg(struct OBJGPU *);
// RmMsgPrefix return value
#define NVRM_MSG_PREFIX_NVRM            NVBIT(0)
#define NVRM_MSG_PREFIX_FILE            NVBIT(1)
#define NVRM_MSG_PREFIX_FUNCTION        NVBIT(2)
#define NVRM_MSG_PREFIX_LINE            NVBIT(3)
#define NVRM_MSG_PREFIX_OSTIMESTAMP     NVBIT(4)
NvU32 RmMsgPrefix(NvU32 prefix, const char *filename, NvU32 linenumber, const char *function, char *str, NvU32 len);
// nvDbgRmMsgCheck return code
#define NVRM_MSG_NORMAL 0    // Use normal message handling (warnings/errors)
#define NVRM_MSG_HIDE   1    // Skip this message
#define NVRM_MSG_PRINT  2    // Force printing of this message
NvU32 nvDbgRmMsgCheck(const char *filename, NvU32 linenumber, const char *function, NvU32 level, const char *format, NvU32 *pPrefix);
void nvDbgDumpBufferBytes(void *pBuffer, NvU32 length);


#if NV_PRINTF_STRINGS_ALLOWED
#define DBG_STRING(str) str
#define DBG_INIT()                               nvDbgInit()
#define DBG_DESTROY()                            nvDbgDestroy()
#define DBG_VSNPRINTF(ptr_size_format_and_stuff) nvDbgVsnprintf ptr_size_format_and_stuff
#define DBG_PRINTBUF(dbglevel, buffer, bufsize)  nvDbg_PrintBuf(DBG_FILE_LINE_FUNCTION, dbglevel, buffer, bufsize)
#define DBG_RMMSG_CHECK(level)            (nvDbgRmMsgCheck(DBG_FILE_LINE_FUNCTION, level, NULL, NULL) == NVRM_MSG_PRINT)
#else  // ! NV_PRINTF_STRINGS_ALLOWED -- debug printf strings not enabled
#define DBG_STRING(str) ""
#define DBG_INIT()      (NV_TRUE)
#define DBG_DESTROY()
#define DBG_VSNPRINTF(ptr_size_format_and_stuff)
#define DBG_PRINTBUF(dbglevel, buffer, bufsize)
#define DBG_RMMSG_CHECK(level)   (0)
#endif // NV_PRINTF_STRINGS_ALLOWED

#if defined(PORT_IS_FUNC_SUPPORTED)
#if PORT_IS_FUNC_SUPPORTED(portMemExValidate)
#define DBG_VAL_PTR(p) portMemExValidate(p, NV_TRUE)
#endif
#endif
#ifndef DBG_VAL_PTR
#define DBG_VAL_PTR(p)
#endif

//
// TODO Bug 5078337: Move these away from kernel/core and rename to indicate
// that they emit XIDs
//
#define NV_ERROR_LOG(pGpu, num, fmt, ...)                               \
    nvErrorLog_va((void*)pGpu, num, fmt, ##__VA_ARGS__);                \
    NVLOG_PRINTF(NV_PRINTF_MODULE, NVLOG_ROUTE_RM, LEVEL_ERROR,         \
                 NV_PRINTF_ADD_PREFIX("Xid %d: " fmt), num, ##__VA_ARGS__)

#define NV_ERROR_LOG_DATA(pGpu, num, fmt, ...)                          \
    portDbgPrintf(NV_PRINTF_ADD_PREFIX(fmt), ##__VA_ARGS__);            \
    NVLOG_PRINTF(NV_PRINTF_MODULE, NVLOG_ROUTE_RM, LEVEL_ERROR,         \
                 NV_PRINTF_ADD_PREFIX(fmt), ##__VA_ARGS__)

void nvErrorLog(void *pVoid, XidContext context, const char *pFormat, va_list arglist);
void nvErrorLog_va(void * pGpu, NvU32 num, const char * pFormat, ...);
void nvErrorLog2_va(void * pGpu, XidContext context, NvBool oobLogging, const char * pFormat, ...);

#ifdef __cplusplus
}
#endif

#endif // _PRINTF_H_
