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

#define READ_AND_CONFIG_MP(ic, txt) (ODM_ReadAndConfig_MP_##ic##txt(pDM_Odm))
#define READ_AND_CONFIG_TC(ic, txt) (ODM_ReadAndConfig_TC_##ic##txt(pDM_Odm))


#if (PHYDM_TESTCHIP_SUPPORT == 1)
#define READ_AND_CONFIG(ic, txt) do {\
                                            if (pDM_Odm->bIsMPChip)\
                                    		    READ_AND_CONFIG_MP(ic,txt);\
                                            else\
                                                READ_AND_CONFIG_TC(ic,txt);\
                                    } while(0)
#else
  #define READ_AND_CONFIG     READ_AND_CONFIG_MP
#endif


#define READ_FIRMWARE_MP(ic, txt) 		(ODM_ReadFirmware_MP_##ic##txt(pDM_Odm, pFirmware, pSize))
#define READ_FIRMWARE_TC(ic, txt) 		(ODM_ReadFirmware_TC_##ic##txt(pDM_Odm, pFirmware, pSize))		

#if (PHYDM_TESTCHIP_SUPPORT == 1)
#define READ_FIRMWARE(ic, txt) do {\
						if (pDM_Odm->bIsMPChip)\
							READ_FIRMWARE_MP(ic,txt);\
						else\
							READ_FIRMWARE_TC(ic,txt);\
					} while(0) 
#else
#define READ_FIRMWARE     READ_FIRMWARE_MP
#endif
						
#define GET_VERSION_MP(ic, txt) 		(ODM_GetVersion_MP_##ic##txt())
#define GET_VERSION_TC(ic, txt) 		(ODM_GetVersion_TC_##ic##txt())
#define GET_VERSION(ic, txt) (pDM_Odm->bIsMPChip?GET_VERSION_MP(ic,txt):GET_VERSION_TC(ic,txt))

u1Byte
odm_QueryRxPwrPercentage(
	IN		s1Byte		AntPower
	)
{
	if ((AntPower <= -100) || (AntPower >= 20))
	{
		return	0;
	}
	else if (AntPower >= 0)
	{
		return	100;
	}
	else
	{
		return	(100+AntPower);
	}
	
}


//
// 2012/01/12 MH MOve some signal strength smooth method to MP HAL layer.
// IF other SW team do not support the feature, remove this section.??
//
s4Byte
odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Lenovo(	
	IN OUT PDM_ODM_T pDM_Odm,
	s4Byte CurrSig 
)
{	
	s4Byte RetSig = 0;
	return RetSig;
}

s4Byte
odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Netcore(	
	IN OUT PDM_ODM_T pDM_Odm,
	s4Byte CurrSig 
)
{
	s4Byte RetSig = 0;
	return RetSig;
}


s4Byte
odm_SignalScaleMapping_92CSeries(	
	IN OUT PDM_ODM_T pDM_Odm,
	IN s4Byte CurrSig 
)
{
	s4Byte RetSig = 0; 

	if((pDM_Odm->SupportInterface  == ODM_ITRF_USB) || (pDM_Odm->SupportInterface  == ODM_ITRF_SDIO))
	{
		if(CurrSig >= 51 && CurrSig <= 100)
		{
			RetSig = 100;
		}
		else if(CurrSig >= 41 && CurrSig <= 50)
		{
			RetSig = 80 + ((CurrSig - 40)*2);
		}
		else if(CurrSig >= 31 && CurrSig <= 40)
		{
			RetSig = 66 + (CurrSig - 30);
		}
		else if(CurrSig >= 21 && CurrSig <= 30)
		{
			RetSig = 54 + (CurrSig - 20);
		}
		else if(CurrSig >= 10 && CurrSig <= 20)
		{
			RetSig = 42 + (((CurrSig - 10) * 2) / 3);
		}
		else if(CurrSig >= 5 && CurrSig <= 9)
		{
			RetSig = 22 + (((CurrSig - 5) * 3) / 2);
		}
		else if(CurrSig >= 1 && CurrSig <= 4)
		{
			RetSig = 6 + (((CurrSig - 1) * 3) / 2);
		}
		else
		{
			RetSig = CurrSig;
		}
	}

	return RetSig;
}

s4Byte odm_SignalScaleMapping(IN OUT PDM_ODM_T pDM_Odm, IN s4Byte CurrSig) {	
	return odm_SignalScaleMapping_92CSeries(pDM_Odm,CurrSig);
}



static u1Byte odm_SQ_process_patch_RT_CID_819x_Lenovo(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte 		isCCKrate,
	IN u1Byte 		PWDB_ALL,
	IN u1Byte 		path,
	IN u1Byte 		RSSI
)
{
	u1Byte	SQ = 0;
	return SQ;
}

static u1Byte odm_SQ_process_patch_RT_CID_819x_Acer(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte 		isCCKrate,
	IN u1Byte 		PWDB_ALL,
	IN u1Byte 		path,
	IN u1Byte 		RSSI
)
{
	u1Byte	SQ = 0;
	
	return SQ;
}
			
static u1Byte 
odm_EVMdbToPercentage(
    IN		s1Byte Value
    )
{
	//
	// -33dB~0dB to 0%~99%
	//
	s1Byte ret_val;
    
	ret_val = Value;
	ret_val /= 2;

	/*DbgPrint("Value=%d\n", Value);*/
	/*ODM_RT_DISP(FRX, RX_PHY_SQ, ("EVMdbToPercentage92C Value=%d / %x\n", ret_val, ret_val));*/
	if (ret_val >= 0)
		ret_val = 0;

	if (ret_val <= -33)
		ret_val = -33;

	ret_val = 0 - ret_val;
	ret_val *= 3;

	if (ret_val == 99)
		ret_val = 100;
	return (u1Byte)ret_val;
}
			
static u1Byte 
odm_EVMdbm_JaguarSeries(
	IN  s1Byte Value
	)
{
	s1Byte ret_val = Value;
	
	// -33dB~0dB to 33dB ~ 0dB
	if(ret_val == -128)
		ret_val = 127;
	else if (ret_val < 0)
		ret_val = 0 - ret_val;
	
	ret_val  = ret_val >> 1;
	return (u1Byte)ret_val;
}

static s2Byte
odm_Cfo(
  IN s1Byte Value
)
{
	s2Byte  ret_val;

	if (Value < 0)
	{
		ret_val = 0 - Value;
		ret_val = (ret_val << 1) + (ret_val >> 1) ;  //  *2.5~=312.5/2^7
		ret_val = ret_val | BIT12;  // set bit12 as 1 for negative cfo
	}
	else
	{
		ret_val = Value;
		ret_val = (ret_val << 1) + (ret_val>>1) ;  //  *2.5~=312.5/2^7
	}
	return ret_val;
}

#if(ODM_IC_11N_SERIES_SUPPORT == 1)

