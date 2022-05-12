// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/fs/lockd/svc.c
 *
 * This is the central lockd service.
 *
 * FIXME: Separate the lockd NFS server functionality from the lockd NFS
 * 	  client functionality. Oh why didn't Sun create two separate
 *	  services in the first place?
 *
 * Authors:	Olaf Kirch (okir@monad.swb.de)
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysctl.h>
#include <linux/moduleparam.h>

#include <linux/sched/signal.h>
#include <linux/errno.h>
#include <linux/in.h>
#include <linux/uio.h>
#include <linux/smp.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/inetdevice.h>

#include <linux/sunrpc/types.h>
#include <linux/sunrpc/stats.h>
#include <linux/sunrpc/clnt.h>
#include <linux/sunrpc/svc.h>
#include <linux/sunrpc/svcsock.h>
#include <linux/sunrpc/svc_xprt.h>
#include <net/ip.h>
#include <net/addrconf.h>
#include <net/ipv6.h>
#include <linux/lockd/lockd.h>
#include <linux/nfs.h>

#include "netns.h"
#include "procfs.h"

#define NLMDBG_FACILITY		NLMDBG_SVC
#define LOCKD_BUFSIZE		(1024 + NLMSVC_XDRSIZE)
#define ALLOWED_SIGS		(sigmask(SIGKILL))

static struct svc_program	nlmsvc_program;

const struct nlmsvc_binding	*nlmsvc_ops;
EXPORT_SYMBOL_GPL(nlmsvc_ops);

static DEFINE_MUTEX(nlmsvc_mutex);
static unsigned int		nlmsvc_users;
static struct task_struct	*nlmsvc_task;
static struct svc_rqst		*nlmsvc_rqst;
unsigned long			nlmsvc_timeout;

static atomic_t nlm_ntf_refcnt = ATOMIC_INIT(0);
static DECLARE_WAIT_QUEUE_HEAD(nlm_ntf_wq);

unsigned int lockd_net_id;

/*
 * These can be set at insmod time (useful for NFS as root filesystem),
 * and also changed through the sysctl interface.  -- Jamie Lokier, Aug 2003
 */
static unsigned long		nlm_grace_period;
static unsigned long		nlm_timeout = LOCKD_DFLT_TIMEO;
static int			nlm_udpport, nlm_tcpport;

/* RLIM_NOFILE defaults to 1024. That seems like a reasonable default here. */
static unsigned int		nlm_max_connections = 1024;

/*
 * Constants needed for the sysctl interface.
 */
static const unsigned long	nlm_grace_period_min = 0;
static const unsigned long	nlm_grace_period_max = 240;
static const unsigned long	nlm_timeout_min = 3;
static const unsigned long	nlm_timeout_max = 20;
static const int		nlm_port_min = 0, nlm_port_max = 65535;

#ifdef CONFIG_SYSCTL
static struct ctl_table_header * nlm_sysctl_table;
#endif

static unsigned long get_lockd_grace_period(void)
{
	/* Note: nlm_timeout should always be nonzero */
	if (nlm_grace_period)
		return roundup(nlm_grace_period, nlm_timeout) * HZ;
	else
		return nlm_timeout * 5 * HZ;
}

static void grace_ender(struct work_struct *grace)
{
	struct delayed_work *dwork = to_delayed_work(grace);
	struct lockd_net *ln = container_of(dwork, struct lockd_net,
					    grace_period_end);

	locks_end_grace(&ln->lockd_manager);
}

static void set_grace_period(struct net *net)
{
	unsigned long grace_period = get_lockd_grace_period();
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	locks_start_grace(net, &ln->lockd_manager);
	cancel_delayed_work_sync(&ln->grace_period_end);
	schedule_delayed_work(&ln->grace_period_end, grace_period);
}

static void restart_grace(void)
{
	if (nlmsvc_ops) {
		struct net *net = &init_net;
		struct lockd_net *ln = net_generic(net, lockd_net_id);

		cancel_delayed_work_sync(&ln->grace_period_end);
		locks_end_grace(&ln->lockd_manager);
		nlmsvc_invalidate_all();
		set_grace_period(net);
	}
}

/*
 * This is the lockd kernel thread
 */
