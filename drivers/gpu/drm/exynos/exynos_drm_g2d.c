// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 Samsung Electronics Co.Ltd
 * Authors: Joonyoung Shim <jy0922.shim@samsung.com>
 */

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include <drm/drm_file.h>
#include <drm/exynos_drm.h>

#include "exynos_drm_drv.h"
#include "exynos_drm_g2d.h"
#include "exynos_drm_gem.h"

#define G2D_HW_MAJOR_VER		4
#define G2D_HW_MINOR_VER		1

/* vaild register range set from user: 0x0104 ~ 0x0880 */
#define G2D_VALID_START			0x0104
#define G2D_VALID_END			0x0880

/* general registers */
#define G2D_SOFT_RESET			0x0000
#define G2D_INTEN			0x0004
#define G2D_INTC_PEND			0x000C
#define G2D_DMA_SFR_BASE_ADDR		0x0080
#define G2D_DMA_COMMAND			0x0084
#define G2D_DMA_STATUS			0x008C
#define G2D_DMA_HOLD_CMD		0x0090

/* command registers */
#define G2D_BITBLT_START		0x0100

/* registers for base address */
#define G2D_SRC_BASE_ADDR		0x0304
#define G2D_SRC_STRIDE			0x0308
#define G2D_SRC_COLOR_MODE		0x030C
#define G2D_SRC_LEFT_TOP		0x0310
#define G2D_SRC_RIGHT_BOTTOM		0x0314
#define G2D_SRC_PLANE2_BASE_ADDR	0x0318
#define G2D_DST_BASE_ADDR		0x0404
#define G2D_DST_STRIDE			0x0408
#define G2D_DST_COLOR_MODE		0x040C
#define G2D_DST_LEFT_TOP		0x0410
#define G2D_DST_RIGHT_BOTTOM		0x0414
#define G2D_DST_PLANE2_BASE_ADDR	0x0418
#define G2D_PAT_BASE_ADDR		0x0500
#define G2D_MSK_BASE_ADDR		0x0520

/* G2D_SOFT_RESET */
#define G2D_SFRCLEAR			(1 << 1)
#define G2D_R				(1 << 0)

/* G2D_INTEN */
#define G2D_INTEN_ACF			(1 << 3)
#define G2D_INTEN_UCF			(1 << 2)
#define G2D_INTEN_GCF			(1 << 1)
#define G2D_INTEN_SCF			(1 << 0)

/* G2D_INTC_PEND */
#define G2D_INTP_ACMD_FIN		(1 << 3)
#define G2D_INTP_UCMD_FIN		(1 << 2)
#define G2D_INTP_GCMD_FIN		(1 << 1)
#define G2D_INTP_SCMD_FIN		(1 << 0)

/* G2D_DMA_COMMAND */
#define G2D_DMA_HALT			(1 << 2)
#define G2D_DMA_CONTINUE		(1 << 1)
#define G2D_DMA_START			(1 << 0)

/* G2D_DMA_STATUS */
#define G2D_DMA_LIST_DONE_COUNT		(0xFF << 17)
#define G2D_DMA_BITBLT_DONE_COUNT	(0xFFFF << 1)
#define G2D_DMA_DONE			(1 << 0)
#define G2D_DMA_LIST_DONE_COUNT_OFFSET	17

/* G2D_DMA_HOLD_CMD */
#define G2D_USER_HOLD			(1 << 2)
#define G2D_LIST_HOLD			(1 << 1)
#define G2D_BITBLT_HOLD			(1 << 0)

/* G2D_BITBLT_START */
#define G2D_START_CASESEL		(1 << 2)
#define G2D_START_NHOLT			(1 << 1)
#define G2D_START_BITBLT		(1 << 0)

/* buffer color format */
#define G2D_FMT_XRGB8888		0
#define G2D_FMT_ARGB8888		1
#define G2D_FMT_RGB565			2
#define G2D_FMT_XRGB1555		3
#define G2D_FMT_ARGB1555		4
#define G2D_FMT_XRGB4444		5
#define G2D_FMT_ARGB4444		6
#define G2D_FMT_PACKED_RGB888		7
#define G2D_FMT_A8			11
#define G2D_FMT_L8			12

/* buffer valid length */
#define G2D_LEN_MIN			1
#define G2D_LEN_MAX			8000

#define G2D_CMDLIST_SIZE		(PAGE_SIZE / 4)
#define G2D_CMDLIST_NUM			64
#define G2D_CMDLIST_POOL_SIZE		(G2D_CMDLIST_SIZE * G2D_CMDLIST_NUM)
#define G2D_CMDLIST_DATA_NUM		(G2D_CMDLIST_SIZE / sizeof(u32) - 2)

/* maximum buffer pool size of userptr is 64MB as default */
#define MAX_POOL		(64 * 1024 * 1024)

enum {
	BUF_TYPE_GEM = 1,
	BUF_TYPE_USERPTR,
};

enum g2d_reg_type {
	REG_TYPE_NONE = -1,
	REG_TYPE_SRC,
	REG_TYPE_SRC_PLANE2,
	REG_TYPE_DST,
	REG_TYPE_DST_PLANE2,
	REG_TYPE_PAT,
	REG_TYPE_MSK,
	MAX_REG_TYPE_NR
};

enum g2d_flag_bits {
	/*
	 * If set, suspends the runqueue worker after the currently
	 * processed node is finished.
	 */
	G2D_BIT_SUSPEND_RUNQUEUE,
	/*
	 * If set, indicates that the engine is currently busy.
	 */
	G2D_BIT_ENGINE_BUSY,
};

/* cmdlist data structure */
struct g2d_cmdlist {
	u32		head;
	unsigned long	data[G2D_CMDLIST_DATA_NUM];
	u32		last;	/* last data offset */
};

/*
 * A structure of buffer description
 *
 * @format: color format
 * @stride: buffer stride/pitch in bytes
 * @left_x: the x coordinates of left top corner
 * @top_y: the y coordinates of left top corner
 * @right_x: the x coordinates of right bottom corner
 * @bottom_y: the y coordinates of right bottom corner
 *
 */
struct g2d_buf_desc {
	unsigned int	format;
	unsigned int	stride;
	unsigned int	left_x;
	unsigned int	top_y;
	unsigned int	right_x;
	unsigned int	bottom_y;
};

/*
 * A structure of buffer information
 *
 * @map_nr: manages the number of mapped buffers
 * @reg_types: stores regitster type in the order of requested command
 * @handles: stores buffer handle in its reg_type position
 * @types: stores buffer type in its reg_type position
 * @descs: stores buffer description in its reg_type position
 *
 */
struct g2d_buf_info {
	unsigned int		map_nr;
	enum g2d_reg_type	reg_types[MAX_REG_TYPE_NR];
	void			*obj[MAX_REG_TYPE_NR];
	unsigned int		types[MAX_REG_TYPE_NR];
	struct g2d_buf_desc	descs[MAX_REG_TYPE_NR];
};