s1Byte
odm_CCKRSSI_8703B(
	IN		u2Byte	LNA_idx,
	IN		u1Byte	VGA_idx
	)
{
	s1Byte	rx_pwr_all = 0x00;
	
	switch (LNA_idx) {
	case 0xf:
		rx_pwr_all = -48 - (2 * VGA_idx);
		break;		
	case 0xb:
		rx_pwr_all = -42 - (2 * VGA_idx); /*TBD*/
		break;
	case 0xa:
		rx_pwr_all = -36 - (2 * VGA_idx);
		break;
	case 8:
		rx_pwr_all = -32 - (2 * VGA_idx);
		break;
	case 7:	
		rx_pwr_all = -28 - (2 * VGA_idx); /*TBD*/
		break;
	case 4:	
		rx_pwr_all = -16 - (2 * VGA_idx);
		break;
	case 0:	
		rx_pwr_all = -2 - (2 * VGA_idx);
		break;
	default:
	/*rx_pwr_all = -53+(2*(31-VGA_idx));*/
	/*DbgPrint("wrong LNA index\n");*/
		break;
			
	}
	return	rx_pwr_all;
}

VOID
odm_RxPhyStatus92CSeries_Parsing(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,		
	IN 		pu1Byte						pPhyStatus,
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{							
	SWAT_T				*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u1Byte				i, Max_spatial_stream;
	s1Byte				rx_pwr[4], rx_pwr_all=0;
	u1Byte				EVM, PWDB_ALL = 0, PWDB_ALL_BT;
	u1Byte				RSSI, total_rssi=0;
	BOOLEAN				isCCKrate=FALSE;	
	u1Byte				rf_rx_num = 0;
	u1Byte				cck_highpwr = 0;
	u1Byte				LNA_idx = 0;
	u1Byte				VGA_idx = 0;
	PPHY_STATUS_RPT_8192CD_T pPhyStaRpt = (PPHY_STATUS_RPT_8192CD_T)pPhyStatus;

	isCCKrate = (pPktinfo->DataRate <= ODM_RATE11M) ? TRUE : FALSE;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = -1;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;


	if(isCCKrate)
	{
		u1Byte report;
		u1Byte cck_agc_rpt;
		
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusCCK++;
		// 
		// (1)Hardware does not provide RSSI for CCK
		// (2)PWDB, Average PWDB cacluated by hardware (for rate adaptive)
		//

		//if(pHalData->eRFPowerState == eRfOn)
			cck_highpwr = pDM_Odm->bCckHighPower;
		//else
		//	cck_highpwr = FALSE;

		cck_agc_rpt =  pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a ;
		
		//2011.11.28 LukeLee: 88E use different LNA & VGA gain table
		//The RSSI formula should be modified according to the gain table
		//In 88E, cck_highpwr is always set to 1
		if (pDM_Odm->SupportICType & (ODM_RTL8188E | ODM_RTL8192E | ODM_RTL8723B | ODM_RTL8188F)) /*3 bit LNA*/
		{
			LNA_idx = ((cck_agc_rpt & 0xE0) >>5);
			VGA_idx = (cck_agc_rpt & 0x1F); 
			if(pDM_Odm->SupportICType & (ODM_RTL8188E|ODM_RTL8192E))
			{
				if(pDM_Odm->cck_agc_report_type == 0 && (pDM_Odm->SupportICType & ODM_RTL8192E) )
				{
					switch(LNA_idx)
					{
						case 7:
							rx_pwr_all = -45  - 2*(VGA_idx);
							break;
						case 6:
							rx_pwr_all = -43 -2*(VGA_idx); 
							break;
						case 5:
							rx_pwr_all = -27 - 2*(VGA_idx); 
							break;
						case 4:
							rx_pwr_all = -21 - 2*(VGA_idx); 
							break;
						case 3:
							rx_pwr_all = -18 - 2*(VGA_idx); 
							break;
						case 2:
							rx_pwr_all = -6 - 2*(VGA_idx);
							break;
						case 1:
							rx_pwr_all = 9 -2*(VGA_idx);
							break;
						case 0:
							rx_pwr_all = 15 -2*(VGA_idx);
							break;
						default:

							break;
					}

					if(pDM_Odm->BoardType & ODM_BOARD_EXT_LNA)
					{
						rx_pwr_all -= pDM_Odm->ExtLNAGain;
					}
					
					PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
				}
				else
				{					
					switch(LNA_idx)
					{
						case 7:
							if(VGA_idx <= 27)
								rx_pwr_all = -100 + 2*(27-VGA_idx); //VGA_idx = 27~2
							else
							rx_pwr_all = -100;
							break;
						case 6:
							rx_pwr_all = -48 + 2*(2-VGA_idx); //VGA_idx = 2~0
							break;
						case 5:
							rx_pwr_all = -42 + 2*(7-VGA_idx); //VGA_idx = 7~5
							break;
						case 4:
							rx_pwr_all = -36 + 2*(7-VGA_idx); //VGA_idx = 7~4
							break;
						case 3:
							//rx_pwr_all = -28 + 2*(7-VGA_idx); //VGA_idx = 7~0
							rx_pwr_all = -24 + 2*(7-VGA_idx); //VGA_idx = 7~0
							break;
						case 2:
							if(cck_highpwr)
								rx_pwr_all = -12 + 2*(5-VGA_idx); //VGA_idx = 5~0
							else
								rx_pwr_all = -6+ 2*(5-VGA_idx);
							break;
						case 1:
								rx_pwr_all = 8-2*VGA_idx;
							break;
						case 0:
							rx_pwr_all = 14-2*VGA_idx;
							break;
						default:
							//DbgPrint("CCK Exception default\n");
							break;
					}
					rx_pwr_all += 8;

					//2012.10.08 LukeLee: Modify for 92E CCK RSSI
					if(pDM_Odm->SupportICType == ODM_RTL8192E)
						rx_pwr_all += 8;
					
					PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
					if(cck_highpwr == FALSE)
					{
						if(PWDB_ALL >= 80)
							PWDB_ALL = ((PWDB_ALL-80)<<1)+((PWDB_ALL-80)>>1)+80;
						else if((PWDB_ALL <= 78) && (PWDB_ALL >= 20))
							PWDB_ALL += 3;
						if(PWDB_ALL>100)
							PWDB_ALL = 100;
					}
				}
			}
			else if (pDM_Odm->SupportICType & (ODM_RTL8188F)) {
#if (RTL8188F_SUPPORT == 1)
				rx_pwr_all = odm_CCKRSSI_8188F(LNA_idx, VGA_idx);
				PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
				if (PWDB_ALL > 100)
					PWDB_ALL = 100;
#endif
			}
		}
		else
		{
			if(!cck_highpwr)
			{			
				report =( cck_agc_rpt & 0xc0 )>>6;
				switch(report)
				{
					// 03312009 modified by cosa
					// Modify the RF RNA gain value to -40, -20, -2, 14 by Jenyu's suggestion
					// Note: different RF with the different RNA gain.
					case 0x3:
						rx_pwr_all = -46 - (cck_agc_rpt & 0x3e);
						break;
					case 0x2:
						rx_pwr_all = -26 - (cck_agc_rpt & 0x3e);
						break;
					case 0x1:
						rx_pwr_all = -12 - (cck_agc_rpt & 0x3e);
						break;
					case 0x0:
						rx_pwr_all = 16 - (cck_agc_rpt & 0x3e);
						break;
				}
			}
			else
			{
				//report = pDrvInfo->cfosho[0] & 0x60;			
				//report = pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a& 0x60;
				
				report = (cck_agc_rpt & 0x60)>>5;
				switch(report)
				{
					case 0x3:
						rx_pwr_all = -46 - ((cck_agc_rpt & 0x1f)<<1) ;
						break;
					case 0x2:
						rx_pwr_all = -26 - ((cck_agc_rpt & 0x1f)<<1);
						break;
					case 0x1:
						rx_pwr_all = -12 - ((cck_agc_rpt & 0x1f)<<1) ;
						break;
					case 0x0:
						rx_pwr_all = 16 - ((cck_agc_rpt & 0x1f)<<1) ;
						break;
				}
			}

			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);

			//Modification for ext-LNA board
			if(pDM_Odm->BoardType & (ODM_BOARD_EXT_LNA | ODM_BOARD_EXT_PA))
			{
				if((cck_agc_rpt>>7) == 0){
					PWDB_ALL = (PWDB_ALL>94)?100:(PWDB_ALL +6);
				}
				else	
	                   {
					if(PWDB_ALL > 38)
						PWDB_ALL -= 16;
					else
						PWDB_ALL = (PWDB_ALL<=16)?(PWDB_ALL>>2):(PWDB_ALL -12);
				}             

				//CCK modification
				if(PWDB_ALL > 25 && PWDB_ALL <= 60)
					PWDB_ALL += 6;
				//else if (PWDB_ALL <= 25)
				//	PWDB_ALL += 8;
			}
			else//Modification for int-LNA board
			{
				if(PWDB_ALL > 99)
				  	PWDB_ALL -= 8;
				else if(PWDB_ALL > 50 && PWDB_ALL <= 68)
					PWDB_ALL += 4;
			}
		}

		pDM_Odm->cck_lna_idx = LNA_idx;
		pDM_Odm->cck_vga_idx = VGA_idx;
		pPhyInfo->RxPWDBAll = PWDB_ALL;
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
#endif		
		//
		// (3) Get Signal Quality (EVM)
		//
		//if(pPktinfo->bPacketMatchBSSID)
		{
			u1Byte	SQ,SQ_rpt;			
			
			if(pPhyInfo->RxPWDBAll > 40 && !pDM_Odm->bInHctTest){
				SQ = 100;
			}
			else{						
				SQ_rpt = pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all;
					
				if(SQ_rpt > 64)
					SQ = 0;
				else if (SQ_rpt < 20)
					SQ = 100;
				else
					SQ = ((64-SQ_rpt) * 100) / 44;
			
			}
			
			//DbgPrint("cck SQ = %d\n", SQ);
			pPhyInfo->SignalQuality = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;
		}

		for (i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++) {
			if (i == 0)
				pPhyInfo->RxMIMOSignalStrength[0] = PWDB_ALL;
			else
				pPhyInfo->RxMIMOSignalStrength[1] = 0;
		}
	}
	else //2 is OFDM rate
	{
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusOFDM++;

		// 
		// (1)Get RSSI for HT rate
		//
		
       	 for(i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++)   
		{
			// 2008/01/30 MH we will judge RF RX path now.
			if (pDM_Odm->RFPathRxEnable & BIT(i))
				rf_rx_num++;
			//else
				//continue;

			rx_pwr[i] = ((pPhyStaRpt->path_agc[i].gain& 0x3F)*2) - 110;
			pDM_Odm->ofdm_agc_idx[i] = (pPhyStaRpt->path_agc[i].gain & 0x3F);

		#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
			pPhyInfo->RxPwr[i] = rx_pwr[i];
		#endif	

			/* Translate DBM to percentage. */
			RSSI = odm_QueryRxPwrPercentage(rx_pwr[i]);
			total_rssi += RSSI;
			//RT_DISP(FRX, RX_PHY_SS, ("RF-%d RXPWR=%x RSSI=%d\n", i, rx_pwr[i], RSSI));


			if(pDM_Odm->SupportICType&ODM_RTL8192C)
			{	
			        //Modification for ext-LNA board	
				if(pDM_Odm->BoardType & (ODM_BOARD_EXT_LNA | ODM_BOARD_EXT_PA))
				{
					if((pPhyStaRpt->path_agc[i].trsw) == 1)
						RSSI = (RSSI>94)?100:(RSSI +6);
					else
						RSSI = (RSSI<=16)?(RSSI>>3):(RSSI -16);

					if((RSSI <= 34) && (RSSI >=4))
						RSSI -= 4;
				}		
			}
		
			pPhyInfo->RxMIMOSignalStrength[i] =(u1Byte) RSSI;

		#if (DM_ODM_SUPPORT_TYPE &  (/*ODM_WIN|*/ODM_CE|ODM_AP))
			//Get Rx snr value in DB		
			pPhyInfo->RxSNR[i] = pDM_Odm->PhyDbgInfo.RxSNRdB[i] = (s4Byte)(pPhyStaRpt->path_rxsnr[i]/2);
		#endif
		}
		
		
		//
		// (2)PWDB, Average PWDB cacluated by hardware (for rate adaptive)
		//
		rx_pwr_all = (((pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all) >> 1 )& 0x7f) -110;		
		
		PWDB_ALL_BT = PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);	
	
	
		pPhyInfo->RxPWDBAll = PWDB_ALL;
		//ODM_RT_TRACE(pDM_Odm,ODM_COMP_RSSI_MONITOR, ODM_DBG_LOUD, ("ODM OFDM RSSI=%d\n",pPhyInfo->RxPWDBAll));
	#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL_BT;
		pPhyInfo->RxPower = rx_pwr_all;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
	#endif
		
		if((pDM_Odm->SupportPlatform == ODM_WIN) &&(pDM_Odm->PatchID==19)){
			//do nothing	
		}else if((pDM_Odm->SupportPlatform == ODM_WIN) &&(pDM_Odm->PatchID==25)){
			//do nothing	
		}
		else{//pMgntInfo->CustomerID != RT_CID_819x_Lenovo
			//
			// (3)EVM of HT rate
			//
			if(pPktinfo->DataRate >=ODM_RATEMCS8 && pPktinfo->DataRate <=ODM_RATEMCS15)
				Max_spatial_stream = 2; //both spatial stream make sense
			else
				Max_spatial_stream = 1; //only spatial stream 1 makes sense

			for(i=0; i<Max_spatial_stream; i++)
			{
				// Do not use shift operation like "rx_evmX >>= 1" because the compilor of free build environment
				// fill most significant bit to "zero" when doing shifting operation which may change a negative 
				// value to positive one, then the dbm value (which is supposed to be negative)  is not correct anymore.			
				EVM = odm_EVMdbToPercentage( (pPhyStaRpt->stream_rxevm[i] ));	//dbm

				//GET_RX_STATUS_DESC_RX_MCS(pDesc), pDrvInfo->rxevm[i], "%", EVM));
				
				//if(pPktinfo->bPacketMatchBSSID)
				{
					if(i==ODM_RF_PATH_A) // Fill value in RFD, Get the first spatial stream only
					{						
						pPhyInfo->SignalQuality = (u1Byte)(EVM & 0xff);
					}					
					pPhyInfo->RxMIMOSignalQuality[i] = (u1Byte)(EVM & 0xff);
				}
			}
		}

		ODM_ParsingCFO(pDM_Odm, pPktinfo, pPhyStaRpt->path_cfotail);
		
	}
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
	//UI BSS List signal strength(in percentage), make it good looking, from 0~100.
	//It is assigned to the BSS List in GetValueFromBeaconOrProbeRsp().
	if(isCCKrate)
	{		
		pPhyInfo->SignalStrength = (u1Byte)PWDB_ALL;
	} else {	
		if (rf_rx_num != 0) {
			total_rssi/=rf_rx_num;
			pPhyInfo->SignalStrength = (u1Byte)total_rssi;
		}
	}
