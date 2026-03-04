/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrlc372/ctrlc372chnc.finn
//

#include "nvdisptypes.h"
#include "ctrl/ctrlc372/ctrlc372base.h"

#define NVC372_CTRL_MAX_POSSIBLE_HEADS   8
#define NVC372_CTRL_MAX_POSSIBLE_WINDOWS 32
#define NVC372_CTRL_MAX_POSSIBLE_TILES   8

#define NVC372_CTRL_CMD_IS_MODE_POSSIBLE (0xc3720101) /* finn: Evaluated from "(FINN_NVC372_DISPLAY_SW_CHNCTL_INTERFACE_ID << 8) | NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS_MESSAGE_ID" */

/*
 * NVC372_CTRL_CMD_IS_MODE_POSSIBLE
 *
 * This command tells whether or not the specified display config is possible.
 * A config might not be possible if the display requirements exceed the GPU
 * capabilities.  Display requirements will be higher with more display
 * surfaces, higher resolutions, higher downscaling factors, etc.  GPU
 * capabilities are determined by clock frequencies, the width of data pipes,
 * amount of mempool available, number of thread groups available, etc.
 *
 * Inputs:
 *   head.headIndex
 *     This is the hardware index number for the head.  Only active heads
 *     should be included in the input structure.
 *
 *   head.maxPixelClkKHz
 *     This parameter specifies the pixel scanout rate (in KHz).
 *
 *   head.rasterSize
 *     This structure specifies the total width and height of the raster that
 *     is sent to the display.  (The width and height are also referred to as
 *     HTotal and VTotal, respectively.)
 *
 *   head.rasterBlankStart
 *     X specifies the pixel column where horizontal blanking begins;
 *     Y specifies the pixel row where vertical blanking begins.
 *
 *   head.rasterBlankEnd
 *     X specifies the pixel column where horizontal blanking ends;
 *     Y specifies the pixel row where vertical blanking ends.
 *
 *   head.rasterVertBlank2
 *     X and Y specify the pixel column/row where horizontal/vertical blanking
 *     ends on the second field of every pair for an interlaced raster.  This
 *     field is not used when the raster is progressive.
 *
 *   head.control.masterLockMode
 *   head.control.masterLockPin
 *   head.control.slaveLockMode
 *   head.control.slaveLockPin
 *     Heads that are raster locked or frame locked together will have
 *     synchronized timing.  For example, vblank will occur at the same time on
 *     all of the heads that are locked together.
 *
 *     "LockMode" tells if a head is raster locked, frame locked, or not locked.
 *
 *     "LockPin" tells which heads are in a group of locked heads.  There
 *     should be one master per group, and all slave heads that are locked to
 *     that master should have the same slaveLockPin number as the master's
 *     masterLockPin number.
 *
 *     Note: The LockModes and LockPins are used only if the min v-pstate is
 *     required (i.e., if NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE
 *     is set).
 * 
 *   head.maxDownscaleFactorH
 *   head.maxDownscaleFactorV
 *     maxDownscaleFactorH and maxDownscaleFactorV represent the maximum amount
 *     by which the the composited image can be reduced in size, horizontally
 *     and vertically, respectively, multiplied by 0x400.  For example, if the
 *     scaler input width is 1024, and the scaler output width is 2048, the
 *     downscale factor would be 1024 / 2048 = 0.5, and multiplying by 0x400
 *     would give 512.
 *
 *   head.outputScalerVerticalTaps
 *     scalerVerticalTaps indicates the maximum number of vertical taps 
 *     allowed in the output scaler.
 * 
 *     Note that there are no #defines for tap values; the parameter is simply
 *     the number of taps (e.g., "2" for 2 taps).
 *
 *   head.bUpscalingAllowedV
 *     bUpscalingAllowed indicates whether or not the composited image can be
 *     increased in size, vertically.
 * 
 *   head.bOverfetchEnabled
 *     bOverfetchEnabled indicates whether or not the vertical overfetch is 
 *     enabled in postcomp scaler.
 *
 *   head.bLtmAllowed
 *     bLtmAllowed indicates whether or not the Local Tone Mapping (LTM) is 
 *     enabled in postcomp.
 *
 *   head.minFrameIdle.leadingRasterLines
 *     leadingRasterLines defines the number of lines between the start of the
 *     frame (vsync) and the start of the active region.  This includes Vsync,
 *     Vertical Back Porch, and the top part of the overscan border.  The
 *     minimum value is 2 because vsync and VBP must be at least 1 line each.
 *
 *   head.minFrameIdle.trailingRasterLines
 *     trailingRasterLines defines the number of lines between the end of the
 *     active region and the end of the frame.  This includes the bottom part
 *     of the overscan border and the Vertical Front Porch.
 *
 *   head.lut
 *     This parameter specifies whether or not the output LUT is enabled, and
 *     the size of the LUT.  The parameter should be an
 *     NVC372_CTRL_IMP_LUT_USAGE_xxx value.
 *     On Volta, the value should be one of these:
 *     NVC372_CTRL_IMP_LUT_USAGE_1025, NVC372_CTRL_IMP_LUT_USAGE_257 or
 *     NVC372_CTRL_IMP_LUT_USAGE_NONE
 *
 *     After Volta, the value should be one of these:
 *     NVC372_CTRL_IMP_LUT_USAGE_HW_MAX (this indicates that LUT is allowed) or
 *     NVC372_CTRL_IMP_LUT_USAGE_NONE
 *     (On older post-Volta products, clients may set other
 *      NVC372_CTRL_IMP_LUT_USAGE_xxx values, but they map to
 *      NVC372_CTRL_IMP_LUT_USAGE_HW_MAX in RM-SW.)
 *
 *   head.cursorSize32p
 *     This parameter specifies the width of the cursor, in units of 32 pixels.
 *     So, for example, "8" would mean 8 * 32 = 256, for a 256x256 cursor.  Zero
 *     means the cursor is disabled.
 *
 *   head.tileMask
 *     This parameter contains a bitmask specifying which tiles must be
 *     assigned to the head.  Normally, this parameter is set to zero, allowing
 *     IMP to calculate the number of tiles, but the tiles may be specified
 *     explicitly for test or debug.  If the mode is not possible with the
 *     specified number of tiles, IMP will report the result as such; the
 *     number of tiles will not be adjusted.
 * 
 *     If tiles are forced for only a subset of active heads, IMP will
 *     calculate the tiles for the remaining heads (if possible).
 * 
 *     If head.bEnableDsc is enabled, head.possibleDscSliceCountMask may
 *     optionally be used to force the number of DSC slices.
 *
 *   head.bEnableDsc
 *     bEnableDsc indicates whether or not DSC is enabled, by default.  If it
 *     is disabled by default, but head.possibleDscSliceCountMask is non-zero,
 *     IMP may still present tiling solutions with DSC enabled, but only if the
 *     mode is not possible otherwise.  (This will be indicated by a non-zero
 *     tileList.headDscSlices output.)
 *
 *   head.dscTargetBppX16
 *     dscTargetBppX16 is the DSC encoder's target bits per pixel, multiplied
 *     by 16.
 *
 *     This field is required only on systems that support tiling, and only if
 *     head.possibleDscSliceCountMask is true.
 *
 *   head.possibleDscSliceCountMask
 *     This is a bit mask indicating how many DSC slices are allowed in a
 *     scanline.  If a bit n is set in the bit mask, it means that one possible
 *     configuration has n+1 DSC slices per scanline.
 *
 *     This field is required only on systems that support tiling.
 *
 *   head.maxDscSliceWidth
 *     The maximum allowed DSC slice width is determined by spec restrictions
 *     and monitor capabilities.
 *
 *     This field is required only on systems that support tiling, and only if
 *     head.possibleDscSliceCountMask is true.
 *
 *   head.bYUV420Format
 *     This parameter indicates output format is YUV420.
 *     Refer to NVD_YUV420_Output_Functional_Description.docx for more details.
 *
 *   head.bIs2Head1Or
 *     This parameter specifies if the head operates in 2Head1Or mode.
 *     Refer to NVD_2_Heads_Driving_1_OR_Functional_Description.docx for more details.
 * 
 *   head.bGetOSLDOutput
 *     This parameter specifies if the client requires output for the OSLD 
 *     (One Shot Mode with Stall Lock Disabled) mode along with legacy outputs.
 * 
 *   head.bDisableMidFrameAndDWCFWatermark
 *     WAR for bug 200508242. 
 *     In linux it is possible that there will be no fullscreen window visible 
 *     for a head. For these cases we would not hit dwcf or midframe watermarks 
 *     leading to fbflcn timing out waiting on ok_to_switch and forcing mclk 
 *     switch. This could lead to underflows. So if that scenario is caught (by
 *     Display Driver) bDisableMidFrameAndDWCFWatermark will be set to true and 
 *     IMP will exclude dwcf and midframe contribution from the "is mclk switch 
 *     guaranteed" calculation for the bandwidth clients of that head.
 *
 *   window.windowIndex
 *     This is the hardware index number for the window.  Only active windows
 *     should be included in the input structure.
 *
 *   window.owningHead
 *     This is the hardware index of the head that will receive the window's
 *     output.
 * 
 *   window.formatUsageBound
 *     This parameter is a bitmask of all possible non-rotated mode data
 *     formats (NVC372_CTRL_FORMAT_xxx values).
 *
 *   window.rotatedFormatUsageBound
 *     This parameter is a bitmask of all possible rotated mode data formats
 *     (NVC372_CTRL_FORMAT_xxx values).
 *
 *   window.surfaceLayout
 *     This parameter is the surface layout of the window.  It is one of
 *     NVC372_CTRL_LAYOUT_xxx values.
 *     The default value of 0U would imply that SW uses legacy equations
 *     (pre NVD5.0) in its computation for fetch BW.
 *
 *   window.maxPixelsFetchedPerLine
 *     This parameter defines the maximum number of pixels that may need to be
 *     fetched in a single line for this window.  Often, this can be set to the
 *     viewportSizeIn.Width.  But if the window is known to be clipped, such
 *     that an entire line will never be fetched, then this parameter can be
 *     set to the clipped size (to improve the chances of the mode being
 *     possible, or possible at a lower v-pstate).
 * 
 *     In some cases, the value of this parameter must be increased by a few
 *     pixels in order to account for scaling overfetch, input chroma overfetch
 *     (420/422->444), and/or chroma output low pass filter overfetch
 *     (444->422/420).  This value is chip dependent; refer to the
 *     MaxPixelsFetchedPerLine parameter in nvdClass_01.mfs for the exact
 *     value.  In no case does the maxPixelsFetchedPerLine value need to exceed
 *     the surface width.
 *
 *   window.maxDownscaleFactorH
 *   window.maxDownscaleFactorV
 *     maxDownscaleFactorH and maxDownscaleFactorV represent the maximum amount
 *     by which the the window image can be reduced in size, horizontally and
 *     vertically, respectively, multiplied by 
 *     NVC372_CTRL_SCALING_FACTOR_MULTIPLIER. For example,
 *     if the scaler input width is 1024, and the scaler output width is 2048,
 *     the downscale factor would be 1024 / 2048 = 0.5, and multiplying by 
 *     NVC372_CTRL_SCALING_FACTOR_MULTIPLIER if 0x400 would give 512.
 *
 *   window.inputScalerVerticalTaps
 *     scalerVerticalTaps indicates the maximum number of vertical taps 
 *     allowed in the input scaler.
 *
 *     Note that there are no #defines for tap values; the parameter is simply
 *     the number of taps (e.g., "2" for 2 taps).
 *
 *   window.bUpscalingAllowedV
 *     bUpscalingAllowed indicates whether or not the composited image can be
 *     increased in size, vertically.
 *
 *   window.bOverfetchEnabled
 *     bOverfetchEnabled indicates whether or not the vertical overfetch is 
 *     enabled in precomp scaler.
 *
 *   window.lut
 *     This parameter specifies whether or not the input LUT is enabled, and
 *     the size of the LUT.  The parameter should be an
 *     NVC372_CTRL_IMP_LUT_USAGE_xxx value.
 *     On Volta, the value should be one of these:
 *     NVC372_CTRL_IMP_LUT_USAGE_1025, NVC372_CTRL_IMP_LUT_USAGE_257 or
 *     NVC372_CTRL_IMP_LUT_USAGE_NONE
 *
 *     After Volta, the value should be one of these:
 *     NVC372_CTRL_IMP_LUT_USAGE_HW_MAX (this indicates that LUT is allowed) or
 *     NVC372_CTRL_IMP_LUT_USAGE_NONE
 *     (On older post-Volta products, clients may set other
 *      NVC372_CTRL_IMP_LUT_USAGE_xxx values, but they map to
 *      NVC372_CTRL_IMP_LUT_USAGE_HW_MAX in RM-SW.)
 *
 *   window.tmoLut
 *     This parameter specifies whether or not the tmo LUT is enabled, and
 *     the size of the LUT. This lut is used for HDR.  The parameter should be
 *     an NVC372_CTRL_IMP_LUT_USAGE_xxx value.
 *     On Volta, the value should be one of these:
 *     NVC372_CTRL_IMP_LUT_USAGE_1025, NVC372_CTRL_IMP_LUT_USAGE_257 or
 *     NVC372_CTRL_IMP_LUT_USAGE_NONE
 *
 *     After Volta, the value should be one of these:
 *     NVC372_CTRL_IMP_LUT_USAGE_HW_MAX (this indicates that LUT is allowed) or
 *     NVC372_CTRL_IMP_LUT_USAGE_NONE
 *     (On older post-Volta products, clients may set other
 *      NVC372_CTRL_IMP_LUT_USAGE_xxx values, but they map to
 *      NVC372_CTRL_IMP_LUT_USAGE_HW_MAX in RM-SW.)
 *
 *   numHeads
 *     This is the number of heads in the "head" array of the
 *     NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS struct.  Only active heads should be
 *     included in the struct.
 *
 *   numWindows
 *     This is the number of windows in the "window" array of the
 *     NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS struct.  Only active windows should
 *     be included in the struct.
 *
 *   options
 *     This parameter specifies a bitmask for options.
 *
 *       NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_GET_MARGIN
 *         tells IMP to calculate worstCaseMargin and worstCaseDomain.
 *       NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE
 *         tells IMP to calculate and report the minimum v-pstate at which the
 *         mode is possible.
 *
 *   bUseCachedPerfState
 *     Indicates that RM should use cached values for the fastest
 *     available perf level (v-pstate for PStates 3.0 or pstate for
 *     PStates 2.0) and dispclk.  This feature allows the query call to
 *     execute faster, and is intended to be used, for example, during
 *     mode enumeration, when many IMP query calls are made in close
 *     succession, and perf conditions are not expected to change between
 *     query calls.  When IMP has not been queried recently, it is
 *     recommended to NOT use cached values, in case perf conditions have
 *     changed and the cached values no longer reflect the current
 *     conditions.
 *
 *   testMclkFreqKHz
 *     This is the mclk frequency specified by the client, in KHz.  RM will 
 *     use this value to compare with the minimum dramclk required by the 
 *     given mode.  The parameter will have value 0 if the client doesn't want 
 *     IMP query to consider this. This input is valid only on Tegra and only
 *     for verification purposes on internal builds.
 *     For this input to work, client must set 
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE in the
 *     "options" field.
 *
 * Outputs:
 *   bIsPossible
 *     This output tells if the specified mode can be supported.  To know if 
 *     the mode is possible with OSLD, the bIsOSLDPossible result must 
 *     be checked.
 *
 *   bIsOSLDPossible
 *     This output is returned for each head and suggests to the clients
 *     if the mode will be possible or not on that head when OSLD is enabled. 
 *     The output is only valid if bGetOSLDOutput is set in the head input.
 *     Note that bIsOSLDPossible is only valid if bIsPossible is true.
 *
 *   minImpVPState
 *     minImpVPState returns the minimum v-pstate at which the mode is possible
 *     (assuming bIsPossible is TRUE).  This output is valid only on dGPU, and
 *     only if NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set in
 *     the "options" field.
 *
 *     If the minimum v-pstate is required for a multi-head config, then
 *     masterLockMode, masterLockPin, slaveLockMode, and slaveLockPin must all
 *     be initialized.
 *   minPState
 *     minPState returns the pstate value corresponding to minImpVPState.  It
 *     is returned as the numeric value of the pstate (P0 -> 0, P1 -> 1, etc.).
 *     This output is valid only on dGPU, and only if
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set
 *     in the "options" field.
 *
 *     Note that the pstate returned by minPstateForGlitchless is not
 *     necessarily sufficient to meet IMP requirements.  The pstate corresponds
 *     to the vpstate returned by minImpVPState, and this vpstate represents
 *     clocks that are sufficient for IMP requirements, but the pstate
 *     typically covers a range of frequencies (depending on the clock), and it
 *     is possible that only part of the range is sufficient for IMP.
 *
 *   minRequiredBandwidthKBPS
 *     minRequiredBandwidthKBPS returns the minimum bandwidth that must be
 *     allocated to display in order to make the mode possible (assuming
 *     bIsPossible is TRUE).  This output is valid only on Tegra, and only if
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set in the
 *     "options" field.
 * 
 *   floorBandwidthKBPS
 *     floorBandwidthKBPS returns the minimum mclk frequency that can support
 *     the mode, and allow glitchless mclk switch, multiplied by the width of
 *     the data pipe.  (This is an approximation of the bandwidth that can be
 *     provided by the min required mclk frequency, ignoring overhead.)  If the
 *     mode is possible, but glitchless mclk switch is not, floorBandwidthKBPS
 *     will be calculated based on the maximum possible mclk frequency.  This
 *     output is valid only on Tegra, and only if
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set in the
 *     "options" field.
 *
 *   minRequiredHubclkKHz
 *     minRequiredHubclkKHz returns the minimum hubclk frequency that can 
 *     support the mode.  This output is valid only on Tegra, and only if 
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set in the
 *     "options" field.
 *
 *   vblankIncreaseInLinesForOSLDMode
 *     vblankIncreaseInLinesForOSLDMode returns the amount, in lines, by 
 *     which vblank needs to be extended to achieve optimized MSCG when OSLD 
 *     Mode is enabled (assuming bIsPossible is TRUE). Features like Panel
 *     Replay and Panel Self Refresh enable OSLD mode. This value is 0 if 
 *     the vblank is large enough to accommodate spool up and MSCG latencies.
 *     This output is valid only if 
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set in the
 *     "options" field and bGetOSLDOutput was specified in the head input.
 *
 *   wakeUpRgLineForOSLDMode
 *     wakeUpRgLineForOSLDMode returns the rg line in the vblank region at which 
 *     the clients will be required to send a timestamped update to achieve
 *     optimized MSCG when OSLD Mode is enabled (assuming bIsPossible is 
 *     is TRUE).  Features like Panel Replay and Panel Self Refresh enable 
 *     OSLD mode. This output is valid only if 
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE was set in the
 *     "options" field and bGetOSLDOutput was specified in the head input.
 * 
 *   worstCaseMargin
 *     worstCaseMargin returns the ratio of available bandwidth to required
 *     bandwidth, multiplied by NV5070_CTRL_IMP_MARGIN_MULTIPLIER.  Available
 *     bandwidth is calculated in the worst case bandwidth domain, i.e., the
 *     domain with the least available margin.  Bandwidth domains include the
 *     IMP-relevant clock domains, and possibly other virtual bandwidth
 *     domains such as AWP.
 *
 *     Note that IMP checks additional parameters besides the bandwidth margins
 *     but only the bandwidth margin is reported here, so it is possible for a
 *     mode to have a more restrictive domain that is not reflected in the
 *     reported margin result.
 *
 *     This result is not guaranteed to be valid if the mode is not possible.
 *
 *     Note also that the result is generally calculated for the highest
 *     v-pstate possible (usually P0).  But if the _NEED_MIN_VPSTATE is
 *     specified, the result will be calculated for the min possible v-pstate
 *     (or the highest possible v-pstate, if the mode is not possible).
 * 
 *     The result is valid only if
 *     NV5070_CTRL_IS_MODE_POSSIBLE_OPTIONS_GET_MARGIN is set in "options".
 *
 *   dispClkKHz
 *     This is the dispclk frequency selected by IMP for this mode. For dGPU,
 *     it will be one of the fixed frequencies from the list of frequencies 
 *     supported by the vbios.
 *
 *   worstCaseDomain
 *     Returns a short text string naming the domain for the margin returned in
 *     "worstCaseMargin".  See "worstCaseMargin" for more information.
 *
 *   numTilingAssignments
 *     This is the number of entries in the tilingAssignments array, each of
 *     which represents a possible distinct tiling configuration.  The client
 *     will do the actual assignment of tiles during the modeset, but the
 *     information provided here will help the client to know how many tiles
 *     to assign to each head.
 *
 *     The first tiling assignment (tilingAssignments[0]) is required; it will
 *     specify that one or more tiles must be assigned to each active head in
 *     order for the mode to be possible.  Subsequent tiling assignments are
 *     optional; each higher assignment will reduce the required dispclk to a
 *     lower frequency, so the client may choose to use some or all of these
 *     assignments because they might reduce power consumption.
 *
 *     The tiling assignments are incremental; the client may choose how many
 *     assignments to use (after the first one, which is required), but they
 *     must be used in sequence.  For example, if there are five possible
 *     assignments (numTilingAssignments = 5), and the client wishes to apply
 *     the third assignment, the tiles from the first two assignments must also
 *     be allocated.
 *
 *     If the client decides to use a particular tiling assignment, it should
 *     assign all tiles specified in that assignment.  (Otherwise, there will
 *     be no benefit from that assignment for reducing dispclk.)
 *
 *     A 3-head example of a set of tiling assignments is:
 *       numTilingAssignments = 3
 *       tilingAssignments[0].numTiles = 4
 *       tilingAssignments[1].numTiles = 2
 *       tilingAssignments[2].numTiles = 1
 *       tileList[0].head = 0   (first tile for tilingAssignments[0])
 *       tileList[0].headDscSlices = 2
 *       tileList[1].head = 0   (second tile for tilingAssignments[0])
 *       tileList[1].headDscSlices = xxx (not used, because it is specified in
 *                                   tileList[0].headDscSlices)
 *       tileList[2].head = 1   (third tile for tilingAssignments[0])
 *       tileList[2].headDscSlices = 1
 *       tileList[3].head = 2   (fourth tile for tilingAssignments[0])
 *       tileList[3].headDscSlices = 1
 *       tileList[4].head = 1   (first tile for tilingAssignments[1])
 *       tileList[4].headDscSlices = 2
 *       tileList[5].head = 2   (second tile for tilingAssignments[1])
 *       tileList[5].headDscSlices = 2
 *       tileList[6].head = 0   (tile for tilingAssignments[2]
 *       tileList[6].headDscSlices = 3
 *
 *     tilingAssignments[0] always specifies the minimum tiling assignment
 *     necessary to make the mode possible.  In this example, two tiles are
 *     required on head 0, but heads 1 and 2 can work with a single tile each.
 *
 *     After the four required tiles are assigned for tilingAssignments[0], the
 *     client may choose to apply tilingAssignment[1] as well, to reduce
 *     dispclk further.  Two additional tiles would be required for this, one
 *     on head 1 and one on head 2.  Note that there would be no benefit to
 *     assigning a tile to only one of these two heads; all heads specified in
 *     the tilingAssignment must be assigned (if the tilingAssignment is to be
 *     used).  After this assignment, head 1 and head 2 would each have two
 *     tiles assigned (one from tilingAssignment[0] and one from
 *     tilingAssignent[1]).  Head 0 would still have 2 tiles assigned.
 *
 *     If tilingAssignments[2] is also used, an additional tile would be
 *     assigned to head 0, bringing the tile total to three for that head.  The
 *     number of DSC slices required for that head would be increased to three.
 *
 *     Note that the tiling assignments do not specify which tiles to use; they
 *     only specify how many tiles to assign to each head.  The client must
 *     choose which tiles to assign, based on their capabilities.
 *
 *   tilingAssignments.numTiles
 *     This is the number of additional tiles required for the indexed tiling
 *     assignment.  The tilingAssignment does not provide any benefit unless
 *     all of its specified tiles are assigned.
 *
 *   tileList.head
 *     This specifies the head to which a tile must be assigned, to receive a
 *     benefit (dispclk reduction) for a given tiling assignment.
 *     tileList entries (head indexes) are assigned consecutively, based on the
 *     tilingAssignments.numTiles entries.  For example, if
 *     tilingAssignments[0].numTiles = 3 and tilingAssignments[1].numTiles = 2,
 *     then the first three tileList entries (indexes 0, 1, and 2) would be for tiling
 *     assignment 0 and the next 2 entries (indexes 3 and 4) would be for
 *     tiling assignment 1.
 *
 *     A single assignment may have multiple tileList.head entries for the same
 *     head (if a single head requires that more than one additional tile be
 *     assigned).
 *
 *     tilelist.head indexes heads as they are indexed in the
 *     NVC372_CTRL_IMP_HEAD array within the IMP input data structure.  (These
 *     do not necessarily correspond to physical head indexes.)
 *
 *   tileList.headDscSlices
 *     headDscSlices gives the recommended number of DSC slices for each
 *     scanline for the head specified in tileList.head.  If a specific tiling
 *     assignment has multiple tiles assigned to the same head, the
 *     headDscSlices value for the first tileList entry should be used;
 *     subsequent entries may be ignored.  If multiple tilingAssignments are
 *     applied, the headDscSlices entry for the highest indexed
 *     tilingAssignment takes precedence over any entries from lower indexed
 *     assignments, for the same head.
 *
 *     This field is relevant only if head.bEnableDsc is true.
 *
 * Possible status values returned are:
 *     NVOS_STATUS_SUCCESS
 *     NVOS_STATUS_ERROR_GENERIC
 */
