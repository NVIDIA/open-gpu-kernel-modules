/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/


#ifndef	__ODM_INTERFACE_H__
#define __ODM_INTERFACE_H__



/*  =========== Macro Define */

#define _reg_all(_name)			ODM_##_name
#define _reg_ic(_name, _ic)		ODM_##_name##_ic
#define _bit_all(_name)			BIT_##_name
#define _bit_ic(_name, _ic)		BIT_##_name##_ic

/*===================================

#define ODM_REG_DIG_11N		0xC50
#define ODM_REG_DIG_11AC	0xDDD

ODM_REG(DIG, _pDM_Odm)
=====================================*/

#define _reg_11N(_name)			ODM_REG_##_name##_11N
#define _bit_11N(_name)			ODM_BIT_##_name##_11N

#define _cat(_name, _ic_type, _func) _func##_11N(_name)

/*  _name: name of register or bit. */
/*  Example: "ODM_REG(R_A_AGC_CORE1, pDM_Odm)" */
/*         gets "ODM_R_A_AGC_CORE1" or "ODM_R_A_AGC_CORE1_8192C", depends on SupportICType. */
#define ODM_REG(_name, _pDM_Odm)	_cat(_name, _pDM_Odm->SupportICType, _reg)
#define ODM_BIT(_name, _pDM_Odm)	_cat(_name, _pDM_Odm->SupportICType, _bit)

#endif	/*  __ODM_INTERFACE_H__ */
