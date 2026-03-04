/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#    define strcmp(str1, str2) portStringCompare(str1, str2, 0x1000)
#    define strncmp(str1, str2, size) portStringCompare(str1, str2, size)
#else // NVRM
#    include <memory.h>
#    include <stdio.h>
#    include <string.h>
#endif // NVRM

#include "libelf.h"
#include <nvtypes.h>
#include <nvmisc.h>

ct_assert(sizeof(LibosElf32Header) == 52);
ct_assert(sizeof(LibosElf64Header) == 64);

ct_assert(sizeof(LibosElf32ProgramHeader) == 32);
ct_assert(sizeof(LibosElf64ProgramHeader) == 56);

static void * elfSimpleAccess(LibosElfImage * image, NvU64 offset, NvU64 size)
{
    if ((offset + size) <= image->size)
        return image->raw + offset;
    return 0;
}

/**
 *
 * @brief Initialize an elf image to point to an unloaded elf.
 *
 * @param[in] image
 *  The image structure to initialize with the provided elf.
 * @param[in] elf
 * @param[in] size
 */
LibosStatus LibosElfImageConstruct(LibosElfImage * image, void * elf, NvU64 size)
{
    if (size < sizeof(LibosElf64Header))
        return LibosErrorIncomplete;

#ifdef LIBOS_TOOL_BAKE
    image->elf64 = malloc(size);
    memcpy(image->elf64, elf, size);
#else
    image->raw = elf;
#endif

    if (LibosElfGetClass(image) != ELFCLASS32 &&
        LibosElfGetClass(image) != ELFCLASS64)
    {
        return LibosErrorArgument;
    }

    image->size = size;
    image->map = elfSimpleAccess;

    return LibosOk;
}

#define EI_CLASS 4

NvU8 LibosElfGetClass(const LibosElfImage * image)
{
    ct_assert(NV_OFFSETOF(LibosElf32Header, ident) == NV_OFFSETOF(LibosElf64Header, ident));
    ct_assert(sizeof(image->elf32->ident) == sizeof(image->elf64->ident));

    return image->elf64->ident[EI_CLASS];
}

/**
 *
 * @brief Returns the offset from the start of the ELF corresponding
 *        to the entry point.
 *
 *        The ELF should be copied directly into memory without modification.
 *        The packaged LIBOS elf contains a self bootstrapping ELF loader.
 *
 *        During boot:
 *            - PHDRs are copied into memory descriptors
 *            - Copy on write sections are duplicated (into additional heap above)
 *
 * @param[in] elf
 * @returns false if not a recognized LIBOS ELF.
 */
NvBool LibosTinyElfGetBootEntry(LibosElfImage * image, NvU64 * physicalEntry)
{
    NvU64 entryVa;

    if (LibosElfGetClass(image) != ELFCLASS64)
    {
        return NV_FALSE;
    }

    entryVa = image->elf64->entry;

    // Find the phdr containing the entry point
    LibosElfProgramHeaderPtr phdr = LibosElfProgramHeaderForRange(image, entryVa, entryVa);
    if (phdr.raw != NULL)
    {
        *physicalEntry = phdr.phdr64->paddr + entryVa - phdr.phdr64->vaddr;
        return NV_TRUE;
    }

    return NV_FALSE;
}

/**
 *
 * @brief Iterates Program Headers
 *
 *  Program headers are loadable regions of the ELF.
 *  Each header may contain any number of sections.
 *
 * @param[in] image
 * @param[in] previous
 *   Previous returned program header. Pass null to get first header.
 */
