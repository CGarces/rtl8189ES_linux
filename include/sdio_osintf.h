// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef __SDIO_OSINTF_H__
#define __SDIO_OSINTF_H__



u8 sd_hal_bus_init(PADAPTER padapter);
u8 sd_hal_bus_deinit(PADAPTER padapter);
void sd_c2h_hdl(PADAPTER padapter);

#ifdef PLATFORM_OS_CE
extern NDIS_STATUS ce_sd_get_dev_hdl(PADAPTER padapter);
SD_API_STATUS ce_sd_int_callback(SD_DEVICE_HANDLE hDevice, PADAPTER padapter);
extern void sd_setup_irs(PADAPTER padapter);
#endif

#endif

