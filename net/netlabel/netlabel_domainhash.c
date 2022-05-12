// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * NetLabel Domain Hash Table
 *
 * This file manages the domain hash table that NetLabel uses to determine
 * which network labeling protocol to use for a given domain.  The NetLabel
 * system manages static and dynamic label mappings for network protocols such
 * as CIPSO and RIPSO.
 *
 * Author: Paul Moore <paul@paul-moore.com>
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2006, 2008
 */

#include <linux/types.h>
#include <linux/rculist.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/audit.h>
#include <linux/slab.h>
#include <net/netlabel.h>
#include <net/cipso_ipv4.h>
#include <net/calipso.h>
#include <asm/bug.h>

#include "netlabel_mgmt.h"
#include "netlabel_addrlist.h"
#include "netlabel_calipso.h"
#include "netlabel_domainhash.h"
#include "netlabel_user.h"

struct netlbl_domhsh_tbl {
	struct list_head *tbl;
	u32 size;
};

/* Domain hash table */
/* updates should be so rare that having one spinlock for the entire hash table
 * should be okay */
static DEFINE_SPINLOCK(netlbl_domhsh_lock);
#define netlbl_domhsh_rcu_deref(p) \
	rcu_dereference_check(p, lockdep_is_held(&netlbl_domhsh_lock))
static struct netlbl_domhsh_tbl __rcu *netlbl_domhsh;
static struct netlbl_dom_map __rcu *netlbl_domhsh_def_ipv4;
static struct netlbl_dom_map __rcu *netlbl_domhsh_def_ipv6;

/*
 * Domain Hash Table Helper Functions
 */

/**
 * netlbl_domhsh_free_entry - Frees a domain hash table entry
 * @entry: the entry's RCU field
 *
 * Description:
 * This function is designed to be used as a callback to the call_rcu()
 * function so that the memory allocated to a hash table entry can be released
 * safely.
 *
 */
static void netlbl_domhsh_free_entry(struct rcu_head *entry)
{
	struct netlbl_dom_map *ptr;
	struct netlbl_af4list *iter4;
	struct netlbl_af4list *tmp4;
#if IS_ENABLED(CONFIG_IPV6)
	struct netlbl_af6list *iter6;
	struct netlbl_af6list *tmp6;
#endif /* IPv6 */

	ptr = container_of(entry, struct netlbl_dom_map, rcu);
	if (ptr->def.type == NETLBL_NLTYPE_ADDRSELECT) {
		netlbl_af4list_foreach_safe(iter4, tmp4,
					    &ptr->def.addrsel->list4) {
			netlbl_af4list_remove_entry(iter4);
			kfree(netlbl_domhsh_addr4_entry(iter4));
		}
#if IS_ENABLED(CONFIG_IPV6)
		netlbl_af6list_foreach_safe(iter6, tmp6,
					    &ptr->def.addrsel->list6) {
			netlbl_af6list_remove_entry(iter6);
			kfree(netlbl_domhsh_addr6_entry(iter6));
		}
#endif /* IPv6 */
		kfree(ptr->def.addrsel);
	}
	kfree(ptr->domain);
	kfree(ptr);
}

/**
 * netlbl_domhsh_hash - Hashing function for the domain hash table
 * @key: the domain name to hash
 *
 * Description:
 * This is the hashing function for the domain hash table, it returns the
 * correct bucket number for the domain.  The caller is responsible for
 * ensuring that the hash table is protected with either a RCU read lock or the
 * hash table lock.
 *
 */
static u32 netlbl_domhsh_hash(const char *key)
{
	u32 iter;
	u32 val;
	u32 len;

	/* This is taken (with slight modification) from
	 * security/selinux/ss/symtab.c:symhash() */

	for (iter = 0, val = 0, len = strlen(key); iter < len; iter++)
		val = (val << 4 | (val >> (8 * sizeof(u32) - 4))) ^ key[iter];
	return val & (netlbl_domhsh_rcu_deref(netlbl_domhsh)->size - 1);
}

static bool netlbl_family_match(u16 f1, u16 f2)
{
	return (f1 == f2) || (f1 == AF_UNSPEC) || (f2 == AF_UNSPEC);
}

