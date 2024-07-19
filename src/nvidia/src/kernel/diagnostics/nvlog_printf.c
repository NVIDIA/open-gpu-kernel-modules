/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************************************************************\
*                                                                           *
*  Description: Common debug print defines and functions                    *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "core/system.h"
#include "os/os.h" // to pick up declarations for osDelay() and osDelayUs()
#include "nvrm_registry.h"

#include <ctrl/ctrl0000/ctrl0000system.h> // NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE

static int inttodecfmtstr(NvS64 sval, char *dest, int fieldwidth, int flags);
static int uinttohexfmtstr(NvU64 uval, char *dest, int fieldwidth, int flags);
static int strtofmtstr(const char *src, char *dest, char *destLimit, int fieldwidth, int precision, int flags);

#if 0
static int float64todecfmtstr(F064 f64val, NvU8 *dest, int fieldwidth, int precision, int flags);
#endif

//
// Numeric & string conversion flags (used if you call the 'XtoYfmtstr' routines directly)
//
enum {
    DONTTERMINATE = 1,  // Don't null-terminate the string if this flag is set
    UNSIGNED_F = 2,     // Force an unsigned number conversion (other sign options are ignored)
    PLUSSIGN_F = 4,     // For signed numbers >= 0, force a '+' in the sign position
    SPACESIGN_F = 8,    // For signed numbers >= 0, force a space in the sign position
    LEFTALIGN_F = 16,   // Left-justify the result in the destination field (overrides zero fill)
    ZEROFILL_F = 32,    // Use leading zeros for padding to a field width
    LOWERCASE_F = 64    // Use lower case hex digits: a-f instead of A-F
};

//
// nvDbgBreakpointEnabled - Returns true if triggering a breakpoint is allowed
//
NvBool osDbgBreakpointEnabled(void);
NvBool nvDbgBreakpointEnabled(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    if (pSys != NULL)
    {
        if (pSys->getProperty(pSys, PDB_PROP_SYS_DEBUGGER_DISABLED))
            return NV_FALSE;
    }
    return osDbgBreakpointEnabled();
}

#if NV_PRINTF_STRINGS_ALLOWED
static PORT_SPINLOCK *_nv_dbg_lock = NULL;
static char   _nv_dbg_string[MAX_ERROR_STRING];

//
// nvDbgInit - Allocate the printf spinlock
//
NvBool
nvDbgInit(void)
{
    if (NULL != _nv_dbg_lock)
    {
        // already initialized
        return NV_TRUE;
    }
    if (portInitialize() != NV_OK)
        return NV_FALSE;

    _nv_dbg_lock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (_nv_dbg_lock == NULL)
        return NV_FALSE;
    else
        return NV_TRUE;
}

//
// nvDbgDestroy - Free the printf spinlock
//
void
nvDbgDestroy(void)
{
    if (NULL != _nv_dbg_lock)
    {
        portSyncSpinlockDestroy(_nv_dbg_lock);
        _nv_dbg_lock = NULL;
        portShutdown();
    }
}

//
// nvDbg_PrintMsg - Common message control for two flavors of printf
//
// Differences for mods builds.
//    * Mods has its own messaging system, and we always pass messages
//      to the mods unless RmMsg explicitly wants to hide a message.
//    * Mods requires messages even when the debugger is not enabled.
//    * Sorry for the #ifdefs, but RmMsg complicates the code enough
//      that it is nice to have one implementation.
//
static NvBool
nvDbg_PrintMsg
(
    const char *filename,
    int        linenumber,
    const char *function,
    int        debuglevel,
    const char *printf_format,
    NvBool     *pForce,
    NvU32      *pPrefix
)
{
    NvU32       rc;
    int debuglevel_min;

#if   defined(DEVELOP) || defined(DEBUG) || defined(QA_BUILD)
    debuglevel_min = LEVEL_NOTICE;
#else
    debuglevel_min = LEVEL_ERROR;
#endif

    OBJSYS *pSys = SYS_GET_INSTANCE();

    if ((NULL == pSys) || (pSys->getProperty(pSys, PDB_PROP_SYS_DEBUGGER_DISABLED)))
    {
        return NV_FALSE;
    }

    //
    // Message is filtered by an explicit RmMsg rule
    //
    rc = nvDbgRmMsgCheck(filename, linenumber, (char *)function, debuglevel, printf_format, pPrefix);
    switch (rc)
    {
        case NVRM_MSG_HIDE:
            // Hide this error message
            return NV_FALSE;

        case NVRM_MSG_PRINT:
            // Force this error message
            *pForce = NV_TRUE;
            return NV_TRUE;

        case NVRM_MSG_NORMAL:
        default:
            if (debuglevel >= debuglevel_min)
            {
                return NV_TRUE;
            }
            break;
    }
    return NV_FALSE;
}

void nvDbg_Printf
(
    const char *filename,
    int        linenumber,
    const char *function,
    int        debuglevel,
    const char *printf_format,
    ...
)
{
    va_list     arglist;
    va_start(arglist, printf_format);
    nvDbg_vPrintf(filename, linenumber, function, debuglevel, printf_format, arglist);
    va_end(arglist);
}

