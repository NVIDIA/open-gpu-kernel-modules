// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2014-2015 Hisilicon Limited.
 */

#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include "hns_enet.h"

#define HNS_PHY_PAGE_MDIX	0
#define HNS_PHY_PAGE_LED	3
#define HNS_PHY_PAGE_COPPER	0

#define HNS_PHY_PAGE_REG	22	/* Page Selection Reg. */
#define HNS_PHY_CSC_REG		16	/* Copper Specific Control Register */
#define HNS_PHY_CSS_REG		17	/* Copper Specific Status Register */
#define HNS_LED_FC_REG		16	/* LED Function Control Reg. */

#define HNS_LED_FORCE_ON	9
#define HNS_LED_FORCE_OFF	8

#define HNS_CHIP_VERSION 660
#define HNS_NET_STATS_CNT 26

#define PHY_MDIX_CTRL_S		(5)
#define PHY_MDIX_CTRL_M		(3 << PHY_MDIX_CTRL_S)

#define PHY_MDIX_STATUS_B	(6)
#define PHY_SPEED_DUP_RESOLVE_B	(11)

/**
 *hns_nic_get_link - get current link status
 *@net_dev: net_device
 *retuen 0 - success , negative --fail
 */
static u32 hns_nic_get_link(struct net_device *net_dev)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	u32 link_stat = priv->link;
	struct hnae_handle *h;

	h = priv->ae_handle;

	if (net_dev->phydev) {
		if (!genphy_read_status(net_dev->phydev))
			link_stat = net_dev->phydev->link;
		else
			link_stat = 0;
	}

	if (h->dev && h->dev->ops && h->dev->ops->get_status)
		link_stat = link_stat && h->dev->ops->get_status(h);
	else
		link_stat = 0;

	return link_stat;
}

static void hns_get_mdix_mode(struct net_device *net_dev,
			      struct ethtool_link_ksettings *cmd)
{
	int mdix_ctrl, mdix, retval, is_resolved;
	struct phy_device *phy_dev = net_dev->phydev;

	if (!phy_dev || !phy_dev->mdio.bus) {
		cmd->base.eth_tp_mdix_ctrl = ETH_TP_MDI_INVALID;
		cmd->base.eth_tp_mdix = ETH_TP_MDI_INVALID;
		return;
	}

	phy_write(phy_dev, HNS_PHY_PAGE_REG, HNS_PHY_PAGE_MDIX);

	retval = phy_read(phy_dev, HNS_PHY_CSC_REG);
	mdix_ctrl = hnae_get_field(retval, PHY_MDIX_CTRL_M, PHY_MDIX_CTRL_S);

	retval = phy_read(phy_dev, HNS_PHY_CSS_REG);
	mdix = hnae_get_bit(retval, PHY_MDIX_STATUS_B);
	is_resolved = hnae_get_bit(retval, PHY_SPEED_DUP_RESOLVE_B);

	phy_write(phy_dev, HNS_PHY_PAGE_REG, HNS_PHY_PAGE_COPPER);

	switch (mdix_ctrl) {
	case 0x0:
		cmd->base.eth_tp_mdix_ctrl = ETH_TP_MDI;
		break;
	case 0x1:
		cmd->base.eth_tp_mdix_ctrl = ETH_TP_MDI_X;
		break;
	case 0x3:
		cmd->base.eth_tp_mdix_ctrl = ETH_TP_MDI_AUTO;
		break;
	default:
		cmd->base.eth_tp_mdix_ctrl = ETH_TP_MDI_INVALID;
		break;
	}

	if (!is_resolved)
		cmd->base.eth_tp_mdix = ETH_TP_MDI_INVALID;
	else if (mdix)
		cmd->base.eth_tp_mdix = ETH_TP_MDI_X;
	else
		cmd->base.eth_tp_mdix = ETH_TP_MDI;
}

/**
 *hns_nic_get_link_ksettings - implement ethtool get link ksettings
 *@net_dev: net_device
 *@cmd: ethtool_link_ksettings
 *retuen 0 - success , negative --fail
 */
static int hns_nic_get_link_ksettings(struct net_device *net_dev,
				      struct ethtool_link_ksettings *cmd)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_handle *h;
	u32 link_stat;
	int ret;
	u8 duplex;
	u16 speed;
	u32 supported, advertising;

	if (!priv || !priv->ae_handle)
		return -ESRCH;

	h = priv->ae_handle;
	if (!h->dev || !h->dev->ops || !h->dev->ops->get_info)
		return -ESRCH;

	ret = h->dev->ops->get_info(h, NULL, &speed, &duplex);
	if (ret < 0) {
		netdev_err(net_dev, "%s get_info error!\n", __func__);
		return -EINVAL;
	}

	ethtool_convert_link_mode_to_legacy_u32(&supported,
						cmd->link_modes.supported);
	ethtool_convert_link_mode_to_legacy_u32(&advertising,
						cmd->link_modes.advertising);

	/* When there is no phy, autoneg is off. */
	cmd->base.autoneg = false;
	cmd->base.speed = speed;
	cmd->base.duplex = duplex;

	if (net_dev->phydev)
		phy_ethtool_ksettings_get(net_dev->phydev, cmd);

	link_stat = hns_nic_get_link(net_dev);
	if (!link_stat) {
		cmd->base.speed = (u32)SPEED_UNKNOWN;
		cmd->base.duplex = DUPLEX_UNKNOWN;
	}

	if (cmd->base.autoneg)
		advertising |= ADVERTISED_Autoneg;

	supported |= h->if_support;
	if (h->phy_if == PHY_INTERFACE_MODE_SGMII) {
		supported |= SUPPORTED_TP;
		advertising |= ADVERTISED_1000baseT_Full;
	} else if (h->phy_if == PHY_INTERFACE_MODE_XGMII) {
		supported |= SUPPORTED_FIBRE;
		advertising |= ADVERTISED_10000baseKR_Full;
	}

	switch (h->media_type) {
	case HNAE_MEDIA_TYPE_FIBER:
		cmd->base.port = PORT_FIBRE;
		break;
	case HNAE_MEDIA_TYPE_COPPER:
		cmd->base.port = PORT_TP;
		break;
	case HNAE_MEDIA_TYPE_UNKNOWN:
	default:
		break;
	}

	if (!(AE_IS_VER1(priv->enet_ver) && h->port_type == HNAE_PORT_DEBUG))
		supported |= SUPPORTED_Pause;

	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.supported,
						supported);
	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.advertising,
						advertising);

	cmd->base.mdio_support = ETH_MDIO_SUPPORTS_C45 | ETH_MDIO_SUPPORTS_C22;
	hns_get_mdix_mode(net_dev, cmd);

	return 0;
}