struct drm_exynos_pending_g2d_event {
	struct drm_pending_event	base;
	struct drm_exynos_g2d_event	event;
};

struct g2d_cmdlist_userptr {
	struct list_head	list;
	dma_addr_t		dma_addr;
	unsigned long		userptr;
	unsigned long		size;
	struct page		**pages;
	unsigned int		npages;
	struct sg_table		*sgt;
	atomic_t		refcount;
	bool			in_pool;
	bool			out_of_list;
};
struct g2d_cmdlist_node {
	struct list_head	list;
	struct g2d_cmdlist	*cmdlist;
	dma_addr_t		dma_addr;
	struct g2d_buf_info	buf_info;

	struct drm_exynos_pending_g2d_event	*event;
};

struct g2d_runqueue_node {
	struct list_head	list;
	struct list_head	run_cmdlist;
	struct list_head	event_list;
	struct drm_file		*filp;
	pid_t			pid;
	struct completion	complete;
	int			async;
};

struct g2d_data {
	struct device			*dev;
	void				*dma_priv;
	struct clk			*gate_clk;
	void __iomem			*regs;
	int				irq;
	struct workqueue_struct		*g2d_workq;
	struct work_struct		runqueue_work;
	struct drm_device		*drm_dev;
	unsigned long			flags;

	/* cmdlist */
	struct g2d_cmdlist_node		*cmdlist_node;
	struct list_head		free_cmdlist;
	struct mutex			cmdlist_mutex;
	dma_addr_t			cmdlist_pool;
	void				*cmdlist_pool_virt;
	unsigned long			cmdlist_dma_attrs;

	/* runqueue*/
	struct g2d_runqueue_node	*runqueue_node;
	struct list_head		runqueue;
	struct mutex			runqueue_mutex;
	struct kmem_cache		*runqueue_slab;

	unsigned long			current_pool;
	unsigned long			max_pool;
};

static inline void g2d_hw_reset(struct g2d_data *g2d)
{
	writel(G2D_R | G2D_SFRCLEAR, g2d->regs + G2D_SOFT_RESET);
	clear_bit(G2D_BIT_ENGINE_BUSY, &g2d->flags);
}

static int g2d_init_cmdlist(struct g2d_data *g2d)
{
	struct device *dev = g2d->dev;
	struct g2d_cmdlist_node *node;
	int nr;
	int ret;
	struct g2d_buf_info *buf_info;

	g2d->cmdlist_dma_attrs = DMA_ATTR_WRITE_COMBINE;

	g2d->cmdlist_pool_virt = dma_alloc_attrs(to_dma_dev(g2d->drm_dev),
						G2D_CMDLIST_POOL_SIZE,
						&g2d->cmdlist_pool, GFP_KERNEL,
						g2d->cmdlist_dma_attrs);
	if (!g2d->cmdlist_pool_virt) {
		dev_err(dev, "failed to allocate dma memory\n");
		return -ENOMEM;
	}

	node = kcalloc(G2D_CMDLIST_NUM, sizeof(*node), GFP_KERNEL);
	if (!node) {
		ret = -ENOMEM;
		goto err;
	}

	for (nr = 0; nr < G2D_CMDLIST_NUM; nr++) {
		unsigned int i;

		node[nr].cmdlist =
			g2d->cmdlist_pool_virt + nr * G2D_CMDLIST_SIZE;
		node[nr].dma_addr =
			g2d->cmdlist_pool + nr * G2D_CMDLIST_SIZE;

		buf_info = &node[nr].buf_info;
		for (i = 0; i < MAX_REG_TYPE_NR; i++)
			buf_info->reg_types[i] = REG_TYPE_NONE;

		list_add_tail(&node[nr].list, &g2d->free_cmdlist);
	}

	return 0;

err:
	dma_free_attrs(to_dma_dev(g2d->drm_dev), G2D_CMDLIST_POOL_SIZE,
			g2d->cmdlist_pool_virt,
			g2d->cmdlist_pool, g2d->cmdlist_dma_attrs);
	return ret;
}

static void g2d_fini_cmdlist(struct g2d_data *g2d)
{
	kfree(g2d->cmdlist_node);

	if (g2d->cmdlist_pool_virt && g2d->cmdlist_pool) {
		dma_free_attrs(to_dma_dev(g2d->drm_dev),
				G2D_CMDLIST_POOL_SIZE,
				g2d->cmdlist_pool_virt,
				g2d->cmdlist_pool, g2d->cmdlist_dma_attrs);
	}
}

static struct g2d_cmdlist_node *g2d_get_cmdlist(struct g2d_data *g2d)
{
	struct device *dev = g2d->dev;
	struct g2d_cmdlist_node *node;

	mutex_lock(&g2d->cmdlist_mutex);
	if (list_empty(&g2d->free_cmdlist)) {
		dev_err(dev, "there is no free cmdlist\n");
		mutex_unlock(&g2d->cmdlist_mutex);
		return NULL;
	}

	node = list_first_entry(&g2d->free_cmdlist, struct g2d_cmdlist_node,
				list);
	list_del_init(&node->list);
	mutex_unlock(&g2d->cmdlist_mutex);

	return node;
}

static void g2d_put_cmdlist(struct g2d_data *g2d, struct g2d_cmdlist_node *node)
{
	mutex_lock(&g2d->cmdlist_mutex);
	list_move_tail(&node->list, &g2d->free_cmdlist);
	mutex_unlock(&g2d->cmdlist_mutex);
}

static void g2d_add_cmdlist_to_inuse(struct drm_exynos_file_private *file_priv,
				     struct g2d_cmdlist_node *node)
{
	struct g2d_cmdlist_node *lnode;

	if (list_empty(&file_priv->inuse_cmdlist))
		goto add_to_list;

	/* this links to base address of new cmdlist */
	lnode = list_entry(file_priv->inuse_cmdlist.prev,
				struct g2d_cmdlist_node, list);
	lnode->cmdlist->data[lnode->cmdlist->last] = node->dma_addr;

add_to_list:
	list_add_tail(&node->list, &file_priv->inuse_cmdlist);

	if (node->event)
		list_add_tail(&node->event->base.link, &file_priv->event_list);
}

static void g2d_userptr_put_dma_addr(struct g2d_data *g2d,
					void *obj,
					bool force)
{
	struct g2d_cmdlist_userptr *g2d_userptr = obj;

	if (!obj)
		return;

	if (force)
		goto out;

	atomic_dec(&g2d_userptr->refcount);

	if (atomic_read(&g2d_userptr->refcount) > 0)
		return;

	if (g2d_userptr->in_pool)
		return;

out:
	dma_unmap_sgtable(to_dma_dev(g2d->drm_dev), g2d_userptr->sgt,
			  DMA_BIDIRECTIONAL, 0);

	unpin_user_pages_dirty_lock(g2d_userptr->pages, g2d_userptr->npages,
				    true);
	kvfree(g2d_userptr->pages);

	if (!g2d_userptr->out_of_list)
		list_del_init(&g2d_userptr->list);

	sg_free_table(g2d_userptr->sgt);
	kfree(g2d_userptr->sgt);
	kfree(g2d_userptr);
}

