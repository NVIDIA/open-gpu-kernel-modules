// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#include "../wifi.h"
#include "../core.h"
#include "../base.h"
#include "../pci.h"
#include "reg.h"
#include "def.h"
#include "phy.h"
#include "dm.h"
#include "fw.h"
#include "hw.h"
#include "trx.h"
#include "led.h"

#include <linux/module.h>

static void rtl92s_init_aspm_vars(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));

	/*close ASPM for AMD defaultly */
	rtlpci->const_amdpci_aspm = 0;

	/* ASPM PS mode.
	 * 0 - Disable ASPM,
	 * 1 - Enable ASPM without Clock Req,
	 * 2 - Enable ASPM with Clock Req,
	 * 3 - Alwyas Enable ASPM with Clock Req,
	 * 4 - Always Enable ASPM without Clock Req.
	 * set defult to RTL8192CE:3 RTL8192E:2
	 * */
	rtlpci->const_pci_aspm = 2;

	/*Setting for PCI-E device */
	rtlpci->const_devicepci_aspm_setting = 0x03;

	/*Setting for PCI-E bridge */
	rtlpci->const_hostpci_aspm_setting = 0x02;

	/* In Hw/Sw Radio Off situation.
	 * 0 - Default,
	 * 1 - From ASPM setting without low Mac Pwr,
	 * 2 - From ASPM setting with low Mac Pwr,
	 * 3 - Bus D3
	 * set default to RTL8192CE:0 RTL8192SE:2
	 */
	rtlpci->const_hwsw_rfoff_d3 = 2;

	/* This setting works for those device with
	 * backdoor ASPM setting such as EPHY setting.
	 * 0 - Not support ASPM,
	 * 1 - Support ASPM,
	 * 2 - According to chipset.
	 */
	rtlpci->const_support_pciaspm = rtlpriv->cfg->mod_params->aspm_support;
}

static void rtl92se_fw_cb(const struct firmware *firmware, void *context)
{
	struct ieee80211_hw *hw = context;
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rt_firmware *pfirmware = NULL;
	char *fw_name = "rtlwifi/rtl8192sefw.bin";

	rtl_dbg(rtlpriv, COMP_ERR, DBG_LOUD,
		"Firmware callback routine entered!\n");
	complete(&rtlpriv->firmware_loading_complete);
	if (!firmware) {
		pr_err("Firmware %s not available\n", fw_name);
		rtlpriv->max_fw_size = 0;
		return;
	}
	if (firmware->size > rtlpriv->max_fw_size) {
		pr_err("Firmware is too big!\n");
		rtlpriv->max_fw_size = 0;
		release_firmware(firmware);
		return;
	}
	pfirmware = (struct rt_firmware *)rtlpriv->rtlhal.pfirmware;
	memcpy(pfirmware->sz_fw_tmpbuffer, firmware->data, firmware->size);
	pfirmware->sz_fw_tmpbufferlen = firmware->size;
	release_firmware(firmware);
}