static int
lockd(void *vrqstp)
{
	int		err = 0;
	struct svc_rqst *rqstp = vrqstp;
	struct net *net = &init_net;
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	/* try_to_freeze() is called from svc_recv() */
	set_freezable();

	/* Allow SIGKILL to tell lockd to drop all of its locks */
	allow_signal(SIGKILL);

	dprintk("NFS locking service started (ver " LOCKD_VERSION ").\n");

	/*
	 * The main request loop. We don't terminate until the last
	 * NFS mount or NFS daemon has gone away.
	 */
	while (!kthread_should_stop()) {
		long timeout = MAX_SCHEDULE_TIMEOUT;
		RPC_IFDEBUG(char buf[RPC_MAX_ADDRBUFLEN]);

		/* update sv_maxconn if it has changed */
		rqstp->rq_server->sv_maxconn = nlm_max_connections;

		if (signalled()) {
			flush_signals(current);
			restart_grace();
			continue;
		}

		timeout = nlmsvc_retry_blocked();

		/*
		 * Find a socket with data available and call its
		 * recvfrom routine.
		 */
		err = svc_recv(rqstp, timeout);
		if (err == -EAGAIN || err == -EINTR)
			continue;
		dprintk("lockd: request from %s\n",
				svc_print_addr(rqstp, buf, sizeof(buf)));

		svc_process(rqstp);
	}
	flush_signals(current);
	if (nlmsvc_ops)
		nlmsvc_invalidate_all();
	nlm_shutdown_hosts();
	cancel_delayed_work_sync(&ln->grace_period_end);
	locks_end_grace(&ln->lockd_manager);
	return 0;
}

static int create_lockd_listener(struct svc_serv *serv, const char *name,
				 struct net *net, const int family,
				 const unsigned short port,
				 const struct cred *cred)
{
	struct svc_xprt *xprt;

	xprt = svc_find_xprt(serv, name, net, family, 0);
	if (xprt == NULL)
		return svc_create_xprt(serv, name, net, family, port,
						SVC_SOCK_DEFAULTS, cred);
	svc_xprt_put(xprt);
	return 0;
}

static int create_lockd_family(struct svc_serv *serv, struct net *net,
			       const int family, const struct cred *cred)
{
	int err;

	err = create_lockd_listener(serv, "udp", net, family, nlm_udpport,
			cred);
	if (err < 0)
		return err;

	return create_lockd_listener(serv, "tcp", net, family, nlm_tcpport,
			cred);
}

/*
 * Ensure there are active UDP and TCP listeners for lockd.
 *
 * Even if we have only TCP NFS mounts and/or TCP NFSDs, some
 * local services (such as rpc.statd) still require UDP, and
 * some NFS servers do not yet support NLM over TCP.
 *
 * Returns zero if all listeners are available; otherwise a
 * negative errno value is returned.
 */
static int make_socks(struct svc_serv *serv, struct net *net,
		const struct cred *cred)
{
	static int warned;
	int err;

	err = create_lockd_family(serv, net, PF_INET, cred);
	if (err < 0)
		goto out_err;

	err = create_lockd_family(serv, net, PF_INET6, cred);
	if (err < 0 && err != -EAFNOSUPPORT)
		goto out_err;

	warned = 0;
	return 0;

out_err:
	if (warned++ == 0)
		printk(KERN_WARNING
			"lockd_up: makesock failed, error=%d\n", err);
	svc_shutdown_net(serv, net);
	return err;
}

static int lockd_up_net(struct svc_serv *serv, struct net *net,
		const struct cred *cred)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);
	int error;

	if (ln->nlmsvc_users++)
		return 0;

	error = svc_bind(serv, net);
	if (error)
		goto err_bind;

	error = make_socks(serv, net, cred);
	if (error < 0)
		goto err_bind;
	set_grace_period(net);
	dprintk("%s: per-net data created; net=%x\n", __func__, net->ns.inum);
	return 0;

err_bind:
	ln->nlmsvc_users--;
	return error;
}

