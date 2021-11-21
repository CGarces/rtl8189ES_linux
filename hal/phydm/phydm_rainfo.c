// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */

//============================================================
// include files
//============================================================
#include "mp_precomp.h"
#include "phydm_precomp.h"

VOID
odm_RSSIMonitorInit(
	IN		PVOID		pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	pRA_T		pRA_Table = &pDM_Odm->DM_RA_Table;
	pRA_Table->firstconnect = false;
}

VOID
odm_RSSIMonitorCheck(
	IN		PVOID		pDM_VOID
)
{
	//
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	if (!(pDM_Odm->SupportAbility & ODM_BB_RSSI_MONITOR))
		return;

	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	odm_RSSIMonitorCheckCE(pDM_Odm);
}	// odm_RSSIMonitorCheck

VOID
odm_RSSIMonitorCheckCE(
	IN		PVOID		pDM_VOID
)
{

	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);	
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);
	int	i;
	int	tmpEntryMaxPWDB = 0, tmpEntryMinPWDB = 0xff;
	u8 	sta_cnt = 0;
	u32	UL_DL_STATE = 0, STBC_TX = 0, TxBF_EN = 0 ,NoiseState = 0;
	u32	PWDB_rssi[NUM_STA] = {0}; //[0~15]:MACID, [16~31]:PWDB_rssi
	BOOLEAN			FirstConnect = false;
	pRA_T			pRA_Table = &pDM_Odm->DM_RA_Table;

	if (pDM_Odm->bLinked != true)
		return;

	FirstConnect = (pDM_Odm->bLinked) && (pRA_Table->firstconnect == false);
	pRA_Table->firstconnect = pDM_Odm->bLinked;

	if ( pDM_Odm->NoisyDecision ) {
		NoiseState = 1;             /* BIT2*/
		//ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD,( "[RSSIMonitorCheckMP] Send H2C to FW\n"));
	} else
		NoiseState = 0;
	

	//if(check_fwstate(&Adapter->mlmepriv, WIFI_AP_STATE|WIFI_ADHOC_STATE|WIFI_ADHOC_MASTER_STATE) == true)
	{
		struct sta_info *psta;

		for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
			if (IS_STA_VALID(psta = pDM_Odm->pODM_StaInfo[i])) {
				if (IS_MCAST(psta->hwaddr))  //if(psta->mac_id ==1)
					continue;

				if (psta->rssi_stat.UndecoratedSmoothedPWDB == (-1))
					continue;

				if (psta->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
					tmpEntryMinPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

				if (psta->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
					tmpEntryMaxPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

#if 0
				DBG_871X("%s mac_id:%u, mac:"MAC_FMT", rssi:%d\n", __func__,
						 psta->mac_id, MAC_ARG(psta->hwaddr), psta->rssi_stat.UndecoratedSmoothedPWDB);
#endif

				if (psta->rssi_stat.UndecoratedSmoothedPWDB != (-1)) {

#ifdef CONFIG_80211N_HT
					if (pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8812) {
#ifdef CONFIG_BEAMFORMING
						BEAMFORMING_CAP Beamform_cap = beamforming_get_entry_beam_cap_by_mac_id(&Adapter->mlmepriv, psta->mac_id);

						if (Beamform_cap & (BEAMFORMER_CAP_HT_EXPLICIT | BEAMFORMER_CAP_VHT_SU))
							TxBF_EN = 1;
						else
							TxBF_EN = 0;

						if (TxBF_EN)
							STBC_TX = 0;
						else
#endif
						{
#ifdef CONFIG_80211AC_VHT
							if (IsSupportedVHT(psta->wireless_mode))
								STBC_TX = TEST_FLAG(psta->vhtpriv.stbc_cap, STBC_VHT_ENABLE_TX);
							else
#endif
								STBC_TX = TEST_FLAG(psta->htpriv.stbc_cap, STBC_HT_ENABLE_TX);
						}
					}
#endif

					if ((pDM_Odm->SupportICType == ODM_RTL8192E) || (pDM_Odm->SupportICType == ODM_RTL8812) || (pDM_Odm->SupportICType == ODM_RTL8821)) {
						PWDB_rssi[sta_cnt++] = (((u8)(psta->mac_id & 0xFF)) | ((psta->rssi_stat.UndecoratedSmoothedPWDB & 0x7F) << 16) | (STBC_TX << 25) | (NoiseState<<26) | (FirstConnect << 29) | (TxBF_EN << 30));
					}
					else
						PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB << 16) | (NoiseState<<26));
					
				}
			}
		}

		//printk("%s==> sta_cnt(%d)\n",__FUNCTION__,sta_cnt);

		for (i = 0; i < sta_cnt; i++) {
			if (PWDB_rssi[i] != (0)) {
				if (pHalData->fw_ractrl == true) { // Report every sta's RSSI to FW
#if((RTL8812A_SUPPORT==1)||(RTL8821A_SUPPORT==1))
					if ((pDM_Odm->SupportICType == ODM_RTL8812) || (pDM_Odm->SupportICType == ODM_RTL8821)) {
						PWDB_rssi[i] |= (UL_DL_STATE << 24);
						rtl8812_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
					}
#endif
#if(RTL8723B_SUPPORT==1)
					if (pDM_Odm->SupportICType == ODM_RTL8723B)
						rtl8723b_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
#endif

#if(RTL8188E_SUPPORT==1)
					if (pDM_Odm->SupportICType == ODM_RTL8188E)
						rtl8188e_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
#endif

#if (RTL8814A_SUPPORT == 1)
					if (pDM_Odm->SupportICType == ODM_RTL8814A)
						rtl8814_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
#endif
#if (RTL8188F_SUPPORT == 1)
					if (pDM_Odm->SupportICType == ODM_RTL8188F) {
						rtl8188f_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
					}
#endif
				} else {
#if((RTL8188E_SUPPORT==1)&&(RATE_ADAPTIVE_SUPPORT == 1))
					if (pDM_Odm->SupportICType == ODM_RTL8188E) {
						ODM_RA_SetRSSI_8188E(
							&(pHalData->odmpriv), (PWDB_rssi[i] & 0xFF), (u8)((PWDB_rssi[i] >> 16) & 0xFF));
					}
#endif
				}
			}
		}
	}



	if (tmpEntryMaxPWDB != 0)	// If associated entry is found
		pHalData->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
	else
		pHalData->EntryMaxUndecoratedSmoothedPWDB = 0;

	if (tmpEntryMinPWDB != 0xff) // If associated entry is found
		pHalData->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
	else
		pHalData->EntryMinUndecoratedSmoothedPWDB = 0;

	FindMinimumRSSI(Adapter);//get pdmpriv->MinUndecoratedPWDBForDM

	pDM_Odm->RSSI_Min = pHalData->MinUndecoratedPWDBForDM;
	//ODM_CmnInfoUpdate(&pHalData->odmpriv ,ODM_CMNINFO_RSSI_MIN, pdmpriv->MinUndecoratedPWDBForDM);

}

