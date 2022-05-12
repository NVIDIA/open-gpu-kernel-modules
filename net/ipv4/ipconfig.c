// SPDX-License-Identifier: GPL-2.0
/*
 *  Automatic Configuration of IP -- use DHCP, BOOTP, RARP, or
 *  user-supplied information to configure own IP address and routes.
 *
 *  Copyright (C) 1996-1998 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *  Derived from network configuration code in fs/nfs/nfsroot.c,
 *  originally Copyright (C) 1995, 1996 Gero Kuhlmann and me.
 *
 *  BOOTP rewritten to construct and analyse packets itself instead
 *  of misusing the IP layer. num_bugs_causing_wrong_arp_replies--;
 *					     -- MJ, December 1998
 *
 *  Fixed ip_auto_config_setup calling at startup in the new "Linker Magic"
 *  initialization scheme.
 *	- Arnaldo Carvalho de Melo <acme@conectiva.com.br>, 08/11/1999
 *
 *  DHCP support added.  To users this looks like a whole separate
 *  protocol, but we know it's just a bag on the side of BOOTP.
 *		-- Chip Salzenberg <chip@valinux.com>, May 2000
 *
 *  Ported DHCP support from 2.2.16 to 2.4.0-test4
 *              -- Eric Biederman <ebiederman@lnxi.com>, 30 Aug 2000
 *
 *  Merged changes from 2.2.19 into 2.4.3
 *              -- Eric Biederman <ebiederman@lnxi.com>, 22 April Aug 2001
 *
 *  Multiple Nameservers in /proc/net/pnp
 *              --  Josef Siemes <jsiemes@web.de>, Aug 2002
 *
 *  NTP servers in /proc/net/ipconfig/ntp_servers
 *              --  Chris Novakovic <chris@chrisn.me.uk>, April 2018
 */

#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/utsname.h>
#include <linux/in.h>
#include <linux/if.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/socket.h>
#include <linux/route.h>
#include <linux/udp.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/major.h>
#include <linux/root_dev.h>
#include <linux/delay.h>
#include <linux/nfs_fs.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <net/net_namespace.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/ipconfig.h>
#include <net/route.h>

#include <linux/uaccess.h>
#include <net/checksum.h>
#include <asm/processor.h>

#if defined(CONFIG_IP_PNP_DHCP)
#define IPCONFIG_DHCP
#endif
#if defined(CONFIG_IP_PNP_BOOTP) || defined(CONFIG_IP_PNP_DHCP)
#define IPCONFIG_BOOTP
#endif
#if defined(CONFIG_IP_PNP_RARP)
#define IPCONFIG_RARP
#endif
#if defined(IPCONFIG_BOOTP) || defined(IPCONFIG_RARP)
#define IPCONFIG_DYNAMIC
#endif

/* Define the friendly delay before and after opening net devices */
#define CONF_POST_OPEN		10	/* After opening: 10 msecs */

/* Define the timeout for waiting for a DHCP/BOOTP/RARP reply */
#define CONF_OPEN_RETRIES 	2	/* (Re)open devices twice */
#define CONF_SEND_RETRIES 	6	/* Send six requests per open */
#define CONF_BASE_TIMEOUT	(HZ*2)	/* Initial timeout: 2 seconds */
#define CONF_TIMEOUT_RANDOM	(HZ)	/* Maximum amount of randomization */
#define CONF_TIMEOUT_MULT	*7/4	/* Rate of timeout growth */
#define CONF_TIMEOUT_MAX	(HZ*30)	/* Maximum allowed timeout */
#define CONF_NAMESERVERS_MAX   3       /* Maximum number of nameservers
					   - '3' from resolv.h */
#define CONF_NTP_SERVERS_MAX   3	/* Maximum number of NTP servers */

#define NONE cpu_to_be32(INADDR_NONE)
#define ANY cpu_to_be32(INADDR_ANY)

/* Wait for carrier timeout default in seconds */
static unsigned int carrier_timeout = 120;

/*
 * Public IP configuration
 */

/* This is used by platforms which might be able to set the ipconfig
 * variables using firmware environment vars.  If this is set, it will
 * ignore such firmware variables.
 */
int ic_set_manually __initdata = 0;		/* IPconfig parameters set manually */

static int ic_enable __initdata;		/* IP config enabled? */

/* Protocol choice */
int ic_proto_enabled __initdata = 0
#ifdef IPCONFIG_BOOTP
			| IC_BOOTP
#endif
#ifdef CONFIG_IP_PNP_DHCP
			| IC_USE_DHCP
#endif
#ifdef IPCONFIG_RARP
			| IC_RARP
#endif
			;

static int ic_host_name_set __initdata;	/* Host name set by us? */

__be32 ic_myaddr = NONE;		/* My IP address */
static __be32 ic_netmask = NONE;	/* Netmask for local subnet */
__be32 ic_gateway = NONE;	/* Gateway IP address */

#ifdef IPCONFIG_DYNAMIC
static __be32 ic_addrservaddr = NONE;	/* IP Address of the IP addresses'server */
#endif

__be32 ic_servaddr = NONE;	/* Boot server IP address */

__be32 root_server_addr = NONE;	/* Address of NFS server */
u8 root_server_path[256] = { 0, };	/* Path to mount as root */

/* vendor class identifier */
static char vendor_class_identifier[253] __initdata;

#if defined(CONFIG_IP_PNP_DHCP)
static char dhcp_client_identifier[253] __initdata;
#endif

/* Persistent data: */

#ifdef IPCONFIG_DYNAMIC
static int ic_proto_used;			/* Protocol used, if any */
#else
#define ic_proto_used 0
#endif
static __be32 ic_nameservers[CONF_NAMESERVERS_MAX]; /* DNS Server IP addresses */
static __be32 ic_ntp_servers[CONF_NTP_SERVERS_MAX]; /* NTP server IP addresses */
static u8 ic_domain[64];		/* DNS (not NIS) domain name */

/*
 * Private state.
 */

/* Name of user-selected boot device */
static char user_dev_name[IFNAMSIZ] __initdata = { 0, };

/* Protocols supported by available interfaces */
static int ic_proto_have_if __initdata;

/* MTU for boot device */
static int ic_dev_mtu __initdata;

#ifdef IPCONFIG_DYNAMIC
static DEFINE_SPINLOCK(ic_recv_lock);
static volatile int ic_got_reply __initdata;    /* Proto(s) that replied */
#endif
#ifdef IPCONFIG_DHCP
static int ic_dhcp_msgtype __initdata;	/* DHCP msg type received */
#endif


/*
 *	Network devices
 */

struct ic_device {
	struct ic_device *next;
	struct net_device *dev;
	unsigned short flags;
	short able;
	__be32 xid;
};

static struct ic_device *ic_first_dev __initdata;	/* List of open device */
static struct ic_device *ic_dev __initdata;		/* Selected device */

static bool __init ic_is_init_dev(struct net_device *dev)
{
	if (dev->flags & IFF_LOOPBACK)
		return false;
	return user_dev_name[0] ? !strcmp(dev->name, user_dev_name) :
	    (!(dev->flags & IFF_LOOPBACK) &&
	     (dev->flags & (IFF_POINTOPOINT|IFF_BROADCAST)) &&
	     strncmp(dev->name, "dummy", 5));
}