static void lockd_down_net(struct svc_serv *serv, struct net *net)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	if (ln->nlmsvc_users) {
		if (--ln->nlmsvc_users == 0) {
			nlm_shutdown_hosts_net(net);
			cancel_delayed_work_sync(&ln->grace_period_end);
			locks_end_grace(&ln->lockd_manager);
			svc_shutdown_net(serv, net);
			dprintk("%s: per-net data destroyed; net=%x\n",
				__func__, net->ns.inum);
		}
	} else {
		pr_err("%s: no users! task=%p, net=%x\n",
			__func__, nlmsvc_task, net->ns.inum);
		BUG();
	}
}

static int lockd_inetaddr_event(struct notifier_block *this,
	unsigned long event, void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct sockaddr_in sin;

	if ((event != NETDEV_DOWN) ||
	    !atomic_inc_not_zero(&nlm_ntf_refcnt))
		goto out;

	if (nlmsvc_rqst) {
		dprintk("lockd_inetaddr_event: removed %pI4\n",
			&ifa->ifa_local);
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = ifa->ifa_local;
		svc_age_temp_xprts_now(nlmsvc_rqst->rq_server,
			(struct sockaddr *)&sin);
	}
	atomic_dec(&nlm_ntf_refcnt);
	wake_up(&nlm_ntf_wq);

out:
	return NOTIFY_DONE;
}

static struct notifier_block lockd_inetaddr_notifier = {
	.notifier_call = lockd_inetaddr_event,
};

#if IS_ENABLED(CONFIG_IPV6)
static int lockd_inet6addr_event(struct notifier_block *this,
	unsigned long event, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct sockaddr_in6 sin6;

	if ((event != NETDEV_DOWN) ||
	    !atomic_inc_not_zero(&nlm_ntf_refcnt))
		goto out;

	if (nlmsvc_rqst) {
		dprintk("lockd_inet6addr_event: removed %pI6\n", &ifa->addr);
		sin6.sin6_family = AF_INET6;
		sin6.sin6_addr = ifa->addr;
		if (ipv6_addr_type(&sin6.sin6_addr) & IPV6_ADDR_LINKLOCAL)
			sin6.sin6_scope_id = ifa->idev->dev->ifindex;
		svc_age_temp_xprts_now(nlmsvc_rqst->rq_server,
			(struct sockaddr *)&sin6);
	}
	atomic_dec(&nlm_ntf_refcnt);
	wake_up(&nlm_ntf_wq);

out:
	return NOTIFY_DONE;
}

static struct notifier_block lockd_inet6addr_notifier = {
	.notifier_call = lockd_inet6addr_event,
};
#endif

