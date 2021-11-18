// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
//============================================================
// Description:
//
// This file is for 92CE/92CU dynamic mechanism only
//
//
//============================================================
#define _RTL8188E_DM_C_

//============================================================
// include files
//============================================================
#include <drv_types.h>
#include <rtl8188e_hal.h>

//============================================================
// Global var
//============================================================


static VOID
dm_CheckProtection(
	IN	PADAPTER	Adapter
	)
{
#if 0
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	u1Byte			CurRate, RateThreshold;

	if(pMgntInfo->pHTInfo->bCurBW40MHz)
		RateThreshold = MGN_MCS1;
	else
		RateThreshold = MGN_MCS3;

	if(Adapter->TxStats.CurrentInitTxRate <= RateThreshold)
	{
		pMgntInfo->bDmDisableProtect = true;
		DbgPrint("Forced disable protect: %x\n", Adapter->TxStats.CurrentInitTxRate);
	}
	else
	{
		pMgntInfo->bDmDisableProtect = false;
		DbgPrint("Enable protect: %x\n", Adapter->TxStats.CurrentInitTxRate);
	}
#endif
}

static VOID
dm_CheckStatistics(
	IN	PADAPTER	Adapter
	)
{
#if 0
	if(!Adapter->MgntInfo.bMediaConnect)
		return;

	//2008.12.10 tynli Add for getting Current_Tx_Rate_Reg flexibly.
	rtw_hal_get_hwreg( Adapter, HW_VAR_INIT_TX_RATE, (pu1Byte)(&Adapter->TxStats.CurrentInitTxRate) );

	// Calculate current Tx Rate(Successful transmited!!)

	// Calculate current Rx Rate(Successful received!!)

	//for tx tx retry count
	rtw_hal_get_hwreg( Adapter, HW_VAR_RETRY_COUNT, (pu1Byte)(&Adapter->TxStats.NumTxRetryCount) );
#endif
}

#ifdef CONFIG_SUPPORT_HW_WPS_PBC
static void dm_CheckPbcGPIO(_adapter *padapter)
{
	u8	tmp1byte;
	u8	bPbcPressed = false;

	if(!padapter->registrypriv.hw_wps_pbc)
		return;

#ifdef CONFIG_USB_HCI
	tmp1byte = rtw_read8(padapter, GPIO_IO_SEL);
	tmp1byte |= (HAL_8188E_HW_GPIO_WPS_BIT);
	rtw_write8(padapter, GPIO_IO_SEL, tmp1byte);	//enable GPIO[2] as output mode

	tmp1byte &= ~(HAL_8188E_HW_GPIO_WPS_BIT);
	rtw_write8(padapter,  GPIO_IN, tmp1byte);		//reset the floating voltage level

	tmp1byte = rtw_read8(padapter, GPIO_IO_SEL);
	tmp1byte &= ~(HAL_8188E_HW_GPIO_WPS_BIT);
	rtw_write8(padapter, GPIO_IO_SEL, tmp1byte);	//enable GPIO[2] as input mode

	tmp1byte =rtw_read8(padapter, GPIO_IN);

	if (tmp1byte == 0xff)
		return ;

	if (tmp1byte&HAL_8188E_HW_GPIO_WPS_BIT)
	{
		bPbcPressed = true;
	}
#else
	tmp1byte = rtw_read8(padapter, GPIO_IN);
	//RT_TRACE(COMP_IO, DBG_TRACE, ("dm_CheckPbcGPIO - %x\n", tmp1byte));

	if (tmp1byte == 0xff || padapter->init_adpt_in_progress)
		return ;

	if((tmp1byte&HAL_8188E_HW_GPIO_WPS_BIT)==0)
	{
		bPbcPressed = true;
	}
#endif

	if( true == bPbcPressed)
	{
		// Here we only set bPbcPressed to true
		// After trigger PBC, the variable will be set to false
		DBG_8192C("CheckPbcGPIO - PBC is pressed\n");
		rtw_request_wps_pbc_event(padapter);
	}
}
#endif//#ifdef CONFIG_SUPPORT_HW_WPS_PBC