/**
 * netlbl_domhsh_search - Search for a domain entry
 * @domain: the domain
 * @family: the address family
 *
 * Description:
 * Searches the domain hash table and returns a pointer to the hash table
 * entry if found, otherwise NULL is returned.  @family may be %AF_UNSPEC
 * which matches any address family entries.  The caller is responsible for
 * ensuring that the hash table is protected with either a RCU read lock or the
 * hash table lock.
 *
 */
static struct netlbl_dom_map *netlbl_domhsh_search(const char *domain,
						   u16 family)
{
	u32 bkt;
	struct list_head *bkt_list;
	struct netlbl_dom_map *iter;

	if (domain != NULL) {
		bkt = netlbl_domhsh_hash(domain);
		bkt_list = &netlbl_domhsh_rcu_deref(netlbl_domhsh)->tbl[bkt];
		list_for_each_entry_rcu(iter, bkt_list, list,
					lockdep_is_held(&netlbl_domhsh_lock))
			if (iter->valid &&
			    netlbl_family_match(iter->family, family) &&
			    strcmp(iter->domain, domain) == 0)
				return iter;
	}

	return NULL;
}

/**
 * netlbl_domhsh_search_def - Search for a domain entry
 * @domain: the domain
 * @family: the address family
 *
 * Description:
 * Searches the domain hash table and returns a pointer to the hash table
 * entry if an exact match is found, if an exact match is not present in the
 * hash table then the default entry is returned if valid otherwise NULL is
 * returned.  @family may be %AF_UNSPEC which matches any address family
 * entries.  The caller is responsible ensuring that the hash table is
 * protected with either a RCU read lock or the hash table lock.
 *
 */
static struct netlbl_dom_map *netlbl_domhsh_search_def(const char *domain,
						       u16 family)
{
	struct netlbl_dom_map *entry;

	entry = netlbl_domhsh_search(domain, family);
	if (entry != NULL)
		return entry;
	if (family == AF_INET || family == AF_UNSPEC) {
		entry = netlbl_domhsh_rcu_deref(netlbl_domhsh_def_ipv4);
		if (entry != NULL && entry->valid)
			return entry;
	}
	if (family == AF_INET6 || family == AF_UNSPEC) {
		entry = netlbl_domhsh_rcu_deref(netlbl_domhsh_def_ipv6);
		if (entry != NULL && entry->valid)
			return entry;
	}

	return NULL;
}

/**
 * netlbl_domhsh_audit_add - Generate an audit entry for an add event
 * @entry: the entry being added
 * @addr4: the IPv4 address information
 * @addr6: the IPv6 address information
 * @result: the result code
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Generate an audit record for adding a new NetLabel/LSM mapping entry with
 * the given information.  Caller is responsible for holding the necessary
 * locks.
 *
 */
static void netlbl_domhsh_audit_add(struct netlbl_dom_map *entry,
				    struct netlbl_af4list *addr4,
				    struct netlbl_af6list *addr6,
				    int result,
				    struct netlbl_audit *audit_info)
{
	struct audit_buffer *audit_buf;
	struct cipso_v4_doi *cipsov4 = NULL;
	struct calipso_doi *calipso = NULL;
	u32 type;

	audit_buf = netlbl_audit_start_common(AUDIT_MAC_MAP_ADD, audit_info);
	if (audit_buf != NULL) {
		audit_log_format(audit_buf, " nlbl_domain=%s",
				 entry->domain ? entry->domain : "(default)");
		if (addr4 != NULL) {
			struct netlbl_domaddr4_map *map4;
			map4 = netlbl_domhsh_addr4_entry(addr4);
			type = map4->def.type;
			cipsov4 = map4->def.cipso;
			netlbl_af4list_audit_addr(audit_buf, 0, NULL,
						  addr4->addr, addr4->mask);
#if IS_ENABLED(CONFIG_IPV6)
		} else if (addr6 != NULL) {
			struct netlbl_domaddr6_map *map6;
			map6 = netlbl_domhsh_addr6_entry(addr6);
			type = map6->def.type;
			calipso = map6->def.calipso;
			netlbl_af6list_audit_addr(audit_buf, 0, NULL,
						  &addr6->addr, &addr6->mask);
#endif /* IPv6 */
		} else {
			type = entry->def.type;
			cipsov4 = entry->def.cipso;
			calipso = entry->def.calipso;
		}
		switch (type) {
		case NETLBL_NLTYPE_UNLABELED:
			audit_log_format(audit_buf, " nlbl_protocol=unlbl");
			break;
		case NETLBL_NLTYPE_CIPSOV4:
			BUG_ON(cipsov4 == NULL);
			audit_log_format(audit_buf,
					 " nlbl_protocol=cipsov4 cipso_doi=%u",
					 cipsov4->doi);
			break;
		case NETLBL_NLTYPE_CALIPSO:
			BUG_ON(calipso == NULL);
			audit_log_format(audit_buf,
					 " nlbl_protocol=calipso calipso_doi=%u",
					 calipso->doi);
			break;
		}
		audit_log_format(audit_buf, " res=%u", result == 0 ? 1 : 0);
		audit_log_end(audit_buf);
	}
}

