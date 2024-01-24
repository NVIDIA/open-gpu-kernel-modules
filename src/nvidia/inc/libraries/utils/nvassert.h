/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /**
  * @file
  * @brief Utility assertion macros
  *
  * @see "NV_ASSERT" confluence page for additional documentation
  */

#ifndef _NV_UTILS_ASSERT_H_
#define _NV_UTILS_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup NV_UTILS_ASSERT Utility Assertion Macros
 *
 * @brief Provides a light abstraction layer for common assert macro patterns.
 *
 * NvPort and NvPrintf are used for debug and logging primitives.
 * If an environment cannot use these directly then it can override
 * the NV_PORT_HEADER and NV_PRINTF_HEADER defines in its makefile
 * to point to appropriate replacements.
 * @{
 */
#include "nvstatus.h"
#include "nvmacro.h"

// Include portability header, falling back to NvPort if not provided.
#ifndef NV_PORT_HEADER
#define NV_PORT_HEADER "nvport/nvport.h"
#endif
#include NV_PORT_HEADER

// Include printf header, falling back to NvPrintf if not provided.
#ifndef NV_PRINTF_HEADER
#define NV_PRINTF_HEADER "utils/nvprintf.h"
#endif
#include NV_PRINTF_HEADER

/*
 * Use __builtin_expect to improve branch predictions on the GNU compiler.
 *
 * Note that these macros convert the parameter to bool.  They should
 * only be used in 'if' statements.
 *
 * '!= 0' is used (instead of a cast to NvBool or !!) to avoid 'will always
 * evaluate as 'true'' warnings in some gcc versions.
 */
#if defined(__GNUC__) && __GNUC__ >= 3
#define NV_LIKELY(expr)   __builtin_expect(((expr) != 0), 1)
#define NV_UNLIKELY(expr) __builtin_expect(((expr) != 0), 0)
#else
#define NV_LIKELY(expr)   ((expr) != 0)
#define NV_UNLIKELY(expr) ((expr) != 0)
#endif

/*
 * Set this to pass expression, function name, file name, and line number
 * to the nvAssertFailed functions.
 *
 * NOTE: NV_PRINTF_STRINGS_ALLOWED defaults to:
 *           defined(DEBUG) || defined(NV_MODS) || defined(QA_BUILD)
 *
 *       RM_ASSERT used this condition to decide whether to print assert strings:
 *           defined(DEBUG) || defined(ASSERT_BUILD) || defined(QA_BUILD)
 */
#if !defined(NV_ASSERT_FAILED_USES_STRINGS)
#if (NV_PRINTF_STRINGS_ALLOWED && (defined(DEBUG) || defined(ASSERT_BUILD) || defined(QA_BUILD)))
#define NV_ASSERT_FAILED_USES_STRINGS  1
#else
#define NV_ASSERT_FAILED_USES_STRINGS  0
#endif
#endif

// Hook NV_ASSERT into RCDB.
#if !defined(NV_JOURNAL_ASSERT_ENABLE)
#if defined(NVRM) && (NVOS_IS_WINDOWS || NVOS_IS_UNIX || NVOS_IS_LIBOS) && !defined(NVWATCH) && !defined(NV_MODS)
#define NV_JOURNAL_ASSERT_ENABLE            1
#else
#define NV_JOURNAL_ASSERT_ENABLE            0
#endif
#endif

#if !defined(COVERITY_ASSERT_FAIL)
#if defined(__COVERITY__)
void __coverity_panic__(void);
#define COVERITY_ASSERT_FAIL() __coverity_panic__()
#else  // defined(__COVERITY__)
#define COVERITY_ASSERT_FAIL() ((void) 0)
#endif // defined(__COVERITY__)
#endif // !defined(COVERITY_ASSERT_FAIL)

/*
 * NV_ASSERT_FAILED, NV_ASSERT_OK_FAILED, NV_CHECK_FAILED, and NV_CHECK_OK_FAILED
 * These macros are defined in three flavors:
 *
 * normal - expr/file/line are concatenated with format string for NVLOG.
 *          expr/file/line are passed in as parameters to a helper function
 *          for NV_PRINTF.
 *
 * normal for GSP-RM - expr/file/line are omitted, since each NV_PRINTF line
 *           already has them.  NVLOG is not used.
 *
 * _FUNC -  expr/file/line are passed in as parameters to a helper function
 *          for both NVLOG and NV_PRINTF.
 *          The _FUNC macros are used for pre-compiled headers on most platforms.
 */
#if defined(GSP_PLUGIN_BUILD) || (defined(NVRM) && NVOS_IS_LIBOS)

void nvAssertInit(void);
void nvAssertDestroy(void);

#if NV_JOURNAL_ASSERT_ENABLE
void nvAssertFailed(void);
void nvAssertOkFailed(NvU32 status);
#else
#define nvAssertFailed(...)
#define nvAssertOkFailed(...)
#endif

#define NV_ASSERT_FAILED(exprStr)                                              \
    do {                                                                       \
        NV_LOG_SPECIAL(LEVEL_ERROR, RM_GSP_LOG_SPECIAL_ASSERT_FAILED,          \
                       exprStr "\n");                                          \
        nvAssertFailed();                                                      \
        COVERITY_ASSERT_FAIL();                                                \
        PORT_BREAKPOINT();                                                     \
    } while(0)

#define NV_ASSERT_OK_FAILED(exprStr, status)                                   \
    do {                                                                       \
        NV_LOG_SPECIAL(LEVEL_ERROR, RM_GSP_LOG_SPECIAL_ASSERT_OK_FAILED,       \
                       exprStr "\n", status);                                  \
        nvAssertOkFailed(status);                                              \
        COVERITY_ASSERT_FAIL();                                                \
        PORT_BREAKPOINT();                                                     \
    } while(0)

#define NV_CHECK_FAILED(level, exprStr)                                        \
   do {                                                                        \
        NV_LOG_SPECIAL(level, RM_GSP_LOG_SPECIAL_CHECK_FAILED,                 \
                       exprStr "\n");                                          \
    } while(0)                                                                 \

#define NV_CHECK_OK_FAILED(level, exprStr, status)                             \
    do {                                                                       \
        NV_LOG_SPECIAL(level, RM_GSP_LOG_SPECIAL_CHECK_OK_FAILED,              \
                       exprStr "\n", status);                                  \
    } while (0)

#else // defined(GSP_PLUGIN_BUILD) || (defined(NVRM) && NVOS_IS_LIBOS)

#if NV_ASSERT_FAILED_USES_STRINGS
#define NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr)   , exprStr, __FILE__, __LINE__
#define NV_ASSERT_FAILED_FUNC_PARAM(exprStr)         exprStr, __FILE__, __LINE__
#define NV_ASSERT_FAILED_FUNC_COMMA_TYPE             ,const char *pszExpr, const char *pszFileName, NvU32 lineNum
#define NV_ASSERT_FAILED_FUNC_TYPE                   const char *pszExpr, const char *pszFileName, NvU32 lineNum
#else
#define NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr)   , __LINE__
#define NV_ASSERT_FAILED_FUNC_PARAM(exprStr)         __LINE__
#define NV_ASSERT_FAILED_FUNC_COMMA_TYPE             , NvU32 lineNum
#define NV_ASSERT_FAILED_FUNC_TYPE                   NvU32 lineNum
#endif

void nvAssertInit(void);
void nvAssertDestroy(void);

// Helper function prototypes for _FAILED macros below.
#if NV_PRINTF_ENABLED || NV_JOURNAL_ASSERT_ENABLE
void nvAssertFailed(NV_ASSERT_FAILED_FUNC_TYPE);
void nvAssertOkFailed(NvU32 status NV_ASSERT_FAILED_FUNC_COMMA_TYPE);
void nvCheckFailed(NvU32 level NV_ASSERT_FAILED_FUNC_COMMA_TYPE);
void nvCheckOkFailed(NvU32 level, NvU32 status NV_ASSERT_FAILED_FUNC_COMMA_TYPE);
void nvAssertFailedNoLog(NV_ASSERT_FAILED_FUNC_TYPE);
void nvAssertOkFailedNoLog(NvU32 status NV_ASSERT_FAILED_FUNC_COMMA_TYPE);
void nvCheckFailedNoLog(NvU32 level NV_ASSERT_FAILED_FUNC_COMMA_TYPE);
void nvCheckOkFailedNoLog(NvU32 level, NvU32 status NV_ASSERT_FAILED_FUNC_COMMA_TYPE);
#else // NV_PRINTF_ENABLED || NV_JOURNAL_ASSERT_ENABLE
#define nvAssertFailed(...)
#define nvAssertOkFailed(...)
#define nvCheckFailed(...)
#define nvCheckOkFailed(...)
#define nvAssertFailedNoLog(...)
#define nvAssertOkFailedNoLog(...)
#define nvCheckFailedNoLog(...)
#define nvCheckOkFailedNoLog(...)
#endif // NV_PRINTF_ENABLED || NV_JOURNAL_ASSERT_ENABLE

#define NV_ASSERT_LOG(level, fmt, ...)                                         \
    NVLOG_PRINTF(NV_PRINTF_MODULE, NVLOG_ROUTE_RM, level,                      \
    NV_PRINTF_ADD_PREFIX(fmt), ##__VA_ARGS__)

#define NV_ASSERT_FAILED(exprStr)                                              \
    do {                                                                       \
        NV_ASSERT_LOG(LEVEL_ERROR, "Assertion failed: " exprStr);              \
        nvAssertFailedNoLog(NV_ASSERT_FAILED_FUNC_PARAM(exprStr));             \
        COVERITY_ASSERT_FAIL();                                                \
        PORT_BREAKPOINT_CHECKED();                                             \
    } while(0)

#define NV_ASSERT_OK_FAILED(exprStr, status)                                   \
    do {                                                                       \
        NV_ASSERT_LOG(LEVEL_ERROR, "Assertion failed: 0x%08X returned from "   \
            exprStr, status);                                                  \
        nvAssertOkFailedNoLog(status                                           \
            NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr));                       \
        COVERITY_ASSERT_FAIL();                                                \
        PORT_BREAKPOINT_CHECKED();                                             \
    } while(0)

#define NV_CHECK_FAILED(level, exprStr)                                        \
    do {                                                                       \
        NV_ASSERT_LOG(level, "Check failed: " exprStr);                        \
        if (NV_PRINTF_LEVEL_ENABLED(level))                                    \
        {                                                                      \
            nvCheckFailedNoLog(level                                           \
                NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr));                   \
        }                                                                      \
    } while(0)

