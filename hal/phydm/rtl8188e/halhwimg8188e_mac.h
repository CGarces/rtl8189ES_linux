// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */

/*Image2HeaderVersion: 2.14*/
#if (RTL8188E_SUPPORT == 1)
#ifndef __INC_MP_MAC_HW_IMG_8188E_H
#define __INC_MP_MAC_HW_IMG_8188E_H


/******************************************************************************
*                           MAC_REG.TXT
******************************************************************************/

void
ODM_ReadAndConfig_MP_8188E_MAC_REG(/* TC: Test Chip, MP: MP Chip*/
	IN   PDM_ODM_T  pDM_Odm
);
u4Byte ODM_GetVersion_MP_8188E_MAC_REG(void);

#endif
#endif /* end of HWIMG_SUPPORT*/

