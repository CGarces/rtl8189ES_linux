// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef __RTL8188E_RF_H__
#define __RTL8188E_RF_H__



int	PHY_RF6052_Config8188E(	IN	PADAPTER		Adapter	);
void		rtl8188e_RF_ChangeTxPath(	IN	PADAPTER	Adapter, 
										IN	u16		DataRate);
void		rtl8188e_PHY_RF6052SetBandwidth(	
										IN	PADAPTER				Adapter,
										IN	CHANNEL_WIDTH		Bandwidth);

#endif//__RTL8188E_RF_H__

