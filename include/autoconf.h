/******************************************************************************
 *
 * Copyright(c) 2007 - 2014 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
/*
 * Public General Config
 */
#define AUTOCONF_INCLUDED

#define RTL871X_MODULE_NAME "8189FS"
#define DRV_NAME "rtl8189fs"

#ifndef CONFIG_RTL8188F
#define CONFIG_RTL8188F
#endif

#define PLATFORM_LINUX


/*
 * Wi-Fi Functions Config
 */
#define CONFIG_80211N_HT
#define CONFIG_RECV_REORDERING_CTRL

//#define CONFIG_IOCTL_CFG80211		// Set from Makefile
#ifdef CONFIG_IOCTL_CFG80211
	/*
	 * Indecate new sta asoc through cfg80211_new_sta
	 * If kernel version >= 3.2 or
	 * version < 3.2 but already apply cfg80211 patch,
	 * RTW_USE_CFG80211_STA_EVENT must be defiend!
	 */
	//#define RTW_USE_CFG80211_STA_EVENT /* Indecate new sta asoc through cfg80211_new_sta */
	//#define CONFIG_DEBUG_CFG80211
	#define CONFIG_SET_SCAN_DENY_TIMER
	/*#define SUPPLICANT_RTK_VERSION_LOWER_THAN_JB42*/ /* wpa_supplicant realtek version <= jb42 will be defined this */
#endif


//#define CONFIG_CONCURRENT_MODE	// Set from Makefile
#ifdef CONFIG_CONCURRENT_MODE
	//#define CONFIG_HWPORT_SWAP				// Port0->Sec , Port1 -> Pri
	#define CONFIG_RUNTIME_PORT_SWITCH
	#ifndef CONFIG_RUNTIME_PORT_SWITCH
		#define CONFIG_TSF_RESET_OFFLOAD			// For 2 PORT TSF SYNC.
	#endif
	//#define DBG_RUNTIME_PORT_SWITCH
	#define CONFIG_SCAN_BACKOP
#endif // CONFIG_CONCURRENT_MODE

#define CONFIG_LAYER2_ROAMING
#define CONFIG_LAYER2_ROAMING_RESUME

//#define CONFIG_80211D


/*
 * Hareware/Firmware Related Config
 */
//#define CONFIG_ANTENNA_DIVERSITY	// Set from Makefile
//#define SUPPORT_HW_RFOFF_DETECTED

//#define CONFIG_SW_LED

#define CONFIG_XMIT_ACK
#ifdef CONFIG_XMIT_ACK
	#define CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#endif

#define CONFIG_C2H_PACKET_EN

#define CONFIG_RF_GAIN_OFFSET

#define DISABLE_BB_RF	0

#define RTW_NOTCH_FILTER 0 /* 0:Disable, 1:Enable, */

/*
 * Interface Related Config
 */
#define CONFIG_SDIO_CHK_HCI_RESUME
#define CONFIG_TX_AGGREGATION
#define CONFIG_SDIO_RX_COPY
#define CONFIG_XMIT_THREAD_MODE
//#define CONFIG_SDIO_TX_ENABLE_AVAL_INT

/* #define CONFIG_RECV_THREAD_MODE */

/*
 * Others
 */

#define CONFIG_SKB_COPY	//for amsdu

#define CONFIG_NEW_SIGNAL_STAT_PROCESS

#define CONFIG_EMBEDDED_FWIMG
//#define CONFIG_FILE_FWIMG

#define CONFIG_LONG_DELAY_ISSUE
//#define CONFIG_PATCH_JOIN_WRONG_CHANNEL
#define CONFIG_ATTEMPT_TO_FIX_AP_BEACON_ERROR


/*
 * Auto Config Section
 */
#ifndef CONFIG_MP_INCLUDED
	#define CONFIG_MP_INCLUDED
#endif

#define MP_DRIVER	1
#define CONFIG_MP_IWPRIV_SUPPORT

	#define CONFIG_IPS
	#define CONFIG_LPS

	#define CONFIG_LPS_LCLK

	#ifdef CONFIG_LPS
		#define CONFIG_CHECK_LEAVE_LPS
		//#define CONFIG_LPS_SLOW_TRANSITION
	#endif

	#ifdef CONFIG_LPS_LCLK
		#define CONFIG_DETECT_CPWM_BY_POLLING
		#define CONFIG_LPS_RPWM_TIMER
		#if defined(CONFIG_LPS_RPWM_TIMER) || defined(CONFIG_DETECT_CPWM_BY_POLLING)
			#define LPS_RPWM_WAIT_MS 300
		#endif
		#define CONFIG_LPS_LCLK_WD_TIMER /* Watch Dog timer in LPS LCLK */
	#endif

	#ifdef CONFIG_IPS
		#define CONFIG_IPS_CHECK_IN_WD /* Do IPS Check in WatchDog */
	#endif

#define BT_30_SUPPORT 0

/*
 * Debug Related Config
 */
#define CONFIG_DEBUG /* DBG_871X, etc... */

#ifdef CONFIG_DEBUG
#define DBG	1	// for ODM & BTCOEX debug
//#define CONFIG_DEBUG_RTL871X /* RT_TRACE, RT_PRINT_DATA, _func_enter_, _func_exit_ */
#else // !CONFIG_DEBUG
#define DBG	0	// for ODM & BTCOEX debug
#endif // !CONFIG_DEBUG

#define CONFIG_PROC_DEBUG

#define DBG_CONFIG_ERROR_DETECT
//#define DBG_XMIT_BUF
//#define DBG_XMIT_BUF_EXT
//#define DBG_CHECK_FW_PS_STATE
//#define DBG_CHECK_FW_PS_STATE_H2C
//#define CONFIG_FW_C2H_DEBUG 