/**
 *hns_nic_set_link_ksettings - implement ethtool set link ksettings
 *@net_dev: net_device
 *@cmd: ethtool_link_ksettings
 *retuen 0 - success , negative --fail
 */
static int hns_nic_set_link_ksettings(struct net_device *net_dev,
				      const struct ethtool_link_ksettings *cmd)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_handle *h;
	u32 speed;

	if (!netif_running(net_dev))
		return -ESRCH;

	if (!priv || !priv->ae_handle || !priv->ae_handle->dev ||
	    !priv->ae_handle->dev->ops)
		return -ENODEV;

	h = priv->ae_handle;
	speed = cmd->base.speed;

	if (h->phy_if == PHY_INTERFACE_MODE_XGMII) {
		if (cmd->base.autoneg == AUTONEG_ENABLE ||
		    speed != SPEED_10000 ||
		    cmd->base.duplex != DUPLEX_FULL)
			return -EINVAL;
	} else if (h->phy_if == PHY_INTERFACE_MODE_SGMII) {
		if (!net_dev->phydev && cmd->base.autoneg == AUTONEG_ENABLE)
			return -EINVAL;

		if (speed == SPEED_1000 && cmd->base.duplex == DUPLEX_HALF)
			return -EINVAL;
		if (net_dev->phydev)
			return phy_ethtool_ksettings_set(net_dev->phydev, cmd);

		if ((speed != SPEED_10 && speed != SPEED_100 &&
		     speed != SPEED_1000) || (cmd->base.duplex != DUPLEX_HALF &&
		     cmd->base.duplex != DUPLEX_FULL))
			return -EINVAL;
	} else {
		netdev_err(net_dev, "Not supported!");
		return -ENOTSUPP;
	}

	if (h->dev->ops->adjust_link) {
		netif_carrier_off(net_dev);
		h->dev->ops->adjust_link(h, (int)speed, cmd->base.duplex);
		netif_carrier_on(net_dev);
		return 0;
	}

	netdev_err(net_dev, "Not supported!");
	return -ENOTSUPP;
}

static const char hns_nic_test_strs[][ETH_GSTRING_LEN] = {
	"Mac    Loopback test",
	"Serdes Loopback test",
	"Phy    Loopback test"
};

static int hns_nic_config_phy_loopback(struct phy_device *phy_dev, u8 en)
{
	int err;

	if (en) {
		/* Doing phy loopback in offline state, phy resuming is
		 * needed to power up the device.
		 */
		err = phy_resume(phy_dev);
		if (err)
			goto out;

		err = phy_loopback(phy_dev, true);
	} else {
		err = phy_loopback(phy_dev, false);
		if (err)
			goto out;

		err = phy_suspend(phy_dev);
	}

out:
	return err;
}

static int __lb_setup(struct net_device *ndev,
		      enum hnae_loop loop)
{
	int ret = 0;
	struct hns_nic_priv *priv = netdev_priv(ndev);
	struct phy_device *phy_dev = ndev->phydev;
	struct hnae_handle *h = priv->ae_handle;

	switch (loop) {
	case MAC_INTERNALLOOP_PHY:
		ret = hns_nic_config_phy_loopback(phy_dev, 0x1);
		if (!ret)
			ret = h->dev->ops->set_loopback(h, loop, 0x1);
		break;
	case MAC_INTERNALLOOP_MAC:
		if ((h->dev->ops->set_loopback) &&
		    (priv->ae_handle->phy_if != PHY_INTERFACE_MODE_XGMII))
			ret = h->dev->ops->set_loopback(h, loop, 0x1);
		break;
	case MAC_INTERNALLOOP_SERDES:
		if (h->dev->ops->set_loopback)
			ret = h->dev->ops->set_loopback(h, loop, 0x1);
		break;
	case MAC_LOOP_PHY_NONE:
		ret = hns_nic_config_phy_loopback(phy_dev, 0x0);
		fallthrough;
	case MAC_LOOP_NONE:
		if (!ret && h->dev->ops->set_loopback) {
			if (priv->ae_handle->phy_if != PHY_INTERFACE_MODE_XGMII)
				ret = h->dev->ops->set_loopback(h,
					MAC_INTERNALLOOP_MAC, 0x0);

			if (!ret)
				ret = h->dev->ops->set_loopback(h,
					MAC_INTERNALLOOP_SERDES, 0x0);
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}

	if (!ret) {
		if (loop == MAC_LOOP_NONE)
			h->dev->ops->set_promisc_mode(
				h, ndev->flags & IFF_PROMISC);
		else
			h->dev->ops->set_promisc_mode(h, 1);
	}
	return ret;
}

static int __lb_up(struct net_device *ndev,
		   enum hnae_loop loop_mode)
{
#define NIC_LB_TEST_WAIT_PHY_LINK_TIME 300
	struct hns_nic_priv *priv = netdev_priv(ndev);
	struct hnae_handle *h = priv->ae_handle;
	int speed, duplex;
	int ret;

