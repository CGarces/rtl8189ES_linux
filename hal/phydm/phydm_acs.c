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


u1Byte
ODM_GetAutoChannelSelectResult(
	IN		PVOID			pDM_VOID,
	IN		u1Byte			Band
)
{
	PDM_ODM_T				pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PACS					pACS = &pDM_Odm->DM_ACS;

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	if(Band == ODM_BAND_2_4G)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("[ACS] ODM_GetAutoChannelSelectResult(): CleanChannel_2G(%d)\n", pACS->CleanChannel_2G));
		return (u1Byte)pACS->CleanChannel_2G;	
	}
	else
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("[ACS] ODM_GetAutoChannelSelectResult(): CleanChannel_5G(%d)\n", pACS->CleanChannel_5G));
		return (u1Byte)pACS->CleanChannel_5G;	
	}
#else
	return (u1Byte)pACS->CleanChannel_2G;
#endif

}

VOID
odm_AutoChannelSelectSetting(
	IN		PVOID			pDM_VOID,
	IN		BOOLEAN			IsEnable
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	PDM_ODM_T					pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u2Byte						period = 0x2710;// 40ms in default
	u2Byte						NHMType = 0x7;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelectSetting()=========> \n"));

	if(IsEnable)
	{//20 ms
		period = 0x1388;
		NHMType = 0x1;
	}

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		//PHY parameters initialize for ac series
		ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11AC+2, period);	//0x990[31:16]=0x2710	Time duration for NHM unit: 4us, 0x2710=40ms
		//ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC, BIT8|BIT9|BIT10, NHMType);	//0x994[9:8]=3			enable CCX
	}
	else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		//PHY parameters initialize for n series
		ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11N+2, period);	//0x894[31:16]=0x2710	Time duration for NHM unit: 4us, 0x2710=40ms
		//ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N, BIT10|BIT9|BIT8, NHMType);	//0x890[9:8]=3			enable CCX		
	}
#endif
}

VOID
odm_AutoChannelSelectInit(
	IN		PVOID			pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	PDM_ODM_T					pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PACS						pACS = &pDM_Odm->DM_ACS;
	u1Byte						i;

	if(!(pDM_Odm->SupportAbility & ODM_BB_NHM_CNT))
		return;

	if(pACS->bForceACSResult)
		return;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelectInit()=========> \n"));

	pACS->CleanChannel_2G = 1;
	pACS->CleanChannel_5G = 36;

	for (i = 0; i < ODM_MAX_CHANNEL_2G; ++i)
	{
		pACS->Channel_Info_2G[0][i] = 0;
		pACS->Channel_Info_2G[1][i] = 0;
	}

	if(pDM_Odm->SupportICType & (ODM_IC_11AC_SERIES|ODM_RTL8192D))
	{
		for (i = 0; i < ODM_MAX_CHANNEL_5G; ++i)
		{
			pACS->Channel_Info_5G[0][i] = 0;
			pACS->Channel_Info_5G[1][i] = 0;
		}
	}
#endif
}

VOID
odm_AutoChannelSelectReset(
	IN		PVOID			pDM_VOID
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	PDM_ODM_T					pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PACS						pACS = &pDM_Odm->DM_ACS;

	if(!(pDM_Odm->SupportAbility & ODM_BB_NHM_CNT))
		return;

	if(pACS->bForceACSResult)
		return;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelectReset()=========> \n"));

	odm_AutoChannelSelectSetting(pDM_Odm,TRUE);// for 20ms measurement
	Phydm_NHMCounterStatisticsReset(pDM_Odm);
#endif
}

