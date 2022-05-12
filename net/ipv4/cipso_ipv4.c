// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CIPSO - Commercial IP Security Option
 *
 * This is an implementation of the CIPSO 2.2 protocol as specified in
 * draft-ietf-cipso-ipsecurity-01.txt with additional tag types as found in
 * FIPS-188.  While CIPSO never became a full IETF RFC standard many vendors
 * have chosen to adopt the protocol and over the years it has become a
 * de-facto standard for labeled networking.
 *
 * The CIPSO draft specification can be found in the kernel's Documentation
 * directory as well as the following URL:
 *   https://tools.ietf.org/id/draft-ietf-cipso-ipsecurity-01.txt
 * The FIPS-188 specification can be found at the following URL:
 *   https://www.itl.nist.gov/fipspubs/fip188.htm
 *
 * Author: Paul Moore <paul.moore@hp.com>
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2006, 2008
 */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/jhash.h>
#include <linux/audit.h>
#include <linux/slab.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/tcp.h>
#include <net/netlabel.h>
#include <net/cipso_ipv4.h>
#include <linux/atomic.h>
#include <linux/bug.h>
#include <asm/unaligned.h>

/* List of available DOI definitions */
/* XXX - This currently assumes a minimal number of different DOIs in use,
 * if in practice there are a lot of different DOIs this list should
 * probably be turned into a hash table or something similar so we
 * can do quick lookups. */
static DEFINE_SPINLOCK(cipso_v4_doi_list_lock);
static LIST_HEAD(cipso_v4_doi_list);

/* Label mapping cache */
int cipso_v4_cache_enabled = 1;
int cipso_v4_cache_bucketsize = 10;
#define CIPSO_V4_CACHE_BUCKETBITS     7
#define CIPSO_V4_CACHE_BUCKETS        (1 << CIPSO_V4_CACHE_BUCKETBITS)
#define CIPSO_V4_CACHE_REORDERLIMIT   10
struct cipso_v4_map_cache_bkt {
	spinlock_t lock;
	u32 size;
	struct list_head list;
};

struct cipso_v4_map_cache_entry {
	u32 hash;
	unsigned char *key;
	size_t key_len;

	struct netlbl_lsm_cache *lsm_data;

	u32 activity;
	struct list_head list;
};

static struct cipso_v4_map_cache_bkt *cipso_v4_cache;

/* Restricted bitmap (tag #1) flags */
int cipso_v4_rbm_optfmt = 0;
int cipso_v4_rbm_strictvalid = 1;

/*
 * Protocol Constants
 */

/* Maximum size of the CIPSO IP option, derived from the fact that the maximum
 * IPv4 header size is 60 bytes and the base IPv4 header is 20 bytes long. */
#define CIPSO_V4_OPT_LEN_MAX          40

/* Length of the base CIPSO option, this includes the option type (1 byte), the
 * option length (1 byte), and the DOI (4 bytes). */
#define CIPSO_V4_HDR_LEN              6

/* Base length of the restrictive category bitmap tag (tag #1). */
#define CIPSO_V4_TAG_RBM_BLEN         4

/* Base length of the enumerated category tag (tag #2). */
#define CIPSO_V4_TAG_ENUM_BLEN        4

/* Base length of the ranged categories bitmap tag (tag #5). */
#define CIPSO_V4_TAG_RNG_BLEN         4
/* The maximum number of category ranges permitted in the ranged category tag
 * (tag #5).  You may note that the IETF draft states that the maximum number
 * of category ranges is 7, but if the low end of the last category range is
 * zero then it is possible to fit 8 category ranges because the zero should
 * be omitted. */
#define CIPSO_V4_TAG_RNG_CAT_MAX      8

/* Base length of the local tag (non-standard tag).
 *  Tag definition (may change between kernel versions)
 *
 * 0          8          16         24         32
 * +----------+----------+----------+----------+
 * | 10000000 | 00000110 | 32-bit secid value  |
 * +----------+----------+----------+----------+
 * | in (host byte order)|
 * +----------+----------+
 *
 */
#define CIPSO_V4_TAG_LOC_BLEN         6

/*
 * Helper Functions
 */

/**
 * cipso_v4_cache_entry_free - Frees a cache entry
 * @entry: the entry to free
 *
 * Description:
 * This function frees the memory associated with a cache entry including the
 * LSM cache data if there are no longer any users, i.e. reference count == 0.
 *
 */
static void cipso_v4_cache_entry_free(struct cipso_v4_map_cache_entry *entry)
{
	if (entry->lsm_data)
		netlbl_secattr_cache_free(entry->lsm_data);
	kfree(entry->key);
	kfree(entry);
}

/**
 * cipso_v4_map_cache_hash - Hashing function for the CIPSO cache
 * @key: the hash key
 * @key_len: the length of the key in bytes
 *
 * Description:
 * The CIPSO tag hashing function.  Returns a 32-bit hash value.
 *
 */
static u32 cipso_v4_map_cache_hash(const unsigned char *key, u32 key_len)
{
	return jhash(key, key_len, 0);
}

/*
 * Label Mapping Cache Functions
 */

/**
 * cipso_v4_cache_init - Initialize the CIPSO cache
 *
 * Description:
 * Initializes the CIPSO label mapping cache, this function should be called
 * before any of the other functions defined in this file.  Returns zero on
 * success, negative values on error.
 *
 */
static int __init cipso_v4_cache_init(void)
{
	u32 iter;

	cipso_v4_cache = kcalloc(CIPSO_V4_CACHE_BUCKETS,
				 sizeof(struct cipso_v4_map_cache_bkt),
				 GFP_KERNEL);
	if (!cipso_v4_cache)
		return -ENOMEM;

	for (iter = 0; iter < CIPSO_V4_CACHE_BUCKETS; iter++) {
		spin_lock_init(&cipso_v4_cache[iter].lock);
		cipso_v4_cache[iter].size = 0;
		INIT_LIST_HEAD(&cipso_v4_cache[iter].list);
	}

	return 0;
}

/**
 * cipso_v4_cache_invalidate - Invalidates the current CIPSO cache
 *
 * Description:
 * Invalidates and frees any entries in the CIPSO cache.  Returns zero on
 * success and negative values on failure.
 *
 */
void cipso_v4_cache_invalidate(void)
{
	struct cipso_v4_map_cache_entry *entry, *tmp_entry;
	u32 iter;

	for (iter = 0; iter < CIPSO_V4_CACHE_BUCKETS; iter++) {
		spin_lock_bh(&cipso_v4_cache[iter].lock);
		list_for_each_entry_safe(entry,
					 tmp_entry,
					 &cipso_v4_cache[iter].list, list) {
			list_del(&entry->list);
			cipso_v4_cache_entry_free(entry);
		}
		cipso_v4_cache[iter].size = 0;
		spin_unlock_bh(&cipso_v4_cache[iter].lock);
	}
}

/**
 * cipso_v4_cache_check - Check the CIPSO cache for a label mapping
 * @key: the buffer to check
 * @key_len: buffer length in bytes
 * @secattr: the security attribute struct to use
 *
 * Description:
 * This function checks the cache to see if a label mapping already exists for
 * the given key.  If there is a match then the cache is adjusted and the
 * @secattr struct is populated with the correct LSM security attributes.  The
 * cache is adjusted in the following manner if the entry is not already the
 * first in the cache bucket:
 *
 *  1. The cache entry's activity counter is incremented
 *  2. The previous (higher ranking) entry's activity counter is decremented
 *  3. If the difference between the two activity counters is geater than
 *     CIPSO_V4_CACHE_REORDERLIMIT the two entries are swapped
 *
 * Returns zero on success, -ENOENT for a cache miss, and other negative values
 * on error.
 *
 */
static int cipso_v4_cache_check(const unsigned char *key,
				u32 key_len,
				struct netlbl_lsm_secattr *secattr)
{
	u32 bkt;
	struct cipso_v4_map_cache_entry *entry;
	struct cipso_v4_map_cache_entry *prev_entry = NULL;
	u32 hash;

	if (!cipso_v4_cache_enabled)
		return -ENOENT;

	hash = cipso_v4_map_cache_hash(key, key_len);
	bkt = hash & (CIPSO_V4_CACHE_BUCKETS - 1);
	spin_lock_bh(&cipso_v4_cache[bkt].lock);
	list_for_each_entry(entry, &cipso_v4_cache[bkt].list, list) {
		if (entry->hash == hash &&
		    entry->key_len == key_len &&
		    memcmp(entry->key, key, key_len) == 0) {
			entry->activity += 1;
			refcount_inc(&entry->lsm_data->refcount);
			secattr->cache = entry->lsm_data;
			secattr->flags |= NETLBL_SECATTR_CACHE;
			secattr->type = NETLBL_NLTYPE_CIPSOV4;
			if (!prev_entry) {
				spin_unlock_bh(&cipso_v4_cache[bkt].lock);
				return 0;
			}

			if (prev_entry->activity > 0)
				prev_entry->activity -= 1;
			if (entry->activity > prev_entry->activity &&
			    entry->activity - prev_entry->activity >
			    CIPSO_V4_CACHE_REORDERLIMIT) {
				__list_del(entry->list.prev, entry->list.next);
				__list_add(&entry->list,
					   prev_entry->list.prev,
					   &prev_entry->list);
			}

			spin_unlock_bh(&cipso_v4_cache[bkt].lock);
			return 0;
		}
		prev_entry = entry;
	}
	spin_unlock_bh(&cipso_v4_cache[bkt].lock);