#define NVC372_CTRL_IMP_LUT_USAGE_NONE   0
#define NVC372_CTRL_IMP_LUT_USAGE_257    1
#define NVC372_CTRL_IMP_LUT_USAGE_1025   2
#define NVC372_CTRL_IMP_LUT_USAGE_HW_MAX 3

typedef struct NVC372_CTRL_IMP_HEAD {
    NvU8  headIndex;

    NvU32 maxPixelClkKHz;

    struct {
        NvU32 width;
        NvU32 height;
    } rasterSize;

    struct {
        NvU32 X;
        NvU32 Y;
    } rasterBlankStart;

    struct {
        NvU32 X;
        NvU32 Y;
    } rasterBlankEnd;

    struct {
        NvU32 yStart;
        NvU32 yEnd;
    } rasterVertBlank2;

    struct {
        NV_DISP_LOCK_MODE masterLockMode;
        NV_DISP_LOCK_PIN  masterLockPin;
        NV_DISP_LOCK_MODE slaveLockMode;
        NV_DISP_LOCK_PIN  slaveLockPin;
    } control;

    NvU32  maxDownscaleFactorH;
    NvU32  maxDownscaleFactorV;
    NvU8   outputScalerVerticalTaps;
    NvBool bUpscalingAllowedV;
    NvBool bOverfetchEnabled;
    NvBool bLtmAllowed;

    struct {
        NvU16 leadingRasterLines;
        NvU16 trailingRasterLines;
    } minFrameIdle;

    NvU8   lut;
    NvU8   cursorSize32p;

    NvU8   tileMask;

    NvBool bEnableDsc;

    NvU16  dscTargetBppX16;

    NvU32  possibleDscSliceCountMask;

    NvU32  maxDscSliceWidth;

    NvBool bYUV420Format;

    NvBool bIs2Head1Or;

    NvBool bGetOSLDOutput;

    NvBool bDisableMidFrameAndDWCFWatermark;
} NVC372_CTRL_IMP_HEAD;
typedef struct NVC372_CTRL_IMP_HEAD *PNVC372_CTRL_IMP_HEAD;

