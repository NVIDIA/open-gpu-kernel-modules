// SPDX-License-Identifier: GPL-2.0-or-later
/*******************************************************************************
 * Filename:  target_core_rd.c
 *
 * This file contains the Storage Engine <-> Ramdisk transport
 * specific functions.
 *
 * (c) Copyright 2003-2013 Datera, Inc.
 *
 * Nicholas A. Bellinger <nab@kernel.org>
 *
 ******************************************************************************/

#include <linux/string.h>
#include <linux/parser.h>
#include <linux/highmem.h>
#include <linux/timer.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <scsi/scsi_proto.h>

#include <target/target_core_base.h>
#include <target/target_core_backend.h>

#include "target_core_rd.h"

static inline struct rd_dev *RD_DEV(struct se_device *dev)
{
	return container_of(dev, struct rd_dev, dev);
}

static int rd_attach_hba(struct se_hba *hba, u32 host_id)
{
	struct rd_host *rd_host;

	rd_host = kzalloc(sizeof(*rd_host), GFP_KERNEL);
	if (!rd_host)
		return -ENOMEM;

	rd_host->rd_host_id = host_id;

	hba->hba_ptr = rd_host;

	pr_debug("CORE_HBA[%d] - TCM Ramdisk HBA Driver %s on"
		" Generic Target Core Stack %s\n", hba->hba_id,
		RD_HBA_VERSION, TARGET_CORE_VERSION);

	return 0;
}

static void rd_detach_hba(struct se_hba *hba)
{
	struct rd_host *rd_host = hba->hba_ptr;

	pr_debug("CORE_HBA[%d] - Detached Ramdisk HBA: %u from"
		" Generic Target Core\n", hba->hba_id, rd_host->rd_host_id);

	kfree(rd_host);
	hba->hba_ptr = NULL;
}

static u32 rd_release_sgl_table(struct rd_dev *rd_dev, struct rd_dev_sg_table *sg_table,
				 u32 sg_table_count)
{
	struct page *pg;
	struct scatterlist *sg;
	u32 i, j, page_count = 0, sg_per_table;

	for (i = 0; i < sg_table_count; i++) {
		sg = sg_table[i].sg_table;
		sg_per_table = sg_table[i].rd_sg_count;

		for (j = 0; j < sg_per_table; j++) {
			pg = sg_page(&sg[j]);
			if (pg) {
				__free_page(pg);
				page_count++;
			}
		}
		kfree(sg);
	}

	kfree(sg_table);
	return page_count;
}

static void rd_release_device_space(struct rd_dev *rd_dev)
{
	u32 page_count;

	if (!rd_dev->sg_table_array || !rd_dev->sg_table_count)
		return;

	page_count = rd_release_sgl_table(rd_dev, rd_dev->sg_table_array,
					  rd_dev->sg_table_count);

	pr_debug("CORE_RD[%u] - Released device space for Ramdisk"
		" Device ID: %u, pages %u in %u tables total bytes %lu\n",
		rd_dev->rd_host->rd_host_id, rd_dev->rd_dev_id, page_count,
		rd_dev->sg_table_count, (unsigned long)page_count * PAGE_SIZE);

	rd_dev->sg_table_array = NULL;
	rd_dev->sg_table_count = 0;
}


/*	rd_build_device_space():
 *
 *
 */
static int rd_allocate_sgl_table(struct rd_dev *rd_dev, struct rd_dev_sg_table *sg_table,
				 u32 total_sg_needed, unsigned char init_payload)
{
	u32 i = 0, j, page_offset = 0, sg_per_table;
	u32 max_sg_per_table = (RD_MAX_ALLOCATION_SIZE /
				sizeof(struct scatterlist));
	struct page *pg;
	struct scatterlist *sg;
	unsigned char *p;

	while (total_sg_needed) {
		unsigned int chain_entry = 0;

		sg_per_table = (total_sg_needed > max_sg_per_table) ?
			max_sg_per_table : total_sg_needed;

		/*
		 * Reserve extra element for chain entry
		 */
		if (sg_per_table < total_sg_needed)
			chain_entry = 1;

		sg = kmalloc_array(sg_per_table + chain_entry, sizeof(*sg),
				GFP_KERNEL);
		if (!sg)
			return -ENOMEM;

		sg_init_table(sg, sg_per_table + chain_entry);

		if (i > 0) {
			sg_chain(sg_table[i - 1].sg_table,
				 max_sg_per_table + 1, sg);
		}

		sg_table[i].sg_table = sg;
		sg_table[i].rd_sg_count = sg_per_table;
		sg_table[i].page_start_offset = page_offset;
		sg_table[i++].page_end_offset = (page_offset + sg_per_table)
						- 1;

		for (j = 0; j < sg_per_table; j++) {
			pg = alloc_pages(GFP_KERNEL, 0);
			if (!pg) {
				pr_err("Unable to allocate scatterlist"
					" pages for struct rd_dev_sg_table\n");
				return -ENOMEM;
			}
			sg_assign_page(&sg[j], pg);
			sg[j].length = PAGE_SIZE;

			p = kmap(pg);
			memset(p, init_payload, PAGE_SIZE);
			kunmap(pg);
		}

		page_offset += sg_per_table;
		total_sg_needed -= sg_per_table;
	}

	return 0;
}

