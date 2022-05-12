// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Target driver for EMC CLARiiON AX/CX-series hardware.
 * Based on code from Lars Marowsky-Bree <lmb@suse.de>
 * and Ed Goggin <egoggin@emc.com>.
 *
 * Copyright (C) 2006 Red Hat, Inc.  All rights reserved.
 * Copyright (C) 2006 Mike Christie
 */
#include <linux/slab.h>
#include <linux/module.h>
#include <scsi/scsi.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_dh.h>
#include <scsi/scsi_device.h>

#define CLARIION_NAME			"emc"

#define CLARIION_TRESPASS_PAGE		0x22
#define CLARIION_BUFFER_SIZE		0xFC
#define CLARIION_TIMEOUT		(60 * HZ)
#define CLARIION_RETRIES		3
#define CLARIION_UNBOUND_LU		-1
#define CLARIION_SP_A			0
#define CLARIION_SP_B			1

/* Flags */
#define CLARIION_SHORT_TRESPASS		1
#define CLARIION_HONOR_RESERVATIONS	2

/* LUN states */
#define CLARIION_LUN_UNINITIALIZED	-1
#define CLARIION_LUN_UNBOUND		0
#define CLARIION_LUN_BOUND		1
#define CLARIION_LUN_OWNED		2

static unsigned char long_trespass[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	CLARIION_TRESPASS_PAGE,	/* Page code */
	0x09,			/* Page length - 2 */
	0x01,			/* Trespass code */
	0xff, 0xff,		/* Trespass target */
	0, 0, 0, 0, 0, 0	/* Reserved bytes / unknown */
};

static unsigned char short_trespass[] = {
	0, 0, 0, 0,
	CLARIION_TRESPASS_PAGE,	/* Page code */
	0x02,			/* Page length - 2 */
	0x01,			/* Trespass code */
	0xff,			/* Trespass target */
};

static const char * lun_state[] =
{
    "not bound",
    "bound",
    "owned",
};

struct clariion_dh_data {
	/*
	 * Flags:
	 *  CLARIION_SHORT_TRESPASS
	 * Use short trespass command (FC-series) or the long version
	 * (default for AX/CX CLARiiON arrays).
	 *
	 *  CLARIION_HONOR_RESERVATIONS
	 * Whether or not (default) to honor SCSI reservations when
	 * initiating a switch-over.
	 */
	unsigned flags;
	/*
	 * I/O buffer for both MODE_SELECT and INQUIRY commands.
	 */
	unsigned char buffer[CLARIION_BUFFER_SIZE];
	/*
	 * LUN state
	 */
	int lun_state;
	/*
	 * SP Port number
	 */
	int port;
	/*
	 * which SP (A=0,B=1,UNBOUND=-1) is the default SP for this
	 * path's mapped LUN
	 */
	int default_sp;
	/*
	 * which SP (A=0,B=1,UNBOUND=-1) is the active SP for this
	 * path's mapped LUN
	 */
	int current_sp;
};

/*
 * Parse MODE_SELECT cmd reply.
 */
static int trespass_endio(struct scsi_device *sdev,
			  struct scsi_sense_hdr *sshdr)
{
	int err = SCSI_DH_IO;

	sdev_printk(KERN_ERR, sdev, "%s: Found valid sense data 0x%2x, "
		    "0x%2x, 0x%2x while sending CLARiiON trespass "
		    "command.\n", CLARIION_NAME, sshdr->sense_key,
		    sshdr->asc, sshdr->ascq);

	if (sshdr->sense_key == 0x05 && sshdr->asc == 0x04 &&
	    sshdr->ascq == 0x00) {
		/*
		 * Array based copy in progress -- do not send
		 * mode_select or copy will be aborted mid-stream.
		 */
		sdev_printk(KERN_INFO, sdev, "%s: Array Based Copy in "
			    "progress while sending CLARiiON trespass "
			    "command.\n", CLARIION_NAME);
		err = SCSI_DH_DEV_TEMP_BUSY;
	} else if (sshdr->sense_key == 0x02 && sshdr->asc == 0x04 &&
		   sshdr->ascq == 0x03) {
		/*
		 * LUN Not Ready - Manual Intervention Required
		 * indicates in-progress ucode upgrade (NDU).
		 */
		sdev_printk(KERN_INFO, sdev, "%s: Detected in-progress "
			    "ucode upgrade NDU operation while sending "
			    "CLARiiON trespass command.\n", CLARIION_NAME);
		err = SCSI_DH_DEV_TEMP_BUSY;
	} else
		err = SCSI_DH_DEV_FAILED;
	return err;
}

