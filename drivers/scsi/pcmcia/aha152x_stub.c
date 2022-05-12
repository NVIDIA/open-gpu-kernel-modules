/*======================================================================

    A driver for Adaptec AHA152X-compatible PCMCIA SCSI cards.

    This driver supports the Adaptec AHA-1460, the New Media Bus
    Toaster, and the New Media Toast & Jam.
    
    aha152x_cs.c 1.54 2000/06/12 21:27:25

    The contents of this file are subject to the Mozilla Public
    License Version 1.1 (the "License"); you may not use this file
    except in compliance with the License. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

    Software distributed under the License is distributed on an "AS
    IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
    implied. See the License for the specific language governing
    rights and limitations under the License.

    The initial developer of the original code is David A. Hinds
    <dahinds@users.sourceforge.net>.  Portions created by David A. Hinds
    are Copyright (C) 1999 David A. Hinds.  All Rights Reserved.

    Alternatively, the contents of this file may be used under the
    terms of the GNU General Public License version 2 (the "GPL"), in which
    case the provisions of the GPL are applicable instead of the
    above.  If you wish to allow the use of your version of this file
    only under the terms of the GPL and not to allow others to use
    your version of this file under the MPL, indicate your decision
    by deleting the provisions above and replace them with the notice
    and other provisions required by the GPL.  If you do not delete
    the provisions above, a recipient may use your version of this
    file under either the MPL or the GPL.
    
======================================================================*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <scsi/scsi.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <scsi/scsi_ioctl.h>

#include "scsi.h"
#include <scsi/scsi_host.h>
#include "aha152x.h"

#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>


/*====================================================================*/

/* Parameters that can be set with 'insmod' */

/* SCSI bus setup options */
static int host_id = 7;
static int reconnect = 1;
static int parity = 1;
static int synchronous = 1;
static int reset_delay = 100;
static int ext_trans = 0;

module_param(host_id, int, 0);
module_param(reconnect, int, 0);
module_param(parity, int, 0);
module_param(synchronous, int, 0);
module_param(reset_delay, int, 0);
module_param(ext_trans, int, 0);

MODULE_LICENSE("Dual MPL/GPL");

/*====================================================================*/

typedef struct scsi_info_t {
	struct pcmcia_device	*p_dev;
    struct Scsi_Host	*host;
} scsi_info_t;

static void aha152x_release_cs(struct pcmcia_device *link);
static void aha152x_detach(struct pcmcia_device *p_dev);
static int aha152x_config_cs(struct pcmcia_device *link);

static int aha152x_probe(struct pcmcia_device *link)
{
    scsi_info_t *info;

    dev_dbg(&link->dev, "aha152x_attach()\n");

    /* Create new SCSI device */
    info = kzalloc(sizeof(*info), GFP_KERNEL);
    if (!info) return -ENOMEM;
    info->p_dev = link;
    link->priv = info;

    link->config_flags |= CONF_ENABLE_IRQ | CONF_AUTO_SET_IO;
    link->config_regs = PRESENT_OPTION;

    return aha152x_config_cs(link);
} /* aha152x_attach */

/*====================================================================*/

static void aha152x_detach(struct pcmcia_device *link)
{
    dev_dbg(&link->dev, "aha152x_detach\n");

    aha152x_release_cs(link);

    /* Unlink device structure, free bits */
    kfree(link->priv);
} /* aha152x_detach */

/*====================================================================*/

static int aha152x_config_check(struct pcmcia_device *p_dev, void *priv_data)
{
	p_dev->io_lines = 10;

	/* For New Media T&J, look for a SCSI window */
	if ((p_dev->resource[0]->end < 0x20) &&
		(p_dev->resource[1]->end >= 0x20))
		p_dev->resource[0]->start = p_dev->resource[1]->start;

	if (p_dev->resource[0]->start >= 0xffff)
		return -EINVAL;

	p_dev->resource[1]->start = p_dev->resource[1]->end = 0;
	p_dev->resource[0]->end = 0x20;
	p_dev->resource[0]->flags &= ~IO_DATA_PATH_WIDTH;
	p_dev->resource[0]->flags |= IO_DATA_PATH_WIDTH_AUTO;

	return pcmcia_request_io(p_dev);
}

static int aha152x_config_cs(struct pcmcia_device *link)
{
    scsi_info_t *info = link->priv;
    struct aha152x_setup s;
    int ret;
    struct Scsi_Host *host;

    dev_dbg(&link->dev, "aha152x_config\n");

    ret = pcmcia_loop_config(link, aha152x_config_check, NULL);
    if (ret)
	    goto failed;

    if (!link->irq)
	    goto failed;

    ret = pcmcia_enable_device(link);
    if (ret)
	    goto failed;
    
    /* Set configuration options for the aha152x driver */
    memset(&s, 0, sizeof(s));
    s.conf        = "PCMCIA setup";
    s.io_port     = link->resource[0]->start;
    s.irq         = link->irq;
    s.scsiid      = host_id;
    s.reconnect   = reconnect;
    s.parity      = parity;
    s.synchronous = synchronous;
    s.delay       = reset_delay;
    if (ext_trans)
        s.ext_trans = ext_trans;

    host = aha152x_probe_one(&s);
    if (host == NULL) {
	printk(KERN_INFO "aha152x_cs: no SCSI devices found\n");
	goto failed;
    }

    info->host = host;

    return 0;

failed:
    aha152x_release_cs(link);
    return -ENODEV;
}

static void aha152x_release_cs(struct pcmcia_device *link)
{
	scsi_info_t *info = link->priv;

	aha152x_release(info->host);
	pcmcia_disable_device(link);
}

static int aha152x_resume(struct pcmcia_device *link)
{
	scsi_info_t *info = link->priv;

	aha152x_host_reset_host(info->host);

	return 0;
}

static const struct pcmcia_device_id aha152x_ids[] = {
	PCMCIA_DEVICE_PROD_ID123("New Media", "SCSI", "Bus Toaster", 0xcdf7e4cc, 0x35f26476, 0xa8851d6e),
	PCMCIA_DEVICE_PROD_ID123("NOTEWORTHY", "SCSI", "Bus Toaster", 0xad89c6e8, 0x35f26476, 0xa8851d6e),
	PCMCIA_DEVICE_PROD_ID12("Adaptec, Inc.", "APA-1460 SCSI Host Adapter", 0x24ba9738, 0x3a3c3d20),
	PCMCIA_DEVICE_PROD_ID12("New Media Corporation", "Multimedia Sound/SCSI", 0x085a850b, 0x80a6535c),
	PCMCIA_DEVICE_PROD_ID12("NOTEWORTHY", "NWCOMB02 SCSI/AUDIO COMBO CARD", 0xad89c6e8, 0x5f9a615b),
	PCMCIA_DEVICE_NULL,
};
MODULE_DEVICE_TABLE(pcmcia, aha152x_ids);

static struct pcmcia_driver aha152x_cs_driver = {
	.owner		= THIS_MODULE,
	.name		= "aha152x_cs",
	.probe		= aha152x_probe,
	.remove		= aha152x_detach,
	.id_table       = aha152x_ids,
	.resume		= aha152x_resume,
};
module_pcmcia_driver(aha152x_cs_driver);