VOID
odm_RateAdaptiveMaskInit(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PODM_RATE_ADAPTIVE	pOdmRA = &pDM_Odm->RateAdaptive;

	pOdmRA->Type = DM_Type_ByDriver;
	if (pOdmRA->Type == DM_Type_ByDriver)
		pDM_Odm->bUseRAMask = true;
	else
		pDM_Odm->bUseRAMask = false;

	pOdmRA->RATRState = DM_RATR_STA_INIT;

	pOdmRA->LdpcThres = 35;
	pOdmRA->bUseLdpc = false;

	pOdmRA->HighRSSIThresh = HIGH_RSSI_THRESH;
	pOdmRA->LowRSSIThresh = LOW_RSSI_THRESH;
}
/*-----------------------------------------------------------------------------
 * Function:	odm_RefreshRateAdaptiveMask()
 *
 * Overview:	Update rate table mask according to rssi
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/27/2009	hpfan	Create Version 0.
 *
 *---------------------------------------------------------------------------*/
VOID
odm_RefreshRateAdaptiveMask(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("odm_RefreshRateAdaptiveMask()---------->\n"));
	if (!(pDM_Odm->SupportAbility & ODM_BB_RA_MASK)) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("odm_RefreshRateAdaptiveMask(): Return cos not supported\n"));
		return;
	}
	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	odm_RefreshRateAdaptiveMaskCE(pDM_Odm);

}

VOID
odm_RefreshRateAdaptiveMaskCE(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u1Byte	i;
	PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
	PODM_RATE_ADAPTIVE		pRA = &pDM_Odm->RateAdaptive;

	if (RTW_CANNOT_RUN(pAdapter)) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n"));
		return;
	}

	if (!pDM_Odm->bUseRAMask) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n"));
		return;
	}

	//printk("==> %s \n",__FUNCTION__);

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		PSTA_INFO_T pstat = pDM_Odm->pODM_StaInfo[i];
		if (IS_STA_VALID(pstat)) {
			if (IS_MCAST(pstat->hwaddr))  //if(psta->mac_id ==1)
				continue;
			if (IS_MCAST(pstat->hwaddr))
				continue;

			if (ODM_RAStateCheck(pDM_Odm, pstat->rssi_stat.UndecoratedSmoothedPWDB, false , &pstat->rssi_level)) {
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level));
				//printk("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level);
				rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
			} else if (pDM_Odm->bChangeState) {
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Change Power Training State, bDisablePowerTraining = %d\n", pDM_Odm->bDisablePowerTraining));
				rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
			}

		}
	}
}

