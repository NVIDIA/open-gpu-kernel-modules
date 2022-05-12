// SPDX-License-Identifier: GPL-2.0-only
/* SF16-FMI, SF16-FMP and SF16-FMD radio driver for Linux radio support
 * heavily based on rtrack driver...
 * (c) 1997 M. Kirkwood
 * (c) 1998 Petr Vandrovec, vandrove@vc.cvut.cz
 *
 * Fitted to new interface by Alan Cox <alan@lxorguk.ukuu.org.uk>
 * Made working and cleaned up functions <mikael.hedin@irf.se>
 * Support for ISAPnP by Ladislav Michl <ladis@psi.cz>
 *
 * Notes on the hardware
 *
 *  Frequency control is done digitally -- ie out(port,encodefreq(95.8));
 *  No volume control - only mute/unmute - you have to use line volume
 *  control on SB-part of SF16-FMI/SF16-FMP/SF16-FMD
 *
 * Converted to V4L2 API by Mauro Carvalho Chehab <mchehab@kernel.org>
 */

#include <linux/kernel.h>	/* __setup			*/
#include <linux/module.h>	/* Modules			*/
#include <linux/init.h>		/* Initdata			*/
#include <linux/ioport.h>	/* request_region		*/
#include <linux/delay.h>	/* udelay			*/
#include <linux/isapnp.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>	/* kernel radio structs		*/
#include <linux/io.h>		/* outb, outb_p			*/
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include "lm7000.h"

MODULE_AUTHOR("Petr Vandrovec, vandrove@vc.cvut.cz and M. Kirkwood");
MODULE_DESCRIPTION("A driver for the SF16-FMI, SF16-FMP and SF16-FMD radio.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.3");

static int io = -1;
static int radio_nr = -1;

module_param(io, int, 0);
MODULE_PARM_DESC(io, "I/O address of the SF16-FMI/SF16-FMP/SF16-FMD card (0x284 or 0x384)");
module_param(radio_nr, int, 0);

struct fmi
{
	struct v4l2_device v4l2_dev;
	struct v4l2_ctrl_handler hdl;
	struct video_device vdev;
	int io;
	bool mute;
	u32 curfreq; /* freq in kHz */
	struct mutex lock;
};

static struct fmi fmi_card;
static struct pnp_dev *dev;
static bool pnp_attached;

#define RSF16_MINFREQ (87U * 16000)
#define RSF16_MAXFREQ (108U * 16000)

#define FMI_BIT_TUN_CE		(1 << 0)
#define FMI_BIT_TUN_CLK		(1 << 1)
#define FMI_BIT_TUN_DATA	(1 << 2)
#define FMI_BIT_VOL_SW		(1 << 3)
#define FMI_BIT_TUN_STRQ	(1 << 4)

static void fmi_set_pins(void *handle, u8 pins)
{
	struct fmi *fmi = handle;
	u8 bits = FMI_BIT_TUN_STRQ;

	if (!fmi->mute)
		bits |= FMI_BIT_VOL_SW;

	if (pins & LM7000_DATA)
		bits |= FMI_BIT_TUN_DATA;
	if (pins & LM7000_CLK)
		bits |= FMI_BIT_TUN_CLK;
	if (pins & LM7000_CE)
		bits |= FMI_BIT_TUN_CE;

	mutex_lock(&fmi->lock);
	outb_p(bits, fmi->io);
	mutex_unlock(&fmi->lock);
}

static inline void fmi_mute(struct fmi *fmi)
{
	mutex_lock(&fmi->lock);
	outb(0x00, fmi->io);
	mutex_unlock(&fmi->lock);
}

static inline void fmi_unmute(struct fmi *fmi)
{
	mutex_lock(&fmi->lock);
	outb(0x08, fmi->io);
	mutex_unlock(&fmi->lock);
}

static inline int fmi_getsigstr(struct fmi *fmi)
{
	int val;
	int res;

	mutex_lock(&fmi->lock);
	val = fmi->mute ? 0x00 : 0x08;	/* mute/unmute */
	outb(val, fmi->io);
	outb(val | 0x10, fmi->io);
	msleep(143);		/* was schedule_timeout(HZ/7) */
	res = (int)inb(fmi->io + 1);
	outb(val, fmi->io);

	mutex_unlock(&fmi->lock);
	return (res & 2) ? 0 : 0xFFFF;
}

