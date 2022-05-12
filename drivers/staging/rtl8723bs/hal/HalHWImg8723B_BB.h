/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
*
* Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#ifndef __INC_MP_BB_HW_IMG_8723B_H
#define __INC_MP_BB_HW_IMG_8723B_H


/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8723B_AGC_TAB(/*  TC: Test Chip, MP: MP Chip */
	struct dm_odm_t *pDM_Odm
);

/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8723B_PHY_REG(/*  TC: Test Chip, MP: MP Chip */
	struct dm_odm_t *pDM_Odm
);

/******************************************************************************
*                           PHY_REG_PG.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8723B_PHY_REG_PG(/*  TC: Test Chip, MP: MP Chip */
	struct dm_odm_t *pDM_Odm
);
u32 ODM_GetVersion_MP_8723B_PHY_REG_PG(void);

#endif
