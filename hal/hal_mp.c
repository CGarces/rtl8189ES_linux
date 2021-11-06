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
#define _HAL_MP_C_
#ifdef CONFIG_MP_INCLUDED
#include <rtl8188f_hal.h>

u8 MgntQuery_NssTxRate(u16 Rate)
{
	u8	NssNum = RF_TX_NUM_NONIMPLEMENT;
	
	if ((Rate >= MGN_MCS8 && Rate <= MGN_MCS15) || 
		 (Rate >= MGN_VHT2SS_MCS0 && Rate <= MGN_VHT2SS_MCS9))
		NssNum = RF_2TX;
	else if ((Rate >= MGN_MCS16 && Rate <= MGN_MCS23) || 
		 (Rate >= MGN_VHT3SS_MCS0 && Rate <= MGN_VHT3SS_MCS9))
		NssNum = RF_3TX;
	else if ((Rate >= MGN_MCS24 && Rate <= MGN_MCS31) || 
		 (Rate >= MGN_VHT4SS_MCS0 && Rate <= MGN_VHT4SS_MCS9))
		NssNum = RF_4TX;
	else
		NssNum = RF_1TX;
		
	return NssNum;
}

void hal_mpt_SwitchRfSetting(PADAPTER	pAdapter)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.MptCtx);
	u8				ChannelToSw = pMptCtx->MptChannelToSw;
	ULONG				ulRateIdx = pMptCtx->MptRateIndex;
	ULONG				ulbandwidth = pMptCtx->MptBandWidth;
	
	/* <20120525, Kordan> Dynamic mechanism for APK, asked by Dennis.*/
	if (IS_HARDWARE_TYPE_8188ES(pAdapter) && (1 <= ChannelToSw && ChannelToSw <= 11) &&
		(ulRateIdx == MPT_RATE_MCS0 || ulRateIdx == MPT_RATE_1M || ulRateIdx == MPT_RATE_6M)) {
		pMptCtx->backup0x52_RF_A = (u1Byte)PHY_QueryRFReg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0);
		pMptCtx->backup0x52_RF_B = (u1Byte)PHY_QueryRFReg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0);
		
		if ((PlatformEFIORead4Byte(pAdapter, 0xF4)&BIT29) == BIT29) {
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0xB);
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0xB);
		} else {
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0xD);
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0xD);
		}
	} else if (IS_HARDWARE_TYPE_8188EE(pAdapter)) { /* <20140903, VincentL> Asked by RF Eason and Edlu*/
	
		if (ChannelToSw == 3 && ulbandwidth == MPT_BW_40MHZ) {
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0xB); /*RF 0x52 = 0x0007E4BD*/
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0xB); /*RF 0x52 = 0x0007E4BD*/
		} else {
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, 0x9); /*RF 0x52 = 0x0007E49D*/
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, 0x9); /*RF 0x52 = 0x0007E49D*/
		}
		
	} else if (IS_HARDWARE_TYPE_8188E(pAdapter)) {
		PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, RF_0x52, 0x000F0, pMptCtx->backup0x52_RF_A);
		PHY_SetRFReg(pAdapter, ODM_RF_PATH_B, RF_0x52, 0x000F0, pMptCtx->backup0x52_RF_B);
	}
}

s32 hal_mpt_SetPowerTracking(PADAPTER padapter, u8 enable)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);


	if (!netif_running(padapter->pnetdev)) {
		RT_TRACE(_module_mp_, _drv_warning_, ("SetPowerTracking! Fail: interface not opened!\n"));
		return _FAIL;
	}

	if (check_fwstate(&padapter->mlmepriv, WIFI_MP_STATE) == _FALSE) {
		RT_TRACE(_module_mp_, _drv_warning_, ("SetPowerTracking! Fail: not in MP mode!\n"));
		return _FAIL;
	}
	if (enable)
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _TRUE;	
	else
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _FALSE;

	return _SUCCESS;
}

void hal_mpt_GetPowerTracking(PADAPTER padapter, u8 *enable)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);


	*enable = pDM_Odm->RFCalibrateInfo.TxPowerTrackControl;
}


