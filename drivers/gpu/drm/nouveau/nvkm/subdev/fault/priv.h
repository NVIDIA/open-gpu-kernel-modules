#ifndef __NVKM_FAULT_PRIV_H__
#define __NVKM_FAULT_PRIV_H__
#define nvkm_fault_buffer(p) container_of((p), struct nvkm_fault_buffer, object)
#define nvkm_fault(p) container_of((p), struct nvkm_fault, subdev)
#include <subdev/fault.h>

#include <core/event.h>
#include <core/object.h>

struct nvkm_fault_buffer {
	struct nvkm_object object;
	struct nvkm_fault *fault;
	int id;
	int entries;
	u32 get;
	u32 put;
	struct nvkm_memory *mem;
	u64 addr;
};

int nvkm_fault_new_(const struct nvkm_fault_func *, struct nvkm_device *, enum nvkm_subdev_type,
		    int inst, struct nvkm_fault **);

struct nvkm_fault_func {
	int (*oneinit)(struct nvkm_fault *);
	void (*init)(struct nvkm_fault *);
	void (*fini)(struct nvkm_fault *);
	void (*intr)(struct nvkm_fault *);
	struct {
		int nr;
		u32 entry_size;
		void (*info)(struct nvkm_fault_buffer *);
		u64 (*pin)(struct nvkm_fault_buffer *);
		void (*init)(struct nvkm_fault_buffer *);
		void (*fini)(struct nvkm_fault_buffer *);
		void (*intr)(struct nvkm_fault_buffer *, bool enable);
	} buffer;
	struct {
		struct nvkm_sclass base;
		int rp;
	} user;
};

void gp100_fault_buffer_intr(struct nvkm_fault_buffer *, bool enable);
void gp100_fault_buffer_fini(struct nvkm_fault_buffer *);
void gp100_fault_buffer_init(struct nvkm_fault_buffer *);
u64 gp100_fault_buffer_pin(struct nvkm_fault_buffer *);
void gp100_fault_buffer_info(struct nvkm_fault_buffer *);
void gp100_fault_intr(struct nvkm_fault *);

u64 gp10b_fault_buffer_pin(struct nvkm_fault_buffer *);

int gv100_fault_oneinit(struct nvkm_fault *);

int nvkm_ufault_new(struct nvkm_device *, const struct nvkm_oclass *,
		    void *, u32, struct nvkm_object **);
#endif
