// SPDX-License-Identifier: GPL-2.0-only
/*******************************************************************************
    AudioScience HPI driver
    Common Linux HPI ioctl and module probe/remove functions

    Copyright (C) 1997-2014  AudioScience Inc. <support@audioscience.com>


*******************************************************************************/
#define SOURCEFILE_NAME "hpioctl.c"

#include "hpi_internal.h"
#include "hpi_version.h"
#include "hpimsginit.h"
#include "hpidebug.h"
#include "hpimsgx.h"
#include "hpioctl.h"
#include "hpicmn.h"

#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/stringify.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/nospec.h>

#ifdef MODULE_FIRMWARE
MODULE_FIRMWARE("asihpi/dsp5000.bin");
MODULE_FIRMWARE("asihpi/dsp6200.bin");
MODULE_FIRMWARE("asihpi/dsp6205.bin");
MODULE_FIRMWARE("asihpi/dsp6400.bin");
MODULE_FIRMWARE("asihpi/dsp6600.bin");
MODULE_FIRMWARE("asihpi/dsp8700.bin");
MODULE_FIRMWARE("asihpi/dsp8900.bin");
#endif

static int prealloc_stream_buf;
module_param(prealloc_stream_buf, int, 0444);
MODULE_PARM_DESC(prealloc_stream_buf,
	"Preallocate size for per-adapter stream buffer");

/* Allow the debug level to be changed after module load.
 E.g.   echo 2 > /sys/module/asihpi/parameters/hpiDebugLevel
*/
module_param(hpi_debug_level, int, 0644);
MODULE_PARM_DESC(hpi_debug_level, "debug verbosity 0..5");

/* List of adapters found */
static struct hpi_adapter adapters[HPI_MAX_ADAPTERS];

/* Wrapper function to HPI_Message to enable dumping of the
   message and response types.
*/
static void hpi_send_recv_f(struct hpi_message *phm, struct hpi_response *phr,
	struct file *file)
{
	if ((phm->adapter_index >= HPI_MAX_ADAPTERS)
		&& (phm->object != HPI_OBJ_SUBSYSTEM))
		phr->error = HPI_ERROR_INVALID_OBJ_INDEX;
	else
		hpi_send_recv_ex(phm, phr, file);
}

/* This is called from hpifunc.c functions, called by ALSA
 * (or other kernel process) In this case there is no file descriptor
 * available for the message cache code
 */
void hpi_send_recv(struct hpi_message *phm, struct hpi_response *phr)
{
	hpi_send_recv_f(phm, phr, HOWNER_KERNEL);
}

EXPORT_SYMBOL(hpi_send_recv);
/* for radio-asihpi */

int asihpi_hpi_release(struct file *file)
{
	struct hpi_message hm;
	struct hpi_response hr;

/* HPI_DEBUG_LOG(INFO,"hpi_release file %p, pid %d\n", file, current->pid); */
	/* close the subsystem just in case the application forgot to. */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_SUBSYSTEM,
		HPI_SUBSYS_CLOSE);
	hpi_send_recv_ex(&hm, &hr, file);
	return 0;
}

