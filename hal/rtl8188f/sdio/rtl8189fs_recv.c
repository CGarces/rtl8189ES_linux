/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
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
#define _RTL8188FS_RECV_C_

#include <rtl8188f_hal.h>


static s32 initrecvbuf(struct recv_buf *precvbuf, PADAPTER padapter)
{
	_rtw_init_listhead(&precvbuf->list);
	_rtw_spinlock_init(&precvbuf->recvbuf_lock);

	precvbuf->adapter = padapter;

	return _SUCCESS;
}

static void freerecvbuf(struct recv_buf *precvbuf)
{
	_rtw_spinlock_free(&precvbuf->recvbuf_lock);
}

s32 rtl8188fs_recv_hdl(_adapter *padapter)
{
	PHAL_DATA_TYPE		pHalData;
	struct recv_priv		*precvpriv;
	struct recv_buf *precvbuf;
	union recv_frame		*precvframe;
	struct recv_frame_hdr	*phdr;
	struct rx_pkt_attrib	*pattrib;
	u8	*ptr;
	u32 pkt_len, pkt_offset, skb_len, alloc_sz;
	_pkt *pkt_copy = NULL;
	u8 shift_sz = 0, rx_report_sz = 0;

	pHalData = GET_HAL_DATA(padapter);
	precvpriv = &padapter->recvpriv;

	do {
		precvbuf = rtw_dequeue_recvbuf(&precvpriv->recv_buf_pending_queue);
		if (NULL == precvbuf)
			break;

		ptr = precvbuf->pdata;

		while (ptr < precvbuf->ptail) {
			precvframe = rtw_alloc_recvframe(&precvpriv->free_recv_queue);
			if (precvframe == NULL) {
				RTW_INFO("%s: no enough recv frame!\n", __FUNCTION__);
				rtw_enqueue_recvbuf_to_head(precvbuf, &precvpriv->recv_buf_pending_queue);
				return RTW_RFRAME_UNAVAIL;
			}

			/* rx desc parsing */
			rtl8188f_query_rx_desc_status(precvframe, ptr);

			pattrib = &precvframe->u.hdr.attrib;

			/* fix Hardware RX data error, drop whole recv_buffer */
			if ((!(pHalData->ReceiveConfig & RCR_ACRC32)) && pattrib->crc_err) {
#if !(MP_DRIVER == 1)
				RTW_INFO("%s()-%d: RX Warning! rx CRC ERROR !!\n", __FUNCTION__, __LINE__);
#endif
				rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
				break;
			}

			rx_report_sz = RXDESC_SIZE + pattrib->drvinfo_sz;
			pkt_offset = rx_report_sz + pattrib->shift_sz + pattrib->pkt_len;

			if ((ptr + pkt_offset) > precvbuf->ptail) {
				RTW_INFO("%s()-%d: : next pkt len(%p,%d) exceed ptail(%p)!\n", __FUNCTION__, __LINE__, ptr, pkt_offset, precvbuf->ptail);
				rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
				break;
			}

			if ((pattrib->crc_err) || (pattrib->icv_err)) {
#ifdef CONFIG_MP_INCLUDED
				if (padapter->registrypriv.mp_mode == 1) {
					if ((check_fwstate(&padapter->mlmepriv, WIFI_MP_STATE) == _TRUE)) { /* &&(padapter->mppriv.check_mp_pkt == 0)) */
						if (pattrib->crc_err == 1)
							padapter->mppriv.rx_crcerrpktcount++;
					}
				} else
#endif
				{
					RTW_INFO("%s: crc_err=%d icv_err=%d, skip!\n", __FUNCTION__, pattrib->crc_err, pattrib->icv_err);
				}
				rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
			} else {
				//	Modified by Albert 20101213
				//	For 8 bytes IP header alignment.
				if (pattrib->qos)	//	Qos data, wireless lan header length is 26
				{
					shift_sz = 6;
				}
				else
				{
					shift_sz = 0;
				}

				skb_len = pattrib->pkt_len;

				// for first fragment packet, driver need allocate 1536+drvinfo_sz+RXDESC_SIZE to defrag packet.
				// modify alloc_sz for recvive crc error packet by thomas 2011-06-02
				if((pattrib->mfrag == 1)&&(pattrib->frag_num == 0)){
					//alloc_sz = 1664;	//1664 is 128 alignment.
					if(skb_len <= 1650)
						alloc_sz = 1664;
					else
						alloc_sz = skb_len + 14;
				}
				else {
					alloc_sz = skb_len;
					//	6 is for IP header 8 bytes alignment in QoS packet case.
					//	8 is for skb->data 4 bytes alignment.
					alloc_sz += 14;
				}

				pkt_copy = rtw_skb_alloc(alloc_sz);

				if (pkt_copy)
				{
					pkt_copy->dev = padapter->pnetdev;
					precvframe->u.hdr.pkt = pkt_copy;
					skb_reserve( pkt_copy, 8 - ((SIZE_PTR)( pkt_copy->data ) & 7 ));//force pkt_copy->data at 8-byte alignment address
					skb_reserve( pkt_copy, shift_sz );//force ip_hdr at 8-byte alignment address according to shift_sz.
					_rtw_memcpy(pkt_copy->data, (ptr + rx_report_sz + pattrib->shift_sz), skb_len);
					precvframe->u.hdr.rx_head = pkt_copy->head;
					precvframe->u.hdr.rx_data = precvframe->u.hdr.rx_tail = pkt_copy->data;
					precvframe->u.hdr.rx_end = skb_end_pointer(pkt_copy);
				}
				else
				{
					if((pattrib->mfrag == 1)&&(pattrib->frag_num == 0))
					{
						DBG_8192C("%s: alloc_skb fail, drop frag frame\n", __FUNCTION__);
						rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
						break;
					}

					precvframe->u.hdr.pkt = rtw_skb_clone(precvbuf->pskb);
					if(precvframe->u.hdr.pkt)
					{
						_pkt	*pkt_clone = precvframe->u.hdr.pkt;

						pkt_clone->data = ptr + rx_report_sz + pattrib->shift_sz;
						skb_reset_tail_pointer(pkt_clone);
						precvframe->u.hdr.rx_head = precvframe->u.hdr.rx_data = precvframe->u.hdr.rx_tail
							= pkt_clone->data;
						precvframe->u.hdr.rx_end =	pkt_clone->data + skb_len;
					}
					else
					{
						DBG_8192C("%s: rtw_skb_clone fail\n", __FUNCTION__);
						rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
						break;
					}
				}

				recvframe_put(precvframe, skb_len);
				/*recvframe_pull(precvframe, drvinfo_sz + RXDESC_SIZE); */

				if (pHalData->ReceiveConfig & RCR_APPFCS)
					recvframe_pull_tail(precvframe, IEEE80211_FCS_LEN);

				/* move to drv info position */
				ptr += RXDESC_SIZE;

				/* update drv info */
				if (pHalData->ReceiveConfig & RCR_APP_BA_SSN) {
					/* rtl8188s_update_bassn(padapter, pdrvinfo); */
					ptr += 4;
				}

				if (pattrib->pkt_rpt_type == NORMAL_RX) {
					/* skip the rx packet with abnormal length */
					if (pattrib->pkt_len < 14 || pattrib->pkt_len > 8192) {
						RTW_INFO("skip abnormal rx packet(%d)\n", pattrib->pkt_len);
						rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
						break;
					}

					if (pattrib->physt)
						rx_query_phy_status(precvframe, ptr);

					if (rtw_recv_entry(precvframe) != _SUCCESS) {
						RT_TRACE(_module_rtl871x_recv_c_, _drv_dump_, ("%s: rtw_recv_entry(precvframe) != _SUCCESS\n",__FUNCTION__));
					}
				} else {
					if (pattrib->pkt_rpt_type == C2H_PACKET)
						rtl8188f_c2h_packet_handler(padapter, precvframe->u.hdr.rx_data, pattrib->pkt_len);
					else
						RTW_INFO("%s: [WARNNING] RX type(%d) not be handled!\n",
							__FUNCTION__, pattrib->pkt_rpt_type);
					rtw_free_recvframe(precvframe, &precvpriv->free_recv_queue);
				}
			}

			pkt_offset = _RND8(pkt_offset);
			precvbuf->pdata += pkt_offset;
			ptr = precvbuf->pdata;
			precvframe = NULL;
			pkt_copy = NULL;
		}

		rtw_enqueue_recvbuf(precvbuf, &precvpriv->free_recv_buf_queue);
	} while (1);

	return _SUCCESS;
}

