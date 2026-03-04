/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief ACPI common routines - non OS dependant
 */

#include "os/os.h"
#include "platform/acpi_common.h"
#include "acpidsmguids.h"
#include "nvhybridacpi.h"
#include "nbci.h"
#include "gps.h"
#include "nvop.h"
#include "nvhybridacpi.h"
#include "nvmisc.h"
#include "jt.h"
#include "pex.h"
#include "mxm_spec.h"
#include "gpu/gsp/gsp_static_config.h"
#include "platform/nbsi/nbsi_read.h"
#include "nvrm_registry.h"

#include "gpu/disp/kern_disp.h"

//
// DSM ACPI Routines common routines for Linux
//
// these are debug strings for printing which DSM subfunction didn't work.
// These map directly to the ACPI_DSM_FUNCTION enum in \interface\nvacpitypes.h.
//
#if NV_PRINTF_ENABLED
const char * const DSMCalls[] = {"NBSI","NVHG","MXM","NBCI","NVOP","PFCG","GPS_2X","JT","PEX","NVPCF_2X","GPS","NVPCF","UNKNOWN","CURRENT"};
#endif

/*
 *  @_isDsmError returns status if the return data is an error indicator.
 *  ACPI returns a dword such as 0x80000002 to indicate a failure. Note there
 *  is a possibility that if a subfunction returns the same data this could be
 *  confused.
 *
 *  @param[in] status    NV_STATUS return status from ACPI call
 *  @param[in] rtnSize   NvU16 number of bytes at rtnvalue
 *  @param[in] rtnvalue  NvU32 * pointer to returned value/status.
 *
 *  @returns NV_TRUE if an error was found
 *           NV_FALSE if an error wasn't found
 */
