/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2005-2014 Brocade Communications Systems, Inc.
 * Copyright (c) 2014- QLogic Corporation.
 * All rights reserved
 * www.qlogic.com
 *
 * Linux driver for QLogic BR-series Fibre Channel Host Bus Adapter.
 */

#ifndef __BFAD_IM_H__
#define __BFAD_IM_H__

#include "bfa_fcs.h"

#define FCPI_NAME " fcpim"

#ifndef KOBJ_NAME_LEN
#define KOBJ_NAME_LEN           20
#endif

bfa_status_t bfad_im_module_init(void);
void bfad_im_module_exit(void);
bfa_status_t bfad_im_probe(struct bfad_s *bfad);
void bfad_im_probe_undo(struct bfad_s *bfad);
bfa_status_t bfad_im_port_new(struct bfad_s *bfad, struct bfad_port_s *port);
void bfad_im_port_delete(struct bfad_s *bfad, struct bfad_port_s *port);
void bfad_im_port_clean(struct bfad_im_port_s *im_port);
int  bfad_im_scsi_host_alloc(struct bfad_s *bfad,
		struct bfad_im_port_s *im_port, struct device *dev);
void bfad_im_scsi_host_free(struct bfad_s *bfad,
				struct bfad_im_port_s *im_port);
u32 bfad_im_supported_speeds(struct bfa_s *bfa);

#define MAX_FCP_TARGET 1024
#define MAX_FCP_LUN 16384
#define BFAD_TARGET_RESET_TMO 60
#define BFAD_LUN_RESET_TMO 60
#define BFA_QUEUE_FULL_RAMP_UP_TIME 120

/*
 * itnim flags
 */
#define IO_DONE_BIT			0

struct bfad_itnim_data_s {
	struct bfad_itnim_s *itnim;
};

struct bfad_im_port_s {
	struct bfad_s         *bfad;
	struct bfad_port_s    *port;
	struct work_struct port_delete_work;
	int             idr_id;
	u16        cur_scsi_id;
	u16	flags;
	struct list_head binding_list;
	struct Scsi_Host *shost;
	struct list_head itnim_mapped_list;
	struct fc_vport *fc_vport;
};

struct bfad_im_port_pointer {
	struct bfad_im_port_s *p;
};

static inline struct bfad_im_port_s *bfad_get_im_port(struct Scsi_Host *host)
{
	struct bfad_im_port_pointer *im_portp = shost_priv(host);
	return im_portp->p;
}

enum bfad_itnim_state {
	ITNIM_STATE_NONE,
	ITNIM_STATE_ONLINE,
	ITNIM_STATE_OFFLINE_PENDING,
	ITNIM_STATE_OFFLINE,
	ITNIM_STATE_TIMEOUT,
	ITNIM_STATE_FREE,
};

/*
 * Per itnim data structure
 */
struct bfad_itnim_s {
	struct list_head list_entry;
	struct bfa_fcs_itnim_s fcs_itnim;
	struct work_struct itnim_work;
	u32        flags;
	enum bfad_itnim_state state;
	struct bfad_im_s *im;
	struct bfad_im_port_s *im_port;
	struct bfad_rport_s *drv_rport;
	struct fc_rport *fc_rport;
	struct bfa_itnim_s *bfa_itnim;
	u16        scsi_tgt_id;
	u16	   channel;
	u16        queue_work;
	unsigned long	last_ramp_up_time;
	unsigned long	last_queue_full_time;
};

enum bfad_binding_type {
	FCP_PWWN_BINDING = 0x1,
	FCP_NWWN_BINDING = 0x2,
	FCP_FCID_BINDING = 0x3,
};

struct bfad_fcp_binding {
	struct list_head list_entry;
	enum bfad_binding_type binding_type;
	u16        scsi_target_id;
	u32        fc_id;
	wwn_t           nwwn;
	wwn_t           pwwn;
};

struct bfad_im_s {
	struct bfad_s         *bfad;
	struct workqueue_struct *drv_workq;
	char            drv_workq_name[KOBJ_NAME_LEN];
	struct work_struct	aen_im_notify_work;
};

