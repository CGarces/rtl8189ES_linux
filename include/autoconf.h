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

#define DRV_NAME "rtl8189fs"

#define PLATFORM_LINUX

#ifndef CONFIG_MP_INCLUDED
	#define CONFIG_MP_INCLUDED
#endif

#define MP_DRIVER	1
#define CONFIG_MP_IWPRIV_SUPPORT

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

