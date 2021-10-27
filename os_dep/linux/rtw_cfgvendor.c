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

#include <drv_types.h>

#ifdef CONFIG_IOCTL_CFG80211

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) || defined(RTW_VENDOR_EXT_SUPPORT)

/*
#include <linux/kernel.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/sched.h>
#include <linux/etherdevice.h>
#include <linux/wireless.h>
#include <linux/ieee80211.h>
#include <linux/wait.h>
#include <net/cfg80211.h>
*/

#include <net/rtnetlink.h>

#ifdef DBG_MEM_ALLOC
extern bool match_mstat_sniff_rules(const enum mstat_f flags, const size_t size);
struct sk_buff *dbg_rtw_cfg80211_vendor_event_alloc(struct wiphy *wiphy, int len, int event_id, gfp_t gfp
	, const enum mstat_f flags, const char *func, const int line)
{
	_adapter *padapter = wiphy_to_adapter(wiphy);
	struct wireless_dev *wdev = padapter->rtw_wdev;

	struct sk_buff *skb;
	unsigned int truesize = 0;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))
	skb = cfg80211_vendor_event_alloc(wiphy, len, event_id, gfp);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, len, event_id, gfp);
#endif

	if(skb)
		truesize = skb->truesize;

	if(!skb || truesize < len || match_mstat_sniff_rules(flags, truesize))
		DBG_871X("DBG_MEM_ALLOC %s:%d %s(%d), skb:%p, truesize=%u\n", func, line, __FUNCTION__, len, skb, truesize);

	rtw_mstat_update(
		flags
		, skb ? MSTAT_ALLOC_SUCCESS : MSTAT_ALLOC_FAIL
		, truesize
	);

	return skb;
}

void dbg_rtw_cfg80211_vendor_event(struct sk_buff *skb, gfp_t gfp
	, const enum mstat_f flags, const char *func, const int line)
{
	unsigned int truesize = skb->truesize;

	if(match_mstat_sniff_rules(flags, truesize))
		DBG_871X("DBG_MEM_ALLOC %s:%d %s, truesize=%u\n", func, line, __FUNCTION__, truesize);

	cfg80211_vendor_event(skb, gfp);

	rtw_mstat_update(
		flags
		, MSTAT_FREE
		, truesize
	);
}

struct sk_buff *dbg_rtw_cfg80211_vendor_cmd_alloc_reply_skb(struct wiphy *wiphy, int len
	, const enum mstat_f flags, const char *func, const int line)
{
	struct sk_buff *skb;
	unsigned int truesize = 0;

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len);

	if(skb)
		truesize = skb->truesize;

	if(!skb || truesize < len || match_mstat_sniff_rules(flags, truesize))
		DBG_871X("DBG_MEM_ALLOC %s:%d %s(%d), skb:%p, truesize=%u\n", func, line, __FUNCTION__, len, skb, truesize);

	rtw_mstat_update(
		flags
		, skb ? MSTAT_ALLOC_SUCCESS : MSTAT_ALLOC_FAIL
		, truesize
	);

	return skb;
}

int dbg_rtw_cfg80211_vendor_cmd_reply(struct sk_buff *skb
	, const enum mstat_f flags, const char *func, const int line)
{
	unsigned int truesize = skb->truesize;
	int ret;

	if(match_mstat_sniff_rules(flags, truesize))
		DBG_871X("DBG_MEM_ALLOC %s:%d %s, truesize=%u\n", func, line, __FUNCTION__, truesize);

	ret = cfg80211_vendor_cmd_reply(skb);

	rtw_mstat_update(
		flags
		, MSTAT_FREE
		, truesize
	);

	return ret;
}