static int rtl92s_init_sw_vars(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	int err = 0;
	u16 earlyrxthreshold = 7;
	char *fw_name = "rtlwifi/rtl8192sefw.bin";

	rtlpriv->dm.dm_initialgain_enable = true;
	rtlpriv->dm.dm_flag = 0;
	rtlpriv->dm.disable_framebursting = false;
	rtlpriv->dm.thermalvalue = 0;
	rtlpriv->dm.useramask = true;

	/* compatible 5G band 91se just 2.4G band & smsp */
	rtlpriv->rtlhal.current_bandtype = BAND_ON_2_4G;
	rtlpriv->rtlhal.bandset = BAND_ON_2_4G;
	rtlpriv->rtlhal.macphymode = SINGLEMAC_SINGLEPHY;

	rtlpci->transmit_config = 0;

	rtlpci->receive_config =
			RCR_APPFCS |
			RCR_APWRMGT |
			/*RCR_ADD3 |*/
			RCR_AMF	|
			RCR_ADF |
			RCR_APP_MIC |
			RCR_APP_ICV |
			RCR_AICV |
			/* Accept ICV error, CRC32 Error */
			RCR_ACRC32 |
			RCR_AB |
			/* Accept Broadcast, Multicast */
			RCR_AM	|
			/* Accept Physical match */
			RCR_APM |
			/* Accept Destination Address packets */
			/*RCR_AAP |*/
			RCR_APP_PHYST_STAFF |
			/* Accept PHY status */
			RCR_APP_PHYST_RXFF |
			(earlyrxthreshold << RCR_FIFO_OFFSET);

	rtlpci->irq_mask[0] = (u32)
			(IMR_ROK |
			IMR_VODOK |
			IMR_VIDOK |
			IMR_BEDOK |
			IMR_BKDOK |
			IMR_HCCADOK |
			IMR_MGNTDOK |
			IMR_COMDOK |
			IMR_HIGHDOK |
			IMR_BDOK |
			IMR_RXCMDOK |
			/*IMR_TIMEOUT0 |*/
			IMR_RDU |
			IMR_RXFOVW	|
			IMR_BCNINT
			/*| IMR_TXFOVW*/
			/*| IMR_TBDOK |
			IMR_TBDER*/);

	rtlpci->irq_mask[1] = (u32) 0;

	rtlpci->shortretry_limit = 0x30;
	rtlpci->longretry_limit = 0x30;

	rtlpci->first_init = true;

	/* for LPS & IPS */
	rtlpriv->psc.inactiveps = rtlpriv->cfg->mod_params->inactiveps;
	rtlpriv->psc.swctrl_lps = rtlpriv->cfg->mod_params->swctrl_lps;
	rtlpriv->psc.fwctrl_lps = rtlpriv->cfg->mod_params->fwctrl_lps;
	if (!rtlpriv->psc.inactiveps)
		pr_info("Power Save off (module option)\n");
	if (!rtlpriv->psc.fwctrl_lps)
		pr_info("FW Power Save off (module option)\n");
	rtlpriv->psc.reg_fwctrl_lps = 3;
	rtlpriv->psc.reg_max_lps_awakeintvl = 5;
	/* for ASPM, you can close aspm through
	 * set const_support_pciaspm = 0 */
	rtl92s_init_aspm_vars(hw);

	if (rtlpriv->psc.reg_fwctrl_lps == 1)
		rtlpriv->psc.fwctrl_psmode = FW_PS_MIN_MODE;
	else if (rtlpriv->psc.reg_fwctrl_lps == 2)
		rtlpriv->psc.fwctrl_psmode = FW_PS_MAX_MODE;
	else if (rtlpriv->psc.reg_fwctrl_lps == 3)
		rtlpriv->psc.fwctrl_psmode = FW_PS_DTIM_MODE;

	/* for firmware buf */
	rtlpriv->rtlhal.pfirmware = vzalloc(sizeof(struct rt_firmware));
	if (!rtlpriv->rtlhal.pfirmware)
		return 1;

	rtlpriv->max_fw_size = RTL8190_MAX_FIRMWARE_CODE_SIZE*2 +
			       sizeof(struct fw_hdr);
	pr_info("Driver for Realtek RTL8192SE/RTL8191SE\n"
		"Loading firmware %s\n", fw_name);
	/* request fw */
	err = request_firmware_nowait(THIS_MODULE, 1, fw_name,
				      rtlpriv->io.dev, GFP_KERNEL, hw,
				      rtl92se_fw_cb);
	if (err) {
		pr_err("Failed to request firmware!\n");
		vfree(rtlpriv->rtlhal.pfirmware);
		rtlpriv->rtlhal.pfirmware = NULL;
		return 1;
	}

	return err;
}

static void rtl92s_deinit_sw_vars(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (rtlpriv->rtlhal.pfirmware) {
		vfree(rtlpriv->rtlhal.pfirmware);
		rtlpriv->rtlhal.pfirmware = NULL;
	}
}

static bool rtl92se_is_tx_desc_closed(struct ieee80211_hw *hw, u8 hw_queue,
				      u16 index)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl8192_tx_ring *ring = &rtlpci->tx_ring[hw_queue];
	u8 *entry = (u8 *)(&ring->desc[ring->idx]);
	u8 own = (u8)rtl92se_get_desc(hw, entry, true, HW_DESC_OWN);

	if (own)
		return false;
	return true;
}

