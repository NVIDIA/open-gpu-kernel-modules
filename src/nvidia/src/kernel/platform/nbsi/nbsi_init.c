/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief NBSI table initialization routines.
 */

#include "os/os.h"
#include "platform/acpi_common.h"
#include "platform/nbsi/nbsi_read.h"
#include "platform/platform.h"
#include "nbci.h"
#include "nvrm_registry.h"
#include "gpu/gpu.h"
#include "nvhybridacpi.h"
#include "nvlimits.h"

// Template to use to read in emulated nbsi table from registry.

#define NBSI_ROM_CODE_SIZE (256*1024) // allocate amt for ROMs to be read into

extern const GUID NV_GUID_UEFI_VARIABLE;

//----------------------------------------------------------------------------
//  NvU32 setNbsiOSstring(* data, data_len, nbsiOSndx, maxNbsiOS)
//
//  This function computes the global nbsiOShash so that future 20 bit
//  fnv hash calculations can start with this hash value.
//
//  Input parameters:
//      *data      - Byte array
//      dataLen    - length of Byte array
//      nbsiOSndx  - index to place this hash to (0 based)
//      maxNbsiOS  - maximum supported OS.
//
//  Example usage to set up searching for Vista64 first, then Vista generic
//     then blank:
//      setNbsiOSstring("Vista64",7,2,3);
//      setNbsiOSstring("Vista",5,1,3);
//      setNbsiOSstring("",0,0,3);
//  Note: The highest OS index is searched first and the lowest (0) is last.
//
//  Returns the current number of OSes supported.
//
//----------------------------------------------------------------------------
static NvU32 setNbsiOSstring
(
    const void * data_v,
    NvU32 dataLen,
    NvU8 nbsiOSndx,
    NvU8 maxNbsiOS
)
{
    NBSI_OBJ *pNbsiObj = getNbsiObject();
    NvU32 hash;
    NvU8 i;
    NvU8 nullPathStr[1] = {0};
    const NvU8 * data = (const NvU8 *)data_v;

    NV_ASSERT(data);
    NV_ASSERT(nbsiOSndx < MAX_NBSI_OS);
    NV_ASSERT(maxNbsiOS < MAX_NBSI_OS+1);

    if (maxNbsiOS <= MAX_NBSI_OS)
    {
        pNbsiObj->curMaxNbsiOSes = maxNbsiOS;
    }
    else
    {
        pNbsiObj->curMaxNbsiOSes = 1;
        nbsiOSndx = 0;
    }

    if (dataLen > MAX_NBSI_OS_STR_LEN)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "NBSI OS string length %d too long for OS ndx %d.\n",
                  dataLen, nbsiOSndx);
        dataLen = MAX_NBSI_OS_STR_LEN;
    }

    // Copy the new string in.
    for (i = 0; i < dataLen; i++)
    {
        pNbsiObj->nbsiOSstr[nbsiOSndx][i] = data[i];
    }
    pNbsiObj->nbsiOSstr[nbsiOSndx][dataLen] = 0;
    pNbsiObj->nbsiOSstrLen[nbsiOSndx] = dataLen;

    //
    // Compute the hash for this OS string. Used in the cases where
    // the elements are blank and only the OS string is needed.
    // Apparently a surprisingly common event for modes.
    //
    hash = fnv32buf(&pNbsiObj->nbsiOSstr[nbsiOSndx][0],
                    pNbsiObj->nbsiOSstrLen[nbsiOSndx],
                    FNV1_32_INIT,
                    0);
    pNbsiObj->nbsiOSstrHash[nbsiOSndx] = ((hash>>20) ^ hash) & MASK_20;

    // precompute a null Path hash
    pNbsiObj->nbsiBlankPathHash = fnv1Hash16(&nullPathStr[0], 0);

    return pNbsiObj->curMaxNbsiOSes;
}

//----------------------------------------------------------------------------
//  NV_STATUS testObjectHash(pGpu, PNBSI_GEN_OBJ)
//
//  This function checks the hash of an object
//
//  Input parameters:
//      pGpu                  pointer to gpu object
//      PNBSI_GEN_OBJ         pointer to generic object
//
//  Output parameters:
//      NV_STATUS:            NV_OK if the hash is valid
//
//----------------------------------------------------------------------------
static NV_STATUS testObjectHash
(
    OBJGPU       *pGpu,
    PNBSI_GEN_OBJ pNbsiGenObj
)
{
    NV_STATUS   status;
    NvU64       tableHash;
    NvU8      * hashStart;
    NvU32       hashLen;

    NV_ASSERT(pNbsiGenObj!=NULL);

    status = NV_OK;
    // validate the hash using Fnv1 CRC
    hashStart = (NvU8 *) pNbsiGenObj;
    hashStart = &hashStart[sizeof(pNbsiGenObj->objHdr.sig)];
    hashLen = pNbsiGenObj->objHdr.size - sizeof(pNbsiGenObj->objHdr.sig);
    tableHash = fnv1Hash64(hashStart, hashLen);
    if (tableHash != pNbsiGenObj->objHdr.sig)
    {
        status = NV_ERR_INVALID_DATA;
        NV_PRINTF(LEVEL_NOTICE,
                  "NBSI tbl computed hash %llx != hash %llx\n",
                  tableHash, pNbsiGenObj->objHdr.sig);
    }

    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS isGlobTypeInNbsiDir(pNbsiDir, wantedGlobType,
//                                *pCntOfGlobsWithThisGlobType, *pNumGlobs)
//
//  This function counts the number of globs in the directory of the
//  wanted type and the total number of globs.
//
//  Input parameters:
//      pNbsiDir                     pointer to nbsi directory object
//      wantedGlobType               wanted glob type (to count)
//
//  Output parameters:
//      NV_STATUS:                   NV_OK if the directory version is okay
//      pCntOfGlobsWithThisGlobType  pointer to count of wantedGlobs found
//      pNumGlobs                    pointer to number of globs in directory
//                                   found
//
//----------------------------------------------------------------------------

static NV_STATUS isGlobTypeInNbsiDir
(
    PNBSI_DIRECTORY   pNbsiDir,
    NBSI_GLOB_TYPE    wantedGlobType,
    NvU8            * pCntOfGlobsWithThisGlobType,
    NvU8            * pNumGlobs
)
{
    NvU8              curGlob;
    NvU16           * pGlobType = NULL;

    NV_ASSERT(pNbsiDir);
    NV_ASSERT(pCntOfGlobsWithThisGlobType);
    NV_ASSERT(pNumGlobs);

    if (pNbsiDir->d.nbsiHeaderString == NBSIDIRHDRSTRING)
    {
        *pNumGlobs = pNbsiDir->d.numGlobs;
        pGlobType = pNbsiDir->d.globType;
    }
    else
    {
        *pNumGlobs = pNbsiDir->od.numGlobs;
        pGlobType = pNbsiDir->od.globType;
    }

    if (wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR)
    {
        *pCntOfGlobsWithThisGlobType = *pNumGlobs;
    }
    else
    {
        *pCntOfGlobsWithThisGlobType = 0;
        for (curGlob = 0; curGlob < *pNumGlobs; curGlob++)
        {
            if (wantedGlobType == pGlobType[curGlob])
            {
                *pCntOfGlobsWithThisGlobType += 1;
            }
        }
    }
    return NV_OK;
}

//----------------------------------------------------------------------------
//  NV_STATUS getNbsiDirSize( pGpu, idx,  pNbsiDir,
//                            pRtnDirSize, tblSource);
//
//  This function tests the integrity of the NBSI dir object
//
//  Input parameters:
//      pGpu                         pointer to gpu object
//      idx                          index number of gpu
//      pNbsiDir                     pointer to nbsi directory object
//      pRtnDirSize                  pointer to return size of dir header
//                                   (including globs types)
//      tblSource                    indicator where the table came from.
//
//  Output parameters:
//      NV_STATUS:                   NV_OK if the directory version is okay and
//                                   the directory fits in the allocated mem.
//
//----------------------------------------------------------------------------

static NV_STATUS getNbsiDirSize
(
    OBJGPU         *pGpu,
    NvU32           idx,
    PNBSI_DIRECTORY pNbsiDir,
    NvU32         * pRtnDirSize,
    NvU16           tblSource
)
{
    NvU8            nbsiDirVer;

    NV_ASSERT(pNbsiDir);
    NV_ASSERT(pRtnDirSize);

    if (pNbsiDir->d.nbsiHeaderString == NBSIDIRHDRSTRING)
    {
        nbsiDirVer = pNbsiDir->d.dirVer;
        *pRtnDirSize =
                   sizeof(pNbsiDir->d.nbsiHeaderString) +
                   sizeof(pNbsiDir->d.size) +
                   sizeof(pNbsiDir->d.numGlobs) +
                   sizeof(pNbsiDir->d.dirVer) +
                   (pNbsiDir->d.numGlobs * sizeof(pNbsiDir->d.globType[0]));
    }
    else
    {
        nbsiDirVer = pNbsiDir->od.dirVer;
        *pRtnDirSize =
                   sizeof(pNbsiDir->od.numGlobs) +
                   sizeof(pNbsiDir->od.dirVer) +
                   (pNbsiDir->od.numGlobs * sizeof(pNbsiDir->od.globType[0]));
    }

    if (nbsiDirVer > MAXNBSIDIRVER)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "GPU%d, source %d, NBSI dir ver %d > max ver %d.\n",
                  idx, tblSource, nbsiDirVer, MAXNBSIDIRVER);
        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

//----------------------------------------------------------------------------
//  NV_STATUS testNbsiDir( pGpu, idx,  pNbsiDir, allocSize, tblSource)
//
//  This function tests the integrity of the NBSI dir object
//
//  Input parameters:
//      pGpu                         pointer to gpu object
//      idx                          index number of gpu
//      pNbsiDir                     pointer to nbsi directory object
//      allocSize                    size of memory allocated for dir (for
//                                   testing)
//      tblSource                    location of table (for error messages)
//
//  Output parameters:
//      NV_STATUS:                   NV_OK if the directory version is okay and
//                                   the directory fits in the allocated mem.
//
//----------------------------------------------------------------------------

static NV_STATUS testNbsiDir
(
    OBJGPU         *pGpu,
    NvU32           idx,
    PNBSI_DIRECTORY pNbsiDir,
    NvU32           allocSize,
    NvU16           tblSource
)
{
    NvU32            testDirSize;
    NV_STATUS        status;

    NV_ASSERT(pNbsiDir);

    // Get the NBSI dir size (Varies with directory header).
    status = getNbsiDirSize(pGpu,
                            idx,
                            pNbsiDir,
                            &testDirSize,
                            tblSource);
    if (status != NV_OK)
    {
        return status;
    }

    if (testDirSize > allocSize)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "GPU%d, source %d, Size of NBSI dir %x > alloc mem %x.\n",
                  idx, tblSource, testDirSize, allocSize);
        return NV_ERR_GENERIC;
    }

    if (pNbsiDir->d.nbsiHeaderString == NBSIDIRHDRSTRING)
    {
        NV_PRINTF(LEVEL_INFO,
                  "nbsi dir (new fmt) entries=%x, source=%x\n",
                  pNbsiDir->d.numGlobs, tblSource);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "nbsi dir (old fmt) entries=%x, source=%x\n",
                  pNbsiDir->od.numGlobs, tblSource);
    }

    return NV_OK;
}

// useful defines to make calls to testNbsiTable more readable.
#define TEST_NBSI_TABLE_DO_ALLOC_SIZE_CHECK   NV_TRUE
#define TEST_NBSI_TABLE_SKIP_ALLOC_SIZE_CHECK NV_FALSE
#define TEST_NBSI_TABLE_DO_HASH_CHECK         NV_TRUE
#define TEST_NBSI_TABLE_SKIP_HASH_CHECK       NV_FALSE
#define TEST_NBSI_TABLE_DO_GLOBTYPE_CHECK     NV_TRUE
#define TEST_NBSI_TABLE_SKIP_GLOBTYPE_CHECK   NV_FALSE

//----------------------------------------------------------------------------
//  NV_STATUS testNbsiTable( pGpu, pNbsiGenObj, wantedGlobType,
//                           i, idx, tableLoc, allocSize,
//                           bDoAllocSzCk, bDoHashCk, bGlobTypeCk )
//
//  This function tests an existing NBSI table.
//
//  Input parameters:
//      pGpu               Current pGpu object
//      pNbsiGenObj        Current driver object to sanity check
//      wantedGlobType     glob type that we want
//      i                  index within directory.
//      idx                GPU instance / index
//      tableLoc           indicator on the source of the table.
//      allocSize          memory allocated for table
//      bDoAllocSzCk       flag indicating to do allocated mem vs table test
//      bDoHashCk          flag indicating to do hash check test
//      bGlobTypeCk        flag indicating to do globtype test
//
//  Output parameters:
//      NV_STATUS:         NV_OK if there were no operational issues
//                         NV_ERR_GENERIC if no table is present or table is bad.
//
//----------------------------------------------------------------------------