#endif /*#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))*/

	//DbgPrint("isCCKrate = %d, pPhyInfo->RxPWDBAll = %d, pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a = 0x%x\n", 
		//isCCKrate, pPhyInfo->RxPWDBAll, pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a);

	//For 92C/92D HW (Hybrid) Antenna Diversity
}
#endif

#if	ODM_IC_11AC_SERIES_SUPPORT

VOID
odm_RxPhyBWJaguarSeries_Parsing(
	OUT		PODM_PHY_INFO_T			pPhyInfo,
	IN		PODM_PACKET_INFO_T			pPktinfo,
	IN		PPHY_STATUS_RPT_8812_T		pPhyStaRpt
)
{

	if(pPktinfo->DataRate <= ODM_RATE54M) {
		switch (pPhyStaRpt->r_RFMOD) {
		case 1:
			if (pPhyStaRpt->sub_chnl == 0)
				pPhyInfo->BandWidth = 1;
			else
				pPhyInfo->BandWidth = 0;
			break;

		case 2:
			if (pPhyStaRpt->sub_chnl == 0)
				pPhyInfo->BandWidth = 2;
			else if (pPhyStaRpt->sub_chnl == 9 || pPhyStaRpt->sub_chnl == 10)
				pPhyInfo->BandWidth = 1;
			else
				pPhyInfo->BandWidth = 0;
			break;

		default:
		case 0:
			pPhyInfo->BandWidth = 0;
			break;
		}
	}

}