//
// Internal function to prepare _nv_dbg_string for printing.
// Should only be called while _nv_dbg_lock is held.
//
static void
_nvDbgPrepareString
(
    const char *file,
    int        line,
    const char *func,
    const char *fmt,
    NvU32      prefix,
    va_list    arglist
)
{
    NvU32 len = 0;

    //
    // If RmMsg has added a prefix, skip the standard NV_PRINTF_PREFIX.
    // If there is no prefix, don't include the RmMsg prefix.
    //
    if (portStringCompare(fmt, NV_PRINTF_PREFIX, sizeof(NV_PRINTF_PREFIX) - 1) == 0)
    {
        len = RmMsgPrefix(prefix, file, line, func, _nv_dbg_string, MAX_ERROR_STRING);
        fmt += sizeof(NV_PRINTF_PREFIX) - 1;
    }

    nvDbgVsnprintf(_nv_dbg_string + len, MAX_ERROR_STRING - len, fmt, arglist);
}

//
// Temporary helper to map LEVEL_xxx constants to a platform specific level.
//
#if PORT_IS_FUNC_SUPPORTED(portDbgExPrintfLevel)

static NvU32 _nvDbgForceLevel(NvBool bForce, NvU32 level)
{
    return bForce ? LEVEL_FATAL : level;
}
#endif

//
// Some varargs interfaces need a va_list interface, but still
// want the common output buffer and the RmMsg handling.
//
void nvDbg_vPrintf
(
    const char *filename,
    int        linenumber,
    const char *function,
    int        debuglevel,
    const char *printf_format,
    va_list    arglist
)
{
    NvBool  force = NV_FALSE;
    NvU32 prefix = 0;

    if (nvDbg_PrintMsg(filename, linenumber, function, debuglevel, printf_format, &force, &prefix))
    {
        portSyncSpinlockAcquire(_nv_dbg_lock);
        _nvDbgPrepareString(filename, linenumber, function, printf_format, prefix, arglist);
#if PORT_IS_FUNC_SUPPORTED(portDbgExPrintfLevel)
        portDbgExPrintfLevel(_nvDbgForceLevel(force, debuglevel),
                             "%.*s", MAX_ERROR_STRING, _nv_dbg_string);
#else
        portDbgPrintString(_nv_dbg_string, MAX_ERROR_STRING);
#endif
        portSyncSpinlockRelease(_nv_dbg_lock);
    }
}


#define IS_PRINT(c)     (((c) >= 0x20) && ((c) <= 0x7E))

void nvDbg_PrintBuf
(
    const char *file,
    int        line,
    const char *function,
    int        dbglevel,
    NvU8       buffer[],
    NvU32      bufsize
)
{
    NvU32 i, j;
    nvDbg_Printf(file, line, function, dbglevel, NV_PRINTF_ADD_PREFIX("printBuf [BEGIN]"));
    for (i = 0; i < bufsize; i += 16)
    {
        nvDbg_Printf(file, line, function, dbglevel, "\n");
        nvDbg_Printf(file, line, function, dbglevel, NV_PRINTF_ADD_PREFIX("printBuf 0x%p  "), buffer + i);
        for (j = 0; j < 16; j++)
        {
            if ((i + j) < bufsize)
            {
                nvDbg_Printf(file, line, function, dbglevel, "%02x", *(buffer + i + j));
            }
            else
            {
                nvDbg_Printf(file, line, function, dbglevel, "  ");
            }
        }
        nvDbg_Printf(file, line, function, dbglevel, " ");
        for (j = 0; j < 16; j++)
        {
            if ((i + j) < bufsize)
            {
                nvDbg_Printf(file, line, function, dbglevel, "%c", IS_PRINT(*(buffer + i + j))? *(buffer + i + j) : '.');
            }
            else
            {
                nvDbg_Printf(file, line, function, dbglevel, " ");
            }
        }
    }
    nvDbg_Printf(file, line, function, dbglevel, "\n");
    nvDbg_Printf(file, line, function, dbglevel, NV_PRINTF_ADD_PREFIX("printBuf [END]\n"));
}

#endif