static dma_addr_t *g2d_userptr_get_dma_addr(struct g2d_data *g2d,
					unsigned long userptr,
					unsigned long size,
					struct drm_file *filp,
					void **obj)
{
	struct drm_exynos_file_private *file_priv = filp->driver_priv;
	struct g2d_cmdlist_userptr *g2d_userptr;
	struct sg_table	*sgt;
	unsigned long start, end;
	unsigned int npages, offset;
	int ret;

	if (!size) {
		DRM_DEV_ERROR(g2d->dev, "invalid userptr size.\n");
		return ERR_PTR(-EINVAL);
	}

	/* check if userptr already exists in userptr_list. */
	list_for_each_entry(g2d_userptr, &file_priv->userptr_list, list) {
		if (g2d_userptr->userptr == userptr) {
			/*
			 * also check size because there could be same address
			 * and different size.
			 */
			if (g2d_userptr->size == size) {
				atomic_inc(&g2d_userptr->refcount);
				*obj = g2d_userptr;

				return &g2d_userptr->dma_addr;
			}

			/*
			 * at this moment, maybe g2d dma is accessing this
			 * g2d_userptr memory region so just remove this
			 * g2d_userptr object from userptr_list not to be
			 * referred again and also except it the userptr
			 * pool to be released after the dma access completion.
			 */
			g2d_userptr->out_of_list = true;
			g2d_userptr->in_pool = false;
			list_del_init(&g2d_userptr->list);

			break;
		}
	}

	g2d_userptr = kzalloc(sizeof(*g2d_userptr), GFP_KERNEL);
	if (!g2d_userptr)
		return ERR_PTR(-ENOMEM);

	atomic_set(&g2d_userptr->refcount, 1);
	g2d_userptr->size = size;

	start = userptr & PAGE_MASK;
	offset = userptr & ~PAGE_MASK;
	end = PAGE_ALIGN(userptr + size);
	npages = (end - start) >> PAGE_SHIFT;
	g2d_userptr->pages = kvmalloc_array(npages, sizeof(*g2d_userptr->pages),
					    GFP_KERNEL);
	if (!g2d_userptr->pages) {
		ret = -ENOMEM;
		goto err_free;
	}

	ret = pin_user_pages_fast(start, npages,
				  FOLL_FORCE | FOLL_WRITE | FOLL_LONGTERM,
				  g2d_userptr->pages);
	if (ret != npages) {
		DRM_DEV_ERROR(g2d->dev,
			      "failed to get user pages from userptr.\n");
		if (ret < 0)
			goto err_destroy_pages;
		npages = ret;
		ret = -EFAULT;
		goto err_unpin_pages;
	}
	g2d_userptr->npages = npages;

	sgt = kzalloc(sizeof(*sgt), GFP_KERNEL);
	if (!sgt) {
		ret = -ENOMEM;
		goto err_unpin_pages;
	}

	ret = sg_alloc_table_from_pages(sgt,
					g2d_userptr->pages,
					npages, offset, size, GFP_KERNEL);
	if (ret < 0) {
		DRM_DEV_ERROR(g2d->dev, "failed to get sgt from pages.\n");
		goto err_free_sgt;
	}

	g2d_userptr->sgt = sgt;

	ret = dma_map_sgtable(to_dma_dev(g2d->drm_dev), sgt,
			      DMA_BIDIRECTIONAL, 0);
	if (ret) {
		DRM_DEV_ERROR(g2d->dev, "failed to map sgt with dma region.\n");
		goto err_sg_free_table;
	}

	g2d_userptr->dma_addr = sgt->sgl[0].dma_address;
	g2d_userptr->userptr = userptr;

	list_add_tail(&g2d_userptr->list, &file_priv->userptr_list);

	if (g2d->current_pool + (npages << PAGE_SHIFT) < g2d->max_pool) {
		g2d->current_pool += npages << PAGE_SHIFT;
		g2d_userptr->in_pool = true;
	}

	*obj = g2d_userptr;

	return &g2d_userptr->dma_addr;

err_sg_free_table:
	sg_free_table(sgt);

err_free_sgt:
	kfree(sgt);

err_unpin_pages:
	unpin_user_pages(g2d_userptr->pages, npages);

err_destroy_pages:
	kvfree(g2d_userptr->pages);

err_free:
	kfree(g2d_userptr);

	return ERR_PTR(ret);
}

static void g2d_userptr_free_all(struct g2d_data *g2d, struct drm_file *filp)
{
	struct drm_exynos_file_private *file_priv = filp->driver_priv;
	struct g2d_cmdlist_userptr *g2d_userptr, *n;

	list_for_each_entry_safe(g2d_userptr, n, &file_priv->userptr_list, list)
		if (g2d_userptr->in_pool)
			g2d_userptr_put_dma_addr(g2d, g2d_userptr, true);

	g2d->current_pool = 0;
}

static enum g2d_reg_type g2d_get_reg_type(struct g2d_data *g2d, int reg_offset)
{
	enum g2d_reg_type reg_type;

	switch (reg_offset) {
	case G2D_SRC_BASE_ADDR:
	case G2D_SRC_STRIDE:
	case G2D_SRC_COLOR_MODE:
	case G2D_SRC_LEFT_TOP:
	case G2D_SRC_RIGHT_BOTTOM:
		reg_type = REG_TYPE_SRC;
		break;
	case G2D_SRC_PLANE2_BASE_ADDR:
		reg_type = REG_TYPE_SRC_PLANE2;
		break;
	case G2D_DST_BASE_ADDR:
	case G2D_DST_STRIDE:
	case G2D_DST_COLOR_MODE:
	case G2D_DST_LEFT_TOP:
	case G2D_DST_RIGHT_BOTTOM:
		reg_type = REG_TYPE_DST;
		break;
	case G2D_DST_PLANE2_BASE_ADDR:
		reg_type = REG_TYPE_DST_PLANE2;
		break;
	case G2D_PAT_BASE_ADDR:
		reg_type = REG_TYPE_PAT;
		break;
	case G2D_MSK_BASE_ADDR:
		reg_type = REG_TYPE_MSK;
		break;
	default:
		reg_type = REG_TYPE_NONE;
		DRM_DEV_ERROR(g2d->dev, "Unknown register offset![%d]\n",
			      reg_offset);
		break;
	}

	return reg_type;
}

