/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

//============================================================
// include files
//============================================================
#include "phydm_precomp.h"

VOID
phydm_ra_dynamic_retry_count(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	pRA_T		        pRA_Table = &pDM_Odm->DM_RA_Table;
	PSTA_INFO_T		pEntry;
	u1Byte	i, retry_offset;
	u4Byte	ma_rx_tp;
	/*ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("pDM_Odm->pre_b_noisy = %d\n", pDM_Odm->pre_b_noisy ));*/
	if (pDM_Odm->pre_b_noisy != pDM_Odm->NoisyDecision) {

		if (pDM_Odm->NoisyDecision) {
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("->Noisy Env. RA fallback value\n"));
			ODM_SetMACReg(pDM_Odm, 0x430, bMaskDWord, 0x0);
			ODM_SetMACReg(pDM_Odm, 0x434, bMaskDWord, 0x04030201);		
		} else {
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("->Clean Env. RA fallback value\n"));
			ODM_SetMACReg(pDM_Odm, 0x430, bMaskDWord, 0x02010000);
			ODM_SetMACReg(pDM_Odm, 0x434, bMaskDWord, 0x06050403);		
		}
		pDM_Odm->pre_b_noisy = pDM_Odm->NoisyDecision;
	}
}

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
VOID
phydm_ra_dynamic_rate_id_on_assoc(
	IN	PVOID	pDM_VOID,
	IN	u1Byte	wireless_mode,
	IN	u1Byte	init_rate_id
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("[ON ASSOC] rf_mode = ((0x%x)), wireless_mode = ((0x%x)), init_rate_id = ((0x%x))\n", pDM_Odm->RFType, wireless_mode, init_rate_id));
	
	if ((pDM_Odm->RFType == ODM_2T2R) | (pDM_Odm->RFType == ODM_2T2R_GREEN) | (pDM_Odm->RFType == ODM_2T3R) | (pDM_Odm->RFType == ODM_2T4R)) {
		
		if ((pDM_Odm->SupportICType & (ODM_RTL8812|ODM_RTL8192E)) &&
			(wireless_mode & (ODM_WM_N24G | ODM_WM_N5G))
			){
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("[ON ASSOC] set N-2SS ARFR5 table\n"));
			ODM_SetMACReg(pDM_Odm, 0x4a4, bMaskDWord, 0xfc1ffff);	/*N-2SS, ARFR5, rate_id = 0xe*/
			ODM_SetMACReg(pDM_Odm, 0x4a8, bMaskDWord, 0x0);		/*N-2SS, ARFR5, rate_id = 0xe*/
		} else if ((pDM_Odm->SupportICType & (ODM_RTL8812)) &&
			(wireless_mode & (ODM_WM_AC_5G | ODM_WM_AC_24G | ODM_WM_AC_ONLY))
			){
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("[ON ASSOC] set AC-2SS ARFR0 table\n"));
			ODM_SetMACReg(pDM_Odm, 0x444, bMaskDWord, 0x0fff);	/*AC-2SS, ARFR0, rate_id = 0x9*/
			ODM_SetMACReg(pDM_Odm, 0x448, bMaskDWord, 0xff01f000);		/*AC-2SS, ARFR0, rate_id = 0x9*/
		}
	}

}

VOID
phydm_ra_dynamic_rate_id_init(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	
	if (pDM_Odm->SupportICType & (ODM_RTL8812|ODM_RTL8192E)) {
		
		ODM_SetMACReg(pDM_Odm, 0x4a4, bMaskDWord, 0xfc1ffff);	/*N-2SS, ARFR5, rate_id = 0xe*/
		ODM_SetMACReg(pDM_Odm, 0x4a8, bMaskDWord, 0x0);		/*N-2SS, ARFR5, rate_id = 0xe*/
		
		ODM_SetMACReg(pDM_Odm, 0x444, bMaskDWord, 0x0fff);		/*AC-2SS, ARFR0, rate_id = 0x9*/
		ODM_SetMACReg(pDM_Odm, 0x448, bMaskDWord, 0xff01f000);	/*AC-2SS, ARFR0, rate_id = 0x9*/
	}
}

VOID
phydm_update_rate_id(
	IN	PVOID	pDM_VOID,
	IN	u1Byte	rate,
	IN	u1Byte	platform_macid
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	pRA_T		pRA_Table = &pDM_Odm->DM_RA_Table;
	u1Byte		current_tx_ss;
	u1Byte		rate_idx = rate & 0x7f; /*remove bit7 SGI*/
	u1Byte		wireless_mode;
	u1Byte		phydm_macid;
	PSTA_INFO_T	pEntry;
	
	if (rate_idx >= ODM_RATEVHTSS2MCS0) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("Rate[%d]: (( VHT2SS-MCS%d ))\n", platform_macid, (rate_idx-ODM_RATEVHTSS2MCS0)));
		/*dummy for SD4 check patch*/
	} else if (rate_idx >= ODM_RATEVHTSS1MCS0) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("Rate[%d]: (( VHT1SS-MCS%d ))\n", platform_macid, (rate_idx-ODM_RATEVHTSS1MCS0)));
		/*dummy for SD4 check patch*/
	} else if (rate_idx >= ODM_RATEMCS0) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("Rate[%d]: (( HT-MCS%d ))\n", platform_macid, (rate_idx-ODM_RATEMCS0)));
		/*dummy for SD4 check patch*/
	} else {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("Rate[%d]: (( HT-MCS%d ))\n", platform_macid, rate_idx));
		/*dummy for SD4 check patch*/
	}
		
	phydm_macid = pDM_Odm->platform2phydm_macid_table[platform_macid];
	pEntry = pDM_Odm->pODM_StaInfo[phydm_macid];
	
	if (IS_STA_VALID(pEntry)) {
		wireless_mode = pEntry->WirelessMode;

		if ((pDM_Odm->RFType  == ODM_2T2R) | (pDM_Odm->RFType  == ODM_2T2R_GREEN) | (pDM_Odm->RFType  == ODM_2T3R) | (pDM_Odm->RFType  == ODM_2T4R)) {
			
			pEntry->ratr_idx = pEntry->ratr_idx_init;
			if (wireless_mode & (ODM_WM_N24G | ODM_WM_N5G)) { /*N mode*/
				if (rate_idx >= ODM_RATEMCS8 && rate_idx <= ODM_RATEMCS15) { /*2SS mode*/
					
					pEntry->ratr_idx = ARFR_5_RATE_ID;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("ARFR_5\n"));
				}
			} else if (wireless_mode & (ODM_WM_AC_5G | ODM_WM_AC_24G | ODM_WM_AC_ONLY)) {/*AC mode*/
				if (rate_idx >= ODM_RATEVHTSS2MCS0 && rate_idx <= ODM_RATEVHTSS2MCS9) {/*2SS mode*/
					
					pEntry->ratr_idx = ARFR_0_RATE_ID;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("ARFR_0\n"));
				}
			}
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("UPdate_RateID[%d]: (( 0x%x ))\n", platform_macid, pEntry->ratr_idx));
		}
	}

}
#endif

