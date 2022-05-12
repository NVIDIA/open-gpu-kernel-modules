/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * NetLabel Network Address Lists
 *
 * This file contains network address list functions used to manage ordered
 * lists of network addresses for use by the NetLabel subsystem.  The NetLabel
 * system manages static and dynamic label mappings for network protocols such
 * as CIPSO and RIPSO.
 *
 * Author: Paul Moore <paul@paul-moore.com>
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2008
 */

#ifndef _NETLABEL_ADDRLIST_H
#define _NETLABEL_ADDRLIST_H

#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/list.h>
#include <linux/in6.h>
#include <linux/audit.h>

/**
 * struct netlbl_af4list - NetLabel IPv4 address list
 * @addr: IPv4 address
 * @mask: IPv4 address mask
 * @valid: valid flag
 * @list: list structure, used internally
 */
struct netlbl_af4list {
	__be32 addr;
	__be32 mask;

	u32 valid;
	struct list_head list;
};

/**
 * struct netlbl_af6list - NetLabel IPv6 address list
 * @addr: IPv6 address
 * @mask: IPv6 address mask
 * @valid: valid flag
 * @list: list structure, used internally
 */
struct netlbl_af6list {
	struct in6_addr addr;
	struct in6_addr mask;

	u32 valid;
	struct list_head list;
};

#define __af4list_entry(ptr) container_of(ptr, struct netlbl_af4list, list)

static inline struct netlbl_af4list *__af4list_valid(struct list_head *s,
						     struct list_head *h)
{
	struct list_head *i = s;
	struct netlbl_af4list *n = __af4list_entry(s);
	while (i != h && !n->valid) {
		i = i->next;
		n = __af4list_entry(i);
	}
	return n;
}

static inline struct netlbl_af4list *__af4list_valid_rcu(struct list_head *s,
							 struct list_head *h)
{
	struct list_head *i = s;
	struct netlbl_af4list *n = __af4list_entry(s);
	while (i != h && !n->valid) {
		i = rcu_dereference(list_next_rcu(i));
		n = __af4list_entry(i);
	}
	return n;
}

#define netlbl_af4list_foreach(iter, head)				\
	for (iter = __af4list_valid((head)->next, head);		\
	     &iter->list != (head);					\
	     iter = __af4list_valid(iter->list.next, head))

#define netlbl_af4list_foreach_rcu(iter, head)				\
	for (iter = __af4list_valid_rcu((head)->next, head);		\
	     &iter->list != (head);					\
	     iter = __af4list_valid_rcu(iter->list.next, head))

#define netlbl_af4list_foreach_safe(iter, tmp, head)			\
	for (iter = __af4list_valid((head)->next, head),		\
		     tmp = __af4list_valid(iter->list.next, head);	\
	     &iter->list != (head);					\
	     iter = tmp, tmp = __af4list_valid(iter->list.next, head))

int netlbl_af4list_add(struct netlbl_af4list *entry,
		       struct list_head *head);
struct netlbl_af4list *netlbl_af4list_remove(__be32 addr, __be32 mask,
					     struct list_head *head);
void netlbl_af4list_remove_entry(struct netlbl_af4list *entry);
struct netlbl_af4list *netlbl_af4list_search(__be32 addr,
					     struct list_head *head);
struct netlbl_af4list *netlbl_af4list_search_exact(__be32 addr,
						   __be32 mask,
						   struct list_head *head);

#ifdef CONFIG_AUDIT
void netlbl_af4list_audit_addr(struct audit_buffer *audit_buf,
			       int src, const char *dev,
			       __be32 addr, __be32 mask);
#else
static inline void netlbl_af4list_audit_addr(struct audit_buffer *audit_buf,
					     int src, const char *dev,
					     __be32 addr, __be32 mask)
{
}
#endif

#if IS_ENABLED(CONFIG_IPV6)

#define __af6list_entry(ptr) container_of(ptr, struct netlbl_af6list, list)

static inline struct netlbl_af6list *__af6list_valid(struct list_head *s,
						     struct list_head *h)
{
	struct list_head *i = s;
	struct netlbl_af6list *n = __af6list_entry(s);
	while (i != h && !n->valid) {
		i = i->next;
		n = __af6list_entry(i);
	}
	return n;
}

static inline struct netlbl_af6list *__af6list_valid_rcu(struct list_head *s,
							 struct list_head *h)
{
	struct list_head *i = s;
	struct netlbl_af6list *n = __af6list_entry(s);
	while (i != h && !n->valid) {
		i = rcu_dereference(list_next_rcu(i));
		n = __af6list_entry(i);
	}
	return n;
}

#define netlbl_af6list_foreach(iter, head)				\
	for (iter = __af6list_valid((head)->next, head);		\
	     &iter->list != (head);					\
	     iter = __af6list_valid(iter->list.next, head))

#define netlbl_af6list_foreach_rcu(iter, head)				\
	for (iter = __af6list_valid_rcu((head)->next, head);		\
	     &iter->list != (head);					\
	     iter = __af6list_valid_rcu(iter->list.next, head))

#define netlbl_af6list_foreach_safe(iter, tmp, head)			\
	for (iter = __af6list_valid((head)->next, head),		\
		     tmp = __af6list_valid(iter->list.next, head);	\
	     &iter->list != (head);					\
	     iter = tmp, tmp = __af6list_valid(iter->list.next, head))

int netlbl_af6list_add(struct netlbl_af6list *entry,
		       struct list_head *head);
struct netlbl_af6list *netlbl_af6list_remove(const struct in6_addr *addr,
					     const struct in6_addr *mask,
					     struct list_head *head);
void netlbl_af6list_remove_entry(struct netlbl_af6list *entry);
struct netlbl_af6list *netlbl_af6list_search(const struct in6_addr *addr,
					     struct list_head *head);
struct netlbl_af6list *netlbl_af6list_search_exact(const struct in6_addr *addr,
						   const struct in6_addr *mask,
						   struct list_head *head);

#ifdef CONFIG_AUDIT
void netlbl_af6list_audit_addr(struct audit_buffer *audit_buf,
			       int src,
			       const char *dev,
			       const struct in6_addr *addr,
			       const struct in6_addr *mask);
#else
static inline void netlbl_af6list_audit_addr(struct audit_buffer *audit_buf,
					     int src,
					     const char *dev,
					     const struct in6_addr *addr,
					     const struct in6_addr *mask)
{
}
#endif
#endif /* IPV6 */

#endif