#define TMPBUF_SIZE  63
//======================================================================================
//
// nvDbgVsnprintf()
//
//======================================================================================
int nvDbgVsnprintf(char *dest, NvU32 destSize, const char *fmt, va_list args)
{
    int ch, precision, flags;
    NvU32 fieldwidth;
    int   longlong;
    NvS32 s32val;
    NvU32 u32val;
    NvS64 s64val;
    NvU64 u64val;

    const char *f;
    const char *specptr;
    char *d;
    char *strpval;
    char *destLimit;
    void *pval;
    char  tmpBuf[TMPBUF_SIZE + 1];
    NvU32 tmpSize;

    if (dest == NULL || destSize == 0)
    {
        return(0);  // If we don't have a destination, we didn't do any characters
    }

    f = fmt;
    d = dest;
    destLimit = dest + destSize - 1;
    dest[destSize - 1] = 0;

    while ((ch = *f++) != '\0')
    {
        if (ch != '%')
        {
            if (d < destLimit)
            {
                *d++ = (NvU8)ch;
            }
            continue;
        }
        longlong = NV_FALSE;
        specptr = f - 1;    // Save a pointer to the '%' specifier, in case of syntax errors
        ch = *f++;

        // revert to correct printf behavior for %
        // from printf.3 regarding '%' format character:
        //     %   A `%' is written.  No argument is converted.  The complete conversion specification is `%%'.
        if (ch == '%') {
            if (d < destLimit)
            {
                *d++ = (NvU8)ch;
            }
            continue;
        }

        flags = DONTTERMINATE;  // Don't terminate substrings -- we'll null-terminate when we're all done
        // Check for left-alignment
        if (ch == '-') {
            flags |= LEFTALIGN_F;
            ch = *f++;
        }
        // Check for using a plus sign for non-negative numbers
        if (ch == '+') {
            flags |= PLUSSIGN_F;
            ch = *f++;
        }
        // Check for using a space character (sign place-holder) for non-negative numbers
        if (ch == ' ') {
            flags |= SPACESIGN_F;
            ch = *f++;
        }
        // Check for leading zero fill
        if (ch == '0') {
            flags |= ZEROFILL_F;
            // Don't bump the character pointer in case '0' was the only digit
        }
        // Collect the field width specifier
        if (ch == '*') {
            // Field width specified by separate argument
            fieldwidth = va_arg(args, int);
            ch = *f++;
        }
        else {
            fieldwidth = 0; // Default field width
            while (ch >= '0' && ch <= '9') {
                fieldwidth = fieldwidth * 10 + ch - '0';
                ch = *f++;
            }
        }

        // Check for a precision specifier
        precision = -1;     // Default unspecified precision
        if (ch == '.') {    // We have a precision specifier, skip the '.'
            ch = *f++;
            if (ch == '*') {
                // precision specified by separate argument
                precision = va_arg(args, int);
                ch = *f++;
            }
            else {
                while (ch >= '0' && ch <= '9') {
                    precision = precision * 10 + ch - '0';
                    ch = *f++;
                }
            }
        }

        if (ch == 'l') {
            ch = *f++;
            if (ch == 'l') {
                longlong = NV_TRUE;
                ch = *f++;
            }
        }

        // Perform the conversion operation
        switch (ch) {
        case 'c':   // Copy an ASCII character
            u32val = va_arg(args, int);
            if (d < destLimit)
            {
                *d++ = (NvU8) u32val;
            }
            break;
        case 'u':   // Copy a formatted, unsigned decimal number
            flags |= UNSIGNED_F;
            if (fieldwidth > TMPBUF_SIZE)
            {
                fieldwidth = TMPBUF_SIZE;
            }
            if ( longlong )  // long long specifier "llu" or "lld"
            {
                u64val = va_arg(args, unsigned long long);
                // Format the number, increment the dest pointer by the characters copied
                tmpSize = inttodecfmtstr(u64val, tmpBuf, fieldwidth, flags);
            }
            else
            {
                u32val = va_arg(args, unsigned int);
                // Format the number, increment the dest pointer by the characters copied
                tmpSize = inttodecfmtstr((NvU64)u32val, tmpBuf, fieldwidth, flags);
            }
            if (d < destLimit)
            {
                tmpSize = (d + tmpSize) < destLimit ? tmpSize : (NvU32)(destLimit - d);
                portMemCopy(d, tmpSize, tmpBuf, tmpSize);
                d += tmpSize;
            }
            break;
        case 'd':   // Copy a formatted, signed decimal number
            if (fieldwidth > TMPBUF_SIZE)
            {
                fieldwidth = TMPBUF_SIZE;
            }
            if ( longlong )  // long long specifier "llu" or "lld"
            {
                s64val = va_arg(args, long long);
                // Format the number, increment the dest pointer by the characters copied
                tmpSize = inttodecfmtstr(s64val, tmpBuf, fieldwidth, flags);
            }
            else
            {
                s32val = va_arg(args, int);
                // Format the number, increment the dest pointer by the characters copied
                tmpSize = inttodecfmtstr((NvS64)s32val, tmpBuf, fieldwidth, flags);
            }
            if (d < destLimit)
            {
                tmpSize = (d + tmpSize) < destLimit ? tmpSize : (NvU32)(destLimit - d);
                portMemCopy(d, tmpSize, tmpBuf, tmpSize);
                d += tmpSize;
            }
            break;
        case 'x':   // Copy a formatted, lower-case hexadecimal number
            flags |= LOWERCASE_F;
        case 'X':   // Copy a formatted, upper-case hexadecimal number
            if (fieldwidth > TMPBUF_SIZE)
            {
                fieldwidth = TMPBUF_SIZE;
            }
            if ( longlong )   // long long specifier "llx" or "llX"
            {
                u64val = va_arg(args, long long);
                // Format the number, increment the dest pointer by the characters copied
                tmpSize = uinttohexfmtstr(u64val, tmpBuf, fieldwidth, flags);
            }
            else
            {
                u32val = va_arg(args, int);
                // Format the number, increment the dest pointer by the characters copied
                tmpSize = uinttohexfmtstr((NvU64)u32val, tmpBuf, fieldwidth, flags);
            }
            if (d < destLimit)
            {
                tmpSize = (d + tmpSize) < destLimit ? tmpSize : (NvU32)(destLimit - d);
                portMemCopy(d, tmpSize, tmpBuf, tmpSize);
                d += tmpSize;
            }
            break;
        case 'p':   // Copy a formatted pointer value
            if (fieldwidth > TMPBUF_SIZE)
            {
                fieldwidth = TMPBUF_SIZE;
            }
            pval = va_arg(args, void *);
            tmpSize = uinttohexfmtstr((NvU64)((NvUPtr)pval), tmpBuf, fieldwidth, flags);
            if (d < destLimit)
            {
                tmpSize = (d + tmpSize) < destLimit ? tmpSize : (NvU32)(destLimit - d);
                portMemCopy(d, tmpSize, tmpBuf, tmpSize);
                d += tmpSize;
            }
            break;
        case 's':   // Copy a formatted string
            strpval = va_arg(args, char *);
            d += strtofmtstr(strpval, d, destLimit, fieldwidth, precision, flags);
            break;
        case 0:     // Gracefully handle premature end-of-string
            f--;    // Back up, now f points to the null character again
        default:    // Unexpected conversion operator, so just echo to the destination
            while (specptr < f)
            {
                if (d < destLimit)
                {
                    *d++ = *specptr;
                }
                specptr++;
            }
            if (ch == 0)
            {
                goto stringdone;
            }
            break;
        }
    }

stringdone:
    if (d <= destLimit)
    {
        *d = '\0'; // Null-terminate the string
    }
    return((int)(d - dest));   // Return the number of characters we [might] transferred
}