static NV_STATUS testNbsiTable
(
    OBJGPU         *pGpu,
    PNBSI_GEN_OBJ   pNbsiGenObj,
    NvU16           wantedGlobType,
    NvU32           i,
    NvU32           idx,
    NvU16           tableLoc,
    NvU32           allocSize,
    NvBool          bDoAllocSzCk,
    NvBool          bDoHashCk,
    NvBool          bGlobTypeCk
)
{
    NV_STATUS        status = NV_ERR_GENERIC;
    PNBSI_DRIVER_OBJ pNbsiDriverObj = (PNBSI_DRIVER_OBJ) pNbsiGenObj;

    NV_ASSERT(pNbsiGenObj);
    NV_ASSERT(allocSize);

    if (pNbsiGenObj && allocSize)
    {
        status = NV_OK;

        // Sanity check 1 for globSize... must be larger than NBSI_MIN_GEN_OBJ_SIZE
        if (pNbsiGenObj->objHdr.size < NBSI_MIN_GEN_OBJ_SIZE)
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "NBSI Gpu%d Tloc=%d/Glob=%d table size 0x%x < min 0x%x\n",
                      idx, tableLoc, i,
                      pNbsiGenObj->objHdr.size, (NvU32)NBSI_MIN_GEN_OBJ_SIZE);
            status = NV_ERR_GENERIC;
        }

        // Sanity check 2 for globSize... must be less than maximum
        if (pNbsiGenObj->objHdr.size > NBSI_MAX_TABLE_SIZE)
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "NBSI Gpu%d Tloc=%d/Glob=%d tbl size 0x%x > max 0x%x\n",
                      idx, tableLoc, i,
                      pNbsiGenObj->objHdr.size, NBSI_MAX_TABLE_SIZE);
            status = NV_ERR_GENERIC;
        }

        // Sanity check 3 for globSize... must be less than allocated size
        if (bDoAllocSzCk & (pNbsiGenObj->objHdr.size > allocSize))
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "NBSI Gpu%d Tloc=%d/Glob=%d table size 0x%x > alloc 0x%x\n",
                      idx, tableLoc, i,
                      pNbsiGenObj->objHdr.size, allocSize);
            status = NV_ERR_GENERIC;
        }

        // Sanity check for number of modules... must be less than max
        if ((pNbsiGenObj->objHdr.globType == NBSI_DRIVER) &&
            (pNbsiDriverObj->numModules > NV2080_CTRL_BIOS_NBSI_NUM_MODULES))
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "NBSI Gpu%d Tloc=%d/Glob=%d numModules %d > max %d\n",
                      idx, tableLoc, i,
                      pNbsiDriverObj->numModules,
                      NV2080_CTRL_BIOS_NBSI_NUM_MODULES);
            status = NV_ERR_GENERIC;
        }

        // Sanity check globType matches the one we wanted.
        if ((pNbsiGenObj->objHdr.globType != wantedGlobType) && bGlobTypeCk)

        {
            NV_PRINTF(LEVEL_NOTICE,
                      "NBSI Gpu%d Tloc=%d/Glob=%d wantedGlobType = %04x != returned globtype = %04x\n",
                      idx, tableLoc, i, wantedGlobType,
                      pNbsiGenObj->objHdr.globType);

            //
            // BUG 986051 Regression... Adding the globtype confirmation to
            // prevent a BSOD, exposed an ASL problem in the C89 SBIOS, where
            // the SBIOS returned the Platform Info Object with a reversed
            // globtype (PI vs IP). Since we won't likely get a fixed SBIOS...
            // allow this exception.
            //

            if ((wantedGlobType != NBSI_PLAT_INFO) ||
                ((wantedGlobType == NBSI_PLAT_INFO) && (pNbsiGenObj->objHdr.globType != NBSI_PLAT_INFO_WAR)))
            {
                status = NV_ERR_GENERIC;
            }
        }

        // Now bail if we've found any errors before the hash calc.
        if (status == NV_OK && bDoHashCk)
        {
            status = testObjectHash(pGpu, pNbsiGenObj);
            if (status != NV_OK)
            {
                // bad hash error message occurred in testObjectHash
                NV_PRINTF(LEVEL_NOTICE,
                          "NBSI Gpu%d TLoc=%d/globType=%x bad hash\n",
                          idx, tableLoc,
                          pNbsiGenObj->objHdr.globType);
            }
        }
    }

    return status;
}

//----------------------------------------------------------------------------
//  NvU8 checkUidMatch(pGpu, pNbsiGenObj, idx, tableLoc, globNdx,
//                     pDriverVersion)
//
//  This function tests if the table matches our platform/device and
//  returns a score of matching. 0 matches nothing, 5 matches all.
//  Search qualifiers are:
//      - First UID matching the Platform SVID/SSID and Chipset VID/DID/Rev
//      - First UID matching the Platform SVID and Chipset VID/DID/Rev
//      - First matching Chipset VID/DID/Rev
//      - First matching Chipset VID/DID
//      - First matching Chipset VID
//
//  Input parameters:
//      pGpu          Current pGpu object
//      pNbsiGenObj   nbsi driver object to test (NULL for dbg print)
//      idx           GPU instance / index
//      tableLoc      indicator on the source of the table.
//      globNdx       index of glob (note if using get object by type
//                    this is index of driver objects NOT all object
//                    types.
//      pDriverVersion pointer to return version (if best fit > 0)
//
//  Output parameters:
//      NvU8          Score 0..5 5 is best fit, 0 is least fit
//      NvU32         Driver version is returned if score > 0.
//
//----------------------------------------------------------------------------
static NvU8 checkUidMatch
(
    OBJGPU          *pGpu,
    PNBSI_DRIVER_OBJ pNbsiGenObj,
    NvU32            idx,
    NvU16            tableLoc,
    NvU8             globNdx,
    NvU32          * pDriverVersion
)
{
    PNBSI_DRIVER_OBJ0 pNbsiGenObj0 = (PNBSI_DRIVER_OBJ0) pNbsiGenObj;
    NBSI_OBJ         *pNbsiObj     = getNbsiObject();
    NvU8  score = 0;
    NvU16 ssid = (NvU16) (pGpu->idInfo.PCISubDeviceID >> 16) & 0xffff;
    NvU16 svid = (NvU16) pGpu->idInfo.PCISubDeviceID & 0xffff;
    NvU16 cdid = (NvU16) (pGpu->idInfo.PCIDeviceID >> 16) & 0xffff;
    NvU16 cvid = (NvU16) pGpu->idInfo.PCIDeviceID & 0xffff;
    NvU8  crev = (NvU8)  pGpu->idInfo.PCIRevisionID & 0xff;
    NvU32 tRev = pNbsiObj->DriverVer.Rev;
    NvU32 tDX  = pNbsiObj->DriverVer.DX;
    NvU32 tOS  = pNbsiObj->DriverVer.OS;
    NvBool  pTestVersionOnVer0100 = NV_FALSE;

    if (pDriverVersion != NULL)
    {
        *pDriverVersion = 0;
    }

    if (pNbsiGenObj == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NBSI current driver version: %d.%d.1%d.%d\n",
                  pNbsiObj->DriverVer.OS,
                  pNbsiObj->DriverVer.DX,
                  pNbsiObj->DriverVer.Rev / 10000,
                  pNbsiObj->DriverVer.Rev % 10000);

        NV_PRINTF(LEVEL_INFO,
                  "NBSI Gpu%d ID info: svid=%x, ssid=%x, cvid=%x, cdid=%x, crev=%x\n",
                  idx, svid, ssid, cvid, cdid, crev);
    }
    else
    {
        if (pNbsiGenObj0->objHdr.majMinVer == NBSI_DRIVERVER_0100)
        {
            //
            // check driver rev. If specified can be greater or equal to to use.
            // if value was 0, assume current version.
            // Driver revision example: 6.14.11.7782
            // The format is: a.bb.1c.dddd a is OS, bb is DX, Rev is cdddd
            //
            if (pNbsiGenObj0->uid.Driver.majVer)
            {
                tOS = pNbsiGenObj0->uid.Driver.majVer;
            }
            if (pNbsiGenObj0->uid.Driver.minVer)
            {
                tDX = pNbsiGenObj0->uid.Driver.minVer;
            }
            if (pNbsiGenObj0->uid.Driver.majRev)
            {
                //
                // this doesn't work correctly since the size is wrong
                // fixed in v 0x101 of header
                //
                tRev = (pNbsiGenObj0->uid.Driver.majRev * 100) +
                       pNbsiGenObj0->uid.Driver.minRev;
            }
            if ((pTestVersionOnVer0100) &&
                ((tOS != pNbsiObj->DriverVer.OS) ||
                (tDX != pNbsiObj->DriverVer.DX) ||
                (tRev < pNbsiObj->DriverVer.Rev)))
            {
                score = 0;
                tRev = 0;
            }
            else
            {
                if ((pNbsiGenObj0->uid.svid &&
                                 (pNbsiGenObj0->uid.svid == svid)) &&
                    (pNbsiGenObj0->uid.ssid &&
                                 (pNbsiGenObj0->uid.ssid == ssid)) &&
                    (pNbsiGenObj0->uid.Chip.vid &&
                                 (pNbsiGenObj0->uid.Chip.vid == cvid)) &&
                    (pNbsiGenObj0->uid.Chip.did &&
                                  (pNbsiGenObj0->uid.Chip.did == cdid)) &&
                    (pNbsiGenObj0->uid.Chip.revId &&
                                  (pNbsiGenObj0->uid.Chip.revId == crev)))
                {
                    score = 5;
                }
                else if ((pNbsiGenObj0->uid.svid       == svid) &&
                         (pNbsiGenObj0->uid.Chip.vid   == cvid) &&
                         (pNbsiGenObj0->uid.Chip.did   == cdid) &&
                         (pNbsiGenObj0->uid.Chip.revId == crev))
                {
                    score = 4;
                }
                else if ((pNbsiGenObj0->uid.Chip.vid   == cvid) &&
                         (pNbsiGenObj0->uid.Chip.did   == cdid) &&
                         (pNbsiGenObj0->uid.Chip.revId == crev))
                {
                    score = 3;
                }
                else if ((pNbsiGenObj0->uid.Chip.vid   == cvid) &&
                         (pNbsiGenObj0->uid.Chip.did   == cdid))
                {
                    score = 2;
                }
                else if (pNbsiGenObj0->uid.Chip.vid   == cvid)
                {
                    score = 1;
                }
            }
        }
        else
        {
            //
            // check driver rev. If specified can be greater or equal to to use.
            // if value was 0, assume current version.
            // Driver revision example: 6.14.11.7782
            // The format is: a.bb.1c.dddd a is OS, bb is DX, Rev is cdddd
            //
            if (pNbsiGenObj->uid.Driver.OS)
            {
                tOS = pNbsiGenObj->uid.Driver.OS;
            }
            if (pNbsiGenObj->uid.Driver.DX)
            {
                tDX = pNbsiGenObj->uid.Driver.DX;
            }
            if (pNbsiGenObj->uid.Driver.Rev)
            {
                tRev = pNbsiGenObj->uid.Driver.Rev;
            }
            if ((tOS != pNbsiObj->DriverVer.OS) ||
                (tDX != pNbsiObj->DriverVer.DX) ||
                (tRev < pNbsiObj->DriverVer.Rev))
            {
                score = 0;
                tRev = 0;
            }
            else
            {
                if (pDriverVersion != NULL)
                {
                    *pDriverVersion = tRev;
                }

                if ((pNbsiGenObj->uid.svid &&
                                 (pNbsiGenObj->uid.svid == svid)) &&
                    (pNbsiGenObj->uid.ssid &&
                                 (pNbsiGenObj->uid.ssid == ssid)) &&
                    (pNbsiGenObj->uid.Chip.vid &&
                                 (pNbsiGenObj->uid.Chip.vid == cvid)) &&
                    (pNbsiGenObj->uid.Chip.did &&
                                  (pNbsiGenObj->uid.Chip.did == cdid)) &&
                    (pNbsiGenObj->uid.Chip.revId &&
                                  (pNbsiGenObj->uid.Chip.revId == crev)))
                {
                    score = 5;
                }
                else if ((pNbsiGenObj->uid.svid       == svid) &&
                         (pNbsiGenObj->uid.Chip.vid   == cvid) &&
                         (pNbsiGenObj->uid.Chip.did   == cdid) &&
                         (pNbsiGenObj->uid.Chip.revId == crev))
                {
                    score = 4;
                }
                else if ((pNbsiGenObj->uid.Chip.vid   == cvid) &&
                         (pNbsiGenObj->uid.Chip.did   == cdid) &&
                         (pNbsiGenObj->uid.Chip.revId == crev))
                {
                    score = 3;
                }
                else if ((pNbsiGenObj->uid.Chip.vid   == cvid) &&
                         (pNbsiGenObj->uid.Chip.did   == cdid))
                {
                    score = 2;
                }
                else if (pNbsiGenObj->uid.Chip.vid   == cvid)
                {
                    score = 1;
                }
            }
        }

        NV_PRINTF(LEVEL_INFO,
                  "NBSI Gpu%d Tloc=%d/Glob=%d object match score/ver = (%x/%d)\n",
                  idx, tableLoc, globNdx, score, tRev);
    }

    return score;
}

//----------------------------------------------------------------------------
//  NV_STATUS extractNBSIObjFromDir(pGpu, idx,
//                                  pNbsiDir, nbsiDirSize, tableLoc,
//                                  wantedGlobType, wantedGlobIndex,
//                                  * pActualGlobIdx,
//                                  * pRtnObj, * pRtnObjSize, * pbFound)
//
//  This function extracts an NBSI object from a memory based (as opposed
//  to ACPI) NBSI directory.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      pNbsiDir              pointer to nbsi directory
//      nbsiDirSize           size of nbsiDir
//      tableLoc              See NBSI_TABLE_LOC_xxxx defines (for debug)
//      wantedGlobType        wanted Glob Type
//      wantedGlobIndex       index of glob type desired (0=best fit)
//      pActualGlobIdx        pointer to actual glob index glob found at
//
//  Output parameters:
//      NV_STATUS             NV_ERR_GENERIC if table structure is bad.
//                            NV_OK if table structure is good.
//      pRtnObj               pointer to memory to hold return object
//      pRtnObjSize           size of memory holding return object
//      pbFound               pointer to NvBool indicating object found.
//
//----------------------------------------------------------------------------

