/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"

#if defined(CONFIG_CRAY_XT)
enum {
    NV_FORMAT_STATE_ORDINARY,
    NV_FORMAT_STATE_INTRODUCTION,
    NV_FORMAT_STATE_FLAGS,
    NV_FORMAT_STATE_FIELD_WIDTH,
    NV_FORMAT_STATE_PRECISION,
    NV_FORMAT_STATE_LENGTH_MODIFIER,
    NV_FORMAT_STATE_CONVERSION_SPECIFIER
};

enum {
    NV_LENGTH_MODIFIER_NONE,
    NV_LENGTH_MODIFIER_CHAR,
    NV_LENGTH_MODIFIER_SHORT_INT,
    NV_LENGTH_MODIFIER_LONG_INT,
    NV_LENGTH_MODIFIER_LONG_LONG_INT
};

#define NV_IS_FLAG(c) \
    ((c) == '#' || (c) == '0' || (c) == '-' || (c) == ' ' || (c) == '+')
#define NV_IS_LENGTH_MODIFIER(c) \
    ((c) == 'h' || (c) == 'l' || (c) == 'L' || (c) == 'q' || (c) == 'j' || \
     (c) == 'z' || (c) == 't')
#define NV_IS_CONVERSION_SPECIFIER(c) \
    ((c) == 'd' || (c) == 'i' || (c) == 'o' || (c) == 'u' || (c) == 'x' || \
     (c) == 'X' || (c) == 'e' || (c) == 'E' || (c) == 'f' || (c) == 'F' || \
     (c) == 'g' || (c) == 'G' || (c) == 'a' || (c) == 'A' || (c) == 'c' || \
     (c) == 's' || (c) == 'p')

#define NV_MAX_NUM_INFO_MMRS 6

NV_STATUS nvos_forward_error_to_cray(
    struct pci_dev *dev,
    NvU32           error_number,
    const char     *format,
    va_list         ap
)
{
    NvU32 num_info_mmrs;
    NvU64 x = 0, info_mmrs[NV_MAX_NUM_INFO_MMRS];
    int state = NV_FORMAT_STATE_ORDINARY;
    int modifier = NV_LENGTH_MODIFIER_NONE;
    NvU32 i, n = 0, m = 0;

    memset(info_mmrs, 0, sizeof(info_mmrs));
    while (*format != '\0')
    {
        switch (state)
        {
            case NV_FORMAT_STATE_ORDINARY:
                if (*format == '%')
                    state = NV_FORMAT_STATE_INTRODUCTION;
                break;
            case NV_FORMAT_STATE_INTRODUCTION:
                if (*format == '%')
                {
                    state = NV_FORMAT_STATE_ORDINARY;
                    break;
                }
            case NV_FORMAT_STATE_FLAGS:
                if (NV_IS_FLAG(*format))
                {
                    state = NV_FORMAT_STATE_FLAGS;
                    break;
                }
                else if (*format == '*')
                {
                    state = NV_FORMAT_STATE_FIELD_WIDTH;
                    break;
                }
            case NV_FORMAT_STATE_FIELD_WIDTH:
                if ((*format >= '0') && (*format <= '9'))
                {
                    state = NV_FORMAT_STATE_FIELD_WIDTH;
                    break;
                }
                else if (*format == '.')
                {
                    state = NV_FORMAT_STATE_PRECISION;
                    break;
                }
            case NV_FORMAT_STATE_PRECISION:
                if ((*format >= '0') && (*format <= '9'))
                {
                    state = NV_FORMAT_STATE_PRECISION;
                    break;
                }
                else if (NV_IS_LENGTH_MODIFIER(*format))
                {
                    state = NV_FORMAT_STATE_LENGTH_MODIFIER;
                    break;
                }
                else if (NV_IS_CONVERSION_SPECIFIER(*format))
                {
                    state = NV_FORMAT_STATE_CONVERSION_SPECIFIER;
                    break;
                }
            case NV_FORMAT_STATE_LENGTH_MODIFIER:
                if ((*format == 'h') || (*format == 'l'))
                {
                    state = NV_FORMAT_STATE_LENGTH_MODIFIER;
                    break;
                }
                else if (NV_IS_CONVERSION_SPECIFIER(*format))
                {
                    state = NV_FORMAT_STATE_CONVERSION_SPECIFIER;
                    break;
                }
        }
        switch (state)
        {
            case NV_FORMAT_STATE_INTRODUCTION:
                modifier = NV_LENGTH_MODIFIER_NONE;
                break;
            case NV_FORMAT_STATE_LENGTH_MODIFIER:
                switch (*format)
                {
                    case 'h':
                        modifier = (modifier == NV_LENGTH_MODIFIER_NONE)
                            ? NV_LENGTH_MODIFIER_SHORT_INT
                            : NV_LENGTH_MODIFIER_CHAR;
                        break;
                    case 'l':
                        modifier = (modifier == NV_LENGTH_MODIFIER_NONE)
                            ? NV_LENGTH_MODIFIER_LONG_INT
                            : NV_LENGTH_MODIFIER_LONG_LONG_INT;
                        break;
                    case 'q':
                        modifier = NV_LENGTH_MODIFIER_LONG_LONG_INT;
                    default:
                        return NV_ERR_INVALID_ARGUMENT;
                }
                break;
            case NV_FORMAT_STATE_CONVERSION_SPECIFIER:
                switch (*format)
                {
                    case 'c':
                    case 'd':
                    case 'i':
                        x = (unsigned int)va_arg(ap, int);
                        break;
                    case 'o':
                    case 'u':
                    case 'x':
                    case 'X':
                        switch (modifier)
                        {
                            case NV_LENGTH_MODIFIER_LONG_LONG_INT:
                                x = va_arg(ap, unsigned long long int);
                                break;
                            case NV_LENGTH_MODIFIER_LONG_INT:
                                x = va_arg(ap, unsigned long int);
                                break;
                            case NV_LENGTH_MODIFIER_CHAR:
                            case NV_LENGTH_MODIFIER_SHORT_INT:
                            case NV_LENGTH_MODIFIER_NONE:
                                x = va_arg(ap, unsigned int);
                                break;
                        }
                        break;
                    default:
                        return NV_ERR_INVALID_ARGUMENT;
                }
                state = NV_FORMAT_STATE_ORDINARY;
                for (i = 0; i < ((modifier == NV_LENGTH_MODIFIER_LONG_LONG_INT)
                                ? 2 : 1); i++)
                {
                    if (m == NV_MAX_NUM_INFO_MMRS)
                        return NV_ERR_INSUFFICIENT_RESOURCES;
                    info_mmrs[m] = ((info_mmrs[m] << 32) | (x & 0xffffffff));
                    x >>= 32;
                    if (++n == 2)
                    {
                        m++;
                        n = 0;
                    }
                }
        }
        format++;
    }

    num_info_mmrs = (m + (n != 0));
    if (num_info_mmrs > 0)
        cray_nvidia_report_error(dev, error_number, num_info_mmrs, info_mmrs);

    return NV_OK;
}
#endif
