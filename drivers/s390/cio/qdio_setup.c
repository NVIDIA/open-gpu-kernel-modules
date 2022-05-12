// SPDX-License-Identifier: GPL-2.0
/*
 * qdio queue initialization
 *
 * Copyright IBM Corp. 2008
 * Author(s): Jan Glauber <jang@linux.vnet.ibm.com>
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/io.h>

#include <asm/ebcdic.h>
#include <asm/qdio.h>

#include "cio.h"
#include "css.h"
#include "device.h"
#include "ioasm.h"
#include "chsc.h"
#include "qdio.h"
#include "qdio_debug.h"

#define QBUFF_PER_PAGE (PAGE_SIZE / sizeof(struct qdio_buffer))

static struct kmem_cache *qdio_q_cache;
static struct kmem_cache *qdio_aob_cache;

struct qaob *qdio_allocate_aob(void)
{
	return kmem_cache_zalloc(qdio_aob_cache, GFP_ATOMIC);
}
EXPORT_SYMBOL_GPL(qdio_allocate_aob);

void qdio_release_aob(struct qaob *aob)
{
	kmem_cache_free(qdio_aob_cache, aob);
}
EXPORT_SYMBOL_GPL(qdio_release_aob);

/**
 * qdio_free_buffers() - free qdio buffers
 * @buf: array of pointers to qdio buffers
 * @count: number of qdio buffers to free
 */
void qdio_free_buffers(struct qdio_buffer **buf, unsigned int count)
{
	int pos;

	for (pos = 0; pos < count; pos += QBUFF_PER_PAGE)
		free_page((unsigned long) buf[pos]);
}
EXPORT_SYMBOL_GPL(qdio_free_buffers);

/**
 * qdio_alloc_buffers() - allocate qdio buffers
 * @buf: array of pointers to qdio buffers
 * @count: number of qdio buffers to allocate
 */
int qdio_alloc_buffers(struct qdio_buffer **buf, unsigned int count)
{
	int pos;

	for (pos = 0; pos < count; pos += QBUFF_PER_PAGE) {
		buf[pos] = (void *) get_zeroed_page(GFP_KERNEL);
		if (!buf[pos]) {
			qdio_free_buffers(buf, count);
			return -ENOMEM;
		}
	}
	for (pos = 0; pos < count; pos++)
		if (pos % QBUFF_PER_PAGE)
			buf[pos] = buf[pos - 1] + 1;
	return 0;
}
EXPORT_SYMBOL_GPL(qdio_alloc_buffers);

/**
 * qdio_reset_buffers() - reset qdio buffers
 * @buf: array of pointers to qdio buffers
 * @count: number of qdio buffers that will be zeroed
 */
void qdio_reset_buffers(struct qdio_buffer **buf, unsigned int count)
{
	int pos;

	for (pos = 0; pos < count; pos++)
		memset(buf[pos], 0, sizeof(struct qdio_buffer));
}
EXPORT_SYMBOL_GPL(qdio_reset_buffers);

/*
 * qebsm is only available under 64bit but the adapter sets the feature
 * flag anyway, so we manually override it.
 */
static inline int qebsm_possible(void)
{
	return css_general_characteristics.qebsm;
}

/*
 * qib_param_field: pointer to 128 bytes or NULL, if no param field
 * nr_input_qs: pointer to nr_queues*128 words of data or NULL
 */
static void set_impl_params(struct qdio_irq *irq_ptr,
			    unsigned int qib_param_field_format,
			    unsigned char *qib_param_field,
			    unsigned long *input_slib_elements,
			    unsigned long *output_slib_elements)
{
	struct qdio_q *q;
	int i, j;

	if (!irq_ptr)
		return;

	irq_ptr->qib.pfmt = qib_param_field_format;
	if (qib_param_field)
		memcpy(irq_ptr->qib.parm, qib_param_field,
		       sizeof(irq_ptr->qib.parm));

	if (!input_slib_elements)
		goto output;

	for_each_input_queue(irq_ptr, q, i) {
		for (j = 0; j < QDIO_MAX_BUFFERS_PER_Q; j++)
			q->slib->slibe[j].parms =
				input_slib_elements[i * QDIO_MAX_BUFFERS_PER_Q + j];
	}
output:
	if (!output_slib_elements)
		return;

	for_each_output_queue(irq_ptr, q, i) {
		for (j = 0; j < QDIO_MAX_BUFFERS_PER_Q; j++)
			q->slib->slibe[j].parms =
				output_slib_elements[i * QDIO_MAX_BUFFERS_PER_Q + j];
	}
}