static NV_STATUS extractNBSIObjFromDir
(
    OBJGPU          *pGpu,
    NvU32            idx,
    PNBSI_DIRECTORY  pNbsiDir,
    NvU32            nbsiDirSize,
    NvU16            tableLoc,
    NBSI_GLOB_TYPE   wantedGlobType,
    NvU8             wantedGlobIdx,
    NvU8           * pActualGlobIdx,
    PNBSI_GEN_OBJ  * pRtnObj,
    NvU32          * pRtnObjSize,
    NBSI_VALIDATE    validationOption,
    NvBool         * pbFound
)
{
    NV_STATUS       status;
    PNBSI_GEN_OBJ   pNbsiGenObj;
    NvU8            numGlobs;
    NvU8            curGlob;
    NvU8            cntOfGlobsWithWantedGlobType;
    NvBool          bMyFound;
    NvU8            cntOfMatchingGlobTypes;
    NvU8            thisScore;
    NvU8            bestDriverObjectMatchScore = 0;
    PNBSI_GEN_OBJ   pBestDriverObjectMatch = NULL;
    NvU8            bestDriverObjectMatchGlob = 0;
    NvU8          * tPtr;
    NvU32           testDirSize;
    NvU32           driverVersion = 0;
    NvU32           bestFitDriverVersion = 0;
    NvBool          bCheckCRC = NV_TRUE;

    NV_ASSERT(pNbsiDir);
    NV_ASSERT(pbFound);
    NV_ASSERT(pRtnObjSize);
    NV_ASSERT(pRtnObj);
    NV_ASSERT(pActualGlobIdx);

    *pbFound = NV_FALSE;

    // test the directory structure integrity
    status = testNbsiDir(pGpu,
                         idx,
                         pNbsiDir,
                         nbsiDirSize,
                         tableLoc);
    if (status != NV_OK)
    {
        return status;
    }

    if (wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR)
    {
        // they want it all.
        *pbFound = NV_TRUE;
        *pRtnObj = (PNBSI_GEN_OBJ) pNbsiDir;
        *pRtnObjSize = nbsiDirSize;
        *pActualGlobIdx = 1;
        return NV_OK;
    }

    // Check if the glob type we want is in the directory
    status = isGlobTypeInNbsiDir(pNbsiDir,
                                 wantedGlobType,
                                 &cntOfMatchingGlobTypes,
                                 &numGlobs);
    if (status != NV_OK)
    {
        return status;
    }

    if (cntOfMatchingGlobTypes == 0)
    {
        return NV_OK;
    }

    // Get the NBSI dir size (Varies with directory header).
    status = getNbsiDirSize(pGpu,
                            idx,
                            pNbsiDir,
                            &testDirSize,
                            tableLoc);
    if (status != NV_OK)
    {
        return status;
    }

    // point at the first object in the directory to start
    tPtr = (NvU8 *) pNbsiDir;
    tPtr = &tPtr[testDirSize];
    pNbsiGenObj = (PNBSI_GEN_OBJ) tPtr;

    curGlob = 0;
    cntOfGlobsWithWantedGlobType = 0;
    bMyFound = NV_FALSE;
    while ((status == NV_OK) &&
           !bMyFound &&
           (curGlob < numGlobs) &&
           (cntOfGlobsWithWantedGlobType < cntOfMatchingGlobTypes))
    {

        if (validationOption == NBSI_VALIDATE_IGNORE_CRC)
        {
            bCheckCRC = NV_FALSE;
        }

        // Sanity check this object entry
        status = testNbsiTable(pGpu,
                               (PNBSI_GEN_OBJ) pNbsiGenObj,
                               wantedGlobType,
                               curGlob,
                               idx,
                               tableLoc,
                               nbsiDirSize,
                               TEST_NBSI_TABLE_DO_ALLOC_SIZE_CHECK,
                               bCheckCRC,
                               TEST_NBSI_TABLE_SKIP_GLOBTYPE_CHECK);

        if (status != NV_OK)
        {
            // Bad table format. Get out now.
            return status;
        }

        if (pNbsiGenObj->objHdr.globType == wantedGlobType)
        {
            cntOfGlobsWithWantedGlobType++;
            if (!wantedGlobIdx ||
                (cntOfGlobsWithWantedGlobType == wantedGlobIdx))
            {
                // if wantedGlobIdx == 0 and driver object do best match
                if (!wantedGlobIdx && (pNbsiGenObj->objHdr.globType == NBSI_DRIVER))
                {
                    // Check the match score for this table
                    thisScore = checkUidMatch(pGpu,
                                              (PNBSI_DRIVER_OBJ) pNbsiGenObj,
                                              idx,
                                              tableLoc,
                                              curGlob,
                                              &driverVersion);

                    // Keep track of the best match
                    if ((thisScore > 0) &&
                        ((thisScore > bestDriverObjectMatchScore) ||
                        ((thisScore == bestDriverObjectMatchScore) &&
                         (driverVersion > bestFitDriverVersion))))
                    {
                        pBestDriverObjectMatch = pNbsiGenObj;
                        bestDriverObjectMatchGlob = curGlob;
                        bestDriverObjectMatchScore = thisScore;
                        bestFitDriverVersion = driverVersion;
                    }
                }
                else
                {
                    bMyFound = NV_TRUE;
                }
            }
        }

        // if not found yet move to the next object
        if (!bMyFound)
        {
            tPtr = (NvU8 *) pNbsiGenObj;
            tPtr = &tPtr[pNbsiGenObj->objHdr.size];
            pNbsiGenObj = (PNBSI_GEN_OBJ) tPtr;
            curGlob++;
        }
    }

    if ((status == NV_OK) &&
        (pBestDriverObjectMatch != NULL) &&
        (wantedGlobType == NBSI_DRIVER) &&
        !wantedGlobIdx)
    {
        // replace last checked with best fit driver glob
        pNbsiGenObj = pBestDriverObjectMatch;
        curGlob = bestDriverObjectMatchGlob;
        bMyFound = NV_TRUE;
        NV_PRINTF(LEVEL_INFO,
                  "NBSI Gpu%d Tloc=%d/Glob=%d best fit (score/ver = (%x/%d))\n",
                  idx, tableLoc, bestDriverObjectMatchGlob,
                  bestDriverObjectMatchScore, bestFitDriverVersion);
    }

    if ((status == NV_OK) && bMyFound)
    {
        *pbFound = bMyFound;
        *pRtnObj = pNbsiGenObj;
        *pRtnObjSize = pNbsiGenObj->objHdr.size;
        *pActualGlobIdx = curGlob;
    }

    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS allocNbsiCache(pGpu, idx,  cacheEntries)
//
//  This function allocates a cache table (array) to hold the nbsi object cache
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      cacheEntries          Number of entries to allow
//
//  Output parameters:
//      NV_STATUS             NV_ERR_GENERIC if unable to allocate memory
//                            NV_OK if memory is allocated for cache table.
//
//----------------------------------------------------------------------------

#define NBSI_INITCACHEENTRYCNT 5
static NV_STATUS allocNbsiCache
(
    OBJGPU    *pGpu,
    NvU32      idx,
    NvU8       cacheEntries
)
{
    NvU32           cacheSize;
    NBSI_CACHE_OBJ  cacheObj;
    PNBSI_CACHE_OBJ pCacheObj;
    PNBSI_CACHE_ENTRY_OBJ pCacheEntryObj;
    NBSI_OBJ *pNbsiObj = getNbsiObject();
    NvU8            i;

    NV_ASSERT(pNbsiObj->pTblCache[idx] == NULL);

    cacheSize = sizeof(cacheObj) +
                (sizeof(pCacheEntryObj) * (cacheEntries - 1));
    pNbsiObj->pTblCache[idx] = portMemAllocNonPaged(cacheSize);
    if (pNbsiObj->pTblCache[idx] == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Unable to allocate 0x%x memory for NBSI cache.\n",
                  cacheSize);
        return NV_ERR_NO_MEMORY;
    }
    pCacheObj = pNbsiObj->pTblCache[idx];
    pCacheObj->tblCacheNumEntries = 0;
    pCacheObj->tblCacheMaxNumEntries = cacheEntries;
    for (i = 0; i < cacheEntries; i++)
    {
        pCacheObj->pCacheEntry[i] = NULL;
    }
    return NV_OK;
}

//----------------------------------------------------------------------------
//  NV_STATUS addNbsiCacheEntry(pGpu, idx,  pTestObj, testObjSize,
//                              testObjIndex, actualGlobIdx,
//                              wantedGlobSource, globSource)
//
//  This function allocates memory to hold a cached copy of an nbsi object.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      pTestObj              pointer to object to add to the cache
//      testObjSize           size of object to add to the cache.
//      testObjIndex          index the object was found in table.
//      actualGlobIdx         actual glob idx for glob (when testObjIndex=0)
//      wantedGlobSource      Original desired source directory
//      globSource            Source of the object
//
//  Output parameters:
//      NV_STATUS             NV_ERR_GENERIC if unable to allocate memory
//                            NV_OK if memory is allocated for cache table.
//
//  Note: If there is not enough room in the cache, the add is skipped.
//        This could be changed in the future (by allocating  new larger cache
//        table, copying the existing table to new larger cache table and
//        increase the entry count. Then releasing the previous memory).
//
//----------------------------------------------------------------------------

static NV_STATUS addNbsiCacheEntry
(
    OBJGPU         *pGpu,
    NvU32           idx,
    PNBSI_GEN_OBJ   pTestObj,
    NvU32           testObjSize,
    NBSI_SOURCE_LOC wantedGlobSource,
    NvU8            wantedObjIndex,
    NBSI_SOURCE_LOC actualGlobSource,
    NvU8            actualGlobIdx
)
{
    NvU8            cacheNdx;
    PNBSI_CACHE_OBJ pCacheObj;
    NBSI_CACHE_ENTRY_OBJ cacheEntry;
    NBSI_OBJ       *pNbsiObj = getNbsiObject();
    NvU16           szOfCacheEntry = sizeof(cacheEntry);

    NV_ASSERT(pTestObj);

    if (pNbsiObj->pTblCache[idx] != NULL)
    {
        pCacheObj = pNbsiObj->pTblCache[idx];
        cacheNdx = pCacheObj->tblCacheNumEntries;

        //
        // in the future we may allow growing the table dynamically
        // but for now just limit it.
        //
        if (cacheNdx < pCacheObj->tblCacheMaxNumEntries)
        {
            // Allocate memory for the new cache entry
            pCacheObj->pCacheEntry[cacheNdx] = portMemAllocNonPaged(szOfCacheEntry);

            if (pCacheObj->pCacheEntry[cacheNdx] == NULL)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "Unable to alloc 0x%x mem for NBSI cache entry\n",
                          szOfCacheEntry);
                return NV_ERR_NO_MEMORY;
            }

            // Allocate memory for the new cache entry object
            pCacheObj->pCacheEntry[cacheNdx]->pObj = portMemAllocNonPaged(testObjSize);

            if (pCacheObj->pCacheEntry[cacheNdx]->pObj == NULL)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "Unable to alloc 0x%x mem for NBSI cache entry\n",
                          testObjSize);
                return NV_ERR_NO_MEMORY;
            }
            portMemCopy(pCacheObj->pCacheEntry[cacheNdx]->pObj, testObjSize, pTestObj, testObjSize);

            pCacheObj->pCacheEntry[cacheNdx]->globType = pTestObj->objHdr.globType;
            pCacheObj->pCacheEntry[cacheNdx]->globSource = wantedGlobSource;
            pCacheObj->pCacheEntry[cacheNdx]->globIndex = wantedObjIndex;
            pCacheObj->pCacheEntry[cacheNdx]->altGlobSource =
                                                           actualGlobSource;
            pCacheObj->pCacheEntry[cacheNdx]->altGlobIndex = actualGlobIdx;
            cacheNdx++;
            pCacheObj->tblCacheNumEntries = cacheNdx;
        }
    }
    return NV_OK;
}

//----------------------------------------------------------------------------
//  NV_STATUS freeNbsiCache(pGpu, idx)
//
//  This function frees the memory used for the cache entries and the cache
//  table.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//
//  Output parameters:
//      NV_STATUS             NV_OK
//
//----------------------------------------------------------------------------

static NV_STATUS freeNbsiCache
(
    OBJGPU    *pGpu,
    NvU32      idx
)
{
    NBSI_OBJ       *pNbsiObj = getNbsiObject();
    PNBSI_CACHE_OBJ pCacheObj;
    NvU8            i;

    if (pNbsiObj->nbsiDrvrTable[idx] != NULL)
    {
        //
        // the nbsiDrvrTable is just a copy of a pointer in the cache, so we
        // just clear the pointer here, and release it with the rest of the
        // cache entries
        //

        pNbsiObj->nbsiDrvrTable[idx] = NULL;
    }

    // free the memory for the cache entries
    if (pNbsiObj->pTblCache[idx] != NULL)
    {
        pCacheObj = pNbsiObj->pTblCache[idx];
        for (i = 0; i < pCacheObj->tblCacheNumEntries; i++)
        {
            portMemFree(pCacheObj->pCacheEntry[i]->pObj);
            pCacheObj->pCacheEntry[i]->pObj = NULL;

            portMemFree(pCacheObj->pCacheEntry[i]);
            pCacheObj->pCacheEntry[i] = NULL;
        }

        // release the memory for the cache table.
        pCacheObj->tblCacheNumEntries = 0;
        portMemFree(pNbsiObj->pTblCache[idx]);
        pNbsiObj->pTblCache[idx] = NULL;
    }
    return NV_OK;
}

//----------------------------------------------------------------------------
//  NV_STATUS getNbsiCacheInfoForGlobType(pGpu, idx,
//                                       globType,
//                                       *pWantedGlobSource, *pWantedGlobIndex,
//                                       *pNbsiDir, *pNbsiDirSize, *pCurTbl)
//
//  This function checks if an object with the globType is in the cache and
//  if it is, returns it's info.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      globType              wanted globtype for cache entry.
//      pWantedGlobIndex      wanted glob index
//      pWantedGlobSource     wanted glob source
//
//  Output parameters:
//      NV_STATUS             NV_OK if object was found in the cache
//                            NV_ERR_GENERIC if the object was not found in the cache
//      pNbsiDir              pointer to object
//      pNbsiDirSize          size of object found
//      pCurTbl               index of cache object entry in table
//
//----------------------------------------------------------------------------