static unsigned long g2d_get_buf_bpp(unsigned int format)
{
	unsigned long bpp;

	switch (format) {
	case G2D_FMT_XRGB8888:
	case G2D_FMT_ARGB8888:
		bpp = 4;
		break;
	case G2D_FMT_RGB565:
	case G2D_FMT_XRGB1555:
	case G2D_FMT_ARGB1555:
	case G2D_FMT_XRGB4444:
	case G2D_FMT_ARGB4444:
		bpp = 2;
		break;
	case G2D_FMT_PACKED_RGB888:
		bpp = 3;
		break;
	default:
		bpp = 1;
		break;
	}

	return bpp;
}

static bool g2d_check_buf_desc_is_valid(struct g2d_data *g2d,
					struct g2d_buf_desc *buf_desc,
					enum g2d_reg_type reg_type,
					unsigned long size)
{
	int width, height;
	unsigned long bpp, last_pos;

	/*
	 * check source and destination buffers only.
	 * so the others are always valid.
	 */
	if (reg_type != REG_TYPE_SRC && reg_type != REG_TYPE_DST)
		return true;

	/* This check also makes sure that right_x > left_x. */
	width = (int)buf_desc->right_x - (int)buf_desc->left_x;
	if (width < G2D_LEN_MIN || width > G2D_LEN_MAX) {
		DRM_DEV_ERROR(g2d->dev, "width[%d] is out of range!\n", width);
		return false;
	}

	/* This check also makes sure that bottom_y > top_y. */
	height = (int)buf_desc->bottom_y - (int)buf_desc->top_y;
	if (height < G2D_LEN_MIN || height > G2D_LEN_MAX) {
		DRM_DEV_ERROR(g2d->dev,
			      "height[%d] is out of range!\n", height);
		return false;
	}

	bpp = g2d_get_buf_bpp(buf_desc->format);

	/* Compute the position of the last byte that the engine accesses. */
	last_pos = ((unsigned long)buf_desc->bottom_y - 1) *
		(unsigned long)buf_desc->stride +
		(unsigned long)buf_desc->right_x * bpp - 1;

	/*
	 * Since right_x > left_x and bottom_y > top_y we already know
	 * that the first_pos < last_pos (first_pos being the position
	 * of the first byte the engine accesses), it just remains to
	 * check if last_pos is smaller then the buffer size.
	 */

	if (last_pos >= size) {
		DRM_DEV_ERROR(g2d->dev, "last engine access position [%lu] "
			      "is out of range [%lu]!\n", last_pos, size);
		return false;
	}

	return true;
}

static int g2d_map_cmdlist_gem(struct g2d_data *g2d,
				struct g2d_cmdlist_node *node,
				struct drm_device *drm_dev,
				struct drm_file *file)
{
	struct g2d_cmdlist *cmdlist = node->cmdlist;
	struct g2d_buf_info *buf_info = &node->buf_info;
	int offset;
	int ret;
	int i;

	for (i = 0; i < buf_info->map_nr; i++) {
		struct g2d_buf_desc *buf_desc;
		enum g2d_reg_type reg_type;
		int reg_pos;
		unsigned long handle;
		dma_addr_t *addr;

		reg_pos = cmdlist->last - 2 * (i + 1);

		offset = cmdlist->data[reg_pos];
		handle = cmdlist->data[reg_pos + 1];

		reg_type = g2d_get_reg_type(g2d, offset);
		if (reg_type == REG_TYPE_NONE) {
			ret = -EFAULT;
			goto err;
		}

		buf_desc = &buf_info->descs[reg_type];

		if (buf_info->types[reg_type] == BUF_TYPE_GEM) {
			struct exynos_drm_gem *exynos_gem;

			exynos_gem = exynos_drm_gem_get(file, handle);
			if (!exynos_gem) {
				ret = -EFAULT;
				goto err;
			}

			if (!g2d_check_buf_desc_is_valid(g2d, buf_desc,
							 reg_type, exynos_gem->size)) {
				exynos_drm_gem_put(exynos_gem);
				ret = -EFAULT;
				goto err;
			}

			addr = &exynos_gem->dma_addr;
			buf_info->obj[reg_type] = exynos_gem;
		} else {
			struct drm_exynos_g2d_userptr g2d_userptr;

			if (copy_from_user(&g2d_userptr, (void __user *)handle,
				sizeof(struct drm_exynos_g2d_userptr))) {
				ret = -EFAULT;
				goto err;
			}

			if (!g2d_check_buf_desc_is_valid(g2d, buf_desc,
							 reg_type,
							 g2d_userptr.size)) {
				ret = -EFAULT;
				goto err;
			}

			addr = g2d_userptr_get_dma_addr(g2d,
							g2d_userptr.userptr,
							g2d_userptr.size,
							file,
							&buf_info->obj[reg_type]);
			if (IS_ERR(addr)) {
				ret = -EFAULT;
				goto err;
			}
		}

		cmdlist->data[reg_pos + 1] = *addr;
		buf_info->reg_types[i] = reg_type;
	}

	return 0;

err:
	buf_info->map_nr = i;
	return ret;
}

static void g2d_unmap_cmdlist_gem(struct g2d_data *g2d,
				  struct g2d_cmdlist_node *node,
				  struct drm_file *filp)
{
	struct g2d_buf_info *buf_info = &node->buf_info;
	int i;

	for (i = 0; i < buf_info->map_nr; i++) {
		struct g2d_buf_desc *buf_desc;
		enum g2d_reg_type reg_type;
		void *obj;

		reg_type = buf_info->reg_types[i];

		buf_desc = &buf_info->descs[reg_type];
		obj = buf_info->obj[reg_type];

		if (buf_info->types[reg_type] == BUF_TYPE_GEM)
			exynos_drm_gem_put(obj);
		else
			g2d_userptr_put_dma_addr(g2d, obj, false);

		buf_info->reg_types[i] = REG_TYPE_NONE;
		buf_info->obj[reg_type] = NULL;
		buf_info->types[reg_type] = 0;
		memset(buf_desc, 0x00, sizeof(*buf_desc));
	}

	buf_info->map_nr = 0;
}

static void g2d_dma_start(struct g2d_data *g2d,
			  struct g2d_runqueue_node *runqueue_node)
{
	struct g2d_cmdlist_node *node =
				list_first_entry(&runqueue_node->run_cmdlist,
						struct g2d_cmdlist_node, list);

	set_bit(G2D_BIT_ENGINE_BUSY, &g2d->flags);
	writel_relaxed(node->dma_addr, g2d->regs + G2D_DMA_SFR_BASE_ADDR);
	writel_relaxed(G2D_DMA_START, g2d->regs + G2D_DMA_COMMAND);
}

static struct g2d_runqueue_node *g2d_get_runqueue_node(struct g2d_data *g2d)
{
	struct g2d_runqueue_node *runqueue_node;