#ifdef CONFIG_PCI_HCI
//
//	Description:
//		Perform interrupt migration dynamically to reduce CPU utilization.
//
//	Assumption:
//		1. Do not enable migration under WIFI test.
//
//	Created by Roger, 2010.03.05.
//
VOID
dm_InterruptMigration(
	IN	PADAPTER	Adapter
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct mlme_priv	*pmlmepriv = &(Adapter->mlmepriv);
	BOOLEAN			bCurrentIntMt, bCurrentACIntDisable;
	BOOLEAN			IntMtToSet = false;
	BOOLEAN			ACIntToSet = false;


	// Retrieve current interrupt migration and Tx four ACs IMR settings first.
	bCurrentIntMt = pHalData->bInterruptMigration;
	bCurrentACIntDisable = pHalData->bDisableTxInt;

	//
	// <Roger_Notes> Currently we use busy traffic for reference instead of RxIntOK counts to prevent non-linear Rx statistics
	// when interrupt migration is set before. 2010.03.05.
	//
	if(!Adapter->registrypriv.wifi_spec &&
		(check_fwstate(pmlmepriv, _FW_LINKED)== true) &&
		pmlmepriv->LinkDetectInfo.bHigherBusyTraffic)
	{
		IntMtToSet = true;

		// To check whether we should disable Tx interrupt or not.
		if(pmlmepriv->LinkDetectInfo.bHigherBusyRxTraffic )
			ACIntToSet = true;
	}

	//Update current settings.
	if( bCurrentIntMt != IntMtToSet ){
		DBG_8192C("%s(): Update interrrupt migration(%d)\n",__FUNCTION__,IntMtToSet);
		if(IntMtToSet)
		{
			//
			// <Roger_Notes> Set interrrupt migration timer and corresponging Tx/Rx counter.
			// timer 25ns*0xfa0=100us for 0xf packets.
			// 2010.03.05.
			//
			rtw_write32(Adapter, REG_INT_MIG, 0xff000fa0);// 0x306:Rx, 0x307:Tx
			pHalData->bInterruptMigration = IntMtToSet;
		}
		else
		{
			// Reset all interrupt migration settings.
			rtw_write32(Adapter, REG_INT_MIG, 0);
			pHalData->bInterruptMigration = IntMtToSet;
		}
	}

	/*if( bCurrentACIntDisable != ACIntToSet ){
		DBG_8192C("%s(): Update AC interrrupt(%d)\n",__FUNCTION__,ACIntToSet);
		if(ACIntToSet) // Disable four ACs interrupts.
		{
			//
			// <Roger_Notes> Disable VO, VI, BE and BK four AC interrupts to gain more efficient CPU utilization.
			// When extremely highly Rx OK occurs, we will disable Tx interrupts.
			// 2010.03.05.
			//
			UpdateInterruptMask8192CE( Adapter, 0, RT_AC_INT_MASKS );
			pHalData->bDisableTxInt = ACIntToSet;
		}
		else// Enable four ACs interrupts.
		{
			UpdateInterruptMask8192CE( Adapter, RT_AC_INT_MASKS, 0 );
			pHalData->bDisableTxInt = ACIntToSet;
		}
	}*/

}

#endif

//
// Initialize GPIO setting registers
//
static void
dm_InitGPIOSetting(
	IN	PADAPTER	Adapter
	)
{
	PHAL_DATA_TYPE		pHalData = GET_HAL_DATA(Adapter);

	u8	tmp1byte;

	tmp1byte = rtw_read8(Adapter, REG_GPIO_MUXCFG);
	tmp1byte &= (GPIOSEL_GPIO | ~GPIOSEL_ENBT);

	rtw_write8(Adapter, REG_GPIO_MUXCFG, tmp1byte);

}