static NvBool
_isDsmError
(
    NV_STATUS status,
    NvU16     rtnSize,
    NvU32    *rtnvalue
)
{
    if (status != NV_OK)
    {
        return NV_TRUE;
    }
    else if (rtnSize == sizeof(NvU32))
    {
        if ((*rtnvalue >= NVHG_ERROR_UNSPECIFIED) &&
            (*rtnvalue <= NVHG_ERROR_PARM_INVALID))
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

/*
 *  @cacheDsmSupportedFunction caches the return from the DSM get supported
 *  functions call. Used later to determine whether to call again.
 *
 *  @param[in] pGpu             OBJGPU
 *  @param[in] acpiDsmFunction  ACPI_DSM_FUNCTION DSM call indicator
 *  @param[in] subfunction      NvU32 subfunction number
 *  @param[in] pInOut           NvU32 * pointer to get supported return data.
 *  @param[in] inOutSize        NvU32 size of data in pInOut
 *
 *  @returns void
 */
void
cacheDsmSupportedFunction
(
    OBJGPU *pGpu,
    ACPI_DSM_FUNCTION acpiDsmFunction,
    NvU32   acpiDsmSubFunction,
    NvU32  *pInOut,
    NvU32   inOutSize
)
{
    if ((acpiDsmSubFunction == NV_ACPI_ALL_FUNC_SUPPORT) &&
        (inOutSize > 0) &&
        (inOutSize <= sizeof(pGpu->acpi.dsm[acpiDsmFunction].suppFuncs)))
    {
        if (_isDsmError(NV_OK, (NvU16) inOutSize, pInOut))
        {
            NV_PRINTF(LEVEL_INFO,
                      "%s DSM functions not available.\n",
                      DSMFuncStr(acpiDsmFunction));
            return;
        }

        // cache return from get supported function list
        portMemCopy((NvU8 *)pGpu->acpi.dsm[acpiDsmFunction].suppFuncs,
                    inOutSize,
                    (NvU8 *)pInOut,
                    inOutSize);

        pGpu->acpi.dsm[acpiDsmFunction].suppFuncsLen = inOutSize;

        // if bit 0 of get supported function is set, indicate success
        if (pGpu->acpi.dsm[acpiDsmFunction].suppFuncs[NV_ACPI_ALL_FUNC_SUPPORT/8] & NV_ACPI_ALL_FUNC_SUPPORTED)
        {
            pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus = DSM_FUNC_STATUS_SUCCESS;
        }

#ifdef NV_PRINTF_ENABLED
        if (inOutSize == 8)
        {
            NV_PRINTF(LEVEL_INFO,
                      "%s DSM get supported subfunction returned 0x%08x size=%d suppStatus=%d\n",
                      DSMFuncStr(acpiDsmFunction), *pInOut,
                      inOutSize,
                      pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "%s DSM get supported subfunction returned 0x%04x size=%d suppStatus=%d\n",
                      DSMFuncStr(acpiDsmFunction), *pInOut,
                      inOutSize,
                      pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus);
        }
#endif
    }
}

/*
 *  @testIfDsmSubFunctionEnabled tests if a DSM subfunction is enabled.
 *
 *  @param[in] pGpu             OBJGPU
 *  @param[in] acpiDsmFunction  ACPI_DSM_FUNCTION DSM call indicator
 *  @param[in] subfunction      NvU32 subfunction number
 *
 *  @returns NV_STATUS of
 *           NV_OK when the subfunction is enabled/supported.
 *           NV_ERR_NOT_SUPPORTED when the subfunction is not supported.
 *           NV_ERR_OPERATING_SYSTEM when get supported list was tried and failed.
 *           NV_ERR_OBJECT_NOT_FOUND when get supportee list hasn't been tried.
 */
NV_STATUS
testIfDsmSubFunctionEnabled
(
    OBJGPU *pGpu,
    ACPI_DSM_FUNCTION acpiDsmFunction,
    NvU32   acpiDsmSubFunction
)
{
    NvU32     idx;
    NvU32     bitToTest;

    NV_ASSERT_OR_RETURN((acpiDsmFunction < ACPI_DSM_FUNCTION_COUNT) || (acpiDsmFunction == ACPI_DSM_FUNCTION_CURRENT),
                      NV_ERR_INVALID_ARGUMENT);

    if (remapDsmFunctionAndSubFunction(pGpu, &acpiDsmFunction, &acpiDsmSubFunction) != NV_OK)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(acpiDsmFunction < ACPI_DSM_FUNCTION_COUNT, NV_ERR_INVALID_ARGUMENT);

    idx = acpiDsmSubFunction / 8;
    bitToTest = NVBIT(acpiDsmSubFunction % 8);

    // Caller asked for a subfunction... do we support it
    switch (pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus)
    {
        case DSM_FUNC_STATUS_OVERRIDE:
        case DSM_FUNC_STATUS_SUCCESS:
            // confirm the supported subfunction bit is set in.
            if (idx > pGpu->acpi.dsm[acpiDsmFunction].suppFuncsLen)
            {
                return NV_ERR_NOT_SUPPORTED;
            }
            if (!(pGpu->acpi.dsm[acpiDsmFunction].suppFuncs[idx] & bitToTest))
            {
                return NV_ERR_NOT_SUPPORTED;
            }
            return NV_OK;
            break;

        case DSM_FUNC_STATUS_FAILED:
            // the get supported function failed... assume all subfunctions won't work.
            return NV_ERR_OPERATING_SYSTEM;
            break;

        default:
        case DSM_FUNC_STATUS_UNKNOWN:
            //
            // Somebody forgot to call _acpiDsmSupportedFuncCacheInit before trying
            // the dsm subfunction itself. This should be fixed!
            //
            NV_PRINTF(LEVEL_ERROR,
                      "%s ACPI DSM called before _acpiDsmSupportedFuncCacheInit subfunction = %x.\n",
                      DSMFuncStr(acpiDsmFunction),
                      acpiDsmSubFunction);
            // DBG_BREAKPOINT();
            return NV_ERR_OBJECT_NOT_FOUND;
    }
}

/*
 *  @testIfDsmFuncSupported returns if a DSM function is supported.
 *  This checks the status of the previously cached copy of the supported
 *  functions list.
 *
 *  @param[in] pGpu             OBJGPU
 *  @param[in] acpiDsmFunction  ACPI_DSM_FUNCTION DSM function
 *
 *  @returns NV_STATUS of
 *           NV_ERR_INVALID_STATE if the pGpu pointer is NULL
 *           NV_ERR_NOT_SUPPORTED if the the get supported functions call
 *                                succeedes but the specific DSM subfunction
 *                                is not supported.
 *           NV_WARN_MORE_PROCESSING_REQUIRED if the get supported subfunctions
 *                                list call hasn't been done yet.
 *           NV_OK                if the DSM function is supported.
 */
NV_STATUS
testIfDsmFuncSupported
(
    OBJGPU *pGpu,
    ACPI_DSM_FUNCTION acpiDsmFunction
)
{
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // no generic functions allowed
    NV_ASSERT_OR_RETURN(acpiDsmFunction < ACPI_DSM_FUNCTION_COUNT, NV_ERR_INVALID_ARGUMENT);

    // should only be called after the cache is inited.
    NV_ASSERT_OR_RETURN((pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus == DSM_FUNC_STATUS_SUCCESS) ||
                      (pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus == DSM_FUNC_STATUS_OVERRIDE) ||
                      (pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus == DSM_FUNC_STATUS_FAILED),
                       NV_ERR_INVALID_ARGUMENT);

    switch (pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus)
    {
        case DSM_FUNC_STATUS_OVERRIDE:
        case DSM_FUNC_STATUS_SUCCESS:
            return NV_OK;
        case DSM_FUNC_STATUS_FAILED:
            return NV_ERR_NOT_SUPPORTED;
        default:
        case DSM_FUNC_STATUS_UNKNOWN:
            // just in case... should be asserted.
            // NV_ASSERT(0);
            return NV_WARN_MORE_PROCESSING_REQUIRED;
    }
}

//
// This table defines the generic subfunction remapping.
//
// At driver startup the following occurs.
// 1) The get supported subfunctions (0) subfunction call is used for each of
//    the acpi functions guids.
// 2) For each of the generic subfunctions, the return data from the get supported
//    subfunction list is used to determine which GUID supports that function.
//    The order is based on the dsmOrderOfPrecedenceList below.
//    Example:
//      For NV_ACPI_GENERIC_FUNC_HYBRIDCAPS we look at the following (in the
//      following order.
//          ACPI_DSM_FUNCTION_NVOP - ignored no compatible subfunction
//          ACPI_DSM_FUNCTION_NBCI/NV_NBCI_FUNC_PLATCAPS
//          ACPI_DSM_FUNCTION_MXM - ignored no compatible subfunction
//          ACPI_DSM_FUNCTION_NVHG/NVHG_FUNC_HYBRIDCAPS
//          ACPI_DSM_FUNCTION_NBSI/NBSI_FUNC_PLATCAPS
//      The first subfunction which is supported in the above list is saved
//      and used whenever a call to
//      ACPI_DSM_FUNCTION_CURRENT/NV_ACPI_GENERIC_FUNC_HYBRIDCAPS is used.
//      So if ACPI_DSM_FUNCTION_NBCI/NV_NBCI_FUNC_PLATCAPS is not supported but
//      ACPI_DSM_FUNCTION_NVHG/NVHG_FUNC_HYBRIDCAPS is. Then the NVHG GUID will
//      be used.
//
static const NvU32 genDsmSubFuncRemapTable[] =
{
    // generic function
    // ACPI_DSM_FUNCTION_CURRENT        , ACPI_DSM_FUNCTION_NBSI  , ACPI_DSM_FUNCTION_NVHG    , ACPI_DSM_FUNCTION_MXM             , ACPI_DSM_FUNCTION_NBCI    , ACPI_DSM_FUNCTION_NVOP , ACPI_DSM_FUNCTION_PCFG , ACPI_DSM_FUNCTION_GPS_2X  , ACPI_DSM_FUNCTION_JT   , ACPI_DSM_FUNCTION_PEX     , ACPI_DSM_FUNCTION_NVPCF_2X, ACPI_DSM_FUNCTION_GPS , ACPI_DSM_FUNCTION_NVPCF,
    NV_ACPI_GENERIC_FUNC_DISPLAYSTATUS  , NBSI_FUNC_DISPLAYSTATUS , NVHG_FUNC_DISPLAYSTATUS   , NV_ACPI_DSM_MXM_FUNC_MXDP         , NV_NBCI_FUNC_DISPLAYSTATUS, NVOP_FUNC_DISPLAYSTATUS, 0                      , 0                         , JT_FUNC_DISPLAYSTATUS  , 0                         , 0                         , 0                     , 0                      ,
    NV_ACPI_GENERIC_FUNC_MDTL           , NBSI_FUNC_MDTL          , NVHG_FUNC_MDTL            , NV_ACPI_DSM_MXM_FUNC_MDTL         , NV_NBCI_FUNC_MDTL         , NVOP_FUNC_MDTL         , 0                      , 0                         , JT_FUNC_MDTL           , 0                         , 0                         , 0                     , 0                      ,
    NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE   , NBSI_FUNC_GETOBJBYTYPE  , NVHG_FUNC_GETOBJBYTYPE    , 0                                 , NV_NBCI_FUNC_GETOBJBYTYPE , NVOP_FUNC_GETOBJBYTYPE , 0                      , GPS_FUNC_GETOBJBYTYPE     , 0                      , 0                         , 0                         , GPS_FUNC_GETOBJBYTYPE , 0                      ,
    NV_ACPI_GENERIC_FUNC_GETALLOBJS     , NBSI_FUNC_GETALLOBJS    , NVHG_FUNC_GETALLOBJS      , 0                                 , NV_NBCI_FUNC_GETALLOBJS   , NVOP_FUNC_GETALLOBJS   , 0                      , GPS_FUNC_GETALLOBJS       , 0                      , 0                         , 0                         , GPS_FUNC_GETALLOBJS   , 0                      ,
    NV_ACPI_GENERIC_FUNC_GETEVENTLIST   , 0                       , NVHG_FUNC_GETEVENTLIST    , NV_ACPI_DSM_MXM_FUNC_GETEVENTLIST , NV_NBCI_FUNC_GETEVENTLIST , 0                      , 0                      , 0                         , 0                      , 0                         , 0                         , 0                     , 0                      ,
    NV_ACPI_GENERIC_FUNC_CALLBACKS      , NBSI_FUNC_CALLBACKS     , NVHG_FUNC_CALLBACKS       , NV_ACPI_DSM_MXM_FUNC_MXCB         , NV_NBCI_FUNC_CALLBACKS    , 0                      , 0                      , GPS_FUNC_GETCALLBACKS     , 0                      , 0                         , 0                         , GPS_FUNC_GETCALLBACKS , 0                      ,
    NV_ACPI_GENERIC_FUNC_GETBACKLIGHT   , 0                       , NVHG_FUNC_GETBACKLIGHT    , 0                                 , NV_NBCI_FUNC_GETBACKLIGHT , 0                      , 0                      , 0                         , 0                      , 0                         , 0                         , 0                     , 0                      ,
    NV_ACPI_GENERIC_FUNC_MSTL           , 0                       , 0                         , 0                                 , NV_NBCI_FUNC_MSTL         , 0                      , 0                      , 0                         , 0                      , 0                         , 0                         , 0                     , 0                      ,
};

/*
 *  @_getRemappedDsmSubfunction takes as input either a generic DSM subfunction
 *  or a NBSI subfunction number and returns the subfunction number as appropriate
 *  for the DSM function desired.
 *
 *  @param[in]  acpiDsmFunction         ACPI_DSM_FUNCTION DSM call indicator (specific)
 *  @param[in]  acpiDsmSubFunction      NvU32 subfunction number (generic)
 *  @param[out] pRemappedDsmSubFunction NvU32 * pointer to return remapped subfunction number
 *
 *  @returns NV_STATUS of
 *           NV_ERR_NOT_SUPPORTED if unknown acpi DSM function or the subfunction
 *                                could not be remapped
 *           NV_OK                if subfunction number was remapped
 */

static NV_STATUS
_getRemappedDsmSubfunction
(
    ACPI_DSM_FUNCTION   acpiDsmFunction,
    NvU32               acpiDsmSubFunction,
    NvU32              *pRemappedDsmSubFunction
)
{
    NvU32     i;

    NV_ASSERT_OR_RETURN(pRemappedDsmSubFunction, NV_ERR_INVALID_ARGUMENT);

    // only specific dsm functions are allowed to remap.
    NV_ASSERT_OR_RETURN(!isGenericDsmFunction(acpiDsmFunction), NV_ERR_INVALID_ARGUMENT);

    // confirm the acpiDsmSubFunction is a generic acpiDsmSubFunction
    NV_ASSERT_OR_RETURN(isGenericDsmSubFunction(acpiDsmSubFunction), NV_ERR_INVALID_ARGUMENT);

    //
    // confirm entry rows matches number of DSM functions
    // Use NV_ASSERT_OR_ELSE_STR for embedded %.
    //
    NV_ASSERT_OR_ELSE_STR(
        !(NV_ARRAY_ELEMENTS(genDsmSubFuncRemapTable) % (ACPI_DSM_FUNCTION_COUNT + 1)),
        "!(NV_ARRAY_ELEMENTS(genDsmSubFuncRemapTable) %% (ACPI_DSM_FUNCTION_COUNT + 1))",
        return NV_ERR_INVALID_ARGUMENT);

    // find the event in the table
    i = 0;
    while (i <= (NV_ARRAY_ELEMENTS(genDsmSubFuncRemapTable) - ACPI_DSM_FUNCTION_COUNT - 1))
    {
        if (acpiDsmSubFunction == genDsmSubFuncRemapTable[i])
        {
            if (genDsmSubFuncRemapTable[i + acpiDsmFunction + 1])
            {
                *pRemappedDsmSubFunction = genDsmSubFuncRemapTable[i + acpiDsmFunction + 1];
                return NV_OK;
            }
            else
            {
                // Found the entry in the table. But that function doesn't support that acpiDsmSubFunction.
                return NV_ERR_NOT_SUPPORTED;
            }
        }
        i += ACPI_DSM_FUNCTION_COUNT + 1;
    }

    //
    // someone called us with a generic acpiDsmSubFunction which isn't the table.
    // Either add it to the table, or don't use a generic acpiDsmSubFunction.
    //
    DBG_BREAKPOINT();
    return NV_ERR_OBJECT_NOT_FOUND;
}

//
// This table defines the order of acpi function GUIDs in which generic
// subfunctions are searched for.
// top priority is first (NVOP). Lowest priority is last (NVPCF).
//
static const ACPI_DSM_FUNCTION dsmOrderOfPrecedenceList[] =
                                  {ACPI_DSM_FUNCTION_NVOP,
                                   ACPI_DSM_FUNCTION_NBCI,
                                   ACPI_DSM_FUNCTION_MXM,
                                   ACPI_DSM_FUNCTION_NVHG,
                                   ACPI_DSM_FUNCTION_NBSI,
                                   ACPI_DSM_FUNCTION_PCFG,
                                   ACPI_DSM_FUNCTION_GPS_2X,
                                   ACPI_DSM_FUNCTION_GPS,
                                   ACPI_DSM_FUNCTION_JT,
                                   ACPI_DSM_FUNCTION_PEX,
                                   ACPI_DSM_FUNCTION_NVPCF_2X,
                                   ACPI_DSM_FUNCTION_NVPCF};

/*
 *  @remapDsmFunctionAndSubFunction remaps generic DSM functions and subfunctions.
 *  When the function and subfunction are generic, this returns the
 *  function/subfunction which is enabled in SBIOS ASL for that subfunction.
 *  When the function is specific and the subfunction is generic it remaps
 *  the generic subfunction to the matching function number (whether the
 *  subfunction is enabled or not).
 *
 *  @param[in] pGpu                        OBJGPU
 *  @param[in/out] pAcpiDsmFunction        ACPI_DSM_FUNCTION * DSM function to remap
 *  @param[in/out] pRemappedDsmSubFunction NvU32 * subfunction to remap
 *
 *  @returns NV_STATUS of
 *           NV_ERR_NOT_SUPPORTED if unknown acpi DSM function or the subfunction
 *                                could not be remapped
 *           NV_OK                if subfunction number was remapped
 */
NV_STATUS
remapDsmFunctionAndSubFunction
(
    OBJGPU            *pGpu,
    ACPI_DSM_FUNCTION *pAcpiDsmFunction,
    NvU32             *pRemappedDsmSubFunction
)
{
    NvU32             testDSMfuncIndex;
    NvU32             remappedDSMSubFunction;
    NvU32             dsmIndex;

    NV_ASSERT_OR_RETURN(pAcpiDsmFunction, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRemappedDsmSubFunction, NV_ERR_INVALID_ARGUMENT);

    NV_PRINTF(LEVEL_INFO,
              "ACPI DSM remapping function = %x Subfunction = %x\n",
              *pAcpiDsmFunction, *pRemappedDsmSubFunction);

    if (!isGenericDsmFunction(*pAcpiDsmFunction))
    {
        // if only subfunction is generic remap it.
        if (isGenericDsmSubFunction(*pRemappedDsmSubFunction))
        {
            if ((_getRemappedDsmSubfunction(*pAcpiDsmFunction, *pRemappedDsmSubFunction, pRemappedDsmSubFunction)) != NV_OK)
            {
                return NV_ERR_NOT_SUPPORTED;
            }
        }
        return NV_OK;
    }

    //
    // Caller asked to use the DSM GUID that supports this subfunction.
    // I need generic subfunction numbers so I can remap them to each DSM GUID in the loop.
    //
    NV_ASSERT_OR_RETURN(isGenericDsmSubFunction(*pRemappedDsmSubFunction), NV_ERR_INVALID_ARGUMENT);

    // If we've done this before and cached the result, return it.
    dsmIndex = *pRemappedDsmSubFunction-NV_ACPI_GENERIC_FUNC_START;
    if (pGpu->acpi.dsmCurrentFuncSupport & NVBIT(dsmIndex))
    {
        *pAcpiDsmFunction = pGpu->acpi.dsmCurrentFunc[dsmIndex];
        *pRemappedDsmSubFunction = pGpu->acpi.dsmCurrentSubFunc[dsmIndex];
        return NV_OK;
    }

    // Loop through (in precendence order) to find the GUID/subfunction until we find one that supports the call.
    testDSMfuncIndex = 0;
    while(testDSMfuncIndex < NV_ARRAY_ELEMENTS(dsmOrderOfPrecedenceList))
    {
        // remap the generic subfunction number of the one that matches this function.
        if ((_getRemappedDsmSubfunction(dsmOrderOfPrecedenceList[testDSMfuncIndex], *pRemappedDsmSubFunction, &remappedDSMSubFunction)) == NV_OK)
        {
            // Does this DSM support this subfunction?
            if (testIfDsmSubFunctionEnabled(pGpu, dsmOrderOfPrecedenceList[testDSMfuncIndex], remappedDSMSubFunction) == NV_OK)
            {
                NV_PRINTF(LEVEL_INFO,
                          "ACPI DSM remap (func=%s/subfunc=0x%x) remapped to (func=%s/subfunc=0x%x).\n",
                          DSMFuncStr(*pAcpiDsmFunction),
                          *pRemappedDsmSubFunction,
                          DSMFuncStr(dsmOrderOfPrecedenceList[testDSMfuncIndex]),
                          remappedDSMSubFunction);

                // This DSM does support this subfunction. Use it.
                *pAcpiDsmFunction = dsmOrderOfPrecedenceList[testDSMfuncIndex];
                *pRemappedDsmSubFunction = remappedDSMSubFunction;

                return NV_OK;
            }
        }
        testDSMfuncIndex++;
    }

    return NV_ERR_NOT_SUPPORTED;
}

/*
 *  @getDsmGetObjectSubfunction This returns the subfunction numbers for
 *  the get object by type and get all object calls based on the dsm function
 *  requested.
 *
 *  @param[in] pGpu                         OBJGPU
 *  @param[in/out] pAcpiDsmFunction         ACPI_DSM_FUNCTION * DSM function to use/remap
 *                                          if generic then will return first function that
 *                                          supports get object by type/get all objects.
 *  @param[out]    pGetObjByTypeSubFunction NvU32 * get object by type subfunction.
 *  @param[out]    pGetAllObjsSubFunction   NvU32 * get all objs subfunction.
 *
 *  @returns NV_STATUS of
 *           NV_ERR_NOT_SUPPORTED if unknown acpi DSM function or the subfunction
 *                                could not be remapped
 *           NV_OK                if subfunction number was remapped
 */
NV_STATUS
getDsmGetObjectSubfunction
(
    OBJGPU            *pGpu,
    ACPI_DSM_FUNCTION  *pAcpiDsmFunction,
    NvU32              *pGetObjByTypeSubFunction,
    NvU32              *pGetAllObjsSubFunction
)
{
    NV_STATUS          rmStatus = NV_ERR_NOT_SUPPORTED;
    NvU32              testDSMfuncIndex;
    ACPI_DSM_FUNCTION  curFuncForGetObjByType;
    ACPI_DSM_FUNCTION  curFuncForGetAllObjects;
    NvU32              dummySubFunc;

    NV_PRINTF(LEVEL_INFO, "entry *pAcpiDsmFunction = %x\n",
              *pAcpiDsmFunction);

    if (*pAcpiDsmFunction == ACPI_DSM_FUNCTION_CURRENT)
    {
        // determine the function that supports getobjbytype and/or getallobjects
        curFuncForGetObjByType = ACPI_DSM_FUNCTION_CURRENT;
        dummySubFunc = NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE;
        if (remapDsmFunctionAndSubFunction(pGpu, &curFuncForGetObjByType, &dummySubFunc) == NV_OK)
        {
            // get object by type supported for at least one guid... default to it.
            *pAcpiDsmFunction = curFuncForGetObjByType;

            // test get all objects to see if it's higher priority.
            curFuncForGetAllObjects = ACPI_DSM_FUNCTION_CURRENT;
            dummySubFunc = NV_ACPI_GENERIC_FUNC_GETALLOBJS;
            if (remapDsmFunctionAndSubFunction(pGpu, &curFuncForGetAllObjects, &dummySubFunc) == NV_OK)
            {
                testDSMfuncIndex = 0;
                while(testDSMfuncIndex < NV_ARRAY_ELEMENTS(dsmOrderOfPrecedenceList))
                {
                    if (dsmOrderOfPrecedenceList[testDSMfuncIndex] == curFuncForGetObjByType)
                    {
                        // found get object by type first. break now... it's already the default
                        break;
                    } else if (dsmOrderOfPrecedenceList[testDSMfuncIndex] == curFuncForGetAllObjects)
                    {
                        // found get all objects at higher priority than get object by type... use it.
                        *pAcpiDsmFunction = curFuncForGetAllObjects;
                        break;
                    }
                    testDSMfuncIndex++;
                }
            }
        }
        else
        {
            curFuncForGetAllObjects = ACPI_DSM_FUNCTION_CURRENT;
            dummySubFunc = NV_ACPI_GENERIC_FUNC_GETALLOBJS;
            if (remapDsmFunctionAndSubFunction(pGpu, &curFuncForGetAllObjects, &dummySubFunc) != NV_OK)
            {
                return NV_ERR_NOT_SUPPORTED;
            }
            // get all objects supported for at least one guid.
            *pAcpiDsmFunction = curFuncForGetAllObjects;
        }
    }

    // determine the get object subfunction numbers for this acpi dsm function.
    switch (*pAcpiDsmFunction)
    {
        case ACPI_DSM_FUNCTION_NBSI:
            *pGetObjByTypeSubFunction = NBSI_FUNC_GETOBJBYTYPE;
            *pGetAllObjsSubFunction = NBSI_FUNC_GETALLOBJS;
            rmStatus = NV_OK;
            break;

        case ACPI_DSM_FUNCTION_NVHG:
            *pGetObjByTypeSubFunction = NVHG_FUNC_GETOBJBYTYPE;
            *pGetAllObjsSubFunction = NVHG_FUNC_GETALLOBJS;
            rmStatus = NV_OK;
            break;

        case ACPI_DSM_FUNCTION_NBCI:
            *pGetObjByTypeSubFunction = NV_NBCI_FUNC_GETOBJBYTYPE;
            *pGetAllObjsSubFunction = NV_NBCI_FUNC_GETALLOBJS;
            rmStatus = NV_OK;
            break;

        case ACPI_DSM_FUNCTION_NVOP:
            *pGetObjByTypeSubFunction = NVOP_FUNC_GETOBJBYTYPE;
            *pGetAllObjsSubFunction = NVOP_FUNC_GETALLOBJS;
            rmStatus = NV_OK;
            break;

        case ACPI_DSM_FUNCTION_GPS_2X:    /* fallthrough */
        case ACPI_DSM_FUNCTION_GPS:
            *pGetObjByTypeSubFunction = GPS_FUNC_GETOBJBYTYPE;
            *pGetAllObjsSubFunction = GPS_FUNC_GETALLOBJS;
            rmStatus = NV_OK;
            break;

        case ACPI_DSM_FUNCTION_PCFG:
        case ACPI_DSM_FUNCTION_PEX:
        default:
            rmStatus = NV_ERR_NOT_SUPPORTED;
            break;
    }

    NV_PRINTF(LEVEL_INFO,
              "exit *pAcpiDsmFunction = 0x%x *pGetObjByTypeSubFunction=0x%x, status=%x\n",
              *pAcpiDsmFunction, *pGetObjByTypeSubFunction,
              rmStatus);

    return rmStatus;
}

/*
 *  @checkDsmCall checks if the function/subfunction call should be performed
 *  Generic function/subfunctions are remapped to real/current functions and
 *  subfunctions.
 *
 *  Notes on handling the Get supported functions list cache.
 *   1) Initially the cache status is set to DSM_FUNC_STATUS_UNKNOWN.
 *   2) First time osCallACPI_DSM is called for get supported functions list
 *      2A) The cache status is DSM_FUNC_STATUS_UNKNOWN so it changes the cache
 *          state to DSM_FUNC_STATUS_FAILED and
 *          returns NV_WARN_MORE_PROCESSING_REQUIRED so
 *          osCallACPI_DSM will do the actual call.
 *      2B) after doing the actual call osCallACPI_DSM calls
 *          cacheDsmSupportedFunction which caches (on success) the return data
 *          and changes the cache state to DSM_FUNC_STATUS_SUCCESS.
 *          If the call failed, the cache state is left at DSM_FUNC_STATUS_FAILED.
 *   3) Later calls from osCallACPI_DSM to get supported functions list.
 *      3A) If the cache status is DSM_FUNC_STATUS_SUCCESS... return cache and
 *          status NV_OK.
 *      3B) If the cache status is DSM_FUNC_STATUS_FAILED... return status
 *          NV_ERR_OPERATING_SYSTEM
 *
 *  The return status when subfunction is get supported functions list is either
 *   1) NV_OK status... pInOut has cached copy... no more processing required.
 *   2) NV_WARN_MORE_PROCESSING_REQUIRED status... osCallACPI_DSM should
 *      continue on and try the subfunction.
 *   3) NV_ERR_OPERATING_SYSTEM status... if the call previously failed.
 *   4) NV_ERR_BUFFER_TOO_SMALL status... if the buffer is too small.
 *
 *  For subfunctions other than get supported subfunction return options are:
 *   1) If the subfunction should be tried it returns NV_WARN_MORE_PROCESSING_REQUIRED.
 *   2) If the subfunction should not be tried it returns NV_ERR_NOT_SUPPORTED
 *
 *  @param[in] pGpu                         OBJGPU
 *  @param[in/out] pAcpiDsmFunction         ACPI_DSM_FUNCTION * DSM function
 *                                          on return this may be remapped if
 *                                          current/generic function was input.
 *  @param[in/out] pAcpiDsmSubFunction      NvU32 * DSM subfunction to use.
 *                                          on return this may be remapped if
 *                                          current/generic subfunction was input.
 *  @param[out] pInOut          NvU32 * pointer to return get supported return
 *                              data previously cached (if requested).
 *  @param[in/out] pSize        NvU32 * size of data in pInOut. On input is
 *                              size of pInOut. On output is size of data
 *                              returned if get supported subfunction call is
 *                              used.
 *
 *  @returns NV_STATUS of
 *           NV_OK all processing is complete. Used when returning the cached
 *                 copy of get supported functions list.
 *           NV_WARN_MORE_PROCESSING_REQUIRED call to subfunction should be
 *                 performed.
 *           NV_ERR_NOT_SUPPORTED Unable to remap function/subfunction.
 *           NV_ERR_BUFFER_TOO_SMALL pInOut is too small to return cached data.
 *           NV_ERR_OPERATING_SYSTEM subfunction is not enabled.
 */
NV_STATUS
checkDsmCall
(
    OBJGPU            *pGpu,
    ACPI_DSM_FUNCTION *pAcpiDsmFunction,
    NvU32             *pAcpiDsmSubFunction,
    NvU32             *pInOut,
    NvU16             *pSize
)
{
    NV_STATUS rmStatus = NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(pAcpiDsmFunction, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pAcpiDsmSubFunction, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pInOut, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSize, NV_ERR_INVALID_ARGUMENT);

    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    if (pKernelDisplay != NULL
        && pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_INTERNAL_PANEL_DISCONNECTED))
    {
        if (*pAcpiDsmFunction == ACPI_DSM_FUNCTION_NBCI)
        {
            return NV_ERR_NOT_SUPPORTED;
        } 
    }

    // Do any remapping of subfunction if function is current
    if (remapDsmFunctionAndSubFunction(pGpu, pAcpiDsmFunction, pAcpiDsmSubFunction) != NV_OK)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(*pAcpiDsmFunction < ACPI_DSM_FUNCTION_COUNT, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(*pAcpiDsmSubFunction < NV_ACPI_GENERIC_FUNC_START, NV_ERR_INVALID_ARGUMENT);

    if (isDsmGetSuppFuncListCached(pGpu, *pAcpiDsmFunction))
    {
        if (*pAcpiDsmSubFunction == NV_ACPI_ALL_FUNC_SUPPORT)
        {
            if (*pSize < pGpu->acpi.dsm[*pAcpiDsmFunction].suppFuncsLen)
            {
                return NV_ERR_BUFFER_TOO_SMALL;
            }
            else
            {
                // Return the cached values.
                *pSize = (NvU16) pGpu->acpi.dsm[*pAcpiDsmFunction].suppFuncsLen;
                portMemCopy((NvU8 *)pInOut,
                            *pSize,
                            (NvU8 *)pGpu->acpi.dsm[*pAcpiDsmFunction].suppFuncs,
                            *pSize);
                return NV_OK;
            }
        }
        else
        {
            // not subfunction 0... check cached supported functions list
            rmStatus = testIfDsmSubFunctionEnabled(pGpu, *pAcpiDsmFunction, *pAcpiDsmSubFunction);
            if (rmStatus != NV_OK)
            {
                // subfunction is not enabled in get supported subfunction list.
                return rmStatus;
            }
        }
    }
    else
    {
        // haven't cached get supported functions yet... or it's failed.
        if (isDsmGetSuppFuncListFailed(pGpu, *pAcpiDsmFunction))
        {
            // get supported subfunctions call failed before. Don't try again.
            return NV_ERR_OPERATING_SYSTEM;
        }

        // assert if subfunction 0 is not the first one called.
        NV_ASSERT_OR_RETURN(*pAcpiDsmSubFunction == NV_ACPI_ALL_FUNC_SUPPORT, NV_ERR_INVALID_ARGUMENT);
    }

    if (*pAcpiDsmSubFunction == NV_ACPI_ALL_FUNC_SUPPORT)
    {
        pGpu->acpi.dsm[*pAcpiDsmFunction].suppFuncStatus = DSM_FUNC_STATUS_FAILED;
    }

    // indicate we can go ahead and read it.
    return NV_WARN_MORE_PROCESSING_REQUIRED;
}

static void
_acpiDsmSupportedFuncCacheInit
(
    OBJGPU *pGpu
)
{
    ACPI_DSM_FUNCTION func;
    NV_STATUS         status = NV_OK;
    NvU16             rtnSize;
    NvU8              supportFuncs[MAX_DSM_SUPPORTED_FUNCS_RTN_LEN];

    // Just checking to make sure this is correct!
    NV_ASSERT_OR_RETURN_VOID(0 == ACPI_DSM_FUNCTION_NBSI);

    portMemSet(supportFuncs, 0, sizeof(supportFuncs));

    //
    // loop through all guids. The read will cache the subfunction list (if
    // available)
    //
    for(func = ACPI_DSM_FUNCTION_NBSI; func < ACPI_DSM_FUNCTION_COUNT; func++)
    {
        if ((func == ACPI_DSM_FUNCTION_GPS) &&
            (pGpu->acpi.dsm[ACPI_DSM_FUNCTION_GPS_2X].suppFuncStatus == DSM_FUNC_STATUS_SUCCESS))
        {
            //
            // If GPS_2X is supported, skip checking leagacy GPS 1X.
            //
            continue;
        }

        //
        // Skip over non-NBCI since we don't want to waste boot time here on Tegra
        // Remove this if we want to support other ACPI functions.
        //
        if (IsTEGRA(pGpu) && (func != ACPI_DSM_FUNCTION_NBCI))
        {
            if (pGpu->acpi.dsm[func].suppFuncStatus == DSM_FUNC_STATUS_UNKNOWN)
            {
                pGpu->acpi.dsm[func].suppFuncStatus = DSM_FUNC_STATUS_FAILED;
            }
        }

        if ((pGpu->acpi.dsm[func].suppFuncStatus == DSM_FUNC_STATUS_OVERRIDE) ||
            (pGpu->acpi.dsm[func].suppFuncStatus == DSM_FUNC_STATUS_FAILED))
        {
            //
            // skip reading the supported functions if
            // it's been over-ridden or previously failed.
            //
            continue;
        }


        // try package type for argument 3.
        pGpu->acpi.dsm[func].bArg3isInteger = NV_FALSE;
        rtnSize = sizeof(supportFuncs);
        status = osCallACPI_DSM(pGpu,
                                func,
                                NV_ACPI_ALL_FUNC_SUPPORT,
                                (NvU32 *) &supportFuncs,
                                &rtnSize);
        if (status != NV_OK)
        {
            if (status == NV_ERR_INVALID_ARGUMENT)
            {
                // maybe an older SBIOS, try integer type for argument 3.
                pGpu->acpi.dsm[func].bArg3isInteger = NV_TRUE;
                rtnSize = sizeof(supportFuncs);
                status = osCallACPI_DSM(pGpu,
                                        func,
                                        NV_ACPI_ALL_FUNC_SUPPORT,
                                        (NvU32 *) &supportFuncs,
                                        &rtnSize);
            }
        }
        if (_isDsmError(status, rtnSize, (NvU32 *) &supportFuncs))
        {
            NV_PRINTF(LEVEL_INFO,
                      "%s DSM function not present in ASL.\n",
                      DSMFuncStr(func));
            //
            // If the call didn't set the failed status force it now.
            // This might be because the call is stubbed out.
            //
            pGpu->acpi.dsm[func].suppFuncStatus = DSM_FUNC_STATUS_FAILED;
        }

    }
}

static void
_acpiDsmCallbackInit
(
    OBJGPU *pGpu
)
{
    ACPI_DSM_FUNCTION func;
    NV_STATUS         status = NV_OK;
    NvU32             callbacks;
    NvU16             rtnSize;
    NvU32             testDSMfuncIndex;
    // lowest priority is first entry, highest priority is last entry

    // this list only includes GUIDS with callbacks.
    ACPI_DSM_FUNCTION callbackOrderOfPrecedenceList[] =
                      {
                          ACPI_DSM_FUNCTION_NBCI,
                          ACPI_DSM_FUNCTION_NVHG,
                          ACPI_DSM_FUNCTION_MXM,
                          ACPI_DSM_FUNCTION_GPS_2X,
                          ACPI_DSM_FUNCTION_GPS,
                      };

    // Initialize these now.
    pGpu->acpi.dispStatusHotplugFunc = ACPI_DSM_FUNCTION_COUNT;
    pGpu->acpi.dispStatusConfigFunc = ACPI_DSM_FUNCTION_COUNT;
    pGpu->acpi.perfPostPowerStateFunc = ACPI_DSM_FUNCTION_COUNT;
    pGpu->acpi.stereo3dStateActiveFunc = ACPI_DSM_FUNCTION_COUNT;

    // Loop through the list of GUIDs that support callbacks in priority order
    for (testDSMfuncIndex = 0;
         testDSMfuncIndex < (NV_ARRAY_ELEMENTS(callbackOrderOfPrecedenceList));
         testDSMfuncIndex++)
    {
        func = callbackOrderOfPrecedenceList[testDSMfuncIndex];
        pGpu->acpi.dsm[func].callbackStatus = DSM_FUNC_STATUS_FAILED;

        rtnSize = sizeof(callbacks);
        callbacks = 0;

        if ((func == ACPI_DSM_FUNCTION_GPS) &&
            (pGpu->acpi.dsm[ACPI_DSM_FUNCTION_GPS_2X].callbackStatus == DSM_FUNC_STATUS_SUCCESS))
        {
            //
            // If GPS_2X has enabled callback, skip leagacy GPS 1X.
            //
            continue;
        }

        if (testIfDsmSubFunctionEnabled(pGpu, func, NV_ACPI_GENERIC_FUNC_CALLBACKS) == NV_OK)
        {
            status = osCallACPI_DSM(pGpu,
                                    func,
                                    NV_ACPI_GENERIC_FUNC_CALLBACKS,
                                    &callbacks,
                                    &rtnSize);

            if (_isDsmError(status, rtnSize, &callbacks))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "SBIOS suggested %s supports function %d, but the call failed!\n",
                          DSMFuncStr(func),
                          NV_ACPI_GENERIC_FUNC_CALLBACKS);
            }
            else
            {
                pGpu->acpi.dsm[func].callback = callbacks;
                pGpu->acpi.dsm[func].callbackStatus = DSM_FUNC_STATUS_SUCCESS;

                // replace lower priority hotplug callback.
                if (FLD_TEST_DRF(_ACPI, _CALLBACKS_RET, _HOTPLUG, _NOTIFY, pGpu->acpi.dsm[func].callback))
                {
                    pGpu->acpi.dispStatusHotplugFunc = func;
                }

                // replace lower priority status config callback.
                if (FLD_TEST_DRF(_ACPI, _CALLBACKS_RET, _CONFIG, _NOTIFY, pGpu->acpi.dsm[func].callback))
                {
                    pGpu->acpi.dispStatusConfigFunc = func;
                }

                // replace lower priority status config callback.
                if (FLD_TEST_DRF(_ACPI, _CALLBACKS_RET, _POSTPOWERSTATE, _NOTIFY, pGpu->acpi.dsm[func].callback))
                {
                    pGpu->acpi.perfPostPowerStateFunc = func;
                }

                // replace 3D stereo active state callback.
                if (FLD_TEST_DRF(_ACPI, _CALLBACKS_RET, _3DSTEREOSTATE_ACTIVE, _NOTIFY, pGpu->acpi.dsm[func].callback))
                {
                    pGpu->acpi.stereo3dStateActiveFunc = func;
                }

            }
        }
    }
}