static int parse_sp_info_reply(struct scsi_device *sdev,
			       struct clariion_dh_data *csdev)
{
	int err = SCSI_DH_OK;

	/* check for in-progress ucode upgrade (NDU) */
	if (csdev->buffer[48] != 0) {
		sdev_printk(KERN_NOTICE, sdev, "%s: Detected in-progress "
			    "ucode upgrade NDU operation while finding "
			    "current active SP.", CLARIION_NAME);
		err = SCSI_DH_DEV_TEMP_BUSY;
		goto out;
	}
	if (csdev->buffer[4] > 2) {
		/* Invalid buffer format */
		sdev_printk(KERN_NOTICE, sdev,
			    "%s: invalid VPD page 0xC0 format\n",
			    CLARIION_NAME);
		err = SCSI_DH_NOSYS;
		goto out;
	}
	switch (csdev->buffer[28] & 0x0f) {
	case 6:
		sdev_printk(KERN_NOTICE, sdev,
			    "%s: ALUA failover mode detected\n",
			    CLARIION_NAME);
		break;
	case 4:
		/* Linux failover */
		break;
	default:
		sdev_printk(KERN_WARNING, sdev,
			    "%s: Invalid failover mode %d\n",
			    CLARIION_NAME, csdev->buffer[28] & 0x0f);
		err = SCSI_DH_NOSYS;
		goto out;
	}

	csdev->default_sp = csdev->buffer[5];
	csdev->lun_state = csdev->buffer[4];
	csdev->current_sp = csdev->buffer[8];
	csdev->port = csdev->buffer[7];
	if (csdev->lun_state == CLARIION_LUN_OWNED)
		sdev->access_state = SCSI_ACCESS_STATE_OPTIMAL;
	else
		sdev->access_state = SCSI_ACCESS_STATE_STANDBY;
	if (csdev->default_sp == csdev->current_sp)
		sdev->access_state |= SCSI_ACCESS_STATE_PREFERRED;
out:
	return err;
}

#define emc_default_str "FC (Legacy)"

static char * parse_sp_model(struct scsi_device *sdev, unsigned char *buffer)
{
	unsigned char len = buffer[4] + 5;
	char *sp_model = NULL;
	unsigned char sp_len, serial_len;

	if (len < 160) {
		sdev_printk(KERN_WARNING, sdev,
			    "%s: Invalid information section length %d\n",
			    CLARIION_NAME, len);
		/* Check for old FC arrays */
		if (!strncmp(buffer + 8, "DGC", 3)) {
			/* Old FC array, not supporting extended information */
			sp_model = emc_default_str;
		}
		goto out;
	}

	/*
	 * Parse extended information for SP model number
	 */
	serial_len = buffer[160];
	if (serial_len == 0 || serial_len + 161 > len) {
		sdev_printk(KERN_WARNING, sdev,
			    "%s: Invalid array serial number length %d\n",
			    CLARIION_NAME, serial_len);
		goto out;
	}
	sp_len = buffer[99];
	if (sp_len == 0 || serial_len + sp_len + 161 > len) {
		sdev_printk(KERN_WARNING, sdev,
			    "%s: Invalid model number length %d\n",
			    CLARIION_NAME, sp_len);
		goto out;
	}
	sp_model = &buffer[serial_len + 161];
	/* Strip whitespace at the end */
	while (sp_len > 1 && sp_model[sp_len - 1] == ' ')
		sp_len--;

	sp_model[sp_len] = '\0';

out:
	return sp_model;
}

static int send_trespass_cmd(struct scsi_device *sdev,
			    struct clariion_dh_data *csdev)
{
	unsigned char *page22;
	unsigned char cdb[MAX_COMMAND_SIZE];
	int err, res = SCSI_DH_OK, len;
	struct scsi_sense_hdr sshdr;
	u64 req_flags = REQ_FAILFAST_DEV | REQ_FAILFAST_TRANSPORT |
		REQ_FAILFAST_DRIVER;

	if (csdev->flags & CLARIION_SHORT_TRESPASS) {
		page22 = short_trespass;
		if (!(csdev->flags & CLARIION_HONOR_RESERVATIONS))
			/* Set Honor Reservations bit */
			page22[6] |= 0x80;
		len = sizeof(short_trespass);
		cdb[0] = MODE_SELECT;
		cdb[1] = 0x10;
		cdb[4] = len;
	} else {
		page22 = long_trespass;
		if (!(csdev->flags & CLARIION_HONOR_RESERVATIONS))
			/* Set Honor Reservations bit */
			page22[10] |= 0x80;
		len = sizeof(long_trespass);
		cdb[0] = MODE_SELECT_10;
		cdb[8] = len;
	}
	BUG_ON((len > CLARIION_BUFFER_SIZE));
	memcpy(csdev->buffer, page22, len);