static void fmi_set_freq(struct fmi *fmi)
{
	fmi->curfreq = clamp(fmi->curfreq, RSF16_MINFREQ, RSF16_MAXFREQ);
	/* rounding in steps of 800 to match the freq
	   that will be used */
	lm7000_set_freq((fmi->curfreq / 800) * 800, fmi, fmi_set_pins);
}

static int vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *v)
{
	strscpy(v->driver, "radio-sf16fmi", sizeof(v->driver));
	strscpy(v->card, "SF16-FMI/FMP/FMD radio", sizeof(v->card));
	strscpy(v->bus_info, "ISA:radio-sf16fmi", sizeof(v->bus_info));
	return 0;
}

static int vidioc_g_tuner(struct file *file, void *priv,
					struct v4l2_tuner *v)
{
	struct fmi *fmi = video_drvdata(file);

	if (v->index > 0)
		return -EINVAL;

	strscpy(v->name, "FM", sizeof(v->name));
	v->type = V4L2_TUNER_RADIO;
	v->rangelow = RSF16_MINFREQ;
	v->rangehigh = RSF16_MAXFREQ;
	v->rxsubchans = V4L2_TUNER_SUB_MONO | V4L2_TUNER_SUB_STEREO;
	v->capability = V4L2_TUNER_CAP_STEREO | V4L2_TUNER_CAP_LOW;
	v->audmode = V4L2_TUNER_MODE_STEREO;
	v->signal = fmi_getsigstr(fmi);
	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
					const struct v4l2_tuner *v)
{
	return v->index ? -EINVAL : 0;
}

static int vidioc_s_frequency(struct file *file, void *priv,
					const struct v4l2_frequency *f)
{
	struct fmi *fmi = video_drvdata(file);

	if (f->tuner != 0 || f->type != V4L2_TUNER_RADIO)
		return -EINVAL;

	fmi->curfreq = f->frequency;
	fmi_set_freq(fmi);

	return 0;
}

static int vidioc_g_frequency(struct file *file, void *priv,
					struct v4l2_frequency *f)
{
	struct fmi *fmi = video_drvdata(file);

	if (f->tuner != 0)
		return -EINVAL;
	f->type = V4L2_TUNER_RADIO;
	f->frequency = fmi->curfreq;
	return 0;
}

static int fmi_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct fmi *fmi = container_of(ctrl->handler, struct fmi, hdl);

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		if (ctrl->val)
			fmi_mute(fmi);
		else
			fmi_unmute(fmi);
		fmi->mute = ctrl->val;
		return 0;
	}
	return -EINVAL;
}

static const struct v4l2_ctrl_ops fmi_ctrl_ops = {
	.s_ctrl = fmi_s_ctrl,
};

static const struct v4l2_file_operations fmi_fops = {
	.owner		= THIS_MODULE,
	.open		= v4l2_fh_open,
	.release	= v4l2_fh_release,
	.poll		= v4l2_ctrl_poll,
	.unlocked_ioctl	= video_ioctl2,
};

