// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef __SDIO_OPS_H__
#define __SDIO_OPS_H__

#include <sdio_ops_linux.h>

extern void sdio_set_intf_ops(_adapter *padapter,struct _io_ops *pops);
	
//extern void sdio_func1cmd52_read(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *rmem);
//extern void sdio_func1cmd52_write(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *wmem);
extern u8 SdioLocalCmd52Read1Byte(PADAPTER padapter, u32 addr);
extern void SdioLocalCmd52Write1Byte(PADAPTER padapter, u32 addr, u8 v);
extern s32 _sdio_local_read(PADAPTER padapter, u32 addr, u32 cnt, u8 *pbuf);
extern s32 sdio_local_read(PADAPTER padapter, u32 addr, u32 cnt, u8 *pbuf);
extern s32 _sdio_local_write(PADAPTER padapter, u32 addr, u32 cnt, u8 *pbuf);
extern s32 sdio_local_write(PADAPTER padapter, u32 addr, u32 cnt, u8 *pbuf);

u32 _sdio_read32(PADAPTER padapter, u32 addr);
s32 _sdio_write32(PADAPTER padapter, u32 addr, u32 val);

extern void sd_int_hdl(PADAPTER padapter);
extern u8 CheckIPSStatus(PADAPTER padapter);

#ifdef CONFIG_RTL8188E
extern void InitInterrupt8188ESdio(PADAPTER padapter);
extern void EnableInterrupt8188ESdio(PADAPTER padapter);
extern void DisableInterrupt8188ESdio(PADAPTER padapter);
extern void UpdateInterruptMask8188ESdio(PADAPTER padapter, u32 AddMSR, u32 RemoveMSR);
extern u8 HalQueryTxBufferStatus8189ESdio(PADAPTER padapter);
extern u8 HalQueryTxOQTBufferStatus8189ESdio(PADAPTER padapter);
extern void ClearInterrupt8188ESdio(PADAPTER padapter);
#endif // CONFIG_RTL8188E

#ifdef CONFIG_RTL8821A
extern void InitInterrupt8821AS(PADAPTER padapter);
extern void EnableInterrupt8821AS(PADAPTER padapter);
extern void DisableInterrupt8821AS(PADAPTER padapter);
extern u8 HalQueryTxBufferStatus8821AS(PADAPTER padapter);
extern u8 HalQueryTxOQTBufferStatus8821ASdio(PADAPTER padapter);
#endif // CONFIG_RTL8188E

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
extern u8 RecvOnePkt(PADAPTER padapter, u32 size);
#endif // CONFIG_WOWLAN
#ifdef CONFIG_RTL8723B
extern void InitInterrupt8723BSdio(PADAPTER padapter);
extern void InitSysInterrupt8723BSdio(PADAPTER padapter);
extern void EnableInterrupt8723BSdio(PADAPTER padapter);
extern void DisableInterrupt8723BSdio(PADAPTER padapter);
extern u8 HalQueryTxBufferStatus8723BSdio(PADAPTER padapter);
extern u8 HalQueryTxOQTBufferStatus8723BSdio(PADAPTER padapter);
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
extern void DisableInterruptButCpwm28723BSdio(PADAPTER padapter);
extern void ClearInterrupt8723BSdio(PADAPTER padapter);
#endif //CONFIG_WOWLAN
#endif

#ifdef CONFIG_RTL8703B
extern void InitInterrupt8703BSdio(PADAPTER padapter);
extern void InitSysInterrupt8703BSdio(PADAPTER padapter);
extern void EnableInterrupt8703BSdio(PADAPTER padapter);
extern void DisableInterrupt8703BSdio(PADAPTER padapter);
extern u8 HalQueryTxBufferStatus8703BSdio(PADAPTER padapter);
extern u8 HalQueryTxOQTBufferStatus8703BSdio(PADAPTER padapter);
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
extern void DisableInterruptButCpwm28703BSdio(PADAPTER padapter);
extern void ClearInterrupt8703BSdio(PADAPTER padapter);
#endif //CONFIG_WOWLAN
#endif

#ifdef CONFIG_RTL8188F
extern void InitInterrupt8188FSdio(PADAPTER padapter);
extern void InitSysInterrupt8188FSdio(PADAPTER padapter);
extern void EnableInterrupt8188FSdio(PADAPTER padapter);
extern void DisableInterrupt8188FSdio(PADAPTER padapter);
extern u8 HalQueryTxBufferStatus8188FSdio(PADAPTER padapter);
extern u8 HalQueryTxOQTBufferStatus8188FSdio(PADAPTER padapter);
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
extern void DisableInterruptButCpwm28188FSdio(PADAPTER padapter);
extern void ClearInterrupt8188FSdio(PADAPTER padapter);
#endif /* defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN) */
#endif

#endif // !__SDIO_OPS_H__

