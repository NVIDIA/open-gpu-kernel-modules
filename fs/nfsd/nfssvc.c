// SPDX-License-Identifier: GPL-2.0
/*
 * Central processing for nfsd.
 *
 * Authors:	Olaf Kirch (okir@monad.swb.de)
 *
 * Copyright (C) 1995, 1996, 1997 Olaf Kirch <okir@monad.swb.de>
 */

#include <linux/sched/signal.h>
#include <linux/freezer.h>
#include <linux/module.h>
#include <linux/fs_struct.h>
#include <linux/swap.h>

#include <linux/sunrpc/stats.h>
#include <linux/sunrpc/svcsock.h>
#include <linux/sunrpc/svc_xprt.h>
#include <linux/lockd/bind.h>
#include <linux/nfsacl.h>
#include <linux/seq_file.h>
#include <linux/inetdevice.h>
#include <net/addrconf.h>
#include <net/ipv6.h>
#include <net/net_namespace.h>
#include "nfsd.h"
#include "cache.h"
#include "vfs.h"
#include "netns.h"
#include "filecache.h"

#include "trace.h"

#define NFSDDBG_FACILITY	NFSDDBG_SVC

extern struct svc_program	nfsd_program;
static int			nfsd(void *vrqstp);
#if defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL)
static int			nfsd_acl_rpcbind_set(struct net *,
						     const struct svc_program *,
						     u32, int,
						     unsigned short,
						     unsigned short);
static __be32			nfsd_acl_init_request(struct svc_rqst *,
						const struct svc_program *,
						struct svc_process_info *);
#endif
static int			nfsd_rpcbind_set(struct net *,
						 const struct svc_program *,
						 u32, int,
						 unsigned short,
						 unsigned short);
static __be32			nfsd_init_request(struct svc_rqst *,
						const struct svc_program *,
						struct svc_process_info *);

/*
 * nfsd_mutex protects nn->nfsd_serv -- both the pointer itself and the members
 * of the svc_serv struct. In particular, ->sv_nrthreads but also to some
 * extent ->sv_temp_socks and ->sv_permsocks. It also protects nfsdstats.th_cnt
 *
 * If (out side the lock) nn->nfsd_serv is non-NULL, then it must point to a
 * properly initialised 'struct svc_serv' with ->sv_nrthreads > 0. That number
 * of nfsd threads must exist and each must listed in ->sp_all_threads in each
 * entry of ->sv_pools[].
 *
 * Transitions of the thread count between zero and non-zero are of particular
 * interest since the svc_serv needs to be created and initialized at that
 * point, or freed.
 *
 * Finally, the nfsd_mutex also protects some of the global variables that are
 * accessed when nfsd starts and that are settable via the write_* routines in
 * nfsctl.c. In particular:
 *
 *	user_recovery_dirname
 *	user_lease_time
 *	nfsd_versions
 */
DEFINE_MUTEX(nfsd_mutex);

/*
 * nfsd_drc_lock protects nfsd_drc_max_pages and nfsd_drc_pages_used.
 * nfsd_drc_max_pages limits the total amount of memory available for
 * version 4.1 DRC caches.
 * nfsd_drc_pages_used tracks the current version 4.1 DRC memory usage.
 */
DEFINE_SPINLOCK(nfsd_drc_lock);
unsigned long	nfsd_drc_max_mem;
unsigned long	nfsd_drc_mem_used;

#if defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL)
static struct svc_stat	nfsd_acl_svcstats;
static const struct svc_version *nfsd_acl_version[] = {
	[2] = &nfsd_acl_version2,
	[3] = &nfsd_acl_version3,
};

#define NFSD_ACL_MINVERS            2
#define NFSD_ACL_NRVERS		ARRAY_SIZE(nfsd_acl_version)

static struct svc_program	nfsd_acl_program = {
	.pg_prog		= NFS_ACL_PROGRAM,
	.pg_nvers		= NFSD_ACL_NRVERS,
	.pg_vers		= nfsd_acl_version,
	.pg_name		= "nfsacl",
	.pg_class		= "nfsd",
	.pg_stats		= &nfsd_acl_svcstats,
	.pg_authenticate	= &svc_set_client,
	.pg_init_request	= nfsd_acl_init_request,
	.pg_rpcbind_set		= nfsd_acl_rpcbind_set,
};

static struct svc_stat	nfsd_acl_svcstats = {
	.program	= &nfsd_acl_program,
};
#endif /* defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL) */

