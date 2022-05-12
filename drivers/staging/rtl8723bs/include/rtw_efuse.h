/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTW_EFUSE_H__
#define __RTW_EFUSE_H__


#define	EFUSE_ERROE_HANDLE		1

#define	PG_STATE_HEADER			0x01
#define	PG_STATE_WORD_0		0x02
#define	PG_STATE_WORD_1		0x04
#define	PG_STATE_WORD_2		0x08
#define	PG_STATE_WORD_3		0x10
#define	PG_STATE_DATA			0x20

#define	PG_SWBYTE_H			0x01
#define	PG_SWBYTE_L			0x02

#define	PGPKT_DATA_SIZE		8

#define	EFUSE_WIFI				0
#define	EFUSE_BT				1

enum {
	TYPE_EFUSE_MAX_SECTION				= 0,
	TYPE_EFUSE_REAL_CONTENT_LEN			= 1,
	TYPE_AVAILABLE_EFUSE_BYTES_BANK		= 2,
	TYPE_AVAILABLE_EFUSE_BYTES_TOTAL	= 3,
	TYPE_EFUSE_MAP_LEN					= 4,
	TYPE_EFUSE_PROTECT_BYTES_BANK		= 5,
	TYPE_EFUSE_CONTENT_LEN_BANK			= 6,
};

#define		EFUSE_MAX_MAP_LEN		512

#define		EFUSE_MAX_HW_SIZE		512
#define		EFUSE_MAX_SECTION_BASE	16

#define EXT_HEADER(header) ((header & 0x1F) == 0x0F)
#define ALL_WORDS_DISABLED(wde)	((wde & 0x0F) == 0x0F)
#define GET_HDR_OFFSET_2_0(header) ((header & 0xE0) >> 5)

#define		EFUSE_REPEAT_THRESHOLD_			3

/*  */
/* 	The following is for BT Efuse definition */
/*  */
#define		EFUSE_BT_MAX_MAP_LEN		1024
#define		EFUSE_MAX_BANK			4
#define		EFUSE_MAX_BT_BANK		(EFUSE_MAX_BANK-1)
/*  */
/*--------------------------Define Parameters-------------------------------*/
#define		EFUSE_MAX_WORD_UNIT			4

/*------------------------------Define structure----------------------------*/
struct pgpkt_struct {
	u8 offset;
	u8 word_en;
	u8 data[8];
	u8 word_cnts;
};

/*------------------------------Define structure----------------------------*/
struct efuse_hal {
	u8 fakeEfuseBank;
	u32 fakeEfuseUsedBytes;
	u8 fakeEfuseContent[EFUSE_MAX_HW_SIZE];
	u8 fakeEfuseInitMap[EFUSE_MAX_MAP_LEN];
	u8 fakeEfuseModifiedMap[EFUSE_MAX_MAP_LEN];

	u16 BTEfuseUsedBytes;
	u8 BTEfuseUsedPercentage;
	u8 BTEfuseContent[EFUSE_MAX_BT_BANK][EFUSE_MAX_HW_SIZE];
	u8 BTEfuseInitMap[EFUSE_BT_MAX_MAP_LEN];
	u8 BTEfuseModifiedMap[EFUSE_BT_MAX_MAP_LEN];

	u16 fakeBTEfuseUsedBytes;
	u8 fakeBTEfuseContent[EFUSE_MAX_BT_BANK][EFUSE_MAX_HW_SIZE];
	u8 fakeBTEfuseInitMap[EFUSE_BT_MAX_MAP_LEN];
	u8 fakeBTEfuseModifiedMap[EFUSE_BT_MAX_MAP_LEN];
};


/*------------------------Export global variable----------------------------*/
extern u8 fakeEfuseBank;
extern u32 fakeEfuseUsedBytes;
extern u8 fakeEfuseContent[];
extern u8 fakeEfuseInitMap[];
extern u8 fakeEfuseModifiedMap[];

extern u32 BTEfuseUsedBytes;
extern u8 BTEfuseContent[EFUSE_MAX_BT_BANK][EFUSE_MAX_HW_SIZE];
extern u8 BTEfuseInitMap[];
extern u8 BTEfuseModifiedMap[];

extern u32 fakeBTEfuseUsedBytes;
extern u8 fakeBTEfuseContent[EFUSE_MAX_BT_BANK][EFUSE_MAX_HW_SIZE];
extern u8 fakeBTEfuseInitMap[];
extern u8 fakeBTEfuseModifiedMap[];
/*------------------------Export global variable----------------------------*/

u16 Efuse_GetCurrentSize(struct adapter *padapter, u8 efuseType, bool bPseudoTest);
u8 Efuse_CalculateWordCnts(u8 word_en);
void EFUSE_GetEfuseDefinition(struct adapter *padapter, u8 efuseType, u8 type, void *pOut, bool bPseudoTest);
u8 efuse_OneByteRead(struct adapter *padapter, u16 addr, u8 *data, bool	 bPseudoTest);
u8 efuse_OneByteWrite(struct adapter *padapter, u16 addr, u8 data, bool	 bPseudoTest);

void Efuse_PowerSwitch(struct adapter *padapter, u8 bWrite, u8  PwrState);
int	Efuse_PgPacketRead(struct adapter *padapter, u8 offset, u8 *data, bool bPseudoTest);
int	Efuse_PgPacketWrite(struct adapter *padapter, u8 offset, u8 word_en, u8 *data, bool bPseudoTest);
void efuse_WordEnableDataRead(u8 word_en, u8 *sourdata, u8 *targetdata);
u8 Efuse_WordEnableDataWrite(struct adapter *padapter, u16 efuse_addr, u8 word_en, u8 *data, bool bPseudoTest);

u8 EFUSE_Read1Byte(struct adapter *padapter, u16 Address);
void EFUSE_ShadowMapUpdate(struct adapter *padapter, u8 efuseType, bool bPseudoTest);
void EFUSE_ShadowRead(struct adapter *padapter, u8 Type, u16 Offset, u32 *Value);
void Rtw_Hal_ReadMACAddrFromFile(struct adapter *padapter);
u32 Rtw_Hal_readPGDataFromConfigFile(struct adapter *padapter);

#endif