	if (list_empty(&g2d->runqueue))
		return NULL;

	runqueue_node = list_first_entry(&g2d->runqueue,
					 struct g2d_runqueue_node, list);
	list_del_init(&runqueue_node->list);
	return runqueue_node;
}

static void g2d_free_runqueue_node(struct g2d_data *g2d,
				   struct g2d_runqueue_node *runqueue_node)
{
	struct g2d_cmdlist_node *node;

	mutex_lock(&g2d->cmdlist_mutex);
	/*
	 * commands in run_cmdlist have been completed so unmap all gem
	 * objects in each command node so that they are unreferenced.
	 */
	list_for_each_entry(node, &runqueue_node->run_cmdlist, list)
		g2d_unmap_cmdlist_gem(g2d, node, runqueue_node->filp);
	list_splice_tail_init(&runqueue_node->run_cmdlist, &g2d->free_cmdlist);
	mutex_unlock(&g2d->cmdlist_mutex);

	kmem_cache_free(g2d->runqueue_slab, runqueue_node);
}

/**
 * g2d_remove_runqueue_nodes - remove items from the list of runqueue nodes
 * @g2d: G2D state object
 * @file: if not zero, only remove items with this DRM file
 *
 * Has to be called under runqueue lock.
 */
static void g2d_remove_runqueue_nodes(struct g2d_data *g2d, struct drm_file *file)
{
	struct g2d_runqueue_node *node, *n;

	if (list_empty(&g2d->runqueue))
		return;

	list_for_each_entry_safe(node, n, &g2d->runqueue, list) {
		if (file && node->filp != file)
			continue;

		list_del_init(&node->list);
		g2d_free_runqueue_node(g2d, node);
	}
}

static void g2d_runqueue_worker(struct work_struct *work)
{
	struct g2d_data *g2d = container_of(work, struct g2d_data,
					    runqueue_work);
	struct g2d_runqueue_node *runqueue_node;

	/*
	 * The engine is busy and the completion of the current node is going
	 * to poke the runqueue worker, so nothing to do here.
	 */
	if (test_bit(G2D_BIT_ENGINE_BUSY, &g2d->flags))
		return;

	mutex_lock(&g2d->runqueue_mutex);

	runqueue_node = g2d->runqueue_node;
	g2d->runqueue_node = NULL;

	if (runqueue_node) {
		pm_runtime_mark_last_busy(g2d->dev);
		pm_runtime_put_autosuspend(g2d->dev);

		complete(&runqueue_node->complete);
		if (runqueue_node->async)
			g2d_free_runqueue_node(g2d, runqueue_node);
	}

	if (!test_bit(G2D_BIT_SUSPEND_RUNQUEUE, &g2d->flags)) {
		g2d->runqueue_node = g2d_get_runqueue_node(g2d);

		if (g2d->runqueue_node) {
			int ret;

			ret = pm_runtime_resume_and_get(g2d->dev);
			if (ret < 0) {
				dev_err(g2d->dev, "failed to enable G2D device.\n");
				return;
			}

			g2d_dma_start(g2d, g2d->runqueue_node);
		}
	}

	mutex_unlock(&g2d->runqueue_mutex);
}

static void g2d_finish_event(struct g2d_data *g2d, u32 cmdlist_no)
{
	struct drm_device *drm_dev = g2d->drm_dev;
	struct g2d_runqueue_node *runqueue_node = g2d->runqueue_node;
	struct drm_exynos_pending_g2d_event *e;
	struct timespec64 now;

	if (list_empty(&runqueue_node->event_list))
		return;

	e = list_first_entry(&runqueue_node->event_list,
			     struct drm_exynos_pending_g2d_event, base.link);

	ktime_get_ts64(&now);
	e->event.tv_sec = now.tv_sec;
	e->event.tv_usec = now.tv_nsec / NSEC_PER_USEC;
	e->event.cmdlist_no = cmdlist_no;

	drm_send_event(drm_dev, &e->base);
}

static irqreturn_t g2d_irq_handler(int irq, void *dev_id)
{
	struct g2d_data *g2d = dev_id;
	u32 pending;

	pending = readl_relaxed(g2d->regs + G2D_INTC_PEND);
	if (pending)
		writel_relaxed(pending, g2d->regs + G2D_INTC_PEND);

	if (pending & G2D_INTP_GCMD_FIN) {
		u32 cmdlist_no = readl_relaxed(g2d->regs + G2D_DMA_STATUS);

		cmdlist_no = (cmdlist_no & G2D_DMA_LIST_DONE_COUNT) >>
						G2D_DMA_LIST_DONE_COUNT_OFFSET;

		g2d_finish_event(g2d, cmdlist_no);

		writel_relaxed(0, g2d->regs + G2D_DMA_HOLD_CMD);
		if (!(pending & G2D_INTP_ACMD_FIN)) {
			writel_relaxed(G2D_DMA_CONTINUE,
					g2d->regs + G2D_DMA_COMMAND);
		}
	}

	if (pending & G2D_INTP_ACMD_FIN) {
		clear_bit(G2D_BIT_ENGINE_BUSY, &g2d->flags);
		queue_work(g2d->g2d_workq, &g2d->runqueue_work);
	}

	return IRQ_HANDLED;
}

/**
 * g2d_wait_finish - wait for the G2D engine to finish the current runqueue node
 * @g2d: G2D state object
 * @file: if not zero, only wait if the current runqueue node belongs
 *        to the DRM file
 *
 * Should the engine not become idle after a 100ms timeout, a hardware
 * reset is issued.
 */
static void g2d_wait_finish(struct g2d_data *g2d, struct drm_file *file)
{
	struct device *dev = g2d->dev;

	struct g2d_runqueue_node *runqueue_node = NULL;
	unsigned int tries = 10;

	mutex_lock(&g2d->runqueue_mutex);

	/* If no node is currently processed, we have nothing to do. */
	if (!g2d->runqueue_node)
		goto out;

	runqueue_node = g2d->runqueue_node;

	/* Check if the currently processed item belongs to us. */
	if (file && runqueue_node->filp != file)
		goto out;

	mutex_unlock(&g2d->runqueue_mutex);

	/* Wait for the G2D engine to finish. */
	while (tries-- && (g2d->runqueue_node == runqueue_node))
		mdelay(10);

	mutex_lock(&g2d->runqueue_mutex);

	if (g2d->runqueue_node != runqueue_node)
		goto out;

	dev_err(dev, "wait timed out, resetting engine...\n");
	g2d_hw_reset(g2d);

	/*
	 * After the hardware reset of the engine we are going to loose
	 * the IRQ which triggers the PM runtime put().
	 * So do this manually here.
	 */
	pm_runtime_mark_last_busy(dev);
	pm_runtime_put_autosuspend(dev);

	complete(&runqueue_node->complete);
	if (runqueue_node->async)
		g2d_free_runqueue_node(g2d, runqueue_node);

out:
	mutex_unlock(&g2d->runqueue_mutex);
}

