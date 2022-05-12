// SPDX-License-Identifier: GPL-2.0+
/* Copyright (c) 2018-2019 Hisilicon Limited. */

#include <linux/debugfs.h>
#include <linux/device.h>

#include "hnae3.h"
#include "hns3_enet.h"

#define HNS3_DBG_READ_LEN 65536
#define HNS3_DBG_WRITE_LEN 1024

static struct dentry *hns3_dbgfs_root;

static int hns3_dbg_queue_info(struct hnae3_handle *h,
			       const char *cmd_buf)
{
	struct hnae3_ae_dev *ae_dev = pci_get_drvdata(h->pdev);
	struct hns3_nic_priv *priv = h->priv;
	struct hns3_enet_ring *ring;
	u32 base_add_l, base_add_h;
	u32 queue_num, queue_max;
	u32 value, i;
	int cnt;

	if (!priv->ring) {
		dev_err(&h->pdev->dev, "priv->ring is NULL\n");
		return -EFAULT;
	}

	queue_max = h->kinfo.num_tqps;
	cnt = kstrtouint(&cmd_buf[11], 0, &queue_num);
	if (cnt)
		queue_num = 0;
	else
		queue_max = queue_num + 1;

	dev_info(&h->pdev->dev, "queue info\n");

	if (queue_num >= h->kinfo.num_tqps) {
		dev_err(&h->pdev->dev,
			"Queue number(%u) is out of range(0-%u)\n", queue_num,
			h->kinfo.num_tqps - 1);
		return -EINVAL;
	}

	for (i = queue_num; i < queue_max; i++) {
		/* Each cycle needs to determine whether the instance is reset,
		 * to prevent reference to invalid memory. And need to ensure
		 * that the following code is executed within 100ms.
		 */
		if (!test_bit(HNS3_NIC_STATE_INITED, &priv->state) ||
		    test_bit(HNS3_NIC_STATE_RESETTING, &priv->state))
			return -EPERM;

		ring = &priv->ring[(u32)(i + h->kinfo.num_tqps)];
		base_add_h = readl_relaxed(ring->tqp->io_base +
					   HNS3_RING_RX_RING_BASEADDR_H_REG);
		base_add_l = readl_relaxed(ring->tqp->io_base +
					   HNS3_RING_RX_RING_BASEADDR_L_REG);
		dev_info(&h->pdev->dev, "RX(%u) BASE ADD: 0x%08x%08x\n", i,
			 base_add_h, base_add_l);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_RX_RING_BD_NUM_REG);
		dev_info(&h->pdev->dev, "RX(%u) RING BD NUM: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_RX_RING_BD_LEN_REG);
		dev_info(&h->pdev->dev, "RX(%u) RING BD LEN: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_RX_RING_TAIL_REG);
		dev_info(&h->pdev->dev, "RX(%u) RING TAIL: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_RX_RING_HEAD_REG);
		dev_info(&h->pdev->dev, "RX(%u) RING HEAD: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_RX_RING_FBDNUM_REG);
		dev_info(&h->pdev->dev, "RX(%u) RING FBDNUM: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_RX_RING_PKTNUM_RECORD_REG);
		dev_info(&h->pdev->dev, "RX(%u) RING PKTNUM: %u\n", i, value);

		ring = &priv->ring[i];
		base_add_h = readl_relaxed(ring->tqp->io_base +
					   HNS3_RING_TX_RING_BASEADDR_H_REG);
		base_add_l = readl_relaxed(ring->tqp->io_base +
					   HNS3_RING_TX_RING_BASEADDR_L_REG);
		dev_info(&h->pdev->dev, "TX(%u) BASE ADD: 0x%08x%08x\n", i,
			 base_add_h, base_add_l);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_BD_NUM_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING BD NUM: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_TC_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING TC: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_TAIL_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING TAIL: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_HEAD_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING HEAD: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_FBDNUM_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING FBDNUM: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_OFFSET_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING OFFSET: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base +
				      HNS3_RING_TX_RING_PKTNUM_RECORD_REG);
		dev_info(&h->pdev->dev, "TX(%u) RING PKTNUM: %u\n", i, value);

		value = readl_relaxed(ring->tqp->io_base + HNS3_RING_EN_REG);
		dev_info(&h->pdev->dev, "TX/RX(%u) RING EN: %s\n", i,
			 value ? "enable" : "disable");

		if (hnae3_ae_dev_tqp_txrx_indep_supported(ae_dev)) {
			value = readl_relaxed(ring->tqp->io_base +
					      HNS3_RING_TX_EN_REG);
			dev_info(&h->pdev->dev, "TX(%u) RING EN: %s\n", i,
				 value ? "enable" : "disable");

			value = readl_relaxed(ring->tqp->io_base +
					      HNS3_RING_RX_EN_REG);
			dev_info(&h->pdev->dev, "RX(%u) RING EN: %s\n", i,
				 value ? "enable" : "disable");
		}

		dev_info(&h->pdev->dev, "\n");
	}

	return 0;
}

static int hns3_dbg_queue_map(struct hnae3_handle *h)
{
	struct hns3_nic_priv *priv = h->priv;
	int i;

	if (!h->ae_algo->ops->get_global_queue_id)
		return -EOPNOTSUPP;

	dev_info(&h->pdev->dev, "map info for queue id and vector id\n");
	dev_info(&h->pdev->dev,
		 "local queue id | global queue id | vector id\n");
	for (i = 0; i < h->kinfo.num_tqps; i++) {
		u16 global_qid;

		global_qid = h->ae_algo->ops->get_global_queue_id(h, i);
		if (!priv->ring || !priv->ring[i].tqp_vector)
			continue;

		dev_info(&h->pdev->dev,
			 "      %4d            %4u            %4d\n",
			 i, global_qid, priv->ring[i].tqp_vector->vector_irq);
	}

	return 0;
}

static int hns3_dbg_bd_info(struct hnae3_handle *h, const char *cmd_buf)
{
	struct hns3_nic_priv *priv = h->priv;
	struct hns3_desc *rx_desc, *tx_desc;
	struct device *dev = &h->pdev->dev;
	struct hns3_enet_ring *ring;
	u32 tx_index, rx_index;
	u32 q_num, value;
	dma_addr_t addr;
	u16 mss_hw_csum;
	u32 l234info;
	int cnt;

	cnt = sscanf(&cmd_buf[8], "%u %u", &q_num, &tx_index);
	if (cnt == 2) {
		rx_index = tx_index;
	} else if (cnt != 1) {
		dev_err(dev, "bd info: bad command string, cnt=%d\n", cnt);
		return -EINVAL;
	}

	if (q_num >= h->kinfo.num_tqps) {
		dev_err(dev, "Queue number(%u) is out of range(0-%u)\n", q_num,
			h->kinfo.num_tqps - 1);
		return -EINVAL;
	}

	ring = &priv->ring[q_num];
	value = readl_relaxed(ring->tqp->io_base + HNS3_RING_TX_RING_TAIL_REG);
	tx_index = (cnt == 1) ? value : tx_index;

	if (tx_index >= ring->desc_num) {
		dev_err(dev, "bd index(%u) is out of range(0-%u)\n", tx_index,
			ring->desc_num - 1);
		return -EINVAL;
	}

	tx_desc = &ring->desc[tx_index];
	addr = le64_to_cpu(tx_desc->addr);
	mss_hw_csum = le16_to_cpu(tx_desc->tx.mss_hw_csum);
	dev_info(dev, "TX Queue Num: %u, BD Index: %u\n", q_num, tx_index);
	dev_info(dev, "(TX)addr: %pad\n", &addr);
	dev_info(dev, "(TX)vlan_tag: %u\n", le16_to_cpu(tx_desc->tx.vlan_tag));
	dev_info(dev, "(TX)send_size: %u\n",
		 le16_to_cpu(tx_desc->tx.send_size));

	if (mss_hw_csum & BIT(HNS3_TXD_HW_CS_B)) {
		u32 offset = le32_to_cpu(tx_desc->tx.ol_type_vlan_len_msec);
		u32 start = le32_to_cpu(tx_desc->tx.type_cs_vlan_tso_len);

		dev_info(dev, "(TX)csum start: %u\n",
			 hnae3_get_field(start,
					 HNS3_TXD_CSUM_START_M,
					 HNS3_TXD_CSUM_START_S));
		dev_info(dev, "(TX)csum offset: %u\n",
			 hnae3_get_field(offset,
					 HNS3_TXD_CSUM_OFFSET_M,
					 HNS3_TXD_CSUM_OFFSET_S));
	} else {
		dev_info(dev, "(TX)vlan_tso: %u\n",
			 tx_desc->tx.type_cs_vlan_tso);
		dev_info(dev, "(TX)l2_len: %u\n", tx_desc->tx.l2_len);
		dev_info(dev, "(TX)l3_len: %u\n", tx_desc->tx.l3_len);
		dev_info(dev, "(TX)l4_len: %u\n", tx_desc->tx.l4_len);
		dev_info(dev, "(TX)vlan_msec: %u\n",
			 tx_desc->tx.ol_type_vlan_msec);
		dev_info(dev, "(TX)ol2_len: %u\n", tx_desc->tx.ol2_len);
		dev_info(dev, "(TX)ol3_len: %u\n", tx_desc->tx.ol3_len);
		dev_info(dev, "(TX)ol4_len: %u\n", tx_desc->tx.ol4_len);
	}

	dev_info(dev, "(TX)vlan_tag: %u\n",
		 le16_to_cpu(tx_desc->tx.outer_vlan_tag));
	dev_info(dev, "(TX)tv: %u\n", le16_to_cpu(tx_desc->tx.tv));
	dev_info(dev, "(TX)paylen_ol4cs: %u\n",
		 le32_to_cpu(tx_desc->tx.paylen_ol4cs));
	dev_info(dev, "(TX)vld_ra_ri: %u\n",
		 le16_to_cpu(tx_desc->tx.bdtp_fe_sc_vld_ra_ri));
	dev_info(dev, "(TX)mss_hw_csum: %u\n", mss_hw_csum);

	ring = &priv->ring[q_num + h->kinfo.num_tqps];
	value = readl_relaxed(ring->tqp->io_base + HNS3_RING_RX_RING_TAIL_REG);
	rx_index = (cnt == 1) ? value : tx_index;
	rx_desc = &ring->desc[rx_index];

	addr = le64_to_cpu(rx_desc->addr);
	l234info = le32_to_cpu(rx_desc->rx.l234_info);
	dev_info(dev, "RX Queue Num: %u, BD Index: %u\n", q_num, rx_index);
	dev_info(dev, "(RX)addr: %pad\n", &addr);
	dev_info(dev, "(RX)l234_info: %u\n", l234info);

	if (l234info & BIT(HNS3_RXD_L2_CSUM_B)) {
		u32 lo, hi;

		lo = hnae3_get_field(l234info, HNS3_RXD_L2_CSUM_L_M,
				     HNS3_RXD_L2_CSUM_L_S);
		hi = hnae3_get_field(l234info, HNS3_RXD_L2_CSUM_H_M,
				     HNS3_RXD_L2_CSUM_H_S);
		dev_info(dev, "(RX)csum: %u\n", lo | hi << 8);
	}

	dev_info(dev, "(RX)pkt_len: %u\n", le16_to_cpu(rx_desc->rx.pkt_len));
	dev_info(dev, "(RX)size: %u\n", le16_to_cpu(rx_desc->rx.size));
	dev_info(dev, "(RX)rss_hash: %u\n", le32_to_cpu(rx_desc->rx.rss_hash));
	dev_info(dev, "(RX)fd_id: %u\n", le16_to_cpu(rx_desc->rx.fd_id));
	dev_info(dev, "(RX)vlan_tag: %u\n", le16_to_cpu(rx_desc->rx.vlan_tag));
	dev_info(dev, "(RX)o_dm_vlan_id_fb: %u\n",
		 le16_to_cpu(rx_desc->rx.o_dm_vlan_id_fb));
	dev_info(dev, "(RX)ot_vlan_tag: %u\n",
		 le16_to_cpu(rx_desc->rx.ot_vlan_tag));
	dev_info(dev, "(RX)bd_base_info: %u\n",
		 le32_to_cpu(rx_desc->rx.bd_base_info));

	return 0;
}

static void hns3_dbg_help(struct hnae3_handle *h)
{
#define HNS3_DBG_BUF_LEN 256

	char printf_buf[HNS3_DBG_BUF_LEN];

	dev_info(&h->pdev->dev, "available commands\n");
	dev_info(&h->pdev->dev, "queue info <number>\n");
	dev_info(&h->pdev->dev, "queue map\n");
	dev_info(&h->pdev->dev, "bd info <q_num> <bd index>\n");
	dev_info(&h->pdev->dev, "dev capability\n");
	dev_info(&h->pdev->dev, "dev spec\n");

	if (!hns3_is_phys_func(h->pdev))
		return;

	dev_info(&h->pdev->dev, "dump fd tcam\n");
	dev_info(&h->pdev->dev, "dump tc\n");
	dev_info(&h->pdev->dev, "dump tm map <q_num>\n");
	dev_info(&h->pdev->dev, "dump tm\n");
	dev_info(&h->pdev->dev, "dump qos pause cfg\n");
	dev_info(&h->pdev->dev, "dump qos pri map\n");
	dev_info(&h->pdev->dev, "dump qos buf cfg\n");
	dev_info(&h->pdev->dev, "dump mng tbl\n");
	dev_info(&h->pdev->dev, "dump reset info\n");
	dev_info(&h->pdev->dev, "dump m7 info\n");
	dev_info(&h->pdev->dev, "dump ncl_config <offset> <length>(in hex)\n");
	dev_info(&h->pdev->dev, "dump mac tnl status\n");
	dev_info(&h->pdev->dev, "dump loopback\n");
	dev_info(&h->pdev->dev, "dump qs shaper [qs id]\n");
	dev_info(&h->pdev->dev, "dump uc mac list <func id>\n");
	dev_info(&h->pdev->dev, "dump mc mac list <func id>\n");
	dev_info(&h->pdev->dev, "dump intr\n");

	memset(printf_buf, 0, HNS3_DBG_BUF_LEN);
	strncat(printf_buf, "dump reg [[bios common] [ssu <port_id>]",
		HNS3_DBG_BUF_LEN - 1);
	strncat(printf_buf + strlen(printf_buf),
		" [igu egu <port_id>] [rpu <tc_queue_num>]",
		HNS3_DBG_BUF_LEN - strlen(printf_buf) - 1);
	strncat(printf_buf + strlen(printf_buf),
		" [rtc] [ppp] [rcb] [tqp <queue_num>] [mac]]\n",
		HNS3_DBG_BUF_LEN - strlen(printf_buf) - 1);
	dev_info(&h->pdev->dev, "%s", printf_buf);

	memset(printf_buf, 0, HNS3_DBG_BUF_LEN);
	strncat(printf_buf, "dump reg dcb <port_id> <pri_id> <pg_id>",
		HNS3_DBG_BUF_LEN - 1);
	strncat(printf_buf + strlen(printf_buf), " <rq_id> <nq_id> <qset_id>\n",
		HNS3_DBG_BUF_LEN - strlen(printf_buf) - 1);
	dev_info(&h->pdev->dev, "%s", printf_buf);
}

static void hns3_dbg_dev_caps(struct hnae3_handle *h)
{
	struct hnae3_ae_dev *ae_dev = pci_get_drvdata(h->pdev);
	unsigned long *caps;

	caps = ae_dev->caps;

	dev_info(&h->pdev->dev, "support FD: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_FD_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support GRO: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_GRO_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support FEC: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_FEC_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support UDP GSO: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_UDP_GSO_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support PTP: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_PTP_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support INT QL: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_INT_QL_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support HW TX csum: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_HW_TX_CSUM_B, caps) ? "yes" : "no");
	dev_info(&h->pdev->dev, "support UDP tunnel csum: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_UDP_TUNNEL_CSUM_B, caps) ?
		 "yes" : "no");
	dev_info(&h->pdev->dev, "support PAUSE: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_PAUSE_B, ae_dev->caps) ?
		 "yes" : "no");
	dev_info(&h->pdev->dev, "support imp-controlled PHY: %s\n",
		 test_bit(HNAE3_DEV_SUPPORT_PHY_IMP_B, caps) ? "yes" : "no");
}

static void hns3_dbg_dev_specs(struct hnae3_handle *h)
{
	struct hnae3_ae_dev *ae_dev = pci_get_drvdata(h->pdev);
	struct hnae3_dev_specs *dev_specs = &ae_dev->dev_specs;
	struct hnae3_knic_private_info *kinfo = &h->kinfo;
	struct hns3_nic_priv *priv  = h->priv;

	dev_info(priv->dev, "MAC entry num: %u\n", dev_specs->mac_entry_num);
	dev_info(priv->dev, "MNG entry num: %u\n", dev_specs->mng_entry_num);
	dev_info(priv->dev, "MAX non tso bd num: %u\n",
		 dev_specs->max_non_tso_bd_num);
	dev_info(priv->dev, "RSS ind tbl size: %u\n",
		 dev_specs->rss_ind_tbl_size);
	dev_info(priv->dev, "RSS key size: %u\n", dev_specs->rss_key_size);
	dev_info(priv->dev, "RSS size: %u\n", kinfo->rss_size);
	dev_info(priv->dev, "Allocated RSS size: %u\n", kinfo->req_rss_size);
	dev_info(priv->dev, "Task queue pairs numbers: %u\n", kinfo->num_tqps);

	dev_info(priv->dev, "RX buffer length: %u\n", kinfo->rx_buf_len);
	dev_info(priv->dev, "Desc num per TX queue: %u\n", kinfo->num_tx_desc);
	dev_info(priv->dev, "Desc num per RX queue: %u\n", kinfo->num_rx_desc);
	dev_info(priv->dev, "Total number of enabled TCs: %u\n",
		 kinfo->tc_info.num_tc);
	dev_info(priv->dev, "MAX INT QL: %u\n", dev_specs->int_ql_max);
	dev_info(priv->dev, "MAX INT GL: %u\n", dev_specs->max_int_gl);
	dev_info(priv->dev, "MAX frame size: %u\n", dev_specs->max_frm_size);
	dev_info(priv->dev, "MAX TM RATE: %uMbps\n", dev_specs->max_tm_rate);
	dev_info(priv->dev, "MAX QSET number: %u\n", dev_specs->max_qset_num);
}

static ssize_t hns3_dbg_cmd_read(struct file *filp, char __user *buffer,
				 size_t count, loff_t *ppos)
{
	int uncopy_bytes;
	char *buf;
	int len;

	if (*ppos != 0)
		return 0;

	if (count < HNS3_DBG_READ_LEN)
		return -ENOSPC;

	buf = kzalloc(HNS3_DBG_READ_LEN, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	len = scnprintf(buf, HNS3_DBG_READ_LEN, "%s\n",
			"Please echo help to cmd to get help information");
	uncopy_bytes = copy_to_user(buffer, buf, len);

	kfree(buf);

	if (uncopy_bytes)
		return -EFAULT;

	return (*ppos = len);
}

static int hns3_dbg_check_cmd(struct hnae3_handle *handle, char *cmd_buf)
{
	int ret = 0;

	if (strncmp(cmd_buf, "help", 4) == 0)
		hns3_dbg_help(handle);
	else if (strncmp(cmd_buf, "queue info", 10) == 0)
		ret = hns3_dbg_queue_info(handle, cmd_buf);
	else if (strncmp(cmd_buf, "queue map", 9) == 0)
		ret = hns3_dbg_queue_map(handle);
	else if (strncmp(cmd_buf, "bd info", 7) == 0)
		ret = hns3_dbg_bd_info(handle, cmd_buf);
	else if (strncmp(cmd_buf, "dev capability", 14) == 0)
		hns3_dbg_dev_caps(handle);
	else if (strncmp(cmd_buf, "dev spec", 8) == 0)
		hns3_dbg_dev_specs(handle);
	else if (handle->ae_algo->ops->dbg_run_cmd)
		ret = handle->ae_algo->ops->dbg_run_cmd(handle, cmd_buf);
	else
		ret = -EOPNOTSUPP;

	return ret;
}

static ssize_t hns3_dbg_cmd_write(struct file *filp, const char __user *buffer,
				  size_t count, loff_t *ppos)
{
	struct hnae3_handle *handle = filp->private_data;
	struct hns3_nic_priv *priv  = handle->priv;
	char *cmd_buf, *cmd_buf_tmp;
	int uncopied_bytes;
	int ret;

	if (*ppos != 0)
		return 0;

	/* Judge if the instance is being reset. */
	if (!test_bit(HNS3_NIC_STATE_INITED, &priv->state) ||
	    test_bit(HNS3_NIC_STATE_RESETTING, &priv->state))
		return 0;

	if (count > HNS3_DBG_WRITE_LEN)
		return -ENOSPC;

	cmd_buf = kzalloc(count + 1, GFP_KERNEL);
	if (!cmd_buf)
		return count;

	uncopied_bytes = copy_from_user(cmd_buf, buffer, count);
	if (uncopied_bytes) {
		kfree(cmd_buf);
		return -EFAULT;
	}

	cmd_buf[count] = '\0';

	cmd_buf_tmp = strchr(cmd_buf, '\n');
	if (cmd_buf_tmp) {
		*cmd_buf_tmp = '\0';
		count = cmd_buf_tmp - cmd_buf + 1;
	}

	ret = hns3_dbg_check_cmd(handle, cmd_buf);
	if (ret)
		hns3_dbg_help(handle);

	kfree(cmd_buf);
	cmd_buf = NULL;

	return count;
}

static ssize_t hns3_dbg_read(struct file *filp, char __user *buffer,
			     size_t count, loff_t *ppos)
{
	struct hnae3_handle *handle = filp->private_data;
	const struct hnae3_ae_ops *ops = handle->ae_algo->ops;
	struct hns3_nic_priv *priv = handle->priv;
	char *cmd_buf, *read_buf;
	ssize_t size = 0;
	int ret = 0;

	read_buf = kzalloc(HNS3_DBG_READ_LEN, GFP_KERNEL);
	if (!read_buf)
		return -ENOMEM;

	cmd_buf = filp->f_path.dentry->d_iname;

	if (ops->dbg_read_cmd)
		ret = ops->dbg_read_cmd(handle, cmd_buf, read_buf,
					HNS3_DBG_READ_LEN);

	if (ret) {
		dev_info(priv->dev, "unknown command\n");
		goto out;
	}

	size = simple_read_from_buffer(buffer, count, ppos, read_buf,
				       strlen(read_buf));

out:
	kfree(read_buf);
	return size;
}

static const struct file_operations hns3_dbg_cmd_fops = {
	.owner = THIS_MODULE,
	.open  = simple_open,
	.read  = hns3_dbg_cmd_read,
	.write = hns3_dbg_cmd_write,
};

static const struct file_operations hns3_dbg_fops = {
	.owner = THIS_MODULE,
	.open  = simple_open,
	.read  = hns3_dbg_read,
};

void hns3_dbg_init(struct hnae3_handle *handle)
{
	struct hnae3_ae_dev *ae_dev = pci_get_drvdata(handle->pdev);
	const char *name = pci_name(handle->pdev);
	struct dentry *entry_dir;

	handle->hnae3_dbgfs = debugfs_create_dir(name, hns3_dbgfs_root);

	debugfs_create_file("cmd", 0600, handle->hnae3_dbgfs, handle,
			    &hns3_dbg_cmd_fops);

	entry_dir = debugfs_create_dir("tm", handle->hnae3_dbgfs);
	if (ae_dev->dev_version > HNAE3_DEVICE_VERSION_V2)
		debugfs_create_file(HNAE3_DBG_TM_NODES, 0600, entry_dir, handle,
				    &hns3_dbg_fops);
	debugfs_create_file(HNAE3_DBG_TM_PRI, 0600, entry_dir, handle,
			    &hns3_dbg_fops);
	debugfs_create_file(HNAE3_DBG_TM_QSET, 0600, entry_dir, handle,
			    &hns3_dbg_fops);
}

void hns3_dbg_uninit(struct hnae3_handle *handle)
{
	debugfs_remove_recursive(handle->hnae3_dbgfs);
	handle->hnae3_dbgfs = NULL;
}

void hns3_dbg_register_debugfs(const char *debugfs_dir_name)
{
	hns3_dbgfs_root = debugfs_create_dir(debugfs_dir_name, NULL);
}

void hns3_dbg_unregister_debugfs(void)
{
	debugfs_remove_recursive(hns3_dbgfs_root);
	hns3_dbgfs_root = NULL;
}