static const struct v4l2_ioctl_ops fmi_ioctl_ops = {
	.vidioc_querycap    = vidioc_querycap,
	.vidioc_g_tuner     = vidioc_g_tuner,
	.vidioc_s_tuner     = vidioc_s_tuner,
	.vidioc_g_frequency = vidioc_g_frequency,
	.vidioc_s_frequency = vidioc_s_frequency,
	.vidioc_log_status  = v4l2_ctrl_log_status,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

/* ladis: this is my card. does any other types exist? */
static struct isapnp_device_id id_table[] = {
		/* SF16-FMI */
	{	ISAPNP_ANY_ID, ISAPNP_ANY_ID,
		ISAPNP_VENDOR('M','F','R'), ISAPNP_FUNCTION(0xad10), 0},
		/* SF16-FMD */
	{	ISAPNP_ANY_ID, ISAPNP_ANY_ID,
		ISAPNP_VENDOR('M','F','R'), ISAPNP_FUNCTION(0xad12), 0},
	{	ISAPNP_CARD_END, },
};

MODULE_DEVICE_TABLE(isapnp, id_table);

static int __init isapnp_fmi_probe(void)
{
	int i = 0;

	while (id_table[i].card_vendor != 0 && dev == NULL) {
		dev = pnp_find_dev(NULL, id_table[i].vendor,
				   id_table[i].function, NULL);
		i++;
	}

	if (!dev)
		return -ENODEV;
	if (pnp_device_attach(dev) < 0)
		return -EAGAIN;
	if (pnp_activate_dev(dev) < 0) {
		printk(KERN_ERR "radio-sf16fmi: PnP configure failed (out of resources?)\n");
		pnp_device_detach(dev);
		return -ENOMEM;
	}
	if (!pnp_port_valid(dev, 0)) {
		pnp_device_detach(dev);
		return -ENODEV;
	}

	i = pnp_port_start(dev, 0);
	printk(KERN_INFO "radio-sf16fmi: PnP reports card at %#x\n", i);

	return i;
}

static int __init fmi_init(void)
{
	struct fmi *fmi = &fmi_card;
	struct v4l2_device *v4l2_dev = &fmi->v4l2_dev;
	struct v4l2_ctrl_handler *hdl = &fmi->hdl;
	int res, i;
	int probe_ports[] = { 0, 0x284, 0x384 };

	if (io < 0) {
		for (i = 0; i < ARRAY_SIZE(probe_ports); i++) {
			io = probe_ports[i];
			if (io == 0) {
				io = isapnp_fmi_probe();
				if (io < 0)
					continue;
				pnp_attached = true;
			}
			if (!request_region(io, 2, "radio-sf16fmi")) {
				if (pnp_attached)
					pnp_device_detach(dev);
				io = -1;
				continue;
			}
			if (pnp_attached ||
			    ((inb(io) & 0xf9) == 0xf9 && (inb(io) & 0x4) == 0))
				break;
			release_region(io, 2);
			io = -1;
		}
	} else {
		if (!request_region(io, 2, "radio-sf16fmi")) {
			printk(KERN_ERR "radio-sf16fmi: port %#x already in use\n", io);
			return -EBUSY;
		}
		if (inb(io) == 0xff) {
			printk(KERN_ERR "radio-sf16fmi: card not present at %#x\n", io);
			release_region(io, 2);
			return -ENODEV;
		}
	}
	if (io < 0) {
		printk(KERN_ERR "radio-sf16fmi: no cards found\n");
		return -ENODEV;
	}

	strscpy(v4l2_dev->name, "sf16fmi", sizeof(v4l2_dev->name));
	fmi->io = io;

	res = v4l2_device_register(NULL, v4l2_dev);
	if (res < 0) {
		release_region(fmi->io, 2);
		if (pnp_attached)
			pnp_device_detach(dev);
		v4l2_err(v4l2_dev, "Could not register v4l2_device\n");
		return res;
	}

	v4l2_ctrl_handler_init(hdl, 1);
	v4l2_ctrl_new_std(hdl, &fmi_ctrl_ops,
			V4L2_CID_AUDIO_MUTE, 0, 1, 1, 1);
	v4l2_dev->ctrl_handler = hdl;
	if (hdl->error) {
		res = hdl->error;
		v4l2_err(v4l2_dev, "Could not register controls\n");
		v4l2_ctrl_handler_free(hdl);
		v4l2_device_unregister(v4l2_dev);
		return res;
	}

	strscpy(fmi->vdev.name, v4l2_dev->name, sizeof(fmi->vdev.name));
	fmi->vdev.v4l2_dev = v4l2_dev;
	fmi->vdev.fops = &fmi_fops;
	fmi->vdev.ioctl_ops = &fmi_ioctl_ops;
	fmi->vdev.release = video_device_release_empty;
	fmi->vdev.device_caps = V4L2_CAP_TUNER | V4L2_CAP_RADIO;
	video_set_drvdata(&fmi->vdev, fmi);

	mutex_init(&fmi->lock);

	/* mute card and set default frequency */
	fmi->mute = true;
	fmi->curfreq = RSF16_MINFREQ;
	fmi_set_freq(fmi);

	if (video_register_device(&fmi->vdev, VFL_TYPE_RADIO, radio_nr) < 0) {
		v4l2_ctrl_handler_free(hdl);
		v4l2_device_unregister(v4l2_dev);
		release_region(fmi->io, 2);
		if (pnp_attached)
			pnp_device_detach(dev);
		return -EINVAL;
	}

	v4l2_info(v4l2_dev, "card driver at 0x%x\n", fmi->io);
	return 0;
}

static void __exit fmi_exit(void)
{
	struct fmi *fmi = &fmi_card;

	v4l2_ctrl_handler_free(&fmi->hdl);
	video_unregister_device(&fmi->vdev);
	v4l2_device_unregister(&fmi->v4l2_dev);
	release_region(fmi->io, 2);
	if (dev && pnp_attached)
		pnp_device_detach(dev);
}

module_init(fmi_init);
module_exit(fmi_exit);
