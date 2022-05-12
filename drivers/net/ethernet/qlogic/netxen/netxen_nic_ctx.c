// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2003 - 2009 NetXen, Inc.
 * Copyright (C) 2009 - QLogic Corporation.
 * All rights reserved.
 */

#include "netxen_nic_hw.h"
#include "netxen_nic.h"

#define NXHAL_VERSION	1

static u32
netxen_poll_rsp(struct netxen_adapter *adapter)
{
	u32 rsp = NX_CDRP_RSP_OK;
	int	timeout = 0;

	do {
		/* give atleast 1ms for firmware to respond */
		msleep(1);

		if (++timeout > NX_OS_CRB_RETRY_COUNT)
			return NX_CDRP_RSP_TIMEOUT;

		rsp = NXRD32(adapter, NX_CDRP_CRB_OFFSET);
	} while (!NX_CDRP_IS_RSP(rsp));

	return rsp;
}

static u32
netxen_issue_cmd(struct netxen_adapter *adapter, struct netxen_cmd_args *cmd)
{
	u32 rsp;
	u32 signature = 0;
	u32 rcode = NX_RCODE_SUCCESS;

	signature = NX_CDRP_SIGNATURE_MAKE(adapter->ahw.pci_func,
						NXHAL_VERSION);
	/* Acquire semaphore before accessing CRB */
	if (netxen_api_lock(adapter))
		return NX_RCODE_TIMEOUT;

	NXWR32(adapter, NX_SIGN_CRB_OFFSET, signature);

	NXWR32(adapter, NX_ARG1_CRB_OFFSET, cmd->req.arg1);

	NXWR32(adapter, NX_ARG2_CRB_OFFSET, cmd->req.arg2);

	NXWR32(adapter, NX_ARG3_CRB_OFFSET, cmd->req.arg3);

	NXWR32(adapter, NX_CDRP_CRB_OFFSET, NX_CDRP_FORM_CMD(cmd->req.cmd));

	rsp = netxen_poll_rsp(adapter);

	if (rsp == NX_CDRP_RSP_TIMEOUT) {
		printk(KERN_ERR "%s: card response timeout.\n",
				netxen_nic_driver_name);

		rcode = NX_RCODE_TIMEOUT;
	} else if (rsp == NX_CDRP_RSP_FAIL) {
		rcode = NXRD32(adapter, NX_ARG1_CRB_OFFSET);

		printk(KERN_ERR "%s: failed card response code:0x%x\n",
				netxen_nic_driver_name, rcode);
	} else if (rsp == NX_CDRP_RSP_OK) {
		cmd->rsp.cmd = NX_RCODE_SUCCESS;
		if (cmd->rsp.arg2)
			cmd->rsp.arg2 = NXRD32(adapter, NX_ARG2_CRB_OFFSET);
		if (cmd->rsp.arg3)
			cmd->rsp.arg3 = NXRD32(adapter, NX_ARG3_CRB_OFFSET);
	}

	if (cmd->rsp.arg1)
		cmd->rsp.arg1 = NXRD32(adapter, NX_ARG1_CRB_OFFSET);
	/* Release semaphore */
	netxen_api_unlock(adapter);

	return rcode;
}

static int
netxen_get_minidump_template_size(struct netxen_adapter *adapter)
{
	struct netxen_cmd_args cmd;
	memset(&cmd, 0, sizeof(cmd));
	cmd.req.cmd = NX_CDRP_CMD_TEMP_SIZE;
	memset(&cmd.rsp, 1, sizeof(struct _cdrp_cmd));
	netxen_issue_cmd(adapter, &cmd);
	if (cmd.rsp.cmd != NX_RCODE_SUCCESS) {
		dev_info(&adapter->pdev->dev,
			"Can't get template size %d\n", cmd.rsp.cmd);
		return -EIO;
	}
	adapter->mdump.md_template_size = cmd.rsp.arg2;
	adapter->mdump.md_template_ver = cmd.rsp.arg3;
	return 0;
}