static void rtl8188fs_recv_tasklet(void *priv)
{
	_adapter *adapter = (_adapter *)priv;
	s32 ret;

	ret = rtl8188fs_recv_hdl(adapter);
	if (ret == RTW_RFRAME_UNAVAIL
		|| ret == RTW_RFRAME_PKT_UNAVAIL
	) {
		/* schedule again and hope recvframe/packet is available next time. */
		#ifdef PLATFORM_LINUX
		tasklet_schedule(&adapter->recvpriv.recv_tasklet);
		#endif
	}
}

/*
 * Initialize recv private variable for hardware dependent
 * 1. recv buf
 * 2. recv tasklet
 *
 */
s32 rtl8188fs_init_recv_priv(PADAPTER padapter)
{
	s32			res;
	u32			i, n;
	struct recv_priv	*precvpriv;
	struct recv_buf		*precvbuf;


	res = _SUCCESS;
	precvpriv = &padapter->recvpriv;

	//3 1. init recv buffer
	_rtw_init_queue(&precvpriv->free_recv_buf_queue);
	_rtw_init_queue(&precvpriv->recv_buf_pending_queue);

	n = NR_RECVBUFF * sizeof(struct recv_buf) + 4;
	precvpriv->pallocated_recv_buf = rtw_zmalloc(n);
	if (precvpriv->pallocated_recv_buf == NULL) {
		res = _FAIL;
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("alloc recv_buf fail!\n"));
		goto exit;
	}

	precvpriv->precv_buf = (u8*)N_BYTE_ALIGMENT((SIZE_PTR)(precvpriv->pallocated_recv_buf), 4);

	// init each recv buffer
	precvbuf = (struct recv_buf*)precvpriv->precv_buf;
	for (i = 0; i < NR_RECVBUFF; i++) {
		res = initrecvbuf(precvbuf, padapter);
		if (res == _FAIL)
			break;

		res = rtw_os_recvbuf_resource_alloc(padapter, precvbuf);
		if (res == _FAIL) {
			freerecvbuf(precvbuf);
			break;
		}

		if (precvbuf->pskb == NULL) {
			SIZE_PTR tmpaddr=0;
			SIZE_PTR alignment=0;

			precvbuf->pskb = rtw_skb_alloc(MAX_RECVBUF_SZ + RECVBUFF_ALIGN_SZ);

			if(precvbuf->pskb) {
				precvbuf->pskb->dev = padapter->pnetdev;

				tmpaddr = (SIZE_PTR)precvbuf->pskb->data;
				alignment = tmpaddr & (RECVBUFF_ALIGN_SZ-1);
				skb_reserve(precvbuf->pskb, (RECVBUFF_ALIGN_SZ - alignment));
			}

			if (precvbuf->pskb == NULL)
				DBG_871X("%s: alloc_skb fail!\n", __FUNCTION__);
		}

		rtw_list_insert_tail(&precvbuf->list, &precvpriv->free_recv_buf_queue.queue);

		precvbuf++;
	}
	precvpriv->free_recv_buf_queue_cnt = i;

	if (res == _FAIL)
		goto initbuferror;

	//3 2. init tasklet