int nvDbgSnprintf(char *dest, NvU32 destSize, const char *fmt, ...)
{
    va_list arglist;
    int len;

    va_start(arglist, fmt);
    len = nvDbgVsnprintf(dest, destSize, fmt, arglist);
    va_end(arglist);

    return len;
}

enum {  // Padding option definitions
    PRESPACE_O = 1,
    PREZERO_O = 2,
    POSTSPACE_O = 4
};

#define NUMBUFSIZE  20  // Should be enough for 64-bit integers in decimal or hex

//======================================================================================
//
// inttodecfmtstr()
//
//  This takes a signed integer value and converts it to a formatted decimal string,
//  using options (field width and flags) like those provided by sprintf().  The 32-bit
//  number is assumed to be signed unless the UNSIGNED_F flag is set.  Look at the code
//  for dbugsprintf() above to see which formatting options are implemented.
//
//======================================================================================
static int inttodecfmtstr(NvS64 sval, char *dest, int fieldwidth, int flags)
{
    int i, digitcount, destcount;
    int sign, signchar;
    int fillcount;
    int pad_options;
    NvU64 uval, quotient, remainder;
    char *intdigp;
    char nbuf[NUMBUFSIZE];

    signchar = ' ';           // avoid compiler init warning
    // Process the sign-related options
    if (flags & UNSIGNED_F) {   // Unsigned conversion
        sign = 0;   // No sign character
    } else {    // We're doing a signed conversion
        sign = 1;   // Assume we'll have a sign character
        if (sval < 0) {
            signchar = '-';
            sval = -sval;   // Make the number positive now so we can 'digitize' it
        } else {    // sval >= 0
            if (flags & PLUSSIGN_F)
                signchar = '+';
            else if (flags & SPACESIGN_F)
                signchar = ' ';
            else
                sign = 0;   // No sign character
        }
    }
    uval = sval;    // Do unsigned math from here on out

    // Convert the number into ASCII decimal digits in our local buffer, counting them
    intdigp = &nbuf[NUMBUFSIZE];    // Point past the last character in the buffer
    digitcount = 0; // Nothing written to our local buffer yet
    do {
        quotient = uval / 10;
        remainder = uval - quotient * 10;
        *--intdigp =  (NvU8) (remainder + '0'); // Put the digit into the next lower buffer slot
        digitcount++;
        uval = quotient;
    } while (uval > 0);

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    fillcount = fieldwidth - (sign + digitcount);   // Account for the sign, if used
    if (fillcount > 0) {    // We need to do left or right padding
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F)
                pad_options = PREZERO_O;
            else
                pad_options = PRESPACE_O;
        }
    }

    destcount = 0;  // Nothing written out to the destination yet

    // Copy any leading spaces
    if (pad_options & PRESPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    // Copy the sign character, if any
    if (sign) {
        *dest++ = (char)signchar;
        destcount++;
    }
    // Copy any leading zeros
    if (pad_options & PREZERO_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = '0';
        destcount += fillcount;
    }
    // Copy the decimal digits from our local buffer
    for (i = 0; i < digitcount; i++)
        *dest++ = *intdigp++;
    destcount += digitcount;

    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *dest = 0;
    return(destcount);  // Return the character count, not including the null
}

//======================================================================================
//
// uinttohexfmtstr()
//
//  This takes an unsigned integer value and converts it to a formatted hexadecimal
//  string, using options (field width and flags) like those provided by sprintf().  Look
//  at the code for dbugsprintf() above to see which formatting options are implemented.
//
//======================================================================================
static int uinttohexfmtstr(NvU64 uval,  char *dest, int fieldwidth, int flags)
{
    int i, digitcount, destcount;
    int c, hexadjust;
    int fillcount;
    char fillchar = ' ';
    int pad_options;
    char *intdigp;
    char nbuf[NUMBUFSIZE];

    hexadjust = 'A' - '9' - 1;
    if (flags & LOWERCASE_F)
        hexadjust += 'a' - 'A';

    // Convert the number into ASCII hex digits in our local buffer, counting them
    intdigp = &nbuf[NUMBUFSIZE];    // Point past the last character in the buffer
    digitcount = 0; // Nothing written to our local buffer yet
    do {
        c = (int)(uval % 16) + '0';
        if (c > '9')    /* A-F */
            c += hexadjust;
        *--intdigp = (NvU8)c; // Put the digit into the next lower buffer slot
        digitcount++;
        uval /= 16;
    } while (uval > 0);

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    fillcount = fieldwidth - digitcount;    // No sign to worry about
    if (fillcount > 0) {    // We need to do left or right padding
        fillchar = ' ';     // Most common fill character is the space
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F) {
                pad_options = PREZERO_O;
                fillchar = '0';
            } else {
                pad_options = PRESPACE_O;
            }
        }
    }

    destcount = 0;  // Nothing written out to the destination yet

    // Copy any leading zeros or spaces
    if (pad_options & (PREZERO_O | PRESPACE_O)) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = fillchar;
        destcount += fillcount;
    }
    // Copy the hex digits from our local buffer
    for (i = 0; i < digitcount; i++)
        *dest++ = *intdigp++;
    destcount += digitcount;

    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = fillchar;
        destcount += fillcount;
    }
    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *dest = 0;
    return(destcount);  // Return the character count, not including the null
}