	err = scsi_execute(sdev, cdb, DMA_TO_DEVICE, csdev->buffer, len, NULL,
			&sshdr, CLARIION_TIMEOUT * HZ, CLARIION_RETRIES,
			req_flags, 0, NULL);
	if (err) {
		if (scsi_sense_valid(&sshdr))
			res = trespass_endio(sdev, &sshdr);
		else {
			sdev_printk(KERN_INFO, sdev,
				    "%s: failed to send MODE SELECT: %x\n",
				    CLARIION_NAME, err);
			res = SCSI_DH_IO;
		}
	}

	return res;
}

static enum scsi_disposition clariion_check_sense(struct scsi_device *sdev,
					struct scsi_sense_hdr *sense_hdr)
{
	switch (sense_hdr->sense_key) {
	case NOT_READY:
		if (sense_hdr->asc == 0x04 && sense_hdr->ascq == 0x03)
			/*
			 * LUN Not Ready - Manual Intervention Required
			 * indicates this is a passive path.
			 *
			 * FIXME: However, if this is seen and EVPD C0
			 * indicates that this is due to a NDU in
			 * progress, we should set FAIL_PATH too.
			 * This indicates we might have to do a SCSI
			 * inquiry in the end_io path. Ugh.
			 *
			 * Can return FAILED only when we want the error
			 * recovery process to kick in.
			 */
			return SUCCESS;
		break;
	case ILLEGAL_REQUEST:
		if (sense_hdr->asc == 0x25 && sense_hdr->ascq == 0x01)
			/*
			 * An array based copy is in progress. Do not
			 * fail the path, do not bypass to another PG,
			 * do not retry. Fail the IO immediately.
			 * (Actually this is the same conclusion as in
			 * the default handler, but lets make sure.)
			 *
			 * Can return FAILED only when we want the error
			 * recovery process to kick in.
			 */
			return SUCCESS;
		break;
	case UNIT_ATTENTION:
		if (sense_hdr->asc == 0x29 && sense_hdr->ascq == 0x00)
			/*
			 * Unit Attention Code. This is the first IO
			 * to the new path, so just retry.
			 */
			return ADD_TO_MLQUEUE;
		break;
	}

	return SCSI_RETURN_NOT_HANDLED;
}

static blk_status_t clariion_prep_fn(struct scsi_device *sdev,
		struct request *req)
{
	struct clariion_dh_data *h = sdev->handler_data;

	if (h->lun_state != CLARIION_LUN_OWNED) {
		req->rq_flags |= RQF_QUIET;
		return BLK_STS_IOERR;
	}

	return BLK_STS_OK;
}

static int clariion_std_inquiry(struct scsi_device *sdev,
				struct clariion_dh_data *csdev)
{
	int err = SCSI_DH_OK;
	char *sp_model;

	sp_model = parse_sp_model(sdev, sdev->inquiry);
	if (!sp_model) {
		err = SCSI_DH_DEV_UNSUPP;
		goto out;
	}

	/*
	 * FC Series arrays do not support long trespass
	 */
	if (!strlen(sp_model) || !strncmp(sp_model, "FC",2))
		csdev->flags |= CLARIION_SHORT_TRESPASS;

	sdev_printk(KERN_INFO, sdev,
		    "%s: detected Clariion %s, flags %x\n",
		    CLARIION_NAME, sp_model, csdev->flags);
out:
	return err;
}

static int clariion_send_inquiry(struct scsi_device *sdev,
				 struct clariion_dh_data *csdev)
{
	int err = SCSI_DH_IO;

	if (!scsi_get_vpd_page(sdev, 0xC0, csdev->buffer,
			       CLARIION_BUFFER_SIZE))
		err = parse_sp_info_reply(sdev, csdev);

	return err;
}

