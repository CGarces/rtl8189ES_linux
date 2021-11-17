// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2013 Realtek Corporation. */
#ifndef __RTW_PROC_H__
#define __RTW_PROC_H__

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

struct rtw_proc_hdl {
	char *name;
	int (*show)(struct seq_file *, void *);
	ssize_t (*write)(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
};

#ifdef CONFIG_PROC_DEBUG

struct proc_dir_entry *get_rtw_drv_proc(void);
int rtw_drv_proc_init(void);
void rtw_drv_proc_deinit(void);
struct proc_dir_entry *rtw_adapter_proc_init(struct net_device *dev);
void rtw_adapter_proc_deinit(struct net_device *dev);
void rtw_adapter_proc_replace(struct net_device *dev);

#else //!CONFIG_PROC_DEBUG

#define get_rtw_drv_proc() NULL
#define rtw_drv_proc_init() 0
#define rtw_drv_proc_deinit() do {} while (0)
#define rtw_adapter_proc_init(dev) NULL
#define rtw_adapter_proc_deinit(dev) do {} while (0)
#define rtw_adapter_proc_replace(dev) do {} while (0)

#endif //!CONFIG_PROC_DEBUG

#endif //__RTW_PROC_H__