void hal_mpt_CCKTxPowerAdjust(PADAPTER Adapter, BOOLEAN bInCH14)
{
	u32		TempVal = 0, TempVal2 = 0, TempVal3 = 0;
	u32		CurrCCKSwingVal = 0, CCKSwingIndex = 12;
	u8		i;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMPT_CONTEXT		pMptCtx = &(Adapter->mppriv.MptCtx);
	u1Byte				u1Channel = pHalData->CurrentChannel;
	ULONG				ulRateIdx = pMptCtx->MptRateIndex;
	u1Byte				DataRate = 0xFF;


	DataRate = MptToMgntRate(ulRateIdx);

	if (u1Channel == 14 && IS_CCK_RATE(DataRate))
		pHalData->bCCKinCH14 = TRUE;
	else
		pHalData->bCCKinCH14 = FALSE;

	if (IS_HARDWARE_TYPE_8188F(Adapter)) {
		/* get current cck swing value and check 0xa22 & 0xa23 later to match the table.*/
		CurrCCKSwingVal = read_bbreg(Adapter, rCCK0_TxFilter1, bMaskHWord);
		CCKSwingIndex = 20; /* default index */

		if (!pHalData->bCCKinCH14) {
			/* Readback the current bb cck swing value and compare with the table to */
			/* get the current swing index */
			for (i = 0; i < CCK_TABLE_SIZE_88F; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)CCKSwingTable_Ch1_Ch13_88F[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)CCKSwingTable_Ch1_Ch13_88F[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}
			write_bbreg(Adapter, 0xa22, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][0]);
			write_bbreg(Adapter, 0xa23, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][1]);
			write_bbreg(Adapter, 0xa24, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][2]);
			write_bbreg(Adapter, 0xa25, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][3]);
			write_bbreg(Adapter, 0xa26, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][4]);
			write_bbreg(Adapter, 0xa27, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][5]);
			write_bbreg(Adapter, 0xa28, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][6]);
			write_bbreg(Adapter, 0xa29, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][7]);
			write_bbreg(Adapter, 0xa9a, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][8]);
			write_bbreg(Adapter, 0xa9b, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][9]);
			write_bbreg(Adapter, 0xa9c, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][10]);
			write_bbreg(Adapter, 0xa9d, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][11]);
			write_bbreg(Adapter, 0xaa0, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][12]);
			write_bbreg(Adapter, 0xaa1, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][13]);
			write_bbreg(Adapter, 0xaa2, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][14]);
			write_bbreg(Adapter, 0xaa3, bMaskByte0, CCKSwingTable_Ch1_Ch13_88F[CCKSwingIndex][15]);
			DBG_871X("%s , CCKSwingTable_Ch1_Ch13_88F[%d]\n", __func__, CCKSwingIndex);
		}  else {
			for (i = 0; i < CCK_TABLE_SIZE_88F; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)CCKSwingTable_Ch14_88F[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)CCKSwingTable_Ch14_88F[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}
			write_bbreg(Adapter, 0xa22, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][0]);
			write_bbreg(Adapter, 0xa23, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][1]);
			write_bbreg(Adapter, 0xa24, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][2]);
			write_bbreg(Adapter, 0xa25, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][3]);
			write_bbreg(Adapter, 0xa26, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][4]);
			write_bbreg(Adapter, 0xa27, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][5]);
			write_bbreg(Adapter, 0xa28, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][6]);
			write_bbreg(Adapter, 0xa29, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][7]);
			write_bbreg(Adapter, 0xa9a, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][8]);
			write_bbreg(Adapter, 0xa9b, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][9]);
			write_bbreg(Adapter, 0xa9c, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][10]);
			write_bbreg(Adapter, 0xa9d, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][11]);
			write_bbreg(Adapter, 0xaa0, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][12]);
			write_bbreg(Adapter, 0xaa1, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][13]);
			write_bbreg(Adapter, 0xaa2, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][14]);
			write_bbreg(Adapter, 0xaa3, bMaskByte0, CCKSwingTable_Ch14_88F[CCKSwingIndex][15]);
			DBG_871X("%s , CCKSwingTable_Ch14_88F[%d]\n", __func__, CCKSwingIndex);
		}
	} else {

		/* get current cck swing value and check 0xa22 & 0xa23 later to match the table.*/
		CurrCCKSwingVal = read_bbreg(Adapter, rCCK0_TxFilter1, bMaskHWord);

		if (!pHalData->bCCKinCH14) {
			/* Readback the current bb cck swing value and compare with the table to */
			/* get the current swing index */
			for (i = 0; i < CCK_TABLE_SIZE; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)CCKSwingTable_Ch1_Ch13[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)CCKSwingTable_Ch1_Ch13[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}

			/*Write 0xa22 0xa23*/
			TempVal = CCKSwingTable_Ch1_Ch13[CCKSwingIndex][0] +
				(CCKSwingTable_Ch1_Ch13[CCKSwingIndex][1] << 8);


			/*Write 0xa24 ~ 0xa27*/
			TempVal2 = 0;
			TempVal2 = CCKSwingTable_Ch1_Ch13[CCKSwingIndex][2] +
				(CCKSwingTable_Ch1_Ch13[CCKSwingIndex][3] << 8) +
				(CCKSwingTable_Ch1_Ch13[CCKSwingIndex][4] << 16) +
				(CCKSwingTable_Ch1_Ch13[CCKSwingIndex][5] << 24);

			/*Write 0xa28  0xa29*/
			TempVal3 = 0;
			TempVal3 = CCKSwingTable_Ch1_Ch13[CCKSwingIndex][6] +
				(CCKSwingTable_Ch1_Ch13[CCKSwingIndex][7] << 8);
		}  else {
			for (i = 0; i < CCK_TABLE_SIZE; i++) {
				if (((CurrCCKSwingVal & 0xff) == (u32)CCKSwingTable_Ch14[i][0]) &&
				    (((CurrCCKSwingVal & 0xff00) >> 8) == (u32)CCKSwingTable_Ch14[i][1])) {
					CCKSwingIndex = i;
					break;
				}
			}

			/*Write 0xa22 0xa23*/
			TempVal = CCKSwingTable_Ch14[CCKSwingIndex][0] +
				  (CCKSwingTable_Ch14[CCKSwingIndex][1] << 8);

			/*Write 0xa24 ~ 0xa27*/
			TempVal2 = 0;
			TempVal2 = CCKSwingTable_Ch14[CCKSwingIndex][2] +
				   (CCKSwingTable_Ch14[CCKSwingIndex][3] << 8) +
				(CCKSwingTable_Ch14[CCKSwingIndex][4] << 16) +
				   (CCKSwingTable_Ch14[CCKSwingIndex][5] << 24);

			/*Write 0xa28  0xa29*/
			TempVal3 = 0;
			TempVal3 = CCKSwingTable_Ch14[CCKSwingIndex][6] +
				   (CCKSwingTable_Ch14[CCKSwingIndex][7] << 8);
		}

		write_bbreg(Adapter, rCCK0_TxFilter1, bMaskHWord, TempVal);
		write_bbreg(Adapter, rCCK0_TxFilter2, bMaskDWord, TempVal2);
		write_bbreg(Adapter, rCCK0_DebugPort, bMaskLWord, TempVal3);
	}

}