VOID
phydm_c2h_ra_report_handler(
	IN PVOID	pDM_VOID,
	IN pu1Byte   CmdBuf,
	IN u1Byte   CmdLen
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	pRA_T		pRA_Table = &pDM_Odm->DM_RA_Table;
	u1Byte	legacy_table[12] = {1,2,5,11,6,9,12,18,24,36,48,54};
	u1Byte	macid = CmdBuf[1];
	
	u1Byte	rate = CmdBuf[0];
	u1Byte	rate_idx = rate & 0x7f; /*remove bit7 SGI*/
	u1Byte	vht_en=(rate_idx >= ODM_RATEVHTSS1MCS0)? 1 :0;	
	u1Byte	b_sgi = (rate & 0x80)>>7;
	
	u1Byte	pre_rate = pRA_Table->link_tx_rate[macid];
	u1Byte	pre_rate_idx = pre_rate & 0x7f; /*remove bit7 SGI*/
	u1Byte	pre_vht_en=(pre_rate_idx >= ODM_RATEVHTSS1MCS0)? 1 :0;	
	u1Byte	pre_b_sgi = (pre_rate & 0x80)>>7;
	
	#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	
	GET_HAL_DATA(Adapter)->CurrentRARate = HwRateToMRate(rate_idx);	
	#endif
	#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	ODM_UpdateInitRate(pDM_Odm, rate_idx);
	#endif

	/*ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD,("RA: rate_idx=0x%x , sgi = %d\n", rate_idx, b_sgi));*/
	/*if (pDM_Odm->SupportICType & (ODM_RTL8703B))*/
	{
		if (CmdLen >= 4) {
			if (CmdBuf[3] == 0) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("Init-Rate Update\n"));
				/**/
			} else if (CmdBuf[3] == 0xff) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("FW Level: Fix rate\n"));
				/**/
			} else if (CmdBuf[3] == 1) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("Try Success\n"));
				/**/
			} else if (CmdBuf[3] == 2) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("Try Fail & Try Again\n"));
				/**/
			} else if (CmdBuf[3] == 3) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("Rate Back\n"));
				/**/
			} else if (CmdBuf[3] == 4) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("start rate by RSSI\n"));
				/**/
			} else if (CmdBuf[3] == 5) {
				ODM_RT_TRACE(pDM_Odm, ODM_FW_DEBUG_TRACE, ODM_DBG_LOUD, ("Try rate\n"));
				/**/
			}
		}
	}
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("Tx Rate Update, MACID[%d] ( %s%s%s%s%d%s%s ) -> ( %s%s%s%s%d%s%s)\n",
		macid,
		((pre_rate_idx >= ODM_RATEVHTSS1MCS0) && (pre_rate_idx <= ODM_RATEVHTSS1MCS9)) ? "VHT 1ss  " : "",
		((pre_rate_idx >= ODM_RATEVHTSS2MCS0) && (pre_rate_idx <= ODM_RATEVHTSS2MCS9)) ? "VHT 2ss " : "",
		((pre_rate_idx >= ODM_RATEVHTSS3MCS0) && (pre_rate_idx <= ODM_RATEVHTSS3MCS9)) ? "VHT 3ss " : "",
		(pre_rate_idx >= ODM_RATEMCS0) ? "MCS " : "",
		(pre_vht_en) ? ((pre_rate_idx - ODM_RATEVHTSS1MCS0)%10) : ((pre_rate_idx >= ODM_RATEMCS0)? (pre_rate_idx - ODM_RATEMCS0) : ((pre_rate_idx <= ODM_RATE54M)?legacy_table[pre_rate_idx]:0)),
		(pre_b_sgi) ? "-S" : "  ",
		(pre_rate_idx >= ODM_RATEMCS0) ? "" : "M",
		((rate_idx >= ODM_RATEVHTSS1MCS0) && (rate_idx <= ODM_RATEVHTSS1MCS9)) ? "VHT 1ss  " : "",
		((rate_idx >= ODM_RATEVHTSS2MCS0) && (rate_idx <= ODM_RATEVHTSS2MCS9)) ? "VHT 2ss " : "",
		((rate_idx >= ODM_RATEVHTSS3MCS0) && (rate_idx <= ODM_RATEVHTSS3MCS9)) ? "VHT 3ss " : "",
		(rate_idx >= ODM_RATEMCS0) ? "MCS " : "",
		(vht_en) ? ((rate_idx - ODM_RATEVHTSS1MCS0)%10) : ((rate_idx >= ODM_RATEMCS0)? (rate_idx - ODM_RATEMCS0) : ((rate_idx <= ODM_RATE54M)?legacy_table[rate_idx]:0)),
		(b_sgi) ? "-S" : "  ",
		(rate_idx >= ODM_RATEMCS0) ? "" : "M" ));

	pRA_Table->link_tx_rate[macid] = rate;


	#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	if (pDM_Odm->SupportICType & (ODM_RTL8812|ODM_RTL8192E))
		phydm_update_rate_id(pDM_Odm, rate, macid);
	#endif

}

VOID
odm_RSSIMonitorInit(
	IN		PVOID		pDM_VOID
)
{
#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	pRA_T		pRA_Table = &pDM_Odm->DM_RA_Table;
	pRA_Table->firstconnect = FALSE;

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN))
	pRA_Table->PT_collision_pre = TRUE;   //used in ODM_DynamicARFBSelect(WIN only)
