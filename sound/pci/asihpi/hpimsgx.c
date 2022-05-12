// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************

    AudioScience HPI driver
    Copyright (C) 1997-2014  AudioScience Inc. <support@audioscience.com>


Extended Message Function With Response Caching

(C) Copyright AudioScience Inc. 2002
*****************************************************************************/
#define SOURCEFILE_NAME "hpimsgx.c"
#include "hpi_internal.h"
#include "hpi_version.h"
#include "hpimsginit.h"
#include "hpicmn.h"
#include "hpimsgx.h"
#include "hpidebug.h"

static const struct pci_device_id asihpi_pci_tbl[] = {
#include "hpipcida.h"
};

static struct hpios_spinlock msgx_lock;

static hpi_handler_func *hpi_entry_points[HPI_MAX_ADAPTERS];
static int logging_enabled = 1;

static hpi_handler_func *hpi_lookup_entry_point_function(const struct hpi_pci
	*pci_info)
{

	int i;

	for (i = 0; asihpi_pci_tbl[i].vendor != 0; i++) {
		if (asihpi_pci_tbl[i].vendor != PCI_ANY_ID
			&& asihpi_pci_tbl[i].vendor !=
			pci_info->pci_dev->vendor)
			continue;
		if (asihpi_pci_tbl[i].device != PCI_ANY_ID
			&& asihpi_pci_tbl[i].device !=
			pci_info->pci_dev->device)
			continue;
		if (asihpi_pci_tbl[i].subvendor != PCI_ANY_ID
			&& asihpi_pci_tbl[i].subvendor !=
			pci_info->pci_dev->subsystem_vendor)
			continue;
		if (asihpi_pci_tbl[i].subdevice != PCI_ANY_ID
			&& asihpi_pci_tbl[i].subdevice !=
			pci_info->pci_dev->subsystem_device)
			continue;

		/* HPI_DEBUG_LOG(DEBUG, " %x,%lx\n", i,
		   asihpi_pci_tbl[i].driver_data); */
		return (hpi_handler_func *) asihpi_pci_tbl[i].driver_data;
	}

	return NULL;
}

static inline void hw_entry_point(struct hpi_message *phm,
	struct hpi_response *phr)
{
	if ((phm->adapter_index < HPI_MAX_ADAPTERS)
		&& hpi_entry_points[phm->adapter_index])
		hpi_entry_points[phm->adapter_index] (phm, phr);
	else
		hpi_init_response(phr, phm->object, phm->function,
			HPI_ERROR_PROCESSING_MESSAGE);
}

static void adapter_open(struct hpi_message *phm, struct hpi_response *phr);
static void adapter_close(struct hpi_message *phm, struct hpi_response *phr);

static void mixer_open(struct hpi_message *phm, struct hpi_response *phr);
static void mixer_close(struct hpi_message *phm, struct hpi_response *phr);

static void outstream_open(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner);
static void outstream_close(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner);
static void instream_open(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner);
static void instream_close(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner);

static void HPIMSGX__reset(u16 adapter_index);

static u16 HPIMSGX__init(struct hpi_message *phm, struct hpi_response *phr);
static void HPIMSGX__cleanup(u16 adapter_index, void *h_owner);

#ifndef DISABLE_PRAGMA_PACK1
#pragma pack(push, 1)
#endif

struct hpi_subsys_response {
	struct hpi_response_header h;
	struct hpi_subsys_res s;
};

struct hpi_adapter_response {
	struct hpi_response_header h;
	struct hpi_adapter_res a;
};

struct hpi_mixer_response {
	struct hpi_response_header h;
	struct hpi_mixer_res m;
};

struct hpi_stream_response {
	struct hpi_response_header h;
	struct hpi_stream_res d;
};

struct adapter_info {
	u16 type;
	u16 num_instreams;
	u16 num_outstreams;
};

struct asi_open_state {
	int open_flag;
	void *h_owner;
};

#ifndef DISABLE_PRAGMA_PACK1
#pragma pack(pop)
#endif

/* Globals */
static struct hpi_adapter_response rESP_HPI_ADAPTER_OPEN[HPI_MAX_ADAPTERS];

static struct hpi_stream_response
	rESP_HPI_OSTREAM_OPEN[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];