#define rtw_cfg80211_vendor_event_alloc(wiphy, len, event_id, gfp) \
	dbg_rtw_cfg80211_vendor_event_alloc(wiphy, len, event_id, gfp, MSTAT_FUNC_CFG_VENDOR|MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
	
#define rtw_cfg80211_vendor_event(skb, gfp) \
	dbg_rtw_cfg80211_vendor_event(skb, gfp, MSTAT_FUNC_CFG_VENDOR|MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
	
#define rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len) \
	dbg_rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len, MSTAT_FUNC_CFG_VENDOR|MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)

#define rtw_cfg80211_vendor_cmd_reply(skb) \
		dbg_rtw_cfg80211_vendor_cmd_reply(skb, MSTAT_FUNC_CFG_VENDOR|MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#else

struct sk_buff *rtw_cfg80211_vendor_event_alloc(
		struct wiphy *wiphy, int len, int event_id, gfp_t gfp)
{
	_adapter *padapter = wiphy_to_adapter(wiphy);
	struct wireless_dev *wdev = padapter->rtw_wdev;
	struct sk_buff *skb;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))
	skb = cfg80211_vendor_event_alloc(wiphy, len, event_id, gfp);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, len, event_id, gfp);
#endif
	return skb;
}

#define rtw_cfg80211_vendor_event(skb, gfp) \
	cfg80211_vendor_event(skb, gfp)
	
#define rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len) \
	cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len)

#define rtw_cfg80211_vendor_cmd_reply(skb) \
	cfg80211_vendor_cmd_reply(skb)
#endif /* DBG_MEM_ALLOC */

/*
 * This API is to be used for asynchronous vendor events. This
 * shouldn't be used in response to a vendor command from its
 * do_it handler context (instead rtw_cfgvendor_send_cmd_reply should
 * be used).
 */
int rtw_cfgvendor_send_async_event(struct wiphy *wiphy,
	struct net_device *dev, int event_id, const void  *data, int len)
{
	u16 kflags;
	struct sk_buff *skb;

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_event_alloc(wiphy, len, event_id, kflags);
	if (!skb) {
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" skb alloc failed", FUNC_NDEV_ARG(dev));
		return -ENOMEM;
	}

	/* Push the data to the skb */
	nla_put_nohdr(skb, len, data);

	rtw_cfg80211_vendor_event(skb, kflags);

	return 0;
}

static int rtw_cfgvendor_send_cmd_reply(struct wiphy *wiphy,
	struct net_device *dev, const void  *data, int len)
{
	struct sk_buff *skb;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len);
	if (unlikely(!skb)) {
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" skb alloc failed", FUNC_NDEV_ARG(dev));
		return -ENOMEM;
	}

	/* Push the data to the skb */
	nla_put_nohdr(skb, len, data);

	return rtw_cfg80211_vendor_cmd_reply(skb);
}

#define WIFI_FEATURE_INFRA              0x0001      /* Basic infrastructure mode        */
#define WIFI_FEATURE_INFRA_5G           0x0002      /* Support for 5 GHz Band           */
#define WIFI_FEATURE_HOTSPOT            0x0004      /* Support for GAS/ANQP             */
#define WIFI_FEATURE_P2P                0x0008      /* Wifi-Direct                      */
#define WIFI_FEATURE_SOFT_AP            0x0010      /* Soft AP                          */
#define WIFI_FEATURE_GSCAN              0x0020      /* Google-Scan APIs                 */
#define WIFI_FEATURE_NAN                0x0040      /* Neighbor Awareness Networking    */
#define WIFI_FEATURE_D2D_RTT            0x0080      /* Device-to-device RTT             */
#define WIFI_FEATURE_D2AP_RTT           0x0100      /* Device-to-AP RTT                 */
#define WIFI_FEATURE_BATCH_SCAN         0x0200      /* Batched Scan (legacy)            */
#define WIFI_FEATURE_PNO                0x0400      /* Preferred network offload        */
#define WIFI_FEATURE_ADDITIONAL_STA     0x0800      /* Support for two STAs             */
#define WIFI_FEATURE_TDLS               0x1000      /* Tunnel directed link setup       */
#define WIFI_FEATURE_TDLS_OFFCHANNEL    0x2000      /* Support for TDLS off channel     */
#define WIFI_FEATURE_EPR                0x4000      /* Enhanced power reporting         */
#define WIFI_FEATURE_AP_STA             0x8000      /* Support for AP STA Concurrency   */