static int __init ic_open_devs(void)
{
	struct ic_device *d, **last;
	struct net_device *dev;
	unsigned short oflags;
	unsigned long start, next_msg;

	last = &ic_first_dev;
	rtnl_lock();

	/* bring loopback device up first */
	for_each_netdev(&init_net, dev) {
		if (!(dev->flags & IFF_LOOPBACK))
			continue;
		if (dev_change_flags(dev, dev->flags | IFF_UP, NULL) < 0)
			pr_err("IP-Config: Failed to open %s\n", dev->name);
	}

	for_each_netdev(&init_net, dev) {
		if (ic_is_init_dev(dev)) {
			int able = 0;
			if (dev->mtu >= 364)
				able |= IC_BOOTP;
			else
				pr_warn("DHCP/BOOTP: Ignoring device %s, MTU %d too small\n",
					dev->name, dev->mtu);
			if (!(dev->flags & IFF_NOARP))
				able |= IC_RARP;
			able &= ic_proto_enabled;
			if (ic_proto_enabled && !able)
				continue;
			oflags = dev->flags;
			if (dev_change_flags(dev, oflags | IFF_UP, NULL) < 0) {
				pr_err("IP-Config: Failed to open %s\n",
				       dev->name);
				continue;
			}
			if (!(d = kmalloc(sizeof(struct ic_device), GFP_KERNEL))) {
				rtnl_unlock();
				return -ENOMEM;
			}
			d->dev = dev;
			*last = d;
			last = &d->next;
			d->flags = oflags;
			d->able = able;
			if (able & IC_BOOTP)
				get_random_bytes(&d->xid, sizeof(__be32));
			else
				d->xid = 0;
			ic_proto_have_if |= able;
			pr_debug("IP-Config: %s UP (able=%d, xid=%08x)\n",
				 dev->name, able, d->xid);
		}
	}

	/* no point in waiting if we could not bring up at least one device */
	if (!ic_first_dev)
		goto have_carrier;

	/* wait for a carrier on at least one device */
	start = jiffies;
	next_msg = start + msecs_to_jiffies(20000);
	while (time_before(jiffies, start +
			   msecs_to_jiffies(carrier_timeout * 1000))) {
		int wait, elapsed;

		for_each_netdev(&init_net, dev)
			if (ic_is_init_dev(dev) && netif_carrier_ok(dev))
				goto have_carrier;

		msleep(1);

		if (time_before(jiffies, next_msg))
			continue;

		elapsed = jiffies_to_msecs(jiffies - start);
		wait = (carrier_timeout * 1000 - elapsed + 500) / 1000;
		pr_info("Waiting up to %d more seconds for network.\n", wait);
		next_msg = jiffies + msecs_to_jiffies(20000);
	}
have_carrier:
	rtnl_unlock();

	*last = NULL;

	if (!ic_first_dev) {
		if (user_dev_name[0])
			pr_err("IP-Config: Device `%s' not found\n",
			       user_dev_name);
		else
			pr_err("IP-Config: No network devices available\n");
		return -ENODEV;
	}
	return 0;
}

/* Close all network interfaces except the one we've autoconfigured, and its
 * lowers, in case it's a stacked virtual interface.
 */
static void __init ic_close_devs(void)
{
	struct net_device *selected_dev = ic_dev ? ic_dev->dev : NULL;
	struct ic_device *d, *next;
	struct net_device *dev;

	rtnl_lock();
	next = ic_first_dev;
	while ((d = next)) {
		bool bring_down = (d != ic_dev);
		struct net_device *lower;
		struct list_head *iter;

		next = d->next;
		dev = d->dev;

		if (selected_dev) {
			netdev_for_each_lower_dev(selected_dev, lower, iter) {
				if (dev == lower) {
					bring_down = false;
					break;
				}
			}
		}
		if (bring_down) {
			pr_debug("IP-Config: Downing %s\n", dev->name);
			dev_change_flags(dev, d->flags, NULL);
		}
		kfree(d);
	}
	rtnl_unlock();
}

/*
 *	Interface to various network functions.
 */

static inline void
set_sockaddr(struct sockaddr_in *sin, __be32 addr, __be16 port)
{
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = addr;
	sin->sin_port = port;
}

/*
 *	Set up interface addresses and routes.
 */

static int __init ic_setup_if(void)
{
	struct ifreq ir;
	struct sockaddr_in *sin = (void *) &ir.ifr_ifru.ifru_addr;
	int err;

	memset(&ir, 0, sizeof(ir));
	strcpy(ir.ifr_ifrn.ifrn_name, ic_dev->dev->name);
	set_sockaddr(sin, ic_myaddr, 0);
	if ((err = devinet_ioctl(&init_net, SIOCSIFADDR, &ir)) < 0) {
		pr_err("IP-Config: Unable to set interface address (%d)\n",
		       err);
		return -1;
	}
	set_sockaddr(sin, ic_netmask, 0);
	if ((err = devinet_ioctl(&init_net, SIOCSIFNETMASK, &ir)) < 0) {
		pr_err("IP-Config: Unable to set interface netmask (%d)\n",
		       err);
		return -1;
	}
	set_sockaddr(sin, ic_myaddr | ~ic_netmask, 0);
	if ((err = devinet_ioctl(&init_net, SIOCSIFBRDADDR, &ir)) < 0) {
		pr_err("IP-Config: Unable to set interface broadcast address (%d)\n",
		       err);
		return -1;
	}
	/* Handle the case where we need non-standard MTU on the boot link (a network
	 * using jumbo frames, for instance).  If we can't set the mtu, don't error
	 * out, we'll try to muddle along.
	 */
	if (ic_dev_mtu != 0) {
		rtnl_lock();
		if ((err = dev_set_mtu(ic_dev->dev, ic_dev_mtu)) < 0)
			pr_err("IP-Config: Unable to set interface mtu to %d (%d)\n",
			       ic_dev_mtu, err);
		rtnl_unlock();
	}
	return 0;
}

static int __init ic_setup_routes(void)
{
	/* No need to setup device routes, only the default route... */

	if (ic_gateway != NONE) {
		struct rtentry rm;
		int err;

		memset(&rm, 0, sizeof(rm));
		if ((ic_gateway ^ ic_myaddr) & ic_netmask) {
			pr_err("IP-Config: Gateway not on directly connected network\n");
			return -1;
		}
		set_sockaddr((struct sockaddr_in *) &rm.rt_dst, 0, 0);
		set_sockaddr((struct sockaddr_in *) &rm.rt_genmask, 0, 0);
		set_sockaddr((struct sockaddr_in *) &rm.rt_gateway, ic_gateway, 0);
		rm.rt_flags = RTF_UP | RTF_GATEWAY;
		if ((err = ip_rt_ioctl(&init_net, SIOCADDRT, &rm)) < 0) {
			pr_err("IP-Config: Cannot add default route (%d)\n",
			       err);
			return -1;
		}
	}

	return 0;
}

/*
 *	Fill in default values for all missing parameters.
 */