static int
netxen_get_minidump_template(struct netxen_adapter *adapter)
{
	dma_addr_t md_template_addr;
	void *addr;
	u32 size;
	struct netxen_cmd_args cmd;
	size = adapter->mdump.md_template_size;

	if (size == 0) {
		dev_err(&adapter->pdev->dev, "Can not capture Minidump "
			"template. Invalid template size.\n");
		return NX_RCODE_INVALID_ARGS;
	}

	addr = dma_alloc_coherent(&adapter->pdev->dev, size,
				  &md_template_addr, GFP_KERNEL);
	if (!addr) {
		dev_err(&adapter->pdev->dev, "Unable to allocate dmable memory for template.\n");
		return -ENOMEM;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&cmd.rsp, 1, sizeof(struct _cdrp_cmd));
	cmd.req.cmd = NX_CDRP_CMD_GET_TEMP_HDR;
	cmd.req.arg1 = LSD(md_template_addr);
	cmd.req.arg2 = MSD(md_template_addr);
	cmd.req.arg3 |= size;
	netxen_issue_cmd(adapter, &cmd);

	if ((cmd.rsp.cmd == NX_RCODE_SUCCESS) && (size == cmd.rsp.arg2)) {
		memcpy(adapter->mdump.md_template, addr, size);
	} else {
		dev_err(&adapter->pdev->dev, "Failed to get minidump template, err_code : %d, requested_size : %d, actual_size : %d\n",
			cmd.rsp.cmd, size, cmd.rsp.arg2);
	}
	dma_free_coherent(&adapter->pdev->dev, size, addr, md_template_addr);
	return 0;
}

static u32
netxen_check_template_checksum(struct netxen_adapter *adapter)
{
	u64 sum =  0 ;
	u32 *buff = adapter->mdump.md_template;
	int count =  adapter->mdump.md_template_size/sizeof(uint32_t) ;

	while (count-- > 0)
		sum += *buff++ ;
	while (sum >> 32)
		sum = (sum & 0xFFFFFFFF) +  (sum >> 32) ;

	return ~sum;
}

int
netxen_setup_minidump(struct netxen_adapter *adapter)
{
	int err = 0, i;
	u32 *template, *tmp_buf;
	err = netxen_get_minidump_template_size(adapter);
	if (err) {
		adapter->mdump.fw_supports_md = 0;
		if ((err == NX_RCODE_CMD_INVALID) ||
			(err == NX_RCODE_CMD_NOT_IMPL)) {
			dev_info(&adapter->pdev->dev,
				"Flashed firmware version does not support minidump, minimum version required is [ %u.%u.%u ]\n",
				NX_MD_SUPPORT_MAJOR, NX_MD_SUPPORT_MINOR,
				NX_MD_SUPPORT_SUBVERSION);
		}
		return err;
	}

	if (!adapter->mdump.md_template_size) {
		dev_err(&adapter->pdev->dev, "Error : Invalid template size "
		",should be non-zero.\n");
		return -EIO;
	}
	adapter->mdump.md_template =
		kmalloc(adapter->mdump.md_template_size, GFP_KERNEL);

	if (!adapter->mdump.md_template)
		return -ENOMEM;

	err = netxen_get_minidump_template(adapter);
	if (err) {
		if (err == NX_RCODE_CMD_NOT_IMPL)
			adapter->mdump.fw_supports_md = 0;
		goto free_template;
	}

	if (netxen_check_template_checksum(adapter)) {
		dev_err(&adapter->pdev->dev, "Minidump template checksum Error\n");
		err = -EIO;
		goto free_template;
	}

	adapter->mdump.md_capture_mask = NX_DUMP_MASK_DEF;
	tmp_buf = (u32 *) adapter->mdump.md_template;
	template = (u32 *) adapter->mdump.md_template;
	for (i = 0; i < adapter->mdump.md_template_size/sizeof(u32); i++)
		*template++ = __le32_to_cpu(*tmp_buf++);
	adapter->mdump.md_capture_buff = NULL;
	adapter->mdump.fw_supports_md = 1;
	adapter->mdump.md_enabled = 0;

	return err;

free_template:
	kfree(adapter->mdump.md_template);
	adapter->mdump.md_template = NULL;
	return err;
}


int
nx_fw_cmd_set_mtu(struct netxen_adapter *adapter, int mtu)
{
	u32 rcode = NX_RCODE_SUCCESS;
	struct netxen_recv_context *recv_ctx = &adapter->recv_ctx;
	struct netxen_cmd_args cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.req.cmd = NX_CDRP_CMD_SET_MTU;
	cmd.req.arg1 = recv_ctx->context_id;
	cmd.req.arg2 = mtu;
	cmd.req.arg3 = 0;

	if (recv_ctx->state == NX_HOST_CTX_STATE_ACTIVE)
		rcode = netxen_issue_cmd(adapter, &cmd);

	if (rcode != NX_RCODE_SUCCESS)
		return -EIO;

	return 0;
}

