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

#ifndef LIBELF_H_
#define LIBELF_H_
#include <nvtypes.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "libos_status.h"

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
} LibosElf64Header;

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
} LibosElf64ProgramHeader;

#define PF_X 1
#define PF_W 2
#define PF_R 4

#define PT_LOAD    1
#define PT_NOTE    4
#define PT_PHDR    6
#define PT_SHLIB   5
#define PT_DYNAMIC 2
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
} LibosElf64SectionHeader;

#define SHT_NOBITS   8
#define SHT_PROGBITS 1

typedef struct
{
    NvU32 name;
    NvU8 info;
    NvU8 other;
    NvU16 shndx;
    NvU64 value;
    NvU64 size;
} LibosElf64Symbol;

typedef struct {
  NvS64 tag; 
  NvU64 ptr;
} LibosElf64Dynamic;

typedef struct LibosElfImage{
    LibosElf64Header * elf;
    NvU64              size;
    void *            (*map)(struct LibosElfImage *image, NvU64 offset, NvU64 size);
} LibosElfImage;

LibosStatus               LibosElfImageConstruct(LibosElfImage * image, void * elf, NvU64 size);

// Program headers
LibosElf64ProgramHeader * LibosElfProgramHeaderForRange(LibosElfImage * image, NvU64 va, NvU64 vaLastByte);
LibosElf64ProgramHeader * LibosElfProgramHeaderNext(LibosElfImage * image, LibosElf64ProgramHeader * previous);

// Section headers
LibosElf64SectionHeader * LibosElfFindSectionByName(LibosElfImage * image, const char *targetName);
LibosStatus               LibosElfMapSection(LibosElfImage * image, LibosElf64SectionHeader * shdr, NvU8 ** start, NvU8 ** end);
LibosElf64SectionHeader * LibosElfFindSectionByAddress(LibosElfImage * image, NvU64 address);
LibosElf64SectionHeader * LibosElfSectionHeaderNext(LibosElfImage * image, LibosElf64SectionHeader * previous);

// Dynamic linker
LibosElf64ProgramHeader * LibosElfHeaderDynamic(LibosElfImage * image);
LibosElf64Dynamic * LibosElfDynamicEntryNext(LibosElfImage * image, LibosElf64ProgramHeader * dynamicTableHeader, LibosElf64Dynamic * previous);

// VA based access
void *                    LibosElfMapVirtual(LibosElfImage * image, NvU64 address, NvU64 size);
const char *              LibosElfMapVirtualString(LibosElfImage * image, NvU64 address, NvBool checkShdrs);

// Libos Tiny entry
NvBool                    LibosTinyElfGetBootEntry(LibosElfImage * image, NvU64 * physicalEntry);

NvBool                    LibosElfCommitData(LibosElfImage * image, NvU64 commitVirtualAddress, NvU64 commitSize);

#ifdef __cplusplus
}
#endif

#endif