static int __init ic_defaults(void)
{
	/*
	 *	At this point we have no userspace running so need not
	 *	claim locks on system_utsname
	 */

	if (!ic_host_name_set)
		sprintf(init_utsname()->nodename, "%pI4", &ic_myaddr);

	if (root_server_addr == NONE)
		root_server_addr = ic_servaddr;

	if (ic_netmask == NONE) {
		if (IN_CLASSA(ntohl(ic_myaddr)))
			ic_netmask = htonl(IN_CLASSA_NET);
		else if (IN_CLASSB(ntohl(ic_myaddr)))
			ic_netmask = htonl(IN_CLASSB_NET);
		else if (IN_CLASSC(ntohl(ic_myaddr)))
			ic_netmask = htonl(IN_CLASSC_NET);
		else if (IN_CLASSE(ntohl(ic_myaddr)))
			ic_netmask = htonl(IN_CLASSE_NET);
		else {
			pr_err("IP-Config: Unable to guess netmask for address %pI4\n",
			       &ic_myaddr);
			return -1;
		}
		pr_notice("IP-Config: Guessing netmask %pI4\n",
			  &ic_netmask);
	}

	return 0;
}

/*
 *	RARP support.
 */

#ifdef IPCONFIG_RARP

static int ic_rarp_recv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev);

static struct packet_type rarp_packet_type __initdata = {
	.type =	cpu_to_be16(ETH_P_RARP),
	.func =	ic_rarp_recv,
};

static inline void __init ic_rarp_init(void)
{
	dev_add_pack(&rarp_packet_type);
}

static inline void __init ic_rarp_cleanup(void)
{
	dev_remove_pack(&rarp_packet_type);
}

/*
 *  Process received RARP packet.
 */
static int __init
ic_rarp_recv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
	struct arphdr *rarp;
	unsigned char *rarp_ptr;
	__be32 sip, tip;
	unsigned char *tha;		/* t for "target" */
	struct ic_device *d;

	if (!net_eq(dev_net(dev), &init_net))
		goto drop;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return NET_RX_DROP;

	if (!pskb_may_pull(skb, sizeof(struct arphdr)))
		goto drop;

	/* Basic sanity checks can be done without the lock.  */
	rarp = (struct arphdr *)skb_transport_header(skb);

	/* If this test doesn't pass, it's not IP, or we should
	 * ignore it anyway.
	 */
	if (rarp->ar_hln != dev->addr_len || dev->type != ntohs(rarp->ar_hrd))
		goto drop;

	/* If it's not a RARP reply, delete it. */
	if (rarp->ar_op != htons(ARPOP_RREPLY))
		goto drop;

	/* If it's not Ethernet, delete it. */
	if (rarp->ar_pro != htons(ETH_P_IP))
		goto drop;

	if (!pskb_may_pull(skb, arp_hdr_len(dev)))
		goto drop;

	/* OK, it is all there and looks valid, process... */
	rarp = (struct arphdr *)skb_transport_header(skb);
	rarp_ptr = (unsigned char *) (rarp + 1);

	/* One reply at a time, please. */
	spin_lock(&ic_recv_lock);

	/* If we already have a reply, just drop the packet */
	if (ic_got_reply)
		goto drop_unlock;

	/* Find the ic_device that the packet arrived on */
	d = ic_first_dev;
	while (d && d->dev != dev)
		d = d->next;
	if (!d)
		goto drop_unlock;	/* should never happen */

	/* Extract variable-width fields */
	rarp_ptr += dev->addr_len;
	memcpy(&sip, rarp_ptr, 4);
	rarp_ptr += 4;
	tha = rarp_ptr;
	rarp_ptr += dev->addr_len;
	memcpy(&tip, rarp_ptr, 4);

	/* Discard packets which are not meant for us. */
	if (memcmp(tha, dev->dev_addr, dev->addr_len))
		goto drop_unlock;

	/* Discard packets which are not from specified server. */
	if (ic_servaddr != NONE && ic_servaddr != sip)
		goto drop_unlock;

	/* We have a winner! */
	ic_dev = d;
	if (ic_myaddr == NONE)
		ic_myaddr = tip;
	ic_servaddr = sip;
	ic_addrservaddr = sip;
	ic_got_reply = IC_RARP;

drop_unlock:
	/* Show's over.  Nothing to see here.  */
	spin_unlock(&ic_recv_lock);

drop:
	/* Throw the packet out. */
	kfree_skb(skb);
	return 0;
}


/*
 *  Send RARP request packet over a single interface.
 */
static void __init ic_rarp_send_if(struct ic_device *d)
{
	struct net_device *dev = d->dev;
	arp_send(ARPOP_RREQUEST, ETH_P_RARP, 0, dev, 0, NULL,
		 dev->dev_addr, dev->dev_addr);
}
#endif

/*
 *  Predefine Nameservers
 */
static inline void __init ic_nameservers_predef(void)
{
	int i;

	for (i = 0; i < CONF_NAMESERVERS_MAX; i++)
		ic_nameservers[i] = NONE;
}

/* Predefine NTP servers */
static inline void __init ic_ntp_servers_predef(void)
{
	int i;

	for (i = 0; i < CONF_NTP_SERVERS_MAX; i++)
		ic_ntp_servers[i] = NONE;
}

/*
 *	DHCP/BOOTP support.
 */

#ifdef IPCONFIG_BOOTP

struct bootp_pkt {		/* BOOTP packet format */
	struct iphdr iph;	/* IP header */
	struct udphdr udph;	/* UDP header */
	u8 op;			/* 1=request, 2=reply */
	u8 htype;		/* HW address type */
	u8 hlen;		/* HW address length */
	u8 hops;		/* Used only by gateways */
	__be32 xid;		/* Transaction ID */
	__be16 secs;		/* Seconds since we started */
	__be16 flags;		/* Just what it says */
	__be32 client_ip;		/* Client's IP address if known */
	__be32 your_ip;		/* Assigned IP address */
	__be32 server_ip;		/* (Next, e.g. NFS) Server's IP address */
	__be32 relay_ip;		/* IP address of BOOTP relay */
	u8 hw_addr[16];		/* Client's HW address */
	u8 serv_name[64];	/* Server host name */
	u8 boot_file[128];	/* Name of boot file */
	u8 exten[312];		/* DHCP options / BOOTP vendor extensions */
};

/* packet ops */
#define BOOTP_REQUEST	1
#define BOOTP_REPLY	2

/* DHCP message types */
#define DHCPDISCOVER	1
#define DHCPOFFER	2
#define DHCPREQUEST	3
#define DHCPDECLINE	4
#define DHCPACK		5
#define DHCPNAK		6
#define DHCPRELEASE	7
#define DHCPINFORM	8

static int ic_bootp_recv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev);

static struct packet_type bootp_packet_type __initdata = {
	.type =	cpu_to_be16(ETH_P_IP),
	.func =	ic_bootp_recv,
};

/*
 *  Initialize DHCP/BOOTP extension fields in the request.
 */

static const u8 ic_bootp_cookie[4] = { 99, 130, 83, 99 };

#ifdef IPCONFIG_DHCP