int
nx_fw_cmd_set_gbe_port(struct netxen_adapter *adapter,
			u32 speed, u32 duplex, u32 autoneg)
{
	struct netxen_cmd_args cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.req.cmd = NX_CDRP_CMD_CONFIG_GBE_PORT;
	cmd.req.arg1 = speed;
	cmd.req.arg2 = duplex;
	cmd.req.arg3 = autoneg;
	return netxen_issue_cmd(adapter, &cmd);
}

static int
nx_fw_cmd_create_rx_ctx(struct netxen_adapter *adapter)
{
	void *addr;
	nx_hostrq_rx_ctx_t *prq;
	nx_cardrsp_rx_ctx_t *prsp;
	nx_hostrq_rds_ring_t *prq_rds;
	nx_hostrq_sds_ring_t *prq_sds;
	nx_cardrsp_rds_ring_t *prsp_rds;
	nx_cardrsp_sds_ring_t *prsp_sds;
	struct nx_host_rds_ring *rds_ring;
	struct nx_host_sds_ring *sds_ring;
	struct netxen_cmd_args cmd;

	dma_addr_t hostrq_phys_addr, cardrsp_phys_addr;
	u64 phys_addr;

	int i, nrds_rings, nsds_rings;
	size_t rq_size, rsp_size;
	u32 cap, reg, val;

	int err;

	struct netxen_recv_context *recv_ctx = &adapter->recv_ctx;

	nrds_rings = adapter->max_rds_rings;
	nsds_rings = adapter->max_sds_rings;

	rq_size =
		SIZEOF_HOSTRQ_RX(nx_hostrq_rx_ctx_t, nrds_rings, nsds_rings);
	rsp_size =
		SIZEOF_CARDRSP_RX(nx_cardrsp_rx_ctx_t, nrds_rings, nsds_rings);

	addr = dma_alloc_coherent(&adapter->pdev->dev, rq_size,
				  &hostrq_phys_addr, GFP_KERNEL);
	if (addr == NULL)
		return -ENOMEM;
	prq = addr;

	addr = dma_alloc_coherent(&adapter->pdev->dev, rsp_size,
				  &cardrsp_phys_addr, GFP_KERNEL);
	if (addr == NULL) {
		err = -ENOMEM;
		goto out_free_rq;
	}
	prsp = addr;

	prq->host_rsp_dma_addr = cpu_to_le64(cardrsp_phys_addr);

	cap = (NX_CAP0_LEGACY_CONTEXT | NX_CAP0_LEGACY_MN);
	cap |= (NX_CAP0_JUMBO_CONTIGUOUS | NX_CAP0_LRO_CONTIGUOUS);

	if (adapter->flags & NETXEN_FW_MSS_CAP)
		cap |= NX_CAP0_HW_LRO_MSS;

	prq->capabilities[0] = cpu_to_le32(cap);
	prq->host_int_crb_mode =
		cpu_to_le32(NX_HOST_INT_CRB_MODE_SHARED);
	prq->host_rds_crb_mode =
		cpu_to_le32(NX_HOST_RDS_CRB_MODE_UNIQUE);

	prq->num_rds_rings = cpu_to_le16(nrds_rings);
	prq->num_sds_rings = cpu_to_le16(nsds_rings);
	prq->rds_ring_offset = cpu_to_le32(0);

	val = le32_to_cpu(prq->rds_ring_offset) +
		(sizeof(nx_hostrq_rds_ring_t) * nrds_rings);
	prq->sds_ring_offset = cpu_to_le32(val);

	prq_rds = (nx_hostrq_rds_ring_t *)(prq->data +
			le32_to_cpu(prq->rds_ring_offset));

	for (i = 0; i < nrds_rings; i++) {

		rds_ring = &recv_ctx->rds_rings[i];

		prq_rds[i].host_phys_addr = cpu_to_le64(rds_ring->phys_addr);
		prq_rds[i].ring_size = cpu_to_le32(rds_ring->num_desc);
		prq_rds[i].ring_kind = cpu_to_le32(i);
		prq_rds[i].buff_size = cpu_to_le64(rds_ring->dma_size);
	}

	prq_sds = (nx_hostrq_sds_ring_t *)(prq->data +
			le32_to_cpu(prq->sds_ring_offset));

	for (i = 0; i < nsds_rings; i++) {

		sds_ring = &recv_ctx->sds_rings[i];

		prq_sds[i].host_phys_addr = cpu_to_le64(sds_ring->phys_addr);
		prq_sds[i].ring_size = cpu_to_le32(sds_ring->num_desc);
		prq_sds[i].msi_index = cpu_to_le16(i);
	}

	phys_addr = hostrq_phys_addr;
	memset(&cmd, 0, sizeof(cmd));
	cmd.req.arg1 = (u32)(phys_addr >> 32);
	cmd.req.arg2 = (u32)(phys_addr & 0xffffffff);
	cmd.req.arg3 = rq_size;
	cmd.req.cmd = NX_CDRP_CMD_CREATE_RX_CTX;
	err = netxen_issue_cmd(adapter, &cmd);
	if (err) {
		printk(KERN_WARNING
			"Failed to create rx ctx in firmware%d\n", err);
		goto out_free_rsp;
	}


	prsp_rds = ((nx_cardrsp_rds_ring_t *)
			 &prsp->data[le32_to_cpu(prsp->rds_ring_offset)]);

	for (i = 0; i < le16_to_cpu(prsp->num_rds_rings); i++) {
		rds_ring = &recv_ctx->rds_rings[i];

		reg = le32_to_cpu(prsp_rds[i].host_producer_crb);
		rds_ring->crb_rcv_producer = netxen_get_ioaddr(adapter,
				NETXEN_NIC_REG(reg - 0x200));
	}

	prsp_sds = ((nx_cardrsp_sds_ring_t *)
			&prsp->data[le32_to_cpu(prsp->sds_ring_offset)]);

	for (i = 0; i < le16_to_cpu(prsp->num_sds_rings); i++) {
		sds_ring = &recv_ctx->sds_rings[i];

		reg = le32_to_cpu(prsp_sds[i].host_consumer_crb);
		sds_ring->crb_sts_consumer = netxen_get_ioaddr(adapter,
				NETXEN_NIC_REG(reg - 0x200));

		reg = le32_to_cpu(prsp_sds[i].interrupt_crb);
		sds_ring->crb_intr_mask = netxen_get_ioaddr(adapter,
				NETXEN_NIC_REG(reg - 0x200));
	}

	recv_ctx->state = le32_to_cpu(prsp->host_ctx_state);
	recv_ctx->context_id = le16_to_cpu(prsp->context_id);
	recv_ctx->virt_port = prsp->virt_port;

out_free_rsp:
	dma_free_coherent(&adapter->pdev->dev, rsp_size, prsp,
			  cardrsp_phys_addr);
out_free_rq:
	dma_free_coherent(&adapter->pdev->dev, rq_size, prq, hostrq_phys_addr);
	return err;
}