	return -ENOENT;
}

/**
 * cipso_v4_cache_add - Add an entry to the CIPSO cache
 * @cipso_ptr: pointer to CIPSO IP option
 * @secattr: the packet's security attributes
 *
 * Description:
 * Add a new entry into the CIPSO label mapping cache.  Add the new entry to
 * head of the cache bucket's list, if the cache bucket is out of room remove
 * the last entry in the list first.  It is important to note that there is
 * currently no checking for duplicate keys.  Returns zero on success,
 * negative values on failure.
 *
 */
int cipso_v4_cache_add(const unsigned char *cipso_ptr,
		       const struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -EPERM;
	u32 bkt;
	struct cipso_v4_map_cache_entry *entry = NULL;
	struct cipso_v4_map_cache_entry *old_entry = NULL;
	u32 cipso_ptr_len;

	if (!cipso_v4_cache_enabled || cipso_v4_cache_bucketsize <= 0)
		return 0;

	cipso_ptr_len = cipso_ptr[1];

	entry = kzalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry)
		return -ENOMEM;
	entry->key = kmemdup(cipso_ptr, cipso_ptr_len, GFP_ATOMIC);
	if (!entry->key) {
		ret_val = -ENOMEM;
		goto cache_add_failure;
	}
	entry->key_len = cipso_ptr_len;
	entry->hash = cipso_v4_map_cache_hash(cipso_ptr, cipso_ptr_len);
	refcount_inc(&secattr->cache->refcount);
	entry->lsm_data = secattr->cache;

	bkt = entry->hash & (CIPSO_V4_CACHE_BUCKETS - 1);
	spin_lock_bh(&cipso_v4_cache[bkt].lock);
	if (cipso_v4_cache[bkt].size < cipso_v4_cache_bucketsize) {
		list_add(&entry->list, &cipso_v4_cache[bkt].list);
		cipso_v4_cache[bkt].size += 1;
	} else {
		old_entry = list_entry(cipso_v4_cache[bkt].list.prev,
				       struct cipso_v4_map_cache_entry, list);
		list_del(&old_entry->list);
		list_add(&entry->list, &cipso_v4_cache[bkt].list);
		cipso_v4_cache_entry_free(old_entry);
	}
	spin_unlock_bh(&cipso_v4_cache[bkt].lock);

	return 0;

cache_add_failure:
	if (entry)
		cipso_v4_cache_entry_free(entry);
	return ret_val;
}

/*
 * DOI List Functions
 */

/**
 * cipso_v4_doi_search - Searches for a DOI definition
 * @doi: the DOI to search for
 *
 * Description:
 * Search the DOI definition list for a DOI definition with a DOI value that
 * matches @doi.  The caller is responsible for calling rcu_read_[un]lock().
 * Returns a pointer to the DOI definition on success and NULL on failure.
 */
static struct cipso_v4_doi *cipso_v4_doi_search(u32 doi)
{
	struct cipso_v4_doi *iter;

	list_for_each_entry_rcu(iter, &cipso_v4_doi_list, list)
		if (iter->doi == doi && refcount_read(&iter->refcount))
			return iter;
	return NULL;
}

/**
 * cipso_v4_doi_add - Add a new DOI to the CIPSO protocol engine
 * @doi_def: the DOI structure
 * @audit_info: NetLabel audit information
 *
 * Description:
 * The caller defines a new DOI for use by the CIPSO engine and calls this
 * function to add it to the list of acceptable domains.  The caller must
 * ensure that the mapping table specified in @doi_def->map meets all of the
 * requirements of the mapping type (see cipso_ipv4.h for details).  Returns
 * zero on success and non-zero on failure.
 *
 */
int cipso_v4_doi_add(struct cipso_v4_doi *doi_def,
		     struct netlbl_audit *audit_info)
{
	int ret_val = -EINVAL;
	u32 iter;
	u32 doi;
	u32 doi_type;
	struct audit_buffer *audit_buf;

	doi = doi_def->doi;
	doi_type = doi_def->type;

	if (doi_def->doi == CIPSO_V4_DOI_UNKNOWN)
		goto doi_add_return;
	for (iter = 0; iter < CIPSO_V4_TAG_MAXCNT; iter++) {
		switch (doi_def->tags[iter]) {
		case CIPSO_V4_TAG_RBITMAP:
			break;
		case CIPSO_V4_TAG_RANGE:
		case CIPSO_V4_TAG_ENUM:
			if (doi_def->type != CIPSO_V4_MAP_PASS)
				goto doi_add_return;
			break;
		case CIPSO_V4_TAG_LOCAL:
			if (doi_def->type != CIPSO_V4_MAP_LOCAL)
				goto doi_add_return;
			break;
		case CIPSO_V4_TAG_INVALID:
			if (iter == 0)
				goto doi_add_return;
			break;
		default:
			goto doi_add_return;
		}
	}

	refcount_set(&doi_def->refcount, 1);

	spin_lock(&cipso_v4_doi_list_lock);
	if (cipso_v4_doi_search(doi_def->doi)) {
		spin_unlock(&cipso_v4_doi_list_lock);
		ret_val = -EEXIST;
		goto doi_add_return;
	}
	list_add_tail_rcu(&doi_def->list, &cipso_v4_doi_list);
	spin_unlock(&cipso_v4_doi_list_lock);
	ret_val = 0;

doi_add_return:
	audit_buf = netlbl_audit_start(AUDIT_MAC_CIPSOV4_ADD, audit_info);
	if (audit_buf) {
		const char *type_str;
		switch (doi_type) {
		case CIPSO_V4_MAP_TRANS:
			type_str = "trans";
			break;
		case CIPSO_V4_MAP_PASS:
			type_str = "pass";
			break;
		case CIPSO_V4_MAP_LOCAL:
			type_str = "local";
			break;
		default:
			type_str = "(unknown)";
		}
		audit_log_format(audit_buf,
				 " cipso_doi=%u cipso_type=%s res=%u",
				 doi, type_str, ret_val == 0 ? 1 : 0);
		audit_log_end(audit_buf);
	}

	return ret_val;
}

/**
 * cipso_v4_doi_free - Frees a DOI definition
 * @doi_def: the DOI definition
 *
 * Description:
 * This function frees all of the memory associated with a DOI definition.
 *
 */
void cipso_v4_doi_free(struct cipso_v4_doi *doi_def)
{
	if (!doi_def)
		return;

	switch (doi_def->type) {
	case CIPSO_V4_MAP_TRANS:
		kfree(doi_def->map.std->lvl.cipso);
		kfree(doi_def->map.std->lvl.local);
		kfree(doi_def->map.std->cat.cipso);
		kfree(doi_def->map.std->cat.local);
		kfree(doi_def->map.std);
		break;
	}
	kfree(doi_def);
}

/**
 * cipso_v4_doi_free_rcu - Frees a DOI definition via the RCU pointer
 * @entry: the entry's RCU field
 *
 * Description:
 * This function is designed to be used as a callback to the call_rcu()
 * function so that the memory allocated to the DOI definition can be released
 * safely.
 *
 */
static void cipso_v4_doi_free_rcu(struct rcu_head *entry)
{
	struct cipso_v4_doi *doi_def;

	doi_def = container_of(entry, struct cipso_v4_doi, rcu);
	cipso_v4_doi_free(doi_def);
}

/**
 * cipso_v4_doi_remove - Remove an existing DOI from the CIPSO protocol engine
 * @doi: the DOI value
 * @audit_info: NetLabel audit information
 *
 * Description:
 * Removes a DOI definition from the CIPSO engine.  The NetLabel routines will
 * be called to release their own LSM domain mappings as well as our own
 * domain list.  Returns zero on success and negative values on failure.
 *
 */
int cipso_v4_doi_remove(u32 doi, struct netlbl_audit *audit_info)
{
	int ret_val;
	struct cipso_v4_doi *doi_def;
	struct audit_buffer *audit_buf;

	spin_lock(&cipso_v4_doi_list_lock);
	doi_def = cipso_v4_doi_search(doi);
	if (!doi_def) {
		spin_unlock(&cipso_v4_doi_list_lock);
		ret_val = -ENOENT;
		goto doi_remove_return;
	}
	list_del_rcu(&doi_def->list);
	spin_unlock(&cipso_v4_doi_list_lock);

	cipso_v4_doi_putdef(doi_def);
	ret_val = 0;

doi_remove_return:
	audit_buf = netlbl_audit_start(AUDIT_MAC_CIPSOV4_DEL, audit_info);
	if (audit_buf) {
		audit_log_format(audit_buf,
				 " cipso_doi=%u res=%u",
				 doi, ret_val == 0 ? 1 : 0);
		audit_log_end(audit_buf);
	}

	return ret_val;
}

/**
 * cipso_v4_doi_getdef - Returns a reference to a valid DOI definition
 * @doi: the DOI value
 *
 * Description:
 * Searches for a valid DOI definition and if one is found it is returned to
 * the caller.  Otherwise NULL is returned.  The caller must ensure that
 * rcu_read_lock() is held while accessing the returned definition and the DOI
 * definition reference count is decremented when the caller is done.
 *
 */
struct cipso_v4_doi *cipso_v4_doi_getdef(u32 doi)
{
	struct cipso_v4_doi *doi_def;

	rcu_read_lock();
	doi_def = cipso_v4_doi_search(doi);
	if (!doi_def)
		goto doi_getdef_return;
	if (!refcount_inc_not_zero(&doi_def->refcount))
		doi_def = NULL;

doi_getdef_return:
	rcu_read_unlock();
	return doi_def;
}

