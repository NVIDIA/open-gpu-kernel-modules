// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Bridge per vlan tunnel port dst_metadata netlink control interface
 *
 *	Authors:
 *	Roopa Prabhu		<roopa@cumulusnetworks.com>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/etherdevice.h>
#include <net/rtnetlink.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <uapi/linux/if_bridge.h>
#include <net/dst_metadata.h>

#include "br_private.h"
#include "br_private_tunnel.h"

static size_t __get_vlan_tinfo_size(void)
{
	return nla_total_size(0) + /* nest IFLA_BRIDGE_VLAN_TUNNEL_INFO */
		  nla_total_size(sizeof(u32)) + /* IFLA_BRIDGE_VLAN_TUNNEL_ID */
		  nla_total_size(sizeof(u16)) + /* IFLA_BRIDGE_VLAN_TUNNEL_VID */
		  nla_total_size(sizeof(u16)); /* IFLA_BRIDGE_VLAN_TUNNEL_FLAGS */
}

bool vlan_tunid_inrange(const struct net_bridge_vlan *v_curr,
			const struct net_bridge_vlan *v_last)
{
	__be32 tunid_curr = tunnel_id_to_key32(v_curr->tinfo.tunnel_id);
	__be32 tunid_last = tunnel_id_to_key32(v_last->tinfo.tunnel_id);

	return (be32_to_cpu(tunid_curr) - be32_to_cpu(tunid_last)) == 1;
}

static int __get_num_vlan_tunnel_infos(struct net_bridge_vlan_group *vg)
{
	struct net_bridge_vlan *v, *vtbegin = NULL, *vtend = NULL;
	int num_tinfos = 0;

	/* Count number of vlan infos */
	list_for_each_entry_rcu(v, &vg->vlan_list, vlist) {
		/* only a context, bridge vlan not activated */
		if (!br_vlan_should_use(v) || !v->tinfo.tunnel_id)
			continue;

		if (!vtbegin) {
			goto initvars;
		} else if ((v->vid - vtend->vid) == 1 &&
			   vlan_tunid_inrange(v, vtend)) {
			vtend = v;
			continue;
		} else {
			if ((vtend->vid - vtbegin->vid) > 0)
				num_tinfos += 2;
			else
				num_tinfos += 1;
		}
initvars:
		vtbegin = v;
		vtend = v;
	}

	if (vtbegin && vtend) {
		if ((vtend->vid - vtbegin->vid) > 0)
			num_tinfos += 2;
		else
			num_tinfos += 1;
	}

	return num_tinfos;
}

int br_get_vlan_tunnel_info_size(struct net_bridge_vlan_group *vg)
{
	int num_tinfos;

	if (!vg)
		return 0;

	rcu_read_lock();
	num_tinfos = __get_num_vlan_tunnel_infos(vg);
	rcu_read_unlock();

	return num_tinfos * __get_vlan_tinfo_size();
}

static int br_fill_vlan_tinfo(struct sk_buff *skb, u16 vid,
			      __be64 tunnel_id, u16 flags)
{
	__be32 tid = tunnel_id_to_key32(tunnel_id);
	struct nlattr *tmap;

	tmap = nla_nest_start_noflag(skb, IFLA_BRIDGE_VLAN_TUNNEL_INFO);
	if (!tmap)
		return -EMSGSIZE;
	if (nla_put_u32(skb, IFLA_BRIDGE_VLAN_TUNNEL_ID,
			be32_to_cpu(tid)))
		goto nla_put_failure;
	if (nla_put_u16(skb, IFLA_BRIDGE_VLAN_TUNNEL_VID,
			vid))
		goto nla_put_failure;
	if (nla_put_u16(skb, IFLA_BRIDGE_VLAN_TUNNEL_FLAGS,
			flags))
		goto nla_put_failure;
	nla_nest_end(skb, tmap);

	return 0;

nla_put_failure:
	nla_nest_cancel(skb, tmap);

	return -EMSGSIZE;
}