// Return Value: BOOLEAN
// - true: RATRState is changed.
BOOLEAN
ODM_RAStateCheck(
	IN		PVOID			pDM_VOID,
	IN		s4Byte			RSSI,
	IN		BOOLEAN			bForceUpdate,
	OUT		pu1Byte			pRATRState
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PODM_RATE_ADAPTIVE pRA = &pDM_Odm->RateAdaptive;
	const u1Byte GoUpGap = 5;
	u1Byte HighRSSIThreshForRA = pRA->HighRSSIThresh;
	u1Byte LowRSSIThreshForRA = pRA->LowRSSIThresh;
	u1Byte RATRState;
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI= (( %d )), Current_RSSI_level = (( %d ))\n", RSSI, *pRATRState));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("[Ori RA RSSI Thresh]  High= (( %d )), Low = (( %d ))\n", HighRSSIThreshForRA, LowRSSIThreshForRA));
	// Threshold Adjustment:
	// when RSSI state trends to go up one or two levels, make sure RSSI is high enough.
	// Here GoUpGap is added to solve the boundary's level alternation issue.

	switch (*pRATRState) {
	case DM_RATR_STA_INIT:
	case DM_RATR_STA_HIGH:
		break;

	case DM_RATR_STA_MIDDLE:
		HighRSSIThreshForRA += GoUpGap;
		break;

	case DM_RATR_STA_LOW:
		HighRSSIThreshForRA += GoUpGap;
		LowRSSIThreshForRA += GoUpGap;
		break;

#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	case DM_RATR_STA_ULTRA_LOW:
		HighRSSIThreshForRA += GoUpGap;
		LowRSSIThreshForRA += GoUpGap;
		UltraLowRSSIThreshForRA += GoUpGap;
		break;
#endif

	default:
		ODM_RT_ASSERT(pDM_Odm, false, ("wrong rssi level setting %d !", *pRATRState));
		break;
	}

	// Decide RATRState by RSSI.
	if (RSSI > HighRSSIThreshForRA)
		RATRState = DM_RATR_STA_HIGH;
	else if (RSSI > LowRSSIThreshForRA)
		RATRState = DM_RATR_STA_MIDDLE;

#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	else if (RSSI > UltraLowRSSIThreshForRA)
		RATRState = DM_RATR_STA_LOW;
	else
		RATRState = DM_RATR_STA_ULTRA_LOW;
#else
	else
		RATRState = DM_RATR_STA_LOW;
#endif
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("[Mod RA RSSI Thresh]  High= (( %d )), Low = (( %d ))\n", HighRSSIThreshForRA, LowRSSIThreshForRA));
	/*printk("==>%s,RATRState:0x%02x ,RSSI:%d\n",__FUNCTION__,RATRState,RSSI);*/

	if (*pRATRState != RATRState || bForceUpdate) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("[RSSI Level Update] %d -> %d\n", *pRATRState, RATRState));
		*pRATRState = RATRState;
		return true;
	}

	return false;
}

VOID
phydm_ra_info_init(
	IN	PVOID	pDM_VOID
	)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;

	#if (defined(CONFIG_RA_DYNAMIC_RTY_LIMIT))
	phydm_ra_dynamic_retry_limit_init(pDM_Odm);
	#endif
	#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	phydm_ra_dynamic_rate_id_init(pDM_Odm);
	#endif

	/*phydm_fw_trace_en_h2c(pDM_Odm, 1, 0, 0);*/
}


#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
u1Byte
odm_Find_RTS_Rate(
	IN		PVOID			pDM_VOID,
	IN		u1Byte			Tx_Rate,
	IN		BOOLEAN			bErpProtect
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u1Byte	RTS_Ini_Rate = ODM_RATE6M;
	
	if (bErpProtect) /* use CCK rate as RTS*/
		RTS_Ini_Rate = ODM_RATE1M;
	else {
		switch (Tx_Rate) {
		case ODM_RATEVHTSS3MCS9:
		case ODM_RATEVHTSS3MCS8:
		case ODM_RATEVHTSS3MCS7:
		case ODM_RATEVHTSS3MCS6:
		case ODM_RATEVHTSS3MCS5:
		case ODM_RATEVHTSS3MCS4:
		case ODM_RATEVHTSS3MCS3:
		case ODM_RATEVHTSS2MCS9:
		case ODM_RATEVHTSS2MCS8:
		case ODM_RATEVHTSS2MCS7:
		case ODM_RATEVHTSS2MCS6:
		case ODM_RATEVHTSS2MCS5:
		case ODM_RATEVHTSS2MCS4:
		case ODM_RATEVHTSS2MCS3:
		case ODM_RATEVHTSS1MCS9:
		case ODM_RATEVHTSS1MCS8:
		case ODM_RATEVHTSS1MCS7:
		case ODM_RATEVHTSS1MCS6:
		case ODM_RATEVHTSS1MCS5:
		case ODM_RATEVHTSS1MCS4:
		case ODM_RATEVHTSS1MCS3:
		case ODM_RATEMCS15:
		case ODM_RATEMCS14:
		case ODM_RATEMCS13:
		case ODM_RATEMCS12:
		case ODM_RATEMCS11:
		case ODM_RATEMCS7:
		case ODM_RATEMCS6:
		case ODM_RATEMCS5:
		case ODM_RATEMCS4:
		case ODM_RATEMCS3:
		case ODM_RATE54M:
		case ODM_RATE48M:
		case ODM_RATE36M:
		case ODM_RATE24M:		
			RTS_Ini_Rate = ODM_RATE24M;
			break;
		case ODM_RATEVHTSS3MCS2:
		case ODM_RATEVHTSS3MCS1:
		case ODM_RATEVHTSS2MCS2:
		case ODM_RATEVHTSS2MCS1:
		case ODM_RATEVHTSS1MCS2:
		case ODM_RATEVHTSS1MCS1:
		case ODM_RATEMCS10:
		case ODM_RATEMCS9:
		case ODM_RATEMCS2:
		case ODM_RATEMCS1:
		case ODM_RATE18M:
		case ODM_RATE12M:
			RTS_Ini_Rate = ODM_RATE12M;
			break;
		case ODM_RATEVHTSS3MCS0:
		case ODM_RATEVHTSS2MCS0:
		case ODM_RATEVHTSS1MCS0:
		case ODM_RATEMCS8:
		case ODM_RATEMCS0:
		case ODM_RATE9M:
		case ODM_RATE6M:
			RTS_Ini_Rate = ODM_RATE6M;
			break;
		case ODM_RATE11M:
		case ODM_RATE5_5M:
		case ODM_RATE2M:
		case ODM_RATE1M:
			RTS_Ini_Rate = ODM_RATE1M;
			break;
		default:
			RTS_Ini_Rate = ODM_RATE6M;
			break;
		}
	}

	if (*pDM_Odm->pBandType == 1) {
		if (RTS_Ini_Rate < ODM_RATE6M)
			RTS_Ini_Rate = ODM_RATE6M;
	}
	return RTS_Ini_Rate;

}