/**
 * cipso_v4_doi_putdef - Releases a reference for the given DOI definition
 * @doi_def: the DOI definition
 *
 * Description:
 * Releases a DOI definition reference obtained from cipso_v4_doi_getdef().
 *
 */
void cipso_v4_doi_putdef(struct cipso_v4_doi *doi_def)
{
	if (!doi_def)
		return;

	if (!refcount_dec_and_test(&doi_def->refcount))
		return;

	cipso_v4_cache_invalidate();
	call_rcu(&doi_def->rcu, cipso_v4_doi_free_rcu);
}

/**
 * cipso_v4_doi_walk - Iterate through the DOI definitions
 * @skip_cnt: skip past this number of DOI definitions, updated
 * @callback: callback for each DOI definition
 * @cb_arg: argument for the callback function
 *
 * Description:
 * Iterate over the DOI definition list, skipping the first @skip_cnt entries.
 * For each entry call @callback, if @callback returns a negative value stop
 * 'walking' through the list and return.  Updates the value in @skip_cnt upon
 * return.  Returns zero on success, negative values on failure.
 *
 */
int cipso_v4_doi_walk(u32 *skip_cnt,
		     int (*callback) (struct cipso_v4_doi *doi_def, void *arg),
		     void *cb_arg)
{
	int ret_val = -ENOENT;
	u32 doi_cnt = 0;
	struct cipso_v4_doi *iter_doi;

	rcu_read_lock();
	list_for_each_entry_rcu(iter_doi, &cipso_v4_doi_list, list)
		if (refcount_read(&iter_doi->refcount) > 0) {
			if (doi_cnt++ < *skip_cnt)
				continue;
			ret_val = callback(iter_doi, cb_arg);
			if (ret_val < 0) {
				doi_cnt--;
				goto doi_walk_return;
			}
		}

doi_walk_return:
	rcu_read_unlock();
	*skip_cnt = doi_cnt;
	return ret_val;
}

/*
 * Label Mapping Functions
 */

/**
 * cipso_v4_map_lvl_valid - Checks to see if the given level is understood
 * @doi_def: the DOI definition
 * @level: the level to check
 *
 * Description:
 * Checks the given level against the given DOI definition and returns a
 * negative value if the level does not have a valid mapping and a zero value
 * if the level is defined by the DOI.
 *
 */
static int cipso_v4_map_lvl_valid(const struct cipso_v4_doi *doi_def, u8 level)
{
	switch (doi_def->type) {
	case CIPSO_V4_MAP_PASS:
		return 0;
	case CIPSO_V4_MAP_TRANS:
		if ((level < doi_def->map.std->lvl.cipso_size) &&
		    (doi_def->map.std->lvl.cipso[level] < CIPSO_V4_INV_LVL))
			return 0;
		break;
	}

	return -EFAULT;
}

/**
 * cipso_v4_map_lvl_hton - Perform a level mapping from the host to the network
 * @doi_def: the DOI definition
 * @host_lvl: the host MLS level
 * @net_lvl: the network/CIPSO MLS level
 *
 * Description:
 * Perform a label mapping to translate a local MLS level to the correct
 * CIPSO level using the given DOI definition.  Returns zero on success,
 * negative values otherwise.
 *
 */
static int cipso_v4_map_lvl_hton(const struct cipso_v4_doi *doi_def,
				 u32 host_lvl,
				 u32 *net_lvl)
{
	switch (doi_def->type) {
	case CIPSO_V4_MAP_PASS:
		*net_lvl = host_lvl;
		return 0;
	case CIPSO_V4_MAP_TRANS:
		if (host_lvl < doi_def->map.std->lvl.local_size &&
		    doi_def->map.std->lvl.local[host_lvl] < CIPSO_V4_INV_LVL) {
			*net_lvl = doi_def->map.std->lvl.local[host_lvl];
			return 0;
		}
		return -EPERM;
	}

	return -EINVAL;
}

/**
 * cipso_v4_map_lvl_ntoh - Perform a level mapping from the network to the host
 * @doi_def: the DOI definition
 * @net_lvl: the network/CIPSO MLS level
 * @host_lvl: the host MLS level
 *
 * Description:
 * Perform a label mapping to translate a CIPSO level to the correct local MLS
 * level using the given DOI definition.  Returns zero on success, negative
 * values otherwise.
 *
 */
static int cipso_v4_map_lvl_ntoh(const struct cipso_v4_doi *doi_def,
				 u32 net_lvl,
				 u32 *host_lvl)
{
	struct cipso_v4_std_map_tbl *map_tbl;

	switch (doi_def->type) {
	case CIPSO_V4_MAP_PASS:
		*host_lvl = net_lvl;
		return 0;
	case CIPSO_V4_MAP_TRANS:
		map_tbl = doi_def->map.std;
		if (net_lvl < map_tbl->lvl.cipso_size &&
		    map_tbl->lvl.cipso[net_lvl] < CIPSO_V4_INV_LVL) {
			*host_lvl = doi_def->map.std->lvl.cipso[net_lvl];
			return 0;
		}
		return -EPERM;
	}

	return -EINVAL;
}

/**
 * cipso_v4_map_cat_rbm_valid - Checks to see if the category bitmap is valid
 * @doi_def: the DOI definition
 * @bitmap: category bitmap
 * @bitmap_len: bitmap length in bytes
 *
 * Description:
 * Checks the given category bitmap against the given DOI definition and
 * returns a negative value if any of the categories in the bitmap do not have
 * a valid mapping and a zero value if all of the categories are valid.
 *
 */
static int cipso_v4_map_cat_rbm_valid(const struct cipso_v4_doi *doi_def,
				      const unsigned char *bitmap,
				      u32 bitmap_len)
{
	int cat = -1;
	u32 bitmap_len_bits = bitmap_len * 8;
	u32 cipso_cat_size;
	u32 *cipso_array;

	switch (doi_def->type) {
	case CIPSO_V4_MAP_PASS:
		return 0;
	case CIPSO_V4_MAP_TRANS:
		cipso_cat_size = doi_def->map.std->cat.cipso_size;
		cipso_array = doi_def->map.std->cat.cipso;
		for (;;) {
			cat = netlbl_bitmap_walk(bitmap,
						 bitmap_len_bits,
						 cat + 1,
						 1);
			if (cat < 0)
				break;
			if (cat >= cipso_cat_size ||
			    cipso_array[cat] >= CIPSO_V4_INV_CAT)
				return -EFAULT;
		}

		if (cat == -1)
			return 0;
		break;
	}

	return -EFAULT;
}

/**
 * cipso_v4_map_cat_rbm_hton - Perform a category mapping from host to network
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @net_cat: the zero'd out category bitmap in network/CIPSO format
 * @net_cat_len: the length of the CIPSO bitmap in bytes
 *
 * Description:
 * Perform a label mapping to translate a local MLS category bitmap to the
 * correct CIPSO bitmap using the given DOI definition.  Returns the minimum
 * size in bytes of the network bitmap on success, negative values otherwise.
 *
 */
static int cipso_v4_map_cat_rbm_hton(const struct cipso_v4_doi *doi_def,
				     const struct netlbl_lsm_secattr *secattr,
				     unsigned char *net_cat,
				     u32 net_cat_len)
{
	int host_spot = -1;
	u32 net_spot = CIPSO_V4_INV_CAT;
	u32 net_spot_max = 0;
	u32 net_clen_bits = net_cat_len * 8;
	u32 host_cat_size = 0;
	u32 *host_cat_array = NULL;

	if (doi_def->type == CIPSO_V4_MAP_TRANS) {
		host_cat_size = doi_def->map.std->cat.local_size;
		host_cat_array = doi_def->map.std->cat.local;
	}

	for (;;) {
		host_spot = netlbl_catmap_walk(secattr->attr.mls.cat,
					       host_spot + 1);
		if (host_spot < 0)
			break;

		switch (doi_def->type) {
		case CIPSO_V4_MAP_PASS:
			net_spot = host_spot;
			break;
		case CIPSO_V4_MAP_TRANS:
			if (host_spot >= host_cat_size)
				return -EPERM;
			net_spot = host_cat_array[host_spot];
			if (net_spot >= CIPSO_V4_INV_CAT)
				return -EPERM;
			break;
		}
		if (net_spot >= net_clen_bits)
			return -ENOSPC;
		netlbl_bitmap_setbit(net_cat, net_spot, 1);

		if (net_spot > net_spot_max)
			net_spot_max = net_spot;
	}

	if (++net_spot_max % 8)
		return net_spot_max / 8 + 1;
	return net_spot_max / 8;
}

/**
 * cipso_v4_map_cat_rbm_ntoh - Perform a category mapping from network to host
 * @doi_def: the DOI definition
 * @net_cat: the category bitmap in network/CIPSO format
 * @net_cat_len: the length of the CIPSO bitmap in bytes
 * @secattr: the security attributes
 *
 * Description:
 * Perform a label mapping to translate a CIPSO bitmap to the correct local
 * MLS category bitmap using the given DOI definition.  Returns zero on
 * success, negative values on failure.
 *
 */
