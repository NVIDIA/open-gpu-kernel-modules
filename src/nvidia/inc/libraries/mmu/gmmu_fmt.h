/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_GMMU_FMT_H_
#define _NV_GMMU_FMT_H_

#include <ctrl/ctrl90f1.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @file gmmu_fmt.h
 *
 * @brief Defines a light abstraction over GPU MMU (GMMU) HW formats.
 *
 * There are two main components of the abstraction:
 * 1. General topology of the format provided by mmu_fmt.h.
 * 2. Description of the fields within PDEs and PTEs described by the
 *    field_desc.h and GMMU_FMT_P*E structs.
 *
 * The GMMU_FMT structure wraps these compoments together.
 *
 * The goals of this abstraction are:
 * G1. Allow common page table management code to work across a variety
 *     of GMMU HW formats.
 * G2. Provide as much low-level control as if directly using the HW
 *     manuals.
 * G3. As close to DRF-macro efficiency as possible for critical paths.
 *     An example of a critical path is writing PTE values in a tight loop.
 *     On the other hand, PDE value writes (some which have more complex
 *     formats) occur orders of magnitude less frequently, and thus can
 *     afford more generality.
 *
 * One design consideration is how to distinguish
 * MMU fields which are specific to certain architectures.
 *
 * The current approach is to describe the union of all fields
 * across the supported formats.
 * HW that does not support a given field must initialize the descriptor to
 * zero (invalid) which will assert in the field setter/getter if used.
 *
 * While this introduces risk of "kitchen sink" syndrome, this approach was
 * taken for the following reasons:
 * 1. There are few fundamental feature differences between GMMU formats.
 * 2. GMMU formats change relatively infrequently (e.g. rarely per-chip).
 */

#include "nvtypes.h"
#include "field_desc.h"
#include "mmu_fmt.h"

//
// Defines needed by PCF programming in PTE V3.
// Index bits are used when callers set flags. The below defines are only used
// for the HW <-> SW translation.
//

//
// Note: The following PCF patterns have not been verified in HW
// and have been currently added to help overcome issues wrt
// PCF patterns tested in rmtest.
//
// SW_MMU_PTE_PCF_PRIVILEGE_RW_ATOMIC_UNCACHED_ACD
// SW_MMU_PTE_PCF_PRIVILEGE_RW_ATOMIC_UNCACHED_ACE
// SW_MMU_PTE_PCF_PRIVILEGE_RW_NO_ATOMIC_CACHED_ACE
// SW_MMU_PTE_PCF_PRIVILEGE_RW_NO_ATOMIC_UNCACHED_ACE
// SW_MMU_PTE_PCF_PRIVILEGE_RO_ATOMIC_UNCACHED_ACE
// SW_MMU_PTE_PCF_PRIVILEGE_RO_NO_ATOMIC_CACHED_ACE
// SW_MMU_PTE_PCF_PRIVILEGE_RO_NO_ATOMIC_UNCACHED_ACE
// SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_CACHED_ACD
// SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_UNCACHED_ACD
// SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_UNCACHED_ACE
//

// Used by PDE
#define SW_MMU_PDE_PCF_INVALID_ATS_ALLOWED                  0x00000201
#define SW_MMU_PDE_PCF_INVALID_ATS_NOT_ALLOWED              0x00000001
#define SW_MMU_PDE_PCF_SPARSE_ATS_ALLOWED                   0x00000204
#define SW_MMU_PDE_PCF_SPARSE_ATS_NOT_ALLOWED               0x00000004

#define SW_MMU_PDE_PCF_VALID_CACHED_ATS_ALLOWED             0x00000200
#define SW_MMU_PDE_PCF_VALID_UNCACHED_ATS_ALLOWED           0x00000220
#define SW_MMU_PDE_PCF_VALID_CACHED_ATS_NOT_ALLOWED         0x00000000
#define SW_MMU_PDE_PCF_VALID_UNCACHED_ATS_NOT_ALLOWED       0x00000020

