/*
 * Copyright 2013 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Ben Skeggs
 */
#include "priv.h"
#include "conn.h"
#include "dp.h"
#include "head.h"
#include "ior.h"
#include "outp.h"

#include <core/client.h>
#include <core/notify.h>
#include <core/oproxy.h>
#include <subdev/bios.h>
#include <subdev/bios/dcb.h>

#include <nvif/class.h>
#include <nvif/cl0046.h>
#include <nvif/event.h>
#include <nvif/unpack.h>

static void
nvkm_disp_vblank_fini(struct nvkm_event *event, int type, int id)
{
	struct nvkm_disp *disp = container_of(event, typeof(*disp), vblank);
	struct nvkm_head *head = nvkm_head_find(disp, id);
	if (head)
		head->func->vblank_put(head);
}

static void
nvkm_disp_vblank_init(struct nvkm_event *event, int type, int id)
{
	struct nvkm_disp *disp = container_of(event, typeof(*disp), vblank);
	struct nvkm_head *head = nvkm_head_find(disp, id);
	if (head)
		head->func->vblank_get(head);
}

static int
nvkm_disp_vblank_ctor(struct nvkm_object *object, void *data, u32 size,
		      struct nvkm_notify *notify)
{
	struct nvkm_disp *disp =
		container_of(notify->event, typeof(*disp), vblank);
	union {
		struct nvif_notify_head_req_v0 v0;
	} *req = data;
	int ret = -ENOSYS;

	if (!(ret = nvif_unpack(ret, &data, &size, req->v0, 0, 0, false))) {
		notify->size = sizeof(struct nvif_notify_head_rep_v0);
		if (ret = -ENXIO, req->v0.head <= disp->vblank.index_nr) {
			notify->types = 1;
			notify->index = req->v0.head;
			return 0;
		}
	}

	return ret;
}

static const struct nvkm_event_func
nvkm_disp_vblank_func = {
	.ctor = nvkm_disp_vblank_ctor,
	.init = nvkm_disp_vblank_init,
	.fini = nvkm_disp_vblank_fini,
};

void
nvkm_disp_vblank(struct nvkm_disp *disp, int head)
{
	struct nvif_notify_head_rep_v0 rep = {};
	nvkm_event_send(&disp->vblank, 1, head, &rep, sizeof(rep));
}

static int
nvkm_disp_hpd_ctor(struct nvkm_object *object, void *data, u32 size,
		   struct nvkm_notify *notify)
{
	struct nvkm_disp *disp =
		container_of(notify->event, typeof(*disp), hpd);
	union {
		struct nvif_notify_conn_req_v0 v0;
	} *req = data;
	struct nvkm_outp *outp;
	int ret = -ENOSYS;

	if (!(ret = nvif_unpack(ret, &data, &size, req->v0, 0, 0, false))) {
		notify->size = sizeof(struct nvif_notify_conn_rep_v0);
		list_for_each_entry(outp, &disp->outp, head) {
			if (ret = -ENXIO, outp->conn->index == req->v0.conn) {
				if (ret = -ENODEV, outp->conn->hpd.event) {
					notify->types = req->v0.mask;
					notify->index = req->v0.conn;
					ret = 0;
				}
				break;
			}
		}
	}

	return ret;
}

static const struct nvkm_event_func
nvkm_disp_hpd_func = {
	.ctor = nvkm_disp_hpd_ctor
};

int
nvkm_disp_ntfy(struct nvkm_object *object, u32 type, struct nvkm_event **event)
{
	struct nvkm_disp *disp = nvkm_disp(object->engine);
	switch (type) {
	case NV04_DISP_NTFY_VBLANK:
		*event = &disp->vblank;
		return 0;
	case NV04_DISP_NTFY_CONN:
		*event = &disp->hpd;
		return 0;
	default:
		break;
	}
	return -EINVAL;
}

static void
nvkm_disp_class_del(struct nvkm_oproxy *oproxy)
{
	struct nvkm_disp *disp = nvkm_disp(oproxy->base.engine);
	spin_lock(&disp->client.lock);
	if (disp->client.object == oproxy)
		disp->client.object = NULL;
	spin_unlock(&disp->client.lock);
}

static const struct nvkm_oproxy_func
nvkm_disp_class = {
	.dtor[1] = nvkm_disp_class_del,
};

static int
nvkm_disp_class_new(struct nvkm_device *device,
		    const struct nvkm_oclass *oclass, void *data, u32 size,
		    struct nvkm_object **pobject)
{
	const struct nvkm_disp_oclass *sclass = oclass->engn;
	struct nvkm_disp *disp = nvkm_disp(oclass->engine);
	struct nvkm_oproxy *oproxy;
	int ret;

	ret = nvkm_oproxy_new_(&nvkm_disp_class, oclass, &oproxy);
	if (ret)
		return ret;
	*pobject = &oproxy->base;