/*
 * @brief Initialize the ACPI DSM caps related information
 *
 * @param[in] pGpu     OBJGPU pointer
 *
 * @returns
 */
static void
_acpiDsmCapsInit
(
    OBJGPU *pGpu
)
{
    ACPI_DSM_FUNCTION func;
    NV_STATUS         status = NV_OK;
    NvU32             platCaps;
    NvU16             rtnSize;
    NvU32             asmDsmSubFunction;

    // handle the NBCI specific platcaps init.
    func = ACPI_DSM_FUNCTION_NBCI;
    asmDsmSubFunction = NV_NBCI_FUNC_PLATCAPS;
    if (testIfDsmSubFunctionEnabled(pGpu, func, asmDsmSubFunction) == NV_OK)
    {
        rtnSize = sizeof(platCaps);
        status = osCallACPI_DSM(pGpu,
                                func,
                                asmDsmSubFunction,
                                &platCaps,
                                &rtnSize);

        if (_isDsmError(status, rtnSize, &platCaps))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "SBIOS suggested %s supports function %d, but the call failed!\n",
                      DSMFuncStr(func), asmDsmSubFunction);
        }
        else
        {
            // cache for later retrieval
            pGpu->acpi.dsmPlatCapsCache[func] = platCaps;

        }
    }
}

