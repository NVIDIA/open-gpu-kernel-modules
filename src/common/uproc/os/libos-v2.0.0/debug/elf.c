/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifdef NVRM

#    include <nvport/nvport.h>
#    define memcpy(d, s, l)    portMemCopy(d, l, s, l)
#    define strcmp(str1, str2) portStringCompare(str1, str2, 0x1000)

#else // NVRM

#    include <memory.h>
#    include <stdio.h>
#    include <string.h>

#endif // NVRM

#include "elf.h"
#include "nvtypes.h"

/**
 *
 * @brief Find the start and end of the ELF section in memory
 *        from the section name.
 *
 * @param[in] elf
 * @param[in] sectionName
 *   The section to find such as .text or .data
 * @param[out] start, end
 *   The start and end of the section in the loaded ELF file
 *   e.g   validPtr >= start && validPtr < end
 * @param[out] va_baase
 *   The virtual address this section is loaded at
 */
NvBool
libosElfFindSectionByName(elf64_header *elf, const char *targetName, NvU8 **start, NvU8 **end, NvU64 *va_base)
{
    elf64_shdr *shdr        = (elf64_shdr *)(((char *)elf) + elf->shoff);
    const char *string_base = ((char *)elf) + shdr[elf->shstrndx].offset;
    NvU32 i;

    for (i = 0; i < elf->shnum; i++, shdr++)
    {
        const char *name = string_base + shdr->name;
        if (strcmp(name, targetName) == 0)
        {
            *start   = (NvU8 *)elf + shdr->offset;
            *end     = (NvU8 *)elf + shdr->offset + shdr->size;
            *va_base = shdr->addr;
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/**
 *
 * @brief Find the start and end of the ELF section in memory
 *        from the section name.
 *
 * @param[in] elf
 * @param[in] sectionName
 *   The section to find such as .text or .data
 * @param[out] start, end
 *   The start and end of the section in the loaded ELF file
 *   e.g   validPtr >= start && validPtr < end
 * @param[out] va_base
 *   The virtual address this section is loaded at
 */
NvBool
libosElfFindSectionByAddress(elf64_header *elf, NvU64 address, NvU8 **start, NvU8 **end, NvU64 *va_base)
{
    elf64_shdr *shdr = (elf64_shdr *)(((char *)elf) + elf->shoff);
    NvU32 i;

    for (i = 0; i < elf->shnum; i++, shdr++)
    {
        if (address >= shdr->addr && address < (shdr->addr + shdr->size))
        {
            *start   = (NvU8 *)elf + shdr->offset;
            *end     = (NvU8 *)elf + shdr->offset + shdr->size;
            *va_base = shdr->addr;
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/**
 *
 * @brief Reads an arbitrary sized block of memory by loaded VA through
 *        the ELF. This can be used to read data from the perspective
 *        of a processor who has loaded the ELF.
 *
 * @param[in] elf
 * @param[in] address
 *   The absolute virtual address to read
 * @param[out] size
 *   The number of bytes that must be valid.
 * @returns
 *   The pointer to the data in question, or NULL if the operation failed)
 */
void *libosElfReadVirtual(elf64_header *elf, NvU64 address, NvU64 size)
{
    NvU8 *start, *end;
    NvU64 va_base;
    NvU64 section_offset;
    NvU64 section_offset_tail;

    // @todo This really should be using the PHDR as theoretically section headers
    //       might be stripped
    if (!libosElfFindSectionByAddress(elf, address, &start, &end, &va_base))
        return 0;

    section_offset = address - va_base;

    // Compute section offset (overflow check)
    section_offset_tail = section_offset + size;
    if (section_offset_tail < section_offset)
        return 0;

    // Bounds check the tail
    if (section_offset_tail > (NvLength)(end - start))
        return 0;

    return (address - va_base) + start;
}

/**
 *
 * @brief Reads an arbitrary length string by loaded VA through
 *        the ELF. This can be used to read data from the perspective
 *        of a processor who has loaded the ELF.
 *
 * @param[in] elf
 * @param[in] address
 *   The absolute virtual address to read
 * @returns
 *   The pointer to the data in question, or NULL if the operation failed)
 *   Ensures that all bytes of the string lie within the ELF same section.
 */
const char *libosElfReadStringVirtual(elf64_header *elf, NvU64 address)
{
    NvU8 *start, *end;
    NvU64 base;
    NvU8 *region;
    NvU8 *i;

    // @todo This really should be using the PHDR as theoretically section headers
    //       might be stripped
    if (!libosElfFindSectionByAddress(elf, address, &start, &end, &base))
        return 0;

    region = (address - base) + start;
    i      = region;

    while (i >= start && i < end)
    {
        if (!*i)
            return (const char *)region;
        i++;
    }

    return 0;
}