// Used by PTEs
#define SW_MMU_PTE_PCF_INVALID                              0x00000001
#define SW_MMU_PTE_PCF_NO_VALID_4KB_PAGE                    0x00000002
#define SW_MMU_PTE_PCF_SPARSE                               0x00000004
#define SW_MMU_PTE_PCF_MAPPING_NOWHERE                      0x00000008

#define SW_MMU_PTE_PCF_PRIVILEGE_RW_ATOMIC_CACHED_ACD       0x00000000
#define SW_MMU_PTE_PCF_PRIVILEGE_RW_ATOMIC_CACHED_ACE       0x00000010
#define SW_MMU_PTE_PCF_PRIVILEGE_RW_ATOMIC_UNCACHED_ACD     0x00000020
#define SW_MMU_PTE_PCF_PRIVILEGE_RW_ATOMIC_UNCACHED_ACE     0x00000030
#define SW_MMU_PTE_PCF_PRIVILEGE_RW_NO_ATOMIC_CACHED_ACE    0x00000050
#define SW_MMU_PTE_PCF_PRIVILEGE_RW_NO_ATOMIC_UNCACHED_ACE  0x00000070
#define SW_MMU_PTE_PCF_PRIVILEGE_RO_ATOMIC_UNCACHED_ACE     0x000000B0
#define SW_MMU_PTE_PCF_PRIVILEGE_RO_NO_ATOMIC_CACHED_ACE    0x000000D0
#define SW_MMU_PTE_PCF_PRIVILEGE_RO_NO_ATOMIC_UNCACHED_ACE  0x000000F0

#define SW_MMU_PTE_PCF_REGULAR_RW_ATOMIC_CACHED_ACD         0x00000100
#define SW_MMU_PTE_PCF_REGULAR_RW_ATOMIC_CACHED_ACE         0x00000110
#define SW_MMU_PTE_PCF_REGULAR_RW_ATOMIC_UNCACHED_ACD       0x00000120
#define SW_MMU_PTE_PCF_REGULAR_RW_ATOMIC_UNCACHED_ACE       0x00000130

#define SW_MMU_PTE_PCF_REGULAR_RW_NO_ATOMIC_CACHED_ACD      0x00000140
#define SW_MMU_PTE_PCF_REGULAR_RW_NO_ATOMIC_CACHED_ACE      0x00000150
#define SW_MMU_PTE_PCF_REGULAR_RW_NO_ATOMIC_UNCACHED_ACD    0x00000160
#define SW_MMU_PTE_PCF_REGULAR_RW_NO_ATOMIC_UNCACHED_ACE    0x00000170

#define SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_CACHED_ACD         0x00000180
#define SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_CACHED_ACE         0x00000190
#define SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_UNCACHED_ACD       0x000001A0
#define SW_MMU_PTE_PCF_REGULAR_RO_ATOMIC_UNCACHED_ACE       0x000001B0

#define SW_MMU_PTE_PCF_REGULAR_RO_NO_ATOMIC_CACHED_ACD      0x000001C0
#define SW_MMU_PTE_PCF_REGULAR_RO_NO_ATOMIC_CACHED_ACE      0x000001D0
#define SW_MMU_PTE_PCF_REGULAR_RO_NO_ATOMIC_UNCACHED_ACD    0x000001E0
#define SW_MMU_PTE_PCF_REGULAR_RO_NO_ATOMIC_UNCACHED_ACE    0x000001F0

//
// Defines all toggles in either PTE or PDE PCF space
// Note: please do not change these defines without careful review!
// PTE and PDE defines are allowed to collide as they will not be
// processed in the same code paths anyway.
//
#define SW_MMU_PCF_INVALID_IDX     0  // Used for PDE and PTE
#define SW_MMU_PCF_NV4K_IDX        1  // PTE specific
#define SW_MMU_PCF_SPARSE_IDX      2  // Used for PDE and PTE
#define SW_MMU_PCF_NOMAPPING_IDX   3  // PTE specific

#define SW_MMU_PCF_ACE_IDX         4  // PTE specific
#define SW_MMU_PCF_UNCACHED_IDX    5  // Used for PDE and PTE
#define SW_MMU_PCF_NOATOMIC_IDX    6  // PTE specific
#define SW_MMU_PCF_RO_IDX          7  // PTE specific

