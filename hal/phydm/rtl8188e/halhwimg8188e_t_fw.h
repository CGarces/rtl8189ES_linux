// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */

/*Image2HeaderVersion: 2.7*/
#if (RTL8188E_T_SUPPORT == 1)
#ifndef __INC_MP_FW_HW_IMG_8188E_T_H
#define __INC_MP_FW_HW_IMG_8188E_T_H


/******************************************************************************
*                           FW_AP.TXT
******************************************************************************/

void
ODM_ReadFirmware_MP_8188E_T_FW_AP(
	IN   PDM_ODM_T    pDM_Odm,
	OUT  u1Byte       *pFirmware,
	OUT  u4Byte       *pFirmwareSize
);

/******************************************************************************
*                           FW_NIC.TXT
******************************************************************************/

void
ODM_ReadFirmware_MP_8188E_T_FW_NIC(
	IN   PDM_ODM_T    pDM_Odm,
	OUT  u1Byte       *pFirmware,
	OUT  u4Byte       *pFirmwareSize
);

/******************************************************************************
*                           FW_NIC_89EM.TXT
******************************************************************************/

void
ODM_ReadFirmware_MP_8188E_T_FW_NIC_89EM(
	IN   PDM_ODM_T    pDM_Odm,
	OUT  u1Byte       *pFirmware,
	OUT  u4Byte       *pFirmwareSize
);

/******************************************************************************
*                           FW_WoWLAN.TXT
******************************************************************************/

void
ODM_ReadFirmware_MP_8188E_T_FW_WoWLAN(
	IN   PDM_ODM_T    pDM_Odm,
	OUT  u1Byte       *pFirmware,
	OUT  u4Byte       *pFirmwareSize
);

#endif
#endif /* end of HWIMG_SUPPORT*/