static const struct svc_version *nfsd_version[] = {
	[2] = &nfsd_version2,
#if defined(CONFIG_NFSD_V3)
	[3] = &nfsd_version3,
#endif
#if defined(CONFIG_NFSD_V4)
	[4] = &nfsd_version4,
#endif
};

#define NFSD_MINVERS    	2
#define NFSD_NRVERS		ARRAY_SIZE(nfsd_version)

struct svc_program		nfsd_program = {
#if defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL)
	.pg_next		= &nfsd_acl_program,
#endif
	.pg_prog		= NFS_PROGRAM,		/* program number */
	.pg_nvers		= NFSD_NRVERS,		/* nr of entries in nfsd_version */
	.pg_vers		= nfsd_version,		/* version table */
	.pg_name		= "nfsd",		/* program name */
	.pg_class		= "nfsd",		/* authentication class */
	.pg_stats		= &nfsd_svcstats,	/* version table */
	.pg_authenticate	= &svc_set_client,	/* export authentication */
	.pg_init_request	= nfsd_init_request,
	.pg_rpcbind_set		= nfsd_rpcbind_set,
};

static bool
nfsd_support_version(int vers)
{
	if (vers >= NFSD_MINVERS && vers < NFSD_NRVERS)
		return nfsd_version[vers] != NULL;
	return false;
}

static bool *
nfsd_alloc_versions(void)
{
	bool *vers = kmalloc_array(NFSD_NRVERS, sizeof(bool), GFP_KERNEL);
	unsigned i;

	if (vers) {
		/* All compiled versions are enabled by default */
		for (i = 0; i < NFSD_NRVERS; i++)
			vers[i] = nfsd_support_version(i);
	}
	return vers;
}

static bool *
nfsd_alloc_minorversions(void)
{
	bool *vers = kmalloc_array(NFSD_SUPPORTED_MINOR_VERSION + 1,
			sizeof(bool), GFP_KERNEL);
	unsigned i;

	if (vers) {
		/* All minor versions are enabled by default */
		for (i = 0; i <= NFSD_SUPPORTED_MINOR_VERSION; i++)
			vers[i] = nfsd_support_version(4);
	}
	return vers;
}

void
nfsd_netns_free_versions(struct nfsd_net *nn)
{
	kfree(nn->nfsd_versions);
	kfree(nn->nfsd4_minorversions);
	nn->nfsd_versions = NULL;
	nn->nfsd4_minorversions = NULL;
}

static void
nfsd_netns_init_versions(struct nfsd_net *nn)
{
	if (!nn->nfsd_versions) {
		nn->nfsd_versions = nfsd_alloc_versions();
		nn->nfsd4_minorversions = nfsd_alloc_minorversions();
		if (!nn->nfsd_versions || !nn->nfsd4_minorversions)
			nfsd_netns_free_versions(nn);
	}
}

int nfsd_vers(struct nfsd_net *nn, int vers, enum vers_op change)
{
	if (vers < NFSD_MINVERS || vers >= NFSD_NRVERS)
		return 0;
	switch(change) {
	case NFSD_SET:
		if (nn->nfsd_versions)
			nn->nfsd_versions[vers] = nfsd_support_version(vers);
		break;
	case NFSD_CLEAR:
		nfsd_netns_init_versions(nn);
		if (nn->nfsd_versions)
			nn->nfsd_versions[vers] = false;
		break;
	case NFSD_TEST:
		if (nn->nfsd_versions)
			return nn->nfsd_versions[vers];
		fallthrough;
	case NFSD_AVAIL:
		return nfsd_support_version(vers);
	}
	return 0;
}

static void
nfsd_adjust_nfsd_versions4(struct nfsd_net *nn)
{
	unsigned i;

	for (i = 0; i <= NFSD_SUPPORTED_MINOR_VERSION; i++) {
		if (nn->nfsd4_minorversions[i])
			return;
	}
	nfsd_vers(nn, 4, NFSD_CLEAR);
}