#define SW_MMU_PCF_REGULAR_IDX     8  // PTE specific
#define SW_MMU_PCF_ATS_ALLOWED_IDX 9  // PDE specific

// Forward declarations.
typedef union  GMMU_ENTRY_VALUE     GMMU_ENTRY_VALUE;
typedef struct GMMU_FIELD_APERTURE  GMMU_FIELD_APERTURE;
typedef struct GMMU_FIELD_ADDRESS   GMMU_FIELD_ADDRESS;
typedef struct GMMU_FMT             GMMU_FMT;
typedef struct GMMU_FMT_PDE         GMMU_FMT_PDE;
typedef struct GMMU_FMT_PDE_MULTI   GMMU_FMT_PDE_MULTI;
typedef struct GMMU_FMT_PTE         GMMU_FMT_PTE;
typedef struct GMMU_COMPR_INFO      GMMU_COMPR_INFO;

/*!
 * Maximum size in bytes of page directory and table entries across
 * the supported formats.
 */
#define GMMU_FMT_MAX_ENTRY_SIZE 16

/*!
 * Default version specifier for API args to indicate no preference.
 * This is not a real version number and not part of the
 * enumeration array below.
 */
#define GMMU_FMT_VERSION_DEFAULT  0

/*!
 * 2-level (40b VA) format supported Fermi through Maxwell.
 * Still supported in Pascal HW as fallback.
 */
#define GMMU_FMT_VERSION_1        1

/*!
 * 5-level (49b VA) format supported on Pascal+.
 */
#define GMMU_FMT_VERSION_2        2

/*!
 * 6-level (57b VA) format supported on Hopper+.
 */
#define GMMU_FMT_VERSION_3        3

/*!
 * Maximum number of MMU versions supported.
 */
#define GMMU_FMT_MAX_VERSION_COUNT 3

/*!
 * Array of format version numbers for enumeration utility.
 */
extern const NvU32 g_gmmuFmtVersions[GMMU_FMT_MAX_VERSION_COUNT];

/*!
 * Maximum number of big page sizes supported by a single GPU.
 */
#define GMMU_FMT_MAX_BIG_PAGE_SIZES 2

/*!
 * Array of big page shifts for enumeration utility.
 */
extern const NvU32 g_gmmuFmtBigPageShifts[GMMU_FMT_MAX_BIG_PAGE_SIZES];

/*!
 * Convenience type for declaring generic temporary GMMU entry values.
 */
union GMMU_ENTRY_VALUE
{
    NvU8   v8[GMMU_FMT_MAX_ENTRY_SIZE / 1];
    NvU32 v32[GMMU_FMT_MAX_ENTRY_SIZE / 4];
    NvU64 v64[GMMU_FMT_MAX_ENTRY_SIZE / 8];
};

/*!
 * Top-level structure describing a GPU MMU format.
 */
struct GMMU_FMT
{
    NvU32 version;

    /*!
     * Root of the page level topology (e.g. the root page directory).
     */
    const MMU_FMT_LEVEL *pRoot;

    /*!
     * Description of page directory entry fields common
     * across page directory levels with a single sub-level.
     */
    const GMMU_FMT_PDE       *pPde;

    /*!
     * Description of page directory entry fields common
     * across page directory levels with two sub-levels.
     */
    const GMMU_FMT_PDE_MULTI *pPdeMulti;

    /*!
     * Description of page table entry fields common
     * across all page table levels in the topology.
     */
    const GMMU_FMT_PTE       *pPte;

    /*!
     * Indicates if the MMU HW supports sparse through the
     * volatile field of each PDE/PTE.
     */
    NvBool bSparseHwSupport;
};

/*!
 * Physical apertures for the supported GMMU formats.
 */