static NV_STATUS getNbsiCacheInfoForGlobType
(
    OBJGPU            *pGpu,
    NvU32              idx,
    NBSI_GLOB_TYPE     globType,
    NBSI_SOURCE_LOC  * pWantedGlobSource,
    NvU8             * pWantedGlobIndex,
    PNBSI_GEN_OBJ    * pNbsiDir,
    NvU32            * pNbsiDirSize,
    NvU8             * pCurTbl
)
{
    NBSI_OBJ       *pNbsiObj = getNbsiObject();
    PNBSI_CACHE_OBJ pCacheObj;
    PNBSI_GEN_OBJ   pGenObj;
    NvU8            curTbl;

    NV_ASSERT(pWantedGlobSource);
    NV_ASSERT(pWantedGlobIndex);
    NV_ASSERT(pNbsiDir);
    NV_ASSERT(pNbsiDirSize);
    NV_ASSERT(pCurTbl);

    if (pNbsiObj->pTblCache[idx] == NULL)
    {
        return NV_ERR_GENERIC;
    }
    pCacheObj = pNbsiObj->pTblCache[idx];
    // Check if a cache entry for this globType/wanted idx table exists
    *pCurTbl = 0;

    for (curTbl = 0; curTbl < pCacheObj->tblCacheNumEntries; curTbl++)
    {
        if ((pCacheObj->pCacheEntry[curTbl]->globType == globType) &&
            ((pCacheObj->pCacheEntry[curTbl]->pObj != NULL)))
        {
            //
            // This matches what we want if the globType is correct (dugh) and
            // either we want to catch the case where they ask for best fit and
            // later ask for the actual source/index  we don't want to
            // duplicate this entry. The cached entry saves 1) whether this was
            // a best fit request in the beginning and it's index
            // first check for best fit request.
            if ((pCacheObj->pCacheEntry[curTbl]->globSource ==
                                                *pWantedGlobSource) &&
                (pCacheObj->pCacheEntry[curTbl]->globIndex ==
                                                *pWantedGlobIndex))
            {
                pGenObj = (PNBSI_GEN_OBJ) pCacheObj->pCacheEntry[curTbl]->pObj;
                *pNbsiDir = pGenObj;
                *pNbsiDirSize = pGenObj->objHdr.size;
                if (pCacheObj->pCacheEntry[curTbl]->globSource == 0)
                {
                    *pWantedGlobSource =
                                 pCacheObj->pCacheEntry[curTbl]->altGlobSource;
                    *pWantedGlobIndex =
                                 pCacheObj->pCacheEntry[curTbl]->altGlobIndex;
                }
                else
                {
                    *pWantedGlobSource =
                                  pCacheObj->pCacheEntry[curTbl]->globSource;
                    *pWantedGlobIndex =
                                  pCacheObj->pCacheEntry[curTbl]->globIndex;
                }
                *pCurTbl = curTbl;
                return NV_OK;
            }

            if ((pCacheObj->pCacheEntry[curTbl]->altGlobSource ==
                                                         *pWantedGlobSource) &&
                (pCacheObj->pCacheEntry[curTbl]->altGlobIndex ==
                                                         *pWantedGlobIndex))
            {
                pGenObj = (PNBSI_GEN_OBJ) pCacheObj->pCacheEntry[curTbl]->pObj;
                *pNbsiDir = pGenObj;
                *pNbsiDirSize = pGenObj->objHdr.size;
                *pWantedGlobSource =
                                 pCacheObj->pCacheEntry[curTbl]->altGlobSource;
                *pWantedGlobIndex =
                                 pCacheObj->pCacheEntry[curTbl]->altGlobIndex;
                *pCurTbl = curTbl;
                return NV_OK;
            }
        }
    }
    return NV_ERR_GENERIC;
}

//----------------------------------------------------------------------------
//  NV_STATUS getNbsiObjFromCache(pGpu, idx,  globType,
//                                *pWantedGlobSource, *pWantedGlobIdx
//                                *pRtnObj, *pRtnObjSize, *globTypeRtnStatus
//
//  This function returns an object from the cache to the callers memory (if
//  it's large enough). If the object is present but the return memory is not
//  large enough to hold it, it returns it's size and sets the rtn status to
//  incomplete.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      globType              wanted globtype
//      pWantedGlobSource     wanted glob source / returned glob source
//      pWantedGlobIdx        wanted glob index
//
//  Output parameters:
//      NV_STATUS             NV_OK if object was found in the cache
//                            NV_ERR_GENERIC if the object was not found in the cache
//      pRtnObj               pointer to memory to hold object
//      pRtnObjSize           pointer to size of memory allocated for object
//      pTotalObjSize         pointer to return total object size
//      globTypeRtnStatus     pointer to return status for extra return info.
//                            NV2080_CTRL_BIOS_GET_NBSI_SUCCESS indicates
//                            object fit in the pRtnObjSize
//                            NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE indicates
//                            object was found in cache but is too large for
//                            allocated memory (as per pRtnObjSize). The
//                            required pRtnObjSize to hold the object is
//                            returned.
//
//----------------------------------------------------------------------------

static NV_STATUS getNbsiObjFromCache
(
    OBJGPU           *pGpu,
    NvU32             idx,
    NBSI_GLOB_TYPE    globType,
    NBSI_SOURCE_LOC * pWantedGlobSource,
    NvU8            * pWantedGlobIdx,
    NvU32             rtnObjOffset,
    PNBSI_GEN_OBJ     pRtnObj,
    NvU32           * pRtnObjSize,
    NvU32           * pTotalObjSize,
    NvU32           * globTypeRtnStatus
)
{
    NvU8          curTbl;
    PNBSI_GEN_OBJ pTempGlob = NULL;
    NvU32         tempGlobSize;
    NV_STATUS     status;
    NvU8 *        bufPtr;

    NV_ASSERT(pWantedGlobSource);
    NV_ASSERT(pWantedGlobIdx);
    NV_ASSERT(pRtnObjSize);
    NV_ASSERT(!(*pRtnObjSize != 0 && pRtnObj == NULL));
    NV_ASSERT(pTotalObjSize);
    NV_ASSERT(globTypeRtnStatus);

    // Check if the a cache entry for this NBSI_DRIVER table exists
    status = getNbsiCacheInfoForGlobType(pGpu,
                                         idx,
                                         globType,
                                         pWantedGlobSource,
                                         pWantedGlobIdx,
                                         &pTempGlob,
                                         &tempGlobSize,
                                         &curTbl);

    // If we found it in cache... return it (if they have enough room)
    if (status == NV_OK)
    {
        NvU32 rtnObjSizeWithOffset;

        // return the full table size
        *pTotalObjSize = tempGlobSize;

        if (!portSafeSubU32(*pTotalObjSize, rtnObjOffset, &rtnObjSizeWithOffset))
        {
            // Failed argument validation.
            status = NV_ERR_INVALID_OFFSET;
        }
        else
        {
            if (*pRtnObjSize >= rtnObjSizeWithOffset)
            {
                // if rtnsize is larger than remaining part of table,
                // then we can return it all this time.
                *pRtnObjSize = rtnObjSizeWithOffset;
                *globTypeRtnStatus = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
            }
            else
            {
                // return what we can and indicate incomplete.
                *globTypeRtnStatus = NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE;
            }

            if (*pRtnObjSize > 0)
            {
                bufPtr = (NvU8 *) pTempGlob;
                bufPtr = &bufPtr[rtnObjOffset];
                portMemCopy(pRtnObj, *pRtnObjSize, bufPtr, *pRtnObjSize);
            }
        }
    }
    return status;
}


//----------------------------------------------------------------------------
//  NV_STATUS getNbsiDirFromRegistry(pGpu, idx,
//                                   pNbsiDir, pNbsiDirSize)
//
//  This function determines if an emulated NBSI table exists in the registry
//  and reads it.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            (such as memory allocation failure)
//                            looking for the table... or no registry
//                            entry exists.
//      pNbsiDir              pointer to directory object.
//      pNbsiDirSize          size of directory object found.
//
//----------------------------------------------------------------------------

static NV_STATUS getNbsiDirFromRegistry
(
    OBJGPU           *pGpu,
    NvU32             idx,
    PNBSI_DIRECTORY * pNbsiDir,
    NvU32           * rtnNbsiDirSize
)
{
    NV_STATUS  status;
    NvU32      nbsiDirSize;

    NV_ASSERT_OR_RETURN( pNbsiDir, NV_ERR_INVALID_POINTER );
    NV_ASSERT_OR_RETURN( rtnNbsiDirSize, NV_ERR_INVALID_POINTER );

    // Check if the key is in the registry... and it's size
    nbsiDirSize = 0;
    status = osReadRegistryStringBase(pGpu,
                                      NV_REG_STR_EMULATED_NBSI_TABLE,
                                      (NvU8 *) *pNbsiDir,
                                      &nbsiDirSize);

    // size returned is non 0 so key is present.
    if (nbsiDirSize)
    {
        // do some minimal testing of first block?
        if (nbsiDirSize > NBSI_MAX_TABLE_SIZE)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Emulated NBSI table too big. 0x%x > than 0x%x!\n",
                      nbsiDirSize, NBSI_MAX_TABLE_SIZE);
            return NV_ERR_GENERIC;
        }

        // Allocate memory for the directory
        *pNbsiDir = portMemAllocNonPaged(nbsiDirSize);
        if (*pNbsiDir == NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Can't allocate 0x%x mem for emulated NBSI table.\n",
                      nbsiDirSize);
            return NV_ERR_NO_MEMORY;
        }

        // read the table in.
        status = osReadRegistryBinary(pGpu,
                                      NV_REG_STR_EMULATED_NBSI_TABLE,
                                      (NvU8 *) *pNbsiDir,
                                      &nbsiDirSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to read emulated NBSI table from reg.\n");
            portMemFree((void*)pNbsiDir);
            pNbsiDir = NULL;
            return status;
        }

        *rtnNbsiDirSize = nbsiDirSize;
        return NV_OK;
    }

    return NV_ERR_GENERIC;
}

//----------------------------------------------------------------------------
//  NV_STATUS determineACPIAccess(pGpu, idx,  pRtnMethod, acpiFunction)
//
//  This function checks if we have ACPI access to the NBSI table
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      pRtnMethod            status for methods available.
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//
//  Output parameters:
//      NV_STATUS:            NV_OK if we have access
//                            !NV_OK if we do not have ACPI access
//
//----------------------------------------------------------------------------