#if 0

//======================================================================================
//
// float64todecfmtstr()
//
//  This takes a 64-bit floating-point value and converts it to a formatted decimal
//  string, using options (field width, precision, and flags) like those provided by
//  sprintf().  Look at the code for dbugsprintf() above to see which formatting options
//  are implemented.
//
//======================================================================================
static int float64todecfmtstr(F064 f64val, NvU8 *dest, int fieldwidth, int precision, int flags)
{
    int i, firstcount, destcount;
    int sign, signchar, decpt;
    int fillcount;
    int pad_options;
    int reducecount, loopdigits, digitsleft;
    NvU32 u32val, quotient, remainder;
    F064 f64mant9 = 0.0, f64mant9factor = 0.0, fone = 0.0, ften = 0.0, fbillion = 0.0, powerof10 = 0.0;
    NvU8 *digp;
    NvU8 nbuf[NUMBUFSIZE];  // This only needs to hold the first 9 digits of the integer part

    // Process the sign-related options
    sign = 1;   // Assume at first we'll have a sign character
    if (f64val < 0.0) {
        signchar = '-';
        f64val = -f64val;   // Make the number positive now so we can 'digitize' it
    } else {    // f64val >= 0.0
        if (flags & PLUSSIGN_F)
            signchar = '+';
        else if (flags & SPACESIGN_F)
            signchar = ' ';
        else
            sign = 0;   // No sign character
    }

    // Round the number to N decimal places.  We add 0.5 x 10^(-N), which is
    //  equivalent to adding 1 / (2*10^N).  We'll use this latter formula.
    fone = 1.0;     // Keep the compiler from always loading these constants from memory
    ften = 10.0;
    powerof10 = fone;   // 10 ^ 0
    for (i = 0; i < precision; i++)
        powerof10 *= ften;  // Build 10 ^ N
    f64val += fone / (2.0 * powerof10);
    // f64val now contains the properly rounded number

    f64mant9 = f64val;  // Start hunting for the mantissa's 9 uppermost decimal digits
    fbillion = 1e9;     // Keep it in a register
    f64mant9factor = fone;
    // Reduce the mantissa to less than 1 billion, so it will fit in a 32-bit integer
    for (reducecount = 0; f64mant9 >= fbillion; reducecount++) {
        f64mant9 /= fbillion;
        f64mant9factor *= fbillion;
    }

    // Process the highest 32-bits of the mantissa so we can count those digits first

    f64mant9 = f64val / f64mant9factor; // Grab highest 9 integer decimal digits
    u32val = (NvU32) f64mant9;   // Drop any fraction
    f64mant9 = u32val;  // Now we have a float with only an integer part
    f64val -= f64mant9 * f64mant9factor;    // Subtract out the previous high digits
    f64mant9factor /= fbillion;     // Adjust our division factor

    // Convert the binary into ASCII decimal digits in our local buffer, counting them
    digp = &nbuf[NUMBUFSIZE];   // Point past the last char. of these 9 digits
    firstcount = 0; // No digits of the first 32-bit integer part yet
    do {
        quotient = u32val / 10;
        remainder = u32val - quotient * 10;
        *--digp = (NvU8) (remainder + '0'); // Put the digit into the next lower buffer slot
        firstcount++;
        u32val = quotient;
    } while (u32val > 0);

    // Figure out whether we'll have a decimal point
    decpt = (precision > 0);    // Don't use a decimal point if no fractional part

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    // We have the information we need to calculate how many output characters we'll have
    fillcount = fieldwidth - (sign + firstcount + (reducecount * 9) + decpt + precision);
    if (fillcount > 0) {    // We need to do left or right padding
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F)
                pad_options = PREZERO_O;
            else
                pad_options = PRESPACE_O;
        }
    }

    destcount = 0;  // Nothing written out to the destination yet

    // Copy any leading spaces
    if (pad_options & PRESPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    // Copy the sign character, if any
    if (sign) {
        *dest++ = signchar;
        destcount++;
    }
    // Copy any leading zeros
    if (pad_options & PREZERO_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = '0';
        destcount += fillcount;
    }
    // Copy the highest chunk of integer digits from the local buffer
    for (i = 0; i < firstcount; i++)
        *dest++ = *digp++;
    destcount += firstcount;

    // Now we need to convert the remaining integer digits, if any
    for (i = 0; i < reducecount; i++) {
        f64mant9 = f64val / f64mant9factor; // Grab 9 more decimal digits
        u32val = (NvU32) f64mant9;       // Drop any fraction
        f64mant9 = u32val;      // Now we have a float with only an integer part
        f64val -= f64mant9 * f64mant9factor;    // Subtract out the previous high digits
        f64mant9factor /= fbillion;         // Adjust our division factor
        // Convert the integer part into ASCII decimal digits, directly to the destination
        dest += 9;              // Point past the last char. of this 9-digit chunk
        digp = dest;
        for (loopdigits = 0; loopdigits < 9; loopdigits++) {
            quotient = u32val / 10;
            remainder = u32val - quotient * 10;
            *--digp = (NvU8) (remainder + '0'); // Put the digit into the next lower buffer slot
            u32val = quotient;
        }
        destcount += 9;
    }
    // f64val has only the fractional part now

    if (!decpt)
        goto checktrailing; // Skip the laborious fraction-processing part

    // Copy the decimal point
    *dest++ = '.';
    destcount++;

    // Similar to how we handled the integer part processing, we'll process up to
    //  9 digits at a time, by multiplying the fraction by a power of 10,
    //  converting to an integer, and converting digits to the destination.

    digitsleft = precision;
    do {
        loopdigits = digitsleft;
        if (loopdigits > 9)
            loopdigits = 9;
        powerof10 = fone;   // 10 ^ 0
        for (i = 0; i < loopdigits; i++)
            powerof10 *= ften;  // Build 10 ^ N
        f64val *= powerof10;    // Push some fractional digits into the integer part
        u32val = (NvU32) f64val; // Conversion truncates any remaining fraction
        f64val -= u32val;   // Remove the integer part, leave remaining fraction digits
        digp = dest + loopdigits;   // Point past the last char. of this chunk
        for (i = 0; i < loopdigits; i++) {
            quotient = u32val / 10;
            remainder = u32val - quotient * 10;
            *--digp = (NvU8) (remainder + '0'); // Put the digit into the next lower buffer slot
            u32val = quotient;
        }
        dest += loopdigits;
        destcount += loopdigits;
        digitsleft -= loopdigits;
    } while (digitsleft > 0);

checktrailing:
    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *dest = 0;
    return(destcount);  // Return the character count, not including the null
}

