/* SPDX-License-Identifier: MIT */
#ifndef __NVIF_NOTIFY_H__
#define __NVIF_NOTIFY_H__

struct nvif_notify {
	struct nvif_object *object;
	const char *name;
	int index;

#define NVIF_NOTIFY_USER 0
#define NVIF_NOTIFY_WORK 1
	unsigned long flags;
	atomic_t putcnt;
	void (*dtor)(struct nvif_notify *);
#define NVIF_NOTIFY_DROP 0
#define NVIF_NOTIFY_KEEP 1
	int  (*func)(struct nvif_notify *);

	/* this is const for a *very* good reason - the data might be on the
	 * stack from an irq handler.  if you're not nvif/notify.c then you
	 * should probably think twice before casting it away...
	 */
	const void *data;
	u32 size;
	struct work_struct work;
};

int  nvif_notify_ctor(struct nvif_object *, const char *name,
		      int (*func)(struct nvif_notify *), bool work, u8 type,
		      void *data, u32 size, u32 reply, struct nvif_notify *);
int  nvif_notify_dtor(struct nvif_notify *);
int  nvif_notify_get(struct nvif_notify *);
int  nvif_notify_put(struct nvif_notify *);
int  nvif_notify(const void *, u32, const void *, u32);
#endif
