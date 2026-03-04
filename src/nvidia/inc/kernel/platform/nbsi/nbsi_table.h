/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NBSITBL_H
#define NBSITBL_H

#include "ctrl/ctrl0000/ctrl0000system.h" // NV0000_SYSTEM_MAX_APPROVAL_COOKIE_STRING_LENGTH
#include "ctrl/ctrl2080/ctrl2080bios.h"   // NV2080_CTRL_BIOS_NBSI_NUM_MODULES
#include "core/core.h"
#include "platform/pci_exp_table.h"

// Maximum number of NBSI OS strings (including generic)
#define MAX_NBSI_OS 3
#define MAX_NBSI_OS_STR_LEN 10

#define NBSI_SOURCE_LOC NvU16

typedef enum _NBSI_TBL_SOURCES                   // keep in sync with nvapi.spec
{
    NBSI_TBL_SOURCE_BEST_FIT = 0,
    NBSI_TBL_SOURCE_REGISTRY = 1,
    NBSI_TBL_SOURCE_VBIOS    = 2,
    NBSI_TBL_SOURCE_SBIOS    = 8,
    NBSI_TBL_SOURCE_ACPI     = 0x10,
    NBSI_TBL_SOURCE_UEFI     = 0x20
} NBSI_TBL_SOURCES, * PNBSI_TBL_SOURCES;
#define NBSI_TBL_SOURCE_MAX 6                    // number of NBSI_TBL_SOURCES entries (not including BEST FIT)

#define NBSI_TBL_SOURCE_ALL      (NBSI_TBL_SOURCE_REGISTRY | \
                                  NBSI_TBL_SOURCE_VBIOS |    \
                                  NBSI_TBL_SOURCE_SBIOS |    \
                                  NBSI_TBL_SOURCE_ACPI |     \
                                  NBSI_TBL_SOURCE_UEFI)
#define NBSI_TBL_SOURCE_NONE     0

typedef enum _NBSI_ACPI_METHOD
{
    NBSI_TBL_SOURCE_ACPI_UNKNOWN,
    NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE,
    NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ,
    NBSI_TBL_SOURCE_ACPI_BOTH_METHODS
} NBSI_ACPI_METHOD, *PNBSI_ACPI_METHOD;

typedef struct _NBSI_CACHE_ENTRY_OBJ {
    NvU16            globType;
    NBSI_SOURCE_LOC  globSource;
    NvU8             globIndex;
    NBSI_SOURCE_LOC  altGlobSource;
    NvU8             altGlobIndex;
    NvU8           * pObj;
} NBSI_CACHE_ENTRY_OBJ, *PNBSI_CACHE_ENTRY_OBJ;

typedef struct _NBSI_CACHE_OBJ {
    // Number of entries we've found and cached.
    NvU8 tblCacheNumEntries;
    // Maximum entries we might have.
    NvU8 tblCacheMaxNumEntries;
    // Pointers to cache entries
    PNBSI_CACHE_ENTRY_OBJ pCacheEntry[1];
} NBSI_CACHE_OBJ, *PNBSI_CACHE_OBJ;

//
// The following two structs (DRVR_VER0 and DRVR_VER) attempt to handle
// the driver version.
// Driver revision example: 7.15.11.7782
// The format is: a.bb.1c.dddd
// where a represents the OS (7 for Vista/Vista64)
//       bb represents the DX version (15 for DX10)
//          the 1 in 1c is specified by MS
//       c.dddd is the NV-specific driver version.
// old version of this structure had byte to hold minRev but the new
// version uses 20 bits.
//
typedef struct _DRVR_VER0
{
    NvU8    majVer : 8;
    NvU8    minVer : 8;
    NvU8    majRev : 8;
    NvU8    minRev : 8;
} DRVR_VER0, * PDRVR_VER0;

typedef struct _DRVR_VER
{
    NvU32 Rev : 20;
    NvU32 DX : 8;
    NvU32 OS : 4;
} DRVR_VER, * PDRVR_VER;

typedef struct _NBSI_OBJ {
    // Setting for current maximum OS strings in use.
    NvU8 curMaxNbsiOSes;

    // Default hash strings for OS strings (done once so to speed up the system)
    NvU32 nbsiOSstrHash[MAX_NBSI_OS];

    // Default string for a blank path hash (done once so to speed up the system)
    NvU16 nbsiBlankPathHash;

    // Contains the OS strings.
    NvU8  nbsiOSstr[MAX_NBSI_OS][MAX_NBSI_OS_STR_LEN+1];
    NvU32 nbsiOSstrLen[MAX_NBSI_OS];

    // Currently available locations of NBSI directory
    NBSI_SOURCE_LOC availDirLoc[NV_MAX_DEVICES];

    // nbsi driver object save.
    NvU8 * nbsiDrvrTable[NV_MAX_DEVICES];

    // pointer to array of pointers (cache of tables)
    PNBSI_CACHE_OBJ pTblCache[NV_MAX_DEVICES];

    // pointer to override tables
    NvU8 * regOverrideList[NV_MAX_DEVICES];

    // Current Driver version for best fit check.
    DRVR_VER DriverVer;

} NBSI_OBJ, *PNBSI_OBJ;