#define NV_CHECK_OK_FAILED(level, exprStr, status)                             \
    do {                                                                       \
        NV_ASSERT_LOG(level, "Check failed: 0x%08X returned from "             \
            exprStr, status);                                                  \
        if (NV_PRINTF_LEVEL_ENABLED(level))                                    \
        {                                                                      \
            nvCheckOkFailedNoLog(level, status                                 \
                 NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr));                  \
        }                                                                      \
    } while(0)

#define NV_ASSERT_FAILED_FUNC(exprStr)                                         \
    do {                                                                       \
        nvAssertFailed(NV_ASSERT_FAILED_FUNC_PARAM(exprStr));                  \
        COVERITY_ASSERT_FAIL();                                                \
        PORT_BREAKPOINT_CHECKED();                                             \
    } while(0)

#define NV_ASSERT_OK_FAILED_FUNC(exprStr, status)                              \
    do {                                                                       \
        nvAssertOkFail(status NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr));     \
        COVERITY_ASSERT_FAIL();                                                \
        PORT_BREAKPOINT_CHECKED();                                             \
    } while(0)

#define NV_CHECK_FAILED_FUNC(level, exprStr)                                   \
    if (NV_PRINTF_LEVEL_ENABLED(level))                                        \
    {                                                                          \
        nvCheckFailed(level NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr))        \
    }