VOID
odm_RxPhyStatusJaguarSeries_Parsing(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,
	IN 		pu1Byte						pPhyStatus,
	IN		PODM_PACKET_INFO_T			pPktinfo
)
{
	u1Byte					i, Max_spatial_stream;
	s1Byte					rx_pwr[4], rx_pwr_all = 0;
	u1Byte					EVM, EVMdbm, PWDB_ALL = 0, PWDB_ALL_BT;
	u1Byte					RSSI, avg_rssi = 0, best_rssi = 0, second_rssi = 0;
	u1Byte					isCCKrate = 0;	
	u1Byte					rf_rx_num = 0;
	u1Byte					cck_highpwr = 0;
	u1Byte					LNA_idx, VGA_idx;
	PPHY_STATUS_RPT_8812_T pPhyStaRpt = (PPHY_STATUS_RPT_8812_T)pPhyStatus;
	pFAT_T					pDM_FatTable = &pDM_Odm->DM_FatTable;

	odm_RxPhyBWJaguarSeries_Parsing(pPhyInfo, pPktinfo, pPhyStaRpt);

	if (pPktinfo->DataRate <= ODM_RATE11M)
		isCCKrate = TRUE;
	else
		isCCKrate = FALSE;

	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = -1;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_C] = -1;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_D] = -1;

	if (isCCKrate) {
		u1Byte cck_agc_rpt;
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusCCK++;

		/*(1)Hardware does not provide RSSI for CCK*/
		/*(2)PWDB, Average PWDB calculated by hardware (for rate adaptive)*/

		/*if(pHalData->eRFPowerState == eRfOn)*/
		cck_highpwr = pDM_Odm->bCckHighPower;
		/*else*/
		/*cck_highpwr = FALSE;*/

		cck_agc_rpt =  pPhyStaRpt->cfosho[0] ;
		LNA_idx = ((cck_agc_rpt & 0xE0) >> 5);
		VGA_idx = (cck_agc_rpt & 0x1F);

		if (pDM_Odm->SupportICType == ODM_RTL8812) {
			switch (LNA_idx) {
			case 7:
				if (VGA_idx <= 27)
					rx_pwr_all = -100 + 2 * (27 - VGA_idx); /*VGA_idx = 27~2*/
				else
					rx_pwr_all = -100;
				break;
			case 6:
				rx_pwr_all = -48 + 2 * (2 - VGA_idx); /*VGA_idx = 2~0*/
				break;
			case 5:
				rx_pwr_all = -42 + 2 * (7 - VGA_idx); /*VGA_idx = 7~5*/
				break;
			case 4:
				rx_pwr_all = -36 + 2 * (7 - VGA_idx); /*VGA_idx = 7~4*/
				break;
			case 3:
				/*rx_pwr_all = -28 + 2*(7-VGA_idx); VGA_idx = 7~0*/
				rx_pwr_all = -24 + 2 * (7 - VGA_idx); /*VGA_idx = 7~0*/
				break;
			case 2:
				if (cck_highpwr)
					rx_pwr_all = -12 + 2 * (5 - VGA_idx); /*VGA_idx = 5~0*/
				else
					rx_pwr_all = -6 + 2 * (5 - VGA_idx);
				break;
			case 1:
				rx_pwr_all = 8 - 2 * VGA_idx;
				break;
			case 0:
				rx_pwr_all = 14 - 2 * VGA_idx;
				break;
			default:
				/*DbgPrint("CCK Exception default\n");*/
				break;
			}
			rx_pwr_all += 6;
			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);

			if (cck_highpwr == FALSE) {
				if (PWDB_ALL >= 80)
					PWDB_ALL = ((PWDB_ALL - 80) << 1) + ((PWDB_ALL - 80) >> 1) + 80;
				else if ((PWDB_ALL <= 78) && (PWDB_ALL >= 20))
					PWDB_ALL += 3;
				if (PWDB_ALL > 100)
					PWDB_ALL = 100;
			}
		} else if (pDM_Odm->SupportICType & (ODM_RTL8821 | ODM_RTL8881A)) {
			s1Byte Pout = -6;

			switch (LNA_idx) {
			case 5:
				rx_pwr_all = Pout - 32 - (2 * VGA_idx);
				break;
			case 4:
				rx_pwr_all = Pout - 24 - (2 * VGA_idx);
				break;
			case 2:
				rx_pwr_all = Pout - 11 - (2 * VGA_idx);
				break;
			case 1:
				rx_pwr_all = Pout + 5 - (2 * VGA_idx);
				break;
			case 0:
				rx_pwr_all = Pout + 21 - (2 * VGA_idx);
				break;
			}
			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
		} else if (pDM_Odm->SupportICType == ODM_RTL8814A || pDM_Odm->SupportICType == ODM_RTL8822B) {
			s1Byte Pout = -6;

			switch (LNA_idx) {
			/*CCK only use LNA: 2, 3, 5, 7*/
			case 7:
				rx_pwr_all = Pout - 32 - (2 * VGA_idx);
				break;
			case 5:
				rx_pwr_all = Pout - 22 - (2 * VGA_idx);
				break;
			case 3:
				rx_pwr_all = Pout - 2 - (2 * VGA_idx);
				break;
			case 2:
				rx_pwr_all = Pout + 5 - (2 * VGA_idx);
				break;
			/*case 6:*/
			/*rx_pwr_all = Pout -26 - (2*VGA_idx);*/
			/*break;*/
			/*case 4:*/
			/*rx_pwr_all = Pout - 8 - (2*VGA_idx);*/
			/*break;*/
			/*case 1:*/
			/*rx_pwr_all = Pout + 21 - (2*VGA_idx);*/
			/*break;*/
			/*case 0:*/
			/*rx_pwr_all = Pout + 10 - (2*VGA_idx);*/
/*			//	break;*/
			default:
/*				//DbgPrint("CCK Exception default\n");*/
				break;
			}
			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
		}

		pPhyInfo->RxPWDBAll = PWDB_ALL;
