/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvport/nvport.h"
#include "utils/nvassert.h"

#if defined(NVRM) && !defined(NVWATCH) && !defined(GSP_PLUGIN_BUILD)
#include "containers/map.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "rmconfig.h"
#else
#if !defined(RMCFG_MODULE_ENABLED)
#define RMCFG_MODULE_x 0
#endif

#if !defined(RMCFG_IS_PLATFORM)
#define RMCFG_FEATURE_PLATFORM_x 0
#endif
#endif

#if RMCFG_MODULE_RCDB && NV_JOURNAL_ASSERT_ENABLE
// Hook NV_ASSERT into RCDB.
void rcdbRmAssert(NvU32 lineNum, NvU64 ip);
void rcdbRmAssertStatus(NvU32 status, NvU32 lineNum, NvU64 ip);
#define NV_JOURNAL_ASSERT_FAILURE(lineNum, ip)                  rcdbRmAssert(lineNum, (NvU64)(ip))
#define NV_JOURNAL_ASSERT_FAILURE_STATUS(lineNum, ip, status)   rcdbRmAssertStatus((status), lineNum, (NvU64)(ip))
#else
#define NV_JOURNAL_ASSERT_FAILURE(lineNum, ip)                  ((void)0)
#define NV_JOURNAL_ASSERT_FAILURE_STATUS(lineNum, ip, status)   ((void)0)
#endif

#if NV_ASSERT_FAILED_BACKTRACE_ENABLE
static void nvAssertFailedBacktrace(NvU64 ip);

// Print call stack in dmesg when assert fails
#define NV_ASSERT_FAILED_BACKTRACE(ip)                          do {nvAssertFailedBacktrace(ip);} while(0)
#else
#define NV_ASSERT_FAILED_BACKTRACE(ip)                          ((void)0)
#endif

#if NV_ASSERT_FAILED_USES_STRINGS
#define NV_ASSERT_FAILED_PRINTF_FMT                  "%s @ %s:%d\n"
#define NV_ASSERT_FAILED_PRINTF_PARAM                pszExpr, trimFN(pszFileName), lineNum
#else
#define NV_ASSERT_FAILED_PRINTF_FMT                  "0x%016llx\n"
#define NV_ASSERT_FAILED_PRINTF_PARAM                ip
#endif