#define NV_CHECK_OK_FAILED_FUNC(level, exprStr, status)                        \
    if (NV_PRINTF_LEVEL_ENABLED(level))                                        \
    {                                                                          \
        nvCheckOkFailed(level, status                                          \
            NV_ASSERT_FAILED_FUNC_COMMA_PARAM(exprStr))                        \
    }

#endif // defined(GSP_PLUGIN_BUILD) || (defined(NVRM) && NVOS_IS_LIBOS)

/*
 * Defines for precompiled headers.
 *
 * On platforms other than GSP-RM, the _INLINE macros cannot be used inside
 * precompiled headers due to conflicting NVLOG_PRINT_IDs.
 */
#if defined(GSP_PLUGIN_BUILD) || (defined(NVRM) && NVOS_IS_LIBOS)
#define NV_ASSERT_FAILED_PRECOMP    NV_ASSERT_FAILED
#else
#define NV_ASSERT_FAILED_PRECOMP    NV_ASSERT_FAILED_FUNC
#endif

// *****************************************************************************
// *                       NV_ASSERT family of macros                          *
// *****************************************************************************
/**
 * General notes:
 *
 * Assert that an expression is true.  If not, do the actions defined
 * in NV_ASSERT_FAILED as well as an "other action":
 *   Print an error message in the debug output
 *   Log an error message in NvLog
 *   Mark as an error condition for coverity
 *   Breakpoint
 *   Log an assert record to the journal
 *   "Other action" as defined by each macro below.
 *
 * The actions are enabled or omitted based on platform and build, and the
 * implementations are platform dependent.
 *
 * The expression is always evaluated even if assertion failures are not logged
 * in the environment.  Use @ref NV_ASSERT_CHECKED if the expression should only
 * be evaluated in checked builds.
 *
 * USE GENEROUSLY FOR any condition that requires immediate action from NVIDIA.
 * Expect to be ARBed on bugs when an assert you added shows up internally
 * or in the field.
 *
 * DO NOT USE for normal run-time conditions, such as a user application
 * passing a bad parameter.
 */

/**
 * Assert that an expression is true.
 *
 * @param[in] expr  Expression that evaluates to a truth value.
 */