#define MAX_FEATURE_SET_CONCURRRENT_GROUPS  3

#include <hal_data.h>
int rtw_dev_get_feature_set(struct net_device *dev)
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	HAL_DATA_TYPE *HalData = GET_HAL_DATA(adapter);
	HAL_VERSION *hal_ver = &HalData->VersionID;

	int feature_set = 0;

	feature_set |= WIFI_FEATURE_INFRA;

	if (IS_8814A_SERIES(*hal_ver) || IS_8812_SERIES(*hal_ver) ||
			IS_8821_SERIES(*hal_ver))
		feature_set |= WIFI_FEATURE_INFRA_5G;

	feature_set |= WIFI_FEATURE_P2P;
	feature_set |= WIFI_FEATURE_SOFT_AP;

	feature_set |= WIFI_FEATURE_ADDITIONAL_STA;

	return feature_set;
}

int *rtw_dev_get_feature_set_matrix(struct net_device *dev, int *num)
{
	int feature_set_full, mem_needed;
	int *ret;

	*num = 0;
	mem_needed = sizeof(int) * MAX_FEATURE_SET_CONCURRRENT_GROUPS;
	ret = (int *)rtw_malloc(mem_needed);

	if (!ret) {
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" failed to allocate %d bytes\n"
			, FUNC_NDEV_ARG(dev), mem_needed);
		return ret;
	}

	feature_set_full = rtw_dev_get_feature_set(dev);

	ret[0] = (feature_set_full & WIFI_FEATURE_INFRA) |
	         (feature_set_full & WIFI_FEATURE_INFRA_5G) |
	         (feature_set_full & WIFI_FEATURE_NAN) |
	         (feature_set_full & WIFI_FEATURE_D2D_RTT) |
	         (feature_set_full & WIFI_FEATURE_D2AP_RTT) |
	         (feature_set_full & WIFI_FEATURE_PNO) |
	         (feature_set_full & WIFI_FEATURE_BATCH_SCAN) |
	         (feature_set_full & WIFI_FEATURE_GSCAN) |
	         (feature_set_full & WIFI_FEATURE_HOTSPOT) |
	         (feature_set_full & WIFI_FEATURE_ADDITIONAL_STA) |
	         (feature_set_full & WIFI_FEATURE_EPR);

	ret[1] = (feature_set_full & WIFI_FEATURE_INFRA) |
	         (feature_set_full & WIFI_FEATURE_INFRA_5G) |
	         /* Not yet verified NAN with P2P */
	         /* (feature_set_full & WIFI_FEATURE_NAN) | */
	         (feature_set_full & WIFI_FEATURE_P2P) |
	         (feature_set_full & WIFI_FEATURE_D2AP_RTT) |
	         (feature_set_full & WIFI_FEATURE_D2D_RTT) |
	         (feature_set_full & WIFI_FEATURE_EPR);

	ret[2] = (feature_set_full & WIFI_FEATURE_INFRA) |
	         (feature_set_full & WIFI_FEATURE_INFRA_5G) |
	         (feature_set_full & WIFI_FEATURE_NAN) |
	         (feature_set_full & WIFI_FEATURE_D2D_RTT) |
	         (feature_set_full & WIFI_FEATURE_D2AP_RTT) |
	         (feature_set_full & WIFI_FEATURE_TDLS) |
	         (feature_set_full & WIFI_FEATURE_TDLS_OFFCHANNEL) |
	         (feature_set_full & WIFI_FEATURE_EPR);
	*num = MAX_FEATURE_SET_CONCURRRENT_GROUPS;

	return ret;
}

