// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef __DRV_TYPES_SDIO_H__
#define __DRV_TYPES_SDIO_H__

// SDIO Header Files
#ifdef PLATFORM_LINUX
	#include <linux/mmc/sdio_func.h> 
	#include <linux/mmc/sdio_ids.h>

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN) || defined(CONFIG_PLATFORM_SPRD)
	#include <linux/mmc/host.h>
	#include <linux/mmc/card.h>
#endif

#ifdef CONFIG_PLATFORM_SPRD
	#include <linux/gpio.h>
	#include <custom_gpio.h>
#endif // CONFIG_PLATFORM_SPRD
#endif

#ifdef PLATFORM_OS_XP
#include <wdm.h>
#include <ntddsd.h>
#endif

#ifdef PLATFORM_OS_CE
#include <sdcardddk.h>
#endif


typedef struct sdio_data
{
	u8  func_number;
 
	u8  tx_block_mode;
	u8  rx_block_mode;
	u32 block_transfer_len;

#ifdef PLATFORM_LINUX
	struct sdio_func	 *func;
	_thread_hdl_ sys_sdio_irq_thd;
#endif

#ifdef PLATFORM_OS_XP
	PDEVICE_OBJECT				pphysdevobj;
	PDEVICE_OBJECT				pfuncdevobj;
	PDEVICE_OBJECT				pnextdevobj;
	SDBUS_INTERFACE_STANDARD	sdbusinft;
	u8							nextdevstacksz;
#endif

#ifdef PLATFORM_OS_CE
	SD_DEVICE_HANDLE			hDevice;
	SD_CARD_RCA					sd_rca;
	SD_CARD_INTERFACE			card_intf;
	BOOLEAN						enableIsarWithStatus;
	WCHAR						active_path[MAX_ACTIVE_REG_PATH];
	SD_HOST_BLOCK_CAPABILITY	sd_host_blk_cap;
#endif
} SDIO_DATA, *PSDIO_DATA;

#endif