static void __qdio_free_queues(struct qdio_q **queues, unsigned int count)
{
	struct qdio_q *q;
	unsigned int i;

	for (i = 0; i < count; i++) {
		q = queues[i];
		free_page((unsigned long) q->slib);
		kmem_cache_free(qdio_q_cache, q);
	}
}

void qdio_free_queues(struct qdio_irq *irq_ptr)
{
	__qdio_free_queues(irq_ptr->input_qs, irq_ptr->max_input_qs);
	irq_ptr->max_input_qs = 0;

	__qdio_free_queues(irq_ptr->output_qs, irq_ptr->max_output_qs);
	irq_ptr->max_output_qs = 0;
}

static int __qdio_allocate_qs(struct qdio_q **irq_ptr_qs, int nr_queues)
{
	struct qdio_q *q;
	int i;

	for (i = 0; i < nr_queues; i++) {
		q = kmem_cache_zalloc(qdio_q_cache, GFP_KERNEL);
		if (!q) {
			__qdio_free_queues(irq_ptr_qs, i);
			return -ENOMEM;
		}

		q->slib = (struct slib *) __get_free_page(GFP_KERNEL);
		if (!q->slib) {
			kmem_cache_free(qdio_q_cache, q);
			__qdio_free_queues(irq_ptr_qs, i);
			return -ENOMEM;
		}
		irq_ptr_qs[i] = q;
	}
	return 0;
}

int qdio_allocate_qs(struct qdio_irq *irq_ptr, int nr_input_qs, int nr_output_qs)
{
	int rc;

	rc = __qdio_allocate_qs(irq_ptr->input_qs, nr_input_qs);
	if (rc)
		return rc;

	rc = __qdio_allocate_qs(irq_ptr->output_qs, nr_output_qs);
	if (rc) {
		__qdio_free_queues(irq_ptr->input_qs, nr_input_qs);
		return rc;
	}

	irq_ptr->max_input_qs = nr_input_qs;
	irq_ptr->max_output_qs = nr_output_qs;
	return 0;
}

static void setup_queues_misc(struct qdio_q *q, struct qdio_irq *irq_ptr,
			      qdio_handler_t *handler, int i)
{
	struct slib *slib = q->slib;

	/* queue must be cleared for qdio_establish */
	memset(q, 0, sizeof(*q));
	memset(slib, 0, PAGE_SIZE);
	q->slib = slib;
	q->irq_ptr = irq_ptr;
	q->mask = 1 << (31 - i);
	q->nr = i;
	q->handler = handler;
}

static void setup_storage_lists(struct qdio_q *q, struct qdio_irq *irq_ptr,
				struct qdio_buffer **sbals_array, int i)
{
	struct qdio_q *prev;
	int j;

	DBF_HEX(&q, sizeof(void *));
	q->sl = (struct sl *)((char *)q->slib + PAGE_SIZE / 2);

	/* fill in sbal */
	for (j = 0; j < QDIO_MAX_BUFFERS_PER_Q; j++)
		q->sbal[j] = *sbals_array++;

	/* fill in slib */
	if (i > 0) {
		prev = (q->is_input_q) ? irq_ptr->input_qs[i - 1]
			: irq_ptr->output_qs[i - 1];
		prev->slib->nsliba = (unsigned long)q->slib;
	}

	q->slib->sla = (unsigned long)q->sl;
	q->slib->slsba = (unsigned long)&q->slsb.val[0];

	/* fill in sl */
	for (j = 0; j < QDIO_MAX_BUFFERS_PER_Q; j++)
		q->sl->element[j].sbal = virt_to_phys(q->sbal[j]);
}

static void setup_queues(struct qdio_irq *irq_ptr,
			 struct qdio_initialize *qdio_init)
{
	struct qdio_q *q;
	int i;

	for_each_input_queue(irq_ptr, q, i) {
		DBF_EVENT("inq:%1d", i);
		setup_queues_misc(q, irq_ptr, qdio_init->input_handler, i);

		q->is_input_q = 1;

		setup_storage_lists(q, irq_ptr,
				    qdio_init->input_sbal_addr_array[i], i);
	}

