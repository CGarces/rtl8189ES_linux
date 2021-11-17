// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
 
 #ifndef __HAL_PHY_RF_H__
 #define __HAL_PHY_RF_H__
 
typedef enum _SPUR_CAL_METHOD {
	PLL_RESET,
	AFE_PHASE_SEL
} SPUR_CAL_METHOD;

typedef enum _PWRTRACK_CONTROL_METHOD {
	BBSWING,
	TXAGC,
	MIX_MODE,
	TSSI_MODE
} PWRTRACK_METHOD;

typedef VOID 	(*FuncSetPwr)(PVOID, PWRTRACK_METHOD, u1Byte, u1Byte);
typedef VOID(*FuncIQK)(PVOID, u1Byte, u1Byte, u1Byte);
typedef VOID 	(*FuncLCK)(PVOID);
typedef VOID  	(*FuncSwing)(PVOID, pu1Byte*, pu1Byte*, pu1Byte*, pu1Byte*);
typedef VOID	(*FuncSwing8814only)(PVOID, pu1Byte*, pu1Byte*, pu1Byte*, pu1Byte*);

typedef struct _TXPWRTRACK_CFG {
	u1Byte 		SwingTableSize_CCK;	
	u1Byte 		SwingTableSize_OFDM;
	u1Byte 		Threshold_IQK;	
	u1Byte		Threshold_DPK;
	u1Byte 		AverageThermalNum;
	u1Byte 		RfPathCount;
	u4Byte 		ThermalRegAddr;	
	FuncSetPwr 	ODM_TxPwrTrackSetPwr;
	FuncIQK 	DoIQK;
	FuncLCK		PHY_LCCalibrate;
	FuncSwing	GetDeltaSwingTable;
	FuncSwing8814only	GetDeltaSwingTable8814only;
} TXPWRTRACK_CFG, *PTXPWRTRACK_CFG;

void ConfigureTxpowerTrack(
	IN		PVOID					pDM_VOID,
	OUT	PTXPWRTRACK_CFG	pConfig
	);


VOID
ODM_ClearTxPowerTrackingState(
	IN		PVOID					pDM_VOID
	);

VOID
ODM_TXPowerTrackingCallback_ThermalMeter(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	IN		PVOID					pDM_VOID
#else
	IN PADAPTER	Adapter
#endif
	);



#define ODM_TARGET_CHNL_NUM_2G_5G	59


VOID
ODM_ResetIQKResult(
	IN		PVOID					pDM_VOID
);
u1Byte 
ODM_GetRightChnlPlaceforIQK(
    IN u1Byte chnl
);

void phydm_rf_init(	IN		PVOID					pDM_VOID);
void phydm_rf_watchdog(	IN		PVOID					pDM_VOID);
								
#endif	// #ifndef __HAL_PHY_RF_H__