static int rd_build_device_space(struct rd_dev *rd_dev)
{
	struct rd_dev_sg_table *sg_table;
	u32 sg_tables, total_sg_needed;
	u32 max_sg_per_table = (RD_MAX_ALLOCATION_SIZE /
				sizeof(struct scatterlist));
	int rc;

	if (rd_dev->rd_page_count <= 0) {
		pr_err("Illegal page count: %u for Ramdisk device\n",
		       rd_dev->rd_page_count);
		return -EINVAL;
	}

	/* Don't need backing pages for NULLIO */
	if (rd_dev->rd_flags & RDF_NULLIO)
		return 0;

	total_sg_needed = rd_dev->rd_page_count;

	sg_tables = (total_sg_needed / max_sg_per_table) + 1;
	sg_table = kcalloc(sg_tables, sizeof(*sg_table), GFP_KERNEL);
	if (!sg_table)
		return -ENOMEM;

	rd_dev->sg_table_array = sg_table;
	rd_dev->sg_table_count = sg_tables;

	rc = rd_allocate_sgl_table(rd_dev, sg_table, total_sg_needed, 0x00);
	if (rc)
		return rc;

	pr_debug("CORE_RD[%u] - Built Ramdisk Device ID: %u space of"
		 " %u pages in %u tables\n", rd_dev->rd_host->rd_host_id,
		 rd_dev->rd_dev_id, rd_dev->rd_page_count,
		 rd_dev->sg_table_count);

	return 0;
}

static void rd_release_prot_space(struct rd_dev *rd_dev)
{
	u32 page_count;

	if (!rd_dev->sg_prot_array || !rd_dev->sg_prot_count)
		return;

	page_count = rd_release_sgl_table(rd_dev, rd_dev->sg_prot_array,
					  rd_dev->sg_prot_count);

	pr_debug("CORE_RD[%u] - Released protection space for Ramdisk"
		 " Device ID: %u, pages %u in %u tables total bytes %lu\n",
		 rd_dev->rd_host->rd_host_id, rd_dev->rd_dev_id, page_count,
		 rd_dev->sg_table_count, (unsigned long)page_count * PAGE_SIZE);

	rd_dev->sg_prot_array = NULL;
	rd_dev->sg_prot_count = 0;
}

static int rd_build_prot_space(struct rd_dev *rd_dev, int prot_length, int block_size)
{
	struct rd_dev_sg_table *sg_table;
	u32 total_sg_needed, sg_tables;
	u32 max_sg_per_table = (RD_MAX_ALLOCATION_SIZE /
				sizeof(struct scatterlist));
	int rc;

	if (rd_dev->rd_flags & RDF_NULLIO)
		return 0;
	/*
	 * prot_length=8byte dif data
	 * tot sg needed = rd_page_count * (PGSZ/block_size) *
	 * 		   (prot_length/block_size) + pad
	 * PGSZ canceled each other.
	 */
	total_sg_needed = (rd_dev->rd_page_count * prot_length / block_size) + 1;

	sg_tables = (total_sg_needed / max_sg_per_table) + 1;
	sg_table = kcalloc(sg_tables, sizeof(*sg_table), GFP_KERNEL);
	if (!sg_table)
		return -ENOMEM;

	rd_dev->sg_prot_array = sg_table;
	rd_dev->sg_prot_count = sg_tables;

	rc = rd_allocate_sgl_table(rd_dev, sg_table, total_sg_needed, 0xff);
	if (rc)
		return rc;

	pr_debug("CORE_RD[%u] - Built Ramdisk Device ID: %u prot space of"
		 " %u pages in %u tables\n", rd_dev->rd_host->rd_host_id,
		 rd_dev->rd_dev_id, total_sg_needed, rd_dev->sg_prot_count);

	return 0;
}

