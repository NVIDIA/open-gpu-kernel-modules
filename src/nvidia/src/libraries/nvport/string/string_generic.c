/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief STRING module implementation for platforms without stdlib support
 */

#include "nvport/nvport.h"
#include "nvmisc.h"


#ifndef NVPORT_STRING_DONT_DEFINE_portStringLength
NvLength
portStringLength
(
    const char *str
)
{
    const char *begin = str;

    PORT_ASSERT_CHECKED(str != NULL);

    while ('\0' != *str) str++;

    return str - begin;
}

#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringLengthSafe
NvLength
portStringLengthSafe
(
    const char *str,
    NvLength maxLength
)
{
    const char *begin = str;

    PORT_ASSERT_CHECKED(str != NULL);

    while ((0 != maxLength--) && ('\0' != *str))
        str++;

    return str - begin;
}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringCompare
NvS32
portStringCompare
(
    const char *str1,
    const char *str2,
    NvLength maxLength
)
{
    NvLength i;

    PORT_ASSERT_CHECKED(str1 != NULL);
    PORT_ASSERT_CHECKED(str2 != NULL);

    for (i = 0; i < maxLength; i++)
    {
        if (str1[i] != str2[i])
        {
            //
            // Cast to unsigned before assigning to NvS32, to avoid sign
            // extension.  E.g., if str1[i] is 0xff, we want s1 to contain
            // 0xff, not -1.  In practice, this shouldn't matter for printable
            // characters, but still...
            //
            NvS32 s1 = (unsigned char)str1[i];
            NvS32 s2 = (unsigned char)str2[i];
            return s1 - s2;
        }

        if ((str1[i] == '\0') &&
            (str2[i] == '\0'))
        {
            break;
        }
    }

    return 0;
}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringCopy
NvLength
portStringCopy
(
    char *dest,
    NvLength destSize,
    const char *src,
    NvLength srcSize
)
{
    NvLength minCopyLength;
    NvLength srcLen;

    PORT_ASSERT_CHECKED(dest != NULL);
    PORT_ASSERT_CHECKED(src != NULL);

    PORT_ASSERT_CHECKED(destSize != 0);
    PORT_ASSERT_CHECKED(srcSize != 0);

    srcLen = portStringLengthSafe(src, srcSize);
    if (srcLen == srcSize) srcLen--;

    minCopyLength = NV_MIN(destSize, srcLen + 1);

    PORT_ASSERT_CHECKED(minCopyLength != 0);

    if (minCopyLength > 1)
        portMemCopy(dest, destSize, src, minCopyLength - 1);

    dest[minCopyLength - 1] = '\0';

    return minCopyLength;
}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringCat
char *
portStringCat
(
    char *str,
    NvLength strSize,
    const char *cat,
    NvLength catSize
)
{
    NvLength strLen;
    NvLength catLen;
    NvLength minCatLength;
    char* begin;

    PORT_ASSERT_CHECKED(str != NULL);
    PORT_ASSERT_CHECKED(cat != NULL);

    strLen = portStringLengthSafe(str, strSize);
    catLen = portStringLengthSafe(cat, catSize);

    // In case of no NULL terminating char in cat.
    if (catLen == catSize) catLen--;

    minCatLength = NV_MIN(strSize - strLen, catLen + 1);
    if (0 == minCatLength)
        return str;

    begin = str;
    str = str + strLen;

    // strncat doesn't count NULL char.
    if (minCatLength > 1)
       portMemCopy(str, strSize, cat, minCatLength - 1);

    begin[strLen + minCatLength - 1] = '\0';
    return begin;
}

#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringConvertAsciiToUtf16
NvLength
portStringConvertAsciiToUtf16
(
    NvU16 *dest,
    NvLength destSize,
    const char *src,
    NvLength srcSize
)
{
    NvLength i, len;

    PORT_ASSERT_CHECKED(dest != NULL);
    PORT_ASSERT_CHECKED(src != NULL);

    if (destSize == 0)
        return 0;

    len = portStringLengthSafe(src, srcSize);
    if (len >= destSize)
        len = destSize - 1;

    i = len;
    while (i-- > 0)
        dest[i] = src[i];

    dest[len] = 0;
    return len;
}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringBufferToHex
NvLength
portStringBufferToHex
(
    char *str,
    NvLength strSize,
    const NvU8 *buf,
    NvLength bufSize
)
{
    NvLength i, len;

    if (strSize == 0)
        return 0;

    PORT_ASSERT_CHECKED(str != NULL);
    PORT_ASSERT_CHECKED(buf != NULL);

    len = bufSize * 2;
    if (len >= strSize)
        len = strSize - 1;

    for (i = 0; i < len; i++)
    {
        NvU8 n = (i % 2) ? (buf[i/2] & 0xF) : (buf[i/2] >> 4);
        str[i] = (n < 0xA) ? ('0' + n) : ('a' + n - 0xA);
    }
    str[len] = 0;
    return len;
}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringBufferToHexGroups
NvLength
portStringBufferToHexGroups
(
    char *str,
    NvLength strSize,
    const NvU8 *buf,
    NvLength bufSize,
    NvLength groupCount,
    const NvU32 *groups,
    const char *separator
)
{
    NvLength group, sepLength, written;

    if (strSize == 0)
        return 0;

    PORT_ASSERT_CHECKED(str != NULL);
    PORT_ASSERT_CHECKED(buf != NULL);
    PORT_ASSERT_CHECKED(groups != NULL);
    PORT_ASSERT_CHECKED(separator != NULL);

    sepLength = portStringLength(separator);

    for (written = 0, group = 0; (group < groupCount) && (written < (strSize - 1)); group++)
    {
        NvLength groupSize = NV_MIN(groups[group] / 2, bufSize);
        written += portStringBufferToHex(str + written, strSize - written, buf, groupSize);
        buf += groupSize;
        bufSize -= groupSize;

        if (group != groupCount - 1)
        {
            portMemCopy(str + written, strSize - written, separator, sepLength);
            written += sepLength;
        }
    }

    str[written] = 0;
    return written;
}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringTok
static const char *portStringChr(const char *s, int c)
{
    while (s && *s != 0 && *s != c)
    {
        s++;
    }

    if (*s == 0)
    {
        return NULL;
    }

    return s;
}

char *portStringTok(char *str, const char *delim, char **saveptr)
{
    char *cp, *start;
    start = cp = str ? str : *saveptr;

    if (cp == NULL)
    {
        return NULL;
    }

    while (*cp && !portStringChr(delim, *cp))
    {
        ++cp;
    }

    if (!*cp)
    {
        if (cp == start)
        {
            return NULL;
        }
        *saveptr = NULL;
        return start;
    }
    else
    {
        *cp++ = '\0';
        *saveptr = cp;
        return start;
    }

}
#endif

#ifndef NVPORT_STRING_DONT_DEFINE_portStringStrStr
char *portStringStrStr(char *str, char *substr)
{
    char* ptr;

    ptr = str;

    while (*ptr)
    {
        if (portStringCompare(ptr, substr, portStringLength(substr)) == 0)
        {
            return ptr;
        }
        ptr++;
    }
    return NULL;
}
#endif