#define NV_ASSERT(expr)                                                        \
    NV_ASSERT_OR_ELSE_STR(expr, #expr, /* no other action */)

/**
 * Assert that an expression is true only in checked builds.
 *
 * @note The expression is only evaluated in checked builds so should
 *       not contain required side-effects.
 *       Also to prevent side effects, no "other action" is permitted.
 *
 * @param[in] expr  Expression that evaluates to a truth value.
 */
#if PORT_IS_CHECKED_BUILD
#define NV_ASSERT_CHECKED(expr)                                                \
    NV_ASSERT_OR_ELSE_STR(expr, #expr, /* no other action */)
#define NV_ASSERT_CHECKED_PRECOMP(expr)                                        \
    NV_ASSERT_OR_ELSE_STR_PRECOMP(expr, #expr, /* no other action */)
#else
#define NV_ASSERT_CHECKED(expr)  ((void)0)
#define NV_ASSERT_CHECKED_PRECOMP(expr)  ((void)0)
#endif

/**
 * Assert that an expression is true or else do something.
 *
 * This macro can't use NV_ASSERT_OR_ELSE_STR when NV_PRINTF is passed in as
 * the elseStmt parameter.
 *
 * @param[in] expr     Expression that evaluates to a truth value.
 * @param[in] elseStmt Statement to evaluate if the expression is false.
 */
#define NV_ASSERT_OR_ELSE(expr, elseStmt)                                      \
    if (1)                                                                     \
    {                                                                          \
        PORT_COVERAGE_PUSH_OFF();                                              \
        if (NV_UNLIKELY(!(expr)))                                              \
        {                                                                      \
            NV_ASSERT_FAILED(#expr);                                           \
            elseStmt;                                                          \
        }                                                                      \
        PORT_COVERAGE_POP();                                                   \
    } else ((void) 0)

/**
 * Assert that an expression is true or else goto a label.
 *
 * @param[in] expr  Expression that evaluates to a truth value.
 * @param[in] label Label to jump to when the expression is false.
 */
#define NV_ASSERT_OR_GOTO(expr, label)                                         \
    NV_ASSERT_OR_ELSE_STR(expr, #expr, goto label)

/**
 * Assert that an expression is true or else return a value.
 *
 * @param[in] expr   Expression that evaluates to a truth value.
 * @param[in] retval Value to return if the expression is false.
 */
#define NV_ASSERT_OR_RETURN(expr, retval)                                      \
    NV_ASSERT_OR_ELSE_STR(expr, #expr, return (retval))

/**
 * Assert that an expression is true or else return void.
 *
 * @param[in] expr   Expression that evaluates to a truth value.
 */
#define NV_ASSERT_OR_RETURN_VOID(expr)                                         \
    NV_ASSERT_OR_ELSE_STR(expr, #expr, return)

/**
 * Assert that an expression is true or else do something.
 *
 * Although it can be used directly, this second level macro is designed to be
 * called from other macros.  Passing expr through multiple levels of macros
 * before it is stringified expands it.  This is especially bad for DRF macros,
 * which result in an embedded %, breaking the format string in the
 * NV_ASSERT_FAILED_INLINE macro defined above.  The macros in this header
 * always pass the stringified expr as a into the second level macros as
 * a separate parameter.
 *
 * @param[in] expr     Expression that evaluates to a truth value.
 * @param[in] exprStr  Expression as a string.
 * @param[in] elseStmt Statement to evaluate if the expression is false.
 */
#define NV_ASSERT_OR_ELSE_STR(expr, exprStr, elseStmt)                         \
    do                                                                         \
    {                                                                          \
        PORT_COVERAGE_PUSH_OFF();                                              \
        if (NV_UNLIKELY(!(expr)))                                              \
        {                                                                      \
            NV_ASSERT_FAILED(exprStr);                                         \
            elseStmt;                                                          \
        }                                                                      \
        PORT_COVERAGE_POP();                                                   \
    } while (0)

// *****************************************************************************
// *                     NV_ASSERT_OK family of macros                         *
// *****************************************************************************
/**
 * General notes:
 *
 * Assert that an expression evaluates to NV_OK.  If not, do the actions defined
 * in NV_ASSERT_OK_FAILED as well as an "other action":
 *   Print an error message in the debug output, including decoded NV_STATUS.
 *   Log an error message in NvLog.
 *   Mark as an error condition for coverity.
 *   Breakpoint.
 *   Log an assert record to the journal.
 *   "Other action" as defined by each macro below.
 *
 * The actions are enabled or omitted based on platform and build, and the
 * implementations are platform dependent.
 *
 * The expression is always evaluated even if assertion failures are not logged
 * in the environment.  Use @ref NV_ASSERT_OK_CHECKED if the expression should
 * only be evaluated in checked builds.
 *
 * USE GENEROUSLY FOR any condition that requires immediate action from NVIDIA.
 * Expect to be ARBed on bugs when an assert you added shows up internally
 * or in the field.
 *
 * DO NOT USE for normal run-time conditions, such as a user application
 * passing a bad parameter.
 */

/**
 * Assert that an expression evaluates to NV_OK.
 *
 * @param[in] expr Expression that evaluates to an NV_STATUS.
 */
#define NV_ASSERT_OK(expr)                                                     \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_ASSERT_OK_OR_ELSE_STR(rm_pvt_status, expr, #expr,                   \
                                 /* no other action */);                       \
    } while(0)

/**
 * Assert that an expression evaluates to NV_OK only in checked builds.
 *
 * @note The expression is only evaluated in checked builds so should
 *       not contain required side-effects.
 *       Also to prevent side effects, no "other action" is permitted,
 *       and the status parameter is omitted.
 *
 * @param[in] expr Expression that evaluates to an NV_STATUS.
 */
#if PORT_IS_CHECKED_BUILD
#define NV_ASSERT_OK_CHECKED(expr)                                             \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_ASSERT_OK_OR_ELSE_STR(rm_pvt_status, expr, #expr,                   \
            return rm_pvt_status);                                             \
    } while(0)
#else
#define NV_ASSERT_OK_CHECKED(expr)  ((void)0)
#endif

/*!
 * Call a function that returns NV_STATUS and assert that the
 * return values is NV_OK. In case this was a first failure
 * update global status @ref status.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] expr   Expression that evaluates to an NV_STATUS.
 */
#define NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status, expr)                      \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_ASSERT_OK_OR_ELSE_STR(rm_pvt_status, expr, #expr,                   \
            if (status == NV_OK) status = rm_pvt_status);                      \
    } while (0)

/**
 * Assert that an expression evaluates to NV_OK or else do something.
 *
 * This macro can't use NV_ASSERT_OK_OR_ELSE_STR when NV_PRINTF is passed in as
 * the elseStmt parameter.
 *
 * @param[in] status   The NV_STATUS variable to capture the status
 * @param[in] expr     Expression that evaluates to an NV_STATUS.
 * @param[in] elseStmt Statement to evaluate if the expression is false.
 */
#define NV_ASSERT_OK_OR_ELSE(status, expr, elseStmt)                           \
    do                                                                         \
    {                                                                          \
        status = (expr);                                                       \
        PORT_COVERAGE_PUSH_OFF();                                              \
        if (NV_UNLIKELY(NV_OK != status))                                      \
        {                                                                      \
            NV_ASSERT_OK_FAILED(#expr, status);                                \
            elseStmt;                                                          \
        }                                                                      \
        PORT_COVERAGE_POP();                                                   \
    } while(0)

/**
 * Assert that an expression evaluates to NV_OK or else goto a label.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] expr Expression that evaluates to an NV_STATUS.
 * @param[in] label Label to jump to when the expression is false.
*/
#define NV_ASSERT_OK_OR_GOTO(status, expr, label)                              \
    NV_ASSERT_OK_OR_ELSE_STR(status, expr, #expr, goto label);

/**
 * Assert that an expression evaluates to NV_TRUE or else goto a label.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] cond   Condition that evaluates to either NV_TRUE or NV_FALSE.
 * @param[in] error  Error to be reflected in @ref status when @cond evaluates
                     to NV_FALSE.
 * @param[in] label  Label to jump to when @ref cond evaluates to NV_FALSE.
*/
#define NV_ASSERT_TRUE_OR_GOTO(status, cond, error, label)                     \
    NV_ASSERT_OK_OR_ELSE_STR(status, ((cond) ? NV_OK : (error)),               \
        #cond, goto label);

/**
 * Assert that an expression evaluates to NV_OK or else return the status.
 *
 * @param[in] expr Expression that evaluates to an NV_STATUS.
 */
#define NV_ASSERT_OK_OR_RETURN(expr)                                           \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_ASSERT_OK_OR_ELSE_STR(rm_pvt_status, expr, #expr,                   \
            return rm_pvt_status);                                             \
    } while(0)

/**
 * Assert that an expression evaluates to NV_OK or else do something.
 *
 * Although it can be used directly, this second level macro is designed to be
 * called from other macros.  Passing expr through multiple levels of macros
 * before it is stringified expands it.  This is especially bad for DRF macros,
 * which result in an embedded %, breaking the format string in the
 * NV_ASSERT_OK_FAILED_INLINE macro defined above.  The macros in this header
 * always pass the stringified expr as a into the second level macros as
 * a separate parameter.
 *
 * @param[in] status   The NV_STATUS variable to capture the status
 * @param[in] expr     Expression that evaluates to an NV_STATUS.
 * @param[in] exprStr  Expression as a string.
 * @param[in] elseStmt Statement to evaluate if the expression is false.
 */
#define NV_ASSERT_OK_OR_ELSE_STR(status, expr, exprStr, elseStmt)              \
    do                                                                         \
    {                                                                          \
        status = (expr);                                                       \
        PORT_COVERAGE_PUSH_OFF();                                              \
        if (NV_UNLIKELY(NV_OK != status))                                      \
        {                                                                      \
            NV_ASSERT_OK_FAILED(exprStr, status);                              \
            elseStmt;                                                          \
        }                                                                      \
        PORT_COVERAGE_POP();                                                   \
    } while(0)

// *****************************************************************************
// *                       NV_CHECK family of macros                           *
// *****************************************************************************
/**
 * General notes:
 *
 * Check that an expression is true.  If not, do the following actions:
 *   Print a message in the debug output at user specified level.
 *   Log a message in NvLog at user specified level.
 *   "Other action" as defined by each macro below.
 *
 * The actions are enabled or omitted based on platform and build, and the
 * implementations are platform dependent.
 *
 * The expression is always evaluated even if check failures are not logged
 * in the environment.  Use @ref NV_CHECK_CHECKED if the expression should only
 * be evaluated in checked builds.
 *
 * USE FOR error conditions that DO NOT require immediate action from NVIDIA,
 * but may be useful in diagnosing other issues.
 */

/**
 * Check that an expression is true.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to a truth value.
 */
#define NV_CHECK(level, expr)                                                  \
    NV_CHECK_OR_ELSE_STR(level, expr, #expr, /* no other action */)

/**
 * Check that an expression is true only in checked builds.
 *
 * @note The expression is only evaluated in checked builds so should
 *       not contain required side-effects.
 *       Also to prevent side effects, no "other action" is permitted.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to a truth value.
 */
#if PORT_IS_CHECKED_BUILD
#define NV_CHECK_CHECKED(level, expr)                                          \
    NV_CHECK_OR_ELSE_STR(level, expr, #expr, /* no other action */)
#else
#define NV_CHECK_CHECKED(level, expr)  ((void)0)
#endif

/**
 * Check that an expression is true or else do something.
 *
 * This macro can't use NV_CHECK_OR_ELSE_STR when NV_PRINTF is passed in as
 * the elseStmt parameter.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr     Expression that evaluates to a truth value.
 * @param[in] elseStmt Statement to evaluate if the expression is false.
 */
#define NV_CHECK_OR_ELSE(level, expr, elseStmt)                                \
    do                                                                         \
    {                                                                          \
        if (NV_UNLIKELY(!(expr)))                                              \
        {                                                                      \
            NV_CHECK_FAILED(level, #expr);                                     \
            elseStmt;                                                          \
        }                                                                      \
    } while (0)

/**
 * Check that an expression is true or else goto a label.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to a truth value.
 * @param[in] label Label to jump to when the expression is false.
 */
#define NV_CHECK_OR_GOTO(level, expr, label)                                   \
    NV_CHECK_OR_ELSE_STR(level, expr, #expr, goto label)

/**
 * Check that an expression is true or else return a value.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to a truth value.
 * @param[in] retval Value to return if the expression is false.
 */
#define NV_CHECK_OR_RETURN(level, expr, retval)                                \
    NV_CHECK_OR_ELSE_STR(level, expr, #expr, return (retval))

/**
 * Check that an expression is true or else return void.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to a truth value.
 */
#define NV_CHECK_OR_RETURN_VOID(level, expr)                                   \
    NV_CHECK_OR_ELSE_STR(level, expr, #expr, return)

/**
 * Check that an expression is true or else do something.
 *
 * Although it can be used directly, this second level macro is designed to be
 * called from other macros.  Passing expr through multiple levels of macros
 * before it is stringified expands it.  This is especially bad for DRF macros,
 * which result in an embedded %, breaking the format string in the
 * NV_CHECK_FAILED_INLINE macro defined above.  The macros in this header
 * always pass the stringified expr as a into the second level macros as
 * a separate parameter.
 *
 * @param[in] level    NV_PRINTF LEVEL to print at
 * @param[in] expr     Expression that evaluates to a truth value.
 * @param[in] exprStr  Expression as a string.
 * @param[in] elseStmt Statement to evaluate if the expression is false.
 */
#define NV_CHECK_OR_ELSE_STR(level, expr, exprStr, elseStmt)                   \
    do                                                                         \
    {                                                                          \
        if (NV_UNLIKELY(!(expr)))                                              \
        {                                                                      \
            NV_CHECK_FAILED(level, exprStr);                                   \
            elseStmt;                                                          \
        }                                                                      \
    } while (0)


// *****************************************************************************
// *                       NV_CHECK_OK family of macros                        *
// *****************************************************************************
/**
 * General notes:
 *
 * Check that an expression evaluates to NV_OK.  If not, do the following actions:
 *   Print a message in the debug output at user specified.
 *   Log a message in NvLog at user specified level.
 *   "Other action" as defined by each macro below.
 *
 * The actions are enabled or omitted based on platform and build, and the
 * implementations are platform dependent.
 *
 * The expression is always evaluated even if assertion failures are not logged
 * in the environment.  Use @ref NV_ASSERT_OK_CHECKED if the expression should
 * only be evaluated in checked builds.
 *
 * USE FOR error conditions that DO NOT require immediate action from NVIDIA,
 * but may be useful in diagnosing other issues.
 */

/**
 * Check that an expression evaluates to NV_OK.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to an NV_STATUS.
 */
#define NV_CHECK_OK(status, level, expr)                                       \
    do                                                                         \
    {                                                                          \
        NV_CHECK_OK_OR_ELSE_STR(status, level, expr, #expr,                    \
                                /* no other action */);                        \
    } while(0)

/**
 * Check that an expression evaluates to NV_OK only in checked builds.
 *
 * @note The expression is only evaluated in checked builds so should
 *       not contain required side-effects.
 *       Also to prevent side effects, no "other action" is permitted,
 *       and the status parameter is omitted.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr Expression that evaluates to an NV_STATUS.
 */
#if PORT_IS_CHECKED_BUILD
#define NV_CHECK_OK_CHECKED(level, expr)                                       \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_CHECK_OK_OR_ELSE_STR(rm_pvt_status, level, expr, #expr,             \
                                /* no other action */);                        \
    } while(0)
#else
#define NV_CHECK_OK_CHECKED(level, expr)  ((void)0)
#endif

/*!
 * Call a function that returns NV_STATUS and check that the return values is
 * NV_OK. If an error is returned, record the error code. In case this was a
 * first failure update global status @ref status.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] level  NV_PRINTF LEVEL to print at
 * @param[in] expr   Expression that evaluates to an NV_STATUS.
 */
#define NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, level, expr)                \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_CHECK_OK_OR_ELSE_STR(rm_pvt_status, level, expr, #expr,             \
            if (status == NV_OK) status = rm_pvt_status);                      \
    } while (0)

/**
 * Check that an expression evaluates to NV_OK or else do something.
 *
 * This macro can't use NV_CHECK_OK_OR_ELSE_STR when NV_PRINTF is passed in as
 * the elseStmt parameter.
 *
 * @param[in] status   The NV_STATUS variable to capture the status
 * @param[in] level    NV_PRINTF LEVEL to print at
 * @param[in] expr     Expression that evaluates to an NV_STATUS.
 * @param[in] elseStmt Statement to evaluate if the expression returns error.
 */
#define NV_CHECK_OK_OR_ELSE(status, level, expr, elseStmt)                     \
    do                                                                         \
    {                                                                          \
        status = (expr);                                                       \
        if (NV_UNLIKELY(NV_OK != status))                                      \
        {                                                                      \
            NV_CHECK_OK_FAILED(level, #expr, status);                          \
            elseStmt;                                                          \
        }                                                                      \
    } while (0)

/**
 * Check that an expression evaluates to NV_OK or else goto a label.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] level  NV_PRINTF LEVEL to print at
 * @param[in] expr   Expression that evaluates to an NV_STATUS.
 * @param[in] label  Label to jump to when the expression returns error.
 */
#define NV_CHECK_OK_OR_GOTO(status, level, expr, label)                        \
    NV_CHECK_OK_OR_ELSE_STR(status, level, expr, #expr, goto label)

/**
 * Check that an expression evaluates to NV_TRUE or else goto a label.
 *
 * @param[in] status The NV_STATUS variable to capture the status
 * @param[in] level  NV_PRINTF LEVEL to print at
 * @param[in] expr   Expression that evaluates to either NV_TRUE or NV_FALSE.
 * @param[in] error  Error to be reflected in @p status when @p expr evaluates
                     to NV_FALSE.
 * @param[in] label  Label to jump to when @p expr evaluates to NV_FALSE.
*/
#define NV_CHECK_TRUE_OR_GOTO(status, level, expr, error, label)       \
    NV_CHECK_OK_OR_ELSE_STR(status, level, ((expr) ? NV_OK : (error)), \
        #expr, goto label);

/**
 * Check that an expression evaluates to NV_OK or return the status.
 *
 * @param[in] level NV_PRINTF LEVEL to print at
 * @param[in] expr  Expression that evaluates to an NV_STATUS.
 */
#define NV_CHECK_OK_OR_RETURN(level, expr)                                     \
    do                                                                         \
    {                                                                          \
        NV_STATUS rm_pvt_status;                                               \
        NV_CHECK_OK_OR_ELSE_STR(rm_pvt_status, level, expr, #expr,             \
            return rm_pvt_status);                                             \
    } while(0)


/**
 * Check that an expression evaluates to NV_OK or else record the error code and
 * do something.
 *
 * Although it can be used directly, this second level macro is designed to be
 * called from other macros.  Passing expr through multiple levels of macros
 * before it is stringified expands it.  This is especially bad for DRF macros,
 * which result in an embedded %, breaking the format string in the
 * NV_CHECK_OK_FAILED_INLINE macro defined above.  The macros in this header
 * always pass the stringified expr as a into the second level macros as
 * a separate parameter.
 *
 * @param[in] status   The NV_STATUS variable to capture the status
 * @param[in] level    NV_PRINTF LEVEL to print at
 * @param[in] expr     Expression that evaluates to an NV_STATUS.
 * @param[in] exprStr  Expression as a string.
 * @param[in] elseStmt Statement to evaluate if the expression returns error.
 */
#define NV_CHECK_OK_OR_ELSE_STR(status, level, expr, exprStr, elseStmt)        \
    do                                                                         \
    {                                                                          \
        status = (expr);                                                       \
        if (NV_UNLIKELY(NV_OK != status))                                      \
        {                                                                      \
            NV_CHECK_OK_FAILED(level, exprStr, status);                        \
            elseStmt;                                                          \
        }                                                                      \
    } while (0)


// *****************************************************************************
// *                 NV_ASSERT_PRECOMP family of macros                        *
// *****************************************************************************
/**
 * General notes:
 *
 * Exactly the same as the NV_ASSERT macros, but always safe to use in
 * precompiled headers.
 *
 * On platforms other than GSP-RM, the _INLINE macros cannot be used inside
 * precompiled headers due to conflicting NVLOG_PRINT_IDs.  The PRECOMP macros
 * work around this issue by calling helper functions for NvLog.
 *
 * Hoping for a better solution, only the macro variants that are currently
 * used in precompiled headers are defined.
 *
 * See the NV_ASSERT documentation above for parameters and use cases.
 */
#define NV_ASSERT_PRECOMP(expr)                                                \
    NV_ASSERT_OR_ELSE_STR_PRECOMP(expr, #expr, /* no other action */)

#define NV_ASSERT_OR_RETURN_PRECOMP(expr, retval)                              \
    NV_ASSERT_OR_ELSE_STR_PRECOMP(expr, #expr, return (retval))

#define NV_ASSERT_OR_RETURN_VOID_PRECOMP(expr)                              \
    NV_ASSERT_OR_ELSE_STR_PRECOMP(expr, #expr, return)

#define NV_ASSERT_OR_ELSE_STR_PRECOMP(expr, exprStr, elseStmt)                 \
    do                                                                         \
    {                                                                          \
        PORT_COVERAGE_PUSH_OFF();                                              \
        if (NV_UNLIKELY(!(expr)))                                              \
        {                                                                      \
            NV_ASSERT_FAILED_PRECOMP(exprStr);                                 \
            elseStmt;                                                          \
        }                                                                      \
        PORT_COVERAGE_POP();                                                   \
    } while (0)

/**
 * @def NV_CHECKED_ONLY
 * @brief Compile a piece of code only in checked builds.
 *
 * This macro helps avoid #ifdefs to improve readability but should be
 * used sparingly.
 *
 * Cases that make heavy use of this should likely define a wrapper
 * macro or other abstraction for the build variation.
 * For example NV_CHECKED_ONLY(NV_PRINTF(...)) is not a good use case.
 *
 * A typical use case is declaring and setting a canary value:
 * ~~~{.c}
 * typedef struct
 * {
 *     NV_CHECKED_ONLY(NvU32 canary);
 *     ...
 * } MY_STRUCT;
 *
 * void initMyStruct(MY_STRUCT *pStruct)
 * {
 *     NV_CHECKED_ONLY(pStruct->canaray = 0xDEADBEEF);
 *     ...
 * }
 *
 * void destroyMyStruct(MY_STRUCT *pStruct)
 * {
 *     NV_ASSERT_CHECKED(pStruct->canaray == 0xDEADBEEF);
 *     ...
 * }
 * ~~~
 */
#if PORT_IS_CHECKED_BUILD
#define NV_CHECKED_ONLY NV_EXPAND
#else
#define NV_CHECKED_ONLY NV_DISCARD
#endif

// Verify prerequisites are defined.
#ifndef PORT_IS_CHECKED_BUILD
#error "NV_PORT_HEADER must define PORT_IS_CHECKED_BUILD"
#endif
#ifndef PORT_BREAKPOINT_CHECKED
#error "NV_PORT_HEADER must define PORT_BREAKPOINT_CHECKED"
#endif
#ifndef PORT_COVERAGE_PUSH_OFF
#error "NV_PORT_HEADER must define PORT_COVERAGE_PUSH_OFF"
#endif
#ifndef PORT_COVERAGE_POP
#error "NV_PORT_HEADER must define PORT_COVERAGE_POP"
#endif
#ifndef NV_PRINTF
#error "NV_PRINTF_HEADER must define NV_PRINTF"
#endif


#ifdef __cplusplus
}
#endif //__cplusplus
/// @}
#endif // _NV_UTILS_ASSERT_H_