	hns_nic_net_reset(ndev);

	ret = __lb_setup(ndev, loop_mode);
	if (ret)
		return ret;

	msleep(200);

	ret = h->dev->ops->start ? h->dev->ops->start(h) : 0;
	if (ret)
		return ret;

	/* link adjust duplex*/
	if (priv->ae_handle->phy_if != PHY_INTERFACE_MODE_XGMII)
		speed = 1000;
	else
		speed = 10000;
	duplex = 1;

	h->dev->ops->adjust_link(h, speed, duplex);

	/* wait adjust link done and phy ready */
	msleep(NIC_LB_TEST_WAIT_PHY_LINK_TIME);

	return 0;
}

static void __lb_other_process(struct hns_nic_ring_data *ring_data,
			       struct sk_buff *skb)
{
	struct net_device *ndev;
	struct hns_nic_priv *priv;
	struct hnae_ring *ring;
	struct netdev_queue *dev_queue;
	struct sk_buff *new_skb;
	unsigned int frame_size;
	int check_ok;
	u32 i;
	char buff[33]; /* 32B data and the last character '\0' */

	if (!ring_data) { /* Just for doing create frame*/
		ndev = skb->dev;
		priv = netdev_priv(ndev);

		frame_size = skb->len;
		memset(skb->data, 0xFF, frame_size);
		if ((!AE_IS_VER1(priv->enet_ver)) &&
		    (priv->ae_handle->port_type == HNAE_PORT_SERVICE)) {
			memcpy(skb->data, ndev->dev_addr, 6);
			skb->data[5] += 0x1f;
		}

		frame_size &= ~1ul;
		memset(&skb->data[frame_size / 2], 0xAA, frame_size / 2 - 1);
		memset(&skb->data[frame_size / 2 + 10], 0xBE,
		       frame_size / 2 - 11);
		memset(&skb->data[frame_size / 2 + 12], 0xAF,
		       frame_size / 2 - 13);
		return;
	}

	ring = ring_data->ring;
	ndev = ring_data->napi.dev;
	if (is_tx_ring(ring)) { /* for tx queue reset*/
		dev_queue = netdev_get_tx_queue(ndev, ring_data->queue_index);
		netdev_tx_reset_queue(dev_queue);
		return;
	}

	frame_size = skb->len;
	frame_size &= ~1ul;
	/* for mutl buffer*/
	new_skb = skb_copy(skb, GFP_ATOMIC);
	dev_kfree_skb_any(skb);
	if (!new_skb) {
		netdev_err(ndev, "skb alloc failed\n");
		return;
	}
	skb = new_skb;

	check_ok = 0;
	if (*(skb->data + 10) == 0xFF) { /* for rx check frame*/
		if ((*(skb->data + frame_size / 2 + 10) == 0xBE) &&
		    (*(skb->data + frame_size / 2 + 12) == 0xAF))
			check_ok = 1;
	}

	if (check_ok) {
		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += skb->len;
	} else {
		ndev->stats.rx_frame_errors++;
		for (i = 0; i < skb->len; i++) {
			snprintf(buff + i % 16 * 2, 3, /* tailing \0*/
				 "%02x", *(skb->data + i));
			if ((i % 16 == 15) || (i == skb->len - 1))
				pr_info("%s\n", buff);
		}
	}
	dev_kfree_skb_any(skb);
}

static int __lb_clean_rings(struct hns_nic_priv *priv,
			    int ringid0, int ringid1, int budget)
{
	int i, ret;
	struct hns_nic_ring_data *ring_data;
	struct net_device *ndev = priv->netdev;
	unsigned long rx_packets = ndev->stats.rx_packets;
	unsigned long rx_bytes = ndev->stats.rx_bytes;
	unsigned long rx_frame_errors = ndev->stats.rx_frame_errors;

	for (i = ringid0; i <= ringid1; i++) {
		ring_data = &priv->ring_data[i];
		(void)ring_data->poll_one(ring_data,
					  budget, __lb_other_process);
	}
	ret = (int)(ndev->stats.rx_packets - rx_packets);
	ndev->stats.rx_packets = rx_packets;
	ndev->stats.rx_bytes = rx_bytes;
	ndev->stats.rx_frame_errors = rx_frame_errors;
	return ret;
}

/**
 * __lb_run_test -  run loopback test
 * @ndev: net device
 * @loop_mode: loopback mode
 */
static int __lb_run_test(struct net_device *ndev,
			 enum hnae_loop loop_mode)
{
#define NIC_LB_TEST_PKT_NUM_PER_CYCLE 1
#define NIC_LB_TEST_RING_ID 0
#define NIC_LB_TEST_FRAME_SIZE 128
/* nic loopback test err  */
#define NIC_LB_TEST_NO_MEM_ERR 1
#define NIC_LB_TEST_TX_CNT_ERR 2
#define NIC_LB_TEST_RX_CNT_ERR 3