void hal_mpt_SetChannel(PADAPTER pAdapter)
{
	u8 eRFPath;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);
	struct mp_priv	*pmp = &pAdapter->mppriv;
	u8		channel = pmp->channel;
	u8		bandwidth = pmp->bandwidth;

	hal_mpt_SwitchRfSetting(pAdapter);

	pHalData->bSwChnl = _TRUE;
	pHalData->bSetChnlBW = _TRUE;
	rtw_hal_set_chnl_bw(pAdapter, channel, bandwidth, 0, 0);

	hal_mpt_CCKTxPowerAdjust(pAdapter, pHalData->bCCKinCH14);

}

/*
 * Notice
 *	Switch bandwitdth may change center frequency(channel)
 */
void hal_mpt_SetBandwidth(PADAPTER pAdapter)
{
	struct mp_priv *pmp = &pAdapter->mppriv;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	
	u8		channel = pmp->channel;
	u8		bandwidth = pmp->bandwidth;

	pHalData->bSwChnl = _TRUE;
	pHalData->bSetChnlBW = _TRUE;
	rtw_hal_set_chnl_bw(pAdapter, channel, bandwidth, 0, 0);
	
	hal_mpt_SwitchRfSetting(pAdapter);
}

void mpt_SetTxPower_Old(PADAPTER pAdapter, MPT_TXPWR_DEF Rate, u8 *pTxPower)
{
	RT_TRACE(_module_mp_, DBG_LOUD, ("===>mpt_SetTxPower_Old(): Case = %d\n", Rate));
	switch (Rate) {
	case MPT_CCK:
			{
			u4Byte	TxAGC = 0, pwr = 0;
			u1Byte	rf;

			pwr = pTxPower[ODM_RF_PATH_A];
			if (pwr < 0x3f) {
				TxAGC = (pwr<<16)|(pwr<<8)|(pwr);
				PHY_SetBBReg(pAdapter, rTxAGC_A_CCK1_Mcs32, bMaskByte1, pTxPower[ODM_RF_PATH_A]);
				PHY_SetBBReg(pAdapter, rTxAGC_B_CCK11_A_CCK2_11, 0xffffff00, TxAGC);
			}
			pwr = pTxPower[ODM_RF_PATH_B];
			if (pwr < 0x3f) {
				TxAGC = (pwr<<16)|(pwr<<8)|(pwr);
				PHY_SetBBReg(pAdapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte0, pTxPower[ODM_RF_PATH_B]);
				PHY_SetBBReg(pAdapter, rTxAGC_B_CCK1_55_Mcs32, 0xffffff00, TxAGC);
			}
		    
			} break;

	case MPT_OFDM_AND_HT:
			{
			u4Byte	TxAGC = 0;
			u1Byte	pwr = 0, rf;
			
			pwr = pTxPower[0];
			if (pwr < 0x3f) {
				TxAGC |= ((pwr<<24)|(pwr<<16)|(pwr<<8)|pwr);
				DBG_871X("HT Tx-rf(A) Power = 0x%x\n", TxAGC);
				
				PHY_SetBBReg(pAdapter, rTxAGC_A_Rate18_06, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_A_Rate54_24, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_A_Mcs03_Mcs00, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_A_Mcs07_Mcs04, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_A_Mcs11_Mcs08, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_A_Mcs15_Mcs12, bMaskDWord, TxAGC);
			}
			TxAGC = 0;
			pwr = pTxPower[1];
			if (pwr < 0x3f) {
				TxAGC |= ((pwr<<24)|(pwr<<16)|(pwr<<8)|pwr);
				DBG_871X("HT Tx-rf(B) Power = 0x%x\n", TxAGC);
				
				PHY_SetBBReg(pAdapter, rTxAGC_B_Rate18_06, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_B_Rate54_24, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_B_Mcs03_Mcs00, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_B_Mcs07_Mcs04, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_B_Mcs11_Mcs08, bMaskDWord, TxAGC);
				PHY_SetBBReg(pAdapter, rTxAGC_B_Mcs15_Mcs12, bMaskDWord, TxAGC);
			}
			} break;

	default:
		break;
	}	
		DBG_871X("<===mpt_SetTxPower_Old()\n");
}