static void
nx_fw_cmd_destroy_rx_ctx(struct netxen_adapter *adapter)
{
	struct netxen_recv_context *recv_ctx = &adapter->recv_ctx;
	struct netxen_cmd_args cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.req.arg1 = recv_ctx->context_id;
	cmd.req.arg2 = NX_DESTROY_CTX_RESET;
	cmd.req.arg3 = 0;
	cmd.req.cmd = NX_CDRP_CMD_DESTROY_RX_CTX;

	if (netxen_issue_cmd(adapter, &cmd)) {
		printk(KERN_WARNING
			"%s: Failed to destroy rx ctx in firmware\n",
			netxen_nic_driver_name);
	}
}

static int
nx_fw_cmd_create_tx_ctx(struct netxen_adapter *adapter)
{
	nx_hostrq_tx_ctx_t	*prq;
	nx_hostrq_cds_ring_t	*prq_cds;
	nx_cardrsp_tx_ctx_t	*prsp;
	void	*rq_addr, *rsp_addr;
	size_t	rq_size, rsp_size;
	u32	temp;
	int	err = 0;
	u64	offset, phys_addr;
	dma_addr_t	rq_phys_addr, rsp_phys_addr;
	struct nx_host_tx_ring *tx_ring = adapter->tx_ring;
	struct netxen_recv_context *recv_ctx = &adapter->recv_ctx;
	struct netxen_cmd_args cmd;

	rq_size = SIZEOF_HOSTRQ_TX(nx_hostrq_tx_ctx_t);
	rq_addr = dma_alloc_coherent(&adapter->pdev->dev, rq_size,
				     &rq_phys_addr, GFP_KERNEL);
	if (!rq_addr)
		return -ENOMEM;

	rsp_size = SIZEOF_CARDRSP_TX(nx_cardrsp_tx_ctx_t);
	rsp_addr = dma_alloc_coherent(&adapter->pdev->dev, rsp_size,
				      &rsp_phys_addr, GFP_KERNEL);
	if (!rsp_addr) {
		err = -ENOMEM;
		goto out_free_rq;
	}

	prq = rq_addr;

	prsp = rsp_addr;

	prq->host_rsp_dma_addr = cpu_to_le64(rsp_phys_addr);

	temp = (NX_CAP0_LEGACY_CONTEXT | NX_CAP0_LEGACY_MN | NX_CAP0_LSO);
	prq->capabilities[0] = cpu_to_le32(temp);

	prq->host_int_crb_mode =
		cpu_to_le32(NX_HOST_INT_CRB_MODE_SHARED);

	prq->interrupt_ctl = 0;
	prq->msi_index = 0;

	prq->dummy_dma_addr = cpu_to_le64(adapter->dummy_dma.phys_addr);

	offset = recv_ctx->phys_addr + sizeof(struct netxen_ring_ctx);
	prq->cmd_cons_dma_addr = cpu_to_le64(offset);

	prq_cds = &prq->cds_ring;

	prq_cds->host_phys_addr = cpu_to_le64(tx_ring->phys_addr);
	prq_cds->ring_size = cpu_to_le32(tx_ring->num_desc);

	phys_addr = rq_phys_addr;
	memset(&cmd, 0, sizeof(cmd));
	cmd.req.arg1 = (u32)(phys_addr >> 32);
	cmd.req.arg2 = ((u32)phys_addr & 0xffffffff);
	cmd.req.arg3 = rq_size;
	cmd.req.cmd = NX_CDRP_CMD_CREATE_TX_CTX;
	err = netxen_issue_cmd(adapter, &cmd);

	if (err == NX_RCODE_SUCCESS) {
		temp = le32_to_cpu(prsp->cds_ring.host_producer_crb);
		tx_ring->crb_cmd_producer = netxen_get_ioaddr(adapter,
				NETXEN_NIC_REG(temp - 0x200));
#if 0
		adapter->tx_state =
			le32_to_cpu(prsp->host_ctx_state);
#endif
		adapter->tx_context_id =
			le16_to_cpu(prsp->context_id);
	} else {
		printk(KERN_WARNING
			"Failed to create tx ctx in firmware%d\n", err);
		err = -EIO;
	}

	dma_free_coherent(&adapter->pdev->dev, rsp_size, rsp_addr,
			  rsp_phys_addr);

out_free_rq:
	dma_free_coherent(&adapter->pdev->dev, rq_size, rq_addr, rq_phys_addr);

	return err;
}