static int cipso_v4_map_cat_rbm_ntoh(const struct cipso_v4_doi *doi_def,
				     const unsigned char *net_cat,
				     u32 net_cat_len,
				     struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	int net_spot = -1;
	u32 host_spot = CIPSO_V4_INV_CAT;
	u32 net_clen_bits = net_cat_len * 8;
	u32 net_cat_size = 0;
	u32 *net_cat_array = NULL;

	if (doi_def->type == CIPSO_V4_MAP_TRANS) {
		net_cat_size = doi_def->map.std->cat.cipso_size;
		net_cat_array = doi_def->map.std->cat.cipso;
	}

	for (;;) {
		net_spot = netlbl_bitmap_walk(net_cat,
					      net_clen_bits,
					      net_spot + 1,
					      1);
		if (net_spot < 0) {
			if (net_spot == -2)
				return -EFAULT;
			return 0;
		}

		switch (doi_def->type) {
		case CIPSO_V4_MAP_PASS:
			host_spot = net_spot;
			break;
		case CIPSO_V4_MAP_TRANS:
			if (net_spot >= net_cat_size)
				return -EPERM;
			host_spot = net_cat_array[net_spot];
			if (host_spot >= CIPSO_V4_INV_CAT)
				return -EPERM;
			break;
		}
		ret_val = netlbl_catmap_setbit(&secattr->attr.mls.cat,
						       host_spot,
						       GFP_ATOMIC);
		if (ret_val != 0)
			return ret_val;
	}

	return -EINVAL;
}

/**
 * cipso_v4_map_cat_enum_valid - Checks to see if the categories are valid
 * @doi_def: the DOI definition
 * @enumcat: category list
 * @enumcat_len: length of the category list in bytes
 *
 * Description:
 * Checks the given categories against the given DOI definition and returns a
 * negative value if any of the categories do not have a valid mapping and a
 * zero value if all of the categories are valid.
 *
 */
static int cipso_v4_map_cat_enum_valid(const struct cipso_v4_doi *doi_def,
				       const unsigned char *enumcat,
				       u32 enumcat_len)
{
	u16 cat;
	int cat_prev = -1;
	u32 iter;

	if (doi_def->type != CIPSO_V4_MAP_PASS || enumcat_len & 0x01)
		return -EFAULT;

	for (iter = 0; iter < enumcat_len; iter += 2) {
		cat = get_unaligned_be16(&enumcat[iter]);
		if (cat <= cat_prev)
			return -EFAULT;
		cat_prev = cat;
	}

	return 0;
}

/**
 * cipso_v4_map_cat_enum_hton - Perform a category mapping from host to network
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @net_cat: the zero'd out category list in network/CIPSO format
 * @net_cat_len: the length of the CIPSO category list in bytes
 *
 * Description:
 * Perform a label mapping to translate a local MLS category bitmap to the
 * correct CIPSO category list using the given DOI definition.   Returns the
 * size in bytes of the network category bitmap on success, negative values
 * otherwise.
 *
 */
static int cipso_v4_map_cat_enum_hton(const struct cipso_v4_doi *doi_def,
				      const struct netlbl_lsm_secattr *secattr,
				      unsigned char *net_cat,
				      u32 net_cat_len)
{
	int cat = -1;
	u32 cat_iter = 0;

	for (;;) {
		cat = netlbl_catmap_walk(secattr->attr.mls.cat, cat + 1);
		if (cat < 0)
			break;
		if ((cat_iter + 2) > net_cat_len)
			return -ENOSPC;

		*((__be16 *)&net_cat[cat_iter]) = htons(cat);
		cat_iter += 2;
	}

	return cat_iter;
}

/**
 * cipso_v4_map_cat_enum_ntoh - Perform a category mapping from network to host
 * @doi_def: the DOI definition
 * @net_cat: the category list in network/CIPSO format
 * @net_cat_len: the length of the CIPSO bitmap in bytes
 * @secattr: the security attributes
 *
 * Description:
 * Perform a label mapping to translate a CIPSO category list to the correct
 * local MLS category bitmap using the given DOI definition.  Returns zero on
 * success, negative values on failure.
 *
 */
static int cipso_v4_map_cat_enum_ntoh(const struct cipso_v4_doi *doi_def,
				      const unsigned char *net_cat,
				      u32 net_cat_len,
				      struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	u32 iter;

	for (iter = 0; iter < net_cat_len; iter += 2) {
		ret_val = netlbl_catmap_setbit(&secattr->attr.mls.cat,
					     get_unaligned_be16(&net_cat[iter]),
					     GFP_ATOMIC);
		if (ret_val != 0)
			return ret_val;
	}

	return 0;
}

/**
 * cipso_v4_map_cat_rng_valid - Checks to see if the categories are valid
 * @doi_def: the DOI definition
 * @rngcat: category list
 * @rngcat_len: length of the category list in bytes
 *
 * Description:
 * Checks the given categories against the given DOI definition and returns a
 * negative value if any of the categories do not have a valid mapping and a
 * zero value if all of the categories are valid.
 *
 */
static int cipso_v4_map_cat_rng_valid(const struct cipso_v4_doi *doi_def,
				      const unsigned char *rngcat,
				      u32 rngcat_len)
{
	u16 cat_high;
	u16 cat_low;
	u32 cat_prev = CIPSO_V4_MAX_REM_CATS + 1;
	u32 iter;

	if (doi_def->type != CIPSO_V4_MAP_PASS || rngcat_len & 0x01)
		return -EFAULT;

	for (iter = 0; iter < rngcat_len; iter += 4) {
		cat_high = get_unaligned_be16(&rngcat[iter]);
		if ((iter + 4) <= rngcat_len)
			cat_low = get_unaligned_be16(&rngcat[iter + 2]);
		else
			cat_low = 0;

		if (cat_high > cat_prev)
			return -EFAULT;

		cat_prev = cat_low;
	}

	return 0;
}

/**
 * cipso_v4_map_cat_rng_hton - Perform a category mapping from host to network
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @net_cat: the zero'd out category list in network/CIPSO format
 * @net_cat_len: the length of the CIPSO category list in bytes
 *
 * Description:
 * Perform a label mapping to translate a local MLS category bitmap to the
 * correct CIPSO category list using the given DOI definition.   Returns the
 * size in bytes of the network category bitmap on success, negative values
 * otherwise.
 *
 */
static int cipso_v4_map_cat_rng_hton(const struct cipso_v4_doi *doi_def,
				     const struct netlbl_lsm_secattr *secattr,
				     unsigned char *net_cat,
				     u32 net_cat_len)
{
	int iter = -1;
	u16 array[CIPSO_V4_TAG_RNG_CAT_MAX * 2];
	u32 array_cnt = 0;
	u32 cat_size = 0;

	/* make sure we don't overflow the 'array[]' variable */
	if (net_cat_len >
	    (CIPSO_V4_OPT_LEN_MAX - CIPSO_V4_HDR_LEN - CIPSO_V4_TAG_RNG_BLEN))
		return -ENOSPC;

	for (;;) {
		iter = netlbl_catmap_walk(secattr->attr.mls.cat, iter + 1);
		if (iter < 0)
			break;
		cat_size += (iter == 0 ? 0 : sizeof(u16));
		if (cat_size > net_cat_len)
			return -ENOSPC;
		array[array_cnt++] = iter;

		iter = netlbl_catmap_walkrng(secattr->attr.mls.cat, iter);
		if (iter < 0)
			return -EFAULT;
		cat_size += sizeof(u16);
		if (cat_size > net_cat_len)
			return -ENOSPC;
		array[array_cnt++] = iter;
	}

	for (iter = 0; array_cnt > 0;) {
		*((__be16 *)&net_cat[iter]) = htons(array[--array_cnt]);
		iter += 2;
		array_cnt--;
		if (array[array_cnt] != 0) {
			*((__be16 *)&net_cat[iter]) = htons(array[array_cnt]);
			iter += 2;
		}
	}

	return cat_size;
}

/**
 * cipso_v4_map_cat_rng_ntoh - Perform a category mapping from network to host
 * @doi_def: the DOI definition
 * @net_cat: the category list in network/CIPSO format
 * @net_cat_len: the length of the CIPSO bitmap in bytes
 * @secattr: the security attributes
 *
 * Description:
 * Perform a label mapping to translate a CIPSO category list to the correct
 * local MLS category bitmap using the given DOI definition.  Returns zero on
 * success, negative values on failure.
 *
 */
static int cipso_v4_map_cat_rng_ntoh(const struct cipso_v4_doi *doi_def,
				     const unsigned char *net_cat,
				     u32 net_cat_len,
				     struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	u32 net_iter;
	u16 cat_low;
	u16 cat_high;

	for (net_iter = 0; net_iter < net_cat_len; net_iter += 4) {
		cat_high = get_unaligned_be16(&net_cat[net_iter]);
		if ((net_iter + 4) <= net_cat_len)
			cat_low = get_unaligned_be16(&net_cat[net_iter + 2]);
		else
			cat_low = 0;

		ret_val = netlbl_catmap_setrng(&secattr->attr.mls.cat,
					       cat_low,
					       cat_high,
					       GFP_ATOMIC);
		if (ret_val != 0)
			return ret_val;
	}

	return 0;
}

/*
 * Protocol Handling Functions
 */

/**
 * cipso_v4_gentag_hdr - Generate a CIPSO option header
 * @doi_def: the DOI definition
 * @len: the total tag length in bytes, not including this header
 * @buf: the CIPSO option buffer
 *
 * Description:
 * Write a CIPSO header into the beginning of @buffer.
 *
 */
static void cipso_v4_gentag_hdr(const struct cipso_v4_doi *doi_def,
				unsigned char *buf,
				u32 len)
{
	buf[0] = IPOPT_CIPSO;
	buf[1] = CIPSO_V4_HDR_LEN + len;
	put_unaligned_be32(doi_def->doi, &buf[2]);
}