	struct hns_nic_priv *priv = netdev_priv(ndev);
	struct hnae_handle *h = priv->ae_handle;
	int i, j, lc, good_cnt, ret_val = 0;
	unsigned int size;
	netdev_tx_t tx_ret_val;
	struct sk_buff *skb;

	size = NIC_LB_TEST_FRAME_SIZE;
	/* allocate test skb */
	skb = alloc_skb(size, GFP_KERNEL);
	if (!skb)
		return NIC_LB_TEST_NO_MEM_ERR;

	/* place data into test skb */
	(void)skb_put(skb, size);
	skb->dev = ndev;
	__lb_other_process(NULL, skb);
	skb->queue_mapping = NIC_LB_TEST_RING_ID;

	lc = 1;
	for (j = 0; j < lc; j++) {
		/* reset count of good packets */
		good_cnt = 0;
		/* place 64 packets on the transmit queue*/
		for (i = 0; i < NIC_LB_TEST_PKT_NUM_PER_CYCLE; i++) {
			(void)skb_get(skb);

			tx_ret_val = (netdev_tx_t)hns_nic_net_xmit_hw(
				ndev, skb,
				&tx_ring_data(priv, skb->queue_mapping));
			if (tx_ret_val == NETDEV_TX_OK)
				good_cnt++;
			else
				break;
		}
		if (good_cnt != NIC_LB_TEST_PKT_NUM_PER_CYCLE) {
			ret_val = NIC_LB_TEST_TX_CNT_ERR;
			dev_err(priv->dev, "%s sent fail, cnt=0x%x, budget=0x%x\n",
				hns_nic_test_strs[loop_mode], good_cnt,
				NIC_LB_TEST_PKT_NUM_PER_CYCLE);
			break;
		}

		/* allow 100 milliseconds for packets to go from Tx to Rx */
		msleep(100);

		good_cnt = __lb_clean_rings(priv,
					    h->q_num, h->q_num * 2 - 1,
					    NIC_LB_TEST_PKT_NUM_PER_CYCLE);
		if (good_cnt != NIC_LB_TEST_PKT_NUM_PER_CYCLE) {
			ret_val = NIC_LB_TEST_RX_CNT_ERR;
			dev_err(priv->dev, "%s recv fail, cnt=0x%x, budget=0x%x\n",
				hns_nic_test_strs[loop_mode], good_cnt,
				NIC_LB_TEST_PKT_NUM_PER_CYCLE);
			break;
		}
		(void)__lb_clean_rings(priv,
				       NIC_LB_TEST_RING_ID, NIC_LB_TEST_RING_ID,
				       NIC_LB_TEST_PKT_NUM_PER_CYCLE);
	}

	/* free the original skb */
	kfree_skb(skb);

	return ret_val;
}

static int __lb_down(struct net_device *ndev, enum hnae_loop loop)
{
	struct hns_nic_priv *priv = netdev_priv(ndev);
	struct hnae_handle *h = priv->ae_handle;
	int ret;

	if (loop == MAC_INTERNALLOOP_PHY)
		ret = __lb_setup(ndev, MAC_LOOP_PHY_NONE);
	else
		ret = __lb_setup(ndev, MAC_LOOP_NONE);
	if (ret)
		netdev_err(ndev, "%s: __lb_setup return error(%d)!\n",
			   __func__,
			   ret);

	if (h->dev->ops->stop)
		h->dev->ops->stop(h);

	usleep_range(10000, 20000);
	(void)__lb_clean_rings(priv, 0, h->q_num - 1, 256);

	hns_nic_net_reset(ndev);

	return 0;
}

/**
 * hns_nic_self_test - self test
 * @ndev: net device
 * @eth_test: test cmd
 * @data: test result
 */
static void hns_nic_self_test(struct net_device *ndev,
			      struct ethtool_test *eth_test, u64 *data)
{
	struct hns_nic_priv *priv = netdev_priv(ndev);
	bool if_running = netif_running(ndev);
#define SELF_TEST_TPYE_NUM 3
	int st_param[SELF_TEST_TPYE_NUM][2];
	int i;
	int test_index = 0;

	st_param[0][0] = MAC_INTERNALLOOP_MAC; /* XGE not supported lb */
	st_param[0][1] = (priv->ae_handle->phy_if != PHY_INTERFACE_MODE_XGMII);
	st_param[1][0] = MAC_INTERNALLOOP_SERDES;
	st_param[1][1] = 1; /*serdes must exist*/
	st_param[2][0] = MAC_INTERNALLOOP_PHY; /* only supporte phy node*/
	st_param[2][1] = ((!!(priv->ae_handle->phy_dev)) &&
		(priv->ae_handle->phy_if != PHY_INTERFACE_MODE_XGMII));