long asihpi_hpi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct hpi_ioctl_linux __user *phpi_ioctl_data;
	void __user *puhm;
	void __user *puhr;
	union hpi_message_buffer_v1 *hm;
	union hpi_response_buffer_v1 *hr;
	u16 msg_size;
	u16 res_max_size;
	u32 uncopied_bytes;
	int err = 0;

	if (cmd != HPI_IOCTL_LINUX)
		return -EINVAL;

	hm = kmalloc(sizeof(*hm), GFP_KERNEL);
	hr = kzalloc(sizeof(*hr), GFP_KERNEL);
	if (!hm || !hr) {
		err = -ENOMEM;
		goto out;
	}

	phpi_ioctl_data = (struct hpi_ioctl_linux __user *)arg;

	/* Read the message and response pointers from user space.  */
	if (get_user(puhm, &phpi_ioctl_data->phm)
		|| get_user(puhr, &phpi_ioctl_data->phr)) {
		err = -EFAULT;
		goto out;
	}

	/* Now read the message size and data from user space.  */
	if (get_user(msg_size, (u16 __user *)puhm)) {
		err = -EFAULT;
		goto out;
	}
	if (msg_size > sizeof(*hm))
		msg_size = sizeof(*hm);

	/* printk(KERN_INFO "message size %d\n", hm->h.wSize); */

	uncopied_bytes = copy_from_user(hm, puhm, msg_size);
	if (uncopied_bytes) {
		HPI_DEBUG_LOG(ERROR, "uncopied bytes %d\n", uncopied_bytes);
		err = -EFAULT;
		goto out;
	}

	/* Override h.size in case it is changed between two userspace fetches */
	hm->h.size = msg_size;

	if (get_user(res_max_size, (u16 __user *)puhr)) {
		err = -EFAULT;
		goto out;
	}
	/* printk(KERN_INFO "user response size %d\n", res_max_size); */
	if (res_max_size < sizeof(struct hpi_response_header)) {
		HPI_DEBUG_LOG(WARNING, "small res size %d\n", res_max_size);
		err = -EFAULT;
		goto out;
	}

	res_max_size = min_t(size_t, res_max_size, sizeof(*hr));

	switch (hm->h.function) {
	case HPI_SUBSYS_CREATE_ADAPTER:
	case HPI_ADAPTER_DELETE:
		/* Application must not use these functions! */
		hr->h.size = sizeof(hr->h);
		hr->h.error = HPI_ERROR_INVALID_OPERATION;
		hr->h.function = hm->h.function;
		uncopied_bytes = copy_to_user(puhr, hr, hr->h.size);
		if (uncopied_bytes)
			err = -EFAULT;
		else
			err = 0;
		goto out;
	}

	hr->h.size = res_max_size;
	if (hm->h.object == HPI_OBJ_SUBSYSTEM) {
		hpi_send_recv_f(&hm->m0, &hr->r0, file);
	} else {
		u16 __user *ptr = NULL;
		u32 size = 0;
		/* -1=no data 0=read from user mem, 1=write to user mem */
		int wrflag = -1;
		struct hpi_adapter *pa = NULL;

		if (hm->h.adapter_index < ARRAY_SIZE(adapters))
			pa = &adapters[array_index_nospec(hm->h.adapter_index,
							  ARRAY_SIZE(adapters))];

		if (!pa || !pa->adapter || !pa->adapter->type) {
			hpi_init_response(&hr->r0, hm->h.object,
				hm->h.function, HPI_ERROR_BAD_ADAPTER_NUMBER);

			uncopied_bytes =
				copy_to_user(puhr, hr, sizeof(hr->h));
			if (uncopied_bytes)
				err = -EFAULT;
			else
				err = 0;
			goto out;
		}

		if (mutex_lock_interruptible(&pa->mutex)) {
			err = -EINTR;
			goto out;
		}

		/* Dig out any pointers embedded in the message.  */
		switch (hm->h.function) {
		case HPI_OSTREAM_WRITE:
		case HPI_ISTREAM_READ:{
				/* Yes, sparse, this is correct. */
				ptr = (u16 __user *)hm->m0.u.d.u.data.pb_data;
				size = hm->m0.u.d.u.data.data_size;

				/* Allocate buffer according to application request.
				   ?Is it better to alloc/free for the duration
				   of the transaction?
				 */
				if (pa->buffer_size < size) {
					HPI_DEBUG_LOG(DEBUG,
						"Realloc adapter %d stream "
						"buffer from %zd to %d\n",
						hm->h.adapter_index,
						pa->buffer_size, size);
					if (pa->p_buffer) {
						pa->buffer_size = 0;
						vfree(pa->p_buffer);
					}
					pa->p_buffer = vmalloc(size);
					if (pa->p_buffer)
						pa->buffer_size = size;
					else {
						HPI_DEBUG_LOG(ERROR,
							"HPI could not allocate "
							"stream buffer size %d\n",
							size);

						mutex_unlock(&pa->mutex);
						err = -EINVAL;
						goto out;
					}
				}

				hm->m0.u.d.u.data.pb_data = pa->p_buffer;
				if (hm->h.function == HPI_ISTREAM_READ)
					/* from card, WRITE to user mem */
					wrflag = 1;
				else
					wrflag = 0;
				break;
			}

		default:
			size = 0;
			break;
		}

		if (size && (wrflag == 0)) {
			uncopied_bytes =
				copy_from_user(pa->p_buffer, ptr, size);
			if (uncopied_bytes)
				HPI_DEBUG_LOG(WARNING,
					"Missed %d of %d "
					"bytes from user\n", uncopied_bytes,
					size);
		}

		hpi_send_recv_f(&hm->m0, &hr->r0, file);

		if (size && (wrflag == 1)) {
			uncopied_bytes =
				copy_to_user(ptr, pa->p_buffer, size);
			if (uncopied_bytes)
				HPI_DEBUG_LOG(WARNING,
					"Missed %d of %d " "bytes to user\n",
					uncopied_bytes, size);
		}

		mutex_unlock(&pa->mutex);
	}

	/* on return response size must be set */
	/*printk(KERN_INFO "response size %d\n", hr->h.wSize); */

	if (!hr->h.size) {
		HPI_DEBUG_LOG(ERROR, "response zero size\n");
		err = -EFAULT;
		goto out;
	}

	if (hr->h.size > res_max_size) {
		HPI_DEBUG_LOG(ERROR, "response too big %d %d\n", hr->h.size,
			res_max_size);
		hr->h.error = HPI_ERROR_RESPONSE_BUFFER_TOO_SMALL;
		hr->h.specific_error = hr->h.size;
		hr->h.size = sizeof(hr->h);
	}

	uncopied_bytes = copy_to_user(puhr, hr, hr->h.size);
	if (uncopied_bytes) {
		HPI_DEBUG_LOG(ERROR, "uncopied bytes %d\n", uncopied_bytes);
		err = -EFAULT;
		goto out;
	}

