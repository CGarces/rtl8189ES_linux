// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef __RTL8188E_LED_H__
#define __RTL8188E_LED_H__


//================================================================================
// Interface to manipulate LED objects.
//================================================================================
#ifdef CONFIG_USB_HCI
void rtl8188eu_InitSwLeds(PADAPTER padapter);
void rtl8188eu_DeInitSwLeds(PADAPTER padapter);
#endif
#ifdef CONFIG_PCI_HCI
void rtl8188ee_InitSwLeds(PADAPTER padapter);
void rtl8188ee_DeInitSwLeds(PADAPTER padapter);
#endif
#if defined (CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
void rtl8188es_InitSwLeds(PADAPTER padapter);
void rtl8188es_DeInitSwLeds(PADAPTER padapter);
#endif

#endif

