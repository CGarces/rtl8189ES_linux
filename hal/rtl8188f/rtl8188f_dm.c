/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
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
/*============================================================ */
/* Description: */
/* */
/* This file is for 92CE/92CU dynamic mechanism only */
/* */
/* */
/*============================================================ */
#define _RTL8188F_DM_C_

/*============================================================ */
/* include files */
/*============================================================ */
#include <rtl8188f_hal.h>

/*============================================================ */
/* Global var */
/*============================================================ */


static VOID
dm_CheckProtection(
	IN	PADAPTER	Adapter
)
{

}

static VOID
dm_CheckStatistics(
	IN	PADAPTER	Adapter
)
{

}

/* */
/* Initialize GPIO setting registers */
/* */
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
/*============================================================ */
/* functions */
/*============================================================ */
static void Init_ODM_ComInfo_8188f(PADAPTER	Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
	u32 SupportAbility = 0;
	u8	cut_ver, fab_ver;

	Init_ODM_ComInfo(Adapter);

	ODM_CmnInfoInit(pDM_Odm, ODM_CMNINFO_PACKAGE_TYPE, pHalData->PackageType);

	fab_ver = ODM_TSMC;
	cut_ver = GET_CVID_CUT_VERSION(pHalData->VersionID);

	DBG_871X("%s(): fab_ver=%d cut_ver=%d\n", __func__, fab_ver, cut_ver);
	ODM_CmnInfoInit(pDM_Odm, ODM_CMNINFO_FAB_VER, fab_ver);
	ODM_CmnInfoInit(pDM_Odm, ODM_CMNINFO_CUT_VER, cut_ver);

#ifdef CONFIG_DISABLE_ODM
	SupportAbility = 0;
#else
	SupportAbility =	ODM_RF_CALIBRATION		|
						ODM_RF_TX_PWR_TRACK	/*| */
						;
	/*if(pHalData->AntDivCfg) */
	/*	pdmpriv->InitODMFlag |= ODM_BB_ANT_DIV; */
#endif

	ODM_CmnInfoUpdate(pDM_Odm, ODM_CMNINFO_ABILITY, SupportAbility);
}

static void Update_ODM_ComInfo_8188f(PADAPTER	Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
	u32 SupportAbility = 0;

	SupportAbility = 0
			 | ODM_BB_DIG
			 | ODM_BB_RA_MASK
			 | ODM_BB_DYNAMIC_TXPWR
			 | ODM_BB_FA_CNT
			 | ODM_BB_RSSI_MONITOR
			 | ODM_BB_CCK_PD
			 /* | ODM_BB_PWR_SAVE */
			 | ODM_BB_CFO_TRACKING
			 | ODM_MAC_EDCA_TURBO
			 | ODM_RF_TX_PWR_TRACK
			 | ODM_RF_CALIBRATION
			 | ODM_BB_NHM_CNT
			 /*| ODM_BB_PWR_TRAIN */
			 ;

	if (rtw_odm_adaptivity_needed(Adapter) == true) {
		rtw_odm_adaptivity_config_msg(RTW_DBGDUMP, Adapter);
		SupportAbility |= ODM_BB_ADAPTIVITY;
	}

#if (MP_DRIVER==1)
	if (Adapter->registrypriv.mp_mode == 1) {
		SupportAbility = 0
						 | ODM_RF_CALIBRATION
						 | ODM_RF_TX_PWR_TRACK
						 ;
	}
#endif/*(MP_DRIVER==1) */

#ifdef CONFIG_DISABLE_ODM
	SupportAbility = 0;
#endif/*CONFIG_DISABLE_ODM */

	ODM_CmnInfoUpdate(pDM_Odm, ODM_CMNINFO_ABILITY, SupportAbility);
}

void
rtl8188f_InitHalDm(
	IN	PADAPTER	Adapter
)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);

	u8	i;

	pHalData->DM_Type = DM_Type_ByDriver;

	Update_ODM_ComInfo_8188f(Adapter);

	ODM_DMInit(pDM_Odm);

}