	if (eth_test->flags == ETH_TEST_FL_OFFLINE) {
		set_bit(NIC_STATE_TESTING, &priv->state);

		if (if_running)
			dev_close(ndev);

		for (i = 0; i < SELF_TEST_TPYE_NUM; i++) {
			if (!st_param[i][1])
				continue;	/* NEXT testing */

			data[test_index] = __lb_up(ndev,
				(enum hnae_loop)st_param[i][0]);
			if (!data[test_index]) {
				data[test_index] = __lb_run_test(
					ndev, (enum hnae_loop)st_param[i][0]);
				(void)__lb_down(ndev,
						(enum hnae_loop)st_param[i][0]);
			}

			if (data[test_index])
				eth_test->flags |= ETH_TEST_FL_FAILED;

			test_index++;
		}

		hns_nic_net_reset(priv->netdev);

		clear_bit(NIC_STATE_TESTING, &priv->state);

		if (if_running)
			(void)dev_open(ndev, NULL);
	}
	/* Online tests aren't run; pass by default */

	(void)msleep_interruptible(4 * 1000);
}

/**
 * hns_nic_get_drvinfo - get net driver info
 * @net_dev: net device
 * @drvinfo: driver info
 */
static void hns_nic_get_drvinfo(struct net_device *net_dev,
				struct ethtool_drvinfo *drvinfo)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);

	strncpy(drvinfo->version, HNAE_DRIVER_VERSION,
		sizeof(drvinfo->version));
	drvinfo->version[sizeof(drvinfo->version) - 1] = '\0';

	strncpy(drvinfo->driver, HNAE_DRIVER_NAME, sizeof(drvinfo->driver));
	drvinfo->driver[sizeof(drvinfo->driver) - 1] = '\0';

	strncpy(drvinfo->bus_info, priv->dev->bus->name,
		sizeof(drvinfo->bus_info));
	drvinfo->bus_info[ETHTOOL_BUSINFO_LEN - 1] = '\0';

	strncpy(drvinfo->fw_version, "N/A", ETHTOOL_FWVERS_LEN);
	drvinfo->eedump_len = 0;
}

/**
 * hns_get_ringparam - get ring parameter
 * @net_dev: net device
 * @param: ethtool parameter
 */
static void hns_get_ringparam(struct net_device *net_dev,
			      struct ethtool_ringparam *param)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_ae_ops *ops;
	struct hnae_queue *queue;
	u32 uplimit = 0;

	queue = priv->ae_handle->qs[0];
	ops = priv->ae_handle->dev->ops;

	if (ops->get_ring_bdnum_limit)
		ops->get_ring_bdnum_limit(queue, &uplimit);

	param->rx_max_pending = uplimit;
	param->tx_max_pending = uplimit;
	param->rx_pending = queue->rx_ring.desc_num;
	param->tx_pending = queue->tx_ring.desc_num;
}

/**
 * hns_get_pauseparam - get pause parameter
 * @net_dev: net device
 * @param: pause parameter
 */
static void hns_get_pauseparam(struct net_device *net_dev,
			       struct ethtool_pauseparam *param)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_ae_ops *ops;

	ops = priv->ae_handle->dev->ops;

	if (ops->get_pauseparam)
		ops->get_pauseparam(priv->ae_handle, &param->autoneg,
					    &param->rx_pause, &param->tx_pause);
}

/**
 * hns_set_pauseparam - set pause parameter
 * @net_dev: net device
 * @param: pause parameter
 *
 * Return 0 on success, negative on failure
 */
static int hns_set_pauseparam(struct net_device *net_dev,
			      struct ethtool_pauseparam *param)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_handle *h;
	struct hnae_ae_ops *ops;

	h = priv->ae_handle;
	ops = h->dev->ops;

	if (!ops->set_pauseparam)
		return -ESRCH;

	return ops->set_pauseparam(priv->ae_handle, param->autoneg,
				   param->rx_pause, param->tx_pause);
}

/**
 * hns_get_coalesce - get coalesce info.
 * @net_dev: net device
 * @ec: coalesce info.
 *
 * Return 0 on success, negative on failure.
 */
static int hns_get_coalesce(struct net_device *net_dev,
			    struct ethtool_coalesce *ec)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_ae_ops *ops;

	ops = priv->ae_handle->dev->ops;

	ec->use_adaptive_rx_coalesce = priv->ae_handle->coal_adapt_en;
	ec->use_adaptive_tx_coalesce = priv->ae_handle->coal_adapt_en;

	if ((!ops->get_coalesce_usecs) ||
	    (!ops->get_max_coalesced_frames))
		return -ESRCH;

	ops->get_coalesce_usecs(priv->ae_handle,
					&ec->tx_coalesce_usecs,
					&ec->rx_coalesce_usecs);

	ops->get_max_coalesced_frames(
		priv->ae_handle,
		&ec->tx_max_coalesced_frames,
		&ec->rx_max_coalesced_frames);

	ops->get_coalesce_range(priv->ae_handle,
				&ec->tx_max_coalesced_frames_low,
				&ec->rx_max_coalesced_frames_low,
				&ec->tx_max_coalesced_frames_high,
				&ec->rx_max_coalesced_frames_high,
				&ec->tx_coalesce_usecs_low,
				&ec->rx_coalesce_usecs_low,
				&ec->tx_coalesce_usecs_high,
				&ec->rx_coalesce_usecs_high);

	return 0;
}

/**
 * hns_set_coalesce - set coalesce info.
 * @net_dev: net device
 * @ec: coalesce info.
 *
 * Return 0 on success, negative on failure.
 */