int nfsd_minorversion(struct nfsd_net *nn, u32 minorversion, enum vers_op change)
{
	if (minorversion > NFSD_SUPPORTED_MINOR_VERSION &&
	    change != NFSD_AVAIL)
		return -1;

	switch(change) {
	case NFSD_SET:
		if (nn->nfsd4_minorversions) {
			nfsd_vers(nn, 4, NFSD_SET);
			nn->nfsd4_minorversions[minorversion] =
				nfsd_vers(nn, 4, NFSD_TEST);
		}
		break;
	case NFSD_CLEAR:
		nfsd_netns_init_versions(nn);
		if (nn->nfsd4_minorversions) {
			nn->nfsd4_minorversions[minorversion] = false;
			nfsd_adjust_nfsd_versions4(nn);
		}
		break;
	case NFSD_TEST:
		if (nn->nfsd4_minorversions)
			return nn->nfsd4_minorversions[minorversion];
		return nfsd_vers(nn, 4, NFSD_TEST);
	case NFSD_AVAIL:
		return minorversion <= NFSD_SUPPORTED_MINOR_VERSION &&
			nfsd_vers(nn, 4, NFSD_AVAIL);
	}
	return 0;
}

/*
 * Maximum number of nfsd processes
 */
#define	NFSD_MAXSERVS		8192

int nfsd_nrthreads(struct net *net)
{
	int rv = 0;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	mutex_lock(&nfsd_mutex);
	if (nn->nfsd_serv)
		rv = nn->nfsd_serv->sv_nrthreads;
	mutex_unlock(&nfsd_mutex);
	return rv;
}

static int nfsd_init_socks(struct net *net, const struct cred *cred)
{
	int error;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	if (!list_empty(&nn->nfsd_serv->sv_permsocks))
		return 0;

	error = svc_create_xprt(nn->nfsd_serv, "udp", net, PF_INET, NFS_PORT,
					SVC_SOCK_DEFAULTS, cred);
	if (error < 0)
		return error;

	error = svc_create_xprt(nn->nfsd_serv, "tcp", net, PF_INET, NFS_PORT,
					SVC_SOCK_DEFAULTS, cred);
	if (error < 0)
		return error;

	return 0;
}

static int nfsd_users = 0;

static int nfsd_startup_generic(void)
{
	int ret;

	if (nfsd_users++)
		return 0;

	ret = nfsd_file_cache_init();
	if (ret)
		goto dec_users;

	ret = nfs4_state_start();
	if (ret)
		goto out_file_cache;
	return 0;

out_file_cache:
	nfsd_file_cache_shutdown();
dec_users:
	nfsd_users--;
	return ret;
}

static void nfsd_shutdown_generic(void)
{
	if (--nfsd_users)
		return;

	nfs4_state_shutdown();
	nfsd_file_cache_shutdown();
}

static bool nfsd_needs_lockd(struct nfsd_net *nn)
{
	return nfsd_vers(nn, 2, NFSD_TEST) || nfsd_vers(nn, 3, NFSD_TEST);
}

void nfsd_copy_boot_verifier(__be32 verf[2], struct nfsd_net *nn)
{
	int seq = 0;

	do {
		read_seqbegin_or_lock(&nn->boot_lock, &seq);
		/*
		 * This is opaque to client, so no need to byte-swap. Use
		 * __force to keep sparse happy. y2038 time_t overflow is
		 * irrelevant in this usage
		 */
		verf[0] = (__force __be32)nn->nfssvc_boot.tv_sec;
		verf[1] = (__force __be32)nn->nfssvc_boot.tv_nsec;
	} while (need_seqretry(&nn->boot_lock, seq));
	done_seqretry(&nn->boot_lock, seq);
}

static void nfsd_reset_boot_verifier_locked(struct nfsd_net *nn)
{
	ktime_get_real_ts64(&nn->nfssvc_boot);
}

void nfsd_reset_boot_verifier(struct nfsd_net *nn)
{
	write_seqlock(&nn->boot_lock);
	nfsd_reset_boot_verifier_locked(nn);
	write_sequnlock(&nn->boot_lock);
}

static int nfsd_startup_net(struct net *net, const struct cred *cred)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	int ret;

	if (nn->nfsd_net_up)
		return 0;

	ret = nfsd_startup_generic();
	if (ret)
		return ret;
	ret = nfsd_init_socks(net, cred);
	if (ret)
		goto out_socks;

	if (nfsd_needs_lockd(nn) && !nn->lockd_up) {
		ret = lockd_up(net, cred);
		if (ret)
			goto out_socks;
		nn->lockd_up = true;
	}

	ret = nfsd_file_cache_start_net(net);
	if (ret)
		goto out_lockd;
	ret = nfs4_state_start_net(net);
	if (ret)
		goto out_filecache;

	nn->nfsd_net_up = true;
	return 0;

out_filecache:
	nfsd_file_cache_shutdown_net(net);
out_lockd:
	if (nn->lockd_up) {
		lockd_down(net);
		nn->lockd_up = false;
	}