void 
mpt_SetTxPower(
		PADAPTER		pAdapter,
		MPT_TXPWR_DEF	Rate,
		pu1Byte	pTxPower
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	u1Byte path = 0 , i = 0, MaxRate = MGN_6M;
	u1Byte StartPath = ODM_RF_PATH_A, EndPath = ODM_RF_PATH_B;
	
	if (IS_HARDWARE_TYPE_8188F(pAdapter))
		EndPath = ODM_RF_PATH_A;

	switch (Rate) {
	case MPT_CCK:
			{
			u1Byte rate[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M};

			for (path = StartPath; path <= EndPath; path++)
				for (i = 0; i < sizeof(rate); ++i)
					PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);	
			}
			break;
		
	case MPT_OFDM:
			{
			u1Byte rate[] = {
				MGN_6M, MGN_9M, MGN_12M, MGN_18M,
				MGN_24M, MGN_36M, MGN_48M, MGN_54M,
				};

			for (path = StartPath; path <= EndPath; path++)
				for (i = 0; i < sizeof(rate); ++i)
					PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);	
			} break;
		
	case MPT_HT:
			{
			u1Byte rate[] = {
			MGN_MCS0, MGN_MCS1, MGN_MCS2, MGN_MCS3, MGN_MCS4,
			MGN_MCS5, MGN_MCS6, MGN_MCS7, MGN_MCS8, MGN_MCS9,
			MGN_MCS10, MGN_MCS11, MGN_MCS12, MGN_MCS13, MGN_MCS14,
			MGN_MCS15, MGN_MCS16, MGN_MCS17, MGN_MCS18, MGN_MCS19,
			MGN_MCS20, MGN_MCS21, MGN_MCS22, MGN_MCS23, MGN_MCS24,
			MGN_MCS25, MGN_MCS26, MGN_MCS27, MGN_MCS28, MGN_MCS29,
			MGN_MCS30, MGN_MCS31,
			};
			if (pHalData->rf_type == RF_3T3R)
				MaxRate = MGN_MCS23;
			else if (pHalData->rf_type == RF_2T2R)
				MaxRate = MGN_MCS15;
			else
				MaxRate = MGN_MCS7;
			
			for (path = StartPath; path <= EndPath; path++) {
				for (i = 0; i < sizeof(rate); ++i) {
					if (rate[i] > MaxRate)
						break;					
				    PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);
				}
			}
			} break;
		
	case MPT_VHT:
			{
			u1Byte rate[] = {
			MGN_VHT1SS_MCS0, MGN_VHT1SS_MCS1, MGN_VHT1SS_MCS2, MGN_VHT1SS_MCS3, MGN_VHT1SS_MCS4,
			MGN_VHT1SS_MCS5, MGN_VHT1SS_MCS6, MGN_VHT1SS_MCS7, MGN_VHT1SS_MCS8, MGN_VHT1SS_MCS9,
			MGN_VHT2SS_MCS0, MGN_VHT2SS_MCS1, MGN_VHT2SS_MCS2, MGN_VHT2SS_MCS3, MGN_VHT2SS_MCS4,
			MGN_VHT2SS_MCS5, MGN_VHT2SS_MCS6, MGN_VHT2SS_MCS7, MGN_VHT2SS_MCS8, MGN_VHT2SS_MCS9,
			MGN_VHT3SS_MCS0, MGN_VHT3SS_MCS1, MGN_VHT3SS_MCS2, MGN_VHT3SS_MCS3, MGN_VHT3SS_MCS4,
			MGN_VHT3SS_MCS5, MGN_VHT3SS_MCS6, MGN_VHT3SS_MCS7, MGN_VHT3SS_MCS8, MGN_VHT3SS_MCS9,
			MGN_VHT4SS_MCS0, MGN_VHT4SS_MCS1, MGN_VHT4SS_MCS2, MGN_VHT4SS_MCS3, MGN_VHT4SS_MCS4,
			MGN_VHT4SS_MCS5, MGN_VHT4SS_MCS6, MGN_VHT4SS_MCS7, MGN_VHT4SS_MCS8, MGN_VHT4SS_MCS9,
			};
					
			if (pHalData->rf_type == RF_3T3R)
				MaxRate = MGN_VHT3SS_MCS9;
			else if (pHalData->rf_type == RF_2T2R || pHalData->rf_type == RF_2T4R)
				MaxRate = MGN_VHT2SS_MCS9;
			else
				MaxRate = MGN_VHT1SS_MCS9;

			for (path = StartPath; path <= EndPath; path++) {
				for (i = 0; i < sizeof(rate); ++i) {
					if (rate[i] > MaxRate)
						break;	
					PHY_SetTxPowerIndex(pAdapter, pTxPower[path], path, rate[i]);
				}
			}
			} break;
			
	default:
			DBG_871X("<===mpt_SetTxPower: Illegal channel!!\n");
			break;
	}

}


void hal_mpt_SetTxPower(PADAPTER pAdapter)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.MptCtx);
	PDM_ODM_T		pDM_Odm = &pHalData->odmpriv;

	if (pHalData->rf_chip < RF_TYPE_MAX) {
		if (IS_HARDWARE_TYPE_8188E(pAdapter) || 
			IS_HARDWARE_TYPE_8188F(pAdapter)) {
			u8 path = (pHalData->AntennaTxPath == ANTENNA_A) ? (ODM_RF_PATH_A) : (ODM_RF_PATH_B);

			DBG_8192C("===> MPT_ProSetTxPower: Old\n");

			RT_TRACE(_module_mp_, DBG_LOUD, ("===> MPT_ProSetTxPower[Old]:\n"));
			mpt_SetTxPower_Old(pAdapter, MPT_CCK, pMptCtx->TxPwrLevel);		
			mpt_SetTxPower_Old(pAdapter, MPT_OFDM_AND_HT, pMptCtx->TxPwrLevel);

		} else {
			DBG_871X("===> MPT_ProSetTxPower: Jaguar\n");
			mpt_SetTxPower(pAdapter, MPT_CCK, pMptCtx->TxPwrLevel);
			mpt_SetTxPower(pAdapter, MPT_OFDM, pMptCtx->TxPwrLevel);
			mpt_SetTxPower(pAdapter, MPT_HT, pMptCtx->TxPwrLevel);
			mpt_SetTxPower(pAdapter, MPT_VHT, pMptCtx->TxPwrLevel);

			}
	} else
		DBG_8192C("RFChipID < RF_TYPE_MAX, the RF chip is not supported - %d\n", pHalData->rf_chip);

	ODM_ClearTxPowerTrackingState(pDM_Odm);

}


