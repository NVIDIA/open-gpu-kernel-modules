#ifndef __NOUVEAU_MEM_H__
#define __NOUVEAU_MEM_H__
#include <drm/ttm/ttm_bo_api.h>
struct ttm_tt;

#include <nvif/mem.h>
#include <nvif/vmm.h>

struct nouveau_mem {
	struct ttm_resource base;
	struct nouveau_cli *cli;
	u8 kind;
	u8 comp;
	struct nvif_mem mem;
	struct nvif_vma vma[2];
};

static inline struct nouveau_mem *
nouveau_mem(struct ttm_resource *reg)
{
	return container_of(reg, struct nouveau_mem, base);
}

int nouveau_mem_new(struct nouveau_cli *, u8 kind, u8 comp,
		    struct ttm_resource **);
void nouveau_mem_del(struct ttm_resource *);
int nouveau_mem_vram(struct ttm_resource *, bool contig, u8 page);
int nouveau_mem_host(struct ttm_resource *, struct ttm_tt *);
void nouveau_mem_fini(struct nouveau_mem *);
int nouveau_mem_map(struct nouveau_mem *, struct nvif_vmm *, struct nvif_vma *);
#endif