out:
	kfree(hm);
	kfree(hr);
	return err;
}

static int asihpi_irq_count;

static irqreturn_t asihpi_isr(int irq, void *dev_id)
{
	struct hpi_adapter *a = dev_id;
	int handled;

	if (!a->adapter->irq_query_and_clear) {
		pr_err("asihpi_isr ASI%04X:%d no handler\n", a->adapter->type,
			a->adapter->index);
		return IRQ_NONE;
	}

	handled = a->adapter->irq_query_and_clear(a->adapter, 0);

	if (!handled)
		return IRQ_NONE;

	asihpi_irq_count++;
	/* printk(KERN_INFO "asihpi_isr %d ASI%04X:%d irq handled\n",
	   asihpi_irq_count, a->adapter->type, a->adapter->index); */

	if (a->interrupt_callback)
		return IRQ_WAKE_THREAD;

	return IRQ_HANDLED;
}

static irqreturn_t asihpi_isr_thread(int irq, void *dev_id)
{
	struct hpi_adapter *a = dev_id;

	if (a->interrupt_callback)
		a->interrupt_callback(a);
	return IRQ_HANDLED;
}

int asihpi_adapter_probe(struct pci_dev *pci_dev,
			 const struct pci_device_id *pci_id)
{
	int idx, nm, low_latency_mode = 0, irq_supported = 0;
	int adapter_index;
	unsigned int memlen;
	struct hpi_message hm;
	struct hpi_response hr;
	struct hpi_adapter adapter;
	struct hpi_pci pci = { 0 };

	memset(&adapter, 0, sizeof(adapter));

	dev_printk(KERN_DEBUG, &pci_dev->dev,
		"probe %04x:%04x,%04x:%04x,%04x\n", pci_dev->vendor,
		pci_dev->device, pci_dev->subsystem_vendor,
		pci_dev->subsystem_device, pci_dev->devfn);

	if (pci_enable_device(pci_dev) < 0) {
		dev_err(&pci_dev->dev,
			"pci_enable_device failed, disabling device\n");
		return -EIO;
	}

	pci_set_master(pci_dev);	/* also sets latency timer if < 16 */

	hpi_init_message_response(&hm, &hr, HPI_OBJ_SUBSYSTEM,
		HPI_SUBSYS_CREATE_ADAPTER);
	hpi_init_response(&hr, HPI_OBJ_SUBSYSTEM, HPI_SUBSYS_CREATE_ADAPTER,
		HPI_ERROR_PROCESSING_MESSAGE);

	hm.adapter_index = HPI_ADAPTER_INDEX_INVALID;

	nm = HPI_MAX_ADAPTER_MEM_SPACES;

	for (idx = 0; idx < nm; idx++) {
		HPI_DEBUG_LOG(INFO, "resource %d %pR\n", idx,
			&pci_dev->resource[idx]);

		if (pci_resource_flags(pci_dev, idx) & IORESOURCE_MEM) {
			memlen = pci_resource_len(pci_dev, idx);
			pci.ap_mem_base[idx] =
				ioremap(pci_resource_start(pci_dev, idx),
				memlen);
			if (!pci.ap_mem_base[idx]) {
				HPI_DEBUG_LOG(ERROR,
					"ioremap failed, aborting\n");
				/* unmap previously mapped pci mem space */
				goto err;
			}
		}
	}

	pci.pci_dev = pci_dev;
	hm.u.s.resource.bus_type = HPI_BUS_PCI;
	hm.u.s.resource.r.pci = &pci;

	/* call CreateAdapterObject on the relevant hpi module */
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);
	if (hr.error)
		goto err;

	adapter_index = hr.u.s.adapter_index;
	adapter.adapter = hpi_find_adapter(adapter_index);

	if (prealloc_stream_buf) {
		adapter.p_buffer = vmalloc(prealloc_stream_buf);
		if (!adapter.p_buffer) {
			HPI_DEBUG_LOG(ERROR,
				"HPI could not allocate "
				"kernel buffer size %d\n",
				prealloc_stream_buf);
			goto err;
		}
	}

	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_OPEN);
	hm.adapter_index = adapter.adapter->index;
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);

	if (hr.error) {
		HPI_DEBUG_LOG(ERROR, "HPI_ADAPTER_OPEN failed, aborting\n");
		goto err;
	}

	/* Check if current mode == Low Latency mode */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_GET_MODE);
	hm.adapter_index = adapter.adapter->index;
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);

	if (!hr.error
		&& hr.u.ax.mode.adapter_mode == HPI_ADAPTER_MODE_LOW_LATENCY)
		low_latency_mode = 1;
	else
		dev_info(&pci_dev->dev,
			"Adapter at index %d is not in low latency mode\n",
			adapter.adapter->index);

	/* Check if IRQs are supported */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_GET_PROPERTY);
	hm.adapter_index = adapter.adapter->index;
	hm.u.ax.property_set.property = HPI_ADAPTER_PROPERTY_SUPPORTS_IRQ;
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);
	if (hr.error || !hr.u.ax.property_get.parameter1) {
		dev_info(&pci_dev->dev,
			"IRQs not supported by adapter at index %d\n",
			adapter.adapter->index);
	} else {
		irq_supported = 1;
	}

	/* WARNING can't init mutex in 'adapter'
	 * and then copy it to adapters[] ?!?!
	 */
	adapters[adapter_index] = adapter;
	mutex_init(&adapters[adapter_index].mutex);
	pci_set_drvdata(pci_dev, &adapters[adapter_index]);

	if (low_latency_mode && irq_supported) {
		if (!adapter.adapter->irq_query_and_clear) {
			dev_err(&pci_dev->dev,
				"no IRQ handler for adapter %d, aborting\n",
				adapter.adapter->index);
			goto err;
		}

		/* Disable IRQ generation on DSP side by setting the rate to 0 */
		hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
			HPI_ADAPTER_SET_PROPERTY);
		hm.adapter_index = adapter.adapter->index;
		hm.u.ax.property_set.property = HPI_ADAPTER_PROPERTY_IRQ_RATE;
		hm.u.ax.property_set.parameter1 = 0;
		hm.u.ax.property_set.parameter2 = 0;
		hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);
		if (hr.error) {
			HPI_DEBUG_LOG(ERROR,
				"HPI_ADAPTER_GET_MODE failed, aborting\n");
			goto err;
		}

		/* Note: request_irq calls asihpi_isr here */
		if (request_threaded_irq(pci_dev->irq, asihpi_isr,
					 asihpi_isr_thread, IRQF_SHARED,
					 "asihpi", &adapters[adapter_index])) {
			dev_err(&pci_dev->dev, "request_irq(%d) failed\n",
				pci_dev->irq);
			goto err;
		}

		adapters[adapter_index].interrupt_mode = 1;

		dev_info(&pci_dev->dev, "using irq %d\n", pci_dev->irq);
		adapters[adapter_index].irq = pci_dev->irq;
	} else {
		dev_info(&pci_dev->dev, "using polled mode\n");
	}

	dev_info(&pci_dev->dev, "probe succeeded for ASI%04X HPI index %d\n",
		 adapter.adapter->type, adapter_index);

	return 0;