static void __init
ic_dhcp_init_options(u8 *options, struct ic_device *d)
{
	u8 mt = ((ic_servaddr == NONE)
		 ? DHCPDISCOVER : DHCPREQUEST);
	u8 *e = options;
	int len;

	pr_debug("DHCP: Sending message type %d (%s)\n", mt, d->dev->name);

	memcpy(e, ic_bootp_cookie, 4);	/* RFC1048 Magic Cookie */
	e += 4;

	*e++ = 53;		/* DHCP message type */
	*e++ = 1;
	*e++ = mt;

	if (mt == DHCPREQUEST) {
		*e++ = 54;	/* Server ID (IP address) */
		*e++ = 4;
		memcpy(e, &ic_servaddr, 4);
		e += 4;

		*e++ = 50;	/* Requested IP address */
		*e++ = 4;
		memcpy(e, &ic_myaddr, 4);
		e += 4;
	}

	/* always? */
	{
		static const u8 ic_req_params[] = {
			1,	/* Subnet mask */
			3,	/* Default gateway */
			6,	/* DNS server */
			12,	/* Host name */
			15,	/* Domain name */
			17,	/* Boot path */
			26,	/* MTU */
			40,	/* NIS domain name */
			42,	/* NTP servers */
		};

		*e++ = 55;	/* Parameter request list */
		*e++ = sizeof(ic_req_params);
		memcpy(e, ic_req_params, sizeof(ic_req_params));
		e += sizeof(ic_req_params);

		if (ic_host_name_set) {
			*e++ = 12;	/* host-name */
			len = strlen(utsname()->nodename);
			*e++ = len;
			memcpy(e, utsname()->nodename, len);
			e += len;
		}
		if (*vendor_class_identifier) {
			pr_info("DHCP: sending class identifier \"%s\"\n",
				vendor_class_identifier);
			*e++ = 60;	/* Class-identifier */
			len = strlen(vendor_class_identifier);
			*e++ = len;
			memcpy(e, vendor_class_identifier, len);
			e += len;
		}
		len = strlen(dhcp_client_identifier + 1);
		/* the minimum length of identifier is 2, include 1 byte type,
		 * and can not be larger than the length of options
		 */
		if (len >= 1 && len < 312 - (e - options) - 1) {
			*e++ = 61;
			*e++ = len + 1;
			memcpy(e, dhcp_client_identifier, len + 1);
			e += len + 1;
		}
	}

	*e++ = 255;	/* End of the list */
}

#endif /* IPCONFIG_DHCP */

static void __init ic_bootp_init_ext(u8 *e)
{
	memcpy(e, ic_bootp_cookie, 4);	/* RFC1048 Magic Cookie */
	e += 4;
	*e++ = 1;		/* Subnet mask request */
	*e++ = 4;
	e += 4;
	*e++ = 3;		/* Default gateway request */
	*e++ = 4;
	e += 4;
#if CONF_NAMESERVERS_MAX > 0
	*e++ = 6;		/* (DNS) name server request */
	*e++ = 4 * CONF_NAMESERVERS_MAX;
	e += 4 * CONF_NAMESERVERS_MAX;
#endif
	*e++ = 12;		/* Host name request */
	*e++ = 32;
	e += 32;
	*e++ = 40;		/* NIS Domain name request */
	*e++ = 32;
	e += 32;
	*e++ = 17;		/* Boot path */
	*e++ = 40;
	e += 40;

	*e++ = 57;		/* set extension buffer size for reply */
	*e++ = 2;
	*e++ = 1;		/* 128+236+8+20+14, see dhcpd sources */
	*e++ = 150;

	*e++ = 255;		/* End of the list */
}


/*
 *  Initialize the DHCP/BOOTP mechanism.
 */
static inline void __init ic_bootp_init(void)
{
	/* Re-initialise all name servers and NTP servers to NONE, in case any
	 * were set via the "ip=" or "nfsaddrs=" kernel command line parameters:
	 * any IP addresses specified there will already have been decoded but
	 * are no longer needed
	 */
	ic_nameservers_predef();
	ic_ntp_servers_predef();

	dev_add_pack(&bootp_packet_type);
}


/*
 *  DHCP/BOOTP cleanup.
 */
static inline void __init ic_bootp_cleanup(void)
{
	dev_remove_pack(&bootp_packet_type);
}


/*
 *  Send DHCP/BOOTP request to single interface.
 */
static void __init ic_bootp_send_if(struct ic_device *d, unsigned long jiffies_diff)
{
	struct net_device *dev = d->dev;
	struct sk_buff *skb;
	struct bootp_pkt *b;
	struct iphdr *h;
	int hlen = LL_RESERVED_SPACE(dev);
	int tlen = dev->needed_tailroom;

	/* Allocate packet */
	skb = alloc_skb(sizeof(struct bootp_pkt) + hlen + tlen + 15,
			GFP_KERNEL);
	if (!skb)
		return;
	skb_reserve(skb, hlen);
	b = skb_put_zero(skb, sizeof(struct bootp_pkt));

	/* Construct IP header */
	skb_reset_network_header(skb);
	h = ip_hdr(skb);
	h->version = 4;
	h->ihl = 5;
	h->tot_len = htons(sizeof(struct bootp_pkt));
	h->frag_off = htons(IP_DF);
	h->ttl = 64;
	h->protocol = IPPROTO_UDP;
	h->daddr = htonl(INADDR_BROADCAST);
	h->check = ip_fast_csum((unsigned char *) h, h->ihl);

	/* Construct UDP header */
	b->udph.source = htons(68);
	b->udph.dest = htons(67);
	b->udph.len = htons(sizeof(struct bootp_pkt) - sizeof(struct iphdr));
	/* UDP checksum not calculated -- explicitly allowed in BOOTP RFC */

	/* Construct DHCP/BOOTP header */
	b->op = BOOTP_REQUEST;
	if (dev->type < 256) /* check for false types */
		b->htype = dev->type;
	else if (dev->type == ARPHRD_FDDI)
		b->htype = ARPHRD_ETHER;
	else {
		pr_warn("Unknown ARP type 0x%04x for device %s\n", dev->type,
			dev->name);
		b->htype = dev->type; /* can cause undefined behavior */
	}

	/* server_ip and your_ip address are both already zero per RFC2131 */
	b->hlen = dev->addr_len;
	memcpy(b->hw_addr, dev->dev_addr, dev->addr_len);
	b->secs = htons(jiffies_diff / HZ);
	b->xid = d->xid;

	/* add DHCP options or BOOTP extensions */
#ifdef IPCONFIG_DHCP
	if (ic_proto_enabled & IC_USE_DHCP)
		ic_dhcp_init_options(b->exten, d);
	else
#endif
		ic_bootp_init_ext(b->exten);

	/* Chain packet down the line... */
	skb->dev = dev;
	skb->protocol = htons(ETH_P_IP);
	if (dev_hard_header(skb, dev, ntohs(skb->protocol),
			    dev->broadcast, dev->dev_addr, skb->len) < 0) {
		kfree_skb(skb);
		printk("E");
		return;
	}

	if (dev_queue_xmit(skb) < 0)
		printk("E");
}


/*
 *  Copy BOOTP-supplied string
 */