static int hns_set_coalesce(struct net_device *net_dev,
			    struct ethtool_coalesce *ec)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_ae_ops *ops;
	int rc1, rc2;

	ops = priv->ae_handle->dev->ops;

	if (ec->tx_coalesce_usecs != ec->rx_coalesce_usecs)
		return -EINVAL;

	if ((!ops->set_coalesce_usecs) ||
	    (!ops->set_coalesce_frames))
		return -ESRCH;

	if (ec->use_adaptive_rx_coalesce != priv->ae_handle->coal_adapt_en)
		priv->ae_handle->coal_adapt_en = ec->use_adaptive_rx_coalesce;

	rc1 = ops->set_coalesce_usecs(priv->ae_handle,
				      ec->rx_coalesce_usecs);

	rc2 = ops->set_coalesce_frames(priv->ae_handle,
				       ec->tx_max_coalesced_frames,
				       ec->rx_max_coalesced_frames);

	if (rc1 || rc2)
		return -EINVAL;

	return 0;
}

/**
 * hns_get_channels - get channel info.
 * @net_dev: net device
 * @ch: channel info.
 */
static void
hns_get_channels(struct net_device *net_dev, struct ethtool_channels *ch)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);

	ch->max_rx = priv->ae_handle->q_num;
	ch->max_tx = priv->ae_handle->q_num;

	ch->rx_count = priv->ae_handle->q_num;
	ch->tx_count = priv->ae_handle->q_num;
}

/**
 * hns_get_ethtool_stats - get detail statistics.
 * @netdev: net device
 * @stats: statistics info.
 * @data: statistics data.
 */
static void hns_get_ethtool_stats(struct net_device *netdev,
				  struct ethtool_stats *stats, u64 *data)
{
	u64 *p = data;
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_handle *h = priv->ae_handle;
	const struct rtnl_link_stats64 *net_stats;
	struct rtnl_link_stats64 temp;

	if (!h->dev->ops->get_stats || !h->dev->ops->update_stats) {
		netdev_err(netdev, "get_stats or update_stats is null!\n");
		return;
	}

	h->dev->ops->update_stats(h, &netdev->stats);

	net_stats = dev_get_stats(netdev, &temp);

	/* get netdev statistics */
	p[0] = net_stats->rx_packets;
	p[1] = net_stats->tx_packets;
	p[2] = net_stats->rx_bytes;
	p[3] = net_stats->tx_bytes;
	p[4] = net_stats->rx_errors;
	p[5] = net_stats->tx_errors;
	p[6] = net_stats->rx_dropped;
	p[7] = net_stats->tx_dropped;
	p[8] = net_stats->multicast;
	p[9] = net_stats->collisions;
	p[10] = net_stats->rx_over_errors;
	p[11] = net_stats->rx_crc_errors;
	p[12] = net_stats->rx_frame_errors;
	p[13] = net_stats->rx_fifo_errors;
	p[14] = net_stats->rx_missed_errors;
	p[15] = net_stats->tx_aborted_errors;
	p[16] = net_stats->tx_carrier_errors;
	p[17] = net_stats->tx_fifo_errors;
	p[18] = net_stats->tx_heartbeat_errors;
	p[19] = net_stats->rx_length_errors;
	p[20] = net_stats->tx_window_errors;
	p[21] = net_stats->rx_compressed;
	p[22] = net_stats->tx_compressed;

	p[23] = netdev->rx_dropped.counter;
	p[24] = netdev->tx_dropped.counter;

	p[25] = priv->tx_timeout_count;

	/* get driver statistics */
	h->dev->ops->get_stats(h, &p[26]);
}

/**
 * hns_get_strings: Return a set of strings that describe the requested objects
 * @netdev: net device
 * @stringset: string set ID.
 * @data: objects data.
 */
static void hns_get_strings(struct net_device *netdev, u32 stringset, u8 *data)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_handle *h = priv->ae_handle;
	u8 *buff = data;

	if (!h->dev->ops->get_strings) {
		netdev_err(netdev, "h->dev->ops->get_strings is null!\n");
		return;
	}

	if (stringset == ETH_SS_TEST) {
		if (priv->ae_handle->phy_if != PHY_INTERFACE_MODE_XGMII)
			ethtool_sprintf(&buff,
					hns_nic_test_strs[MAC_INTERNALLOOP_MAC]);
		ethtool_sprintf(&buff,
				hns_nic_test_strs[MAC_INTERNALLOOP_SERDES]);
		if ((netdev->phydev) && (!netdev->phydev->is_c45))
			ethtool_sprintf(&buff,
					hns_nic_test_strs[MAC_INTERNALLOOP_PHY]);

	} else {
		ethtool_sprintf(&buff, "rx_packets");
		ethtool_sprintf(&buff, "tx_packets");
		ethtool_sprintf(&buff, "rx_bytes");
		ethtool_sprintf(&buff, "tx_bytes");
		ethtool_sprintf(&buff, "rx_errors");
		ethtool_sprintf(&buff, "tx_errors");
		ethtool_sprintf(&buff, "rx_dropped");
		ethtool_sprintf(&buff, "tx_dropped");
		ethtool_sprintf(&buff, "multicast");
		ethtool_sprintf(&buff, "collisions");
		ethtool_sprintf(&buff, "rx_over_errors");
		ethtool_sprintf(&buff, "rx_crc_errors");
		ethtool_sprintf(&buff, "rx_frame_errors");
		ethtool_sprintf(&buff, "rx_fifo_errors");
		ethtool_sprintf(&buff, "rx_missed_errors");
		ethtool_sprintf(&buff, "tx_aborted_errors");
		ethtool_sprintf(&buff, "tx_carrier_errors");
		ethtool_sprintf(&buff, "tx_fifo_errors");
		ethtool_sprintf(&buff, "tx_heartbeat_errors");
		ethtool_sprintf(&buff, "rx_length_errors");
		ethtool_sprintf(&buff, "tx_window_errors");
		ethtool_sprintf(&buff, "rx_compressed");
		ethtool_sprintf(&buff, "tx_compressed");
		ethtool_sprintf(&buff, "netdev_rx_dropped");
		ethtool_sprintf(&buff, "netdev_tx_dropped");

		ethtool_sprintf(&buff, "netdev_tx_timeout");

		h->dev->ops->get_strings(h, stringset, buff);
	}
}