/*
 * @brief Initialize the cache of generic function/subfunctions
 *
 * @param[in] pGpu     OBJGPU pointer
 *
 * @returns
 */
static void
_acpiGenFuncCacheInit
(
    OBJGPU *pGpu
)
{
    NvU32       testGenSubFunc;
    NvU32       dsmIndex;
    NV_STATUS   status = NV_OK;

    // create a bitwise list of generic dsm supported functions available.
    pGpu->acpi.dsmCurrentFuncSupport = 0;

    // Loop through each generic subfunction, determine which is active available.
    for (testGenSubFunc = NV_ACPI_GENERIC_FUNC_START; testGenSubFunc <= NV_ACPI_GENERIC_FUNC_LAST_SUBFUNCTION; testGenSubFunc++)
    {
        dsmIndex = testGenSubFunc-NV_ACPI_GENERIC_FUNC_START;

        pGpu->acpi.dsmCurrentFunc[dsmIndex] = ACPI_DSM_FUNCTION_CURRENT;
        pGpu->acpi.dsmCurrentSubFunc[dsmIndex] = testGenSubFunc;

        //
        // This remaps the generic function/subfunction if it can.
        // If not available it leaves it as ACPI_DSM_FUNCTION_CURRENT and the generic subfunction.
        //

        status = remapDsmFunctionAndSubFunction(pGpu, &pGpu->acpi.dsmCurrentFunc[dsmIndex], &pGpu->acpi.dsmCurrentSubFunc[dsmIndex]);
        if (status == NV_OK)
        {
            if (pGpu->acpi.dsmCurrentFunc[dsmIndex] == ACPI_DSM_FUNCTION_CURRENT)
            {
                NV_PRINTF(LEVEL_INFO,
                          "DSM Generic subfunction 0x%x is not supported. Leaving entry at func %s subfunction 0x%x.\n",
                          testGenSubFunc,
                          DSMFuncStr(pGpu->acpi.dsmCurrentFunc[dsmIndex]),
                          pGpu->acpi.dsmCurrentSubFunc[dsmIndex]);
            }
            else
            {
                // set the bit indicating we do support this generic dsm subfunction.
                pGpu->acpi.dsmCurrentFuncSupport |= NVBIT(dsmIndex);
                NV_PRINTF(LEVEL_INFO,
                          "DSM Generic subfunction 0x%x supported. Mapping to func %s subfunction 0x%x\n",
                          testGenSubFunc,
                          DSMFuncStr(pGpu->acpi.dsmCurrentFunc[dsmIndex]),
                          pGpu->acpi.dsmCurrentSubFunc[dsmIndex]);
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "DSM Test generic subfunction 0x%x is not supported. Indicates possible table corruption.\n",
                      testGenSubFunc);
        }
    }
}

