/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright 2019 Linaro, Ltd, Rob Herring <robh@kernel.org> */

#ifndef __PANFROST_MMU_H__
#define __PANFROST_MMU_H__

struct panfrost_gem_mapping;
struct panfrost_file_priv;
struct panfrost_mmu;

int panfrost_mmu_map(struct panfrost_gem_mapping *mapping);
void panfrost_mmu_unmap(struct panfrost_gem_mapping *mapping);

int panfrost_mmu_init(struct panfrost_device *pfdev);
void panfrost_mmu_fini(struct panfrost_device *pfdev);
void panfrost_mmu_reset(struct panfrost_device *pfdev);

u32 panfrost_mmu_as_get(struct panfrost_device *pfdev, struct panfrost_mmu *mmu);
void panfrost_mmu_as_put(struct panfrost_device *pfdev, struct panfrost_mmu *mmu);

int panfrost_mmu_pgtable_alloc(struct panfrost_file_priv *priv);
void panfrost_mmu_pgtable_free(struct panfrost_file_priv *priv);

#endif