#endif // 0

//======================================================================================
//
// strtofmtstr()
//
//  This takes a source C string and converts it to a formatted output C string,
//  using options (field width, precision, and flags) like those provided by sprintf().  Look at
//  the code for nvDbgVsnprintf() above to see which formatting options are implemented.
//
//  fieldwidth - minimum total characters to output (including pad)
//  precision  - maximum characters from src to output; or entire string if negative
//======================================================================================
static int strtofmtstr(const char *src, char *dest, char *destLimit, int fieldwidth, int precision, int flags)
{
    int i, srclen;
    int fillcount;
    char fillchar = ' ';
    int pad_options;
    const char *s;
    char *d;

    // Make sure we have a source string to work with
    if (src == NULL)
    {
        src = "";
    }

    // For padding calculations, we need to know the source string length
    for (s = src, srclen = 0; *s != 0; s++)
        srclen++;

    // But truncated to precision, if specified.
    if(precision >= 0 && srclen > precision)
        srclen = precision;

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    fillcount = fieldwidth - srclen;

    if (fillcount > 0) {    // We need to do left or right padding
        fillchar = ' ';     // Most common fill character is the space
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F) {
                pad_options = PREZERO_O;
                fillchar = '0';
            } else {
                pad_options = PRESPACE_O;
            }
        }
    }

    s = src;
    d = dest;

    // Copy any leading zeros or spaces
    if (pad_options & (PREZERO_O | PRESPACE_O))
    {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
        {
            if (d < destLimit)
            {
                *d++ = fillchar;
            }
        }
    }
    // Copy the characters from the source string
    for (i = 0; i < srclen; i++)
    {
        if (d < destLimit)
        {
            *d++ = *s++;
        }
    }

    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O)
    {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
        {
            if (d < destLimit)
            {
                *d++ = fillchar;
            }
        }
    }

    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *d = 0;
    return((int)(d - dest));   // Return the character count, not including the null
}

#if NV_PRINTF_STRINGS_ALLOWED
//
// String matching helper for nvDbgRmMsgCheck.
// strstr with the length of the pattern string
// passed in.
//

static const char *nv_strnstr
(
    const char *str,
    const char *pat,
    int  patlen
)
{
    int len;

    // Should be NULL, but this makes noun optional
    if (pat == NULL)
    {
        return str;
    }

    while (*str)
    {
        len = 0;
        while (len < patlen)
        {
            if (str[len] != pat[len])
                break;
            len++;
        }
        if  (len == patlen)
        {
            return str;
        }
        str++;
    }
    return NULL;
}

//
// Buffer to store RmMsg string. This is stored in bss
// so it can be updated in the debugger dynamically.
//
char RmMsg[NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE];

//
// nvDbgRmMsgCheck
//    Override priority of debug printf based on file, function with optional
//    line ranges. Rules are matched on each printf. Rules are applied left to
//    right and the final result is the cumulative result of all rules.
//
// Format
//    rule = [!][filename|function][:startline][-endline][@level][^prefix]
//    Format = rule[,rule]
//
// See RmMsg wiki for detailed documentation

