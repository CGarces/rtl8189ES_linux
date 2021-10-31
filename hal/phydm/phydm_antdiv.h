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

#ifndef	__PHYDMANTDIV_H__
#define    __PHYDMANTDIV_H__

//1 ============================================================
//1  Definition 
//1 ============================================================

#define	MAIN_ANT	1

/*smart antenna*/
#define SUPPORT_RF_PATH_NUM 4
#define SUPPORT_BEAM_PATTERN_NUM 4

#define ODM_N_ANTDIV_SUPPORT		(ODM_RTL8188E|ODM_RTL8192E|ODM_RTL8723B|ODM_RTL8188F)
#define ODM_AC_ANTDIV_SUPPORT	(ODM_RTL8821|ODM_RTL8881A|ODM_RTL8812)
#define ODM_ANTDIV_SUPPORT		(ODM_N_ANTDIV_SUPPORT|ODM_AC_ANTDIV_SUPPORT)

//1 ============================================================
//1  structure
//1 ============================================================


typedef struct _SW_Antenna_Switch_
{
	u1Byte		Double_chk_flag;	/*If current antenna RSSI > "RSSI_CHECK_THRESHOLD", than check this antenna again*/
	u1Byte		try_flag;
	s4Byte		PreRSSI;
	u1Byte		CurAntenna;
	u1Byte		PreAntenna;
	u1Byte		RSSI_Trying;
	u1Byte 		reset_idx;
	u1Byte		Train_time;
	u1Byte		Train_time_flag; /*base on RSSI difference between two antennas*/
	RT_TIMER	phydm_SwAntennaSwitchTimer;
	u4Byte		PktCnt_SWAntDivByCtrlFrame;
	BOOLEAN		bSWAntDivByCtrlFrame;
	/* AntDect (Before link Antenna Switch check) need to be moved*/
	u2Byte		Single_Ant_Counter;
	u2Byte		Dual_Ant_Counter;
	u2Byte		Aux_FailDetec_Counter;
	u2Byte		Retry_Counter;	
	u1Byte		SWAS_NoLink_State;
	u4Byte		SWAS_NoLink_BK_Reg948;
	BOOLEAN		ANTA_ON;	/*To indicate Ant A is or not*/
	BOOLEAN		ANTB_ON;	/*To indicate Ant B is on or not*/
	BOOLEAN		Pre_Aux_FailDetec;
	BOOLEAN		RSSI_AntDect_bResult;	
	u1Byte		Ant5G;
	u1Byte		Ant2G;

	
}SWAT_T, *pSWAT_T;

typedef struct _FAST_ANTENNA_TRAINNING_
{
	u1Byte	Bssid[6];
	u1Byte	antsel_rx_keep_0;
	u1Byte	antsel_rx_keep_1;
	u1Byte	antsel_rx_keep_2;
	u1Byte	antsel_rx_keep_3;
	u4Byte	antSumRSSI[7];
	u4Byte	antRSSIcnt[7];
	u4Byte	antAveRSSI[7];
	u1Byte	FAT_State;
	u4Byte	TrainIdx;
	u1Byte	antsel_a[ODM_ASSOCIATE_ENTRY_NUM];
	u1Byte	antsel_b[ODM_ASSOCIATE_ENTRY_NUM];
	u1Byte	antsel_c[ODM_ASSOCIATE_ENTRY_NUM];
	u4Byte	MainAnt_Sum[ODM_ASSOCIATE_ENTRY_NUM];
	u4Byte	AuxAnt_Sum[ODM_ASSOCIATE_ENTRY_NUM];
	u4Byte	MainAnt_Cnt[ODM_ASSOCIATE_ENTRY_NUM];
	u4Byte	AuxAnt_Cnt[ODM_ASSOCIATE_ENTRY_NUM];
	u1Byte	RxIdleAnt;
	u1Byte	AntDiv_OnOff;
	BOOLEAN	bBecomeLinked;
	u4Byte	MinMaxRSSI;
	u1Byte	idx_AntDiv_counter_2G;
	u1Byte	idx_AntDiv_counter_5G;
	u1Byte	AntDiv_2G_5G;
	u4Byte    CCK_counter_main;
	u4Byte    CCK_counter_aux;	
	u4Byte    OFDM_counter_main;
	u4Byte    OFDM_counter_aux;

	BOOLEAN	fix_ant_bfee;
	BOOLEAN	enable_ctrl_frame_antdiv;
	BOOLEAN	use_ctrl_frame_antdiv;
	u1Byte	hw_antsw_occur;
}FAT_T,*pFAT_T;


//1 ============================================================
//1  enumeration
//1 ============================================================



typedef enum _FAT_STATE /*Fast antenna training*/
{
	FAT_BEFORE_LINK_STATE	= 0,
	FAT_PREPARE_STATE			= 1,
	FAT_TRAINING_STATE		= 2,
	FAT_DECISION_STATE		= 3
}FAT_STATE_E, *PFAT_STATE_E;

typedef enum _ANT_DIV_TYPE
{
	CG_TRX_HW_ANTDIV			= 0x01,
	CGCS_RX_HW_ANTDIV		= 0x02,
	CG_TRX_SMART_ANTDIV		= 0x03,
	S0S1_SW_ANTDIV			= 0x04, /*8723B intrnal switch S0 S1*/
	HL_SW_SMART_ANT_TYPE1	= 0x10 /*Hong-Lin Smart antenna use for 8821AE which is a 2 Ant. entitys, and each Ant. is equipped with 4 antenna patterns*/
}ANT_DIV_TYPE_E, *PANT_DIV_TYPE_E;

#endif /*#ifndef	__ODMANTDIV_H__*/