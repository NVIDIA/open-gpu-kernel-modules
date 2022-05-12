/*
 * Copyright (c) 2004 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies. All rights reserved.
 * Copyright (c) 2004 Voltaire, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/string.h>
#include <linux/slab.h>

#include <rdma/ib_verbs.h>
#include <rdma/ib_mad.h>
#include <rdma/ib_smi.h>

#include "mthca_dev.h"
#include "mthca_cmd.h"

enum {
	MTHCA_VENDOR_CLASS1 = 0x9,
	MTHCA_VENDOR_CLASS2 = 0xa
};

static int mthca_update_rate(struct mthca_dev *dev, u8 port_num)
{
	struct ib_port_attr *tprops = NULL;
	int                  ret;

	tprops = kmalloc(sizeof *tprops, GFP_KERNEL);
	if (!tprops)
		return -ENOMEM;

	ret = ib_query_port(&dev->ib_dev, port_num, tprops);
	if (ret) {
		dev_warn(&dev->ib_dev.dev,
			 "ib_query_port failed (%d) forport %d\n", ret,
			 port_num);
		goto out;
	}

	dev->rate[port_num - 1] = tprops->active_speed *
				  ib_width_enum_to_int(tprops->active_width);

out:
	kfree(tprops);
	return ret;
}

static void update_sm_ah(struct mthca_dev *dev,
			 u8 port_num, u16 lid, u8 sl)
{
	struct ib_ah *new_ah;
	struct rdma_ah_attr ah_attr;
	unsigned long flags;

	if (!dev->send_agent[port_num - 1][0])
		return;

	memset(&ah_attr, 0, sizeof ah_attr);
	ah_attr.type = rdma_ah_find_type(&dev->ib_dev, port_num);
	rdma_ah_set_dlid(&ah_attr, lid);
	rdma_ah_set_sl(&ah_attr, sl);
	rdma_ah_set_port_num(&ah_attr, port_num);

	new_ah = rdma_create_ah(dev->send_agent[port_num - 1][0]->qp->pd,
				&ah_attr, 0);
	if (IS_ERR(new_ah))
		return;

	spin_lock_irqsave(&dev->sm_lock, flags);
	if (dev->sm_ah[port_num - 1])
		rdma_destroy_ah(dev->sm_ah[port_num - 1], 0);
	dev->sm_ah[port_num - 1] = new_ah;
	spin_unlock_irqrestore(&dev->sm_lock, flags);
}

/*
 * Snoop SM MADs for port info and P_Key table sets, so we can
 * synthesize LID change and P_Key change events.
 */
static void smp_snoop(struct ib_device *ibdev,
		      u8 port_num,
		      const struct ib_mad *mad,
		      u16 prev_lid)
{
	struct ib_event event;

	if ((mad->mad_hdr.mgmt_class  == IB_MGMT_CLASS_SUBN_LID_ROUTED ||
	     mad->mad_hdr.mgmt_class  == IB_MGMT_CLASS_SUBN_DIRECTED_ROUTE) &&
	    mad->mad_hdr.method     == IB_MGMT_METHOD_SET) {
		if (mad->mad_hdr.attr_id == IB_SMP_ATTR_PORT_INFO) {
			struct ib_port_info *pinfo =
				(struct ib_port_info *) ((struct ib_smp *) mad)->data;
			u16 lid = be16_to_cpu(pinfo->lid);

			mthca_update_rate(to_mdev(ibdev), port_num);
			update_sm_ah(to_mdev(ibdev), port_num,
				     be16_to_cpu(pinfo->sm_lid),
				     pinfo->neighbormtu_mastersmsl & 0xf);

			event.device           = ibdev;
			event.element.port_num = port_num;

			if (pinfo->clientrereg_resv_subnetto & 0x80) {
				event.event    = IB_EVENT_CLIENT_REREGISTER;
				ib_dispatch_event(&event);
			}

			if (prev_lid != lid) {
				event.event    = IB_EVENT_LID_CHANGE;
				ib_dispatch_event(&event);
			}
		}

		if (mad->mad_hdr.attr_id == IB_SMP_ATTR_PKEY_TABLE) {
			event.device           = ibdev;
			event.event            = IB_EVENT_PKEY_CHANGE;
			event.element.port_num = port_num;
			ib_dispatch_event(&event);
		}
	}
}