/*
 * @brief Initialize the ACPI DSM features such as mdtl support.
 *
 * @param[in] pGpu     OBJGPU pointer
 *
 * @returns
 */
static void
_acpiDsmFeatureInit
(
    OBJGPU *pGpu
)
{
    if (pGpu->acpi.MDTLFeatureSupport == DSM_FUNC_STATUS_UNKNOWN)
    {
        //
        // The mdtl feature requires both mdtl and displaystatus subfunctions supported.
        // We could add verify more validity checks like...
        // 1) Read in the MDTL table and confirming the contents are valid
        // 2) Try the displaystatus status command and see it returns valid data.
        // For now, we trust the SBIOS ASL. If it tells us this is supported then
        // the rest will work as well.
        //
        if ((testIfDsmSubFunctionEnabled(pGpu, ACPI_DSM_FUNCTION_CURRENT, NV_ACPI_GENERIC_FUNC_MDTL) == NV_OK) &&
            (testIfDsmSubFunctionEnabled(pGpu, ACPI_DSM_FUNCTION_CURRENT, NV_ACPI_GENERIC_FUNC_DISPLAYSTATUS) == NV_OK))
        {
            pGpu->acpi.MDTLFeatureSupport = DSM_FUNC_STATUS_SUCCESS;
        }
        else
        {
            pGpu->acpi.MDTLFeatureSupport = DSM_FUNC_STATUS_FAILED;
        }
    }
}

