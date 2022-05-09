/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GSP_ELF_H_
#define GSP_ELF_H_

#include <nvtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

// Structures for Loader
typedef struct
{
    NvU8 ident[16];
    NvU16 type;
    NvU16 machine;
    NvU32 version;
    NvU64 entry;
    NvU64 phoff;
    NvU64 shoff;
    NvU32 flags;
    NvU16 ehsize;
    NvU16 phentsize;
    NvU16 phnum;
    NvU16 shentsize;
    NvU16 shnum;
    NvU16 shstrndx;
} elf64_header;

typedef struct
{
    NvU32 type;
    NvU32 flags;
    NvU64 offset;
    NvU64 vaddr;
    NvU64 paddr;
    NvU64 filesz;
    NvU64 memsz;
    NvU64 align;
} elf64_phdr;

#define PF_X 1
#define PF_W 2
#define PF_R 4

#define PT_LOAD 1

// Structures for finding Symbols
typedef struct
{
    NvU32 name;
    NvU32 type;
    NvU64 flags;
    NvU64 addr;
    NvU64 offset;
    NvU64 size;
    NvU32 link;
    NvU32 info;
    NvU64 addralign;
    NvU64 entsize;
} elf64_shdr;

typedef struct
{
    NvU32 name;
    NvU8 info;
    NvU8 other;
    NvU16 shndx;
    NvU64 value;
    NvU64 size;
} elf64_sym;

// Core ELF API
NvBool libosElfFindSectionByName(
    elf64_header *elf, const char *sectionName, NvU8 **start, NvU8 **end, NvU64 *va_base);
NvBool
libosElfFindSectionByAddress(elf64_header *elf, NvU64 address, NvU8 **start, NvU8 **end, NvU64 *va_base);
void *libosElfReadVirtual(elf64_header *elf, NvU64 address, NvU64 size);
const char *libosElfReadStringVirtual(elf64_header *elf, NvU64 address);

#ifdef __cplusplus
}
#endif

#endif