static NV_STATUS determineACPIAccess
(
    OBJGPU           *pGpu,
    NvU32             idx,
    PNBSI_ACPI_METHOD pRtnMethod,
    ACPI_DSM_FUNCTION acpiFunction
)
{
    NV_STATUS  status = NV_ERR_NOT_SUPPORTED;
    NV_STATUS  statusByType = NV_ERR_NOT_SUPPORTED;
    NV_STATUS  statusAllObj = NV_ERR_NOT_SUPPORTED;
    NvU32      getObjectByTypeSubfunction = NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE;
    NvU32      getAllObjectsSubfunction = NV_ACPI_GENERIC_FUNC_GETALLOBJS;

    NV_ASSERT(pRtnMethod);
    *pRtnMethod = NBSI_TBL_SOURCE_ACPI_UNKNOWN;

    // Determine the subfunctions for getobjectbytype and getallobjects
    status = getDsmGetObjectSubfunction(pGpu, &acpiFunction, &getObjectByTypeSubfunction, &getAllObjectsSubfunction);
    if (status != NV_OK)
    {
        return status;
    }

    status = testIfDsmSubFunctionEnabled(pGpu, acpiFunction, NV_ACPI_ALL_FUNC_SUPPORT);
    if (status != NV_OK)
    {
        return status;
    }

    statusByType = testIfDsmSubFunctionEnabled(pGpu, acpiFunction, getObjectByTypeSubfunction);
    statusAllObj = testIfDsmSubFunctionEnabled(pGpu, acpiFunction, getAllObjectsSubfunction);
    if ((statusByType != NV_OK) && (statusAllObj != NV_OK))
    {
        // return if both subfunctions are not supported get out
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        // one or both should work... indicate which
        if ((statusByType == NV_OK) && (statusAllObj == NV_OK))
        {
            *pRtnMethod = NBSI_TBL_SOURCE_ACPI_BOTH_METHODS;
        }
        else if (statusByType == NV_OK)
        {
            *pRtnMethod = NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE;
        }
        else if (statusAllObj == NV_OK)
        {
            *pRtnMethod = NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ;
        }
        status = NV_OK;
    }

    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS getNbsiDirectory(pGpu, idx,
//                             searchDir, *pNbsiDir, *pNbsiDirSize,
//                             curSource,
//                             *pbFreeDirMemRequired,
//                             *pAcpiMethod,
//                             acpiFunction)
//
//  This function gets the NBSI directory (in all but ACPI and UEFI cases)
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      curSource             source for the NBSI dir
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//
//  Output parameters:
//      NV_STATUS             NV_OK
//      *pNbsiDir             pointer to return directory found.
//      *activeDir            bitmap of directory found to search.
//      *pNbsiDirSize         pointer to the size of the NBSI dir.
//      *pbFreeDirMemRequired pointer to NvBool indicating if the directory
//                            being returned is in newly allocated memory (or
//                            just a pointer to existing memory ie VBIOS)
//      *pAcpiMethod          for ACPI based tables, access method(s)
//
//----------------------------------------------------------------------------

static NV_STATUS getNbsiDirectory
(
    OBJGPU             *pGpu,
    NvU32               idx,
    PNBSI_DIRECTORY   * pNbsiDir,
    NvU32             * pNbsiDirSize,
    NBSI_SOURCE_LOC     curSource,
    NvBool            * pbFreeDirMemRequired,
    PNBSI_ACPI_METHOD   pAcpiMethod,
    ACPI_DSM_FUNCTION   acpiFunction
)
{
    NV_STATUS         status;

    NV_ASSERT(pNbsiDir);
    NV_ASSERT(pNbsiDirSize);
    NV_ASSERT(pbFreeDirMemRequired);
    NV_ASSERT(pAcpiMethod);

    *pNbsiDir = NULL;
    *pNbsiDirSize = 0;
    *pbFreeDirMemRequired = NV_FALSE;

    status = NV_ERR_GENERIC;
    if (curSource & NBSI_TBL_SOURCE_REGISTRY)
    {
        status = getNbsiDirFromRegistry(pGpu,
                                        idx,
                                        pNbsiDir,
                                        pNbsiDirSize);
        if (status == NV_OK)
        {
            *pbFreeDirMemRequired = NV_TRUE;
        }
    }
    else if (curSource & NBSI_TBL_SOURCE_VBIOS)
    {
    return NV_ERR_NOT_SUPPORTED;
    }
    else if (curSource & NBSI_TBL_SOURCE_SBIOS)
    {
    return NV_ERR_NOT_SUPPORTED;
    }
    else if (curSource & NBSI_TBL_SOURCE_ACPI)
    {
        status = determineACPIAccess(pGpu, idx,  pAcpiMethod, acpiFunction);
    }
    else if (curSource & NBSI_TBL_SOURCE_UEFI)
    {
        status = NV_OK;
    }

    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS nbsiObjTypeCallAcpi(pGpu,
//                                acpiFunction,
//                                inOutData, inOutDataSz,
//                                outBuffer, outBufferSz, globTypeWanted,
//                                curGlob, * sizeToRead)
//
//  This function retrieves an ACPI based object.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      inOutData             ACPI inout buffer
//      inOutDataSz           ACPI inout buffer size
//      outBuffer             output buffer
//      outBufferSz           output buffer size
//      globTypeWanted        desired object Type
//      curGlob               glob index
//      *sizeToRead           size to read
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            Such as bad parameters or failed read.
//      rtnSize               set to amount read (always set to inOutDataSz)
//
//----------------------------------------------------------------------------
static NV_STATUS nbsiObjTypeCallAcpi
(
    OBJGPU             *pGpu,
    ACPI_DSM_FUNCTION   acpiFunction,
    NvU8              * inOutData,
    NvU32               inOutDataSz,
    NvU8              * outBuffer,
    NvU32               outBufferSz,
    NBSI_GLOB_TYPE      globTypeWanted,
    NvU8                curGlob,
    NvU32             * sizeToRead
)
{
    NV_STATUS   status = NV_OK;
    NvU32       leftToRead;
    NvU16       rtnSize;
    NvU16       acpiRtnSize = (NvU16) inOutDataSz;

    // curGlob fits in 15:12... so make sure we don't ask for anything higher (can happen if the SBIOS keeps sending us the same one).
    if (curGlob >= 16)
    {
        NV_PRINTF(LEVEL_NOTICE, "curGlob bits do not fit in 15:12. Returning early\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    leftToRead = *sizeToRead;
    *sizeToRead = 0;
    while ((status == NV_OK) && (leftToRead))
    {
        // 31:16 object type, 15:12 object instance, 11:0 page offset
        *(NvU32 *) inOutData = (globTypeWanted & 0xffff) << 16 |
                               (curGlob & 0xf) << 12 |
                               ((*sizeToRead/inOutDataSz) & 0xfff);
        status = osCallACPI_DSM(pGpu,
                                acpiFunction,
                                NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE,
                                (NvU32 *)inOutData,
                                (NvU16 *)&acpiRtnSize);

        if ((acpiRtnSize == 0) ||
            ((status == NV_OK) &&
             (acpiRtnSize==4) &&
             ((*(NvU32*)inOutData >= NVHG_ERROR_UNSPECIFIED) &&
              (*(NvU32*)inOutData <= 0x80000005) )))
        {
            status = NV_ERR_GENERIC;
        }

        if (status == NV_OK)
        {
            rtnSize = NV_MIN((NvU16) leftToRead, (NvU16) acpiRtnSize);
            NV_ASSERT((*sizeToRead+rtnSize)<=outBufferSz);
            portMemCopy(&outBuffer[*sizeToRead], rtnSize, inOutData, rtnSize);
            leftToRead -= rtnSize;
            *sizeToRead += rtnSize;
        }
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS nbsiObjTypeCallUefi(pGpu,
//                                inOutData, inOutDataSz,
//                                outBuffer, outBufferSz, globTypeWanted,
//                                curGlob, * sizeToRead)
//
//  This function retrieves an UEFI based object.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      inOutData             inout buffer
//      inOutDataSz           inout buffer size
//      outBuffer             output buffer
//      outBufferSz           output buffer size
//      globTypeWanted        desired object Type
//      curGlob               glob index
//      *sizeToRead           size to read
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            Such as bad parameters or failed read.
//      rtnSize               set to amount read (always set to inOutDataSz)
//
//----------------------------------------------------------------------------
static NV_STATUS nbsiObjTypeCallUefi
(
    OBJGPU             *pGpu,
    NvU8              * inOutData,
    NvU32               inOutDataSz,
    NvU8              * outBuffer,
    NvU32               outBufferSz,
    NBSI_GLOB_TYPE      globTypeWanted,
    NvU8                curGlob,
    NvU32             * sizeToRead
)
{
    NV_STATUS   status = NV_OK;
    NvU32       uefiRtnSize = inOutDataSz;
    char        uefiVariableName[] = "NBSI_GLOB_000";

    NV_ASSERT_OR_RETURN(curGlob < 16, NV_ERR_INVALID_ARGUMENT);

    // Per Matt's recommendation, only support glob type "OP" for now.
    if (globTypeWanted != NBSI_OPTIMUS_PLAT)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Change 000 in variable name to glob type and instance hex digit.
    uefiVariableName[10] = (globTypeWanted & 0xFF00) >> 8;
    uefiVariableName[11] = globTypeWanted & 0xFF;
    uefiVariableName[12] = curGlob + (curGlob < 10 ? '0' : 'A' - 10);

    status = osGetUefiVariable(uefiVariableName,
                               (LPGUID)&NV_GUID_UEFI_VARIABLE,
                               inOutData,
                              &uefiRtnSize);

    if (status == NV_OK)
    {
        portMemCopy(outBuffer, uefiRtnSize, inOutData, uefiRtnSize);
        *sizeToRead = uefiRtnSize;
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS getTableDataUsingObjTypeCall(pGpu,
//                           curDir,    acpiFunction,
//                           inOutData, inOutDataSz,
//                           outBuffer, outBufferSz, globTypeWanted,
//                           curGlob, * sizeToRead)
//
//  This function retrieves an ACPI based object.
//  With RID 61791, also supports objects as UEFI runtime variables.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      curDir                table source (acpi vs uefi)
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      inOutData             ACPI inout buffer
//      inOutDataSz           ACPI inout buffer size
//      outBuffer             output buffer
//      outBufferSz           output buffer size
//      globTypeWanted        desired object Type
//      curGlob               glob index
//      *sizeToRead           size to read
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            Such as bad parameters or failed read.
//      rtnSize               set to amount read (always set to inOutDataSz)
//
//----------------------------------------------------------------------------
static NV_STATUS getTableDataUsingObjTypeCall
(
    OBJGPU             *pGpu,
    NvU16               curDir,
    ACPI_DSM_FUNCTION   acpiFunction,
    NvU8              * inOutData,
    NvU32               inOutDataSz,
    NvU8              * outBuffer,
    NvU32               outBufferSz,
    NBSI_GLOB_TYPE      globTypeWanted,
    NvU8                curGlob,
    NvU32             * sizeToRead
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT(inOutData);
    NV_ASSERT(outBuffer);
    NV_ASSERT(sizeToRead);
    NV_ASSERT(!(*sizeToRead > outBufferSz));

    if (*sizeToRead > outBufferSz)
    {
        return NV_ERR_GENERIC;
    }

    if (curDir == NBSI_TBL_SOURCE_ACPI)
    {
        status = nbsiObjTypeCallAcpi(pGpu,
                                     acpiFunction,
                                     inOutData,
                                     inOutDataSz,
                                     outBuffer,
                                     outBufferSz,
                                     globTypeWanted,
                                     curGlob,
                                     sizeToRead);
    }
    else if (curDir == NBSI_TBL_SOURCE_UEFI)
    {
        status = nbsiObjTypeCallUefi(pGpu,
                                     inOutData,
                                     inOutDataSz,
                                     outBuffer,
                                     outBufferSz,
                                     globTypeWanted,
                                     curGlob,
                                     sizeToRead);
    }
    else
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS getTableUsingObjTypeCall(pGpu, idx,
//                          curDir,
//                          acpiFunction, validationOption,
//                          wantedGlobType, pActualGlobIdx,
//                          inOutData, inOutDataSz, tmpBuffer, tmpBufferSz)
//
//  This function queries for the ACPI based NBSI table and checks the headers.
//  With RID 67191, also supports querying via UEFI runtime variables.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   index to current gpu
//      curDir                table source (acpi vs uefi)
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      validationOption      test validation option
//      wantedGlobType        glob type wanted
//      wantedGlobIdx         glob type index wanted
//      pActualGlobIdx        pointer to actual index found
//      inOutData             acpi inout buffer for reads
//      inOutDataSz           size of acpi inout buffer
//      tmpBuffer             temporary buffer for data manipulation
//                            (should be equal to inOutDataSz)
//      tmpBufferSz           size of temporary buffer
//
//  Output parameters:
//      pRtnObj               pointer to return object
//      pRtnObjSize           pointer to return object size
//      pbFound               pointer to NvBool indicating object was found
//      status == NV_OK       no issues in looking for object (although
//                            object may or may not be present... see
//                            pbFound)
//      status != NV_OK       if error occurred while getting the object
//
//----------------------------------------------------------------------------

static NV_STATUS getTableUsingObjTypeCall
(
    OBJGPU             *pGpu,
    NvU32               idx,
    NvU16               curDir,
    ACPI_DSM_FUNCTION   acpiFunction,
    NBSI_VALIDATE       validationOption,
    NBSI_GLOB_TYPE      wantedGlobType,
    NvU8                wantedGlobIdx,
    NvU8              * pActualGlobIdx,
    PNBSI_GEN_OBJ     * pRtnObj,
    NvU32             * pRtnObjSize,
    NvBool            * pbFound,
    NvU8              * inOutData,
    NvU32               inOutDataSz,
    NvU8              * tmpBuffer,
    NvU32               tmpBufferSz
)
{
    NV_STATUS        status = NV_OK;
    PNBSI_DRIVER_OBJ pNbsiDrvrObj;
    NBSI_DRIVER_OBJ  nbsiDriverObj;
    NvU32            drvrObjHdrSize = sizeof(nbsiDriverObj) -
                                      sizeof(nbsiDriverObj.objData);
    NvU8             curGlob;
    NvU32            curGlobSize;
    NvU32            rtnSize;
    NvU8             thisScore;
    NvBool           bMyFound;
    NvU8             bestDriverObjectMatchScore = 0;
    NvU8             bestDriverObjectMatchGlob = 0;
    NvU32            bestDriverObjectMatchSize = 0;
    NvU32            driverVersion = 0;
    NvU32            bestFitDriverVersion = 0;
    NvBool           bCheckCRC = NV_TRUE;

    NV_ASSERT(pRtnObj);
    NV_ASSERT(pRtnObjSize);
    NV_ASSERT(pbFound);
    NV_ASSERT(pActualGlobIdx);
    NV_ASSERT(inOutData);
    NV_ASSERT(tmpBuffer);

    *pbFound = NV_FALSE;

    if (wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR)
    {
        // shouldn't get here... we can't handle this with get obj type call
        return NV_ERR_GENERIC;
    }

    if (wantedGlobIdx)
    {
        curGlob = wantedGlobIdx;
    }
    else
    {
        curGlob = 0;
    }

    bMyFound = NV_FALSE;
    curGlobSize = 0;
    while ((status == NV_OK) && !bMyFound)
    {
        // try to read curGlobHeader for this type
        rtnSize = drvrObjHdrSize;
        status = getTableDataUsingObjTypeCall(pGpu,
                                              curDir,
                                              acpiFunction,
                                              inOutData,
                                              inOutDataSz,
                                              tmpBuffer,
                                              tmpBufferSz,
                                              wantedGlobType,
                                              curGlob,
                                              &rtnSize);

        if (status != NV_OK)
        {
            // error returned on get, must be no more globs
            break;
        }

        // Confirm this glob header looks okay.
        pNbsiDrvrObj = (PNBSI_DRIVER_OBJ) tmpBuffer;
        status = testNbsiTable(pGpu,
                               (PNBSI_GEN_OBJ) pNbsiDrvrObj,
                               wantedGlobType,
                               curGlob,
                               idx,
                               NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE,
                               tmpBufferSz,
                               TEST_NBSI_TABLE_SKIP_ALLOC_SIZE_CHECK,
                               TEST_NBSI_TABLE_SKIP_HASH_CHECK,
                               TEST_NBSI_TABLE_DO_GLOBTYPE_CHECK);
        if (status != NV_OK)
        {
            // bad table format
            // pNbsiObj->nbsiTableState[idx][objt] = NBSI_TABLE_BAD;
            return status;
        }

        // if they wanted a specific glob index we're done
        if (wantedGlobIdx || (wantedGlobType != NBSI_DRIVER))
        {
            // we've got the header for the glob we want
            curGlobSize = pNbsiDrvrObj->objHdr.size;
            bMyFound = NV_TRUE;
        }
        else
        {
            // we need to find the best fit NBSI_DRIVER
            // Check the match score for this table
            thisScore = checkUidMatch(pGpu,
                                      (PNBSI_DRIVER_OBJ) pNbsiDrvrObj,
                                      idx,
                                      NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE,
                                      curGlob,
                                      &driverVersion);

            // Keep track of the best match
            if ((thisScore > 0) &&
                ((thisScore > bestDriverObjectMatchScore) ||
                ((thisScore == bestDriverObjectMatchScore) &&
                 (driverVersion > bestFitDriverVersion))))
            {
                bestDriverObjectMatchGlob = curGlob;
                bestDriverObjectMatchScore = thisScore;
                bestDriverObjectMatchSize = pNbsiDrvrObj->objHdr.size;
                bestFitDriverVersion = driverVersion;
            }
        }
        if (!bMyFound)
        {
            curGlob++;
        }
    }

    if (bestDriverObjectMatchSize &&
        (wantedGlobType == NBSI_DRIVER) &&
        !wantedGlobIdx)
    {
        // replace last checked with best fit driver glob
        curGlob = bestDriverObjectMatchGlob;
        curGlobSize = bestDriverObjectMatchSize;
        bMyFound = NV_TRUE;
        NV_PRINTF(LEVEL_INFO,
                  "NBSI Gpu%d Tloc=%d/Glob=%d best fit (score/ver = (%x/%d))\n",
                  idx, NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE,
                  bestDriverObjectMatchGlob, bestDriverObjectMatchScore,
                  bestFitDriverVersion);
    }

    if (bMyFound)
    {
        // we know which glob number we want to get the entire glob for
        // allocate memory to return the entire glob in
        pNbsiDrvrObj = portMemAllocNonPaged(curGlobSize);
        if (pNbsiDrvrObj == NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Can't alloc 0x%x bytes for ACPI NBSI table.\n",
                      curGlobSize);
            return NV_ERR_GENERIC;
        }

        // read all of the current glob in
        rtnSize = curGlobSize;
        status = getTableDataUsingObjTypeCall(pGpu,
                                              curDir,
                                              acpiFunction,
                                              inOutData,
                                              inOutDataSz,
                                              (NvU8 *) pNbsiDrvrObj,
                                              curGlobSize,
                                              wantedGlobType,
                                              curGlob,
                                              &rtnSize);

        if (status != NV_OK)
        {
            // this shouldn't happen since we did this once before
            portMemFree(pNbsiDrvrObj);
            pNbsiDrvrObj = NULL;
            return status;
        }

        if (validationOption == NBSI_VALIDATE_IGNORE_CRC)
        {
            bCheckCRC = NV_FALSE;
        }

        // Confirm this glob header looks okay.
        status = testNbsiTable(pGpu,
                               (PNBSI_GEN_OBJ) pNbsiDrvrObj,
                               wantedGlobType,
                               curGlob,
                               idx,
                               NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE,
                               curGlobSize,
                               TEST_NBSI_TABLE_DO_ALLOC_SIZE_CHECK,
                               bCheckCRC,
                               TEST_NBSI_TABLE_DO_GLOBTYPE_CHECK);
        if (status != NV_OK)
        {
            // bad table format (most likely hash test failure)
            portMemFree(pNbsiDrvrObj);
            pNbsiDrvrObj = NULL;
            return status;
        }

        *pRtnObj = (PNBSI_GEN_OBJ) pNbsiDrvrObj;
        *pRtnObjSize = pNbsiDrvrObj->objHdr.size;
        *pActualGlobIdx = curGlob;
        *pbFound = NV_TRUE;
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS getTableDataUsingAllObjectCall(pGpu, acpiFunction,
//                                           inOutData, inOutDataSz,
//                                           outBuffer, outBufferSz,
//                                           curOffset, *sizeToRead)
//
//  This function queries for the ACPI based NBSI table.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      inOutData             ACPI inout buffer
//      inOutDataSz           ACPI inout buffer size
//      outBuffer             output buffer
//      outBufferSz           output buffer size
//      *curOffset            offset to read from
//      *sizeToRead           size to read
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            Such as bad parameters or failed read.
//      curOffset             set to original offset plus rtnsize
//      rtnSize               set to amount read.
//
//----------------------------------------------------------------------------

static NV_STATUS getTableDataUsingAllObjectCall
(
    OBJGPU             *pGpu,
    ACPI_DSM_FUNCTION   acpiFunction,
    NvU8              * inOutData,
    NvU32               inOutDataSz,
    NvU8              * outBuffer,
    NvU32               outBufferSz,
    NvU32               curOffset,
    NvU32             * sizeToRead
)
{
    NV_STATUS status = NV_OK;
    NvU32     leftToRead;
    NvU32     bufferOffset;
    NvU32     rtnSize = 0;
    NvU16     acpiRtnSize = (NvU16) inOutDataSz;

    NV_ASSERT(inOutData);
    NV_ASSERT(outBuffer);
    NV_ASSERT(sizeToRead);
    NV_ASSERT(!(*sizeToRead > outBufferSz));

    if (*sizeToRead > outBufferSz)
    {
        return NV_ERR_GENERIC;
    }

    leftToRead = *sizeToRead;
    *sizeToRead = 0;
    while ((status == NV_OK) && (leftToRead))
    {
        // get page the data is in.
        *(NvU32 *) inOutData = curOffset / inOutDataSz;
        status = osCallACPI_DSM(pGpu,
                                acpiFunction,
                                NV_ACPI_GENERIC_FUNC_GETALLOBJS,
                                (NvU32 *)inOutData,
                                (NvU16 *)&rtnSize);
        if ((acpiRtnSize == 0) ||
            ((status == NV_OK) &&
             (acpiRtnSize==4) &&
             ((*(NvU32*)inOutData >= NVHG_ERROR_UNSPECIFIED) &&
              (*(NvU32*)inOutData <= 0x80000005) )))
        {
            status = NV_ERR_GENERIC;
        }

        if (status == NV_OK)
        {
            bufferOffset = curOffset % inOutDataSz;
            if ((bufferOffset + leftToRead) > acpiRtnSize)
            {
                rtnSize = acpiRtnSize - bufferOffset;
            }
            else
            {
                rtnSize = leftToRead;
            }
            NV_ASSERT((*sizeToRead+rtnSize)<=outBufferSz);
            portMemCopy(&outBuffer[*sizeToRead], rtnSize, &inOutData[curOffset % inOutDataSz], rtnSize);
            leftToRead -= rtnSize;
            *sizeToRead += rtnSize;
            curOffset += rtnSize;
        }
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS getTableUsingAllObjectCall(pGpu, idx,
//                                       acpiFunction, validationOption,
//                                       wantedGlobType, wantedGlobIdx,
//                                       *pActualGlobIdx,
//                                       *pRtnObj, *pRtnObjSize, *pbFound,
//                                       inOutData, inOutDataSz, tmpBuffer,
//                                       tmpBufferSz)
//
//  This function queries for the ACPI based NBSI table.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   index to current gpu
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      validationOption      test validation option
//      wantedGlobType        desired glob type
//      wantedGlobIdx         desired glob index
//      pActualGlobIdx        pointer to actual index wanted object is at
//      inOutData             acpi inout buffer for reads
//      inOutDataSz           size of acpi inout buffer
//      tmpBuffer             temporary buffer for data manipulation
//                            (should be equal to inOutDataSz)
//      tmpBufferSz           size of temporary buffer
//
//  Output parameters:
//      pRtnObj               pointer to return object
//      pRtnObjSize           pointer to return object size
//      pbFound               pointer to NvBool indicating object was found
//      status == NV_OK       no issues in looking for object (although
//                            object may or may not be present... see
//                            pbFound)
//      status != NV_OK       if error occurred while getting the object
//
//----------------------------------------------------------------------------

static NV_STATUS getTableUsingAllObjectCall
(
    OBJGPU             *pGpu,
    NvU32               idx,
    ACPI_DSM_FUNCTION   acpiFunction,
    NBSI_VALIDATE       validationOption,
    NBSI_GLOB_TYPE      wantedGlobType,
    NvU8                wantedGlobIdx,
    NvU8              * pActualGlobIdx,
    PNBSI_GEN_OBJ     * pRtnObj,
    NvU32             * pRtnObjSize,
    NvBool            * pbFound,
    NvU8              * inOutData,
    NvU32               inOutDataSz,
    NvU8              * tmpBuffer,
    NvU32               tmpBufferSz
)
{
    NV_STATUS        status = NV_OK;
    PNBSI_DIRECTORY  pNbsiDir;
    NvU32            nbsiDirSize;
    PNBSI_DRIVER_OBJ pNbsiDrvrObj;
    NBSI_DRIVER_OBJ  nbsiDrvrObj;
    NvU8             numGlobs;
    NvU8             curGlob;
    NvU32            curGlobSize;
    NvU8             cntOfGlobsWithWantedGlobType;
    NvU8             cntOfMatchingGlobTypes;
    NvU32            rtnSize;
    NvU8             thisScore;
    NvBool           bMyFound;
    NvU8             bestDriverObjectMatchScore = 0;
    NvU32            bestDriverObjectMatchOffset = 0;
    NvU32            bestDriverObjectMatchSize = 0;
    NvU8             bestDriverObjectIndex = 0;
    NvU32            curOffset;
    NvU32            dirContentsSize = 0;
    NvU32            driverVersion = 0;
    NvU32            bestFitDriverVersion = 0;
    NvBool           bCheckCRC = NV_TRUE;

    NV_ASSERT(pRtnObj);
    NV_ASSERT(pRtnObjSize);
    NV_ASSERT(pActualGlobIdx);
    NV_ASSERT(pbFound);
    NV_ASSERT(inOutData);
    NV_ASSERT(tmpBuffer);

    *pbFound = NV_FALSE;

    // read in the directory (assume new format and one globtype entry)
    nbsiDirSize = sizeof(pNbsiDir->d);
    rtnSize = nbsiDirSize;
    curOffset = 0;
    status = getTableDataUsingAllObjectCall(pGpu,
                                            acpiFunction,
                                            inOutData,
                                            inOutDataSz,
                                            tmpBuffer,
                                            tmpBufferSz,
                                            curOffset,
                                            &rtnSize);
    if (status != NV_OK)
    {
        // error returned on get, must be no more globs
        return status;
    }

    // determine the size of the directory
    pNbsiDir = (PNBSI_DIRECTORY) tmpBuffer;
    status = getNbsiDirSize(pGpu,
                            idx,
                            pNbsiDir,
                            &nbsiDirSize,
                            NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ);
    if (status != NV_OK)
    {
        // error returned on get, must be no more globs
        return status;
    }

    // read all of the directory this time
    curOffset = 0;
    rtnSize = nbsiDirSize;
    status = getTableDataUsingAllObjectCall(pGpu,
                                            acpiFunction,
                                            inOutData,
                                            inOutDataSz,
                                            tmpBuffer,
                                            tmpBufferSz,
                                            curOffset,
                                            &rtnSize);
    if (status != NV_OK)
    {
        // error returned on get, must be no more globs
        return status;
    }

    status = testNbsiDir(pGpu,
                         idx,
                         pNbsiDir,
                         nbsiDirSize,
                         NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ);
    if (status != NV_OK)
    {
        return status;
    }

    status = isGlobTypeInNbsiDir(pNbsiDir,
                                 wantedGlobType,
                                 &cntOfMatchingGlobTypes,
                                 &numGlobs);
    if (status != NV_OK)
    {
        return status;
    }

    if (cntOfMatchingGlobTypes == 0)
    {
        return NV_OK;
    }

    // point at the first object in the directory
    curOffset += nbsiDirSize;

    curGlob = 0;
    cntOfGlobsWithWantedGlobType = 0;
    curGlobSize = 0;
    bMyFound = NV_FALSE;
    while ((status == NV_OK) &&
           !bMyFound &&
           (curGlob < numGlobs) &&
           (cntOfGlobsWithWantedGlobType < cntOfMatchingGlobTypes))
    {
        // read the generic object so we can test.
        rtnSize = sizeof(nbsiDrvrObj) - sizeof(nbsiDrvrObj.objData);
        status = getTableDataUsingAllObjectCall(pGpu,
                                                acpiFunction,
                                                inOutData,
                                                inOutDataSz,
                                                tmpBuffer,
                                                tmpBufferSz,
                                                curOffset,
                                                &rtnSize);
        if (status != NV_OK)
        {
            // error returned on get, must be no more globs
            break;
        }

        // Confirm this glob header looks okay.
        pNbsiDrvrObj = (PNBSI_DRIVER_OBJ) tmpBuffer;
        status = testNbsiTable(pGpu,
                               (PNBSI_GEN_OBJ) pNbsiDrvrObj,
                               wantedGlobType,
                               curGlob,
                               idx,
                               NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ,
                               tmpBufferSz,
                               TEST_NBSI_TABLE_SKIP_ALLOC_SIZE_CHECK,
                               TEST_NBSI_TABLE_SKIP_HASH_CHECK,
                               TEST_NBSI_TABLE_SKIP_GLOBTYPE_CHECK);
        if (status != NV_OK)
        {
            // bad table format
            // pNbsiObj->nbsiTableState[idx][objt] = NBSI_TABLE_BAD;
            return status;
        }

        curGlob++;
        if (wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR)
        {
            dirContentsSize += pNbsiDrvrObj->objHdr.size;
        }
        else
        {
            if (pNbsiDrvrObj->objHdr.globType == wantedGlobType)
            {
                cntOfGlobsWithWantedGlobType++;
                // if want best fit (wantedGlobIdx==0) or we are at real one
                if (!wantedGlobIdx ||
                    (cntOfGlobsWithWantedGlobType == wantedGlobIdx))
                {
                    curGlobSize = pNbsiDrvrObj->objHdr.size;
                    // if wantedGlobIdx == 0 and driver object use best match
                    if (!wantedGlobIdx &&
                                      (pNbsiDrvrObj->objHdr.globType == NBSI_DRIVER))
                    {
                        // Check the match score for this table
                        thisScore =
                                checkUidMatch(pGpu,
                                              (PNBSI_DRIVER_OBJ) pNbsiDrvrObj,
                                              idx,
                                              NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ,
                                              curGlob,
                                              &driverVersion);

                        // Keep track of the best match
                        if ((thisScore > 0) &&
                            ((thisScore > bestDriverObjectMatchScore) ||
                            ((thisScore == bestDriverObjectMatchScore) &&
                             (driverVersion > bestFitDriverVersion))))
                        {
                            bestDriverObjectMatchSize = pNbsiDrvrObj->objHdr.size;
                            bestDriverObjectMatchOffset = curOffset;
                            bestDriverObjectMatchScore = thisScore;
                            bestDriverObjectIndex =
                                                  cntOfGlobsWithWantedGlobType;
                            bestFitDriverVersion = driverVersion;
                        }
                    }
                    else
                    {
                        bestDriverObjectIndex = cntOfGlobsWithWantedGlobType;
                        bMyFound = NV_TRUE;
                    }
                }
            }
        }

        // if not found yet move to the next object
        if (!bMyFound)
        {
            curOffset = curOffset + pNbsiDrvrObj->objHdr.size;
        }
    }

    if ((status == NV_OK) &&
        bestDriverObjectMatchSize &&
        (wantedGlobType == NBSI_DRIVER) &&
        !wantedGlobIdx)
    {
        // replace last checked with best fit driver glob
        curOffset = bestDriverObjectMatchOffset;
        curGlobSize = bestDriverObjectMatchSize;
        bMyFound = NV_TRUE;
        NV_PRINTF(LEVEL_INFO,
                  "NBSI Gpu%d Tloc=%d/Glob=%d best fit (score/ver = (%x/%d))\n",
                  idx, NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ,
                  bestDriverObjectIndex, bestDriverObjectMatchScore,
                  bestFitDriverVersion);
    }

    if ((wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR) &&
        ((nbsiDirSize + dirContentsSize) > 0) &&
        (status == NV_OK))
    {
        curOffset = 0;
        curGlob = 0;
        bestDriverObjectIndex = 0;
        curGlobSize = nbsiDirSize + dirContentsSize;
        bMyFound = NV_TRUE;
    }

    if ((status == NV_OK) && bMyFound)
    {
        // we know which glob number we want to get the entire glob for
        // allocate memory to return the entire glob in
        pNbsiDrvrObj = portMemAllocNonPaged(curGlobSize);
        if (pNbsiDrvrObj == NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Can't alloc 0x%x bytes for ACPI NBSI table.\n",
                      curGlobSize);
            return NV_ERR_GENERIC;
        }

        // read all of the current glob in
        rtnSize = curGlobSize;
        status = getTableDataUsingAllObjectCall(pGpu,
                                                acpiFunction,
                                                inOutData,
                                                inOutDataSz,
                                                (NvU8 *) pNbsiDrvrObj,
                                                curGlobSize,
                                                curOffset,
                                                &rtnSize);
        if (status != NV_OK)
        {
            // this shouldn't happen since we did this once before
            portMemFree(pNbsiDrvrObj);
            pNbsiDrvrObj = NULL;
            return status;
        }

        if (wantedGlobType != (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR)
        {
            if (validationOption == NBSI_VALIDATE_IGNORE_CRC)
            {
                bCheckCRC = NV_FALSE;
            }

            // Confirm this glob header looks okay.
            status = testNbsiTable(pGpu,
                                   (PNBSI_GEN_OBJ) pNbsiDrvrObj,
                                   wantedGlobType,
                                   curGlob,
                                   idx,
                                   NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ,
                                   curGlobSize,
                                   TEST_NBSI_TABLE_DO_ALLOC_SIZE_CHECK,
                                   bCheckCRC,
                                   TEST_NBSI_TABLE_DO_GLOBTYPE_CHECK);
            if (status != NV_OK)
            {
                // bad table format (most likely hash test failure)
                portMemFree(pNbsiDrvrObj);
                pNbsiDrvrObj = NULL;
                return status;
            }
        }

        *pRtnObj = (PNBSI_GEN_OBJ) pNbsiDrvrObj;
        if (wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR)
        {
            *pRtnObjSize = curGlobSize;
            *pActualGlobIdx = curGlob;
        }
        else
        {
            *pRtnObjSize = pNbsiDrvrObj->objHdr.size;
            *pActualGlobIdx = bestDriverObjectIndex;
        }
        *pbFound = NV_TRUE;
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS _extractNBSIObjFromACPIDir(pGpu, idx,
//                                       curDir,
//                                       acpiFunction, validationOption, acpiMethod,
//                                       wantedGlobType, wantedGlobIdx,
//                                       pActualGlobIdx,
//                                       *pRtnObj, *pRtnObjSize, *pbFound)
//
//  This extracts an NBSI from an ACPI directory.
//  With RID 67191, also supports BY_OBJ_TYPE retrieval via UEFI runtime
//  variables.
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      curDir                table source (acpi or uefi)
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      validationOption      test validation option
//      acpiMethod            Acpi method(s) available to access table
//      wantedGlobType        desired globtype
//      wantedGlobIdx         desired glob index
//      pActualGlobIdx        rtn actual index.
//      pRtnObj               pointer to memory allocated for return object
//                            (or NULL if pRtnObjSize is 0)
//      pRtnObjSize           pointer to return object size (use 0 to query
//                            for object presence and size. (query will cache
//                            if globIdx is 0 and room is available in cache)
//      pbFound               pointer to NvBool for return status.
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            (such as memory allocation failure)
//                            looking for the table.
//      *pRtnObj              pointer to return object (if pbFound=NV_TRUE)
//      *pRtnObjSize          pointer to return object size.
//      *pbFound              pointer to NvBool set to NV_TRUE if object was found
//
//----------------------------------------------------------------------------

static NV_STATUS _extractNBSIObjFromACPIDir
(
    OBJGPU             *pGpu,
    NvU32               idx,
    NvU16               curDir,
    ACPI_DSM_FUNCTION   acpiFunction,
    NBSI_VALIDATE       validationOption,
    NBSI_ACPI_METHOD    acpiMethod,
    NBSI_GLOB_TYPE      wantedGlobType,
    NvU8                wantedGlobIdx,
    NvU8              * pActualGlobIdx,
    PNBSI_GEN_OBJ     * pRtnObj,
    NvU32             * pRtnObjSize,
    NvBool            * pbFound
)
{
    NV_STATUS  status = NV_ERR_GENERIC;
    void*      inOutData = NULL;
    NvU32      inOutDataSz = NV_ACPI_DSM_READ_SIZE;
    void*      tmpBuffer = NULL;
    NvU32      tmpBufferSz = NV_ACPI_DSM_READ_SIZE;

    NV_ASSERT(pActualGlobIdx);
    NV_ASSERT(pRtnObj);
    NV_ASSERT(pRtnObjSize);
    NV_ASSERT(pbFound);

    if ((curDir != NBSI_TBL_SOURCE_ACPI) &&
        (curDir != NBSI_TBL_SOURCE_UEFI))
    {
        // Function only supports ACPI and UEFI sources.
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((curDir == NBSI_TBL_SOURCE_UEFI) ||
        (acpiMethod == NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE) ||
        (acpiMethod == NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ) ||
        (acpiMethod == NBSI_TBL_SOURCE_ACPI_BOTH_METHODS))
    {
        // Allocate memory for the ACPI inout parameter buffer
        inOutData = portMemAllocNonPaged(inOutDataSz);
        if (inOutData == NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to allocate 0x%x bytes for ACPI parm memory.\n",
                      inOutDataSz);
            status = NV_ERR_NO_MEMORY;
            goto failed;
        }

        // Allocate memory for the temporary/local collection buffer
        tmpBuffer = portMemAllocNonPaged(tmpBufferSz);
        if (tmpBuffer == NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to allocate 0x%x bytes for ACPI parm memory.\n",
                      tmpBufferSz);
            status = NV_ERR_NO_MEMORY;
            goto failed;
        }

        //
        // Access types.
        // UEFI only supports GetByObjType.
        // ACPI supports both GetByObjType and GetAllObjs.
        // ACPI requires sub-function support.
        //
        status = NV_ERR_INVALID_ACCESS_TYPE;
        if (((curDir == NBSI_TBL_SOURCE_UEFI) ||
             (acpiMethod == NBSI_TBL_SOURCE_ACPI_BY_OBJ_TYPE) ||
             (acpiMethod == NBSI_TBL_SOURCE_ACPI_BOTH_METHODS)) &&
            (wantedGlobType != (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR))
        {
            status = getTableUsingObjTypeCall(pGpu,
                                              idx,
                                              curDir,
                                              acpiFunction,
                                              validationOption,
                                              wantedGlobType,
                                              wantedGlobIdx,
                                              pActualGlobIdx,
                                              pRtnObj,
                                              pRtnObjSize,
                                              pbFound,
                                              inOutData,
                                              inOutDataSz,
                                              tmpBuffer,
                                              tmpBufferSz);
        }
        else if (curDir == NBSI_TBL_SOURCE_ACPI)
        {
            if ((acpiMethod == NBSI_TBL_SOURCE_ACPI_BY_ALL_OBJ) ||
                ((wantedGlobType == (NBSI_GLOB_TYPE) GLOB_TYPE_GET_NBSI_DIR) &&
                 (acpiMethod == NBSI_TBL_SOURCE_ACPI_BOTH_METHODS)))
            {
                status = getTableUsingAllObjectCall(pGpu,
                                                    idx,
                                                    acpiFunction,
                                                    validationOption,
                                                    wantedGlobType,
                                                    wantedGlobIdx,
                                                    pActualGlobIdx,
                                                    pRtnObj,
                                                    pRtnObjSize,
                                                    pbFound,
                                                    inOutData,
                                                    inOutDataSz,
                                                    tmpBuffer,
                                                    tmpBufferSz);
            }
        }
    }

failed:
    if (inOutData != NULL)
        portMemFree((void*)inOutData);
    if (tmpBuffer != NULL)
        portMemFree((void*)tmpBuffer);

    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS getNbsiObjByType(pGpu, globType,
//                             *pWantedGlobIdx, *pWantedGlobSource,
//                             rtnObjOffset,
//                             *pRtnObj, *pRtnObjSize,
//                             *pTotalObjSize, *pRtnGlobStatus,
//                             acpiFunction, validateOption)
//
//  This function gets an object with globType (and wantedGlobIdx) from the
//  NBSI table. If wantedGlobIdx is 0, it will cache the object in the nbsi
//  cache for future reads.
//  If the object exists and will fit in the users memory it returns success
//  in pRtnGlobStatus, otherwise it sets this to incomplete
//
//  Input parameters:
//      pGpu                  Current pGpu object
//      idx                   GPU instance / index
//      globType              desired globtype
//      wantedGlobIdx         desired glob index
//      pWantedGlobSource     in/out directory source of directory wanted
//      rtnObjOffset          offset into object to be returned (0=start)
//      pRtnObj               pointer to memory allocated for return object
//                            (or NULL if pRtnObjSize is 0)
//      pRtnObjSize           pointer to return object size (use 0 to query
//                            for object presence and size. (query will cache
//                            if globIdx is 0 and room is available in cache)
//      pTotalObjSize         total size of object being retrieved.
//      pRtnGlobStatus        pointer to return status
//      acpiFunction          selector for acpi function (nbsi vs hybrid)
//      validationOption      object validation option
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            (such as memory allocation failure)
//                            looking for the table.
//      *pRtnObj              pointer to memory for return object
//      *pRtnObjSize          pointer to return object size.
//      *pRtnGlobStatus
//        NV2080_CTRL_BIOS_GET_NBSI_NO_TABLE - no NBSI table found
//        NV2080_CTRL_BIOS_GET_NBSI_BAD_TABLE - corrupted NBSI table found
//        NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE - object was found but it's size
//                            is larger than pRtnObjSize. pRtnObjSize is
//                            set to actual size of object.
//        NV2080_CTRL_BIOS_GET_NBSI_SUCCESS - object found and copied to
//                            pRtnObj and pRtnObjSize set to actual size
//        NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND - NBSI table present, but object
//                            was not found matching globType and globTypeIdx
//
//----------------------------------------------------------------------------

NV_STATUS getNbsiObjByType
(
    OBJGPU             *pGpu,
    NvU16               globType,
    NBSI_SOURCE_LOC   * pWantedGlobSource,
    NvU8              * pWantedGlobIdx,
    NvU32               wantedRtnObjOffset,
    NvU8              * pRtnObj,
    NvU32             * pRtnObjSize,
    NvU32             * pTotalObjSize,
    NvU32             * pRtnGlobStatus,
    ACPI_DSM_FUNCTION   acpiFunction,
    NBSI_VALIDATE       validationOption
)
{
    NvU32              idx;
    NV_STATUS          status = NV_ERR_GENERIC;
    PNBSI_DIRECTORY    pNbsiDir = NULL;
    NvU32              nbsiDirSize = 0;
    NvBool             bFound;
    NvBool             bFreeDirMemRequired;
    NvBool             bFreeTestObjRequired;
    NBSI_GEN_OBJ      *pTestObj;
    NvU32              testObjSize;
    NvU8               actualGlobIdx = 0;
    NvU8             * bufPtr;
    NvU16              curDir;
    NvU16              dirList;
    NvU16              searchDirNdx = 0;
    NBSI_SOURCE_LOC    nbsiDirLocs[] = {NBSI_TBL_SOURCE_REGISTRY,
                                        NBSI_TBL_SOURCE_VBIOS,
                                        NBSI_TBL_SOURCE_SBIOS,
                                        NBSI_TBL_SOURCE_ACPI,
                                        NBSI_TBL_SOURCE_UEFI,
                                        0};
    NBSI_ACPI_METHOD   acpiMethod = NBSI_TBL_SOURCE_ACPI_UNKNOWN;
    NBSI_OBJ *pNbsiObj = getNbsiObject();

    NV_ASSERT(pWantedGlobSource);
    NV_ASSERT(pWantedGlobIdx);
    NV_ASSERT(pTotalObjSize);
    NV_ASSERT(pRtnGlobStatus);
    NV_ASSERT(!(*pRtnObjSize != 0 && pRtnObj == NULL));

    if (pGpu == NULL)
    {
        return NV_ERR_GENERIC;
    }

    idx = gpuGetInstance(pGpu);
    if (idx >= NV_MAX_DEVICES)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "Invalid gpu index %d. Aborting NBSI get object.\n",
                  idx);
        return NV_ERR_GENERIC;
    }

    if (globType == GLOB_TYPE_GET_NBSI_ACPI_RAW)
    {
        // TODO: Add offset arg validation when ACPI calls get support from GSP firmware.
        NvU16 rtnSize;
        //
        // (IN) wantedRtnObjOffset = acpi function,
        // (IN/OUT) inoutdata to data (always use 4K!)
        // (IN/OUT) pSizeOfData In = size of inoutdata, Out = size returned
        //
        rtnSize = (NvU16) (*pRtnObjSize & 0xffff);
        status = osCallACPI_DSM(pGpu,
                                acpiFunction,
                                wantedRtnObjOffset,
                                (NvU32 *)pRtnObj,
                                (NvU16 *)&rtnSize);

        *pRtnObjSize = rtnSize;
        *pWantedGlobSource = (NBSI_SOURCE_LOC) status;
        return NV_OK;
    }

    if ((globType != NBSI_DRIVER) && (*pWantedGlobIdx==1))
    {
        //
        // object types other than NBSI_DRIVER have best fit index of 1
        // So if they ask for best fit (0) or 1 it's the same object.
        // If they ask for 0 it gets in the cache as 0 or 1.
        // If they ask for 1 first, need to allow 0 as well.
        //
        *pWantedGlobIdx = 0;
    }

    // Currently only NBSI and NBCI objects are cached...
    if ((acpiFunction == ACPI_DSM_FUNCTION_NBSI) ||
        (acpiFunction == ACPI_DSM_FUNCTION_NBCI))
    {
        status = getNbsiObjFromCache( pGpu,
                                      idx,
                                      globType,
                                      pWantedGlobSource,
                                      pWantedGlobIdx,
                                      wantedRtnObjOffset,
                                      (PNBSI_GEN_OBJ) pRtnObj,
                                      pRtnObjSize,
                                      pTotalObjSize,
                                      pRtnGlobStatus);
        if (status != NV_ERR_GENERIC)
        {
            // It's in the cache, it may or may not fit.
            return status;
        }
    }

    // always assume the registry may be present... even if it wasn't before
    pNbsiObj->availDirLoc[idx] |= NBSI_TBL_SOURCE_REGISTRY;

    //
    // Since multiple DSM functions may contain getobject/getallobect subfunctions...
    // declaring all ACPI functions as invalid is no longer true.
    // So we'll always renable the acpi option.
    // Also now that there's generic functions/subfunctions and a cache of dsm
    // supported subfunctions this should prevent alot of re-attempts on acpi
    // calls that won't work.
    //
    pNbsiObj->availDirLoc[idx] |= NBSI_TBL_SOURCE_ACPI;

    //
    // loop through each possible table source until we find the object we're
    // looking for
    //
    if (*pWantedGlobSource == 0)
    {
        if ((acpiFunction == ACPI_DSM_FUNCTION_NBSI) ||
            (acpiFunction == ACPI_DSM_FUNCTION_NBCI) ||
            (acpiFunction == ACPI_DSM_FUNCTION_CURRENT))
        {
            dirList = pNbsiObj->availDirLoc[idx];
        }
        else
        {
            dirList = NBSI_TBL_SOURCE_ACPI;
        }
    }
    else
    {
        if ((acpiFunction == ACPI_DSM_FUNCTION_NBSI) ||
            (acpiFunction == ACPI_DSM_FUNCTION_NBCI) ||
            (acpiFunction == ACPI_DSM_FUNCTION_CURRENT))
        {
            dirList = *pWantedGlobSource & pNbsiObj->availDirLoc[idx];
        }
        else
        {
            dirList = *pWantedGlobSource;
        }
    }

    bFound = NV_FALSE;
    *pRtnGlobStatus = NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND;
    searchDirNdx = 0;
    while (!bFound && nbsiDirLocs[searchDirNdx])
    {
        curDir = nbsiDirLocs[searchDirNdx] & dirList;
        if (curDir == 0)
        {
            searchDirNdx++;
        }
        else
        {
            // Not in the cache so get it from the directory
            status = getNbsiDirectory(pGpu,
                                      idx,
                                      &pNbsiDir,
                                      &nbsiDirSize,
                                      curDir,
                                      &bFreeDirMemRequired,
                                      &acpiMethod,
                                      acpiFunction);

            if (status != NV_OK)
            {
                // remove any directories not found.
                pNbsiObj->availDirLoc[idx] &= ~curDir;

                // bump to next possible.
                searchDirNdx++;
                // check if searched all directory sources and come up empty.
                if (nbsiDirLocs[searchDirNdx] == 0)
                {
                    *pRtnGlobStatus = NV2080_CTRL_BIOS_GET_NBSI_NO_TABLE;
                    if (bFreeDirMemRequired)
                    {
                        portMemFree((void*)pNbsiDir);
                    }
                    return status;
                }
            }
            else
            {
                testObjSize = 0;
                pTestObj = NULL;
                bFreeTestObjRequired = NV_FALSE;
                if ((curDir & NBSI_TBL_SOURCE_ACPI) ||
                    (curDir & NBSI_TBL_SOURCE_UEFI))
                {
                    status = _extractNBSIObjFromACPIDir(pGpu,
                                                        idx,
                                                        curDir,
                                                        acpiFunction,
                                                        validationOption,
                                                        acpiMethod,
                                                        globType,
                                                        *pWantedGlobIdx,
                                                        &actualGlobIdx,
                                                        &pTestObj,
                                                        &testObjSize,
                                                        &bFound);
                    if (pTestObj)
                    {
                        // pTestObj was allocated and needs to be released.
                        bFreeTestObjRequired = NV_TRUE;
                    }
                }
                else
                {
                    status = extractNBSIObjFromDir(pGpu,
                                                   idx,
                                                   pNbsiDir,
                                                   nbsiDirSize,
                                                   curDir,
                                                   globType,
                                                   *pWantedGlobIdx,
                                                   &actualGlobIdx,
                                                   &pTestObj,
                                                   &testObjSize,
                                                   validationOption,
                                                   &bFound);
                }

                if (bFound == NV_TRUE)
                {
                    NvU32 rtnObjSizeWithOffset;

                    // Currently only NBSI and NBCI objects are cached...
                    if ((acpiFunction == ACPI_DSM_FUNCTION_NBSI) ||
                        (acpiFunction == ACPI_DSM_FUNCTION_NBCI))
                    {
                        if ((globType != NBSI_DRIVER) && (actualGlobIdx==1))
                        {
                            //
                            // object types other than NBSI_DRIVER have best fit
                            // index of 1.
                            // So if they ask for best fit (0) or 1 it's the same
                            // object.
                            // If they ask for 0 it gets in the cache as 0 or 1.
                            // If they ask for 1 first, need to allow 0 as well.
                            //
                            *pWantedGlobIdx = 0;
                        }
                        // Cache everything (until we run out of cache entries)
                        if (globType != GLOB_TYPE_GET_NBSI_DIR)
                        {
                            addNbsiCacheEntry(pGpu,
                                              idx,
                                              pTestObj,
                                              testObjSize,
                                              *pWantedGlobSource,
                                              *pWantedGlobIdx,
                                              curDir,
                                              actualGlobIdx);
                        }
                    }

                    // return the full table size
                    *pTotalObjSize = testObjSize;

                    if (!portSafeSubU32(*pTotalObjSize, wantedRtnObjOffset, &rtnObjSizeWithOffset))
                    {
                        // Failed argument validation.
                        status = NV_ERR_INVALID_OFFSET;
                    }
                    else
                    {
                        if (*pRtnObjSize >= rtnObjSizeWithOffset)
                        {
                            // if rtnsize is larger than remaining part of table,
                            // then we can return it all this time.
                            *pRtnObjSize = rtnObjSizeWithOffset;
                            *pRtnGlobStatus = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
                        }
                        else
                        {
                            // return what we can and indicate incomplete.
                            *pRtnGlobStatus = NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE;
                        }

                        if (*pRtnObjSize > 0)
                        {
                            bufPtr = (NvU8 *) pTestObj;
                            bufPtr = &bufPtr[wantedRtnObjOffset];
                            portMemCopy(pRtnObj, *pRtnObjSize, bufPtr, *pRtnObjSize);
                        }
                    }
                }
                else
                {
                    if (status == NV_ERR_INVALID_DATA)
                    {
                        *pRtnGlobStatus = NV2080_CTRL_BIOS_GET_NBSI_BAD_HASH;
                    }

                    searchDirNdx++;
                    //
                    // check if we've searched all directory sources and
                    // come up empty.
                    //
                    if (nbsiDirLocs[searchDirNdx] == 0)
                    {
                        if (*pRtnGlobStatus != NV2080_CTRL_BIOS_GET_NBSI_BAD_HASH)
                        {
                            *pRtnGlobStatus =
                                        NV2080_CTRL_BIOS_GET_NBSI_NO_TABLE;
                        }

                        // release any memory allocated.
                        if (bFreeTestObjRequired && pTestObj)
                        {
                            portMemFree((void*)pTestObj);
                        }

                        // check to released memory temp allocated for the directory
                        if (bFreeDirMemRequired)
                        {
                            portMemFree((void*)pNbsiDir);
                        }
                        return status;
                    }
                }

                if (bFreeTestObjRequired && pTestObj)
                {
                    portMemFree((void*)pTestObj);
                    pTestObj = NULL;
                }
            }

            // check to released memory temp allocated for the directory
            if (bFreeDirMemRequired)
            {
                portMemFree((void*)pNbsiDir);
                pNbsiDir = NULL;
            }
        }
    }
    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS initNbsiTable(pGpu)
//
//  This function finds, allocates and intializes the nbsi table.
//
//  Input parameters:
//      pGpu                  pointer to this pGpu object
//      none
//
//  Output parameters:
//      NV_STATUS:            NV_OK if there were no operational issues
//                            (such as memory allocation failure)
//                            looking for the table.
//----------------------------------------------------------------------------

NV_STATUS initNbsiTable(OBJGPU *pGpu)
{
    NvU32           idx;
    NV_STATUS       status;
    NvU32           globTypeRtnStatus;
    NBSI_GEN_OBJ   *pNbsiDir = NULL;
    NvU32           nbsiDirSize;
    NvU32           totalObjSize;
    NvU8            curTbl;
    NBSI_SOURCE_LOC nbsiDriverSource;
    NvU8            nbsiDriverIndex;
    NBSI_OBJ       *pNbsiObj = getNbsiObject();

    if (pGpu == NULL)
    {
        return NV_ERR_GENERIC;
    }

    idx = gpuGetInstance(pGpu);
    if (idx >= NV_MAX_DEVICES)
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid gpu index %d. Aborting NBSI init.\n", idx);
        return NV_ERR_GENERIC;
    }

    if (pNbsiObj->nbsiDrvrTable[idx] != NULL)
    {
        NV_PRINTF(LEVEL_NOTICE, "NBSI table already initialized for GPU index %d. Aborting NBSI init.\n", idx);
        return NV_WARN_NOTHING_TO_DO;
    }

    NV_PRINTF(LEVEL_INFO, "Initializing NBSI tables for gpu %d\n", idx);

    status = allocNbsiCache(pGpu, idx,  NBSI_INITCACHEENTRYCNT);
    if (status != NV_OK)
    {
        return status;
    }
    // driver version - filled in in nbsiinit.c
    pNbsiObj->DriverVer.OS = 0; // a  (7=vista, 6=nt)
    pNbsiObj->DriverVer.DX = 0; // bb (15=vista, 14=others)
#ifdef NV_DRIVER_VERSION_NUMBER
    pNbsiObj->DriverVer.Rev = NV_DRIVER_VERSION_NUMBER; // cdddd
#else
    pNbsiObj->DriverVer.Rev = 0; // cdddd
#endif

    if (NVOS_IS_WINDOWS)
    {
        if (NVCPU_IS_64_BITS)
        {
            setNbsiOSstring("Vista64",7,2,3);
            setNbsiOSstring("Vista",5,1,3);
            setNbsiOSstring("",0,0,3);
        }
        else
        {
            setNbsiOSstring("Vista",5,1,2);
            setNbsiOSstring("",0,0,2);
        }
        pNbsiObj->DriverVer.OS = 7;
        pNbsiObj->DriverVer.DX = 15;
    }
    else
    {
        setNbsiOSstring("",0,0,1);
    }

    // print debug info.
    checkUidMatch(pGpu, NULL, idx, 0, 0, NULL);

    // By asking how big the NBSI_DRIVER is, it caches it (if found)
    nbsiDirSize = 0;
    nbsiDriverSource = 0;
    nbsiDriverIndex = 0;
    status = getNbsiObjByType(pGpu,
                              NBSI_DRIVER,
                              &nbsiDriverSource,
                              &nbsiDriverIndex,
                              0, // offset
                              NULL,
                              &nbsiDirSize,
                              &totalObjSize,
                              &globTypeRtnStatus,
                              ACPI_DSM_FUNCTION_NBCI,
                              NBSI_VALIDATE_ALL);

    // If we didn't find it.
    if (status != NV_OK)
    {
        // if NBCI failed, lets try NBSI...
        nbsiDirSize = 0;
        nbsiDriverSource = 0;
        nbsiDriverIndex = 0;
        status = getNbsiObjByType(pGpu,
                                  NBSI_DRIVER,
                                  &nbsiDriverSource,
                                  &nbsiDriverIndex,
                                  0, // offset
                                  NULL,
                                  &nbsiDirSize,
                                  &totalObjSize,
                                  &globTypeRtnStatus,
                                  ACPI_DSM_FUNCTION_NBSI,
                                  NBSI_VALIDATE_ALL);
    }

    if ((status == NV_OK) &&
        (globTypeRtnStatus == NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE))
    {
        // Now since it's cached find what memory location it's at and save it.
        nbsiDriverSource = 0;
        nbsiDriverIndex = 0;
        status = getNbsiCacheInfoForGlobType(pGpu,
                                             idx,
                                             NBSI_DRIVER,
                                             &nbsiDriverSource,
                                             &nbsiDriverIndex,
                                             &pNbsiDir,
                                             &nbsiDirSize,
                                             &curTbl);
        if (status == NV_OK)
        {
            // make a copy of the pointer for easy access... no duplicate mem
            pNbsiObj->nbsiDrvrTable[idx] = (NvU8 *) pNbsiDir;
        }
    }

    //
    // even if the nbsi driver object is not found, the nbsi table could hold
    // other objects. So we'll assume success for now.
    //
    status = NV_OK;
    if (nbsiDriverSource == 0)
    {
        NV_PRINTF(LEVEL_INFO, "No NBSI table for gpu %d found.\n", idx);
    }
    else if (nbsiDriverSource & NBSI_TBL_SOURCE_REGISTRY)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Using NBSI driver object for gpu %d from registry.\n",
                  idx);
    }
    else if (nbsiDriverSource & NBSI_TBL_SOURCE_VBIOS)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Using NBSI driver object for gpu %d from VBIOS.\n",
                  idx);
    }
    else if (nbsiDriverSource & NBSI_TBL_SOURCE_SBIOS)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Using NBSI driver object for gpu %d from SBIOS.\n",
                  idx);
    }
    else if (nbsiDriverSource & NBSI_TBL_SOURCE_ACPI)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Using NBSI driver object for gpu %d from ACPI table.\n",
                  idx);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Using NBSI driver object for gpu %d from unknown source.\n",
                  idx);
    }

    return status;
}

//----------------------------------------------------------------------------
//  NV_STATUS freeNbsiTable(pGpu)
//
//  This function frees up the nbsi cache table
//
//  Input parameters:
//      pGpu          pointer to this GPU object
//
//  Output parameters:
//      none
//
//----------------------------------------------------------------------------

void freeNbsiTable(OBJGPU *pGpu)
{
    NvU32     idx = gpuGetInstance(pGpu);
    NBSI_OBJ *pNbsiObj = getNbsiObject();

    if (idx >= NV_MAX_DEVICES)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Invalid gpu index %d. Aborting free NBSI table.\n",
                  idx);
        return;
    }

    // free up memory used by all cache entries
    freeNbsiCache( pGpu, idx);

    // Free up the nbsi reg override list.
    portMemFree(pNbsiObj->regOverrideList[idx]);
    pNbsiObj->regOverrideList[idx] = NULL;
}

void initNbsiObject(NBSI_OBJ *pNbsiObj)
{
    NvU32 i;
    for (i = 0; i < MAX_NBSI_OS; i++)
    {
        pNbsiObj->nbsiOSstr[i][0] = 0;
        pNbsiObj->nbsiOSstrLen[i] = 0;
        pNbsiObj->nbsiOSstrHash[i] = FNV1_32_INIT;
    }
    pNbsiObj->curMaxNbsiOSes = 1;

    // driver version a.bb.1c.dddd
    pNbsiObj->DriverVer.OS = 0; // a
    pNbsiObj->DriverVer.DX = 0; // bb
    pNbsiObj->DriverVer.Rev = 0; // cdddd

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        pNbsiObj->availDirLoc[i] = NBSI_TBL_SOURCE_ALL;
        pNbsiObj->pTblCache[i] = NULL;
        pNbsiObj->nbsiDrvrTable[i] = NULL;
        pNbsiObj->regOverrideList[i] = NULL;
    }
}

NBSI_OBJ *getNbsiObject(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJPFM *pPfm = SYS_GET_PFM(pSys);
    return &pPfm->nbsi;
}