#endif
#endif
}

VOID
ODM_RAPostActionOnAssoc(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;

	pDM_Odm->H2C_RARpt_connect = 1;
	odm_RSSIMonitorCheck(pDM_Odm);
	pDM_Odm->H2C_RARpt_connect = 0;
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
	switch	(pDM_Odm->SupportPlatform) {
	case	ODM_WIN:
		odm_RSSIMonitorCheckMP(pDM_Odm);
		break;

	case	ODM_CE:
		odm_RSSIMonitorCheckCE(pDM_Odm);
		break;

	case	ODM_AP:
		odm_RSSIMonitorCheckAP(pDM_Odm);
		break;

	case	ODM_ADSL:
		//odm_DIGAP(pDM_Odm);
		break;
	}

}	// odm_RSSIMonitorCheck

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
s4Byte
phydm_FindMinimumRSSI(
IN		PDM_ODM_T		pDM_Odm,
IN		PADAPTER		pAdapter,
IN OUT	BOOLEAN	*pbLink_temp

	)
{	
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMGNT_INFO		pMgntInfo = &(pAdapter->MgntInfo);
	BOOLEAN			act_as_ap = ACTING_AS_AP(pAdapter);

	/*DbgPrint("bMediaConnect = %d,  ACTING_AS_AP = %d ,  EntryMinUndecoratedSmoothedPWDB = %d\n",
		pMgntInfo->bMediaConnect,act_as_ap,pHalData->EntryMinUndecoratedSmoothedPWDB);*/

	
	/* 1.Determine the minimum RSSI */
	if ((!pMgntInfo->bMediaConnect) ||	
		(act_as_ap && (pHalData->EntryMinUndecoratedSmoothedPWDB == 0))) {/* We should check AP mode and Entry info.into consideration, revised by Roger, 2013.10.18*/
	
		pHalData->MinUndecoratedPWDBForDM = 0;
		*pbLink_temp = FALSE;

	} else
		*pbLink_temp = TRUE; 
	

	if (pMgntInfo->bMediaConnect) {	/* Default port*/
	
		if (act_as_ap || pMgntInfo->mIbss) {
			pHalData->MinUndecoratedPWDBForDM = pHalData->EntryMinUndecoratedSmoothedPWDB;
			/**/
		} else {
			pHalData->MinUndecoratedPWDBForDM = pHalData->UndecoratedSmoothedPWDB;
			/**/
		}
	} else { /* associated entry pwdb*/
		pHalData->MinUndecoratedPWDBForDM = pHalData->EntryMinUndecoratedSmoothedPWDB;
		/**/
	}

	return pHalData->MinUndecoratedPWDBForDM;
}

#endif