LibosElfProgramHeaderPtr LibosElfProgramHeaderNext(LibosElfImage * image, LibosElfProgramHeaderPtr previous)
{
    NvU8 *phdrTable = NULL, * phdrTableEnd = NULL;
    NvU64 phoff = 0, phnum = 0;
    size_t phdrSz = 0;

    switch (LibosElfGetClass(image))
    {
        case ELFCLASS32:
        {
            phoff = image->elf32->phoff;
            phnum = image->elf32->phnum;
            phdrSz = sizeof(LibosElf32ProgramHeader);
            break;
        }

        case ELFCLASS64:
        {
            phoff = image->elf64->phoff;
            phnum = image->elf64->phnum;
            phdrSz = sizeof(LibosElf64ProgramHeader);
            break;
        }

        default:
            return (LibosElfProgramHeaderPtr){NULL};
    }

    phdrTable = image->map(image, phoff, phdrSz * phnum);
    phdrTableEnd = phdrTable + phdrSz * phnum;

    NvU8 * next;
    if (previous.phdr32 == NULL)
        next = phdrTable;
    else
        next = previous.raw + phdrSz;

    if (next == phdrTableEnd)
        return (LibosElfProgramHeaderPtr){0};

    return (LibosElfProgramHeaderPtr){.raw = next};
}

/**
 *
 * @brief Iterates Sections
 *
 *
 * @param[in] image
 * @param[in] previous
 *   Previous returned section header. Pass null to get first header.
 */
LibosElfSectionHeaderPtr LibosElfSectionHeaderNext(LibosElfImage * image, LibosElfSectionHeaderPtr previous)
{
    NvU8 *shdrTable = NULL, * shdrTableEnd = NULL;
    NvU64 shoff = 0, shnum = 0;
    size_t shdrSz = 0;

    switch (LibosElfGetClass(image))
    {
        case ELFCLASS32:
        {
            shoff = image->elf32->shoff;
            shnum = image->elf32->shnum;
            shdrSz = sizeof(LibosElf32SectionHeader);
            break;
        }

        case ELFCLASS64:
        {
            shoff = image->elf64->shoff;
            shnum = image->elf64->shnum;
            shdrSz = sizeof(LibosElf64SectionHeader);
            break;
        }

        default:
            return (LibosElfSectionHeaderPtr){NULL};
    }


    shdrTable = image->map(image, shoff, shdrSz * shnum);
    shdrTableEnd = shdrTable + shdrSz * shnum;

    NvU8 * next;
    if (previous.raw == NULL)
        next = shdrTable;
    else
        next = previous.raw + shdrSz;

    if (next == shdrTableEnd)
        return (LibosElfSectionHeaderPtr){NULL};

    return (LibosElfSectionHeaderPtr){.raw = next};
}

/**
 *
 * @brief Locates the program header containing this region of virtual addresses.
 *
 * @param[in] image
 * @param[in] va
 * @param[in] vaLastByte
 *   The program header must contain all bytes in the range [va, vaLastByte] inclusive.
 */
LibosElfProgramHeaderPtr LibosElfProgramHeaderForRange(LibosElfImage * image, NvU64 va, NvU64 vaLastByte)
{
    LibosElfProgramHeaderPtr phdr;
    for (phdr = LibosElfProgramHeaderNext(image, (LibosElfProgramHeaderPtr){NULL});
         phdr.raw != NULL;
         phdr = LibosElfProgramHeaderNext(image, phdr))
    {
        // Is this memory region within this PHDR?
        if (LibosElfGetClass(image) == ELFCLASS32 &&
            va >= phdr.phdr32->vaddr && (vaLastByte - phdr.phdr32->vaddr) < phdr.phdr32->memsz)
        {
            return phdr;
        }

        if (LibosElfGetClass(image) == ELFCLASS64 &&
            va >= phdr.phdr64->vaddr && (vaLastByte - phdr.phdr64->vaddr) < phdr.phdr64->memsz)
        {
            return phdr;
        }
    }

    return (LibosElfProgramHeaderPtr){NULL};
}

/**
 *
 * @brief Find the start and end of the ELF section in memory
 *        from the section name.
 *
 * @param[in] image
 * @param[in] sectionName
 *   The section to find such as .text or .data
 */