static void
_acpiCacheMethodData
(
    OBJGPU *pGpu
)
{
    NV_STATUS status;
    NvU32 inOut = 0;
    NvU16 rtnSize = sizeof(inOut);
    NvU32 tableLen = 0, acpiidIndex = 0, mode = 0, muxPartId = 0, state = 0;

    // This bit is used for checking if pGpu::acpiMethodData need to be used or not.
    pGpu->acpiMethodData.bValid = NV_TRUE;

    // Fill in the DOD Method Data.
    pGpu->acpiMethodData.dodMethodData.acpiIdListLen = sizeof(pGpu->acpiMethodData.dodMethodData.acpiIdList);

    status = osCallACPI_DOD(pGpu, pGpu->acpiMethodData.dodMethodData.acpiIdList, &pGpu->acpiMethodData.dodMethodData.acpiIdListLen);

    pGpu->acpiMethodData.dodMethodData.status = status;

    // Fill in the JT Method Data.
    status = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_JT, JT_FUNC_CAPS, &inOut, &rtnSize);

    pGpu->acpiMethodData.jtMethodData.status  = status;
    pGpu->acpiMethodData.jtMethodData.jtCaps  = inOut;
    pGpu->acpiMethodData.jtMethodData.jtRevId = (NvU16)DRF_VAL(_JT_FUNC, _CAPS, _REVISION_ID, inOut);
    gpuSetGC6SBIOSCapabilities(pGpu);

    // Fill in the MUX Method Data.
    portMemSet(pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable, 0, sizeof(pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable));
    portMemSet(pGpu->acpiMethodData.muxMethodData.acpiIdMuxPartTable, 0, sizeof(pGpu->acpiMethodData.muxMethodData.acpiIdMuxPartTable));
    portMemSet(pGpu->acpiMethodData.muxMethodData.acpiIdMuxStateTable, 0, sizeof(pGpu->acpiMethodData.muxMethodData.acpiIdMuxStateTable));    
    if (pGpu->acpiMethodData.dodMethodData.status == NV_OK)
    {
        tableLen = pGpu->acpiMethodData.dodMethodData.acpiIdListLen / sizeof(NvU32);
        pGpu->acpiMethodData.muxMethodData.tableLen = tableLen;
        for (acpiidIndex = 0; acpiidIndex < tableLen; acpiidIndex++)
        {
            status = osCallACPI_MXDM(pGpu, pGpu->acpiMethodData.dodMethodData.acpiIdList[acpiidIndex], &mode);
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable[acpiidIndex].acpiId = pGpu->acpiMethodData.dodMethodData.acpiIdList[acpiidIndex];
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable[acpiidIndex].mode = mode;
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable[acpiidIndex].status = status;

            status = osCallACPI_MXID(pGpu, pGpu->acpiMethodData.dodMethodData.acpiIdList[acpiidIndex], &muxPartId);
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxPartTable[acpiidIndex].acpiId = pGpu->acpiMethodData.dodMethodData.acpiIdList[acpiidIndex];
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxPartTable[acpiidIndex].mode = muxPartId;
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxPartTable[acpiidIndex].status = status;

            status = osCallACPI_MXDS(pGpu, pGpu->acpiMethodData.dodMethodData.acpiIdList[acpiidIndex], &state);
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxStateTable[acpiidIndex].acpiId = pGpu->acpiMethodData.dodMethodData.acpiIdList[acpiidIndex];
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxStateTable[acpiidIndex].mode = state;
            pGpu->acpiMethodData.muxMethodData.acpiIdMuxStateTable[acpiidIndex].status = status;
            mode = muxPartId = state = 0;
        }
    }

    // Fill in the Optimus caps Method Data.
    status = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVOP, NVOP_FUNC_OPTIMUSCAPS,
                            &pGpu->acpiMethodData.capsMethodData.optimusCaps, &rtnSize);
    pGpu->acpiMethodData.capsMethodData.status = status;
}

