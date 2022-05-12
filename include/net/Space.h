/* SPDX-License-Identifier: GPL-2.0 */
/* A unified ethernet device probe.  This is the easiest way to have every
 * ethernet adaptor have the name "eth[0123...]".
 */

struct net_device *hp100_probe(int unit);
struct net_device *ultra_probe(int unit);
struct net_device *wd_probe(int unit);
struct net_device *ne_probe(int unit);
struct net_device *fmv18x_probe(int unit);
struct net_device *i82596_probe(int unit);
struct net_device *ni65_probe(int unit);
struct net_device *sonic_probe(int unit);
struct net_device *smc_init(int unit);
struct net_device *atarilance_probe(int unit);
struct net_device *sun3lance_probe(int unit);
struct net_device *sun3_82586_probe(int unit);
struct net_device *apne_probe(int unit);
struct net_device *cs89x0_probe(int unit);
struct net_device *mvme147lance_probe(int unit);
struct net_device *tc515_probe(int unit);
struct net_device *lance_probe(int unit);
struct net_device *cops_probe(int unit);
struct net_device *ltpc_probe(void);

/* Fibre Channel adapters */
int iph5526_probe(struct net_device *dev);

/* SBNI adapters */
int sbni_probe(int unit);