/*		//if(pPktinfo->StationID == 0)*/
/*		//{*/
/*		//	DbgPrint("CCK: LNA_idx = %d, VGA_idx = %d, pPhyInfo->RxPWDBAll = %d\n",*/
/*		//		LNA_idx, VGA_idx, pPhyInfo->RxPWDBAll);*/
/*		//}*/
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
#endif
		/*(3) Get Signal Quality (EVM)*/
		if (pPktinfo->bPacketMatchBSSID) {
			u1Byte	SQ, SQ_rpt;

			if ((pDM_Odm->SupportPlatform == ODM_WIN) &&
				(pDM_Odm->PatchID == RT_CID_819x_Lenovo)) {
				SQ = odm_SQ_process_patch_RT_CID_819x_Lenovo(pDM_Odm, isCCKrate, PWDB_ALL, 0, 0);
			} else if (pPhyInfo->RxPWDBAll > 40 && !pDM_Odm->bInHctTest) {
				SQ = 100;
			} else {
				SQ_rpt = pPhyStaRpt->pwdb_all;

				if (SQ_rpt > 64)
					SQ = 0;
				else if (SQ_rpt < 20)
					SQ = 100;
				else
					SQ = ((64 - SQ_rpt) * 100) / 44;
			}

/*			//DbgPrint("cck SQ = %d\n", SQ);*/
			pPhyInfo->SignalQuality = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = SQ;
		}

		for (i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX_JAGUAR; i++) {
			if (i == 0)
				pPhyInfo->RxMIMOSignalStrength[0] = PWDB_ALL;
			else
				pPhyInfo->RxMIMOSignalStrength[i] = 0;
		}
	} else {		 
		/*is OFDM rate*/
		pDM_FatTable->hw_antsw_occur = pPhyStaRpt->hw_antsw_occur;
		
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusOFDM++;

		/*(1)Get RSSI for OFDM rate*/

		for (i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX_JAGUAR; i++) {
			/*2008/01/30 MH we will judge RF RX path now.*/
/*			//DbgPrint("pDM_Odm->RFPathRxEnable = %x\n", pDM_Odm->RFPathRxEnable);*/
			if (pDM_Odm->RFPathRxEnable & BIT(i))
				rf_rx_num++;
/*			//else*/
/*			//continue;*/
			/*2012.05.25 LukeLee: Testchip AGC report is wrong, it should be restored back to old formula in MP chip*/
/*			//if((pDM_Odm->SupportICType & (ODM_RTL8812|ODM_RTL8821)) && (!pDM_Odm->bIsMPChip))*/
			if (i < ODM_RF_PATH_C)
				rx_pwr[i] = (pPhyStaRpt->gain_trsw[i] & 0x7F) - 110;
			else
				rx_pwr[i] = (pPhyStaRpt->gain_trsw_cd[i - 2] & 0x7F) - 110;
/*			//else*/
			/*rx_pwr[i] = ((pPhyStaRpt->gain_trsw[i]& 0x3F)*2) - 110;  OLD FORMULA*/

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
			pPhyInfo->RxPwr[i] = rx_pwr[i];
#endif

			/* Translate DBM to percentage. */
			RSSI = odm_QueryRxPwrPercentage(rx_pwr[i]);	
		
			/*total_rssi += RSSI;*/
			/*Get the best two RSSI*/
			if (RSSI > best_rssi && RSSI > second_rssi) {
				second_rssi = best_rssi;
				best_rssi = RSSI;
			} else if (RSSI > second_rssi && RSSI <= best_rssi)
				second_rssi = RSSI;

			/*RT_DISP(FRX, RX_PHY_SS, ("RF-%d RXPWR=%x RSSI=%d\n", i, rx_pwr[i], RSSI));*/

			pPhyInfo->RxMIMOSignalStrength[i] = (u1Byte) RSSI;


			/*Get Rx snr value in DB*/
			if (i < ODM_RF_PATH_C)
				pPhyInfo->RxSNR[i] = pDM_Odm->PhyDbgInfo.RxSNRdB[i] = pPhyStaRpt->rxsnr[i] / 2;
			else if (pDM_Odm->SupportICType & (ODM_RTL8814A | ODM_RTL8822B))
				pPhyInfo->RxSNR[i] = pDM_Odm->PhyDbgInfo.RxSNRdB[i] = pPhyStaRpt->csi_current[i - 2] / 2;

#if (DM_ODM_SUPPORT_TYPE != ODM_AP)
			/*(2) CFO_short  & CFO_tail*/
			if (i < ODM_RF_PATH_C) {
				pPhyInfo->Cfo_short[i] = odm_Cfo((pPhyStaRpt->cfosho[i]));
				pPhyInfo->Cfo_tail[i] = odm_Cfo((pPhyStaRpt->cfotail[i]));
			}
#endif
		}

		/*(3)PWDB, Average PWDB calculated by hardware (for rate adaptive)*/

		/*2012.05.25 LukeLee: Testchip AGC report is wrong, it should be restored back to old formula in MP chip*/
		if ((pDM_Odm->SupportICType & (ODM_RTL8812 | ODM_RTL8821 | ODM_RTL8881A)) && (!pDM_Odm->bIsMPChip))
			rx_pwr_all = (pPhyStaRpt->pwdb_all & 0x7f) - 110;
		else
			rx_pwr_all = (((pPhyStaRpt->pwdb_all) >> 1) & 0x7f) - 110;	 /*OLD FORMULA*/

		PWDB_ALL_BT = PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);

		pPhyInfo->RxPWDBAll = PWDB_ALL;
		/*ODM_RT_TRACE(pDM_Odm,ODM_COMP_RSSI_MONITOR, ODM_DBG_LOUD, ("ODM OFDM RSSI=%d\n",pPhyInfo->RxPWDBAll));*/
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL_BT;
		pPhyInfo->RxPower = rx_pwr_all;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