static struct se_device *rd_alloc_device(struct se_hba *hba, const char *name)
{
	struct rd_dev *rd_dev;
	struct rd_host *rd_host = hba->hba_ptr;

	rd_dev = kzalloc(sizeof(*rd_dev), GFP_KERNEL);
	if (!rd_dev)
		return NULL;

	rd_dev->rd_host = rd_host;

	return &rd_dev->dev;
}

static int rd_configure_device(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);
	struct rd_host *rd_host = dev->se_hba->hba_ptr;
	int ret;

	if (!(rd_dev->rd_flags & RDF_HAS_PAGE_COUNT)) {
		pr_debug("Missing rd_pages= parameter\n");
		return -EINVAL;
	}

	ret = rd_build_device_space(rd_dev);
	if (ret < 0)
		goto fail;

	dev->dev_attrib.hw_block_size = RD_BLOCKSIZE;
	dev->dev_attrib.hw_max_sectors = UINT_MAX;
	dev->dev_attrib.hw_queue_depth = RD_MAX_DEVICE_QUEUE_DEPTH;
	dev->dev_attrib.is_nonrot = 1;

	rd_dev->rd_dev_id = rd_host->rd_host_dev_id_count++;

	pr_debug("CORE_RD[%u] - Added TCM MEMCPY Ramdisk Device ID: %u of"
		" %u pages in %u tables, %lu total bytes\n",
		rd_host->rd_host_id, rd_dev->rd_dev_id, rd_dev->rd_page_count,
		rd_dev->sg_table_count,
		(unsigned long)(rd_dev->rd_page_count * PAGE_SIZE));

	return 0;

fail:
	rd_release_device_space(rd_dev);
	return ret;
}

static void rd_dev_call_rcu(struct rcu_head *p)
{
	struct se_device *dev = container_of(p, struct se_device, rcu_head);
	struct rd_dev *rd_dev = RD_DEV(dev);

	kfree(rd_dev);
}

static void rd_free_device(struct se_device *dev)
{
	call_rcu(&dev->rcu_head, rd_dev_call_rcu);
}

static void rd_destroy_device(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	rd_release_device_space(rd_dev);
}

static struct rd_dev_sg_table *rd_get_sg_table(struct rd_dev *rd_dev, u32 page)
{
	struct rd_dev_sg_table *sg_table;
	u32 i, sg_per_table = (RD_MAX_ALLOCATION_SIZE /
				sizeof(struct scatterlist));

	i = page / sg_per_table;
	if (i < rd_dev->sg_table_count) {
		sg_table = &rd_dev->sg_table_array[i];
		if ((sg_table->page_start_offset <= page) &&
		    (sg_table->page_end_offset >= page))
			return sg_table;
	}

	pr_err("Unable to locate struct rd_dev_sg_table for page: %u\n",
			page);

	return NULL;
}

static struct rd_dev_sg_table *rd_get_prot_table(struct rd_dev *rd_dev, u32 page)
{
	struct rd_dev_sg_table *sg_table;
	u32 i, sg_per_table = (RD_MAX_ALLOCATION_SIZE /
				sizeof(struct scatterlist));

	i = page / sg_per_table;
	if (i < rd_dev->sg_prot_count) {
		sg_table = &rd_dev->sg_prot_array[i];
		if ((sg_table->page_start_offset <= page) &&
		     (sg_table->page_end_offset >= page))
			return sg_table;
	}

	pr_err("Unable to locate struct prot rd_dev_sg_table for page: %u\n",
			page);

	return NULL;
}

static sense_reason_t rd_do_prot_rw(struct se_cmd *cmd, bool is_read)
{
	struct se_device *se_dev = cmd->se_dev;
	struct rd_dev *dev = RD_DEV(se_dev);
	struct rd_dev_sg_table *prot_table;
	struct scatterlist *prot_sg;
	u32 sectors = cmd->data_length / se_dev->dev_attrib.block_size;
	u32 prot_offset, prot_page;
	u32 prot_npages __maybe_unused;
	u64 tmp;
	sense_reason_t rc = 0;

	tmp = cmd->t_task_lba * se_dev->prot_length;
	prot_offset = do_div(tmp, PAGE_SIZE);
	prot_page = tmp;

	prot_table = rd_get_prot_table(dev, prot_page);
	if (!prot_table)
		return TCM_LOGICAL_UNIT_COMMUNICATION_FAILURE;

	prot_sg = &prot_table->sg_table[prot_page -
					prot_table->page_start_offset];

	if (se_dev->dev_attrib.pi_prot_verify) {
		if (is_read)
			rc = sbc_dif_verify(cmd, cmd->t_task_lba, sectors, 0,
					    prot_sg, prot_offset);
		else
			rc = sbc_dif_verify(cmd, cmd->t_task_lba, sectors, 0,
					    cmd->t_prot_sg, 0);
	}
	if (!rc)
		sbc_dif_copy_prot(cmd, sectors, is_read, prot_sg, prot_offset);

	return rc;
}