static struct rtl_hal_ops rtl8192se_hal_ops = {
	.init_sw_vars = rtl92s_init_sw_vars,
	.deinit_sw_vars = rtl92s_deinit_sw_vars,
	.read_eeprom_info = rtl92se_read_eeprom_info,
	.interrupt_recognized = rtl92se_interrupt_recognized,
	.hw_init = rtl92se_hw_init,
	.hw_disable = rtl92se_card_disable,
	.hw_suspend = rtl92se_suspend,
	.hw_resume = rtl92se_resume,
	.enable_interrupt = rtl92se_enable_interrupt,
	.disable_interrupt = rtl92se_disable_interrupt,
	.set_network_type = rtl92se_set_network_type,
	.set_chk_bssid = rtl92se_set_check_bssid,
	.set_qos = rtl92se_set_qos,
	.set_bcn_reg = rtl92se_set_beacon_related_registers,
	.set_bcn_intv = rtl92se_set_beacon_interval,
	.update_interrupt_mask = rtl92se_update_interrupt_mask,
	.get_hw_reg = rtl92se_get_hw_reg,
	.set_hw_reg = rtl92se_set_hw_reg,
	.update_rate_tbl = rtl92se_update_hal_rate_tbl,
	.fill_tx_desc = rtl92se_tx_fill_desc,
	.fill_tx_cmddesc = rtl92se_tx_fill_cmddesc,
	.query_rx_desc = rtl92se_rx_query_desc,
	.set_channel_access = rtl92se_update_channel_access_setting,
	.radio_onoff_checking = rtl92se_gpio_radio_on_off_checking,
	.set_bw_mode = rtl92s_phy_set_bw_mode,
	.switch_channel = rtl92s_phy_sw_chnl,
	.dm_watchdog = rtl92s_dm_watchdog,
	.scan_operation_backup = rtl92s_phy_scan_operation_backup,
	.set_rf_power_state = rtl92s_phy_set_rf_power_state,
	.led_control = rtl92se_led_control,
	.set_desc = rtl92se_set_desc,
	.get_desc = rtl92se_get_desc,
	.is_tx_desc_closed = rtl92se_is_tx_desc_closed,
	.tx_polling = rtl92se_tx_polling,
	.enable_hw_sec = rtl92se_enable_hw_security_config,
	.set_key = rtl92se_set_key,
	.init_sw_leds = rtl92se_init_sw_leds,
	.get_bbreg = rtl92s_phy_query_bb_reg,
	.set_bbreg = rtl92s_phy_set_bb_reg,
	.get_rfreg = rtl92s_phy_query_rf_reg,
	.set_rfreg = rtl92s_phy_set_rf_reg,
	.get_btc_status = rtl_btc_status_false,
};

static struct rtl_mod_params rtl92se_mod_params = {
	.sw_crypto = false,
	.inactiveps = true,
	.swctrl_lps = true,
	.fwctrl_lps = false,
	.aspm_support = 2,
	.debug_level = 0,
	.debug_mask = 0,
};

/* Because memory R/W bursting will cause system hang/crash
 * for 92se, so we don't read back after every write action */