// Examples:
//   "dmanv50.c" - enable all printfs in dmanv50.c
//   "fifoAlloc_NV50" - enable all printfs in function fifoAlloc_NV50
//   "!fifoAlloc_NV50" - disable all printfs in function fifoAlloc_NV50
//   "dmanv50.c:150" - enable printf on line 150 of dmanv50.c
//   "dmanv50.c:100-200" - enable printf on lines 100-200 in dmanv50.c
//   "dmanv50.c:100-200,!dmanv50:125" - same but disable printf on line 125
//   "fifo^*" - enable verbose prefix for fifo
//   ":" - enable all printfs
//   "!" - disable all printfs (dangerous!)
//
NvU32
nvDbgRmMsgCheck
(
    const char * filename,
    NvU32        linenumber,
    const char * function,
    NvU32        debuglevel,
    const char * printf_format,
    NvU32      * pPrefix
)
{
    enum { NOUN, STARTLINE, ENDLINE, LEVEL, PREFIX } state;
    int  status = NVRM_MSG_NORMAL;
    int  inc;
    char *noun;
    NvU32 nounlen;
    NvU32 startline;
    NvU32 endline;
    NvU32 level;
    NvU32 prefix = NVRM_MSG_PREFIX_NVRM | NVRM_MSG_PREFIX_FUNCTION;
    NvU32 tempPrefix;
    char *p;

    // Handle the normal case quickly.
    if (RmMsg[0] == '\0')
    {
        goto done;
    }

    p = RmMsg;

    while (*p != '\0')
    {
        // Initial default state for this rule
        inc = 1;
        noun = NULL;
        nounlen = 0;
        startline = 0;
        endline = 0x7fffffff;
        tempPrefix = NVRM_MSG_PREFIX_NVRM | NVRM_MSG_PREFIX_FUNCTION;
        level = LEVEL_INFO;         // default to everything
        state = NOUN;

        for (; *p != '\0' && *p != ','; p++)
        {
            if (*p == ':')
            {
                state = STARTLINE;
                continue;
            }
            else if (*p == '-')
            {
                state = ENDLINE;
                endline = 0;
                continue;
            }
            else if (*p == '!' && !noun)
            {
                state = NOUN;
                inc = 0;
                continue;
            }
            else if (*p == '@')
            {
                state = LEVEL;
                level = 0;
                continue;
            }
            else if (*p == '^')
            {
                state = PREFIX;
                tempPrefix = NVRM_MSG_PREFIX_NVRM | NVRM_MSG_PREFIX_FUNCTION;
                continue;
            }
            switch (state)
            {
                case NOUN:
                    if (noun == NULL)
                    {
                        noun = p;
                    }
                    nounlen++;
                    break;
                case STARTLINE:
                    if ((*p >= '0') && (*p <= '9'))
                    {
                        startline *= 10;
                        startline += *p - '0';
                        endline = startline;            // only one line
                    }
                    break;
                case ENDLINE:
                    if ((*p >= '0') && (*p <= '9'))
                    {
                        endline *= 10;
                        endline += *p - '0';
                    }
                    break;
                case LEVEL:
                    if ((*p >= '0') && (*p <= '9'))
                    {
                        level *= 10;
                        level += *p - '0';
                    }
                    break;
                case PREFIX:
                    switch (*p)
                    {
                        case '*':
                            tempPrefix = NVRM_MSG_PREFIX_NVRM | NVRM_MSG_PREFIX_FILE |
                                         NVRM_MSG_PREFIX_LINE | NVRM_MSG_PREFIX_FUNCTION |
                                         NVRM_MSG_PREFIX_OSTIMESTAMP;
                            break;
                        case 'n':
                            tempPrefix |= NVRM_MSG_PREFIX_NVRM;
                            break;
                        case 'N':
                            tempPrefix &= ~NVRM_MSG_PREFIX_NVRM;
                            break;
                        case 'c':
                            tempPrefix |= NVRM_MSG_PREFIX_FILE;
                            break;
                        case 'C':
                            tempPrefix &= ~NVRM_MSG_PREFIX_FILE;
                            break;
                        case 'l':
                            tempPrefix |= NVRM_MSG_PREFIX_LINE;
                            break;
                        case 'L':
                            tempPrefix &= ~NVRM_MSG_PREFIX_LINE;
                            break;
                        case 'f':
                            tempPrefix |= NVRM_MSG_PREFIX_FUNCTION;
                            break;
                        case 'F':
                            tempPrefix &= ~NVRM_MSG_PREFIX_FUNCTION;
                            break;
                        case 't':
                            tempPrefix |= NVRM_MSG_PREFIX_OSTIMESTAMP;
                            break;
                        case 'T':
                            tempPrefix &= ~NVRM_MSG_PREFIX_OSTIMESTAMP;
                            break;
                    }
                    break;
                default:            // ignore any trainling words
                    break;
            }
        }

        // Does the last rule hit
        if (((nv_strnstr(filename, noun, nounlen) != NULL) ||
             (nv_strnstr(function, noun, nounlen) != NULL)) &&
            (linenumber >= startline) &&
            (linenumber <= endline))
        {
            status = inc ? NVRM_MSG_PRINT : NVRM_MSG_HIDE;
            prefix = tempPrefix;

            if (status == NVRM_MSG_PRINT && debuglevel < level)
            {
                status = NVRM_MSG_HIDE;
            }
        }

        if (*p == '\0')
        {
            break;
        }
        p++;
    }

done:
    if (pPrefix != NULL)
    {
        *pPrefix = prefix;
    }

    return status;
}