	spin_lock(&disp->client.lock);
	if (disp->client.object) {
		spin_unlock(&disp->client.lock);
		return -EBUSY;
	}
	disp->client.object = oproxy;
	spin_unlock(&disp->client.lock);

	return sclass->ctor(disp, oclass, data, size, &oproxy->object);
}

static const struct nvkm_device_oclass
nvkm_disp_sclass = {
	.ctor = nvkm_disp_class_new,
};

static int
nvkm_disp_class_get(struct nvkm_oclass *oclass, int index,
		    const struct nvkm_device_oclass **class)
{
	struct nvkm_disp *disp = nvkm_disp(oclass->engine);
	if (index == 0) {
		const struct nvkm_disp_oclass *root = disp->func->root(disp);
		oclass->base = root->base;
		oclass->engn = root;
		*class = &nvkm_disp_sclass;
		return 0;
	}
	return 1;
}

static void
nvkm_disp_intr(struct nvkm_engine *engine)
{
	struct nvkm_disp *disp = nvkm_disp(engine);
	disp->func->intr(disp);
}

static int
nvkm_disp_fini(struct nvkm_engine *engine, bool suspend)
{
	struct nvkm_disp *disp = nvkm_disp(engine);
	struct nvkm_conn *conn;
	struct nvkm_outp *outp;

	if (disp->func->fini)
		disp->func->fini(disp);

	list_for_each_entry(outp, &disp->outp, head) {
		nvkm_outp_fini(outp);
	}

	list_for_each_entry(conn, &disp->conn, head) {
		nvkm_conn_fini(conn);
	}

	return 0;
}

static int
nvkm_disp_init(struct nvkm_engine *engine)
{
	struct nvkm_disp *disp = nvkm_disp(engine);
	struct nvkm_conn *conn;
	struct nvkm_outp *outp;
	struct nvkm_ior *ior;

	list_for_each_entry(conn, &disp->conn, head) {
		nvkm_conn_init(conn);
	}

	list_for_each_entry(outp, &disp->outp, head) {
		nvkm_outp_init(outp);
	}

	if (disp->func->init) {
		int ret = disp->func->init(disp);
		if (ret)
			return ret;
	}

	/* Set 'normal' (ie. when it's attached to a head) state for
	 * each output resource to 'fully enabled'.
	 */
	list_for_each_entry(ior, &disp->ior, head) {
		ior->func->power(ior, true, true, true, true, true);
	}

	return 0;
}

static int
nvkm_disp_oneinit(struct nvkm_engine *engine)
{
	struct nvkm_disp *disp = nvkm_disp(engine);
	struct nvkm_subdev *subdev = &disp->engine.subdev;
	struct nvkm_bios *bios = subdev->device->bios;
	struct nvkm_outp *outp, *outt, *pair;
	struct nvkm_conn *conn;
	struct nvkm_head *head;
	struct nvkm_ior *ior;
	struct nvbios_connE connE;
	struct dcb_output dcbE;
	u8  hpd = 0, ver, hdr;
	u32 data;
	int ret, i;

	/* Create output path objects for each VBIOS display path. */
	i = -1;
	while ((data = dcb_outp_parse(bios, ++i, &ver, &hdr, &dcbE))) {
		if (ver < 0x40) /* No support for chipsets prior to NV50. */
			break;
		if (dcbE.type == DCB_OUTPUT_UNUSED)
			continue;
		if (dcbE.type == DCB_OUTPUT_EOL)
			break;
		outp = NULL;

		switch (dcbE.type) {
		case DCB_OUTPUT_ANALOG:
		case DCB_OUTPUT_TV:
		case DCB_OUTPUT_TMDS:
		case DCB_OUTPUT_LVDS:
			ret = nvkm_outp_new(disp, i, &dcbE, &outp);
			break;
		case DCB_OUTPUT_DP:
			ret = nvkm_dp_new(disp, i, &dcbE, &outp);
			break;
		case DCB_OUTPUT_WFD:
			/* No support for WFD yet. */
			ret = -ENODEV;
			continue;
		default:
			nvkm_warn(subdev, "dcb %d type %d unknown\n",
				  i, dcbE.type);
			continue;
		}

		if (ret) {
			if (outp) {
				if (ret != -ENODEV)
					OUTP_ERR(outp, "ctor failed: %d", ret);
				else
					OUTP_DBG(outp, "not supported");
				nvkm_outp_del(&outp);
				continue;
			}
			nvkm_error(subdev, "failed to create outp %d\n", i);
			continue;
		}

		list_add_tail(&outp->head, &disp->outp);
		hpd = max(hpd, (u8)(dcbE.connector + 1));
	}

	/* Create connector objects based on available output paths. */
	list_for_each_entry_safe(outp, outt, &disp->outp, head) {
		/* VBIOS data *should* give us the most useful information. */
		data = nvbios_connEp(bios, outp->info.connector, &ver, &hdr,
				     &connE);

		/* No bios connector data... */
		if (!data) {
			/* Heuristic: anything with the same ccb index is
			 * considered to be on the same connector, any
			 * output path without an associated ccb entry will
			 * be put on its own connector.
			 */
			int ccb_index = outp->info.i2c_index;
			if (ccb_index != 0xf) {
				list_for_each_entry(pair, &disp->outp, head) {
					if (pair->info.i2c_index == ccb_index) {
						outp->conn = pair->conn;
						break;
					}
				}
			}

			/* Connector shared with another output path. */
			if (outp->conn)
				continue;

			memset(&connE, 0x00, sizeof(connE));
			connE.type = DCB_CONNECTOR_NONE;
			i = -1;
		} else {
			i = outp->info.connector;
		}

		/* Check that we haven't already created this connector. */
		list_for_each_entry(conn, &disp->conn, head) {
			if (conn->index == outp->info.connector) {
				outp->conn = conn;
				break;
			}
		}

		if (outp->conn)
			continue;

		/* Apparently we need to create a new one! */
		ret = nvkm_conn_new(disp, i, &connE, &outp->conn);
		if (ret) {
			nvkm_error(&disp->engine.subdev,
				   "failed to create outp %d conn: %d\n",
				   outp->index, ret);
			nvkm_conn_del(&outp->conn);
			list_del(&outp->head);
			nvkm_outp_del(&outp);
			continue;
		}

		list_add_tail(&outp->conn->head, &disp->conn);
	}

	ret = nvkm_event_init(&nvkm_disp_hpd_func, 3, hpd, &disp->hpd);
	if (ret)
		return ret;

	if (disp->func->oneinit) {
		ret = disp->func->oneinit(disp);
		if (ret)
			return ret;
	}

	/* Enforce identity-mapped SOR assignment for panels, which have
	 * certain bits (ie. backlight controls) wired to a specific SOR.
	 */
	list_for_each_entry(outp, &disp->outp, head) {
		if (outp->conn->info.type == DCB_CONNECTOR_LVDS ||
		    outp->conn->info.type == DCB_CONNECTOR_eDP) {
			ior = nvkm_ior_find(disp, SOR, ffs(outp->info.or) - 1);
			if (!WARN_ON(!ior))
				ior->identity = true;
			outp->identity = true;
		}
	}

	i = 0;
	list_for_each_entry(head, &disp->head, head)
		i = max(i, head->id + 1);

	return nvkm_event_init(&nvkm_disp_vblank_func, 1, i, &disp->vblank);
}