static void node_desc_override(struct ib_device *dev,
			       struct ib_mad *mad)
{
	if ((mad->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_LID_ROUTED ||
	     mad->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_DIRECTED_ROUTE) &&
	    mad->mad_hdr.method == IB_MGMT_METHOD_GET_RESP &&
	    mad->mad_hdr.attr_id == IB_SMP_ATTR_NODE_DESC) {
		mutex_lock(&to_mdev(dev)->cap_mask_mutex);
		memcpy(((struct ib_smp *) mad)->data, dev->node_desc,
		       IB_DEVICE_NODE_DESC_MAX);
		mutex_unlock(&to_mdev(dev)->cap_mask_mutex);
	}
}

static void forward_trap(struct mthca_dev *dev,
			 u32 port_num,
			 const struct ib_mad *mad)
{
	int qpn = mad->mad_hdr.mgmt_class != IB_MGMT_CLASS_SUBN_LID_ROUTED;
	struct ib_mad_send_buf *send_buf;
	struct ib_mad_agent *agent = dev->send_agent[port_num - 1][qpn];
	int ret;
	unsigned long flags;

	if (agent) {
		send_buf = ib_create_send_mad(agent, qpn, 0, 0, IB_MGMT_MAD_HDR,
					      IB_MGMT_MAD_DATA, GFP_ATOMIC,
					      IB_MGMT_BASE_VERSION);
		if (IS_ERR(send_buf))
			return;
		/*
		 * We rely here on the fact that MLX QPs don't use the
		 * address handle after the send is posted (this is
		 * wrong following the IB spec strictly, but we know
		 * it's OK for our devices).
		 */
		spin_lock_irqsave(&dev->sm_lock, flags);
		memcpy(send_buf->mad, mad, sizeof *mad);
		if ((send_buf->ah = dev->sm_ah[port_num - 1]))
			ret = ib_post_send_mad(send_buf, NULL);
		else
			ret = -EINVAL;
		spin_unlock_irqrestore(&dev->sm_lock, flags);

		if (ret)
			ib_free_send_mad(send_buf);
	}
}

