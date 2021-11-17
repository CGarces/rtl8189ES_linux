// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */
#ifndef _RTW_IOCTL_QUERY_H_
#define _RTW_IOCTL_QUERY_H_


#ifdef PLATFORM_WINDOWS

u8 query_802_11_capability(_adapter*	padapter,u8*	pucBuf,u32 *	pulOutLen);
u8 query_802_11_association_information (_adapter * padapter, PNDIS_802_11_ASSOCIATION_INFORMATION pAssocInfo);

#endif


#endif