VOID
odm_RSSIMonitorCheckMP(
	IN	PVOID	pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u1Byte			H2C_Parameter[4] = {0};
	u4Byte			i;
	BOOLEAN			bExtRAInfo = FALSE;
	u1Byte			cmdlen = 3;
	u1Byte			TxBF_EN = 0, stbc_en = 0;

	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PRT_WLAN_STA	pEntry = NULL;
	s4Byte			tmpEntryMaxPWDB = 0, tmpEntryMinPWDB = 0xff;
	PMGNT_INFO		pMgntInfo = &Adapter->MgntInfo;
	PMGNT_INFO		pDefaultMgntInfo = &Adapter->MgntInfo;
	u8Byte			curTxOkCnt = 0, curRxOkCnt = 0;
	//BOOLEAN			FirstConnect = 0;
	pRA_T			pRA_Table = &pDM_Odm->DM_RA_Table;
	pDIG_T			pDM_DigTable = &pDM_Odm->DM_DigTable;

	PADAPTER	pLoopAdapter = GetDefaultAdapter(Adapter);

	if (pDM_Odm->SupportICType & EXT_RA_INFO_SUPPORT_IC) {
		bExtRAInfo = TRUE;
		cmdlen = 4;
	}

	//FirstConnect = (pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == FALSE);
	//pRA_Table->firstconnect = pHalData->bLinked;


	/*
		if(pDM_Odm->SupportICType == ODM_RTL8188E && (pDefaultMgntInfo->CustomerID==RT_CID_819x_HP))
		{
			if(curRxOkCnt >(curTxOkCnt*6))
				PlatformEFIOWrite4Byte(Adapter, REG_ARFR0, 0x8f015);
			else
				PlatformEFIOWrite4Byte(Adapter, REG_ARFR0, 0xff015);
		}


		if(pDM_Odm->SupportICType == ODM_RTL8812 || pDM_Odm->SupportICType == ODM_RTL8821 ||
		   pDM_Odm->SupportICType == ODM_RTL8814A|| pDM_Odm->SupportICType == ODM_RTL8822B)
		{
			if(curRxOkCnt >(curTxOkCnt*6))
				H2C_Parameter[3]|=RAINFO_BE_RX_STATE;
		}
	*/

	while (pLoopAdapter) {

		if (pLoopAdapter != NULL) {
			pMgntInfo = &pLoopAdapter->MgntInfo;
			curTxOkCnt = pLoopAdapter->TxStats.NumTxBytesUnicast - pMgntInfo->lastTxOkCnt;
			curRxOkCnt = pLoopAdapter->RxStats.NumRxBytesUnicast - pMgntInfo->lastRxOkCnt;
			pMgntInfo->lastTxOkCnt = curTxOkCnt;
			pMgntInfo->lastRxOkCnt = curRxOkCnt;
		}

		for (i = 0; i < ASSOCIATE_ENTRY_NUM; i++) {

			if (IsAPModeExist(pLoopAdapter)) {
				if (GetFirstExtAdapter(pLoopAdapter) != NULL &&
					GetFirstExtAdapter(pLoopAdapter) == pLoopAdapter)
					pEntry = AsocEntry_EnumStation(pLoopAdapter, i);
				else if (GetFirstGOPort(pLoopAdapter) != NULL &&
						 IsFirstGoAdapter(pLoopAdapter))
					pEntry = AsocEntry_EnumStation(pLoopAdapter, i);
			} else {
				if (GetDefaultAdapter(pLoopAdapter) == pLoopAdapter)
					pEntry = AsocEntry_EnumStation(pLoopAdapter, i);
			}

			if (pEntry != NULL) {
				if (pEntry->bAssociated) {

					RT_DISP_ADDR(FDM, DM_PWDB, ("pEntry->MacAddr ="), pEntry->MacAddr);
					RT_DISP(FDM, DM_PWDB, ("pEntry->rssi = 0x%x(%d)\n",
										   pEntry->rssi_stat.UndecoratedSmoothedPWDB, pEntry->rssi_stat.UndecoratedSmoothedPWDB));

					//2 BF_en
#if (BEAMFORMING_SUPPORT)
					Beamform_cap = phydm_Beamforming_GetEntryBeamCapByMacId(pDM_Odm, pEntry->AssociatedMacId);
					if (Beamform_cap & (BEAMFORMER_CAP_HT_EXPLICIT | BEAMFORMER_CAP_VHT_SU))
						TxBF_EN = 1;
#endif
					//2 STBC_en
					if ((IS_WIRELESS_MODE_AC(Adapter) && TEST_FLAG(pEntry->VHTInfo.STBC, STBC_VHT_ENABLE_TX)) ||
						TEST_FLAG(pEntry->HTInfo.STBC, STBC_HT_ENABLE_TX))
						stbc_en = 1;

					if (pEntry->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
						tmpEntryMinPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;
					if (pEntry->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
						tmpEntryMaxPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;

					if (bExtRAInfo) {
						if (curRxOkCnt > (curTxOkCnt * 6))
							H2C_Parameter[3] |= RAINFO_BE_RX_STATE;

						if (TxBF_EN)
							H2C_Parameter[3] |= RAINFO_BF_STATE;
						else {
							if (stbc_en)
								H2C_Parameter[3] |= RAINFO_STBC_STATE;
						}

                        if ( pDM_Odm->NoisyDecision )
                        {
                            H2C_Parameter[3] |= RAINFO_NOISY_STATE;             // BIT2 
                        }
						else
							H2C_Parameter[3] &= (~RAINFO_NOISY_STATE);
                        
						if (pDM_Odm->H2C_RARpt_connect)
							H2C_Parameter[3] |= RAINFO_INIT_RSSI_RATE_STATE;
					}

					H2C_Parameter[2] = (u1Byte)(pEntry->rssi_stat.UndecoratedSmoothedPWDB & 0xFF);
					//H2C_Parameter[1] = 0x20;   // fw v12 cmdid 5:use max macid ,for nic ,default macid is 0 ,max macid is 1
					H2C_Parameter[0] = (pEntry->AssociatedMacId);

					ODM_FillH2CCmd(pDM_Odm, ODM_H2C_RSSI_REPORT, cmdlen, H2C_Parameter);
				}
			} else
				break;
		}

		pLoopAdapter = GetNextExtAdapter(pLoopAdapter);
	}

	if (tmpEntryMaxPWDB != 0) {	// If associated entry is found
		pHalData->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
		RT_DISP(FDM, DM_PWDB, ("EntryMaxPWDB = 0x%x(%d)\n",	tmpEntryMaxPWDB, tmpEntryMaxPWDB));
	} else
		pHalData->EntryMaxUndecoratedSmoothedPWDB = 0;

	if (tmpEntryMinPWDB != 0xff) { // If associated entry is found
		pHalData->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
		RT_DISP(FDM, DM_PWDB, ("EntryMinPWDB = 0x%x(%d)\n", tmpEntryMinPWDB, tmpEntryMinPWDB));

	} else
		pHalData->EntryMinUndecoratedSmoothedPWDB = 0;

	// Indicate Rx signal strength to FW.
	if (pHalData->bUseRAMask) {
		PRT_HIGH_THROUGHPUT 		pHTInfo = GET_HT_INFO(pDefaultMgntInfo);
		PRT_VERY_HIGH_THROUGHPUT	pVHTInfo = GET_VHT_INFO(pDefaultMgntInfo);

		//2 STBC_en
		if ((IS_WIRELESS_MODE_AC(Adapter) && TEST_FLAG(pVHTInfo->VhtCurStbc, STBC_VHT_ENABLE_TX)) ||
			TEST_FLAG(pHTInfo->HtCurStbc, STBC_HT_ENABLE_TX))
			stbc_en = 1;

		if (bExtRAInfo) {
			if (TxBF_EN)
				H2C_Parameter[3] |= RAINFO_BF_STATE;
			else {
				if (stbc_en)
					H2C_Parameter[3] |= RAINFO_STBC_STATE;
			}

			if (pDM_Odm->H2C_RARpt_connect)
				H2C_Parameter[3] |= RAINFO_INIT_RSSI_RATE_STATE;
			
            if ( pDM_Odm->NoisyDecision==1 )
            {
                H2C_Parameter[3] |= RAINFO_NOISY_STATE;             // BIT2
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_NOISY_DETECT, ODM_DBG_LOUD, ("[RSSIMonitorCheckMP] Send H2C to FW\n"));
            }
			else
				H2C_Parameter[3] &= (~RAINFO_NOISY_STATE);

			ODM_RT_TRACE(pDM_Odm, ODM_COMP_NOISY_DETECT, ODM_DBG_LOUD, ("[RSSIMonitorCheckMP] H2C_Parameter=%x\n", H2C_Parameter[3]));
		}

		H2C_Parameter[2] = (u1Byte)(pHalData->UndecoratedSmoothedPWDB & 0xFF);
		//H2C_Parameter[1] = 0x20;	// fw v12 cmdid 5:use max macid ,for nic ,default macid is 0 ,max macid is 1
		H2C_Parameter[0] = 0;		// fw v12 cmdid 5:use max macid ,for nic ,default macid is 0 ,max macid is 1

		ODM_FillH2CCmd(pDM_Odm, ODM_H2C_RSSI_REPORT, cmdlen, H2C_Parameter);

		// BT 3.0 HS mode Rssi
		if (pDM_Odm->bBtHsOperation) {
			H2C_Parameter[2] = pDM_Odm->btHsRssi;
			//H2C_Parameter[1] = 0x0;
			H2C_Parameter[0] = 2;

			ODM_FillH2CCmd(pDM_Odm, ODM_H2C_RSSI_REPORT, cmdlen, H2C_Parameter);
		}
	} else
		PlatformEFIOWrite1Byte(Adapter, 0x4fe, (u1Byte)pHalData->UndecoratedSmoothedPWDB);

	if ((pDM_Odm->SupportICType == ODM_RTL8812) || (pDM_Odm->SupportICType == ODM_RTL8192E))
		odm_RSSIDumpToRegister(pDM_Odm);


	{
		PADAPTER pLoopAdapter = GetDefaultAdapter(Adapter);
		BOOLEAN		default_pointer_value, *pbLink_temp = &default_pointer_value;
		s4Byte	GlobalRSSI_min = 0xFF, LocalRSSI_Min;
		BOOLEAN		bLink = FALSE;

		while (pLoopAdapter) {
			LocalRSSI_Min = phydm_FindMinimumRSSI(pDM_Odm, pLoopAdapter, pbLink_temp);
			//DbgPrint("pHalData->bLinked=%d, LocalRSSI_Min=%d\n", pHalData->bLinked, LocalRSSI_Min);
			if ((LocalRSSI_Min < GlobalRSSI_min) && (LocalRSSI_Min != 0))
				GlobalRSSI_min = LocalRSSI_Min;

			if (*pbLink_temp)
				bLink = TRUE;

			pLoopAdapter = GetNextExtAdapter(pLoopAdapter);
		}

		pHalData->bLinked = bLink;
		ODM_CmnInfoUpdate(&pHalData->DM_OutSrc , ODM_CMNINFO_LINK, (u8Byte)bLink);

		if (bLink)
			ODM_CmnInfoUpdate(&pHalData->DM_OutSrc, ODM_CMNINFO_RSSI_MIN, (u8Byte)GlobalRSSI_min);
		else
			ODM_CmnInfoUpdate(&pHalData->DM_OutSrc, ODM_CMNINFO_RSSI_MIN, 0);

	}

#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
}

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
/*H2C_RSSI_REPORT*/
s8 phydm_rssi_report(PDM_ODM_T pDM_Odm, u8 mac_id)
{
	PADAPTER Adapter = pDM_Odm->Adapter;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(Adapter);
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);
	u8 H2C_Parameter[4] = {0};
	u8 UL_DL_STATE = 0, STBC_TX = 0, TxBF_EN = 0;
	u8 cmdlen = 4, first_connect = _FALSE;
	u64	curTxOkCnt = 0, curRxOkCnt = 0;
	PSTA_INFO_T pEntry = pDM_Odm->pODM_StaInfo[mac_id];
	
	if (!IS_STA_VALID(pEntry))
		return _FAIL;

	if (mac_id != pEntry->mac_id) {
		DBG_871X("%s mac_id:%u:%u invalid\n", __func__, mac_id, pEntry->mac_id);
		rtw_warn_on(1);
		return _FAIL;
	}	
	
	if (IS_MCAST(pEntry->hwaddr))  /*if(psta->mac_id ==1)*/
		return _FAIL;

	if (pEntry->rssi_stat.UndecoratedSmoothedPWDB == (-1)) {
		DBG_871X("%s mac_id:%u, mac:"MAC_FMT", rssi == -1\n", __func__, pEntry->mac_id, MAC_ARG(pEntry->hwaddr));
		return _FAIL;
	}

	curTxOkCnt = pdvobjpriv->traffic_stat.cur_tx_bytes;
	curRxOkCnt = pdvobjpriv->traffic_stat.cur_rx_bytes;
	if (curRxOkCnt > (curTxOkCnt * 6))
		UL_DL_STATE = 1;
	else
		UL_DL_STATE = 0;
	
	if (TxBF_EN)
		STBC_TX = 0;
	else {
		#ifdef CONFIG_80211AC_VHT
		if (IsSupportedVHT(pEntry->wireless_mode))
			STBC_TX = TEST_FLAG(pEntry->vhtpriv.stbc_cap, STBC_VHT_ENABLE_TX);
		else
		#endif
			STBC_TX = TEST_FLAG(pEntry->htpriv.stbc_cap, STBC_HT_ENABLE_TX);
	}
		
	H2C_Parameter[0] = (u8)(pEntry->mac_id & 0xFF);
	H2C_Parameter[2] = pEntry->rssi_stat.UndecoratedSmoothedPWDB & 0x7F;
		
	if (UL_DL_STATE)
		H2C_Parameter[3] |= RAINFO_BE_RX_STATE;
		
	if (TxBF_EN)
		H2C_Parameter[3] |= RAINFO_BF_STATE;
	if (STBC_TX)
		H2C_Parameter[3] |= RAINFO_STBC_STATE;
	if (pDM_Odm->NoisyDecision)
		H2C_Parameter[3] |= RAINFO_NOISY_STATE;
		
	if (pEntry->ra_rpt_linked == _FALSE) {
		H2C_Parameter[3] |= RAINFO_INIT_RSSI_RATE_STATE;
		pEntry->ra_rpt_linked = _TRUE;
		first_connect = _TRUE;
	}
		
	if (first_connect) {
		DBG_871X("%s mac_id:%u, mac:"MAC_FMT", rssi:%d\n", __func__,
			pEntry->mac_id, MAC_ARG(pEntry->hwaddr), pEntry->rssi_stat.UndecoratedSmoothedPWDB);
			
		DBG_871X("%s RAINFO - TP:%s, TxBF:%s, STBC:%s, Noisy:%s, Firstcont:%s\n", __func__,
			(UL_DL_STATE) ? "DL" : "UL", (TxBF_EN) ? "EN" : "DIS", (STBC_TX) ? "EN" : "DIS",
			(pDM_Odm->NoisyDecision) ? "True" : "False", (first_connect) ? "True" : "False");
	}
		
	if (pHalData->fw_ractrl == _TRUE) {
		ODM_FillH2CCmd(pDM_Odm, ODM_H2C_RSSI_REPORT, cmdlen, H2C_Parameter);
	}
	return _SUCCESS;
}