LibosElfSectionHeaderPtr LibosElfFindSectionByName(LibosElfImage * image, const char *targetName)
{
    // @todo: Enumerate, and introduce function for kth section header
    NvU32 i = 0;
    LibosElfSectionHeaderPtr shdr;
    NvU32 shstrndx, shnum;
    const char *shstr = NULL;
    size_t      shstrSize = 0;

    shdr.raw = NULL;

    switch (LibosElfGetClass(image))
    {
        case ELFCLASS32:
        {
            shstrndx = image->elf32->shstrndx;
            shnum = image->elf32->shnum;
            break;
        }

        case ELFCLASS64:
        {
            shstrndx = image->elf64->shstrndx;
            shnum = image->elf64->shnum;
            break;
        }

        default:
        {
            return (LibosElfSectionHeaderPtr){NULL};
        }
    }

    if (shstrndx >= shnum)
        return (LibosElfSectionHeaderPtr){NULL};

    for (shdr = LibosElfSectionHeaderNext(image, shdr); shdr.shdr32 != NULL; shdr = LibosElfSectionHeaderNext(image, shdr), i++)
    {
        const char * name = NULL;
        NvU32 shdrName = 0;

        if (LibosElfGetClass(image) == ELFCLASS32)
        {
            if (i == 0)
            {
                shstr = (const char *) image->map(image, shdr.shdr32[image->elf32->shstrndx].offset, shdr.shdr32[image->elf32->shstrndx].size);
                shstrSize = shdr.shdr32[shstrndx].size;
            }

            shdrName = shdr.shdr32->name;
        }
        else if (LibosElfGetClass(image) == ELFCLASS64)
        {
            if (i == 0)
            {
                shstr = (const char *) image->map(image, shdr.shdr64[image->elf64->shstrndx].offset, shdr.shdr64[image->elf64->shstrndx].size);
                shstrSize = shdr.shdr64[shstrndx].size;
            }

            shdrName = shdr.shdr64->name;
        }

        if (shdrName >= shstrSize)
        {
            return (LibosElfSectionHeaderPtr){NULL};
        }

        name = shstr + shdrName;
        if (strncmp(name, targetName, shstrSize - shdrName) == 0)
        {
            return shdr;
        }
    }

    return (LibosElfSectionHeaderPtr){NULL};
}

/**
 *
 * @brief Find the start and end of the loaded section.
 *
 * @returns
 *  LibosErrorIncomplete if there are no bits assigned to the section
 * @param[in] image
 * @param[in] shdr
 *   The section header to find the start and end of.
 * @param[out] start, end
 *   The start and end of the section in the loaded ELF file
 *   e.g   validPtr >= start && validPtr < end
 */
LibosStatus LibosElfMapSection(LibosElfImage * image, LibosElfSectionHeaderPtr shdr, NvU8 ** start, NvU8 ** end)
{
    NvU8 * mapping;
    NvU32 type;
    NvU64 addr, size, offset;

    switch (LibosElfGetClass(image))
    {
        case ELFCLASS32:
        {
            type = shdr.shdr32->type;
            addr = shdr.shdr32->addr;
            size = shdr.shdr32->size;
            offset = shdr.shdr32->offset;
            break;
        }

        case ELFCLASS64:
        {
            type = shdr.shdr64->type;
            addr = shdr.shdr64->addr;
            size = shdr.shdr64->size;
            offset = shdr.shdr64->offset;
            break;
        }

        default:
        {
            return LibosErrorFailed;
        }
    }

    if (type == SHT_NOBITS)
        // Try mapping through the PHDR as a last ditch approach
        mapping = LibosElfMapVirtual(image, addr, size);
    else
        mapping = image->map(image, offset, size);

    *start   = mapping;
    *end     = mapping + size;

    return LibosOk;
}

/**
 *
 * @brief Find the start and end of the ELF section in memory
 *        from the section name.
 *
 * @param[in] image
 * @param[in] address
 *   Query the section containing the requested byte
 */
LibosElfSectionHeaderPtr LibosElfFindSectionByAddress(LibosElfImage * image, NvU64 address)
{
    LibosElfSectionHeaderPtr shdr;
    for (shdr = LibosElfSectionHeaderNext(image, (LibosElfSectionHeaderPtr){0});
        shdr.shdr32 != NULL;
        shdr = LibosElfSectionHeaderNext(image, shdr))
    {
        NvU64 shdrAddr, shdrSize;

        switch (LibosElfGetClass(image))
        {
            case ELFCLASS32:
            {
                shdrAddr = shdr.shdr32->addr;
                shdrSize = shdr.shdr32->size;
                break;
            }

            case ELFCLASS64:
            {
                shdrAddr = shdr.shdr64->addr;
                shdrSize = shdr.shdr64->size;
                break;
            }

            default:
            {
                return (LibosElfSectionHeaderPtr){0};
            }
        }

        if (address >= shdrAddr && address < (shdrAddr + shdrSize))
            return shdr;
    }

    return (LibosElfSectionHeaderPtr){0};
}