out_socks:
	nfsd_shutdown_generic();
	return ret;
}

static void nfsd_shutdown_net(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	nfsd_file_cache_shutdown_net(net);
	nfs4_state_shutdown_net(net);
	if (nn->lockd_up) {
		lockd_down(net);
		nn->lockd_up = false;
	}
	nn->nfsd_net_up = false;
	nfsd_shutdown_generic();
}

static int nfsd_inetaddr_event(struct notifier_block *this, unsigned long event,
	void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct net_device *dev = ifa->ifa_dev->dev;
	struct net *net = dev_net(dev);
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	struct sockaddr_in sin;

	if ((event != NETDEV_DOWN) ||
	    !atomic_inc_not_zero(&nn->ntf_refcnt))
		goto out;

	if (nn->nfsd_serv) {
		dprintk("nfsd_inetaddr_event: removed %pI4\n", &ifa->ifa_local);
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = ifa->ifa_local;
		svc_age_temp_xprts_now(nn->nfsd_serv, (struct sockaddr *)&sin);
	}
	atomic_dec(&nn->ntf_refcnt);
	wake_up(&nn->ntf_wq);

out:
	return NOTIFY_DONE;
}

static struct notifier_block nfsd_inetaddr_notifier = {
	.notifier_call = nfsd_inetaddr_event,
};

#if IS_ENABLED(CONFIG_IPV6)
static int nfsd_inet6addr_event(struct notifier_block *this,
	unsigned long event, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct net_device *dev = ifa->idev->dev;
	struct net *net = dev_net(dev);
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	struct sockaddr_in6 sin6;

	if ((event != NETDEV_DOWN) ||
	    !atomic_inc_not_zero(&nn->ntf_refcnt))
		goto out;

	if (nn->nfsd_serv) {
		dprintk("nfsd_inet6addr_event: removed %pI6\n", &ifa->addr);
		sin6.sin6_family = AF_INET6;
		sin6.sin6_addr = ifa->addr;
		if (ipv6_addr_type(&sin6.sin6_addr) & IPV6_ADDR_LINKLOCAL)
			sin6.sin6_scope_id = ifa->idev->dev->ifindex;
		svc_age_temp_xprts_now(nn->nfsd_serv, (struct sockaddr *)&sin6);
	}
	atomic_dec(&nn->ntf_refcnt);
	wake_up(&nn->ntf_wq);
out:
	return NOTIFY_DONE;
}

static struct notifier_block nfsd_inet6addr_notifier = {
	.notifier_call = nfsd_inet6addr_event,
};
#endif

/* Only used under nfsd_mutex, so this atomic may be overkill: */
static atomic_t nfsd_notifier_refcount = ATOMIC_INIT(0);