static const struct rtl_hal_cfg rtl92se_hal_cfg = {
	.bar_id = 1,
	.write_readback = false,
	.name = "rtl92s_pci",
	.ops = &rtl8192se_hal_ops,
	.mod_params = &rtl92se_mod_params,

	.maps[SYS_ISO_CTRL] = REG_SYS_ISO_CTRL,
	.maps[SYS_FUNC_EN] = REG_SYS_FUNC_EN,
	.maps[SYS_CLK] = SYS_CLKR,
	.maps[MAC_RCR_AM] = RCR_AM,
	.maps[MAC_RCR_AB] = RCR_AB,
	.maps[MAC_RCR_ACRC32] = RCR_ACRC32,
	.maps[MAC_RCR_ACF] = RCR_ACF,
	.maps[MAC_RCR_AAP] = RCR_AAP,
	.maps[MAC_HIMR] = INTA_MASK,
	.maps[MAC_HIMRE] = INTA_MASK + 4,

	.maps[EFUSE_TEST] = REG_EFUSE_TEST,
	.maps[EFUSE_CTRL] = REG_EFUSE_CTRL,
	.maps[EFUSE_CLK] = REG_EFUSE_CLK,
	.maps[EFUSE_CLK_CTRL] = REG_EFUSE_CTRL,
	.maps[EFUSE_PWC_EV12V] = 0, /* nouse for 8192se */
	.maps[EFUSE_FEN_ELDR] = 0, /* nouse for 8192se */
	.maps[EFUSE_LOADER_CLK_EN] = 0,/* nouse for 8192se */
	.maps[EFUSE_ANA8M] = EFUSE_ANA8M,
	.maps[EFUSE_HWSET_MAX_SIZE] = HWSET_MAX_SIZE_92S,
	.maps[EFUSE_MAX_SECTION_MAP] = EFUSE_MAX_SECTION,
	.maps[EFUSE_REAL_CONTENT_SIZE] = EFUSE_REAL_CONTENT_LEN,
	.maps[EFUSE_OOB_PROTECT_BYTES_LEN] = EFUSE_OOB_PROTECT_BYTES,

	.maps[RWCAM] = REG_RWCAM,
	.maps[WCAMI] = REG_WCAMI,
	.maps[RCAMO] = REG_RCAMO,
	.maps[CAMDBG] = REG_CAMDBG,
	.maps[SECR] = REG_SECR,
	.maps[SEC_CAM_NONE] = CAM_NONE,
	.maps[SEC_CAM_WEP40] = CAM_WEP40,
	.maps[SEC_CAM_TKIP] = CAM_TKIP,
	.maps[SEC_CAM_AES] = CAM_AES,
	.maps[SEC_CAM_WEP104] = CAM_WEP104,

	.maps[RTL_IMR_BCNDMAINT6] = IMR_BCNDMAINT6,
	.maps[RTL_IMR_BCNDMAINT5] = IMR_BCNDMAINT5,
	.maps[RTL_IMR_BCNDMAINT4] = IMR_BCNDMAINT4,
	.maps[RTL_IMR_BCNDMAINT3] = IMR_BCNDMAINT3,
	.maps[RTL_IMR_BCNDMAINT2] = IMR_BCNDMAINT2,
	.maps[RTL_IMR_BCNDMAINT1] = IMR_BCNDMAINT1,
	.maps[RTL_IMR_BCNDOK8] = IMR_BCNDOK8,
	.maps[RTL_IMR_BCNDOK7] = IMR_BCNDOK7,
	.maps[RTL_IMR_BCNDOK6] = IMR_BCNDOK6,
	.maps[RTL_IMR_BCNDOK5] = IMR_BCNDOK5,
	.maps[RTL_IMR_BCNDOK4] = IMR_BCNDOK4,
	.maps[RTL_IMR_BCNDOK3] = IMR_BCNDOK3,
	.maps[RTL_IMR_BCNDOK2] = IMR_BCNDOK2,
	.maps[RTL_IMR_BCNDOK1] = IMR_BCNDOK1,
	.maps[RTL_IMR_TIMEOUT2] = IMR_TIMEOUT2,
	.maps[RTL_IMR_TIMEOUT1] = IMR_TIMEOUT1,

	.maps[RTL_IMR_TXFOVW] = IMR_TXFOVW,
	.maps[RTL_IMR_PSTIMEOUT] = IMR_PSTIMEOUT,
	.maps[RTL_IMR_BCNINT] = IMR_BCNINT,
	.maps[RTL_IMR_RXFOVW] = IMR_RXFOVW,
	.maps[RTL_IMR_RDU] = IMR_RDU,
	.maps[RTL_IMR_ATIMEND] = IMR_ATIMEND,
	.maps[RTL_IMR_BDOK] = IMR_BDOK,
	.maps[RTL_IMR_MGNTDOK] = IMR_MGNTDOK,
	.maps[RTL_IMR_TBDER] = IMR_TBDER,
	.maps[RTL_IMR_HIGHDOK] = IMR_HIGHDOK,
	.maps[RTL_IMR_COMDOK] = IMR_COMDOK,
	.maps[RTL_IMR_TBDOK] = IMR_TBDOK,
	.maps[RTL_IMR_BKDOK] = IMR_BKDOK,
	.maps[RTL_IMR_BEDOK] = IMR_BEDOK,
	.maps[RTL_IMR_VIDOK] = IMR_VIDOK,
	.maps[RTL_IMR_VODOK] = IMR_VODOK,
	.maps[RTL_IMR_ROK] = IMR_ROK,
	.maps[RTL_IBSS_INT_MASKS] = (IMR_BCNINT | IMR_TBDOK | IMR_TBDER),

	.maps[RTL_RC_CCK_RATE1M] = DESC_RATE1M,
	.maps[RTL_RC_CCK_RATE2M] = DESC_RATE2M,
	.maps[RTL_RC_CCK_RATE5_5M] = DESC_RATE5_5M,
	.maps[RTL_RC_CCK_RATE11M] = DESC_RATE11M,
	.maps[RTL_RC_OFDM_RATE6M] = DESC_RATE6M,
	.maps[RTL_RC_OFDM_RATE9M] = DESC_RATE9M,
	.maps[RTL_RC_OFDM_RATE12M] = DESC_RATE12M,
	.maps[RTL_RC_OFDM_RATE18M] = DESC_RATE18M,
	.maps[RTL_RC_OFDM_RATE24M] = DESC_RATE24M,
	.maps[RTL_RC_OFDM_RATE36M] = DESC_RATE36M,
	.maps[RTL_RC_OFDM_RATE48M] = DESC_RATE48M,
	.maps[RTL_RC_OFDM_RATE54M] = DESC_RATE54M,

	.maps[RTL_RC_HT_RATEMCS7] = DESC_RATEMCS7,
	.maps[RTL_RC_HT_RATEMCS15] = DESC_RATEMCS15,
};