static int br_fill_vlan_tinfo_range(struct sk_buff *skb,
				    struct net_bridge_vlan *vtbegin,
				    struct net_bridge_vlan *vtend)
{
	int err;

	if (vtend && (vtend->vid - vtbegin->vid) > 0) {
		/* add range to skb */
		err = br_fill_vlan_tinfo(skb, vtbegin->vid,
					 vtbegin->tinfo.tunnel_id,
					 BRIDGE_VLAN_INFO_RANGE_BEGIN);
		if (err)
			return err;

		err = br_fill_vlan_tinfo(skb, vtend->vid,
					 vtend->tinfo.tunnel_id,
					 BRIDGE_VLAN_INFO_RANGE_END);
		if (err)
			return err;
	} else {
		err = br_fill_vlan_tinfo(skb, vtbegin->vid,
					 vtbegin->tinfo.tunnel_id,
					 0);
		if (err)
			return err;
	}

	return 0;
}

int br_fill_vlan_tunnel_info(struct sk_buff *skb,
			     struct net_bridge_vlan_group *vg)
{
	struct net_bridge_vlan *vtbegin = NULL;
	struct net_bridge_vlan *vtend = NULL;
	struct net_bridge_vlan *v;
	int err;

	/* Count number of vlan infos */
	list_for_each_entry_rcu(v, &vg->vlan_list, vlist) {
		/* only a context, bridge vlan not activated */
		if (!br_vlan_should_use(v))
			continue;

		if (!v->tinfo.tunnel_dst)
			continue;

		if (!vtbegin) {
			goto initvars;
		} else if ((v->vid - vtend->vid) == 1 &&
			    vlan_tunid_inrange(v, vtend)) {
			vtend = v;
			continue;
		} else {
			err = br_fill_vlan_tinfo_range(skb, vtbegin, vtend);
			if (err)
				return err;
		}
initvars:
		vtbegin = v;
		vtend = v;
	}

	if (vtbegin) {
		err = br_fill_vlan_tinfo_range(skb, vtbegin, vtend);
		if (err)
			return err;
	}

	return 0;
}

static const struct nla_policy vlan_tunnel_policy[IFLA_BRIDGE_VLAN_TUNNEL_MAX + 1] = {
	[IFLA_BRIDGE_VLAN_TUNNEL_ID] = { .type = NLA_U32 },
	[IFLA_BRIDGE_VLAN_TUNNEL_VID] = { .type = NLA_U16 },
	[IFLA_BRIDGE_VLAN_TUNNEL_FLAGS] = { .type = NLA_U16 },
};

int br_vlan_tunnel_info(const struct net_bridge_port *p, int cmd,
			u16 vid, u32 tun_id, bool *changed)
{
	int err = 0;

	if (!p)
		return -EINVAL;

	switch (cmd) {
	case RTM_SETLINK:
		err = nbp_vlan_tunnel_info_add(p, vid, tun_id);
		if (!err)
			*changed = true;
		break;
	case RTM_DELLINK:
		if (!nbp_vlan_tunnel_info_delete(p, vid))
			*changed = true;
		break;
	}

	return err;
}

int br_parse_vlan_tunnel_info(struct nlattr *attr,
			      struct vtunnel_info *tinfo)
{
	struct nlattr *tb[IFLA_BRIDGE_VLAN_TUNNEL_MAX + 1];
	u32 tun_id;
	u16 vid, flags = 0;
	int err;

	memset(tinfo, 0, sizeof(*tinfo));

	err = nla_parse_nested_deprecated(tb, IFLA_BRIDGE_VLAN_TUNNEL_MAX,
					  attr, vlan_tunnel_policy, NULL);
	if (err < 0)
		return err;

	if (!tb[IFLA_BRIDGE_VLAN_TUNNEL_ID] ||
	    !tb[IFLA_BRIDGE_VLAN_TUNNEL_VID])
		return -EINVAL;