static sense_reason_t
rd_execute_rw(struct se_cmd *cmd, struct scatterlist *sgl, u32 sgl_nents,
	      enum dma_data_direction data_direction)
{
	struct se_device *se_dev = cmd->se_dev;
	struct rd_dev *dev = RD_DEV(se_dev);
	struct rd_dev_sg_table *table;
	struct scatterlist *rd_sg;
	struct sg_mapping_iter m;
	u32 rd_offset;
	u32 rd_size;
	u32 rd_page;
	u32 src_len;
	u64 tmp;
	sense_reason_t rc;

	if (dev->rd_flags & RDF_NULLIO) {
		target_complete_cmd(cmd, SAM_STAT_GOOD);
		return 0;
	}

	tmp = cmd->t_task_lba * se_dev->dev_attrib.block_size;
	rd_offset = do_div(tmp, PAGE_SIZE);
	rd_page = tmp;
	rd_size = cmd->data_length;

	table = rd_get_sg_table(dev, rd_page);
	if (!table)
		return TCM_LOGICAL_UNIT_COMMUNICATION_FAILURE;

	rd_sg = &table->sg_table[rd_page - table->page_start_offset];

	pr_debug("RD[%u]: %s LBA: %llu, Size: %u Page: %u, Offset: %u\n",
			dev->rd_dev_id,
			data_direction == DMA_FROM_DEVICE ? "Read" : "Write",
			cmd->t_task_lba, rd_size, rd_page, rd_offset);

	if (cmd->prot_type && se_dev->dev_attrib.pi_prot_type &&
	    data_direction == DMA_TO_DEVICE) {
		rc = rd_do_prot_rw(cmd, false);
		if (rc)
			return rc;
	}

	src_len = PAGE_SIZE - rd_offset;
	sg_miter_start(&m, sgl, sgl_nents,
			data_direction == DMA_FROM_DEVICE ?
				SG_MITER_TO_SG : SG_MITER_FROM_SG);
	while (rd_size) {
		u32 len;
		void *rd_addr;

		sg_miter_next(&m);
		if (!(u32)m.length) {
			pr_debug("RD[%u]: invalid sgl %p len %zu\n",
				 dev->rd_dev_id, m.addr, m.length);
			sg_miter_stop(&m);
			return TCM_INCORRECT_AMOUNT_OF_DATA;
		}
		len = min((u32)m.length, src_len);
		if (len > rd_size) {
			pr_debug("RD[%u]: size underrun page %d offset %d "
				 "size %d\n", dev->rd_dev_id,
				 rd_page, rd_offset, rd_size);
			len = rd_size;
		}
		m.consumed = len;

		rd_addr = sg_virt(rd_sg) + rd_offset;

		if (data_direction == DMA_FROM_DEVICE)
			memcpy(m.addr, rd_addr, len);
		else
			memcpy(rd_addr, m.addr, len);

		rd_size -= len;
		if (!rd_size)
			continue;

		src_len -= len;
		if (src_len) {
			rd_offset += len;
			continue;
		}

		/* rd page completed, next one please */
		rd_page++;
		rd_offset = 0;
		src_len = PAGE_SIZE;
		if (rd_page <= table->page_end_offset) {
			rd_sg++;
			continue;
		}

		table = rd_get_sg_table(dev, rd_page);
		if (!table) {
			sg_miter_stop(&m);
			return TCM_LOGICAL_UNIT_COMMUNICATION_FAILURE;
		}

		/* since we increment, the first sg entry is correct */
		rd_sg = table->sg_table;
	}
	sg_miter_stop(&m);

	if (cmd->prot_type && se_dev->dev_attrib.pi_prot_type &&
	    data_direction == DMA_FROM_DEVICE) {
		rc = rd_do_prot_rw(cmd, true);
		if (rc)
			return rc;
	}

	target_complete_cmd(cmd, SAM_STAT_GOOD);
	return 0;
}

enum {
	Opt_rd_pages, Opt_rd_nullio, Opt_rd_dummy, Opt_err
};

