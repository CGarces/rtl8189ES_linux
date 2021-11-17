// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2012 Realtek Corporation. */
#ifndef __RTW_ODM_H__
#define __RTW_ODM_H__

#include <drv_types.h>
#include "../hal/phydm/phydm_types.h"
/*
* This file provides utilities/wrappers for rtw driver to use ODM
*/

void rtw_odm_dbg_comp_msg(void *sel,_adapter *adapter);
void rtw_odm_dbg_comp_set(_adapter *adapter, u64 comps);
void rtw_odm_dbg_level_msg(void *sel,_adapter *adapter);
void rtw_odm_dbg_level_set(_adapter *adapter, u32 level);

void rtw_odm_ability_msg(void *sel, _adapter *adapter);
void rtw_odm_ability_set(_adapter *adapter, u32 ability);

bool rtw_odm_adaptivity_needed(_adapter *adapter);
void rtw_odm_adaptivity_parm_msg(void *sel,_adapter *adapter);
void rtw_odm_adaptivity_parm_set(_adapter *adapter, s8 TH_L2H_ini, s8 TH_EDCCA_HL_diff, s8 TH_L2H_ini_mode2, s8 TH_EDCCA_HL_diff_mode2, u8 EDCCA_enable);
void rtw_odm_get_perpkt_rssi(void *sel, _adapter *adapter);
void rtw_odm_acquirespinlock(_adapter *adapter,	RT_SPINLOCK_TYPE type);
void rtw_odm_releasespinlock(_adapter *adapter,	RT_SPINLOCK_TYPE type);

#ifdef CONFIG_DFS_MASTER
VOID rtw_odm_radar_detect_reset(_adapter *adapter);
VOID rtw_odm_radar_detect_disable(_adapter *adapter);
VOID rtw_odm_radar_detect_enable(_adapter *adapter);
BOOLEAN rtw_odm_radar_detect(_adapter *adapter);
#endif /* CONFIG_DFS_MASTER */
#endif // __RTW_ODM_H__