static const struct pci_device_id rtl92se_pci_ids[] = {
	{RTL_PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8192, rtl92se_hal_cfg)},
	{RTL_PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8171, rtl92se_hal_cfg)},
	{RTL_PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8172, rtl92se_hal_cfg)},
	{RTL_PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8173, rtl92se_hal_cfg)},
	{RTL_PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8174, rtl92se_hal_cfg)},
	{},
};

MODULE_DEVICE_TABLE(pci, rtl92se_pci_ids);

MODULE_AUTHOR("lizhaoming	<chaoming_li@realsil.com.cn>");
MODULE_AUTHOR("Realtek WlanFAE	<wlanfae@realtek.com>");
MODULE_AUTHOR("Larry Finger	<Larry.Finger@lwfinger.net>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek 8192S/8191S 802.11n PCI wireless");
MODULE_FIRMWARE("rtlwifi/rtl8192sefw.bin");

module_param_named(swenc, rtl92se_mod_params.sw_crypto, bool, 0444);
module_param_named(debug_level, rtl92se_mod_params.debug_level, int, 0644);
module_param_named(debug_mask, rtl92se_mod_params.debug_mask, ullong, 0644);
module_param_named(ips, rtl92se_mod_params.inactiveps, bool, 0444);
module_param_named(swlps, rtl92se_mod_params.swctrl_lps, bool, 0444);
module_param_named(fwlps, rtl92se_mod_params.fwctrl_lps, bool, 0444);
module_param_named(aspm, rtl92se_mod_params.aspm_support, int, 0444);
MODULE_PARM_DESC(swenc, "Set to 1 for software crypto (default 0)\n");
MODULE_PARM_DESC(ips, "Set to 0 to not use link power save (default 1)\n");
MODULE_PARM_DESC(swlps, "Set to 1 to use SW control power save (default 1)\n");
MODULE_PARM_DESC(fwlps, "Set to 1 to use FW control power save (default 0)\n");
MODULE_PARM_DESC(aspm, "Set to 1 to enable ASPM (default 1)\n");
MODULE_PARM_DESC(debug_level, "Set debug level (0-5) (default 0)");
MODULE_PARM_DESC(debug_mask, "Set debug mask (default 0)");

static SIMPLE_DEV_PM_OPS(rtlwifi_pm_ops, rtl_pci_suspend, rtl_pci_resume);

static struct pci_driver rtl92se_driver = {
	.name = KBUILD_MODNAME,
	.id_table = rtl92se_pci_ids,
	.probe = rtl_pci_probe,
	.remove = rtl_pci_disconnect,
	.driver.pm = &rtlwifi_pm_ops,
};

module_pci_driver(rtl92se_driver);
