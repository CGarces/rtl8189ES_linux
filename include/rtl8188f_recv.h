/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
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
#ifndef __RTL8188F_RECV_H__
#define __RTL8188F_RECV_H__

#define MAX_RECVBUF_SZ (RX_DMA_BOUNDARY_8188F + 1)

// Rx smooth factor
#define	Rx_Smooth_Factor (20)

s32 rtl8188fs_init_recv_priv(PADAPTER padapter);
void rtl8188fs_free_recv_priv(PADAPTER padapter);
s32 rtl8188fs_recv_hdl(_adapter *padapter);

void rtl8188f_query_rx_desc_status(union recv_frame *precvframe, u8 *pdesc);

#endif /* __RTL8188F_RECV_H__ */

