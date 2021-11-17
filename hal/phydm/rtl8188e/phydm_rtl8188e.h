// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef	__ODM_RTL8188E_H__
#define __ODM_RTL8188E_H__


#define	MAIN_ANT_CG_TRX	1
#define	AUX_ANT_CG_TRX	0
#define	MAIN_ANT_CGCS_RX	0
#define	AUX_ANT_CGCS_RX	1

VOID
ODM_DIG_LowerBound_88E(
	IN		PDM_ODM_T		pDM_Odm
);




#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

#define SwAntDivResetBeforeLink		ODM_SwAntDivResetBeforeLink

VOID ODM_SwAntDivResetBeforeLink(IN	PDM_ODM_T	pDM_Odm);

VOID
ODM_SetTxAntByTxInfo_88E(
	IN		PDM_ODM_T		pDM_Odm,
	IN		pu1Byte			pDesc,
	IN		u1Byte			macId	
);
#else// (DM_ODM_SUPPORT_TYPE == ODM_AP)
VOID
ODM_SetTxAntByTxInfo_88E(
	IN		PDM_ODM_T		pDM_Odm	
);
#endif

VOID
odm_PrimaryCCA_Init(
	IN		PDM_ODM_T		pDM_Odm);

BOOLEAN
ODM_DynamicPrimaryCCA_DupRTS(
	IN		PDM_ODM_T		pDM_Odm);

VOID
odm_DynamicPrimaryCCA(
	IN		PDM_ODM_T		pDM_Odm);

#endif