static int __init ic_bootp_string(char *dest, char *src, int len, int max)
{
	if (!len)
		return 0;
	if (len > max-1)
		len = max-1;
	memcpy(dest, src, len);
	dest[len] = '\0';
	return 1;
}


/*
 *  Process BOOTP extensions.
 */
static void __init ic_do_bootp_ext(u8 *ext)
{
	u8 servers;
	int i;
	__be16 mtu;

	u8 *c;

	pr_debug("DHCP/BOOTP: Got extension %d:", *ext);
	for (c=ext+2; c<ext+2+ext[1]; c++)
		pr_debug(" %02x", *c);
	pr_debug("\n");

	switch (*ext++) {
	case 1:		/* Subnet mask */
		if (ic_netmask == NONE)
			memcpy(&ic_netmask, ext+1, 4);
		break;
	case 3:		/* Default gateway */
		if (ic_gateway == NONE)
			memcpy(&ic_gateway, ext+1, 4);
		break;
	case 6:		/* DNS server */
		servers= *ext/4;
		if (servers > CONF_NAMESERVERS_MAX)
			servers = CONF_NAMESERVERS_MAX;
		for (i = 0; i < servers; i++) {
			if (ic_nameservers[i] == NONE)
				memcpy(&ic_nameservers[i], ext+1+4*i, 4);
		}
		break;
	case 12:	/* Host name */
		if (!ic_host_name_set) {
			ic_bootp_string(utsname()->nodename, ext+1, *ext,
					__NEW_UTS_LEN);
			ic_host_name_set = 1;
		}
		break;
	case 15:	/* Domain name (DNS) */
		if (!ic_domain[0])
			ic_bootp_string(ic_domain, ext+1, *ext, sizeof(ic_domain));
		break;
	case 17:	/* Root path */
		if (!root_server_path[0])
			ic_bootp_string(root_server_path, ext+1, *ext,
					sizeof(root_server_path));
		break;
	case 26:	/* Interface MTU */
		memcpy(&mtu, ext+1, sizeof(mtu));
		ic_dev_mtu = ntohs(mtu);
		break;
	case 40:	/* NIS Domain name (_not_ DNS) */
		ic_bootp_string(utsname()->domainname, ext+1, *ext,
				__NEW_UTS_LEN);
		break;
	case 42:	/* NTP servers */
		servers = *ext / 4;
		if (servers > CONF_NTP_SERVERS_MAX)
			servers = CONF_NTP_SERVERS_MAX;
		for (i = 0; i < servers; i++) {
			if (ic_ntp_servers[i] == NONE)
				memcpy(&ic_ntp_servers[i], ext+1+4*i, 4);
		}
		break;
	}
}


/*
 *  Receive BOOTP reply.
 */
static int __init ic_bootp_recv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
	struct bootp_pkt *b;
	struct iphdr *h;
	struct ic_device *d;
	int len, ext_len;

	if (!net_eq(dev_net(dev), &init_net))
		goto drop;

	/* Perform verifications before taking the lock.  */
	if (skb->pkt_type == PACKET_OTHERHOST)
		goto drop;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return NET_RX_DROP;

	if (!pskb_may_pull(skb,
			   sizeof(struct iphdr) +
			   sizeof(struct udphdr)))
		goto drop;

	b = (struct bootp_pkt *)skb_network_header(skb);
	h = &b->iph;

	if (h->ihl != 5 || h->version != 4 || h->protocol != IPPROTO_UDP)
		goto drop;

	/* Fragments are not supported */
	if (ip_is_fragment(h)) {
		net_err_ratelimited("DHCP/BOOTP: Ignoring fragmented reply\n");
		goto drop;
	}

	if (skb->len < ntohs(h->tot_len))
		goto drop;

	if (ip_fast_csum((char *) h, h->ihl))
		goto drop;

	if (b->udph.source != htons(67) || b->udph.dest != htons(68))
		goto drop;

	if (ntohs(h->tot_len) < ntohs(b->udph.len) + sizeof(struct iphdr))
		goto drop;

	len = ntohs(b->udph.len) - sizeof(struct udphdr);
	ext_len = len - (sizeof(*b) -
			 sizeof(struct iphdr) -
			 sizeof(struct udphdr) -
			 sizeof(b->exten));
	if (ext_len < 0)
		goto drop;

	/* Ok the front looks good, make sure we can get at the rest.  */
	if (!pskb_may_pull(skb, skb->len))
		goto drop;

	b = (struct bootp_pkt *)skb_network_header(skb);
	h = &b->iph;

	/* One reply at a time, please. */
	spin_lock(&ic_recv_lock);

	/* If we already have a reply, just drop the packet */
	if (ic_got_reply)
		goto drop_unlock;

	/* Find the ic_device that the packet arrived on */
	d = ic_first_dev;
	while (d && d->dev != dev)
		d = d->next;
	if (!d)
		goto drop_unlock;  /* should never happen */

	/* Is it a reply to our BOOTP request? */
	if (b->op != BOOTP_REPLY ||
	    b->xid != d->xid) {
		net_err_ratelimited("DHCP/BOOTP: Reply not for us on %s, op[%x] xid[%x]\n",
				    d->dev->name, b->op, b->xid);
		goto drop_unlock;
	}

	/* Parse extensions */
	if (ext_len >= 4 &&
	    !memcmp(b->exten, ic_bootp_cookie, 4)) { /* Check magic cookie */
		u8 *end = (u8 *) b + ntohs(b->iph.tot_len);
		u8 *ext;

#ifdef IPCONFIG_DHCP
		if (ic_proto_enabled & IC_USE_DHCP) {
			__be32 server_id = NONE;
			int mt = 0;

			ext = &b->exten[4];
			while (ext < end && *ext != 0xff) {
				u8 *opt = ext++;
				if (*opt == 0)	/* Padding */
					continue;
				ext += *ext + 1;
				if (ext >= end)
					break;
				switch (*opt) {
				case 53:	/* Message type */
					if (opt[1])
						mt = opt[2];
					break;
				case 54:	/* Server ID (IP address) */
					if (opt[1] >= 4)
						memcpy(&server_id, opt + 2, 4);
					break;
				}
			}

			pr_debug("DHCP: Got message type %d (%s)\n", mt, d->dev->name);

			switch (mt) {
			case DHCPOFFER:
				/* While in the process of accepting one offer,
				 * ignore all others.
				 */
				if (ic_myaddr != NONE)
					goto drop_unlock;

				/* Let's accept that offer. */
				ic_myaddr = b->your_ip;
				ic_servaddr = server_id;
				pr_debug("DHCP: Offered address %pI4 by server %pI4\n",
					 &ic_myaddr, &b->iph.saddr);
				/* The DHCP indicated server address takes
				 * precedence over the bootp header one if
				 * they are different.
				 */
				if ((server_id != NONE) &&
				    (b->server_ip != server_id))
					b->server_ip = ic_servaddr;
				break;

			case DHCPACK:
				if (memcmp(dev->dev_addr, b->hw_addr, dev->addr_len) != 0)
					goto drop_unlock;

				/* Yeah! */
				break;

			default:
				/* Urque.  Forget it*/
				ic_myaddr = NONE;
				ic_servaddr = NONE;
				goto drop_unlock;
			}

			ic_dhcp_msgtype = mt;

		}
#endif /* IPCONFIG_DHCP */

		ext = &b->exten[4];
		while (ext < end && *ext != 0xff) {
			u8 *opt = ext++;
			if (*opt == 0)	/* Padding */
				continue;
			ext += *ext + 1;
			if (ext < end)
				ic_do_bootp_ext(opt);
		}
	}

	/* We have a winner! */
	ic_dev = d;
	ic_myaddr = b->your_ip;
	ic_servaddr = b->server_ip;
	ic_addrservaddr = b->iph.saddr;
	if (ic_gateway == NONE && b->relay_ip)
		ic_gateway = b->relay_ip;
	if (ic_nameservers[0] == NONE)
		ic_nameservers[0] = ic_servaddr;
	ic_got_reply = IC_BOOTP;

