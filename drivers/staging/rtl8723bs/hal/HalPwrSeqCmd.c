// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	HalPwrSeqCmd.c

Abstract:
	Implement HW Power sequence configuration CMD handling routine for Realtek devices.

Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2011-10-26 Lucas            Modify to be compatible with SD4-CE driver.
	2011-07-07 Roger            Create.

--*/
#include <drv_types.h>
#include <rtw_debug.h>
#include <HalPwrSeqCmd.h>


/*  */
/*  Description: */
/*  This routine deal with the Power Configuration CMDs parsing for RTL8723/RTL8188E Series IC. */
/*  */
/*  Assumption: */
/*  We should follow specific format which was released from HW SD. */
/*  */
/*  2011.07.07, added by Roger. */
/*  */
u8 HalPwrSeqCmdParsing(
	struct adapter *padapter,
	u8 CutVersion,
	u8 FabVersion,
	u8 InterfaceType,
	struct wlan_pwr_cfg PwrSeqCmd[]
)
{
	struct wlan_pwr_cfg PwrCfgCmd;
	u8 bPollingBit = false;
	u32 AryIdx = 0;
	u8 value = 0;
	u32 offset = 0;
	u32 pollingCount = 0; /*  polling autoload done. */
	u32 maxPollingCnt = 5000;

	do {
		PwrCfgCmd = PwrSeqCmd[AryIdx];

		/* 2 Only Handle the command whose FAB, CUT, and Interface are matched */
		if (
			(GET_PWR_CFG_FAB_MASK(PwrCfgCmd) & FabVersion) &&
			(GET_PWR_CFG_CUT_MASK(PwrCfgCmd) & CutVersion) &&
			(GET_PWR_CFG_INTF_MASK(PwrCfgCmd) & InterfaceType)
		) {
			switch (GET_PWR_CFG_CMD(PwrCfgCmd)) {
			case PWR_CMD_READ:
				break;

			case PWR_CMD_WRITE:
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);

				/*  */
				/*  <Roger_Notes> We should deal with interface specific address mapping for some interfaces, e.g., SDIO interface */
				/*  2011.07.07. */
				/*  */
				if (GET_PWR_CFG_BASE(PwrCfgCmd) == PWR_BASEADDR_SDIO) {
					/*  Read Back SDIO Local value */
					value = SdioLocalCmd52Read1Byte(padapter, offset);

					value &= ~(GET_PWR_CFG_MASK(PwrCfgCmd));
					value |= (
						GET_PWR_CFG_VALUE(PwrCfgCmd) &
						GET_PWR_CFG_MASK(PwrCfgCmd)
					);

					/*  Write Back SDIO Local value */
					SdioLocalCmd52Write1Byte(padapter, offset, value);
				} else {
					/*  Read the value from system register */
					value = rtw_read8(padapter, offset);

					value &= (~(GET_PWR_CFG_MASK(PwrCfgCmd)));
					value |= (
						GET_PWR_CFG_VALUE(PwrCfgCmd)
						&GET_PWR_CFG_MASK(PwrCfgCmd)
					);

					/*  Write the value back to system register */
					rtw_write8(padapter, offset, value);
				}
				break;

			case PWR_CMD_POLLING:

				bPollingBit = false;
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);
				do {
					if (GET_PWR_CFG_BASE(PwrCfgCmd) == PWR_BASEADDR_SDIO)
						value = SdioLocalCmd52Read1Byte(padapter, offset);
					else
						value = rtw_read8(padapter, offset);

					value = value&GET_PWR_CFG_MASK(PwrCfgCmd);
					if (
						value == (GET_PWR_CFG_VALUE(PwrCfgCmd) &
						GET_PWR_CFG_MASK(PwrCfgCmd))
					)
						bPollingBit = true;
					else
						udelay(10);

					if (pollingCount++ > maxPollingCnt)
						return false;

				} while (!bPollingBit);

				break;

			case PWR_CMD_DELAY:
				if (GET_PWR_CFG_VALUE(PwrCfgCmd) == PWRSEQ_DELAY_US)
					udelay(GET_PWR_CFG_OFFSET(PwrCfgCmd));
				else
					udelay(GET_PWR_CFG_OFFSET(PwrCfgCmd)*1000);
				break;

			case PWR_CMD_END:
				/*  When this command is parsed, end the process */
				return true;

			default:
				break;
			}
		}

		AryIdx++;/* Add Array Index */
	} while (1);

	return true;
}