typedef struct NVC372_CTRL_IMP_WINDOW {
    NvU32  windowIndex;
    NvU32  owningHead;
    NvU32  formatUsageBound;
    NvU32  rotatedFormatUsageBound;
    NvU32  maxPixelsFetchedPerLine;
    NvU32  maxDownscaleFactorH;
    NvU32  maxDownscaleFactorV;
    NvU8   inputScalerVerticalTaps;
    NvBool bUpscalingAllowedV;
    NvBool bOverfetchEnabled;
    NvU8   lut;
    NvU8   tmoLut;
    NvU8   surfaceLayout;
} NVC372_CTRL_IMP_WINDOW;
typedef struct NVC372_CTRL_IMP_WINDOW *PNVC372_CTRL_IMP_WINDOW;

typedef struct NVC372_TILING_ASSIGNMENT {
    NvU8 numTiles;
} NVC372_TILING_ASSIGNMENT;

typedef struct NVC372_TILE_ENTRY {
    NvU8 head;
    NvU8 headDscSlices;
} NVC372_TILE_ENTRY;

#define NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_GET_MARGIN       (0x00000001)
#define NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE (0x00000002)

#define NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS {
    NVC372_CTRL_CMD_BASE_PARAMS base;

    NvU8                        numHeads;
    NvU8                        numWindows;

    NVC372_CTRL_IMP_HEAD        head[NVC372_CTRL_MAX_POSSIBLE_HEADS];

    // C form: NVC372_CTRL_IMP_WINDOW window[NVC372_CTRL_MAX_POSSIBLE_WINDOWS];
    NVC372_CTRL_IMP_WINDOW      window[NVC372_CTRL_MAX_POSSIBLE_WINDOWS];

    NvU32                       options;

    NvU32                       testMclkFreqKHz;

    NvBool                      bIsPossible;

    NvBool                      bIsOSLDPossible[NVC372_CTRL_MAX_POSSIBLE_HEADS];

    NvU32                       minImpVPState;

    NvU32                       minPState;

    NvU32                       minRequiredBandwidthKBPS;

    NvU32                       floorBandwidthKBPS;

    NvU32                       minRequiredHubclkKHz;

    NvU32                       vblankIncreaseInLinesForOSLDMode[NVC372_CTRL_MAX_POSSIBLE_HEADS];

    NvU32                       wakeUpRgLineForOSLDMode[NVC372_CTRL_MAX_POSSIBLE_HEADS];

    NvU32                       worstCaseMargin;

    NvU32                       dispClkKHz;

    NvU32                       numTilingAssignments;

    NVC372_TILING_ASSIGNMENT    tilingAssignments[NVC372_CTRL_MAX_POSSIBLE_TILES];

    NVC372_TILE_ENTRY           tileList[NVC372_CTRL_MAX_POSSIBLE_TILES];

    char                        worstCaseDomain[8];

    NvBool                      bUseCachedPerfState;
} NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS;
typedef struct NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *PNVC372_CTRL_IS_MODE_POSSIBLE_PARAMS;