static int clariion_activate(struct scsi_device *sdev,
				activate_complete fn, void *data)
{
	struct clariion_dh_data *csdev = sdev->handler_data;
	int result;

	result = clariion_send_inquiry(sdev, csdev);
	if (result != SCSI_DH_OK)
		goto done;

	if (csdev->lun_state == CLARIION_LUN_OWNED)
		goto done;

	result = send_trespass_cmd(sdev, csdev);
	if (result != SCSI_DH_OK)
		goto done;
	sdev_printk(KERN_INFO, sdev,"%s: %s trespass command sent\n",
		    CLARIION_NAME,
		    csdev->flags&CLARIION_SHORT_TRESPASS?"short":"long" );

	/* Update status */
	result = clariion_send_inquiry(sdev, csdev);
	if (result != SCSI_DH_OK)
		goto done;

done:
	sdev_printk(KERN_INFO, sdev,
		    "%s: at SP %c Port %d (%s, default SP %c)\n",
		    CLARIION_NAME, csdev->current_sp + 'A',
		    csdev->port, lun_state[csdev->lun_state],
		    csdev->default_sp + 'A');

	if (fn)
		fn(data, result);
	return 0;
}
/*
 * params - parameters in the following format
 *      "no_of_params\0param1\0param2\0param3\0...\0"
 *      for example, string for 2 parameters with value 10 and 21
 *      is specified as "2\010\021\0".
 */
static int clariion_set_params(struct scsi_device *sdev, const char *params)
{
	struct clariion_dh_data *csdev = sdev->handler_data;
	unsigned int hr = 0, st = 0, argc;
	const char *p = params;
	int result = SCSI_DH_OK;

	if ((sscanf(params, "%u", &argc) != 1) || (argc != 2))
		return -EINVAL;

	while (*p++)
		;
	if ((sscanf(p, "%u", &st) != 1) || (st > 1))
		return -EINVAL;

	while (*p++)
		;
	if ((sscanf(p, "%u", &hr) != 1) || (hr > 1))
		return -EINVAL;

	if (st)
		csdev->flags |= CLARIION_SHORT_TRESPASS;
	else
		csdev->flags &= ~CLARIION_SHORT_TRESPASS;

	if (hr)
		csdev->flags |= CLARIION_HONOR_RESERVATIONS;
	else
		csdev->flags &= ~CLARIION_HONOR_RESERVATIONS;

	/*
	 * If this path is owned, we have to send a trespass command
	 * with the new parameters. If not, simply return. Next trespass
	 * command would use the parameters.
	 */
	if (csdev->lun_state != CLARIION_LUN_OWNED)
		goto done;

	csdev->lun_state = CLARIION_LUN_UNINITIALIZED;
	result = send_trespass_cmd(sdev, csdev);
	if (result != SCSI_DH_OK)
		goto done;

	/* Update status */
	result = clariion_send_inquiry(sdev, csdev);

done:
	return result;
}

static int clariion_bus_attach(struct scsi_device *sdev)
{
	struct clariion_dh_data *h;
	int err;

	h = kzalloc(sizeof(*h) , GFP_KERNEL);
	if (!h)
		return SCSI_DH_NOMEM;
	h->lun_state = CLARIION_LUN_UNINITIALIZED;
	h->default_sp = CLARIION_UNBOUND_LU;
	h->current_sp = CLARIION_UNBOUND_LU;

	err = clariion_std_inquiry(sdev, h);
	if (err != SCSI_DH_OK)
		goto failed;

	err = clariion_send_inquiry(sdev, h);
	if (err != SCSI_DH_OK)
		goto failed;

	sdev_printk(KERN_INFO, sdev,
		    "%s: connected to SP %c Port %d (%s, default SP %c)\n",
		    CLARIION_NAME, h->current_sp + 'A',
		    h->port, lun_state[h->lun_state],
		    h->default_sp + 'A');

	sdev->handler_data = h;
	return SCSI_DH_OK;

failed:
	kfree(h);
	return err;
}

static void clariion_bus_detach(struct scsi_device *sdev)
{
	kfree(sdev->handler_data);
	sdev->handler_data = NULL;
}

static struct scsi_device_handler clariion_dh = {
	.name		= CLARIION_NAME,
	.module		= THIS_MODULE,
	.attach		= clariion_bus_attach,
	.detach		= clariion_bus_detach,
	.check_sense	= clariion_check_sense,
	.activate	= clariion_activate,
	.prep_fn	= clariion_prep_fn,
	.set_params	= clariion_set_params,
};

static int __init clariion_init(void)
{
	int r;

	r = scsi_register_device_handler(&clariion_dh);
	if (r != 0)
		printk(KERN_ERR "%s: Failed to register scsi device handler.",
			CLARIION_NAME);
	return r;
}

static void __exit clariion_exit(void)
{
	scsi_unregister_device_handler(&clariion_dh);
}

module_init(clariion_init);
module_exit(clariion_exit);

MODULE_DESCRIPTION("EMC CX/AX/FC-family driver");
MODULE_AUTHOR("Mike Christie <michaelc@cs.wisc.edu>, Chandra Seetharaman <sekharan@us.ibm.com>");
MODULE_LICENSE("GPL");