VOID
odm_AutoChannelSelect(
	IN		PVOID			pDM_VOID,
	IN		u1Byte			Channel
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	PDM_ODM_T					pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PACS						pACS = &pDM_Odm->DM_ACS;
	u1Byte						ChannelIDX = 0, SearchIDX = 0;
	u2Byte						MaxScore=0;

	if(!(pDM_Odm->SupportAbility & ODM_BB_NHM_CNT))
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_AutoChannelSelect(): Return: SupportAbility ODM_BB_NHM_CNT is disabled\n"));
		return;
	}

	if(pACS->bForceACSResult)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_AutoChannelSelect(): Force 2G clean channel = %d, 5G clean channel = %d\n",
			pACS->CleanChannel_2G, pACS->CleanChannel_5G));
		return;
	}

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelect(): Channel = %d=========> \n", Channel));

	Phydm_GetNHMCounterStatistics(pDM_Odm);
	odm_AutoChannelSelectSetting(pDM_Odm,FALSE);

	if(Channel >=1 && Channel <=14)
	{
		ChannelIDX = Channel - 1;
		pACS->Channel_Info_2G[1][ChannelIDX]++;
		
		if(pACS->Channel_Info_2G[1][ChannelIDX] >= 2)
			pACS->Channel_Info_2G[0][ChannelIDX] = (pACS->Channel_Info_2G[0][ChannelIDX] >> 1) + 
			(pACS->Channel_Info_2G[0][ChannelIDX] >> 2) + (pDM_Odm->NHM_cnt_0>>2);
		else
			pACS->Channel_Info_2G[0][ChannelIDX] = pDM_Odm->NHM_cnt_0;
	
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelect(): NHM_cnt_0 = %d \n", pDM_Odm->NHM_cnt_0));
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelect(): Channel_Info[0][%d] = %d, Channel_Info[1][%d] = %d\n", ChannelIDX, pACS->Channel_Info_2G[0][ChannelIDX], ChannelIDX, pACS->Channel_Info_2G[1][ChannelIDX]));

		for(SearchIDX = 0; SearchIDX < ODM_MAX_CHANNEL_2G; SearchIDX++)
		{
			if(pACS->Channel_Info_2G[1][SearchIDX] != 0)
			{
				if(pACS->Channel_Info_2G[0][SearchIDX] >= MaxScore)
				{
					MaxScore = pACS->Channel_Info_2G[0][SearchIDX];
					pACS->CleanChannel_2G = SearchIDX+1;
				}
			}
		}
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("(1)odm_AutoChannelSelect(): 2G: CleanChannel_2G = %d, MaxScore = %d \n", 
			pACS->CleanChannel_2G, MaxScore));

	}
	else if(Channel >= 36)
	{
		// Need to do
		pACS->CleanChannel_5G = Channel;
	}
#endif
}

#if ( DM_ODM_SUPPORT_TYPE & ODM_AP )