	for_each_output_queue(irq_ptr, q, i) {
		DBF_EVENT("outq:%1d", i);
		setup_queues_misc(q, irq_ptr, qdio_init->output_handler, i);

		q->is_input_q = 0;
		setup_storage_lists(q, irq_ptr,
				    qdio_init->output_sbal_addr_array[i], i);

		tasklet_setup(&q->u.out.tasklet, qdio_outbound_tasklet);
		timer_setup(&q->u.out.timer, qdio_outbound_timer, 0);
	}
}

static void process_ac_flags(struct qdio_irq *irq_ptr, unsigned char qdioac)
{
	if (qdioac & AC1_SIGA_INPUT_NEEDED)
		irq_ptr->siga_flag.input = 1;
	if (qdioac & AC1_SIGA_OUTPUT_NEEDED)
		irq_ptr->siga_flag.output = 1;
	if (qdioac & AC1_SIGA_SYNC_NEEDED)
		irq_ptr->siga_flag.sync = 1;
	if (!(qdioac & AC1_AUTOMATIC_SYNC_ON_THININT))
		irq_ptr->siga_flag.sync_after_ai = 1;
	if (!(qdioac & AC1_AUTOMATIC_SYNC_ON_OUT_PCI))
		irq_ptr->siga_flag.sync_out_after_pci = 1;
}

static void check_and_setup_qebsm(struct qdio_irq *irq_ptr,
				  unsigned char qdioac, unsigned long token)
{
	if (!(irq_ptr->qib.rflags & QIB_RFLAGS_ENABLE_QEBSM))
		goto no_qebsm;
	if (!(qdioac & AC1_SC_QEBSM_AVAILABLE) ||
	    (!(qdioac & AC1_SC_QEBSM_ENABLED)))
		goto no_qebsm;

	irq_ptr->sch_token = token;

	DBF_EVENT("V=V:1");
	DBF_EVENT("%8lx", irq_ptr->sch_token);
	return;

no_qebsm:
	irq_ptr->sch_token = 0;
	irq_ptr->qib.rflags &= ~QIB_RFLAGS_ENABLE_QEBSM;
	DBF_EVENT("noV=V");
}

/*
 * If there is a qdio_irq we use the chsc_page and store the information
 * in the qdio_irq, otherwise we copy it to the specified structure.
 */
int qdio_setup_get_ssqd(struct qdio_irq *irq_ptr,
			struct subchannel_id *schid,
			struct qdio_ssqd_desc *data)
{
	struct chsc_ssqd_area *ssqd;
	int rc;

	DBF_EVENT("getssqd:%4x", schid->sch_no);
	if (!irq_ptr) {
		ssqd = (struct chsc_ssqd_area *)__get_free_page(GFP_KERNEL);
		if (!ssqd)
			return -ENOMEM;
	} else {
		ssqd = (struct chsc_ssqd_area *)irq_ptr->chsc_page;
	}

	rc = chsc_ssqd(*schid, ssqd);
	if (rc)
		goto out;

	if (!(ssqd->qdio_ssqd.flags & CHSC_FLAG_QDIO_CAPABILITY) ||
	    !(ssqd->qdio_ssqd.flags & CHSC_FLAG_VALIDITY) ||
	    (ssqd->qdio_ssqd.sch != schid->sch_no))
		rc = -EINVAL;

	if (!rc)
		memcpy(data, &ssqd->qdio_ssqd, sizeof(*data));

out:
	if (!irq_ptr)
		free_page((unsigned long)ssqd);

	return rc;
}

void qdio_setup_ssqd_info(struct qdio_irq *irq_ptr)
{
	unsigned char qdioac;
	int rc;

	rc = qdio_setup_get_ssqd(irq_ptr, &irq_ptr->schid, &irq_ptr->ssqd_desc);
	if (rc) {
		DBF_ERROR("%4x ssqd ERR", irq_ptr->schid.sch_no);
		DBF_ERROR("rc:%x", rc);
		/* all flags set, worst case */
		qdioac = AC1_SIGA_INPUT_NEEDED | AC1_SIGA_OUTPUT_NEEDED |
			 AC1_SIGA_SYNC_NEEDED;
	} else
		qdioac = irq_ptr->ssqd_desc.qdioac1;

	check_and_setup_qebsm(irq_ptr, qdioac, irq_ptr->ssqd_desc.sch_token);
	process_ac_flags(irq_ptr, qdioac);
	DBF_EVENT("ac 1:%2x 2:%4x", qdioac, irq_ptr->ssqd_desc.qdioac2);
	DBF_EVENT("3:%4x qib:%4x", irq_ptr->ssqd_desc.qdioac3, irq_ptr->qib.ac);
}