/* valid format values */
#define NVC372_CTRL_FORMAT_RGB_PACKED_1_BPP                           (0x00000001)
#define NVC372_CTRL_FORMAT_RGB_PACKED_2_BPP                           (0x00000002)
#define NVC372_CTRL_FORMAT_RGB_PACKED_4_BPP                           (0x00000004)
#define NVC372_CTRL_FORMAT_RGB_PACKED_8_BPP                           (0x00000008)
#define NVC372_CTRL_FORMAT_YUV_PACKED_422                             (0x00000010)
#define NVC372_CTRL_FORMAT_YUV_PLANAR_420                             (0x00000020)
#define NVC372_CTRL_FORMAT_YUV_PLANAR_444                             (0x00000040)
#define NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_420                        (0x00000080)
#define NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_422                        (0x00000100)
#define NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_422R                       (0x00000200)
#define NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_444                        (0x00000400)
#define NVC372_CTRL_FORMAT_EXT_YUV_PLANAR_420                         (0x00000800)
#define NVC372_CTRL_FORMAT_EXT_YUV_PLANAR_444                         (0x00001000)
#define NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_420                    (0x00002000)
#define NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_422                    (0x00004000)
#define NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_422R                   (0x00008000)
#define NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_444                    (0x00010000)

/* valid layout values */
#define NVC372_CTRL_LAYOUT_PITCH_BLOCKLINEAR                          0
#define NVC372_CTRL_LAYOUT_PITCH                                      1
#define NVC372_CTRL_LAYOUT_BLOCKLINEAR                                2

