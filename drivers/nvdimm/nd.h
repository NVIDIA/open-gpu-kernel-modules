/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright(c) 2013-2015 Intel Corporation. All rights reserved.
 */
#ifndef __ND_H__
#define __ND_H__
#include <linux/libnvdimm.h>
#include <linux/badblocks.h>
#include <linux/blkdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/ndctl.h>
#include <linux/types.h>
#include <linux/nd.h>
#include "label.h"

enum {
	/*
	 * Limits the maximum number of block apertures a dimm can
	 * support and is an input to the geometry/on-disk-format of a
	 * BTT instance
	 */
	ND_MAX_LANES = 256,
	INT_LBASIZE_ALIGNMENT = 64,
	NVDIMM_IO_ATOMIC = 1,
};

struct nvdimm_drvdata {
	struct device *dev;
	int nslabel_size;
	struct nd_cmd_get_config_size nsarea;
	void *data;
	int ns_current, ns_next;
	struct resource dpa;
	struct kref kref;
};

struct nd_region_data {
	int ns_count;
	int ns_active;
	unsigned int hints_shift;
	void __iomem *flush_wpq[];
};

static inline void __iomem *ndrd_get_flush_wpq(struct nd_region_data *ndrd,
		int dimm, int hint)
{
	unsigned int num = 1 << ndrd->hints_shift;
	unsigned int mask = num - 1;

	return ndrd->flush_wpq[dimm * num + (hint & mask)];
}

static inline void ndrd_set_flush_wpq(struct nd_region_data *ndrd, int dimm,
		int hint, void __iomem *flush)
{
	unsigned int num = 1 << ndrd->hints_shift;
	unsigned int mask = num - 1;

	ndrd->flush_wpq[dimm * num + (hint & mask)] = flush;
}

static inline struct nd_namespace_index *to_namespace_index(
		struct nvdimm_drvdata *ndd, int i)
{
	if (i < 0)
		return NULL;

	return ndd->data + sizeof_namespace_index(ndd) * i;
}

static inline struct nd_namespace_index *to_current_namespace_index(
		struct nvdimm_drvdata *ndd)
{
	return to_namespace_index(ndd, ndd->ns_current);
}

static inline struct nd_namespace_index *to_next_namespace_index(
		struct nvdimm_drvdata *ndd)
{
	return to_namespace_index(ndd, ndd->ns_next);
}

unsigned sizeof_namespace_label(struct nvdimm_drvdata *ndd);

#define namespace_label_has(ndd, field) \
	(offsetof(struct nd_namespace_label, field) \
		< sizeof_namespace_label(ndd))