typedef enum
{
    /*!
     * Indicates an invalid aperture.
     * @note Only supported for GPU PDEs to distinguish invalid sub-sevels.
     */
    GMMU_APERTURE_INVALID,

    /*!
     * GPU-local video memory (a.k.a. FB).
     * @note Only supported for GPU PDEs and PTEs.
     */
    GMMU_APERTURE_VIDEO,

    /*!
     * GPU-peer video memory.
     * @note Only supported for GPU PTEs.
     * @note Peer index must be initialized in the appropriate address field.
     */
    GMMU_APERTURE_PEER,

    /*!
     * Non-coherent system memory.
     *
     * (GPU) MMU will NOT maintain coherence with CPU L2 cache.
     *
     * Higher-level APIs should only allow this when it is known
     * the memory is not cacheable by CPU or the coherency is
     * managed explicitly (e.g. w/ flushes in SW).
     * Also consider that this path is not necessarily faster.
     */
    GMMU_APERTURE_SYS_NONCOH,

    /*!
     * Coherent system memory.
     *
     * (GPU) MMU will snoop CPU L2 cache if possible.
     * TODO: Wiki link on arch differences.
     *
     * This is usually the safer choice over NONCOH since it works
     * whether the memory is cached by CPU L2 or not.
     * On some CPU architectures going through CPU L2 may
     * even be faster than the non-coherent path.
     */
    GMMU_APERTURE_SYS_COH,

    // Last value.
    GMMU_APERTURE__COUNT
} GMMU_APERTURE;

/*!
 * Aperture field descriptor.
 */
struct GMMU_FIELD_APERTURE
{
    NV_FIELD_ENUM _enum;
};

#define INIT_FIELD_APERTURE(pAper, drf, _entries)                               \
    do {                                                                        \
        INIT_FIELD_ENUM(&(pAper)->_enum, drf, GMMU_APERTURE__COUNT, _entries);  \
    } while(0)

/*!
 * Encode and set a GMMU aperture enum value to a HW aperture field.
 */
static NV_FORCEINLINE void
gmmuFieldSetAperture
(
    const GMMU_FIELD_APERTURE *pAperture,
    const GMMU_APERTURE        value,
    NvU8                      *pMem
)
{
    nvFieldSetEnum(&pAperture->_enum, value, pMem);
}

/*!
 * Get and decode a HW aperture field value to a GMMU aperture enum value.
 */
static NV_FORCEINLINE GMMU_APERTURE
gmmuFieldGetAperture
(
    const GMMU_FIELD_APERTURE *pAperture,
    const NvU8                *pMem
)
{
    return (GMMU_APERTURE)nvFieldGetEnum(&pAperture->_enum, pMem);
}

/*!
 * Address field descriptor.
 */
struct GMMU_FIELD_ADDRESS
{
    NV_FIELD_DESC64 desc;
    NvU32           shift;
};

#define INIT_FIELD_ADDRESS(pAddr, drf, _shift)          \
    do {                                                \
        INIT_FIELD_DESC64(&(pAddr)->desc, drf);         \
        (pAddr)->shift = _shift;                        \
    } while(0)

/*!
 * Encode (shift) and set a GMMU address field.
 */
static NV_FORCEINLINE void
gmmuFieldSetAddress
(
    const GMMU_FIELD_ADDRESS *pField,
    const NvU64               address,
    NvU8                     *pMem
)
{
    NV_ASSERT_CHECKED_PRECOMP(0 == (address & (NVBIT64(pField->shift) - 1)));
    nvFieldSet64(&pField->desc, address >> pField->shift, pMem);
}

/*!
 * Get and decode (shift) a GMMU address field.
 */
static NV_FORCEINLINE NvU64
gmmuFieldGetAddress
(
    const GMMU_FIELD_ADDRESS *pField,
    const NvU8               *pMem
)
{
    return nvFieldGet64(&pField->desc, pMem) << pField->shift;
}

/*!
 * Page directory entry (PDE) format.
 */
struct GMMU_FMT_PDE
{
    /*!
     * Version information is needed to interpret fields differently.
     * Should be always the same as version in GMMU_FMT above.
     */
    NvU32 version;

    /*!
     * Aperture field indicating which physical address space the sublevel resides.
     */
    GMMU_FIELD_APERTURE fldAperture;