VOID
phydm_AutoChannelSelectSettingAP(
    IN  PVOID   pDM_VOID,
    IN  u4Byte  setting,             // 0: STORE_DEFAULT_NHM_SETTING; 1: RESTORE_DEFAULT_NHM_SETTING, 2: ACS_NHM_SETTING
    IN  u4Byte  acs_step
)
{
    PDM_ODM_T           pDM_Odm = (PDM_ODM_T)pDM_VOID;
    prtl8192cd_priv       priv           = pDM_Odm->priv;
    PACS                    pACS         = &pDM_Odm->DM_ACS;

    ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("odm_AutoChannelSelectSettingAP()=========> \n"));

    //3 Store Default Setting
    if(setting == STORE_DEFAULT_NHM_SETTING)
    {
        ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("STORE_DEFAULT_NHM_SETTING\n"));
    
        if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)     // store Reg0x990, Reg0x994, Reg0x998, Reg0x99C, Reg0x9a0
        {
            pACS->Reg0x990 = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TIMER_11AC);                // Reg0x990
            pACS->Reg0x994 = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC);           // Reg0x994
            pACS->Reg0x998 = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11AC);       // Reg0x998
            pACS->Reg0x99C = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11AC);       // Reg0x99c
            pACS->Reg0x9A0 = ODM_Read1Byte(pDM_Odm, ODM_REG_NHM_TH8_11AC);                   // Reg0x9a0, u1Byte            
        }
        else if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
        {
            pACS->Reg0x890 = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N);             // Reg0x890
            pACS->Reg0x894 = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TIMER_11N);                  // Reg0x894
            pACS->Reg0x898 = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11N);         // Reg0x898
            pACS->Reg0x89C = ODM_Read4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11N);         // Reg0x89c
            pACS->Reg0xE28 = ODM_Read1Byte(pDM_Odm, ODM_REG_NHM_TH8_11N);                     // Reg0xe28, u1Byte    
        }
    }

    //3 Restore Default Setting
    else if(setting == RESTORE_DEFAULT_NHM_SETTING)
    {
        ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("RESTORE_DEFAULT_NHM_SETTING\n"));
        
        if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)     // store Reg0x990, Reg0x994, Reg0x998, Reg0x99C, Reg0x9a0
        {
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TIMER_11AC,          pACS->Reg0x990);
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC,     pACS->Reg0x994);
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11AC, pACS->Reg0x998);
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11AC, pACS->Reg0x99C);
            ODM_Write1Byte(pDM_Odm, ODM_REG_NHM_TH8_11AC,             pACS->Reg0x9A0);   
        }
        else if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
        {
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N,     pACS->Reg0x890);
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TIMER_11N,          pACS->Reg0x894);
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11N, pACS->Reg0x898);
            ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11N, pACS->Reg0x89C);
            ODM_Write1Byte(pDM_Odm, ODM_REG_NHM_TH8_11N,             pACS->Reg0xE28); 
        }        
    }

    //3 ACS Setting
    else if(setting == ACS_NHM_SETTING)
    {        
        ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("ACS_NHM_SETTING\n"));
        u2Byte  period;
        period = 0x61a8;
        pACS->ACS_Step = acs_step;
            
        if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
        {   
            //4 Set NHM period, 0x990[31:16]=0x61a8, Time duration for NHM unit: 4us, 0x61a8=100ms
            ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11AC+2, period);
            //4 Set NHM ignore_cca=1, ignore_txon=1, ccx_en=0
            ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC,BIT8|BIT9|BIT10, 3);
            
            if(pACS->ACS_Step == 0)
            {
                //4 Set IGI
                ODM_SetBBReg(pDM_Odm,0xc50,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x3E);
                if (get_rf_mimo_mode(priv) != MIMO_1T1R)
					ODM_SetBBReg(pDM_Odm,0xe50,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x3E);
                    
                //4 Set ACS NHM threshold
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11AC, 0x82786e64);
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11AC, 0xffffff8c);
                ODM_Write1Byte(pDM_Odm, ODM_REG_NHM_TH8_11AC, 0xff);
                ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC+2, 0xffff);
                
            }
            else if(pACS->ACS_Step == 1)
            {
                //4 Set IGI
                ODM_SetBBReg(pDM_Odm,0xc50,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x2A);
                if (get_rf_mimo_mode(priv) != MIMO_1T1R)
					ODM_SetBBReg(pDM_Odm,0xe50,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x2A);

                //4 Set ACS NHM threshold
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11AC, 0x5a50463c);
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11AC, 0xffffff64);
                
            }

        }
        else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
        {
            //4 Set NHM period, 0x894[31:16]=0x61a8, Time duration for NHM unit: 4us, 0x61a8=100ms
            ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11N+2, period);
            //4 Set NHM ignore_cca=1, ignore_txon=1, ccx_en=0
            ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N,BIT8|BIT9|BIT10, 3);
            
            if(pACS->ACS_Step == 0)
            {
                //4 Set IGI
                ODM_SetBBReg(pDM_Odm,0xc50,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x3E);
                if (get_rf_mimo_mode(priv) != MIMO_1T1R)
					ODM_SetBBReg(pDM_Odm,0xc58,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x3E);
            
                //4 Set ACS NHM threshold
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11N, 0x82786e64);
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11N, 0xffffff8c);
                ODM_Write1Byte(pDM_Odm, ODM_REG_NHM_TH8_11N, 0xff);
                ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N+2, 0xffff);
                
            }
            else if(pACS->ACS_Step == 1)
            {
                //4 Set IGI
                ODM_SetBBReg(pDM_Odm,0xc50,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x2A);
                if (get_rf_mimo_mode(priv) != MIMO_1T1R)
					ODM_SetBBReg(pDM_Odm,0xc58,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,0x2A);
            
                //4 Set ACS NHM threshold
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11N, 0x5a50463c);
                ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11N, 0xffffff64);

            }            
        }
    }	
	
}