drop_unlock:
	/* Show's over.  Nothing to see here.  */
	spin_unlock(&ic_recv_lock);

drop:
	/* Throw the packet out. */
	kfree_skb(skb);

	return 0;
}


#endif


/*
 *	Dynamic IP configuration -- DHCP, BOOTP, RARP.
 */

#ifdef IPCONFIG_DYNAMIC

static int __init ic_dynamic(void)
{
	int retries;
	struct ic_device *d;
	unsigned long start_jiffies, timeout, jiff;
	int do_bootp = ic_proto_have_if & IC_BOOTP;
	int do_rarp = ic_proto_have_if & IC_RARP;

	/*
	 * If none of DHCP/BOOTP/RARP was selected, return with an error.
	 * This routine gets only called when some pieces of information
	 * are missing, and without DHCP/BOOTP/RARP we are unable to get it.
	 */
	if (!ic_proto_enabled) {
		pr_err("IP-Config: Incomplete network configuration information\n");
		return -1;
	}

#ifdef IPCONFIG_BOOTP
	if ((ic_proto_enabled ^ ic_proto_have_if) & IC_BOOTP)
		pr_err("DHCP/BOOTP: No suitable device found\n");
#endif
#ifdef IPCONFIG_RARP
	if ((ic_proto_enabled ^ ic_proto_have_if) & IC_RARP)
		pr_err("RARP: No suitable device found\n");
#endif

	if (!ic_proto_have_if)
		/* Error message already printed */
		return -1;

	/*
	 * Setup protocols
	 */
#ifdef IPCONFIG_BOOTP
	if (do_bootp)
		ic_bootp_init();
#endif
#ifdef IPCONFIG_RARP
	if (do_rarp)
		ic_rarp_init();
#endif

	/*
	 * Send requests and wait, until we get an answer. This loop
	 * seems to be a terrible waste of CPU time, but actually there is
	 * only one process running at all, so we don't need to use any
	 * scheduler functions.
	 * [Actually we could now, but the nothing else running note still
	 *  applies.. - AC]
	 */
	pr_notice("Sending %s%s%s requests .",
		  do_bootp
		  ? ((ic_proto_enabled & IC_USE_DHCP) ? "DHCP" : "BOOTP") : "",
		  (do_bootp && do_rarp) ? " and " : "",
		  do_rarp ? "RARP" : "");

	start_jiffies = jiffies;
	d = ic_first_dev;
	retries = CONF_SEND_RETRIES;
	get_random_bytes(&timeout, sizeof(timeout));
	timeout = CONF_BASE_TIMEOUT + (timeout % (unsigned int) CONF_TIMEOUT_RANDOM);
	for (;;) {
#ifdef IPCONFIG_BOOTP
		if (do_bootp && (d->able & IC_BOOTP))
			ic_bootp_send_if(d, jiffies - start_jiffies);
#endif
#ifdef IPCONFIG_RARP
		if (do_rarp && (d->able & IC_RARP))
			ic_rarp_send_if(d);
#endif

		if (!d->next) {
			jiff = jiffies + timeout;
			while (time_before(jiffies, jiff) && !ic_got_reply)
				schedule_timeout_uninterruptible(1);
		}
#ifdef IPCONFIG_DHCP
		/* DHCP isn't done until we get a DHCPACK. */
		if ((ic_got_reply & IC_BOOTP) &&
		    (ic_proto_enabled & IC_USE_DHCP) &&
		    ic_dhcp_msgtype != DHCPACK) {
			ic_got_reply = 0;
			/* continue on device that got the reply */
			d = ic_dev;
			pr_cont(",");
			continue;
		}
#endif /* IPCONFIG_DHCP */

		if (ic_got_reply) {
			pr_cont(" OK\n");
			break;
		}

		if ((d = d->next))
			continue;

		if (! --retries) {
			pr_cont(" timed out!\n");
			break;
		}

		d = ic_first_dev;

		timeout = timeout CONF_TIMEOUT_MULT;
		if (timeout > CONF_TIMEOUT_MAX)
			timeout = CONF_TIMEOUT_MAX;

		pr_cont(".");
	}

#ifdef IPCONFIG_BOOTP
	if (do_bootp)
		ic_bootp_cleanup();
#endif
#ifdef IPCONFIG_RARP
	if (do_rarp)
		ic_rarp_cleanup();
#endif

	if (!ic_got_reply) {
		ic_myaddr = NONE;
		return -1;
	}

	pr_info("IP-Config: Got %s answer from %pI4, my address is %pI4\n",
		((ic_got_reply & IC_RARP) ? "RARP"
		: (ic_proto_enabled & IC_USE_DHCP) ? "DHCP" : "BOOTP"),
		&ic_addrservaddr, &ic_myaddr);

	return 0;
}

#endif /* IPCONFIG_DYNAMIC */

#ifdef CONFIG_PROC_FS
/* proc_dir_entry for /proc/net/ipconfig */
static struct proc_dir_entry *ipconfig_dir;

/* Name servers: */
static int pnp_seq_show(struct seq_file *seq, void *v)
{
	int i;

	if (ic_proto_used & IC_PROTO)
		seq_printf(seq, "#PROTO: %s\n",
			   (ic_proto_used & IC_RARP) ? "RARP"
			   : (ic_proto_used & IC_USE_DHCP) ? "DHCP" : "BOOTP");
	else
		seq_puts(seq, "#MANUAL\n");

	if (ic_domain[0])
		seq_printf(seq,
			   "domain %s\n", ic_domain);
	for (i = 0; i < CONF_NAMESERVERS_MAX; i++) {
		if (ic_nameservers[i] != NONE)
			seq_printf(seq, "nameserver %pI4\n",
				   &ic_nameservers[i]);
	}
	if (ic_servaddr != NONE)
		seq_printf(seq, "bootserver %pI4\n",
			   &ic_servaddr);
	return 0;
}

/* Create the /proc/net/ipconfig directory */
static int __init ipconfig_proc_net_init(void)
{
	ipconfig_dir = proc_net_mkdir(&init_net, "ipconfig", init_net.proc_net);
	if (!ipconfig_dir)
		return -ENOMEM;

	return 0;
}