static void nfsd_last_thread(struct svc_serv *serv, struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	atomic_dec(&nn->ntf_refcnt);
	/* check if the notifier still has clients */
	if (atomic_dec_return(&nfsd_notifier_refcount) == 0) {
		unregister_inetaddr_notifier(&nfsd_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
		unregister_inet6addr_notifier(&nfsd_inet6addr_notifier);
#endif
	}
	wait_event(nn->ntf_wq, atomic_read(&nn->ntf_refcnt) == 0);

	/*
	 * write_ports can create the server without actually starting
	 * any threads--if we get shut down before any threads are
	 * started, then nfsd_last_thread will be run before any of this
	 * other initialization has been done except the rpcb information.
	 */
	svc_rpcb_cleanup(serv, net);
	if (!nn->nfsd_net_up)
		return;

	nfsd_shutdown_net(net);
	pr_info("nfsd: last server has exited, flushing export cache\n");
	nfsd_export_flush(net);
}

void nfsd_reset_versions(struct nfsd_net *nn)
{
	int i;

	for (i = 0; i < NFSD_NRVERS; i++)
		if (nfsd_vers(nn, i, NFSD_TEST))
			return;

	for (i = 0; i < NFSD_NRVERS; i++)
		if (i != 4)
			nfsd_vers(nn, i, NFSD_SET);
		else {
			int minor = 0;
			while (nfsd_minorversion(nn, minor, NFSD_SET) >= 0)
				minor++;
		}
}

/*
 * Each session guarantees a negotiated per slot memory cache for replies
 * which in turn consumes memory beyond the v2/v3/v4.0 server. A dedicated
 * NFSv4.1 server might want to use more memory for a DRC than a machine
 * with mutiple services.
 *
 * Impose a hard limit on the number of pages for the DRC which varies
 * according to the machines free pages. This is of course only a default.
 *
 * For now this is a #defined shift which could be under admin control
 * in the future.
 */
static void set_max_drc(void)
{
	#define NFSD_DRC_SIZE_SHIFT	7
	nfsd_drc_max_mem = (nr_free_buffer_pages()
					>> NFSD_DRC_SIZE_SHIFT) * PAGE_SIZE;
	nfsd_drc_mem_used = 0;
	dprintk("%s nfsd_drc_max_mem %lu \n", __func__, nfsd_drc_max_mem);
}

static int nfsd_get_default_max_blksize(void)
{
	struct sysinfo i;
	unsigned long long target;
	unsigned long ret;

	si_meminfo(&i);
	target = (i.totalram - i.totalhigh) << PAGE_SHIFT;
	/*
	 * Aim for 1/4096 of memory per thread This gives 1MB on 4Gig
	 * machines, but only uses 32K on 128M machines.  Bottom out at
	 * 8K on 32M and smaller.  Of course, this is only a default.
	 */
	target >>= 12;

	ret = NFSSVC_MAXBLKSIZE;
	while (ret > target && ret >= 8*1024*2)
		ret /= 2;
	return ret;
}

static const struct svc_serv_ops nfsd_thread_sv_ops = {
	.svo_shutdown		= nfsd_last_thread,
	.svo_function		= nfsd,
	.svo_enqueue_xprt	= svc_xprt_do_enqueue,
	.svo_setup		= svc_set_num_threads,
	.svo_module		= THIS_MODULE,
};

static void nfsd_complete_shutdown(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	WARN_ON(!mutex_is_locked(&nfsd_mutex));

	nn->nfsd_serv = NULL;
	complete(&nn->nfsd_shutdown_complete);
}

void nfsd_shutdown_threads(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	struct svc_serv *serv;

	mutex_lock(&nfsd_mutex);
	serv = nn->nfsd_serv;
	if (serv == NULL) {
		mutex_unlock(&nfsd_mutex);
		return;
	}

	svc_get(serv);
	/* Kill outstanding nfsd threads */
	serv->sv_ops->svo_setup(serv, NULL, 0);
	nfsd_destroy(net);
	mutex_unlock(&nfsd_mutex);
	/* Wait for shutdown of nfsd_serv to complete */
	wait_for_completion(&nn->nfsd_shutdown_complete);
}

bool i_am_nfsd(void)
{
	return kthread_func(current) == nfsd;
}

int nfsd_create_serv(struct net *net)
{
	int error;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	WARN_ON(!mutex_is_locked(&nfsd_mutex));
	if (nn->nfsd_serv) {
		svc_get(nn->nfsd_serv);
		return 0;
	}
	if (nfsd_max_blksize == 0)
		nfsd_max_blksize = nfsd_get_default_max_blksize();
	nfsd_reset_versions(nn);
	nn->nfsd_serv = svc_create_pooled(&nfsd_program, nfsd_max_blksize,
						&nfsd_thread_sv_ops);
	if (nn->nfsd_serv == NULL)
		return -ENOMEM;
	init_completion(&nn->nfsd_shutdown_complete);

	nn->nfsd_serv->sv_maxconn = nn->max_connections;
	error = svc_bind(nn->nfsd_serv, net);
	if (error < 0) {
		svc_destroy(nn->nfsd_serv);
		nfsd_complete_shutdown(net);
		return error;
	}

	set_max_drc();
	/* check if the notifier is already set */
	if (atomic_inc_return(&nfsd_notifier_refcount) == 1) {
		register_inetaddr_notifier(&nfsd_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
		register_inet6addr_notifier(&nfsd_inet6addr_notifier);
#endif
	}
	atomic_inc(&nn->ntf_refcnt);
	nfsd_reset_boot_verifier(nn);
	return 0;
}

int nfsd_nrpools(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	if (nn->nfsd_serv == NULL)
		return 0;
	else
		return nn->nfsd_serv->sv_nrpools;
}

int nfsd_get_nrthreads(int n, int *nthreads, struct net *net)
{
	int i = 0;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	if (nn->nfsd_serv != NULL) {
		for (i = 0; i < nn->nfsd_serv->sv_nrpools && i < n; i++)
			nthreads[i] = nn->nfsd_serv->sv_pools[i].sp_nrthreads;
	}

	return 0;
}

void nfsd_destroy(struct net *net)
{
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	int destroy = (nn->nfsd_serv->sv_nrthreads == 1);

	if (destroy)
		svc_shutdown_net(nn->nfsd_serv, net);
	svc_destroy(nn->nfsd_serv);
	if (destroy)
		nfsd_complete_shutdown(net);
}

int nfsd_set_nrthreads(int n, int *nthreads, struct net *net)
{
	int i = 0;
	int tot = 0;
	int err = 0;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	WARN_ON(!mutex_is_locked(&nfsd_mutex));

	if (nn->nfsd_serv == NULL || n <= 0)
		return 0;

	if (n > nn->nfsd_serv->sv_nrpools)
		n = nn->nfsd_serv->sv_nrpools;

	/* enforce a global maximum number of threads */
	tot = 0;
	for (i = 0; i < n; i++) {
		nthreads[i] = min(nthreads[i], NFSD_MAXSERVS);
		tot += nthreads[i];
	}
	if (tot > NFSD_MAXSERVS) {
		/* total too large: scale down requested numbers */
		for (i = 0; i < n && tot > 0; i++) {
		    	int new = nthreads[i] * NFSD_MAXSERVS / tot;
			tot -= (nthreads[i] - new);
			nthreads[i] = new;
		}
		for (i = 0; i < n && tot > 0; i++) {
			nthreads[i]--;
			tot--;
		}
	}

	/*
	 * There must always be a thread in pool 0; the admin
	 * can't shut down NFS completely using pool_threads.
	 */
	if (nthreads[0] == 0)
		nthreads[0] = 1;

	/* apply the new numbers */
	svc_get(nn->nfsd_serv);
	for (i = 0; i < n; i++) {
		err = nn->nfsd_serv->sv_ops->svo_setup(nn->nfsd_serv,
				&nn->nfsd_serv->sv_pools[i], nthreads[i]);
		if (err)
			break;
	}
	nfsd_destroy(net);
	return err;
}

/*
 * Adjust the number of threads and return the new number of threads.
 * This is also the function that starts the server if necessary, if
 * this is the first time nrservs is nonzero.
 */
int
nfsd_svc(int nrservs, struct net *net, const struct cred *cred)
{
	int	error;
	bool	nfsd_up_before;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	mutex_lock(&nfsd_mutex);
	dprintk("nfsd: creating service\n");

	nrservs = max(nrservs, 0);
	nrservs = min(nrservs, NFSD_MAXSERVS);
	error = 0;

	if (nrservs == 0 && nn->nfsd_serv == NULL)
		goto out;

	strlcpy(nn->nfsd_name, utsname()->nodename,
		sizeof(nn->nfsd_name));

	error = nfsd_create_serv(net);
	if (error)
		goto out;

	nfsd_up_before = nn->nfsd_net_up;

	error = nfsd_startup_net(net, cred);
	if (error)
		goto out_destroy;
	error = nn->nfsd_serv->sv_ops->svo_setup(nn->nfsd_serv,
			NULL, nrservs);
	if (error)
		goto out_shutdown;
	/* We are holding a reference to nn->nfsd_serv which
	 * we don't want to count in the return value,
	 * so subtract 1
	 */
	error = nn->nfsd_serv->sv_nrthreads - 1;
out_shutdown:
	if (error < 0 && !nfsd_up_before)
		nfsd_shutdown_net(net);
out_destroy:
	nfsd_destroy(net);		/* Release server */
out:
	mutex_unlock(&nfsd_mutex);
	return error;
}

#if defined(CONFIG_NFSD_V2_ACL) || defined(CONFIG_NFSD_V3_ACL)
static bool
nfsd_support_acl_version(int vers)
{
	if (vers >= NFSD_ACL_MINVERS && vers < NFSD_ACL_NRVERS)
		return nfsd_acl_version[vers] != NULL;
	return false;
}

static int
nfsd_acl_rpcbind_set(struct net *net, const struct svc_program *progp,
		     u32 version, int family, unsigned short proto,
		     unsigned short port)
{
	if (!nfsd_support_acl_version(version) ||
	    !nfsd_vers(net_generic(net, nfsd_net_id), version, NFSD_TEST))
		return 0;
	return svc_generic_rpcbind_set(net, progp, version, family,
			proto, port);
}

static __be32
nfsd_acl_init_request(struct svc_rqst *rqstp,
		      const struct svc_program *progp,
		      struct svc_process_info *ret)
{
	struct nfsd_net *nn = net_generic(SVC_NET(rqstp), nfsd_net_id);
	int i;

	if (likely(nfsd_support_acl_version(rqstp->rq_vers) &&
	    nfsd_vers(nn, rqstp->rq_vers, NFSD_TEST)))
		return svc_generic_init_request(rqstp, progp, ret);

	ret->mismatch.lovers = NFSD_ACL_NRVERS;
	for (i = NFSD_ACL_MINVERS; i < NFSD_ACL_NRVERS; i++) {
		if (nfsd_support_acl_version(rqstp->rq_vers) &&
		    nfsd_vers(nn, i, NFSD_TEST)) {
			ret->mismatch.lovers = i;
			break;
		}
	}
	if (ret->mismatch.lovers == NFSD_ACL_NRVERS)
		return rpc_prog_unavail;
	ret->mismatch.hivers = NFSD_ACL_MINVERS;
	for (i = NFSD_ACL_NRVERS - 1; i >= NFSD_ACL_MINVERS; i--) {
		if (nfsd_support_acl_version(rqstp->rq_vers) &&
		    nfsd_vers(nn, i, NFSD_TEST)) {
			ret->mismatch.hivers = i;
			break;
		}
	}
	return rpc_prog_mismatch;
}
#endif

static int
nfsd_rpcbind_set(struct net *net, const struct svc_program *progp,
		 u32 version, int family, unsigned short proto,
		 unsigned short port)
{
	if (!nfsd_vers(net_generic(net, nfsd_net_id), version, NFSD_TEST))
		return 0;
	return svc_generic_rpcbind_set(net, progp, version, family,
			proto, port);
}

static __be32
nfsd_init_request(struct svc_rqst *rqstp,
		  const struct svc_program *progp,
		  struct svc_process_info *ret)
{
	struct nfsd_net *nn = net_generic(SVC_NET(rqstp), nfsd_net_id);
	int i;

	if (likely(nfsd_vers(nn, rqstp->rq_vers, NFSD_TEST)))
		return svc_generic_init_request(rqstp, progp, ret);

	ret->mismatch.lovers = NFSD_NRVERS;
	for (i = NFSD_MINVERS; i < NFSD_NRVERS; i++) {
		if (nfsd_vers(nn, i, NFSD_TEST)) {
			ret->mismatch.lovers = i;
			break;
		}
	}
	if (ret->mismatch.lovers == NFSD_NRVERS)
		return rpc_prog_unavail;
	ret->mismatch.hivers = NFSD_MINVERS;
	for (i = NFSD_NRVERS - 1; i >= NFSD_MINVERS; i--) {
		if (nfsd_vers(nn, i, NFSD_TEST)) {
			ret->mismatch.hivers = i;
			break;
		}
	}
	return rpc_prog_mismatch;
}

/*
 * This is the NFS server kernel thread
 */
static int
nfsd(void *vrqstp)
{
	struct svc_rqst *rqstp = (struct svc_rqst *) vrqstp;
	struct svc_xprt *perm_sock = list_entry(rqstp->rq_server->sv_permsocks.next, typeof(struct svc_xprt), xpt_list);
	struct net *net = perm_sock->xpt_net;
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);
	int err;

	/* Lock module and set up kernel thread */
	mutex_lock(&nfsd_mutex);

	/* At this point, the thread shares current->fs
	 * with the init process. We need to create files with the
	 * umask as defined by the client instead of init's umask. */
	if (unshare_fs_struct() < 0) {
		printk("Unable to start nfsd thread: out of memory\n");
		goto out;
	}

	current->fs->umask = 0;

	/*
	 * thread is spawned with all signals set to SIG_IGN, re-enable
	 * the ones that will bring down the thread
	 */
	allow_signal(SIGKILL);
	allow_signal(SIGHUP);
	allow_signal(SIGINT);
	allow_signal(SIGQUIT);

	nfsdstats.th_cnt++;
	mutex_unlock(&nfsd_mutex);

	set_freezable();

	/*
	 * The main request loop
	 */
	for (;;) {
		/* Update sv_maxconn if it has changed */
		rqstp->rq_server->sv_maxconn = nn->max_connections;

		/*
		 * Find a socket with data available and call its
		 * recvfrom routine.
		 */
		while ((err = svc_recv(rqstp, 60*60*HZ)) == -EAGAIN)
			;
		if (err == -EINTR)
			break;
		validate_process_creds();
		svc_process(rqstp);
		validate_process_creds();
	}

	/* Clear signals before calling svc_exit_thread() */
	flush_signals(current);

	mutex_lock(&nfsd_mutex);
	nfsdstats.th_cnt --;

out:
	rqstp->rq_server = NULL;

	/* Release the thread */
	svc_exit_thread(rqstp);

	nfsd_destroy(net);

	/* Release module */
	mutex_unlock(&nfsd_mutex);
	module_put_and_exit(0);
	return 0;
}

/**
 * nfsd_dispatch - Process an NFS or NFSACL Request
 * @rqstp: incoming request
 * @statp: pointer to location of accept_stat field in RPC Reply buffer
 *
 * This RPC dispatcher integrates the NFS server's duplicate reply cache.
 *
 * Return values:
 *  %0: Processing complete; do not send a Reply
 *  %1: Processing complete; send Reply in rqstp->rq_res
 */
int nfsd_dispatch(struct svc_rqst *rqstp, __be32 *statp)
{
	const struct svc_procedure *proc = rqstp->rq_procinfo;
	struct kvec *argv = &rqstp->rq_arg.head[0];
	struct kvec *resv = &rqstp->rq_res.head[0];
	__be32 *p;

	/*
	 * Give the xdr decoder a chance to change this if it wants
	 * (necessary in the NFSv4.0 compound case)
	 */
	rqstp->rq_cachetype = proc->pc_cachetype;

	svcxdr_init_decode(rqstp);
	if (!proc->pc_decode(rqstp, argv->iov_base))
		goto out_decode_err;

	switch (nfsd_cache_lookup(rqstp)) {
	case RC_DOIT:
		break;
	case RC_REPLY:
		goto out_cached_reply;
	case RC_DROPIT:
		goto out_dropit;
	}

	/*
	 * Need to grab the location to store the status, as
	 * NFSv4 does some encoding while processing
	 */
	p = resv->iov_base + resv->iov_len;
	svcxdr_init_encode(rqstp);

	*statp = proc->pc_func(rqstp);
	if (*statp == rpc_drop_reply || test_bit(RQ_DROPME, &rqstp->rq_flags))
		goto out_update_drop;

	if (!proc->pc_encode(rqstp, p))
		goto out_encode_err;

	nfsd_cache_update(rqstp, rqstp->rq_cachetype, statp + 1);
out_cached_reply:
	return 1;

out_decode_err:
	trace_nfsd_garbage_args_err(rqstp);
	*statp = rpc_garbage_args;
	return 1;

out_update_drop:
	nfsd_cache_update(rqstp, RC_NOCACHE, NULL);
out_dropit:
	return 0;

out_encode_err:
	trace_nfsd_cant_encode_err(rqstp);
	nfsd_cache_update(rqstp, RC_NOCACHE, NULL);
	*statp = rpc_system_err;
	return 1;
}

/**
 * nfssvc_decode_voidarg - Decode void arguments
 * @rqstp: Server RPC transaction context
 * @p: buffer containing arguments to decode
 *
 * Return values:
 *   %0: Arguments were not valid
 *   %1: Decoding was successful
 */
int nfssvc_decode_voidarg(struct svc_rqst *rqstp, __be32 *p)
{
	return 1;
}

/**
 * nfssvc_encode_voidres - Encode void results
 * @rqstp: Server RPC transaction context
 * @p: buffer in which to encode results
 *
 * Return values:
 *   %0: Local error while encoding
 *   %1: Encoding was successful
 */
int nfssvc_encode_voidres(struct svc_rqst *rqstp, __be32 *p)
{
	return 1;
}

int nfsd_pool_stats_open(struct inode *inode, struct file *file)
{
	int ret;
	struct nfsd_net *nn = net_generic(inode->i_sb->s_fs_info, nfsd_net_id);

	mutex_lock(&nfsd_mutex);
	if (nn->nfsd_serv == NULL) {
		mutex_unlock(&nfsd_mutex);
		return -ENODEV;
	}
	/* bump up the psudo refcount while traversing */
	svc_get(nn->nfsd_serv);
	ret = svc_pool_stats_open(nn->nfsd_serv, file);
	mutex_unlock(&nfsd_mutex);
	return ret;
}

int nfsd_pool_stats_release(struct inode *inode, struct file *file)
{
	int ret = seq_release(inode, file);
	struct net *net = inode->i_sb->s_fs_info;

	mutex_lock(&nfsd_mutex);
	/* this function really, really should have been called svc_put() */
	nfsd_destroy(net);
	mutex_unlock(&nfsd_mutex);
	return ret;
}