void hal_mpt_SetDataRate(PADAPTER pAdapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.MptCtx);
	u32 DataRate;

	DataRate = MptToMgntRate(pMptCtx->MptRateIndex);
	
	hal_mpt_SwitchRfSetting(pAdapter);

	hal_mpt_CCKTxPowerAdjust(pAdapter, pHalData->bCCKinCH14);
}


#define RF_PATH_AB	22

VOID mpt_SetRFPath_819X(PADAPTER	pAdapter)
{
	HAL_DATA_TYPE			*pHalData	= GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.MptCtx);
	u4Byte			ulAntennaTx, ulAntennaRx;
	R_ANTENNA_SELECT_OFDM	*p_ofdm_tx;	/* OFDM Tx register */
	R_ANTENNA_SELECT_CCK	*p_cck_txrx;
	u1Byte		r_rx_antenna_ofdm = 0, r_ant_select_cck_val = 0;
	u1Byte		chgTx = 0, chgRx = 0;
	u4Byte		r_ant_sel_cck_val = 0, r_ant_select_ofdm_val = 0, r_ofdm_tx_en_val = 0;

	ulAntennaTx = pHalData->AntennaTxPath;
	ulAntennaRx = pHalData->AntennaRxPath;
	
	p_ofdm_tx = (R_ANTENNA_SELECT_OFDM *)&r_ant_select_ofdm_val;
	p_cck_txrx = (R_ANTENNA_SELECT_CCK *)&r_ant_select_cck_val;

	p_ofdm_tx->r_ant_ht1			= 0x1;
	p_ofdm_tx->r_ant_ht2			= 0x2;/*Second TX RF path is A*/
	p_ofdm_tx->r_ant_non_ht			= 0x3;/*/ 0x1+0x2=0x3 */

	switch (ulAntennaTx) {
	case ANTENNA_A:
			p_ofdm_tx->r_tx_antenna		= 0x1;
			r_ofdm_tx_en_val		= 0x1;
			p_ofdm_tx->r_ant_l		= 0x1;
			p_ofdm_tx->r_ant_ht_s1		= 0x1;
			p_ofdm_tx->r_ant_non_ht_s1	= 0x1;
			p_cck_txrx->r_ccktx_enable	= 0x8;
			chgTx = 1;
			/*/ From SD3 Willis suggestion !!! Set RF A=TX and B as standby*/
			/*/if (IS_HARDWARE_TYPE_8192S(pAdapter))*/
			{
				PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 2);
				PHY_SetBBReg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 1);
				r_ofdm_tx_en_val			= 0x3;
				/*/ Power save*/
				/*/cosa r_ant_select_ofdm_val = 0x11111111;*/
				/*/ We need to close RFB by SW control*/
			if (pHalData->rf_type == RF_2T2R) {
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 0);
			}
			}
			pMptCtx->MptRfPath = ODM_RF_PATH_A;
			break;
	case ANTENNA_B:
			p_ofdm_tx->r_tx_antenna		= 0x2;
			r_ofdm_tx_en_val		= 0x2;
			p_ofdm_tx->r_ant_l		= 0x2;
			p_ofdm_tx->r_ant_ht_s1		= 0x2;
			p_ofdm_tx->r_ant_non_ht_s1	= 0x2;
			p_cck_txrx->r_ccktx_enable	= 0x4;
			chgTx = 1;
			/*/ From SD3 Willis suggestion !!! Set RF A as standby*/
			/*/if (IS_HARDWARE_TYPE_8192S(pAdapter))*/
			{
				PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 2);

				/*/ 2008/10/31 MH From SD3 Willi's suggestion. We must read RF 1T table.*/
				/*/ 2009/01/08 MH From Sd3 Willis. We need to close RFA by SW control*/
			if (pHalData->rf_type == RF_2T2R || pHalData->rf_type == RF_1T2R) {
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 0);
				/*/PHY_SetBBReg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);*/
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 1);
			}
			}
			pMptCtx->MptRfPath = ODM_RF_PATH_B;		
			break;
	case ANTENNA_AB:/*/ For 8192S*/
			p_ofdm_tx->r_tx_antenna		= 0x3;
			r_ofdm_tx_en_val		= 0x3;
			p_ofdm_tx->r_ant_l		= 0x3;
			p_ofdm_tx->r_ant_ht_s1		= 0x3;
			p_ofdm_tx->r_ant_non_ht_s1	= 0x3;
			p_cck_txrx->r_ccktx_enable	= 0xC;
			chgTx = 1;
			/*/ From SD3Willis suggestion !!! Set RF B as standby*/
			/*/if (IS_HARDWARE_TYPE_8192S(pAdapter))*/
			{
			PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 2);
			PHY_SetBBReg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 2);
			/* Disable Power save*/			
			/*cosa r_ant_select_ofdm_val = 0x3321333;*/
			/* 2009/01/08 MH From Sd3 Willis. We need to enable RFA/B by SW control*/
			if (pHalData->rf_type == RF_2T2R) {
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 0);

				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 0);
				/*/PHY_SetBBReg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);*/
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 1);
			}
			}			
			pMptCtx->MptRfPath = ODM_RF_PATH_AB;
			break;
	default:
				break;
	}

	
	
