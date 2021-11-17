// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
 
#ifndef	__PHYDMDYNAMICBBPOWERSAVING_H__
#define    __PHYDMDYNAMICBBPOWERSAVING_H__

#define DYNAMIC_BBPWRSAV_VERSION	"1.0"

typedef struct _Dynamic_Power_Saving_
{
	u1Byte		PreCCAState;
	u1Byte		CurCCAState;

	u1Byte		PreRFState;
	u1Byte		CurRFState;

	int		    Rssi_val_min;
	
	u1Byte		initialize;
	u4Byte		Reg874,RegC70,Reg85C,RegA74;
	
}PS_T,*pPS_T;

#define dm_RF_Saving	ODM_RF_Saving

void ODM_RF_Saving(
	IN		PVOID					pDM_VOID,
	IN	u1Byte		bForceInNormal
);

VOID 
odm_DynamicBBPowerSavingInit(
	IN		PVOID					pDM_VOID
	);

VOID 
odm_DynamicBBPowerSaving(
	IN		PVOID					pDM_VOID
	);

VOID
odm_1R_CCA(
	IN		PVOID					pDM_VOID
	);

#endif