VOID
odm_Set_RA_DM_ARFB_by_Noisy(
	IN	PDM_ODM_T	pDM_Odm
)
{
	/*DbgPrint("DM_ARFB ====>\n");*/
	if (pDM_Odm->bNoisyState) {
		ODM_Write4Byte(pDM_Odm, 0x430, 0x00000000);
		ODM_Write4Byte(pDM_Odm, 0x434, 0x05040200);
		/*DbgPrint("DM_ARFB ====> Noisy State\n");*/
	} else {
		ODM_Write4Byte(pDM_Odm, 0x430, 0x02010000);
		ODM_Write4Byte(pDM_Odm, 0x434, 0x07050403);
		/*DbgPrint("DM_ARFB ====> Clean State\n");*/
	}

}

VOID
ODM_UpdateNoisyState(
	IN	PVOID		pDM_VOID,
	IN	BOOLEAN		bNoisyStateFromC2H
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	/*DbgPrint("Get C2H Command! NoisyState=0x%x\n ", bNoisyStateFromC2H);*/
	if (pDM_Odm->SupportICType == ODM_RTL8821  || pDM_Odm->SupportICType == ODM_RTL8812  ||
		pDM_Odm->SupportICType == ODM_RTL8723B || pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8188E)
		pDM_Odm->bNoisyState = bNoisyStateFromC2H;
	odm_Set_RA_DM_ARFB_by_Noisy(pDM_Odm);
};

u4Byte
Set_RA_DM_Ratrbitmap_by_Noisy(
	IN	PVOID			pDM_VOID,
	IN	WIRELESS_MODE	WirelessMode,
	IN	u4Byte			ratr_bitmap,
	IN	u1Byte			rssi_level
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u4Byte ret_bitmap = ratr_bitmap;
	
	switch (WirelessMode) {
	case WIRELESS_MODE_AC_24G:
	case WIRELESS_MODE_AC_5G:
	case WIRELESS_MODE_AC_ONLY:
		if (pDM_Odm->bNoisyState) { /*in Noisy State*/
			if (rssi_level == 1)
				ret_bitmap &= 0xfc3e0c08;		// Reserve MCS 5-9
			else if (rssi_level == 2)
				ret_bitmap &= 0xfe3f8e08;		// Reserve MCS 3-9
			else if (rssi_level == 3)
				ret_bitmap &= 0xffffffff;
			else
				ret_bitmap &= 0xffffffff;
		} else {                                /* in SNR State*/
			if (rssi_level == 1)
				ret_bitmap &= 0xfe3f0e08;		// Reserve MCS 4-9
			else if (rssi_level == 2)
				ret_bitmap &= 0xff3fcf8c;		// Reserve MCS 2-9
			else if (rssi_level == 3)
				ret_bitmap &= 0xffffffff;
			else
				ret_bitmap &= 0xffffffff;
		}
		break;
	case WIRELESS_MODE_B:
	case WIRELESS_MODE_A:
	case WIRELESS_MODE_G:
	case WIRELESS_MODE_N_24G:
	case WIRELESS_MODE_N_5G:
		if (pDM_Odm->bNoisyState) {
			if (rssi_level == 1)
				ret_bitmap &= 0x0f0e0c08;		// Reserve MCS 4-7; MCS12-15
			else if (rssi_level == 2)
				ret_bitmap &= 0x0fcfce0c;		// Reserve MCS 2-7; MCS10-15
			else if (rssi_level == 3)
				ret_bitmap &= 0xffffffff;
			else
				ret_bitmap &= 0xffffffff;
		} else {
			if (rssi_level == 1)
				ret_bitmap &= 0x0f8f8e08;		// Reserve MCS 3-7; MCS11-15
			else if (rssi_level == 2)
				ret_bitmap &= 0x0fefef8c;		// Reserve MCS 1-7; MCS9-15
			else if (rssi_level == 3)
				ret_bitmap &= 0xffffffff;
			else
				ret_bitmap &= 0xffffffff;
		}
		break;
	default:
		break;
	}
	/*DbgPrint("DM_RAMask ====> rssi_LV = %d, BITMAP = %x\n", rssi_level, ret_bitmap);*/
	return ret_bitmap;

}