    /*!
     * Physical address field when aperture is system memory.
     */
    GMMU_FIELD_ADDRESS fldAddrSysmem;

    /*!
     * Physical address field when aperture is video memory.
     */
    GMMU_FIELD_ADDRESS fldAddrVidmem;

    /*!
     * Physical address field (used by V3 format only).
     */
    GMMU_FIELD_ADDRESS fldAddr;

    /*!
     * Indicates GPU reads memory on every access to the
     * next page directory/table level.
     *
     * @note This is not the same as caching, and is ignored for some
     *       apertures on some chips.
     *       TODO: Wiki link to explain arch differences.
     */
    NV_FIELD_BOOL fldVolatile;

    /*!
     * PDE_PCF field for V3 format.
     */
    NV_FIELD_DESC32 fldPdePcf;
};

/*!
 * Get the PDE physical address field format for a given aperture.
 */
const GMMU_FIELD_ADDRESS *gmmuFmtPdePhysAddrFld(
                        const GMMU_FMT_PDE *pPde,
                        const GMMU_APERTURE aperture);

/*!
 * Multi (e.g. dual) page directory entry format.
 */
struct GMMU_FMT_PDE_MULTI
{
    /*!
     * Reciprocal exponent field for partial sub-level size.
     * Minimum size of each sub-level is FullLevelSize / (2 ^ sizeRecipExpMax).
     */
    NV_FIELD_DESC32 fldSizeRecipExp;

    /*!
     * Per-sub-level information.
     */
    GMMU_FMT_PDE subLevels[MMU_FMT_MAX_SUB_LEVELS];
};

/*!
 * Retrieve the PDE format corresponding to a particular level and sub-level.
 *
 * @param[in]  pFmt     MMU format.
 * @param[in]  pLevel   Level format.
 * @param[in]  subLevel Sub-level index <= MMU_FMT_MAX_SUB_LEVELS.
 *
 * @returns Sub-level PDE format or NULL if not a page directory level.
 */
const GMMU_FMT_PDE* gmmuFmtGetPde(
            const GMMU_FMT      *pFmt,
            const MMU_FMT_LEVEL *pLevel,
            const NvU32          subLevel);

/*!
 * Page table entry (PTE) format.
 */
struct GMMU_FMT_PTE
{
    /*!
     * Version information is needed to interpret fields differently.
     * Should be always the same as version in GMMU_FMT above.
     */
    NvU32 version;

    /*!
     * Field that determines if the PTE is valid.
     */
    NV_FIELD_BOOL fldValid;

    /*!
     * Aperture field indicating which the physical page resides.
     */
    GMMU_FIELD_APERTURE fldAperture;

    /*!
     * Physical address field when aperture is system memory.
     */
    GMMU_FIELD_ADDRESS fldAddrSysmem;

    /*!
     * Physical address field when aperture is video memory.
     */
    GMMU_FIELD_ADDRESS fldAddrVidmem;

    /*!
     * Physical address field when aperture is peer memory.
     */
    GMMU_FIELD_ADDRESS fldAddrPeer;

    /*!
     * Peer index field when aperture is peer memory.
     */
    NV_FIELD_DESC32 fldPeerIndex;

    /*!
     * Indicates GPU reads/writes memory on every access to the page.
     *
     * @note This is not the same as caching, and is ignored for some
     *       apertures on some chips.
     *       TODO: Wiki link to explain arch differences.
     */
    NV_FIELD_BOOL fldVolatile;

    /*!
     * Indicates to generate a read-only (RO) fault on writes.
     *
     * @note This does not affect L1 cache access if
     *       fldWriteDisable is supported.
     */
    NV_FIELD_BOOL fldReadOnly;

    /*!
     * Indicates to generate a write-only (WO) fault on L1 reads.
     * @note Only supported on some GPU architectures.
     */
    NV_FIELD_BOOL fldReadDisable;

    /*!
     * Indicates to generate a read-only (WO) fault on L1 writes.
     * @note Only supported on some GPU architectures.
     */
    NV_FIELD_BOOL fldWriteDisable;