/**
 * hns_get_sset_count - get string set count returned by nic_get_strings
 * @netdev: net device
 * @stringset: string set index, 0: self test string; 1: statistics string.
 *
 * Return string set count.
 */
static int hns_get_sset_count(struct net_device *netdev, int stringset)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_handle *h = priv->ae_handle;
	struct hnae_ae_ops *ops = h->dev->ops;

	if (!ops->get_sset_count) {
		netdev_err(netdev, "get_sset_count is null!\n");
		return -EOPNOTSUPP;
	}
	if (stringset == ETH_SS_TEST) {
		u32 cnt = (sizeof(hns_nic_test_strs) / ETH_GSTRING_LEN);

		if (priv->ae_handle->phy_if == PHY_INTERFACE_MODE_XGMII)
			cnt--;

		if ((!netdev->phydev) || (netdev->phydev->is_c45))
			cnt--;

		return cnt;
	} else if (stringset == ETH_SS_STATS) {
		return (HNS_NET_STATS_CNT + ops->get_sset_count(h, stringset));
	} else {
		return -EOPNOTSUPP;
	}
}

/**
 * hns_phy_led_set - set phy LED status.
 * @netdev: net device
 * @value: LED state.
 *
 * Return 0 on success, negative on failure.
 */
static int hns_phy_led_set(struct net_device *netdev, int value)
{
	int retval;
	struct phy_device *phy_dev = netdev->phydev;

	retval = phy_write(phy_dev, HNS_PHY_PAGE_REG, HNS_PHY_PAGE_LED);
	retval |= phy_write(phy_dev, HNS_LED_FC_REG, value);
	retval |= phy_write(phy_dev, HNS_PHY_PAGE_REG, HNS_PHY_PAGE_COPPER);
	if (retval) {
		netdev_err(netdev, "mdiobus_write fail !\n");
		return retval;
	}
	return 0;
}

/**
 * hns_set_phys_id - set phy identify LED.
 * @netdev: net device
 * @state: LED state.
 *
 * Return 0 on success, negative on failure.
 */
static int
hns_set_phys_id(struct net_device *netdev, enum ethtool_phys_id_state state)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_handle *h = priv->ae_handle;
	struct phy_device *phy_dev = netdev->phydev;
	int ret;

	if (phy_dev)
		switch (state) {
		case ETHTOOL_ID_ACTIVE:
			ret = phy_write(phy_dev, HNS_PHY_PAGE_REG,
					HNS_PHY_PAGE_LED);
			if (ret)
				return ret;

			priv->phy_led_val = phy_read(phy_dev, HNS_LED_FC_REG);

			ret = phy_write(phy_dev, HNS_PHY_PAGE_REG,
					HNS_PHY_PAGE_COPPER);
			if (ret)
				return ret;
			return 2;
		case ETHTOOL_ID_ON:
			ret = hns_phy_led_set(netdev, HNS_LED_FORCE_ON);
			if (ret)
				return ret;
			break;
		case ETHTOOL_ID_OFF:
			ret = hns_phy_led_set(netdev, HNS_LED_FORCE_OFF);
			if (ret)
				return ret;
			break;
		case ETHTOOL_ID_INACTIVE:
			ret = phy_write(phy_dev, HNS_PHY_PAGE_REG,
					HNS_PHY_PAGE_LED);
			if (ret)
				return ret;

			ret = phy_write(phy_dev, HNS_LED_FC_REG,
					priv->phy_led_val);
			if (ret)
				return ret;

			ret = phy_write(phy_dev, HNS_PHY_PAGE_REG,
					HNS_PHY_PAGE_COPPER);
			if (ret)
				return ret;
			break;
		default:
			return -EINVAL;
		}
	else
		switch (state) {
		case ETHTOOL_ID_ACTIVE:
			return h->dev->ops->set_led_id(h, HNAE_LED_ACTIVE);
		case ETHTOOL_ID_ON:
			return h->dev->ops->set_led_id(h, HNAE_LED_ON);
		case ETHTOOL_ID_OFF:
			return h->dev->ops->set_led_id(h, HNAE_LED_OFF);
		case ETHTOOL_ID_INACTIVE:
			return h->dev->ops->set_led_id(h, HNAE_LED_INACTIVE);
		default:
			return -EINVAL;
		}

	return 0;
}

/**
 * hns_get_regs - get net device register
 * @net_dev: net device
 * @cmd: ethtool cmd
 * @data: register data
 */
static void hns_get_regs(struct net_device *net_dev, struct ethtool_regs *cmd,
			 void *data)
{
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_ae_ops *ops;

	ops = priv->ae_handle->dev->ops;

	cmd->version = HNS_CHIP_VERSION;
	if (!ops->get_regs) {
		netdev_err(net_dev, "ops->get_regs is null!\n");
		return;
	}
	ops->get_regs(priv->ae_handle, data);
}