	tun_id = nla_get_u32(tb[IFLA_BRIDGE_VLAN_TUNNEL_ID]);
	vid = nla_get_u16(tb[IFLA_BRIDGE_VLAN_TUNNEL_VID]);
	if (vid >= VLAN_VID_MASK)
		return -ERANGE;

	if (tb[IFLA_BRIDGE_VLAN_TUNNEL_FLAGS])
		flags = nla_get_u16(tb[IFLA_BRIDGE_VLAN_TUNNEL_FLAGS]);

	tinfo->tunid = tun_id;
	tinfo->vid = vid;
	tinfo->flags = flags;

	return 0;
}

/* send a notification if v_curr can't enter the range and start a new one */
static void __vlan_tunnel_handle_range(const struct net_bridge_port *p,
				       struct net_bridge_vlan **v_start,
				       struct net_bridge_vlan **v_end,
				       int v_curr, bool curr_change)
{
	struct net_bridge_vlan_group *vg;
	struct net_bridge_vlan *v;

	vg = nbp_vlan_group(p);
	if (!vg)
		return;

	v = br_vlan_find(vg, v_curr);

	if (!*v_start)
		goto out_init;

	if (v && curr_change && br_vlan_can_enter_range(v, *v_end)) {
		*v_end = v;
		return;
	}

	br_vlan_notify(p->br, p, (*v_start)->vid, (*v_end)->vid, RTM_NEWVLAN);
out_init:
	/* we start a range only if there are any changes to notify about */
	*v_start = curr_change ? v : NULL;
	*v_end = *v_start;
}

int br_process_vlan_tunnel_info(const struct net_bridge *br,
				const struct net_bridge_port *p, int cmd,
				struct vtunnel_info *tinfo_curr,
				struct vtunnel_info *tinfo_last,
				bool *changed)
{
	int err;

	if (tinfo_curr->flags & BRIDGE_VLAN_INFO_RANGE_BEGIN) {
		if (tinfo_last->flags & BRIDGE_VLAN_INFO_RANGE_BEGIN)
			return -EINVAL;
		memcpy(tinfo_last, tinfo_curr, sizeof(struct vtunnel_info));
	} else if (tinfo_curr->flags & BRIDGE_VLAN_INFO_RANGE_END) {
		struct net_bridge_vlan *v_start = NULL, *v_end = NULL;
		int t, v;

		if (!(tinfo_last->flags & BRIDGE_VLAN_INFO_RANGE_BEGIN))
			return -EINVAL;
		if ((tinfo_curr->vid - tinfo_last->vid) !=
		    (tinfo_curr->tunid - tinfo_last->tunid))
			return -EINVAL;
		t = tinfo_last->tunid;
		for (v = tinfo_last->vid; v <= tinfo_curr->vid; v++) {
			bool curr_change = false;

			err = br_vlan_tunnel_info(p, cmd, v, t, &curr_change);
			if (err)
				break;
			t++;

			if (curr_change)
				*changed = curr_change;
			 __vlan_tunnel_handle_range(p, &v_start, &v_end, v,
						    curr_change);
		}
		if (v_start && v_end)
			br_vlan_notify(br, p, v_start->vid, v_end->vid,
				       RTM_NEWVLAN);
		if (err)
			return err;

		memset(tinfo_last, 0, sizeof(struct vtunnel_info));
		memset(tinfo_curr, 0, sizeof(struct vtunnel_info));
	} else {
		if (tinfo_last->flags)
			return -EINVAL;
		err = br_vlan_tunnel_info(p, cmd, tinfo_curr->vid,
					  tinfo_curr->tunid, changed);
		if (err)
			return err;
		br_vlan_notify(br, p, tinfo_curr->vid, 0, RTM_NEWVLAN);
		memset(tinfo_last, 0, sizeof(struct vtunnel_info));
		memset(tinfo_curr, 0, sizeof(struct vtunnel_info));
	}

	return 0;
}