static void
nx_fw_cmd_destroy_tx_ctx(struct netxen_adapter *adapter)
{
	struct netxen_cmd_args cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.req.arg1 = adapter->tx_context_id;
	cmd.req.arg2 = NX_DESTROY_CTX_RESET;
	cmd.req.arg3 = 0;
	cmd.req.cmd = NX_CDRP_CMD_DESTROY_TX_CTX;
	if (netxen_issue_cmd(adapter, &cmd)) {
		printk(KERN_WARNING
			"%s: Failed to destroy tx ctx in firmware\n",
			netxen_nic_driver_name);
	}
}

int
nx_fw_cmd_query_phy(struct netxen_adapter *adapter, u32 reg, u32 *val)
{
	u32 rcode;
	struct netxen_cmd_args cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.req.arg1 = reg;
	cmd.req.arg2 = 0;
	cmd.req.arg3 = 0;
	cmd.req.cmd = NX_CDRP_CMD_READ_PHY;
	cmd.rsp.arg1 = 1;
	rcode = netxen_issue_cmd(adapter, &cmd);
	if (rcode != NX_RCODE_SUCCESS)
		return -EIO;

	if (val == NULL)
		return -EIO;

	*val = cmd.rsp.arg1;
	return 0;
}

int
nx_fw_cmd_set_phy(struct netxen_adapter *adapter, u32 reg, u32 val)
{
	u32 rcode;
	struct netxen_cmd_args cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.req.arg1 = reg;
	cmd.req.arg2 = val;
	cmd.req.arg3 = 0;
	cmd.req.cmd = NX_CDRP_CMD_WRITE_PHY;
	rcode = netxen_issue_cmd(adapter, &cmd);
	if (rcode != NX_RCODE_SUCCESS)
		return -EIO;

	return 0;
}