#define bfad_get_aen_entry(_drv, _entry) do {				\
	unsigned long	_flags;						\
	spin_lock_irqsave(&(_drv)->bfad_aen_spinlock, _flags);		\
	bfa_q_deq(&(_drv)->free_aen_q, &(_entry));			\
	if (_entry)							\
		list_add_tail(&(_entry)->qe, &(_drv)->active_aen_q);	\
	spin_unlock_irqrestore(&(_drv)->bfad_aen_spinlock, _flags);	\
} while (0)

/* post fc_host vendor event */
static inline void bfad_im_post_vendor_event(struct bfa_aen_entry_s *entry,
					     struct bfad_s *drv, int cnt,
					     enum bfa_aen_category cat,
					     int evt)
{
	struct timespec64 ts;

	ktime_get_real_ts64(&ts);
	/*
	 * 'unsigned long aen_tv_sec' overflows in y2106 on 32-bit
	 * architectures, or in 2038 if user space interprets it
	 * as 'signed'.
	 */
	entry->aen_tv_sec = ts.tv_sec;
	entry->aen_tv_usec = ts.tv_nsec / NSEC_PER_USEC;
	entry->bfad_num = drv->inst_no;
	entry->seq_num = cnt;
	entry->aen_category = cat;
	entry->aen_type = evt;
	if (drv->bfad_flags & BFAD_FC4_PROBE_DONE)
		queue_work(drv->im->drv_workq, &drv->im->aen_im_notify_work);
}

struct Scsi_Host *bfad_scsi_host_alloc(struct bfad_im_port_s *im_port,
				struct bfad_s *);
bfa_status_t bfad_thread_workq(struct bfad_s *bfad);
void bfad_destroy_workq(struct bfad_im_s *im);
void bfad_fc_host_init(struct bfad_im_port_s *im_port);
void bfad_scsi_host_free(struct bfad_s *bfad,
				 struct bfad_im_port_s *im_port);
void bfad_ramp_up_qdepth(struct bfad_itnim_s *itnim,
				 struct scsi_device *sdev);
void bfad_handle_qfull(struct bfad_itnim_s *itnim, struct scsi_device *sdev);
struct bfad_itnim_s *bfad_get_itnim(struct bfad_im_port_s *im_port, int id);

extern struct scsi_host_template bfad_im_scsi_host_template;
extern struct scsi_host_template bfad_im_vport_template;
extern struct fc_function_template bfad_im_fc_function_template;
extern struct fc_function_template bfad_im_vport_fc_function_template;
extern struct scsi_transport_template *bfad_im_scsi_transport_template;
extern struct scsi_transport_template *bfad_im_scsi_vport_transport_template;

extern struct device_attribute *bfad_im_host_attrs[];
extern struct device_attribute *bfad_im_vport_attrs[];

irqreturn_t bfad_intx(int irq, void *dev_id);

int bfad_im_bsg_request(struct bsg_job *job);
int bfad_im_bsg_timeout(struct bsg_job *job);

/*
 * Macro to set the SCSI device sdev_bflags - sdev_bflags are used by the
 * SCSI mid-layer to choose LUN Scanning mode REPORT_LUNS vs. Sequential Scan
 *
 * Internally iterate's over all the ITNIM's part of the im_port & set's the
 * sdev_bflags for the scsi_device associated with LUN #0.
 */
#define bfad_reset_sdev_bflags(__im_port, __lunmask_cfg) do {		\
	struct scsi_device *__sdev = NULL;				\
	struct bfad_itnim_s *__itnim = NULL;				\
	u32 scan_flags = BLIST_NOREPORTLUN | BLIST_SPARSELUN;		\
	list_for_each_entry(__itnim, &((__im_port)->itnim_mapped_list),	\
			    list_entry) {				\
		__sdev = scsi_device_lookup((__im_port)->shost,		\
					    __itnim->channel,		\
					    __itnim->scsi_tgt_id, 0);	\
		if (__sdev) {						\
			if ((__lunmask_cfg) == BFA_TRUE)		\
				__sdev->sdev_bflags |= scan_flags;	\
			else						\
				__sdev->sdev_bflags &= ~scan_flags;	\
			scsi_device_put(__sdev);			\
		}							\
	}								\
} while (0)

#endif