/**
 * cipso_v4_gentag_rbm - Generate a CIPSO restricted bitmap tag (type #1)
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @buffer: the option buffer
 * @buffer_len: length of buffer in bytes
 *
 * Description:
 * Generate a CIPSO option using the restricted bitmap tag, tag type #1.  The
 * actual buffer length may be larger than the indicated size due to
 * translation between host and network category bitmaps.  Returns the size of
 * the tag on success, negative values on failure.
 *
 */
static int cipso_v4_gentag_rbm(const struct cipso_v4_doi *doi_def,
			       const struct netlbl_lsm_secattr *secattr,
			       unsigned char *buffer,
			       u32 buffer_len)
{
	int ret_val;
	u32 tag_len;
	u32 level;

	if ((secattr->flags & NETLBL_SECATTR_MLS_LVL) == 0)
		return -EPERM;

	ret_val = cipso_v4_map_lvl_hton(doi_def,
					secattr->attr.mls.lvl,
					&level);
	if (ret_val != 0)
		return ret_val;

	if (secattr->flags & NETLBL_SECATTR_MLS_CAT) {
		ret_val = cipso_v4_map_cat_rbm_hton(doi_def,
						    secattr,
						    &buffer[4],
						    buffer_len - 4);
		if (ret_val < 0)
			return ret_val;

		/* This will send packets using the "optimized" format when
		 * possible as specified in  section 3.4.2.6 of the
		 * CIPSO draft. */
		if (cipso_v4_rbm_optfmt && ret_val > 0 && ret_val <= 10)
			tag_len = 14;
		else
			tag_len = 4 + ret_val;
	} else
		tag_len = 4;

	buffer[0] = CIPSO_V4_TAG_RBITMAP;
	buffer[1] = tag_len;
	buffer[3] = level;

	return tag_len;
}

/**
 * cipso_v4_parsetag_rbm - Parse a CIPSO restricted bitmap tag
 * @doi_def: the DOI definition
 * @tag: the CIPSO tag
 * @secattr: the security attributes
 *
 * Description:
 * Parse a CIPSO restricted bitmap tag (tag type #1) and return the security
 * attributes in @secattr.  Return zero on success, negatives values on
 * failure.
 *
 */
static int cipso_v4_parsetag_rbm(const struct cipso_v4_doi *doi_def,
				 const unsigned char *tag,
				 struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	u8 tag_len = tag[1];
	u32 level;

	ret_val = cipso_v4_map_lvl_ntoh(doi_def, tag[3], &level);
	if (ret_val != 0)
		return ret_val;
	secattr->attr.mls.lvl = level;
	secattr->flags |= NETLBL_SECATTR_MLS_LVL;

	if (tag_len > 4) {
		ret_val = cipso_v4_map_cat_rbm_ntoh(doi_def,
						    &tag[4],
						    tag_len - 4,
						    secattr);
		if (ret_val != 0) {
			netlbl_catmap_free(secattr->attr.mls.cat);
			return ret_val;
		}

		if (secattr->attr.mls.cat)
			secattr->flags |= NETLBL_SECATTR_MLS_CAT;
	}

	return 0;
}

/**
 * cipso_v4_gentag_enum - Generate a CIPSO enumerated tag (type #2)
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @buffer: the option buffer
 * @buffer_len: length of buffer in bytes
 *
 * Description:
 * Generate a CIPSO option using the enumerated tag, tag type #2.  Returns the
 * size of the tag on success, negative values on failure.
 *
 */
static int cipso_v4_gentag_enum(const struct cipso_v4_doi *doi_def,
				const struct netlbl_lsm_secattr *secattr,
				unsigned char *buffer,
				u32 buffer_len)
{
	int ret_val;
	u32 tag_len;
	u32 level;

	if (!(secattr->flags & NETLBL_SECATTR_MLS_LVL))
		return -EPERM;

	ret_val = cipso_v4_map_lvl_hton(doi_def,
					secattr->attr.mls.lvl,
					&level);
	if (ret_val != 0)
		return ret_val;

	if (secattr->flags & NETLBL_SECATTR_MLS_CAT) {
		ret_val = cipso_v4_map_cat_enum_hton(doi_def,
						     secattr,
						     &buffer[4],
						     buffer_len - 4);
		if (ret_val < 0)
			return ret_val;

		tag_len = 4 + ret_val;
	} else
		tag_len = 4;

	buffer[0] = CIPSO_V4_TAG_ENUM;
	buffer[1] = tag_len;
	buffer[3] = level;

	return tag_len;
}

/**
 * cipso_v4_parsetag_enum - Parse a CIPSO enumerated tag
 * @doi_def: the DOI definition
 * @tag: the CIPSO tag
 * @secattr: the security attributes
 *
 * Description:
 * Parse a CIPSO enumerated tag (tag type #2) and return the security
 * attributes in @secattr.  Return zero on success, negatives values on
 * failure.
 *
 */
static int cipso_v4_parsetag_enum(const struct cipso_v4_doi *doi_def,
				  const unsigned char *tag,
				  struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	u8 tag_len = tag[1];
	u32 level;

	ret_val = cipso_v4_map_lvl_ntoh(doi_def, tag[3], &level);
	if (ret_val != 0)
		return ret_val;
	secattr->attr.mls.lvl = level;
	secattr->flags |= NETLBL_SECATTR_MLS_LVL;

	if (tag_len > 4) {
		ret_val = cipso_v4_map_cat_enum_ntoh(doi_def,
						     &tag[4],
						     tag_len - 4,
						     secattr);
		if (ret_val != 0) {
			netlbl_catmap_free(secattr->attr.mls.cat);
			return ret_val;
		}

		secattr->flags |= NETLBL_SECATTR_MLS_CAT;
	}

	return 0;
}

/**
 * cipso_v4_gentag_rng - Generate a CIPSO ranged tag (type #5)
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @buffer: the option buffer
 * @buffer_len: length of buffer in bytes
 *
 * Description:
 * Generate a CIPSO option using the ranged tag, tag type #5.  Returns the
 * size of the tag on success, negative values on failure.
 *
 */
static int cipso_v4_gentag_rng(const struct cipso_v4_doi *doi_def,
			       const struct netlbl_lsm_secattr *secattr,
			       unsigned char *buffer,
			       u32 buffer_len)
{
	int ret_val;
	u32 tag_len;
	u32 level;

	if (!(secattr->flags & NETLBL_SECATTR_MLS_LVL))
		return -EPERM;

	ret_val = cipso_v4_map_lvl_hton(doi_def,
					secattr->attr.mls.lvl,
					&level);
	if (ret_val != 0)
		return ret_val;

	if (secattr->flags & NETLBL_SECATTR_MLS_CAT) {
		ret_val = cipso_v4_map_cat_rng_hton(doi_def,
						    secattr,
						    &buffer[4],
						    buffer_len - 4);
		if (ret_val < 0)
			return ret_val;

		tag_len = 4 + ret_val;
	} else
		tag_len = 4;

	buffer[0] = CIPSO_V4_TAG_RANGE;
	buffer[1] = tag_len;
	buffer[3] = level;

	return tag_len;
}

/**
 * cipso_v4_parsetag_rng - Parse a CIPSO ranged tag
 * @doi_def: the DOI definition
 * @tag: the CIPSO tag
 * @secattr: the security attributes
 *
 * Description:
 * Parse a CIPSO ranged tag (tag type #5) and return the security attributes
 * in @secattr.  Return zero on success, negatives values on failure.
 *
 */
static int cipso_v4_parsetag_rng(const struct cipso_v4_doi *doi_def,
				 const unsigned char *tag,
				 struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	u8 tag_len = tag[1];
	u32 level;

	ret_val = cipso_v4_map_lvl_ntoh(doi_def, tag[3], &level);
	if (ret_val != 0)
		return ret_val;
	secattr->attr.mls.lvl = level;
	secattr->flags |= NETLBL_SECATTR_MLS_LVL;

	if (tag_len > 4) {
		ret_val = cipso_v4_map_cat_rng_ntoh(doi_def,
						    &tag[4],
						    tag_len - 4,
						    secattr);
		if (ret_val != 0) {
			netlbl_catmap_free(secattr->attr.mls.cat);
			return ret_val;
		}

		if (secattr->attr.mls.cat)
			secattr->flags |= NETLBL_SECATTR_MLS_CAT;
	}

	return 0;
}

/**
 * cipso_v4_gentag_loc - Generate a CIPSO local tag (non-standard)
 * @doi_def: the DOI definition
 * @secattr: the security attributes
 * @buffer: the option buffer
 * @buffer_len: length of buffer in bytes
 *
 * Description:
 * Generate a CIPSO option using the local tag.  Returns the size of the tag
 * on success, negative values on failure.
 *
 */
static int cipso_v4_gentag_loc(const struct cipso_v4_doi *doi_def,
			       const struct netlbl_lsm_secattr *secattr,
			       unsigned char *buffer,
			       u32 buffer_len)
{
	if (!(secattr->flags & NETLBL_SECATTR_SECID))
		return -EPERM;

	buffer[0] = CIPSO_V4_TAG_LOCAL;
	buffer[1] = CIPSO_V4_TAG_LOC_BLEN;
	*(u32 *)&buffer[2] = secattr->attr.secid;

	return CIPSO_V4_TAG_LOC_BLEN;
}