//============================================================
// functions
//============================================================
static void Init_ODM_ComInfo_88E(PADAPTER	Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
	u32  SupportAbility = 0;
	u8	cut_ver,fab_ver;

	Init_ODM_ComInfo(Adapter);

	ODM_CmnInfoInit(pDM_Odm,ODM_CMNINFO_IC_TYPE,ODM_RTL8188E);

	fab_ver = ODM_TSMC;
	cut_ver = ODM_CUT_A;

	if(IS_VENDOR_8188E_I_CUT_SERIES(Adapter))
		cut_ver = ODM_CUT_I;

	ODM_CmnInfoInit(pDM_Odm,ODM_CMNINFO_FAB_VER,fab_ver);
	ODM_CmnInfoInit(pDM_Odm,ODM_CMNINFO_CUT_VER,cut_ver);

 	ODM_CmnInfoInit(pDM_Odm, ODM_CMNINFO_RF_ANTENNA_TYPE, pHalData->TRxAntDivType);
	
	#ifdef CONFIG_DISABLE_ODM
	SupportAbility = 0;
	#else
	SupportAbility = 	ODM_RF_CALIBRATION |
					ODM_RF_TX_PWR_TRACK
					;	
	/* if(pHalData->AntDivCfg)
		SupportAbility |= ODM_BB_ANT_DIV; */
	#endif	

	ODM_CmnInfoUpdate(pDM_Odm,ODM_CMNINFO_ABILITY,SupportAbility);
	
}
static void Update_ODM_ComInfo_88E(PADAPTER	Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
	u32  SupportAbility = 0;	
	int i;

	SupportAbility = 0
		| ODM_BB_DIG
		| ODM_BB_RA_MASK
		| ODM_BB_DYNAMIC_TXPWR
		| ODM_BB_FA_CNT
		| ODM_BB_RSSI_MONITOR
		| ODM_BB_CCK_PD
		//| ODM_BB_PWR_SAVE	
		| ODM_BB_CFO_TRACKING
		| ODM_RF_CALIBRATION
		| ODM_RF_TX_PWR_TRACK
		| ODM_BB_NHM_CNT
		| ODM_BB_PRIMARY_CCA
//		| ODM_BB_PWR_TRAIN
		;

	if (rtw_odm_adaptivity_needed(Adapter) == true)
		SupportAbility |= ODM_BB_ADAPTIVITY;

	if (!Adapter->registrypriv.qos_opt_enable) {
		SupportAbility |= ODM_MAC_EDCA_TURBO;
	}
	
	if(pHalData->AntDivCfg)
		SupportAbility |= ODM_BB_ANT_DIV;

#if (MP_DRIVER==1)
	if (Adapter->registrypriv.mp_mode == 1) {
		SupportAbility = 0
			| ODM_RF_CALIBRATION
			| ODM_RF_TX_PWR_TRACK
			;
	}
#endif//(MP_DRIVER==1)

#ifdef CONFIG_DISABLE_ODM
	SupportAbility = 0;
#endif//CONFIG_DISABLE_ODM

	ODM_CmnInfoUpdate(pDM_Odm,ODM_CMNINFO_ABILITY,SupportAbility);

	ODM_CmnInfoInit(pDM_Odm, ODM_CMNINFO_RF_ANTENNA_TYPE, pHalData->TRxAntDivType);
}

void
rtl8188e_InitHalDm(
	IN	PADAPTER	Adapter
	)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);	
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
	u8	i;

#ifdef CONFIG_USB_HCI
	dm_InitGPIOSetting(Adapter);
#endif

	pHalData->DM_Type = DM_Type_ByDriver;

	Update_ODM_ComInfo_88E(Adapter);
	ODM_DMInit(pDM_Odm);
}