static u64 ctx_addr_sig_regs[][3] = {
	{NETXEN_NIC_REG(0x188), NETXEN_NIC_REG(0x18c), NETXEN_NIC_REG(0x1c0)},
	{NETXEN_NIC_REG(0x190), NETXEN_NIC_REG(0x194), NETXEN_NIC_REG(0x1c4)},
	{NETXEN_NIC_REG(0x198), NETXEN_NIC_REG(0x19c), NETXEN_NIC_REG(0x1c8)},
	{NETXEN_NIC_REG(0x1a0), NETXEN_NIC_REG(0x1a4), NETXEN_NIC_REG(0x1cc)}
};

#define CRB_CTX_ADDR_REG_LO(FUNC_ID)	(ctx_addr_sig_regs[FUNC_ID][0])
#define CRB_CTX_ADDR_REG_HI(FUNC_ID)	(ctx_addr_sig_regs[FUNC_ID][2])
#define CRB_CTX_SIGNATURE_REG(FUNC_ID)	(ctx_addr_sig_regs[FUNC_ID][1])

#define lower32(x)	((u32)((x) & 0xffffffff))
#define upper32(x)	((u32)(((u64)(x) >> 32) & 0xffffffff))

static struct netxen_recv_crb recv_crb_registers[] = {
	/* Instance 0 */
	{
		/* crb_rcv_producer: */
		{
			NETXEN_NIC_REG(0x100),
			/* Jumbo frames */
			NETXEN_NIC_REG(0x110),
			/* LRO */
			NETXEN_NIC_REG(0x120)
		},
		/* crb_sts_consumer: */
		{
			NETXEN_NIC_REG(0x138),
			NETXEN_NIC_REG_2(0x000),
			NETXEN_NIC_REG_2(0x004),
			NETXEN_NIC_REG_2(0x008),
		},
		/* sw_int_mask */
		{
			CRB_SW_INT_MASK_0,
			NETXEN_NIC_REG_2(0x044),
			NETXEN_NIC_REG_2(0x048),
			NETXEN_NIC_REG_2(0x04c),
		},
	},
	/* Instance 1 */
	{
		/* crb_rcv_producer: */
		{
			NETXEN_NIC_REG(0x144),
			/* Jumbo frames */
			NETXEN_NIC_REG(0x154),
			/* LRO */
			NETXEN_NIC_REG(0x164)
		},
		/* crb_sts_consumer: */
		{
			NETXEN_NIC_REG(0x17c),
			NETXEN_NIC_REG_2(0x020),
			NETXEN_NIC_REG_2(0x024),
			NETXEN_NIC_REG_2(0x028),
		},
		/* sw_int_mask */
		{
			CRB_SW_INT_MASK_1,
			NETXEN_NIC_REG_2(0x064),
			NETXEN_NIC_REG_2(0x068),
			NETXEN_NIC_REG_2(0x06c),
		},
	},
	/* Instance 2 */
	{
		/* crb_rcv_producer: */
		{
			NETXEN_NIC_REG(0x1d8),
			/* Jumbo frames */
			NETXEN_NIC_REG(0x1f8),
			/* LRO */
			NETXEN_NIC_REG(0x208)
		},
		/* crb_sts_consumer: */
		{
			NETXEN_NIC_REG(0x220),
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
		},
		/* sw_int_mask */
		{
			CRB_SW_INT_MASK_2,
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
		},
	},
	/* Instance 3 */
	{
		/* crb_rcv_producer: */
		{
			NETXEN_NIC_REG(0x22c),
			/* Jumbo frames */
			NETXEN_NIC_REG(0x23c),
			/* LRO */
			NETXEN_NIC_REG(0x24c)
		},
		/* crb_sts_consumer: */
		{
			NETXEN_NIC_REG(0x264),
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
		},
		/* sw_int_mask */
		{
			CRB_SW_INT_MASK_3,
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
			NETXEN_NIC_REG_2(0x03c),
		},
	},
};