/**
 * cipso_v4_parsetag_loc - Parse a CIPSO local tag
 * @doi_def: the DOI definition
 * @tag: the CIPSO tag
 * @secattr: the security attributes
 *
 * Description:
 * Parse a CIPSO local tag and return the security attributes in @secattr.
 * Return zero on success, negatives values on failure.
 *
 */
static int cipso_v4_parsetag_loc(const struct cipso_v4_doi *doi_def,
				 const unsigned char *tag,
				 struct netlbl_lsm_secattr *secattr)
{
	secattr->attr.secid = *(u32 *)&tag[2];
	secattr->flags |= NETLBL_SECATTR_SECID;

	return 0;
}

/**
 * cipso_v4_optptr - Find the CIPSO option in the packet
 * @skb: the packet
 *
 * Description:
 * Parse the packet's IP header looking for a CIPSO option.  Returns a pointer
 * to the start of the CIPSO option on success, NULL if one is not found.
 *
 */
unsigned char *cipso_v4_optptr(const struct sk_buff *skb)
{
	const struct iphdr *iph = ip_hdr(skb);
	unsigned char *optptr = (unsigned char *)&(ip_hdr(skb)[1]);
	int optlen;
	int taglen;

	for (optlen = iph->ihl*4 - sizeof(struct iphdr); optlen > 1; ) {
		switch (optptr[0]) {
		case IPOPT_END:
			return NULL;
		case IPOPT_NOOP:
			taglen = 1;
			break;
		default:
			taglen = optptr[1];
		}
		if (!taglen || taglen > optlen)
			return NULL;
		if (optptr[0] == IPOPT_CIPSO)
			return optptr;

		optlen -= taglen;
		optptr += taglen;
	}

	return NULL;
}

/**
 * cipso_v4_validate - Validate a CIPSO option
 * @skb: the packet
 * @option: the start of the option, on error it is set to point to the error
 *
 * Description:
 * This routine is called to validate a CIPSO option, it checks all of the
 * fields to ensure that they are at least valid, see the draft snippet below
 * for details.  If the option is valid then a zero value is returned and
 * the value of @option is unchanged.  If the option is invalid then a
 * non-zero value is returned and @option is adjusted to point to the
 * offending portion of the option.  From the IETF draft ...
 *
 *  "If any field within the CIPSO options, such as the DOI identifier, is not
 *   recognized the IP datagram is discarded and an ICMP 'parameter problem'
 *   (type 12) is generated and returned.  The ICMP code field is set to 'bad
 *   parameter' (code 0) and the pointer is set to the start of the CIPSO field
 *   that is unrecognized."
 *
 */
int cipso_v4_validate(const struct sk_buff *skb, unsigned char **option)
{
	unsigned char *opt = *option;
	unsigned char *tag;
	unsigned char opt_iter;
	unsigned char err_offset = 0;
	u8 opt_len;
	u8 tag_len;
	struct cipso_v4_doi *doi_def = NULL;
	u32 tag_iter;

	/* caller already checks for length values that are too large */
	opt_len = opt[1];
	if (opt_len < 8) {
		err_offset = 1;
		goto validate_return;
	}

	rcu_read_lock();
	doi_def = cipso_v4_doi_search(get_unaligned_be32(&opt[2]));
	if (!doi_def) {
		err_offset = 2;
		goto validate_return_locked;
	}

	opt_iter = CIPSO_V4_HDR_LEN;
	tag = opt + opt_iter;
	while (opt_iter < opt_len) {
		for (tag_iter = 0; doi_def->tags[tag_iter] != tag[0];)
			if (doi_def->tags[tag_iter] == CIPSO_V4_TAG_INVALID ||
			    ++tag_iter == CIPSO_V4_TAG_MAXCNT) {
				err_offset = opt_iter;
				goto validate_return_locked;
			}

		if (opt_iter + 1 == opt_len) {
			err_offset = opt_iter;
			goto validate_return_locked;
		}
		tag_len = tag[1];
		if (tag_len > (opt_len - opt_iter)) {
			err_offset = opt_iter + 1;
			goto validate_return_locked;
		}

		switch (tag[0]) {
		case CIPSO_V4_TAG_RBITMAP:
			if (tag_len < CIPSO_V4_TAG_RBM_BLEN) {
				err_offset = opt_iter + 1;
				goto validate_return_locked;
			}

			/* We are already going to do all the verification
			 * necessary at the socket layer so from our point of
			 * view it is safe to turn these checks off (and less
			 * work), however, the CIPSO draft says we should do
			 * all the CIPSO validations here but it doesn't
			 * really specify _exactly_ what we need to validate
			 * ... so, just make it a sysctl tunable. */
			if (cipso_v4_rbm_strictvalid) {
				if (cipso_v4_map_lvl_valid(doi_def,
							   tag[3]) < 0) {
					err_offset = opt_iter + 3;
					goto validate_return_locked;
				}
				if (tag_len > CIPSO_V4_TAG_RBM_BLEN &&
				    cipso_v4_map_cat_rbm_valid(doi_def,
							    &tag[4],
							    tag_len - 4) < 0) {
					err_offset = opt_iter + 4;
					goto validate_return_locked;
				}
			}
			break;
		case CIPSO_V4_TAG_ENUM:
			if (tag_len < CIPSO_V4_TAG_ENUM_BLEN) {
				err_offset = opt_iter + 1;
				goto validate_return_locked;
			}

			if (cipso_v4_map_lvl_valid(doi_def,
						   tag[3]) < 0) {
				err_offset = opt_iter + 3;
				goto validate_return_locked;
			}
			if (tag_len > CIPSO_V4_TAG_ENUM_BLEN &&
			    cipso_v4_map_cat_enum_valid(doi_def,
							&tag[4],
							tag_len - 4) < 0) {
				err_offset = opt_iter + 4;
				goto validate_return_locked;
			}
			break;
		case CIPSO_V4_TAG_RANGE:
			if (tag_len < CIPSO_V4_TAG_RNG_BLEN) {
				err_offset = opt_iter + 1;
				goto validate_return_locked;
			}

			if (cipso_v4_map_lvl_valid(doi_def,
						   tag[3]) < 0) {
				err_offset = opt_iter + 3;
				goto validate_return_locked;
			}
			if (tag_len > CIPSO_V4_TAG_RNG_BLEN &&
			    cipso_v4_map_cat_rng_valid(doi_def,
						       &tag[4],
						       tag_len - 4) < 0) {
				err_offset = opt_iter + 4;
				goto validate_return_locked;
			}
			break;
		case CIPSO_V4_TAG_LOCAL:
			/* This is a non-standard tag that we only allow for
			 * local connections, so if the incoming interface is
			 * not the loopback device drop the packet. Further,
			 * there is no legitimate reason for setting this from
			 * userspace so reject it if skb is NULL. */
			if (!skb || !(skb->dev->flags & IFF_LOOPBACK)) {
				err_offset = opt_iter;
				goto validate_return_locked;
			}
			if (tag_len != CIPSO_V4_TAG_LOC_BLEN) {
				err_offset = opt_iter + 1;
				goto validate_return_locked;
			}
			break;
		default:
			err_offset = opt_iter;
			goto validate_return_locked;
		}

		tag += tag_len;
		opt_iter += tag_len;
	}

validate_return_locked:
	rcu_read_unlock();
validate_return:
	*option = opt + err_offset;
	return err_offset;
}

/**
 * cipso_v4_error - Send the correct response for a bad packet
 * @skb: the packet
 * @error: the error code
 * @gateway: CIPSO gateway flag
 *
 * Description:
 * Based on the error code given in @error, send an ICMP error message back to
 * the originating host.  From the IETF draft ...
 *
 *  "If the contents of the CIPSO [option] are valid but the security label is
 *   outside of the configured host or port label range, the datagram is
 *   discarded and an ICMP 'destination unreachable' (type 3) is generated and
 *   returned.  The code field of the ICMP is set to 'communication with
 *   destination network administratively prohibited' (code 9) or to
 *   'communication with destination host administratively prohibited'
 *   (code 10).  The value of the code is dependent on whether the originator
 *   of the ICMP message is acting as a CIPSO host or a CIPSO gateway.  The
 *   recipient of the ICMP message MUST be able to handle either value.  The
 *   same procedure is performed if a CIPSO [option] can not be added to an
 *   IP packet because it is too large to fit in the IP options area."
 *
 *  "If the error is triggered by receipt of an ICMP message, the message is
 *   discarded and no response is permitted (consistent with general ICMP
 *   processing rules)."
 *
 */
void cipso_v4_error(struct sk_buff *skb, int error, u32 gateway)
{
	unsigned char optbuf[sizeof(struct ip_options) + 40];
	struct ip_options *opt = (struct ip_options *)optbuf;
	int res;

	if (ip_hdr(skb)->protocol == IPPROTO_ICMP || error != -EACCES)
		return;

	/*
	 * We might be called above the IP layer,
	 * so we can not use icmp_send and IPCB here.
	 */

	memset(opt, 0, sizeof(struct ip_options));
	opt->optlen = ip_hdr(skb)->ihl*4 - sizeof(struct iphdr);
	rcu_read_lock();
	res = __ip_options_compile(dev_net(skb->dev), opt, skb, NULL);
	rcu_read_unlock();

	if (res)
		return;

	if (gateway)
		__icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_ANO, 0, opt);
	else
		__icmp_send(skb, ICMP_DEST_UNREACH, ICMP_HOST_ANO, 0, opt);
}

