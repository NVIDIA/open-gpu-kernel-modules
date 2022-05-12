// SPDX-License-Identifier: GPL-2.0-only
/*
 * QLogic Fibre Channel HBA Driver
 * Copyright (c)  2003-2014 QLogic Corporation
 */
#include "qla_def.h"
#include "qla_tmpl.h"

#define ISPREG(vha)	(&(vha)->hw->iobase->isp24)
#define IOBAR(reg)	offsetof(typeof(*(reg)), iobase_addr)
#define IOBASE(vha)	IOBAR(ISPREG(vha))
#define INVALID_ENTRY ((struct qla27xx_fwdt_entry *)0xffffffffffffffffUL)

static inline void
qla27xx_insert16(uint16_t value, void *buf, ulong *len)
{
	if (buf) {
		buf += *len;
		*(__le16 *)buf = cpu_to_le16(value);
	}
	*len += sizeof(value);
}

static inline void
qla27xx_insert32(uint32_t value, void *buf, ulong *len)
{
	if (buf) {
		buf += *len;
		*(__le32 *)buf = cpu_to_le32(value);
	}
	*len += sizeof(value);
}

static inline void
qla27xx_insertbuf(void *mem, ulong size, void *buf, ulong *len)
{
	if (buf && mem && size) {
		buf += *len;
		memcpy(buf, mem, size);
	}
	*len += size;
}

static inline void
qla27xx_read8(void __iomem *window, void *buf, ulong *len)
{
	uint8_t value = ~0;

	if (buf) {
		value = rd_reg_byte(window);
	}
	qla27xx_insert32(value, buf, len);
}

static inline void
qla27xx_read16(void __iomem *window, void *buf, ulong *len)
{
	uint16_t value = ~0;

	if (buf) {
		value = rd_reg_word(window);
	}
	qla27xx_insert32(value, buf, len);
}

static inline void
qla27xx_read32(void __iomem *window, void *buf, ulong *len)
{
	uint32_t value = ~0;

	if (buf) {
		value = rd_reg_dword(window);
	}
	qla27xx_insert32(value, buf, len);
}

static inline void (*qla27xx_read_vector(uint width))(void __iomem*, void *, ulong *)
{
	return
	    (width == 1) ? qla27xx_read8 :
	    (width == 2) ? qla27xx_read16 :
			   qla27xx_read32;
}

static inline void
qla27xx_read_reg(__iomem struct device_reg_24xx *reg,
	uint offset, void *buf, ulong *len)
{
	void __iomem *window = (void __iomem *)reg + offset;

	qla27xx_read32(window, buf, len);
}

static inline void
qla27xx_write_reg(__iomem struct device_reg_24xx *reg,
	uint offset, uint32_t data, void *buf)
{
	if (buf) {
		void __iomem *window = (void __iomem *)reg + offset;

		wrt_reg_dword(window, data);
	}
}

static inline void
qla27xx_read_window(__iomem struct device_reg_24xx *reg,
	uint32_t addr, uint offset, uint count, uint width, void *buf,
	ulong *len)
{
	void __iomem *window = (void __iomem *)reg + offset;
	void (*readn)(void __iomem*, void *, ulong *) = qla27xx_read_vector(width);

	qla27xx_write_reg(reg, IOBAR(reg), addr, buf);
	while (count--) {
		qla27xx_insert32(addr, buf, len);
		readn(window, buf, len);
		window += width;
		addr++;
	}
}

static inline void
qla27xx_skip_entry(struct qla27xx_fwdt_entry *ent, void *buf)
{
	if (buf)
		ent->hdr.driver_flags |= DRIVER_FLAG_SKIP_ENTRY;
}