int mthca_process_mad(struct ib_device *ibdev, int mad_flags, u32 port_num,
		      const struct ib_wc *in_wc, const struct ib_grh *in_grh,
		      const struct ib_mad *in, struct ib_mad *out,
		      size_t *out_mad_size, u16 *out_mad_pkey_index)
{
	int err;
	u16 slid = in_wc ? ib_lid_cpu16(in_wc->slid) : be16_to_cpu(IB_LID_PERMISSIVE);
	u16 prev_lid = 0;
	struct ib_port_attr pattr;

	/* Forward locally generated traps to the SM */
	if (in->mad_hdr.method == IB_MGMT_METHOD_TRAP && !slid) {
		forward_trap(to_mdev(ibdev), port_num, in);
		return IB_MAD_RESULT_SUCCESS | IB_MAD_RESULT_CONSUMED;
	}

	/*
	 * Only handle SM gets, sets and trap represses for SM class
	 *
	 * Only handle PMA and Mellanox vendor-specific class gets and
	 * sets for other classes.
	 */
	if (in->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_LID_ROUTED ||
	    in->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_DIRECTED_ROUTE) {
		if (in->mad_hdr.method != IB_MGMT_METHOD_GET &&
		    in->mad_hdr.method != IB_MGMT_METHOD_SET &&
		    in->mad_hdr.method != IB_MGMT_METHOD_TRAP_REPRESS)
			return IB_MAD_RESULT_SUCCESS;

		/*
		 * Don't process SMInfo queries or vendor-specific
		 * MADs -- the SMA can't handle them.
		 */
		if (in->mad_hdr.attr_id == IB_SMP_ATTR_SM_INFO ||
		    ((in->mad_hdr.attr_id & IB_SMP_ATTR_VENDOR_MASK) ==
		     IB_SMP_ATTR_VENDOR_MASK))
			return IB_MAD_RESULT_SUCCESS;
	} else if (in->mad_hdr.mgmt_class == IB_MGMT_CLASS_PERF_MGMT ||
		   in->mad_hdr.mgmt_class == MTHCA_VENDOR_CLASS1     ||
		   in->mad_hdr.mgmt_class == MTHCA_VENDOR_CLASS2) {
		if (in->mad_hdr.method != IB_MGMT_METHOD_GET &&
		    in->mad_hdr.method != IB_MGMT_METHOD_SET)
			return IB_MAD_RESULT_SUCCESS;
	} else
		return IB_MAD_RESULT_SUCCESS;
	if ((in->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_LID_ROUTED ||
	     in->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_DIRECTED_ROUTE) &&
	    in->mad_hdr.method == IB_MGMT_METHOD_SET &&
	    in->mad_hdr.attr_id == IB_SMP_ATTR_PORT_INFO &&
	    !ib_query_port(ibdev, port_num, &pattr))
		prev_lid = ib_lid_cpu16(pattr.lid);

	err = mthca_MAD_IFC(to_mdev(ibdev), mad_flags & IB_MAD_IGNORE_MKEY,
			    mad_flags & IB_MAD_IGNORE_BKEY, port_num, in_wc,
			    in_grh, in, out);
	if (err == -EBADMSG)
		return IB_MAD_RESULT_SUCCESS;
	else if (err) {
		mthca_err(to_mdev(ibdev), "MAD_IFC returned %d\n", err);
		return IB_MAD_RESULT_FAILURE;
	}

	if (!out->mad_hdr.status) {
		smp_snoop(ibdev, port_num, in, prev_lid);
		node_desc_override(ibdev, out);
	}

	/* set return bit in status of directed route responses */
	if (in->mad_hdr.mgmt_class == IB_MGMT_CLASS_SUBN_DIRECTED_ROUTE)
		out->mad_hdr.status |= cpu_to_be16(1 << 15);

	if (in->mad_hdr.method == IB_MGMT_METHOD_TRAP_REPRESS)
		/* no response for trap repress */
		return IB_MAD_RESULT_SUCCESS | IB_MAD_RESULT_CONSUMED;

	return IB_MAD_RESULT_SUCCESS | IB_MAD_RESULT_REPLY;
}

static void send_handler(struct ib_mad_agent *agent,
			 struct ib_mad_send_wc *mad_send_wc)
{
	ib_free_send_mad(mad_send_wc->send_buf);
}

int mthca_create_agents(struct mthca_dev *dev)
{
	struct ib_mad_agent *agent;
	int p, q;
	int ret;

	spin_lock_init(&dev->sm_lock);

	for (p = 0; p < dev->limits.num_ports; ++p)
		for (q = 0; q <= 1; ++q) {
			agent = ib_register_mad_agent(&dev->ib_dev, p + 1,
						      q ? IB_QPT_GSI : IB_QPT_SMI,
						      NULL, 0, send_handler,
						      NULL, NULL, 0);
			if (IS_ERR(agent)) {
				ret = PTR_ERR(agent);
				goto err;
			}
			dev->send_agent[p][q] = agent;
		}


	for (p = 1; p <= dev->limits.num_ports; ++p) {
		ret = mthca_update_rate(dev, p);
		if (ret) {
			mthca_err(dev, "Failed to obtain port %d rate."
				  " aborting.\n", p);
			goto err;
		}
	}

	return 0;

err:
	for (p = 0; p < dev->limits.num_ports; ++p)
		for (q = 0; q <= 1; ++q)
			if (dev->send_agent[p][q])
				ib_unregister_mad_agent(dev->send_agent[p][q]);

	return ret;
}

void mthca_free_agents(struct mthca_dev *dev)
{
	struct ib_mad_agent *agent;
	int p, q;

	for (p = 0; p < dev->limits.num_ports; ++p) {
		for (q = 0; q <= 1; ++q) {
			agent = dev->send_agent[p][q];
			dev->send_agent[p][q] = NULL;
			ib_unregister_mad_agent(agent);
		}

		if (dev->sm_ah[p])
			rdma_destroy_ah(dev->sm_ah[p],
					RDMA_DESTROY_AH_SLEEPABLE);
	}
}