/* valid impResult values */
#define NVC372_CTRL_IMP_MODE_POSSIBLE                                 0
#define NVC372_CTRL_IMP_NOT_ENOUGH_MEMPOOL                            1
#define NVC372_CTRL_IMP_REQ_LIMIT_TOO_HIGH                            2
#define NVC372_CTRL_IMP_VBLANK_TOO_SMALL                              3
#define NVC372_CTRL_IMP_HUBCLK_TOO_LOW                                4
#define NVC372_CTRL_IMP_INSUFFICIENT_BANDWIDTH                        5
#define NVC372_CTRL_IMP_DISPCLK_TOO_LOW                               6
#define NVC372_CTRL_IMP_ELV_START_TOO_HIGH                            7
#define NVC372_CTRL_IMP_OSLD_ELV_START_TOO_HIGH                       8
#define NVC372_CTRL_IMP_INSUFFICIENT_THREAD_GROUPS                    9
#define NVC372_CTRL_IMP_INVALID_PARAMETER                             10
#define NVC372_CTRL_IMP_UNRECOGNIZED_FORMAT                           11
#define NVC372_CTRL_IMP_UNSPECIFIED                                   12

/*
 * The calculated margin is multiplied by a constant, so that it can be
 * represented as an integer with reasonable precision.  "0x400" was chosen
 * because it is a power of two, which might allow some compilers/CPUs to
 * simplify the calculation by doing a shift instead of a multiply/divide.
 * (And 0x400 is 1024, which is close to 1000, so that may simplify visual
 * interpretation of the raw margin value.)
 */