static int rtw_cfgvendor_get_feature_set(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	int reply;

	reply = rtw_dev_get_feature_set(wdev_to_ndev(wdev));

	err =  rtw_cfgvendor_send_cmd_reply(wiphy, wdev_to_ndev(wdev), &reply, sizeof(int));

	if (unlikely(err))
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" Vendor Command reply failed ret:%d \n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);

	return err;
}

static int rtw_cfgvendor_get_feature_set_matrix(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	struct sk_buff *skb;
	int *reply;
	int num, mem_needed, i;

	reply = rtw_dev_get_feature_set_matrix(wdev_to_ndev(wdev), &num);

	if (!reply) {
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" Could not get feature list matrix\n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)));
		err = -EINVAL;
		return err;
	}

	mem_needed = VENDOR_REPLY_OVERHEAD + (ATTRIBUTE_U32_LEN * num) +
	             ATTRIBUTE_U32_LEN;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, mem_needed);
	if (unlikely(!skb)) {
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" skb alloc failed", FUNC_NDEV_ARG(wdev_to_ndev(wdev)));
		err = -ENOMEM;
		goto exit;
	}

	nla_put_u32(skb, ANDR_WIFI_ATTRIBUTE_NUM_FEATURE_SET, num);
	for (i = 0; i < num; i++) {
		nla_put_u32(skb, ANDR_WIFI_ATTRIBUTE_FEATURE_SET, reply[i]);
	}

	err =  rtw_cfg80211_vendor_cmd_reply(skb);

	if (unlikely(err))
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT" Vendor Command reply failed ret:%d \n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);
exit:
	rtw_mfree((u8*)reply, sizeof(int)*num);
	return err;
}


static int wl_cfgvendor_priv_string_handler(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	u8 resp[1] = {'\0'};

	DBG_871X_LEVEL(_drv_always_, FUNC_NDEV_FMT" %s\n", FUNC_NDEV_ARG(wdev_to_ndev(wdev)), (char*)data);
	err =  rtw_cfgvendor_send_cmd_reply(wiphy, wdev_to_ndev(wdev), resp, 1);
	if (unlikely(err))
		DBG_871X_LEVEL(_drv_err_, FUNC_NDEV_FMT"Vendor Command reply failed ret:%d \n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);

	return err;
}

static const struct wiphy_vendor_command rtw_vendor_cmds [] = {
	{
		{
			.vendor_id = OUI_BRCM,
			.subcmd = BRCM_VENDOR_SCMD_PRIV_STR
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = wl_cfgvendor_priv_string_handler,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0))
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = ANDR_WIFI_SUBCMD_GET_FEATURE_SET
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_get_feature_set,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0))
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = ANDR_WIFI_SUBCMD_GET_FEATURE_SET_MATRIX
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_get_feature_set_matrix,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0))
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	}
};

static const struct  nl80211_vendor_cmd_info rtw_vendor_events [] = {
		{ OUI_BRCM, BRCM_VENDOR_EVENT_UNSPEC },
		{ OUI_BRCM, BRCM_VENDOR_EVENT_PRIV_STR },
};

int rtw_cfgvendor_attach(struct wiphy *wiphy)
{

	DBG_871X("Register RTW cfg80211 vendor cmd(0x%x) interface \n", NL80211_CMD_VENDOR);

	wiphy->vendor_commands	= rtw_vendor_cmds;
	wiphy->n_vendor_commands = ARRAY_SIZE(rtw_vendor_cmds);
	wiphy->vendor_events	= rtw_vendor_events;
	wiphy->n_vendor_events	= ARRAY_SIZE(rtw_vendor_events);

	return 0;
}

int rtw_cfgvendor_detach(struct wiphy *wiphy)
{
	DBG_871X("Vendor: Unregister RTW cfg80211 vendor interface \n");

	wiphy->vendor_commands  = NULL;
	wiphy->vendor_events    = NULL;
	wiphy->n_vendor_commands = 0;
	wiphy->n_vendor_events  = 0;

	return 0;
}
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) || defined(RTW_VENDOR_EXT_SUPPORT) */

#endif /* CONFIG_IOCTL_CFG80211 */