/**
 * netlbl_domhsh_validate - Validate a new domain mapping entry
 * @entry: the entry to validate
 *
 * This function validates the new domain mapping entry to ensure that it is
 * a valid entry.  Returns zero on success, negative values on failure.
 *
 */
static int netlbl_domhsh_validate(const struct netlbl_dom_map *entry)
{
	struct netlbl_af4list *iter4;
	struct netlbl_domaddr4_map *map4;
#if IS_ENABLED(CONFIG_IPV6)
	struct netlbl_af6list *iter6;
	struct netlbl_domaddr6_map *map6;
#endif /* IPv6 */

	if (entry == NULL)
		return -EINVAL;

	if (entry->family != AF_INET && entry->family != AF_INET6 &&
	    (entry->family != AF_UNSPEC ||
	     entry->def.type != NETLBL_NLTYPE_UNLABELED))
		return -EINVAL;

	switch (entry->def.type) {
	case NETLBL_NLTYPE_UNLABELED:
		if (entry->def.cipso != NULL || entry->def.calipso != NULL ||
		    entry->def.addrsel != NULL)
			return -EINVAL;
		break;
	case NETLBL_NLTYPE_CIPSOV4:
		if (entry->family != AF_INET ||
		    entry->def.cipso == NULL)
			return -EINVAL;
		break;
	case NETLBL_NLTYPE_CALIPSO:
		if (entry->family != AF_INET6 ||
		    entry->def.calipso == NULL)
			return -EINVAL;
		break;
	case NETLBL_NLTYPE_ADDRSELECT:
		netlbl_af4list_foreach(iter4, &entry->def.addrsel->list4) {
			map4 = netlbl_domhsh_addr4_entry(iter4);
			switch (map4->def.type) {
			case NETLBL_NLTYPE_UNLABELED:
				if (map4->def.cipso != NULL)
					return -EINVAL;
				break;
			case NETLBL_NLTYPE_CIPSOV4:
				if (map4->def.cipso == NULL)
					return -EINVAL;
				break;
			default:
				return -EINVAL;
			}
		}
#if IS_ENABLED(CONFIG_IPV6)
		netlbl_af6list_foreach(iter6, &entry->def.addrsel->list6) {
			map6 = netlbl_domhsh_addr6_entry(iter6);
			switch (map6->def.type) {
			case NETLBL_NLTYPE_UNLABELED:
				if (map6->def.calipso != NULL)
					return -EINVAL;
				break;
			case NETLBL_NLTYPE_CALIPSO:
				if (map6->def.calipso == NULL)
					return -EINVAL;
				break;
			default:
				return -EINVAL;
			}
		}
#endif /* IPv6 */
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/*
 * Domain Hash Table Functions
 */

/**
 * netlbl_domhsh_init - Init for the domain hash
 * @size: the number of bits to use for the hash buckets
 *
 * Description:
 * Initializes the domain hash table, should be called only by
 * netlbl_user_init() during initialization.  Returns zero on success, non-zero
 * values on error.
 *
 */
int __init netlbl_domhsh_init(u32 size)
{
	u32 iter;
	struct netlbl_domhsh_tbl *hsh_tbl;

	if (size == 0)
		return -EINVAL;

	hsh_tbl = kmalloc(sizeof(*hsh_tbl), GFP_KERNEL);
	if (hsh_tbl == NULL)
		return -ENOMEM;
	hsh_tbl->size = 1 << size;
	hsh_tbl->tbl = kcalloc(hsh_tbl->size,
			       sizeof(struct list_head),
			       GFP_KERNEL);
	if (hsh_tbl->tbl == NULL) {
		kfree(hsh_tbl);
		return -ENOMEM;
	}
	for (iter = 0; iter < hsh_tbl->size; iter++)
		INIT_LIST_HEAD(&hsh_tbl->tbl[iter]);

	spin_lock(&netlbl_domhsh_lock);
	rcu_assign_pointer(netlbl_domhsh, hsh_tbl);
	spin_unlock(&netlbl_domhsh_lock);

	return 0;
}

/**
 * netlbl_domhsh_add - Adds a entry to the domain hash table
 * @entry: the entry to add
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Adds a new entry to the domain hash table and handles any updates to the
 * lower level protocol handler (i.e. CIPSO).  @entry->family may be set to
 * %AF_UNSPEC which will add an entry that matches all address families.  This
 * is only useful for the unlabelled type and will only succeed if there is no
 * existing entry for any address family with the same domain.  Returns zero
 * on success, negative on failure.
 *
 */
int netlbl_domhsh_add(struct netlbl_dom_map *entry,
		      struct netlbl_audit *audit_info)
{
	int ret_val = 0;
	struct netlbl_dom_map *entry_old, *entry_b;
	struct netlbl_af4list *iter4;
	struct netlbl_af4list *tmp4;
#if IS_ENABLED(CONFIG_IPV6)
	struct netlbl_af6list *iter6;
	struct netlbl_af6list *tmp6;
#endif /* IPv6 */

	ret_val = netlbl_domhsh_validate(entry);
	if (ret_val != 0)
		return ret_val;

	/* XXX - we can remove this RCU read lock as the spinlock protects the
	 *       entire function, but before we do we need to fixup the
	 *       netlbl_af[4,6]list RCU functions to do "the right thing" with
	 *       respect to rcu_dereference() when only a spinlock is held. */
	rcu_read_lock();
	spin_lock(&netlbl_domhsh_lock);
	if (entry->domain != NULL)
		entry_old = netlbl_domhsh_search(entry->domain, entry->family);
	else
		entry_old = netlbl_domhsh_search_def(entry->domain,
						     entry->family);
	if (entry_old == NULL) {
		entry->valid = 1;

		if (entry->domain != NULL) {
			u32 bkt = netlbl_domhsh_hash(entry->domain);
			list_add_tail_rcu(&entry->list,
				    &rcu_dereference(netlbl_domhsh)->tbl[bkt]);
		} else {
			INIT_LIST_HEAD(&entry->list);
			switch (entry->family) {
			case AF_INET:
				rcu_assign_pointer(netlbl_domhsh_def_ipv4,
						   entry);
				break;
			case AF_INET6:
				rcu_assign_pointer(netlbl_domhsh_def_ipv6,
						   entry);
				break;
			case AF_UNSPEC:
				if (entry->def.type !=
				    NETLBL_NLTYPE_UNLABELED) {
					ret_val = -EINVAL;
					goto add_return;
				}
				entry_b = kzalloc(sizeof(*entry_b), GFP_ATOMIC);
				if (entry_b == NULL) {
					ret_val = -ENOMEM;
					goto add_return;
				}
				entry_b->family = AF_INET6;
				entry_b->def.type = NETLBL_NLTYPE_UNLABELED;
				entry_b->valid = 1;
				entry->family = AF_INET;
				rcu_assign_pointer(netlbl_domhsh_def_ipv4,
						   entry);
				rcu_assign_pointer(netlbl_domhsh_def_ipv6,
						   entry_b);
				break;
			default:
				/* Already checked in
				 * netlbl_domhsh_validate(). */
				ret_val = -EINVAL;
				goto add_return;
			}
		}

		if (entry->def.type == NETLBL_NLTYPE_ADDRSELECT) {
			netlbl_af4list_foreach_rcu(iter4,
						   &entry->def.addrsel->list4)
				netlbl_domhsh_audit_add(entry, iter4, NULL,
							ret_val, audit_info);
#if IS_ENABLED(CONFIG_IPV6)
			netlbl_af6list_foreach_rcu(iter6,
						   &entry->def.addrsel->list6)
				netlbl_domhsh_audit_add(entry, NULL, iter6,
							ret_val, audit_info);
#endif /* IPv6 */
		} else
			netlbl_domhsh_audit_add(entry, NULL, NULL,
						ret_val, audit_info);
	} else if (entry_old->def.type == NETLBL_NLTYPE_ADDRSELECT &&
		   entry->def.type == NETLBL_NLTYPE_ADDRSELECT) {
		struct list_head *old_list4;
		struct list_head *old_list6;

		old_list4 = &entry_old->def.addrsel->list4;
		old_list6 = &entry_old->def.addrsel->list6;

		/* we only allow the addition of address selectors if all of
		 * the selectors do not exist in the existing domain map */
		netlbl_af4list_foreach_rcu(iter4, &entry->def.addrsel->list4)
			if (netlbl_af4list_search_exact(iter4->addr,
							iter4->mask,
							old_list4)) {
				ret_val = -EEXIST;
				goto add_return;
			}
#if IS_ENABLED(CONFIG_IPV6)
		netlbl_af6list_foreach_rcu(iter6, &entry->def.addrsel->list6)
			if (netlbl_af6list_search_exact(&iter6->addr,
							&iter6->mask,
							old_list6)) {
				ret_val = -EEXIST;
				goto add_return;
			}
#endif /* IPv6 */

		netlbl_af4list_foreach_safe(iter4, tmp4,
					    &entry->def.addrsel->list4) {
			netlbl_af4list_remove_entry(iter4);
			iter4->valid = 1;
			ret_val = netlbl_af4list_add(iter4, old_list4);
			netlbl_domhsh_audit_add(entry_old, iter4, NULL,
						ret_val, audit_info);
			if (ret_val != 0)
				goto add_return;
		}
#if IS_ENABLED(CONFIG_IPV6)
		netlbl_af6list_foreach_safe(iter6, tmp6,
					    &entry->def.addrsel->list6) {
			netlbl_af6list_remove_entry(iter6);
			iter6->valid = 1;
			ret_val = netlbl_af6list_add(iter6, old_list6);
			netlbl_domhsh_audit_add(entry_old, NULL, iter6,
						ret_val, audit_info);
			if (ret_val != 0)
				goto add_return;
		}
#endif /* IPv6 */
		/* cleanup the new entry since we've moved everything over */
		netlbl_domhsh_free_entry(&entry->rcu);
	} else
		ret_val = -EINVAL;

add_return:
	spin_unlock(&netlbl_domhsh_lock);
	rcu_read_unlock();
	return ret_val;
}

/**
 * netlbl_domhsh_add_default - Adds the default entry to the domain hash table
 * @entry: the entry to add
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Adds a new default entry to the domain hash table and handles any updates
 * to the lower level protocol handler (i.e. CIPSO).  Returns zero on success,
 * negative on failure.
 *
 */
int netlbl_domhsh_add_default(struct netlbl_dom_map *entry,
			      struct netlbl_audit *audit_info)
{
	return netlbl_domhsh_add(entry, audit_info);
}

/**
 * netlbl_domhsh_remove_entry - Removes a given entry from the domain table
 * @entry: the entry to remove
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Removes an entry from the domain hash table and handles any updates to the
 * lower level protocol handler (i.e. CIPSO).  Caller is responsible for
 * ensuring that the RCU read lock is held.  Returns zero on success, negative
 * on failure.
 *
 */
int netlbl_domhsh_remove_entry(struct netlbl_dom_map *entry,
			       struct netlbl_audit *audit_info)
{
	int ret_val = 0;
	struct audit_buffer *audit_buf;
	struct netlbl_af4list *iter4;
	struct netlbl_domaddr4_map *map4;
#if IS_ENABLED(CONFIG_IPV6)
	struct netlbl_af6list *iter6;
	struct netlbl_domaddr6_map *map6;
#endif /* IPv6 */

	if (entry == NULL)
		return -ENOENT;

	spin_lock(&netlbl_domhsh_lock);
	if (entry->valid) {
		entry->valid = 0;
		if (entry == rcu_dereference(netlbl_domhsh_def_ipv4))
			RCU_INIT_POINTER(netlbl_domhsh_def_ipv4, NULL);
		else if (entry == rcu_dereference(netlbl_domhsh_def_ipv6))
			RCU_INIT_POINTER(netlbl_domhsh_def_ipv6, NULL);
		else
			list_del_rcu(&entry->list);
	} else
		ret_val = -ENOENT;
	spin_unlock(&netlbl_domhsh_lock);

	if (ret_val)
		return ret_val;

	audit_buf = netlbl_audit_start_common(AUDIT_MAC_MAP_DEL, audit_info);
	if (audit_buf != NULL) {
		audit_log_format(audit_buf,
				 " nlbl_domain=%s res=1",
				 entry->domain ? entry->domain : "(default)");
		audit_log_end(audit_buf);
	}

	switch (entry->def.type) {
	case NETLBL_NLTYPE_ADDRSELECT:
		netlbl_af4list_foreach_rcu(iter4, &entry->def.addrsel->list4) {
			map4 = netlbl_domhsh_addr4_entry(iter4);
			cipso_v4_doi_putdef(map4->def.cipso);
		}
#if IS_ENABLED(CONFIG_IPV6)
		netlbl_af6list_foreach_rcu(iter6, &entry->def.addrsel->list6) {
			map6 = netlbl_domhsh_addr6_entry(iter6);
			calipso_doi_putdef(map6->def.calipso);
		}
#endif /* IPv6 */
		break;
	case NETLBL_NLTYPE_CIPSOV4:
		cipso_v4_doi_putdef(entry->def.cipso);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case NETLBL_NLTYPE_CALIPSO:
		calipso_doi_putdef(entry->def.calipso);
		break;
#endif /* IPv6 */
	}
	call_rcu(&entry->rcu, netlbl_domhsh_free_entry);

	return ret_val;
}

/**
 * netlbl_domhsh_remove_af4 - Removes an address selector entry
 * @domain: the domain
 * @addr: IPv4 address
 * @mask: IPv4 address mask
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Removes an individual address selector from a domain mapping and potentially
 * the entire mapping if it is empty.  Returns zero on success, negative values
 * on failure.
 *
 */
int netlbl_domhsh_remove_af4(const char *domain,
			     const struct in_addr *addr,
			     const struct in_addr *mask,
			     struct netlbl_audit *audit_info)
{
	struct netlbl_dom_map *entry_map;
	struct netlbl_af4list *entry_addr;
	struct netlbl_af4list *iter4;
#if IS_ENABLED(CONFIG_IPV6)
	struct netlbl_af6list *iter6;
#endif /* IPv6 */
	struct netlbl_domaddr4_map *entry;

	rcu_read_lock();

	if (domain)
		entry_map = netlbl_domhsh_search(domain, AF_INET);
	else
		entry_map = netlbl_domhsh_search_def(domain, AF_INET);
	if (entry_map == NULL ||
	    entry_map->def.type != NETLBL_NLTYPE_ADDRSELECT)
		goto remove_af4_failure;

	spin_lock(&netlbl_domhsh_lock);
	entry_addr = netlbl_af4list_remove(addr->s_addr, mask->s_addr,
					   &entry_map->def.addrsel->list4);
	spin_unlock(&netlbl_domhsh_lock);

	if (entry_addr == NULL)
		goto remove_af4_failure;
	netlbl_af4list_foreach_rcu(iter4, &entry_map->def.addrsel->list4)
		goto remove_af4_single_addr;
#if IS_ENABLED(CONFIG_IPV6)
	netlbl_af6list_foreach_rcu(iter6, &entry_map->def.addrsel->list6)
		goto remove_af4_single_addr;
#endif /* IPv6 */
	/* the domain mapping is empty so remove it from the mapping table */
	netlbl_domhsh_remove_entry(entry_map, audit_info);

remove_af4_single_addr:
	rcu_read_unlock();
	/* yick, we can't use call_rcu here because we don't have a rcu head
	 * pointer but hopefully this should be a rare case so the pause
	 * shouldn't be a problem */
	synchronize_rcu();
	entry = netlbl_domhsh_addr4_entry(entry_addr);
	cipso_v4_doi_putdef(entry->def.cipso);
	kfree(entry);
	return 0;

remove_af4_failure:
	rcu_read_unlock();
	return -ENOENT;
}

#if IS_ENABLED(CONFIG_IPV6)
/**
 * netlbl_domhsh_remove_af6 - Removes an address selector entry
 * @domain: the domain
 * @addr: IPv6 address
 * @mask: IPv6 address mask
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Removes an individual address selector from a domain mapping and potentially
 * the entire mapping if it is empty.  Returns zero on success, negative values
 * on failure.
 *
 */
int netlbl_domhsh_remove_af6(const char *domain,
			     const struct in6_addr *addr,
			     const struct in6_addr *mask,
			     struct netlbl_audit *audit_info)
{
	struct netlbl_dom_map *entry_map;
	struct netlbl_af6list *entry_addr;
	struct netlbl_af4list *iter4;
	struct netlbl_af6list *iter6;
	struct netlbl_domaddr6_map *entry;

	rcu_read_lock();

	if (domain)
		entry_map = netlbl_domhsh_search(domain, AF_INET6);
	else
		entry_map = netlbl_domhsh_search_def(domain, AF_INET6);
	if (entry_map == NULL ||
	    entry_map->def.type != NETLBL_NLTYPE_ADDRSELECT)
		goto remove_af6_failure;

	spin_lock(&netlbl_domhsh_lock);
	entry_addr = netlbl_af6list_remove(addr, mask,
					   &entry_map->def.addrsel->list6);
	spin_unlock(&netlbl_domhsh_lock);

	if (entry_addr == NULL)
		goto remove_af6_failure;
	netlbl_af4list_foreach_rcu(iter4, &entry_map->def.addrsel->list4)
		goto remove_af6_single_addr;
	netlbl_af6list_foreach_rcu(iter6, &entry_map->def.addrsel->list6)
		goto remove_af6_single_addr;
	/* the domain mapping is empty so remove it from the mapping table */
	netlbl_domhsh_remove_entry(entry_map, audit_info);

remove_af6_single_addr:
	rcu_read_unlock();
	/* yick, we can't use call_rcu here because we don't have a rcu head
	 * pointer but hopefully this should be a rare case so the pause
	 * shouldn't be a problem */
	synchronize_rcu();
	entry = netlbl_domhsh_addr6_entry(entry_addr);
	calipso_doi_putdef(entry->def.calipso);
	kfree(entry);
	return 0;

remove_af6_failure:
	rcu_read_unlock();
	return -ENOENT;
}
#endif /* IPv6 */

/**
 * netlbl_domhsh_remove - Removes an entry from the domain hash table
 * @domain: the domain to remove
 * @family: address family
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Removes an entry from the domain hash table and handles any updates to the
 * lower level protocol handler (i.e. CIPSO).  @family may be %AF_UNSPEC which
 * removes all address family entries.  Returns zero on success, negative on
 * failure.
 *
 */
int netlbl_domhsh_remove(const char *domain, u16 family,
			 struct netlbl_audit *audit_info)
{
	int ret_val = -EINVAL;
	struct netlbl_dom_map *entry;

	rcu_read_lock();

	if (family == AF_INET || family == AF_UNSPEC) {
		if (domain)
			entry = netlbl_domhsh_search(domain, AF_INET);
		else
			entry = netlbl_domhsh_search_def(domain, AF_INET);
		ret_val = netlbl_domhsh_remove_entry(entry, audit_info);
		if (ret_val && ret_val != -ENOENT)
			goto done;
	}
	if (family == AF_INET6 || family == AF_UNSPEC) {
		int ret_val2;

		if (domain)
			entry = netlbl_domhsh_search(domain, AF_INET6);
		else
			entry = netlbl_domhsh_search_def(domain, AF_INET6);
		ret_val2 = netlbl_domhsh_remove_entry(entry, audit_info);
		if (ret_val2 != -ENOENT)
			ret_val = ret_val2;
	}
done:
	rcu_read_unlock();

	return ret_val;
}

/**
 * netlbl_domhsh_remove_default - Removes the default entry from the table
 * @family: address family
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Removes/resets the default entry corresponding to @family from the domain
 * hash table and handles any updates to the lower level protocol handler
 * (i.e. CIPSO).  @family may be %AF_UNSPEC which removes all address family
 * entries.  Returns zero on success, negative on failure.
 *
 */
int netlbl_domhsh_remove_default(u16 family, struct netlbl_audit *audit_info)
{
	return netlbl_domhsh_remove(NULL, family, audit_info);
}

/**
 * netlbl_domhsh_getentry - Get an entry from the domain hash table
 * @domain: the domain name to search for
 * @family: address family
 *
 * Description:
 * Look through the domain hash table searching for an entry to match @domain,
 * with address family @family, return a pointer to a copy of the entry or
 * NULL.  The caller is responsible for ensuring that rcu_read_[un]lock() is
 * called.
 *
 */
struct netlbl_dom_map *netlbl_domhsh_getentry(const char *domain, u16 family)
{
	if (family == AF_UNSPEC)
		return NULL;
	return netlbl_domhsh_search_def(domain, family);
}

/**
 * netlbl_domhsh_getentry_af4 - Get an entry from the domain hash table
 * @domain: the domain name to search for
 * @addr: the IP address to search for
 *
 * Description:
 * Look through the domain hash table searching for an entry to match @domain
 * and @addr, return a pointer to a copy of the entry or NULL.  The caller is
 * responsible for ensuring that rcu_read_[un]lock() is called.
 *
 */
struct netlbl_dommap_def *netlbl_domhsh_getentry_af4(const char *domain,
						     __be32 addr)
{
	struct netlbl_dom_map *dom_iter;
	struct netlbl_af4list *addr_iter;

	dom_iter = netlbl_domhsh_search_def(domain, AF_INET);
	if (dom_iter == NULL)
		return NULL;

	if (dom_iter->def.type != NETLBL_NLTYPE_ADDRSELECT)
		return &dom_iter->def;
	addr_iter = netlbl_af4list_search(addr, &dom_iter->def.addrsel->list4);
	if (addr_iter == NULL)
		return NULL;
	return &(netlbl_domhsh_addr4_entry(addr_iter)->def);
}

#if IS_ENABLED(CONFIG_IPV6)
/**
 * netlbl_domhsh_getentry_af6 - Get an entry from the domain hash table
 * @domain: the domain name to search for
 * @addr: the IP address to search for
 *
 * Description:
 * Look through the domain hash table searching for an entry to match @domain
 * and @addr, return a pointer to a copy of the entry or NULL.  The caller is
 * responsible for ensuring that rcu_read_[un]lock() is called.
 *
 */
struct netlbl_dommap_def *netlbl_domhsh_getentry_af6(const char *domain,
						   const struct in6_addr *addr)
{
	struct netlbl_dom_map *dom_iter;
	struct netlbl_af6list *addr_iter;

	dom_iter = netlbl_domhsh_search_def(domain, AF_INET6);
	if (dom_iter == NULL)
		return NULL;

	if (dom_iter->def.type != NETLBL_NLTYPE_ADDRSELECT)
		return &dom_iter->def;
	addr_iter = netlbl_af6list_search(addr, &dom_iter->def.addrsel->list6);
	if (addr_iter == NULL)
		return NULL;
	return &(netlbl_domhsh_addr6_entry(addr_iter)->def);
}
#endif /* IPv6 */

/**
 * netlbl_domhsh_walk - Iterate through the domain mapping hash table
 * @skip_bkt: the number of buckets to skip at the start
 * @skip_chain: the number of entries to skip in the first iterated bucket
 * @callback: callback for each entry
 * @cb_arg: argument for the callback function
 *
 * Description:
 * Interate over the domain mapping hash table, skipping the first @skip_bkt
 * buckets and @skip_chain entries.  For each entry in the table call
 * @callback, if @callback returns a negative value stop 'walking' through the
 * table and return.  Updates the values in @skip_bkt and @skip_chain on
 * return.  Returns zero on success, negative values on failure.
 *
 */
int netlbl_domhsh_walk(u32 *skip_bkt,
		     u32 *skip_chain,
		     int (*callback) (struct netlbl_dom_map *entry, void *arg),
		     void *cb_arg)
{
	int ret_val = -ENOENT;
	u32 iter_bkt;
	struct list_head *iter_list;
	struct netlbl_dom_map *iter_entry;
	u32 chain_cnt = 0;

	rcu_read_lock();
	for (iter_bkt = *skip_bkt;
	     iter_bkt < rcu_dereference(netlbl_domhsh)->size;
	     iter_bkt++, chain_cnt = 0) {
		iter_list = &rcu_dereference(netlbl_domhsh)->tbl[iter_bkt];
		list_for_each_entry_rcu(iter_entry, iter_list, list)
			if (iter_entry->valid) {
				if (chain_cnt++ < *skip_chain)
					continue;
				ret_val = callback(iter_entry, cb_arg);
				if (ret_val < 0) {
					chain_cnt--;
					goto walk_return;
				}
			}
	}

walk_return:
	rcu_read_unlock();
	*skip_bkt = iter_bkt;
	*skip_chain = chain_cnt;
	return ret_val;
}
