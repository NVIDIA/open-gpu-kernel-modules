/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_wardatabase.cpp                                                *
*         EDID and OUI based workarounds for panel/TCON issues              *
*                                                                           *
\***************************************************************************/
#include "dp_wardatabase.h"
#include "dp_edid.h"
#include "dp_connectorimpl.h"
#include "dp_printf.h"

using namespace DisplayPort;

void ConnectorImpl::applyOuiWARs()
{
    switch (ouiId)
    {
        // Megachips Mystique
        case 0xE18000:
            if (((modelName[0] == 'D') && (modelName[1] == 'p') && (modelName[2] == '1') &&
                        (modelName[3] == '.') && (modelName[4] == '1')))
            {
                //
                // Mystique based link box for HTC Vive has a peculiar behaviour
                // of sending a link retraining pulse if the link is powered down in the absence
                // of an active stream. Bug# 1793084. Set the flag so that link is not powered down.
                //
                bKeepOptLinkAlive = true;
            }

            if (((modelName[0] == 'D') && (modelName[1] == 'p') && (modelName[2] == '1') &&
                        (modelName[3] == '.') && (modelName[4] == '2')))
            {
                //
                // ASUS monitor loses link sometimes during assessing link or link training.
                // So if we retrain link by lowering config from HBR2 to HBR we see black screen
                // Set the flag so that we first retry link training with same link config
                // before following link training fallback. Bug #1846925
                //
                bNoFallbackInPostLQA = true;
            }
            break;

        // Synaptics
        case 0x24CC90:
            if ((modelName[0] == 'S') && (modelName[1] == 'Y') && (modelName[2] == 'N') &&
                (modelName[3] == 'A') && (((modelName[4] == 'S') &&
                ((modelName[5] == '1') || (modelName[5] == '2') ||
                 (modelName[5] == '3') || (modelName[5] == '#') ||
                 (modelName[5] == '\"')))||((modelName[4] == 0x84) &&
                 (modelName[5] == '0'))))
            {
                //
                // Extended latency from link-train end to FEC enable pattern
                // to avoid link lost or blank screen with Synaptics branch.
                //
                LT2FecLatencyMs = 57;

                if (bDscMstCapBug3143315)
                {
                    //
                    // Synaptics branch device doesn't support Virtual Peer Devices so DSC
                    // capability of downstream device should be decided based on device's own
                    // and its parent's DSC capability
                    //
                    bDscCapBasedOnParent = true;
                }
            }
            break;
    }
}

