/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * RDMA Network Block Driver
 *
 * Copyright (c) 2014 - 2018 ProfitBricks GmbH. All rights reserved.
 * Copyright (c) 2018 - 2019 1&1 IONOS Cloud GmbH. All rights reserved.
 * Copyright (c) 2019 - 2020 1&1 IONOS SE. All rights reserved.
 */
#ifndef RNBD_SRV_DEV_H
#define RNBD_SRV_DEV_H

#include <linux/fs.h>
#include "rnbd-proto.h"

struct rnbd_dev {
	struct block_device	*bdev;
	struct bio_set		*ibd_bio_set;
	fmode_t			blk_open_flags;
	char			name[BDEVNAME_SIZE];
};

struct rnbd_dev_blk_io {
	struct rnbd_dev *dev;
	void		 *priv;
	/* have to be last member for front_pad usage of bioset_init */
	struct bio	bio;
};

/**
 * rnbd_dev_open() - Open a device
 * @flags:	open flags
 * @bs:		bio_set to use during block io,
 */
struct rnbd_dev *rnbd_dev_open(const char *path, fmode_t flags,
			       struct bio_set *bs);

/**
 * rnbd_dev_close() - Close a device
 */
void rnbd_dev_close(struct rnbd_dev *dev);

void rnbd_endio(void *priv, int error);

void rnbd_dev_bi_end_io(struct bio *bio);

struct bio *rnbd_bio_map_kern(void *data, struct bio_set *bs,
			      unsigned int len, gfp_t gfp_mask);

static inline int rnbd_dev_get_max_segs(const struct rnbd_dev *dev)
{
	return queue_max_segments(bdev_get_queue(dev->bdev));
}

static inline int rnbd_dev_get_max_hw_sects(const struct rnbd_dev *dev)
{
	return queue_max_hw_sectors(bdev_get_queue(dev->bdev));
}

static inline int rnbd_dev_get_secure_discard(const struct rnbd_dev *dev)
{
	return blk_queue_secure_erase(bdev_get_queue(dev->bdev));
}

static inline int rnbd_dev_get_max_discard_sects(const struct rnbd_dev *dev)
{
	if (!blk_queue_discard(bdev_get_queue(dev->bdev)))
		return 0;

	return blk_queue_get_max_sectors(bdev_get_queue(dev->bdev),
					 REQ_OP_DISCARD);
}

static inline int rnbd_dev_get_discard_granularity(const struct rnbd_dev *dev)
{
	return bdev_get_queue(dev->bdev)->limits.discard_granularity;
}

static inline int rnbd_dev_get_discard_alignment(const struct rnbd_dev *dev)
{
	return bdev_get_queue(dev->bdev)->limits.discard_alignment;
}

#endif /* RNBD_SRV_DEV_H */