//
// RmMsgPrefix - Add the RmMsg prefix to the passed in string, returning
// the length of the formatted string.
//
// Format: "NVRM: file linenum function timestamp: "
//
NvU32
RmMsgPrefix
(
    NvU32 prefix,
    const char *filename,
    NvU32 linenumber,
    const char *function,
    char *str,
    NvU32 totalLen
)
{
    const char *space = "";
    NvU32 len = 0;
    NvU32 sec, usec;

    *str = '\0';

    if (prefix & NVRM_MSG_PREFIX_NVRM)
    {
        portStringCopy(str + len, totalLen - len, NV_PRINTF_PREFIX, sizeof(NV_PRINTF_PREFIX));
        len += sizeof(NV_PRINTF_PREFIX) - 1;
        portStringCopy(str + len, totalLen - len, NV_PRINTF_PREFIX_SEPARATOR, sizeof(NV_PRINTF_PREFIX_SEPARATOR));
        len += sizeof(NV_PRINTF_PREFIX_SEPARATOR) - 1;
    }

    if (prefix & NVRM_MSG_PREFIX_FILE)
    {
        len += nvDbgSnprintf(str + len, totalLen - len, "%s%s", space, filename);
        space = " ";
    }

    if (prefix & NVRM_MSG_PREFIX_LINE)
    {
        len += nvDbgSnprintf(str + len, totalLen - len, "%s%d", space, linenumber);
        space = " ";
    }

    if (prefix & NVRM_MSG_PREFIX_FUNCTION)
    {
        len += nvDbgSnprintf(str + len, totalLen - len, "%s%s", space, function);
        space = " ";
    }

    if (prefix & NVRM_MSG_PREFIX_OSTIMESTAMP)
    {
        osGetCurrentTime(&sec, &usec);

        len += nvDbgSnprintf(str + len, totalLen - len, "%s%d.%06d", space, sec, usec);
    }

    return len;
}

//
// Initialize RmMsg from the registry.  Skip if the string was initialized
// already initialized (from the debugger).
// Called from the platform specific platform code.
//
void nvDbgInitRmMsg(OBJGPU *pGpu)
{
    NvU32 len = NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE;

    if (RmMsg[0] == '\0')
    {
        if (osReadRegistryString(pGpu, NV_REG_STR_RM_MSG,
                    (NvU8*)RmMsg, &len) != NV_OK)
        {
            len = NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE;
        }
    }
}

#else // #else ! NV_PRINTF_STRINGS_ALLOWED

void nvDbgInitRmMsg(OBJGPU *pGpu)
{
}

NvU32
nvDbgRmMsgCheck
(
    const char * filename,
    NvU32        linenumber,
    const char * function,
    NvU32        debuglevel,
    const char * printf_format,
    NvU32      * pPrefix
)
{
    return NVRM_MSG_HIDE;
}

#endif // #if NV_PRINTF_STRINGS_ALLOWED

/*!
 * @brief Does a byte by byte dump of the buffer passed.
 *
 * @param[in]   pBuffer     Pointer to the buffer to dump.
 * @param[in]   length      Length of the buffer to dump (in # of bytes).
 */
void
nvDbgDumpBufferBytes
(
    void *pBuffer,
    NvU32 length
)
{
    NvU8   *s              =  (NvU8 *)pBuffer;
    NvU32   remainingBytes = length % 16;
    NvU32   i;

    NV_PRINTF(LEVEL_ERROR,
              "                  x0  x1  x2  x3  x4  x5  x6  x7    x8  x9  xa  xb  xc  xd  xe  xf\n");

    for (i = 0; i < (length / 16); i++)
    {

        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9], s[10], s[11], s[12], s[13], s[14], s[15]);

        s += 16;
    }

    /*
     * 16 statement switch, so that these are added to nvlog correctly.
     */
    switch (remainingBytes)
    {
    case 0:
    default:
        break;
    case 1:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  ..  ..  ..  ..  ..  ..  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0]);
        break;
    case 2:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  ..  ..  ..  ..  ..  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1]);
        break;
    case 3:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  ..  ..  ..  ..  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2]);
        break;
    case 4:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  ..  ..  ..  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3]);
        break;
    case 5:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  ..  ..  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4]);
        break;
    case 6:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  ..  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5]);
        break;
    case 7:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  ..    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6]);
        break;
    case 8:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    ..  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
        break;
    case 9:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  ..  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8]);
        break;
    case 10:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  ..  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9]);
        break;
    case 11:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  %02x  ..  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9], s[10]);
        break;
    case 12:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  %02x  %02x  ..  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9], s[10], s[11]);
        break;
    case 13:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  %02x  %02x  %02x  ..  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9], s[10], s[11], s[12]);
        break;
    case 14:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  %02x  %02x  %02x  %02x  ..  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9], s[10], s[11], s[12], s[13]);
        break;
    case 15:
        NV_PRINTF(LEVEL_ERROR,
                  "%p  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x    %02x  %02x  %02x  %02x  %02x  %02x  %02x  ..\n",
                  s, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                  s[9], s[10], s[11], s[12], s[13], s[14]);
        break;
    }
}