void Edid::applyEdidWorkArounds(NvU32 warFlag, const DpMonitorDenylistData *pDenylistData)
{

    unsigned ManufacturerID = this->getManufId();
    unsigned ProductID = this->getProductId();
    unsigned YearWeek = this->getYearWeek();

    //
    // Work around EDID problems, using manufacturer, product ID, and date of manufacture,
    // to identify each case.
    //
    switch (ManufacturerID)
    {
        // Apple
        case 0x1006:
            if (0x9227 == ProductID)
            {
                this->WARFlags.powerOnBeforeLt = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> WAR for Apple thunderbolt J29 panel");
                DP_PRINTF(DP_NOTICE, "DP-WAR>     - Monitor needs to be powered up before LT. Bug 933051");
            }
            break;

        // Acer
        case 0x7204:
            // Bug 451868: Acer AL1512 monitor has a wrong extension count:
            if(0xad15 == ProductID && YearWeek <= 0x0d01)
            {
                // clear the extension count
                buffer.data[0x7E] = 0;
                this->WARFlags.extensionCountDisabled = true;
                this->WARFlags.dataForced = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Edid override on Acer AL1512");
                DP_PRINTF(DP_NOTICE, "DP-WAR>     - Disabling extension count.Bug 451868");
            }
            break;

        // Westinghouse
        case 0x855C:

            // Westinghouse 37" 1080p TV.  LVM-37w3  (Port DVI1 EDID).
            // Westinghouse 42" 1080p TV.  LVM-42w2  (Port DVI1 EDID).
            if (ProductID == 0x3703 || ProductID == 0x4202)
            {
                // Claims HDMI support, but audio causes picture corruption.
                // Removing HDMI extension block

                if (buffer.getLength() > 0x80 &&
                    buffer.data[0x7E] == 1 &&             // extension block present
                    buffer.data[0x80] == 0x02 &&          // CEA block
                    buffer.data[0x81] == 0x03 &&          //    revision 3
                    !(buffer.data[0x83] & 0x40))          //  No basic audio, must not be the HDMI port
                {
                    // clear the extension count
                    buffer.data[0x7E] = 0;
                    this->WARFlags.extensionCountDisabled = true;
                    this->WARFlags.dataForced = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on Westinghouse AL1512 LVM- <37/42> w <2/3>");
                    DP_PRINTF(DP_NOTICE, "DP-WAR>     - Disabling extension count.");
                }
            }
            break;

        // IBM
        case 0x4D24:
            if(ProductID == 0x1A03)
            {
                // 2001 Week 50
                if (YearWeek == 0x0B32)
                {
                    // Override IBM T210. IBM T210 reports 2048x1536x60Hz in the edid but it's
                    // actually 2048x1536x40Hz. See bug 76347. This hack was, earlier, in disp driver
                    // Now it's being moved down to keep all overrides in same place.
                    // This hack was also preventing disp driver from comparing entire edid when
                    // trying to figure out whether or not the edid for some device has changed.
                    buffer.data[0x36] = 0x32;
                    buffer.data[0x37] = 0x3E;
                    this->WARFlags.dataForced = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on IBM T210");
                    DP_PRINTF(DP_NOTICE, "DP-WAR>    2048x1536x60Hz(misreported) -> 2048x1536x40Hz. Bug 76347");
                }
            }
            break;
        // GWY (Gateway) or EMA (eMachines)
        case 0xF91E: // GWY
        case 0xA115: // EMA
            // Some Gateway monitors present the eMachines mfg code, so these two cases are combined.
            // Future fixes may require the two cases to be separated.
            // Fix for Bug 343870.  NOTE: Problem found on G80; fix applied to all GPUs.
            if ((ProductID >= 0x0776 ) && (ProductID <= 0x0779)) // Product id's range from decimal 1910 to 1913
            {
                // if detailed pixel clock frequency = 106.50MHz
                if ( (buffer.data[0x36] == 0x9A) &&
                     (buffer.data[0x37] == 0x29) )
                {
                    // then change detailed pixel clock frequency to 106.54MHz to fix bug 343870
                    buffer.data[0x36] = 0x9E;
                    buffer.data[0x37] = 0x29;
                    this->WARFlags.dataForced = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on GWY/EMA");
                    DP_PRINTF(DP_NOTICE, "DP-WAR>   106.50MHz(misreported) -> 106.50MHz.Bug 343870");
                }
            }
            break;

        // INX
        case 0x2C0C:
            // INX L15CX monitor has an invalid detailed timing 10x311 @ 78Hz.
            if( ProductID == 0x1502)
            {
                // remove detailed timing #4: zero out the first 3 bytes of DTD#4 block
                buffer.data[0x6c] = 0x0;
                buffer.data[0x6d] = 0x0;
                buffer.data[0x6e] = 0x0;
                this->WARFlags.dataForced = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on INX L15CX");
                DP_PRINTF(DP_NOTICE, "DP-WAR>   Removing invalid detailed timing 10x311 @ 78Hz");
            }
            break;

        // AUO
        case 0xAF06:
            if ((ProductID == 0x103C) || (ProductID == 0x113C))
            {
                //
                // Acer have faulty AUO eDP panels which have
                // wrong HBlank in the EDID. Correcting it here.
                //
                buffer.data[0x39] = 0x4B; // new hblank width: 75
                buffer.data[0x3F] = 0x1B; // new hsync pulse width: 27
                this->WARFlags.dataForced = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on AUO eDP panel");
                DP_PRINTF(DP_NOTICE, "DP-WAR> Modifying HBlank and HSync pulse width.");
                DP_PRINTF(DP_NOTICE, "DP-WAR> Bugs 907998, 1001160");
            }
            else if (ProductID == 0x109B || ProductID == 0x119B)
            {
                this->WARFlags.useLegacyAddress = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> AUO eDP");
                DP_PRINTF(DP_NOTICE, "implements only Legacy interrupt address range");

                // Bug 1792962 - Panel got glitch on D3 write, apply this WAR.
                this->WARFlags.disableDpcdPowerOff = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Disable DPCD Power Off");
            }
            break;

        // LPL
        case 0x0C32:
            if (ProductID == 0x0000)
            {
                //
                // Patch EDID for Quanta - Toshiba LG 1440x900 panel.  See Bug 201428
                // Must 1st verify that we have that panel.  It has MFG id 32, 0C
                //    BUT product ID for this (and other different LG panels) are 0000.
                //    So verify that the last "Custom Timing" area of the EDID has
                //    a "Monitor Description" of type FE = "ASCII Data String" which
                //    has this panel's name = "LP171WX2-A4K5".
                //
                if ( (buffer.data[0x71] == 0x4C) &&
                    (buffer.data[0x72] == 0x50) &&
                    (buffer.data[0x73] == 0x31) &&
                    (buffer.data[0x74] == 0x37) &&
                    (buffer.data[0x75] == 0x31) &&
                    (buffer.data[0x76] == 0x57) &&
                    (buffer.data[0x77] == 0x58) &&
                    (buffer.data[0x78] == 0x32) &&
                    (buffer.data[0x79] == 0x2D) &&
                    (buffer.data[0x7A] == 0x41) &&
                    (buffer.data[0x7B] == 0x34) &&
                    (buffer.data[0x7C] == 0x4B) &&
                    (buffer.data[0x7D] == 0x35) )
                {
                    //
                    // Was 0x95, 0x25 = -> 0x2595 = 9621 or 96.21 Mhz.
                    //     96,210,000 / 1760 / 912 = 59.939 Hz
                    // Want 60 * 1760 * 912 ~= 9631 or 96.31 MHz
                    //     9631 = 0x259F -> 0x9F 0x25.
                    // So, change byte 36 from 0x95 to 0x9F.
                    //
                    buffer.data[0x36] = 0x9F;
                    this->WARFlags.dataForced = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on Quanta - Toshiba LG 1440x900");
                    DP_PRINTF(DP_NOTICE, "DP-WAR>   Correcting pclk. Bug 201428");
                }
            }
            else
            if (ProductID == 0xE300)
            {
                //
                // Patch EDID for MSI - LG LPL 1280x800 panel.  See Bug 359313
                // Must 1st verify that we have that panel.  It has MFG id 32, 0C
                //    BUT product ID for this (and other different LG panels) are E300.
                //    So verify that the last "Custom Timing" area of the EDID has
                //    a "Monitor Description" of type FE = "ASCII Data String" which
                //    has this panel's name = "LP154WX4-TLC3".
                //
                if ( (buffer.data[0x71] == 0x4C) &&
                    (buffer.data[0x72] == 0x50) &&
                    (buffer.data[0x73] == 0x31) &&
                    (buffer.data[0x74] == 0x35) &&
                    (buffer.data[0x75] == 0x34) &&
                    (buffer.data[0x76] == 0x57) &&
                    (buffer.data[0x77] == 0x58) &&
                    (buffer.data[0x78] == 0x34) &&
                    (buffer.data[0x79] == 0x2D) &&
                    (buffer.data[0x7A] == 0x54) &&
                    (buffer.data[0x7B] == 0x4C) &&
                    (buffer.data[0x7C] == 0x43) &&
                    (buffer.data[0x7D] == 0x33) )
                {
                    //
                    // Was 0xBC, 0x1B = -> 0x1BBC = 7100 or 71.00 Mhz.
                    //     71,000,000 / 1488 / 826 = 59.939 Hz
                    // Want 60 * 1488 * 826 ~= 7111 or 71.11 MHz
                    //     7111 = 0x1BC7 -> 0xC7 0x1B.
                    // So, change byte 36 from 0xBC to 0xC7.
                    //
                    buffer.data[0x36] = 0xC7;
                    this->WARFlags.dataForced = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on  MSI - LG LPL 1280x800");
                    DP_PRINTF(DP_NOTICE, "DP-WAR>   Correcting pclk. Bug 359313");
                }
            }
            break;

       // SKY
       case 0x794D:
            if (ProductID == 0x9880)
            {
                //
                // Override for Haier TV to remove resolution
                // 1366x768 from EDID data. Refer bug 351680 & 327891
                // Overriding 18 bytes from offset 0x36.
                //
                buffer.data[0x36] = 0x01;
                buffer.data[0x37] = 0x1D;
                buffer.data[0x38] = 0x00;
                buffer.data[0x39] = 0x72;
                buffer.data[0x3A] = 0x51;
                buffer.data[0x3B] = 0xD0;
                buffer.data[0x3C] = 0x1E;
                buffer.data[0x3D] = 0x20;
                buffer.data[0x3E] = 0x6E;
                buffer.data[0x3F] = 0x28;
                buffer.data[0x40] = 0x55;
                buffer.data[0x41] = 0x00;
                buffer.data[0x42] = 0xC4;
                buffer.data[0x43] = 0x8E;
                buffer.data[0x44] = 0x21;
                buffer.data[0x45] = 0x00;
                buffer.data[0x46] = 0x00;
                buffer.data[0x47] = 0x1E;

                this->WARFlags.dataForced = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Edid overrid on  Haier TV.");
                DP_PRINTF(DP_NOTICE, "DP-WAR>   Removing 1366x768. bug 351680 & 327891");

            }
            break;
        // HP
        case 0xF022:
            switch (ProductID)
            {
                case 0x192F:
                    //
                    // WAR for bug 1643712 - Issue specific to HP Z1 G2 (Zeus) All-In-One
                    // Putting the Rx in power save mode before BL_EN is deasserted, makes this specific sink unhappy
                    // Bug 1559465 will address the right power down sequence. We need to revisit this WAR once Bug 1559465 is fixed.
                    //
                    this->WARFlags.disableDpcdPowerOff = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Disable DPCD Power Off");
                    DP_PRINTF(DP_NOTICE, "DP-WAR> HP Z1 G2 (Zeus) AIO Bug 1643712");
                    break;
            }
            break;

        // Sharp
        case 0x104d:
            switch (ProductID)
            {
                case 0x141c: // HP Valor QHD+ N15P-Q3 Sharp EDP
                    //
                    // HP Valor QHD+ N15P-Q3 EDP needs 50 ms delay
                    // after D3 to avoid black screen issues.
                    //
                    this->WARFlags.delayAfterD3 = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> HP Valor QHD+ N15P-Q3 Sharp EDP needs 50 ms after D3");
                    DP_PRINTF(DP_NOTICE, "DP-WAR> bug 1520011");
                    break;

                //Sharp EDPs that declares DP1.2 but doesn't implement ESI address space
                case 0x1414:
                case 0x1430:
                case 0x1445:
                case 0x1446:
                case 0x144C:
                case 0x1450:
                case 0x1467:
                case 0x145e:
                    //
                    // Use Legacy address space for DP1.2 panel
                    //
                    this->WARFlags.useLegacyAddress = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> Sharp EDP implements only Legacy interrupt address range");
                    break;

                case 0x143B:
                    //
                    // Bug 200113041
                    // Need to be unique to identify this Sharp panel. Besides
                    // manufacturer ID and ProductID, we have to add the mode
                    // name to make this happen as LQ156D1JW05 in ASCII.
                    //
                    if ((buffer.data[0x71] == 0x4C) &&
                        (buffer.data[0x72] == 0x51) &&
                        (buffer.data[0x73] == 0x31) &&
                        (buffer.data[0x74] == 0x35) &&
                        (buffer.data[0x75] == 0x36) &&
                        (buffer.data[0x76] == 0x44) &&
                        (buffer.data[0x77] == 0x31) &&
                        (buffer.data[0x78] == 0x4A) &&
                        (buffer.data[0x79] == 0x57) &&
                        (buffer.data[0x7A] == 0x30) &&
                        (buffer.data[0x7B] == 0x35) &&
                        (buffer.data[0x7C] == 0x0A) &&
                        (buffer.data[0x7D] == 0x20))
                    {
                        this->WARFlags.useLegacyAddress = true;
                        DP_PRINTF(DP_NOTICE, "DP-WAR> Sharp EDP implements only Legacy interrupt address range");
                    }
                    break;
            }
            break;

        // EIZO
        case 0xc315:
            if (ProductID == 0x2227)
            {
                //
                // The EIZO FlexScan SX2762W generates a redundant long HPD
                // pulse after a modeset, which triggers another modeset on GPUs
                // without flush mode, triggering an infinite link training
                // loop.
                //
                this->WARFlags.ignoreRedundantHotplug = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> EIZO FlexScan SX2762W generates redundant");
                DP_PRINTF(DP_NOTICE, "DP-WAR> hotplugs (bug 1048796)");
                break;
            }
            break;

        // MEI-Panasonic
        case 0xa934:
            if (ProductID == 0x96a2)
            {
                //
                // Bug 200113041
                // Need to be unique to identify this MEI-Panasonic panel.
                // Besides manufacturer ID and ProductID, we have to add the
                // model name to make this happen as VVX17P051J00^ in ASCII.
                //
                if ((buffer.data[0x71] == 0x56) &&
                    (buffer.data[0x72] == 0x56) &&
                    (buffer.data[0x73] == 0x58) &&
                    (buffer.data[0x74] == 0x31) &&
                    (buffer.data[0x75] == 0x37) &&
                    (buffer.data[0x76] == 0x50) &&
                    (buffer.data[0x77] == 0x30) &&
                    (buffer.data[0x78] == 0x35) &&
                    (buffer.data[0x79] == 0x31) &&
                    (buffer.data[0x7A] == 0x4A) &&
                    (buffer.data[0x7B] == 0x30) &&
                    (buffer.data[0x7C] == 0x30) &&
                    (buffer.data[0x7D] == 0x0A))
                {
                    this->WARFlags.useLegacyAddress = true;
                    DP_PRINTF(DP_NOTICE, "DP-WAR> MEI-Panasonic EDP");
                    DP_PRINTF(DP_NOTICE, "implements only Legacy interrupt address range");
                }
            }
            break;

        // LG
        case 0xE430:
            if (ProductID == 0x0469)
            {
                //
                // The LG display can't be driven at FHD with 2*RBR.
                // Force max link config
                //
                this->WARFlags.forceMaxLinkConfig = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Force maximum link config WAR required on LG panel.");
                DP_PRINTF(DP_NOTICE, "DP-WAR>   bug 1649626");
                break;
            }
            break;
        case 0x8F34:
            if (ProductID == 0xAA55)
            {
                this->WARFlags.forceMaxLinkConfig = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Force maximum link config WAR required on Sharp-CerebrEx panel.");
            }
            break;

        // Dell
        case 0xAC10:
            // Dell U2713H has problem with LQA. Disable it.
            if ((ProductID == 0xA092) || (ProductID == 0xF046))
            {
                this->WARFlags.reassessMaxLink = true;
            }
            break;

        // CMN
        case 0xAE0D:
            if (ProductID == 0x1747)
            {
                this->WARFlags.useLegacyAddress = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> CMN eDP");
                DP_PRINTF(DP_NOTICE, "implements only Legacy interrupt address range");
            }
            break;

        // BenQ
        case 0xD109:
            if ((ProductID == 0x7F2B) || (ProductID == 0x7F2F))
            {
                this->WARFlags.ignoreRedundantHotplug = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> BenQ GSync power on/off redundant hotplug");
            }
            break;

        // MSI
        case 0x834C:
            if (ProductID == 0x4C48)
            {
                this->WARFlags.useLegacyAddress = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> MSI eDP\n");
                DP_PRINTF(DP_NOTICE, "implements only Legacy interrupt address range\n");
            }
            break;

        // Unigraf
        case 0xC754:
        case 0x1863:
            {
                DP_PRINTF(DP_NOTICE, "DP-WAR> Unigraf device, keep link alive during detection\n");
                this->WARFlags.keepLinkAlive = true;
            }
            break;

        // BOE
        case 0xE509:
            if ((ProductID == 0x977) || (ProductID == 0x974) || (ProductID == 0x9D9))
            {
                this->WARFlags.bIgnoreDscCap = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> BOE panels incorrectly exposing DSC capability. Ignoring it.");
            }
            break;

        // NCP
        case 0x7038:
            if (ProductID == 0x005F)
            {
                this->WARFlags.bIgnoreDscCap = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> NCP panels incorrectly exposing DSC capability. Ignoring it.");
            }
            break;

        //
        // This panel advertise DSC capabilities, but panel doesn't support DSC
        // So ignoring DSC capability on this panel
        //
        case 0x6F0E:
            if (ProductID == 0x1609)
            {
                this->WARFlags.bIgnoreDscCap = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Ignoring DSC capability on Lenovo CSOT 1609 Panel.");
                DP_PRINTF(DP_NOTICE, "DP-WAR> Bug 3444252");
            }
            break;

        // Asus
        case 0x6D1E:
            if(ProductID == 0x7707)
            {
                this->WARFlags.bIgnoreDscCap = true;
                DP_PRINTF(DP_NOTICE, "DP-WAR> Panel incorrectly exposing DSC capability. Ignoring it.");
                DP_PRINTF(DP_NOTICE, "DP-WAR> Bug 3543158");
            }
            break;

        default:
            break;
    }

    // Find out if the monitor needs a WAR to applied.
    if (warFlag)
    {
        if (warFlag & DP_MONITOR_CAPABILITY_DP_SKIP_REDUNDANT_LT)
        {
            this->WARFlags.skipRedundantLt = true;
        }

        if (warFlag & DP_MONITOR_CAPABILITY_DP_SKIP_CABLE_BW_CHECK)
        {
            this->WARFlags.skipCableBWCheck = true;
            this->WARData.maxLaneAtHighRate = pDenylistData->dpSkipCheckLink.maxLaneAtHighRate;
            this->WARData.maxLaneAtLowRate = pDenylistData->dpSkipCheckLink.maxLaneAtLowRate;
        }

        if (warFlag & DP_MONITOR_CAPABILITY_DP_WRITE_0x600_BEFORE_LT)
        {
            // all HP monitors need to be powered up before link training
            this->WARFlags.powerOnBeforeLt = true;
            DP_PRINTF(DP_NOTICE, "DP-WAR> HP monitors need to be powered up before LT");
        }

        if (warFlag & DP_MONITOR_CAPABILITY_DP_OVERRIDE_OPTIMAL_LINK_CONFIG)
        {
            //
            // Instead of calculating the optimum link config
            // based on timing, bpc etc. just used a default
            // fixed link config for the monitor for all modes
            //
            this->WARFlags.overrideOptimalLinkCfg = true;
            // Force the fix max LT
            this->WARFlags.forceMaxLinkConfig = true;
            this->WARData.optimalLinkRate = pDenylistData->dpOverrideOptimalLinkConfig.linkRate;
            this->WARData.optimalLaneCount = pDenylistData->dpOverrideOptimalLinkConfig.laneCount;
            DP_PRINTF(DP_NOTICE, "DP-WAR> Overriding optimal link config on Dell U2410.");
            DP_PRINTF(DP_NOTICE, "DP-WAR>   bug 632801");
        }

        if (warFlag & DP_MONITOR_CAPABILITY_DP_OVERRIDE_MAX_LANE_COUNT)
        {
            //
            // Some monitors claim more lanes than they actually support.
            // This particular Lenovo monitos has just 2 lanes, but its DPCD says 4.
            // This WAR is to override the max lane count read from DPCD.
            //
            this->WARFlags.overrideMaxLaneCount = true;
            this->WARData.maxLaneCount = pDenylistData->dpMaxLaneCountOverride;
            DP_PRINTF(DP_NOTICE, "DP-WAR> Overriding max lane count on Lenovo L2440x.");
            DP_PRINTF(DP_NOTICE, "DP-WAR>   bug 687952");
        }
    }

    if (this->WARFlags.dataForced)
    {
        DP_PRINTF(DP_NOTICE, "DP-WAR> EDID was overridden for some data. Patching CRC.");
        this->patchCrc();
    }
}