static void *
nvkm_disp_dtor(struct nvkm_engine *engine)
{
	struct nvkm_disp *disp = nvkm_disp(engine);
	struct nvkm_conn *conn;
	struct nvkm_outp *outp;
	void *data = disp;

	if (disp->func->dtor)
		data = disp->func->dtor(disp);

	nvkm_event_fini(&disp->vblank);
	nvkm_event_fini(&disp->hpd);

	while (!list_empty(&disp->conn)) {
		conn = list_first_entry(&disp->conn, typeof(*conn), head);
		list_del(&conn->head);
		nvkm_conn_del(&conn);
	}

	while (!list_empty(&disp->outp)) {
		outp = list_first_entry(&disp->outp, typeof(*outp), head);
		list_del(&outp->head);
		nvkm_outp_del(&outp);
	}

	while (!list_empty(&disp->ior)) {
		struct nvkm_ior *ior =
			list_first_entry(&disp->ior, typeof(*ior), head);
		nvkm_ior_del(&ior);
	}

	while (!list_empty(&disp->head)) {
		struct nvkm_head *head =
			list_first_entry(&disp->head, typeof(*head), head);
		nvkm_head_del(&head);
	}

	return data;
}

static const struct nvkm_engine_func
nvkm_disp = {
	.dtor = nvkm_disp_dtor,
	.oneinit = nvkm_disp_oneinit,
	.init = nvkm_disp_init,
	.fini = nvkm_disp_fini,
	.intr = nvkm_disp_intr,
	.base.sclass = nvkm_disp_class_get,
};

int
nvkm_disp_ctor(const struct nvkm_disp_func *func, struct nvkm_device *device,
	       enum nvkm_subdev_type type, int inst, struct nvkm_disp *disp)
{
	disp->func = func;
	INIT_LIST_HEAD(&disp->head);
	INIT_LIST_HEAD(&disp->ior);
	INIT_LIST_HEAD(&disp->outp);
	INIT_LIST_HEAD(&disp->conn);
	spin_lock_init(&disp->client.lock);
	return nvkm_engine_ctor(&nvkm_disp, device, type, inst, true, &disp->engine);
}

int
nvkm_disp_new_(const struct nvkm_disp_func *func, struct nvkm_device *device,
	       enum nvkm_subdev_type type, int inst, struct nvkm_disp **pdisp)
{
	if (!(*pdisp = kzalloc(sizeof(**pdisp), GFP_KERNEL)))
		return -ENOMEM;
	return nvkm_disp_ctor(func, device, type, inst, *pdisp);
}