#ifndef VARIABLE_SIZE_ARRAY
#define VARIABLE_SIZE_ARRAY 1 // The size of this array may vary.
#endif

// For offsets which are present but not defined (an error case) have
// a predefined undefined link. This allows me to build a table which
// has all 12 module types defined, but valid links on only the ones I'm
// using... And I can fail the search if asked to search one that's not
// defined.
#define NBSI_UNDEFINED_OFFSET 0xffffffff

typedef enum _NBSI_ELEMENT_TYPES
{
    NBSI_BYTE = 0,
    NBSI_WORD,
    NBSI_DWORD,
    NBSI_QWORD,
    NBSI_BYTE_ARRAY,
    NBSI_BYTE_ARRAY_EXTENDED,
    numNBSI_TYPES
} NBSI_ELEMENT_TYPES;

// size of element header in bytes. 24 bits.
#define NBSI_ELEMENT_HDRSIZ 3

#pragma pack(1)
typedef struct _NBSI_ELEMENT
{
    struct ELEMENT_HDR_PARTA
    {
        NvU8   type: 4;
        NvU8   lsnValueID: 4;
    } hdrPartA;
    struct ELEMENT_HDR_PARTB
    {
        NvU16   mswValueID;
    } hdrPartB;
    union ELEMENT_DATA
    {
        NvU8    dataByte;
        NvU16   dataWord;
        NvU32   dataDWord;
        NvU64   dataQWord;
        struct
        {
            NvU8   size;
            NvU8   data[VARIABLE_SIZE_ARRAY];
        } ba;
        struct
        {
            NvU16  size;
            NvU8   data[VARIABLE_SIZE_ARRAY];
        } bax;
    } data;
} NBSI_ELEMENT, *PNBSI_ELEMENT;
#pragma pack()

#pragma pack(1)
typedef struct _NBSI_ELEMENTS
{
    NvU32         numElements;
    NBSI_ELEMENT  elements[VARIABLE_SIZE_ARRAY];
} NBSI_ELEMENTS, *PNBSI_ELEMENTS;
#pragma pack()

#pragma pack(1)
typedef struct _NBSI_SCOPE
{
    NvU16   pathID;
    NvU32   offset; // Relative Offset from this member i.e. &ulOffset
} NBSI_SCOPE, *PNBSI_SCOPE;
#pragma pack()

#pragma pack(1)
typedef struct _NBSI_SCOPES
{
    NvU32       numPaths;
    NBSI_SCOPE  paths[VARIABLE_SIZE_ARRAY];
} NBSI_SCOPES, *PNBSI_SCOPES;
#pragma pack()

#pragma pack(1)
typedef struct _NBSI_MODULE
{
    NvU16   moduleID;
    NvU32   offset;  // Relative Offset from this member i.e. &ulOffset
} NBSI_MODULE, *PNBSI_MODULE;
#pragma pack()

// Maximum understood dir version
#define MAXNBSIDIRVER 1

#define NBSIDIRHDRSTRING (NvU32) (('N'<<24)+('B'<<16)+('S'<<8)+'I')
#define MXMHDRSTRING (NvU32) (('_'<<24)+('M'<<16)+('X'<<8)+'M')

// A directory of globs
#pragma pack(1)
typedef union _NBSI_DIRECTORY
{
    struct OLD_FORMAT                   // Original version for Beta
    {
        NvU8  numGlobs;                 // number of globs
        NvU8  dirVer;                   // dirVer
        NvU16 globType[1];              // NBSI_GLOB_TYPE (placeholder)
    } od;
    struct NEW_FORMAT                   // Shipping version
    {
        NvU32 nbsiHeaderString;         // header string NBSIDIRHDRSTRING
        NvU32 size;                     // size of entire directory
        NvU8  numGlobs;                 // number of globs
        NvU8  dirVer;                   // dirVer
        NvU16 globType[1];              // NBSI_GLOB_TYPE (placeholder
    } d;
} NBSI_DIRECTORY, *PNBSI_DIRECTORY;
#pragma pack()

#define nbsiobjtype(ch1,ch2) (NvU16) ((ch1<<8) + ch2)
typedef enum _NBSI_GLOB_TYPES
{
    NBSI_RSRVD_GLOB     = 0,     // Reserved Glob type
    NBSI_DRIVER         = nbsiobjtype('D','R'),  // Driver Object
    NBSI_VBIOS          = nbsiobjtype('V','B'),  // VBIOS Object
    NBSI_HDCP           = nbsiobjtype('H','K'),  // HDCP Keys
    NBSI_INFOROM        = nbsiobjtype('I','R'),  // InfoROM object
    NBSI_HDD            = nbsiobjtype('H','D'),  // Storage Driver
    NBSI_NONVOLATILE    = nbsiobjtype('N','V'),  // CMOS settings
    NBSI_PLAT_INFO      = nbsiobjtype('P','I'),  // PlatformInfo Object
    NBSI_PLAT_INFO_WAR  = nbsiobjtype('I','P'),  // PlatformInfo WAR Bug 986051
    NBSI_VALKEY         = nbsiobjtype('V','K'),  // Validation key
    NBSI_TEGRA_INFO     = nbsiobjtype('T','G'),  // Tegra Info object
    NBSI_TEGRA_DCB      = nbsiobjtype('T','D'),  // Tegra DCB object
    NBSI_TEGRA_PANEL    = nbsiobjtype('T','P'),  // Tegra TPB object
    NBSI_TEGRA_DSI      = nbsiobjtype('T','S'),  // Tegra DSI information
    NBSI_SYS_INFO       = nbsiobjtype('G','D'),  // System Info object
    NBSI_TEGRA_TMDS     = nbsiobjtype('T','T'),  // Tegra TMDS configuration block
    NBSI_OPTIMUS_PLAT   = nbsiobjtype('O','P'),  // Optimus Platform key
} NBSI_GLOB_TYPE, * PNBSI_GLOB_TYPE;