/**
 * cipso_v4_genopt - Generate a CIPSO option
 * @buf: the option buffer
 * @buf_len: the size of opt_buf
 * @doi_def: the CIPSO DOI to use
 * @secattr: the security attributes
 *
 * Description:
 * Generate a CIPSO option using the DOI definition and security attributes
 * passed to the function.  Returns the length of the option on success and
 * negative values on failure.
 *
 */
static int cipso_v4_genopt(unsigned char *buf, u32 buf_len,
			   const struct cipso_v4_doi *doi_def,
			   const struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	u32 iter;

	if (buf_len <= CIPSO_V4_HDR_LEN)
		return -ENOSPC;

	/* XXX - This code assumes only one tag per CIPSO option which isn't
	 * really a good assumption to make but since we only support the MAC
	 * tags right now it is a safe assumption. */
	iter = 0;
	do {
		memset(buf, 0, buf_len);
		switch (doi_def->tags[iter]) {
		case CIPSO_V4_TAG_RBITMAP:
			ret_val = cipso_v4_gentag_rbm(doi_def,
						   secattr,
						   &buf[CIPSO_V4_HDR_LEN],
						   buf_len - CIPSO_V4_HDR_LEN);
			break;
		case CIPSO_V4_TAG_ENUM:
			ret_val = cipso_v4_gentag_enum(doi_def,
						   secattr,
						   &buf[CIPSO_V4_HDR_LEN],
						   buf_len - CIPSO_V4_HDR_LEN);
			break;
		case CIPSO_V4_TAG_RANGE:
			ret_val = cipso_v4_gentag_rng(doi_def,
						   secattr,
						   &buf[CIPSO_V4_HDR_LEN],
						   buf_len - CIPSO_V4_HDR_LEN);
			break;
		case CIPSO_V4_TAG_LOCAL:
			ret_val = cipso_v4_gentag_loc(doi_def,
						   secattr,
						   &buf[CIPSO_V4_HDR_LEN],
						   buf_len - CIPSO_V4_HDR_LEN);
			break;
		default:
			return -EPERM;
		}

		iter++;
	} while (ret_val < 0 &&
		 iter < CIPSO_V4_TAG_MAXCNT &&
		 doi_def->tags[iter] != CIPSO_V4_TAG_INVALID);
	if (ret_val < 0)
		return ret_val;
	cipso_v4_gentag_hdr(doi_def, buf, ret_val);
	return CIPSO_V4_HDR_LEN + ret_val;
}

/**
 * cipso_v4_sock_setattr - Add a CIPSO option to a socket
 * @sk: the socket
 * @doi_def: the CIPSO DOI to use
 * @secattr: the specific security attributes of the socket
 *
 * Description:
 * Set the CIPSO option on the given socket using the DOI definition and
 * security attributes passed to the function.  This function requires
 * exclusive access to @sk, which means it either needs to be in the
 * process of being created or locked.  Returns zero on success and negative
 * values on failure.
 *
 */
int cipso_v4_sock_setattr(struct sock *sk,
			  const struct cipso_v4_doi *doi_def,
			  const struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -EPERM;
	unsigned char *buf = NULL;
	u32 buf_len;
	u32 opt_len;
	struct ip_options_rcu *old, *opt = NULL;
	struct inet_sock *sk_inet;
	struct inet_connection_sock *sk_conn;

	/* In the case of sock_create_lite(), the sock->sk field is not
	 * defined yet but it is not a problem as the only users of these
	 * "lite" PF_INET sockets are functions which do an accept() call
	 * afterwards so we will label the socket as part of the accept(). */
	if (!sk)
		return 0;

	/* We allocate the maximum CIPSO option size here so we are probably
	 * being a little wasteful, but it makes our life _much_ easier later
	 * on and after all we are only talking about 40 bytes. */
	buf_len = CIPSO_V4_OPT_LEN_MAX;
	buf = kmalloc(buf_len, GFP_ATOMIC);
	if (!buf) {
		ret_val = -ENOMEM;
		goto socket_setattr_failure;
	}

	ret_val = cipso_v4_genopt(buf, buf_len, doi_def, secattr);
	if (ret_val < 0)
		goto socket_setattr_failure;
	buf_len = ret_val;

	/* We can't use ip_options_get() directly because it makes a call to
	 * ip_options_get_alloc() which allocates memory with GFP_KERNEL and
	 * we won't always have CAP_NET_RAW even though we _always_ want to
	 * set the IPOPT_CIPSO option. */
	opt_len = (buf_len + 3) & ~3;
	opt = kzalloc(sizeof(*opt) + opt_len, GFP_ATOMIC);
	if (!opt) {
		ret_val = -ENOMEM;
		goto socket_setattr_failure;
	}
	memcpy(opt->opt.__data, buf, buf_len);
	opt->opt.optlen = opt_len;
	opt->opt.cipso = sizeof(struct iphdr);
	kfree(buf);
	buf = NULL;

	sk_inet = inet_sk(sk);

	old = rcu_dereference_protected(sk_inet->inet_opt,
					lockdep_sock_is_held(sk));
	if (sk_inet->is_icsk) {
		sk_conn = inet_csk(sk);
		if (old)
			sk_conn->icsk_ext_hdr_len -= old->opt.optlen;
		sk_conn->icsk_ext_hdr_len += opt->opt.optlen;
		sk_conn->icsk_sync_mss(sk, sk_conn->icsk_pmtu_cookie);
	}
	rcu_assign_pointer(sk_inet->inet_opt, opt);
	if (old)
		kfree_rcu(old, rcu);

	return 0;

socket_setattr_failure:
	kfree(buf);
	kfree(opt);
	return ret_val;
}

/**
 * cipso_v4_req_setattr - Add a CIPSO option to a connection request socket
 * @req: the connection request socket
 * @doi_def: the CIPSO DOI to use
 * @secattr: the specific security attributes of the socket
 *
 * Description:
 * Set the CIPSO option on the given socket using the DOI definition and
 * security attributes passed to the function.  Returns zero on success and
 * negative values on failure.
 *
 */
int cipso_v4_req_setattr(struct request_sock *req,
			 const struct cipso_v4_doi *doi_def,
			 const struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -EPERM;
	unsigned char *buf = NULL;
	u32 buf_len;
	u32 opt_len;
	struct ip_options_rcu *opt = NULL;
	struct inet_request_sock *req_inet;

	/* We allocate the maximum CIPSO option size here so we are probably
	 * being a little wasteful, but it makes our life _much_ easier later
	 * on and after all we are only talking about 40 bytes. */
	buf_len = CIPSO_V4_OPT_LEN_MAX;
	buf = kmalloc(buf_len, GFP_ATOMIC);
	if (!buf) {
		ret_val = -ENOMEM;
		goto req_setattr_failure;
	}

	ret_val = cipso_v4_genopt(buf, buf_len, doi_def, secattr);
	if (ret_val < 0)
		goto req_setattr_failure;
	buf_len = ret_val;

	/* We can't use ip_options_get() directly because it makes a call to
	 * ip_options_get_alloc() which allocates memory with GFP_KERNEL and
	 * we won't always have CAP_NET_RAW even though we _always_ want to
	 * set the IPOPT_CIPSO option. */
	opt_len = (buf_len + 3) & ~3;
	opt = kzalloc(sizeof(*opt) + opt_len, GFP_ATOMIC);
	if (!opt) {
		ret_val = -ENOMEM;
		goto req_setattr_failure;
	}
	memcpy(opt->opt.__data, buf, buf_len);
	opt->opt.optlen = opt_len;
	opt->opt.cipso = sizeof(struct iphdr);
	kfree(buf);
	buf = NULL;

	req_inet = inet_rsk(req);
	opt = xchg((__force struct ip_options_rcu **)&req_inet->ireq_opt, opt);
	if (opt)
		kfree_rcu(opt, rcu);

	return 0;

req_setattr_failure:
	kfree(buf);
	kfree(opt);
	return ret_val;
}

/**
 * cipso_v4_delopt - Delete the CIPSO option from a set of IP options
 * @opt_ptr: IP option pointer
 *
 * Description:
 * Deletes the CIPSO IP option from a set of IP options and makes the necessary
 * adjustments to the IP option structure.  Returns zero on success, negative
 * values on failure.
 *
 */
static int cipso_v4_delopt(struct ip_options_rcu __rcu **opt_ptr)
{
	struct ip_options_rcu *opt = rcu_dereference_protected(*opt_ptr, 1);
	int hdr_delta = 0;

	if (!opt || opt->opt.cipso == 0)
		return 0;
	if (opt->opt.srr || opt->opt.rr || opt->opt.ts || opt->opt.router_alert) {
		u8 cipso_len;
		u8 cipso_off;
		unsigned char *cipso_ptr;
		int iter;
		int optlen_new;

		cipso_off = opt->opt.cipso - sizeof(struct iphdr);
		cipso_ptr = &opt->opt.__data[cipso_off];
		cipso_len = cipso_ptr[1];

		if (opt->opt.srr > opt->opt.cipso)
			opt->opt.srr -= cipso_len;
		if (opt->opt.rr > opt->opt.cipso)
			opt->opt.rr -= cipso_len;
		if (opt->opt.ts > opt->opt.cipso)
			opt->opt.ts -= cipso_len;
		if (opt->opt.router_alert > opt->opt.cipso)
			opt->opt.router_alert -= cipso_len;
		opt->opt.cipso = 0;

		memmove(cipso_ptr, cipso_ptr + cipso_len,
			opt->opt.optlen - cipso_off - cipso_len);

		/* determining the new total option length is tricky because of
		 * the padding necessary, the only thing i can think to do at
		 * this point is walk the options one-by-one, skipping the
		 * padding at the end to determine the actual option size and
		 * from there we can determine the new total option length */
		iter = 0;
		optlen_new = 0;
		while (iter < opt->opt.optlen)
			if (opt->opt.__data[iter] != IPOPT_NOP) {
				iter += opt->opt.__data[iter + 1];
				optlen_new = iter;
			} else
				iter++;
		hdr_delta = opt->opt.optlen;
		opt->opt.optlen = (optlen_new + 3) & ~3;
		hdr_delta -= opt->opt.optlen;
	} else {
		/* only the cipso option was present on the socket so we can
		 * remove the entire option struct */
		*opt_ptr = NULL;
		hdr_delta = opt->opt.optlen;
		kfree_rcu(opt, rcu);
	}

	return hdr_delta;
}

