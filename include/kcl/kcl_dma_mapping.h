/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_DMA_MAPPING_H
#define AMDKCL_DMA_MAPPING_H

#include <linux/dma-mapping.h>
#include <kcl/kcl_mem_encrypt.h>

/*
 * commit v4.8-11962-ga9a62c938441
 * dma-mapping: introduce the DMA_ATTR_NO_WARN attribute
 */
#ifndef DMA_ATTR_NO_WARN
#define DMA_ATTR_NO_WARN (0UL)
#endif

/*
* commit v5.3-rc1-57-g06532750010e
* dma-mapping: use dma_get_mask in dma_addressing_limited
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
#define AMDKCL_DMA_ADDRESSING_LIMITED_WORKAROUND
#endif

#ifdef HAVE_LINUX_DMA_ATTRS_H
static inline
void _kcl_convert_long_to_dma_attrs(struct dma_attrs *dma_attrs,
							unsigned long attrs)
{
	int i;

	init_dma_attrs(dma_attrs);

	for (i = 0; i < DMA_ATTR_MAX; i++) {
		if (attrs & (1 << i))
			dma_set_attr(i, dma_attrs);
	}
}

static inline
void *kcl_dma_alloc_attrs(struct device *dev, size_t size, dma_addr_t *dma_handle,
		gfp_t flag, unsigned long attrs)
{
	struct dma_attrs dma_attrs;

	_kcl_convert_long_to_dma_attrs(&dma_attrs, attrs);
	return dma_alloc_attrs(dev, size, dma_handle, flag, &dma_attrs);
}

static inline
void kcl_dma_free_attrs(struct device *dev, size_t size, void *cpu_addr,
		dma_addr_t dma_handle, unsigned long attrs)
{
	struct dma_attrs dma_attrs;

	_kcl_convert_long_to_dma_attrs(&dma_attrs, attrs);
	dma_free_attrs(dev, size, cpu_addr, dma_handle, &dma_attrs);
}
#else
static inline void *kcl_dma_alloc_attrs(struct device *dev, size_t size,
                                      dma_addr_t *dma_handle, gfp_t flag,
                                      unsigned long attrs)
{
	return dma_alloc_attrs(dev, size, dma_handle, flag, attrs);
}
static inline void kcl_dma_free_attrs(struct device *dev, size_t size, void *cpu_addr,
		dma_addr_t dma_handle, unsigned long attrs)
{
	return dma_free_attrs(dev, size, cpu_addr, dma_handle, attrs);
}
#endif

#ifndef HAVE_DMA_MAP_SGTABLE
#ifdef HAVE_LINUX_DMA_ATTRS_H
static inline
int _kcl_dma_map_sg_attrs(struct device *dev, struct scatterlist *sg, int nents,
		enum dma_data_direction dir, unsigned long attrs)
{
	struct dma_attrs dma_attrs;

	_kcl_convert_long_to_dma_attrs(&dma_attrs, attrs);
	return dma_map_sg_attrs(dev, sg, nents, dir, &dma_attrs);
}

static inline
void _kcl_dma_unmap_sg_attrs(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir,
				      unsigned long attrs)

{
	struct dma_attrs dma_attrs;

	_kcl_convert_long_to_dma_attrs(&dma_attrs, attrs);
	dma_unmap_sg_attrs(dev, sg, nents, dir, &dma_attrs);
}

#else
static inline
int _kcl_dma_map_sg_attrs(struct device *dev, struct scatterlist *sg, int nents,
		enum dma_data_direction dir, unsigned long attrs)
{
	return dma_map_sg_attrs(dev, sg, nents, dir, attrs);
}
static inline
void _kcl_dma_unmap_sg_attrs(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir,
				      unsigned long attrs)
{
	dma_unmap_sg_attrs(dev, sg, nents, dir, attrs);
}
#endif /* HAVE_LINUX_DMA_ATTRS_H */

static inline int dma_map_sgtable(struct device *dev, struct sg_table *sgt,
		enum dma_data_direction dir, unsigned long attrs)
{
	int nents;

	nents = _kcl_dma_map_sg_attrs(dev, sgt->sgl, sgt->orig_nents, dir, attrs);
	if (nents <= 0)
		return -EINVAL;
	sgt->nents = nents;
	return 0;
}

static inline void dma_unmap_sgtable(struct device *dev, struct sg_table *sgt,
		enum dma_data_direction dir, unsigned long attrs)
{
	_kcl_dma_unmap_sg_attrs(dev, sgt->sgl, sgt->orig_nents, dir, attrs);
}
#endif

#ifndef HAVE_DMA_MAP_RESOURCE
static inline dma_addr_t dma_map_resource(struct device *dev,
                                          phys_addr_t phys_addr,
                                          size_t size,
                                          enum dma_data_direction dir,
                                          unsigned long attrs)
{
        pr_warn_once("%s is not supported\n", __func__);

        return phys_addr;
}

static inline void dma_unmap_resource(struct device *dev, dma_addr_t addr,
                                      size_t size, enum dma_data_direction dir,
                                      unsigned long attrs)
{
        pr_warn_once("%s is not supported\n", __func__);
}
#endif

/*
 * v5.8-rc3-2-g68d237056e00 ("scatterlist: protect parameters of the sg_table related macros")
 * v5.7-rc5-33-g709d6d73c756 ("scatterlist: add generic wrappers for iterating over sgtable objects")
 * Copied from include/linux/scatterlist.h
 */
#ifndef for_each_sgtable_sg
#define for_each_sgtable_sg(sgt, sg, i)		\
	for_each_sg((sgt)->sgl, sg, (sgt)->orig_nents, i)
#endif

#endif