#pragma pack(1)
typedef struct _NBSI_UID0
{
      NvU16   svid;       // Sub-system Vendor ID
      NvU16   ssid;       // Sub-system Device ID
      struct {
          NvU16   vid;    // Chip Vendor ID (NVIDIA)
          NvU16   did;    // Chip Device ID
          NvU8    revId;  // Chip RevID
      } Chip;
      DRVR_VER0 Driver;   // Target Driver Version
      struct {            // Target BIOS Version
          NvU8    minVer : 8;
          NvU8    majVer : 8;
      } VBIOS;
      struct {            // Platform Firmware Version
          NvU8    minVer : 4;
          NvU8    majVer : 4;
      } Platform;
} NBSI_UID0, *PNBSI_UID0;
#pragma pack()

#pragma pack(1)
typedef struct _NBSI_UID
{
      NvU16   svid;       // Sub-system Vendor ID
      NvU16   ssid;       // Sub-system Device ID
      struct {
          NvU16   vid;    // Chip Vendor ID (NVIDIA)
          NvU16   did;    // Chip Device ID
          NvU8    revId;  // Chip RevID
      } Chip;
      DRVR_VER Driver;    // Target Driver Version
      struct {            // Target BIOS Version
          NvU8    minVer : 8;
          NvU8    majVer : 8;
      } VBIOS;
      struct {            // Platform Firmware Version
          NvU8    minVer : 4;
          NvU8    majVer : 4;
      } Platform;
} NBSI_UID, *PNBSI_UID;
#pragma pack()

#pragma pack(1)
typedef struct _DSM_GEN_OBJ_HDR
{
    NvU64       sig;          // Common hash signature
    NvU16       globType;     // NBSI_GLOB_TYPE (i.e. NBSI_HDCP, NBSI_VALKEY etc.)
    NvU32       size;         // Entire size in bytes object, including header and object data.
    NvU16       majMinVer;    // Version of Generic Object in Maj:Min format
} DSM_GEN_OBJ_HDR, *PDSM_GEN_OBJ_HDR;
#pragma pack()
#define DSM_GEN_HDR_SIZE (sizeof(DSM_GEN_OBJ_HDR))


#pragma pack(1)
typedef struct _NBSI_GEN_OBJ
{
    DSM_GEN_OBJ_HDR  objHdr;
    // actual packed object data
    NvU8             objData[VARIABLE_SIZE_ARRAY];
} NBSI_GEN_OBJ, *PNBSI_GEN_OBJ;
#pragma pack()
#define NBSI_GEN_HDR_SIZE (sizeof(NBSI_GEN_OBJ)-VARIABLE_SIZE_ARRAY)

#define NBSI_MAX_TABLE_SIZE (256*1024) // define some maximum size
#define NBSI_MIN_GEN_OBJ_SIZE NBSI_GEN_HDR_SIZE

#define NBSI_DRIVERVER_0100 0x0100
#pragma pack(1)
typedef struct _NBSI_DRIVER_OBJ0
{
    DSM_GEN_OBJ_HDR objHdr;       // object header. globType = NBSI_GLOB_TYPE ('DR')
    NBSI_UID0       uid;          // Platform UID
    NvU32           numModules;
                                  // num_modules may vary... this is placeholder
    NBSI_MODULE     modules[NV2080_CTRL_BIOS_NBSI_NUM_MODULES];
    // actual packed object data
    NvU8            objData[VARIABLE_SIZE_ARRAY];
} NBSI_DRIVER_OBJ0, *PNBSI_DRIVER_OBJ0;
#pragma pack()

#pragma pack(1)
typedef struct _NBSI_DRIVER_OBJ
{
    DSM_GEN_OBJ_HDR objHdr;       // object header. globType = NBSI_GLOB_TYPE ('DR')
    NBSI_UID        uid;          // Platform UID
    NvU32           numModules;
                                  // num_modules may vary... this is placeholder
    NBSI_MODULE     modules[NV2080_CTRL_BIOS_NBSI_NUM_MODULES];
    // actual packed object data
    NvU8            objData[VARIABLE_SIZE_ARRAY];
} NBSI_DRIVER_OBJ, *PNBSI_DRIVER_OBJ;
#pragma pack()

#endif // NBSITBL_H