static void lockd_unregister_notifiers(void)
{
	unregister_inetaddr_notifier(&lockd_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
	unregister_inet6addr_notifier(&lockd_inet6addr_notifier);
#endif
	wait_event(nlm_ntf_wq, atomic_read(&nlm_ntf_refcnt) == 0);
}

static void lockd_svc_exit_thread(void)
{
	atomic_dec(&nlm_ntf_refcnt);
	lockd_unregister_notifiers();
	svc_exit_thread(nlmsvc_rqst);
}

static int lockd_start_svc(struct svc_serv *serv)
{
	int error;

	if (nlmsvc_rqst)
		return 0;

	/*
	 * Create the kernel thread and wait for it to start.
	 */
	nlmsvc_rqst = svc_prepare_thread(serv, &serv->sv_pools[0], NUMA_NO_NODE);
	if (IS_ERR(nlmsvc_rqst)) {
		error = PTR_ERR(nlmsvc_rqst);
		printk(KERN_WARNING
			"lockd_up: svc_rqst allocation failed, error=%d\n",
			error);
		lockd_unregister_notifiers();
		goto out_rqst;
	}

	atomic_inc(&nlm_ntf_refcnt);
	svc_sock_update_bufs(serv);
	serv->sv_maxconn = nlm_max_connections;

	nlmsvc_task = kthread_create(lockd, nlmsvc_rqst, "%s", serv->sv_name);
	if (IS_ERR(nlmsvc_task)) {
		error = PTR_ERR(nlmsvc_task);
		printk(KERN_WARNING
			"lockd_up: kthread_run failed, error=%d\n", error);
		goto out_task;
	}
	nlmsvc_rqst->rq_task = nlmsvc_task;
	wake_up_process(nlmsvc_task);

	dprintk("lockd_up: service started\n");
	return 0;

out_task:
	lockd_svc_exit_thread();
	nlmsvc_task = NULL;
out_rqst:
	nlmsvc_rqst = NULL;
	return error;
}

static const struct svc_serv_ops lockd_sv_ops = {
	.svo_shutdown		= svc_rpcb_cleanup,
	.svo_enqueue_xprt	= svc_xprt_do_enqueue,
};

static struct svc_serv *lockd_create_svc(void)
{
	struct svc_serv *serv;

	/*
	 * Check whether we're already up and running.
	 */
	if (nlmsvc_rqst) {
		/*
		 * Note: increase service usage, because later in case of error
		 * svc_destroy() will be called.
		 */
		svc_get(nlmsvc_rqst->rq_server);
		return nlmsvc_rqst->rq_server;
	}

	/*
	 * Sanity check: if there's no pid,
	 * we should be the first user ...
	 */
	if (nlmsvc_users)
		printk(KERN_WARNING
			"lockd_up: no pid, %d users??\n", nlmsvc_users);

	if (!nlm_timeout)
		nlm_timeout = LOCKD_DFLT_TIMEO;
	nlmsvc_timeout = nlm_timeout * HZ;

	serv = svc_create(&nlmsvc_program, LOCKD_BUFSIZE, &lockd_sv_ops);
	if (!serv) {
		printk(KERN_WARNING "lockd_up: create service failed\n");
		return ERR_PTR(-ENOMEM);
	}
	register_inetaddr_notifier(&lockd_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
	register_inet6addr_notifier(&lockd_inet6addr_notifier);
#endif
	dprintk("lockd_up: service created\n");
	return serv;
}

/*
 * Bring up the lockd process if it's not already up.
 */
int lockd_up(struct net *net, const struct cred *cred)
{
	struct svc_serv *serv;
	int error;

	mutex_lock(&nlmsvc_mutex);

	serv = lockd_create_svc();
	if (IS_ERR(serv)) {
		error = PTR_ERR(serv);
		goto err_create;
	}

	error = lockd_up_net(serv, net, cred);
	if (error < 0) {
		lockd_unregister_notifiers();
		goto err_put;
	}

	error = lockd_start_svc(serv);
	if (error < 0) {
		lockd_down_net(serv, net);
		goto err_put;
	}
	nlmsvc_users++;
	/*
	 * Note: svc_serv structures have an initial use count of 1,
	 * so we exit through here on both success and failure.
	 */
err_put:
	svc_destroy(serv);
err_create:
	mutex_unlock(&nlmsvc_mutex);
	return error;
}
EXPORT_SYMBOL_GPL(lockd_up);

/*
 * Decrement the user count and bring down lockd if we're the last.
 */
void
lockd_down(struct net *net)
{
	mutex_lock(&nlmsvc_mutex);
	lockd_down_net(nlmsvc_rqst->rq_server, net);
	if (nlmsvc_users) {
		if (--nlmsvc_users)
			goto out;
	} else {
		printk(KERN_ERR "lockd_down: no users! task=%p\n",
			nlmsvc_task);
		BUG();
	}

	if (!nlmsvc_task) {
		printk(KERN_ERR "lockd_down: no lockd running.\n");
		BUG();
	}
	kthread_stop(nlmsvc_task);
	dprintk("lockd_down: service stopped\n");
	lockd_svc_exit_thread();
	dprintk("lockd_down: service destroyed\n");
	nlmsvc_task = NULL;
	nlmsvc_rqst = NULL;
out:
	mutex_unlock(&nlmsvc_mutex);
}
EXPORT_SYMBOL_GPL(lockd_down);

#ifdef CONFIG_SYSCTL

/*
 * Sysctl parameters (same as module parameters, different interface).
 */

static struct ctl_table nlm_sysctls[] = {
	{
		.procname	= "nlm_grace_period",
		.data		= &nlm_grace_period,
		.maxlen		= sizeof(unsigned long),
		.mode		= 0644,
		.proc_handler	= proc_doulongvec_minmax,
		.extra1		= (unsigned long *) &nlm_grace_period_min,
		.extra2		= (unsigned long *) &nlm_grace_period_max,
	},
	{
		.procname	= "nlm_timeout",
		.data		= &nlm_timeout,
		.maxlen		= sizeof(unsigned long),
		.mode		= 0644,
		.proc_handler	= proc_doulongvec_minmax,
		.extra1		= (unsigned long *) &nlm_timeout_min,
		.extra2		= (unsigned long *) &nlm_timeout_max,
	},
	{
		.procname	= "nlm_udpport",
		.data		= &nlm_udpport,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= (int *) &nlm_port_min,
		.extra2		= (int *) &nlm_port_max,
	},
	{
		.procname	= "nlm_tcpport",
		.data		= &nlm_tcpport,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= (int *) &nlm_port_min,
		.extra2		= (int *) &nlm_port_max,
	},
	{
		.procname	= "nsm_use_hostnames",
		.data		= &nsm_use_hostnames,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{
		.procname	= "nsm_local_state",
		.data		= &nsm_local_state,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{ }
};

static struct ctl_table nlm_sysctl_dir[] = {
	{
		.procname	= "nfs",
		.mode		= 0555,
		.child		= nlm_sysctls,
	},
	{ }
};

static struct ctl_table nlm_sysctl_root[] = {
	{
		.procname	= "fs",
		.mode		= 0555,
		.child		= nlm_sysctl_dir,
	},
	{ }
};

#endif	/* CONFIG_SYSCTL */

/*
 * Module (and sysfs) parameters.
 */

#define param_set_min_max(name, type, which_strtol, min, max)		\
static int param_set_##name(const char *val, const struct kernel_param *kp) \
{									\
	char *endp;							\
	__typeof__(type) num = which_strtol(val, &endp, 0);		\
	if (endp == val || *endp || num < (min) || num > (max))		\
		return -EINVAL;						\
	*((type *) kp->arg) = num;					\
	return 0;							\
}

static inline int is_callback(u32 proc)
{
	return proc == NLMPROC_GRANTED
		|| proc == NLMPROC_GRANTED_MSG
		|| proc == NLMPROC_TEST_RES
		|| proc == NLMPROC_LOCK_RES
		|| proc == NLMPROC_CANCEL_RES
		|| proc == NLMPROC_UNLOCK_RES
		|| proc == NLMPROC_NSM_NOTIFY;
}


static int lockd_authenticate(struct svc_rqst *rqstp)
{
	rqstp->rq_client = NULL;
	switch (rqstp->rq_authop->flavour) {
		case RPC_AUTH_NULL:
		case RPC_AUTH_UNIX:
			if (rqstp->rq_proc == 0)
				return SVC_OK;
			if (is_callback(rqstp->rq_proc)) {
				/* Leave it to individual procedures to
				 * call nlmsvc_lookup_host(rqstp)
				 */
				return SVC_OK;
			}
			return svc_set_client(rqstp);
	}
	return SVC_DENIED;
}


param_set_min_max(port, int, simple_strtol, 0, 65535)
param_set_min_max(grace_period, unsigned long, simple_strtoul,
		  nlm_grace_period_min, nlm_grace_period_max)
param_set_min_max(timeout, unsigned long, simple_strtoul,
		  nlm_timeout_min, nlm_timeout_max)

MODULE_AUTHOR("Olaf Kirch <okir@monad.swb.de>");
MODULE_DESCRIPTION("NFS file locking service version " LOCKD_VERSION ".");
MODULE_LICENSE("GPL");

module_param_call(nlm_grace_period, param_set_grace_period, param_get_ulong,
		  &nlm_grace_period, 0644);
module_param_call(nlm_timeout, param_set_timeout, param_get_ulong,
		  &nlm_timeout, 0644);
module_param_call(nlm_udpport, param_set_port, param_get_int,
		  &nlm_udpport, 0644);
module_param_call(nlm_tcpport, param_set_port, param_get_int,
		  &nlm_tcpport, 0644);
module_param(nsm_use_hostnames, bool, 0644);
module_param(nlm_max_connections, uint, 0644);

static int lockd_init_net(struct net *net)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	INIT_DELAYED_WORK(&ln->grace_period_end, grace_ender);
	INIT_LIST_HEAD(&ln->lockd_manager.list);
	ln->lockd_manager.block_opens = false;
	INIT_LIST_HEAD(&ln->nsm_handles);
	return 0;
}

static void lockd_exit_net(struct net *net)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	WARN_ONCE(!list_empty(&ln->lockd_manager.list),
		  "net %x %s: lockd_manager.list is not empty\n",
		  net->ns.inum, __func__);
	WARN_ONCE(!list_empty(&ln->nsm_handles),
		  "net %x %s: nsm_handles list is not empty\n",
		  net->ns.inum, __func__);
	WARN_ONCE(delayed_work_pending(&ln->grace_period_end),
		  "net %x %s: grace_period_end was not cancelled\n",
		  net->ns.inum, __func__);
}

static struct pernet_operations lockd_net_ops = {
	.init = lockd_init_net,
	.exit = lockd_exit_net,
	.id = &lockd_net_id,
	.size = sizeof(struct lockd_net),
};


/*
 * Initialising and terminating the module.
 */

static int __init init_nlm(void)
{
	int err;

#ifdef CONFIG_SYSCTL
	err = -ENOMEM;
	nlm_sysctl_table = register_sysctl_table(nlm_sysctl_root);
	if (nlm_sysctl_table == NULL)
		goto err_sysctl;
#endif
	err = register_pernet_subsys(&lockd_net_ops);
	if (err)
		goto err_pernet;

	err = lockd_create_procfs();
	if (err)
		goto err_procfs;

	return 0;

err_procfs:
	unregister_pernet_subsys(&lockd_net_ops);
err_pernet:
#ifdef CONFIG_SYSCTL
	unregister_sysctl_table(nlm_sysctl_table);
err_sysctl:
#endif
	return err;
}

static void __exit exit_nlm(void)
{
	/* FIXME: delete all NLM clients */
	nlm_shutdown_hosts();
	lockd_remove_procfs();
	unregister_pernet_subsys(&lockd_net_ops);
#ifdef CONFIG_SYSCTL
	unregister_sysctl_table(nlm_sysctl_table);
#endif
}

module_init(init_nlm);
module_exit(exit_nlm);

/*
 * Define NLM program and procedures
 */
static unsigned int nlmsvc_version1_count[17];
static const struct svc_version	nlmsvc_version1 = {
	.vs_vers	= 1,
	.vs_nproc	= 17,
	.vs_proc	= nlmsvc_procedures,
	.vs_count	= nlmsvc_version1_count,
	.vs_xdrsize	= NLMSVC_XDRSIZE,
};
static unsigned int nlmsvc_version3_count[24];
static const struct svc_version	nlmsvc_version3 = {
	.vs_vers	= 3,
	.vs_nproc	= 24,
	.vs_proc	= nlmsvc_procedures,
	.vs_count	= nlmsvc_version3_count,
	.vs_xdrsize	= NLMSVC_XDRSIZE,
};
#ifdef CONFIG_LOCKD_V4
static unsigned int nlmsvc_version4_count[24];
static const struct svc_version	nlmsvc_version4 = {
	.vs_vers	= 4,
	.vs_nproc	= 24,
	.vs_proc	= nlmsvc_procedures4,
	.vs_count	= nlmsvc_version4_count,
	.vs_xdrsize	= NLMSVC_XDRSIZE,
};
#endif
static const struct svc_version *nlmsvc_version[] = {
	[1] = &nlmsvc_version1,
	[3] = &nlmsvc_version3,
#ifdef CONFIG_LOCKD_V4
	[4] = &nlmsvc_version4,
#endif
};

static struct svc_stat		nlmsvc_stats;

#define NLM_NRVERS	ARRAY_SIZE(nlmsvc_version)
static struct svc_program	nlmsvc_program = {
	.pg_prog		= NLM_PROGRAM,		/* program number */
	.pg_nvers		= NLM_NRVERS,		/* number of entries in nlmsvc_version */
	.pg_vers		= nlmsvc_version,	/* version table */
	.pg_name		= "lockd",		/* service name */
	.pg_class		= "nfsd",		/* share authentication with nfsd */
	.pg_stats		= &nlmsvc_stats,	/* stats table */
	.pg_authenticate	= &lockd_authenticate,	/* export authentication */
	.pg_init_request	= svc_generic_init_request,
	.pg_rpcbind_set		= svc_generic_rpcbind_set,
};
