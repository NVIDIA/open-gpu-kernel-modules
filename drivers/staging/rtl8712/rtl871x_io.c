// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * rtl871x_io.c
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 * Linux device driver for RTL8192SU
 *
 * Modifications for inclusion into the Linux staging tree are
 * Copyright(c) 2010 Larry Finger. All rights reserved.
 *
 * Contact information:
 * WLAN FAE <wlanfae@realtek.com>
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 ******************************************************************************/
/*
 *
 * The purpose of rtl871x_io.c
 *
 * a. provides the API
 * b. provides the protocol engine
 * c. provides the software interface between caller and the hardware interface
 *
 * For r8712u, both sync/async operations are provided.
 *
 * Only sync read/write_mem operations are provided.
 *
 */

#define _RTL871X_IO_C_

#include "osdep_service.h"
#include "drv_types.h"
#include "rtl871x_io.h"
#include "osdep_intf.h"
#include "usb_ops.h"

static uint _init_intf_hdl(struct _adapter *padapter,
			   struct intf_hdl *pintf_hdl)
{
	struct	intf_priv	*pintf_priv;
	void (*set_intf_option)(u32 *poption) = NULL;
	void (*set_intf_funs)(struct intf_hdl *pintf_hdl);
	void (*set_intf_ops)(struct _io_ops	*pops);
	uint (*init_intf_priv)(struct intf_priv *pintfpriv);

	set_intf_option = &(r8712_usb_set_intf_option);
	set_intf_funs = &(r8712_usb_set_intf_funs);
	set_intf_ops = &r8712_usb_set_intf_ops;
	init_intf_priv = &r8712_usb_init_intf_priv;
	pintf_priv = pintf_hdl->pintfpriv = kmalloc(sizeof(struct intf_priv),
						    GFP_ATOMIC);
	if (!pintf_priv)
		goto _init_intf_hdl_fail;
	pintf_hdl->adapter = (u8 *)padapter;
	set_intf_option(&pintf_hdl->intf_option);
	set_intf_funs(pintf_hdl);
	set_intf_ops(&pintf_hdl->io_ops);
	pintf_priv->intf_dev = (u8 *)&padapter->dvobjpriv;
	if (init_intf_priv(pintf_priv) == _FAIL)
		goto _init_intf_hdl_fail;
	return _SUCCESS;
_init_intf_hdl_fail:
	kfree(pintf_priv);
	return _FAIL;
}

static void _unload_intf_hdl(struct intf_priv *pintfpriv)
{
	void (*unload_intf_priv)(struct intf_priv *pintfpriv);

	unload_intf_priv = &r8712_usb_unload_intf_priv;
	unload_intf_priv(pintfpriv);
	kfree(pintfpriv);
}

static uint register_intf_hdl(u8 *dev, struct intf_hdl *pintfhdl)
{
	struct _adapter *adapter = (struct _adapter *)dev;

	pintfhdl->intf_option = 0;
	pintfhdl->adapter = dev;
	pintfhdl->intf_dev = (u8 *)&adapter->dvobjpriv;
	if (!_init_intf_hdl(adapter, pintfhdl))
		goto register_intf_hdl_fail;
	return _SUCCESS;
register_intf_hdl_fail:
	return false;
}

static  void unregister_intf_hdl(struct intf_hdl *pintfhdl)
{
	_unload_intf_hdl(pintfhdl->pintfpriv);
	memset((u8 *)pintfhdl, 0, sizeof(struct intf_hdl));
}

uint r8712_alloc_io_queue(struct _adapter *adapter)
{
	u32 i;
	struct io_queue *pio_queue;
	struct io_req *pio_req;

	pio_queue = kmalloc(sizeof(*pio_queue), GFP_ATOMIC);
	if (!pio_queue)
		goto alloc_io_queue_fail;
	INIT_LIST_HEAD(&pio_queue->free_ioreqs);
	INIT_LIST_HEAD(&pio_queue->processing);
	INIT_LIST_HEAD(&pio_queue->pending);
	spin_lock_init(&pio_queue->lock);
	pio_queue->pallocated_free_ioreqs_buf = kzalloc(NUM_IOREQ *
						(sizeof(struct io_req)) + 4,
						GFP_ATOMIC);
	if ((pio_queue->pallocated_free_ioreqs_buf) == NULL)
		goto alloc_io_queue_fail;
	pio_queue->free_ioreqs_buf = pio_queue->pallocated_free_ioreqs_buf + 4
			- ((addr_t)(pio_queue->pallocated_free_ioreqs_buf)
			& 3);
	pio_req = (struct io_req *)(pio_queue->free_ioreqs_buf);
	for (i = 0; i < NUM_IOREQ; i++) {
		INIT_LIST_HEAD(&pio_req->list);
		list_add_tail(&pio_req->list, &pio_queue->free_ioreqs);
		pio_req++;
	}
	if ((register_intf_hdl((u8 *)adapter, &pio_queue->intf)) == _FAIL)
		goto alloc_io_queue_fail;
	adapter->pio_queue = pio_queue;
	return _SUCCESS;
alloc_io_queue_fail:
	if (pio_queue) {
		kfree(pio_queue->pallocated_free_ioreqs_buf);
		kfree(pio_queue);
	}
	adapter->pio_queue = NULL;
	return _FAIL;
}

void r8712_free_io_queue(struct _adapter *adapter)
{
	struct io_queue *pio_queue = adapter->pio_queue;

	if (pio_queue) {
		kfree(pio_queue->pallocated_free_ioreqs_buf);
		adapter->pio_queue = NULL;
		unregister_intf_hdl(&pio_queue->intf);
		kfree(pio_queue);
	}
}