/*// r_rx_antenna_ofdm, bit0=A, bit1=B, bit2=C, bit3=D
// r_cckrx_enable : CCK default, 0=A, 1=B, 2=C, 3=D
// r_cckrx_enable_2 : CCK option, 0=A, 1=B, 2=C, 3=D	*/
	switch (ulAntennaRx) {
	case ANTENNA_A:
		r_rx_antenna_ofdm		= 0x1;	/* A*/
		p_cck_txrx->r_cckrx_enable	= 0x0;	/* default: A*/
		p_cck_txrx->r_cckrx_enable_2	= 0x0;	/* option: A*/
		chgRx = 1;
		break;
	case ANTENNA_B:
		r_rx_antenna_ofdm			= 0x2;	/*/ B*/
		p_cck_txrx->r_cckrx_enable	= 0x1;	/*/ default: B*/
		p_cck_txrx->r_cckrx_enable_2	= 0x1;	/*/ option: B*/
		chgRx = 1;
		break;
	case ANTENNA_AB:/*/ For 8192S and 8192E/U...*/
		r_rx_antenna_ofdm		= 0x3;/*/ AB*/
		p_cck_txrx->r_cckrx_enable	= 0x0;/*/ default:A*/
		p_cck_txrx->r_cckrx_enable_2	= 0x1;/*/ option:B*/
		chgRx = 1;
		break;
	default:
		break;
	}


	if (chgTx && chgRx) {
		switch (pHalData->rf_chip) {
		case RF_8225:
		case RF_8256:
		case RF_6052:
				/*/r_ant_sel_cck_val = r_ant_select_cck_val;*/
				PHY_SetBBReg(pAdapter, rFPGA1_TxInfo, 0x7fffffff, r_ant_select_ofdm_val);		/*/OFDM Tx*/
				PHY_SetBBReg(pAdapter, rFPGA0_TxInfo, 0x0000000f, r_ofdm_tx_en_val);		/*/OFDM Tx*/
				PHY_SetBBReg(pAdapter, rOFDM0_TRxPathEnable, 0x0000000f, r_rx_antenna_ofdm);	/*/OFDM Rx*/
				PHY_SetBBReg(pAdapter, rOFDM1_TRxPathEnable, 0x0000000f, r_rx_antenna_ofdm);	/*/OFDM Rx*/
				PHY_SetBBReg(pAdapter, rCCK0_AFESetting, bMaskByte3, r_ant_select_cck_val);/*/r_ant_sel_cck_val); /CCK TxRx*/
				break;

		default:
				DBG_871X("Unsupported RFChipID for switching antenna.\n");
				break;
		}
	}
}	/* MPT_ProSetRFPath */


void hal_mpt_SetAntenna(PADAPTER	pAdapter)

{
	DBG_871X("Do %s\n", __func__);
	
	mpt_SetRFPath_819X(pAdapter);
	DBG_871X("mpt_SetRFPath_819X Do %s\n", __func__);

}


s32 hal_mpt_SetThermalMeter(PADAPTER pAdapter, u8 target_ther)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);

	if (!netif_running(pAdapter->pnetdev)) {
		RT_TRACE(_module_mp_, _drv_warning_, ("SetThermalMeter! Fail: interface not opened!\n"));
		return _FAIL;
	}


	if (check_fwstate(&pAdapter->mlmepriv, WIFI_MP_STATE) == _FALSE) {
		RT_TRACE(_module_mp_, _drv_warning_, ("SetThermalMeter: Fail! not in MP mode!\n"));
		return _FAIL;
	}


	target_ther &= 0xff;
	if (target_ther < 0x07)
		target_ther = 0x07;
	else if (target_ther > 0x1d)
		target_ther = 0x1d;

	pHalData->EEPROMThermalMeter = target_ther;

	return _SUCCESS;
}


void hal_mpt_TriggerRFThermalMeter(PADAPTER pAdapter)
{
	PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, 0x42, BIT17 | BIT16, 0x03);

}


u8 hal_mpt_ReadRFThermalMeter(PADAPTER pAdapter)

{
	u32 ThermalValue = 0;

	ThermalValue = (u1Byte)PHY_QueryRFReg(pAdapter, ODM_RF_PATH_A, 0x42, 0xfc00);	/*0x42: RF Reg[15:10]*/
	return (u8)ThermalValue;

}


void hal_mpt_GetThermalMeter(PADAPTER pAdapter, u8 *value)
{
	hal_mpt_TriggerRFThermalMeter(pAdapter);
	rtw_msleep_os(1000);
	*value = hal_mpt_ReadRFThermalMeter(pAdapter);
}


void hal_mpt_SetSingleCarrierTx(PADAPTER pAdapter, u8 bStart)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);
	
	pAdapter->mppriv.MptCtx.bSingleCarrier = bStart;
	
	if (bStart) {/*/ Start Single Carrier.*/
		RT_TRACE(_module_mp_, _drv_alert_, ("SetSingleCarrierTx: test start\n"));
		/*/ Start Single Carrier.*/
		/*/ 1. if OFDM block on?*/
		if (!PHY_QueryBBReg(pAdapter, rFPGA0_RFMOD, bOFDMEn))
			PHY_SetBBReg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 1); /*set OFDM block on*/

		/*/ 2. set CCK test mode off, set to CCK normal mode*/
		PHY_SetBBReg(pAdapter, rCCK0_System, bCCKBBMode, 0);

		/*/ 3. turn on scramble setting*/
		PHY_SetBBReg(pAdapter, rCCK0_System, bCCKScramble, 1);

		/*/ 4. Turn On Continue Tx and turn off the other test modes.*/
		PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_SingleCarrier);

	} else {
		/*/ Stop Single Carrier.*/
		/*/ Stop Single Carrier.*/
		/*/ Turn off all test modes.*/
		PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_ALL_OFF);

		rtw_msleep_os(10);
		/*/BB Reset*/
	    PHY_SetBBReg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
	    PHY_SetBBReg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);
	}
}