static void qdio_fill_qdr_desc(struct qdesfmt0 *desc, struct qdio_q *queue)
{
	desc->sliba = virt_to_phys(queue->slib);
	desc->sla = virt_to_phys(queue->sl);
	desc->slsba = virt_to_phys(&queue->slsb);

	desc->akey = PAGE_DEFAULT_KEY >> 4;
	desc->bkey = PAGE_DEFAULT_KEY >> 4;
	desc->ckey = PAGE_DEFAULT_KEY >> 4;
	desc->dkey = PAGE_DEFAULT_KEY >> 4;
}

static void setup_qdr(struct qdio_irq *irq_ptr,
		      struct qdio_initialize *qdio_init)
{
	struct qdesfmt0 *desc = &irq_ptr->qdr->qdf0[0];
	int i;

	irq_ptr->qdr->qfmt = qdio_init->q_format;
	irq_ptr->qdr->ac = qdio_init->qdr_ac;
	irq_ptr->qdr->iqdcnt = qdio_init->no_input_qs;
	irq_ptr->qdr->oqdcnt = qdio_init->no_output_qs;
	irq_ptr->qdr->iqdsz = sizeof(struct qdesfmt0) / 4; /* size in words */
	irq_ptr->qdr->oqdsz = sizeof(struct qdesfmt0) / 4;
	irq_ptr->qdr->qiba = virt_to_phys(&irq_ptr->qib);
	irq_ptr->qdr->qkey = PAGE_DEFAULT_KEY >> 4;

	for (i = 0; i < qdio_init->no_input_qs; i++)
		qdio_fill_qdr_desc(desc++, irq_ptr->input_qs[i]);

	for (i = 0; i < qdio_init->no_output_qs; i++)
		qdio_fill_qdr_desc(desc++, irq_ptr->output_qs[i]);
}

static void setup_qib(struct qdio_irq *irq_ptr,
		      struct qdio_initialize *init_data)
{
	if (qebsm_possible())
		irq_ptr->qib.rflags |= QIB_RFLAGS_ENABLE_QEBSM;

	irq_ptr->qib.rflags |= init_data->qib_rflags;

	irq_ptr->qib.qfmt = init_data->q_format;
	if (init_data->no_input_qs)
		irq_ptr->qib.isliba =
			(unsigned long)(irq_ptr->input_qs[0]->slib);
	if (init_data->no_output_qs)
		irq_ptr->qib.osliba =
			(unsigned long)(irq_ptr->output_qs[0]->slib);
	memcpy(irq_ptr->qib.ebcnam, dev_name(&irq_ptr->cdev->dev), 8);
	ASCEBC(irq_ptr->qib.ebcnam, 8);
}

