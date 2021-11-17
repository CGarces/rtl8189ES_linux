// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
 
#ifndef	__PHYDMDYNAMICTXPOWER_H__
#define    __PHYDMDYNAMICTXPOWER_H__

/*#define DYNAMIC_TXPWR_VERSION	"1.0"*/
#define DYNAMIC_TXPWR_VERSION	"1.1" /*2015.01.13*/

#define		TX_POWER_NEAR_FIELD_THRESH_LVL2	74
#define		TX_POWER_NEAR_FIELD_THRESH_LVL1	67
#define		TX_POWER_NEAR_FIELD_THRESH_AP		0x3F
#define		TX_POWER_NEAR_FIELD_THRESH_8812	60

#define		TxHighPwrLevel_Normal		0	
#define		TxHighPwrLevel_Level1		1
#define		TxHighPwrLevel_Level2		2
#define		TxHighPwrLevel_BT1			3
#define		TxHighPwrLevel_BT2			4
#define		TxHighPwrLevel_15			5
#define		TxHighPwrLevel_35			6
#define		TxHighPwrLevel_50			7
#define		TxHighPwrLevel_70			8
#define		TxHighPwrLevel_100			9

VOID 
odm_DynamicTxPowerInit(
	IN		PVOID					pDM_VOID
	);

VOID
odm_DynamicTxPowerRestorePowerIndex(
	IN		PVOID					pDM_VOID
	);

VOID 
odm_DynamicTxPowerNIC(
	IN		PVOID					pDM_VOID
	);

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
VOID
odm_DynamicTxPowerSavePowerIndex(
	IN		PVOID					pDM_VOID
	);

VOID
odm_DynamicTxPowerWritePowerIndex(
	IN		PVOID					pDM_VOID, 
	IN 	u1Byte		Value);

VOID 
odm_DynamicTxPower_92C(
	IN		PVOID					pDM_VOID
	);

VOID 
odm_DynamicTxPower_92D(
	IN		PVOID					pDM_VOID
	);
#endif

VOID 
odm_DynamicTxPower(
	IN		PVOID					pDM_VOID
	);

VOID 
odm_DynamicTxPowerAP(
	IN		PVOID					pDM_VOID
	);

#endif