#ifdef PLATFORM_LINUX
	tasklet_init(&precvpriv->recv_tasklet,
	     (void(*)(unsigned long))rtl8188fs_recv_tasklet,
	     (unsigned long)padapter);
#endif

	goto exit;

initbuferror:
	precvbuf = (struct recv_buf*)precvpriv->precv_buf;
	if (precvbuf) {
		n = precvpriv->free_recv_buf_queue_cnt;
		precvpriv->free_recv_buf_queue_cnt = 0;
		for (i = 0; i < n ; i++)
		{
			rtw_list_delete(&precvbuf->list);
			rtw_os_recvbuf_resource_free(padapter, precvbuf);
			freerecvbuf(precvbuf);
			precvbuf++;
		}
		precvpriv->precv_buf = NULL;
	}

	if (precvpriv->pallocated_recv_buf) {
		n = NR_RECVBUFF * sizeof(struct recv_buf) + 4;
		rtw_mfree(precvpriv->pallocated_recv_buf, n);
		precvpriv->pallocated_recv_buf = NULL;
	}

exit:
	return res;
}

/*
 * Free recv private variable of hardware dependent
 * 1. recv buf
 * 2. recv tasklet
 *
 */
void rtl8188fs_free_recv_priv(PADAPTER padapter)
{
	u32			i, n;
	struct recv_priv	*precvpriv;
	struct recv_buf		*precvbuf;


	precvpriv = &padapter->recvpriv;

	//3 1. kill tasklet
#ifdef PLATFORM_LINUX
	tasklet_kill(&precvpriv->recv_tasklet);
#endif

	//3 2. free all recv buffers
	precvbuf = (struct recv_buf*)precvpriv->precv_buf;
	if (precvbuf) {
		n = NR_RECVBUFF;
		precvpriv->free_recv_buf_queue_cnt = 0;
		for (i = 0; i < n ; i++)
		{
			rtw_list_delete(&precvbuf->list);
			rtw_os_recvbuf_resource_free(padapter, precvbuf);
			freerecvbuf(precvbuf);
			precvbuf++;
		}
		precvpriv->precv_buf = NULL;
	}

	if (precvpriv->pallocated_recv_buf) {
		n = NR_RECVBUFF * sizeof(struct recv_buf) + 4;
		rtw_mfree(precvpriv->pallocated_recv_buf, n);
		precvpriv->pallocated_recv_buf = NULL;
	}
}