/* Create a new file under /proc/net/ipconfig */
static int ipconfig_proc_net_create(const char *name,
				    const struct proc_ops *proc_ops)
{
	char *pname;
	struct proc_dir_entry *p;

	if (!ipconfig_dir)
		return -ENOMEM;

	pname = kasprintf(GFP_KERNEL, "%s%s", "ipconfig/", name);
	if (!pname)
		return -ENOMEM;

	p = proc_create(pname, 0444, init_net.proc_net, proc_ops);
	kfree(pname);
	if (!p)
		return -ENOMEM;

	return 0;
}

/* Write NTP server IP addresses to /proc/net/ipconfig/ntp_servers */
static int ntp_servers_show(struct seq_file *seq, void *v)
{
	int i;

	for (i = 0; i < CONF_NTP_SERVERS_MAX; i++) {
		if (ic_ntp_servers[i] != NONE)
			seq_printf(seq, "%pI4\n", &ic_ntp_servers[i]);
	}
	return 0;
}
DEFINE_PROC_SHOW_ATTRIBUTE(ntp_servers);
#endif /* CONFIG_PROC_FS */

/*
 *  Extract IP address from the parameter string if needed. Note that we
 *  need to have root_server_addr set _before_ IPConfig gets called as it
 *  can override it.
 */
__be32 __init root_nfs_parse_addr(char *name)
{
	__be32 addr;
	int octets = 0;
	char *cp, *cq;

	cp = cq = name;
	while (octets < 4) {
		while (*cp >= '0' && *cp <= '9')
			cp++;
		if (cp == cq || cp - cq > 3)
			break;
		if (*cp == '.' || octets == 3)
			octets++;
		if (octets < 4)
			cp++;
		cq = cp;
	}
	if (octets == 4 && (*cp == ':' || *cp == '\0')) {
		if (*cp == ':')
			*cp++ = '\0';
		addr = in_aton(name);
		memmove(name, cp, strlen(cp) + 1);
	} else
		addr = NONE;

	return addr;
}

#define DEVICE_WAIT_MAX		12 /* 12 seconds */

static int __init wait_for_devices(void)
{
	int i;

	for (i = 0; i < DEVICE_WAIT_MAX; i++) {
		struct net_device *dev;
		int found = 0;

		/* make sure deferred device probes are finished */
		wait_for_device_probe();

		rtnl_lock();
		for_each_netdev(&init_net, dev) {
			if (ic_is_init_dev(dev)) {
				found = 1;
				break;
			}
		}
		rtnl_unlock();
		if (found)
			return 0;
		ssleep(1);
	}
	return -ENODEV;
}

/*
 *	IP Autoconfig dispatcher.
 */

static int __init ip_auto_config(void)
{
	__be32 addr;
#ifdef IPCONFIG_DYNAMIC
	int retries = CONF_OPEN_RETRIES;
#endif
	int err;
	unsigned int i, count;

	/* Initialise all name servers and NTP servers to NONE (but only if the
	 * "ip=" or "nfsaddrs=" kernel command line parameters weren't decoded,
	 * otherwise we'll overwrite the IP addresses specified there)
	 */
	if (ic_set_manually == 0) {
		ic_nameservers_predef();
		ic_ntp_servers_predef();
	}

#ifdef CONFIG_PROC_FS
	proc_create_single("pnp", 0444, init_net.proc_net, pnp_seq_show);

	if (ipconfig_proc_net_init() == 0)
		ipconfig_proc_net_create("ntp_servers", &ntp_servers_proc_ops);
#endif /* CONFIG_PROC_FS */

	if (!ic_enable)
		return 0;

	pr_debug("IP-Config: Entered.\n");
#ifdef IPCONFIG_DYNAMIC
 try_try_again:
#endif
	/* Wait for devices to appear */
	err = wait_for_devices();
	if (err)
		return err;

	/* Setup all network devices */
	err = ic_open_devs();
	if (err)
		return err;

	/* Give drivers a chance to settle */
	msleep(CONF_POST_OPEN);

	/*
	 * If the config information is insufficient (e.g., our IP address or
	 * IP address of the boot server is missing or we have multiple network
	 * interfaces and no default was set), use BOOTP or RARP to get the
	 * missing values.
	 */
	if (ic_myaddr == NONE ||
#if defined(CONFIG_ROOT_NFS) || defined(CONFIG_CIFS_ROOT)
	    (root_server_addr == NONE &&
	     ic_servaddr == NONE &&
	     (ROOT_DEV == Root_NFS || ROOT_DEV == Root_CIFS)) ||
#endif
	    ic_first_dev->next) {
#ifdef IPCONFIG_DYNAMIC
		if (ic_dynamic() < 0) {
			ic_close_devs();

			/*
			 * I don't know why, but sometimes the
			 * eepro100 driver (at least) gets upset and
			 * doesn't work the first time it's opened.
			 * But then if you close it and reopen it, it
			 * works just fine.  So we need to try that at
			 * least once before giving up.
			 *
			 * Also, if the root will be NFS-mounted, we
			 * have nowhere to go if DHCP fails.  So we
			 * just have to keep trying forever.
			 *
			 * 				-- Chip
			 */
#ifdef CONFIG_ROOT_NFS
			if (ROOT_DEV ==  Root_NFS) {
				pr_err("IP-Config: Retrying forever (NFS root)...\n");
				goto try_try_again;
			}
#endif
#ifdef CONFIG_CIFS_ROOT
			if (ROOT_DEV == Root_CIFS) {
				pr_err("IP-Config: Retrying forever (CIFS root)...\n");
				goto try_try_again;
			}
#endif

			if (--retries) {
				pr_err("IP-Config: Reopening network devices...\n");
				goto try_try_again;
			}

			/* Oh, well.  At least we tried. */
			pr_err("IP-Config: Auto-configuration of network failed\n");
			return -1;
		}
#else /* !DYNAMIC */
		pr_err("IP-Config: Incomplete network configuration information\n");
		ic_close_devs();
		return -1;
#endif /* IPCONFIG_DYNAMIC */
	} else {
		/* Device selected manually or only one device -> use it */
		ic_dev = ic_first_dev;
	}

	addr = root_nfs_parse_addr(root_server_path);
	if (root_server_addr == NONE)
		root_server_addr = addr;

	/*
	 * Use defaults wherever applicable.
	 */
	if (ic_defaults() < 0)
		return -1;

	/*
	 * Record which protocol was actually used.
	 */
#ifdef IPCONFIG_DYNAMIC
	ic_proto_used = ic_got_reply | (ic_proto_enabled & IC_USE_DHCP);
#endif

#ifndef IPCONFIG_SILENT
	/*
	 * Clue in the operator.
	 */
	pr_info("IP-Config: Complete:\n");

	pr_info("     device=%s, hwaddr=%*phC, ipaddr=%pI4, mask=%pI4, gw=%pI4\n",
		ic_dev->dev->name, ic_dev->dev->addr_len, ic_dev->dev->dev_addr,
		&ic_myaddr, &ic_netmask, &ic_gateway);
	pr_info("     host=%s, domain=%s, nis-domain=%s\n",
		utsname()->nodename, ic_domain, utsname()->domainname);
	pr_info("     bootserver=%pI4, rootserver=%pI4, rootpath=%s",
		&ic_servaddr, &root_server_addr, root_server_path);
	if (ic_dev_mtu)
		pr_cont(", mtu=%d", ic_dev_mtu);
	/* Name servers (if any): */
	for (i = 0, count = 0; i < CONF_NAMESERVERS_MAX; i++) {
		if (ic_nameservers[i] != NONE) {
			if (i == 0)
				pr_info("     nameserver%u=%pI4",
					i, &ic_nameservers[i]);
			else
				pr_cont(", nameserver%u=%pI4",
					i, &ic_nameservers[i]);

			count++;
		}
		if ((i + 1 == CONF_NAMESERVERS_MAX) && count > 0)
			pr_cont("\n");
	}
	/* NTP servers (if any): */
	for (i = 0, count = 0; i < CONF_NTP_SERVERS_MAX; i++) {
		if (ic_ntp_servers[i] != NONE) {
			if (i == 0)
				pr_info("     ntpserver%u=%pI4",
					i, &ic_ntp_servers[i]);
			else
				pr_cont(", ntpserver%u=%pI4",
					i, &ic_ntp_servers[i]);

			count++;
		}
		if ((i + 1 == CONF_NTP_SERVERS_MAX) && count > 0)
			pr_cont("\n");
	}
#endif /* !SILENT */

	/*
	 * Close all network devices except the device we've
	 * autoconfigured and set up routes.
	 */
	if (ic_setup_if() < 0 || ic_setup_routes() < 0)
		err = -1;
	else
		err = 0;

	ic_close_devs();

	return err;
}