static int g2d_check_reg_offset(struct g2d_data *g2d,
				struct g2d_cmdlist_node *node,
				int nr, bool for_addr)
{
	struct g2d_cmdlist *cmdlist = node->cmdlist;
	int reg_offset;
	int index;
	int i;

	for (i = 0; i < nr; i++) {
		struct g2d_buf_info *buf_info = &node->buf_info;
		struct g2d_buf_desc *buf_desc;
		enum g2d_reg_type reg_type;
		unsigned long value;

		index = cmdlist->last - 2 * (i + 1);

		reg_offset = cmdlist->data[index] & ~0xfffff000;
		if (reg_offset < G2D_VALID_START || reg_offset > G2D_VALID_END)
			goto err;
		if (reg_offset % 4)
			goto err;

		switch (reg_offset) {
		case G2D_SRC_BASE_ADDR:
		case G2D_SRC_PLANE2_BASE_ADDR:
		case G2D_DST_BASE_ADDR:
		case G2D_DST_PLANE2_BASE_ADDR:
		case G2D_PAT_BASE_ADDR:
		case G2D_MSK_BASE_ADDR:
			if (!for_addr)
				goto err;

			reg_type = g2d_get_reg_type(g2d, reg_offset);

			/* check userptr buffer type. */
			if ((cmdlist->data[index] & ~0x7fffffff) >> 31) {
				buf_info->types[reg_type] = BUF_TYPE_USERPTR;
				cmdlist->data[index] &= ~G2D_BUF_USERPTR;
			} else
				buf_info->types[reg_type] = BUF_TYPE_GEM;
			break;
		case G2D_SRC_STRIDE:
		case G2D_DST_STRIDE:
			if (for_addr)
				goto err;

			reg_type = g2d_get_reg_type(g2d, reg_offset);

			buf_desc = &buf_info->descs[reg_type];
			buf_desc->stride = cmdlist->data[index + 1];
			break;
		case G2D_SRC_COLOR_MODE:
		case G2D_DST_COLOR_MODE:
			if (for_addr)
				goto err;

			reg_type = g2d_get_reg_type(g2d, reg_offset);

			buf_desc = &buf_info->descs[reg_type];
			value = cmdlist->data[index + 1];

			buf_desc->format = value & 0xf;
			break;
		case G2D_SRC_LEFT_TOP:
		case G2D_DST_LEFT_TOP:
			if (for_addr)
				goto err;

			reg_type = g2d_get_reg_type(g2d, reg_offset);

			buf_desc = &buf_info->descs[reg_type];
			value = cmdlist->data[index + 1];

			buf_desc->left_x = value & 0x1fff;
			buf_desc->top_y = (value & 0x1fff0000) >> 16;
			break;
		case G2D_SRC_RIGHT_BOTTOM:
		case G2D_DST_RIGHT_BOTTOM:
			if (for_addr)
				goto err;

			reg_type = g2d_get_reg_type(g2d, reg_offset);

			buf_desc = &buf_info->descs[reg_type];
			value = cmdlist->data[index + 1];

			buf_desc->right_x = value & 0x1fff;
			buf_desc->bottom_y = (value & 0x1fff0000) >> 16;
			break;
		default:
			if (for_addr)
				goto err;
			break;
		}
	}

	return 0;

err:
	dev_err(g2d->dev, "Bad register offset: 0x%lx\n", cmdlist->data[index]);
	return -EINVAL;
}

/* ioctl functions */
int exynos_g2d_get_ver_ioctl(struct drm_device *drm_dev, void *data,
			     struct drm_file *file)
{
	struct drm_exynos_g2d_get_ver *ver = data;

	ver->major = G2D_HW_MAJOR_VER;
	ver->minor = G2D_HW_MINOR_VER;

	return 0;
}

int exynos_g2d_set_cmdlist_ioctl(struct drm_device *drm_dev, void *data,
				 struct drm_file *file)
{
	struct drm_exynos_file_private *file_priv = file->driver_priv;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	struct g2d_data *g2d = dev_get_drvdata(priv->g2d_dev);
	struct drm_exynos_g2d_set_cmdlist *req = data;
	struct drm_exynos_g2d_cmd *cmd;
	struct drm_exynos_pending_g2d_event *e;
	struct g2d_cmdlist_node *node;
	struct g2d_cmdlist *cmdlist;
	int size;
	int ret;

	node = g2d_get_cmdlist(g2d);
	if (!node)
		return -ENOMEM;

	/*
	 * To avoid an integer overflow for the later size computations, we
	 * enforce a maximum number of submitted commands here. This limit is
	 * sufficient for all conceivable usage cases of the G2D.
	 */
	if (req->cmd_nr > G2D_CMDLIST_DATA_NUM ||
	    req->cmd_buf_nr > G2D_CMDLIST_DATA_NUM) {
		dev_err(g2d->dev, "number of submitted G2D commands exceeds limit\n");
		return -EINVAL;
	}

	node->event = NULL;

	if (req->event_type != G2D_EVENT_NOT) {
		e = kzalloc(sizeof(*node->event), GFP_KERNEL);
		if (!e) {
			ret = -ENOMEM;
			goto err;
		}

		e->event.base.type = DRM_EXYNOS_G2D_EVENT;
		e->event.base.length = sizeof(e->event);
		e->event.user_data = req->user_data;

		ret = drm_event_reserve_init(drm_dev, file, &e->base, &e->event.base);
		if (ret) {
			kfree(e);
			goto err;
		}

		node->event = e;
	}

	cmdlist = node->cmdlist;

	cmdlist->last = 0;

	/*
	 * If don't clear SFR registers, the cmdlist is affected by register
	 * values of previous cmdlist. G2D hw executes SFR clear command and
	 * a next command at the same time then the next command is ignored and
	 * is executed rightly from next next command, so needs a dummy command
	 * to next command of SFR clear command.
	 */
	cmdlist->data[cmdlist->last++] = G2D_SOFT_RESET;
	cmdlist->data[cmdlist->last++] = G2D_SFRCLEAR;
	cmdlist->data[cmdlist->last++] = G2D_SRC_BASE_ADDR;
	cmdlist->data[cmdlist->last++] = 0;

	/*
	 * 'LIST_HOLD' command should be set to the DMA_HOLD_CMD_REG
	 * and GCF bit should be set to INTEN register if user wants
	 * G2D interrupt event once current command list execution is
	 * finished.
	 * Otherwise only ACF bit should be set to INTEN register so
	 * that one interrupt is occurred after all command lists
	 * have been completed.
	 */
	if (node->event) {
		cmdlist->data[cmdlist->last++] = G2D_INTEN;
		cmdlist->data[cmdlist->last++] = G2D_INTEN_ACF | G2D_INTEN_GCF;
		cmdlist->data[cmdlist->last++] = G2D_DMA_HOLD_CMD;
		cmdlist->data[cmdlist->last++] = G2D_LIST_HOLD;
	} else {
		cmdlist->data[cmdlist->last++] = G2D_INTEN;
		cmdlist->data[cmdlist->last++] = G2D_INTEN_ACF;
	}

