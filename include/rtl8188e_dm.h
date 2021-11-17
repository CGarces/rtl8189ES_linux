// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef __RTL8188E_DM_H__
#define __RTL8188E_DM_H__

void rtl8188e_init_dm_priv(IN PADAPTER Adapter);
void rtl8188e_deinit_dm_priv(IN PADAPTER Adapter);
void rtl8188e_InitHalDm(IN PADAPTER Adapter);
void rtl8188e_HalDmWatchDog(IN PADAPTER Adapter);

//VOID rtl8192c_dm_CheckTXPowerTracking(IN PADAPTER Adapter);

//void rtl8192c_dm_RF_Saving(IN PADAPTER pAdapter, IN u8 bForceInNormal);

#ifdef CONFIG_ANTENNA_DIVERSITY
void	AntDivCompare8188E(PADAPTER Adapter, WLAN_BSSID_EX *dst, WLAN_BSSID_EX *src);
u8 AntDivBeforeLink8188E(PADAPTER Adapter );
#endif
#endif