/**
 * hns_get_regs_len - get total register len.
 * @net_dev: net device
 *
 * Return total register len.
 */
static int hns_get_regs_len(struct net_device *net_dev)
{
	u32 reg_num;
	struct hns_nic_priv *priv = netdev_priv(net_dev);
	struct hnae_ae_ops *ops;

	ops = priv->ae_handle->dev->ops;
	if (!ops->get_regs_len) {
		netdev_err(net_dev, "ops->get_regs_len is null!\n");
		return -EOPNOTSUPP;
	}

	reg_num = ops->get_regs_len(priv->ae_handle);
	if (reg_num > 0)
		return reg_num * sizeof(u32);
	else
		return reg_num;	/* error code */
}

/**
 * hns_nic_nway_reset - nway reset
 * @netdev: net device
 *
 * Return 0 on success, negative on failure
 */
static int hns_nic_nway_reset(struct net_device *netdev)
{
	struct phy_device *phy = netdev->phydev;

	if (!netif_running(netdev))
		return 0;

	if (!phy)
		return -EOPNOTSUPP;

	if (phy->autoneg != AUTONEG_ENABLE)
		return -EINVAL;

	return genphy_restart_aneg(phy);
}

static u32
hns_get_rss_key_size(struct net_device *netdev)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_ae_ops *ops;

	if (AE_IS_VER1(priv->enet_ver)) {
		netdev_err(netdev,
			   "RSS feature is not supported on this hardware\n");
		return 0;
	}

	ops = priv->ae_handle->dev->ops;
	return ops->get_rss_key_size(priv->ae_handle);
}

static u32
hns_get_rss_indir_size(struct net_device *netdev)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_ae_ops *ops;

	if (AE_IS_VER1(priv->enet_ver)) {
		netdev_err(netdev,
			   "RSS feature is not supported on this hardware\n");
		return 0;
	}

	ops = priv->ae_handle->dev->ops;
	return ops->get_rss_indir_size(priv->ae_handle);
}

static int
hns_get_rss(struct net_device *netdev, u32 *indir, u8 *key, u8 *hfunc)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_ae_ops *ops;

	if (AE_IS_VER1(priv->enet_ver)) {
		netdev_err(netdev,
			   "RSS feature is not supported on this hardware\n");
		return -EOPNOTSUPP;
	}

	ops = priv->ae_handle->dev->ops;

	if (!indir)
		return 0;

	return ops->get_rss(priv->ae_handle, indir, key, hfunc);
}

static int
hns_set_rss(struct net_device *netdev, const u32 *indir, const u8 *key,
	    const u8 hfunc)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);
	struct hnae_ae_ops *ops;

	if (AE_IS_VER1(priv->enet_ver)) {
		netdev_err(netdev,
			   "RSS feature is not supported on this hardware\n");
		return -EOPNOTSUPP;
	}

	ops = priv->ae_handle->dev->ops;

	if (hfunc != ETH_RSS_HASH_NO_CHANGE && hfunc != ETH_RSS_HASH_TOP) {
		netdev_err(netdev, "Invalid hfunc!\n");
		return -EOPNOTSUPP;
	}

	return ops->set_rss(priv->ae_handle, indir, key, hfunc);
}

static int hns_get_rxnfc(struct net_device *netdev,
			 struct ethtool_rxnfc *cmd,
			 u32 *rule_locs)
{
	struct hns_nic_priv *priv = netdev_priv(netdev);

	switch (cmd->cmd) {
	case ETHTOOL_GRXRINGS:
		cmd->data = priv->ae_handle->q_num;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static const struct ethtool_ops hns_ethtool_ops = {
	.supported_coalesce_params = ETHTOOL_COALESCE_USECS |
				     ETHTOOL_COALESCE_MAX_FRAMES |
				     ETHTOOL_COALESCE_USE_ADAPTIVE |
				     ETHTOOL_COALESCE_USECS_LOW_HIGH |
				     ETHTOOL_COALESCE_MAX_FRAMES_LOW_HIGH,
	.get_drvinfo = hns_nic_get_drvinfo,
	.get_link  = hns_nic_get_link,
	.get_ringparam = hns_get_ringparam,
	.get_pauseparam = hns_get_pauseparam,
	.set_pauseparam = hns_set_pauseparam,
	.get_coalesce = hns_get_coalesce,
	.set_coalesce = hns_set_coalesce,
	.get_channels = hns_get_channels,
	.self_test = hns_nic_self_test,
	.get_strings = hns_get_strings,
	.get_sset_count = hns_get_sset_count,
	.get_ethtool_stats = hns_get_ethtool_stats,
	.set_phys_id = hns_set_phys_id,
	.get_regs_len = hns_get_regs_len,
	.get_regs = hns_get_regs,
	.nway_reset = hns_nic_nway_reset,
	.get_rxfh_key_size = hns_get_rss_key_size,
	.get_rxfh_indir_size = hns_get_rss_indir_size,
	.get_rxfh = hns_get_rss,
	.set_rxfh = hns_set_rss,
	.get_rxnfc = hns_get_rxnfc,
	.get_link_ksettings  = hns_nic_get_link_ksettings,
	.set_link_ksettings  = hns_nic_set_link_ksettings,
};

void hns_ethtool_set_ops(struct net_device *ndev)
{
	ndev->ethtool_ops = &hns_ethtool_ops;
}