void phydm_ra_rssi_rpt_wk_hdl(PVOID pContext)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pContext;
	int i;
	u8 mac_id = 0xFF;
	PSTA_INFO_T	pEntry = NULL;	
	
	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pEntry = pDM_Odm->pODM_StaInfo[i];
		if (IS_STA_VALID(pEntry)) {
			if (IS_MCAST(pEntry->hwaddr))  /*if(psta->mac_id ==1)*/
				continue;
			if (pEntry->ra_rpt_linked == _FALSE) {
				mac_id = i;
				break;
			}
		}
	}
	if (mac_id != 0xFF)
		phydm_rssi_report(pDM_Odm, mac_id);
}
void phydm_ra_rssi_rpt_wk(PVOID pContext)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pContext;
	
	rtw_run_in_thread_cmd(pDM_Odm->Adapter, phydm_ra_rssi_rpt_wk_hdl, pDM_Odm);
}
#endif

VOID
odm_RSSIMonitorCheckCE(
	IN		PVOID		pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);	
	PSTA_INFO_T           pEntry;
	int	i;
	int	tmpEntryMaxPWDB = 0, tmpEntryMinPWDB = 0xff;
	u8	sta_cnt = 0;
	
	if (pDM_Odm->bLinked != _TRUE)
		return;	

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pEntry = pDM_Odm->pODM_StaInfo[i];
		if (IS_STA_VALID(pEntry)) {
			if (IS_MCAST(pEntry->hwaddr))  /*if(psta->mac_id ==1)*/
				continue;

			if (pEntry->rssi_stat.UndecoratedSmoothedPWDB == (-1))
				continue;

			if (pEntry->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
				tmpEntryMinPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;

			if (pEntry->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
				tmpEntryMaxPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;

			if (phydm_rssi_report(pDM_Odm, i))
				sta_cnt++;
		}
	}
	/*DBG_871X("%s==> sta_cnt(%d)\n", __func__, sta_cnt);*/

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
#endif//if (DM_ODM_SUPPORT_TYPE == ODM_CE)
}