VOID
ODM_UpdateInitRate(
	IN	PVOID		pDM_VOID,
	IN	u1Byte		Rate
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u1Byte			p = 0;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("Get C2H Command! Rate=0x%x\n", Rate));

	if (pDM_Odm->SupportICType == ODM_RTL8821  || pDM_Odm->SupportICType == ODM_RTL8812  ||
		pDM_Odm->SupportICType == ODM_RTL8723B || pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8188E) {
		pDM_Odm->TxRate = Rate;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if DEV_BUS_TYPE == RT_PCI_INTERFACE
#if USE_WORKITEM
		PlatformScheduleWorkItem(&pDM_Odm->RaRptWorkitem);
#else
		if (pDM_Odm->SupportICType == ODM_RTL8821) {
#if (RTL8821A_SUPPORT == 1)
			ODM_TxPwrTrackSetPwr8821A(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
#endif
		} else if (pDM_Odm->SupportICType == ODM_RTL8812) {
			for (p = ODM_RF_PATH_A; p < MAX_PATH_NUM_8812A; p++) {
#if (RTL8812A_SUPPORT == 1)
				ODM_TxPwrTrackSetPwr8812A(pDM_Odm, MIX_MODE, p, 0);
#endif
			}
		} else if (pDM_Odm->SupportICType == ODM_RTL8723B) {
#if (RTL8723B_SUPPORT == 1)
			ODM_TxPwrTrackSetPwr_8723B(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
#endif
		} else if (pDM_Odm->SupportICType == ODM_RTL8188E) {
#if (RTL8188E_SUPPORT == 1)
			ODM_TxPwrTrackSetPwr88E(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
#endif
		}
#endif
#else
		PlatformScheduleWorkItem(&pDM_Odm->RaRptWorkitem);
#endif
#endif
	} else
		return;
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

VOID
odm_RSSIDumpToRegister(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		Adapter = pDM_Odm->Adapter;

	if (pDM_Odm->SupportICType == ODM_RTL8812) {
		PlatformEFIOWrite1Byte(Adapter, rA_RSSIDump_Jaguar, Adapter->RxStats.RxRSSIPercentage[0]);
		PlatformEFIOWrite1Byte(Adapter, rB_RSSIDump_Jaguar, Adapter->RxStats.RxRSSIPercentage[1]);

		/* Rx EVM*/
		PlatformEFIOWrite1Byte(Adapter, rS1_RXevmDump_Jaguar, Adapter->RxStats.RxEVMdbm[0]);
		PlatformEFIOWrite1Byte(Adapter, rS2_RXevmDump_Jaguar, Adapter->RxStats.RxEVMdbm[1]);

		/* Rx SNR*/
		PlatformEFIOWrite1Byte(Adapter, rA_RXsnrDump_Jaguar, (u1Byte)(Adapter->RxStats.RxSNRdB[0]));
		PlatformEFIOWrite1Byte(Adapter, rB_RXsnrDump_Jaguar, (u1Byte)(Adapter->RxStats.RxSNRdB[1]));

		/* Rx Cfo_Short*/
		PlatformEFIOWrite2Byte(Adapter, rA_CfoShortDump_Jaguar, Adapter->RxStats.RxCfoShort[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoShortDump_Jaguar, Adapter->RxStats.RxCfoShort[1]);

		/* Rx Cfo_Tail*/
		PlatformEFIOWrite2Byte(Adapter, rA_CfoLongDump_Jaguar, Adapter->RxStats.RxCfoTail[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoLongDump_Jaguar, Adapter->RxStats.RxCfoTail[1]);
	} else if (pDM_Odm->SupportICType == ODM_RTL8192E) {
		PlatformEFIOWrite1Byte(Adapter, rA_RSSIDump_92E, Adapter->RxStats.RxRSSIPercentage[0]);
		PlatformEFIOWrite1Byte(Adapter, rB_RSSIDump_92E, Adapter->RxStats.RxRSSIPercentage[1]);
		/* Rx EVM*/
		PlatformEFIOWrite1Byte(Adapter, rS1_RXevmDump_92E, Adapter->RxStats.RxEVMdbm[0]);
		PlatformEFIOWrite1Byte(Adapter, rS2_RXevmDump_92E, Adapter->RxStats.RxEVMdbm[1]);
		/* Rx SNR*/
		PlatformEFIOWrite1Byte(Adapter, rA_RXsnrDump_92E, (u1Byte)(Adapter->RxStats.RxSNRdB[0]));
		PlatformEFIOWrite1Byte(Adapter, rB_RXsnrDump_92E, (u1Byte)(Adapter->RxStats.RxSNRdB[1]));
		/* Rx Cfo_Short*/
		PlatformEFIOWrite2Byte(Adapter, rA_CfoShortDump_92E, Adapter->RxStats.RxCfoShort[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoShortDump_92E, Adapter->RxStats.RxCfoShort[1]);
		/* Rx Cfo_Tail*/
		PlatformEFIOWrite2Byte(Adapter, rA_CfoLongDump_92E, Adapter->RxStats.RxCfoTail[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoLongDump_92E, Adapter->RxStats.RxCfoTail[1]);
	}
}

VOID
odm_RefreshLdpcRtsMP(
	IN	PADAPTER			pAdapter,
	IN	PDM_ODM_T			pDM_Odm,
	IN	u1Byte				mMacId,
	IN	u1Byte				IOTPeer,
	IN	s4Byte				UndecoratedSmoothedPWDB
)
{
	BOOLEAN					bCtlLdpc = false;
	PMGNT_INFO				pMgntInfo = GetDefaultMgntInfo(pAdapter);
	PODM_RATE_ADAPTIVE		pRA = &pDM_Odm->RateAdaptive;

	if (pDM_Odm->SupportICType != ODM_RTL8821 && pDM_Odm->SupportICType != ODM_RTL8812)
		return;

	if ((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->CutVersion == ODM_CUT_A))
		bCtlLdpc = true;
	else if (pDM_Odm->SupportICType == ODM_RTL8812 &&
			 IOTPeer == HT_IOT_PEER_REALTEK_JAGUAR_CCUTAP)
		bCtlLdpc = true;

	if (bCtlLdpc) {
		if (UndecoratedSmoothedPWDB < (pRA->LdpcThres - 5))
			MgntSet_TX_LDPC(pAdapter, mMacId, true);
		else if (UndecoratedSmoothedPWDB > pRA->LdpcThres)
			MgntSet_TX_LDPC(pAdapter, mMacId, false);
	}

	if (UndecoratedSmoothedPWDB < (pRA->RtsThres - 5))
		pRA->bLowerRtsRate = true;
	else if (UndecoratedSmoothedPWDB > pRA->RtsThres)
		pRA->bLowerRtsRate = false;
}

VOID
ODM_DynamicARFBSelect(
	IN		PVOID		pDM_VOID,
	IN 		u1Byte			rate,
	IN  		BOOLEAN			Collision_State
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	pRA_T			pRA_Table = &pDM_Odm->DM_RA_Table;

	if (pDM_Odm->SupportICType != ODM_RTL8192E)
		return;

	if (Collision_State == pRA_Table->PT_collision_pre)
		return;

	if (rate >= DESC_RATEMCS8  && rate <= DESC_RATEMCS12) {
		if (Collision_State == 1) {
			if (rate == DESC_RATEMCS12) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07060501);
			} else if (rate == DESC_RATEMCS11) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07070605);
			} else if (rate == DESC_RATEMCS10) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08080706);
			} else if (rate == DESC_RATEMCS9) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08080707);
			} else {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09090808);
			}
		} else { /* Collision_State == 0*/
			if (rate == DESC_RATEMCS12) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x05010000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09080706);
			} else if (rate == DESC_RATEMCS11) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x06050000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09080807);
			} else if (rate == DESC_RATEMCS10) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x07060000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x0a090908);
			} else if (rate == DESC_RATEMCS9) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x07070000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x0a090808);
			} else {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x08080000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x0b0a0909);
			}
		}
	} else { /* MCS13~MCS15,  1SS, G-mode*/
		if (Collision_State == 1) {
			if (rate == DESC_RATEMCS15) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x05040302);
			} else if (rate == DESC_RATEMCS14) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x06050302);
			} else if (rate == DESC_RATEMCS13) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07060502);
			} else {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x06050402);
			}
		} else { // Collision_State == 0
			if (rate == DESC_RATEMCS15) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x03020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07060504);
			} else if (rate == DESC_RATEMCS14) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x03020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08070605);
			} else if (rate == DESC_RATEMCS13) {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x05020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09080706);
			} else {

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x04020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08070605);
			}


		}

	}
	pRA_Table->PT_collision_pre = Collision_State;
}