/**
 *
 * @brief Reads an arbitrary sized block of memory by loaded VA through
 *        the ELF. This can be used to read data from the perspective
 *        of a processor who has loaded the ELF.
 *
 * @param[in] image
 * @param[in] address
 *   The absolute virtual address to read
 * @param[out] size
 *   The number of bytes that must be valid.
 * @returns
 *   The pointer to the data in question, or NULL if the operation failed)
 */
void *LibosElfMapVirtual(LibosElfImage * image, NvU64 address, NvU64 size)
{
    LibosElfProgramHeaderPtr phdr = LibosElfProgramHeaderForRange(image, address, address + size - 1);

    if (phdr.raw != NULL)
    {
        NvU64 vaddr, offset;

        switch (LibosElfGetClass(image))
        {
            case ELFCLASS32:
            {
                vaddr = phdr.phdr32->vaddr;
                offset = phdr.phdr32->offset;
                break;
            }

            case ELFCLASS64:
            {
                vaddr = phdr.phdr64->vaddr;
                offset = phdr.phdr64->offset;
                break;
            }

            default:
            {
                return NULL;
            }
        }

        return image->map(image, address - vaddr + offset, size);
    }

    LibosElfSectionHeaderPtr shdr = LibosElfFindSectionByAddress(image, address);

    if (shdr.raw != NULL)
    {
        NvU64 shdrOffset, shdrSize, shdrAddr;

        switch (LibosElfGetClass(image))
        {
            case ELFCLASS32:
            {
                shdrOffset = shdr.shdr32->offset;
                shdrSize = shdr.shdr32->size;
                shdrAddr = shdr.shdr32->addr;
                break;
            }

            case ELFCLASS64:
            {
                shdrOffset = shdr.shdr64->offset;
                shdrSize = shdr.shdr64->size;
                shdrAddr = shdr.shdr64->addr;
                break;
            }

            default:
            {
                return NULL;
            }
        }

        NvU64 section_end = shdrOffset + shdrSize;
        NvU64 section_offset = address - shdrAddr;

        // Compute section offset (overflow check)
        NvU64 section_offset_tail = section_offset + size;
        if (section_offset_tail < section_offset)
            return NULL;

        // Bounds check the tail
        if (section_offset_tail > (NvLength)(section_end - shdrOffset))
            return NULL;

        return image->map(image, address - shdrAddr + shdrOffset, size);
    }

    return NULL;
}

/**
 *
 * @brief Reads an arbitrary length string by loaded VA through
 *        the ELF. This can be used to read data from the perspective
 *        of a processor who has loaded the ELF.
 *
 * @param[in] image
 * @param[in] address
 *   The virtual address of the string constant.
 * @returns
 *   The string constant. Returns null if the string isn't entirely contained
 *   in the image.
 */
const char *LibosElfMapVirtualString(LibosElfImage * image, NvU64 address, NvBool checkShdrs)
{
    NvU64 stringOffset = 0;
    const char * string = NULL;

    do
    {
        NvU64 offset = 0, vaddr = 0;

        // Ensure the entire string is in the same PHDR
        LibosElfProgramHeaderPtr phdr = LibosElfProgramHeaderForRange(image, address, address + stringOffset);

        if (phdr.raw == NULL && checkShdrs)
        {
            LibosElfSectionHeaderPtr shdr = LibosElfFindSectionByAddress(image, address);

            if (shdr.raw == NULL)
                return NULL;

            switch (LibosElfGetClass(image))
            {
                case ELFCLASS32:
                {
                    offset = shdr.shdr32->offset;
                    vaddr = shdr.shdr32->addr;
                    break;
                }

                case ELFCLASS64:
                {
                    offset = shdr.shdr64->offset;
                    vaddr = shdr.shdr64->addr;
                    break;
                }

                default:
                {
                    return NULL;
                }
            }
        }
        else if (phdr.raw != NULL)
        {
            // Update the mapping
            switch (LibosElfGetClass(image))
            {
                case ELFCLASS32:
                {
                    offset = phdr.phdr32->offset;
                    vaddr = phdr.phdr32->vaddr;
                    break;
                }

                case ELFCLASS64:
                {
                    offset = phdr.phdr64->offset;
                    vaddr = phdr.phdr64->vaddr;
                    break;
                }

                default:
                {
                    return NULL;
                }
            }
        }

        string = (const char *) image->map(image, offset + (address - vaddr), stringOffset+1);

        if (!string)
            return NULL;

        stringOffset++;
    } while (string[stringOffset]);

    return string;
}