#define NVC372_CTRL_IMP_MARGIN_MULTIPLIER                             (0x00000400)

/* scaling factor */
#define NVC372_CTRL_SCALING_FACTOR_MULTIPLIER                         (0x00000400)

#define NVC372_CTRL_CMD_NUM_DISPLAY_ID_DWORDS_PER_HEAD                2
#define NVC372_CTRL_CMD_MAX_SORS                                      4

#define NVC372_CTRL_CMD_IS_MODE_POSSIBLE_OR_SETTINGS                  (0xc3720102) /* finn: Evaluated from "(FINN_NVC372_DISPLAY_SW_CHNCTL_INTERFACE_ID << 8) | NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS_MESSAGE_ID" */

/* 
 * NVC372_CTRL_CMD_IS_MODE_POSSIBLE_OR_SETTINGS
 *
 * This command tells us if output resource pixel clocks requested by client
 * is possible or not. Note that this will not be used for displayport sor as 
 * it will be handled by displayport library.
 * 
 * Inputs: 
 *   numHeads
 *     This is the number of heads in the "head" array of the
 *     NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS struct.  Only active heads 
 *     should be included in the struct.
 *
 *   head.headIndex
 *     This is the hardware index number for the head.  Only an active head
 *     should be included in the input structure.
 *
 *   head.maxPixelClkKHz
 *     This parameter specifies the pixel scanout rate (in KHz).
 *
 *   head.displayId
 *     Array of displayId's associated with the head. This is limited by
 *     NVC372_CTRL_CMD_NUM_DISPLAY_ID_DWORDS_PER_HEAD.
 *
 *   sor.ownerMask
 *     Consists of a mask of all heads that drive this sor.
 *
 *   sor.protocol
 *     Defines the protocol of the sor in question.
 *
 *   sor.pixelReplicateMode
 *     Defines which pixel replication mode is requested. This can be off
 *     or X2 or X4 mode.
 *
 * Outputs:
 *   bIsPossible
 *     This tells us that the requested pixel clock can be supported.
 */