    /*!
     * Indicates to fault on non-priviledged access.
     */
    NV_FIELD_BOOL fldPrivilege;

    /*!
     * See HW manuals.
     */
    NV_FIELD_BOOL fldEncrypted;

    /*!
     * Indicates to lock the PTE in the GPU TLBs, giving precedence over
     * unlocked-PTEs.
     * TLB invalidate will still evict the PTE.
     */
    NV_FIELD_BOOL fldLocked;

    /*!
     * TODO: TBD
     */
    NV_FIELD_BOOL fldAtomicDisable;

    /*!
     * Kind (storage format) field.
     */
    NV_FIELD_DESC32 fldKind;

    /*!
     * Compression tag field.
     */
    NV_FIELD_DESC32 fldCompTagLine;

    /*!
     * Compression tag sub-index field.
     */
    NV_FIELD_DESC32 fldCompTagSubIndex;

    /*!
     * PTE_PCF field for V3 format.
     */
    NV_FIELD_DESC32 fldPtePcf;
};

/*!
 * Determine if an entry is a PTE or PDE based either on its static format or
 * dynamic value.
 *
 * @param[in]  pFmt   MMU format.
 * @param[in]  pLevel Level format.
 * @param[in]  pEntry Entry value of size pLevel->entrySize.
 *
 * @returns true if the entry is a PTE, false if it is a PDE.
 */
NvBool gmmuFmtEntryIsPte(
            const GMMU_FMT      *pFmt,
            const MMU_FMT_LEVEL *pLevel,
            const NvU8          *pEntry);

/*!
 * Get the PTE physical address field format for a given aperture.
 */
const GMMU_FIELD_ADDRESS *gmmuFmtPtePhysAddrFld(
                        const GMMU_FMT_PTE *pPte,
                        const GMMU_APERTURE aperture);

/*!
 * GPU compression attributes for a physical surface.
 *
 * This info will be returned by RM from HW resource alloc API.
 */
struct GMMU_COMPR_INFO
{
    /*!
     * log2 of compression page size.
     */
    NvU32 compPageShift;

    /*!
     * Compressed kind.
     */
    NvU32 compressedKind;

    /*!
     * Index of the first compression page relative to the surface.
     * e.g. if the entire surface is compressed this is 0.
     */
    NvU32 compPageIndexLo;

    /*!
     * Index of the last compression page relative to the surface.
     * e.g. (compPageIndexHi - compPageIndexLo + 1) is the number of comp
     *      tag lines used for the surface.
     * CompPageIndex is tracked in a (1 << compPageShift) granularity
     */
    NvU32 compPageIndexHi;

    /*!
     * Starting comptag line to use at compPageIndexLo.
     * Comptags are used contiguously up to the maximum
     * compTagLineMin + (compPageIndexHi - compPageIndexLo).
     */
    NvU32 compTagLineMin;

    /*!
     *  Granularity of comptagline assignment.
     *  Used for Verif only, Deprecated from Turing
     */
    NvU32 compTagLineMultiplier;
};

/*!
 * Update a PTE value's compression fields based
 * on the legacy compression attributes of the surface being mapped.
 *
 * @param[in]     pFmt        MMU format.
 * @param[in]     pLevel      Level format.
 * @param[in]     pCompr      Compression info of the physical surface.
 * @param[in]     surfOffset  Offset in bytes into the physical surface.
 * @param[in]     startPteIndex Starting pte index for comptagSubIndex calculation.
 * @param[in]     numPages    Number of pages (PTEs) to update.
 * @param[in,out] pEntries    Array of PTE values to update of length
 *                            numPages * pLevel->entrySize.
 */
void gmmuFmtInitPteCompTags(
                const GMMU_FMT        *pFmt,
                const MMU_FMT_LEVEL   *pLevel,
                const GMMU_COMPR_INFO *pCompr,
                const NvU64            surfOffset,
                const NvU32            startPteIndex,
                const NvU32            numPages,
                NvU8                  *pEntries);


#ifdef __cplusplus
}
#endif

#endif