#ifdef LIBOS_TOOL_BAKE

NvBool LibosElfCommitData(LibosElfImage *elf, NvU64 commitVirtualAddress, NvU64 commitSize)

{
    if (!commitSize)
        return NV_FALSE;

    if (LibosElfGetClass(elf) != ELFCLASS64)
        return NV_FALSE;

    // Ensure the commit size doesn't wrap
    NvU64 lastByte;
    if (__builtin_add_overflow(commitVirtualAddress, commitSize - 1, &lastByte))
        return NV_FALSE;

    // Ensure that the commit region doesn't partially overlap any sections
    for (LibosElfSectionHeaderPtr psec = LibosElfSectionHeaderNext(elf, (LibosElfSectionHeaderPtr){0});
        psec.shdr64 != NULL;
        psec = LibosElfSectionHeaderNext(elf, psec))
        if (lastByte >= psec.shdr64->addr && (lastByte - psec.shdr64->addr) < psec.shdr64->size)
            lastByte = psec.shdr64->addr + psec.shdr64->size - 1;

    // Find the containing PHDR
    LibosElfProgramHeaderPtr phdr = LibosElf64ProgramHeaderForRange(elf, commitVirtualAddress, lastByte);

    // Is the area already committed?
    if (lastByte < (phdr.phdr64->vaddr + phdr.phdr64->filesz))
        return NV_TRUE;

    // Find insertion point in the ELF image
    NvU64 insertionOffset = phdr.phdr64->offset + phdr.phdr64->filesz;
    NvU64 insertionCount  = (lastByte + 1) - (phdr.phdr64->vaddr + phdr.phdr64->filesz);

    // Update the file backed size
    phdr.phdr64->filesz = lastByte - phdr.phdr64->vaddr + 1;

    // Update all PHDR offsets
    for (LibosElfProgramHeaderPtr phdr = LibosElfProgramHeaderNext(elf, 0); phdr.raw != NULL; phdr = LibosElfProgramHeaderNext(elf, phdr))
        if (phdr.phdr64->offset > insertionOffset)
            phdr.phdr64->offset += insertionCount;

    // Update all section offsets
    for (LibosElfSectionHeaderPtr psec = LibosElfSectionHeaderNext(elf, 0); psec.raw != NULL; psec = LibosElfSectionHeaderNext(elf, psec))
    {
        if (psec.shdr64->offset > insertionOffset)
            psec.shdr64->offset += insertionCount;

        // Did we just commit a section? Convert from nobits to progbits
        if (psec.shdr64->addr >= phdr->vaddr && (psec.shdr64->addr+psec.shdr64->size) <= (phdr.phdr64->vaddr + phdr.phdr64->filesz))
            if (psec.shdr64->type == SHT_NOBITS) {
                // @todo: Set the offset field
                psec.shdr64->type = SHT_PROGBITS;
            }
    }

    if (elf->elf->phoff > insertionOffset)
        elf->elf->phoff += insertionCount;

    if (elf->elf->shoff > insertionOffset)
        elf->elf->shoff += insertionCount;

    // Reallocate the file and create the hole
    elf->elf = realloc(elf->elf, elf->size + insertionCount);
    memmove(((NvU8*)elf->elf) + insertionOffset + insertionCount, ((NvU8*)elf->elf) + insertionOffset, elf->size - insertionOffset);

    // Zero the hole
    memset(((NvU8*)elf->elf) + insertionOffset, 0, insertionCount);

    elf->size += insertionCount;

    return NV_TRUE;
}

#endif