static struct hpi_stream_response
	rESP_HPI_ISTREAM_OPEN[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];

static struct hpi_mixer_response rESP_HPI_MIXER_OPEN[HPI_MAX_ADAPTERS];

static struct adapter_info aDAPTER_INFO[HPI_MAX_ADAPTERS];

/* use these to keep track of opens from user mode apps/DLLs */
static struct asi_open_state
	outstream_user_open[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];

static struct asi_open_state
	instream_user_open[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];

static void subsys_message(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{
	if (phm->adapter_index != HPI_ADAPTER_INDEX_INVALID)
		HPI_DEBUG_LOG(WARNING,
			"suspicious adapter index %d in subsys message 0x%x.\n",
			phm->adapter_index, phm->function);

	switch (phm->function) {
	case HPI_SUBSYS_GET_VERSION:
		hpi_init_response(phr, HPI_OBJ_SUBSYSTEM,
			HPI_SUBSYS_GET_VERSION, 0);
		phr->u.s.version = HPI_VER >> 8;	/* return major.minor */
		phr->u.s.data = HPI_VER;	/* return major.minor.release */
		break;
	case HPI_SUBSYS_OPEN:
		/*do not propagate the message down the chain */
		hpi_init_response(phr, HPI_OBJ_SUBSYSTEM, HPI_SUBSYS_OPEN, 0);
		break;
	case HPI_SUBSYS_CLOSE:
		/*do not propagate the message down the chain */
		hpi_init_response(phr, HPI_OBJ_SUBSYSTEM, HPI_SUBSYS_CLOSE,
			0);
		HPIMSGX__cleanup(HPIMSGX_ALLADAPTERS, h_owner);
		break;
	case HPI_SUBSYS_DRIVER_LOAD:
		/* Initialize this module's internal state */
		hpios_msgxlock_init(&msgx_lock);
		memset(&hpi_entry_points, 0, sizeof(hpi_entry_points));
		/* Init subsys_findadapters response to no-adapters */
		HPIMSGX__reset(HPIMSGX_ALLADAPTERS);
		hpi_init_response(phr, HPI_OBJ_SUBSYSTEM,
			HPI_SUBSYS_DRIVER_LOAD, 0);
		/* individual HPIs dont implement driver load */
		HPI_COMMON(phm, phr);
		break;
	case HPI_SUBSYS_DRIVER_UNLOAD:
		HPI_COMMON(phm, phr);
		HPIMSGX__cleanup(HPIMSGX_ALLADAPTERS, h_owner);
		hpi_init_response(phr, HPI_OBJ_SUBSYSTEM,
			HPI_SUBSYS_DRIVER_UNLOAD, 0);
		return;

	case HPI_SUBSYS_GET_NUM_ADAPTERS:
	case HPI_SUBSYS_GET_ADAPTER:
		HPI_COMMON(phm, phr);
		break;

	case HPI_SUBSYS_CREATE_ADAPTER:
		HPIMSGX__init(phm, phr);
		break;

	default:
		/* Must explicitly handle every subsys message in this switch */
		hpi_init_response(phr, HPI_OBJ_SUBSYSTEM, phm->function,
			HPI_ERROR_INVALID_FUNC);
		break;
	}
}

static void adapter_message(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{
	switch (phm->function) {
	case HPI_ADAPTER_OPEN:
		adapter_open(phm, phr);
		break;
	case HPI_ADAPTER_CLOSE:
		adapter_close(phm, phr);
		break;
	case HPI_ADAPTER_DELETE:
		HPIMSGX__cleanup(phm->adapter_index, h_owner);
		{
			struct hpi_message hm;
			struct hpi_response hr;
			hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
				HPI_ADAPTER_CLOSE);
			hm.adapter_index = phm->adapter_index;
			hw_entry_point(&hm, &hr);
		}
		hw_entry_point(phm, phr);
		break;

	default:
		hw_entry_point(phm, phr);
		break;
	}
}

static void mixer_message(struct hpi_message *phm, struct hpi_response *phr)
{
	switch (phm->function) {
	case HPI_MIXER_OPEN:
		mixer_open(phm, phr);
		break;
	case HPI_MIXER_CLOSE:
		mixer_close(phm, phr);
		break;
	default:
		hw_entry_point(phm, phr);
		break;
	}
}

static void outstream_message(struct hpi_message *phm,
	struct hpi_response *phr, void *h_owner)
{
	if (phm->obj_index >= aDAPTER_INFO[phm->adapter_index].num_outstreams) {
		hpi_init_response(phr, HPI_OBJ_OSTREAM, phm->function,
			HPI_ERROR_INVALID_OBJ_INDEX);
		return;
	}

	switch (phm->function) {
	case HPI_OSTREAM_OPEN:
		outstream_open(phm, phr, h_owner);
		break;
	case HPI_OSTREAM_CLOSE:
		outstream_close(phm, phr, h_owner);
		break;
	default:
		hw_entry_point(phm, phr);
		break;
	}
}

static void instream_message(struct hpi_message *phm,
	struct hpi_response *phr, void *h_owner)
{
	if (phm->obj_index >= aDAPTER_INFO[phm->adapter_index].num_instreams) {
		hpi_init_response(phr, HPI_OBJ_ISTREAM, phm->function,
			HPI_ERROR_INVALID_OBJ_INDEX);
		return;
	}

	switch (phm->function) {
	case HPI_ISTREAM_OPEN:
		instream_open(phm, phr, h_owner);
		break;
	case HPI_ISTREAM_CLOSE:
		instream_close(phm, phr, h_owner);
		break;
	default:
		hw_entry_point(phm, phr);
		break;
	}
}

/* NOTE: HPI_Message() must be defined in the driver as a wrapper for
 * HPI_MessageEx so that functions in hpifunc.c compile.
 */
void hpi_send_recv_ex(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{

	if (logging_enabled)
		HPI_DEBUG_MESSAGE(DEBUG, phm);

	if (phm->type != HPI_TYPE_REQUEST) {
		hpi_init_response(phr, phm->object, phm->function,
			HPI_ERROR_INVALID_TYPE);
		return;
	}

	if (phm->adapter_index >= HPI_MAX_ADAPTERS
		&& phm->adapter_index != HPIMSGX_ALLADAPTERS) {
		hpi_init_response(phr, phm->object, phm->function,
			HPI_ERROR_BAD_ADAPTER_NUMBER);
		return;
	}

	switch (phm->object) {
	case HPI_OBJ_SUBSYSTEM:
		subsys_message(phm, phr, h_owner);
		break;

	case HPI_OBJ_ADAPTER:
		adapter_message(phm, phr, h_owner);
		break;

	case HPI_OBJ_MIXER:
		mixer_message(phm, phr);
		break;

	case HPI_OBJ_OSTREAM:
		outstream_message(phm, phr, h_owner);
		break;

	case HPI_OBJ_ISTREAM:
		instream_message(phm, phr, h_owner);
		break;

	default:
		hw_entry_point(phm, phr);
		break;
	}

	if (logging_enabled)
		HPI_DEBUG_RESPONSE(phr);

	if (phr->error >= HPI_ERROR_DSP_COMMUNICATION) {
		hpi_debug_level_set(HPI_DEBUG_LEVEL_ERROR);
		logging_enabled = 0;
	}
}

static void adapter_open(struct hpi_message *phm, struct hpi_response *phr)
{
	HPI_DEBUG_LOG(VERBOSE, "adapter_open\n");
	memcpy(phr, &rESP_HPI_ADAPTER_OPEN[phm->adapter_index],
		sizeof(rESP_HPI_ADAPTER_OPEN[0]));
}

static void adapter_close(struct hpi_message *phm, struct hpi_response *phr)
{
	HPI_DEBUG_LOG(VERBOSE, "adapter_close\n");
	hpi_init_response(phr, HPI_OBJ_ADAPTER, HPI_ADAPTER_CLOSE, 0);
}

static void mixer_open(struct hpi_message *phm, struct hpi_response *phr)
{
	memcpy(phr, &rESP_HPI_MIXER_OPEN[phm->adapter_index],
		sizeof(rESP_HPI_MIXER_OPEN[0]));
}

static void mixer_close(struct hpi_message *phm, struct hpi_response *phr)
{
	hpi_init_response(phr, HPI_OBJ_MIXER, HPI_MIXER_CLOSE, 0);
}

static void instream_open(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{

	struct hpi_message hm;
	struct hpi_response hr;

	hpi_init_response(phr, HPI_OBJ_ISTREAM, HPI_ISTREAM_OPEN, 0);

	hpios_msgxlock_lock(&msgx_lock);

	if (instream_user_open[phm->adapter_index][phm->obj_index].open_flag)
		phr->error = HPI_ERROR_OBJ_ALREADY_OPEN;
	else if (rESP_HPI_ISTREAM_OPEN[phm->adapter_index]
		[phm->obj_index].h.error)
		memcpy(phr,
			&rESP_HPI_ISTREAM_OPEN[phm->adapter_index][phm->
				obj_index],
			sizeof(rESP_HPI_ISTREAM_OPEN[0][0]));
	else {
		instream_user_open[phm->adapter_index][phm->
			obj_index].open_flag = 1;
		hpios_msgxlock_unlock(&msgx_lock);

		/* issue a reset */
		hpi_init_message_response(&hm, &hr, HPI_OBJ_ISTREAM,
			HPI_ISTREAM_RESET);
		hm.adapter_index = phm->adapter_index;
		hm.obj_index = phm->obj_index;
		hw_entry_point(&hm, &hr);

		hpios_msgxlock_lock(&msgx_lock);
		if (hr.error) {
			instream_user_open[phm->adapter_index][phm->
				obj_index].open_flag = 0;
			phr->error = hr.error;
		} else {
			instream_user_open[phm->adapter_index][phm->
				obj_index].open_flag = 1;
			instream_user_open[phm->adapter_index][phm->
				obj_index].h_owner = h_owner;
			memcpy(phr,
				&rESP_HPI_ISTREAM_OPEN[phm->adapter_index]
				[phm->obj_index],
				sizeof(rESP_HPI_ISTREAM_OPEN[0][0]));
		}
	}
	hpios_msgxlock_unlock(&msgx_lock);
}

static void instream_close(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{

	struct hpi_message hm;
	struct hpi_response hr;

	hpi_init_response(phr, HPI_OBJ_ISTREAM, HPI_ISTREAM_CLOSE, 0);

	hpios_msgxlock_lock(&msgx_lock);
	if (h_owner ==
		instream_user_open[phm->adapter_index][phm->
			obj_index].h_owner) {
		/* HPI_DEBUG_LOG(INFO,"closing adapter %d "
		   "instream %d owned by %p\n",
		   phm->wAdapterIndex, phm->wObjIndex, hOwner); */
		instream_user_open[phm->adapter_index][phm->
			obj_index].h_owner = NULL;
		hpios_msgxlock_unlock(&msgx_lock);
		/* issue a reset */
		hpi_init_message_response(&hm, &hr, HPI_OBJ_ISTREAM,
			HPI_ISTREAM_RESET);
		hm.adapter_index = phm->adapter_index;
		hm.obj_index = phm->obj_index;
		hw_entry_point(&hm, &hr);
		hpios_msgxlock_lock(&msgx_lock);
		if (hr.error) {
			instream_user_open[phm->adapter_index][phm->
				obj_index].h_owner = h_owner;
			phr->error = hr.error;
		} else {
			instream_user_open[phm->adapter_index][phm->
				obj_index].open_flag = 0;
			instream_user_open[phm->adapter_index][phm->
				obj_index].h_owner = NULL;
		}
	} else {
		HPI_DEBUG_LOG(WARNING,
			"%p trying to close %d instream %d owned by %p\n",
			h_owner, phm->adapter_index, phm->obj_index,
			instream_user_open[phm->adapter_index][phm->
				obj_index].h_owner);
		phr->error = HPI_ERROR_OBJ_NOT_OPEN;
	}
	hpios_msgxlock_unlock(&msgx_lock);
}

static void outstream_open(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{

	struct hpi_message hm;
	struct hpi_response hr;

	hpi_init_response(phr, HPI_OBJ_OSTREAM, HPI_OSTREAM_OPEN, 0);

	hpios_msgxlock_lock(&msgx_lock);

	if (outstream_user_open[phm->adapter_index][phm->obj_index].open_flag)
		phr->error = HPI_ERROR_OBJ_ALREADY_OPEN;
	else if (rESP_HPI_OSTREAM_OPEN[phm->adapter_index]
		[phm->obj_index].h.error)
		memcpy(phr,
			&rESP_HPI_OSTREAM_OPEN[phm->adapter_index][phm->
				obj_index],
			sizeof(rESP_HPI_OSTREAM_OPEN[0][0]));
	else {
		outstream_user_open[phm->adapter_index][phm->
			obj_index].open_flag = 1;
		hpios_msgxlock_unlock(&msgx_lock);

		/* issue a reset */
		hpi_init_message_response(&hm, &hr, HPI_OBJ_OSTREAM,
			HPI_OSTREAM_RESET);
		hm.adapter_index = phm->adapter_index;
		hm.obj_index = phm->obj_index;
		hw_entry_point(&hm, &hr);

		hpios_msgxlock_lock(&msgx_lock);
		if (hr.error) {
			outstream_user_open[phm->adapter_index][phm->
				obj_index].open_flag = 0;
			phr->error = hr.error;
		} else {
			outstream_user_open[phm->adapter_index][phm->
				obj_index].open_flag = 1;
			outstream_user_open[phm->adapter_index][phm->
				obj_index].h_owner = h_owner;
			memcpy(phr,
				&rESP_HPI_OSTREAM_OPEN[phm->adapter_index]
				[phm->obj_index],
				sizeof(rESP_HPI_OSTREAM_OPEN[0][0]));
		}
	}
	hpios_msgxlock_unlock(&msgx_lock);
}

static void outstream_close(struct hpi_message *phm, struct hpi_response *phr,
	void *h_owner)
{

	struct hpi_message hm;
	struct hpi_response hr;

	hpi_init_response(phr, HPI_OBJ_OSTREAM, HPI_OSTREAM_CLOSE, 0);

	hpios_msgxlock_lock(&msgx_lock);

	if (h_owner ==
		outstream_user_open[phm->adapter_index][phm->
			obj_index].h_owner) {
		/* HPI_DEBUG_LOG(INFO,"closing adapter %d "
		   "outstream %d owned by %p\n",
		   phm->wAdapterIndex, phm->wObjIndex, hOwner); */
		outstream_user_open[phm->adapter_index][phm->
			obj_index].h_owner = NULL;
		hpios_msgxlock_unlock(&msgx_lock);
		/* issue a reset */
		hpi_init_message_response(&hm, &hr, HPI_OBJ_OSTREAM,
			HPI_OSTREAM_RESET);
		hm.adapter_index = phm->adapter_index;
		hm.obj_index = phm->obj_index;
		hw_entry_point(&hm, &hr);
		hpios_msgxlock_lock(&msgx_lock);
		if (hr.error) {
			outstream_user_open[phm->adapter_index][phm->
				obj_index].h_owner = h_owner;
			phr->error = hr.error;
		} else {
			outstream_user_open[phm->adapter_index][phm->
				obj_index].open_flag = 0;
			outstream_user_open[phm->adapter_index][phm->
				obj_index].h_owner = NULL;
		}
	} else {
		HPI_DEBUG_LOG(WARNING,
			"%p trying to close %d outstream %d owned by %p\n",
			h_owner, phm->adapter_index, phm->obj_index,
			outstream_user_open[phm->adapter_index][phm->
				obj_index].h_owner);
		phr->error = HPI_ERROR_OBJ_NOT_OPEN;
	}
	hpios_msgxlock_unlock(&msgx_lock);
}

static u16 adapter_prepare(u16 adapter)
{
	struct hpi_message hm;
	struct hpi_response hr;

	/* Open the adapter and streams */
	u16 i;

	/* call to HPI_ADAPTER_OPEN */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_OPEN);
	hm.adapter_index = adapter;
	hw_entry_point(&hm, &hr);
	memcpy(&rESP_HPI_ADAPTER_OPEN[adapter], &hr,
		sizeof(rESP_HPI_ADAPTER_OPEN[0]));
	if (hr.error)
		return hr.error;

	/* call to HPI_ADAPTER_GET_INFO */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_ADAPTER,
		HPI_ADAPTER_GET_INFO);
	hm.adapter_index = adapter;
	hw_entry_point(&hm, &hr);
	if (hr.error)
		return hr.error;

	aDAPTER_INFO[adapter].num_outstreams = hr.u.ax.info.num_outstreams;
	aDAPTER_INFO[adapter].num_instreams = hr.u.ax.info.num_instreams;
	aDAPTER_INFO[adapter].type = hr.u.ax.info.adapter_type;

	/* call to HPI_OSTREAM_OPEN */
	for (i = 0; i < aDAPTER_INFO[adapter].num_outstreams; i++) {
		hpi_init_message_response(&hm, &hr, HPI_OBJ_OSTREAM,
			HPI_OSTREAM_OPEN);
		hm.adapter_index = adapter;
		hm.obj_index = i;
		hw_entry_point(&hm, &hr);
		memcpy(&rESP_HPI_OSTREAM_OPEN[adapter][i], &hr,
			sizeof(rESP_HPI_OSTREAM_OPEN[0][0]));
		outstream_user_open[adapter][i].open_flag = 0;
		outstream_user_open[adapter][i].h_owner = NULL;
	}

	/* call to HPI_ISTREAM_OPEN */
	for (i = 0; i < aDAPTER_INFO[adapter].num_instreams; i++) {
		hpi_init_message_response(&hm, &hr, HPI_OBJ_ISTREAM,
			HPI_ISTREAM_OPEN);
		hm.adapter_index = adapter;
		hm.obj_index = i;
		hw_entry_point(&hm, &hr);
		memcpy(&rESP_HPI_ISTREAM_OPEN[adapter][i], &hr,
			sizeof(rESP_HPI_ISTREAM_OPEN[0][0]));
		instream_user_open[adapter][i].open_flag = 0;
		instream_user_open[adapter][i].h_owner = NULL;
	}

	/* call to HPI_MIXER_OPEN */
	hpi_init_message_response(&hm, &hr, HPI_OBJ_MIXER, HPI_MIXER_OPEN);
	hm.adapter_index = adapter;
	hw_entry_point(&hm, &hr);
	memcpy(&rESP_HPI_MIXER_OPEN[adapter], &hr,
		sizeof(rESP_HPI_MIXER_OPEN[0]));

	return 0;
}

static void HPIMSGX__reset(u16 adapter_index)
{
	int i;
	u16 adapter;
	struct hpi_response hr;

	if (adapter_index == HPIMSGX_ALLADAPTERS) {
		for (adapter = 0; adapter < HPI_MAX_ADAPTERS; adapter++) {

			hpi_init_response(&hr, HPI_OBJ_ADAPTER,
				HPI_ADAPTER_OPEN, HPI_ERROR_BAD_ADAPTER);
			memcpy(&rESP_HPI_ADAPTER_OPEN[adapter], &hr,
				sizeof(rESP_HPI_ADAPTER_OPEN[adapter]));

			hpi_init_response(&hr, HPI_OBJ_MIXER, HPI_MIXER_OPEN,
				HPI_ERROR_INVALID_OBJ);
			memcpy(&rESP_HPI_MIXER_OPEN[adapter], &hr,
				sizeof(rESP_HPI_MIXER_OPEN[adapter]));

			for (i = 0; i < HPI_MAX_STREAMS; i++) {
				hpi_init_response(&hr, HPI_OBJ_OSTREAM,
					HPI_OSTREAM_OPEN,
					HPI_ERROR_INVALID_OBJ);
				memcpy(&rESP_HPI_OSTREAM_OPEN[adapter][i],
					&hr,
					sizeof(rESP_HPI_OSTREAM_OPEN[adapter]
						[i]));
				hpi_init_response(&hr, HPI_OBJ_ISTREAM,
					HPI_ISTREAM_OPEN,
					HPI_ERROR_INVALID_OBJ);
				memcpy(&rESP_HPI_ISTREAM_OPEN[adapter][i],
					&hr,
					sizeof(rESP_HPI_ISTREAM_OPEN[adapter]
						[i]));
			}
		}
	} else if (adapter_index < HPI_MAX_ADAPTERS) {
		rESP_HPI_ADAPTER_OPEN[adapter_index].h.error =
			HPI_ERROR_BAD_ADAPTER;
		rESP_HPI_MIXER_OPEN[adapter_index].h.error =
			HPI_ERROR_INVALID_OBJ;
		for (i = 0; i < HPI_MAX_STREAMS; i++) {
			rESP_HPI_OSTREAM_OPEN[adapter_index][i].h.error =
				HPI_ERROR_INVALID_OBJ;
			rESP_HPI_ISTREAM_OPEN[adapter_index][i].h.error =
				HPI_ERROR_INVALID_OBJ;
		}
	}
}

static u16 HPIMSGX__init(struct hpi_message *phm,
	/* HPI_SUBSYS_CREATE_ADAPTER structure with */
	/* resource list or NULL=find all */
	struct hpi_response *phr
	/* response from HPI_ADAPTER_GET_INFO */
	)
{
	hpi_handler_func *entry_point_func;
	struct hpi_response hr;

	/* Init response here so we can pass in previous adapter list */
	hpi_init_response(&hr, phm->object, phm->function,
		HPI_ERROR_INVALID_OBJ);

	entry_point_func =
		hpi_lookup_entry_point_function(phm->u.s.resource.r.pci);

	if (entry_point_func) {
		HPI_DEBUG_MESSAGE(DEBUG, phm);
		entry_point_func(phm, &hr);
	} else {
		phr->error = HPI_ERROR_PROCESSING_MESSAGE;
		return phr->error;
	}
	if (hr.error == 0) {
		/* the adapter was created successfully
		   save the mapping for future use */
		hpi_entry_points[hr.u.s.adapter_index] = entry_point_func;
		/* prepare adapter (pre-open streams etc.) */
		HPI_DEBUG_LOG(DEBUG,
			"HPI_SUBSYS_CREATE_ADAPTER successful,"
			" preparing adapter\n");
		adapter_prepare(hr.u.s.adapter_index);
	}
	memcpy(phr, &hr, hr.size);
	return phr->error;
}

static void HPIMSGX__cleanup(u16 adapter_index, void *h_owner)
{
	int i, adapter, adapter_limit;

	if (!h_owner)
		return;

	if (adapter_index == HPIMSGX_ALLADAPTERS) {
		adapter = 0;
		adapter_limit = HPI_MAX_ADAPTERS;
	} else {
		adapter = adapter_index;
		adapter_limit = adapter + 1;
	}

	for (; adapter < adapter_limit; adapter++) {
		/*      printk(KERN_INFO "Cleanup adapter #%d\n",wAdapter); */
		for (i = 0; i < HPI_MAX_STREAMS; i++) {
			if (h_owner ==
				outstream_user_open[adapter][i].h_owner) {
				struct hpi_message hm;
				struct hpi_response hr;

				HPI_DEBUG_LOG(DEBUG,
					"Close adapter %d ostream %d\n",
					adapter, i);

				hpi_init_message_response(&hm, &hr,
					HPI_OBJ_OSTREAM, HPI_OSTREAM_RESET);
				hm.adapter_index = (u16)adapter;
				hm.obj_index = (u16)i;
				hw_entry_point(&hm, &hr);

				hm.function = HPI_OSTREAM_HOSTBUFFER_FREE;
				hw_entry_point(&hm, &hr);

				hm.function = HPI_OSTREAM_GROUP_RESET;
				hw_entry_point(&hm, &hr);

				outstream_user_open[adapter][i].open_flag = 0;
				outstream_user_open[adapter][i].h_owner =
					NULL;
			}
			if (h_owner == instream_user_open[adapter][i].h_owner) {
				struct hpi_message hm;
				struct hpi_response hr;

				HPI_DEBUG_LOG(DEBUG,
					"Close adapter %d istream %d\n",
					adapter, i);

				hpi_init_message_response(&hm, &hr,
					HPI_OBJ_ISTREAM, HPI_ISTREAM_RESET);
				hm.adapter_index = (u16)adapter;
				hm.obj_index = (u16)i;
				hw_entry_point(&hm, &hr);

				hm.function = HPI_ISTREAM_HOSTBUFFER_FREE;
				hw_entry_point(&hm, &hr);

				hm.function = HPI_ISTREAM_GROUP_RESET;
				hw_entry_point(&hm, &hr);

				instream_user_open[adapter][i].open_flag = 0;
				instream_user_open[adapter][i].h_owner = NULL;
			}
		}
	}
}
