// SPDX-License-Identifier: GPL-2.0
/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet sysctl support functions
 *
 * Author:      Steve Whitehouse <SteveW@ACM.org>
 *
 *
 * Changes:
 * Steve Whitehouse - C99 changes and default device handling
 * Steve Whitehouse - Memory buffer settings, like the tcp ones
 *
 */
#include <linux/mm.h>
#include <linux/sysctl.h>
#include <linux/fs.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/flow.h>

#include <linux/uaccess.h>

#include <net/dn.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>


int decnet_debug_level;
int decnet_time_wait = 30;
int decnet_dn_count = 1;
int decnet_di_count = 3;
int decnet_dr_count = 3;
int decnet_log_martians = 1;
int decnet_no_fc_max_cwnd = NSP_MIN_WINDOW;

/* Reasonable defaults, I hope, based on tcp's defaults */
long sysctl_decnet_mem[3] = { 768 << 3, 1024 << 3, 1536 << 3 };
int sysctl_decnet_wmem[3] = { 4 * 1024, 16 * 1024, 128 * 1024 };
int sysctl_decnet_rmem[3] = { 4 * 1024, 87380, 87380 * 2 };

#ifdef CONFIG_SYSCTL
extern int decnet_dst_gc_interval;
static int min_decnet_time_wait[] = { 5 };
static int max_decnet_time_wait[] = { 600 };
static int min_state_count[] = { 1 };
static int max_state_count[] = { NSP_MAXRXTSHIFT };
static int min_decnet_dst_gc_interval[] = { 1 };
static int max_decnet_dst_gc_interval[] = { 60 };
static int min_decnet_no_fc_max_cwnd[] = { NSP_MIN_WINDOW };
static int max_decnet_no_fc_max_cwnd[] = { NSP_MAX_WINDOW };
static char node_name[7] = "???";

static struct ctl_table_header *dn_table_header = NULL;

/*
 * ctype.h :-)
 */
#define ISNUM(x) (((x) >= '0') && ((x) <= '9'))
#define ISLOWER(x) (((x) >= 'a') && ((x) <= 'z'))
#define ISUPPER(x) (((x) >= 'A') && ((x) <= 'Z'))
#define ISALPHA(x) (ISLOWER(x) || ISUPPER(x))
#define INVALID_END_CHAR(x) (ISNUM(x) || ISALPHA(x))

static void strip_it(char *str)
{
	for(;;) {
		switch (*str) {
		case ' ':
		case '\n':
		case '\r':
		case ':':
			*str = 0;
			fallthrough;
		case 0:
			return;
		}
		str++;
	}
}

/*
 * Simple routine to parse an ascii DECnet address
 * into a network order address.
 */
static int parse_addr(__le16 *addr, char *str)
{
	__u16 area, node;

	while(*str && !ISNUM(*str)) str++;

	if (*str == 0)
		return -1;

	area = (*str++ - '0');
	if (ISNUM(*str)) {
		area *= 10;
		area += (*str++ - '0');
	}

	if (*str++ != '.')
		return -1;

	if (!ISNUM(*str))
		return -1;

	node = *str++ - '0';
	if (ISNUM(*str)) {
		node *= 10;
		node += (*str++ - '0');
	}
	if (ISNUM(*str)) {
		node *= 10;
		node += (*str++ - '0');
	}
	if (ISNUM(*str)) {
		node *= 10;
		node += (*str++ - '0');
	}

	if ((node > 1023) || (area > 63))
		return -1;

	if (INVALID_END_CHAR(*str))
		return -1;

	*addr = cpu_to_le16((area << 10) | node);

	return 0;
}

static int dn_node_address_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos)
{
	char addr[DN_ASCBUF_LEN];
	size_t len;
	__le16 dnaddr;

	if (!*lenp || (*ppos && !write)) {
		*lenp = 0;
		return 0;
	}

	if (write) {
		len = (*lenp < DN_ASCBUF_LEN) ? *lenp : (DN_ASCBUF_LEN-1);
		memcpy(addr, buffer, len);
		addr[len] = 0;
		strip_it(addr);

		if (parse_addr(&dnaddr, addr))
			return -EINVAL;

		dn_dev_devices_off();

		decnet_address = dnaddr;

		dn_dev_devices_on();

		*ppos += len;

		return 0;
	}

	dn_addr2asc(le16_to_cpu(decnet_address), addr);
	len = strlen(addr);
	addr[len++] = '\n';

	if (len > *lenp)
		len = *lenp;
	memcpy(buffer, addr, len);
	*lenp = len;
	*ppos += len;

	return 0;
}