err:
	while (--idx >= 0) {
		if (pci.ap_mem_base[idx]) {
			iounmap(pci.ap_mem_base[idx]);
			pci.ap_mem_base[idx] = NULL;
		}
	}

	if (adapter.p_buffer) {
		adapter.buffer_size = 0;
		vfree(adapter.p_buffer);
	}

	HPI_DEBUG_LOG(ERROR, "adapter_probe failed\n");
	return -ENODEV;
}

void asihpi_adapter_remove(struct pci_dev *pci_dev)
{
	int idx;
	struct hpi_message hm;
	struct hpi_response hr;
	struct hpi_adapter *pa;
	struct hpi_pci pci;

	pa = pci_get_drvdata(pci_dev);
	pci = pa->adapter->pci;

	/* Disable IRQ generation on DSP side */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_SET_PROPERTY);
	hm.adapter_index = pa->adapter->index;
	hm.u.ax.property_set.property = HPI_ADAPTER_PROPERTY_IRQ_RATE;
	hm.u.ax.property_set.parameter1 = 0;
	hm.u.ax.property_set.parameter2 = 0;
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);

	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_DELETE);
	hm.adapter_index = pa->adapter->index;
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);

	/* unmap PCI memory space, mapped during device init. */
	for (idx = 0; idx < HPI_MAX_ADAPTER_MEM_SPACES; ++idx)
		iounmap(pci.ap_mem_base[idx]);

	if (pa->irq)
		free_irq(pa->irq, pa);

	vfree(pa->p_buffer);

	if (1)
		dev_info(&pci_dev->dev,
			 "remove %04x:%04x,%04x:%04x,%04x, HPI index %d\n",
			 pci_dev->vendor, pci_dev->device,
			 pci_dev->subsystem_vendor, pci_dev->subsystem_device,
			 pci_dev->devfn, pa->adapter->index);

	memset(pa, 0, sizeof(*pa));
}

void __init asihpi_init(void)
{
	struct hpi_message hm;
	struct hpi_response hr;

	memset(adapters, 0, sizeof(adapters));

	printk(KERN_INFO "ASIHPI driver " HPI_VER_STRING "\n");

	hpi_init_message_response(&hm, &hr, HPI_OBJ_SUBSYSTEM,
		HPI_SUBSYS_DRIVER_LOAD);
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);
}

void asihpi_exit(void)
{
	struct hpi_message hm;
	struct hpi_response hr;

	hpi_init_message_response(&hm, &hr, HPI_OBJ_SUBSYSTEM,
		HPI_SUBSYS_DRIVER_UNLOAD);
	hpi_send_recv_ex(&hm, &hr, HOWNER_KERNEL);
}