VOID
odm_RSSIMonitorCheckAP(
	IN		PVOID		pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
#if (RTL8881A_SUPPORT||RTL8192E_SUPPORT||RTL8814A_SUPPORT)

	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u1Byte 			H2C_Parameter[4] = {0};
	u4Byte			 i;
	BOOLEAN			bExtRAInfo = FALSE;
	u1Byte			cmdlen = 3 ;
	u1Byte			TxBF_EN = 0, stbc_en = 0;

	prtl8192cd_priv	priv		= pDM_Odm->priv;
	PSTA_INFO_T 		pstat;
	BOOLEAN			act_bfer = FALSE;

	if (pDM_Odm->H2C_RARpt_connect) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("[RA Init] First Connected\n"));
		/**/
	} else if (priv->up_time % 2)
		return;

	if (pDM_Odm->SupportICType & EXT_RA_INFO_SUPPORT_IC) {
		bExtRAInfo = TRUE;
		cmdlen = 4;
	}

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pstat = pDM_Odm->pODM_StaInfo[i];

		if (IS_STA_VALID(pstat)) {
			if (pstat->sta_in_firmware != 1)
				continue;

			//2 BF_en
#ifdef BEAMFORMING_SUPPORT
			BEAMFORMING_CAP Beamform_cap = Beamforming_GetEntryBeamCapByMacId(priv, pstat->aid);

			if (Beamform_cap == BEAMFORMER_CAP_HT_EXPLICIT || Beamform_cap == BEAMFORMER_CAP_VHT_SU ||
				Beamform_cap == (BEAMFORMER_CAP_HT_EXPLICIT | BEAMFORMEE_CAP_HT_EXPLICIT) ||
				Beamform_cap == (BEAMFORMER_CAP_VHT_SU | BEAMFORMEE_CAP_VHT_SU)) {
				TxBF_EN = 1;
				act_bfer = TRUE;
			}
#endif

			//2 STBC_en
			if ((priv->pmib->dot11nConfigEntry.dot11nSTBC) &&
				((pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_))
#ifdef RTK_AC_SUPPORT
				 || (pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(_VHTCAP_RX_STBC_CAP_))
#endif
				))
				stbc_en = 1;

			//2 RAINFO

			if (bExtRAInfo) {
				if ((pstat->rx_avarage)  > ((pstat->tx_avarage) * 6))
					H2C_Parameter[3] |= RAINFO_BE_RX_STATE;

				if (TxBF_EN)
					H2C_Parameter[3] |= RAINFO_BF_STATE;
				else {
					if (stbc_en)
						H2C_Parameter[3] |= RAINFO_STBC_STATE;
				}

                if ( pDM_Odm->NoisyDecision )
                {
                    H2C_Parameter[3] |= RAINFO_NOISY_STATE;             // BIT2
                }
				else
					H2C_Parameter[3] &= (~RAINFO_NOISY_STATE);
				
				if (pDM_Odm->H2C_RARpt_connect) {
					H2C_Parameter[3] |= RAINFO_INIT_RSSI_RATE_STATE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_RATE_ADAPTIVE, ODM_DBG_LOUD, ("[RA Init] set Init rate by RSSI\n"));
				}

				/*ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_RA_DBG, ODM_DBG_LOUD, ("[RAINFO] H2C_Para[3] = %x\n",H2C_Parameter[3]));*/
			}

			H2C_Parameter[2] = (u1Byte)(pstat->rssi & 0xFF);
			H2C_Parameter[0] = REMAP_AID(pstat);

            ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD,
            ("H2C_Parameter[3]=%d\n", H2C_Parameter[3]));

			//ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_RA_DBG, ODM_DBG_LOUD, ("[RSSI] H2C_Para[2] = %x,  \n",H2C_Parameter[2]));
			//ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_RA_DBG, ODM_DBG_LOUD, ("[MACID] H2C_Para[0] = %x,  \n",H2C_Parameter[0]));

			ODM_FillH2CCmd(pDM_Odm, ODM_H2C_RSSI_REPORT, cmdlen, H2C_Parameter);

		}
	}

#endif
#endif

}


