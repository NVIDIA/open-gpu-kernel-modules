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
#include <nvctassert.h>
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
    NvU32 entry;
    NvU32 phoff;
    NvU32 shoff;
    NvU32 flags;
    NvU16 ehsize;
    NvU16 phentsize;
    NvU16 phnum;
    NvU16 shentsize;
    NvU16 shnum;
    NvU16 shstrndx;
} LibosElf32Header;

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

#define ELFCLASS32 1
#define ELFCLASS64 2

typedef struct
{
    NvU32 type;
    NvU32 offset;
    NvU32 vaddr;
    NvU32 paddr;
    NvU32 filesz;
    NvU32 memsz;
    NvU32 flags;
    NvU32 align;
} LibosElf32ProgramHeader;

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

typedef union
{
    LibosElf32ProgramHeader * phdr32;
    LibosElf64ProgramHeader * phdr64;
    NvU8                    * raw;
} LibosElfProgramHeaderPtr;

#define PF_X 1
#define PF_W 2
#define PF_R 4

#define PT_LOAD    1
#define PT_NOTE    4
#define PT_PHDR    6
#define PT_TLS     7
#define PT_SHLIB   5
#define PT_DYNAMIC 2

// Structures for finding Symbols
typedef struct
{
    NvU32 name;
    NvU32 type;
    NvU32 flags;
    NvU32 addr;
    NvU32 offset;
    NvU32 size;
    NvU32 link;
    NvU32 info;
    NvU32 addralign;
    NvU32 entsize;
} LibosElf32SectionHeader;

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

typedef union
{
    LibosElf32SectionHeader * shdr32;
    LibosElf64SectionHeader * shdr64;
    NvU8                    * raw;
} LibosElfSectionHeaderPtr;

#define SHT_NOBITS   8
#define SHT_PROGBITS 1

// TODO
typedef struct
{
    NvU32 name;
    NvU32 value;
    NvU32 size;
    NvU8 info;
    NvU8 other;
    NvU16 shndx;
} LibosElf32Symbol;

typedef struct
{
    NvU32 name;
    NvU8 info;
    NvU8 other;
    NvU16 shndx;
    NvU64 value;
    NvU64 size;
} LibosElf64Symbol;

typedef struct LibosElfImage
{
    union
    {
        LibosElf32Header * elf32;
        LibosElf64Header * elf64;
        NvU8             * raw;
    };
    NvU64              size;
    void *            (*map)(struct LibosElfImage *image, NvU64 offset, NvU64 size);
} LibosElfImage;

typedef struct
{
    NvU32 namesz; /* name size */
    NvU32 descsz; /* descriptor size */
    NvU32 type; /* descriptor type */
    NvU8 data[0]; /* name + descriptor*/
} LibosElfNoteHeader;

LibosStatus               LibosElfImageConstruct(LibosElfImage * image, void * elf, NvU64 size);
NvU8                      LibosElfGetClass(const LibosElfImage * image);

// Program headers
LibosElfProgramHeaderPtr LibosElfProgramHeaderForRange(LibosElfImage * image, NvU64 va, NvU64 vaLastByte);
LibosElfProgramHeaderPtr  LibosElfProgramHeaderNext(LibosElfImage * image, LibosElfProgramHeaderPtr previous);

// Section headers
LibosElfSectionHeaderPtr LibosElfFindSectionByName(LibosElfImage * image, const char *targetName);
LibosStatus               LibosElfMapSection(LibosElfImage * image, LibosElfSectionHeaderPtr shdr, NvU8 ** start, NvU8 ** end);
LibosElfSectionHeaderPtr LibosElfFindSectionByAddress(LibosElfImage * image, NvU64 address);

LibosElfSectionHeaderPtr  LibosElfSectionHeaderNext(LibosElfImage * image, LibosElfSectionHeaderPtr previous);

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