VOID
phydm_GetNHMStatisticsAP(
    IN  PVOID       pDM_VOID,
    IN  u4Byte      idx,                // @ 2G, Real channel number = idx+1
    IN  u4Byte      acs_step
)
{
    PDM_ODM_T	    pDM_Odm = (PDM_ODM_T)pDM_VOID;
    prtl8192cd_priv     priv    = pDM_Odm->priv;
    PACS                  pACS    = &pDM_Odm->DM_ACS;
    u4Byte                value32 = 0;
    u1Byte                i;

    pACS->ACS_Step = acs_step;

    if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
    {
        //4 Check if NHM result is ready        
        for (i=0; i<20; i++) {
            
            ODM_delay_ms(1);
            if ( ODM_GetBBReg(pDM_Odm,rFPGA0_PSDReport,BIT17) )
                break;
        }
        
        //4 Get NHM Statistics        
        if ( pACS->ACS_Step==1 ) {
            
            value32 = ODM_Read4Byte(pDM_Odm,ODM_REG_NHM_CNT7_TO_CNT4_11N);
            
            pACS->NHM_Cnt[idx][9] = (value32 & bMaskByte1) >> 8;
            pACS->NHM_Cnt[idx][8] = (value32 & bMaskByte0);

            value32 = ODM_Read4Byte(pDM_Odm,ODM_REG_NHM_CNT_11N);    // ODM_REG_NHM_CNT3_TO_CNT0_11N

            pACS->NHM_Cnt[idx][7] = (value32 & bMaskByte3) >> 24;
            pACS->NHM_Cnt[idx][6] = (value32 & bMaskByte2) >> 16;
            pACS->NHM_Cnt[idx][5] = (value32 & bMaskByte1) >> 8;

        } else if (pACS->ACS_Step==2) {
        
            value32 = ODM_Read4Byte(pDM_Odm,ODM_REG_NHM_CNT_11N);   // ODM_REG_NHM_CNT3_TO_CNT0_11N

            pACS->NHM_Cnt[idx][4] = ODM_Read1Byte(pDM_Odm, ODM_REG_NHM_CNT7_TO_CNT4_11N);            
            pACS->NHM_Cnt[idx][3] = (value32 & bMaskByte3) >> 24;
            pACS->NHM_Cnt[idx][2] = (value32 & bMaskByte2) >> 16;
            pACS->NHM_Cnt[idx][1] = (value32 & bMaskByte1) >> 8;
            pACS->NHM_Cnt[idx][0] = (value32 & bMaskByte0);
        }
    }
    else if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
    {
        //4 Check if NHM result is ready        
        for (i=0; i<20; i++) {
            
            ODM_delay_ms(1);
            if (ODM_GetBBReg(pDM_Odm,ODM_REG_NHM_DUR_READY_11AC,BIT17))
                break;
        }
    
        if ( pACS->ACS_Step==1 ) {
            
            value32 = ODM_Read4Byte(pDM_Odm,ODM_REG_NHM_CNT7_TO_CNT4_11AC);
            
            pACS->NHM_Cnt[idx][9] = (value32 & bMaskByte1) >> 8;
            pACS->NHM_Cnt[idx][8] = (value32 & bMaskByte0);

            value32 = ODM_Read4Byte(pDM_Odm,ODM_REG_NHM_CNT_11AC);     // ODM_REG_NHM_CNT3_TO_CNT0_11AC

            pACS->NHM_Cnt[idx][7] = (value32 & bMaskByte3) >> 24;
            pACS->NHM_Cnt[idx][6] = (value32 & bMaskByte2) >> 16;
            pACS->NHM_Cnt[idx][5] = (value32 & bMaskByte1) >> 8;

        } else if (pACS->ACS_Step==2) {
        
            value32 = ODM_Read4Byte(pDM_Odm,ODM_REG_NHM_CNT_11AC);      // ODM_REG_NHM_CNT3_TO_CNT0_11AC

            pACS->NHM_Cnt[idx][4] = ODM_Read1Byte(pDM_Odm, ODM_REG_NHM_CNT7_TO_CNT4_11AC);            
            pACS->NHM_Cnt[idx][3] = (value32 & bMaskByte3) >> 24;
            pACS->NHM_Cnt[idx][2] = (value32 & bMaskByte2) >> 16;
            pACS->NHM_Cnt[idx][1] = (value32 & bMaskByte1) >> 8;
            pACS->NHM_Cnt[idx][0] = (value32 & bMaskByte0);
        }            
    }

}