static int dn_def_dev_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos)
{
	size_t len;
	struct net_device *dev;
	char devname[17];

	if (!*lenp || (*ppos && !write)) {
		*lenp = 0;
		return 0;
	}

	if (write) {
		if (*lenp > 16)
			return -E2BIG;

		memcpy(devname, buffer, *lenp);
		devname[*lenp] = 0;
		strip_it(devname);

		dev = dev_get_by_name(&init_net, devname);
		if (dev == NULL)
			return -ENODEV;

		if (dev->dn_ptr == NULL) {
			dev_put(dev);
			return -ENODEV;
		}

		if (dn_dev_set_default(dev, 1)) {
			dev_put(dev);
			return -ENODEV;
		}
		*ppos += *lenp;

		return 0;
	}

	dev = dn_dev_get_default();
	if (dev == NULL) {
		*lenp = 0;
		return 0;
	}

	strcpy(devname, dev->name);
	dev_put(dev);
	len = strlen(devname);
	devname[len++] = '\n';

	if (len > *lenp) len = *lenp;

	memcpy(buffer, devname, len);
	*lenp = len;
	*ppos += len;

	return 0;
}

static struct ctl_table dn_table[] = {
	{
		.procname = "node_address",
		.maxlen = 7,
		.mode = 0644,
		.proc_handler = dn_node_address_handler,
	},
	{
		.procname = "node_name",
		.data = node_name,
		.maxlen = 7,
		.mode = 0644,
		.proc_handler = proc_dostring,
	},
	{
		.procname = "default_device",
		.maxlen = 16,
		.mode = 0644,
		.proc_handler = dn_def_dev_handler,
	},
	{
		.procname = "time_wait",
		.data = &decnet_time_wait,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec_minmax,
		.extra1 = &min_decnet_time_wait,
		.extra2 = &max_decnet_time_wait
	},
	{
		.procname = "dn_count",
		.data = &decnet_dn_count,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec_minmax,
		.extra1 = &min_state_count,
		.extra2 = &max_state_count
	},
	{
		.procname = "di_count",
		.data = &decnet_di_count,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec_minmax,
		.extra1 = &min_state_count,
		.extra2 = &max_state_count
	},
	{
		.procname = "dr_count",
		.data = &decnet_dr_count,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec_minmax,
		.extra1 = &min_state_count,
		.extra2 = &max_state_count
	},
	{
		.procname = "dst_gc_interval",
		.data = &decnet_dst_gc_interval,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec_minmax,
		.extra1 = &min_decnet_dst_gc_interval,
		.extra2 = &max_decnet_dst_gc_interval
	},
	{
		.procname = "no_fc_max_cwnd",
		.data = &decnet_no_fc_max_cwnd,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec_minmax,
		.extra1 = &min_decnet_no_fc_max_cwnd,
		.extra2 = &max_decnet_no_fc_max_cwnd
	},
       {
		.procname = "decnet_mem",
		.data = &sysctl_decnet_mem,
		.maxlen = sizeof(sysctl_decnet_mem),
		.mode = 0644,
		.proc_handler = proc_doulongvec_minmax
	},
	{
		.procname = "decnet_rmem",
		.data = &sysctl_decnet_rmem,
		.maxlen = sizeof(sysctl_decnet_rmem),
		.mode = 0644,
		.proc_handler = proc_dointvec,
	},
	{
		.procname = "decnet_wmem",
		.data = &sysctl_decnet_wmem,
		.maxlen = sizeof(sysctl_decnet_wmem),
		.mode = 0644,
		.proc_handler = proc_dointvec,
	},
	{
		.procname = "debug",
		.data = &decnet_debug_level,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = proc_dointvec,
	},
	{ }
};

void dn_register_sysctl(void)
{
	dn_table_header = register_net_sysctl(&init_net, "net/decnet", dn_table);
}

void dn_unregister_sysctl(void)
{
	unregister_net_sysctl_table(dn_table_header);
}

#else  /* CONFIG_SYSCTL */
void dn_unregister_sysctl(void)
{
}
void dn_register_sysctl(void)
{
}

#endif