void hal_mpt_SetSingleToneTx(PADAPTER pAdapter, u8 bStart)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.MptCtx);
	u4Byte			ulAntennaTx = pHalData->AntennaTxPath;
	static u4Byte		regRF = 0, regBB0 = 0, regBB1 = 0, regBB2 = 0, regBB3 = 0;
	u8 rfPath;

	switch (ulAntennaTx) {
	case ANTENNA_B:
			rfPath = ODM_RF_PATH_B;
			break;
	case ANTENNA_C:
			rfPath = ODM_RF_PATH_C;
			break;
	case ANTENNA_D:
			rfPath = ODM_RF_PATH_D;
			break;
	case ANTENNA_A:
	default:	
			rfPath = ODM_RF_PATH_A;
			break;
	}

	pAdapter->mppriv.MptCtx.bSingleTone = bStart;
	if (bStart) {
		/*/ Start Single Tone.*/
		/*/ <20120326, Kordan> To amplify the power of tone for Xtal calibration. (asked by Edlu)*/
		if (IS_HARDWARE_TYPE_8188E(pAdapter)) {
			regRF = PHY_QueryRFReg(pAdapter, rfPath, LNA_Low_Gain_3, bRFRegOffsetMask);
			
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, LNA_Low_Gain_3, BIT1, 0x1); /*/ RF LO enabled*/	
			PHY_SetBBReg(pAdapter, rFPGA0_RFMOD, bCCKEn, 0x0);
			PHY_SetBBReg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 0x0);
		} else if (IS_HARDWARE_TYPE_8188F(pAdapter)) {
			/*Set BB REG 88C: Prevent SingleTone Fail*/
			PHY_SetBBReg(pAdapter, rFPGA0_AnalogParameter4, 0xF00000, 0xF);
			PHY_SetRFReg(pAdapter, pMptCtx->MptRfPath, LNA_Low_Gain_3, BIT1, 0x1);
			PHY_SetRFReg(pAdapter, pMptCtx->MptRfPath, RF_AC, 0xF0000, 0x2);

		}
		else	/*/ Turn On SingleTone and turn off the other test modes.*/
			PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_SingleTone);			

		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	} else {/*/ Stop Single Ton e.*/

		if (IS_HARDWARE_TYPE_8188E(pAdapter)) {
			PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, LNA_Low_Gain_3, bRFRegOffsetMask, regRF);
			PHY_SetBBReg(pAdapter, rFPGA0_RFMOD, bCCKEn, 0x1);
			PHY_SetBBReg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 0x1);
		} else if (IS_HARDWARE_TYPE_8188F(pAdapter)) {
			PHY_SetRFReg(pAdapter, pMptCtx->MptRfPath, RF_AC, 0xF0000, 0x3); /*Tx mode*/
			PHY_SetRFReg(pAdapter, pMptCtx->MptRfPath, LNA_Low_Gain_3, BIT1, 0x0); /*RF LO disabled*/
			/*Set BB REG 88C: Prevent SingleTone Fail*/
			PHY_SetBBReg(pAdapter, rFPGA0_AnalogParameter4, 0xF00000, 0xc);	
		}
		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);

	}
}


void hal_mpt_SetCarrierSuppressionTx(PADAPTER pAdapter, u8 bStart)
{
	u8 Rate;
	pAdapter->mppriv.MptCtx.bCarrierSuppression = bStart;

	Rate = HwRateToMPTRate(pAdapter->mppriv.rateidx);
	if (bStart) {/* Start Carrier Suppression.*/
		RT_TRACE(_module_mp_, _drv_alert_, ("SetCarrierSuppressionTx: test start\n"));
		if (Rate <= MPT_RATE_11M) {
			/*/ 1. if CCK block on?*/
			if (!read_bbreg(pAdapter, rFPGA0_RFMOD, bCCKEn))
				write_bbreg(pAdapter, rFPGA0_RFMOD, bCCKEn, bEnable);/*set CCK block on*/

			/*/Turn Off All Test Mode*/
			PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_ALL_OFF);

			write_bbreg(pAdapter, rCCK0_System, bCCKBBMode, 0x2);    /*/transmit mode*/
			write_bbreg(pAdapter, rCCK0_System, bCCKScramble, 0x0);  /*/turn off scramble setting*/

			/*/Set CCK Tx Test Rate*/
			write_bbreg(pAdapter, rCCK0_System, bCCKTxRate, 0x0);    /*/Set FTxRate to 1Mbps*/
		}

		 /*Set for dynamic set Power index*/
		 write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
		 write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	} else {/* Stop Carrier Suppression.*/	
		RT_TRACE(_module_mp_, _drv_alert_, ("SetCarrierSuppressionTx: test stop\n"));

		if (Rate <= MPT_RATE_11M) {
			write_bbreg(pAdapter, rCCK0_System, bCCKBBMode, 0x0);    /*normal mode*/
			write_bbreg(pAdapter, rCCK0_System, bCCKScramble, 0x1);  /*turn on scramble setting*/

			/*BB Reset*/
			write_bbreg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
			write_bbreg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);
		}
		/*Stop for dynamic set Power index*/
		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);
	}
	DBG_871X("\n MPT_ProSetCarrierSupp() is finished.\n");
}