	/*
	 * Check the size of cmdlist. The 2 that is added last comes from
	 * the implicit G2D_BITBLT_START that is appended once we have
	 * checked all the submitted commands.
	 */
	size = cmdlist->last + req->cmd_nr * 2 + req->cmd_buf_nr * 2 + 2;
	if (size > G2D_CMDLIST_DATA_NUM) {
		dev_err(g2d->dev, "cmdlist size is too big\n");
		ret = -EINVAL;
		goto err_free_event;
	}

	cmd = (struct drm_exynos_g2d_cmd *)(unsigned long)req->cmd;

	if (copy_from_user(cmdlist->data + cmdlist->last,
				(void __user *)cmd,
				sizeof(*cmd) * req->cmd_nr)) {
		ret = -EFAULT;
		goto err_free_event;
	}
	cmdlist->last += req->cmd_nr * 2;

	ret = g2d_check_reg_offset(g2d, node, req->cmd_nr, false);
	if (ret < 0)
		goto err_free_event;

	node->buf_info.map_nr = req->cmd_buf_nr;
	if (req->cmd_buf_nr) {
		struct drm_exynos_g2d_cmd *cmd_buf;

		cmd_buf = (struct drm_exynos_g2d_cmd *)
				(unsigned long)req->cmd_buf;

		if (copy_from_user(cmdlist->data + cmdlist->last,
					(void __user *)cmd_buf,
					sizeof(*cmd_buf) * req->cmd_buf_nr)) {
			ret = -EFAULT;
			goto err_free_event;
		}
		cmdlist->last += req->cmd_buf_nr * 2;

		ret = g2d_check_reg_offset(g2d, node, req->cmd_buf_nr, true);
		if (ret < 0)
			goto err_free_event;

		ret = g2d_map_cmdlist_gem(g2d, node, drm_dev, file);
		if (ret < 0)
			goto err_unmap;
	}

	cmdlist->data[cmdlist->last++] = G2D_BITBLT_START;
	cmdlist->data[cmdlist->last++] = G2D_START_BITBLT;

	/* head */
	cmdlist->head = cmdlist->last / 2;

	/* tail */
	cmdlist->data[cmdlist->last] = 0;

	g2d_add_cmdlist_to_inuse(file_priv, node);

	return 0;

err_unmap:
	g2d_unmap_cmdlist_gem(g2d, node, file);
err_free_event:
	if (node->event)
		drm_event_cancel_free(drm_dev, &node->event->base);
err:
	g2d_put_cmdlist(g2d, node);
	return ret;
}

int exynos_g2d_exec_ioctl(struct drm_device *drm_dev, void *data,
			  struct drm_file *file)
{
	struct drm_exynos_file_private *file_priv = file->driver_priv;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	struct g2d_data *g2d = dev_get_drvdata(priv->g2d_dev);
	struct drm_exynos_g2d_exec *req = data;
	struct g2d_runqueue_node *runqueue_node;
	struct list_head *run_cmdlist;
	struct list_head *event_list;

	runqueue_node = kmem_cache_alloc(g2d->runqueue_slab, GFP_KERNEL);
	if (!runqueue_node)
		return -ENOMEM;

	run_cmdlist = &runqueue_node->run_cmdlist;
	event_list = &runqueue_node->event_list;
	INIT_LIST_HEAD(run_cmdlist);
	INIT_LIST_HEAD(event_list);
	init_completion(&runqueue_node->complete);
	runqueue_node->async = req->async;

	list_splice_init(&file_priv->inuse_cmdlist, run_cmdlist);
	list_splice_init(&file_priv->event_list, event_list);

	if (list_empty(run_cmdlist)) {
		dev_err(g2d->dev, "there is no inuse cmdlist\n");
		kmem_cache_free(g2d->runqueue_slab, runqueue_node);
		return -EPERM;
	}

	mutex_lock(&g2d->runqueue_mutex);
	runqueue_node->pid = current->pid;
	runqueue_node->filp = file;
	list_add_tail(&runqueue_node->list, &g2d->runqueue);
	mutex_unlock(&g2d->runqueue_mutex);

	/* Let the runqueue know that there is work to do. */
	queue_work(g2d->g2d_workq, &g2d->runqueue_work);

	if (runqueue_node->async)
		goto out;

	wait_for_completion(&runqueue_node->complete);
	g2d_free_runqueue_node(g2d, runqueue_node);

out:
	return 0;
}

int g2d_open(struct drm_device *drm_dev, struct drm_file *file)
{
	struct drm_exynos_file_private *file_priv = file->driver_priv;

	INIT_LIST_HEAD(&file_priv->inuse_cmdlist);
	INIT_LIST_HEAD(&file_priv->event_list);
	INIT_LIST_HEAD(&file_priv->userptr_list);

	return 0;
}

void g2d_close(struct drm_device *drm_dev, struct drm_file *file)
{
	struct drm_exynos_file_private *file_priv = file->driver_priv;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	struct g2d_data *g2d;
	struct g2d_cmdlist_node *node, *n;

	if (!priv->g2d_dev)
		return;

	g2d = dev_get_drvdata(priv->g2d_dev);

	/* Remove the runqueue nodes that belong to us. */
	mutex_lock(&g2d->runqueue_mutex);
	g2d_remove_runqueue_nodes(g2d, file);
	mutex_unlock(&g2d->runqueue_mutex);

	/*
	 * Wait for the runqueue worker to finish its current node.
	 * After this the engine should no longer be accessing any
	 * memory belonging to us.
	 */
	g2d_wait_finish(g2d, file);

	/*
	 * Even after the engine is idle, there might still be stale cmdlists
	 * (i.e. cmdlisst which we submitted but never executed) around, with
	 * their corresponding GEM/userptr buffers.
	 * Properly unmap these buffers here.
	 */
	mutex_lock(&g2d->cmdlist_mutex);
	list_for_each_entry_safe(node, n, &file_priv->inuse_cmdlist, list) {
		g2d_unmap_cmdlist_gem(g2d, node, file);
		list_move_tail(&node->list, &g2d->free_cmdlist);
	}
	mutex_unlock(&g2d->cmdlist_mutex);

	/* release all g2d_userptr in pool. */
	g2d_userptr_free_all(g2d, file);
}