#define NVC372_CTRL_IS_MODE_POSSIBLE_DISPLAY_ID_SKIP_IMP_OUTPUT_CHECK (0xAAAAAAAA)

typedef struct NVC372_CTRL_IMP_OR_SETTINGS_HEAD {
    NvU8                               headIndex;
    NvU32                              maxPixelClkKhz;

    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP outputResourcePixelDepthBPP;

    NvU32                              displayId[NVC372_CTRL_CMD_NUM_DISPLAY_ID_DWORDS_PER_HEAD];
} NVC372_CTRL_IMP_OR_SETTINGS_HEAD;
typedef struct NVC372_CTRL_IMP_OR_SETTINGS_HEAD *PNVC372_CTRL_IMP_OR_SETTINGS_HEAD;

#define NVC372_CTRL_CMD_SOR_OWNER_MASK_NONE                   (0x00000000)
#define NVC372_CTRL_CMD_SOR_OWNER_MASK_HEAD(i)                          (1 << i)

#define NVC372_CTRL_CMD_SOR_PROTOCOL_SINGLE_TMDS_A            (0x00000000)
#define NVC372_CTRL_CMD_SOR_PROTOCOL_SINGLE_TMDS_B            (0x00000001)
#define NVC372_CTRL_CMD_SOR_PROTOCOL_DUAL_TMDS                (0x00000002)
#define NVC372_CTRL_CMD_SOR_PROTOCOL_SUPPORTED                (0xFFFFFFFF)