static int
netxen_init_old_ctx(struct netxen_adapter *adapter)
{
	struct netxen_recv_context *recv_ctx;
	struct nx_host_rds_ring *rds_ring;
	struct nx_host_sds_ring *sds_ring;
	struct nx_host_tx_ring *tx_ring;
	int ring;
	int port = adapter->portnum;
	struct netxen_ring_ctx *hwctx;
	u32 signature;

	tx_ring = adapter->tx_ring;
	recv_ctx = &adapter->recv_ctx;
	hwctx = recv_ctx->hwctx;

	hwctx->cmd_ring_addr = cpu_to_le64(tx_ring->phys_addr);
	hwctx->cmd_ring_size = cpu_to_le32(tx_ring->num_desc);


	for (ring = 0; ring < adapter->max_rds_rings; ring++) {
		rds_ring = &recv_ctx->rds_rings[ring];

		hwctx->rcv_rings[ring].addr =
			cpu_to_le64(rds_ring->phys_addr);
		hwctx->rcv_rings[ring].size =
			cpu_to_le32(rds_ring->num_desc);
	}

	for (ring = 0; ring < adapter->max_sds_rings; ring++) {
		sds_ring = &recv_ctx->sds_rings[ring];

		if (ring == 0) {
			hwctx->sts_ring_addr = cpu_to_le64(sds_ring->phys_addr);
			hwctx->sts_ring_size = cpu_to_le32(sds_ring->num_desc);
		}
		hwctx->sts_rings[ring].addr = cpu_to_le64(sds_ring->phys_addr);
		hwctx->sts_rings[ring].size = cpu_to_le32(sds_ring->num_desc);
		hwctx->sts_rings[ring].msi_index = cpu_to_le16(ring);
	}
	hwctx->sts_ring_count = cpu_to_le32(adapter->max_sds_rings);

	signature = (adapter->max_sds_rings > 1) ?
		NETXEN_CTX_SIGNATURE_V2 : NETXEN_CTX_SIGNATURE;

	NXWR32(adapter, CRB_CTX_ADDR_REG_LO(port),
			lower32(recv_ctx->phys_addr));
	NXWR32(adapter, CRB_CTX_ADDR_REG_HI(port),
			upper32(recv_ctx->phys_addr));
	NXWR32(adapter, CRB_CTX_SIGNATURE_REG(port),
			signature | port);
	return 0;
}

int netxen_alloc_hw_resources(struct netxen_adapter *adapter)
{
	void *addr;
	int err = 0;
	int ring;
	struct netxen_recv_context *recv_ctx;
	struct nx_host_rds_ring *rds_ring;
	struct nx_host_sds_ring *sds_ring;
	struct nx_host_tx_ring *tx_ring;

	struct pci_dev *pdev = adapter->pdev;
	struct net_device *netdev = adapter->netdev;
	int port = adapter->portnum;

	recv_ctx = &adapter->recv_ctx;
	tx_ring = adapter->tx_ring;

	addr = dma_alloc_coherent(&pdev->dev,
				  sizeof(struct netxen_ring_ctx) + sizeof(uint32_t),
				  &recv_ctx->phys_addr, GFP_KERNEL);
	if (addr == NULL) {
		dev_err(&pdev->dev, "failed to allocate hw context\n");
		return -ENOMEM;
	}

	recv_ctx->hwctx = addr;
	recv_ctx->hwctx->ctx_id = cpu_to_le32(port);
	recv_ctx->hwctx->cmd_consumer_offset =
		cpu_to_le64(recv_ctx->phys_addr +
			sizeof(struct netxen_ring_ctx));
	tx_ring->hw_consumer =
		(__le32 *)(((char *)addr) + sizeof(struct netxen_ring_ctx));

	/* cmd desc ring */
	addr = dma_alloc_coherent(&pdev->dev, TX_DESC_RINGSIZE(tx_ring),
				  &tx_ring->phys_addr, GFP_KERNEL);

	if (addr == NULL) {
		dev_err(&pdev->dev, "%s: failed to allocate tx desc ring\n",
				netdev->name);
		err = -ENOMEM;
		goto err_out_free;
	}

	tx_ring->desc_head = addr;

	for (ring = 0; ring < adapter->max_rds_rings; ring++) {
		rds_ring = &recv_ctx->rds_rings[ring];
		addr = dma_alloc_coherent(&adapter->pdev->dev,
					  RCV_DESC_RINGSIZE(rds_ring),
					  &rds_ring->phys_addr, GFP_KERNEL);
		if (addr == NULL) {
			dev_err(&pdev->dev,
				"%s: failed to allocate rds ring [%d]\n",
				netdev->name, ring);
			err = -ENOMEM;
			goto err_out_free;
		}
		rds_ring->desc_head = addr;

		if (NX_IS_REVISION_P2(adapter->ahw.revision_id))
			rds_ring->crb_rcv_producer =
				netxen_get_ioaddr(adapter,
			recv_crb_registers[port].crb_rcv_producer[ring]);
	}

	for (ring = 0; ring < adapter->max_sds_rings; ring++) {
		sds_ring = &recv_ctx->sds_rings[ring];

		addr = dma_alloc_coherent(&adapter->pdev->dev,
					  STATUS_DESC_RINGSIZE(sds_ring),
					  &sds_ring->phys_addr, GFP_KERNEL);
		if (addr == NULL) {
			dev_err(&pdev->dev,
				"%s: failed to allocate sds ring [%d]\n",
				netdev->name, ring);
			err = -ENOMEM;
			goto err_out_free;
		}
		sds_ring->desc_head = addr;

		if (NX_IS_REVISION_P2(adapter->ahw.revision_id)) {
			sds_ring->crb_sts_consumer =
				netxen_get_ioaddr(adapter,
				recv_crb_registers[port].crb_sts_consumer[ring]);

			sds_ring->crb_intr_mask =
				netxen_get_ioaddr(adapter,
				recv_crb_registers[port].sw_int_mask[ring]);
		}
	}


	if (!NX_IS_REVISION_P2(adapter->ahw.revision_id)) {
		if (test_and_set_bit(__NX_FW_ATTACHED, &adapter->state))
			goto done;
		err = nx_fw_cmd_create_rx_ctx(adapter);
		if (err)
			goto err_out_free;
		err = nx_fw_cmd_create_tx_ctx(adapter);
		if (err)
			goto err_out_free;
	} else {
		err = netxen_init_old_ctx(adapter);
		if (err)
			goto err_out_free;
	}

done:
	return 0;

err_out_free:
	netxen_free_hw_resources(adapter);
	return err;
}