#define nd_dbg_dpa(r, d, res, fmt, arg...) \
	dev_dbg((r) ? &(r)->dev : (d)->dev, "%s: %.13s: %#llx @ %#llx " fmt, \
		(r) ? dev_name((d)->dev) : "", res ? res->name : "null", \
		(unsigned long long) (res ? resource_size(res) : 0), \
		(unsigned long long) (res ? res->start : 0), ##arg)

#define for_each_dpa_resource(ndd, res) \
	for (res = (ndd)->dpa.child; res; res = res->sibling)

#define for_each_dpa_resource_safe(ndd, res, next) \
	for (res = (ndd)->dpa.child, next = res ? res->sibling : NULL; \
			res; res = next, next = next ? next->sibling : NULL)

struct nd_percpu_lane {
	int count;
	spinlock_t lock;
};

enum nd_label_flags {
	ND_LABEL_REAP,
};
struct nd_label_ent {
	struct list_head list;
	unsigned long flags;
	struct nd_namespace_label *label;
};

enum nd_mapping_lock_class {
	ND_MAPPING_CLASS0,
	ND_MAPPING_UUID_SCAN,
};

struct nd_mapping {
	struct nvdimm *nvdimm;
	u64 start;
	u64 size;
	int position;
	struct list_head labels;
	struct mutex lock;
	/*
	 * @ndd is for private use at region enable / disable time for
	 * get_ndd() + put_ndd(), all other nd_mapping to ndd
	 * conversions use to_ndd() which respects enabled state of the
	 * nvdimm.
	 */
	struct nvdimm_drvdata *ndd;
};

struct nd_region {
	struct device dev;
	struct ida ns_ida;
	struct ida btt_ida;
	struct ida pfn_ida;
	struct ida dax_ida;
	unsigned long flags;
	struct device *ns_seed;
	struct device *btt_seed;
	struct device *pfn_seed;
	struct device *dax_seed;
	unsigned long align;
	u16 ndr_mappings;
	u64 ndr_size;
	u64 ndr_start;
	int id, num_lanes, ro, numa_node, target_node;
	void *provider_data;
	struct kernfs_node *bb_state;
	struct badblocks bb;
	struct nd_interleave_set *nd_set;
	struct nd_percpu_lane __percpu *lane;
	int (*flush)(struct nd_region *nd_region, struct bio *bio);
	struct nd_mapping mapping[];
};

struct nd_blk_region {
	int (*enable)(struct nvdimm_bus *nvdimm_bus, struct device *dev);
	int (*do_io)(struct nd_blk_region *ndbr, resource_size_t dpa,
			void *iobuf, u64 len, int rw);
	void *blk_provider_data;
	struct nd_region nd_region;
};

/*
 * Lookup next in the repeating sequence of 01, 10, and 11.
 */
static inline unsigned nd_inc_seq(unsigned seq)
{
	static const unsigned next[] = { 0, 2, 3, 1 };

	return next[seq & 3];
}

struct btt;
struct nd_btt {
	struct device dev;
	struct nd_namespace_common *ndns;
	struct btt *btt;
	unsigned long lbasize;
	u64 size;
	u8 *uuid;
	int id;
	int initial_offset;
	u16 version_major;
	u16 version_minor;
};

enum nd_pfn_mode {
	PFN_MODE_NONE,
	PFN_MODE_RAM,
	PFN_MODE_PMEM,
};

struct nd_pfn {
	int id;
	u8 *uuid;
	struct device dev;
	unsigned long align;
	unsigned long npfns;
	enum nd_pfn_mode mode;
	struct nd_pfn_sb *pfn_sb;
	struct nd_namespace_common *ndns;
};

struct nd_dax {
	struct nd_pfn nd_pfn;
};

static inline u32 nd_info_block_reserve(void)
{
	return ALIGN(SZ_8K, PAGE_SIZE);
}

enum nd_async_mode {
	ND_SYNC,
	ND_ASYNC,
};

int nd_integrity_init(struct gendisk *disk, unsigned long meta_size);
void wait_nvdimm_bus_probe_idle(struct device *dev);
void nd_device_register(struct device *dev);
void nd_device_unregister(struct device *dev, enum nd_async_mode mode);
void nd_device_notify(struct device *dev, enum nvdimm_event event);
int nd_uuid_store(struct device *dev, u8 **uuid_out, const char *buf,
		size_t len);
ssize_t nd_size_select_show(unsigned long current_size,
		const unsigned long *supported, char *buf);
ssize_t nd_size_select_store(struct device *dev, const char *buf,
		unsigned long *current_size, const unsigned long *supported);
int __init nvdimm_init(void);
int __init nd_region_init(void);
int __init nd_label_init(void);
void nvdimm_exit(void);
void nd_region_exit(void);
struct nvdimm;
extern const struct attribute_group nd_device_attribute_group;
extern const struct attribute_group nd_numa_attribute_group;
extern const struct attribute_group *nvdimm_bus_attribute_groups[];
struct nvdimm_drvdata *to_ndd(struct nd_mapping *nd_mapping);
int nvdimm_check_config_data(struct device *dev);
int nvdimm_init_nsarea(struct nvdimm_drvdata *ndd);
int nvdimm_init_config_data(struct nvdimm_drvdata *ndd);
int nvdimm_get_config_data(struct nvdimm_drvdata *ndd, void *buf,
			   size_t offset, size_t len);
int nvdimm_set_config_data(struct nvdimm_drvdata *ndd, size_t offset,
		void *buf, size_t len);
long nvdimm_clear_poison(struct device *dev, phys_addr_t phys,
		unsigned int len);
void nvdimm_set_labeling(struct device *dev);
void nvdimm_set_locked(struct device *dev);
void nvdimm_clear_locked(struct device *dev);
int nvdimm_security_setup_events(struct device *dev);
#if IS_ENABLED(CONFIG_NVDIMM_KEYS)
int nvdimm_security_unlock(struct device *dev);
#else
static inline int nvdimm_security_unlock(struct device *dev)
{
	return 0;
}
#endif
struct nd_btt *to_nd_btt(struct device *dev);

struct nd_gen_sb {
	char reserved[SZ_4K - 8];
	__le64 checksum;
};

u64 nd_sb_checksum(struct nd_gen_sb *sb);
#if IS_ENABLED(CONFIG_BTT)
int nd_btt_probe(struct device *dev, struct nd_namespace_common *ndns);
bool is_nd_btt(struct device *dev);
struct device *nd_btt_create(struct nd_region *nd_region);
#else
static inline int nd_btt_probe(struct device *dev,
		struct nd_namespace_common *ndns)
{
	return -ENODEV;
}

static inline bool is_nd_btt(struct device *dev)
{
	return false;
}

static inline struct device *nd_btt_create(struct nd_region *nd_region)
{
	return NULL;
}
#endif

struct nd_pfn *to_nd_pfn(struct device *dev);
#if IS_ENABLED(CONFIG_NVDIMM_PFN)

#define MAX_NVDIMM_ALIGN	4

int nd_pfn_probe(struct device *dev, struct nd_namespace_common *ndns);
bool is_nd_pfn(struct device *dev);
struct device *nd_pfn_create(struct nd_region *nd_region);
struct device *nd_pfn_devinit(struct nd_pfn *nd_pfn,
		struct nd_namespace_common *ndns);
int nd_pfn_validate(struct nd_pfn *nd_pfn, const char *sig);
extern const struct attribute_group *nd_pfn_attribute_groups[];
#else
static inline int nd_pfn_probe(struct device *dev,
		struct nd_namespace_common *ndns)
{
	return -ENODEV;
}

static inline bool is_nd_pfn(struct device *dev)
{
	return false;
}

static inline struct device *nd_pfn_create(struct nd_region *nd_region)
{
	return NULL;
}

static inline int nd_pfn_validate(struct nd_pfn *nd_pfn, const char *sig)
{
	return -ENODEV;
}
#endif

struct nd_dax *to_nd_dax(struct device *dev);
#if IS_ENABLED(CONFIG_NVDIMM_DAX)
int nd_dax_probe(struct device *dev, struct nd_namespace_common *ndns);
bool is_nd_dax(struct device *dev);
struct device *nd_dax_create(struct nd_region *nd_region);
#else
static inline int nd_dax_probe(struct device *dev,
		struct nd_namespace_common *ndns)
{
	return -ENODEV;
}

static inline bool is_nd_dax(struct device *dev)
{
	return false;
}

static inline struct device *nd_dax_create(struct nd_region *nd_region)
{
	return NULL;
}
#endif

int nd_region_to_nstype(struct nd_region *nd_region);
int nd_region_register_namespaces(struct nd_region *nd_region, int *err);
u64 nd_region_interleave_set_cookie(struct nd_region *nd_region,
		struct nd_namespace_index *nsindex);
u64 nd_region_interleave_set_altcookie(struct nd_region *nd_region);
void nvdimm_bus_lock(struct device *dev);
void nvdimm_bus_unlock(struct device *dev);
bool is_nvdimm_bus_locked(struct device *dev);
void nvdimm_check_and_set_ro(struct gendisk *disk);
void nvdimm_drvdata_release(struct kref *kref);
void put_ndd(struct nvdimm_drvdata *ndd);
int nd_label_reserve_dpa(struct nvdimm_drvdata *ndd);
void nvdimm_free_dpa(struct nvdimm_drvdata *ndd, struct resource *res);
struct resource *nvdimm_allocate_dpa(struct nvdimm_drvdata *ndd,
		struct nd_label_id *label_id, resource_size_t start,
		resource_size_t n);
resource_size_t nvdimm_namespace_capacity(struct nd_namespace_common *ndns);
bool nvdimm_namespace_locked(struct nd_namespace_common *ndns);
struct nd_namespace_common *nvdimm_namespace_common_probe(struct device *dev);
int nvdimm_namespace_attach_btt(struct nd_namespace_common *ndns);
int nvdimm_namespace_detach_btt(struct nd_btt *nd_btt);
const char *nvdimm_namespace_disk_name(struct nd_namespace_common *ndns,
		char *name);
unsigned int pmem_sector_size(struct nd_namespace_common *ndns);
struct range;
void nvdimm_badblocks_populate(struct nd_region *nd_region,
		struct badblocks *bb, const struct range *range);
int devm_namespace_enable(struct device *dev, struct nd_namespace_common *ndns,
		resource_size_t size);
void devm_namespace_disable(struct device *dev,
		struct nd_namespace_common *ndns);
#if IS_ENABLED(CONFIG_ND_CLAIM)
/* max struct page size independent of kernel config */
#define MAX_STRUCT_PAGE_SIZE 64
int nvdimm_setup_pfn(struct nd_pfn *nd_pfn, struct dev_pagemap *pgmap);
#else
static inline int nvdimm_setup_pfn(struct nd_pfn *nd_pfn,
				   struct dev_pagemap *pgmap)
{
	return -ENXIO;
}
#endif
int nd_blk_region_init(struct nd_region *nd_region);
int nd_region_activate(struct nd_region *nd_region);
static inline bool is_bad_pmem(struct badblocks *bb, sector_t sector,
		unsigned int len)
{
	if (bb->count) {
		sector_t first_bad;
		int num_bad;

		return !!badblocks_check(bb, sector, len / 512, &first_bad,
				&num_bad);
	}

	return false;
}
resource_size_t nd_namespace_blk_validate(struct nd_namespace_blk *nsblk);
const u8 *nd_dev_to_uuid(struct device *dev);
bool pmem_should_map_pages(struct device *dev);
#endif /* __ND_H__ */