VOID
ODM_RateAdaptiveStateApInit(
	IN	PVOID		PADAPTER_VOID,
	IN	PRT_WLAN_STA  	pEntry
)
{
	PADAPTER		Adapter = (PADAPTER)PADAPTER_VOID;
	pEntry->Ratr_State = DM_RATR_STA_INIT;
}
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE) /*#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)*/

static void
FindMinimumRSSI(
	IN	PADAPTER	pAdapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);

	/*Determine the minimum RSSI*/

	if ((pDM_Odm->bLinked != true) &&
		(pHalData->EntryMinUndecoratedSmoothedPWDB == 0)) {
		pHalData->MinUndecoratedPWDBForDM = 0;
		/*ODM_RT_TRACE(pDM_Odm,COMP_BB_POWERSAVING, DBG_LOUD, ("Not connected to any\n"));*/
	} else
		pHalData->MinUndecoratedPWDBForDM = pHalData->EntryMinUndecoratedSmoothedPWDB;

	/*DBG_8192C("%s=>MinUndecoratedPWDBForDM(%d)\n",__FUNCTION__,pdmpriv->MinUndecoratedPWDBForDM);*/
	/*ODM_RT_TRACE(pDM_Odm,COMP_DIG, DBG_LOUD, ("MinUndecoratedPWDBForDM =%d\n",pHalData->MinUndecoratedPWDBForDM));*/
}