#endif

		if ((pDM_Odm->SupportPlatform == ODM_WIN) && (pDM_Odm->PatchID == 19)) {
			/*do nothing*/
		} else {
			/*pMgntInfo->CustomerID != RT_CID_819x_Lenovo*/

			/*(4)EVM of OFDM rate*/
			
			if ((pPktinfo->DataRate >= ODM_RATEMCS8) &&
				(pPktinfo->DataRate <= ODM_RATEMCS15))
				Max_spatial_stream = 2;
			else if ((pPktinfo->DataRate >= ODM_RATEVHTSS2MCS0) &&
					 (pPktinfo->DataRate <= ODM_RATEVHTSS2MCS9))
				Max_spatial_stream = 2;
			else if ((pPktinfo->DataRate >= ODM_RATEMCS16) &&
					 (pPktinfo->DataRate <= ODM_RATEMCS23))
				Max_spatial_stream = 3;
			else if ((pPktinfo->DataRate >= ODM_RATEVHTSS3MCS0) &&
					 (pPktinfo->DataRate <= ODM_RATEVHTSS3MCS9))
				Max_spatial_stream = 3;
			else
				Max_spatial_stream = 1;

			if (pPktinfo->bPacketMatchBSSID) {
				/*DbgPrint("pPktinfo->DataRate = %d\n", pPktinfo->DataRate);*/

				for (i = 0; i < Max_spatial_stream; i++) {
					/*Do not use shift operation like "rx_evmX >>= 1" because the compilor of free build environment*/
					/*fill most significant bit to "zero" when doing shifting operation which may change a negative*/
					/*value to positive one, then the dbm value (which is supposed to be negative)  is not correct anymore.*/

					if (pPktinfo->DataRate >= ODM_RATE6M && pPktinfo->DataRate <= ODM_RATE54M) {
						if (i == ODM_RF_PATH_A) {
							EVM = odm_EVMdbToPercentage((pPhyStaRpt->sigevm));	/*dbm*/
							EVM += 20;
							if (EVM > 100)
								EVM = 100;
						}
					} else {
						if (i < ODM_RF_PATH_C) {
							if (pPhyStaRpt->rxevm[i] == -128)
								pPhyStaRpt->rxevm[i] = -25;
							EVM = odm_EVMdbToPercentage((pPhyStaRpt->rxevm[i]));	/*dbm*/
						} else {
							if (pPhyStaRpt->rxevm_cd[i - 2] == -128){
								pPhyStaRpt->rxevm_cd[i - 2] = -25;
							}
							EVM = odm_EVMdbToPercentage((pPhyStaRpt->rxevm_cd[i - 2]));	/*dbm*/
						}
					}

					if (i < ODM_RF_PATH_C)
						EVMdbm = odm_EVMdbm_JaguarSeries(pPhyStaRpt->rxevm[i]);
					else
						EVMdbm = odm_EVMdbm_JaguarSeries(pPhyStaRpt->rxevm_cd[i - 2]);
					/*RT_DISP(FRX, RX_PHY_SQ, ("RXRATE=%x RXEVM=%x EVM=%s%d\n",*/
					/*pPktinfo->DataRate, pPhyStaRpt->rxevm[i], "%", EVM));*/

					{
						if (i == ODM_RF_PATH_A) { 
							/*Fill value in RFD, Get the first spatial stream only*/
							pPhyInfo->SignalQuality = EVM;
						}
						pPhyInfo->RxMIMOSignalQuality[i] = EVM;
#if (DM_ODM_SUPPORT_TYPE != ODM_AP)
						pPhyInfo->RxMIMOEVMdbm[i] = EVMdbm;
#endif
					}
				}
			}
		}

		ODM_ParsingCFO(pDM_Odm, pPktinfo, pPhyStaRpt->cfotail);

	}
/*	//DbgPrint("isCCKrate= %d, pPhyInfo->SignalStrength=%d % PWDB_AL=%d rf_rx_num=%d\n", isCCKrate, pPhyInfo->SignalStrength, PWDB_ALL, rf_rx_num);*/

	/*UI BSS List signal strength(in percentage), make it good looking, from 0~100.*/
	/*It is assigned to the BSS List in GetValueFromBeaconOrProbeRsp().*/
	if (isCCKrate) {
		pPhyInfo->SignalStrength = (u1Byte)(odm_SignalScaleMapping(pDM_Odm, PWDB_ALL));/*PWDB_ALL;*/
	} else {	
		if (rf_rx_num != 0) {
			/* 2015/01 Sean, use the best two RSSI only, suggested by Ynlin and ChenYu.*/
			if (rf_rx_num == 1)
				avg_rssi = best_rssi;
			else
				avg_rssi = (best_rssi + second_rssi)/2;
			pPhyInfo->SignalStrength = (u1Byte)(odm_SignalScaleMapping(pDM_Odm, avg_rssi));
		}
	}
	pDM_Odm->RxPWDBAve = pDM_Odm->RxPWDBAve + pPhyInfo->RxPWDBAll;

	pDM_Odm->DM_FatTable.antsel_rx_keep_0 = pPhyStaRpt->antidx_anta;
	pDM_Odm->DM_FatTable.antsel_rx_keep_1 = pPhyStaRpt->antidx_antb;
	pDM_Odm->DM_FatTable.antsel_rx_keep_2 = pPhyStaRpt->antidx_antc;
	pDM_Odm->DM_FatTable.antsel_rx_keep_3 = pPhyStaRpt->antidx_antd;
	/*ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("StaID[%d]:  antidx_anta = ((%d)), MatchBSSID =  ((%d))\n", pPktinfo->StationID, pPhyStaRpt->antidx_anta, pPktinfo->bPacketMatchBSSID));*/


/*		DbgPrint("pPhyStaRpt->antidx_anta = %d, pPhyStaRpt->antidx_antb = %d\n",*/
/*			pPhyStaRpt->antidx_anta, pPhyStaRpt->antidx_antb);*/
/*		DbgPrint("----------------------------\n");*/
/*		DbgPrint("pPktinfo->StationID=%d, pPktinfo->DataRate=0x%x\n",pPktinfo->StationID, pPktinfo->DataRate);*/
/*		DbgPrint("pPhyStaRpt->r_RFMOD = %d\n", pPhyStaRpt->r_RFMOD);*/
/*		DbgPrint("pPhyStaRpt->gain_trsw[0]=0x%x, pPhyStaRpt->gain_trsw[1]=0x%x\n",*/
/*				pPhyStaRpt->gain_trsw[0],pPhyStaRpt->gain_trsw[1]);*/
/*		DbgPrint("pPhyStaRpt->gain_trsw[2]=0x%x, pPhyStaRpt->gain_trsw[3]=0x%x\n",*/
/*				pPhyStaRpt->gain_trsw_cd[0],pPhyStaRpt->gain_trsw_cd[1]);*/
/*		DbgPrint("pPhyStaRpt->pwdb_all = 0x%x, pPhyInfo->RxPWDBAll = %d\n", pPhyStaRpt->pwdb_all, pPhyInfo->RxPWDBAll);*/
/*		DbgPrint("pPhyStaRpt->cfotail[i] = 0x%x, pPhyStaRpt->CFO_tail[i] = 0x%x\n", pPhyStaRpt->cfotail[0], pPhyStaRpt->cfotail[1]);*/
/*		DbgPrint("pPhyStaRpt->rxevm[0] = %d, pPhyStaRpt->rxevm[1] = %d\n", pPhyStaRpt->rxevm[0], pPhyStaRpt->rxevm[1]);*/
/*		DbgPrint("pPhyStaRpt->rxevm[2] = %d, pPhyStaRpt->rxevm[3] = %d\n", pPhyStaRpt->rxevm_cd[0], pPhyStaRpt->rxevm_cd[1]);*/
/*		DbgPrint("pPhyInfo->RxMIMOSignalStrength[0]=%d, pPhyInfo->RxMIMOSignalStrength[1]=%d, RxPWDBAll=%d\n",*/
/*				pPhyInfo->RxMIMOSignalStrength[0], pPhyInfo->RxMIMOSignalStrength[1], pPhyInfo->RxPWDBAll);*/
/*		DbgPrint("pPhyInfo->RxMIMOSignalStrength[2]=%d, pPhyInfo->RxMIMOSignalStrength[3]=%d\n",*/
/*				pPhyInfo->RxMIMOSignalStrength[2], pPhyInfo->RxMIMOSignalStrength[3]);*/
/*		DbgPrint("ppPhyInfo->RxMIMOSignalQuality[0]=%d, pPhyInfo->RxMIMOSignalQuality[1]=%d\n",*/
/*				pPhyInfo->RxMIMOSignalQuality[0], pPhyInfo->RxMIMOSignalQuality[1]);*/
/*		DbgPrint("ppPhyInfo->RxMIMOSignalQuality[2]=%d, pPhyInfo->RxMIMOSignalQuality[3]=%d\n",*/
/*				pPhyInfo->RxMIMOSignalQuality[2], pPhyInfo->RxMIMOSignalQuality[3]);*/

}