int qdio_setup_irq(struct qdio_irq *irq_ptr, struct qdio_initialize *init_data)
{
	struct ccw_device *cdev = irq_ptr->cdev;
	struct ciw *ciw;

	memset(&irq_ptr->qib, 0, sizeof(irq_ptr->qib));
	memset(&irq_ptr->siga_flag, 0, sizeof(irq_ptr->siga_flag));
	memset(&irq_ptr->ccw, 0, sizeof(irq_ptr->ccw));
	memset(&irq_ptr->ssqd_desc, 0, sizeof(irq_ptr->ssqd_desc));
	memset(&irq_ptr->perf_stat, 0, sizeof(irq_ptr->perf_stat));

	irq_ptr->debugfs_dev = NULL;
	irq_ptr->sch_token = irq_ptr->perf_stat_enabled = 0;
	irq_ptr->state = QDIO_IRQ_STATE_INACTIVE;

	/* wipes qib.ac, required by ar7063 */
	memset(irq_ptr->qdr, 0, sizeof(struct qdr));

	irq_ptr->int_parm = init_data->int_parm;
	irq_ptr->nr_input_qs = init_data->no_input_qs;
	irq_ptr->nr_output_qs = init_data->no_output_qs;
	irq_ptr->scan_threshold = init_data->scan_threshold;
	ccw_device_get_schid(cdev, &irq_ptr->schid);
	setup_queues(irq_ptr, init_data);

	irq_ptr->irq_poll = init_data->irq_poll;
	set_bit(QDIO_IRQ_DISABLED, &irq_ptr->poll_state);

	setup_qib(irq_ptr, init_data);
	set_impl_params(irq_ptr, init_data->qib_param_field_format,
			init_data->qib_param_field,
			init_data->input_slib_elements,
			init_data->output_slib_elements);

	/* fill input and output descriptors */
	setup_qdr(irq_ptr, init_data);

	/* qdr, qib, sls, slsbs, slibs, sbales are filled now */

	/* set our IRQ handler */
	spin_lock_irq(get_ccwdev_lock(cdev));
	irq_ptr->orig_handler = cdev->handler;
	cdev->handler = qdio_int_handler;
	spin_unlock_irq(get_ccwdev_lock(cdev));

	/* get qdio commands */
	ciw = ccw_device_get_ciw(cdev, CIW_TYPE_EQUEUE);
	if (!ciw) {
		DBF_ERROR("%4x NO EQ", irq_ptr->schid.sch_no);
		return -EINVAL;
	}
	irq_ptr->equeue = *ciw;

	ciw = ccw_device_get_ciw(cdev, CIW_TYPE_AQUEUE);
	if (!ciw) {
		DBF_ERROR("%4x NO AQ", irq_ptr->schid.sch_no);
		return -EINVAL;
	}
	irq_ptr->aqueue = *ciw;

	return 0;
}

void qdio_shutdown_irq(struct qdio_irq *irq)
{
	struct ccw_device *cdev = irq->cdev;

	/* restore IRQ handler */
	spin_lock_irq(get_ccwdev_lock(cdev));
	cdev->handler = irq->orig_handler;
	cdev->private->intparm = 0;
	spin_unlock_irq(get_ccwdev_lock(cdev));
}

void qdio_print_subchannel_info(struct qdio_irq *irq_ptr)
{
	char s[80];

	snprintf(s, 80, "qdio: %s %s on SC %x using "
		 "AI:%d QEBSM:%d PRI:%d TDD:%d SIGA:%s%s%s%s%s\n",
		 dev_name(&irq_ptr->cdev->dev),
		 (irq_ptr->qib.qfmt == QDIO_QETH_QFMT) ? "OSA" :
			((irq_ptr->qib.qfmt == QDIO_ZFCP_QFMT) ? "ZFCP" : "HS"),
		 irq_ptr->schid.sch_no,
		 is_thinint_irq(irq_ptr),
		 (irq_ptr->sch_token) ? 1 : 0,
		 pci_out_supported(irq_ptr) ? 1 : 0,
		 css_general_characteristics.aif_tdd,
		 (irq_ptr->siga_flag.input) ? "R" : " ",
		 (irq_ptr->siga_flag.output) ? "W" : " ",
		 (irq_ptr->siga_flag.sync) ? "S" : " ",
		 (irq_ptr->siga_flag.sync_after_ai) ? "A" : " ",
		 (irq_ptr->siga_flag.sync_out_after_pci) ? "P" : " ");
	printk(KERN_INFO "%s", s);
}

int __init qdio_setup_init(void)
{
	int rc;

	qdio_q_cache = kmem_cache_create("qdio_q", sizeof(struct qdio_q),
					 256, 0, NULL);
	if (!qdio_q_cache)
		return -ENOMEM;

	qdio_aob_cache = kmem_cache_create("qdio_aob",
					sizeof(struct qaob),
					sizeof(struct qaob),
					0,
					NULL);
	if (!qdio_aob_cache) {
		rc = -ENOMEM;
		goto free_qdio_q_cache;
	}

	/* Check for OSA/FCP thin interrupts (bit 67). */
	DBF_EVENT("thinint:%1d",
		  (css_general_characteristics.aif_osa) ? 1 : 0);

	/* Check for QEBSM support in general (bit 58). */
	DBF_EVENT("cssQEBSM:%1d", (qebsm_possible()) ? 1 : 0);
	rc = 0;
out:
	return rc;
free_qdio_q_cache:
	kmem_cache_destroy(qdio_q_cache);
	goto out;
}

void qdio_setup_exit(void)
{
	kmem_cache_destroy(qdio_aob_cache);
	kmem_cache_destroy(qdio_q_cache);
}