void hal_mpt_SetCCKContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	u32 cckrate;

	if (bStart) {
		RT_TRACE(_module_mp_, _drv_alert_,
			 ("SetCCKContinuousTx: test start\n"));

		/*/ 1. if CCK block on?*/
		if (!read_bbreg(pAdapter, rFPGA0_RFMOD, bCCKEn))
			write_bbreg(pAdapter, rFPGA0_RFMOD, bCCKEn, bEnable);/*set CCK block on*/

		/*/Turn Off All Test Mode*/
		PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_ALL_OFF);

		/*/Set CCK Tx Test Rate*/

		cckrate  = pAdapter->mppriv.rateidx;

		write_bbreg(pAdapter, rCCK0_System, bCCKTxRate, cckrate);
		write_bbreg(pAdapter, rCCK0_System, bCCKBBMode, 0x2);	/*/transmit mode*/
		write_bbreg(pAdapter, rCCK0_System, bCCKScramble, bEnable);	/*/turn on scramble setting*/

		PHY_SetBBReg(pAdapter, 0xa14, 0x300, 0x3);  /* rCCK0_RxHP 0xa15[1:0] = 11 force cck rxiq = 0*/
		PHY_SetBBReg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x1);		/*/ 0xc08[16] = 1 force ofdm rxiq = ofdm txiq*/
		PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, BIT14, 1);
		PHY_SetBBReg(pAdapter, 0x0B34, BIT14, 1);

		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	} else {
		RT_TRACE(_module_mp_, _drv_info_,
			 ("SetCCKContinuousTx: test stop\n"));

		write_bbreg(pAdapter, rCCK0_System, bCCKBBMode, 0x0);	/*/normal mode*/
		write_bbreg(pAdapter, rCCK0_System, bCCKScramble, bEnable);	/*/turn on scramble setting*/

		PHY_SetBBReg(pAdapter, 0xa14, 0x300, 0x0);/* rCCK0_RxHP 0xa15[1:0] = 2b00*/
		PHY_SetBBReg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x0);		/*/ 0xc08[16] = 0*/
		
		PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, BIT14, 0);
		PHY_SetBBReg(pAdapter, 0x0B34, BIT14, 0);
		
		/*/BB Reset*/
		write_bbreg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
		write_bbreg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);

		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);
	}

	pAdapter->mppriv.MptCtx.bCckContTx = bStart;
	pAdapter->mppriv.MptCtx.bOfdmContTx = _FALSE;
}

void hal_mpt_SetOFDMContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pAdapter);

	if (bStart) {
		RT_TRACE(_module_mp_, _drv_info_, ("SetOFDMContinuousTx: test start\n"));/*/ 1. if OFDM block on?*/
		if (!PHY_QueryBBReg(pAdapter, rFPGA0_RFMOD, bOFDMEn))
			PHY_SetBBReg(pAdapter, rFPGA0_RFMOD, bOFDMEn, 1);/*/set OFDM block on*/

		/*/ 2. set CCK test mode off, set to CCK normal mode*/
		PHY_SetBBReg(pAdapter, rCCK0_System, bCCKBBMode, 0);

		/*/ 3. turn on scramble setting*/
		PHY_SetBBReg(pAdapter, rCCK0_System, bCCKScramble, 1);

		PHY_SetBBReg(pAdapter, 0xa14, 0x300, 0x3);			/* rCCK0_RxHP 0xa15[1:0] = 2b'11*/
		PHY_SetBBReg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x1);		/* 0xc08[16] = 1*/

		/*/ 4. Turn On Continue Tx and turn off the other test modes.*/
		PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_ContinuousTx);

		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000500);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000500);

	} else {
		RT_TRACE(_module_mp_, _drv_info_, ("SetOFDMContinuousTx: test stop\n"));
		PHY_SetBBReg(pAdapter, rOFDM1_LSTF, BIT30|BIT29|BIT28, OFDM_ALL_OFF);
		/*/Delay 10 ms*/
		rtw_msleep_os(10);
		
		PHY_SetBBReg(pAdapter, 0xa14, 0x300, 0x0);/*/ 0xa15[1:0] = 0*/
		PHY_SetBBReg(pAdapter, rOFDM0_TRMuxPar, 0x10000, 0x0);/*/ 0xc08[16] = 0*/
		
		/*/BB Reset*/
		PHY_SetBBReg(pAdapter, rPMAC_Reset, bBBResetB, 0x0);
		PHY_SetBBReg(pAdapter, rPMAC_Reset, bBBResetB, 0x1);

		write_bbreg(pAdapter, rFPGA0_XA_HSSIParameter1, bMaskDWord, 0x01000100);
		write_bbreg(pAdapter, rFPGA0_XB_HSSIParameter1, bMaskDWord, 0x01000100);
	}

	pAdapter->mppriv.MptCtx.bCckContTx = _FALSE;
	pAdapter->mppriv.MptCtx.bOfdmContTx = bStart;
}

void hal_mpt_SetContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	u8 Rate;
	RT_TRACE(_module_mp_, _drv_info_,
		 ("SetContinuousTx: rate:%d\n", pAdapter->mppriv.rateidx));

	Rate = HwRateToMPTRate(pAdapter->mppriv.rateidx);
	pAdapter->mppriv.MptCtx.bStartContTx = bStart;

	if (Rate <= MPT_RATE_11M)
		hal_mpt_SetCCKContinuousTx(pAdapter, bStart);
	else if (Rate >= MPT_RATE_6M) 
		hal_mpt_SetOFDMContinuousTx(pAdapter, bStart);
}

#endif /* CONFIG_MP_INCLUDE*/