void netxen_free_hw_resources(struct netxen_adapter *adapter)
{
	struct netxen_recv_context *recv_ctx;
	struct nx_host_rds_ring *rds_ring;
	struct nx_host_sds_ring *sds_ring;
	struct nx_host_tx_ring *tx_ring;
	int ring;

	int port = adapter->portnum;

	if (!NX_IS_REVISION_P2(adapter->ahw.revision_id)) {
		if (!test_and_clear_bit(__NX_FW_ATTACHED, &adapter->state))
			goto done;

		nx_fw_cmd_destroy_rx_ctx(adapter);
		nx_fw_cmd_destroy_tx_ctx(adapter);
	} else {
		netxen_api_lock(adapter);
		NXWR32(adapter, CRB_CTX_SIGNATURE_REG(port),
				NETXEN_CTX_D3_RESET | port);
		netxen_api_unlock(adapter);
	}

	/* Allow dma queues to drain after context reset */
	msleep(20);

done:
	recv_ctx = &adapter->recv_ctx;

	if (recv_ctx->hwctx != NULL) {
		dma_free_coherent(&adapter->pdev->dev,
				  sizeof(struct netxen_ring_ctx) + sizeof(uint32_t),
				  recv_ctx->hwctx, recv_ctx->phys_addr);
		recv_ctx->hwctx = NULL;
	}

	tx_ring = adapter->tx_ring;
	if (tx_ring->desc_head != NULL) {
		dma_free_coherent(&adapter->pdev->dev,
				  TX_DESC_RINGSIZE(tx_ring),
				  tx_ring->desc_head, tx_ring->phys_addr);
		tx_ring->desc_head = NULL;
	}

	for (ring = 0; ring < adapter->max_rds_rings; ring++) {
		rds_ring = &recv_ctx->rds_rings[ring];

		if (rds_ring->desc_head != NULL) {
			dma_free_coherent(&adapter->pdev->dev,
					  RCV_DESC_RINGSIZE(rds_ring),
					  rds_ring->desc_head,
					  rds_ring->phys_addr);
			rds_ring->desc_head = NULL;
		}
	}

	for (ring = 0; ring < adapter->max_sds_rings; ring++) {
		sds_ring = &recv_ctx->sds_rings[ring];

		if (sds_ring->desc_head != NULL) {
			dma_free_coherent(&adapter->pdev->dev,
					  STATUS_DESC_RINGSIZE(sds_ring),
					  sds_ring->desc_head,
					  sds_ring->phys_addr);
			sds_ring->desc_head = NULL;
		}
	}
}