#endif

VOID
odm_Init_RSSIForDM(
	IN OUT	PDM_ODM_T	pDM_Odm
	)
{

}

VOID
odm_Process_RSSIForDM(	
	IN OUT	PDM_ODM_T					pDM_Odm,
	IN		PODM_PHY_INFO_T				pPhyInfo,
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{
	
	s4Byte			UndecoratedSmoothedPWDB, UndecoratedSmoothedCCK, UndecoratedSmoothedOFDM, RSSI_Ave;
	u1Byte			i, isCCKrate=0;	
	u1Byte			RSSI_max, RSSI_min;
	u4Byte			OFDM_pkt=0; 
	u4Byte			Weighting=0;
	PSTA_INFO_T           	pEntry;

	if (pPktinfo->StationID >= ODM_ASSOCIATE_ENTRY_NUM)
		return;

	//
	// 2012/05/30 MH/Luke.Lee Add some description 
	// In windows driver: AP/IBSS mode STA
	//
	//if (pDM_Odm->SupportPlatform == ODM_WIN)
	//{
	//	pEntry = pDM_Odm->pODM_StaInfo[pDM_Odm->pAidMap[pPktinfo->StationID-1]];			
	//}
	//else
		pEntry = pDM_Odm->pODM_StaInfo[pPktinfo->StationID];							

	if(!IS_STA_VALID(pEntry))
		return;

	if ((!pPktinfo->bPacketMatchBSSID))/*data frame only*/
		return;

	if(pPktinfo->bPacketBeacon)
		pDM_Odm->PhyDbgInfo.NumQryBeaconPkt++;
	
	isCCKrate = (pPktinfo->DataRate <= ODM_RATE11M )?TRUE :FALSE;
	pDM_Odm->RxRate = pPktinfo->DataRate;

	//-----------------Smart Antenna Debug Message------------------//
	
	UndecoratedSmoothedCCK =  pEntry->rssi_stat.UndecoratedSmoothedCCK;
	UndecoratedSmoothedOFDM = pEntry->rssi_stat.UndecoratedSmoothedOFDM;
	UndecoratedSmoothedPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;	
	
	if (pPktinfo->bPacketToSelf || pPktinfo->bPacketBeacon) {
		if (!isCCKrate) {
			//ofdm rate 
			if (pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B] == 0) {
				RSSI_Ave = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				pDM_Odm->RSSI_A = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				pDM_Odm->RSSI_B = 0;
			} else {
				/*DbgPrint("pRfd->Status.RxMIMOSignalStrength[0] = %d, pRfd->Status.RxMIMOSignalStrength[1] = %d\n",*/ 
					/*pRfd->Status.RxMIMOSignalStrength[0], pRfd->Status.RxMIMOSignalStrength[1]);*/
				pDM_Odm->RSSI_A =  pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				pDM_Odm->RSSI_B = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B];
			
				if (pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A] > pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B]) {
					RSSI_max = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
					RSSI_min = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B];
				} else {
					RSSI_max = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B];
					RSSI_min = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				}
				if ((RSSI_max - RSSI_min) < 3)
					RSSI_Ave = RSSI_max;
				else if ((RSSI_max - RSSI_min) < 6)
					RSSI_Ave = RSSI_max - 1;
				else if ((RSSI_max - RSSI_min) < 10)
					RSSI_Ave = RSSI_max - 2;
				else
					RSSI_Ave = RSSI_max - 3;
			}

			//1 Process OFDM RSSI
			if(UndecoratedSmoothedOFDM <= 0)	// initialize
			{
				UndecoratedSmoothedOFDM = pPhyInfo->RxPWDBAll;
			}
			else
			{
				if(pPhyInfo->RxPWDBAll > (u4Byte)UndecoratedSmoothedOFDM)
				{
					UndecoratedSmoothedOFDM = 	
							( ((UndecoratedSmoothedOFDM)*(Rx_Smooth_Factor-1)) + 
							(RSSI_Ave)) /(Rx_Smooth_Factor);
					UndecoratedSmoothedOFDM = UndecoratedSmoothedOFDM + 1;
				}
				else
				{
					UndecoratedSmoothedOFDM = 	
							( ((UndecoratedSmoothedOFDM)*(Rx_Smooth_Factor-1)) + 
							(RSSI_Ave)) /(Rx_Smooth_Factor);
				}
			}				
			if (pEntry->rssi_stat.OFDM_pkt != 64) {
				i = 63;
				pEntry->rssi_stat.OFDM_pkt -= (u4Byte)(((pEntry->rssi_stat.PacketMap>>i)&BIT0)-1);
			}
			pEntry->rssi_stat.PacketMap = (pEntry->rssi_stat.PacketMap<<1) | BIT0;			
										
		}
		else
		{
			RSSI_Ave = pPhyInfo->RxPWDBAll;
			pDM_Odm->RSSI_A = (u1Byte) pPhyInfo->RxPWDBAll;
			pDM_Odm->RSSI_B = 0xFF;
			pDM_Odm->RSSI_C = 0xFF;
			pDM_Odm->RSSI_D = 0xFF;

			//1 Process CCK RSSI
			if(UndecoratedSmoothedCCK <= 0)	// initialize
			{
				UndecoratedSmoothedCCK = pPhyInfo->RxPWDBAll;
			}
			else
			{
				if(pPhyInfo->RxPWDBAll > (u4Byte)UndecoratedSmoothedCCK)
				{
					UndecoratedSmoothedCCK = 	
							( ((UndecoratedSmoothedCCK)*(Rx_Smooth_Factor-1)) + 
							(pPhyInfo->RxPWDBAll)) /(Rx_Smooth_Factor);
					UndecoratedSmoothedCCK = UndecoratedSmoothedCCK + 1;
				}
				else
				{
					UndecoratedSmoothedCCK = 	
							( ((UndecoratedSmoothedCCK)*(Rx_Smooth_Factor-1)) + 
							(pPhyInfo->RxPWDBAll)) /(Rx_Smooth_Factor);
				}
			}
			i = 63;
			pEntry->rssi_stat.OFDM_pkt -= (u4Byte)((pEntry->rssi_stat.PacketMap>>i)&BIT0);			
			pEntry->rssi_stat.PacketMap = pEntry->rssi_stat.PacketMap<<1;			
		}

		//if(pEntry)
		{
			//2011.07.28 LukeLee: modified to prevent unstable CCK RSSI
			if (pEntry->rssi_stat.OFDM_pkt == 64) { /* speed up when all packets are OFDM*/
				UndecoratedSmoothedPWDB = UndecoratedSmoothedOFDM;
			} else {
				if (pEntry->rssi_stat.ValidBit < 64)
					pEntry->rssi_stat.ValidBit++;

				if (pEntry->rssi_stat.ValidBit == 64) {
					Weighting = ((pEntry->rssi_stat.OFDM_pkt<<4) > 64)?64:(pEntry->rssi_stat.OFDM_pkt<<4);
					UndecoratedSmoothedPWDB = (Weighting*UndecoratedSmoothedOFDM+(64-Weighting)*UndecoratedSmoothedCCK)>>6;
				} else {
					if (pEntry->rssi_stat.ValidBit != 0)
						UndecoratedSmoothedPWDB = (pEntry->rssi_stat.OFDM_pkt*UndecoratedSmoothedOFDM+(pEntry->rssi_stat.ValidBit-pEntry->rssi_stat.OFDM_pkt)*UndecoratedSmoothedCCK)/pEntry->rssi_stat.ValidBit;
					else
						UndecoratedSmoothedPWDB = 0;
				}
			}
			#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
			if (pEntry->rssi_stat.UndecoratedSmoothedPWDB == -1)
				phydm_ra_rssi_rpt_wk(pDM_Odm);
			#endif
			pEntry->rssi_stat.UndecoratedSmoothedCCK = UndecoratedSmoothedCCK;
			pEntry->rssi_stat.UndecoratedSmoothedOFDM = UndecoratedSmoothedOFDM;
			pEntry->rssi_stat.UndecoratedSmoothedPWDB = UndecoratedSmoothedPWDB;

			//DbgPrint("OFDM_pkt=%d, Weighting=%d\n", OFDM_pkt, Weighting);
			//DbgPrint("UndecoratedSmoothedOFDM=%d, UndecoratedSmoothedPWDB=%d, UndecoratedSmoothedCCK=%d\n", 
			//	UndecoratedSmoothedOFDM, UndecoratedSmoothedPWDB, UndecoratedSmoothedCCK);
			
		}
	
	}
}