static match_table_t tokens = {
	{Opt_rd_pages, "rd_pages=%d"},
	{Opt_rd_nullio, "rd_nullio=%d"},
	{Opt_rd_dummy, "rd_dummy=%d"},
	{Opt_err, NULL}
};

static ssize_t rd_set_configfs_dev_params(struct se_device *dev,
		const char *page, ssize_t count)
{
	struct rd_dev *rd_dev = RD_DEV(dev);
	char *orig, *ptr, *opts;
	substring_t args[MAX_OPT_ARGS];
	int arg, token;

	opts = kstrdup(page, GFP_KERNEL);
	if (!opts)
		return -ENOMEM;

	orig = opts;

	while ((ptr = strsep(&opts, ",\n")) != NULL) {
		if (!*ptr)
			continue;

		token = match_token(ptr, tokens, args);
		switch (token) {
		case Opt_rd_pages:
			match_int(args, &arg);
			rd_dev->rd_page_count = arg;
			pr_debug("RAMDISK: Referencing Page"
				" Count: %u\n", rd_dev->rd_page_count);
			rd_dev->rd_flags |= RDF_HAS_PAGE_COUNT;
			break;
		case Opt_rd_nullio:
			match_int(args, &arg);
			if (arg != 1)
				break;

			pr_debug("RAMDISK: Setting NULLIO flag: %d\n", arg);
			rd_dev->rd_flags |= RDF_NULLIO;
			break;
		case Opt_rd_dummy:
			match_int(args, &arg);
			if (arg != 1)
				break;

			pr_debug("RAMDISK: Setting DUMMY flag: %d\n", arg);
			rd_dev->rd_flags |= RDF_DUMMY;
			break;
		default:
			break;
		}
	}

	kfree(orig);
	return count;
}

static ssize_t rd_show_configfs_dev_params(struct se_device *dev, char *b)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	ssize_t bl = sprintf(b, "TCM RamDisk ID: %u  RamDisk Makeup: rd_mcp\n",
			rd_dev->rd_dev_id);
	bl += sprintf(b + bl, "        PAGES/PAGE_SIZE: %u*%lu"
			"  SG_table_count: %u  nullio: %d dummy: %d\n",
			rd_dev->rd_page_count,
			PAGE_SIZE, rd_dev->sg_table_count,
			!!(rd_dev->rd_flags & RDF_NULLIO),
			!!(rd_dev->rd_flags & RDF_DUMMY));
	return bl;
}

static u32 rd_get_device_type(struct se_device *dev)
{
	if (RD_DEV(dev)->rd_flags & RDF_DUMMY)
		return 0x3f; /* Unknown device type, not connected */
	else
		return sbc_get_device_type(dev);
}

static sector_t rd_get_blocks(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	unsigned long long blocks_long = ((rd_dev->rd_page_count * PAGE_SIZE) /
			dev->dev_attrib.block_size) - 1;

	return blocks_long;
}

static int rd_init_prot(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

        if (!dev->dev_attrib.pi_prot_type)
		return 0;

	return rd_build_prot_space(rd_dev, dev->prot_length,
				   dev->dev_attrib.block_size);
}

static void rd_free_prot(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	rd_release_prot_space(rd_dev);
}

static struct sbc_ops rd_sbc_ops = {
	.execute_rw		= rd_execute_rw,
};

static sense_reason_t
rd_parse_cdb(struct se_cmd *cmd)
{
	return sbc_parse_cdb(cmd, &rd_sbc_ops);
}

static const struct target_backend_ops rd_mcp_ops = {
	.name			= "rd_mcp",
	.inquiry_prod		= "RAMDISK-MCP",
	.inquiry_rev		= RD_MCP_VERSION,
	.attach_hba		= rd_attach_hba,
	.detach_hba		= rd_detach_hba,
	.alloc_device		= rd_alloc_device,
	.configure_device	= rd_configure_device,
	.destroy_device		= rd_destroy_device,
	.free_device		= rd_free_device,
	.parse_cdb		= rd_parse_cdb,
	.set_configfs_dev_params = rd_set_configfs_dev_params,
	.show_configfs_dev_params = rd_show_configfs_dev_params,
	.get_device_type	= rd_get_device_type,
	.get_blocks		= rd_get_blocks,
	.init_prot		= rd_init_prot,
	.free_prot		= rd_free_prot,
	.tb_dev_attrib_attrs	= sbc_attrib_attrs,
};

int __init rd_module_init(void)
{
	return transport_backend_register(&rd_mcp_ops);
}

void rd_module_exit(void)
{
	target_backend_unregister(&rd_mcp_ops);
}
