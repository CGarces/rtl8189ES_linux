// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */

#ifndef	__PHYDMCFOTRACK_H__
#define    __PHYDMCFOTRACK_H__

#define CFO_TRACKING_VERSION	"1.1" /*2015.02.09*/

#define		CFO_TH_XTAL_HIGH			20			// kHz
#define		CFO_TH_XTAL_LOW			10			// kHz
#define		CFO_TH_ATC					80			// kHz

typedef struct _CFO_TRACKING_
{
	BOOLEAN			bATCStatus;
	BOOLEAN			largeCFOHit;
	BOOLEAN			bAdjust;
	u1Byte			CrystalCap;
	u1Byte			DefXCap;
	int				CFO_tail[2];
	int				CFO_ave_pre;
	u4Byte			packetCount;
	u4Byte			packetCount_pre;

	BOOLEAN			bForceXtalCap;
	BOOLEAN			bReset;
}CFO_TRACKING, *PCFO_TRACKING;

VOID
ODM_CfoTrackingReset(
	IN		PVOID					pDM_VOID
);

VOID
ODM_CfoTrackingInit(
	IN		PVOID					pDM_VOID
);

VOID
ODM_CfoTracking(
	IN		PVOID					pDM_VOID
);

VOID
ODM_ParsingCFO(
	IN		PVOID					pDM_VOID,
	IN		PVOID					pPktinfo_VOID,
	IN     	s1Byte* 					pcfotail
);

#endif