u8Byte
PhyDM_Get_Rate_Bitmap_Ex(
	IN	PVOID		pDM_VOID,
	IN	u4Byte		macid,
	IN	u8Byte		ra_mask,
	IN	u1Byte		rssi_level,
	OUT		u8Byte	*dm_RA_Mask,
	OUT		u1Byte	*dm_RteID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PSTA_INFO_T	pEntry;
	u8Byte	rate_bitmap = 0;
	u1Byte	WirelessMode;

	pEntry = pDM_Odm->pODM_StaInfo[macid];
	if (!IS_STA_VALID(pEntry))
		return ra_mask;
	WirelessMode = pEntry->wireless_mode;
	switch (WirelessMode) {
	case ODM_WM_B:
		if (ra_mask & 0x000000000000000c) /* 11M or 5.5M enable */
			rate_bitmap = 0x000000000000000d;
		else
			rate_bitmap = 0x000000000000000f;
		break;

	case (ODM_WM_G):
	case (ODM_WM_A):
		if (rssi_level == DM_RATR_STA_HIGH)
			rate_bitmap = 0x0000000000000f00;
		else
			rate_bitmap = 0x0000000000000ff0;
		break;

	case (ODM_WM_B|ODM_WM_G):
		if (rssi_level == DM_RATR_STA_HIGH)
			rate_bitmap = 0x0000000000000f00;
		else if (rssi_level == DM_RATR_STA_MIDDLE)
			rate_bitmap = 0x0000000000000ff0;
		else
			rate_bitmap = 0x0000000000000ff5;
		break;

	case (ODM_WM_B|ODM_WM_G|ODM_WM_N24G):
	case (ODM_WM_B|ODM_WM_N24G):
	case (ODM_WM_G|ODM_WM_N24G):
	case (ODM_WM_A|ODM_WM_N5G): {
		if (pDM_Odm->RFType == ODM_1T2R || pDM_Odm->RFType == ODM_1T1R) {
			if (rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x00000000000f0000;
			else if (rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x00000000000ff000;
			else {
				if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
					rate_bitmap = 0x00000000000ff015;
				else
					rate_bitmap = 0x00000000000ff005;
			}
		} else if (pDM_Odm->RFType == ODM_2T2R  || pDM_Odm->RFType == ODM_2T3R  || pDM_Odm->RFType == ODM_2T4R) {
			if (rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x000000000f8f0000;
			else if (rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x000000000f8ff000;
			else {
				if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
					rate_bitmap = 0x000000000f8ff015;
				else
					rate_bitmap = 0x000000000f8ff005;
			}
		} else {
			if (rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x0000000f0f0f0000;
			else if (rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x0000000fcfcfe000;
			else {
				if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
					rate_bitmap = 0x0000000ffffff015;
				else
					rate_bitmap = 0x0000000ffffff005;
			}
		}
	}
	break;

	case (ODM_WM_AC|ODM_WM_G):
		if (rssi_level == 1)
			rate_bitmap = 0x00000000fc3f0000;
		else if (rssi_level == 2)
			rate_bitmap = 0x00000000fffff000;
		else
			rate_bitmap = 0x00000000ffffffff;
		break;

	case (ODM_WM_AC|ODM_WM_A):

		if (pDM_Odm->RFType == ODM_1T2R || pDM_Odm->RFType == ODM_1T1R) {
			if (rssi_level == 1)				/* add by Gary for ac-series */
				rate_bitmap = 0x00000000003f8000;
			else if (rssi_level == 2)
				rate_bitmap = 0x00000000003fe000;
			else
				rate_bitmap = 0x00000000003ff010;
		} else if (pDM_Odm->RFType == ODM_2T2R  || pDM_Odm->RFType == ODM_2T3R  || pDM_Odm->RFType == ODM_2T4R) {
			if (rssi_level == 1)				/* add by Gary for ac-series */
				rate_bitmap = 0x00000000fe3f8000;       /* VHT 2SS MCS3~9 */
			else if (rssi_level == 2)
				rate_bitmap = 0x00000000fffff000;       /* VHT 2SS MCS0~9 */
			else
				rate_bitmap = 0x00000000fffff010;       /* All */
		} else {
			if (rssi_level == 1)				/* add by Gary for ac-series */
				rate_bitmap = 0x000003f8fe3f8000ULL;       /* VHT 3SS MCS3~9 */
			else if (rssi_level == 2)
				rate_bitmap = 0x000003fffffff000ULL;       /* VHT3SS MCS0~9 */
			else
				rate_bitmap = 0x000003fffffff010ULL;       /* All */
		}
		break;

	default:
		if (pDM_Odm->RFType == ODM_1T2R || pDM_Odm->RFType == ODM_1T1R)
			rate_bitmap = 0x00000000000fffff;
		else if (pDM_Odm->RFType == ODM_2T2R  || pDM_Odm->RFType == ODM_2T3R  || pDM_Odm->RFType == ODM_2T4R)
			rate_bitmap = 0x000000000fffffff;
		else
			rate_bitmap = 0x0000003fffffffffULL;
		break;

	}
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, (" ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%016llx\n", rssi_level, WirelessMode, rate_bitmap));

	return (ra_mask & rate_bitmap);
}


u4Byte
ODM_Get_Rate_Bitmap(
	IN	PVOID		pDM_VOID,
	IN	u4Byte		macid,
	IN	u4Byte 		ra_mask,
	IN	u1Byte 		rssi_level
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PSTA_INFO_T   	pEntry;
	u4Byte 	rate_bitmap = 0;
	u1Byte 	WirelessMode;
	//u1Byte 	WirelessMode =*(pDM_Odm->pWirelessMode);


	pEntry = pDM_Odm->pODM_StaInfo[macid];
	if (!IS_STA_VALID(pEntry))
		return ra_mask;

	WirelessMode = pEntry->wireless_mode;

	switch (WirelessMode) {
	case ODM_WM_B:
		if (ra_mask & 0x0000000c)		//11M or 5.5M enable
			rate_bitmap = 0x0000000d;
		else
			rate_bitmap = 0x0000000f;
		break;

	case (ODM_WM_G):
	case (ODM_WM_A):
		if (rssi_level == DM_RATR_STA_HIGH)
			rate_bitmap = 0x00000f00;
		else
			rate_bitmap = 0x00000ff0;
		break;

	case (ODM_WM_B|ODM_WM_G):
		if (rssi_level == DM_RATR_STA_HIGH)
			rate_bitmap = 0x00000f00;
		else if (rssi_level == DM_RATR_STA_MIDDLE)
			rate_bitmap = 0x00000ff0;
		else
			rate_bitmap = 0x00000ff5;
		break;

	case (ODM_WM_B|ODM_WM_G|ODM_WM_N24G)	:
	case (ODM_WM_B|ODM_WM_N24G)	:
	case (ODM_WM_G|ODM_WM_N24G)	:
	case (ODM_WM_A|ODM_WM_N5G)	: {
		if (pDM_Odm->RFType == ODM_1T2R || pDM_Odm->RFType == ODM_1T1R) {
			if (rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x000f0000;
			else if (rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x000ff000;
			else {
				if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
					rate_bitmap = 0x000ff015;
				else
					rate_bitmap = 0x000ff005;
			}
		} else {
			if (rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x0f8f0000;
			else if (rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x0f8ff000;
			else {
				if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
					rate_bitmap = 0x0f8ff015;
				else
					rate_bitmap = 0x0f8ff005;
			}
		}
	}
	break;

	case (ODM_WM_AC|ODM_WM_G):
		if (rssi_level == 1)
			rate_bitmap = 0xfc3f0000;
		else if (rssi_level == 2)
			rate_bitmap = 0xfffff000;
		else
			rate_bitmap = 0xffffffff;
		break;

	case (ODM_WM_AC|ODM_WM_A):

		if (pDM_Odm->RFType == RF_1T1R) {
			if (rssi_level == 1)				// add by Gary for ac-series
				rate_bitmap = 0x003f8000;
			else if (rssi_level == 2)
				rate_bitmap = 0x003ff000;
			else
				rate_bitmap = 0x003ff010;
		} else {
			if (rssi_level == 1)				// add by Gary for ac-series
				rate_bitmap = 0xfe3f8000;       // VHT 2SS MCS3~9
			else if (rssi_level == 2)
				rate_bitmap = 0xfffff000;       // VHT 2SS MCS0~9
			else
				rate_bitmap = 0xfffff010;       // All
		}
		break;

	default:
		if (pDM_Odm->RFType == RF_1T2R)
			rate_bitmap = 0x000fffff;
		else
			rate_bitmap = 0x0fffffff;
		break;

	}

	DBG_871X("%s ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x\n", __func__, rssi_level, WirelessMode, rate_bitmap);
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, (" ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x\n", rssi_level, WirelessMode, rate_bitmap));

	return (ra_mask & rate_bitmap);

}

#endif //#if (DM_ODM_SUPPORT_TYPE == ODM_CE)

#endif /*#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN| ODM_CE))*/