static int g2d_bind(struct device *dev, struct device *master, void *data)
{
	struct g2d_data *g2d = dev_get_drvdata(dev);
	struct drm_device *drm_dev = data;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	int ret;

	g2d->drm_dev = drm_dev;

	/* allocate dma-aware cmdlist buffer. */
	ret = g2d_init_cmdlist(g2d);
	if (ret < 0) {
		dev_err(dev, "cmdlist init failed\n");
		return ret;
	}

	ret = exynos_drm_register_dma(drm_dev, dev, &g2d->dma_priv);
	if (ret < 0) {
		dev_err(dev, "failed to enable iommu.\n");
		g2d_fini_cmdlist(g2d);
		return ret;
	}
	priv->g2d_dev = dev;

	dev_info(dev, "The Exynos G2D (ver %d.%d) successfully registered.\n",
			G2D_HW_MAJOR_VER, G2D_HW_MINOR_VER);
	return 0;
}

static void g2d_unbind(struct device *dev, struct device *master, void *data)
{
	struct g2d_data *g2d = dev_get_drvdata(dev);
	struct drm_device *drm_dev = data;
	struct exynos_drm_private *priv = drm_dev->dev_private;

	/* Suspend operation and wait for engine idle. */
	set_bit(G2D_BIT_SUSPEND_RUNQUEUE, &g2d->flags);
	g2d_wait_finish(g2d, NULL);
	priv->g2d_dev = NULL;

	cancel_work_sync(&g2d->runqueue_work);
	exynos_drm_unregister_dma(g2d->drm_dev, dev, &g2d->dma_priv);
}

static const struct component_ops g2d_component_ops = {
	.bind	= g2d_bind,
	.unbind = g2d_unbind,
};

static int g2d_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct g2d_data *g2d;
	int ret;

	g2d = devm_kzalloc(dev, sizeof(*g2d), GFP_KERNEL);
	if (!g2d)
		return -ENOMEM;

	g2d->runqueue_slab = kmem_cache_create("g2d_runqueue_slab",
			sizeof(struct g2d_runqueue_node), 0, 0, NULL);
	if (!g2d->runqueue_slab)
		return -ENOMEM;

	g2d->dev = dev;

	g2d->g2d_workq = create_singlethread_workqueue("g2d");
	if (!g2d->g2d_workq) {
		dev_err(dev, "failed to create workqueue\n");
		ret = -EINVAL;
		goto err_destroy_slab;
	}

	INIT_WORK(&g2d->runqueue_work, g2d_runqueue_worker);
	INIT_LIST_HEAD(&g2d->free_cmdlist);
	INIT_LIST_HEAD(&g2d->runqueue);

	mutex_init(&g2d->cmdlist_mutex);
	mutex_init(&g2d->runqueue_mutex);

	g2d->gate_clk = devm_clk_get(dev, "fimg2d");
	if (IS_ERR(g2d->gate_clk)) {
		dev_err(dev, "failed to get gate clock\n");
		ret = PTR_ERR(g2d->gate_clk);
		goto err_destroy_workqueue;
	}

	pm_runtime_use_autosuspend(dev);
	pm_runtime_set_autosuspend_delay(dev, 2000);
	pm_runtime_enable(dev);
	clear_bit(G2D_BIT_SUSPEND_RUNQUEUE, &g2d->flags);
	clear_bit(G2D_BIT_ENGINE_BUSY, &g2d->flags);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	g2d->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(g2d->regs)) {
		ret = PTR_ERR(g2d->regs);
		goto err_put_clk;
	}

	g2d->irq = platform_get_irq(pdev, 0);
	if (g2d->irq < 0) {
		ret = g2d->irq;
		goto err_put_clk;
	}

	ret = devm_request_irq(dev, g2d->irq, g2d_irq_handler, 0,
								"drm_g2d", g2d);
	if (ret < 0) {
		dev_err(dev, "irq request failed\n");
		goto err_put_clk;
	}

	g2d->max_pool = MAX_POOL;

	platform_set_drvdata(pdev, g2d);

	ret = component_add(dev, &g2d_component_ops);
	if (ret < 0) {
		dev_err(dev, "failed to register drm g2d device\n");
		goto err_put_clk;
	}

	return 0;

err_put_clk:
	pm_runtime_disable(dev);
err_destroy_workqueue:
	destroy_workqueue(g2d->g2d_workq);
err_destroy_slab:
	kmem_cache_destroy(g2d->runqueue_slab);
	return ret;
}

static int g2d_remove(struct platform_device *pdev)
{
	struct g2d_data *g2d = platform_get_drvdata(pdev);

	component_del(&pdev->dev, &g2d_component_ops);

	/* There should be no locking needed here. */
	g2d_remove_runqueue_nodes(g2d, NULL);

	pm_runtime_dont_use_autosuspend(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	g2d_fini_cmdlist(g2d);
	destroy_workqueue(g2d->g2d_workq);
	kmem_cache_destroy(g2d->runqueue_slab);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int g2d_suspend(struct device *dev)
{
	struct g2d_data *g2d = dev_get_drvdata(dev);

	/*
	 * Suspend the runqueue worker operation and wait until the G2D
	 * engine is idle.
	 */
	set_bit(G2D_BIT_SUSPEND_RUNQUEUE, &g2d->flags);
	g2d_wait_finish(g2d, NULL);
	flush_work(&g2d->runqueue_work);

	return 0;
}

static int g2d_resume(struct device *dev)
{
	struct g2d_data *g2d = dev_get_drvdata(dev);

	clear_bit(G2D_BIT_SUSPEND_RUNQUEUE, &g2d->flags);
	queue_work(g2d->g2d_workq, &g2d->runqueue_work);

	return 0;
}
#endif

#ifdef CONFIG_PM
static int g2d_runtime_suspend(struct device *dev)
{
	struct g2d_data *g2d = dev_get_drvdata(dev);

	clk_disable_unprepare(g2d->gate_clk);

	return 0;
}

static int g2d_runtime_resume(struct device *dev)
{
	struct g2d_data *g2d = dev_get_drvdata(dev);
	int ret;

	ret = clk_prepare_enable(g2d->gate_clk);
	if (ret < 0)
		dev_warn(dev, "failed to enable clock.\n");

	return ret;
}
#endif

static const struct dev_pm_ops g2d_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(g2d_suspend, g2d_resume)
	SET_RUNTIME_PM_OPS(g2d_runtime_suspend, g2d_runtime_resume, NULL)
};

static const struct of_device_id exynos_g2d_match[] = {
	{ .compatible = "samsung,exynos5250-g2d" },
	{ .compatible = "samsung,exynos4212-g2d" },
	{},
};
MODULE_DEVICE_TABLE(of, exynos_g2d_match);

struct platform_driver g2d_driver = {
	.probe		= g2d_probe,
	.remove		= g2d_remove,
	.driver		= {
		.name	= "exynos-drm-g2d",
		.owner	= THIS_MODULE,
		.pm	= &g2d_pm_ops,
		.of_match_table = exynos_g2d_match,
	},
};