VOID
rtl8188e_HalDmWatchDog(
	IN	PADAPTER	Adapter
	)
{
	BOOLEAN		bFwCurrentInPSMode = false;
	BOOLEAN		bFwPSAwake = true;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
#ifdef CONFIG_CONCURRENT_MODE
	PADAPTER pbuddy_adapter = Adapter->pbuddy_adapter;
#endif //CONFIG_CONCURRENT_MODE

	_func_enter_;

	if (!rtw_is_hw_init_completed(Adapter))
		goto skip_dm;

#ifdef CONFIG_LPS
	bFwCurrentInPSMode = adapter_to_pwrctl(Adapter)->bFwCurrentInPSMode;
	rtw_hal_get_hwreg(Adapter, HW_VAR_FWLPS_RF_ON, (u8 *)(&bFwPSAwake));
#endif

#ifdef CONFIG_P2P_PS
	// Fw is under p2p powersaving mode, driver should stop dynamic mechanism.
	// modifed by thomas. 2011.06.11.
	if(Adapter->wdinfo.p2p_ps_mode)
		bFwPSAwake = false;
#endif //CONFIG_P2P_PS

	if ((rtw_is_hw_init_completed(Adapter))
		&& ((!bFwCurrentInPSMode) && bFwPSAwake)) {
		//
		// Calculate Tx/Rx statistics.
		//
		dm_CheckStatistics(Adapter);
		
		rtw_hal_check_rxfifo_full(Adapter);
		//
		// Dynamically switch RTS/CTS protection.
		//
		//dm_CheckProtection(Adapter);

#ifdef CONFIG_PCI_HCI
		// 20100630 Joseph: Disable Interrupt Migration mechanism temporarily because it degrades Rx throughput.
		// Tx Migration settings.
		//dm_InterruptMigration(Adapter);

		//if(Adapter->HalFunc.TxCheckStuckHandler(Adapter))
		//	PlatformScheduleWorkItem(&(GET_HAL_DATA(Adapter)->HalResetWorkItem));
#endif
	
	}


	//ODM
	if (rtw_is_hw_init_completed(Adapter)) {
		u8	bLinked=false;
		u8	bsta_state=false;
		#ifdef CONFIG_DISABLE_ODM
		pHalData->odmpriv.SupportAbility = 0;
		#endif

		if(rtw_linked_check(Adapter)){			
			bLinked = true;
			if (check_fwstate(&Adapter->mlmepriv, WIFI_STATION_STATE))
				bsta_state = true;
		}
		
#ifdef CONFIG_CONCURRENT_MODE
		if(pbuddy_adapter && rtw_linked_check(pbuddy_adapter)){
			bLinked = true;
			if(pbuddy_adapter && check_fwstate(&pbuddy_adapter->mlmepriv, WIFI_STATION_STATE))
				bsta_state = true;
		}
#endif //CONFIG_CONCURRENT_MODE

		ODM_CmnInfoUpdate(&pHalData->odmpriv ,ODM_CMNINFO_LINK, bLinked);
		ODM_CmnInfoUpdate(&pHalData->odmpriv ,ODM_CMNINFO_STATION_STATE, bsta_state);


		ODM_DMWatchdog(&pHalData->odmpriv);
			
	}

skip_dm:
	
#ifdef CONFIG_SUPPORT_HW_WPS_PBC
	// Check GPIO to determine current Pbc status.
	dm_CheckPbcGPIO(Adapter);
#endif	
	return;
}

void rtl8188e_init_dm_priv(IN PADAPTER Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T 		podmpriv = &pHalData->odmpriv;

	//_rtw_spinlock_init(&(pHalData->odm_stainfo_lock));
	Init_ODM_ComInfo_88E(Adapter);
	ODM_InitAllTimers(podmpriv );	
	PHYDM_InitDebugSetting(podmpriv);	
}

void rtl8188e_deinit_dm_priv(IN PADAPTER Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T 		podmpriv = &pHalData->odmpriv;
	//_rtw_spinlock_free(&pHalData->odm_stainfo_lock);
	ODM_CancelAllTimers(podmpriv);	
}