VOID
odm_RateAdaptiveMaskInit(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PODM_RATE_ADAPTIVE	pOdmRA = &pDM_Odm->RateAdaptive;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PMGNT_INFO		pMgntInfo = &pDM_Odm->Adapter->MgntInfo;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pDM_Odm->Adapter);

	pMgntInfo->Ratr_State = DM_RATR_STA_INIT;

	if (pMgntInfo->DM_Type == DM_Type_ByDriver)
		pHalData->bUseRAMask = TRUE;
	else
		pHalData->bUseRAMask = FALSE;

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	pOdmRA->Type = DM_Type_ByDriver;
	if (pOdmRA->Type == DM_Type_ByDriver)
		pDM_Odm->bUseRAMask = _TRUE;
	else
		pDM_Odm->bUseRAMask = _FALSE;
#endif

	pOdmRA->RATRState = DM_RATR_STA_INIT;

#if(DM_ODM_SUPPORT_TYPE & ODM_WIN)
	if (pDM_Odm->SupportICType == ODM_RTL8812)
		pOdmRA->LdpcThres = 50;
	else
		pOdmRA->LdpcThres = 35;

	pOdmRA->RtsThres = 35;

#elif(DM_ODM_SUPPORT_TYPE & ODM_CE)
	pOdmRA->LdpcThres = 35;
	pOdmRA->bUseLdpc = FALSE;

#else
	pOdmRA->UltraLowRSSIThresh = 9;

#endif

	pOdmRA->HighRSSIThresh = 50;
#if (DM_ODM_SUPPORT_TYPE == ODM_AP) && \
	((DEV_BUS_TYPE == RT_USB_INTERFACE) || (DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	pOdmRA->LowRSSIThresh = 23;
#else
	pOdmRA->LowRSSIThresh = 20;
#endif
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
	switch	(pDM_Odm->SupportPlatform) {
	case	ODM_WIN:
		odm_RefreshRateAdaptiveMaskMP(pDM_Odm);
		break;

	case	ODM_CE:
		odm_RefreshRateAdaptiveMaskCE(pDM_Odm);
		break;

	case	ODM_AP:
	case	ODM_ADSL:
		odm_RefreshRateAdaptiveMaskAPADSL(pDM_Odm);
		break;
	}

}

VOID
odm_RefreshRateAdaptiveMaskMP(
	IN		PVOID		pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER				pAdapter	 =  pDM_Odm->Adapter;
	PADAPTER 				pTargetAdapter = NULL;
	HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(pAdapter);
	PMGNT_INFO				pMgntInfo = GetDefaultMgntInfo(pAdapter);

	if (pAdapter->bDriverStopped) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n"));
		return;
	}

	if (!pHalData->bUseRAMask) {
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n"));
		return;
	}

	// if default port is connected, update RA table for default port (infrastructure mode only)
	if (pMgntInfo->mAssoc && (!ACTING_AS_AP(pAdapter))) {
		odm_RefreshLdpcRtsMP(pAdapter, pDM_Odm, pMgntInfo->mMacId,  pMgntInfo->IOTPeer, pHalData->UndecoratedSmoothedPWDB);
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("odm_RefreshRateAdaptiveMask(): Infrasture Mode\n"));
		if (ODM_RAStateCheck(pDM_Odm, pHalData->UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pMgntInfo->Ratr_State)) {
			ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target AP addr : "), pMgntInfo->Bssid);
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pHalData->UndecoratedSmoothedPWDB, pMgntInfo->Ratr_State));
			pAdapter->HalFunc.UpdateHalRAMaskHandler(pAdapter, pMgntInfo->mMacId, NULL, pMgntInfo->Ratr_State);
		} else if (pDM_Odm->bChangeState) {
			ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target AP addr : "), pMgntInfo->Bssid);
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Change Power Training State, bDisablePowerTraining = %d\n", pDM_Odm->bDisablePowerTraining));
			pAdapter->HalFunc.UpdateHalRAMaskHandler(pAdapter, pMgntInfo->mMacId, NULL, pMgntInfo->Ratr_State);
		}
	}

	//
	// The following part configure AP/VWifi/IBSS rate adaptive mask.
	//

	if (pMgntInfo->mIbss) 	// Target: AP/IBSS peer.
		pTargetAdapter = GetDefaultAdapter(pAdapter);
	else
		pTargetAdapter = GetFirstAPAdapter(pAdapter);

	// if extension port (softap) is started, updaet RA table for more than one clients associate
	if (pTargetAdapter != NULL) {
		int	i;
		PRT_WLAN_STA	pEntry;

		for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
			pEntry = AsocEntry_EnumStation(pTargetAdapter, i);
			if (NULL != pEntry) {
				if (pEntry->bAssociated) {
					odm_RefreshLdpcRtsMP(pAdapter, pDM_Odm, pEntry->AssociatedMacId, pEntry->IOTPeer, pEntry->rssi_stat.UndecoratedSmoothedPWDB);

					if (ODM_RAStateCheck(pDM_Odm, pEntry->rssi_stat.UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pEntry->Ratr_State)) {
						ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pEntry->MacAddr);
						ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pEntry->rssi_stat.UndecoratedSmoothedPWDB, pEntry->Ratr_State));
						pAdapter->HalFunc.UpdateHalRAMaskHandler(pTargetAdapter, pEntry->AssociatedMacId, pEntry, pEntry->Ratr_State);
					} else if (pDM_Odm->bChangeState) {
						ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Change Power Training State, bDisablePowerTraining = %d\n", pDM_Odm->bDisablePowerTraining));
						pAdapter->HalFunc.UpdateHalRAMaskHandler(pAdapter, pMgntInfo->mMacId, NULL, pMgntInfo->Ratr_State);
					}
				}
			}
		}
	}

	if (pMgntInfo->bSetTXPowerTrainingByOid)
		pMgntInfo->bSetTXPowerTrainingByOid = FALSE;
#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
}


VOID
odm_RefreshRateAdaptiveMaskCE(
	IN	PVOID	pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
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

			if (TRUE == ODM_RAStateCheck(pDM_Odm, pstat->rssi_stat.UndecoratedSmoothedPWDB, FALSE , &pstat->rssi_level)) {
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level));
				//printk("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level);
				rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
			} else if (pDM_Odm->bChangeState) {
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Change Power Training State, bDisablePowerTraining = %d\n", pDM_Odm->bDisablePowerTraining));
				rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
			}

		}
	}

#endif
}

