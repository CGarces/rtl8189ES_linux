// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef	__ODMNOISEMONITOR_H__
#define __ODMNOISEMONITOR_H__

#define	ODM_MAX_CHANNEL_NUM					38//14+24
struct noise_level
{
	//u1Byte				value_a, value_b;
	u1Byte				value[MAX_RF_PATH];
	//s1Byte				sval_a, sval_b;
	s1Byte				sval[MAX_RF_PATH];
	
	//s4Byte				noise_a=0, noise_b=0,sum_a=0, sum_b=0;
	//s4Byte				noise[ODM_RF_PATH_MAX];
	s4Byte				sum[MAX_RF_PATH];
	//u1Byte				valid_cnt_a=0, valid_cnt_b=0, 
	u1Byte				valid[MAX_RF_PATH];
	u1Byte				valid_cnt[MAX_RF_PATH];

};


typedef struct _ODM_NOISE_MONITOR_
{
	s1Byte			noise[MAX_RF_PATH];
	s2Byte			noise_all;	
}ODM_NOISE_MONITOR;

s2Byte ODM_InbandNoise_Monitor(PVOID pDM_VOID,u8 bPauseDIG,u8 IGIValue,u32 max_time);

#endif