/*
 * @brief Initialize the ACPI Device Specific Methods
 *
 * @param[in] pGpu     OBJGPU pointer
 *
 * @returns
 */

void acpiDsmInit
(
    OBJGPU *pGpu
)
{
    // initialize the cache of the supported Functions list.
    _acpiDsmSupportedFuncCacheInit(pGpu);
    _acpiGenFuncCacheInit(pGpu);
    _acpiDsmCallbackInit(pGpu);
    _acpiDsmCapsInit(pGpu);
    _acpiDsmFeatureInit(pGpu);
    _acpiCacheMethodData(pGpu);
}

/*!
 * @brief Get NBSI object data accordingly by global source through ACPI function
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[out] pNbsiObjData      NBSI object data
 * @param[in]  pSzOfpNbsiObjData Size of NBSI object data
 * @param[in]  acpiFunction      ACPI function
 * @param[in]  objType           NBSI global object type
 * @param[in]  validateOption    NBSI valitation option
 *
 * @returns NV_OK All processing is complete.
 * @returns NV_ERR_NOT_SUPPORTED NBSI function not supported.
 * @returns NV_ERR_BUFFER_TOO_SMALL pNbsiObjData is too small to return cached data.
 * @returns NV_ERR_GENERIC Otherwise.
 */
NV_STATUS
getAcpiDsmObjectData
(
    OBJGPU             *pGpu,
    NvU8              **pNbsiObjData,
    NvU32              *pSzOfpNbsiObjData,
    ACPI_DSM_FUNCTION   acpiFunction,
    NBSI_GLOB_TYPE      objType,
    NBSI_VALIDATE       validateOption
)
{
    NV_STATUS       status;
    NvU32           rtnStatus;
    NBSI_SOURCE_LOC globSrc;
    NvU8            globIdx;
    NvU32           totalGlobSize;

    NV_ASSERT(pNbsiObjData);
    NV_ASSERT(pSzOfpNbsiObjData);

    // read best fit, but leave size 0 so it returns the size.
    globIdx = 0;
    if ((acpiFunction == ACPI_DSM_FUNCTION_NBSI) ||
        (acpiFunction == ACPI_DSM_FUNCTION_NBCI) ||
        (acpiFunction == ACPI_DSM_FUNCTION_CURRENT))
    {
        globSrc = 0; // scan all NBSI/NBCI sources
    }
    else
    {
        globSrc = NBSI_TBL_SOURCE_ACPI;
    }

    status = getNbsiObjByType(pGpu, objType, &globSrc, &globIdx, 0, *pNbsiObjData, pSzOfpNbsiObjData, &totalGlobSize, &rtnStatus, acpiFunction, validateOption);
    if (status == NV_OK)
    {
        status = NV_ERR_GENERIC;

        // got it
        if (rtnStatus == NV2080_CTRL_BIOS_GET_NBSI_SUCCESS)
        {
            status = NV_OK;
        }

        // almost got it but a bad hash was found.
        if (rtnStatus == NV2080_CTRL_BIOS_GET_NBSI_BAD_HASH)
        {
            NV_PRINTF(LEVEL_INFO,
                      "ACPI DSM object (type=0x%x) signature check failed!\n",
                      objType);
            status = NV_ERR_GENERIC;
            return status;
        }

        // couldn't fit it
        if (rtnStatus == NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE)
        {
          // return the actual size needed
          *pSzOfpNbsiObjData = totalGlobSize;
          status = NV_ERR_BUFFER_TOO_SMALL;
        }
    }

    return status;
}

/*
 * Clear DSM function cache status
 *
 * @param[in] pGpu                  OBJGPU
 * @param[in] acpiDsmFunction       ACPI_DSM_FUNCTION DSM function
 * @param[in] acpiDsmSubFunction    NvU32
 */
void uncacheDsmFuncStatus
(
    OBJGPU              *pGpu,
    ACPI_DSM_FUNCTION    acpiDsmFunction,
    NvU32                acpiDsmSubFunction
)
{
    if (acpiDsmSubFunction == NV_ACPI_ALL_FUNC_SUPPORT)
    {
        pGpu->acpi.dsm[acpiDsmFunction].suppFuncsLen = 0;
        pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus = DSM_FUNC_STATUS_UNKNOWN;
    }
}