#if(ODM_IC_11N_SERIES_SUPPORT ==1)
//
// Endianness before calling this API
//
VOID
ODM_PhyStatusQuery_92CSeries(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T				pPhyInfo,
	IN 		pu1Byte						pPhyStatus,	
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{
	odm_RxPhyStatus92CSeries_Parsing(pDM_Odm, pPhyInfo, pPhyStatus, pPktinfo);
	odm_Process_RSSIForDM(pDM_Odm, pPhyInfo, pPktinfo);
}
#endif


//
// Endianness before calling this API
//
#if	ODM_IC_11AC_SERIES_SUPPORT

VOID
ODM_PhyStatusQuery_JaguarSeries(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,
	IN 		pu1Byte						pPhyStatus,	
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{
	odm_RxPhyStatusJaguarSeries_Parsing(
							pDM_Odm,
							pPhyInfo,
							pPhyStatus,
							pPktinfo);
	
	odm_Process_RSSIForDM(pDM_Odm,pPhyInfo,pPktinfo);
}
#endif

VOID
ODM_PhyStatusQuery(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,
	IN 		pu1Byte						pPhyStatus,	
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{

#if	ODM_IC_11AC_SERIES_SUPPORT
	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		ODM_PhyStatusQuery_JaguarSeries(pDM_Odm, pPhyInfo, pPhyStatus, pPktinfo);
#endif

#if	ODM_IC_11N_SERIES_SUPPORT
	if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES )
		ODM_PhyStatusQuery_92CSeries(pDM_Odm,pPhyInfo,pPhyStatus,pPktinfo);
#endif
}
	
// For future use.
VOID
ODM_MacStatusQuery(
	IN OUT	PDM_ODM_T					pDM_Odm,
	IN 		pu1Byte						pMacStatus,
	IN		u1Byte						MacID,	
	IN		BOOLEAN						bPacketMatchBSSID,
	IN		BOOLEAN						bPacketToSelf,
	IN		BOOLEAN						bPacketBeacon
	)
{
	// 2011/10/19 Driver team will handle in the future.
	
}


//
// If you want to add a new IC, Please follow below template and generate a new one.
// 
//

HAL_STATUS
ODM_ConfigRFWithHeaderFile(
	IN 	PDM_ODM_T	        	pDM_Odm,
	IN 	ODM_RF_Config_Type 		ConfigType,
	IN 	ODM_RF_RADIO_PATH_E 	eRFPath
    )
{
   ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		("===>ODM_ConfigRFWithHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));

//1 All platforms support

#if (RTL8188F_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8188F) {
		if (ConfigType == CONFIG_RF_RADIO) {
			if (eRFPath == ODM_RF_PATH_A)
				READ_AND_CONFIG_MP(8188F, _RadioA);
		} else if (ConfigType == CONFIG_RF_TXPWR_LMT)
			READ_AND_CONFIG_MP(8188F, _TXPWR_LMT);
	}
#endif

	return HAL_STATUS_SUCCESS;
}

HAL_STATUS
ODM_ConfigRFWithTxPwrTrackHeaderFile(
	IN 	PDM_ODM_T	        	pDM_Odm
    )
{
   	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		 ("===>ODM_ConfigRFWithTxPwrTrackHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
   	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				 ("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				 pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));

//1 All platforms support

#if RTL8188F_SUPPORT
	if (pDM_Odm->SupportICType == ODM_RTL8188F) {
		if (pDM_Odm->SupportInterface == ODM_ITRF_USB)
			READ_AND_CONFIG_MP(8188F, _TxPowerTrack_USB);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_SDIO)
			READ_AND_CONFIG_MP(8188F, _TxPowerTrack_SDIO);
	}
#endif

	return HAL_STATUS_SUCCESS;
}

HAL_STATUS
ODM_ConfigBBWithHeaderFile(
	IN 	PDM_ODM_T	             	pDM_Odm,
	IN 	ODM_BB_Config_Type 		ConfigType
	)
{
//1 All platforms support

#if (RTL8188F_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8188F) {
		if (ConfigType == CONFIG_BB_PHY_REG) 
			READ_AND_CONFIG_MP(8188F, _PHY_REG);
		else if (ConfigType == CONFIG_BB_AGC_TAB) 
			READ_AND_CONFIG_MP(8188F, _AGC_TAB);
		else if (ConfigType == CONFIG_BB_PHY_REG_PG) 
			READ_AND_CONFIG_MP(8188F, _PHY_REG_PG);
	}
#endif
	return HAL_STATUS_SUCCESS; 
}                 

HAL_STATUS
ODM_ConfigMACWithHeaderFile(
	IN 	PDM_ODM_T	pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		("===>ODM_ConfigMACWithHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));

//1 All platforms support

#if (RTL8188F_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8188F) 
		READ_AND_CONFIG_MP(8188F, _MAC_REG);
#endif

	return HAL_STATUS_SUCCESS;    
} 

HAL_STATUS
ODM_ConfigFWWithHeaderFile(
	IN 	PDM_ODM_T			pDM_Odm,
	IN 	ODM_FW_Config_Type 	ConfigType,
	OUT u1Byte				*pFirmware,
	OUT u4Byte				*pSize
	)
{
#if (DM_ODM_SUPPORT_TYPE != ODM_AP)

#if (RTL8188F_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8188F) {
		if (ConfigType == CONFIG_FW_NIC)
			READ_FIRMWARE_MP(8188F, _FW_NIC);
		else if (ConfigType == CONFIG_FW_WoWLAN)
			READ_FIRMWARE_MP(8188F, _FW_WoWLAN);
	}
#endif

#endif//(DM_ODM_SUPPORT_TYPE != ODM_AP)
	return HAL_STATUS_SUCCESS;    
} 

u4Byte 
ODM_GetHWImgVersion(
	IN	PDM_ODM_T	pDM_Odm
	)
{
    u4Byte  Version=0;

/*1 All platforms support*/

#if (RTL8188F_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8188F)
		Version = GET_VERSION_MP(8188F, _MAC_REG);
#endif
	return Version;
}