/**
 * cipso_v4_sock_delattr - Delete the CIPSO option from a socket
 * @sk: the socket
 *
 * Description:
 * Removes the CIPSO option from a socket, if present.
 *
 */
void cipso_v4_sock_delattr(struct sock *sk)
{
	struct inet_sock *sk_inet;
	int hdr_delta;

	sk_inet = inet_sk(sk);

	hdr_delta = cipso_v4_delopt(&sk_inet->inet_opt);
	if (sk_inet->is_icsk && hdr_delta > 0) {
		struct inet_connection_sock *sk_conn = inet_csk(sk);
		sk_conn->icsk_ext_hdr_len -= hdr_delta;
		sk_conn->icsk_sync_mss(sk, sk_conn->icsk_pmtu_cookie);
	}
}

/**
 * cipso_v4_req_delattr - Delete the CIPSO option from a request socket
 * @req: the request socket
 *
 * Description:
 * Removes the CIPSO option from a request socket, if present.
 *
 */
void cipso_v4_req_delattr(struct request_sock *req)
{
	cipso_v4_delopt(&inet_rsk(req)->ireq_opt);
}

/**
 * cipso_v4_getattr - Helper function for the cipso_v4_*_getattr functions
 * @cipso: the CIPSO v4 option
 * @secattr: the security attributes
 *
 * Description:
 * Inspect @cipso and return the security attributes in @secattr.  Returns zero
 * on success and negative values on failure.
 *
 */
int cipso_v4_getattr(const unsigned char *cipso,
		     struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -ENOMSG;
	u32 doi;
	struct cipso_v4_doi *doi_def;

	if (cipso_v4_cache_check(cipso, cipso[1], secattr) == 0)
		return 0;

	doi = get_unaligned_be32(&cipso[2]);
	rcu_read_lock();
	doi_def = cipso_v4_doi_search(doi);
	if (!doi_def)
		goto getattr_return;
	/* XXX - This code assumes only one tag per CIPSO option which isn't
	 * really a good assumption to make but since we only support the MAC
	 * tags right now it is a safe assumption. */
	switch (cipso[6]) {
	case CIPSO_V4_TAG_RBITMAP:
		ret_val = cipso_v4_parsetag_rbm(doi_def, &cipso[6], secattr);
		break;
	case CIPSO_V4_TAG_ENUM:
		ret_val = cipso_v4_parsetag_enum(doi_def, &cipso[6], secattr);
		break;
	case CIPSO_V4_TAG_RANGE:
		ret_val = cipso_v4_parsetag_rng(doi_def, &cipso[6], secattr);
		break;
	case CIPSO_V4_TAG_LOCAL:
		ret_val = cipso_v4_parsetag_loc(doi_def, &cipso[6], secattr);
		break;
	}
	if (ret_val == 0)
		secattr->type = NETLBL_NLTYPE_CIPSOV4;

getattr_return:
	rcu_read_unlock();
	return ret_val;
}

/**
 * cipso_v4_sock_getattr - Get the security attributes from a sock
 * @sk: the sock
 * @secattr: the security attributes
 *
 * Description:
 * Query @sk to see if there is a CIPSO option attached to the sock and if
 * there is return the CIPSO security attributes in @secattr.  This function
 * requires that @sk be locked, or privately held, but it does not do any
 * locking itself.  Returns zero on success and negative values on failure.
 *
 */
int cipso_v4_sock_getattr(struct sock *sk, struct netlbl_lsm_secattr *secattr)
{
	struct ip_options_rcu *opt;
	int res = -ENOMSG;

	rcu_read_lock();
	opt = rcu_dereference(inet_sk(sk)->inet_opt);
	if (opt && opt->opt.cipso)
		res = cipso_v4_getattr(opt->opt.__data +
						opt->opt.cipso -
						sizeof(struct iphdr),
				       secattr);
	rcu_read_unlock();
	return res;
}

/**
 * cipso_v4_skbuff_setattr - Set the CIPSO option on a packet
 * @skb: the packet
 * @doi_def: the DOI structure
 * @secattr: the security attributes
 *
 * Description:
 * Set the CIPSO option on the given packet based on the security attributes.
 * Returns a pointer to the IP header on success and NULL on failure.
 *
 */
int cipso_v4_skbuff_setattr(struct sk_buff *skb,
			    const struct cipso_v4_doi *doi_def,
			    const struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	struct iphdr *iph;
	struct ip_options *opt = &IPCB(skb)->opt;
	unsigned char buf[CIPSO_V4_OPT_LEN_MAX];
	u32 buf_len = CIPSO_V4_OPT_LEN_MAX;
	u32 opt_len;
	int len_delta;

	ret_val = cipso_v4_genopt(buf, buf_len, doi_def, secattr);
	if (ret_val < 0)
		return ret_val;
	buf_len = ret_val;
	opt_len = (buf_len + 3) & ~3;

	/* we overwrite any existing options to ensure that we have enough
	 * room for the CIPSO option, the reason is that we _need_ to guarantee
	 * that the security label is applied to the packet - we do the same
	 * thing when using the socket options and it hasn't caused a problem,
	 * if we need to we can always revisit this choice later */

	len_delta = opt_len - opt->optlen;
	/* if we don't ensure enough headroom we could panic on the skb_push()
	 * call below so make sure we have enough, we are also "mangling" the
	 * packet so we should probably do a copy-on-write call anyway */
	ret_val = skb_cow(skb, skb_headroom(skb) + len_delta);
	if (ret_val < 0)
		return ret_val;

	if (len_delta > 0) {
		/* we assume that the header + opt->optlen have already been
		 * "pushed" in ip_options_build() or similar */
		iph = ip_hdr(skb);
		skb_push(skb, len_delta);
		memmove((char *)iph - len_delta, iph, iph->ihl << 2);
		skb_reset_network_header(skb);
		iph = ip_hdr(skb);
	} else if (len_delta < 0) {
		iph = ip_hdr(skb);
		memset(iph + 1, IPOPT_NOP, opt->optlen);
	} else
		iph = ip_hdr(skb);

	if (opt->optlen > 0)
		memset(opt, 0, sizeof(*opt));
	opt->optlen = opt_len;
	opt->cipso = sizeof(struct iphdr);
	opt->is_changed = 1;

	/* we have to do the following because we are being called from a
	 * netfilter hook which means the packet already has had the header
	 * fields populated and the checksum calculated - yes this means we
	 * are doing more work than needed but we do it to keep the core
	 * stack clean and tidy */
	memcpy(iph + 1, buf, buf_len);
	if (opt_len > buf_len)
		memset((char *)(iph + 1) + buf_len, 0, opt_len - buf_len);
	if (len_delta != 0) {
		iph->ihl = 5 + (opt_len >> 2);
		iph->tot_len = htons(skb->len);
	}
	ip_send_check(iph);

	return 0;
}

/**
 * cipso_v4_skbuff_delattr - Delete any CIPSO options from a packet
 * @skb: the packet
 *
 * Description:
 * Removes any and all CIPSO options from the given packet.  Returns zero on
 * success, negative values on failure.
 *
 */
int cipso_v4_skbuff_delattr(struct sk_buff *skb)
{
	int ret_val;
	struct iphdr *iph;
	struct ip_options *opt = &IPCB(skb)->opt;
	unsigned char *cipso_ptr;

	if (opt->cipso == 0)
		return 0;

	/* since we are changing the packet we should make a copy */
	ret_val = skb_cow(skb, skb_headroom(skb));
	if (ret_val < 0)
		return ret_val;

	/* the easiest thing to do is just replace the cipso option with noop
	 * options since we don't change the size of the packet, although we
	 * still need to recalculate the checksum */

	iph = ip_hdr(skb);
	cipso_ptr = (unsigned char *)iph + opt->cipso;
	memset(cipso_ptr, IPOPT_NOOP, cipso_ptr[1]);
	opt->cipso = 0;
	opt->is_changed = 1;

	ip_send_check(iph);

	return 0;
}

/*
 * Setup Functions
 */

/**
 * cipso_v4_init - Initialize the CIPSO module
 *
 * Description:
 * Initialize the CIPSO module and prepare it for use.  Returns zero on success
 * and negative values on failure.
 *
 */
static int __init cipso_v4_init(void)
{
	int ret_val;

	ret_val = cipso_v4_cache_init();
	if (ret_val != 0)
		panic("Failed to initialize the CIPSO/IPv4 cache (%d)\n",
		      ret_val);

	return 0;
}

subsys_initcall(cipso_v4_init);
