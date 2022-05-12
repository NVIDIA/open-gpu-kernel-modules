// SPDX-License-Identifier: GPL-2.0-only
/*
 * QLogic Fibre Channel HBA Driver
 * Copyright (c)  2003-2014 QLogic Corporation
 */
#include "qla_def.h"
#include "qla_gbl.h"

#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "qla_devtbl.h"

#ifdef CONFIG_SPARC
#include <asm/prom.h>
#endif

#include "qla_target.h"

/*
*  QLogic ISP2x00 Hardware Support Function Prototypes.
*/
static int qla2x00_isp_firmware(scsi_qla_host_t *);
static int qla2x00_setup_chip(scsi_qla_host_t *);
static int qla2x00_fw_ready(scsi_qla_host_t *);
static int qla2x00_configure_hba(scsi_qla_host_t *);
static int qla2x00_configure_loop(scsi_qla_host_t *);
static int qla2x00_configure_local_loop(scsi_qla_host_t *);
static int qla2x00_configure_fabric(scsi_qla_host_t *);
static int qla2x00_find_all_fabric_devs(scsi_qla_host_t *);
static int qla2x00_restart_isp(scsi_qla_host_t *);

static struct qla_chip_state_84xx *qla84xx_get_chip(struct scsi_qla_host *);
static int qla84xx_init_chip(scsi_qla_host_t *);
static int qla25xx_init_queues(struct qla_hw_data *);
static int qla24xx_post_prli_work(struct scsi_qla_host*, fc_port_t *);
static void qla24xx_handle_gpdb_event(scsi_qla_host_t *vha,
				      struct event_arg *ea);
static void qla24xx_handle_prli_done_event(struct scsi_qla_host *,
    struct event_arg *);
static void __qla24xx_handle_gpdb_event(scsi_qla_host_t *, struct event_arg *);

/* SRB Extensions ---------------------------------------------------------- */

void
qla2x00_sp_timeout(struct timer_list *t)
{
	srb_t *sp = from_timer(sp, t, u.iocb_cmd.timer);
	struct srb_iocb *iocb;

	WARN_ON(irqs_disabled());
	iocb = &sp->u.iocb_cmd;
	iocb->timeout(sp);
}

void qla2x00_sp_free(srb_t *sp)
{
	struct srb_iocb *iocb = &sp->u.iocb_cmd;

	del_timer(&iocb->timer);
	qla2x00_rel_sp(sp);
}

void qla2xxx_rel_done_warning(srb_t *sp, int res)
{
	WARN_ONCE(1, "Calling done() of an already freed srb %p object\n", sp);
}

void qla2xxx_rel_free_warning(srb_t *sp)
{
	WARN_ONCE(1, "Calling free() of an already freed srb %p object\n", sp);
}

/* Asynchronous Login/Logout Routines -------------------------------------- */

unsigned long
qla2x00_get_async_timeout(struct scsi_qla_host *vha)
{
	unsigned long tmo;
	struct qla_hw_data *ha = vha->hw;

	/* Firmware should use switch negotiated r_a_tov for timeout. */
	tmo = ha->r_a_tov / 10 * 2;
	if (IS_QLAFX00(ha)) {
		tmo = FX00_DEF_RATOV * 2;
	} else if (!IS_FWI2_CAPABLE(ha)) {
		/*
		 * Except for earlier ISPs where the timeout is seeded from the
		 * initialization control block.
		 */
		tmo = ha->login_timeout;
	}
	return tmo;
}

static void qla24xx_abort_iocb_timeout(void *data)
{
	srb_t *sp = data;
	struct srb_iocb *abt = &sp->u.iocb_cmd;
	struct qla_qpair *qpair = sp->qpair;
	u32 handle;
	unsigned long flags;

	if (sp->cmd_sp)
		ql_dbg(ql_dbg_async, sp->vha, 0x507c,
		    "Abort timeout - cmd hdl=%x, cmd type=%x hdl=%x, type=%x\n",
		    sp->cmd_sp->handle, sp->cmd_sp->type,
		    sp->handle, sp->type);
	else
		ql_dbg(ql_dbg_async, sp->vha, 0x507c,
		    "Abort timeout 2 - hdl=%x, type=%x\n",
		    sp->handle, sp->type);

	spin_lock_irqsave(qpair->qp_lock_ptr, flags);
	for (handle = 1; handle < qpair->req->num_outstanding_cmds; handle++) {
		if (sp->cmd_sp && (qpair->req->outstanding_cmds[handle] ==
		    sp->cmd_sp))
			qpair->req->outstanding_cmds[handle] = NULL;

		/* removing the abort */
		if (qpair->req->outstanding_cmds[handle] == sp) {
			qpair->req->outstanding_cmds[handle] = NULL;
			break;
		}
	}
	spin_unlock_irqrestore(qpair->qp_lock_ptr, flags);

	if (sp->cmd_sp)
		sp->cmd_sp->done(sp->cmd_sp, QLA_OS_TIMER_EXPIRED);

	abt->u.abt.comp_status = cpu_to_le16(CS_TIMEOUT);
	sp->done(sp, QLA_OS_TIMER_EXPIRED);
}

static void qla24xx_abort_sp_done(srb_t *sp, int res)
{
	struct srb_iocb *abt = &sp->u.iocb_cmd;
	srb_t *orig_sp = sp->cmd_sp;

	if (orig_sp)
		qla_wait_nvme_release_cmd_kref(orig_sp);

	del_timer(&sp->u.iocb_cmd.timer);
	if (sp->flags & SRB_WAKEUP_ON_COMP)
		complete(&abt->u.abt.comp);
	else
		sp->free(sp);
}

int qla24xx_async_abort_cmd(srb_t *cmd_sp, bool wait)
{
	scsi_qla_host_t *vha = cmd_sp->vha;
	struct srb_iocb *abt_iocb;
	srb_t *sp;
	int rval = QLA_FUNCTION_FAILED;

	sp = qla2xxx_get_qpair_sp(cmd_sp->vha, cmd_sp->qpair, cmd_sp->fcport,
				  GFP_ATOMIC);
	if (!sp)
		return rval;

	abt_iocb = &sp->u.iocb_cmd;
	sp->type = SRB_ABT_CMD;
	sp->name = "abort";
	sp->qpair = cmd_sp->qpair;
	sp->cmd_sp = cmd_sp;
	if (wait)
		sp->flags = SRB_WAKEUP_ON_COMP;

	abt_iocb->timeout = qla24xx_abort_iocb_timeout;
	init_completion(&abt_iocb->u.abt.comp);
	/* FW can send 2 x ABTS's timeout/20s */
	qla2x00_init_timer(sp, 42);

	abt_iocb->u.abt.cmd_hndl = cmd_sp->handle;
	abt_iocb->u.abt.req_que_no = cpu_to_le16(cmd_sp->qpair->req->id);

	sp->done = qla24xx_abort_sp_done;

	ql_dbg(ql_dbg_async, vha, 0x507c,
	       "Abort command issued - hdl=%x, type=%x\n", cmd_sp->handle,
	       cmd_sp->type);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS) {
		sp->free(sp);
		return rval;
	}

	if (wait) {
		wait_for_completion(&abt_iocb->u.abt.comp);
		rval = abt_iocb->u.abt.comp_status == CS_COMPLETE ?
			QLA_SUCCESS : QLA_FUNCTION_FAILED;
		sp->free(sp);
	}

	return rval;
}

void
qla2x00_async_iocb_timeout(void *data)
{
	srb_t *sp = data;
	fc_port_t *fcport = sp->fcport;
	struct srb_iocb *lio = &sp->u.iocb_cmd;
	int rc, h;
	unsigned long flags;

	if (fcport) {
		ql_dbg(ql_dbg_disc, fcport->vha, 0x2071,
		    "Async-%s timeout - hdl=%x portid=%06x %8phC.\n",
		    sp->name, sp->handle, fcport->d_id.b24, fcport->port_name);

		fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);
	} else {
		pr_info("Async-%s timeout - hdl=%x.\n",
		    sp->name, sp->handle);
	}

	switch (sp->type) {
	case SRB_LOGIN_CMD:
		rc = qla24xx_async_abort_cmd(sp, false);
		if (rc) {
			/* Retry as needed. */
			lio->u.logio.data[0] = MBS_COMMAND_ERROR;
			lio->u.logio.data[1] =
				lio->u.logio.flags & SRB_LOGIN_RETRIED ?
				QLA_LOGIO_LOGIN_RETRIED : 0;
			spin_lock_irqsave(sp->qpair->qp_lock_ptr, flags);
			for (h = 1; h < sp->qpair->req->num_outstanding_cmds;
			    h++) {
				if (sp->qpair->req->outstanding_cmds[h] ==
				    sp) {
					sp->qpair->req->outstanding_cmds[h] =
					    NULL;
					break;
				}
			}
			spin_unlock_irqrestore(sp->qpair->qp_lock_ptr, flags);
			sp->done(sp, QLA_FUNCTION_TIMEOUT);
		}
		break;
	case SRB_LOGOUT_CMD:
	case SRB_CT_PTHRU_CMD:
	case SRB_MB_IOCB:
	case SRB_NACK_PLOGI:
	case SRB_NACK_PRLI:
	case SRB_NACK_LOGO:
	case SRB_CTRL_VP:
	default:
		rc = qla24xx_async_abort_cmd(sp, false);
		if (rc) {
			spin_lock_irqsave(sp->qpair->qp_lock_ptr, flags);
			for (h = 1; h < sp->qpair->req->num_outstanding_cmds;
			    h++) {
				if (sp->qpair->req->outstanding_cmds[h] ==
				    sp) {
					sp->qpair->req->outstanding_cmds[h] =
					    NULL;
					break;
				}
			}
			spin_unlock_irqrestore(sp->qpair->qp_lock_ptr, flags);
			sp->done(sp, QLA_FUNCTION_TIMEOUT);
		}
		break;
	}
}

static void qla2x00_async_login_sp_done(srb_t *sp, int res)
{
	struct scsi_qla_host *vha = sp->vha;
	struct srb_iocb *lio = &sp->u.iocb_cmd;
	struct event_arg ea;

	ql_dbg(ql_dbg_disc, vha, 0x20dd,
	    "%s %8phC res %d \n", __func__, sp->fcport->port_name, res);

	sp->fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);

	if (!test_bit(UNLOADING, &vha->dpc_flags)) {
		memset(&ea, 0, sizeof(ea));
		ea.fcport = sp->fcport;
		ea.data[0] = lio->u.logio.data[0];
		ea.data[1] = lio->u.logio.data[1];
		ea.iop[0] = lio->u.logio.iop[0];
		ea.iop[1] = lio->u.logio.iop[1];
		ea.sp = sp;
		qla24xx_handle_plogi_done_event(vha, &ea);
	}

	sp->free(sp);
}

static inline bool
fcport_is_smaller(fc_port_t *fcport)
{
	if (wwn_to_u64(fcport->port_name) <
	    wwn_to_u64(fcport->vha->port_name))
		return true;
	else
		return false;
}

static inline bool
fcport_is_bigger(fc_port_t *fcport)
{
	return !fcport_is_smaller(fcport);
}

int
qla2x00_async_login(struct scsi_qla_host *vha, fc_port_t *fcport,
    uint16_t *data)
{
	srb_t *sp;
	struct srb_iocb *lio;
	int rval = QLA_FUNCTION_FAILED;

	if (!vha->flags.online || (fcport->flags & FCF_ASYNC_SENT) ||
	    fcport->loop_id == FC_NO_LOOP_ID) {
		ql_log(ql_log_warn, vha, 0xffff,
		    "%s: %8phC - not sending command.\n",
		    __func__, fcport->port_name);
		return rval;
	}

	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	qla2x00_set_fcport_disc_state(fcport, DSC_LOGIN_PEND);
	fcport->flags |= FCF_ASYNC_SENT;
	fcport->logout_completed = 0;

	sp->type = SRB_LOGIN_CMD;
	sp->name = "login";
	sp->gen1 = fcport->rscn_gen;
	sp->gen2 = fcport->login_gen;

	lio = &sp->u.iocb_cmd;
	lio->timeout = qla2x00_async_iocb_timeout;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha) + 2);

	sp->done = qla2x00_async_login_sp_done;
	if (N2N_TOPO(fcport->vha->hw) && fcport_is_bigger(fcport))
		lio->u.logio.flags |= SRB_LOGIN_PRLI_ONLY;
	else
		lio->u.logio.flags |= SRB_LOGIN_COND_PLOGI;

	if (NVME_TARGET(vha->hw, fcport))
		lio->u.logio.flags |= SRB_LOGIN_SKIP_PRLI;

	ql_log(ql_log_warn, vha, 0x2072,
	       "Async-login - %8phC hdl=%x, loopid=%x portid=%02x%02x%02x retries=%d.\n",
	       fcport->port_name, sp->handle, fcport->loop_id,
	       fcport->d_id.b.domain, fcport->d_id.b.area, fcport->d_id.b.al_pa,
	       fcport->login_retry);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS) {
		fcport->flags |= FCF_LOGIN_NEEDED;
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		goto done_free_sp;
	}

	return rval;

done_free_sp:
	sp->free(sp);
	fcport->flags &= ~FCF_ASYNC_SENT;
done:
	fcport->flags &= ~FCF_ASYNC_ACTIVE;
	return rval;
}

static void qla2x00_async_logout_sp_done(srb_t *sp, int res)
{
	sp->fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);
	sp->fcport->login_gen++;
	qlt_logo_completion_handler(sp->fcport, res);
	sp->free(sp);
}

int
qla2x00_async_logout(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	srb_t *sp;
	struct srb_iocb *lio;
	int rval = QLA_FUNCTION_FAILED;

	fcport->flags |= FCF_ASYNC_SENT;
	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	sp->type = SRB_LOGOUT_CMD;
	sp->name = "logout";

	lio = &sp->u.iocb_cmd;
	lio->timeout = qla2x00_async_iocb_timeout;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha) + 2);

	sp->done = qla2x00_async_logout_sp_done;

	ql_dbg(ql_dbg_disc, vha, 0x2070,
	    "Async-logout - hdl=%x loop-id=%x portid=%02x%02x%02x %8phC.\n",
	    sp->handle, fcport->loop_id, fcport->d_id.b.domain,
		fcport->d_id.b.area, fcport->d_id.b.al_pa,
		fcport->port_name);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS)
		goto done_free_sp;
	return rval;

done_free_sp:
	sp->free(sp);
done:
	fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);
	return rval;
}

void
qla2x00_async_prlo_done(struct scsi_qla_host *vha, fc_port_t *fcport,
    uint16_t *data)
{
	fcport->flags &= ~FCF_ASYNC_ACTIVE;
	/* Don't re-login in target mode */
	if (!fcport->tgt_session)
		qla2x00_mark_device_lost(vha, fcport, 1);
	qlt_logo_completion_handler(fcport, data[0]);
}

static void qla2x00_async_prlo_sp_done(srb_t *sp, int res)
{
	struct srb_iocb *lio = &sp->u.iocb_cmd;
	struct scsi_qla_host *vha = sp->vha;

	sp->fcport->flags &= ~FCF_ASYNC_ACTIVE;
	if (!test_bit(UNLOADING, &vha->dpc_flags))
		qla2x00_post_async_prlo_done_work(sp->fcport->vha, sp->fcport,
		    lio->u.logio.data);
	sp->free(sp);
}

int
qla2x00_async_prlo(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	srb_t *sp;
	struct srb_iocb *lio;
	int rval;

	rval = QLA_FUNCTION_FAILED;
	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	sp->type = SRB_PRLO_CMD;
	sp->name = "prlo";

	lio = &sp->u.iocb_cmd;
	lio->timeout = qla2x00_async_iocb_timeout;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha) + 2);

	sp->done = qla2x00_async_prlo_sp_done;

	ql_dbg(ql_dbg_disc, vha, 0x2070,
	    "Async-prlo - hdl=%x loop-id=%x portid=%02x%02x%02x.\n",
	    sp->handle, fcport->loop_id, fcport->d_id.b.domain,
	    fcport->d_id.b.area, fcport->d_id.b.al_pa);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS)
		goto done_free_sp;

	return rval;

done_free_sp:
	sp->free(sp);
done:
	fcport->flags &= ~FCF_ASYNC_ACTIVE;
	return rval;
}

static
void qla24xx_handle_adisc_event(scsi_qla_host_t *vha, struct event_arg *ea)
{
	struct fc_port *fcport = ea->fcport;

	ql_dbg(ql_dbg_disc, vha, 0x20d2,
	    "%s %8phC DS %d LS %d rc %d login %d|%d rscn %d|%d lid %d\n",
	    __func__, fcport->port_name, fcport->disc_state,
	    fcport->fw_login_state, ea->rc, fcport->login_gen, ea->sp->gen2,
	    fcport->rscn_gen, ea->sp->gen1, fcport->loop_id);

	WARN_ONCE(!qla2xxx_is_valid_mbs(ea->data[0]), "mbs: %#x\n",
		  ea->data[0]);

	if (ea->data[0] != MBS_COMMAND_COMPLETE) {
		ql_dbg(ql_dbg_disc, vha, 0x2066,
		    "%s %8phC: adisc fail: post delete\n",
		    __func__, ea->fcport->port_name);
		/* deleted = 0 & logout_on_delete = force fw cleanup */
		fcport->deleted = 0;
		fcport->logout_on_delete = 1;
		qlt_schedule_sess_for_deletion(ea->fcport);
		return;
	}

	if (ea->fcport->disc_state == DSC_DELETE_PEND)
		return;

	if (ea->sp->gen2 != ea->fcport->login_gen) {
		/* target side must have changed it. */
		ql_dbg(ql_dbg_disc, vha, 0x20d3,
		    "%s %8phC generation changed\n",
		    __func__, ea->fcport->port_name);
		return;
	} else if (ea->sp->gen1 != ea->fcport->rscn_gen) {
		qla_rscn_replay(fcport);
		qlt_schedule_sess_for_deletion(fcport);
		return;
	}

	__qla24xx_handle_gpdb_event(vha, ea);
}

static int qla_post_els_plogi_work(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	struct qla_work_evt *e;

	e = qla2x00_alloc_work(vha, QLA_EVT_ELS_PLOGI);
	if (!e)
		return QLA_FUNCTION_FAILED;

	e->u.fcport.fcport = fcport;
	fcport->flags |= FCF_ASYNC_ACTIVE;
	qla2x00_set_fcport_disc_state(fcport, DSC_LOGIN_PEND);
	return qla2x00_post_work(vha, e);
}

static void qla2x00_async_adisc_sp_done(srb_t *sp, int res)
{
	struct scsi_qla_host *vha = sp->vha;
	struct event_arg ea;
	struct srb_iocb *lio = &sp->u.iocb_cmd;

	ql_dbg(ql_dbg_disc, vha, 0x2066,
	    "Async done-%s res %x %8phC\n",
	    sp->name, res, sp->fcport->port_name);

	sp->fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);

	memset(&ea, 0, sizeof(ea));
	ea.rc = res;
	ea.data[0] = lio->u.logio.data[0];
	ea.data[1] = lio->u.logio.data[1];
	ea.iop[0] = lio->u.logio.iop[0];
	ea.iop[1] = lio->u.logio.iop[1];
	ea.fcport = sp->fcport;
	ea.sp = sp;

	qla24xx_handle_adisc_event(vha, &ea);

	sp->free(sp);
}

int
qla2x00_async_adisc(struct scsi_qla_host *vha, fc_port_t *fcport,
    uint16_t *data)
{
	srb_t *sp;
	struct srb_iocb *lio;
	int rval = QLA_FUNCTION_FAILED;

	if (!vha->flags.online || (fcport->flags & FCF_ASYNC_SENT))
		return rval;

	fcport->flags |= FCF_ASYNC_SENT;
	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	sp->type = SRB_ADISC_CMD;
	sp->name = "adisc";

	lio = &sp->u.iocb_cmd;
	lio->timeout = qla2x00_async_iocb_timeout;
	sp->gen1 = fcport->rscn_gen;
	sp->gen2 = fcport->login_gen;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha) + 2);

	sp->done = qla2x00_async_adisc_sp_done;
	if (data[1] & QLA_LOGIO_LOGIN_RETRIED)
		lio->u.logio.flags |= SRB_LOGIN_RETRIED;

	ql_dbg(ql_dbg_disc, vha, 0x206f,
	    "Async-adisc - hdl=%x loopid=%x portid=%06x %8phC.\n",
	    sp->handle, fcport->loop_id, fcport->d_id.b24, fcport->port_name);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS)
		goto done_free_sp;

	return rval;

done_free_sp:
	sp->free(sp);
done:
	fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);
	qla2x00_post_async_adisc_work(vha, fcport, data);
	return rval;
}

static bool qla2x00_is_reserved_id(scsi_qla_host_t *vha, uint16_t loop_id)
{
	struct qla_hw_data *ha = vha->hw;

	if (IS_FWI2_CAPABLE(ha))
		return loop_id > NPH_LAST_HANDLE;

	return (loop_id > ha->max_loop_id && loop_id < SNS_FIRST_LOOP_ID) ||
		loop_id == MANAGEMENT_SERVER || loop_id == BROADCAST;
}

/**
 * qla2x00_find_new_loop_id - scan through our port list and find a new usable loop ID
 * @vha: adapter state pointer.
 * @dev: port structure pointer.
 *
 * Returns:
 *	qla2x00 local function return status code.
 *
 * Context:
 *	Kernel context.
 */
static int qla2x00_find_new_loop_id(scsi_qla_host_t *vha, fc_port_t *dev)
{
	int	rval;
	struct qla_hw_data *ha = vha->hw;
	unsigned long flags = 0;

	rval = QLA_SUCCESS;

	spin_lock_irqsave(&ha->vport_slock, flags);

	dev->loop_id = find_first_zero_bit(ha->loop_id_map, LOOPID_MAP_SIZE);
	if (dev->loop_id >= LOOPID_MAP_SIZE ||
	    qla2x00_is_reserved_id(vha, dev->loop_id)) {
		dev->loop_id = FC_NO_LOOP_ID;
		rval = QLA_FUNCTION_FAILED;
	} else {
		set_bit(dev->loop_id, ha->loop_id_map);
	}
	spin_unlock_irqrestore(&ha->vport_slock, flags);

	if (rval == QLA_SUCCESS)
		ql_dbg(ql_dbg_disc, dev->vha, 0x2086,
		       "Assigning new loopid=%x, portid=%x.\n",
		       dev->loop_id, dev->d_id.b24);
	else
		ql_log(ql_log_warn, dev->vha, 0x2087,
		       "No loop_id's available, portid=%x.\n",
		       dev->d_id.b24);

	return rval;
}

void qla2x00_clear_loop_id(fc_port_t *fcport)
{
	struct qla_hw_data *ha = fcport->vha->hw;

	if (fcport->loop_id == FC_NO_LOOP_ID ||
	    qla2x00_is_reserved_id(fcport->vha, fcport->loop_id))
		return;

	clear_bit(fcport->loop_id, ha->loop_id_map);
	fcport->loop_id = FC_NO_LOOP_ID;
}

static void qla24xx_handle_gnl_done_event(scsi_qla_host_t *vha,
	struct event_arg *ea)
{
	fc_port_t *fcport, *conflict_fcport;
	struct get_name_list_extended *e;
	u16 i, n, found = 0, loop_id;
	port_id_t id;
	u64 wwn;
	u16 data[2];
	u8 current_login_state, nvme_cls;

	fcport = ea->fcport;
	ql_dbg(ql_dbg_disc, vha, 0xffff,
	    "%s %8phC DS %d LS rc %d %d login %d|%d rscn %d|%d lid %d\n",
	    __func__, fcport->port_name, fcport->disc_state,
	    fcport->fw_login_state, ea->rc,
	    fcport->login_gen, fcport->last_login_gen,
	    fcport->rscn_gen, fcport->last_rscn_gen, vha->loop_id);

	if (fcport->disc_state == DSC_DELETE_PEND)
		return;

	if (ea->rc) { /* rval */
		if (fcport->login_retry == 0) {
			ql_dbg(ql_dbg_disc, vha, 0x20de,
			    "GNL failed Port login retry %8phN, retry cnt=%d.\n",
			    fcport->port_name, fcport->login_retry);
		}
		return;
	}

	if (fcport->last_rscn_gen != fcport->rscn_gen) {
		qla_rscn_replay(fcport);
		qlt_schedule_sess_for_deletion(fcport);
		return;
	} else if (fcport->last_login_gen != fcport->login_gen) {
		ql_dbg(ql_dbg_disc, vha, 0x20e0,
		    "%s %8phC login gen changed\n",
		    __func__, fcport->port_name);
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		return;
	}

	n = ea->data[0] / sizeof(struct get_name_list_extended);

	ql_dbg(ql_dbg_disc, vha, 0x20e1,
	    "%s %d %8phC n %d %02x%02x%02x lid %d \n",
	    __func__, __LINE__, fcport->port_name, n,
	    fcport->d_id.b.domain, fcport->d_id.b.area,
	    fcport->d_id.b.al_pa, fcport->loop_id);

	for (i = 0; i < n; i++) {
		e = &vha->gnl.l[i];
		wwn = wwn_to_u64(e->port_name);
		id.b.domain = e->port_id[2];
		id.b.area = e->port_id[1];
		id.b.al_pa = e->port_id[0];
		id.b.rsvd_1 = 0;

		if (memcmp((u8 *)&wwn, fcport->port_name, WWN_SIZE))
			continue;

		if (IS_SW_RESV_ADDR(id))
			continue;

		found = 1;

		loop_id = le16_to_cpu(e->nport_handle);
		loop_id = (loop_id & 0x7fff);
		nvme_cls = e->current_login_state >> 4;
		current_login_state = e->current_login_state & 0xf;

		if (PRLI_PHASE(nvme_cls)) {
			current_login_state = nvme_cls;
			fcport->fc4_type &= ~FS_FC4TYPE_FCP;
			fcport->fc4_type |= FS_FC4TYPE_NVME;
		} else if (PRLI_PHASE(current_login_state)) {
			fcport->fc4_type |= FS_FC4TYPE_FCP;
			fcport->fc4_type &= ~FS_FC4TYPE_NVME;
		}

		ql_dbg(ql_dbg_disc, vha, 0x20e2,
		    "%s found %8phC CLS [%x|%x] fc4_type %d ID[%06x|%06x] lid[%d|%d]\n",
		    __func__, fcport->port_name,
		    e->current_login_state, fcport->fw_login_state,
		    fcport->fc4_type, id.b24, fcport->d_id.b24,
		    loop_id, fcport->loop_id);

		switch (fcport->disc_state) {
		case DSC_DELETE_PEND:
		case DSC_DELETED:
			break;
		default:
			if ((id.b24 != fcport->d_id.b24 &&
			    fcport->d_id.b24 &&
			    fcport->loop_id != FC_NO_LOOP_ID) ||
			    (fcport->loop_id != FC_NO_LOOP_ID &&
				fcport->loop_id != loop_id)) {
				ql_dbg(ql_dbg_disc, vha, 0x20e3,
				    "%s %d %8phC post del sess\n",
				    __func__, __LINE__, fcport->port_name);
				if (fcport->n2n_flag)
					fcport->d_id.b24 = 0;
				qlt_schedule_sess_for_deletion(fcport);
				return;
			}
			break;
		}

		fcport->loop_id = loop_id;
		if (fcport->n2n_flag)
			fcport->d_id.b24 = id.b24;

		wwn = wwn_to_u64(fcport->port_name);
		qlt_find_sess_invalidate_other(vha, wwn,
			id, loop_id, &conflict_fcport);

		if (conflict_fcport) {
			/*
			 * Another share fcport share the same loop_id &
			 * nport id. Conflict fcport needs to finish
			 * cleanup before this fcport can proceed to login.
			 */
			conflict_fcport->conflict = fcport;
			fcport->login_pause = 1;
		}

		switch (vha->hw->current_topology) {
		default:
			switch (current_login_state) {
			case DSC_LS_PRLI_COMP:
				ql_dbg(ql_dbg_disc + ql_dbg_verbose,
				    vha, 0x20e4, "%s %d %8phC post gpdb\n",
				    __func__, __LINE__, fcport->port_name);

				if ((e->prli_svc_param_word_3[0] & BIT_4) == 0)
					fcport->port_type = FCT_INITIATOR;
				else
					fcport->port_type = FCT_TARGET;
				data[0] = data[1] = 0;
				qla2x00_post_async_adisc_work(vha, fcport,
				    data);
				break;
			case DSC_LS_PORT_UNAVAIL:
			default:
				if (fcport->loop_id == FC_NO_LOOP_ID) {
					qla2x00_find_new_loop_id(vha, fcport);
					fcport->fw_login_state =
					    DSC_LS_PORT_UNAVAIL;
				}
				ql_dbg(ql_dbg_disc, vha, 0x20e5,
				    "%s %d %8phC\n", __func__, __LINE__,
				    fcport->port_name);
				qla24xx_fcport_handle_login(vha, fcport);
				break;
			}
			break;
		case ISP_CFG_N:
			fcport->fw_login_state = current_login_state;
			fcport->d_id = id;
			switch (current_login_state) {
			case DSC_LS_PRLI_PEND:
				/*
				 * In the middle of PRLI. Let it finish.
				 * Allow relogin code to recheck state again
				 * with GNL. Push disc_state back to DELETED
				 * so GNL can go out again
				 */
				qla2x00_set_fcport_disc_state(fcport,
				    DSC_DELETED);
				break;
			case DSC_LS_PRLI_COMP:
				if ((e->prli_svc_param_word_3[0] & BIT_4) == 0)
					fcport->port_type = FCT_INITIATOR;
				else
					fcport->port_type = FCT_TARGET;

				data[0] = data[1] = 0;
				qla2x00_post_async_adisc_work(vha, fcport,
				    data);
				break;
			case DSC_LS_PLOGI_COMP:
				if (fcport_is_bigger(fcport)) {
					/* local adapter is smaller */
					if (fcport->loop_id != FC_NO_LOOP_ID)
						qla2x00_clear_loop_id(fcport);

					fcport->loop_id = loop_id;
					qla24xx_fcport_handle_login(vha,
					    fcport);
					break;
				}
				fallthrough;
			default:
				if (fcport_is_smaller(fcport)) {
					/* local adapter is bigger */
					if (fcport->loop_id != FC_NO_LOOP_ID)
						qla2x00_clear_loop_id(fcport);

					fcport->loop_id = loop_id;
					qla24xx_fcport_handle_login(vha,
					    fcport);
				}
				break;
			}
			break;
		} /* switch (ha->current_topology) */
	}

	if (!found) {
		switch (vha->hw->current_topology) {
		case ISP_CFG_F:
		case ISP_CFG_FL:
			for (i = 0; i < n; i++) {
				e = &vha->gnl.l[i];
				id.b.domain = e->port_id[0];
				id.b.area = e->port_id[1];
				id.b.al_pa = e->port_id[2];
				id.b.rsvd_1 = 0;
				loop_id = le16_to_cpu(e->nport_handle);

				if (fcport->d_id.b24 == id.b24) {
					conflict_fcport =
					    qla2x00_find_fcport_by_wwpn(vha,
						e->port_name, 0);
					if (conflict_fcport) {
						ql_dbg(ql_dbg_disc + ql_dbg_verbose,
						    vha, 0x20e5,
						    "%s %d %8phC post del sess\n",
						    __func__, __LINE__,
						    conflict_fcport->port_name);
						qlt_schedule_sess_for_deletion
							(conflict_fcport);
					}
				}
				/*
				 * FW already picked this loop id for
				 * another fcport
				 */
				if (fcport->loop_id == loop_id)
					fcport->loop_id = FC_NO_LOOP_ID;
			}
			qla24xx_fcport_handle_login(vha, fcport);
			break;
		case ISP_CFG_N:
			qla2x00_set_fcport_disc_state(fcport, DSC_DELETED);
			if (time_after_eq(jiffies, fcport->dm_login_expire)) {
				if (fcport->n2n_link_reset_cnt < 2) {
					fcport->n2n_link_reset_cnt++;
					/*
					 * remote port is not sending PLOGI.
					 * Reset link to kick start his state
					 * machine
					 */
					set_bit(N2N_LINK_RESET,
					    &vha->dpc_flags);
				} else {
					if (fcport->n2n_chip_reset < 1) {
						ql_log(ql_log_info, vha, 0x705d,
						    "Chip reset to bring laser down");
						set_bit(ISP_ABORT_NEEDED,
						    &vha->dpc_flags);
						fcport->n2n_chip_reset++;
					} else {
						ql_log(ql_log_info, vha, 0x705d,
						    "Remote port %8ph is not coming back\n",
						    fcport->port_name);
						fcport->scan_state = 0;
					}
				}
				qla2xxx_wake_dpc(vha);
			} else {
				/*
				 * report port suppose to do PLOGI. Give him
				 * more time. FW will catch it.
				 */
				set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
			}
			break;
		default:
			break;
		}
	}
} /* gnl_event */

static void qla24xx_async_gnl_sp_done(srb_t *sp, int res)
{
	struct scsi_qla_host *vha = sp->vha;
	unsigned long flags;
	struct fc_port *fcport = NULL, *tf;
	u16 i, n = 0, loop_id;
	struct event_arg ea;
	struct get_name_list_extended *e;
	u64 wwn;
	struct list_head h;
	bool found = false;

	ql_dbg(ql_dbg_disc, vha, 0x20e7,
	    "Async done-%s res %x mb[1]=%x mb[2]=%x \n",
	    sp->name, res, sp->u.iocb_cmd.u.mbx.in_mb[1],
	    sp->u.iocb_cmd.u.mbx.in_mb[2]);

	if (res == QLA_FUNCTION_TIMEOUT)
		return;

	sp->fcport->flags &= ~(FCF_ASYNC_SENT|FCF_ASYNC_ACTIVE);
	memset(&ea, 0, sizeof(ea));
	ea.sp = sp;
	ea.rc = res;

	if (sp->u.iocb_cmd.u.mbx.in_mb[1] >=
	    sizeof(struct get_name_list_extended)) {
		n = sp->u.iocb_cmd.u.mbx.in_mb[1] /
		    sizeof(struct get_name_list_extended);
		ea.data[0] = sp->u.iocb_cmd.u.mbx.in_mb[1]; /* amnt xfered */
	}

	for (i = 0; i < n; i++) {
		e = &vha->gnl.l[i];
		loop_id = le16_to_cpu(e->nport_handle);
		/* mask out reserve bit */
		loop_id = (loop_id & 0x7fff);
		set_bit(loop_id, vha->hw->loop_id_map);
		wwn = wwn_to_u64(e->port_name);

		ql_dbg(ql_dbg_disc, vha, 0x20e8,
		    "%s %8phC %02x:%02x:%02x CLS %x/%x lid %x \n",
		    __func__, &wwn, e->port_id[2], e->port_id[1],
		    e->port_id[0], e->current_login_state, e->last_login_state,
		    (loop_id & 0x7fff));
	}

	spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);

	INIT_LIST_HEAD(&h);
	fcport = tf = NULL;
	if (!list_empty(&vha->gnl.fcports))
		list_splice_init(&vha->gnl.fcports, &h);
	spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);

	list_for_each_entry_safe(fcport, tf, &h, gnl_entry) {
		list_del_init(&fcport->gnl_entry);
		spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
		fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);
		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);
		ea.fcport = fcport;

		qla24xx_handle_gnl_done_event(vha, &ea);
	}

	/* create new fcport if fw has knowledge of new sessions */
	for (i = 0; i < n; i++) {
		port_id_t id;
		u64 wwnn;

		e = &vha->gnl.l[i];
		wwn = wwn_to_u64(e->port_name);

		found = false;
		list_for_each_entry_safe(fcport, tf, &vha->vp_fcports, list) {
			if (!memcmp((u8 *)&wwn, fcport->port_name,
			    WWN_SIZE)) {
				found = true;
				break;
			}
		}

		id.b.domain = e->port_id[2];
		id.b.area = e->port_id[1];
		id.b.al_pa = e->port_id[0];
		id.b.rsvd_1 = 0;

		if (!found && wwn && !IS_SW_RESV_ADDR(id)) {
			ql_dbg(ql_dbg_disc, vha, 0x2065,
			    "%s %d %8phC %06x post new sess\n",
			    __func__, __LINE__, (u8 *)&wwn, id.b24);
			wwnn = wwn_to_u64(e->node_name);
			qla24xx_post_newsess_work(vha, &id, (u8 *)&wwn,
			    (u8 *)&wwnn, NULL, 0);
		}
	}

	spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
	vha->gnl.sent = 0;
	if (!list_empty(&vha->gnl.fcports)) {
		/* retrigger gnl */
		list_for_each_entry_safe(fcport, tf, &vha->gnl.fcports,
		    gnl_entry) {
			list_del_init(&fcport->gnl_entry);
			fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);
			if (qla24xx_post_gnl_work(vha, fcport) == QLA_SUCCESS)
				break;
		}
	}
	spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);

	sp->free(sp);
}

int qla24xx_async_gnl(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	srb_t *sp;
	struct srb_iocb *mbx;
	int rval = QLA_FUNCTION_FAILED;
	unsigned long flags;
	u16 *mb;

	if (!vha->flags.online || (fcport->flags & FCF_ASYNC_SENT))
		return rval;

	ql_dbg(ql_dbg_disc, vha, 0x20d9,
	    "Async-gnlist WWPN %8phC \n", fcport->port_name);

	spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
	fcport->flags |= FCF_ASYNC_SENT;
	qla2x00_set_fcport_disc_state(fcport, DSC_GNL);
	fcport->last_rscn_gen = fcport->rscn_gen;
	fcport->last_login_gen = fcport->login_gen;

	list_add_tail(&fcport->gnl_entry, &vha->gnl.fcports);
	if (vha->gnl.sent) {
		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);
		return QLA_SUCCESS;
	}
	vha->gnl.sent = 1;
	spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);

	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	sp->type = SRB_MB_IOCB;
	sp->name = "gnlist";
	sp->gen1 = fcport->rscn_gen;
	sp->gen2 = fcport->login_gen;

	mbx = &sp->u.iocb_cmd;
	mbx->timeout = qla2x00_async_iocb_timeout;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha)+2);

	mb = sp->u.iocb_cmd.u.mbx.out_mb;
	mb[0] = MBC_PORT_NODE_NAME_LIST;
	mb[1] = BIT_2 | BIT_3;
	mb[2] = MSW(vha->gnl.ldma);
	mb[3] = LSW(vha->gnl.ldma);
	mb[6] = MSW(MSD(vha->gnl.ldma));
	mb[7] = LSW(MSD(vha->gnl.ldma));
	mb[8] = vha->gnl.size;
	mb[9] = vha->vp_idx;

	sp->done = qla24xx_async_gnl_sp_done;

	ql_dbg(ql_dbg_disc, vha, 0x20da,
	    "Async-%s - OUT WWPN %8phC hndl %x\n",
	    sp->name, fcport->port_name, sp->handle);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS)
		goto done_free_sp;

	return rval;

done_free_sp:
	sp->free(sp);
done:
	fcport->flags &= ~(FCF_ASYNC_ACTIVE | FCF_ASYNC_SENT);
	return rval;
}

int qla24xx_post_gnl_work(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	struct qla_work_evt *e;

	e = qla2x00_alloc_work(vha, QLA_EVT_GNL);
	if (!e)
		return QLA_FUNCTION_FAILED;

	e->u.fcport.fcport = fcport;
	fcport->flags |= FCF_ASYNC_ACTIVE;
	return qla2x00_post_work(vha, e);
}

static void qla24xx_async_gpdb_sp_done(srb_t *sp, int res)
{
	struct scsi_qla_host *vha = sp->vha;
	struct qla_hw_data *ha = vha->hw;
	fc_port_t *fcport = sp->fcport;
	u16 *mb = sp->u.iocb_cmd.u.mbx.in_mb;
	struct event_arg ea;

	ql_dbg(ql_dbg_disc, vha, 0x20db,
	    "Async done-%s res %x, WWPN %8phC mb[1]=%x mb[2]=%x \n",
	    sp->name, res, fcport->port_name, mb[1], mb[2]);

	fcport->flags &= ~(FCF_ASYNC_SENT | FCF_ASYNC_ACTIVE);

	if (res == QLA_FUNCTION_TIMEOUT)
		goto done;

	memset(&ea, 0, sizeof(ea));
	ea.fcport = fcport;
	ea.sp = sp;

	qla24xx_handle_gpdb_event(vha, &ea);

done:
	dma_pool_free(ha->s_dma_pool, sp->u.iocb_cmd.u.mbx.in,
		sp->u.iocb_cmd.u.mbx.in_dma);

	sp->free(sp);
}

static int qla24xx_post_prli_work(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	struct qla_work_evt *e;

	if (vha->host->active_mode == MODE_TARGET)
		return QLA_FUNCTION_FAILED;

	e = qla2x00_alloc_work(vha, QLA_EVT_PRLI);
	if (!e)
		return QLA_FUNCTION_FAILED;

	e->u.fcport.fcport = fcport;

	return qla2x00_post_work(vha, e);
}

static void qla2x00_async_prli_sp_done(srb_t *sp, int res)
{
	struct scsi_qla_host *vha = sp->vha;
	struct srb_iocb *lio = &sp->u.iocb_cmd;
	struct event_arg ea;

	ql_dbg(ql_dbg_disc, vha, 0x2129,
	    "%s %8phC res %d \n", __func__,
	    sp->fcport->port_name, res);

	sp->fcport->flags &= ~FCF_ASYNC_SENT;

	if (!test_bit(UNLOADING, &vha->dpc_flags)) {
		memset(&ea, 0, sizeof(ea));
		ea.fcport = sp->fcport;
		ea.data[0] = lio->u.logio.data[0];
		ea.data[1] = lio->u.logio.data[1];
		ea.iop[0] = lio->u.logio.iop[0];
		ea.iop[1] = lio->u.logio.iop[1];
		ea.sp = sp;

		qla24xx_handle_prli_done_event(vha, &ea);
	}

	sp->free(sp);
}

int
qla24xx_async_prli(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	srb_t *sp;
	struct srb_iocb *lio;
	int rval = QLA_FUNCTION_FAILED;

	if (!vha->flags.online) {
		ql_dbg(ql_dbg_disc, vha, 0xffff, "%s %d %8phC exit\n",
		    __func__, __LINE__, fcport->port_name);
		return rval;
	}

	if ((fcport->fw_login_state == DSC_LS_PLOGI_PEND ||
	    fcport->fw_login_state == DSC_LS_PRLI_PEND) &&
	    qla_dual_mode_enabled(vha)) {
		ql_dbg(ql_dbg_disc, vha, 0xffff, "%s %d %8phC exit\n",
		    __func__, __LINE__, fcport->port_name);
		return rval;
	}

	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		return rval;

	fcport->flags |= FCF_ASYNC_SENT;
	fcport->logout_completed = 0;

	sp->type = SRB_PRLI_CMD;
	sp->name = "prli";

	lio = &sp->u.iocb_cmd;
	lio->timeout = qla2x00_async_iocb_timeout;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha) + 2);

	sp->done = qla2x00_async_prli_sp_done;
	lio->u.logio.flags = 0;

	if (NVME_TARGET(vha->hw, fcport))
		lio->u.logio.flags |= SRB_LOGIN_NVME_PRLI;

	ql_dbg(ql_dbg_disc, vha, 0x211b,
	    "Async-prli - %8phC hdl=%x, loopid=%x portid=%06x retries=%d fc4type %x priority %x %s.\n",
	    fcport->port_name, sp->handle, fcport->loop_id, fcport->d_id.b24,
	    fcport->login_retry, fcport->fc4_type, vha->hw->fc4_type_priority,
	    NVME_TARGET(vha->hw, fcport) ? "nvme" : "fcp");

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS) {
		fcport->flags |= FCF_LOGIN_NEEDED;
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		goto done_free_sp;
	}

	return rval;

done_free_sp:
	sp->free(sp);
	fcport->flags &= ~FCF_ASYNC_SENT;
	return rval;
}

int qla24xx_post_gpdb_work(struct scsi_qla_host *vha, fc_port_t *fcport, u8 opt)
{
	struct qla_work_evt *e;

	e = qla2x00_alloc_work(vha, QLA_EVT_GPDB);
	if (!e)
		return QLA_FUNCTION_FAILED;

	e->u.fcport.fcport = fcport;
	e->u.fcport.opt = opt;
	fcport->flags |= FCF_ASYNC_ACTIVE;
	return qla2x00_post_work(vha, e);
}

int qla24xx_async_gpdb(struct scsi_qla_host *vha, fc_port_t *fcport, u8 opt)
{
	srb_t *sp;
	struct srb_iocb *mbx;
	int rval = QLA_FUNCTION_FAILED;
	u16 *mb;
	dma_addr_t pd_dma;
	struct port_database_24xx *pd;
	struct qla_hw_data *ha = vha->hw;

	if (!vha->flags.online || (fcport->flags & FCF_ASYNC_SENT) ||
	    fcport->loop_id == FC_NO_LOOP_ID) {
		ql_log(ql_log_warn, vha, 0xffff,
		    "%s: %8phC - not sending command.\n",
		    __func__, fcport->port_name);
		return rval;
	}

	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	qla2x00_set_fcport_disc_state(fcport, DSC_GPDB);

	fcport->flags |= FCF_ASYNC_SENT;
	sp->type = SRB_MB_IOCB;
	sp->name = "gpdb";
	sp->gen1 = fcport->rscn_gen;
	sp->gen2 = fcport->login_gen;

	mbx = &sp->u.iocb_cmd;
	mbx->timeout = qla2x00_async_iocb_timeout;
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha) + 2);

	pd = dma_pool_zalloc(ha->s_dma_pool, GFP_KERNEL, &pd_dma);
	if (pd == NULL) {
		ql_log(ql_log_warn, vha, 0xd043,
		    "Failed to allocate port database structure.\n");
		goto done_free_sp;
	}

	mb = sp->u.iocb_cmd.u.mbx.out_mb;
	mb[0] = MBC_GET_PORT_DATABASE;
	mb[1] = fcport->loop_id;
	mb[2] = MSW(pd_dma);
	mb[3] = LSW(pd_dma);
	mb[6] = MSW(MSD(pd_dma));
	mb[7] = LSW(MSD(pd_dma));
	mb[9] = vha->vp_idx;
	mb[10] = opt;

	mbx->u.mbx.in = pd;
	mbx->u.mbx.in_dma = pd_dma;

	sp->done = qla24xx_async_gpdb_sp_done;

	ql_dbg(ql_dbg_disc, vha, 0x20dc,
	    "Async-%s %8phC hndl %x opt %x\n",
	    sp->name, fcport->port_name, sp->handle, opt);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS)
		goto done_free_sp;
	return rval;

done_free_sp:
	if (pd)
		dma_pool_free(ha->s_dma_pool, pd, pd_dma);

	sp->free(sp);
	fcport->flags &= ~FCF_ASYNC_SENT;
done:
	fcport->flags &= ~FCF_ASYNC_ACTIVE;
	qla24xx_post_gpdb_work(vha, fcport, opt);
	return rval;
}

static
void __qla24xx_handle_gpdb_event(scsi_qla_host_t *vha, struct event_arg *ea)
{
	unsigned long flags;

	spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
	ea->fcport->login_gen++;
	ea->fcport->deleted = 0;
	ea->fcport->logout_on_delete = 1;

	if (!ea->fcport->login_succ && !IS_SW_RESV_ADDR(ea->fcport->d_id)) {
		vha->fcport_count++;
		ea->fcport->login_succ = 1;

		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);
		qla24xx_sched_upd_fcport(ea->fcport);
		spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
	} else if (ea->fcport->login_succ) {
		/*
		 * We have an existing session. A late RSCN delivery
		 * must have triggered the session to be re-validate.
		 * Session is still valid.
		 */
		ql_dbg(ql_dbg_disc, vha, 0x20d6,
		    "%s %d %8phC session revalidate success\n",
		    __func__, __LINE__, ea->fcport->port_name);
		qla2x00_set_fcport_disc_state(ea->fcport, DSC_LOGIN_COMPLETE);
	}
	spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);
}

static
void qla24xx_handle_gpdb_event(scsi_qla_host_t *vha, struct event_arg *ea)
{
	fc_port_t *fcport = ea->fcport;
	struct port_database_24xx *pd;
	struct srb *sp = ea->sp;
	uint8_t	ls;

	pd = (struct port_database_24xx *)sp->u.iocb_cmd.u.mbx.in;

	fcport->flags &= ~FCF_ASYNC_SENT;

	ql_dbg(ql_dbg_disc, vha, 0x20d2,
	    "%s %8phC DS %d LS %d fc4_type %x rc %d\n", __func__,
	    fcport->port_name, fcport->disc_state, pd->current_login_state,
	    fcport->fc4_type, ea->rc);

	if (fcport->disc_state == DSC_DELETE_PEND)
		return;

	if (NVME_TARGET(vha->hw, fcport))
		ls = pd->current_login_state >> 4;
	else
		ls = pd->current_login_state & 0xf;

	if (ea->sp->gen2 != fcport->login_gen) {
		/* target side must have changed it. */

		ql_dbg(ql_dbg_disc, vha, 0x20d3,
		    "%s %8phC generation changed\n",
		    __func__, fcport->port_name);
		return;
	} else if (ea->sp->gen1 != fcport->rscn_gen) {
		qla_rscn_replay(fcport);
		qlt_schedule_sess_for_deletion(fcport);
		return;
	}

	switch (ls) {
	case PDS_PRLI_COMPLETE:
		__qla24xx_parse_gpdb(vha, fcport, pd);
		break;
	case PDS_PLOGI_PENDING:
	case PDS_PLOGI_COMPLETE:
	case PDS_PRLI_PENDING:
	case PDS_PRLI2_PENDING:
		/* Set discovery state back to GNL to Relogin attempt */
		if (qla_dual_mode_enabled(vha) ||
		    qla_ini_mode_enabled(vha)) {
			qla2x00_set_fcport_disc_state(fcport, DSC_GNL);
			set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		}
		return;
	case PDS_LOGO_PENDING:
	case PDS_PORT_UNAVAILABLE:
	default:
		ql_dbg(ql_dbg_disc, vha, 0x20d5, "%s %d %8phC post del sess\n",
		    __func__, __LINE__, fcport->port_name);
		qlt_schedule_sess_for_deletion(fcport);
		return;
	}
	__qla24xx_handle_gpdb_event(vha, ea);
} /* gpdb event */

static void qla_chk_n2n_b4_login(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	u8 login = 0;
	int rc;

	if (qla_tgt_mode_enabled(vha))
		return;

	if (qla_dual_mode_enabled(vha)) {
		if (N2N_TOPO(vha->hw)) {
			u64 mywwn, wwn;

			mywwn = wwn_to_u64(vha->port_name);
			wwn = wwn_to_u64(fcport->port_name);
			if (mywwn > wwn)
				login = 1;
			else if ((fcport->fw_login_state == DSC_LS_PLOGI_COMP)
			    && time_after_eq(jiffies,
				    fcport->plogi_nack_done_deadline))
				login = 1;
		} else {
			login = 1;
		}
	} else {
		/* initiator mode */
		login = 1;
	}

	if (login && fcport->login_retry) {
		fcport->login_retry--;
		if (fcport->loop_id == FC_NO_LOOP_ID) {
			fcport->fw_login_state = DSC_LS_PORT_UNAVAIL;
			rc = qla2x00_find_new_loop_id(vha, fcport);
			if (rc) {
				ql_dbg(ql_dbg_disc, vha, 0x20e6,
				    "%s %d %8phC post del sess - out of loopid\n",
				    __func__, __LINE__, fcport->port_name);
				fcport->scan_state = 0;
				qlt_schedule_sess_for_deletion(fcport);
				return;
			}
		}
		ql_dbg(ql_dbg_disc, vha, 0x20bf,
		    "%s %d %8phC post login\n",
		    __func__, __LINE__, fcport->port_name);
		qla2x00_post_async_login_work(vha, fcport, NULL);
	}
}

int qla24xx_fcport_handle_login(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	u16 data[2];
	u64 wwn;
	u16 sec;

	ql_dbg(ql_dbg_disc, vha, 0x20d8,
	    "%s %8phC DS %d LS %d P %d fl %x confl %p rscn %d|%d login %d lid %d scan %d\n",
	    __func__, fcport->port_name, fcport->disc_state,
	    fcport->fw_login_state, fcport->login_pause, fcport->flags,
	    fcport->conflict, fcport->last_rscn_gen, fcport->rscn_gen,
	    fcport->login_gen, fcport->loop_id, fcport->scan_state);

	if (fcport->scan_state != QLA_FCPORT_FOUND)
		return 0;

	if ((fcport->loop_id != FC_NO_LOOP_ID) &&
	    qla_dual_mode_enabled(vha) &&
	    ((fcport->fw_login_state == DSC_LS_PLOGI_PEND) ||
	     (fcport->fw_login_state == DSC_LS_PRLI_PEND)))
		return 0;

	if (fcport->fw_login_state == DSC_LS_PLOGI_COMP &&
	    !N2N_TOPO(vha->hw)) {
		if (time_before_eq(jiffies, fcport->plogi_nack_done_deadline)) {
			set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
			return 0;
		}
	}

	/* Target won't initiate port login if fabric is present */
	if (vha->host->active_mode == MODE_TARGET && !N2N_TOPO(vha->hw))
		return 0;

	if (fcport->flags & FCF_ASYNC_SENT) {
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		return 0;
	}

	switch (fcport->disc_state) {
	case DSC_DELETED:
		wwn = wwn_to_u64(fcport->node_name);
		switch (vha->hw->current_topology) {
		case ISP_CFG_N:
			if (fcport_is_smaller(fcport)) {
				/* this adapter is bigger */
				if (fcport->login_retry) {
					if (fcport->loop_id == FC_NO_LOOP_ID) {
						qla2x00_find_new_loop_id(vha,
						    fcport);
						fcport->fw_login_state =
						    DSC_LS_PORT_UNAVAIL;
					}
					fcport->login_retry--;
					qla_post_els_plogi_work(vha, fcport);
				} else {
					ql_log(ql_log_info, vha, 0x705d,
					    "Unable to reach remote port %8phC",
					    fcport->port_name);
				}
			} else {
				qla24xx_post_gnl_work(vha, fcport);
			}
			break;
		default:
			if (wwn == 0)    {
				ql_dbg(ql_dbg_disc, vha, 0xffff,
				    "%s %d %8phC post GNNID\n",
				    __func__, __LINE__, fcport->port_name);
				qla24xx_post_gnnid_work(vha, fcport);
			} else if (fcport->loop_id == FC_NO_LOOP_ID) {
				ql_dbg(ql_dbg_disc, vha, 0x20bd,
				    "%s %d %8phC post gnl\n",
				    __func__, __LINE__, fcport->port_name);
				qla24xx_post_gnl_work(vha, fcport);
			} else {
				qla_chk_n2n_b4_login(vha, fcport);
			}
			break;
		}
		break;

	case DSC_GNL:
		switch (vha->hw->current_topology) {
		case ISP_CFG_N:
			if ((fcport->current_login_state & 0xf) == 0x6) {
				ql_dbg(ql_dbg_disc, vha, 0x2118,
				    "%s %d %8phC post GPDB work\n",
				    __func__, __LINE__, fcport->port_name);
				fcport->chip_reset =
					vha->hw->base_qpair->chip_reset;
				qla24xx_post_gpdb_work(vha, fcport, 0);
			}  else {
				ql_dbg(ql_dbg_disc, vha, 0x2118,
				    "%s %d %8phC post %s PRLI\n",
				    __func__, __LINE__, fcport->port_name,
				    NVME_TARGET(vha->hw, fcport) ? "NVME" :
				    "FC");
				qla24xx_post_prli_work(vha, fcport);
			}
			break;
		default:
			if (fcport->login_pause) {
				ql_dbg(ql_dbg_disc, vha, 0x20d8,
				    "%s %d %8phC exit\n",
				    __func__, __LINE__,
				    fcport->port_name);
				fcport->last_rscn_gen = fcport->rscn_gen;
				fcport->last_login_gen = fcport->login_gen;
				set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
				break;
			}
			qla_chk_n2n_b4_login(vha, fcport);
			break;
		}
		break;

	case DSC_LOGIN_FAILED:
		if (N2N_TOPO(vha->hw))
			qla_chk_n2n_b4_login(vha, fcport);
		else
			qlt_schedule_sess_for_deletion(fcport);
		break;

	case DSC_LOGIN_COMPLETE:
		/* recheck login state */
		data[0] = data[1] = 0;
		qla2x00_post_async_adisc_work(vha, fcport, data);
		break;

	case DSC_LOGIN_PEND:
		if (fcport->fw_login_state == DSC_LS_PLOGI_COMP)
			qla24xx_post_prli_work(vha, fcport);
		break;

	case DSC_UPD_FCPORT:
		sec =  jiffies_to_msecs(jiffies -
		    fcport->jiffies_at_registration)/1000;
		if (fcport->sec_since_registration < sec && sec &&
		    !(sec % 60)) {
			fcport->sec_since_registration = sec;
			ql_dbg(ql_dbg_disc, fcport->vha, 0xffff,
			    "%s %8phC - Slow Rport registration(%d Sec)\n",
			    __func__, fcport->port_name, sec);
		}

		if (fcport->next_disc_state != DSC_DELETE_PEND)
			fcport->next_disc_state = DSC_ADISC;
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		break;

	default:
		break;
	}

	return 0;
}

int qla24xx_post_newsess_work(struct scsi_qla_host *vha, port_id_t *id,
    u8 *port_name, u8 *node_name, void *pla, u8 fc4_type)
{
	struct qla_work_evt *e;

	e = qla2x00_alloc_work(vha, QLA_EVT_NEW_SESS);
	if (!e)
		return QLA_FUNCTION_FAILED;

	e->u.new_sess.id = *id;
	e->u.new_sess.pla = pla;
	e->u.new_sess.fc4_type = fc4_type;
	memcpy(e->u.new_sess.port_name, port_name, WWN_SIZE);
	if (node_name)
		memcpy(e->u.new_sess.node_name, node_name, WWN_SIZE);

	return qla2x00_post_work(vha, e);
}

void qla2x00_handle_rscn(scsi_qla_host_t *vha, struct event_arg *ea)
{
	fc_port_t *fcport;
	unsigned long flags;

	fcport = qla2x00_find_fcport_by_nportid(vha, &ea->id, 1);
	if (fcport) {
		fcport->scan_needed = 1;
		fcport->rscn_gen++;
	}

	spin_lock_irqsave(&vha->work_lock, flags);
	if (vha->scan.scan_flags == 0) {
		ql_dbg(ql_dbg_disc, vha, 0xffff, "%s: schedule\n", __func__);
		vha->scan.scan_flags |= SF_QUEUED;
		schedule_delayed_work(&vha->scan.scan_work, 5);
	}
	spin_unlock_irqrestore(&vha->work_lock, flags);
}

void qla24xx_handle_relogin_event(scsi_qla_host_t *vha,
	struct event_arg *ea)
{
	fc_port_t *fcport = ea->fcport;

	if (test_bit(UNLOADING, &vha->dpc_flags))
		return;

	ql_dbg(ql_dbg_disc, vha, 0x2102,
	    "%s %8phC DS %d LS %d P %d del %d cnfl %p rscn %d|%d login %d|%d fl %x\n",
	    __func__, fcport->port_name, fcport->disc_state,
	    fcport->fw_login_state, fcport->login_pause,
	    fcport->deleted, fcport->conflict,
	    fcport->last_rscn_gen, fcport->rscn_gen,
	    fcport->last_login_gen, fcport->login_gen,
	    fcport->flags);

	if (fcport->last_rscn_gen != fcport->rscn_gen) {
		ql_dbg(ql_dbg_disc, vha, 0x20e9, "%s %d %8phC post gnl\n",
		    __func__, __LINE__, fcport->port_name);
		qla24xx_post_gnl_work(vha, fcport);
		return;
	}

	qla24xx_fcport_handle_login(vha, fcport);
}

void qla_handle_els_plogi_done(scsi_qla_host_t *vha,
				      struct event_arg *ea)
{
	/* for pure Target Mode, PRLI will not be initiated */
	if (vha->host->active_mode == MODE_TARGET)
		return;

	ql_dbg(ql_dbg_disc, vha, 0x2118,
	    "%s %d %8phC post PRLI\n",
	    __func__, __LINE__, ea->fcport->port_name);
	qla24xx_post_prli_work(vha, ea->fcport);
}

/*
 * RSCN(s) came in for this fcport, but the RSCN(s) was not able
 * to be consumed by the fcport
 */
void qla_rscn_replay(fc_port_t *fcport)
{
	struct event_arg ea;

	switch (fcport->disc_state) {
	case DSC_DELETE_PEND:
		return;
	default:
		break;
	}

	if (fcport->scan_needed) {
		memset(&ea, 0, sizeof(ea));
		ea.id = fcport->d_id;
		ea.id.b.rsvd_1 = RSCN_PORT_ADDR;
		qla2x00_handle_rscn(fcport->vha, &ea);
	}
}

static void
qla2x00_tmf_iocb_timeout(void *data)
{
	srb_t *sp = data;
	struct srb_iocb *tmf = &sp->u.iocb_cmd;
	int rc, h;
	unsigned long flags;

	rc = qla24xx_async_abort_cmd(sp, false);
	if (rc) {
		spin_lock_irqsave(sp->qpair->qp_lock_ptr, flags);
		for (h = 1; h < sp->qpair->req->num_outstanding_cmds; h++) {
			if (sp->qpair->req->outstanding_cmds[h] == sp) {
				sp->qpair->req->outstanding_cmds[h] = NULL;
				break;
			}
		}
		spin_unlock_irqrestore(sp->qpair->qp_lock_ptr, flags);
		tmf->u.tmf.comp_status = cpu_to_le16(CS_TIMEOUT);
		tmf->u.tmf.data = QLA_FUNCTION_FAILED;
		complete(&tmf->u.tmf.comp);
	}
}

static void qla2x00_tmf_sp_done(srb_t *sp, int res)
{
	struct srb_iocb *tmf = &sp->u.iocb_cmd;

	complete(&tmf->u.tmf.comp);
}

int
qla2x00_async_tm_cmd(fc_port_t *fcport, uint32_t flags, uint32_t lun,
	uint32_t tag)
{
	struct scsi_qla_host *vha = fcport->vha;
	struct srb_iocb *tm_iocb;
	srb_t *sp;
	int rval = QLA_FUNCTION_FAILED;

	sp = qla2x00_get_sp(vha, fcport, GFP_KERNEL);
	if (!sp)
		goto done;

	tm_iocb = &sp->u.iocb_cmd;
	sp->type = SRB_TM_CMD;
	sp->name = "tmf";

	tm_iocb->timeout = qla2x00_tmf_iocb_timeout;
	init_completion(&tm_iocb->u.tmf.comp);
	qla2x00_init_timer(sp, qla2x00_get_async_timeout(vha));

	tm_iocb->u.tmf.flags = flags;
	tm_iocb->u.tmf.lun = lun;
	tm_iocb->u.tmf.data = tag;
	sp->done = qla2x00_tmf_sp_done;

	ql_dbg(ql_dbg_taskm, vha, 0x802f,
	    "Async-tmf hdl=%x loop-id=%x portid=%02x%02x%02x.\n",
	    sp->handle, fcport->loop_id, fcport->d_id.b.domain,
	    fcport->d_id.b.area, fcport->d_id.b.al_pa);

	rval = qla2x00_start_sp(sp);
	if (rval != QLA_SUCCESS)
		goto done_free_sp;
	wait_for_completion(&tm_iocb->u.tmf.comp);

	rval = tm_iocb->u.tmf.data;

	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x8030,
		    "TM IOCB failed (%x).\n", rval);
	}

	if (!test_bit(UNLOADING, &vha->dpc_flags) && !IS_QLAFX00(vha->hw)) {
		flags = tm_iocb->u.tmf.flags;
		lun = (uint16_t)tm_iocb->u.tmf.lun;

		/* Issue Marker IOCB */
		qla2x00_marker(vha, vha->hw->base_qpair,
		    fcport->loop_id, lun,
		    flags == TCF_LUN_RESET ? MK_SYNC_ID_LUN : MK_SYNC_ID);
	}

done_free_sp:
	sp->free(sp);
	fcport->flags &= ~FCF_ASYNC_SENT;
done:
	return rval;
}

int
qla24xx_async_abort_command(srb_t *sp)
{
	unsigned long   flags = 0;

	uint32_t	handle;
	fc_port_t	*fcport = sp->fcport;
	struct qla_qpair *qpair = sp->qpair;
	struct scsi_qla_host *vha = fcport->vha;
	struct req_que *req = qpair->req;

	spin_lock_irqsave(qpair->qp_lock_ptr, flags);
	for (handle = 1; handle < req->num_outstanding_cmds; handle++) {
		if (req->outstanding_cmds[handle] == sp)
			break;
	}
	spin_unlock_irqrestore(qpair->qp_lock_ptr, flags);

	if (handle == req->num_outstanding_cmds) {
		/* Command not found. */
		return QLA_FUNCTION_FAILED;
	}
	if (sp->type == SRB_FXIOCB_DCMD)
		return qlafx00_fx_disc(vha, &vha->hw->mr.fcport,
		    FXDISC_ABORT_IOCTL);

	return qla24xx_async_abort_cmd(sp, true);
}

static void
qla24xx_handle_prli_done_event(struct scsi_qla_host *vha, struct event_arg *ea)
{
	WARN_ONCE(!qla2xxx_is_valid_mbs(ea->data[0]), "mbs: %#x\n",
		  ea->data[0]);

	switch (ea->data[0]) {
	case MBS_COMMAND_COMPLETE:
		ql_dbg(ql_dbg_disc, vha, 0x2118,
		    "%s %d %8phC post gpdb\n",
		    __func__, __LINE__, ea->fcport->port_name);

		ea->fcport->chip_reset = vha->hw->base_qpair->chip_reset;
		ea->fcport->logout_on_delete = 1;
		ea->fcport->nvme_prli_service_param = ea->iop[0];
		if (ea->iop[0] & NVME_PRLI_SP_FIRST_BURST)
			ea->fcport->nvme_first_burst_size =
			    (ea->iop[1] & 0xffff) * 512;
		else
			ea->fcport->nvme_first_burst_size = 0;
		qla24xx_post_gpdb_work(vha, ea->fcport, 0);
		break;
	default:
		if ((ea->iop[0] == LSC_SCODE_ELS_REJECT) &&
		    (ea->iop[1] == 0x50000)) {   /* reson 5=busy expl:0x0 */
			set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
			ea->fcport->fw_login_state = DSC_LS_PLOGI_COMP;
			break;
		}

		ql_dbg(ql_dbg_disc, vha, 0x2118,
		       "%s %d %8phC priority %s, fc4type %x\n",
		       __func__, __LINE__, ea->fcport->port_name,
		       vha->hw->fc4_type_priority == FC4_PRIORITY_FCP ?
		       "FCP" : "NVMe", ea->fcport->fc4_type);

		if (N2N_TOPO(vha->hw)) {
			if (vha->hw->fc4_type_priority == FC4_PRIORITY_NVME) {
				ea->fcport->fc4_type &= ~FS_FC4TYPE_NVME;
				ea->fcport->fc4_type |= FS_FC4TYPE_FCP;
			} else {
				ea->fcport->fc4_type &= ~FS_FC4TYPE_FCP;
				ea->fcport->fc4_type |= FS_FC4TYPE_NVME;
			}

			if (ea->fcport->n2n_link_reset_cnt < 3) {
				ea->fcport->n2n_link_reset_cnt++;
				vha->relogin_jif = jiffies + 2 * HZ;
				/*
				 * PRLI failed. Reset link to kick start
				 * state machine
				 */
				set_bit(N2N_LINK_RESET, &vha->dpc_flags);
			} else {
				ql_log(ql_log_warn, vha, 0x2119,
				       "%s %d %8phC Unable to reconnect\n",
				       __func__, __LINE__,
				       ea->fcport->port_name);
			}
		} else {
			/*
			 * switch connect. login failed. Take connection down
			 * and allow relogin to retrigger
			 */
			if (NVME_FCP_TARGET(ea->fcport)) {
				ql_dbg(ql_dbg_disc, vha, 0x2118,
				       "%s %d %8phC post %s prli\n",
				       __func__, __LINE__,
				       ea->fcport->port_name,
				       (ea->fcport->fc4_type & FS_FC4TYPE_NVME)
				       ? "NVMe" : "FCP");
				if (vha->hw->fc4_type_priority == FC4_PRIORITY_NVME)
					ea->fcport->fc4_type &= ~FS_FC4TYPE_NVME;
				else
					ea->fcport->fc4_type &= ~FS_FC4TYPE_FCP;
			}

			ea->fcport->flags &= ~FCF_ASYNC_SENT;
			ea->fcport->keep_nport_handle = 0;
			ea->fcport->logout_on_delete = 1;
			qlt_schedule_sess_for_deletion(ea->fcport);
		}
		break;
	}
}

void
qla24xx_handle_plogi_done_event(struct scsi_qla_host *vha, struct event_arg *ea)
{
	port_id_t cid;	/* conflict Nport id */
	u16 lid;
	struct fc_port *conflict_fcport;
	unsigned long flags;
	struct fc_port *fcport = ea->fcport;

	ql_dbg(ql_dbg_disc, vha, 0xffff,
	    "%s %8phC DS %d LS %d rc %d login %d|%d rscn %d|%d data %x|%x iop %x|%x\n",
	    __func__, fcport->port_name, fcport->disc_state,
	    fcport->fw_login_state, ea->rc, ea->sp->gen2, fcport->login_gen,
	    ea->sp->gen1, fcport->rscn_gen,
	    ea->data[0], ea->data[1], ea->iop[0], ea->iop[1]);

	if ((fcport->fw_login_state == DSC_LS_PLOGI_PEND) ||
	    (fcport->fw_login_state == DSC_LS_PRLI_PEND)) {
		ql_dbg(ql_dbg_disc, vha, 0x20ea,
		    "%s %d %8phC Remote is trying to login\n",
		    __func__, __LINE__, fcport->port_name);
		return;
	}

	if ((fcport->disc_state == DSC_DELETE_PEND) ||
	    (fcport->disc_state == DSC_DELETED)) {
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		return;
	}

	if (ea->sp->gen2 != fcport->login_gen) {
		/* target side must have changed it. */
		ql_dbg(ql_dbg_disc, vha, 0x20d3,
		    "%s %8phC generation changed\n",
		    __func__, fcport->port_name);
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		return;
	} else if (ea->sp->gen1 != fcport->rscn_gen) {
		ql_dbg(ql_dbg_disc, vha, 0x20d3,
		    "%s %8phC RSCN generation changed\n",
		    __func__, fcport->port_name);
		qla_rscn_replay(fcport);
		qlt_schedule_sess_for_deletion(fcport);
		return;
	}

	WARN_ONCE(!qla2xxx_is_valid_mbs(ea->data[0]), "mbs: %#x\n",
		  ea->data[0]);

	switch (ea->data[0]) {
	case MBS_COMMAND_COMPLETE:
		/*
		 * Driver must validate login state - If PRLI not complete,
		 * force a relogin attempt via implicit LOGO, PLOGI, and PRLI
		 * requests.
		 */
		if (NVME_TARGET(vha->hw, ea->fcport)) {
			ql_dbg(ql_dbg_disc, vha, 0x2117,
				"%s %d %8phC post prli\n",
				__func__, __LINE__, ea->fcport->port_name);
			qla24xx_post_prli_work(vha, ea->fcport);
		} else {
			ql_dbg(ql_dbg_disc, vha, 0x20ea,
			    "%s %d %8phC LoopID 0x%x in use with %06x. post gpdb\n",
			    __func__, __LINE__, ea->fcport->port_name,
			    ea->fcport->loop_id, ea->fcport->d_id.b24);

			set_bit(ea->fcport->loop_id, vha->hw->loop_id_map);
			spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
			ea->fcport->chip_reset = vha->hw->base_qpair->chip_reset;
			ea->fcport->logout_on_delete = 1;
			ea->fcport->send_els_logo = 0;
			ea->fcport->fw_login_state = DSC_LS_PRLI_COMP;
			spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);

			qla24xx_post_gpdb_work(vha, ea->fcport, 0);
		}
		break;
	case MBS_COMMAND_ERROR:
		ql_dbg(ql_dbg_disc, vha, 0x20eb, "%s %d %8phC cmd error %x\n",
		    __func__, __LINE__, ea->fcport->port_name, ea->data[1]);

		ea->fcport->flags &= ~FCF_ASYNC_SENT;
		qla2x00_set_fcport_disc_state(ea->fcport, DSC_LOGIN_FAILED);
		if (ea->data[1] & QLA_LOGIO_LOGIN_RETRIED)
			set_bit(RELOGIN_NEEDED, &vha->dpc_flags);
		else
			qla2x00_mark_device_lost(vha, ea->fcport, 1);
		break;
	case MBS_LOOP_ID_USED:
		/* data[1] = IO PARAM 1 = nport ID  */
		cid.b.domain = (ea->iop[1] >> 16) & 0xff;
		cid.b.area   = (ea->iop[1] >>  8) & 0xff;
		cid.b.al_pa  = ea->iop[1] & 0xff;
		cid.b.rsvd_1 = 0;

		ql_dbg(ql_dbg_disc, vha, 0x20ec,
		    "%s %d %8phC lid %#x in use with pid %06x post gnl\n",
		    __func__, __LINE__, ea->fcport->port_name,
		    ea->fcport->loop_id, cid.b24);

		set_bit(ea->fcport->loop_id, vha->hw->loop_id_map);
		ea->fcport->loop_id = FC_NO_LOOP_ID;
		qla24xx_post_gnl_work(vha, ea->fcport);
		break;
	case MBS_PORT_ID_USED:
		lid = ea->iop[1] & 0xffff;
		qlt_find_sess_invalidate_other(vha,
		    wwn_to_u64(ea->fcport->port_name),
		    ea->fcport->d_id, lid, &conflict_fcport);

		if (conflict_fcport) {
			/*
			 * Another fcport share the same loop_id/nport id.
			 * Conflict fcport needs to finish cleanup before this
			 * fcport can proceed to login.
			 */
			conflict_fcport->conflict = ea->fcport;
			ea->fcport->login_pause = 1;

			ql_dbg(ql_dbg_disc, vha, 0x20ed,
			    "%s %d %8phC NPortId %06x inuse with loopid 0x%x. post gidpn\n",
			    __func__, __LINE__, ea->fcport->port_name,
			    ea->fcport->d_id.b24, lid);
		} else {
			ql_dbg(ql_dbg_disc, vha, 0x20ed,
			    "%s %d %8phC NPortId %06x inuse with loopid 0x%x. sched delete\n",
			    __func__, __LINE__, ea->fcport->port_name,
			    ea->fcport->d_id.b24, lid);

			qla2x00_clear_loop_id(ea->fcport);
			set_bit(lid, vha->hw->loop_id_map);
			ea->fcport->loop_id = lid;
			ea->fcport->keep_nport_handle = 0;
			ea->fcport->logout_on_delete = 1;
			qlt_schedule_sess_for_deletion(ea->fcport);
		}
		break;
	}
	return;
}

/****************************************************************************/
/*                QLogic ISP2x00 Hardware Support Functions.                */
/****************************************************************************/

static int
qla83xx_nic_core_fw_load(scsi_qla_host_t *vha)
{
	int rval = QLA_SUCCESS;
	struct qla_hw_data *ha = vha->hw;
	uint32_t idc_major_ver, idc_minor_ver;
	uint16_t config[4];

	qla83xx_idc_lock(vha, 0);

	/* SV: TODO: Assign initialization timeout from
	 * flash-info / other param
	 */
	ha->fcoe_dev_init_timeout = QLA83XX_IDC_INITIALIZATION_TIMEOUT;
	ha->fcoe_reset_timeout = QLA83XX_IDC_RESET_ACK_TIMEOUT;

	/* Set our fcoe function presence */
	if (__qla83xx_set_drv_presence(vha) != QLA_SUCCESS) {
		ql_dbg(ql_dbg_p3p, vha, 0xb077,
		    "Error while setting DRV-Presence.\n");
		rval = QLA_FUNCTION_FAILED;
		goto exit;
	}

	/* Decide the reset ownership */
	qla83xx_reset_ownership(vha);

	/*
	 * On first protocol driver load:
	 * Init-Owner: Set IDC-Major-Version and Clear IDC-Lock-Recovery
	 * register.
	 * Others: Check compatibility with current IDC Major version.
	 */
	qla83xx_rd_reg(vha, QLA83XX_IDC_MAJOR_VERSION, &idc_major_ver);
	if (ha->flags.nic_core_reset_owner) {
		/* Set IDC Major version */
		idc_major_ver = QLA83XX_SUPP_IDC_MAJOR_VERSION;
		qla83xx_wr_reg(vha, QLA83XX_IDC_MAJOR_VERSION, idc_major_ver);

		/* Clearing IDC-Lock-Recovery register */
		qla83xx_wr_reg(vha, QLA83XX_IDC_LOCK_RECOVERY, 0);
	} else if (idc_major_ver != QLA83XX_SUPP_IDC_MAJOR_VERSION) {
		/*
		 * Clear further IDC participation if we are not compatible with
		 * the current IDC Major Version.
		 */
		ql_log(ql_log_warn, vha, 0xb07d,
		    "Failing load, idc_major_ver=%d, expected_major_ver=%d.\n",
		    idc_major_ver, QLA83XX_SUPP_IDC_MAJOR_VERSION);
		__qla83xx_clear_drv_presence(vha);
		rval = QLA_FUNCTION_FAILED;
		goto exit;
	}
	/* Each function sets its supported Minor version. */
	qla83xx_rd_reg(vha, QLA83XX_IDC_MINOR_VERSION, &idc_minor_ver);
	idc_minor_ver |= (QLA83XX_SUPP_IDC_MINOR_VERSION << (ha->portnum * 2));
	qla83xx_wr_reg(vha, QLA83XX_IDC_MINOR_VERSION, idc_minor_ver);

	if (ha->flags.nic_core_reset_owner) {
		memset(config, 0, sizeof(config));
		if (!qla81xx_get_port_config(vha, config))
			qla83xx_wr_reg(vha, QLA83XX_IDC_DEV_STATE,
			    QLA8XXX_DEV_READY);
	}

	rval = qla83xx_idc_state_handler(vha);

exit:
	qla83xx_idc_unlock(vha, 0);

	return rval;
}

/*
* qla2x00_initialize_adapter
*      Initialize board.
*
* Input:
*      ha = adapter block pointer.
*
* Returns:
*      0 = success
*/
int
qla2x00_initialize_adapter(scsi_qla_host_t *vha)
{
	int	rval;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req = ha->req_q_map[0];
	struct device_reg_24xx __iomem *reg = &ha->iobase->isp24;

	memset(&vha->qla_stats, 0, sizeof(vha->qla_stats));
	memset(&vha->fc_host_stat, 0, sizeof(vha->fc_host_stat));

	/* Clear adapter flags. */
	vha->flags.online = 0;
	ha->flags.chip_reset_done = 0;
	vha->flags.reset_active = 0;
	ha->flags.pci_channel_io_perm_failure = 0;
	ha->flags.eeh_busy = 0;
	vha->qla_stats.jiffies_at_last_reset = get_jiffies_64();
	atomic_set(&vha->loop_down_timer, LOOP_DOWN_TIME);
	atomic_set(&vha->loop_state, LOOP_DOWN);
	vha->device_flags = DFLG_NO_CABLE;
	vha->dpc_flags = 0;
	vha->flags.management_server_logged_in = 0;
	vha->marker_needed = 0;
	ha->isp_abort_cnt = 0;
	ha->beacon_blink_led = 0;

	set_bit(0, ha->req_qid_map);
	set_bit(0, ha->rsp_qid_map);

	ql_dbg(ql_dbg_init, vha, 0x0040,
	    "Configuring PCI space...\n");
	rval = ha->isp_ops->pci_config(vha);
	if (rval) {
		ql_log(ql_log_warn, vha, 0x0044,
		    "Unable to configure PCI space.\n");
		return (rval);
	}

	ha->isp_ops->reset_chip(vha);

	/* Check for secure flash support */
	if (IS_QLA28XX(ha)) {
		if (rd_reg_word(&reg->mailbox12) & BIT_0)
			ha->flags.secure_adapter = 1;
		ql_log(ql_log_info, vha, 0xffff, "Secure Adapter: %s\n",
		    (ha->flags.secure_adapter) ? "Yes" : "No");
	}


	rval = qla2xxx_get_flash_info(vha);
	if (rval) {
		ql_log(ql_log_fatal, vha, 0x004f,
		    "Unable to validate FLASH data.\n");
		return rval;
	}

	if (IS_QLA8044(ha)) {
		qla8044_read_reset_template(vha);

		/* NOTE: If ql2xdontresethba==1, set IDC_CTRL DONTRESET_BIT0.
		 * If DONRESET_BIT0 is set, drivers should not set dev_state
		 * to NEED_RESET. But if NEED_RESET is set, drivers should
		 * should honor the reset. */
		if (ql2xdontresethba == 1)
			qla8044_set_idc_dontreset(vha);
	}

	ha->isp_ops->get_flash_version(vha, req->ring);
	ql_dbg(ql_dbg_init, vha, 0x0061,
	    "Configure NVRAM parameters...\n");

	/* Let priority default to FCP, can be overridden by nvram_config */
	ha->fc4_type_priority = FC4_PRIORITY_FCP;

	ha->isp_ops->nvram_config(vha);

	if (ha->fc4_type_priority != FC4_PRIORITY_FCP &&
	    ha->fc4_type_priority != FC4_PRIORITY_NVME)
		ha->fc4_type_priority = FC4_PRIORITY_FCP;

	ql_log(ql_log_info, vha, 0xffff, "FC4 priority set to %s\n",
	       ha->fc4_type_priority == FC4_PRIORITY_FCP ? "FCP" : "NVMe");

	if (ha->flags.disable_serdes) {
		/* Mask HBA via NVRAM settings? */
		ql_log(ql_log_info, vha, 0x0077,
		    "Masking HBA WWPN %8phN (via NVRAM).\n", vha->port_name);
		return QLA_FUNCTION_FAILED;
	}

	ql_dbg(ql_dbg_init, vha, 0x0078,
	    "Verifying loaded RISC code...\n");

	/* If smartsan enabled then require fdmi and rdp enabled */
	if (ql2xsmartsan) {
		ql2xfdmienable = 1;
		ql2xrdpenable = 1;
	}

	if (qla2x00_isp_firmware(vha) != QLA_SUCCESS) {
		rval = ha->isp_ops->chip_diag(vha);
		if (rval)
			return (rval);
		rval = qla2x00_setup_chip(vha);
		if (rval)
			return (rval);
	}

	if (IS_QLA84XX(ha)) {
		ha->cs84xx = qla84xx_get_chip(vha);
		if (!ha->cs84xx) {
			ql_log(ql_log_warn, vha, 0x00d0,
			    "Unable to configure ISP84XX.\n");
			return QLA_FUNCTION_FAILED;
		}
	}

	if (qla_ini_mode_enabled(vha) || qla_dual_mode_enabled(vha))
		rval = qla2x00_init_rings(vha);

	/* No point in continuing if firmware initialization failed. */
	if (rval != QLA_SUCCESS)
		return rval;

	ha->flags.chip_reset_done = 1;

	if (rval == QLA_SUCCESS && IS_QLA84XX(ha)) {
		/* Issue verify 84xx FW IOCB to complete 84xx initialization */
		rval = qla84xx_init_chip(vha);
		if (rval != QLA_SUCCESS) {
			ql_log(ql_log_warn, vha, 0x00d4,
			    "Unable to initialize ISP84XX.\n");
			qla84xx_put_chip(vha);
		}
	}

	/* Load the NIC Core f/w if we are the first protocol driver. */
	if (IS_QLA8031(ha)) {
		rval = qla83xx_nic_core_fw_load(vha);
		if (rval)
			ql_log(ql_log_warn, vha, 0x0124,
			    "Error in initializing NIC Core f/w.\n");
	}

	if (IS_QLA24XX_TYPE(ha) || IS_QLA25XX(ha))
		qla24xx_read_fcp_prio_cfg(vha);

	if (IS_P3P_TYPE(ha))
		qla82xx_set_driver_version(vha, QLA2XXX_VERSION);
	else
		qla25xx_set_driver_version(vha, QLA2XXX_VERSION);

	return (rval);
}

/**
 * qla2100_pci_config() - Setup ISP21xx PCI configuration registers.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla2100_pci_config(scsi_qla_host_t *vha)
{
	uint16_t w;
	unsigned long flags;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;

	pci_set_master(ha->pdev);
	pci_try_set_mwi(ha->pdev);

	pci_read_config_word(ha->pdev, PCI_COMMAND, &w);
	w |= (PCI_COMMAND_PARITY | PCI_COMMAND_SERR);
	pci_write_config_word(ha->pdev, PCI_COMMAND, w);

	pci_disable_rom(ha->pdev);

	/* Get PCI bus information. */
	spin_lock_irqsave(&ha->hardware_lock, flags);
	ha->pci_attr = rd_reg_word(&reg->ctrl_status);
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	return QLA_SUCCESS;
}

/**
 * qla2300_pci_config() - Setup ISP23xx PCI configuration registers.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla2300_pci_config(scsi_qla_host_t *vha)
{
	uint16_t	w;
	unsigned long   flags = 0;
	uint32_t	cnt;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;

	pci_set_master(ha->pdev);
	pci_try_set_mwi(ha->pdev);

	pci_read_config_word(ha->pdev, PCI_COMMAND, &w);
	w |= (PCI_COMMAND_PARITY | PCI_COMMAND_SERR);

	if (IS_QLA2322(ha) || IS_QLA6322(ha))
		w &= ~PCI_COMMAND_INTX_DISABLE;
	pci_write_config_word(ha->pdev, PCI_COMMAND, w);

	/*
	 * If this is a 2300 card and not 2312, reset the
	 * COMMAND_INVALIDATE due to a bug in the 2300. Unfortunately,
	 * the 2310 also reports itself as a 2300 so we need to get the
	 * fb revision level -- a 6 indicates it really is a 2300 and
	 * not a 2310.
	 */
	if (IS_QLA2300(ha)) {
		spin_lock_irqsave(&ha->hardware_lock, flags);

		/* Pause RISC. */
		wrt_reg_word(&reg->hccr, HCCR_PAUSE_RISC);
		for (cnt = 0; cnt < 30000; cnt++) {
			if ((rd_reg_word(&reg->hccr) & HCCR_RISC_PAUSE) != 0)
				break;

			udelay(10);
		}

		/* Select FPM registers. */
		wrt_reg_word(&reg->ctrl_status, 0x20);
		rd_reg_word(&reg->ctrl_status);

		/* Get the fb rev level */
		ha->fb_rev = RD_FB_CMD_REG(ha, reg);

		if (ha->fb_rev == FPM_2300)
			pci_clear_mwi(ha->pdev);

		/* Deselect FPM registers. */
		wrt_reg_word(&reg->ctrl_status, 0x0);
		rd_reg_word(&reg->ctrl_status);

		/* Release RISC module. */
		wrt_reg_word(&reg->hccr, HCCR_RELEASE_RISC);
		for (cnt = 0; cnt < 30000; cnt++) {
			if ((rd_reg_word(&reg->hccr) & HCCR_RISC_PAUSE) == 0)
				break;

			udelay(10);
		}

		spin_unlock_irqrestore(&ha->hardware_lock, flags);
	}

	pci_write_config_byte(ha->pdev, PCI_LATENCY_TIMER, 0x80);

	pci_disable_rom(ha->pdev);

	/* Get PCI bus information. */
	spin_lock_irqsave(&ha->hardware_lock, flags);
	ha->pci_attr = rd_reg_word(&reg->ctrl_status);
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	return QLA_SUCCESS;
}

/**
 * qla24xx_pci_config() - Setup ISP24xx PCI configuration registers.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla24xx_pci_config(scsi_qla_host_t *vha)
{
	uint16_t w;
	unsigned long flags = 0;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_24xx __iomem *reg = &ha->iobase->isp24;

	pci_set_master(ha->pdev);
	pci_try_set_mwi(ha->pdev);

	pci_read_config_word(ha->pdev, PCI_COMMAND, &w);
	w |= (PCI_COMMAND_PARITY | PCI_COMMAND_SERR);
	w &= ~PCI_COMMAND_INTX_DISABLE;
	pci_write_config_word(ha->pdev, PCI_COMMAND, w);

	pci_write_config_byte(ha->pdev, PCI_LATENCY_TIMER, 0x80);

	/* PCI-X -- adjust Maximum Memory Read Byte Count (2048). */
	if (pci_find_capability(ha->pdev, PCI_CAP_ID_PCIX))
		pcix_set_mmrbc(ha->pdev, 2048);

	/* PCIe -- adjust Maximum Read Request Size (2048). */
	if (pci_is_pcie(ha->pdev))
		pcie_set_readrq(ha->pdev, 4096);

	pci_disable_rom(ha->pdev);

	ha->chip_revision = ha->pdev->revision;

	/* Get PCI bus information. */
	spin_lock_irqsave(&ha->hardware_lock, flags);
	ha->pci_attr = rd_reg_dword(&reg->ctrl_status);
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	return QLA_SUCCESS;
}

/**
 * qla25xx_pci_config() - Setup ISP25xx PCI configuration registers.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla25xx_pci_config(scsi_qla_host_t *vha)
{
	uint16_t w;
	struct qla_hw_data *ha = vha->hw;

	pci_set_master(ha->pdev);
	pci_try_set_mwi(ha->pdev);

	pci_read_config_word(ha->pdev, PCI_COMMAND, &w);
	w |= (PCI_COMMAND_PARITY | PCI_COMMAND_SERR);
	w &= ~PCI_COMMAND_INTX_DISABLE;
	pci_write_config_word(ha->pdev, PCI_COMMAND, w);

	/* PCIe -- adjust Maximum Read Request Size (2048). */
	if (pci_is_pcie(ha->pdev))
		pcie_set_readrq(ha->pdev, 4096);

	pci_disable_rom(ha->pdev);

	ha->chip_revision = ha->pdev->revision;

	return QLA_SUCCESS;
}

/**
 * qla2x00_isp_firmware() - Choose firmware image.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
static int
qla2x00_isp_firmware(scsi_qla_host_t *vha)
{
	int  rval;
	uint16_t loop_id, topo, sw_cap;
	uint8_t domain, area, al_pa;
	struct qla_hw_data *ha = vha->hw;

	/* Assume loading risc code */
	rval = QLA_FUNCTION_FAILED;

	if (ha->flags.disable_risc_code_load) {
		ql_log(ql_log_info, vha, 0x0079, "RISC CODE NOT loaded.\n");

		/* Verify checksum of loaded RISC code. */
		rval = qla2x00_verify_checksum(vha, ha->fw_srisc_address);
		if (rval == QLA_SUCCESS) {
			/* And, verify we are not in ROM code. */
			rval = qla2x00_get_adapter_id(vha, &loop_id, &al_pa,
			    &area, &domain, &topo, &sw_cap);
		}
	}

	if (rval)
		ql_dbg(ql_dbg_init, vha, 0x007a,
		    "**** Load RISC code ****.\n");

	return (rval);
}

/**
 * qla2x00_reset_chip() - Reset ISP chip.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla2x00_reset_chip(scsi_qla_host_t *vha)
{
	unsigned long   flags = 0;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;
	uint32_t	cnt;
	uint16_t	cmd;
	int rval = QLA_FUNCTION_FAILED;

	if (unlikely(pci_channel_offline(ha->pdev)))
		return rval;

	ha->isp_ops->disable_intrs(ha);

	spin_lock_irqsave(&ha->hardware_lock, flags);

	/* Turn off master enable */
	cmd = 0;
	pci_read_config_word(ha->pdev, PCI_COMMAND, &cmd);
	cmd &= ~PCI_COMMAND_MASTER;
	pci_write_config_word(ha->pdev, PCI_COMMAND, cmd);

	if (!IS_QLA2100(ha)) {
		/* Pause RISC. */
		wrt_reg_word(&reg->hccr, HCCR_PAUSE_RISC);
		if (IS_QLA2200(ha) || IS_QLA2300(ha)) {
			for (cnt = 0; cnt < 30000; cnt++) {
				if ((rd_reg_word(&reg->hccr) &
				    HCCR_RISC_PAUSE) != 0)
					break;
				udelay(100);
			}
		} else {
			rd_reg_word(&reg->hccr);	/* PCI Posting. */
			udelay(10);
		}

		/* Select FPM registers. */
		wrt_reg_word(&reg->ctrl_status, 0x20);
		rd_reg_word(&reg->ctrl_status);		/* PCI Posting. */

		/* FPM Soft Reset. */
		wrt_reg_word(&reg->fpm_diag_config, 0x100);
		rd_reg_word(&reg->fpm_diag_config);	/* PCI Posting. */

		/* Toggle Fpm Reset. */
		if (!IS_QLA2200(ha)) {
			wrt_reg_word(&reg->fpm_diag_config, 0x0);
			rd_reg_word(&reg->fpm_diag_config); /* PCI Posting. */
		}

		/* Select frame buffer registers. */
		wrt_reg_word(&reg->ctrl_status, 0x10);
		rd_reg_word(&reg->ctrl_status);		/* PCI Posting. */

		/* Reset frame buffer FIFOs. */
		if (IS_QLA2200(ha)) {
			WRT_FB_CMD_REG(ha, reg, 0xa000);
			RD_FB_CMD_REG(ha, reg);		/* PCI Posting. */
		} else {
			WRT_FB_CMD_REG(ha, reg, 0x00fc);

			/* Read back fb_cmd until zero or 3 seconds max */
			for (cnt = 0; cnt < 3000; cnt++) {
				if ((RD_FB_CMD_REG(ha, reg) & 0xff) == 0)
					break;
				udelay(100);
			}
		}

		/* Select RISC module registers. */
		wrt_reg_word(&reg->ctrl_status, 0);
		rd_reg_word(&reg->ctrl_status);		/* PCI Posting. */

		/* Reset RISC processor. */
		wrt_reg_word(&reg->hccr, HCCR_RESET_RISC);
		rd_reg_word(&reg->hccr);		/* PCI Posting. */

		/* Release RISC processor. */
		wrt_reg_word(&reg->hccr, HCCR_RELEASE_RISC);
		rd_reg_word(&reg->hccr);		/* PCI Posting. */
	}

	wrt_reg_word(&reg->hccr, HCCR_CLR_RISC_INT);
	wrt_reg_word(&reg->hccr, HCCR_CLR_HOST_INT);

	/* Reset ISP chip. */
	wrt_reg_word(&reg->ctrl_status, CSR_ISP_SOFT_RESET);

	/* Wait for RISC to recover from reset. */
	if (IS_QLA2100(ha) || IS_QLA2200(ha) || IS_QLA2300(ha)) {
		/*
		 * It is necessary to for a delay here since the card doesn't
		 * respond to PCI reads during a reset. On some architectures
		 * this will result in an MCA.
		 */
		udelay(20);
		for (cnt = 30000; cnt; cnt--) {
			if ((rd_reg_word(&reg->ctrl_status) &
			    CSR_ISP_SOFT_RESET) == 0)
				break;
			udelay(100);
		}
	} else
		udelay(10);

	/* Reset RISC processor. */
	wrt_reg_word(&reg->hccr, HCCR_RESET_RISC);

	wrt_reg_word(&reg->semaphore, 0);

	/* Release RISC processor. */
	wrt_reg_word(&reg->hccr, HCCR_RELEASE_RISC);
	rd_reg_word(&reg->hccr);			/* PCI Posting. */

	if (IS_QLA2100(ha) || IS_QLA2200(ha) || IS_QLA2300(ha)) {
		for (cnt = 0; cnt < 30000; cnt++) {
			if (RD_MAILBOX_REG(ha, reg, 0) != MBS_BUSY)
				break;

			udelay(100);
		}
	} else
		udelay(100);

	/* Turn on master enable */
	cmd |= PCI_COMMAND_MASTER;
	pci_write_config_word(ha->pdev, PCI_COMMAND, cmd);

	/* Disable RISC pause on FPM parity error. */
	if (!IS_QLA2100(ha)) {
		wrt_reg_word(&reg->hccr, HCCR_DISABLE_PARITY_PAUSE);
		rd_reg_word(&reg->hccr);		/* PCI Posting. */
	}

	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	return QLA_SUCCESS;
}

/**
 * qla81xx_reset_mpi() - Reset's MPI FW via Write MPI Register MBC.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
static int
qla81xx_reset_mpi(scsi_qla_host_t *vha)
{
	uint16_t mb[4] = {0x1010, 0, 1, 0};

	if (!IS_QLA81XX(vha->hw))
		return QLA_SUCCESS;

	return qla81xx_write_mpi_register(vha, mb);
}

static int
qla_chk_risc_recovery(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_24xx __iomem *reg = &ha->iobase->isp24;
	__le16 __iomem *mbptr = &reg->mailbox0;
	int i;
	u16 mb[32];
	int rc = QLA_SUCCESS;

	if (!IS_QLA27XX(ha) && !IS_QLA28XX(ha))
		return rc;

	/* this check is only valid after RISC reset */
	mb[0] = rd_reg_word(mbptr);
	mbptr++;
	if (mb[0] == 0xf) {
		rc = QLA_FUNCTION_FAILED;

		for (i = 1; i < 32; i++) {
			mb[i] = rd_reg_word(mbptr);
			mbptr++;
		}

		ql_log(ql_log_warn, vha, 0x1015,
		       "RISC reset failed. mb[0-7] %04xh %04xh %04xh %04xh %04xh %04xh %04xh %04xh\n",
		       mb[0], mb[1], mb[2], mb[3], mb[4], mb[5], mb[6], mb[7]);
		ql_log(ql_log_warn, vha, 0x1015,
		       "RISC reset failed. mb[8-15] %04xh %04xh %04xh %04xh %04xh %04xh %04xh %04xh\n",
		       mb[8], mb[9], mb[10], mb[11], mb[12], mb[13], mb[14],
		       mb[15]);
		ql_log(ql_log_warn, vha, 0x1015,
		       "RISC reset failed. mb[16-23] %04xh %04xh %04xh %04xh %04xh %04xh %04xh %04xh\n",
		       mb[16], mb[17], mb[18], mb[19], mb[20], mb[21], mb[22],
		       mb[23]);
		ql_log(ql_log_warn, vha, 0x1015,
		       "RISC reset failed. mb[24-31] %04xh %04xh %04xh %04xh %04xh %04xh %04xh %04xh\n",
		       mb[24], mb[25], mb[26], mb[27], mb[28], mb[29], mb[30],
		       mb[31]);
	}
	return rc;
}

/**
 * qla24xx_reset_risc() - Perform full reset of ISP24xx RISC.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
static inline int
qla24xx_reset_risc(scsi_qla_host_t *vha)
{
	unsigned long flags = 0;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_24xx __iomem *reg = &ha->iobase->isp24;
	uint32_t cnt;
	uint16_t wd;
	static int abts_cnt; /* ISP abort retry counts */
	int rval = QLA_SUCCESS;
	int print = 1;

	spin_lock_irqsave(&ha->hardware_lock, flags);

	/* Reset RISC. */
	wrt_reg_dword(&reg->ctrl_status, CSRX_DMA_SHUTDOWN|MWB_4096_BYTES);
	for (cnt = 0; cnt < 30000; cnt++) {
		if ((rd_reg_dword(&reg->ctrl_status) & CSRX_DMA_ACTIVE) == 0)
			break;

		udelay(10);
	}

	if (!(rd_reg_dword(&reg->ctrl_status) & CSRX_DMA_ACTIVE))
		set_bit(DMA_SHUTDOWN_CMPL, &ha->fw_dump_cap_flags);

	ql_dbg(ql_dbg_init + ql_dbg_verbose, vha, 0x017e,
	    "HCCR: 0x%x, Control Status %x, DMA active status:0x%x\n",
	    rd_reg_dword(&reg->hccr),
	    rd_reg_dword(&reg->ctrl_status),
	    (rd_reg_dword(&reg->ctrl_status) & CSRX_DMA_ACTIVE));

	wrt_reg_dword(&reg->ctrl_status,
	    CSRX_ISP_SOFT_RESET|CSRX_DMA_SHUTDOWN|MWB_4096_BYTES);
	pci_read_config_word(ha->pdev, PCI_COMMAND, &wd);

	udelay(100);

	/* Wait for firmware to complete NVRAM accesses. */
	rd_reg_word(&reg->mailbox0);
	for (cnt = 10000; rd_reg_word(&reg->mailbox0) != 0 &&
	    rval == QLA_SUCCESS; cnt--) {
		barrier();
		if (cnt)
			udelay(5);
		else
			rval = QLA_FUNCTION_TIMEOUT;
	}

	if (rval == QLA_SUCCESS)
		set_bit(ISP_MBX_RDY, &ha->fw_dump_cap_flags);

	ql_dbg(ql_dbg_init + ql_dbg_verbose, vha, 0x017f,
	    "HCCR: 0x%x, MailBox0 Status 0x%x\n",
	    rd_reg_dword(&reg->hccr),
	    rd_reg_word(&reg->mailbox0));

	/* Wait for soft-reset to complete. */
	rd_reg_dword(&reg->ctrl_status);
	for (cnt = 0; cnt < 60; cnt++) {
		barrier();
		if ((rd_reg_dword(&reg->ctrl_status) &
		    CSRX_ISP_SOFT_RESET) == 0)
			break;

		udelay(5);
	}
	if (!(rd_reg_dword(&reg->ctrl_status) & CSRX_ISP_SOFT_RESET))
		set_bit(ISP_SOFT_RESET_CMPL, &ha->fw_dump_cap_flags);

	ql_dbg(ql_dbg_init + ql_dbg_verbose, vha, 0x015d,
	    "HCCR: 0x%x, Soft Reset status: 0x%x\n",
	    rd_reg_dword(&reg->hccr),
	    rd_reg_dword(&reg->ctrl_status));

	/* If required, do an MPI FW reset now */
	if (test_and_clear_bit(MPI_RESET_NEEDED, &vha->dpc_flags)) {
		if (qla81xx_reset_mpi(vha) != QLA_SUCCESS) {
			if (++abts_cnt < 5) {
				set_bit(ISP_ABORT_NEEDED, &vha->dpc_flags);
				set_bit(MPI_RESET_NEEDED, &vha->dpc_flags);
			} else {
				/*
				 * We exhausted the ISP abort retries. We have to
				 * set the board offline.
				 */
				abts_cnt = 0;
				vha->flags.online = 0;
			}
		}
	}

	wrt_reg_dword(&reg->hccr, HCCRX_SET_RISC_RESET);
	rd_reg_dword(&reg->hccr);

	wrt_reg_dword(&reg->hccr, HCCRX_REL_RISC_PAUSE);
	rd_reg_dword(&reg->hccr);

	wrt_reg_dword(&reg->hccr, HCCRX_CLR_RISC_RESET);
	mdelay(10);
	rd_reg_dword(&reg->hccr);

	wd = rd_reg_word(&reg->mailbox0);
	for (cnt = 300; wd != 0 && rval == QLA_SUCCESS; cnt--) {
		barrier();
		if (cnt) {
			mdelay(1);
			if (print && qla_chk_risc_recovery(vha))
				print = 0;

			wd = rd_reg_word(&reg->mailbox0);
		} else {
			rval = QLA_FUNCTION_TIMEOUT;

			ql_log(ql_log_warn, vha, 0x015e,
			       "RISC reset timeout\n");
		}
	}

	if (rval == QLA_SUCCESS)
		set_bit(RISC_RDY_AFT_RESET, &ha->fw_dump_cap_flags);

	ql_dbg(ql_dbg_init + ql_dbg_verbose, vha, 0x015e,
	    "Host Risc 0x%x, mailbox0 0x%x\n",
	    rd_reg_dword(&reg->hccr),
	     rd_reg_word(&reg->mailbox0));

	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	ql_dbg(ql_dbg_init + ql_dbg_verbose, vha, 0x015f,
	    "Driver in %s mode\n",
	    IS_NOPOLLING_TYPE(ha) ? "Interrupt" : "Polling");

	if (IS_NOPOLLING_TYPE(ha))
		ha->isp_ops->enable_intrs(ha);

	return rval;
}

static void
qla25xx_read_risc_sema_reg(scsi_qla_host_t *vha, uint32_t *data)
{
	struct device_reg_24xx __iomem *reg = &vha->hw->iobase->isp24;

	wrt_reg_dword(&reg->iobase_addr, RISC_REGISTER_BASE_OFFSET);
	*data = rd_reg_dword(&reg->iobase_window + RISC_REGISTER_WINDOW_OFFSET);
}

static void
qla25xx_write_risc_sema_reg(scsi_qla_host_t *vha, uint32_t data)
{
	struct device_reg_24xx __iomem *reg = &vha->hw->iobase->isp24;

	wrt_reg_dword(&reg->iobase_addr, RISC_REGISTER_BASE_OFFSET);
	wrt_reg_dword(&reg->iobase_window + RISC_REGISTER_WINDOW_OFFSET, data);
}

static void
qla25xx_manipulate_risc_semaphore(scsi_qla_host_t *vha)
{
	uint32_t wd32 = 0;
	uint delta_msec = 100;
	uint elapsed_msec = 0;
	uint timeout_msec;
	ulong n;

	if (vha->hw->pdev->subsystem_device != 0x0175 &&
	    vha->hw->pdev->subsystem_device != 0x0240)
		return;

	wrt_reg_dword(&vha->hw->iobase->isp24.hccr, HCCRX_SET_RISC_PAUSE);
	udelay(100);

attempt:
	timeout_msec = TIMEOUT_SEMAPHORE;
	n = timeout_msec / delta_msec;
	while (n--) {
		qla25xx_write_risc_sema_reg(vha, RISC_SEMAPHORE_SET);
		qla25xx_read_risc_sema_reg(vha, &wd32);
		if (wd32 & RISC_SEMAPHORE)
			break;
		msleep(delta_msec);
		elapsed_msec += delta_msec;
		if (elapsed_msec > TIMEOUT_TOTAL_ELAPSED)
			goto force;
	}

	if (!(wd32 & RISC_SEMAPHORE))
		goto force;

	if (!(wd32 & RISC_SEMAPHORE_FORCE))
		goto acquired;

	qla25xx_write_risc_sema_reg(vha, RISC_SEMAPHORE_CLR);
	timeout_msec = TIMEOUT_SEMAPHORE_FORCE;
	n = timeout_msec / delta_msec;
	while (n--) {
		qla25xx_read_risc_sema_reg(vha, &wd32);
		if (!(wd32 & RISC_SEMAPHORE_FORCE))
			break;
		msleep(delta_msec);
		elapsed_msec += delta_msec;
		if (elapsed_msec > TIMEOUT_TOTAL_ELAPSED)
			goto force;
	}

	if (wd32 & RISC_SEMAPHORE_FORCE)
		qla25xx_write_risc_sema_reg(vha, RISC_SEMAPHORE_FORCE_CLR);

	goto attempt;

force:
	qla25xx_write_risc_sema_reg(vha, RISC_SEMAPHORE_FORCE_SET);

acquired:
	return;
}

/**
 * qla24xx_reset_chip() - Reset ISP24xx chip.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla24xx_reset_chip(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	int rval = QLA_FUNCTION_FAILED;

	if (pci_channel_offline(ha->pdev) &&
	    ha->flags.pci_channel_io_perm_failure) {
		return rval;
	}

	ha->isp_ops->disable_intrs(ha);

	qla25xx_manipulate_risc_semaphore(vha);

	/* Perform RISC reset. */
	rval = qla24xx_reset_risc(vha);

	return rval;
}

/**
 * qla2x00_chip_diag() - Test chip for proper operation.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla2x00_chip_diag(scsi_qla_host_t *vha)
{
	int		rval;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;
	unsigned long	flags = 0;
	uint16_t	data;
	uint32_t	cnt;
	uint16_t	mb[5];
	struct req_que *req = ha->req_q_map[0];

	/* Assume a failed state */
	rval = QLA_FUNCTION_FAILED;

	ql_dbg(ql_dbg_init, vha, 0x007b, "Testing device at %p.\n",
	       &reg->flash_address);

	spin_lock_irqsave(&ha->hardware_lock, flags);

	/* Reset ISP chip. */
	wrt_reg_word(&reg->ctrl_status, CSR_ISP_SOFT_RESET);

	/*
	 * We need to have a delay here since the card will not respond while
	 * in reset causing an MCA on some architectures.
	 */
	udelay(20);
	data = qla2x00_debounce_register(&reg->ctrl_status);
	for (cnt = 6000000 ; cnt && (data & CSR_ISP_SOFT_RESET); cnt--) {
		udelay(5);
		data = rd_reg_word(&reg->ctrl_status);
		barrier();
	}

	if (!cnt)
		goto chip_diag_failed;

	ql_dbg(ql_dbg_init, vha, 0x007c,
	    "Reset register cleared by chip reset.\n");

	/* Reset RISC processor. */
	wrt_reg_word(&reg->hccr, HCCR_RESET_RISC);
	wrt_reg_word(&reg->hccr, HCCR_RELEASE_RISC);

	/* Workaround for QLA2312 PCI parity error */
	if (IS_QLA2100(ha) || IS_QLA2200(ha) || IS_QLA2300(ha)) {
		data = qla2x00_debounce_register(MAILBOX_REG(ha, reg, 0));
		for (cnt = 6000000; cnt && (data == MBS_BUSY); cnt--) {
			udelay(5);
			data = RD_MAILBOX_REG(ha, reg, 0);
			barrier();
		}
	} else
		udelay(10);

	if (!cnt)
		goto chip_diag_failed;

	/* Check product ID of chip */
	ql_dbg(ql_dbg_init, vha, 0x007d, "Checking product ID of chip.\n");

	mb[1] = RD_MAILBOX_REG(ha, reg, 1);
	mb[2] = RD_MAILBOX_REG(ha, reg, 2);
	mb[3] = RD_MAILBOX_REG(ha, reg, 3);
	mb[4] = qla2x00_debounce_register(MAILBOX_REG(ha, reg, 4));
	if (mb[1] != PROD_ID_1 || (mb[2] != PROD_ID_2 && mb[2] != PROD_ID_2a) ||
	    mb[3] != PROD_ID_3) {
		ql_log(ql_log_warn, vha, 0x0062,
		    "Wrong product ID = 0x%x,0x%x,0x%x.\n",
		    mb[1], mb[2], mb[3]);

		goto chip_diag_failed;
	}
	ha->product_id[0] = mb[1];
	ha->product_id[1] = mb[2];
	ha->product_id[2] = mb[3];
	ha->product_id[3] = mb[4];

	/* Adjust fw RISC transfer size */
	if (req->length > 1024)
		ha->fw_transfer_size = REQUEST_ENTRY_SIZE * 1024;
	else
		ha->fw_transfer_size = REQUEST_ENTRY_SIZE *
		    req->length;

	if (IS_QLA2200(ha) &&
	    RD_MAILBOX_REG(ha, reg, 7) == QLA2200A_RISC_ROM_VER) {
		/* Limit firmware transfer size with a 2200A */
		ql_dbg(ql_dbg_init, vha, 0x007e, "Found QLA2200A Chip.\n");

		ha->device_type |= DT_ISP2200A;
		ha->fw_transfer_size = 128;
	}

	/* Wrap Incoming Mailboxes Test. */
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	ql_dbg(ql_dbg_init, vha, 0x007f, "Checking mailboxes.\n");
	rval = qla2x00_mbx_reg_test(vha);
	if (rval)
		ql_log(ql_log_warn, vha, 0x0080,
		    "Failed mailbox send register test.\n");
	else
		/* Flag a successful rval */
		rval = QLA_SUCCESS;
	spin_lock_irqsave(&ha->hardware_lock, flags);

chip_diag_failed:
	if (rval)
		ql_log(ql_log_info, vha, 0x0081,
		    "Chip diagnostics **** FAILED ****.\n");

	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	return (rval);
}

/**
 * qla24xx_chip_diag() - Test ISP24xx for proper operation.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
int
qla24xx_chip_diag(scsi_qla_host_t *vha)
{
	int rval;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req = ha->req_q_map[0];

	if (IS_P3P_TYPE(ha))
		return QLA_SUCCESS;

	ha->fw_transfer_size = REQUEST_ENTRY_SIZE * req->length;

	rval = qla2x00_mbx_reg_test(vha);
	if (rval) {
		ql_log(ql_log_warn, vha, 0x0082,
		    "Failed mailbox send register test.\n");
	} else {
		/* Flag a successful rval */
		rval = QLA_SUCCESS;
	}

	return rval;
}

static void
qla2x00_init_fce_trace(scsi_qla_host_t *vha)
{
	int rval;
	dma_addr_t tc_dma;
	void *tc;
	struct qla_hw_data *ha = vha->hw;

	if (!IS_FWI2_CAPABLE(ha))
		return;

	if (!IS_QLA25XX(ha) && !IS_QLA81XX(ha) && !IS_QLA83XX(ha) &&
	    !IS_QLA27XX(ha) && !IS_QLA28XX(ha))
		return;

	if (ha->fce) {
		ql_dbg(ql_dbg_init, vha, 0x00bd,
		       "%s: FCE Mem is already allocated.\n",
		       __func__);
		return;
	}

	/* Allocate memory for Fibre Channel Event Buffer. */
	tc = dma_alloc_coherent(&ha->pdev->dev, FCE_SIZE, &tc_dma,
				GFP_KERNEL);
	if (!tc) {
		ql_log(ql_log_warn, vha, 0x00be,
		       "Unable to allocate (%d KB) for FCE.\n",
		       FCE_SIZE / 1024);
		return;
	}

	rval = qla2x00_enable_fce_trace(vha, tc_dma, FCE_NUM_BUFFERS,
					ha->fce_mb, &ha->fce_bufs);
	if (rval) {
		ql_log(ql_log_warn, vha, 0x00bf,
		       "Unable to initialize FCE (%d).\n", rval);
		dma_free_coherent(&ha->pdev->dev, FCE_SIZE, tc, tc_dma);
		return;
	}

	ql_dbg(ql_dbg_init, vha, 0x00c0,
	       "Allocated (%d KB) for FCE...\n", FCE_SIZE / 1024);

	ha->flags.fce_enabled = 1;
	ha->fce_dma = tc_dma;
	ha->fce = tc;
}

static void
qla2x00_init_eft_trace(scsi_qla_host_t *vha)
{
	int rval;
	dma_addr_t tc_dma;
	void *tc;
	struct qla_hw_data *ha = vha->hw;

	if (!IS_FWI2_CAPABLE(ha))
		return;

	if (ha->eft) {
		ql_dbg(ql_dbg_init, vha, 0x00bd,
		    "%s: EFT Mem is already allocated.\n",
		    __func__);
		return;
	}

	/* Allocate memory for Extended Trace Buffer. */
	tc = dma_alloc_coherent(&ha->pdev->dev, EFT_SIZE, &tc_dma,
				GFP_KERNEL);
	if (!tc) {
		ql_log(ql_log_warn, vha, 0x00c1,
		       "Unable to allocate (%d KB) for EFT.\n",
		       EFT_SIZE / 1024);
		return;
	}

	rval = qla2x00_enable_eft_trace(vha, tc_dma, EFT_NUM_BUFFERS);
	if (rval) {
		ql_log(ql_log_warn, vha, 0x00c2,
		       "Unable to initialize EFT (%d).\n", rval);
		dma_free_coherent(&ha->pdev->dev, EFT_SIZE, tc, tc_dma);
		return;
	}

	ql_dbg(ql_dbg_init, vha, 0x00c3,
	       "Allocated (%d KB) EFT ...\n", EFT_SIZE / 1024);

	ha->eft_dma = tc_dma;
	ha->eft = tc;
}

static void
qla2x00_alloc_offload_mem(scsi_qla_host_t *vha)
{
	qla2x00_init_fce_trace(vha);
	qla2x00_init_eft_trace(vha);
}

void
qla2x00_alloc_fw_dump(scsi_qla_host_t *vha)
{
	uint32_t dump_size, fixed_size, mem_size, req_q_size, rsp_q_size,
	    eft_size, fce_size, mq_size;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req = ha->req_q_map[0];
	struct rsp_que *rsp = ha->rsp_q_map[0];
	struct qla2xxx_fw_dump *fw_dump;

	dump_size = fixed_size = mem_size = eft_size = fce_size = mq_size = 0;
	req_q_size = rsp_q_size = 0;

	if (IS_QLA2100(ha) || IS_QLA2200(ha)) {
		fixed_size = sizeof(struct qla2100_fw_dump);
	} else if (IS_QLA23XX(ha)) {
		fixed_size = offsetof(struct qla2300_fw_dump, data_ram);
		mem_size = (ha->fw_memory_size - 0x11000 + 1) *
		    sizeof(uint16_t);
	} else if (IS_FWI2_CAPABLE(ha)) {
		if (IS_QLA83XX(ha) || IS_QLA27XX(ha) || IS_QLA28XX(ha))
			fixed_size = offsetof(struct qla83xx_fw_dump, ext_mem);
		else if (IS_QLA81XX(ha))
			fixed_size = offsetof(struct qla81xx_fw_dump, ext_mem);
		else if (IS_QLA25XX(ha))
			fixed_size = offsetof(struct qla25xx_fw_dump, ext_mem);
		else
			fixed_size = offsetof(struct qla24xx_fw_dump, ext_mem);

		mem_size = (ha->fw_memory_size - 0x100000 + 1) *
		    sizeof(uint32_t);
		if (ha->mqenable) {
			if (!IS_QLA83XX(ha) && !IS_QLA27XX(ha) &&
			    !IS_QLA28XX(ha))
				mq_size = sizeof(struct qla2xxx_mq_chain);
			/*
			 * Allocate maximum buffer size for all queues - Q0.
			 * Resizing must be done at end-of-dump processing.
			 */
			mq_size += (ha->max_req_queues - 1) *
			    (req->length * sizeof(request_t));
			mq_size += (ha->max_rsp_queues - 1) *
			    (rsp->length * sizeof(response_t));
		}
		if (ha->tgt.atio_ring)
			mq_size += ha->tgt.atio_q_length * sizeof(request_t);

		qla2x00_init_fce_trace(vha);
		if (ha->fce)
			fce_size = sizeof(struct qla2xxx_fce_chain) + FCE_SIZE;
		qla2x00_init_eft_trace(vha);
		if (ha->eft)
			eft_size = EFT_SIZE;
	}

	if (IS_QLA27XX(ha) || IS_QLA28XX(ha)) {
		struct fwdt *fwdt = ha->fwdt;
		uint j;

		for (j = 0; j < 2; j++, fwdt++) {
			if (!fwdt->template) {
				ql_dbg(ql_dbg_init, vha, 0x00ba,
				    "-> fwdt%u no template\n", j);
				continue;
			}
			ql_dbg(ql_dbg_init, vha, 0x00fa,
			    "-> fwdt%u calculating fwdump size...\n", j);
			fwdt->dump_size = qla27xx_fwdt_calculate_dump_size(
			    vha, fwdt->template);
			ql_dbg(ql_dbg_init, vha, 0x00fa,
			    "-> fwdt%u calculated fwdump size = %#lx bytes\n",
			    j, fwdt->dump_size);
			dump_size += fwdt->dump_size;
		}
		/* Add space for spare MPI fw dump. */
		dump_size += ha->fwdt[1].dump_size;
	} else {
		req_q_size = req->length * sizeof(request_t);
		rsp_q_size = rsp->length * sizeof(response_t);
		dump_size = offsetof(struct qla2xxx_fw_dump, isp);
		dump_size += fixed_size + mem_size + req_q_size + rsp_q_size
			+ eft_size;
		ha->chain_offset = dump_size;
		dump_size += mq_size + fce_size;
		if (ha->exchoffld_buf)
			dump_size += sizeof(struct qla2xxx_offld_chain) +
				ha->exchoffld_size;
		if (ha->exlogin_buf)
			dump_size += sizeof(struct qla2xxx_offld_chain) +
				ha->exlogin_size;
	}

	if (!ha->fw_dump_len || dump_size > ha->fw_dump_alloc_len) {

		ql_dbg(ql_dbg_init, vha, 0x00c5,
		    "%s dump_size %d fw_dump_len %d fw_dump_alloc_len %d\n",
		    __func__, dump_size, ha->fw_dump_len,
		    ha->fw_dump_alloc_len);

		fw_dump = vmalloc(dump_size);
		if (!fw_dump) {
			ql_log(ql_log_warn, vha, 0x00c4,
			    "Unable to allocate (%d KB) for firmware dump.\n",
			    dump_size / 1024);
		} else {
			mutex_lock(&ha->optrom_mutex);
			if (ha->fw_dumped) {
				memcpy(fw_dump, ha->fw_dump, ha->fw_dump_len);
				vfree(ha->fw_dump);
				ha->fw_dump = fw_dump;
				ha->fw_dump_alloc_len =  dump_size;
				ql_dbg(ql_dbg_init, vha, 0x00c5,
				    "Re-Allocated (%d KB) and save firmware dump.\n",
				    dump_size / 1024);
			} else {
				vfree(ha->fw_dump);
				ha->fw_dump = fw_dump;

				ha->fw_dump_len = ha->fw_dump_alloc_len =
				    dump_size;
				ql_dbg(ql_dbg_init, vha, 0x00c5,
				    "Allocated (%d KB) for firmware dump.\n",
				    dump_size / 1024);

				if (IS_QLA27XX(ha) || IS_QLA28XX(ha)) {
					ha->mpi_fw_dump = (char *)fw_dump +
						ha->fwdt[1].dump_size;
					mutex_unlock(&ha->optrom_mutex);
					return;
				}

				ha->fw_dump->signature[0] = 'Q';
				ha->fw_dump->signature[1] = 'L';
				ha->fw_dump->signature[2] = 'G';
				ha->fw_dump->signature[3] = 'C';
				ha->fw_dump->version = htonl(1);

				ha->fw_dump->fixed_size = htonl(fixed_size);
				ha->fw_dump->mem_size = htonl(mem_size);
				ha->fw_dump->req_q_size = htonl(req_q_size);
				ha->fw_dump->rsp_q_size = htonl(rsp_q_size);

				ha->fw_dump->eft_size = htonl(eft_size);
				ha->fw_dump->eft_addr_l =
				    htonl(LSD(ha->eft_dma));
				ha->fw_dump->eft_addr_h =
				    htonl(MSD(ha->eft_dma));

				ha->fw_dump->header_size =
					htonl(offsetof
					    (struct qla2xxx_fw_dump, isp));
			}
			mutex_unlock(&ha->optrom_mutex);
		}
	}
}

static int
qla81xx_mpi_sync(scsi_qla_host_t *vha)
{
#define MPS_MASK	0xe0
	int rval;
	uint16_t dc;
	uint32_t dw;

	if (!IS_QLA81XX(vha->hw))
		return QLA_SUCCESS;

	rval = qla2x00_write_ram_word(vha, 0x7c00, 1);
	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x0105,
		    "Unable to acquire semaphore.\n");
		goto done;
	}

	pci_read_config_word(vha->hw->pdev, 0x54, &dc);
	rval = qla2x00_read_ram_word(vha, 0x7a15, &dw);
	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x0067, "Unable to read sync.\n");
		goto done_release;
	}

	dc &= MPS_MASK;
	if (dc == (dw & MPS_MASK))
		goto done_release;

	dw &= ~MPS_MASK;
	dw |= dc;
	rval = qla2x00_write_ram_word(vha, 0x7a15, dw);
	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x0114, "Unable to gain sync.\n");
	}

done_release:
	rval = qla2x00_write_ram_word(vha, 0x7c00, 0);
	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x006d,
		    "Unable to release semaphore.\n");
	}

done:
	return rval;
}

int
qla2x00_alloc_outstanding_cmds(struct qla_hw_data *ha, struct req_que *req)
{
	/* Don't try to reallocate the array */
	if (req->outstanding_cmds)
		return QLA_SUCCESS;

	if (!IS_FWI2_CAPABLE(ha))
		req->num_outstanding_cmds = DEFAULT_OUTSTANDING_COMMANDS;
	else {
		if (ha->cur_fw_xcb_count <= ha->cur_fw_iocb_count)
			req->num_outstanding_cmds = ha->cur_fw_xcb_count;
		else
			req->num_outstanding_cmds = ha->cur_fw_iocb_count;
	}

	req->outstanding_cmds = kcalloc(req->num_outstanding_cmds,
					sizeof(srb_t *),
					GFP_KERNEL);

	if (!req->outstanding_cmds) {
		/*
		 * Try to allocate a minimal size just so we can get through
		 * initialization.
		 */
		req->num_outstanding_cmds = MIN_OUTSTANDING_COMMANDS;
		req->outstanding_cmds = kcalloc(req->num_outstanding_cmds,
						sizeof(srb_t *),
						GFP_KERNEL);

		if (!req->outstanding_cmds) {
			ql_log(ql_log_fatal, NULL, 0x0126,
			    "Failed to allocate memory for "
			    "outstanding_cmds for req_que %p.\n", req);
			req->num_outstanding_cmds = 0;
			return QLA_FUNCTION_FAILED;
		}
	}

	return QLA_SUCCESS;
}

#define PRINT_FIELD(_field, _flag, _str) {		\
	if (a0->_field & _flag) {\
		if (p) {\
			strcat(ptr, "|");\
			ptr++;\
			leftover--;\
		} \
		len = snprintf(ptr, leftover, "%s", _str);	\
		p = 1;\
		leftover -= len;\
		ptr += len; \
	} \
}

static void qla2xxx_print_sfp_info(struct scsi_qla_host *vha)
{
#define STR_LEN 64
	struct sff_8247_a0 *a0 = (struct sff_8247_a0 *)vha->hw->sfp_data;
	u8 str[STR_LEN], *ptr, p;
	int leftover, len;

	memset(str, 0, STR_LEN);
	snprintf(str, SFF_VEN_NAME_LEN+1, a0->vendor_name);
	ql_dbg(ql_dbg_init, vha, 0x015a,
	    "SFP MFG Name: %s\n", str);

	memset(str, 0, STR_LEN);
	snprintf(str, SFF_PART_NAME_LEN+1, a0->vendor_pn);
	ql_dbg(ql_dbg_init, vha, 0x015c,
	    "SFP Part Name: %s\n", str);

	/* media */
	memset(str, 0, STR_LEN);
	ptr = str;
	leftover = STR_LEN;
	p = len = 0;
	PRINT_FIELD(fc_med_cc9, FC_MED_TW, "Twin AX");
	PRINT_FIELD(fc_med_cc9, FC_MED_TP, "Twisted Pair");
	PRINT_FIELD(fc_med_cc9, FC_MED_MI, "Min Coax");
	PRINT_FIELD(fc_med_cc9, FC_MED_TV, "Video Coax");
	PRINT_FIELD(fc_med_cc9, FC_MED_M6, "MultiMode 62.5um");
	PRINT_FIELD(fc_med_cc9, FC_MED_M5, "MultiMode 50um");
	PRINT_FIELD(fc_med_cc9, FC_MED_SM, "SingleMode");
	ql_dbg(ql_dbg_init, vha, 0x0160,
	    "SFP Media: %s\n", str);

	/* link length */
	memset(str, 0, STR_LEN);
	ptr = str;
	leftover = STR_LEN;
	p = len = 0;
	PRINT_FIELD(fc_ll_cc7, FC_LL_VL, "Very Long");
	PRINT_FIELD(fc_ll_cc7, FC_LL_S, "Short");
	PRINT_FIELD(fc_ll_cc7, FC_LL_I, "Intermediate");
	PRINT_FIELD(fc_ll_cc7, FC_LL_L, "Long");
	PRINT_FIELD(fc_ll_cc7, FC_LL_M, "Medium");
	ql_dbg(ql_dbg_init, vha, 0x0196,
	    "SFP Link Length: %s\n", str);

	memset(str, 0, STR_LEN);
	ptr = str;
	leftover = STR_LEN;
	p = len = 0;
	PRINT_FIELD(fc_ll_cc7, FC_LL_SA, "Short Wave (SA)");
	PRINT_FIELD(fc_ll_cc7, FC_LL_LC, "Long Wave(LC)");
	PRINT_FIELD(fc_tec_cc8, FC_TEC_SN, "Short Wave (SN)");
	PRINT_FIELD(fc_tec_cc8, FC_TEC_SL, "Short Wave (SL)");
	PRINT_FIELD(fc_tec_cc8, FC_TEC_LL, "Long Wave (LL)");
	ql_dbg(ql_dbg_init, vha, 0x016e,
	    "SFP FC Link Tech: %s\n", str);

	if (a0->length_km)
		ql_dbg(ql_dbg_init, vha, 0x016f,
		    "SFP Distant: %d km\n", a0->length_km);
	if (a0->length_100m)
		ql_dbg(ql_dbg_init, vha, 0x0170,
		    "SFP Distant: %d m\n", a0->length_100m*100);
	if (a0->length_50um_10m)
		ql_dbg(ql_dbg_init, vha, 0x0189,
		    "SFP Distant (WL=50um): %d m\n", a0->length_50um_10m * 10);
	if (a0->length_62um_10m)
		ql_dbg(ql_dbg_init, vha, 0x018a,
		  "SFP Distant (WL=62.5um): %d m\n", a0->length_62um_10m * 10);
	if (a0->length_om4_10m)
		ql_dbg(ql_dbg_init, vha, 0x0194,
		    "SFP Distant (OM4): %d m\n", a0->length_om4_10m * 10);
	if (a0->length_om3_10m)
		ql_dbg(ql_dbg_init, vha, 0x0195,
		    "SFP Distant (OM3): %d m\n", a0->length_om3_10m * 10);
}


/**
 * qla24xx_detect_sfp()
 *
 * @vha: adapter state pointer.
 *
 * @return
 *	0 -- Configure firmware to use short-range settings -- normal
 *	     buffer-to-buffer credits.
 *
 *	1 -- Configure firmware to use long-range settings -- extra
 *	     buffer-to-buffer credits should be allocated with
 *	     ha->lr_distance containing distance settings from NVRAM or SFP
 *	     (if supported).
 */
int
qla24xx_detect_sfp(scsi_qla_host_t *vha)
{
	int rc, used_nvram;
	struct sff_8247_a0 *a;
	struct qla_hw_data *ha = vha->hw;
	struct nvram_81xx *nv = ha->nvram;
#define LR_DISTANCE_UNKNOWN	2
	static const char * const types[] = { "Short", "Long" };
	static const char * const lengths[] = { "(10km)", "(5km)", "" };
	u8 ll = 0;

	/* Seed with NVRAM settings. */
	used_nvram = 0;
	ha->flags.lr_detected = 0;
	if (IS_BPM_RANGE_CAPABLE(ha) &&
	    (nv->enhanced_features & NEF_LR_DIST_ENABLE)) {
		used_nvram = 1;
		ha->flags.lr_detected = 1;
		ha->lr_distance =
		    (nv->enhanced_features >> LR_DIST_NV_POS)
		     & LR_DIST_NV_MASK;
	}

	if (!IS_BPM_ENABLED(vha))
		goto out;
	/* Determine SR/LR capabilities of SFP/Transceiver. */
	rc = qla2x00_read_sfp_dev(vha, NULL, 0);
	if (rc)
		goto out;

	used_nvram = 0;
	a = (struct sff_8247_a0 *)vha->hw->sfp_data;
	qla2xxx_print_sfp_info(vha);

	ha->flags.lr_detected = 0;
	ll = a->fc_ll_cc7;
	if (ll & FC_LL_VL || ll & FC_LL_L) {
		/* Long range, track length. */
		ha->flags.lr_detected = 1;

		if (a->length_km > 5 || a->length_100m > 50)
			ha->lr_distance = LR_DISTANCE_10K;
		else
			ha->lr_distance = LR_DISTANCE_5K;
	}

out:
	ql_dbg(ql_dbg_async, vha, 0x507b,
	    "SFP detect: %s-Range SFP %s (nvr=%x ll=%x lr=%x lrd=%x).\n",
	    types[ha->flags.lr_detected],
	    ha->flags.lr_detected ? lengths[ha->lr_distance] :
	       lengths[LR_DISTANCE_UNKNOWN],
	    used_nvram, ll, ha->flags.lr_detected, ha->lr_distance);
	return ha->flags.lr_detected;
}

void qla_init_iocb_limit(scsi_qla_host_t *vha)
{
	u16 i, num_qps;
	u32 limit;
	struct qla_hw_data *ha = vha->hw;

	num_qps = ha->num_qpairs + 1;
	limit = (ha->orig_fw_iocb_count * QLA_IOCB_PCT_LIMIT) / 100;

	ha->base_qpair->fwres.iocbs_total = ha->orig_fw_iocb_count;
	ha->base_qpair->fwres.iocbs_limit = limit;
	ha->base_qpair->fwres.iocbs_qp_limit = limit / num_qps;
	ha->base_qpair->fwres.iocbs_used = 0;
	for (i = 0; i < ha->max_qpairs; i++) {
		if (ha->queue_pair_map[i])  {
			ha->queue_pair_map[i]->fwres.iocbs_total =
				ha->orig_fw_iocb_count;
			ha->queue_pair_map[i]->fwres.iocbs_limit = limit;
			ha->queue_pair_map[i]->fwres.iocbs_qp_limit =
				limit / num_qps;
			ha->queue_pair_map[i]->fwres.iocbs_used = 0;
		}
	}
}

/**
 * qla2x00_setup_chip() - Load and start RISC firmware.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
static int
qla2x00_setup_chip(scsi_qla_host_t *vha)
{
	int rval;
	uint32_t srisc_address = 0;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;
	unsigned long flags;
	uint16_t fw_major_version;
	int done_once = 0;

	if (IS_P3P_TYPE(ha)) {
		rval = ha->isp_ops->load_risc(vha, &srisc_address);
		if (rval == QLA_SUCCESS) {
			qla2x00_stop_firmware(vha);
			goto enable_82xx_npiv;
		} else
			goto failed;
	}

	if (!IS_FWI2_CAPABLE(ha) && !IS_QLA2100(ha) && !IS_QLA2200(ha)) {
		/* Disable SRAM, Instruction RAM and GP RAM parity.  */
		spin_lock_irqsave(&ha->hardware_lock, flags);
		wrt_reg_word(&reg->hccr, (HCCR_ENABLE_PARITY + 0x0));
		rd_reg_word(&reg->hccr);
		spin_unlock_irqrestore(&ha->hardware_lock, flags);
	}

	qla81xx_mpi_sync(vha);

execute_fw_with_lr:
	/* Load firmware sequences */
	rval = ha->isp_ops->load_risc(vha, &srisc_address);
	if (rval == QLA_SUCCESS) {
		ql_dbg(ql_dbg_init, vha, 0x00c9,
		    "Verifying Checksum of loaded RISC code.\n");

		rval = qla2x00_verify_checksum(vha, srisc_address);
		if (rval == QLA_SUCCESS) {
			/* Start firmware execution. */
			ql_dbg(ql_dbg_init, vha, 0x00ca,
			    "Starting firmware.\n");

			if (ql2xexlogins)
				ha->flags.exlogins_enabled = 1;

			if (qla_is_exch_offld_enabled(vha))
				ha->flags.exchoffld_enabled = 1;

			rval = qla2x00_execute_fw(vha, srisc_address);
			/* Retrieve firmware information. */
			if (rval == QLA_SUCCESS) {
				/* Enable BPM support? */
				if (!done_once++ && qla24xx_detect_sfp(vha)) {
					ql_dbg(ql_dbg_init, vha, 0x00ca,
					    "Re-starting firmware -- BPM.\n");
					/* Best-effort - re-init. */
					ha->isp_ops->reset_chip(vha);
					ha->isp_ops->chip_diag(vha);
					goto execute_fw_with_lr;
				}

				if (IS_ZIO_THRESHOLD_CAPABLE(ha))
					qla27xx_set_zio_threshold(vha,
					    ha->last_zio_threshold);

				rval = qla2x00_set_exlogins_buffer(vha);
				if (rval != QLA_SUCCESS)
					goto failed;

				rval = qla2x00_set_exchoffld_buffer(vha);
				if (rval != QLA_SUCCESS)
					goto failed;

enable_82xx_npiv:
				fw_major_version = ha->fw_major_version;
				if (IS_P3P_TYPE(ha))
					qla82xx_check_md_needed(vha);
				else
					rval = qla2x00_get_fw_version(vha);
				if (rval != QLA_SUCCESS)
					goto failed;
				ha->flags.npiv_supported = 0;
				if (IS_QLA2XXX_MIDTYPE(ha) &&
					 (ha->fw_attributes & BIT_2)) {
					ha->flags.npiv_supported = 1;
					if ((!ha->max_npiv_vports) ||
					    ((ha->max_npiv_vports + 1) %
					    MIN_MULTI_ID_FABRIC))
						ha->max_npiv_vports =
						    MIN_MULTI_ID_FABRIC - 1;
				}
				qla2x00_get_resource_cnts(vha);
				qla_init_iocb_limit(vha);

				/*
				 * Allocate the array of outstanding commands
				 * now that we know the firmware resources.
				 */
				rval = qla2x00_alloc_outstanding_cmds(ha,
				    vha->req);
				if (rval != QLA_SUCCESS)
					goto failed;

				if (!fw_major_version && !(IS_P3P_TYPE(ha)))
					qla2x00_alloc_offload_mem(vha);

				if (ql2xallocfwdump && !(IS_P3P_TYPE(ha)))
					qla2x00_alloc_fw_dump(vha);

			} else {
				goto failed;
			}
		} else {
			ql_log(ql_log_fatal, vha, 0x00cd,
			    "ISP Firmware failed checksum.\n");
			goto failed;
		}

		/* Enable PUREX PASSTHRU */
		if (ql2xrdpenable || ha->flags.scm_supported_f)
			qla25xx_set_els_cmds_supported(vha);
	} else
		goto failed;

	if (!IS_FWI2_CAPABLE(ha) && !IS_QLA2100(ha) && !IS_QLA2200(ha)) {
		/* Enable proper parity. */
		spin_lock_irqsave(&ha->hardware_lock, flags);
		if (IS_QLA2300(ha))
			/* SRAM parity */
			wrt_reg_word(&reg->hccr, HCCR_ENABLE_PARITY + 0x1);
		else
			/* SRAM, Instruction RAM and GP RAM parity */
			wrt_reg_word(&reg->hccr, HCCR_ENABLE_PARITY + 0x7);
		rd_reg_word(&reg->hccr);
		spin_unlock_irqrestore(&ha->hardware_lock, flags);
	}

	if (IS_QLA27XX(ha) || IS_QLA28XX(ha))
		ha->flags.fac_supported = 1;
	else if (rval == QLA_SUCCESS && IS_FAC_REQUIRED(ha)) {
		uint32_t size;

		rval = qla81xx_fac_get_sector_size(vha, &size);
		if (rval == QLA_SUCCESS) {
			ha->flags.fac_supported = 1;
			ha->fdt_block_size = size << 2;
		} else {
			ql_log(ql_log_warn, vha, 0x00ce,
			    "Unsupported FAC firmware (%d.%02d.%02d).\n",
			    ha->fw_major_version, ha->fw_minor_version,
			    ha->fw_subminor_version);

			if (IS_QLA83XX(ha) || IS_QLA27XX(ha) ||
			    IS_QLA28XX(ha)) {
				ha->flags.fac_supported = 0;
				rval = QLA_SUCCESS;
			}
		}
	}
failed:
	if (rval) {
		ql_log(ql_log_fatal, vha, 0x00cf,
		    "Setup chip ****FAILED****.\n");
	}

	return (rval);
}

/**
 * qla2x00_init_response_q_entries() - Initializes response queue entries.
 * @rsp: response queue
 *
 * Beginning of request ring has initialization control block already built
 * by nvram config routine.
 *
 * Returns 0 on success.
 */
void
qla2x00_init_response_q_entries(struct rsp_que *rsp)
{
	uint16_t cnt;
	response_t *pkt;

	rsp->ring_ptr = rsp->ring;
	rsp->ring_index    = 0;
	rsp->status_srb = NULL;
	pkt = rsp->ring_ptr;
	for (cnt = 0; cnt < rsp->length; cnt++) {
		pkt->signature = RESPONSE_PROCESSED;
		pkt++;
	}
}

/**
 * qla2x00_update_fw_options() - Read and process firmware options.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
void
qla2x00_update_fw_options(scsi_qla_host_t *vha)
{
	uint16_t swing, emphasis, tx_sens, rx_sens;
	struct qla_hw_data *ha = vha->hw;

	memset(ha->fw_options, 0, sizeof(ha->fw_options));
	qla2x00_get_fw_options(vha, ha->fw_options);

	if (IS_QLA2100(ha) || IS_QLA2200(ha))
		return;

	/* Serial Link options. */
	ql_dbg(ql_dbg_init + ql_dbg_buffer, vha, 0x0115,
	    "Serial link options.\n");
	ql_dump_buffer(ql_dbg_init + ql_dbg_buffer, vha, 0x0109,
	    ha->fw_seriallink_options, sizeof(ha->fw_seriallink_options));

	ha->fw_options[1] &= ~FO1_SET_EMPHASIS_SWING;
	if (ha->fw_seriallink_options[3] & BIT_2) {
		ha->fw_options[1] |= FO1_SET_EMPHASIS_SWING;

		/*  1G settings */
		swing = ha->fw_seriallink_options[2] & (BIT_2 | BIT_1 | BIT_0);
		emphasis = (ha->fw_seriallink_options[2] &
		    (BIT_4 | BIT_3)) >> 3;
		tx_sens = ha->fw_seriallink_options[0] &
		    (BIT_3 | BIT_2 | BIT_1 | BIT_0);
		rx_sens = (ha->fw_seriallink_options[0] &
		    (BIT_7 | BIT_6 | BIT_5 | BIT_4)) >> 4;
		ha->fw_options[10] = (emphasis << 14) | (swing << 8);
		if (IS_QLA2300(ha) || IS_QLA2312(ha) || IS_QLA6312(ha)) {
			if (rx_sens == 0x0)
				rx_sens = 0x3;
			ha->fw_options[10] |= (tx_sens << 4) | rx_sens;
		} else if (IS_QLA2322(ha) || IS_QLA6322(ha))
			ha->fw_options[10] |= BIT_5 |
			    ((rx_sens & (BIT_1 | BIT_0)) << 2) |
			    (tx_sens & (BIT_1 | BIT_0));

		/*  2G settings */
		swing = (ha->fw_seriallink_options[2] &
		    (BIT_7 | BIT_6 | BIT_5)) >> 5;
		emphasis = ha->fw_seriallink_options[3] & (BIT_1 | BIT_0);
		tx_sens = ha->fw_seriallink_options[1] &
		    (BIT_3 | BIT_2 | BIT_1 | BIT_0);
		rx_sens = (ha->fw_seriallink_options[1] &
		    (BIT_7 | BIT_6 | BIT_5 | BIT_4)) >> 4;
		ha->fw_options[11] = (emphasis << 14) | (swing << 8);
		if (IS_QLA2300(ha) || IS_QLA2312(ha) || IS_QLA6312(ha)) {
			if (rx_sens == 0x0)
				rx_sens = 0x3;
			ha->fw_options[11] |= (tx_sens << 4) | rx_sens;
		} else if (IS_QLA2322(ha) || IS_QLA6322(ha))
			ha->fw_options[11] |= BIT_5 |
			    ((rx_sens & (BIT_1 | BIT_0)) << 2) |
			    (tx_sens & (BIT_1 | BIT_0));
	}

	/* FCP2 options. */
	/*  Return command IOCBs without waiting for an ABTS to complete. */
	ha->fw_options[3] |= BIT_13;

	/* LED scheme. */
	if (ha->flags.enable_led_scheme)
		ha->fw_options[2] |= BIT_12;

	/* Detect ISP6312. */
	if (IS_QLA6312(ha))
		ha->fw_options[2] |= BIT_13;

	/* Set Retry FLOGI in case of P2P connection */
	if (ha->operating_mode == P2P) {
		ha->fw_options[2] |= BIT_3;
		ql_dbg(ql_dbg_disc, vha, 0x2100,
		    "(%s): Setting FLOGI retry BIT in fw_options[2]: 0x%x\n",
			__func__, ha->fw_options[2]);
	}

	/* Update firmware options. */
	qla2x00_set_fw_options(vha, ha->fw_options);
}

void
qla24xx_update_fw_options(scsi_qla_host_t *vha)
{
	int rval;
	struct qla_hw_data *ha = vha->hw;

	if (IS_P3P_TYPE(ha))
		return;

	/*  Hold status IOCBs until ABTS response received. */
	if (ql2xfwholdabts)
		ha->fw_options[3] |= BIT_12;

	/* Set Retry FLOGI in case of P2P connection */
	if (ha->operating_mode == P2P) {
		ha->fw_options[2] |= BIT_3;
		ql_dbg(ql_dbg_disc, vha, 0x2101,
		    "(%s): Setting FLOGI retry BIT in fw_options[2]: 0x%x\n",
			__func__, ha->fw_options[2]);
	}

	/* Move PUREX, ABTS RX & RIDA to ATIOQ */
	if (ql2xmvasynctoatio &&
	    (IS_QLA83XX(ha) || IS_QLA27XX(ha) || IS_QLA28XX(ha))) {
		if (qla_tgt_mode_enabled(vha) ||
		    qla_dual_mode_enabled(vha))
			ha->fw_options[2] |= BIT_11;
		else
			ha->fw_options[2] &= ~BIT_11;
	}

	if (IS_QLA25XX(ha) || IS_QLA83XX(ha) || IS_QLA27XX(ha) ||
	    IS_QLA28XX(ha)) {
		/*
		 * Tell FW to track each exchange to prevent
		 * driver from using stale exchange.
		 */
		if (qla_tgt_mode_enabled(vha) ||
		    qla_dual_mode_enabled(vha))
			ha->fw_options[2] |= BIT_4;
		else
			ha->fw_options[2] &= ~BIT_4;

		/* Reserve 1/2 of emergency exchanges for ELS.*/
		if (qla2xuseresexchforels)
			ha->fw_options[2] |= BIT_8;
		else
			ha->fw_options[2] &= ~BIT_8;
	}

	if (ql2xrdpenable || ha->flags.scm_supported_f)
		ha->fw_options[1] |= ADD_FO1_ENABLE_PUREX_IOCB;

	/* Enable Async 8130/8131 events -- transceiver insertion/removal */
	if (IS_BPM_RANGE_CAPABLE(ha))
		ha->fw_options[3] |= BIT_10;

	ql_dbg(ql_dbg_init, vha, 0x00e8,
	    "%s, add FW options 1-3 = 0x%04x 0x%04x 0x%04x mode %x\n",
	    __func__, ha->fw_options[1], ha->fw_options[2],
	    ha->fw_options[3], vha->host->active_mode);

	if (ha->fw_options[1] || ha->fw_options[2] || ha->fw_options[3])
		qla2x00_set_fw_options(vha, ha->fw_options);

	/* Update Serial Link options. */
	if ((le16_to_cpu(ha->fw_seriallink_options24[0]) & BIT_0) == 0)
		return;

	rval = qla2x00_set_serdes_params(vha,
	    le16_to_cpu(ha->fw_seriallink_options24[1]),
	    le16_to_cpu(ha->fw_seriallink_options24[2]),
	    le16_to_cpu(ha->fw_seriallink_options24[3]));
	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x0104,
		    "Unable to update Serial Link options (%x).\n", rval);
	}
}

void
qla2x00_config_rings(struct scsi_qla_host *vha)
{
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;
	struct req_que *req = ha->req_q_map[0];
	struct rsp_que *rsp = ha->rsp_q_map[0];

	/* Setup ring parameters in initialization control block. */
	ha->init_cb->request_q_outpointer = cpu_to_le16(0);
	ha->init_cb->response_q_inpointer = cpu_to_le16(0);
	ha->init_cb->request_q_length = cpu_to_le16(req->length);
	ha->init_cb->response_q_length = cpu_to_le16(rsp->length);
	put_unaligned_le64(req->dma, &ha->init_cb->request_q_address);
	put_unaligned_le64(rsp->dma, &ha->init_cb->response_q_address);

	wrt_reg_word(ISP_REQ_Q_IN(ha, reg), 0);
	wrt_reg_word(ISP_REQ_Q_OUT(ha, reg), 0);
	wrt_reg_word(ISP_RSP_Q_IN(ha, reg), 0);
	wrt_reg_word(ISP_RSP_Q_OUT(ha, reg), 0);
	rd_reg_word(ISP_RSP_Q_OUT(ha, reg));		/* PCI Posting. */
}

void
qla24xx_config_rings(struct scsi_qla_host *vha)
{
	struct qla_hw_data *ha = vha->hw;
	device_reg_t *reg = ISP_QUE_REG(ha, 0);
	struct device_reg_2xxx __iomem *ioreg = &ha->iobase->isp;
	struct qla_msix_entry *msix;
	struct init_cb_24xx *icb;
	uint16_t rid = 0;
	struct req_que *req = ha->req_q_map[0];
	struct rsp_que *rsp = ha->rsp_q_map[0];

	/* Setup ring parameters in initialization control block. */
	icb = (struct init_cb_24xx *)ha->init_cb;
	icb->request_q_outpointer = cpu_to_le16(0);
	icb->response_q_inpointer = cpu_to_le16(0);
	icb->request_q_length = cpu_to_le16(req->length);
	icb->response_q_length = cpu_to_le16(rsp->length);
	put_unaligned_le64(req->dma, &icb->request_q_address);
	put_unaligned_le64(rsp->dma, &icb->response_q_address);

	/* Setup ATIO queue dma pointers for target mode */
	icb->atio_q_inpointer = cpu_to_le16(0);
	icb->atio_q_length = cpu_to_le16(ha->tgt.atio_q_length);
	put_unaligned_le64(ha->tgt.atio_dma, &icb->atio_q_address);

	if (IS_SHADOW_REG_CAPABLE(ha))
		icb->firmware_options_2 |= cpu_to_le32(BIT_30|BIT_29);

	if (ha->mqenable || IS_QLA83XX(ha) || IS_QLA27XX(ha) ||
	    IS_QLA28XX(ha)) {
		icb->qos = cpu_to_le16(QLA_DEFAULT_QUE_QOS);
		icb->rid = cpu_to_le16(rid);
		if (ha->flags.msix_enabled) {
			msix = &ha->msix_entries[1];
			ql_dbg(ql_dbg_init, vha, 0x0019,
			    "Registering vector 0x%x for base que.\n",
			    msix->entry);
			icb->msix = cpu_to_le16(msix->entry);
		}
		/* Use alternate PCI bus number */
		if (MSB(rid))
			icb->firmware_options_2 |= cpu_to_le32(BIT_19);
		/* Use alternate PCI devfn */
		if (LSB(rid))
			icb->firmware_options_2 |= cpu_to_le32(BIT_18);

		/* Use Disable MSIX Handshake mode for capable adapters */
		if ((ha->fw_attributes & BIT_6) && (IS_MSIX_NACK_CAPABLE(ha)) &&
		    (ha->flags.msix_enabled)) {
			icb->firmware_options_2 &= cpu_to_le32(~BIT_22);
			ha->flags.disable_msix_handshake = 1;
			ql_dbg(ql_dbg_init, vha, 0x00fe,
			    "MSIX Handshake Disable Mode turned on.\n");
		} else {
			icb->firmware_options_2 |= cpu_to_le32(BIT_22);
		}
		icb->firmware_options_2 |= cpu_to_le32(BIT_23);

		wrt_reg_dword(&reg->isp25mq.req_q_in, 0);
		wrt_reg_dword(&reg->isp25mq.req_q_out, 0);
		wrt_reg_dword(&reg->isp25mq.rsp_q_in, 0);
		wrt_reg_dword(&reg->isp25mq.rsp_q_out, 0);
	} else {
		wrt_reg_dword(&reg->isp24.req_q_in, 0);
		wrt_reg_dword(&reg->isp24.req_q_out, 0);
		wrt_reg_dword(&reg->isp24.rsp_q_in, 0);
		wrt_reg_dword(&reg->isp24.rsp_q_out, 0);
	}

	qlt_24xx_config_rings(vha);

	/* If the user has configured the speed, set it here */
	if (ha->set_data_rate) {
		ql_dbg(ql_dbg_init, vha, 0x00fd,
		    "Speed set by user : %s Gbps \n",
		    qla2x00_get_link_speed_str(ha, ha->set_data_rate));
		icb->firmware_options_3 = cpu_to_le32(ha->set_data_rate << 13);
	}

	/* PCI posting */
	rd_reg_word(&ioreg->hccr);
}

/**
 * qla2x00_init_rings() - Initializes firmware.
 * @vha: HA context
 *
 * Beginning of request ring has initialization control block already built
 * by nvram config routine.
 *
 * Returns 0 on success.
 */
int
qla2x00_init_rings(scsi_qla_host_t *vha)
{
	int	rval;
	unsigned long flags = 0;
	int cnt, que;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req;
	struct rsp_que *rsp;
	struct mid_init_cb_24xx *mid_init_cb =
	    (struct mid_init_cb_24xx *) ha->init_cb;

	spin_lock_irqsave(&ha->hardware_lock, flags);

	/* Clear outstanding commands array. */
	for (que = 0; que < ha->max_req_queues; que++) {
		req = ha->req_q_map[que];
		if (!req || !test_bit(que, ha->req_qid_map))
			continue;
		req->out_ptr = (uint16_t *)(req->ring + req->length);
		*req->out_ptr = 0;
		for (cnt = 1; cnt < req->num_outstanding_cmds; cnt++)
			req->outstanding_cmds[cnt] = NULL;

		req->current_outstanding_cmd = 1;

		/* Initialize firmware. */
		req->ring_ptr  = req->ring;
		req->ring_index    = 0;
		req->cnt      = req->length;
	}

	for (que = 0; que < ha->max_rsp_queues; que++) {
		rsp = ha->rsp_q_map[que];
		if (!rsp || !test_bit(que, ha->rsp_qid_map))
			continue;
		rsp->in_ptr = (uint16_t *)(rsp->ring + rsp->length);
		*rsp->in_ptr = 0;
		/* Initialize response queue entries */
		if (IS_QLAFX00(ha))
			qlafx00_init_response_q_entries(rsp);
		else
			qla2x00_init_response_q_entries(rsp);
	}

	ha->tgt.atio_ring_ptr = ha->tgt.atio_ring;
	ha->tgt.atio_ring_index = 0;
	/* Initialize ATIO queue entries */
	qlt_init_atio_q_entries(vha);

	ha->isp_ops->config_rings(vha);

	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	ql_dbg(ql_dbg_init, vha, 0x00d1, "Issue init firmware.\n");

	if (IS_QLAFX00(ha)) {
		rval = qlafx00_init_firmware(vha, ha->init_cb_size);
		goto next_check;
	}

	/* Update any ISP specific firmware options before initialization. */
	ha->isp_ops->update_fw_options(vha);

	if (ha->flags.npiv_supported) {
		if (ha->operating_mode == LOOP && !IS_CNA_CAPABLE(ha))
			ha->max_npiv_vports = MIN_MULTI_ID_FABRIC - 1;
		mid_init_cb->count = cpu_to_le16(ha->max_npiv_vports);
	}

	if (IS_FWI2_CAPABLE(ha)) {
		mid_init_cb->options = cpu_to_le16(BIT_1);
		mid_init_cb->init_cb.execution_throttle =
		    cpu_to_le16(ha->cur_fw_xcb_count);
		ha->flags.dport_enabled =
			(le32_to_cpu(mid_init_cb->init_cb.firmware_options_1) &
			 BIT_7) != 0;
		ql_dbg(ql_dbg_init, vha, 0x0191, "DPORT Support: %s.\n",
		    (ha->flags.dport_enabled) ? "enabled" : "disabled");
		/* FA-WWPN Status */
		ha->flags.fawwpn_enabled =
			(le32_to_cpu(mid_init_cb->init_cb.firmware_options_1) &
			 BIT_6) != 0;
		ql_dbg(ql_dbg_init, vha, 0x00bc, "FA-WWPN Support: %s.\n",
		    (ha->flags.fawwpn_enabled) ? "enabled" : "disabled");
	}

	rval = qla2x00_init_firmware(vha, ha->init_cb_size);
next_check:
	if (rval) {
		ql_log(ql_log_fatal, vha, 0x00d2,
		    "Init Firmware **** FAILED ****.\n");
	} else {
		ql_dbg(ql_dbg_init, vha, 0x00d3,
		    "Init Firmware -- success.\n");
		QLA_FW_STARTED(ha);
		vha->u_ql2xexchoffld = vha->u_ql2xiniexchg = 0;
	}

	return (rval);
}

/**
 * qla2x00_fw_ready() - Waits for firmware ready.
 * @vha: HA context
 *
 * Returns 0 on success.
 */
static int
qla2x00_fw_ready(scsi_qla_host_t *vha)
{
	int		rval;
	unsigned long	wtime, mtime, cs84xx_time;
	uint16_t	min_wait;	/* Minimum wait time if loop is down */
	uint16_t	wait_time;	/* Wait time if loop is coming ready */
	uint16_t	state[6];
	struct qla_hw_data *ha = vha->hw;

	if (IS_QLAFX00(vha->hw))
		return qlafx00_fw_ready(vha);

	rval = QLA_SUCCESS;

	/* Time to wait for loop down */
	if (IS_P3P_TYPE(ha))
		min_wait = 30;
	else
		min_wait = 20;

	/*
	 * Firmware should take at most one RATOV to login, plus 5 seconds for
	 * our own processing.
	 */
	if ((wait_time = (ha->retry_count*ha->login_timeout) + 5) < min_wait) {
		wait_time = min_wait;
	}

	/* Min wait time if loop down */
	mtime = jiffies + (min_wait * HZ);

	/* wait time before firmware ready */
	wtime = jiffies + (wait_time * HZ);

	/* Wait for ISP to finish LIP */
	if (!vha->flags.init_done)
		ql_log(ql_log_info, vha, 0x801e,
		    "Waiting for LIP to complete.\n");

	do {
		memset(state, -1, sizeof(state));
		rval = qla2x00_get_firmware_state(vha, state);
		if (rval == QLA_SUCCESS) {
			if (state[0] < FSTATE_LOSS_OF_SYNC) {
				vha->device_flags &= ~DFLG_NO_CABLE;
			}
			if (IS_QLA84XX(ha) && state[0] != FSTATE_READY) {
				ql_dbg(ql_dbg_taskm, vha, 0x801f,
				    "fw_state=%x 84xx=%x.\n", state[0],
				    state[2]);
				if ((state[2] & FSTATE_LOGGED_IN) &&
				     (state[2] & FSTATE_WAITING_FOR_VERIFY)) {
					ql_dbg(ql_dbg_taskm, vha, 0x8028,
					    "Sending verify iocb.\n");

					cs84xx_time = jiffies;
					rval = qla84xx_init_chip(vha);
					if (rval != QLA_SUCCESS) {
						ql_log(ql_log_warn,
						    vha, 0x8007,
						    "Init chip failed.\n");
						break;
					}

					/* Add time taken to initialize. */
					cs84xx_time = jiffies - cs84xx_time;
					wtime += cs84xx_time;
					mtime += cs84xx_time;
					ql_dbg(ql_dbg_taskm, vha, 0x8008,
					    "Increasing wait time by %ld. "
					    "New time %ld.\n", cs84xx_time,
					    wtime);
				}
			} else if (state[0] == FSTATE_READY) {
				ql_dbg(ql_dbg_taskm, vha, 0x8037,
				    "F/W Ready - OK.\n");

				qla2x00_get_retry_cnt(vha, &ha->retry_count,
				    &ha->login_timeout, &ha->r_a_tov);

				rval = QLA_SUCCESS;
				break;
			}

			rval = QLA_FUNCTION_FAILED;

			if (atomic_read(&vha->loop_down_timer) &&
			    state[0] != FSTATE_READY) {
				/* Loop down. Timeout on min_wait for states
				 * other than Wait for Login.
				 */
				if (time_after_eq(jiffies, mtime)) {
					ql_log(ql_log_info, vha, 0x8038,
					    "Cable is unplugged...\n");

					vha->device_flags |= DFLG_NO_CABLE;
					break;
				}
			}
		} else {
			/* Mailbox cmd failed. Timeout on min_wait. */
			if (time_after_eq(jiffies, mtime) ||
				ha->flags.isp82xx_fw_hung)
				break;
		}

		if (time_after_eq(jiffies, wtime))
			break;

		/* Delay for a while */
		msleep(500);
	} while (1);

	ql_dbg(ql_dbg_taskm, vha, 0x803a,
	    "fw_state=%x (%x, %x, %x, %x %x) curr time=%lx.\n", state[0],
	    state[1], state[2], state[3], state[4], state[5], jiffies);

	if (rval && !(vha->device_flags & DFLG_NO_CABLE)) {
		ql_log(ql_log_warn, vha, 0x803b,
		    "Firmware ready **** FAILED ****.\n");
	}

	return (rval);
}

/*
*  qla2x00_configure_hba
*      Setup adapter context.
*
* Input:
*      ha = adapter state pointer.
*
* Returns:
*      0 = success
*
* Context:
*      Kernel context.
*/
static int
qla2x00_configure_hba(scsi_qla_host_t *vha)
{
	int       rval;
	uint16_t      loop_id;
	uint16_t      topo;
	uint16_t      sw_cap;
	uint8_t       al_pa;
	uint8_t       area;
	uint8_t       domain;
	char		connect_type[22];
	struct qla_hw_data *ha = vha->hw;
	scsi_qla_host_t *base_vha = pci_get_drvdata(ha->pdev);
	port_id_t id;
	unsigned long flags;

	/* Get host addresses. */
	rval = qla2x00_get_adapter_id(vha,
	    &loop_id, &al_pa, &area, &domain, &topo, &sw_cap);
	if (rval != QLA_SUCCESS) {
		if (LOOP_TRANSITION(vha) || atomic_read(&ha->loop_down_timer) ||
		    IS_CNA_CAPABLE(ha) ||
		    (rval == QLA_COMMAND_ERROR && loop_id == 0x7)) {
			ql_dbg(ql_dbg_disc, vha, 0x2008,
			    "Loop is in a transition state.\n");
		} else {
			ql_log(ql_log_warn, vha, 0x2009,
			    "Unable to get host loop ID.\n");
			if (IS_FWI2_CAPABLE(ha) && (vha == base_vha) &&
			    (rval == QLA_COMMAND_ERROR && loop_id == 0x1b)) {
				ql_log(ql_log_warn, vha, 0x1151,
				    "Doing link init.\n");
				if (qla24xx_link_initialize(vha) == QLA_SUCCESS)
					return rval;
			}
			set_bit(ISP_ABORT_NEEDED, &vha->dpc_flags);
		}
		return (rval);
	}

	if (topo == 4) {
		ql_log(ql_log_info, vha, 0x200a,
		    "Cannot get topology - retrying.\n");
		return (QLA_FUNCTION_FAILED);
	}

	vha->loop_id = loop_id;

	/* initialize */
	ha->min_external_loopid = SNS_FIRST_LOOP_ID;
	ha->operating_mode = LOOP;
	ha->switch_cap = 0;

	switch (topo) {
	case 0:
		ql_dbg(ql_dbg_disc, vha, 0x200b, "HBA in NL topology.\n");
		ha->current_topology = ISP_CFG_NL;
		strcpy(connect_type, "(Loop)");
		break;

	case 1:
		ql_dbg(ql_dbg_disc, vha, 0x200c, "HBA in FL topology.\n");
		ha->switch_cap = sw_cap;
		ha->current_topology = ISP_CFG_FL;
		strcpy(connect_type, "(FL_Port)");
		break;

	case 2:
		ql_dbg(ql_dbg_disc, vha, 0x200d, "HBA in N P2P topology.\n");
		ha->operating_mode = P2P;
		ha->current_topology = ISP_CFG_N;
		strcpy(connect_type, "(N_Port-to-N_Port)");
		break;

	case 3:
		ql_dbg(ql_dbg_disc, vha, 0x200e, "HBA in F P2P topology.\n");
		ha->switch_cap = sw_cap;
		ha->operating_mode = P2P;
		ha->current_topology = ISP_CFG_F;
		strcpy(connect_type, "(F_Port)");
		break;

	default:
		ql_dbg(ql_dbg_disc, vha, 0x200f,
		    "HBA in unknown topology %x, using NL.\n", topo);
		ha->current_topology = ISP_CFG_NL;
		strcpy(connect_type, "(Loop)");
		break;
	}

	/* Save Host port and loop ID. */
	/* byte order - Big Endian */
	id.b.domain = domain;
	id.b.area = area;
	id.b.al_pa = al_pa;
	id.b.rsvd_1 = 0;
	spin_lock_irqsave(&ha->hardware_lock, flags);
	if (!(topo == 2 && ha->flags.n2n_bigger))
		qlt_update_host_map(vha, id);
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	if (!vha->flags.init_done)
		ql_log(ql_log_info, vha, 0x2010,
		    "Topology - %s, Host Loop address 0x%x.\n",
		    connect_type, vha->loop_id);

	return(rval);
}

inline void
qla2x00_set_model_info(scsi_qla_host_t *vha, uint8_t *model, size_t len,
		       const char *def)
{
	char *st, *en;
	uint16_t index;
	uint64_t zero[2] = { 0 };
	struct qla_hw_data *ha = vha->hw;
	int use_tbl = !IS_QLA24XX_TYPE(ha) && !IS_QLA25XX(ha) &&
	    !IS_CNA_CAPABLE(ha) && !IS_QLA2031(ha);

	if (len > sizeof(zero))
		len = sizeof(zero);
	if (memcmp(model, &zero, len) != 0) {
		memcpy(ha->model_number, model, len);
		st = en = ha->model_number;
		en += len - 1;
		while (en > st) {
			if (*en != 0x20 && *en != 0x00)
				break;
			*en-- = '\0';
		}

		index = (ha->pdev->subsystem_device & 0xff);
		if (use_tbl &&
		    ha->pdev->subsystem_vendor == PCI_VENDOR_ID_QLOGIC &&
		    index < QLA_MODEL_NAMES)
			strlcpy(ha->model_desc,
			    qla2x00_model_name[index * 2 + 1],
			    sizeof(ha->model_desc));
	} else {
		index = (ha->pdev->subsystem_device & 0xff);
		if (use_tbl &&
		    ha->pdev->subsystem_vendor == PCI_VENDOR_ID_QLOGIC &&
		    index < QLA_MODEL_NAMES) {
			strlcpy(ha->model_number,
				qla2x00_model_name[index * 2],
				sizeof(ha->model_number));
			strlcpy(ha->model_desc,
			    qla2x00_model_name[index * 2 + 1],
			    sizeof(ha->model_desc));
		} else {
			strlcpy(ha->model_number, def,
				sizeof(ha->model_number));
		}
	}
	if (IS_FWI2_CAPABLE(ha))
		qla2xxx_get_vpd_field(vha, "\x82", ha->model_desc,
		    sizeof(ha->model_desc));
}

/* On sparc systems, obtain port and node WWN from firmware
 * properties.
 */
static void qla2xxx_nvram_wwn_from_ofw(scsi_qla_host_t *vha, nvram_t *nv)
{
#ifdef CONFIG_SPARC
	struct qla_hw_data *ha = vha->hw;
	struct pci_dev *pdev = ha->pdev;
	struct device_node *dp = pci_device_to_OF_node(pdev);
	const u8 *val;
	int len;

	val = of_get_property(dp, "port-wwn", &len);
	if (val && len >= WWN_SIZE)
		memcpy(nv->port_name, val, WWN_SIZE);

	val = of_get_property(dp, "node-wwn", &len);
	if (val && len >= WWN_SIZE)
		memcpy(nv->node_name, val, WWN_SIZE);
#endif
}

/*
* NVRAM configuration for ISP 2xxx
*
* Input:
*      ha                = adapter block pointer.
*
* Output:
*      initialization control block in response_ring
*      host adapters parameters in host adapter block
*
* Returns:
*      0 = success.
*/
int
qla2x00_nvram_config(scsi_qla_host_t *vha)
{
	int             rval;
	uint8_t         chksum = 0;
	uint16_t        cnt;
	uint8_t         *dptr1, *dptr2;
	struct qla_hw_data *ha = vha->hw;
	init_cb_t       *icb = ha->init_cb;
	nvram_t         *nv = ha->nvram;
	uint8_t         *ptr = ha->nvram;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;

	rval = QLA_SUCCESS;

	/* Determine NVRAM starting address. */
	ha->nvram_size = sizeof(*nv);
	ha->nvram_base = 0;
	if (!IS_QLA2100(ha) && !IS_QLA2200(ha) && !IS_QLA2300(ha))
		if ((rd_reg_word(&reg->ctrl_status) >> 14) == 1)
			ha->nvram_base = 0x80;

	/* Get NVRAM data and calculate checksum. */
	ha->isp_ops->read_nvram(vha, ptr, ha->nvram_base, ha->nvram_size);
	for (cnt = 0, chksum = 0; cnt < ha->nvram_size; cnt++)
		chksum += *ptr++;

	ql_dbg(ql_dbg_init + ql_dbg_buffer, vha, 0x010f,
	    "Contents of NVRAM.\n");
	ql_dump_buffer(ql_dbg_init + ql_dbg_buffer, vha, 0x0110,
	    nv, ha->nvram_size);

	/* Bad NVRAM data, set defaults parameters. */
	if (chksum || memcmp("ISP ", nv->id, sizeof(nv->id)) ||
	    nv->nvram_version < 1) {
		/* Reset NVRAM data. */
		ql_log(ql_log_warn, vha, 0x0064,
		    "Inconsistent NVRAM detected: checksum=%#x id=%.4s version=%#x.\n",
		    chksum, nv->id, nv->nvram_version);
		ql_log(ql_log_warn, vha, 0x0065,
		    "Falling back to "
		    "functioning (yet invalid -- WWPN) defaults.\n");

		/*
		 * Set default initialization control block.
		 */
		memset(nv, 0, ha->nvram_size);
		nv->parameter_block_version = ICB_VERSION;

		if (IS_QLA23XX(ha)) {
			nv->firmware_options[0] = BIT_2 | BIT_1;
			nv->firmware_options[1] = BIT_7 | BIT_5;
			nv->add_firmware_options[0] = BIT_5;
			nv->add_firmware_options[1] = BIT_5 | BIT_4;
			nv->frame_payload_size = cpu_to_le16(2048);
			nv->special_options[1] = BIT_7;
		} else if (IS_QLA2200(ha)) {
			nv->firmware_options[0] = BIT_2 | BIT_1;
			nv->firmware_options[1] = BIT_7 | BIT_5;
			nv->add_firmware_options[0] = BIT_5;
			nv->add_firmware_options[1] = BIT_5 | BIT_4;
			nv->frame_payload_size = cpu_to_le16(1024);
		} else if (IS_QLA2100(ha)) {
			nv->firmware_options[0] = BIT_3 | BIT_1;
			nv->firmware_options[1] = BIT_5;
			nv->frame_payload_size = cpu_to_le16(1024);
		}

		nv->max_iocb_allocation = cpu_to_le16(256);
		nv->execution_throttle = cpu_to_le16(16);
		nv->retry_count = 8;
		nv->retry_delay = 1;

		nv->port_name[0] = 33;
		nv->port_name[3] = 224;
		nv->port_name[4] = 139;

		qla2xxx_nvram_wwn_from_ofw(vha, nv);

		nv->login_timeout = 4;

		/*
		 * Set default host adapter parameters
		 */
		nv->host_p[1] = BIT_2;
		nv->reset_delay = 5;
		nv->port_down_retry_count = 8;
		nv->max_luns_per_target = cpu_to_le16(8);
		nv->link_down_timeout = 60;

		rval = 1;
	}

	/* Reset Initialization control block */
	memset(icb, 0, ha->init_cb_size);

	/*
	 * Setup driver NVRAM options.
	 */
	nv->firmware_options[0] |= (BIT_6 | BIT_1);
	nv->firmware_options[0] &= ~(BIT_5 | BIT_4);
	nv->firmware_options[1] |= (BIT_5 | BIT_0);
	nv->firmware_options[1] &= ~BIT_4;

	if (IS_QLA23XX(ha)) {
		nv->firmware_options[0] |= BIT_2;
		nv->firmware_options[0] &= ~BIT_3;
		nv->special_options[0] &= ~BIT_6;
		nv->add_firmware_options[1] |= BIT_5 | BIT_4;

		if (IS_QLA2300(ha)) {
			if (ha->fb_rev == FPM_2310) {
				strcpy(ha->model_number, "QLA2310");
			} else {
				strcpy(ha->model_number, "QLA2300");
			}
		} else {
			qla2x00_set_model_info(vha, nv->model_number,
			    sizeof(nv->model_number), "QLA23xx");
		}
	} else if (IS_QLA2200(ha)) {
		nv->firmware_options[0] |= BIT_2;
		/*
		 * 'Point-to-point preferred, else loop' is not a safe
		 * connection mode setting.
		 */
		if ((nv->add_firmware_options[0] & (BIT_6 | BIT_5 | BIT_4)) ==
		    (BIT_5 | BIT_4)) {
			/* Force 'loop preferred, else point-to-point'. */
			nv->add_firmware_options[0] &= ~(BIT_6 | BIT_5 | BIT_4);
			nv->add_firmware_options[0] |= BIT_5;
		}
		strcpy(ha->model_number, "QLA22xx");
	} else /*if (IS_QLA2100(ha))*/ {
		strcpy(ha->model_number, "QLA2100");
	}

	/*
	 * Copy over NVRAM RISC parameter block to initialization control block.
	 */
	dptr1 = (uint8_t *)icb;
	dptr2 = (uint8_t *)&nv->parameter_block_version;
	cnt = (uint8_t *)&icb->request_q_outpointer - (uint8_t *)&icb->version;
	while (cnt--)
		*dptr1++ = *dptr2++;

	/* Copy 2nd half. */
	dptr1 = (uint8_t *)icb->add_firmware_options;
	cnt = (uint8_t *)icb->reserved_3 - (uint8_t *)icb->add_firmware_options;
	while (cnt--)
		*dptr1++ = *dptr2++;
	ha->frame_payload_size = le16_to_cpu(icb->frame_payload_size);
	/* Use alternate WWN? */
	if (nv->host_p[1] & BIT_7) {
		memcpy(icb->node_name, nv->alternate_node_name, WWN_SIZE);
		memcpy(icb->port_name, nv->alternate_port_name, WWN_SIZE);
	}

	/* Prepare nodename */
	if ((icb->firmware_options[1] & BIT_6) == 0) {
		/*
		 * Firmware will apply the following mask if the nodename was
		 * not provided.
		 */
		memcpy(icb->node_name, icb->port_name, WWN_SIZE);
		icb->node_name[0] &= 0xF0;
	}

	/*
	 * Set host adapter parameters.
	 */

	/*
	 * BIT_7 in the host-parameters section allows for modification to
	 * internal driver logging.
	 */
	if (nv->host_p[0] & BIT_7)
		ql2xextended_error_logging = QL_DBG_DEFAULT1_MASK;
	ha->flags.disable_risc_code_load = ((nv->host_p[0] & BIT_4) ? 1 : 0);
	/* Always load RISC code on non ISP2[12]00 chips. */
	if (!IS_QLA2100(ha) && !IS_QLA2200(ha))
		ha->flags.disable_risc_code_load = 0;
	ha->flags.enable_lip_reset = ((nv->host_p[1] & BIT_1) ? 1 : 0);
	ha->flags.enable_lip_full_login = ((nv->host_p[1] & BIT_2) ? 1 : 0);
	ha->flags.enable_target_reset = ((nv->host_p[1] & BIT_3) ? 1 : 0);
	ha->flags.enable_led_scheme = (nv->special_options[1] & BIT_4) ? 1 : 0;
	ha->flags.disable_serdes = 0;

	ha->operating_mode =
	    (icb->add_firmware_options[0] & (BIT_6 | BIT_5 | BIT_4)) >> 4;

	memcpy(ha->fw_seriallink_options, nv->seriallink_options,
	    sizeof(ha->fw_seriallink_options));

	/* save HBA serial number */
	ha->serial0 = icb->port_name[5];
	ha->serial1 = icb->port_name[6];
	ha->serial2 = icb->port_name[7];
	memcpy(vha->node_name, icb->node_name, WWN_SIZE);
	memcpy(vha->port_name, icb->port_name, WWN_SIZE);

	icb->execution_throttle = cpu_to_le16(0xFFFF);

	ha->retry_count = nv->retry_count;

	/* Set minimum login_timeout to 4 seconds. */
	if (nv->login_timeout != ql2xlogintimeout)
		nv->login_timeout = ql2xlogintimeout;
	if (nv->login_timeout < 4)
		nv->login_timeout = 4;
	ha->login_timeout = nv->login_timeout;

	/* Set minimum RATOV to 100 tenths of a second. */
	ha->r_a_tov = 100;

	ha->loop_reset_delay = nv->reset_delay;

	/* Link Down Timeout = 0:
	 *
	 * 	When Port Down timer expires we will start returning
	 *	I/O's to OS with "DID_NO_CONNECT".
	 *
	 * Link Down Timeout != 0:
	 *
	 *	 The driver waits for the link to come up after link down
	 *	 before returning I/Os to OS with "DID_NO_CONNECT".
	 */
	if (nv->link_down_timeout == 0) {
		ha->loop_down_abort_time =
		    (LOOP_DOWN_TIME - LOOP_DOWN_TIMEOUT);
	} else {
		ha->link_down_timeout =	 nv->link_down_timeout;
		ha->loop_down_abort_time =
		    (LOOP_DOWN_TIME - ha->link_down_timeout);
	}

	/*
	 * Need enough time to try and get the port back.
	 */
	ha->port_down_retry_count = nv->port_down_retry_count;
	if (qlport_down_retry)
		ha->port_down_retry_count = qlport_down_retry;
	/* Set login_retry_count */
	ha->login_retry_count  = nv->retry_count;
	if (ha->port_down_retry_count == nv->port_down_retry_count &&
	    ha->port_down_retry_count > 3)
		ha->login_retry_count = ha->port_down_retry_count;
	else if (ha->port_down_retry_count > (int)ha->login_retry_count)
		ha->login_retry_count = ha->port_down_retry_count;
	if (ql2xloginretrycount)
		ha->login_retry_count = ql2xloginretrycount;

	icb->lun_enables = cpu_to_le16(0);
	icb->command_resource_count = 0;
	icb->immediate_notify_resource_count = 0;
	icb->timeout = cpu_to_le16(0);

	if (IS_QLA2100(ha) || IS_QLA2200(ha)) {
		/* Enable RIO */
		icb->firmware_options[0] &= ~BIT_3;
		icb->add_firmware_options[0] &=
		    ~(BIT_3 | BIT_2 | BIT_1 | BIT_0);
		icb->add_firmware_options[0] |= BIT_2;
		icb->response_accumulation_timer = 3;
		icb->interrupt_delay_timer = 5;

		vha->flags.process_response_queue = 1;
	} else {
		/* Enable ZIO. */
		if (!vha->flags.init_done) {
			ha->zio_mode = icb->add_firmware_options[0] &
			    (BIT_3 | BIT_2 | BIT_1 | BIT_0);
			ha->zio_timer = icb->interrupt_delay_timer ?
			    icb->interrupt_delay_timer : 2;
		}
		icb->add_firmware_options[0] &=
		    ~(BIT_3 | BIT_2 | BIT_1 | BIT_0);
		vha->flags.process_response_queue = 0;
		if (ha->zio_mode != QLA_ZIO_DISABLED) {
			ha->zio_mode = QLA_ZIO_MODE_6;

			ql_log(ql_log_info, vha, 0x0068,
			    "ZIO mode %d enabled; timer delay (%d us).\n",
			    ha->zio_mode, ha->zio_timer * 100);

			icb->add_firmware_options[0] |= (uint8_t)ha->zio_mode;
			icb->interrupt_delay_timer = (uint8_t)ha->zio_timer;
			vha->flags.process_response_queue = 1;
		}
	}

	if (rval) {
		ql_log(ql_log_warn, vha, 0x0069,
		    "NVRAM configuration failed.\n");
	}
	return (rval);
}

static void
qla2x00_rport_del(void *data)
{
	fc_port_t *fcport = data;
	struct fc_rport *rport;
	unsigned long flags;

	spin_lock_irqsave(fcport->vha->host->host_lock, flags);
	rport = fcport->drport ? fcport->drport : fcport->rport;
	fcport->drport = NULL;
	spin_unlock_irqrestore(fcport->vha->host->host_lock, flags);
	if (rport) {
		ql_dbg(ql_dbg_disc, fcport->vha, 0x210b,
		    "%s %8phN. rport %p roles %x\n",
		    __func__, fcport->port_name, rport,
		    rport->roles);

		fc_remote_port_delete(rport);
	}
}

void qla2x00_set_fcport_state(fc_port_t *fcport, int state)
{
	int old_state;

	old_state = atomic_read(&fcport->state);
	atomic_set(&fcport->state, state);

	/* Don't print state transitions during initial allocation of fcport */
	if (old_state && old_state != state) {
		ql_dbg(ql_dbg_disc, fcport->vha, 0x207d,
		       "FCPort %8phC state transitioned from %s to %s - portid=%02x%02x%02x.\n",
		       fcport->port_name, port_state_str[old_state],
		       port_state_str[state], fcport->d_id.b.domain,
		       fcport->d_id.b.area, fcport->d_id.b.al_pa);
	}
}

/**
 * qla2x00_alloc_fcport() - Allocate a generic fcport.
 * @vha: HA context
 * @flags: allocation flags
 *
 * Returns a pointer to the allocated fcport, or NULL, if none available.
 */
fc_port_t *
qla2x00_alloc_fcport(scsi_qla_host_t *vha, gfp_t flags)
{
	fc_port_t *fcport;

	fcport = kzalloc(sizeof(fc_port_t), flags);
	if (!fcport)
		return NULL;

	fcport->ct_desc.ct_sns = dma_alloc_coherent(&vha->hw->pdev->dev,
		sizeof(struct ct_sns_pkt), &fcport->ct_desc.ct_sns_dma,
		flags);
	if (!fcport->ct_desc.ct_sns) {
		ql_log(ql_log_warn, vha, 0xd049,
		    "Failed to allocate ct_sns request.\n");
		kfree(fcport);
		return NULL;
	}

	/* Setup fcport template structure. */
	fcport->vha = vha;
	fcport->port_type = FCT_UNKNOWN;
	fcport->loop_id = FC_NO_LOOP_ID;
	qla2x00_set_fcport_state(fcport, FCS_UNCONFIGURED);
	fcport->supported_classes = FC_COS_UNSPECIFIED;
	fcport->fp_speed = PORT_SPEED_UNKNOWN;

	fcport->disc_state = DSC_DELETED;
	fcport->fw_login_state = DSC_LS_PORT_UNAVAIL;
	fcport->deleted = QLA_SESS_DELETED;
	fcport->login_retry = vha->hw->login_retry_count;
	fcport->chip_reset = vha->hw->base_qpair->chip_reset;
	fcport->logout_on_delete = 1;
	fcport->tgt_link_down_time = QLA2XX_MAX_LINK_DOWN_TIME;
	fcport->tgt_short_link_down_cnt = 0;
	fcport->dev_loss_tmo = 0;

	if (!fcport->ct_desc.ct_sns) {
		ql_log(ql_log_warn, vha, 0xd049,
		    "Failed to allocate ct_sns request.\n");
		kfree(fcport);
		return NULL;
	}

	INIT_WORK(&fcport->del_work, qla24xx_delete_sess_fn);
	INIT_WORK(&fcport->free_work, qlt_free_session_done);
	INIT_WORK(&fcport->reg_work, qla_register_fcport_fn);
	INIT_LIST_HEAD(&fcport->gnl_entry);
	INIT_LIST_HEAD(&fcport->list);

	INIT_LIST_HEAD(&fcport->sess_cmd_list);
	spin_lock_init(&fcport->sess_cmd_lock);

	return fcport;
}

void
qla2x00_free_fcport(fc_port_t *fcport)
{
	if (fcport->ct_desc.ct_sns) {
		dma_free_coherent(&fcport->vha->hw->pdev->dev,
			sizeof(struct ct_sns_pkt), fcport->ct_desc.ct_sns,
			fcport->ct_desc.ct_sns_dma);

		fcport->ct_desc.ct_sns = NULL;
	}
	list_del(&fcport->list);
	qla2x00_clear_loop_id(fcport);
	kfree(fcport);
}

static void qla_get_login_template(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	int rval;
	u32 *bp, sz;
	__be32 *q;

	memset(ha->init_cb, 0, ha->init_cb_size);
	sz = min_t(int, sizeof(struct fc_els_flogi), ha->init_cb_size);
	rval = qla24xx_get_port_login_templ(vha, ha->init_cb_dma,
					    ha->init_cb, sz);
	if (rval != QLA_SUCCESS) {
		ql_dbg(ql_dbg_init, vha, 0x00d1,
		       "PLOGI ELS param read fail.\n");
		return;
	}
	q = (__be32 *)&ha->plogi_els_payld.fl_csp;

	bp = (uint32_t *)ha->init_cb;
	cpu_to_be32_array(q, bp, sz / 4);
	ha->flags.plogi_template_valid = 1;
}

/*
 * qla2x00_configure_loop
 *      Updates Fibre Channel Device Database with what is actually on loop.
 *
 * Input:
 *      ha                = adapter block pointer.
 *
 * Returns:
 *      0 = success.
 *      1 = error.
 *      2 = database was full and device was not configured.
 */
static int
qla2x00_configure_loop(scsi_qla_host_t *vha)
{
	int  rval;
	unsigned long flags, save_flags;
	struct qla_hw_data *ha = vha->hw;

	rval = QLA_SUCCESS;

	/* Get Initiator ID */
	if (test_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags)) {
		rval = qla2x00_configure_hba(vha);
		if (rval != QLA_SUCCESS) {
			ql_dbg(ql_dbg_disc, vha, 0x2013,
			    "Unable to configure HBA.\n");
			return (rval);
		}
	}

	save_flags = flags = vha->dpc_flags;
	ql_dbg(ql_dbg_disc, vha, 0x2014,
	    "Configure loop -- dpc flags = 0x%lx.\n", flags);

	/*
	 * If we have both an RSCN and PORT UPDATE pending then handle them
	 * both at the same time.
	 */
	clear_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags);
	clear_bit(RSCN_UPDATE, &vha->dpc_flags);

	qla2x00_get_data_rate(vha);
	qla_get_login_template(vha);

	/* Determine what we need to do */
	if ((ha->current_topology == ISP_CFG_FL ||
	    ha->current_topology == ISP_CFG_F) &&
	    (test_bit(LOCAL_LOOP_UPDATE, &flags))) {

		set_bit(RSCN_UPDATE, &flags);
		clear_bit(LOCAL_LOOP_UPDATE, &flags);

	} else if (ha->current_topology == ISP_CFG_NL ||
		   ha->current_topology == ISP_CFG_N) {
		clear_bit(RSCN_UPDATE, &flags);
		set_bit(LOCAL_LOOP_UPDATE, &flags);
	} else if (!vha->flags.online ||
	    (test_bit(ABORT_ISP_ACTIVE, &flags))) {
		set_bit(RSCN_UPDATE, &flags);
		set_bit(LOCAL_LOOP_UPDATE, &flags);
	}

	if (test_bit(LOCAL_LOOP_UPDATE, &flags)) {
		if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags)) {
			ql_dbg(ql_dbg_disc, vha, 0x2015,
			    "Loop resync needed, failing.\n");
			rval = QLA_FUNCTION_FAILED;
		} else
			rval = qla2x00_configure_local_loop(vha);
	}

	if (rval == QLA_SUCCESS && test_bit(RSCN_UPDATE, &flags)) {
		if (LOOP_TRANSITION(vha)) {
			ql_dbg(ql_dbg_disc, vha, 0x2099,
			    "Needs RSCN update and loop transition.\n");
			rval = QLA_FUNCTION_FAILED;
		}
		else
			rval = qla2x00_configure_fabric(vha);
	}

	if (rval == QLA_SUCCESS) {
		if (atomic_read(&vha->loop_down_timer) ||
		    test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags)) {
			rval = QLA_FUNCTION_FAILED;
		} else {
			atomic_set(&vha->loop_state, LOOP_READY);
			ql_dbg(ql_dbg_disc, vha, 0x2069,
			    "LOOP READY.\n");
			ha->flags.fw_init_done = 1;

			/*
			 * Process any ATIO queue entries that came in
			 * while we weren't online.
			 */
			if (qla_tgt_mode_enabled(vha) ||
			    qla_dual_mode_enabled(vha)) {
				spin_lock_irqsave(&ha->tgt.atio_lock, flags);
				qlt_24xx_process_atio_queue(vha, 0);
				spin_unlock_irqrestore(&ha->tgt.atio_lock,
				    flags);
			}
		}
	}

	if (rval) {
		ql_dbg(ql_dbg_disc, vha, 0x206a,
		    "%s *** FAILED ***.\n", __func__);
	} else {
		ql_dbg(ql_dbg_disc, vha, 0x206b,
		    "%s: exiting normally.\n", __func__);
	}

	/* Restore state if a resync event occurred during processing */
	if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags)) {
		if (test_bit(LOCAL_LOOP_UPDATE, &save_flags))
			set_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags);
		if (test_bit(RSCN_UPDATE, &save_flags)) {
			set_bit(RSCN_UPDATE, &vha->dpc_flags);
		}
	}

	return (rval);
}

static int qla2x00_configure_n2n_loop(scsi_qla_host_t *vha)
{
	unsigned long flags;
	fc_port_t *fcport;

	if (test_and_clear_bit(N2N_LOGIN_NEEDED, &vha->dpc_flags))
		set_bit(RELOGIN_NEEDED, &vha->dpc_flags);

	list_for_each_entry(fcport, &vha->vp_fcports, list) {
		if (fcport->n2n_flag) {
			qla24xx_fcport_handle_login(vha, fcport);
			return QLA_SUCCESS;
		}
	}

	spin_lock_irqsave(&vha->work_lock, flags);
	vha->scan.scan_retry++;
	spin_unlock_irqrestore(&vha->work_lock, flags);

	if (vha->scan.scan_retry < MAX_SCAN_RETRIES) {
		set_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags);
		set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
	}
	return QLA_FUNCTION_FAILED;
}

/*
 * qla2x00_configure_local_loop
 *	Updates Fibre Channel Device Database with local loop devices.
 *
 * Input:
 *	ha = adapter block pointer.
 *
 * Returns:
 *	0 = success.
 */
static int
qla2x00_configure_local_loop(scsi_qla_host_t *vha)
{
	int		rval, rval2;
	int		found_devs;
	int		found;
	fc_port_t	*fcport, *new_fcport;
	uint16_t	index;
	uint16_t	entries;
	struct gid_list_info *gid;
	uint16_t	loop_id;
	uint8_t		domain, area, al_pa;
	struct qla_hw_data *ha = vha->hw;
	unsigned long flags;

	/* Inititae N2N login. */
	if (N2N_TOPO(ha))
		return qla2x00_configure_n2n_loop(vha);

	found_devs = 0;
	new_fcport = NULL;
	entries = MAX_FIBRE_DEVICES_LOOP;

	/* Get list of logged in devices. */
	memset(ha->gid_list, 0, qla2x00_gid_list_size(ha));
	rval = qla2x00_get_id_list(vha, ha->gid_list, ha->gid_list_dma,
	    &entries);
	if (rval != QLA_SUCCESS)
		goto err;

	ql_dbg(ql_dbg_disc, vha, 0x2011,
	    "Entries in ID list (%d).\n", entries);
	ql_dump_buffer(ql_dbg_disc + ql_dbg_buffer, vha, 0x2075,
	    ha->gid_list, entries * sizeof(*ha->gid_list));

	if (entries == 0) {
		spin_lock_irqsave(&vha->work_lock, flags);
		vha->scan.scan_retry++;
		spin_unlock_irqrestore(&vha->work_lock, flags);

		if (vha->scan.scan_retry < MAX_SCAN_RETRIES) {
			set_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags);
			set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
		}
	} else {
		vha->scan.scan_retry = 0;
	}

	list_for_each_entry(fcport, &vha->vp_fcports, list) {
		fcport->scan_state = QLA_FCPORT_SCAN;
	}

	/* Allocate temporary fcport for any new fcports discovered. */
	new_fcport = qla2x00_alloc_fcport(vha, GFP_KERNEL);
	if (new_fcport == NULL) {
		ql_log(ql_log_warn, vha, 0x2012,
		    "Memory allocation failed for fcport.\n");
		rval = QLA_MEMORY_ALLOC_FAILED;
		goto err;
	}
	new_fcport->flags &= ~FCF_FABRIC_DEVICE;

	/* Add devices to port list. */
	gid = ha->gid_list;
	for (index = 0; index < entries; index++) {
		domain = gid->domain;
		area = gid->area;
		al_pa = gid->al_pa;
		if (IS_QLA2100(ha) || IS_QLA2200(ha))
			loop_id = gid->loop_id_2100;
		else
			loop_id = le16_to_cpu(gid->loop_id);
		gid = (void *)gid + ha->gid_list_info_size;

		/* Bypass reserved domain fields. */
		if ((domain & 0xf0) == 0xf0)
			continue;

		/* Bypass if not same domain and area of adapter. */
		if (area && domain && ((area != vha->d_id.b.area) ||
		    (domain != vha->d_id.b.domain)) &&
		    (ha->current_topology == ISP_CFG_NL))
			continue;


		/* Bypass invalid local loop ID. */
		if (loop_id > LAST_LOCAL_LOOP_ID)
			continue;

		memset(new_fcport->port_name, 0, WWN_SIZE);

		/* Fill in member data. */
		new_fcport->d_id.b.domain = domain;
		new_fcport->d_id.b.area = area;
		new_fcport->d_id.b.al_pa = al_pa;
		new_fcport->loop_id = loop_id;
		new_fcport->scan_state = QLA_FCPORT_FOUND;

		rval2 = qla2x00_get_port_database(vha, new_fcport, 0);
		if (rval2 != QLA_SUCCESS) {
			ql_dbg(ql_dbg_disc, vha, 0x2097,
			    "Failed to retrieve fcport information "
			    "-- get_port_database=%x, loop_id=0x%04x.\n",
			    rval2, new_fcport->loop_id);
			/* Skip retry if N2N */
			if (ha->current_topology != ISP_CFG_N) {
				ql_dbg(ql_dbg_disc, vha, 0x2105,
				    "Scheduling resync.\n");
				set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
				continue;
			}
		}

		spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
		/* Check for matching device in port list. */
		found = 0;
		fcport = NULL;
		list_for_each_entry(fcport, &vha->vp_fcports, list) {
			if (memcmp(new_fcport->port_name, fcport->port_name,
			    WWN_SIZE))
				continue;

			fcport->flags &= ~FCF_FABRIC_DEVICE;
			fcport->loop_id = new_fcport->loop_id;
			fcport->port_type = new_fcport->port_type;
			fcport->d_id.b24 = new_fcport->d_id.b24;
			memcpy(fcport->node_name, new_fcport->node_name,
			    WWN_SIZE);
			fcport->scan_state = QLA_FCPORT_FOUND;
			found++;
			break;
		}

		if (!found) {
			/* New device, add to fcports list. */
			list_add_tail(&new_fcport->list, &vha->vp_fcports);

			/* Allocate a new replacement fcport. */
			fcport = new_fcport;

			spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);

			new_fcport = qla2x00_alloc_fcport(vha, GFP_KERNEL);

			if (new_fcport == NULL) {
				ql_log(ql_log_warn, vha, 0xd031,
				    "Failed to allocate memory for fcport.\n");
				rval = QLA_MEMORY_ALLOC_FAILED;
				goto err;
			}
			spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);
			new_fcport->flags &= ~FCF_FABRIC_DEVICE;
		}

		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);

		/* Base iIDMA settings on HBA port speed. */
		fcport->fp_speed = ha->link_data_rate;

		found_devs++;
	}

	list_for_each_entry(fcport, &vha->vp_fcports, list) {
		if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
			break;

		if (fcport->scan_state == QLA_FCPORT_SCAN) {
			if ((qla_dual_mode_enabled(vha) ||
			    qla_ini_mode_enabled(vha)) &&
			    atomic_read(&fcport->state) == FCS_ONLINE) {
				qla2x00_mark_device_lost(vha, fcport,
					ql2xplogiabsentdevice);
				if (fcport->loop_id != FC_NO_LOOP_ID &&
				    (fcport->flags & FCF_FCP2_DEVICE) == 0 &&
				    fcport->port_type != FCT_INITIATOR &&
				    fcport->port_type != FCT_BROADCAST) {
					ql_dbg(ql_dbg_disc, vha, 0x20f0,
					    "%s %d %8phC post del sess\n",
					    __func__, __LINE__,
					    fcport->port_name);

					qlt_schedule_sess_for_deletion(fcport);
					continue;
				}
			}
		}

		if (fcport->scan_state == QLA_FCPORT_FOUND)
			qla24xx_fcport_handle_login(vha, fcport);
	}

	qla2x00_free_fcport(new_fcport);

	return rval;

err:
	ql_dbg(ql_dbg_disc, vha, 0x2098,
	       "Configure local loop error exit: rval=%x.\n", rval);
	return rval;
}

static void
qla2x00_iidma_fcport(scsi_qla_host_t *vha, fc_port_t *fcport)
{
	int rval;
	uint16_t mb[MAILBOX_REGISTER_COUNT];
	struct qla_hw_data *ha = vha->hw;

	if (!IS_IIDMA_CAPABLE(ha))
		return;

	if (atomic_read(&fcport->state) != FCS_ONLINE)
		return;

	if (fcport->fp_speed == PORT_SPEED_UNKNOWN ||
	    fcport->fp_speed > ha->link_data_rate ||
	    !ha->flags.gpsc_supported)
		return;

	rval = qla2x00_set_idma_speed(vha, fcport->loop_id, fcport->fp_speed,
	    mb);
	if (rval != QLA_SUCCESS) {
		ql_dbg(ql_dbg_disc, vha, 0x2004,
		    "Unable to adjust iIDMA %8phN -- %04x %x %04x %04x.\n",
		    fcport->port_name, rval, fcport->fp_speed, mb[0], mb[1]);
	} else {
		ql_dbg(ql_dbg_disc, vha, 0x2005,
		    "iIDMA adjusted to %s GB/s (%X) on %8phN.\n",
		    qla2x00_get_link_speed_str(ha, fcport->fp_speed),
		    fcport->fp_speed, fcport->port_name);
	}
}

void qla_do_iidma_work(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	qla2x00_iidma_fcport(vha, fcport);
	qla24xx_update_fcport_fcp_prio(vha, fcport);
}

int qla_post_iidma_work(struct scsi_qla_host *vha, fc_port_t *fcport)
{
	struct qla_work_evt *e;

	e = qla2x00_alloc_work(vha, QLA_EVT_IIDMA);
	if (!e)
		return QLA_FUNCTION_FAILED;

	e->u.fcport.fcport = fcport;
	return qla2x00_post_work(vha, e);
}

/* qla2x00_reg_remote_port is reserved for Initiator Mode only.*/
static void
qla2x00_reg_remote_port(scsi_qla_host_t *vha, fc_port_t *fcport)
{
	struct fc_rport_identifiers rport_ids;
	struct fc_rport *rport;
	unsigned long flags;

	if (atomic_read(&fcport->state) == FCS_ONLINE)
		return;

	rport_ids.node_name = wwn_to_u64(fcport->node_name);
	rport_ids.port_name = wwn_to_u64(fcport->port_name);
	rport_ids.port_id = fcport->d_id.b.domain << 16 |
	    fcport->d_id.b.area << 8 | fcport->d_id.b.al_pa;
	rport_ids.roles = FC_RPORT_ROLE_UNKNOWN;
	fcport->rport = rport = fc_remote_port_add(vha->host, 0, &rport_ids);
	if (!rport) {
		ql_log(ql_log_warn, vha, 0x2006,
		    "Unable to allocate fc remote port.\n");
		return;
	}

	spin_lock_irqsave(fcport->vha->host->host_lock, flags);
	*((fc_port_t **)rport->dd_data) = fcport;
	spin_unlock_irqrestore(fcport->vha->host->host_lock, flags);
	fcport->dev_loss_tmo = rport->dev_loss_tmo;

	rport->supported_classes = fcport->supported_classes;

	rport_ids.roles = FC_PORT_ROLE_UNKNOWN;
	if (fcport->port_type == FCT_INITIATOR)
		rport_ids.roles |= FC_PORT_ROLE_FCP_INITIATOR;
	if (fcport->port_type == FCT_TARGET)
		rport_ids.roles |= FC_PORT_ROLE_FCP_TARGET;
	if (fcport->port_type & FCT_NVME_INITIATOR)
		rport_ids.roles |= FC_PORT_ROLE_NVME_INITIATOR;
	if (fcport->port_type & FCT_NVME_TARGET)
		rport_ids.roles |= FC_PORT_ROLE_NVME_TARGET;
	if (fcport->port_type & FCT_NVME_DISCOVERY)
		rport_ids.roles |= FC_PORT_ROLE_NVME_DISCOVERY;

	fc_remote_port_rolechg(rport, rport_ids.roles);

	ql_dbg(ql_dbg_disc, vha, 0x20ee,
	    "%s: %8phN. rport %ld:0:%d (%p) is %s mode\n",
	    __func__, fcport->port_name, vha->host_no,
	    rport->scsi_target_id, rport,
	    (fcport->port_type == FCT_TARGET) ? "tgt" :
	    ((fcport->port_type & FCT_NVME) ? "nvme" : "ini"));
}

/*
 * qla2x00_update_fcport
 *	Updates device on list.
 *
 * Input:
 *	ha = adapter block pointer.
 *	fcport = port structure pointer.
 *
 * Return:
 *	0  - Success
 *  BIT_0 - error
 *
 * Context:
 *	Kernel context.
 */
void
qla2x00_update_fcport(scsi_qla_host_t *vha, fc_port_t *fcport)
{
	if (IS_SW_RESV_ADDR(fcport->d_id))
		return;

	ql_dbg(ql_dbg_disc, vha, 0x20ef, "%s %8phC\n",
	    __func__, fcport->port_name);

	qla2x00_set_fcport_disc_state(fcport, DSC_UPD_FCPORT);
	fcport->login_retry = vha->hw->login_retry_count;
	fcport->flags &= ~(FCF_LOGIN_NEEDED | FCF_ASYNC_SENT);
	fcport->deleted = 0;
	if (vha->hw->current_topology == ISP_CFG_NL)
		fcport->logout_on_delete = 0;
	else
		fcport->logout_on_delete = 1;
	fcport->n2n_chip_reset = fcport->n2n_link_reset_cnt = 0;

	if (fcport->tgt_link_down_time < fcport->dev_loss_tmo) {
		fcport->tgt_short_link_down_cnt++;
		fcport->tgt_link_down_time = QLA2XX_MAX_LINK_DOWN_TIME;
	}

	switch (vha->hw->current_topology) {
	case ISP_CFG_N:
	case ISP_CFG_NL:
		fcport->keep_nport_handle = 1;
		break;
	default:
		break;
	}

	qla2x00_iidma_fcport(vha, fcport);

	qla2x00_dfs_create_rport(vha, fcport);

	if (NVME_TARGET(vha->hw, fcport)) {
		qla_nvme_register_remote(vha, fcport);
		qla2x00_set_fcport_disc_state(fcport, DSC_LOGIN_COMPLETE);
		qla2x00_set_fcport_state(fcport, FCS_ONLINE);
		return;
	}

	qla24xx_update_fcport_fcp_prio(vha, fcport);

	switch (vha->host->active_mode) {
	case MODE_INITIATOR:
		qla2x00_reg_remote_port(vha, fcport);
		break;
	case MODE_TARGET:
		if (!vha->vha_tgt.qla_tgt->tgt_stop &&
			!vha->vha_tgt.qla_tgt->tgt_stopped)
			qlt_fc_port_added(vha, fcport);
		break;
	case MODE_DUAL:
		qla2x00_reg_remote_port(vha, fcport);
		if (!vha->vha_tgt.qla_tgt->tgt_stop &&
			!vha->vha_tgt.qla_tgt->tgt_stopped)
			qlt_fc_port_added(vha, fcport);
		break;
	default:
		break;
	}

	qla2x00_set_fcport_state(fcport, FCS_ONLINE);

	if (IS_IIDMA_CAPABLE(vha->hw) && vha->hw->flags.gpsc_supported) {
		if (fcport->id_changed) {
			fcport->id_changed = 0;
			ql_dbg(ql_dbg_disc, vha, 0x20d7,
			    "%s %d %8phC post gfpnid fcp_cnt %d\n",
			    __func__, __LINE__, fcport->port_name,
			    vha->fcport_count);
			qla24xx_post_gfpnid_work(vha, fcport);
		} else {
			ql_dbg(ql_dbg_disc, vha, 0x20d7,
			    "%s %d %8phC post gpsc fcp_cnt %d\n",
			    __func__, __LINE__, fcport->port_name,
			    vha->fcport_count);
			qla24xx_post_gpsc_work(vha, fcport);
		}
	}

	qla2x00_set_fcport_disc_state(fcport, DSC_LOGIN_COMPLETE);
}

void qla_register_fcport_fn(struct work_struct *work)
{
	fc_port_t *fcport = container_of(work, struct fc_port, reg_work);
	u32 rscn_gen = fcport->rscn_gen;
	u16 data[2];

	if (IS_SW_RESV_ADDR(fcport->d_id))
		return;

	qla2x00_update_fcport(fcport->vha, fcport);

	if (rscn_gen != fcport->rscn_gen) {
		/* RSCN(s) came in while registration */
		switch (fcport->next_disc_state) {
		case DSC_DELETE_PEND:
			qlt_schedule_sess_for_deletion(fcport);
			break;
		case DSC_ADISC:
			data[0] = data[1] = 0;
			qla2x00_post_async_adisc_work(fcport->vha, fcport,
			    data);
			break;
		default:
			break;
		}
	}
}

/*
 * qla2x00_configure_fabric
 *      Setup SNS devices with loop ID's.
 *
 * Input:
 *      ha = adapter block pointer.
 *
 * Returns:
 *      0 = success.
 *      BIT_0 = error
 */
static int
qla2x00_configure_fabric(scsi_qla_host_t *vha)
{
	int	rval;
	fc_port_t	*fcport;
	uint16_t	mb[MAILBOX_REGISTER_COUNT];
	uint16_t	loop_id;
	LIST_HEAD(new_fcports);
	struct qla_hw_data *ha = vha->hw;
	int		discovery_gen;

	/* If FL port exists, then SNS is present */
	if (IS_FWI2_CAPABLE(ha))
		loop_id = NPH_F_PORT;
	else
		loop_id = SNS_FL_PORT;
	rval = qla2x00_get_port_name(vha, loop_id, vha->fabric_node_name, 1);
	if (rval != QLA_SUCCESS) {
		ql_dbg(ql_dbg_disc, vha, 0x20a0,
		    "MBX_GET_PORT_NAME failed, No FL Port.\n");

		vha->device_flags &= ~SWITCH_FOUND;
		return (QLA_SUCCESS);
	}
	vha->device_flags |= SWITCH_FOUND;

	rval = qla2x00_get_port_name(vha, loop_id, vha->fabric_port_name, 0);
	if (rval != QLA_SUCCESS)
		ql_dbg(ql_dbg_disc, vha, 0x20ff,
		    "Failed to get Fabric Port Name\n");

	if (qla_tgt_mode_enabled(vha) || qla_dual_mode_enabled(vha)) {
		rval = qla2x00_send_change_request(vha, 0x3, 0);
		if (rval != QLA_SUCCESS)
			ql_log(ql_log_warn, vha, 0x121,
			    "Failed to enable receiving of RSCN requests: 0x%x.\n",
			    rval);
	}

	do {
		qla2x00_mgmt_svr_login(vha);

		/* Ensure we are logged into the SNS. */
		loop_id = NPH_SNS_LID(ha);
		rval = ha->isp_ops->fabric_login(vha, loop_id, 0xff, 0xff,
		    0xfc, mb, BIT_1|BIT_0);
		if (rval != QLA_SUCCESS || mb[0] != MBS_COMMAND_COMPLETE) {
			ql_dbg(ql_dbg_disc, vha, 0x20a1,
			    "Failed SNS login: loop_id=%x mb[0]=%x mb[1]=%x mb[2]=%x mb[6]=%x mb[7]=%x (%x).\n",
			    loop_id, mb[0], mb[1], mb[2], mb[6], mb[7], rval);
			set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
			return rval;
		}

		/* FDMI support. */
		if (ql2xfdmienable &&
		    test_and_clear_bit(REGISTER_FDMI_NEEDED, &vha->dpc_flags))
			qla2x00_fdmi_register(vha);

		if (test_and_clear_bit(REGISTER_FC4_NEEDED, &vha->dpc_flags)) {
			if (qla2x00_rft_id(vha)) {
				/* EMPTY */
				ql_dbg(ql_dbg_disc, vha, 0x20a2,
				    "Register FC-4 TYPE failed.\n");
				if (test_bit(LOOP_RESYNC_NEEDED,
				    &vha->dpc_flags))
					break;
			}
			if (qla2x00_rff_id(vha, FC4_TYPE_FCP_SCSI)) {
				/* EMPTY */
				ql_dbg(ql_dbg_disc, vha, 0x209a,
				    "Register FC-4 Features failed.\n");
				if (test_bit(LOOP_RESYNC_NEEDED,
				    &vha->dpc_flags))
					break;
			}
			if (vha->flags.nvme_enabled) {
				if (qla2x00_rff_id(vha, FC_TYPE_NVME)) {
					ql_dbg(ql_dbg_disc, vha, 0x2049,
					    "Register NVME FC Type Features failed.\n");
				}
			}
			if (qla2x00_rnn_id(vha)) {
				/* EMPTY */
				ql_dbg(ql_dbg_disc, vha, 0x2104,
				    "Register Node Name failed.\n");
				if (test_bit(LOOP_RESYNC_NEEDED,
				    &vha->dpc_flags))
					break;
			} else if (qla2x00_rsnn_nn(vha)) {
				/* EMPTY */
				ql_dbg(ql_dbg_disc, vha, 0x209b,
				    "Register Symbolic Node Name failed.\n");
				if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
					break;
			}
		}


		/* Mark the time right before querying FW for connected ports.
		 * This process is long, asynchronous and by the time it's done,
		 * collected information might not be accurate anymore. E.g.
		 * disconnected port might have re-connected and a brand new
		 * session has been created. In this case session's generation
		 * will be newer than discovery_gen. */
		qlt_do_generation_tick(vha, &discovery_gen);

		if (USE_ASYNC_SCAN(ha)) {
			rval = qla24xx_async_gpnft(vha, FC4_TYPE_FCP_SCSI,
			    NULL);
			if (rval)
				set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
		} else  {
			list_for_each_entry(fcport, &vha->vp_fcports, list)
				fcport->scan_state = QLA_FCPORT_SCAN;

			rval = qla2x00_find_all_fabric_devs(vha);
		}
		if (rval != QLA_SUCCESS)
			break;
	} while (0);

	if (!vha->nvme_local_port && vha->flags.nvme_enabled)
		qla_nvme_register_hba(vha);

	if (rval)
		ql_dbg(ql_dbg_disc, vha, 0x2068,
		    "Configure fabric error exit rval=%d.\n", rval);

	return (rval);
}

/*
 * qla2x00_find_all_fabric_devs
 *
 * Input:
 *	ha = adapter block pointer.
 *	dev = database device entry pointer.
 *
 * Returns:
 *	0 = success.
 *
 * Context:
 *	Kernel context.
 */
static int
qla2x00_find_all_fabric_devs(scsi_qla_host_t *vha)
{
	int		rval;
	uint16_t	loop_id;
	fc_port_t	*fcport, *new_fcport;
	int		found;

	sw_info_t	*swl;
	int		swl_idx;
	int		first_dev, last_dev;
	port_id_t	wrap = {}, nxt_d_id;
	struct qla_hw_data *ha = vha->hw;
	struct scsi_qla_host *base_vha = pci_get_drvdata(ha->pdev);
	unsigned long flags;

	rval = QLA_SUCCESS;

	/* Try GID_PT to get device list, else GAN. */
	if (!ha->swl)
		ha->swl = kcalloc(ha->max_fibre_devices, sizeof(sw_info_t),
		    GFP_KERNEL);
	swl = ha->swl;
	if (!swl) {
		/*EMPTY*/
		ql_dbg(ql_dbg_disc, vha, 0x209c,
		    "GID_PT allocations failed, fallback on GA_NXT.\n");
	} else {
		memset(swl, 0, ha->max_fibre_devices * sizeof(sw_info_t));
		if (qla2x00_gid_pt(vha, swl) != QLA_SUCCESS) {
			swl = NULL;
			if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
				return rval;
		} else if (qla2x00_gpn_id(vha, swl) != QLA_SUCCESS) {
			swl = NULL;
			if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
				return rval;
		} else if (qla2x00_gnn_id(vha, swl) != QLA_SUCCESS) {
			swl = NULL;
			if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
				return rval;
		} else if (qla2x00_gfpn_id(vha, swl) != QLA_SUCCESS) {
			swl = NULL;
			if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
				return rval;
		}

		/* If other queries succeeded probe for FC-4 type */
		if (swl) {
			qla2x00_gff_id(vha, swl);
			if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
				return rval;
		}
	}
	swl_idx = 0;

	/* Allocate temporary fcport for any new fcports discovered. */
	new_fcport = qla2x00_alloc_fcport(vha, GFP_KERNEL);
	if (new_fcport == NULL) {
		ql_log(ql_log_warn, vha, 0x209d,
		    "Failed to allocate memory for fcport.\n");
		return (QLA_MEMORY_ALLOC_FAILED);
	}
	new_fcport->flags |= (FCF_FABRIC_DEVICE | FCF_LOGIN_NEEDED);
	/* Set start port ID scan at adapter ID. */
	first_dev = 1;
	last_dev = 0;

	/* Starting free loop ID. */
	loop_id = ha->min_external_loopid;
	for (; loop_id <= ha->max_loop_id; loop_id++) {
		if (qla2x00_is_reserved_id(vha, loop_id))
			continue;

		if (ha->current_topology == ISP_CFG_FL &&
		    (atomic_read(&vha->loop_down_timer) ||
		     LOOP_TRANSITION(vha))) {
			atomic_set(&vha->loop_down_timer, 0);
			set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
			set_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags);
			break;
		}

		if (swl != NULL) {
			if (last_dev) {
				wrap.b24 = new_fcport->d_id.b24;
			} else {
				new_fcport->d_id.b24 = swl[swl_idx].d_id.b24;
				memcpy(new_fcport->node_name,
				    swl[swl_idx].node_name, WWN_SIZE);
				memcpy(new_fcport->port_name,
				    swl[swl_idx].port_name, WWN_SIZE);
				memcpy(new_fcport->fabric_port_name,
				    swl[swl_idx].fabric_port_name, WWN_SIZE);
				new_fcport->fp_speed = swl[swl_idx].fp_speed;
				new_fcport->fc4_type = swl[swl_idx].fc4_type;

				new_fcport->nvme_flag = 0;
				if (vha->flags.nvme_enabled &&
				    swl[swl_idx].fc4_type & FS_FC4TYPE_NVME) {
					ql_log(ql_log_info, vha, 0x2131,
					    "FOUND: NVME port %8phC as FC Type 28h\n",
					    new_fcport->port_name);
				}

				if (swl[swl_idx].d_id.b.rsvd_1 != 0) {
					last_dev = 1;
				}
				swl_idx++;
			}
		} else {
			/* Send GA_NXT to the switch */
			rval = qla2x00_ga_nxt(vha, new_fcport);
			if (rval != QLA_SUCCESS) {
				ql_log(ql_log_warn, vha, 0x209e,
				    "SNS scan failed -- assuming "
				    "zero-entry result.\n");
				rval = QLA_SUCCESS;
				break;
			}
		}

		/* If wrap on switch device list, exit. */
		if (first_dev) {
			wrap.b24 = new_fcport->d_id.b24;
			first_dev = 0;
		} else if (new_fcport->d_id.b24 == wrap.b24) {
			ql_dbg(ql_dbg_disc, vha, 0x209f,
			    "Device wrap (%02x%02x%02x).\n",
			    new_fcport->d_id.b.domain,
			    new_fcport->d_id.b.area,
			    new_fcport->d_id.b.al_pa);
			break;
		}

		/* Bypass if same physical adapter. */
		if (new_fcport->d_id.b24 == base_vha->d_id.b24)
			continue;

		/* Bypass virtual ports of the same host. */
		if (qla2x00_is_a_vp_did(vha, new_fcport->d_id.b24))
			continue;

		/* Bypass if same domain and area of adapter. */
		if (((new_fcport->d_id.b24 & 0xffff00) ==
		    (vha->d_id.b24 & 0xffff00)) && ha->current_topology ==
			ISP_CFG_FL)
			    continue;

		/* Bypass reserved domain fields. */
		if ((new_fcport->d_id.b.domain & 0xf0) == 0xf0)
			continue;

		/* Bypass ports whose FCP-4 type is not FCP_SCSI */
		if (ql2xgffidenable &&
		    (!(new_fcport->fc4_type & FS_FC4TYPE_FCP) &&
		    new_fcport->fc4_type != 0))
			continue;

		spin_lock_irqsave(&vha->hw->tgt.sess_lock, flags);

		/* Locate matching device in database. */
		found = 0;
		list_for_each_entry(fcport, &vha->vp_fcports, list) {
			if (memcmp(new_fcport->port_name, fcport->port_name,
			    WWN_SIZE))
				continue;

			fcport->scan_state = QLA_FCPORT_FOUND;

			found++;

			/* Update port state. */
			memcpy(fcport->fabric_port_name,
			    new_fcport->fabric_port_name, WWN_SIZE);
			fcport->fp_speed = new_fcport->fp_speed;

			/*
			 * If address the same and state FCS_ONLINE
			 * (or in target mode), nothing changed.
			 */
			if (fcport->d_id.b24 == new_fcport->d_id.b24 &&
			    (atomic_read(&fcport->state) == FCS_ONLINE ||
			     (vha->host->active_mode == MODE_TARGET))) {
				break;
			}

			if (fcport->login_retry == 0)
				fcport->login_retry =
					vha->hw->login_retry_count;
			/*
			 * If device was not a fabric device before.
			 */
			if ((fcport->flags & FCF_FABRIC_DEVICE) == 0) {
				fcport->d_id.b24 = new_fcport->d_id.b24;
				qla2x00_clear_loop_id(fcport);
				fcport->flags |= (FCF_FABRIC_DEVICE |
				    FCF_LOGIN_NEEDED);
				break;
			}

			/*
			 * Port ID changed or device was marked to be updated;
			 * Log it out if still logged in and mark it for
			 * relogin later.
			 */
			if (qla_tgt_mode_enabled(base_vha)) {
				ql_dbg(ql_dbg_tgt_mgt, vha, 0xf080,
					 "port changed FC ID, %8phC"
					 " old %x:%x:%x (loop_id 0x%04x)-> new %x:%x:%x\n",
					 fcport->port_name,
					 fcport->d_id.b.domain,
					 fcport->d_id.b.area,
					 fcport->d_id.b.al_pa,
					 fcport->loop_id,
					 new_fcport->d_id.b.domain,
					 new_fcport->d_id.b.area,
					 new_fcport->d_id.b.al_pa);
				fcport->d_id.b24 = new_fcport->d_id.b24;
				break;
			}

			fcport->d_id.b24 = new_fcport->d_id.b24;
			fcport->flags |= FCF_LOGIN_NEEDED;
			break;
		}

		if (found && NVME_TARGET(vha->hw, fcport)) {
			if (fcport->disc_state == DSC_DELETE_PEND) {
				qla2x00_set_fcport_disc_state(fcport, DSC_GNL);
				vha->fcport_count--;
				fcport->login_succ = 0;
			}
		}

		if (found) {
			spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);
			continue;
		}
		/* If device was not in our fcports list, then add it. */
		new_fcport->scan_state = QLA_FCPORT_FOUND;
		list_add_tail(&new_fcport->list, &vha->vp_fcports);

		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, flags);


		/* Allocate a new replacement fcport. */
		nxt_d_id.b24 = new_fcport->d_id.b24;
		new_fcport = qla2x00_alloc_fcport(vha, GFP_KERNEL);
		if (new_fcport == NULL) {
			ql_log(ql_log_warn, vha, 0xd032,
			    "Memory allocation failed for fcport.\n");
			return (QLA_MEMORY_ALLOC_FAILED);
		}
		new_fcport->flags |= (FCF_FABRIC_DEVICE | FCF_LOGIN_NEEDED);
		new_fcport->d_id.b24 = nxt_d_id.b24;
	}

	qla2x00_free_fcport(new_fcport);

	/*
	 * Logout all previous fabric dev marked lost, except FCP2 devices.
	 */
	list_for_each_entry(fcport, &vha->vp_fcports, list) {
		if (test_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags))
			break;

		if ((fcport->flags & FCF_FABRIC_DEVICE) == 0)
			continue;

		if (fcport->scan_state == QLA_FCPORT_SCAN) {
			if ((qla_dual_mode_enabled(vha) ||
			    qla_ini_mode_enabled(vha)) &&
			    atomic_read(&fcport->state) == FCS_ONLINE) {
				qla2x00_mark_device_lost(vha, fcport,
					ql2xplogiabsentdevice);
				if (fcport->loop_id != FC_NO_LOOP_ID &&
				    (fcport->flags & FCF_FCP2_DEVICE) == 0 &&
				    fcport->port_type != FCT_INITIATOR &&
				    fcport->port_type != FCT_BROADCAST) {
					ql_dbg(ql_dbg_disc, vha, 0x20f0,
					    "%s %d %8phC post del sess\n",
					    __func__, __LINE__,
					    fcport->port_name);
					qlt_schedule_sess_for_deletion(fcport);
					continue;
				}
			}
		}

		if (fcport->scan_state == QLA_FCPORT_FOUND &&
		    (fcport->flags & FCF_LOGIN_NEEDED) != 0)
			qla24xx_fcport_handle_login(vha, fcport);
	}
	return (rval);
}

/* FW does not set aside Loop id for MGMT Server/FFFFFAh */
int
qla2x00_reserve_mgmt_server_loop_id(scsi_qla_host_t *vha)
{
	int loop_id = FC_NO_LOOP_ID;
	int lid = NPH_MGMT_SERVER - vha->vp_idx;
	unsigned long flags;
	struct qla_hw_data *ha = vha->hw;

	if (vha->vp_idx == 0) {
		set_bit(NPH_MGMT_SERVER, ha->loop_id_map);
		return NPH_MGMT_SERVER;
	}

	/* pick id from high and work down to low */
	spin_lock_irqsave(&ha->vport_slock, flags);
	for (; lid > 0; lid--) {
		if (!test_bit(lid, vha->hw->loop_id_map)) {
			set_bit(lid, vha->hw->loop_id_map);
			loop_id = lid;
			break;
		}
	}
	spin_unlock_irqrestore(&ha->vport_slock, flags);

	return loop_id;
}

/*
 * qla2x00_fabric_login
 *	Issue fabric login command.
 *
 * Input:
 *	ha = adapter block pointer.
 *	device = pointer to FC device type structure.
 *
 * Returns:
 *      0 - Login successfully
 *      1 - Login failed
 *      2 - Initiator device
 *      3 - Fatal error
 */
int
qla2x00_fabric_login(scsi_qla_host_t *vha, fc_port_t *fcport,
    uint16_t *next_loopid)
{
	int	rval;
	int	retry;
	uint16_t tmp_loopid;
	uint16_t mb[MAILBOX_REGISTER_COUNT];
	struct qla_hw_data *ha = vha->hw;

	retry = 0;
	tmp_loopid = 0;

	for (;;) {
		ql_dbg(ql_dbg_disc, vha, 0x2000,
		    "Trying Fabric Login w/loop id 0x%04x for port "
		    "%02x%02x%02x.\n",
		    fcport->loop_id, fcport->d_id.b.domain,
		    fcport->d_id.b.area, fcport->d_id.b.al_pa);

		/* Login fcport on switch. */
		rval = ha->isp_ops->fabric_login(vha, fcport->loop_id,
		    fcport->d_id.b.domain, fcport->d_id.b.area,
		    fcport->d_id.b.al_pa, mb, BIT_0);
		if (rval != QLA_SUCCESS) {
			return rval;
		}
		if (mb[0] == MBS_PORT_ID_USED) {
			/*
			 * Device has another loop ID.  The firmware team
			 * recommends the driver perform an implicit login with
			 * the specified ID again. The ID we just used is save
			 * here so we return with an ID that can be tried by
			 * the next login.
			 */
			retry++;
			tmp_loopid = fcport->loop_id;
			fcport->loop_id = mb[1];

			ql_dbg(ql_dbg_disc, vha, 0x2001,
			    "Fabric Login: port in use - next loop "
			    "id=0x%04x, port id= %02x%02x%02x.\n",
			    fcport->loop_id, fcport->d_id.b.domain,
			    fcport->d_id.b.area, fcport->d_id.b.al_pa);

		} else if (mb[0] == MBS_COMMAND_COMPLETE) {
			/*
			 * Login succeeded.
			 */
			if (retry) {
				/* A retry occurred before. */
				*next_loopid = tmp_loopid;
			} else {
				/*
				 * No retry occurred before. Just increment the
				 * ID value for next login.
				 */
				*next_loopid = (fcport->loop_id + 1);
			}

			if (mb[1] & BIT_0) {
				fcport->port_type = FCT_INITIATOR;
			} else {
				fcport->port_type = FCT_TARGET;
				if (mb[1] & BIT_1) {
					fcport->flags |= FCF_FCP2_DEVICE;
				}
			}

			if (mb[10] & BIT_0)
				fcport->supported_classes |= FC_COS_CLASS2;
			if (mb[10] & BIT_1)
				fcport->supported_classes |= FC_COS_CLASS3;

			if (IS_FWI2_CAPABLE(ha)) {
				if (mb[10] & BIT_7)
					fcport->flags |=
					    FCF_CONF_COMP_SUPPORTED;
			}

			rval = QLA_SUCCESS;
			break;
		} else if (mb[0] == MBS_LOOP_ID_USED) {
			/*
			 * Loop ID already used, try next loop ID.
			 */
			fcport->loop_id++;
			rval = qla2x00_find_new_loop_id(vha, fcport);
			if (rval != QLA_SUCCESS) {
				/* Ran out of loop IDs to use */
				break;
			}
		} else if (mb[0] == MBS_COMMAND_ERROR) {
			/*
			 * Firmware possibly timed out during login. If NO
			 * retries are left to do then the device is declared
			 * dead.
			 */
			*next_loopid = fcport->loop_id;
			ha->isp_ops->fabric_logout(vha, fcport->loop_id,
			    fcport->d_id.b.domain, fcport->d_id.b.area,
			    fcport->d_id.b.al_pa);
			qla2x00_mark_device_lost(vha, fcport, 1);

			rval = 1;
			break;
		} else {
			/*
			 * unrecoverable / not handled error
			 */
			ql_dbg(ql_dbg_disc, vha, 0x2002,
			    "Failed=%x port_id=%02x%02x%02x loop_id=%x "
			    "jiffies=%lx.\n", mb[0], fcport->d_id.b.domain,
			    fcport->d_id.b.area, fcport->d_id.b.al_pa,
			    fcport->loop_id, jiffies);

			*next_loopid = fcport->loop_id;
			ha->isp_ops->fabric_logout(vha, fcport->loop_id,
			    fcport->d_id.b.domain, fcport->d_id.b.area,
			    fcport->d_id.b.al_pa);
			qla2x00_clear_loop_id(fcport);
			fcport->login_retry = 0;

			rval = 3;
			break;
		}
	}

	return (rval);
}

/*
 * qla2x00_local_device_login
 *	Issue local device login command.
 *
 * Input:
 *	ha = adapter block pointer.
 *	loop_id = loop id of device to login to.
 *
 * Returns (Where's the #define!!!!):
 *      0 - Login successfully
 *      1 - Login failed
 *      3 - Fatal error
 */
int
qla2x00_local_device_login(scsi_qla_host_t *vha, fc_port_t *fcport)
{
	int		rval;
	uint16_t	mb[MAILBOX_REGISTER_COUNT];

	memset(mb, 0, sizeof(mb));
	rval = qla2x00_login_local_device(vha, fcport, mb, BIT_0);
	if (rval == QLA_SUCCESS) {
		/* Interrogate mailbox registers for any errors */
		if (mb[0] == MBS_COMMAND_ERROR)
			rval = 1;
		else if (mb[0] == MBS_COMMAND_PARAMETER_ERROR)
			/* device not in PCB table */
			rval = 3;
	}

	return (rval);
}

/*
 *  qla2x00_loop_resync
 *      Resync with fibre channel devices.
 *
 * Input:
 *      ha = adapter block pointer.
 *
 * Returns:
 *      0 = success
 */
int
qla2x00_loop_resync(scsi_qla_host_t *vha)
{
	int rval = QLA_SUCCESS;
	uint32_t wait_time;

	clear_bit(ISP_ABORT_RETRY, &vha->dpc_flags);
	if (vha->flags.online) {
		if (!(rval = qla2x00_fw_ready(vha))) {
			/* Wait at most MAX_TARGET RSCNs for a stable link. */
			wait_time = 256;
			do {
				if (!IS_QLAFX00(vha->hw)) {
					/*
					 * Issue a marker after FW becomes
					 * ready.
					 */
					qla2x00_marker(vha, vha->hw->base_qpair,
					    0, 0, MK_SYNC_ALL);
					vha->marker_needed = 0;
				}

				/* Remap devices on Loop. */
				clear_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);

				if (IS_QLAFX00(vha->hw))
					qlafx00_configure_devices(vha);
				else
					qla2x00_configure_loop(vha);

				wait_time--;
			} while (!atomic_read(&vha->loop_down_timer) &&
				!(test_bit(ISP_ABORT_NEEDED, &vha->dpc_flags))
				&& wait_time && (test_bit(LOOP_RESYNC_NEEDED,
				&vha->dpc_flags)));
		}
	}

	if (test_bit(ISP_ABORT_NEEDED, &vha->dpc_flags))
		return (QLA_FUNCTION_FAILED);

	if (rval)
		ql_dbg(ql_dbg_disc, vha, 0x206c,
		    "%s *** FAILED ***.\n", __func__);

	return (rval);
}

/*
* qla2x00_perform_loop_resync
* Description: This function will set the appropriate flags and call
*              qla2x00_loop_resync. If successful loop will be resynced
* Arguments : scsi_qla_host_t pointer
* returm    : Success or Failure
*/

int qla2x00_perform_loop_resync(scsi_qla_host_t *ha)
{
	int32_t rval = 0;

	if (!test_and_set_bit(LOOP_RESYNC_ACTIVE, &ha->dpc_flags)) {
		/*Configure the flags so that resync happens properly*/
		atomic_set(&ha->loop_down_timer, 0);
		if (!(ha->device_flags & DFLG_NO_CABLE)) {
			atomic_set(&ha->loop_state, LOOP_UP);
			set_bit(LOCAL_LOOP_UPDATE, &ha->dpc_flags);
			set_bit(REGISTER_FC4_NEEDED, &ha->dpc_flags);
			set_bit(LOOP_RESYNC_NEEDED, &ha->dpc_flags);

			rval = qla2x00_loop_resync(ha);
		} else
			atomic_set(&ha->loop_state, LOOP_DEAD);

		clear_bit(LOOP_RESYNC_ACTIVE, &ha->dpc_flags);
	}

	return rval;
}

void
qla2x00_update_fcports(scsi_qla_host_t *base_vha)
{
	fc_port_t *fcport;
	struct scsi_qla_host *vha;
	struct qla_hw_data *ha = base_vha->hw;
	unsigned long flags;

	spin_lock_irqsave(&ha->vport_slock, flags);
	/* Go with deferred removal of rport references. */
	list_for_each_entry(vha, &base_vha->hw->vp_list, list) {
		atomic_inc(&vha->vref_count);
		list_for_each_entry(fcport, &vha->vp_fcports, list) {
			if (fcport->drport &&
			    atomic_read(&fcport->state) != FCS_UNCONFIGURED) {
				spin_unlock_irqrestore(&ha->vport_slock, flags);
				qla2x00_rport_del(fcport);

				spin_lock_irqsave(&ha->vport_slock, flags);
			}
		}
		atomic_dec(&vha->vref_count);
		wake_up(&vha->vref_waitq);
	}
	spin_unlock_irqrestore(&ha->vport_slock, flags);
}

/* Assumes idc_lock always held on entry */
void
qla83xx_reset_ownership(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	uint32_t drv_presence, drv_presence_mask;
	uint32_t dev_part_info1, dev_part_info2, class_type;
	uint32_t class_type_mask = 0x3;
	uint16_t fcoe_other_function = 0xffff, i;

	if (IS_QLA8044(ha)) {
		drv_presence = qla8044_rd_direct(vha,
		    QLA8044_CRB_DRV_ACTIVE_INDEX);
		dev_part_info1 = qla8044_rd_direct(vha,
		    QLA8044_CRB_DEV_PART_INFO_INDEX);
		dev_part_info2 = qla8044_rd_direct(vha,
		    QLA8044_CRB_DEV_PART_INFO2);
	} else {
		qla83xx_rd_reg(vha, QLA83XX_IDC_DRV_PRESENCE, &drv_presence);
		qla83xx_rd_reg(vha, QLA83XX_DEV_PARTINFO1, &dev_part_info1);
		qla83xx_rd_reg(vha, QLA83XX_DEV_PARTINFO2, &dev_part_info2);
	}
	for (i = 0; i < 8; i++) {
		class_type = ((dev_part_info1 >> (i * 4)) & class_type_mask);
		if ((class_type == QLA83XX_CLASS_TYPE_FCOE) &&
		    (i != ha->portnum)) {
			fcoe_other_function = i;
			break;
		}
	}
	if (fcoe_other_function == 0xffff) {
		for (i = 0; i < 8; i++) {
			class_type = ((dev_part_info2 >> (i * 4)) &
			    class_type_mask);
			if ((class_type == QLA83XX_CLASS_TYPE_FCOE) &&
			    ((i + 8) != ha->portnum)) {
				fcoe_other_function = i + 8;
				break;
			}
		}
	}
	/*
	 * Prepare drv-presence mask based on fcoe functions present.
	 * However consider only valid physical fcoe function numbers (0-15).
	 */
	drv_presence_mask = ~((1 << (ha->portnum)) |
			((fcoe_other_function == 0xffff) ?
			 0 : (1 << (fcoe_other_function))));

	/* We are the reset owner iff:
	 *    - No other protocol drivers present.
	 *    - This is the lowest among fcoe functions. */
	if (!(drv_presence & drv_presence_mask) &&
			(ha->portnum < fcoe_other_function)) {
		ql_dbg(ql_dbg_p3p, vha, 0xb07f,
		    "This host is Reset owner.\n");
		ha->flags.nic_core_reset_owner = 1;
	}
}

static int
__qla83xx_set_drv_ack(scsi_qla_host_t *vha)
{
	int rval = QLA_SUCCESS;
	struct qla_hw_data *ha = vha->hw;
	uint32_t drv_ack;

	rval = qla83xx_rd_reg(vha, QLA83XX_IDC_DRIVER_ACK, &drv_ack);
	if (rval == QLA_SUCCESS) {
		drv_ack |= (1 << ha->portnum);
		rval = qla83xx_wr_reg(vha, QLA83XX_IDC_DRIVER_ACK, drv_ack);
	}

	return rval;
}

static int
__qla83xx_clear_drv_ack(scsi_qla_host_t *vha)
{
	int rval = QLA_SUCCESS;
	struct qla_hw_data *ha = vha->hw;
	uint32_t drv_ack;

	rval = qla83xx_rd_reg(vha, QLA83XX_IDC_DRIVER_ACK, &drv_ack);
	if (rval == QLA_SUCCESS) {
		drv_ack &= ~(1 << ha->portnum);
		rval = qla83xx_wr_reg(vha, QLA83XX_IDC_DRIVER_ACK, drv_ack);
	}

	return rval;
}

static const char *
qla83xx_dev_state_to_string(uint32_t dev_state)
{
	switch (dev_state) {
	case QLA8XXX_DEV_COLD:
		return "COLD/RE-INIT";
	case QLA8XXX_DEV_INITIALIZING:
		return "INITIALIZING";
	case QLA8XXX_DEV_READY:
		return "READY";
	case QLA8XXX_DEV_NEED_RESET:
		return "NEED RESET";
	case QLA8XXX_DEV_NEED_QUIESCENT:
		return "NEED QUIESCENT";
	case QLA8XXX_DEV_FAILED:
		return "FAILED";
	case QLA8XXX_DEV_QUIESCENT:
		return "QUIESCENT";
	default:
		return "Unknown";
	}
}

/* Assumes idc-lock always held on entry */
void
qla83xx_idc_audit(scsi_qla_host_t *vha, int audit_type)
{
	struct qla_hw_data *ha = vha->hw;
	uint32_t idc_audit_reg = 0, duration_secs = 0;

	switch (audit_type) {
	case IDC_AUDIT_TIMESTAMP:
		ha->idc_audit_ts = (jiffies_to_msecs(jiffies) / 1000);
		idc_audit_reg = (ha->portnum) |
		    (IDC_AUDIT_TIMESTAMP << 7) | (ha->idc_audit_ts << 8);
		qla83xx_wr_reg(vha, QLA83XX_IDC_AUDIT, idc_audit_reg);
		break;

	case IDC_AUDIT_COMPLETION:
		duration_secs = ((jiffies_to_msecs(jiffies) -
		    jiffies_to_msecs(ha->idc_audit_ts)) / 1000);
		idc_audit_reg = (ha->portnum) |
		    (IDC_AUDIT_COMPLETION << 7) | (duration_secs << 8);
		qla83xx_wr_reg(vha, QLA83XX_IDC_AUDIT, idc_audit_reg);
		break;

	default:
		ql_log(ql_log_warn, vha, 0xb078,
		    "Invalid audit type specified.\n");
		break;
	}
}

/* Assumes idc_lock always held on entry */
static int
qla83xx_initiating_reset(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	uint32_t  idc_control, dev_state;

	__qla83xx_get_idc_control(vha, &idc_control);
	if ((idc_control & QLA83XX_IDC_RESET_DISABLED)) {
		ql_log(ql_log_info, vha, 0xb080,
		    "NIC Core reset has been disabled. idc-control=0x%x\n",
		    idc_control);
		return QLA_FUNCTION_FAILED;
	}

	/* Set NEED-RESET iff in READY state and we are the reset-owner */
	qla83xx_rd_reg(vha, QLA83XX_IDC_DEV_STATE, &dev_state);
	if (ha->flags.nic_core_reset_owner && dev_state == QLA8XXX_DEV_READY) {
		qla83xx_wr_reg(vha, QLA83XX_IDC_DEV_STATE,
		    QLA8XXX_DEV_NEED_RESET);
		ql_log(ql_log_info, vha, 0xb056, "HW State: NEED RESET.\n");
		qla83xx_idc_audit(vha, IDC_AUDIT_TIMESTAMP);
	} else {
		const char *state = qla83xx_dev_state_to_string(dev_state);

		ql_log(ql_log_info, vha, 0xb057, "HW State: %s.\n", state);

		/* SV: XXX: Is timeout required here? */
		/* Wait for IDC state change READY -> NEED_RESET */
		while (dev_state == QLA8XXX_DEV_READY) {
			qla83xx_idc_unlock(vha, 0);
			msleep(200);
			qla83xx_idc_lock(vha, 0);
			qla83xx_rd_reg(vha, QLA83XX_IDC_DEV_STATE, &dev_state);
		}
	}

	/* Send IDC ack by writing to drv-ack register */
	__qla83xx_set_drv_ack(vha);

	return QLA_SUCCESS;
}

int
__qla83xx_set_idc_control(scsi_qla_host_t *vha, uint32_t idc_control)
{
	return qla83xx_wr_reg(vha, QLA83XX_IDC_CONTROL, idc_control);
}

int
__qla83xx_get_idc_control(scsi_qla_host_t *vha, uint32_t *idc_control)
{
	return qla83xx_rd_reg(vha, QLA83XX_IDC_CONTROL, idc_control);
}

static int
qla83xx_check_driver_presence(scsi_qla_host_t *vha)
{
	uint32_t drv_presence = 0;
	struct qla_hw_data *ha = vha->hw;

	qla83xx_rd_reg(vha, QLA83XX_IDC_DRV_PRESENCE, &drv_presence);
	if (drv_presence & (1 << ha->portnum))
		return QLA_SUCCESS;
	else
		return QLA_TEST_FAILED;
}

int
qla83xx_nic_core_reset(scsi_qla_host_t *vha)
{
	int rval = QLA_SUCCESS;
	struct qla_hw_data *ha = vha->hw;

	ql_dbg(ql_dbg_p3p, vha, 0xb058,
	    "Entered  %s().\n", __func__);

	if (vha->device_flags & DFLG_DEV_FAILED) {
		ql_log(ql_log_warn, vha, 0xb059,
		    "Device in unrecoverable FAILED state.\n");
		return QLA_FUNCTION_FAILED;
	}

	qla83xx_idc_lock(vha, 0);

	if (qla83xx_check_driver_presence(vha) != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0xb05a,
		    "Function=0x%x has been removed from IDC participation.\n",
		    ha->portnum);
		rval = QLA_FUNCTION_FAILED;
		goto exit;
	}

	qla83xx_reset_ownership(vha);

	rval = qla83xx_initiating_reset(vha);

	/*
	 * Perform reset if we are the reset-owner,
	 * else wait till IDC state changes to READY/FAILED.
	 */
	if (rval == QLA_SUCCESS) {
		rval = qla83xx_idc_state_handler(vha);

		if (rval == QLA_SUCCESS)
			ha->flags.nic_core_hung = 0;
		__qla83xx_clear_drv_ack(vha);
	}

exit:
	qla83xx_idc_unlock(vha, 0);

	ql_dbg(ql_dbg_p3p, vha, 0xb05b, "Exiting %s.\n", __func__);

	return rval;
}

int
qla2xxx_mctp_dump(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	int rval = QLA_FUNCTION_FAILED;

	if (!IS_MCTP_CAPABLE(ha)) {
		/* This message can be removed from the final version */
		ql_log(ql_log_info, vha, 0x506d,
		    "This board is not MCTP capable\n");
		return rval;
	}

	if (!ha->mctp_dump) {
		ha->mctp_dump = dma_alloc_coherent(&ha->pdev->dev,
		    MCTP_DUMP_SIZE, &ha->mctp_dump_dma, GFP_KERNEL);

		if (!ha->mctp_dump) {
			ql_log(ql_log_warn, vha, 0x506e,
			    "Failed to allocate memory for mctp dump\n");
			return rval;
		}
	}

#define MCTP_DUMP_STR_ADDR	0x00000000
	rval = qla2x00_dump_mctp_data(vha, ha->mctp_dump_dma,
	    MCTP_DUMP_STR_ADDR, MCTP_DUMP_SIZE/4);
	if (rval != QLA_SUCCESS) {
		ql_log(ql_log_warn, vha, 0x506f,
		    "Failed to capture mctp dump\n");
	} else {
		ql_log(ql_log_info, vha, 0x5070,
		    "Mctp dump capture for host (%ld/%p).\n",
		    vha->host_no, ha->mctp_dump);
		ha->mctp_dumped = 1;
	}

	if (!ha->flags.nic_core_reset_hdlr_active && !ha->portnum) {
		ha->flags.nic_core_reset_hdlr_active = 1;
		rval = qla83xx_restart_nic_firmware(vha);
		if (rval)
			/* NIC Core reset failed. */
			ql_log(ql_log_warn, vha, 0x5071,
			    "Failed to restart nic firmware\n");
		else
			ql_dbg(ql_dbg_p3p, vha, 0xb084,
			    "Restarted NIC firmware successfully.\n");
		ha->flags.nic_core_reset_hdlr_active = 0;
	}

	return rval;

}

/*
* qla2x00_quiesce_io
* Description: This function will block the new I/Os
*              Its not aborting any I/Os as context
*              is not destroyed during quiescence
* Arguments: scsi_qla_host_t
* return   : void
*/
void
qla2x00_quiesce_io(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	struct scsi_qla_host *vp;

	ql_dbg(ql_dbg_dpc, vha, 0x401d,
	    "Quiescing I/O - ha=%p.\n", ha);

	atomic_set(&ha->loop_down_timer, LOOP_DOWN_TIME);
	if (atomic_read(&vha->loop_state) != LOOP_DOWN) {
		atomic_set(&vha->loop_state, LOOP_DOWN);
		qla2x00_mark_all_devices_lost(vha);
		list_for_each_entry(vp, &ha->vp_list, list)
			qla2x00_mark_all_devices_lost(vp);
	} else {
		if (!atomic_read(&vha->loop_down_timer))
			atomic_set(&vha->loop_down_timer,
					LOOP_DOWN_TIME);
	}
	/* Wait for pending cmds to complete */
	WARN_ON_ONCE(qla2x00_eh_wait_for_pending_commands(vha, 0, 0, WAIT_HOST)
		     != QLA_SUCCESS);
}

void
qla2x00_abort_isp_cleanup(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	struct scsi_qla_host *vp;
	unsigned long flags;
	fc_port_t *fcport;
	u16 i;

	/* For ISP82XX, driver waits for completion of the commands.
	 * online flag should be set.
	 */
	if (!(IS_P3P_TYPE(ha)))
		vha->flags.online = 0;
	ha->flags.chip_reset_done = 0;
	clear_bit(ISP_ABORT_NEEDED, &vha->dpc_flags);
	vha->qla_stats.total_isp_aborts++;

	ql_log(ql_log_info, vha, 0x00af,
	    "Performing ISP error recovery - ha=%p.\n", ha);

	ha->flags.purge_mbox = 1;
	/* For ISP82XX, reset_chip is just disabling interrupts.
	 * Driver waits for the completion of the commands.
	 * the interrupts need to be enabled.
	 */
	if (!(IS_P3P_TYPE(ha)))
		ha->isp_ops->reset_chip(vha);

	ha->link_data_rate = PORT_SPEED_UNKNOWN;
	SAVE_TOPO(ha);
	ha->flags.rida_fmt2 = 0;
	ha->flags.n2n_ae = 0;
	ha->flags.lip_ae = 0;
	ha->current_topology = 0;
	QLA_FW_STOPPED(ha);
	ha->flags.fw_init_done = 0;
	ha->chip_reset++;
	ha->base_qpair->chip_reset = ha->chip_reset;
	for (i = 0; i < ha->max_qpairs; i++) {
		if (ha->queue_pair_map[i])
			ha->queue_pair_map[i]->chip_reset =
				ha->base_qpair->chip_reset;
	}

	/* purge MBox commands */
	if (atomic_read(&ha->num_pend_mbx_stage3)) {
		clear_bit(MBX_INTR_WAIT, &ha->mbx_cmd_flags);
		complete(&ha->mbx_intr_comp);
	}

	i = 0;
	while (atomic_read(&ha->num_pend_mbx_stage3) ||
	    atomic_read(&ha->num_pend_mbx_stage2) ||
	    atomic_read(&ha->num_pend_mbx_stage1)) {
		msleep(20);
		i++;
		if (i > 50)
			break;
	}
	ha->flags.purge_mbox = 0;

	atomic_set(&vha->loop_down_timer, LOOP_DOWN_TIME);
	if (atomic_read(&vha->loop_state) != LOOP_DOWN) {
		atomic_set(&vha->loop_state, LOOP_DOWN);
		qla2x00_mark_all_devices_lost(vha);

		spin_lock_irqsave(&ha->vport_slock, flags);
		list_for_each_entry(vp, &ha->vp_list, list) {
			atomic_inc(&vp->vref_count);
			spin_unlock_irqrestore(&ha->vport_slock, flags);

			qla2x00_mark_all_devices_lost(vp);

			spin_lock_irqsave(&ha->vport_slock, flags);
			atomic_dec(&vp->vref_count);
		}
		spin_unlock_irqrestore(&ha->vport_slock, flags);
	} else {
		if (!atomic_read(&vha->loop_down_timer))
			atomic_set(&vha->loop_down_timer,
			    LOOP_DOWN_TIME);
	}

	/* Clear all async request states across all VPs. */
	list_for_each_entry(fcport, &vha->vp_fcports, list) {
		fcport->flags &= ~(FCF_LOGIN_NEEDED | FCF_ASYNC_SENT);
		fcport->scan_state = 0;
	}
	spin_lock_irqsave(&ha->vport_slock, flags);
	list_for_each_entry(vp, &ha->vp_list, list) {
		atomic_inc(&vp->vref_count);
		spin_unlock_irqrestore(&ha->vport_slock, flags);

		list_for_each_entry(fcport, &vp->vp_fcports, list)
			fcport->flags &= ~(FCF_LOGIN_NEEDED | FCF_ASYNC_SENT);

		spin_lock_irqsave(&ha->vport_slock, flags);
		atomic_dec(&vp->vref_count);
	}
	spin_unlock_irqrestore(&ha->vport_slock, flags);

	/* Make sure for ISP 82XX IO DMA is complete */
	if (IS_P3P_TYPE(ha)) {
		qla82xx_chip_reset_cleanup(vha);
		ql_log(ql_log_info, vha, 0x00b4,
		       "Done chip reset cleanup.\n");

		/* Done waiting for pending commands. Reset online flag */
		vha->flags.online = 0;
	}

	/* Requeue all commands in outstanding command list. */
	qla2x00_abort_all_cmds(vha, DID_RESET << 16);
	/* memory barrier */
	wmb();
}

/*
*  qla2x00_abort_isp
*      Resets ISP and aborts all outstanding commands.
*
* Input:
*      ha           = adapter block pointer.
*
* Returns:
*      0 = success
*/
int
qla2x00_abort_isp(scsi_qla_host_t *vha)
{
	int rval;
	uint8_t        status = 0;
	struct qla_hw_data *ha = vha->hw;
	struct scsi_qla_host *vp;
	struct req_que *req = ha->req_q_map[0];
	unsigned long flags;

	if (vha->flags.online) {
		qla2x00_abort_isp_cleanup(vha);

		if (vha->hw->flags.port_isolated)
			return status;

		if (qla2x00_isp_reg_stat(ha)) {
			ql_log(ql_log_info, vha, 0x803f,
			       "ISP Abort - ISP reg disconnect, exiting.\n");
			return status;
		}

		if (test_and_clear_bit(ISP_ABORT_TO_ROM, &vha->dpc_flags)) {
			ha->flags.chip_reset_done = 1;
			vha->flags.online = 1;
			status = 0;
			clear_bit(ISP_ABORT_RETRY, &vha->dpc_flags);
			return status;
		}

		if (IS_QLA8031(ha)) {
			ql_dbg(ql_dbg_p3p, vha, 0xb05c,
			    "Clearing fcoe driver presence.\n");
			if (qla83xx_clear_drv_presence(vha) != QLA_SUCCESS)
				ql_dbg(ql_dbg_p3p, vha, 0xb073,
				    "Error while clearing DRV-Presence.\n");
		}

		if (unlikely(pci_channel_offline(ha->pdev) &&
		    ha->flags.pci_channel_io_perm_failure)) {
			clear_bit(ISP_ABORT_RETRY, &vha->dpc_flags);
			status = 0;
			return status;
		}

		switch (vha->qlini_mode) {
		case QLA2XXX_INI_MODE_DISABLED:
			if (!qla_tgt_mode_enabled(vha))
				return 0;
			break;
		case QLA2XXX_INI_MODE_DUAL:
			if (!qla_dual_mode_enabled(vha))
				return 0;
			break;
		case QLA2XXX_INI_MODE_ENABLED:
		default:
			break;
		}

		ha->isp_ops->get_flash_version(vha, req->ring);

		if (qla2x00_isp_reg_stat(ha)) {
			ql_log(ql_log_info, vha, 0x803f,
			       "ISP Abort - ISP reg disconnect pre nvram config, exiting.\n");
			return status;
		}
		ha->isp_ops->nvram_config(vha);

		if (qla2x00_isp_reg_stat(ha)) {
			ql_log(ql_log_info, vha, 0x803f,
			       "ISP Abort - ISP reg disconnect post nvmram config, exiting.\n");
			return status;
		}
		if (!qla2x00_restart_isp(vha)) {
			clear_bit(RESET_MARKER_NEEDED, &vha->dpc_flags);

			if (!atomic_read(&vha->loop_down_timer)) {
				/*
				 * Issue marker command only when we are going
				 * to start the I/O .
				 */
				vha->marker_needed = 1;
			}

			vha->flags.online = 1;

			ha->isp_ops->enable_intrs(ha);

			ha->isp_abort_cnt = 0;
			clear_bit(ISP_ABORT_RETRY, &vha->dpc_flags);

			if (IS_QLA81XX(ha) || IS_QLA8031(ha))
				qla2x00_get_fw_version(vha);
			if (ha->fce) {
				ha->flags.fce_enabled = 1;
				memset(ha->fce, 0,
				    fce_calc_size(ha->fce_bufs));
				rval = qla2x00_enable_fce_trace(vha,
				    ha->fce_dma, ha->fce_bufs, ha->fce_mb,
				    &ha->fce_bufs);
				if (rval) {
					ql_log(ql_log_warn, vha, 0x8033,
					    "Unable to reinitialize FCE "
					    "(%d).\n", rval);
					ha->flags.fce_enabled = 0;
				}
			}

			if (ha->eft) {
				memset(ha->eft, 0, EFT_SIZE);
				rval = qla2x00_enable_eft_trace(vha,
				    ha->eft_dma, EFT_NUM_BUFFERS);
				if (rval) {
					ql_log(ql_log_warn, vha, 0x8034,
					    "Unable to reinitialize EFT "
					    "(%d).\n", rval);
				}
			}
		} else {	/* failed the ISP abort */
			vha->flags.online = 1;
			if (test_bit(ISP_ABORT_RETRY, &vha->dpc_flags)) {
				if (ha->isp_abort_cnt == 0) {
					ql_log(ql_log_fatal, vha, 0x8035,
					    "ISP error recover failed - "
					    "board disabled.\n");
					/*
					 * The next call disables the board
					 * completely.
					 */
					qla2x00_abort_isp_cleanup(vha);
					vha->flags.online = 0;
					clear_bit(ISP_ABORT_RETRY,
					    &vha->dpc_flags);
					status = 0;
				} else { /* schedule another ISP abort */
					ha->isp_abort_cnt--;
					ql_dbg(ql_dbg_taskm, vha, 0x8020,
					    "ISP abort - retry remaining %d.\n",
					    ha->isp_abort_cnt);
					status = 1;
				}
			} else {
				ha->isp_abort_cnt = MAX_RETRIES_OF_ISP_ABORT;
				ql_dbg(ql_dbg_taskm, vha, 0x8021,
				    "ISP error recovery - retrying (%d) "
				    "more times.\n", ha->isp_abort_cnt);
				set_bit(ISP_ABORT_RETRY, &vha->dpc_flags);
				status = 1;
			}
		}

	}

	if (vha->hw->flags.port_isolated) {
		qla2x00_abort_isp_cleanup(vha);
		return status;
	}

	if (!status) {
		ql_dbg(ql_dbg_taskm, vha, 0x8022, "%s succeeded.\n", __func__);
		qla2x00_configure_hba(vha);
		spin_lock_irqsave(&ha->vport_slock, flags);
		list_for_each_entry(vp, &ha->vp_list, list) {
			if (vp->vp_idx) {
				atomic_inc(&vp->vref_count);
				spin_unlock_irqrestore(&ha->vport_slock, flags);

				qla2x00_vp_abort_isp(vp);

				spin_lock_irqsave(&ha->vport_slock, flags);
				atomic_dec(&vp->vref_count);
			}
		}
		spin_unlock_irqrestore(&ha->vport_slock, flags);

		if (IS_QLA8031(ha)) {
			ql_dbg(ql_dbg_p3p, vha, 0xb05d,
			    "Setting back fcoe driver presence.\n");
			if (qla83xx_set_drv_presence(vha) != QLA_SUCCESS)
				ql_dbg(ql_dbg_p3p, vha, 0xb074,
				    "Error while setting DRV-Presence.\n");
		}
	} else {
		ql_log(ql_log_warn, vha, 0x8023, "%s **** FAILED ****.\n",
		       __func__);
	}

	return(status);
}

/*
*  qla2x00_restart_isp
*      restarts the ISP after a reset
*
* Input:
*      ha = adapter block pointer.
*
* Returns:
*      0 = success
*/
static int
qla2x00_restart_isp(scsi_qla_host_t *vha)
{
	int status;
	struct qla_hw_data *ha = vha->hw;

	/* If firmware needs to be loaded */
	if (qla2x00_isp_firmware(vha)) {
		vha->flags.online = 0;
		status = ha->isp_ops->chip_diag(vha);
		if (status)
			return status;
		status = qla2x00_setup_chip(vha);
		if (status)
			return status;
	}

	status = qla2x00_init_rings(vha);
	if (status)
		return status;

	clear_bit(RESET_MARKER_NEEDED, &vha->dpc_flags);
	ha->flags.chip_reset_done = 1;

	/* Initialize the queues in use */
	qla25xx_init_queues(ha);

	status = qla2x00_fw_ready(vha);
	if (status) {
		/* if no cable then assume it's good */
		return vha->device_flags & DFLG_NO_CABLE ? 0 : status;
	}

	/* Issue a marker after FW becomes ready. */
	qla2x00_marker(vha, ha->base_qpair, 0, 0, MK_SYNC_ALL);
	set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);

	return 0;
}

static int
qla25xx_init_queues(struct qla_hw_data *ha)
{
	struct rsp_que *rsp = NULL;
	struct req_que *req = NULL;
	struct scsi_qla_host *base_vha = pci_get_drvdata(ha->pdev);
	int ret = -1;
	int i;

	for (i = 1; i < ha->max_rsp_queues; i++) {
		rsp = ha->rsp_q_map[i];
		if (rsp && test_bit(i, ha->rsp_qid_map)) {
			rsp->options &= ~BIT_0;
			ret = qla25xx_init_rsp_que(base_vha, rsp);
			if (ret != QLA_SUCCESS)
				ql_dbg(ql_dbg_init, base_vha, 0x00ff,
				    "%s Rsp que: %d init failed.\n",
				    __func__, rsp->id);
			else
				ql_dbg(ql_dbg_init, base_vha, 0x0100,
				    "%s Rsp que: %d inited.\n",
				    __func__, rsp->id);
		}
	}
	for (i = 1; i < ha->max_req_queues; i++) {
		req = ha->req_q_map[i];
		if (req && test_bit(i, ha->req_qid_map)) {
			/* Clear outstanding commands array. */
			req->options &= ~BIT_0;
			ret = qla25xx_init_req_que(base_vha, req);
			if (ret != QLA_SUCCESS)
				ql_dbg(ql_dbg_init, base_vha, 0x0101,
				    "%s Req que: %d init failed.\n",
				    __func__, req->id);
			else
				ql_dbg(ql_dbg_init, base_vha, 0x0102,
				    "%s Req que: %d inited.\n",
				    __func__, req->id);
		}
	}
	return ret;
}

/*
* qla2x00_reset_adapter
*      Reset adapter.
*
* Input:
*      ha = adapter block pointer.
*/
int
qla2x00_reset_adapter(scsi_qla_host_t *vha)
{
	unsigned long flags = 0;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_2xxx __iomem *reg = &ha->iobase->isp;

	vha->flags.online = 0;
	ha->isp_ops->disable_intrs(ha);

	spin_lock_irqsave(&ha->hardware_lock, flags);
	wrt_reg_word(&reg->hccr, HCCR_RESET_RISC);
	rd_reg_word(&reg->hccr);			/* PCI Posting. */
	wrt_reg_word(&reg->hccr, HCCR_RELEASE_RISC);
	rd_reg_word(&reg->hccr);			/* PCI Posting. */
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	return QLA_SUCCESS;
}

int
qla24xx_reset_adapter(scsi_qla_host_t *vha)
{
	unsigned long flags = 0;
	struct qla_hw_data *ha = vha->hw;
	struct device_reg_24xx __iomem *reg = &ha->iobase->isp24;

	if (IS_P3P_TYPE(ha))
		return QLA_SUCCESS;

	vha->flags.online = 0;
	ha->isp_ops->disable_intrs(ha);

	spin_lock_irqsave(&ha->hardware_lock, flags);
	wrt_reg_dword(&reg->hccr, HCCRX_SET_RISC_RESET);
	rd_reg_dword(&reg->hccr);
	wrt_reg_dword(&reg->hccr, HCCRX_REL_RISC_PAUSE);
	rd_reg_dword(&reg->hccr);
	spin_unlock_irqrestore(&ha->hardware_lock, flags);

	if (IS_NOPOLLING_TYPE(ha))
		ha->isp_ops->enable_intrs(ha);

	return QLA_SUCCESS;
}

/* On sparc systems, obtain port and node WWN from firmware
 * properties.
 */
static void qla24xx_nvram_wwn_from_ofw(scsi_qla_host_t *vha,
	struct nvram_24xx *nv)
{
#ifdef CONFIG_SPARC
	struct qla_hw_data *ha = vha->hw;
	struct pci_dev *pdev = ha->pdev;
	struct device_node *dp = pci_device_to_OF_node(pdev);
	const u8 *val;
	int len;

	val = of_get_property(dp, "port-wwn", &len);
	if (val && len >= WWN_SIZE)
		memcpy(nv->port_name, val, WWN_SIZE);

	val = of_get_property(dp, "node-wwn", &len);
	if (val && len >= WWN_SIZE)
		memcpy(nv->node_name, val, WWN_SIZE);
#endif
}

int
qla24xx_nvram_config(scsi_qla_host_t *vha)
{
	int   rval;
	struct init_cb_24xx *icb;
	struct nvram_24xx *nv;
	__le32 *dptr;
	uint8_t  *dptr1, *dptr2;
	uint32_t chksum;
	uint16_t cnt;
	struct qla_hw_data *ha = vha->hw;

	rval = QLA_SUCCESS;
	icb = (struct init_cb_24xx *)ha->init_cb;
	nv = ha->nvram;

	/* Determine NVRAM starting address. */
	if (ha->port_no == 0) {
		ha->nvram_base = FA_NVRAM_FUNC0_ADDR;
		ha->vpd_base = FA_NVRAM_VPD0_ADDR;
	} else {
		ha->nvram_base = FA_NVRAM_FUNC1_ADDR;
		ha->vpd_base = FA_NVRAM_VPD1_ADDR;
	}

	ha->nvram_size = sizeof(*nv);
	ha->vpd_size = FA_NVRAM_VPD_SIZE;

	/* Get VPD data into cache */
	ha->vpd = ha->nvram + VPD_OFFSET;
	ha->isp_ops->read_nvram(vha, ha->vpd,
	    ha->nvram_base - FA_NVRAM_FUNC0_ADDR, FA_NVRAM_VPD_SIZE * 4);

	/* Get NVRAM data into cache and calculate checksum. */
	dptr = (__force __le32 *)nv;
	ha->isp_ops->read_nvram(vha, dptr, ha->nvram_base, ha->nvram_size);
	for (cnt = 0, chksum = 0; cnt < ha->nvram_size >> 2; cnt++, dptr++)
		chksum += le32_to_cpu(*dptr);

	ql_dbg(ql_dbg_init + ql_dbg_buffer, vha, 0x006a,
	    "Contents of NVRAM\n");
	ql_dump_buffer(ql_dbg_init + ql_dbg_buffer, vha, 0x010d,
	    nv, ha->nvram_size);

	/* Bad NVRAM data, set defaults parameters. */
	if (chksum || memcmp("ISP ", nv->id, sizeof(nv->id)) ||
	    le16_to_cpu(nv->nvram_version) < ICB_VERSION) {
		/* Reset NVRAM data. */
		ql_log(ql_log_warn, vha, 0x006b,
		    "Inconsistent NVRAM checksum=%#x id=%.4s version=%#x.\n",
		    chksum, nv->id, nv->nvram_version);
		ql_dump_buffer(ql_dbg_init, vha, 0x006b, nv, sizeof(*nv));
		ql_log(ql_log_warn, vha, 0x006c,
		    "Falling back to functioning (yet invalid -- WWPN) "
		    "defaults.\n");

		/*
		 * Set default initialization control block.
		 */
		memset(nv, 0, ha->nvram_size);
		nv->nvram_version = cpu_to_le16(ICB_VERSION);
		nv->version = cpu_to_le16(ICB_VERSION);
		nv->frame_payload_size = cpu_to_le16(2048);
		nv->execution_throttle = cpu_to_le16(0xFFFF);
		nv->exchange_count = cpu_to_le16(0);
		nv->hard_address = cpu_to_le16(124);
		nv->port_name[0] = 0x21;
		nv->port_name[1] = 0x00 + ha->port_no + 1;
		nv->port_name[2] = 0x00;
		nv->port_name[3] = 0xe0;
		nv->port_name[4] = 0x8b;
		nv->port_name[5] = 0x1c;
		nv->port_name[6] = 0x55;
		nv->port_name[7] = 0x86;
		nv->node_name[0] = 0x20;
		nv->node_name[1] = 0x00;
		nv->node_name[2] = 0x00;
		nv->node_name[3] = 0xe0;
		nv->node_name[4] = 0x8b;
		nv->node_name[5] = 0x1c;
		nv->node_name[6] = 0x55;
		nv->node_name[7] = 0x86;
		qla24xx_nvram_wwn_from_ofw(vha, nv);
		nv->login_retry_count = cpu_to_le16(8);
		nv->interrupt_delay_timer = cpu_to_le16(0);
		nv->login_timeout = cpu_to_le16(0);
		nv->firmware_options_1 =
		    cpu_to_le32(BIT_14|BIT_13|BIT_2|BIT_1);
		nv->firmware_options_2 = cpu_to_le32(2 << 4);
		nv->firmware_options_2 |= cpu_to_le32(BIT_12);
		nv->firmware_options_3 = cpu_to_le32(2 << 13);
		nv->host_p = cpu_to_le32(BIT_11|BIT_10);
		nv->efi_parameters = cpu_to_le32(0);
		nv->reset_delay = 5;
		nv->max_luns_per_target = cpu_to_le16(128);
		nv->port_down_retry_count = cpu_to_le16(30);
		nv->link_down_timeout = cpu_to_le16(30);

		rval = 1;
	}

	if (qla_tgt_mode_enabled(vha)) {
		/* Don't enable full login after initial LIP */
		nv->firmware_options_1 &= cpu_to_le32(~BIT_13);
		/* Don't enable LIP full login for initiator */
		nv->host_p &= cpu_to_le32(~BIT_10);
	}

	qlt_24xx_config_nvram_stage1(vha, nv);

	/* Reset Initialization control block */
	memset(icb, 0, ha->init_cb_size);

	/* Copy 1st segment. */
	dptr1 = (uint8_t *)icb;
	dptr2 = (uint8_t *)&nv->version;
	cnt = (uint8_t *)&icb->response_q_inpointer - (uint8_t *)&icb->version;
	while (cnt--)
		*dptr1++ = *dptr2++;

	icb->login_retry_count = nv->login_retry_count;
	icb->link_down_on_nos = nv->link_down_on_nos;

	/* Copy 2nd segment. */
	dptr1 = (uint8_t *)&icb->interrupt_delay_timer;
	dptr2 = (uint8_t *)&nv->interrupt_delay_timer;
	cnt = (uint8_t *)&icb->reserved_3 -
	    (uint8_t *)&icb->interrupt_delay_timer;
	while (cnt--)
		*dptr1++ = *dptr2++;
	ha->frame_payload_size = le16_to_cpu(icb->frame_payload_size);
	/*
	 * Setup driver NVRAM options.
	 */
	qla2x00_set_model_info(vha, nv->model_name, sizeof(nv->model_name),
	    "QLA2462");

	qlt_24xx_config_nvram_stage2(vha, icb);

	if (nv->host_p & cpu_to_le32(BIT_15)) {
		/* Use alternate WWN? */
		memcpy(icb->node_name, nv->alternate_node_name, WWN_SIZE);
		memcpy(icb->port_name, nv->alternate_port_name, WWN_SIZE);
	}

	/* Prepare nodename */
	if ((icb->firmware_options_1 & cpu_to_le32(BIT_14)) == 0) {
		/*
		 * Firmware will apply the following mask if the nodename was
		 * not provided.
		 */
		memcpy(icb->node_name, icb->port_name, WWN_SIZE);
		icb->node_name[0] &= 0xF0;
	}

	/* Set host adapter parameters. */
	ha->flags.disable_risc_code_load = 0;
	ha->flags.enable_lip_reset = 0;
	ha->flags.enable_lip_full_login =
	    le32_to_cpu(nv->host_p) & BIT_10 ? 1 : 0;
	ha->flags.enable_target_reset =
	    le32_to_cpu(nv->host_p) & BIT_11 ? 1 : 0;
	ha->flags.enable_led_scheme = 0;
	ha->flags.disable_serdes = le32_to_cpu(nv->host_p) & BIT_5 ? 1 : 0;

	ha->operating_mode = (le32_to_cpu(icb->firmware_options_2) &
	    (BIT_6 | BIT_5 | BIT_4)) >> 4;

	memcpy(ha->fw_seriallink_options24, nv->seriallink_options,
	    sizeof(ha->fw_seriallink_options24));

	/* save HBA serial number */
	ha->serial0 = icb->port_name[5];
	ha->serial1 = icb->port_name[6];
	ha->serial2 = icb->port_name[7];
	memcpy(vha->node_name, icb->node_name, WWN_SIZE);
	memcpy(vha->port_name, icb->port_name, WWN_SIZE);

	icb->execution_throttle = cpu_to_le16(0xFFFF);

	ha->retry_count = le16_to_cpu(nv->login_retry_count);

	/* Set minimum login_timeout to 4 seconds. */
	if (le16_to_cpu(nv->login_timeout) < ql2xlogintimeout)
		nv->login_timeout = cpu_to_le16(ql2xlogintimeout);
	if (le16_to_cpu(nv->login_timeout) < 4)
		nv->login_timeout = cpu_to_le16(4);
	ha->login_timeout = le16_to_cpu(nv->login_timeout);

	/* Set minimum RATOV to 100 tenths of a second. */
	ha->r_a_tov = 100;

	ha->loop_reset_delay = nv->reset_delay;

	/* Link Down Timeout = 0:
	 *
	 * 	When Port Down timer expires we will start returning
	 *	I/O's to OS with "DID_NO_CONNECT".
	 *
	 * Link Down Timeout != 0:
	 *
	 *	 The driver waits for the link to come up after link down
	 *	 before returning I/Os to OS with "DID_NO_CONNECT".
	 */
	if (le16_to_cpu(nv->link_down_timeout) == 0) {
		ha->loop_down_abort_time =
		    (LOOP_DOWN_TIME - LOOP_DOWN_TIMEOUT);
	} else {
		ha->link_down_timeout =	le16_to_cpu(nv->link_down_timeout);
		ha->loop_down_abort_time =
		    (LOOP_DOWN_TIME - ha->link_down_timeout);
	}

	/* Need enough time to try and get the port back. */
	ha->port_down_retry_count = le16_to_cpu(nv->port_down_retry_count);
	if (qlport_down_retry)
		ha->port_down_retry_count = qlport_down_retry;

	/* Set login_retry_count */
	ha->login_retry_count  = le16_to_cpu(nv->login_retry_count);
	if (ha->port_down_retry_count ==
	    le16_to_cpu(nv->port_down_retry_count) &&
	    ha->port_down_retry_count > 3)
		ha->login_retry_count = ha->port_down_retry_count;
	else if (ha->port_down_retry_count > (int)ha->login_retry_count)
		ha->login_retry_count = ha->port_down_retry_count;
	if (ql2xloginretrycount)
		ha->login_retry_count = ql2xloginretrycount;

	/* N2N: driver will initiate Login instead of FW */
	icb->firmware_options_3 |= cpu_to_le32(BIT_8);

	/* Enable ZIO. */
	if (!vha->flags.init_done) {
		ha->zio_mode = le32_to_cpu(icb->firmware_options_2) &
		    (BIT_3 | BIT_2 | BIT_1 | BIT_0);
		ha->zio_timer = le16_to_cpu(icb->interrupt_delay_timer) ?
		    le16_to_cpu(icb->interrupt_delay_timer) : 2;
	}
	icb->firmware_options_2 &= cpu_to_le32(
	    ~(BIT_3 | BIT_2 | BIT_1 | BIT_0));
	if (ha->zio_mode != QLA_ZIO_DISABLED) {
		ha->zio_mode = QLA_ZIO_MODE_6;

		ql_log(ql_log_info, vha, 0x006f,
		    "ZIO mode %d enabled; timer delay (%d us).\n",
		    ha->zio_mode, ha->zio_timer * 100);

		icb->firmware_options_2 |= cpu_to_le32(
		    (uint32_t)ha->zio_mode);
		icb->interrupt_delay_timer = cpu_to_le16(ha->zio_timer);
	}

	if (rval) {
		ql_log(ql_log_warn, vha, 0x0070,
		    "NVRAM configuration failed.\n");
	}
	return (rval);
}

static void
qla27xx_print_image(struct scsi_qla_host *vha, char *name,
    struct qla27xx_image_status *image_status)
{
	ql_dbg(ql_dbg_init, vha, 0x018b,
	    "%s %s: mask=%#02x gen=%#04x ver=%u.%u map=%#01x sum=%#08x sig=%#08x\n",
	    name, "status",
	    image_status->image_status_mask,
	    le16_to_cpu(image_status->generation),
	    image_status->ver_major,
	    image_status->ver_minor,
	    image_status->bitmap,
	    le32_to_cpu(image_status->checksum),
	    le32_to_cpu(image_status->signature));
}

static bool
qla28xx_check_aux_image_status_signature(
    struct qla27xx_image_status *image_status)
{
	ulong signature = le32_to_cpu(image_status->signature);

	return signature != QLA28XX_AUX_IMG_STATUS_SIGN;
}

static bool
qla27xx_check_image_status_signature(struct qla27xx_image_status *image_status)
{
	ulong signature = le32_to_cpu(image_status->signature);

	return
	    signature != QLA27XX_IMG_STATUS_SIGN &&
	    signature != QLA28XX_IMG_STATUS_SIGN;
}

static ulong
qla27xx_image_status_checksum(struct qla27xx_image_status *image_status)
{
	__le32 *p = (__force __le32 *)image_status;
	uint n = sizeof(*image_status) / sizeof(*p);
	uint32_t sum = 0;

	for ( ; n--; p++)
		sum += le32_to_cpup(p);

	return sum;
}

static inline uint
qla28xx_component_bitmask(struct qla27xx_image_status *aux, uint bitmask)
{
	return aux->bitmap & bitmask ?
	    QLA27XX_SECONDARY_IMAGE : QLA27XX_PRIMARY_IMAGE;
}

static void
qla28xx_component_status(
    struct active_regions *active_regions, struct qla27xx_image_status *aux)
{
	active_regions->aux.board_config =
	    qla28xx_component_bitmask(aux, QLA28XX_AUX_IMG_BOARD_CONFIG);

	active_regions->aux.vpd_nvram =
	    qla28xx_component_bitmask(aux, QLA28XX_AUX_IMG_VPD_NVRAM);

	active_regions->aux.npiv_config_0_1 =
	    qla28xx_component_bitmask(aux, QLA28XX_AUX_IMG_NPIV_CONFIG_0_1);

	active_regions->aux.npiv_config_2_3 =
	    qla28xx_component_bitmask(aux, QLA28XX_AUX_IMG_NPIV_CONFIG_2_3);
}

static int
qla27xx_compare_image_generation(
    struct qla27xx_image_status *pri_image_status,
    struct qla27xx_image_status *sec_image_status)
{
	/* calculate generation delta as uint16 (this accounts for wrap) */
	int16_t delta =
	    le16_to_cpu(pri_image_status->generation) -
	    le16_to_cpu(sec_image_status->generation);

	ql_dbg(ql_dbg_init, NULL, 0x0180, "generation delta = %d\n", delta);

	return delta;
}

void
qla28xx_get_aux_images(
	struct scsi_qla_host *vha, struct active_regions *active_regions)
{
	struct qla_hw_data *ha = vha->hw;
	struct qla27xx_image_status pri_aux_image_status, sec_aux_image_status;
	bool valid_pri_image = false, valid_sec_image = false;
	bool active_pri_image = false, active_sec_image = false;

	if (!ha->flt_region_aux_img_status_pri) {
		ql_dbg(ql_dbg_init, vha, 0x018a, "Primary aux image not addressed\n");
		goto check_sec_image;
	}

	qla24xx_read_flash_data(vha, (uint32_t *)&pri_aux_image_status,
	    ha->flt_region_aux_img_status_pri,
	    sizeof(pri_aux_image_status) >> 2);
	qla27xx_print_image(vha, "Primary aux image", &pri_aux_image_status);

	if (qla28xx_check_aux_image_status_signature(&pri_aux_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018b,
		    "Primary aux image signature (%#x) not valid\n",
		    le32_to_cpu(pri_aux_image_status.signature));
		goto check_sec_image;
	}

	if (qla27xx_image_status_checksum(&pri_aux_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018c,
		    "Primary aux image checksum failed\n");
		goto check_sec_image;
	}

	valid_pri_image = true;

	if (pri_aux_image_status.image_status_mask & 1) {
		ql_dbg(ql_dbg_init, vha, 0x018d,
		    "Primary aux image is active\n");
		active_pri_image = true;
	}

check_sec_image:
	if (!ha->flt_region_aux_img_status_sec) {
		ql_dbg(ql_dbg_init, vha, 0x018a,
		    "Secondary aux image not addressed\n");
		goto check_valid_image;
	}

	qla24xx_read_flash_data(vha, (uint32_t *)&sec_aux_image_status,
	    ha->flt_region_aux_img_status_sec,
	    sizeof(sec_aux_image_status) >> 2);
	qla27xx_print_image(vha, "Secondary aux image", &sec_aux_image_status);

	if (qla28xx_check_aux_image_status_signature(&sec_aux_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018b,
		    "Secondary aux image signature (%#x) not valid\n",
		    le32_to_cpu(sec_aux_image_status.signature));
		goto check_valid_image;
	}

	if (qla27xx_image_status_checksum(&sec_aux_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018c,
		    "Secondary aux image checksum failed\n");
		goto check_valid_image;
	}

	valid_sec_image = true;

	if (sec_aux_image_status.image_status_mask & 1) {
		ql_dbg(ql_dbg_init, vha, 0x018d,
		    "Secondary aux image is active\n");
		active_sec_image = true;
	}

check_valid_image:
	if (valid_pri_image && active_pri_image &&
	    valid_sec_image && active_sec_image) {
		if (qla27xx_compare_image_generation(&pri_aux_image_status,
		    &sec_aux_image_status) >= 0) {
			qla28xx_component_status(active_regions,
			    &pri_aux_image_status);
		} else {
			qla28xx_component_status(active_regions,
			    &sec_aux_image_status);
		}
	} else if (valid_pri_image && active_pri_image) {
		qla28xx_component_status(active_regions, &pri_aux_image_status);
	} else if (valid_sec_image && active_sec_image) {
		qla28xx_component_status(active_regions, &sec_aux_image_status);
	}

	ql_dbg(ql_dbg_init, vha, 0x018f,
	    "aux images active: BCFG=%u VPD/NVR=%u NPIV0/1=%u NPIV2/3=%u\n",
	    active_regions->aux.board_config,
	    active_regions->aux.vpd_nvram,
	    active_regions->aux.npiv_config_0_1,
	    active_regions->aux.npiv_config_2_3);
}

void
qla27xx_get_active_image(struct scsi_qla_host *vha,
    struct active_regions *active_regions)
{
	struct qla_hw_data *ha = vha->hw;
	struct qla27xx_image_status pri_image_status, sec_image_status;
	bool valid_pri_image = false, valid_sec_image = false;
	bool active_pri_image = false, active_sec_image = false;

	if (!ha->flt_region_img_status_pri) {
		ql_dbg(ql_dbg_init, vha, 0x018a, "Primary image not addressed\n");
		goto check_sec_image;
	}

	if (qla24xx_read_flash_data(vha, (uint32_t *)&pri_image_status,
	    ha->flt_region_img_status_pri, sizeof(pri_image_status) >> 2) !=
	    QLA_SUCCESS) {
		WARN_ON_ONCE(true);
		goto check_sec_image;
	}
	qla27xx_print_image(vha, "Primary image", &pri_image_status);

	if (qla27xx_check_image_status_signature(&pri_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018b,
		    "Primary image signature (%#x) not valid\n",
		    le32_to_cpu(pri_image_status.signature));
		goto check_sec_image;
	}

	if (qla27xx_image_status_checksum(&pri_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018c,
		    "Primary image checksum failed\n");
		goto check_sec_image;
	}

	valid_pri_image = true;

	if (pri_image_status.image_status_mask & 1) {
		ql_dbg(ql_dbg_init, vha, 0x018d,
		    "Primary image is active\n");
		active_pri_image = true;
	}

check_sec_image:
	if (!ha->flt_region_img_status_sec) {
		ql_dbg(ql_dbg_init, vha, 0x018a, "Secondary image not addressed\n");
		goto check_valid_image;
	}

	qla24xx_read_flash_data(vha, (uint32_t *)(&sec_image_status),
	    ha->flt_region_img_status_sec, sizeof(sec_image_status) >> 2);
	qla27xx_print_image(vha, "Secondary image", &sec_image_status);

	if (qla27xx_check_image_status_signature(&sec_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018b,
		    "Secondary image signature (%#x) not valid\n",
		    le32_to_cpu(sec_image_status.signature));
		goto check_valid_image;
	}

	if (qla27xx_image_status_checksum(&sec_image_status)) {
		ql_dbg(ql_dbg_init, vha, 0x018c,
		    "Secondary image checksum failed\n");
		goto check_valid_image;
	}

	valid_sec_image = true;

	if (sec_image_status.image_status_mask & 1) {
		ql_dbg(ql_dbg_init, vha, 0x018d,
		    "Secondary image is active\n");
		active_sec_image = true;
	}

check_valid_image:
	if (valid_pri_image && active_pri_image)
		active_regions->global = QLA27XX_PRIMARY_IMAGE;

	if (valid_sec_image && active_sec_image) {
		if (!active_regions->global ||
		    qla27xx_compare_image_generation(
			&pri_image_status, &sec_image_status) < 0) {
			active_regions->global = QLA27XX_SECONDARY_IMAGE;
		}
	}

	ql_dbg(ql_dbg_init, vha, 0x018f, "active image %s (%u)\n",
	    active_regions->global == QLA27XX_DEFAULT_IMAGE ?
		"default (boot/fw)" :
	    active_regions->global == QLA27XX_PRIMARY_IMAGE ?
		"primary" :
	    active_regions->global == QLA27XX_SECONDARY_IMAGE ?
		"secondary" : "invalid",
	    active_regions->global);
}

bool qla24xx_risc_firmware_invalid(uint32_t *dword)
{
	return
	    !(dword[4] | dword[5] | dword[6] | dword[7]) ||
	    !(~dword[4] | ~dword[5] | ~dword[6] | ~dword[7]);
}

static int
qla24xx_load_risc_flash(scsi_qla_host_t *vha, uint32_t *srisc_addr,
    uint32_t faddr)
{
	int rval;
	uint templates, segments, fragment;
	ulong i;
	uint j;
	ulong dlen;
	uint32_t *dcode;
	uint32_t risc_addr, risc_size, risc_attr = 0;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req = ha->req_q_map[0];
	struct fwdt *fwdt = ha->fwdt;

	ql_dbg(ql_dbg_init, vha, 0x008b,
	    "FW: Loading firmware from flash (%x).\n", faddr);

	dcode = (uint32_t *)req->ring;
	qla24xx_read_flash_data(vha, dcode, faddr, 8);
	if (qla24xx_risc_firmware_invalid(dcode)) {
		ql_log(ql_log_fatal, vha, 0x008c,
		    "Unable to verify the integrity of flash firmware "
		    "image.\n");
		ql_log(ql_log_fatal, vha, 0x008d,
		    "Firmware data: %08x %08x %08x %08x.\n",
		    dcode[0], dcode[1], dcode[2], dcode[3]);

		return QLA_FUNCTION_FAILED;
	}

	dcode = (uint32_t *)req->ring;
	*srisc_addr = 0;
	segments = FA_RISC_CODE_SEGMENTS;
	for (j = 0; j < segments; j++) {
		ql_dbg(ql_dbg_init, vha, 0x008d,
		    "-> Loading segment %u...\n", j);
		qla24xx_read_flash_data(vha, dcode, faddr, 10);
		risc_addr = be32_to_cpu((__force __be32)dcode[2]);
		risc_size = be32_to_cpu((__force __be32)dcode[3]);
		if (!*srisc_addr) {
			*srisc_addr = risc_addr;
			risc_attr = be32_to_cpu((__force __be32)dcode[9]);
		}

		dlen = ha->fw_transfer_size >> 2;
		for (fragment = 0; risc_size; fragment++) {
			if (dlen > risc_size)
				dlen = risc_size;

			ql_dbg(ql_dbg_init, vha, 0x008e,
			    "-> Loading fragment %u: %#x <- %#x (%#lx dwords)...\n",
			    fragment, risc_addr, faddr, dlen);
			qla24xx_read_flash_data(vha, dcode, faddr, dlen);
			for (i = 0; i < dlen; i++)
				dcode[i] = swab32(dcode[i]);

			rval = qla2x00_load_ram(vha, req->dma, risc_addr, dlen);
			if (rval) {
				ql_log(ql_log_fatal, vha, 0x008f,
				    "-> Failed load firmware fragment %u.\n",
				    fragment);
				return QLA_FUNCTION_FAILED;
			}

			faddr += dlen;
			risc_addr += dlen;
			risc_size -= dlen;
		}
	}

	if (!IS_QLA27XX(ha) && !IS_QLA28XX(ha))
		return QLA_SUCCESS;

	templates = (risc_attr & BIT_9) ? 2 : 1;
	ql_dbg(ql_dbg_init, vha, 0x0160, "-> templates = %u\n", templates);
	for (j = 0; j < templates; j++, fwdt++) {
		vfree(fwdt->template);
		fwdt->template = NULL;
		fwdt->length = 0;

		dcode = (uint32_t *)req->ring;
		qla24xx_read_flash_data(vha, dcode, faddr, 7);
		risc_size = be32_to_cpu((__force __be32)dcode[2]);
		ql_dbg(ql_dbg_init, vha, 0x0161,
		    "-> fwdt%u template array at %#x (%#x dwords)\n",
		    j, faddr, risc_size);
		if (!risc_size || !~risc_size) {
			ql_dbg(ql_dbg_init, vha, 0x0162,
			    "-> fwdt%u failed to read array\n", j);
			goto failed;
		}

		/* skip header and ignore checksum */
		faddr += 7;
		risc_size -= 8;

		ql_dbg(ql_dbg_init, vha, 0x0163,
		    "-> fwdt%u template allocate template %#x words...\n",
		    j, risc_size);
		fwdt->template = vmalloc(risc_size * sizeof(*dcode));
		if (!fwdt->template) {
			ql_log(ql_log_warn, vha, 0x0164,
			    "-> fwdt%u failed allocate template.\n", j);
			goto failed;
		}

		dcode = fwdt->template;
		qla24xx_read_flash_data(vha, dcode, faddr, risc_size);

		if (!qla27xx_fwdt_template_valid(dcode)) {
			ql_log(ql_log_warn, vha, 0x0165,
			    "-> fwdt%u failed template validate\n", j);
			goto failed;
		}

		dlen = qla27xx_fwdt_template_size(dcode);
		ql_dbg(ql_dbg_init, vha, 0x0166,
		    "-> fwdt%u template size %#lx bytes (%#lx words)\n",
		    j, dlen, dlen / sizeof(*dcode));
		if (dlen > risc_size * sizeof(*dcode)) {
			ql_log(ql_log_warn, vha, 0x0167,
			    "-> fwdt%u template exceeds array (%-lu bytes)\n",
			    j, dlen - risc_size * sizeof(*dcode));
			goto failed;
		}

		fwdt->length = dlen;
		ql_dbg(ql_dbg_init, vha, 0x0168,
		    "-> fwdt%u loaded template ok\n", j);

		faddr += risc_size + 1;
	}

	return QLA_SUCCESS;

failed:
	vfree(fwdt->template);
	fwdt->template = NULL;
	fwdt->length = 0;

	return QLA_SUCCESS;
}

#define QLA_FW_URL "http://ldriver.qlogic.com/firmware/"

int
qla2x00_load_risc(scsi_qla_host_t *vha, uint32_t *srisc_addr)
{
	int	rval;
	int	i, fragment;
	uint16_t *wcode;
	__be16	 *fwcode;
	uint32_t risc_addr, risc_size, fwclen, wlen, *seg;
	struct fw_blob *blob;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req = ha->req_q_map[0];

	/* Load firmware blob. */
	blob = qla2x00_request_firmware(vha);
	if (!blob) {
		ql_log(ql_log_info, vha, 0x0083,
		    "Firmware image unavailable.\n");
		ql_log(ql_log_info, vha, 0x0084,
		    "Firmware images can be retrieved from: "QLA_FW_URL ".\n");
		return QLA_FUNCTION_FAILED;
	}

	rval = QLA_SUCCESS;

	wcode = (uint16_t *)req->ring;
	*srisc_addr = 0;
	fwcode = (__force __be16 *)blob->fw->data;
	fwclen = 0;

	/* Validate firmware image by checking version. */
	if (blob->fw->size < 8 * sizeof(uint16_t)) {
		ql_log(ql_log_fatal, vha, 0x0085,
		    "Unable to verify integrity of firmware image (%zd).\n",
		    blob->fw->size);
		goto fail_fw_integrity;
	}
	for (i = 0; i < 4; i++)
		wcode[i] = be16_to_cpu(fwcode[i + 4]);
	if ((wcode[0] == 0xffff && wcode[1] == 0xffff && wcode[2] == 0xffff &&
	    wcode[3] == 0xffff) || (wcode[0] == 0 && wcode[1] == 0 &&
		wcode[2] == 0 && wcode[3] == 0)) {
		ql_log(ql_log_fatal, vha, 0x0086,
		    "Unable to verify integrity of firmware image.\n");
		ql_log(ql_log_fatal, vha, 0x0087,
		    "Firmware data: %04x %04x %04x %04x.\n",
		    wcode[0], wcode[1], wcode[2], wcode[3]);
		goto fail_fw_integrity;
	}

	seg = blob->segs;
	while (*seg && rval == QLA_SUCCESS) {
		risc_addr = *seg;
		*srisc_addr = *srisc_addr == 0 ? *seg : *srisc_addr;
		risc_size = be16_to_cpu(fwcode[3]);

		/* Validate firmware image size. */
		fwclen += risc_size * sizeof(uint16_t);
		if (blob->fw->size < fwclen) {
			ql_log(ql_log_fatal, vha, 0x0088,
			    "Unable to verify integrity of firmware image "
			    "(%zd).\n", blob->fw->size);
			goto fail_fw_integrity;
		}

		fragment = 0;
		while (risc_size > 0 && rval == QLA_SUCCESS) {
			wlen = (uint16_t)(ha->fw_transfer_size >> 1);
			if (wlen > risc_size)
				wlen = risc_size;
			ql_dbg(ql_dbg_init, vha, 0x0089,
			    "Loading risc segment@ risc addr %x number of "
			    "words 0x%x.\n", risc_addr, wlen);

			for (i = 0; i < wlen; i++)
				wcode[i] = swab16((__force u32)fwcode[i]);

			rval = qla2x00_load_ram(vha, req->dma, risc_addr,
			    wlen);
			if (rval) {
				ql_log(ql_log_fatal, vha, 0x008a,
				    "Failed to load segment %d of firmware.\n",
				    fragment);
				break;
			}

			fwcode += wlen;
			risc_addr += wlen;
			risc_size -= wlen;
			fragment++;
		}

		/* Next segment. */
		seg++;
	}
	return rval;

fail_fw_integrity:
	return QLA_FUNCTION_FAILED;
}

static int
qla24xx_load_risc_blob(scsi_qla_host_t *vha, uint32_t *srisc_addr)
{
	int	rval;
	uint templates, segments, fragment;
	uint32_t *dcode;
	ulong dlen;
	uint32_t risc_addr, risc_size, risc_attr = 0;
	ulong i;
	uint j;
	struct fw_blob *blob;
	__be32 *fwcode;
	struct qla_hw_data *ha = vha->hw;
	struct req_que *req = ha->req_q_map[0];
	struct fwdt *fwdt = ha->fwdt;

	ql_dbg(ql_dbg_init, vha, 0x0090,
	    "-> FW: Loading via request-firmware.\n");

	blob = qla2x00_request_firmware(vha);
	if (!blob) {
		ql_log(ql_log_warn, vha, 0x0092,
		    "-> Firmware file not found.\n");

		return QLA_FUNCTION_FAILED;
	}

	fwcode = (__force __be32 *)blob->fw->data;
	dcode = (__force uint32_t *)fwcode;
	if (qla24xx_risc_firmware_invalid(dcode)) {
		ql_log(ql_log_fatal, vha, 0x0093,
		    "Unable to verify integrity of firmware image (%zd).\n",
		    blob->fw->size);
		ql_log(ql_log_fatal, vha, 0x0095,
		    "Firmware data: %08x %08x %08x %08x.\n",
		    dcode[0], dcode[1], dcode[2], dcode[3]);
		return QLA_FUNCTION_FAILED;
	}

	dcode = (uint32_t *)req->ring;
	*srisc_addr = 0;
	segments = FA_RISC_CODE_SEGMENTS;
	for (j = 0; j < segments; j++) {
		ql_dbg(ql_dbg_init, vha, 0x0096,
		    "-> Loading segment %u...\n", j);
		risc_addr = be32_to_cpu(fwcode[2]);
		risc_size = be32_to_cpu(fwcode[3]);

		if (!*srisc_addr) {
			*srisc_addr = risc_addr;
			risc_attr = be32_to_cpu(fwcode[9]);
		}

		dlen = ha->fw_transfer_size >> 2;
		for (fragment = 0; risc_size; fragment++) {
			if (dlen > risc_size)
				dlen = risc_size;

			ql_dbg(ql_dbg_init, vha, 0x0097,
			    "-> Loading fragment %u: %#x <- %#x (%#lx words)...\n",
			    fragment, risc_addr,
			    (uint32_t)(fwcode - (typeof(fwcode))blob->fw->data),
			    dlen);

			for (i = 0; i < dlen; i++)
				dcode[i] = swab32((__force u32)fwcode[i]);

			rval = qla2x00_load_ram(vha, req->dma, risc_addr, dlen);
			if (rval) {
				ql_log(ql_log_fatal, vha, 0x0098,
				    "-> Failed load firmware fragment %u.\n",
				    fragment);
				return QLA_FUNCTION_FAILED;
			}

			fwcode += dlen;
			risc_addr += dlen;
			risc_size -= dlen;
		}
	}

	if (!IS_QLA27XX(ha) && !IS_QLA28XX(ha))
		return QLA_SUCCESS;

	templates = (risc_attr & BIT_9) ? 2 : 1;
	ql_dbg(ql_dbg_init, vha, 0x0170, "-> templates = %u\n", templates);
	for (j = 0; j < templates; j++, fwdt++) {
		vfree(fwdt->template);
		fwdt->template = NULL;
		fwdt->length = 0;

		risc_size = be32_to_cpu(fwcode[2]);
		ql_dbg(ql_dbg_init, vha, 0x0171,
		    "-> fwdt%u template array at %#x (%#x dwords)\n",
		    j, (uint32_t)((void *)fwcode - (void *)blob->fw->data),
		    risc_size);
		if (!risc_size || !~risc_size) {
			ql_dbg(ql_dbg_init, vha, 0x0172,
			    "-> fwdt%u failed to read array\n", j);
			goto failed;
		}

		/* skip header and ignore checksum */
		fwcode += 7;
		risc_size -= 8;

		ql_dbg(ql_dbg_init, vha, 0x0173,
		    "-> fwdt%u template allocate template %#x words...\n",
		    j, risc_size);
		fwdt->template = vmalloc(risc_size * sizeof(*dcode));
		if (!fwdt->template) {
			ql_log(ql_log_warn, vha, 0x0174,
			    "-> fwdt%u failed allocate template.\n", j);
			goto failed;
		}

		dcode = fwdt->template;
		for (i = 0; i < risc_size; i++)
			dcode[i] = (__force u32)fwcode[i];

		if (!qla27xx_fwdt_template_valid(dcode)) {
			ql_log(ql_log_warn, vha, 0x0175,
			    "-> fwdt%u failed template validate\n", j);
			goto failed;
		}

		dlen = qla27xx_fwdt_template_size(dcode);
		ql_dbg(ql_dbg_init, vha, 0x0176,
		    "-> fwdt%u template size %#lx bytes (%#lx words)\n",
		    j, dlen, dlen / sizeof(*dcode));
		if (dlen > risc_size * sizeof(*dcode)) {
			ql_log(ql_log_warn, vha, 0x0177,
			    "-> fwdt%u template exceeds array (%-lu bytes)\n",
			    j, dlen - risc_size * sizeof(*dcode));
			goto failed;
		}

		fwdt->length = dlen;
		ql_dbg(ql_dbg_init, vha, 0x0178,
		    "-> fwdt%u loaded template ok\n", j);

		fwcode += risc_size + 1;
	}

	return QLA_SUCCESS;

failed:
	vfree(fwdt->template);
	fwdt->template = NULL;
	fwdt->length = 0;

	return QLA_SUCCESS;
}

int
qla24xx_load_risc(scsi_qla_host_t *vha, uint32_t *srisc_addr)
{
	int rval;

	if (ql2xfwloadbin == 1)
		return qla81xx_load_risc(vha, srisc_addr);

	/*
	 * FW Load priority:
	 * 1) Firmware via request-firmware interface (.bin file).
	 * 2) Firmware residing in flash.
	 */
	rval = qla24xx_load_risc_blob(vha, srisc_addr);
	if (rval == QLA_SUCCESS)
		return rval;

	return qla24xx_load_risc_flash(vha, srisc_addr,
	    vha->hw->flt_region_fw);
}

int
qla81xx_load_risc(scsi_qla_host_t *vha, uint32_t *srisc_addr)
{
	int rval;
	struct qla_hw_data *ha = vha->hw;
	struct active_regions active_regions = { };

	if (ql2xfwloadbin == 2)
		goto try_blob_fw;

	/* FW Load priority:
	 * 1) Firmware residing in flash.
	 * 2) Firmware via request-firmware interface (.bin file).
	 * 3) Golden-Firmware residing in flash -- (limited operation).
	 */

	if (!IS_QLA27XX(ha) && !IS_QLA28XX(ha))
		goto try_primary_fw;

	qla27xx_get_active_image(vha, &active_regions);

	if (active_regions.global != QLA27XX_SECONDARY_IMAGE)
		goto try_primary_fw;

	ql_dbg(ql_dbg_init, vha, 0x008b,
	    "Loading secondary firmware image.\n");
	rval = qla24xx_load_risc_flash(vha, srisc_addr, ha->flt_region_fw_sec);
	if (!rval)
		return rval;

try_primary_fw:
	ql_dbg(ql_dbg_init, vha, 0x008b,
	    "Loading primary firmware image.\n");
	rval = qla24xx_load_risc_flash(vha, srisc_addr, ha->flt_region_fw);
	if (!rval)
		return rval;

try_blob_fw:
	rval = qla24xx_load_risc_blob(vha, srisc_addr);
	if (!rval || !ha->flt_region_gold_fw)
		return rval;

	ql_log(ql_log_info, vha, 0x0099,
	    "Attempting to fallback to golden firmware.\n");
	rval = qla24xx_load_risc_flash(vha, srisc_addr, ha->flt_region_gold_fw);
	if (rval)
		return rval;

	ql_log(ql_log_info, vha, 0x009a, "Need firmware flash update.\n");
	ha->flags.running_gold_fw = 1;
	return rval;
}

void
qla2x00_try_to_stop_firmware(scsi_qla_host_t *vha)
{
	int ret, retries;
	struct qla_hw_data *ha = vha->hw;

	if (ha->flags.pci_channel_io_perm_failure)
		return;
	if (!IS_FWI2_CAPABLE(ha))
		return;
	if (!ha->fw_major_version)
		return;
	if (!ha->flags.fw_started)
		return;

	ret = qla2x00_stop_firmware(vha);
	for (retries = 5; ret != QLA_SUCCESS && ret != QLA_FUNCTION_TIMEOUT &&
	    ret != QLA_INVALID_COMMAND && retries ; retries--) {
		ha->isp_ops->reset_chip(vha);
		if (ha->isp_ops->chip_diag(vha) != QLA_SUCCESS)
			continue;
		if (qla2x00_setup_chip(vha) != QLA_SUCCESS)
			continue;
		ql_log(ql_log_info, vha, 0x8015,
		    "Attempting retry of stop-firmware command.\n");
		ret = qla2x00_stop_firmware(vha);
	}

	QLA_FW_STOPPED(ha);
	ha->flags.fw_init_done = 0;
}

int
qla24xx_configure_vhba(scsi_qla_host_t *vha)
{
	int rval = QLA_SUCCESS;
	int rval2;
	uint16_t mb[MAILBOX_REGISTER_COUNT];
	struct qla_hw_data *ha = vha->hw;
	struct scsi_qla_host *base_vha = pci_get_drvdata(ha->pdev);

	if (!vha->vp_idx)
		return -EINVAL;

	rval = qla2x00_fw_ready(base_vha);

	if (rval == QLA_SUCCESS) {
		clear_bit(RESET_MARKER_NEEDED, &vha->dpc_flags);
		qla2x00_marker(vha, ha->base_qpair, 0, 0, MK_SYNC_ALL);
	}

	vha->flags.management_server_logged_in = 0;

	/* Login to SNS first */
	rval2 = ha->isp_ops->fabric_login(vha, NPH_SNS, 0xff, 0xff, 0xfc, mb,
	    BIT_1);
	if (rval2 != QLA_SUCCESS || mb[0] != MBS_COMMAND_COMPLETE) {
		if (rval2 == QLA_MEMORY_ALLOC_FAILED)
			ql_dbg(ql_dbg_init, vha, 0x0120,
			    "Failed SNS login: loop_id=%x, rval2=%d\n",
			    NPH_SNS, rval2);
		else
			ql_dbg(ql_dbg_init, vha, 0x0103,
			    "Failed SNS login: loop_id=%x mb[0]=%x mb[1]=%x "
			    "mb[2]=%x mb[6]=%x mb[7]=%x.\n",
			    NPH_SNS, mb[0], mb[1], mb[2], mb[6], mb[7]);
		return (QLA_FUNCTION_FAILED);
	}

	atomic_set(&vha->loop_down_timer, 0);
	atomic_set(&vha->loop_state, LOOP_UP);
	set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
	set_bit(LOCAL_LOOP_UPDATE, &vha->dpc_flags);
	rval = qla2x00_loop_resync(base_vha);

	return rval;
}

/* 84XX Support **************************************************************/

static LIST_HEAD(qla_cs84xx_list);
static DEFINE_MUTEX(qla_cs84xx_mutex);

static struct qla_chip_state_84xx *
qla84xx_get_chip(struct scsi_qla_host *vha)
{
	struct qla_chip_state_84xx *cs84xx;
	struct qla_hw_data *ha = vha->hw;

	mutex_lock(&qla_cs84xx_mutex);

	/* Find any shared 84xx chip. */
	list_for_each_entry(cs84xx, &qla_cs84xx_list, list) {
		if (cs84xx->bus == ha->pdev->bus) {
			kref_get(&cs84xx->kref);
			goto done;
		}
	}

	cs84xx = kzalloc(sizeof(*cs84xx), GFP_KERNEL);
	if (!cs84xx)
		goto done;

	kref_init(&cs84xx->kref);
	spin_lock_init(&cs84xx->access_lock);
	mutex_init(&cs84xx->fw_update_mutex);
	cs84xx->bus = ha->pdev->bus;

	list_add_tail(&cs84xx->list, &qla_cs84xx_list);
done:
	mutex_unlock(&qla_cs84xx_mutex);
	return cs84xx;
}

static void
__qla84xx_chip_release(struct kref *kref)
{
	struct qla_chip_state_84xx *cs84xx =
	    container_of(kref, struct qla_chip_state_84xx, kref);

	mutex_lock(&qla_cs84xx_mutex);
	list_del(&cs84xx->list);
	mutex_unlock(&qla_cs84xx_mutex);
	kfree(cs84xx);
}

void
qla84xx_put_chip(struct scsi_qla_host *vha)
{
	struct qla_hw_data *ha = vha->hw;

	if (ha->cs84xx)
		kref_put(&ha->cs84xx->kref, __qla84xx_chip_release);
}

static int
qla84xx_init_chip(scsi_qla_host_t *vha)
{
	int rval;
	uint16_t status[2];
	struct qla_hw_data *ha = vha->hw;

	mutex_lock(&ha->cs84xx->fw_update_mutex);

	rval = qla84xx_verify_chip(vha, status);

	mutex_unlock(&ha->cs84xx->fw_update_mutex);

	return rval != QLA_SUCCESS || status[0] ? QLA_FUNCTION_FAILED :
	    QLA_SUCCESS;
}

/* 81XX Support **************************************************************/

int
qla81xx_nvram_config(scsi_qla_host_t *vha)
{
	int   rval;
	struct init_cb_81xx *icb;
	struct nvram_81xx *nv;
	__le32 *dptr;
	uint8_t  *dptr1, *dptr2;
	uint32_t chksum;
	uint16_t cnt;
	struct qla_hw_data *ha = vha->hw;
	uint32_t faddr;
	struct active_regions active_regions = { };

	rval = QLA_SUCCESS;
	icb = (struct init_cb_81xx *)ha->init_cb;
	nv = ha->nvram;

	/* Determine NVRAM starting address. */
	ha->nvram_size = sizeof(*nv);
	ha->vpd_size = FA_NVRAM_VPD_SIZE;
	if (IS_P3P_TYPE(ha) || IS_QLA8031(ha))
		ha->vpd_size = FA_VPD_SIZE_82XX;

	if (IS_QLA28XX(ha) || IS_QLA27XX(ha))
		qla28xx_get_aux_images(vha, &active_regions);

	/* Get VPD data into cache */
	ha->vpd = ha->nvram + VPD_OFFSET;

	faddr = ha->flt_region_vpd;
	if (IS_QLA28XX(ha)) {
		if (active_regions.aux.vpd_nvram == QLA27XX_SECONDARY_IMAGE)
			faddr = ha->flt_region_vpd_sec;
		ql_dbg(ql_dbg_init, vha, 0x0110,
		    "Loading %s nvram image.\n",
		    active_regions.aux.vpd_nvram == QLA27XX_PRIMARY_IMAGE ?
		    "primary" : "secondary");
	}
	ha->isp_ops->read_optrom(vha, ha->vpd, faddr << 2, ha->vpd_size);

	/* Get NVRAM data into cache and calculate checksum. */
	faddr = ha->flt_region_nvram;
	if (IS_QLA28XX(ha)) {
		if (active_regions.aux.vpd_nvram == QLA27XX_SECONDARY_IMAGE)
			faddr = ha->flt_region_nvram_sec;
	}
	ql_dbg(ql_dbg_init, vha, 0x0110,
	    "Loading %s nvram image.\n",
	    active_regions.aux.vpd_nvram == QLA27XX_PRIMARY_IMAGE ?
	    "primary" : "secondary");
	ha->isp_ops->read_optrom(vha, ha->nvram, faddr << 2, ha->nvram_size);

	dptr = (__force __le32 *)nv;
	for (cnt = 0, chksum = 0; cnt < ha->nvram_size >> 2; cnt++, dptr++)
		chksum += le32_to_cpu(*dptr);

	ql_dbg(ql_dbg_init + ql_dbg_buffer, vha, 0x0111,
	    "Contents of NVRAM:\n");
	ql_dump_buffer(ql_dbg_init + ql_dbg_buffer, vha, 0x0112,
	    nv, ha->nvram_size);

	/* Bad NVRAM data, set defaults parameters. */
	if (chksum || memcmp("ISP ", nv->id, sizeof(nv->id)) ||
	    le16_to_cpu(nv->nvram_version) < ICB_VERSION) {
		/* Reset NVRAM data. */
		ql_log(ql_log_info, vha, 0x0073,
		    "Inconsistent NVRAM checksum=%#x id=%.4s version=%#x.\n",
		    chksum, nv->id, le16_to_cpu(nv->nvram_version));
		ql_dump_buffer(ql_dbg_init, vha, 0x0073, nv, sizeof(*nv));
		ql_log(ql_log_info, vha, 0x0074,
		    "Falling back to functioning (yet invalid -- WWPN) "
		    "defaults.\n");

		/*
		 * Set default initialization control block.
		 */
		memset(nv, 0, ha->nvram_size);
		nv->nvram_version = cpu_to_le16(ICB_VERSION);
		nv->version = cpu_to_le16(ICB_VERSION);
		nv->frame_payload_size = cpu_to_le16(2048);
		nv->execution_throttle = cpu_to_le16(0xFFFF);
		nv->exchange_count = cpu_to_le16(0);
		nv->port_name[0] = 0x21;
		nv->port_name[1] = 0x00 + ha->port_no + 1;
		nv->port_name[2] = 0x00;
		nv->port_name[3] = 0xe0;
		nv->port_name[4] = 0x8b;
		nv->port_name[5] = 0x1c;
		nv->port_name[6] = 0x55;
		nv->port_name[7] = 0x86;
		nv->node_name[0] = 0x20;
		nv->node_name[1] = 0x00;
		nv->node_name[2] = 0x00;
		nv->node_name[3] = 0xe0;
		nv->node_name[4] = 0x8b;
		nv->node_name[5] = 0x1c;
		nv->node_name[6] = 0x55;
		nv->node_name[7] = 0x86;
		nv->login_retry_count = cpu_to_le16(8);
		nv->interrupt_delay_timer = cpu_to_le16(0);
		nv->login_timeout = cpu_to_le16(0);
		nv->firmware_options_1 =
		    cpu_to_le32(BIT_14|BIT_13|BIT_2|BIT_1);
		nv->firmware_options_2 = cpu_to_le32(2 << 4);
		nv->firmware_options_2 |= cpu_to_le32(BIT_12);
		nv->firmware_options_3 = cpu_to_le32(2 << 13);
		nv->host_p = cpu_to_le32(BIT_11|BIT_10);
		nv->efi_parameters = cpu_to_le32(0);
		nv->reset_delay = 5;
		nv->max_luns_per_target = cpu_to_le16(128);
		nv->port_down_retry_count = cpu_to_le16(30);
		nv->link_down_timeout = cpu_to_le16(180);
		nv->enode_mac[0] = 0x00;
		nv->enode_mac[1] = 0xC0;
		nv->enode_mac[2] = 0xDD;
		nv->enode_mac[3] = 0x04;
		nv->enode_mac[4] = 0x05;
		nv->enode_mac[5] = 0x06 + ha->port_no + 1;

		rval = 1;
	}

	if (IS_T10_PI_CAPABLE(ha))
		nv->frame_payload_size &= cpu_to_le16(~7);

	qlt_81xx_config_nvram_stage1(vha, nv);

	/* Reset Initialization control block */
	memset(icb, 0, ha->init_cb_size);

	/* Copy 1st segment. */
	dptr1 = (uint8_t *)icb;
	dptr2 = (uint8_t *)&nv->version;
	cnt = (uint8_t *)&icb->response_q_inpointer - (uint8_t *)&icb->version;
	while (cnt--)
		*dptr1++ = *dptr2++;

	icb->login_retry_count = nv->login_retry_count;

	/* Copy 2nd segment. */
	dptr1 = (uint8_t *)&icb->interrupt_delay_timer;
	dptr2 = (uint8_t *)&nv->interrupt_delay_timer;
	cnt = (uint8_t *)&icb->reserved_5 -
	    (uint8_t *)&icb->interrupt_delay_timer;
	while (cnt--)
		*dptr1++ = *dptr2++;

	memcpy(icb->enode_mac, nv->enode_mac, sizeof(icb->enode_mac));
	/* Some boards (with valid NVRAMs) still have NULL enode_mac!! */
	if (!memcmp(icb->enode_mac, "\0\0\0\0\0\0", sizeof(icb->enode_mac))) {
		icb->enode_mac[0] = 0x00;
		icb->enode_mac[1] = 0xC0;
		icb->enode_mac[2] = 0xDD;
		icb->enode_mac[3] = 0x04;
		icb->enode_mac[4] = 0x05;
		icb->enode_mac[5] = 0x06 + ha->port_no + 1;
	}

	/* Use extended-initialization control block. */
	memcpy(ha->ex_init_cb, &nv->ex_version, sizeof(*ha->ex_init_cb));
	ha->frame_payload_size = le16_to_cpu(icb->frame_payload_size);
	/*
	 * Setup driver NVRAM options.
	 */
	qla2x00_set_model_info(vha, nv->model_name, sizeof(nv->model_name),
	    "QLE8XXX");

	qlt_81xx_config_nvram_stage2(vha, icb);

	/* Use alternate WWN? */
	if (nv->host_p & cpu_to_le32(BIT_15)) {
		memcpy(icb->node_name, nv->alternate_node_name, WWN_SIZE);
		memcpy(icb->port_name, nv->alternate_port_name, WWN_SIZE);
	}

	/* Prepare nodename */
	if ((icb->firmware_options_1 & cpu_to_le32(BIT_14)) == 0) {
		/*
		 * Firmware will apply the following mask if the nodename was
		 * not provided.
		 */
		memcpy(icb->node_name, icb->port_name, WWN_SIZE);
		icb->node_name[0] &= 0xF0;
	}

	if (IS_QLA28XX(ha) || IS_QLA27XX(ha)) {
		if ((nv->enhanced_features & BIT_7) == 0)
			ha->flags.scm_supported_a = 1;
	}

	/* Set host adapter parameters. */
	ha->flags.disable_risc_code_load = 0;
	ha->flags.enable_lip_reset = 0;
	ha->flags.enable_lip_full_login =
	    le32_to_cpu(nv->host_p) & BIT_10 ? 1 : 0;
	ha->flags.enable_target_reset =
	    le32_to_cpu(nv->host_p) & BIT_11 ? 1 : 0;
	ha->flags.enable_led_scheme = 0;
	ha->flags.disable_serdes = le32_to_cpu(nv->host_p) & BIT_5 ? 1 : 0;

	ha->operating_mode = (le32_to_cpu(icb->firmware_options_2) &
	    (BIT_6 | BIT_5 | BIT_4)) >> 4;

	/* save HBA serial number */
	ha->serial0 = icb->port_name[5];
	ha->serial1 = icb->port_name[6];
	ha->serial2 = icb->port_name[7];
	memcpy(vha->node_name, icb->node_name, WWN_SIZE);
	memcpy(vha->port_name, icb->port_name, WWN_SIZE);

	icb->execution_throttle = cpu_to_le16(0xFFFF);

	ha->retry_count = le16_to_cpu(nv->login_retry_count);

	/* Set minimum login_timeout to 4 seconds. */
	if (le16_to_cpu(nv->login_timeout) < ql2xlogintimeout)
		nv->login_timeout = cpu_to_le16(ql2xlogintimeout);
	if (le16_to_cpu(nv->login_timeout) < 4)
		nv->login_timeout = cpu_to_le16(4);
	ha->login_timeout = le16_to_cpu(nv->login_timeout);

	/* Set minimum RATOV to 100 tenths of a second. */
	ha->r_a_tov = 100;

	ha->loop_reset_delay = nv->reset_delay;

	/* Link Down Timeout = 0:
	 *
	 *	When Port Down timer expires we will start returning
	 *	I/O's to OS with "DID_NO_CONNECT".
	 *
	 * Link Down Timeout != 0:
	 *
	 *	 The driver waits for the link to come up after link down
	 *	 before returning I/Os to OS with "DID_NO_CONNECT".
	 */
	if (le16_to_cpu(nv->link_down_timeout) == 0) {
		ha->loop_down_abort_time =
		    (LOOP_DOWN_TIME - LOOP_DOWN_TIMEOUT);
	} else {
		ha->link_down_timeout =	le16_to_cpu(nv->link_down_timeout);
		ha->loop_down_abort_time =
		    (LOOP_DOWN_TIME - ha->link_down_timeout);
	}

	/* Need enough time to try and get the port back. */
	ha->port_down_retry_count = le16_to_cpu(nv->port_down_retry_count);
	if (qlport_down_retry)
		ha->port_down_retry_count = qlport_down_retry;

	/* Set login_retry_count */
	ha->login_retry_count  = le16_to_cpu(nv->login_retry_count);
	if (ha->port_down_retry_count ==
	    le16_to_cpu(nv->port_down_retry_count) &&
	    ha->port_down_retry_count > 3)
		ha->login_retry_count = ha->port_down_retry_count;
	else if (ha->port_down_retry_count > (int)ha->login_retry_count)
		ha->login_retry_count = ha->port_down_retry_count;
	if (ql2xloginretrycount)
		ha->login_retry_count = ql2xloginretrycount;

	/* if not running MSI-X we need handshaking on interrupts */
	if (!vha->hw->flags.msix_enabled &&
	    (IS_QLA83XX(ha) || IS_QLA27XX(ha) || IS_QLA28XX(ha)))
		icb->firmware_options_2 |= cpu_to_le32(BIT_22);

	/* Enable ZIO. */
	if (!vha->flags.init_done) {
		ha->zio_mode = le32_to_cpu(icb->firmware_options_2) &
		    (BIT_3 | BIT_2 | BIT_1 | BIT_0);
		ha->zio_timer = le16_to_cpu(icb->interrupt_delay_timer) ?
		    le16_to_cpu(icb->interrupt_delay_timer) : 2;
	}
	icb->firmware_options_2 &= cpu_to_le32(
	    ~(BIT_3 | BIT_2 | BIT_1 | BIT_0));
	vha->flags.process_response_queue = 0;
	if (ha->zio_mode != QLA_ZIO_DISABLED) {
		ha->zio_mode = QLA_ZIO_MODE_6;

		ql_log(ql_log_info, vha, 0x0075,
		    "ZIO mode %d enabled; timer delay (%d us).\n",
		    ha->zio_mode,
		    ha->zio_timer * 100);

		icb->firmware_options_2 |= cpu_to_le32(
		    (uint32_t)ha->zio_mode);
		icb->interrupt_delay_timer = cpu_to_le16(ha->zio_timer);
		vha->flags.process_response_queue = 1;
	}

	 /* enable RIDA Format2 */
	icb->firmware_options_3 |= cpu_to_le32(BIT_0);

	/* N2N: driver will initiate Login instead of FW */
	icb->firmware_options_3 |= cpu_to_le32(BIT_8);

	/* Determine NVMe/FCP priority for target ports */
	ha->fc4_type_priority = qla2xxx_get_fc4_priority(vha);

	if (rval) {
		ql_log(ql_log_warn, vha, 0x0076,
		    "NVRAM configuration failed.\n");
	}
	return (rval);
}

int
qla82xx_restart_isp(scsi_qla_host_t *vha)
{
	int status, rval;
	struct qla_hw_data *ha = vha->hw;
	struct scsi_qla_host *vp;
	unsigned long flags;

	status = qla2x00_init_rings(vha);
	if (!status) {
		clear_bit(RESET_MARKER_NEEDED, &vha->dpc_flags);
		ha->flags.chip_reset_done = 1;

		status = qla2x00_fw_ready(vha);
		if (!status) {
			/* Issue a marker after FW becomes ready. */
			qla2x00_marker(vha, ha->base_qpair, 0, 0, MK_SYNC_ALL);
			vha->flags.online = 1;
			set_bit(LOOP_RESYNC_NEEDED, &vha->dpc_flags);
		}

		/* if no cable then assume it's good */
		if ((vha->device_flags & DFLG_NO_CABLE))
			status = 0;
	}

	if (!status) {
		clear_bit(RESET_MARKER_NEEDED, &vha->dpc_flags);

		if (!atomic_read(&vha->loop_down_timer)) {
			/*
			 * Issue marker command only when we are going
			 * to start the I/O .
			 */
			vha->marker_needed = 1;
		}

		ha->isp_ops->enable_intrs(ha);

		ha->isp_abort_cnt = 0;
		clear_bit(ISP_ABORT_RETRY, &vha->dpc_flags);

		/* Update the firmware version */
		status = qla82xx_check_md_needed(vha);

		if (ha->fce) {
			ha->flags.fce_enabled = 1;
			memset(ha->fce, 0,
			    fce_calc_size(ha->fce_bufs));
			rval = qla2x00_enable_fce_trace(vha,
			    ha->fce_dma, ha->fce_bufs, ha->fce_mb,
			    &ha->fce_bufs);
			if (rval) {
				ql_log(ql_log_warn, vha, 0x8001,
				    "Unable to reinitialize FCE (%d).\n",
				    rval);
				ha->flags.fce_enabled = 0;
			}
		}

		if (ha->eft) {
			memset(ha->eft, 0, EFT_SIZE);
			rval = qla2x00_enable_eft_trace(vha,
			    ha->eft_dma, EFT_NUM_BUFFERS);
			if (rval) {
				ql_log(ql_log_warn, vha, 0x8010,
				    "Unable to reinitialize EFT (%d).\n",
				    rval);
			}
		}
	}

	if (!status) {
		ql_dbg(ql_dbg_taskm, vha, 0x8011,
		    "qla82xx_restart_isp succeeded.\n");

		spin_lock_irqsave(&ha->vport_slock, flags);
		list_for_each_entry(vp, &ha->vp_list, list) {
			if (vp->vp_idx) {
				atomic_inc(&vp->vref_count);
				spin_unlock_irqrestore(&ha->vport_slock, flags);

				qla2x00_vp_abort_isp(vp);

				spin_lock_irqsave(&ha->vport_slock, flags);
				atomic_dec(&vp->vref_count);
			}
		}
		spin_unlock_irqrestore(&ha->vport_slock, flags);

	} else {
		ql_log(ql_log_warn, vha, 0x8016,
		    "qla82xx_restart_isp **** FAILED ****.\n");
	}

	return status;
}

/*
 * qla24xx_get_fcp_prio
 *	Gets the fcp cmd priority value for the logged in port.
 *	Looks for a match of the port descriptors within
 *	each of the fcp prio config entries. If a match is found,
 *	the tag (priority) value is returned.
 *
 * Input:
 *	vha = scsi host structure pointer.
 *	fcport = port structure pointer.
 *
 * Return:
 *	non-zero (if found)
 *	-1 (if not found)
 *
 * Context:
 * 	Kernel context
 */
static int
qla24xx_get_fcp_prio(scsi_qla_host_t *vha, fc_port_t *fcport)
{
	int i, entries;
	uint8_t pid_match, wwn_match;
	int priority;
	uint32_t pid1, pid2;
	uint64_t wwn1, wwn2;
	struct qla_fcp_prio_entry *pri_entry;
	struct qla_hw_data *ha = vha->hw;

	if (!ha->fcp_prio_cfg || !ha->flags.fcp_prio_enabled)
		return -1;

	priority = -1;
	entries = ha->fcp_prio_cfg->num_entries;
	pri_entry = &ha->fcp_prio_cfg->entry[0];

	for (i = 0; i < entries; i++) {
		pid_match = wwn_match = 0;

		if (!(pri_entry->flags & FCP_PRIO_ENTRY_VALID)) {
			pri_entry++;
			continue;
		}

		/* check source pid for a match */
		if (pri_entry->flags & FCP_PRIO_ENTRY_SPID_VALID) {
			pid1 = pri_entry->src_pid & INVALID_PORT_ID;
			pid2 = vha->d_id.b24 & INVALID_PORT_ID;
			if (pid1 == INVALID_PORT_ID)
				pid_match++;
			else if (pid1 == pid2)
				pid_match++;
		}

		/* check destination pid for a match */
		if (pri_entry->flags & FCP_PRIO_ENTRY_DPID_VALID) {
			pid1 = pri_entry->dst_pid & INVALID_PORT_ID;
			pid2 = fcport->d_id.b24 & INVALID_PORT_ID;
			if (pid1 == INVALID_PORT_ID)
				pid_match++;
			else if (pid1 == pid2)
				pid_match++;
		}

		/* check source WWN for a match */
		if (pri_entry->flags & FCP_PRIO_ENTRY_SWWN_VALID) {
			wwn1 = wwn_to_u64(vha->port_name);
			wwn2 = wwn_to_u64(pri_entry->src_wwpn);
			if (wwn2 == (uint64_t)-1)
				wwn_match++;
			else if (wwn1 == wwn2)
				wwn_match++;
		}

		/* check destination WWN for a match */
		if (pri_entry->flags & FCP_PRIO_ENTRY_DWWN_VALID) {
			wwn1 = wwn_to_u64(fcport->port_name);
			wwn2 = wwn_to_u64(pri_entry->dst_wwpn);
			if (wwn2 == (uint64_t)-1)
				wwn_match++;
			else if (wwn1 == wwn2)
				wwn_match++;
		}

		if (pid_match == 2 || wwn_match == 2) {
			/* Found a matching entry */
			if (pri_entry->flags & FCP_PRIO_ENTRY_TAG_VALID)
				priority = pri_entry->tag;
			break;
		}

		pri_entry++;
	}

	return priority;
}

/*
 * qla24xx_update_fcport_fcp_prio
 *	Activates fcp priority for the logged in fc port
 *
 * Input:
 *	vha = scsi host structure pointer.
 *	fcp = port structure pointer.
 *
 * Return:
 *	QLA_SUCCESS or QLA_FUNCTION_FAILED
 *
 * Context:
 *	Kernel context.
 */
int
qla24xx_update_fcport_fcp_prio(scsi_qla_host_t *vha, fc_port_t *fcport)
{
	int ret;
	int priority;
	uint16_t mb[5];

	if (fcport->port_type != FCT_TARGET ||
	    fcport->loop_id == FC_NO_LOOP_ID)
		return QLA_FUNCTION_FAILED;

	priority = qla24xx_get_fcp_prio(vha, fcport);
	if (priority < 0)
		return QLA_FUNCTION_FAILED;

	if (IS_P3P_TYPE(vha->hw)) {
		fcport->fcp_prio = priority & 0xf;
		return QLA_SUCCESS;
	}

	ret = qla24xx_set_fcp_prio(vha, fcport->loop_id, priority, mb);
	if (ret == QLA_SUCCESS) {
		if (fcport->fcp_prio != priority)
			ql_dbg(ql_dbg_user, vha, 0x709e,
			    "Updated FCP_CMND priority - value=%d loop_id=%d "
			    "port_id=%02x%02x%02x.\n", priority,
			    fcport->loop_id, fcport->d_id.b.domain,
			    fcport->d_id.b.area, fcport->d_id.b.al_pa);
		fcport->fcp_prio = priority & 0xf;
	} else
		ql_dbg(ql_dbg_user, vha, 0x704f,
		    "Unable to update FCP_CMND priority - ret=0x%x for "
		    "loop_id=%d port_id=%02x%02x%02x.\n", ret, fcport->loop_id,
		    fcport->d_id.b.domain, fcport->d_id.b.area,
		    fcport->d_id.b.al_pa);
	return  ret;
}

/*
 * qla24xx_update_all_fcp_prio
 *	Activates fcp priority for all the logged in ports
 *
 * Input:
 *	ha = adapter block pointer.
 *
 * Return:
 *	QLA_SUCCESS or QLA_FUNCTION_FAILED
 *
 * Context:
 *	Kernel context.
 */
int
qla24xx_update_all_fcp_prio(scsi_qla_host_t *vha)
{
	int ret;
	fc_port_t *fcport;

	ret = QLA_FUNCTION_FAILED;
	/* We need to set priority for all logged in ports */
	list_for_each_entry(fcport, &vha->vp_fcports, list)
		ret = qla24xx_update_fcport_fcp_prio(vha, fcport);

	return ret;
}

struct qla_qpair *qla2xxx_create_qpair(struct scsi_qla_host *vha, int qos,
	int vp_idx, bool startqp)
{
	int rsp_id = 0;
	int  req_id = 0;
	int i;
	struct qla_hw_data *ha = vha->hw;
	uint16_t qpair_id = 0;
	struct qla_qpair *qpair = NULL;
	struct qla_msix_entry *msix;

	if (!(ha->fw_attributes & BIT_6) || !ha->flags.msix_enabled) {
		ql_log(ql_log_warn, vha, 0x00181,
		    "FW/Driver is not multi-queue capable.\n");
		return NULL;
	}

	if (ql2xmqsupport || ql2xnvmeenable) {
		qpair = kzalloc(sizeof(struct qla_qpair), GFP_KERNEL);
		if (qpair == NULL) {
			ql_log(ql_log_warn, vha, 0x0182,
			    "Failed to allocate memory for queue pair.\n");
			return NULL;
		}

		qpair->hw = vha->hw;
		qpair->vha = vha;
		qpair->qp_lock_ptr = &qpair->qp_lock;
		spin_lock_init(&qpair->qp_lock);
		qpair->use_shadow_reg = IS_SHADOW_REG_CAPABLE(ha) ? 1 : 0;

		/* Assign available que pair id */
		mutex_lock(&ha->mq_lock);
		qpair_id = find_first_zero_bit(ha->qpair_qid_map, ha->max_qpairs);
		if (ha->num_qpairs >= ha->max_qpairs) {
			mutex_unlock(&ha->mq_lock);
			ql_log(ql_log_warn, vha, 0x0183,
			    "No resources to create additional q pair.\n");
			goto fail_qid_map;
		}
		ha->num_qpairs++;
		set_bit(qpair_id, ha->qpair_qid_map);
		ha->queue_pair_map[qpair_id] = qpair;
		qpair->id = qpair_id;
		qpair->vp_idx = vp_idx;
		qpair->fw_started = ha->flags.fw_started;
		INIT_LIST_HEAD(&qpair->hints_list);
		qpair->chip_reset = ha->base_qpair->chip_reset;
		qpair->enable_class_2 = ha->base_qpair->enable_class_2;
		qpair->enable_explicit_conf =
		    ha->base_qpair->enable_explicit_conf;

		for (i = 0; i < ha->msix_count; i++) {
			msix = &ha->msix_entries[i];
			if (msix->in_use)
				continue;
			qpair->msix = msix;
			ql_dbg(ql_dbg_multiq, vha, 0xc00f,
			    "Vector %x selected for qpair\n", msix->vector);
			break;
		}
		if (!qpair->msix) {
			ql_log(ql_log_warn, vha, 0x0184,
			    "Out of MSI-X vectors!.\n");
			goto fail_msix;
		}

		qpair->msix->in_use = 1;
		list_add_tail(&qpair->qp_list_elem, &vha->qp_list);
		qpair->pdev = ha->pdev;
		if (IS_QLA27XX(ha) || IS_QLA83XX(ha) || IS_QLA28XX(ha))
			qpair->reqq_start_iocbs = qla_83xx_start_iocbs;

		mutex_unlock(&ha->mq_lock);

		/* Create response queue first */
		rsp_id = qla25xx_create_rsp_que(ha, 0, 0, 0, qpair, startqp);
		if (!rsp_id) {
			ql_log(ql_log_warn, vha, 0x0185,
			    "Failed to create response queue.\n");
			goto fail_rsp;
		}

		qpair->rsp = ha->rsp_q_map[rsp_id];

		/* Create request queue */
		req_id = qla25xx_create_req_que(ha, 0, vp_idx, 0, rsp_id, qos,
		    startqp);
		if (!req_id) {
			ql_log(ql_log_warn, vha, 0x0186,
			    "Failed to create request queue.\n");
			goto fail_req;
		}

		qpair->req = ha->req_q_map[req_id];
		qpair->rsp->req = qpair->req;
		qpair->rsp->qpair = qpair;
		/* init qpair to this cpu. Will adjust at run time. */
		qla_cpu_update(qpair, smp_processor_id());

		if (IS_T10_PI_CAPABLE(ha) && ql2xenabledif) {
			if (ha->fw_attributes & BIT_4)
				qpair->difdix_supported = 1;
		}

		qpair->srb_mempool = mempool_create_slab_pool(SRB_MIN_REQ, srb_cachep);
		if (!qpair->srb_mempool) {
			ql_log(ql_log_warn, vha, 0xd036,
			    "Failed to create srb mempool for qpair %d\n",
			    qpair->id);
			goto fail_mempool;
		}

		/* Mark as online */
		qpair->online = 1;

		if (!vha->flags.qpairs_available)
			vha->flags.qpairs_available = 1;

		ql_dbg(ql_dbg_multiq, vha, 0xc00d,
		    "Request/Response queue pair created, id %d\n",
		    qpair->id);
		ql_dbg(ql_dbg_init, vha, 0x0187,
		    "Request/Response queue pair created, id %d\n",
		    qpair->id);
	}
	return qpair;

fail_mempool:
fail_req:
	qla25xx_delete_rsp_que(vha, qpair->rsp);
fail_rsp:
	mutex_lock(&ha->mq_lock);
	qpair->msix->in_use = 0;
	list_del(&qpair->qp_list_elem);
	if (list_empty(&vha->qp_list))
		vha->flags.qpairs_available = 0;
fail_msix:
	ha->queue_pair_map[qpair_id] = NULL;
	clear_bit(qpair_id, ha->qpair_qid_map);
	ha->num_qpairs--;
	mutex_unlock(&ha->mq_lock);
fail_qid_map:
	kfree(qpair);
	return NULL;
}

int qla2xxx_delete_qpair(struct scsi_qla_host *vha, struct qla_qpair *qpair)
{
	int ret = QLA_FUNCTION_FAILED;
	struct qla_hw_data *ha = qpair->hw;

	qpair->delete_in_progress = 1;

	ret = qla25xx_delete_req_que(vha, qpair->req);
	if (ret != QLA_SUCCESS)
		goto fail;

	ret = qla25xx_delete_rsp_que(vha, qpair->rsp);
	if (ret != QLA_SUCCESS)
		goto fail;

	mutex_lock(&ha->mq_lock);
	ha->queue_pair_map[qpair->id] = NULL;
	clear_bit(qpair->id, ha->qpair_qid_map);
	ha->num_qpairs--;
	list_del(&qpair->qp_list_elem);
	if (list_empty(&vha->qp_list)) {
		vha->flags.qpairs_available = 0;
		vha->flags.qpairs_req_created = 0;
		vha->flags.qpairs_rsp_created = 0;
	}
	mempool_destroy(qpair->srb_mempool);
	kfree(qpair);
	mutex_unlock(&ha->mq_lock);

	return QLA_SUCCESS;
fail:
	return ret;
}

uint64_t
qla2x00_count_set_bits(uint32_t num)
{
	/* Brian Kernighan's Algorithm */
	u64 count = 0;

	while (num) {
		num &= (num - 1);
		count++;
	}
	return count;
}

uint64_t
qla2x00_get_num_tgts(scsi_qla_host_t *vha)
{
	fc_port_t *f, *tf;
	u64 count = 0;

	f = NULL;
	tf = NULL;

	list_for_each_entry_safe(f, tf, &vha->vp_fcports, list) {
		if (f->port_type != FCT_TARGET)
			continue;
		count++;
	}
	return count;
}

int qla2xxx_reset_stats(struct Scsi_Host *host, u32 flags)
{
	scsi_qla_host_t *vha = shost_priv(host);
	fc_port_t *fcport = NULL;
	unsigned long int_flags;

	if (flags & QLA2XX_HW_ERROR)
		vha->hw_err_cnt = 0;
	if (flags & QLA2XX_SHT_LNK_DWN)
		vha->short_link_down_cnt = 0;
	if (flags & QLA2XX_INT_ERR)
		vha->interface_err_cnt = 0;
	if (flags & QLA2XX_CMD_TIMEOUT)
		vha->cmd_timeout_cnt = 0;
	if (flags & QLA2XX_RESET_CMD_ERR)
		vha->reset_cmd_err_cnt = 0;
	if (flags & QLA2XX_TGT_SHT_LNK_DOWN) {
		spin_lock_irqsave(&vha->hw->tgt.sess_lock, int_flags);
		list_for_each_entry(fcport, &vha->vp_fcports, list) {
			fcport->tgt_short_link_down_cnt = 0;
			fcport->tgt_link_down_time = QLA2XX_MAX_LINK_DOWN_TIME;
		}
		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, int_flags);
	}
	vha->link_down_time = QLA2XX_MAX_LINK_DOWN_TIME;
	return 0;
}

int qla2xxx_start_stats(struct Scsi_Host *host, u32 flags)
{
	return qla2xxx_reset_stats(host, flags);
}

int qla2xxx_stop_stats(struct Scsi_Host *host, u32 flags)
{
	return qla2xxx_reset_stats(host, flags);
}

int qla2xxx_get_ini_stats(struct Scsi_Host *host, u32 flags,
			  void *data, u64 size)
{
	scsi_qla_host_t *vha = shost_priv(host);
	struct ql_vnd_host_stats_resp *resp = (struct ql_vnd_host_stats_resp *)data;
	struct ql_vnd_stats *rsp_data = &resp->stats;
	u64 ini_entry_count = 0;
	u64 i = 0;
	u64 entry_count = 0;
	u64 num_tgt = 0;
	u32 tmp_stat_type = 0;
	fc_port_t *fcport = NULL;
	unsigned long int_flags;

	/* Copy stat type to work on it */
	tmp_stat_type = flags;

	if (tmp_stat_type & BIT_17) {
		num_tgt = qla2x00_get_num_tgts(vha);
		/* unset BIT_17 */
		tmp_stat_type &= ~(1 << 17);
	}
	ini_entry_count = qla2x00_count_set_bits(tmp_stat_type);

	entry_count = ini_entry_count + num_tgt;

	rsp_data->entry_count = entry_count;

	i = 0;
	if (flags & QLA2XX_HW_ERROR) {
		rsp_data->entry[i].stat_type = QLA2XX_HW_ERROR;
		rsp_data->entry[i].tgt_num = 0x0;
		rsp_data->entry[i].cnt = vha->hw_err_cnt;
		i++;
	}

	if (flags & QLA2XX_SHT_LNK_DWN) {
		rsp_data->entry[i].stat_type = QLA2XX_SHT_LNK_DWN;
		rsp_data->entry[i].tgt_num = 0x0;
		rsp_data->entry[i].cnt = vha->short_link_down_cnt;
		i++;
	}

	if (flags & QLA2XX_INT_ERR) {
		rsp_data->entry[i].stat_type = QLA2XX_INT_ERR;
		rsp_data->entry[i].tgt_num = 0x0;
		rsp_data->entry[i].cnt = vha->interface_err_cnt;
		i++;
	}

	if (flags & QLA2XX_CMD_TIMEOUT) {
		rsp_data->entry[i].stat_type = QLA2XX_CMD_TIMEOUT;
		rsp_data->entry[i].tgt_num = 0x0;
		rsp_data->entry[i].cnt = vha->cmd_timeout_cnt;
		i++;
	}

	if (flags & QLA2XX_RESET_CMD_ERR) {
		rsp_data->entry[i].stat_type = QLA2XX_RESET_CMD_ERR;
		rsp_data->entry[i].tgt_num = 0x0;
		rsp_data->entry[i].cnt = vha->reset_cmd_err_cnt;
		i++;
	}

	/* i will continue from previous loop, as target
	 * entries are after initiator
	 */
	if (flags & QLA2XX_TGT_SHT_LNK_DOWN) {
		spin_lock_irqsave(&vha->hw->tgt.sess_lock, int_flags);
		list_for_each_entry(fcport, &vha->vp_fcports, list) {
			if (fcport->port_type != FCT_TARGET)
				continue;
			if (!fcport->rport)
				continue;
			rsp_data->entry[i].stat_type = QLA2XX_TGT_SHT_LNK_DOWN;
			rsp_data->entry[i].tgt_num = fcport->rport->number;
			rsp_data->entry[i].cnt = fcport->tgt_short_link_down_cnt;
			i++;
		}
		spin_unlock_irqrestore(&vha->hw->tgt.sess_lock, int_flags);
	}
	resp->status = EXT_STATUS_OK;

	return 0;
}

int qla2xxx_get_tgt_stats(struct Scsi_Host *host, u32 flags,
			  struct fc_rport *rport, void *data, u64 size)
{
	struct ql_vnd_tgt_stats_resp *tgt_data = data;
	fc_port_t *fcport = *(fc_port_t **)rport->dd_data;

	tgt_data->status = 0;
	tgt_data->stats.entry_count = 1;
	tgt_data->stats.entry[0].stat_type = flags;
	tgt_data->stats.entry[0].tgt_num = rport->number;
	tgt_data->stats.entry[0].cnt = fcport->tgt_short_link_down_cnt;

	return 0;
}

int qla2xxx_disable_port(struct Scsi_Host *host)
{
	scsi_qla_host_t *vha = shost_priv(host);

	vha->hw->flags.port_isolated = 1;

	if (qla2x00_chip_is_down(vha))
		return 0;

	if (vha->flags.online) {
		qla2x00_abort_isp_cleanup(vha);
		qla2x00_wait_for_sess_deletion(vha);
	}

	return 0;
}

int qla2xxx_enable_port(struct Scsi_Host *host)
{
	scsi_qla_host_t *vha = shost_priv(host);

	vha->hw->flags.port_isolated = 0;
	/* Set the flag to 1, so that isp_abort can proceed */
	vha->flags.online = 1;
	set_bit(ISP_ABORT_NEEDED, &vha->dpc_flags);
	qla2xxx_wake_dpc(vha);

	return 0;
}