VOID
odm_RefreshRateAdaptiveMaskAPADSL(
	IN	PVOID	pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	struct rtl8192cd_priv *priv = pDM_Odm->priv;
	struct aid_obj *aidarray;
	u4Byte i;
	PSTA_INFO_T pstat;

	if (priv->up_time % 2)
		return;

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pstat = pDM_Odm->pODM_StaInfo[i];

		if (IS_STA_VALID(pstat)) {
#if defined(UNIVERSAL_REPEATER) || defined(MBSSID)
			aidarray = container_of(pstat, struct aid_obj, station);
			priv = aidarray->priv;
#endif

			if (!priv->pmib->dot11StationConfigEntry.autoRate)
				continue;

			if (ODM_RAStateCheck(pDM_Odm, (s4Byte)pstat->rssi, FALSE, &pstat->rssi_level)) {
				ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pstat->hwaddr);
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi, pstat->rssi_level));

#ifdef CONFIG_WLAN_HAL
				if (IS_HAL_CHIP(priv)) {
#ifdef WDS
//					if(!(pstat->state & WIFI_WDS))//if WDS donot setting
#endif
					GET_HAL_INTERFACE(priv)->UpdateHalRAMaskHandler(priv, pstat, pstat->rssi_level);
				} else
#endif
#ifdef CONFIG_RTL_8812_SUPPORT
					if (GET_CHIP_VER(priv) == VERSION_8812E)
						UpdateHalRAMask8812(priv, pstat, 3);
					else
#endif
#ifdef CONFIG_RTL_88E_SUPPORT
						if (GET_CHIP_VER(priv) == VERSION_8188E) {
#ifdef TXREPORT
							add_RATid(priv, pstat);
#endif
						} else
#endif
						{
#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_92C_SUPPORT)
							add_update_RATid(priv, pstat);
#endif
						}
			}
		}
	}
#endif
}


// Return Value: BOOLEAN
// - TRUE: RATRState is changed.
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
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	u1Byte UltraLowRSSIThreshForRA = pRA->UltraLowRSSIThresh;
	if (pDM_Odm->SupportICType == ODM_RTL8881A)
		LowRSSIThreshForRA = 30;		// for LDPC / BCC switch
#endif

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
		ODM_RT_ASSERT(pDM_Odm, FALSE, ("wrong rssi level setting %d !", *pRATRState));
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
		return TRUE;
	}

	return FALSE;
}

VOID
odm_RefreshBasicRateMask(
	IN	PVOID	pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		Adapter	 =  pDM_Odm->Adapter;
	static u1Byte		Stage = 0;
	u1Byte			CurStage = 0;
	OCTET_STRING 	osRateSet;
	PMGNT_INFO		pMgntInfo = GetDefaultMgntInfo(Adapter);
	u1Byte 			RateSet[5] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M};

	if (pDM_Odm->SupportICType != ODM_RTL8812 && pDM_Odm->SupportICType != ODM_RTL8821)
		return;

	if (pDM_Odm->bLinked == FALSE)	// unlink Default port information
		CurStage = 0;
	else if (pDM_Odm->RSSI_Min < 40)	// link RSSI  < 40%
		CurStage = 1;
	else if (pDM_Odm->RSSI_Min > 45)	// link RSSI > 45%
		CurStage = 3;
	else
		CurStage = 2;					// link  25% <= RSSI <= 30%

	if (CurStage != Stage) {
		if (CurStage == 1) {
			FillOctetString(osRateSet, RateSet, 5);
			FilterSupportRate(pMgntInfo->mBrates, &osRateSet, FALSE);
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_BASIC_RATE, (pu1Byte)&osRateSet);
		} else if (CurStage == 3 && (Stage == 1 || Stage == 2))
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_BASIC_RATE, (pu1Byte)(&pMgntInfo->mBrates));
	}

	Stage = CurStage;
#endif
}


VOID
phydm_ra_info_init(
	IN	PVOID	pDM_VOID
	)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;

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
	
	return ret_bitmap;
	
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

	pDM_Odm->TxRate = Rate;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if DEV_BUS_TYPE == RT_PCI_INTERFACE
#if USE_WORKITEM
		PlatformScheduleWorkItem(&pDM_Odm->RaRptWorkitem);
#else
		if (pDM_Odm->SupportICType == ODM_RTL8723B) {
#if (RTL8723B_SUPPORT == 1)
			ODM_TxPwrTrackSetPwr_8723B(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
#endif
		} else if (pDM_Odm->SupportICType == ODM_RTL8192E) {
			for (p = ODM_RF_PATH_A; p < MAX_PATH_NUM_8192E; p++) {
#if (RTL8192E_SUPPORT == 1)
				ODM_TxPwrTrackSetPwr92E(pDM_Odm, MIX_MODE, p, 0);
#endif
			}
		}
#endif
#else
		PlatformScheduleWorkItem(&pDM_Odm->RaRptWorkitem);
#endif
#endif

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
	BOOLEAN					bCtlLdpc = FALSE;
	PMGNT_INFO				pMgntInfo = GetDefaultMgntInfo(pAdapter);
	PODM_RATE_ADAPTIVE		pRA = &pDM_Odm->RateAdaptive;

	if (pDM_Odm->SupportICType != ODM_RTL8821 && pDM_Odm->SupportICType != ODM_RTL8812)
		return;

	if ((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->CutVersion == ODM_CUT_A))
		bCtlLdpc = TRUE;
	else if (pDM_Odm->SupportICType == ODM_RTL8812 &&
			 IOTPeer == HT_IOT_PEER_REALTEK_JAGUAR_CCUTAP)
		bCtlLdpc = TRUE;

	if (bCtlLdpc) {
		if (UndecoratedSmoothedPWDB < (pRA->LdpcThres - 5))
			MgntSet_TX_LDPC(pAdapter, mMacId, TRUE);
		else if (UndecoratedSmoothedPWDB > pRA->LdpcThres)
			MgntSet_TX_LDPC(pAdapter, mMacId, FALSE);
	}

	if (UndecoratedSmoothedPWDB < (pRA->RtsThres - 5))
		pRA->bLowerRtsRate = TRUE;
	else if (UndecoratedSmoothedPWDB > pRA->RtsThres)
		pRA->bLowerRtsRate = FALSE;
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

	if ((pDM_Odm->bLinked != _TRUE) &&
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