static inline struct qla27xx_fwdt_entry *
qla27xx_next_entry(struct qla27xx_fwdt_entry *ent)
{
	return (void *)ent + le32_to_cpu(ent->hdr.size);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t0(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc, vha, 0xd100,
	    "%s: nop [%lx]\n", __func__, *len);
	qla27xx_skip_entry(ent, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t255(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc, vha, 0xd1ff,
	    "%s: end [%lx]\n", __func__, *len);
	qla27xx_skip_entry(ent, buf);

	/* terminate */
	return NULL;
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t256(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong addr = le32_to_cpu(ent->t256.base_addr);
	uint offset = ent->t256.pci_offset;
	ulong count = le16_to_cpu(ent->t256.reg_count);
	uint width = ent->t256.reg_width;

	ql_dbg(ql_dbg_misc, vha, 0xd200,
	    "%s: rdio t1 [%lx]\n", __func__, *len);
	qla27xx_read_window(ISPREG(vha), addr, offset, count, width, buf, len);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t257(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong addr = le32_to_cpu(ent->t257.base_addr);
	uint offset = ent->t257.pci_offset;
	ulong data = le32_to_cpu(ent->t257.write_data);

	ql_dbg(ql_dbg_misc, vha, 0xd201,
	    "%s: wrio t1 [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), IOBASE(vha), addr, buf);
	qla27xx_write_reg(ISPREG(vha), offset, data, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t258(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	uint banksel = ent->t258.banksel_offset;
	ulong bank = le32_to_cpu(ent->t258.bank);
	ulong addr = le32_to_cpu(ent->t258.base_addr);
	uint offset = ent->t258.pci_offset;
	uint count = le16_to_cpu(ent->t258.reg_count);
	uint width = ent->t258.reg_width;

	ql_dbg(ql_dbg_misc, vha, 0xd202,
	    "%s: rdio t2 [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), banksel, bank, buf);
	qla27xx_read_window(ISPREG(vha), addr, offset, count, width, buf, len);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t259(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong addr = le32_to_cpu(ent->t259.base_addr);
	uint banksel = ent->t259.banksel_offset;
	ulong bank = le32_to_cpu(ent->t259.bank);
	uint offset = ent->t259.pci_offset;
	ulong data = le32_to_cpu(ent->t259.write_data);

	ql_dbg(ql_dbg_misc, vha, 0xd203,
	    "%s: wrio t2 [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), IOBASE(vha), addr, buf);
	qla27xx_write_reg(ISPREG(vha), banksel, bank, buf);
	qla27xx_write_reg(ISPREG(vha), offset, data, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t260(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	uint offset = ent->t260.pci_offset;

	ql_dbg(ql_dbg_misc, vha, 0xd204,
	    "%s: rdpci [%lx]\n", __func__, *len);
	qla27xx_insert32(offset, buf, len);
	qla27xx_read_reg(ISPREG(vha), offset, buf, len);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t261(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	uint offset = ent->t261.pci_offset;
	ulong data = le32_to_cpu(ent->t261.write_data);

	ql_dbg(ql_dbg_misc, vha, 0xd205,
	    "%s: wrpci [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), offset, data, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t262(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	uint area = ent->t262.ram_area;
	ulong start = le32_to_cpu(ent->t262.start_addr);
	ulong end = le32_to_cpu(ent->t262.end_addr);
	ulong dwords;
	int rc;

	ql_dbg(ql_dbg_misc, vha, 0xd206,
	    "%s: rdram(%x) [%lx]\n", __func__, ent->t262.ram_area, *len);

	if (area == T262_RAM_AREA_CRITICAL_RAM) {
		;
	} else if (area == T262_RAM_AREA_EXTERNAL_RAM) {
		end = vha->hw->fw_memory_size;
		if (buf)
			ent->t262.end_addr = cpu_to_le32(end);
	} else if (area == T262_RAM_AREA_SHARED_RAM) {
		start = vha->hw->fw_shared_ram_start;
		end = vha->hw->fw_shared_ram_end;
		if (buf) {
			ent->t262.start_addr = cpu_to_le32(start);
			ent->t262.end_addr = cpu_to_le32(end);
		}
	} else if (area == T262_RAM_AREA_DDR_RAM) {
		start = vha->hw->fw_ddr_ram_start;
		end = vha->hw->fw_ddr_ram_end;
		if (buf) {
			ent->t262.start_addr = cpu_to_le32(start);
			ent->t262.end_addr = cpu_to_le32(end);
		}
	} else if (area == T262_RAM_AREA_MISC) {
		if (buf) {
			ent->t262.start_addr = cpu_to_le32(start);
			ent->t262.end_addr = cpu_to_le32(end);
		}
	} else {
		ql_dbg(ql_dbg_misc, vha, 0xd022,
		    "%s: unknown area %x\n", __func__, area);
		qla27xx_skip_entry(ent, buf);
		goto done;
	}

	if (end < start || start == 0 || end == 0) {
		ql_dbg(ql_dbg_misc, vha, 0xd023,
		    "%s: unusable range (start=%lx end=%lx)\n",
		    __func__, start, end);
		qla27xx_skip_entry(ent, buf);
		goto done;
	}

	dwords = end - start + 1;
	if (buf) {
		buf += *len;
		rc = qla24xx_dump_ram(vha->hw, start, buf, dwords, &buf);
		if (rc != QLA_SUCCESS) {
			ql_dbg(ql_dbg_async, vha, 0xffff,
			    "%s: dump ram MB failed. Area %xh start %lxh end %lxh\n",
			    __func__, area, start, end);
			return INVALID_ENTRY;
		}
	}
	*len += dwords * sizeof(uint32_t);
done:
	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t263(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	uint type = ent->t263.queue_type;
	uint count = 0;
	uint i;
	uint length;

	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd207,
	    "%s: getq(%x) [%lx]\n", __func__, type, *len);
	if (type == T263_QUEUE_TYPE_REQ) {
		for (i = 0; i < vha->hw->max_req_queues; i++) {
			struct req_que *req = vha->hw->req_q_map[i];

			if (req || !buf) {
				length = req ?
				    req->length : REQUEST_ENTRY_CNT_24XX;
				qla27xx_insert16(i, buf, len);
				qla27xx_insert16(length, buf, len);
				qla27xx_insertbuf(req ? req->ring : NULL,
				    length * sizeof(*req->ring), buf, len);
				count++;
			}
		}
	} else if (type == T263_QUEUE_TYPE_RSP) {
		for (i = 0; i < vha->hw->max_rsp_queues; i++) {
			struct rsp_que *rsp = vha->hw->rsp_q_map[i];

			if (rsp || !buf) {
				length = rsp ?
				    rsp->length : RESPONSE_ENTRY_CNT_MQ;
				qla27xx_insert16(i, buf, len);
				qla27xx_insert16(length, buf, len);
				qla27xx_insertbuf(rsp ? rsp->ring : NULL,
				    length * sizeof(*rsp->ring), buf, len);
				count++;
			}
		}
	} else if (QLA_TGT_MODE_ENABLED() &&
	    ent->t263.queue_type == T263_QUEUE_TYPE_ATIO) {
		struct qla_hw_data *ha = vha->hw;
		struct atio *atr = ha->tgt.atio_ring;

		if (atr || !buf) {
			length = ha->tgt.atio_q_length;
			qla27xx_insert16(0, buf, len);
			qla27xx_insert16(length, buf, len);
			qla27xx_insertbuf(atr, length * sizeof(*atr), buf, len);
			count++;
		}
	} else {
		ql_dbg(ql_dbg_misc, vha, 0xd026,
		    "%s: unknown queue %x\n", __func__, type);
		qla27xx_skip_entry(ent, buf);
	}

	if (buf) {
		if (count)
			ent->t263.num_queues = count;
		else
			qla27xx_skip_entry(ent, buf);
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t264(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc, vha, 0xd208,
	    "%s: getfce [%lx]\n", __func__, *len);
	if (vha->hw->fce) {
		if (buf) {
			ent->t264.fce_trace_size = FCE_SIZE;
			ent->t264.write_pointer = vha->hw->fce_wr;
			ent->t264.base_pointer = vha->hw->fce_dma;
			ent->t264.fce_enable_mb0 = vha->hw->fce_mb[0];
			ent->t264.fce_enable_mb2 = vha->hw->fce_mb[2];
			ent->t264.fce_enable_mb3 = vha->hw->fce_mb[3];
			ent->t264.fce_enable_mb4 = vha->hw->fce_mb[4];
			ent->t264.fce_enable_mb5 = vha->hw->fce_mb[5];
			ent->t264.fce_enable_mb6 = vha->hw->fce_mb[6];
		}
		qla27xx_insertbuf(vha->hw->fce, FCE_SIZE, buf, len);
	} else {
		ql_dbg(ql_dbg_misc, vha, 0xd027,
		    "%s: missing fce\n", __func__);
		qla27xx_skip_entry(ent, buf);
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t265(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd209,
	    "%s: pause risc [%lx]\n", __func__, *len);
	if (buf)
		qla24xx_pause_risc(ISPREG(vha), vha->hw);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t266(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc, vha, 0xd20a,
	    "%s: reset risc [%lx]\n", __func__, *len);
	if (buf)
		WARN_ON_ONCE(qla24xx_soft_reset(vha->hw) != QLA_SUCCESS);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t267(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	uint offset = ent->t267.pci_offset;
	ulong data = le32_to_cpu(ent->t267.data);

	ql_dbg(ql_dbg_misc, vha, 0xd20b,
	    "%s: dis intr [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), offset, data, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t268(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc, vha, 0xd20c,
	    "%s: gethb(%x) [%lx]\n", __func__, ent->t268.buf_type, *len);
	switch (ent->t268.buf_type) {
	case T268_BUF_TYPE_EXTD_TRACE:
		if (vha->hw->eft) {
			if (buf) {
				ent->t268.buf_size = EFT_SIZE;
				ent->t268.start_addr = vha->hw->eft_dma;
			}
			qla27xx_insertbuf(vha->hw->eft, EFT_SIZE, buf, len);
		} else {
			ql_dbg(ql_dbg_misc, vha, 0xd028,
			    "%s: missing eft\n", __func__);
			qla27xx_skip_entry(ent, buf);
		}
		break;
	case T268_BUF_TYPE_EXCH_BUFOFF:
		if (vha->hw->exchoffld_buf) {
			if (buf) {
				ent->t268.buf_size = vha->hw->exchoffld_size;
				ent->t268.start_addr =
					vha->hw->exchoffld_buf_dma;
			}
			qla27xx_insertbuf(vha->hw->exchoffld_buf,
			    vha->hw->exchoffld_size, buf, len);
		} else {
			ql_dbg(ql_dbg_misc, vha, 0xd028,
			    "%s: missing exch offld\n", __func__);
			qla27xx_skip_entry(ent, buf);
		}
		break;
	case T268_BUF_TYPE_EXTD_LOGIN:
		if (vha->hw->exlogin_buf) {
			if (buf) {
				ent->t268.buf_size = vha->hw->exlogin_size;
				ent->t268.start_addr =
					vha->hw->exlogin_buf_dma;
			}
			qla27xx_insertbuf(vha->hw->exlogin_buf,
			    vha->hw->exlogin_size, buf, len);
		} else {
			ql_dbg(ql_dbg_misc, vha, 0xd028,
			    "%s: missing ext login\n", __func__);
			qla27xx_skip_entry(ent, buf);
		}
		break;

	case T268_BUF_TYPE_REQ_MIRROR:
	case T268_BUF_TYPE_RSP_MIRROR:
		/*
		 * Mirror pointers are not implemented in the
		 * driver, instead shadow pointers are used by
		 * the drier. Skip these entries.
		 */
		qla27xx_skip_entry(ent, buf);
		break;
	default:
		ql_dbg(ql_dbg_async, vha, 0xd02b,
		    "%s: unknown buffer %x\n", __func__, ent->t268.buf_type);
		qla27xx_skip_entry(ent, buf);
		break;
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t269(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc, vha, 0xd20d,
	    "%s: scratch [%lx]\n", __func__, *len);
	qla27xx_insert32(0xaaaaaaaa, buf, len);
	qla27xx_insert32(0xbbbbbbbb, buf, len);
	qla27xx_insert32(0xcccccccc, buf, len);
	qla27xx_insert32(0xdddddddd, buf, len);
	qla27xx_insert32(*len + sizeof(uint32_t), buf, len);
	if (buf)
		ent->t269.scratch_size = 5 * sizeof(uint32_t);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t270(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong addr = le32_to_cpu(ent->t270.addr);
	ulong dwords = le32_to_cpu(ent->t270.count);

	ql_dbg(ql_dbg_misc, vha, 0xd20e,
	    "%s: rdremreg [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), IOBASE_ADDR, 0x40, buf);
	while (dwords--) {
		qla27xx_write_reg(ISPREG(vha), 0xc0, addr|0x80000000, buf);
		qla27xx_insert32(addr, buf, len);
		qla27xx_read_reg(ISPREG(vha), 0xc4, buf, len);
		addr += sizeof(uint32_t);
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t271(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong addr = le32_to_cpu(ent->t271.addr);
	ulong data = le32_to_cpu(ent->t271.data);

	ql_dbg(ql_dbg_misc, vha, 0xd20f,
	    "%s: wrremreg [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), IOBASE(vha), 0x40, buf);
	qla27xx_write_reg(ISPREG(vha), 0xc4, data, buf);
	qla27xx_write_reg(ISPREG(vha), 0xc0, addr, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t272(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong dwords = le32_to_cpu(ent->t272.count);
	ulong start = le32_to_cpu(ent->t272.addr);

	ql_dbg(ql_dbg_misc, vha, 0xd210,
	    "%s: rdremram [%lx]\n", __func__, *len);
	if (buf) {
		ql_dbg(ql_dbg_misc, vha, 0xd02c,
		    "%s: @%lx -> (%lx dwords)\n", __func__, start, dwords);
		buf += *len;
		qla27xx_dump_mpi_ram(vha->hw, start, buf, dwords, &buf);
	}
	*len += dwords * sizeof(uint32_t);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t273(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong dwords = le32_to_cpu(ent->t273.count);
	ulong addr = le32_to_cpu(ent->t273.addr);
	uint32_t value;

	ql_dbg(ql_dbg_misc, vha, 0xd211,
	    "%s: pcicfg [%lx]\n", __func__, *len);
	while (dwords--) {
		value = ~0;
		if (pci_read_config_dword(vha->hw->pdev, addr, &value))
			ql_dbg(ql_dbg_misc, vha, 0xd02d,
			    "%s: failed pcicfg read at %lx\n", __func__, addr);
		qla27xx_insert32(addr, buf, len);
		qla27xx_insert32(value, buf, len);
		addr += sizeof(uint32_t);
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t274(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong type = ent->t274.queue_type;
	uint count = 0;
	uint i;

	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd212,
	    "%s: getqsh(%lx) [%lx]\n", __func__, type, *len);
	if (type == T274_QUEUE_TYPE_REQ_SHAD) {
		for (i = 0; i < vha->hw->max_req_queues; i++) {
			struct req_que *req = vha->hw->req_q_map[i];

			if (req || !buf) {
				qla27xx_insert16(i, buf, len);
				qla27xx_insert16(1, buf, len);
				qla27xx_insert32(req && req->out_ptr ?
				    *req->out_ptr : 0, buf, len);
				count++;
			}
		}
	} else if (type == T274_QUEUE_TYPE_RSP_SHAD) {
		for (i = 0; i < vha->hw->max_rsp_queues; i++) {
			struct rsp_que *rsp = vha->hw->rsp_q_map[i];

			if (rsp || !buf) {
				qla27xx_insert16(i, buf, len);
				qla27xx_insert16(1, buf, len);
				qla27xx_insert32(rsp && rsp->in_ptr ?
				    *rsp->in_ptr : 0, buf, len);
				count++;
			}
		}
	} else if (QLA_TGT_MODE_ENABLED() &&
	    ent->t274.queue_type == T274_QUEUE_TYPE_ATIO_SHAD) {
		struct qla_hw_data *ha = vha->hw;
		struct atio *atr = ha->tgt.atio_ring_ptr;

		if (atr || !buf) {
			qla27xx_insert16(0, buf, len);
			qla27xx_insert16(1, buf, len);
			qla27xx_insert32(ha->tgt.atio_q_in ?
			    readl(ha->tgt.atio_q_in) : 0, buf, len);
			count++;
		}
	} else {
		ql_dbg(ql_dbg_misc, vha, 0xd02f,
		    "%s: unknown queue %lx\n", __func__, type);
		qla27xx_skip_entry(ent, buf);
	}

	if (buf) {
		if (count)
			ent->t274.num_queues = count;
		else
			qla27xx_skip_entry(ent, buf);
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t275(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong offset = offsetof(typeof(*ent), t275.buffer);
	ulong length = le32_to_cpu(ent->t275.length);
	ulong size = le32_to_cpu(ent->hdr.size);
	void *buffer = ent->t275.buffer;

	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd213,
	    "%s: buffer(%lx) [%lx]\n", __func__, length, *len);
	if (!length) {
		ql_dbg(ql_dbg_misc, vha, 0xd020,
		    "%s: buffer zero length\n", __func__);
		qla27xx_skip_entry(ent, buf);
		goto done;
	}
	if (offset + length > size) {
		length = size - offset;
		ql_dbg(ql_dbg_misc, vha, 0xd030,
		    "%s: buffer overflow, truncate [%lx]\n", __func__, length);
		ent->t275.length = cpu_to_le32(length);
	}

	qla27xx_insertbuf(buffer, length, buf, len);
done:
	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t276(struct scsi_qla_host *vha,
    struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd214,
	    "%s: cond [%lx]\n", __func__, *len);

	if (buf) {
		ulong cond1 = le32_to_cpu(ent->t276.cond1);
		ulong cond2 = le32_to_cpu(ent->t276.cond2);
		uint type = vha->hw->pdev->device >> 4 & 0xf;
		uint func = vha->hw->port_no & 0x3;

		if (type != cond1 || func != cond2) {
			struct qla27xx_fwdt_template *tmp = buf;

			tmp->count--;
			ent = qla27xx_next_entry(ent);
			qla27xx_skip_entry(ent, buf);
		}
	}

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t277(struct scsi_qla_host *vha,
    struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong cmd_addr = le32_to_cpu(ent->t277.cmd_addr);
	ulong wr_cmd_data = le32_to_cpu(ent->t277.wr_cmd_data);
	ulong data_addr = le32_to_cpu(ent->t277.data_addr);

	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd215,
	    "%s: rdpep [%lx]\n", __func__, *len);
	qla27xx_insert32(wr_cmd_data, buf, len);
	qla27xx_write_reg(ISPREG(vha), cmd_addr, wr_cmd_data, buf);
	qla27xx_read_reg(ISPREG(vha), data_addr, buf, len);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_t278(struct scsi_qla_host *vha,
    struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong cmd_addr = le32_to_cpu(ent->t278.cmd_addr);
	ulong wr_cmd_data = le32_to_cpu(ent->t278.wr_cmd_data);
	ulong data_addr = le32_to_cpu(ent->t278.data_addr);
	ulong wr_data = le32_to_cpu(ent->t278.wr_data);

	ql_dbg(ql_dbg_misc + ql_dbg_verbose, vha, 0xd216,
	    "%s: wrpep [%lx]\n", __func__, *len);
	qla27xx_write_reg(ISPREG(vha), data_addr, wr_data, buf);
	qla27xx_write_reg(ISPREG(vha), cmd_addr, wr_cmd_data, buf);

	return qla27xx_next_entry(ent);
}

static struct qla27xx_fwdt_entry *
qla27xx_fwdt_entry_other(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_entry *ent, void *buf, ulong *len)
{
	ulong type = le32_to_cpu(ent->hdr.type);

	ql_dbg(ql_dbg_misc, vha, 0xd2ff,
	    "%s: other %lx [%lx]\n", __func__, type, *len);
	qla27xx_skip_entry(ent, buf);

	return qla27xx_next_entry(ent);
}

static struct {
	uint type;
	typeof(qla27xx_fwdt_entry_other)(*call);
} qla27xx_fwdt_entry_call[] = {
	{ ENTRY_TYPE_NOP,		qla27xx_fwdt_entry_t0    },
	{ ENTRY_TYPE_TMP_END,		qla27xx_fwdt_entry_t255  },
	{ ENTRY_TYPE_RD_IOB_T1,		qla27xx_fwdt_entry_t256  },
	{ ENTRY_TYPE_WR_IOB_T1,		qla27xx_fwdt_entry_t257  },
	{ ENTRY_TYPE_RD_IOB_T2,		qla27xx_fwdt_entry_t258  },
	{ ENTRY_TYPE_WR_IOB_T2,		qla27xx_fwdt_entry_t259  },
	{ ENTRY_TYPE_RD_PCI,		qla27xx_fwdt_entry_t260  },
	{ ENTRY_TYPE_WR_PCI,		qla27xx_fwdt_entry_t261  },
	{ ENTRY_TYPE_RD_RAM,		qla27xx_fwdt_entry_t262  },
	{ ENTRY_TYPE_GET_QUEUE,		qla27xx_fwdt_entry_t263  },
	{ ENTRY_TYPE_GET_FCE,		qla27xx_fwdt_entry_t264  },
	{ ENTRY_TYPE_PSE_RISC,		qla27xx_fwdt_entry_t265  },
	{ ENTRY_TYPE_RST_RISC,		qla27xx_fwdt_entry_t266  },
	{ ENTRY_TYPE_DIS_INTR,		qla27xx_fwdt_entry_t267  },
	{ ENTRY_TYPE_GET_HBUF,		qla27xx_fwdt_entry_t268  },
	{ ENTRY_TYPE_SCRATCH,		qla27xx_fwdt_entry_t269  },
	{ ENTRY_TYPE_RDREMREG,		qla27xx_fwdt_entry_t270  },
	{ ENTRY_TYPE_WRREMREG,		qla27xx_fwdt_entry_t271  },
	{ ENTRY_TYPE_RDREMRAM,		qla27xx_fwdt_entry_t272  },
	{ ENTRY_TYPE_PCICFG,		qla27xx_fwdt_entry_t273  },
	{ ENTRY_TYPE_GET_SHADOW,	qla27xx_fwdt_entry_t274  },
	{ ENTRY_TYPE_WRITE_BUF,		qla27xx_fwdt_entry_t275  },
	{ ENTRY_TYPE_CONDITIONAL,	qla27xx_fwdt_entry_t276  },
	{ ENTRY_TYPE_RDPEPREG,		qla27xx_fwdt_entry_t277  },
	{ ENTRY_TYPE_WRPEPREG,		qla27xx_fwdt_entry_t278  },
	{ -1,				qla27xx_fwdt_entry_other }
};

static inline
typeof(qla27xx_fwdt_entry_call->call)(qla27xx_find_entry(uint type))
{
	typeof(*qla27xx_fwdt_entry_call) *list = qla27xx_fwdt_entry_call;

	while (list->type < type)
		list++;

	if (list->type == type)
		return list->call;
	return qla27xx_fwdt_entry_other;
}

static void
qla27xx_walk_template(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_template *tmp, void *buf, ulong *len)
{
	struct qla27xx_fwdt_entry *ent = (void *)tmp +
	    le32_to_cpu(tmp->entry_offset);
	ulong type;

	tmp->count = le32_to_cpu(tmp->entry_count);
	ql_dbg(ql_dbg_misc, vha, 0xd01a,
	    "%s: entry count %u\n", __func__, tmp->count);
	while (ent && tmp->count--) {
		type = le32_to_cpu(ent->hdr.type);
		ent = qla27xx_find_entry(type)(vha, ent, buf, len);
		if (!ent)
			break;

		if (ent == INVALID_ENTRY) {
			*len = 0;
			ql_dbg(ql_dbg_async, vha, 0xffff,
			    "Unable to capture FW dump");
			goto bailout;
		}
	}

	if (tmp->count)
		ql_dbg(ql_dbg_misc, vha, 0xd018,
		    "%s: entry count residual=+%u\n", __func__, tmp->count);

	if (ent)
		ql_dbg(ql_dbg_misc, vha, 0xd019,
		    "%s: missing end entry\n", __func__);

bailout:
	cpu_to_le32s(&tmp->count);	/* endianize residual count */
}

static void
qla27xx_time_stamp(struct qla27xx_fwdt_template *tmp)
{
	tmp->capture_timestamp = cpu_to_le32(jiffies);
}

static void
qla27xx_driver_info(struct qla27xx_fwdt_template *tmp)
{
	uint8_t v[] = { 0, 0, 0, 0, 0, 0 };

	WARN_ON_ONCE(sscanf(qla2x00_version_str,
			    "%hhu.%hhu.%hhu.%hhu",
			    v + 0, v + 1, v + 2, v + 3) != 4);

	tmp->driver_info[0] = cpu_to_le32(
		v[3] << 24 | v[2] << 16 | v[1] << 8 | v[0]);
	tmp->driver_info[1] = cpu_to_le32(v[5] << 8 | v[4]);
	tmp->driver_info[2] = __constant_cpu_to_le32(0x12345678);
}

static void
qla27xx_firmware_info(struct scsi_qla_host *vha,
    struct qla27xx_fwdt_template *tmp)
{
	tmp->firmware_version[0] = cpu_to_le32(vha->hw->fw_major_version);
	tmp->firmware_version[1] = cpu_to_le32(vha->hw->fw_minor_version);
	tmp->firmware_version[2] = cpu_to_le32(vha->hw->fw_subminor_version);
	tmp->firmware_version[3] = cpu_to_le32(
		vha->hw->fw_attributes_h << 16 | vha->hw->fw_attributes);
	tmp->firmware_version[4] = cpu_to_le32(
	  vha->hw->fw_attributes_ext[1] << 16 | vha->hw->fw_attributes_ext[0]);
}

static void
ql27xx_edit_template(struct scsi_qla_host *vha,
	struct qla27xx_fwdt_template *tmp)
{
	qla27xx_time_stamp(tmp);
	qla27xx_driver_info(tmp);
	qla27xx_firmware_info(vha, tmp);
}

static inline uint32_t
qla27xx_template_checksum(void *p, ulong size)
{
	__le32 *buf = p;
	uint64_t sum = 0;

	size /= sizeof(*buf);

	for ( ; size--; buf++)
		sum += le32_to_cpu(*buf);

	sum = (sum & 0xffffffff) + (sum >> 32);

	return ~sum;
}

static inline int
qla27xx_verify_template_checksum(struct qla27xx_fwdt_template *tmp)
{
	return qla27xx_template_checksum(tmp,
		le32_to_cpu(tmp->template_size)) == 0;
}

static inline int
qla27xx_verify_template_header(struct qla27xx_fwdt_template *tmp)
{
	return le32_to_cpu(tmp->template_type) == TEMPLATE_TYPE_FWDUMP;
}

static ulong
qla27xx_execute_fwdt_template(struct scsi_qla_host *vha,
    struct qla27xx_fwdt_template *tmp, void *buf)
{
	ulong len = 0;

	if (qla27xx_fwdt_template_valid(tmp)) {
		len = le32_to_cpu(tmp->template_size);
		tmp = memcpy(buf, tmp, len);
		ql27xx_edit_template(vha, tmp);
		qla27xx_walk_template(vha, tmp, buf, &len);
	}

	return len;
}

ulong
qla27xx_fwdt_calculate_dump_size(struct scsi_qla_host *vha, void *p)
{
	struct qla27xx_fwdt_template *tmp = p;
	ulong len = 0;

	if (qla27xx_fwdt_template_valid(tmp)) {
		len = le32_to_cpu(tmp->template_size);
		qla27xx_walk_template(vha, tmp, NULL, &len);
	}

	return len;
}

ulong
qla27xx_fwdt_template_size(void *p)
{
	struct qla27xx_fwdt_template *tmp = p;

	return le32_to_cpu(tmp->template_size);
}

int
qla27xx_fwdt_template_valid(void *p)
{
	struct qla27xx_fwdt_template *tmp = p;

	if (!qla27xx_verify_template_header(tmp)) {
		ql_log(ql_log_warn, NULL, 0xd01c,
		    "%s: template type %x\n", __func__,
		    le32_to_cpu(tmp->template_type));
		return false;
	}

	if (!qla27xx_verify_template_checksum(tmp)) {
		ql_log(ql_log_warn, NULL, 0xd01d,
		    "%s: failed template checksum\n", __func__);
		return false;
	}

	return true;
}

void
qla27xx_mpi_fwdump(scsi_qla_host_t *vha, int hardware_locked)
{
	ulong flags = 0;

	if (!hardware_locked)
		spin_lock_irqsave(&vha->hw->hardware_lock, flags);
	if (!vha->hw->mpi_fw_dump) {
		ql_log(ql_log_warn, vha, 0x02f3, "-> mpi_fwdump no buffer\n");
	} else {
		struct fwdt *fwdt = &vha->hw->fwdt[1];
		ulong len;
		void *buf = vha->hw->mpi_fw_dump;
		bool walk_template_only = false;

		if (vha->hw->mpi_fw_dumped) {
			/* Use the spare area for any further dumps. */
			buf += fwdt->dump_size;
			walk_template_only = true;
			ql_log(ql_log_warn, vha, 0x02f4,
			       "-> MPI firmware already dumped -- dump saving to temporary buffer %p.\n",
			       buf);
		}

		ql_log(ql_log_warn, vha, 0x02f5, "-> fwdt1 running...\n");
		if (!fwdt->template) {
			ql_log(ql_log_warn, vha, 0x02f6,
			       "-> fwdt1 no template\n");
			goto bailout;
		}
		len = qla27xx_execute_fwdt_template(vha, fwdt->template, buf);
		if (len == 0) {
			goto bailout;
		} else if (len != fwdt->dump_size) {
			ql_log(ql_log_warn, vha, 0x02f7,
			       "-> fwdt1 fwdump residual=%+ld\n",
			       fwdt->dump_size - len);
		}
		vha->hw->stat.num_mpi_reset++;
		if (walk_template_only)
			goto bailout;

		vha->hw->mpi_fw_dump_len = len;
		vha->hw->mpi_fw_dumped = 1;

		ql_log(ql_log_warn, vha, 0x02f8,
		       "-> MPI firmware dump saved to buffer (%lu/%p)\n",
		       vha->host_no, vha->hw->mpi_fw_dump);
		qla2x00_post_uevent_work(vha, QLA_UEVENT_CODE_FW_DUMP);
	}

bailout:
	if (!hardware_locked)
		spin_unlock_irqrestore(&vha->hw->hardware_lock, flags);
}

void
qla27xx_fwdump(scsi_qla_host_t *vha)
{
	lockdep_assert_held(&vha->hw->hardware_lock);

	if (!vha->hw->fw_dump) {
		ql_log(ql_log_warn, vha, 0xd01e, "-> fwdump no buffer\n");
	} else if (vha->hw->fw_dumped) {
		ql_log(ql_log_warn, vha, 0xd01f,
		    "-> Firmware already dumped (%p) -- ignoring request\n",
		    vha->hw->fw_dump);
	} else {
		struct fwdt *fwdt = vha->hw->fwdt;
		ulong len;
		void *buf = vha->hw->fw_dump;

		ql_log(ql_log_warn, vha, 0xd011, "-> fwdt0 running...\n");
		if (!fwdt->template) {
			ql_log(ql_log_warn, vha, 0xd012,
			       "-> fwdt0 no template\n");
			return;
		}
		len = qla27xx_execute_fwdt_template(vha, fwdt->template, buf);
		if (len == 0) {
			return;
		} else if (len != fwdt->dump_size) {
			ql_log(ql_log_warn, vha, 0xd013,
			       "-> fwdt0 fwdump residual=%+ld\n",
				fwdt->dump_size - len);
		}

		vha->hw->fw_dump_len = len;
		vha->hw->fw_dumped = true;

		ql_log(ql_log_warn, vha, 0xd015,
		    "-> Firmware dump saved to buffer (%lu/%p) <%lx>\n",
		    vha->host_no, vha->hw->fw_dump, vha->hw->fw_dump_cap_flags);
		qla2x00_post_uevent_work(vha, QLA_UEVENT_CODE_FW_DUMP);
	}
}