#if !RMCFG_FEATURE_PLATFORM_GSP && !defined(GSP_PLUGIN_BUILD)
#define NV_ASSERT_PRINTF(level, fmt, ...)            NV_PRINTF_STRING          \
    (NV_PRINTF_MODULE, level, NV_PRINTF_ADD_PREFIX(fmt), ##__VA_ARGS__)
#else
#define NV_ASSERT_PRINTF(level, fmt, ...)
#define NV_ASSERT_LOG(level, fmt, ...)
#endif

#define PATH_SEP   '/'

/*
 * Trim path from source filename.
 */
#if NV_ASSERT_FAILED_USES_STRINGS
static const char *trimFN(const char *pszFileName)
{
    NvLength i;

    for (i = 0; pszFileName[i] != 0; i++)
        ;

    for (; i > 0; i--)
    {
        if (pszFileName[i] == PATH_SEP)
            return &pszFileName[i + 1];
    }

    return pszFileName;
}
#endif

#if NV_PRINTF_ENABLED || NV_JOURNAL_ASSERT_ENABLE
/*
 * Helper function for NV_ASSERT_FAILED
 */
void
nvAssertFailed
(
    NV_ASSERT_FAILED_FUNC_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(LEVEL_ERROR, "Assertion failed: " NV_ASSERT_FAILED_PRINTF_FMT,
        NV_ASSERT_FAILED_PRINTF_PARAM);
    NV_ASSERT_LOG(LEVEL_ERROR, "Assertion failed @ 0x%016x", ip);
    NV_JOURNAL_ASSERT_FAILURE(lineNum, ip);
    NV_ASSERT_FAILED_BACKTRACE(ip);
}

/*
 * Helper functions for NV_ASSERT_OK_FAILED
 */
void
nvAssertOkFailed
(
    NvU32       status
    NV_ASSERT_FAILED_FUNC_COMMA_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(LEVEL_ERROR,
        "Assertion failed: %s (0x%08X) returned from " NV_ASSERT_FAILED_PRINTF_FMT,
        nvstatusToString(status), status, NV_ASSERT_FAILED_PRINTF_PARAM);
    NV_ASSERT_LOG(LEVEL_ERROR, "Assertion failed: 0x%08X returned from 0x%016llx",
        status, ip);
    NV_JOURNAL_ASSERT_FAILURE_STATUS(lineNum, ip, status);
    NV_ASSERT_FAILED_BACKTRACE(ip);
}

/*
 * Helper function for NV_CHECK_FAILED
 */
void
nvCheckFailed
(
    NvU32       level
    NV_ASSERT_FAILED_FUNC_COMMA_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(level, "Check failed: " NV_ASSERT_FAILED_PRINTF_FMT,
        NV_ASSERT_FAILED_PRINTF_PARAM);
    NV_ASSERT_LOG(level, "Check failed @ 0x%016llx", ip);
}

/*
 * Helper function for NV_CHECK_OK_FAILED
 */
void
nvCheckOkFailed
(
    NvU32       level,
    NvU32       status
    NV_ASSERT_FAILED_FUNC_COMMA_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(level,
        "Check failed: %s (0x%08X) returned from " NV_ASSERT_FAILED_PRINTF_FMT,
        nvstatusToString(status), status, NV_ASSERT_FAILED_PRINTF_PARAM);
    NV_ASSERT_LOG(level, "Check failed: 0x%08X returned from 0x%016llx", status, ip);
}

/*
 * Helper function for NV_ASSERT_FAILED
 */
void
nvAssertFailedNoLog
(
    NV_ASSERT_FAILED_FUNC_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(LEVEL_ERROR, "Assertion failed: " NV_ASSERT_FAILED_PRINTF_FMT,
        NV_ASSERT_FAILED_PRINTF_PARAM);
    NV_JOURNAL_ASSERT_FAILURE(lineNum, ip);
    NV_ASSERT_FAILED_BACKTRACE(ip);
}

/*
 * Helper function for NV_ASSERT_OK_FAILED
 */
void
nvAssertOkFailedNoLog
(
    NvU32       status
    NV_ASSERT_FAILED_FUNC_COMMA_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(LEVEL_ERROR,
        "Assertion failed: %s (0x%08X) returned from " NV_ASSERT_FAILED_PRINTF_FMT,
        nvstatusToString(status), status, NV_ASSERT_FAILED_PRINTF_PARAM);
    NV_JOURNAL_ASSERT_FAILURE_STATUS(lineNum, ip, status);
    NV_ASSERT_FAILED_BACKTRACE(ip);
}

/*
 * Helper function for NV_CHECK_FAILED
 */
void
nvCheckFailedNoLog
(
    NvU32       level
    NV_ASSERT_FAILED_FUNC_COMMA_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(level, "Check failed: " NV_ASSERT_FAILED_PRINTF_FMT,
        NV_ASSERT_FAILED_PRINTF_PARAM);
}

/*
 * Helper function for NV_CHECK_OK_FAILED
 */
void
nvCheckOkFailedNoLog
(
    NvU32       level,
    NvU32       status
    NV_ASSERT_FAILED_FUNC_COMMA_TYPE
)
{
    NvU64 ip = portUtilGetReturnAddress();
    PORT_UNREFERENCED_VARIABLE(ip);

    NV_ASSERT_PRINTF(level,
        "Check failed: %s (0x%08X) returned from " NV_ASSERT_FAILED_PRINTF_FMT,
        nvstatusToString(status), status, NV_ASSERT_FAILED_PRINTF_PARAM);
}

#endif

#if NV_ASSERT_FAILED_BACKTRACE_ENABLE
MAKE_MAP(AssertedIPMap, NvU8);

static struct
{
    AssertedIPMap map;
    NvU32 mode;
    PORT_MUTEX *mtx;
    NvBool init;
    OS_THREAD_HANDLE tid;
} osAssertInternal;

void nvAssertInit(void)
{
    if (osAssertInternal.init)
        return;

    osAssertInternal.mode = NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_UNIQUE;

    // Map is not thread-safe and osAssertFailed can be called concurrently.
    osReadRegistryDword(NULL, NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE, &osAssertInternal.mode);
    if (osAssertInternal.mode == NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_UNIQUE)
    {
        osAssertInternal.mtx = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
        if (!osAssertInternal.mtx)
        {
            osAssertInternal.mode = NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_DISABLE;
        }
        else
        {
            mapInit(&osAssertInternal.map, portMemAllocatorGetGlobalNonPaged());
        }
    }
    osAssertInternal.init = NV_TRUE;
}

static void nvAssertFailedBacktrace(NvU64 ip)
{
    if (!osAssertInternal.init)
        return;

    if (osAssertInternal.mode == NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_UNIQUE)
    {
        OS_THREAD_HANDLE tid;
        if (osGetCurrentThread(&tid) != NV_OK)
            return;

        // nvport mutex is not reentrant and will deadlock with nested locking.
        // If the next condition holds, we're in a reentrant call.
        if (tid == osAssertInternal.tid)
            return;

        portSyncMutexAcquire(osAssertInternal.mtx);
        osAssertInternal.tid = tid;

        if (!mapFind(&osAssertInternal.map, ip))
        {
            // If we're out of memory, do not dump anything to avoid spam
            if (mapInsertNew(&osAssertInternal.map, ip))
                osAssertFailed();
        }

        osAssertInternal.tid = 0;
        portSyncMutexRelease(osAssertInternal.mtx);
    }
    else if (osAssertInternal.mode == NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_ENABLE)
        osAssertFailed();
}

void nvAssertDestroy(void)
{
    if (!osAssertInternal.init)
        return;

    if (osAssertInternal.mode == NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_UNIQUE && osAssertInternal.mtx)
    {
        portSyncMutexDestroy(osAssertInternal.mtx);
        mapDestroy(&osAssertInternal.map);
    }
    osAssertInternal.init = 0;
}

#else

// We do not expose NV_ASSERT_FAILED_BACKTRACE outside this file. The callers will use these stubs.
void nvAssertInit(void)
{
}

void nvAssertDestroy(void)
{
}
#endif /* defined(NV_ASSERT_FAILED_BACKTRACE) */