#define NVC372_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_OFF (0x00000000)
#define NVC372_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_X2  (0x00000001)
#define NVC372_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_X4  (0x00000002)

typedef struct NVC372_CTRL_IMP_OR_SETTINGS_SOR {
    NvU32 ownerMask;
    NvU32 protocol;
    NvU32 pixelReplicateMode;
} NVC372_CTRL_IMP_OR_SETTINGS_SOR;
typedef struct NVC372_CTRL_IMP_OR_SETTINGS_SOR *PNVC372_CTRL_IMP_OR_SETTINGS_SOR;

#define NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS {
    NVC372_CTRL_CMD_BASE_PARAMS      base;

    NvU32                            numHeads;

    NVC372_CTRL_IMP_OR_SETTINGS_HEAD head[NVC372_CTRL_MAX_POSSIBLE_HEADS];

    NVC372_CTRL_IMP_OR_SETTINGS_SOR  sor[NVC372_CTRL_CMD_MAX_SORS];

    NvBool                           bIsPossible;
} NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS;
typedef struct NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *PNVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS;

#define NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE (0xc3720103) /* finn: Evaluated from "(FINN_NVC372_DISPLAY_SW_CHNCTL_INTERFACE_ID << 8) | NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS_MESSAGE_ID" */

/*
 * NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE
 *
 * This control call is used by clients to inform RM about video adaptive refresh rate enable/disable.
 * Based on the state, RM will enable/disable supported low power features.
 *
 * Inputs:
 *   displayID
 *      displayId of panel on which video adaptive refresh rate is enabled/disabled.
 *
 *   bEnable
 *      NV_TRUE to enable video adaptive refresh rate mode.
 *      NV_FALSE to disable video adaptive refresh rate mode.
 *
 * Outputs:
 *   Possible status values returned are:
 *      NV_OK
 *      NV_ERR_NOT_SUPPORTED
 */

#define NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS {
    NvU32  displayID;
    NvBool bEnable;
} NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS;
typedef struct NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *PNVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS;


#define NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN (0xc3720104) /* finn: Evaluated from "(FINN_NVC372_DISPLAY_SW_CHNCTL_INTERFACE_ID << 8) | NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS_MESSAGE_ID" */

/*
 * NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN
 *
 * This control call is used by clients to query the active viewport for the
 * provided window precalculated at the beginning of each frame.
 *
 * Inputs:
 *   windowIndex
 *      Index of the window to be queried.  Must be connected to an active head.
 *
 * Outputs:
 *   activeViewportPointIn
 *      X and Y coordinates of the active viewport on the provided window for
 *      the most recent frame.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT if the window index is invalid
 *      NV_ERR_INVALID_STATE if the window index isn't connected to a head
 *      NV_ERR_NOT_SUPPORTED
 */
#define NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS {
    NVC372_CTRL_CMD_BASE_PARAMS base;

    NvU32                       windowIndex;

    struct {
        NvU32 x;
        NvU32 y;
    } activeViewportPointIn;
} NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS;
typedef struct NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *PNVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS;

/* _ctrlc372chnc_h_ */