#endif

VOID
phydm_CLMInit(
	IN		PVOID			pDM_VOID,
	IN		u2Byte			sampleNum			/*unit : 4us*/
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;		

	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES) {
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_TIME_PERIOD_11AC, bMaskLWord, sampleNum);	/*4us sample 1 time*/
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_11AC, BIT8, 0x1);							/*Enable CCX for CLM*/
	} else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES) {
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_TIME_PERIOD_11N, bMaskLWord, sampleNum);	/*4us sample 1 time*/
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_11N, BIT8, 0x1);								/*Enable CCX for CLM*/	
	}

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("[%s] : CLM sampleNum = %d\n", __func__, sampleNum));
		
}

VOID
phydm_CLMtrigger(
	IN		PVOID			pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES) {
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_11AC, BIT0, 0x0);	/*Trigger CLM*/
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_11AC, BIT0, 0x1);
	} else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES) {
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_11N, BIT0, 0x0);	/*Trigger CLM*/
		ODM_SetBBReg(pDM_Odm, ODM_REG_CLM_11N, BIT0, 0x1);
	}
}

BOOLEAN
phydm_checkCLMready(
	IN		PVOID			pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u4Byte			value32 = 0;
	BOOLEAN			ret = FALSE;
	
	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		value32 = ODM_GetBBReg(pDM_Odm, ODM_REG_CLM_RESULT_11AC, bMaskDWord);				/*make sure CLM calc is ready*/
	else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
		value32 = ODM_GetBBReg(pDM_Odm, ODM_REG_CLM_READY_11N, bMaskDWord);				/*make sure CLM calc is ready*/

	if (value32 & BIT16)
		ret = TRUE;
	else
		ret = FALSE;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("[%s] : CLM ready = %d\n", __func__, ret));

	return ret;
}

u2Byte
phydm_getCLMresult(
	IN		PVOID			pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u4Byte			value32 = 0;
	u2Byte			results = 0;
	
	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		value32 = ODM_GetBBReg(pDM_Odm, ODM_REG_CLM_RESULT_11AC, bMaskDWord);				/*read CLM calc result*/
	else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
		value32 = ODM_GetBBReg(pDM_Odm, ODM_REG_CLM_RESULT_11N, bMaskDWord);				/*read CLM calc result*/

	results = (u2Byte)(value32 & bMaskLWord);

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ACS, ODM_DBG_LOUD, ("[%s] : CLM result = %d\n", __func__, results));
	
	return results;
/*results are number of CCA times in sampleNum*/
}