late_initcall(ip_auto_config);


/*
 *  Decode any IP configuration options in the "ip=" or "nfsaddrs=" kernel
 *  command line parameter.  See Documentation/admin-guide/nfs/nfsroot.rst.
 */
static int __init ic_proto_name(char *name)
{
	if (!strcmp(name, "on") || !strcmp(name, "any")) {
		return 1;
	}
	if (!strcmp(name, "off") || !strcmp(name, "none")) {
		return 0;
	}
#ifdef CONFIG_IP_PNP_DHCP
	else if (!strncmp(name, "dhcp", 4)) {
		char *client_id;

		ic_proto_enabled &= ~IC_RARP;
		client_id = strstr(name, "dhcp,");
		if (client_id) {
			char *v;

			client_id = client_id + 5;
			v = strchr(client_id, ',');
			if (!v)
				return 1;
			*v = 0;
			if (kstrtou8(client_id, 0, dhcp_client_identifier))
				pr_debug("DHCP: Invalid client identifier type\n");
			strncpy(dhcp_client_identifier + 1, v + 1, 251);
			*v = ',';
		}
		return 1;
	}
#endif
#ifdef CONFIG_IP_PNP_BOOTP
	else if (!strcmp(name, "bootp")) {
		ic_proto_enabled &= ~(IC_RARP | IC_USE_DHCP);
		return 1;
	}
#endif
#ifdef CONFIG_IP_PNP_RARP
	else if (!strcmp(name, "rarp")) {
		ic_proto_enabled &= ~(IC_BOOTP | IC_USE_DHCP);
		return 1;
	}
#endif
#ifdef IPCONFIG_DYNAMIC
	else if (!strcmp(name, "both")) {
		ic_proto_enabled &= ~IC_USE_DHCP; /* backward compat :-( */
		return 1;
	}
#endif
	return 0;
}

static int __init ip_auto_config_setup(char *addrs)
{
	char *cp, *ip, *dp;
	int num = 0;

	ic_set_manually = 1;
	ic_enable = 1;

	/*
	 * If any dhcp, bootp etc options are set, leave autoconfig on
	 * and skip the below static IP processing.
	 */
	if (ic_proto_name(addrs))
		return 1;

	/* If no static IP is given, turn off autoconfig and bail.  */
	if (*addrs == 0 ||
	    strcmp(addrs, "off") == 0 ||
	    strcmp(addrs, "none") == 0) {
		ic_enable = 0;
		return 1;
	}

	/* Initialise all name servers and NTP servers to NONE */
	ic_nameservers_predef();
	ic_ntp_servers_predef();

	/* Parse string for static IP assignment.  */
	ip = addrs;
	while (ip && *ip) {
		if ((cp = strchr(ip, ':')))
			*cp++ = '\0';
		if (strlen(ip) > 0) {
			pr_debug("IP-Config: Parameter #%d: `%s'\n", num, ip);
			switch (num) {
			case 0:
				if ((ic_myaddr = in_aton(ip)) == ANY)
					ic_myaddr = NONE;
				break;
			case 1:
				if ((ic_servaddr = in_aton(ip)) == ANY)
					ic_servaddr = NONE;
				break;
			case 2:
				if ((ic_gateway = in_aton(ip)) == ANY)
					ic_gateway = NONE;
				break;
			case 3:
				if ((ic_netmask = in_aton(ip)) == ANY)
					ic_netmask = NONE;
				break;
			case 4:
				if ((dp = strchr(ip, '.'))) {
					*dp++ = '\0';
					strlcpy(utsname()->domainname, dp,
						sizeof(utsname()->domainname));
				}
				strlcpy(utsname()->nodename, ip,
					sizeof(utsname()->nodename));
				ic_host_name_set = 1;
				break;
			case 5:
				strlcpy(user_dev_name, ip, sizeof(user_dev_name));
				break;
			case 6:
				if (ic_proto_name(ip) == 0 &&
				    ic_myaddr == NONE) {
					ic_enable = 0;
				}
				break;
			case 7:
				if (CONF_NAMESERVERS_MAX >= 1) {
					ic_nameservers[0] = in_aton(ip);
					if (ic_nameservers[0] == ANY)
						ic_nameservers[0] = NONE;
				}
				break;
			case 8:
				if (CONF_NAMESERVERS_MAX >= 2) {
					ic_nameservers[1] = in_aton(ip);
					if (ic_nameservers[1] == ANY)
						ic_nameservers[1] = NONE;
				}
				break;
			case 9:
				if (CONF_NTP_SERVERS_MAX >= 1) {
					ic_ntp_servers[0] = in_aton(ip);
					if (ic_ntp_servers[0] == ANY)
						ic_ntp_servers[0] = NONE;
				}
				break;
			}
		}
		ip = cp;
		num++;
	}

	return 1;
}
__setup("ip=", ip_auto_config_setup);

static int __init nfsaddrs_config_setup(char *addrs)
{
	return ip_auto_config_setup(addrs);
}
__setup("nfsaddrs=", nfsaddrs_config_setup);

static int __init vendor_class_identifier_setup(char *addrs)
{
	if (strlcpy(vendor_class_identifier, addrs,
		    sizeof(vendor_class_identifier))
	    >= sizeof(vendor_class_identifier))
		pr_warn("DHCP: vendorclass too long, truncated to \"%s\"\n",
			vendor_class_identifier);
	return 1;
}
__setup("dhcpclass=", vendor_class_identifier_setup);

static int __init set_carrier_timeout(char *str)
{
	ssize_t ret;

	if (!str)
		return 0;

	ret = kstrtouint(str, 0, &carrier_timeout);
	if (ret)
		return 0;

	return 1;
}
__setup("carrier_timeout=", set_carrier_timeout);