VOID
rtl8188f_HalDmWatchDog(
	IN	PADAPTER	Adapter
)
{
	BOOLEAN		bFwCurrentInPSMode = false;
	BOOLEAN		bFwPSAwake = true;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);

	if (Adapter->registrypriv.mp_mode == 1 && Adapter->mppriv.mp_dm == 0) /* for MP power tracking */
		return;

	if (!rtw_is_hw_init_completed(Adapter))
		goto skip_dm;

	bFwCurrentInPSMode = adapter_to_pwrctl(Adapter)->bFwCurrentInPSMode;
	rtw_hal_get_hwreg(Adapter, HW_VAR_FWLPS_RF_ON, (u8 *)(&bFwPSAwake));

	/* Fw is under p2p powersaving mode, driver should stop dynamic mechanism. */
	/* modifed by thomas. 2011.06.11. */
	if (Adapter->wdinfo.p2p_ps_mode)
		bFwPSAwake = false;

	if ((rtw_is_hw_init_completed(Adapter))
		&& ((!bFwCurrentInPSMode) && bFwPSAwake)) {
		/* */
		/* Calculate Tx/Rx statistics. */
		/* */
		dm_CheckStatistics(Adapter);
		rtw_hal_check_rxfifo_full(Adapter);
		/* */
		/* Dynamically switch RTS/CTS protection. */
		/* */
		/*dm_CheckProtection(Adapter); */

	}

	/*ODM */
	if (rtw_is_hw_init_completed(Adapter)) {
		u8	bLinked = false;
		u8	bsta_state = false;
		u8	bBtDisabled = true;

		if (rtw_linked_check(Adapter)) {
			bLinked = true;
			if (check_fwstate(&Adapter->mlmepriv, WIFI_STATION_STATE))
				bsta_state = true;
		}

		ODM_CmnInfoUpdate(&pHalData->odmpriv , ODM_CMNINFO_LINK, bLinked);
		ODM_CmnInfoUpdate(&pHalData->odmpriv , ODM_CMNINFO_STATION_STATE, bsta_state);

		ODM_CmnInfoUpdate(&pHalData->odmpriv, ODM_CMNINFO_BT_ENABLED, ((bBtDisabled == true) ? false : true));

		ODM_DMWatchdog(&pHalData->odmpriv);
	}

skip_dm:

	return;
}

void rtl8188f_hal_dm_in_lps(PADAPTER padapter)
{
	u32	PWDB_rssi = 0;
	struct mlme_priv 	*pmlmepriv = &padapter->mlmepriv;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(padapter);
	PDM_ODM_T		pDM_Odm = &pHalData->odmpriv;
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta = NULL;

	DBG_871X("%s, RSSI_Min=%d\n", __func__, pDM_Odm->RSSI_Min);

	/*update IGI */
	ODM_Write_DIG(pDM_Odm, pDM_Odm->RSSI_Min);


	/*set rssi to fw */
	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta && (psta->rssi_stat.UndecoratedSmoothedPWDB > 0)) {
		PWDB_rssi = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB << 16));

		rtl8188f_set_rssi_cmd(padapter, (u8 *)&PWDB_rssi);
	}

}

void rtl8188f_HalDmWatchDog_in_LPS(IN	PADAPTER	Adapter)
{
	u8	bLinked = false;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	struct mlme_priv 	*pmlmepriv = &Adapter->mlmepriv;
	PDM_ODM_T		pDM_Odm = &pHalData->odmpriv;
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;
	struct sta_priv *pstapriv = &Adapter->stapriv;
	struct sta_info *psta = NULL;

	if (!rtw_is_hw_init_completed(Adapter))
		goto skip_lps_dm;


	if (rtw_linked_check(Adapter))
		bLinked = true;

	ODM_CmnInfoUpdate(&pHalData->odmpriv , ODM_CMNINFO_LINK, bLinked);

	if (bLinked == false)
		goto skip_lps_dm;

	if (!(pDM_Odm->SupportAbility & ODM_BB_RSSI_MONITOR))
		goto skip_lps_dm;


	/*ODM_DMWatchdog(&pHalData->odmpriv); */
	/*Do DIG by RSSI In LPS-32K */

	/*.1 Find MIN-RSSI */
	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta == NULL)
		goto skip_lps_dm;

	pHalData->EntryMinUndecoratedSmoothedPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

	DBG_871X("CurIGValue=%d, EntryMinUndecoratedSmoothedPWDB = %d\n", pDM_DigTable->CurIGValue, pHalData->EntryMinUndecoratedSmoothedPWDB);

	if (pHalData->EntryMinUndecoratedSmoothedPWDB <= 0)
		goto skip_lps_dm;

	pHalData->MinUndecoratedPWDBForDM = pHalData->EntryMinUndecoratedSmoothedPWDB;

	pDM_Odm->RSSI_Min = pHalData->MinUndecoratedPWDBForDM;

	/*if(pDM_DigTable->CurIGValue != pDM_Odm->RSSI_Min) */
	if ((pDM_DigTable->CurIGValue > pDM_Odm->RSSI_Min + 5) ||
		(pDM_DigTable->CurIGValue < pDM_Odm->RSSI_Min - 5))

	{
		rtw_dm_in_lps_wk_cmd(Adapter);
	}


skip_lps_dm:

	return;

}

void rtl8188f_init_dm_priv(IN PADAPTER Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T 		podmpriv = &pHalData->odmpriv;
	Init_ODM_ComInfo_8188f(Adapter);
}

void rtl8188f_deinit_dm_priv(IN PADAPTER Adapter)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T 		podmpriv = &pHalData->odmpriv;
}
