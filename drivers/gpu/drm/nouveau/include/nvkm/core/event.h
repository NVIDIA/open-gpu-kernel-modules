/* SPDX-License-Identifier: MIT */
#ifndef __NVKM_EVENT_H__
#define __NVKM_EVENT_H__
#include <core/os.h>
struct nvkm_notify;
struct nvkm_object;

struct nvkm_event {
	const struct nvkm_event_func *func;

	int types_nr;
	int index_nr;

	spinlock_t refs_lock;
	spinlock_t list_lock;
	struct list_head list;
	int *refs;
};

struct nvkm_event_func {
	int  (*ctor)(struct nvkm_object *, void *data, u32 size,
		     struct nvkm_notify *);
	void (*send)(void *data, u32 size, struct nvkm_notify *);
	void (*init)(struct nvkm_event *, int type, int index);
	void (*fini)(struct nvkm_event *, int type, int index);
};

int  nvkm_event_init(const struct nvkm_event_func *func, int types_nr,
		     int index_nr, struct nvkm_event *);
void nvkm_event_fini(struct nvkm_event *);
void nvkm_event_get(struct nvkm_event *, u32 types, int index);
void nvkm_event_put(struct nvkm_event *, u32 types, int index);
void nvkm_event_send(struct nvkm_event *, u32 types, int index,
		     void *data, u32 size);
#endif
