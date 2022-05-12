/*
 * Copyright 2020 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "amdgpu.h"
#include "amdgpu_psp.h"
#include "amdgpu_ucode.h"
#include "soc15_common.h"
#include "psp_v13_0.h"

#include "mp/mp_13_0_2_offset.h"
#include "mp/mp_13_0_2_sh_mask.h"

MODULE_FIRMWARE("amdgpu/aldebaran_sos.bin");
MODULE_FIRMWARE("amdgpu/aldebaran_ta.bin");
MODULE_FIRMWARE("amdgpu/yellow_carp_asd.bin");
MODULE_FIRMWARE("amdgpu/yellow_carp_toc.bin");
MODULE_FIRMWARE("amdgpu/yellow_carp_ta.bin");
MODULE_FIRMWARE("amdgpu/psp_13_0_8_asd.bin");
MODULE_FIRMWARE("amdgpu/psp_13_0_8_toc.bin");
MODULE_FIRMWARE("amdgpu/psp_13_0_8_ta.bin");

/* For large FW files the time to complete can be very long */
#define USBC_PD_POLLING_LIMIT_S 240

/* Read USB-PD from LFB */
#define GFX_CMD_USB_PD_USE_LFB 0x480

static int psp_v13_0_init_microcode(struct psp_context *psp)
{
	struct amdgpu_device *adev = psp->adev;
	const char *chip_name;
	int err = 0;

	switch (adev->ip_versions[MP0_HWIP][0]) {
	case IP_VERSION(13, 0, 2):
		chip_name = "aldebaran";
		break;
	case IP_VERSION(13, 0, 1):
	case IP_VERSION(13, 0, 3):
		chip_name = "yellow_carp";
		break;
	case IP_VERSION(13, 0, 8):
		chip_name = "psp_13_0_8";
		break;
	default:
		BUG();
	}
	switch (adev->ip_versions[MP0_HWIP][0]) {
	case IP_VERSION(13, 0, 2):
		err = psp_init_sos_microcode(psp, chip_name);
		if (err)
			return err;
		err = psp_init_ta_microcode(&adev->psp, chip_name);
		if (err)
			return err;
		break;
	case IP_VERSION(13, 0, 1):
	case IP_VERSION(13, 0, 3):
	case IP_VERSION(13, 0, 8):
		err = psp_init_asd_microcode(psp, chip_name);
		if (err)
			return err;
		err = psp_init_toc_microcode(psp, chip_name);
		if (err)
			return err;
		err = psp_init_ta_microcode(psp, chip_name);
		if (err)
			return err;
		break;
	default:
		BUG();
	}

	return 0;
}

static bool psp_v13_0_is_sos_alive(struct psp_context *psp)
{
	struct amdgpu_device *adev = psp->adev;
	uint32_t sol_reg;

	sol_reg = RREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_81);

	return sol_reg != 0x0;
}

static int psp_v13_0_wait_for_bootloader(struct psp_context *psp)
{
	struct amdgpu_device *adev = psp->adev;

	int ret;
	int retry_loop;

	for (retry_loop = 0; retry_loop < 10; retry_loop++) {
		/* Wait for bootloader to signify that is
		    ready having bit 31 of C2PMSG_35 set to 1 */
		ret = psp_wait_for(psp,
				   SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_35),
				   0x80000000,
				   0x80000000,
				   false);

		if (ret == 0)
			return 0;
	}

	return ret;
}

static int psp_v13_0_bootloader_load_component(struct psp_context  	*psp,
					       struct psp_bin_desc 	*bin_desc,
					       enum psp_bootloader_cmd  bl_cmd)
{
	int ret;
	uint32_t psp_gfxdrv_command_reg = 0;
	struct amdgpu_device *adev = psp->adev;

	/* Check tOS sign of life register to confirm sys driver and sOS
	 * are already been loaded.
	 */
	if (psp_v13_0_is_sos_alive(psp))
		return 0;

	ret = psp_v13_0_wait_for_bootloader(psp);
	if (ret)
		return ret;

	memset(psp->fw_pri_buf, 0, PSP_1_MEG);

	/* Copy PSP KDB binary to memory */
	memcpy(psp->fw_pri_buf, bin_desc->start_addr, bin_desc->size_bytes);

	/* Provide the PSP KDB to bootloader */
	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_36,
	       (uint32_t)(psp->fw_pri_mc_addr >> 20));
	psp_gfxdrv_command_reg = bl_cmd;
	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_35,
	       psp_gfxdrv_command_reg);

	ret = psp_v13_0_wait_for_bootloader(psp);

	return ret;
}

static int psp_v13_0_bootloader_load_kdb(struct psp_context *psp)
{
	return psp_v13_0_bootloader_load_component(psp, &psp->kdb, PSP_BL__LOAD_KEY_DATABASE);
}

static int psp_v13_0_bootloader_load_sysdrv(struct psp_context *psp)
{
	return psp_v13_0_bootloader_load_component(psp, &psp->sys, PSP_BL__LOAD_SYSDRV);
}

static int psp_v13_0_bootloader_load_soc_drv(struct psp_context *psp)
{
	return psp_v13_0_bootloader_load_component(psp, &psp->soc_drv, PSP_BL__LOAD_SOCDRV);
}

static int psp_v13_0_bootloader_load_intf_drv(struct psp_context *psp)
{
	return psp_v13_0_bootloader_load_component(psp, &psp->intf_drv, PSP_BL__LOAD_INTFDRV);
}

static int psp_v13_0_bootloader_load_dbg_drv(struct psp_context *psp)
{
	return psp_v13_0_bootloader_load_component(psp, &psp->dbg_drv, PSP_BL__LOAD_DBGDRV);
}

static int psp_v13_0_bootloader_load_sos(struct psp_context *psp)
{
	int ret;
	unsigned int psp_gfxdrv_command_reg = 0;
	struct amdgpu_device *adev = psp->adev;

	/* Check sOS sign of life register to confirm sys driver and sOS
	 * are already been loaded.
	 */
	if (psp_v13_0_is_sos_alive(psp))
		return 0;

	ret = psp_v13_0_wait_for_bootloader(psp);
	if (ret)
		return ret;

	memset(psp->fw_pri_buf, 0, PSP_1_MEG);

	/* Copy Secure OS binary to PSP memory */
	memcpy(psp->fw_pri_buf, psp->sos.start_addr, psp->sos.size_bytes);

	/* Provide the PSP secure OS to bootloader */
	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_36,
	       (uint32_t)(psp->fw_pri_mc_addr >> 20));
	psp_gfxdrv_command_reg = PSP_BL__LOAD_SOSDRV;
	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_35,
	       psp_gfxdrv_command_reg);

	/* there might be handshake issue with hardware which needs delay */
	mdelay(20);
	ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_81),
			   RREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_81),
			   0, true);

	return ret;
}

static int psp_v13_0_ring_init(struct psp_context *psp,
			      enum psp_ring_type ring_type)
{
	int ret = 0;
	struct psp_ring *ring;
	struct amdgpu_device *adev = psp->adev;

	ring = &psp->km_ring;

	ring->ring_type = ring_type;

	/* allocate 4k Page of Local Frame Buffer memory for ring */
	ring->ring_size = 0x1000;
	ret = amdgpu_bo_create_kernel(adev, ring->ring_size, PAGE_SIZE,
				      AMDGPU_GEM_DOMAIN_VRAM,
				      &adev->firmware.rbuf,
				      &ring->ring_mem_mc_addr,
				      (void **)&ring->ring_mem);
	if (ret) {
		ring->ring_size = 0;
		return ret;
	}

	return 0;
}

static int psp_v13_0_ring_stop(struct psp_context *psp,
			       enum psp_ring_type ring_type)
{
	int ret = 0;
	struct amdgpu_device *adev = psp->adev;

	if (amdgpu_sriov_vf(adev)) {
		/* Write the ring destroy command*/
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_101,
			     GFX_CTRL_CMD_ID_DESTROY_GPCOM_RING);
		/* there might be handshake issue with hardware which needs delay */
		mdelay(20);
		/* Wait for response flag (bit 31) */
		ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_101),
				   0x80000000, 0x80000000, false);
	} else {
		/* Write the ring destroy command*/
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_64,
			     GFX_CTRL_CMD_ID_DESTROY_RINGS);
		/* there might be handshake issue with hardware which needs delay */
		mdelay(20);
		/* Wait for response flag (bit 31) */
		ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_64),
				   0x80000000, 0x80000000, false);
	}

	return ret;
}

static int psp_v13_0_ring_create(struct psp_context *psp,
				 enum psp_ring_type ring_type)
{
	int ret = 0;
	unsigned int psp_ring_reg = 0;
	struct psp_ring *ring = &psp->km_ring;
	struct amdgpu_device *adev = psp->adev;

	if (amdgpu_sriov_vf(adev)) {
		ret = psp_v13_0_ring_stop(psp, ring_type);
		if (ret) {
			DRM_ERROR("psp_v13_0_ring_stop_sriov failed!\n");
			return ret;
		}

		/* Write low address of the ring to C2PMSG_102 */
		psp_ring_reg = lower_32_bits(ring->ring_mem_mc_addr);
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_102, psp_ring_reg);
		/* Write high address of the ring to C2PMSG_103 */
		psp_ring_reg = upper_32_bits(ring->ring_mem_mc_addr);
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_103, psp_ring_reg);

		/* Write the ring initialization command to C2PMSG_101 */
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_101,
			     GFX_CTRL_CMD_ID_INIT_GPCOM_RING);

		/* there might be handshake issue with hardware which needs delay */
		mdelay(20);

		/* Wait for response flag (bit 31) in C2PMSG_101 */
		ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_101),
				   0x80000000, 0x8000FFFF, false);

	} else {
		/* Wait for sOS ready for ring creation */
		ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_64),
				   0x80000000, 0x80000000, false);
		if (ret) {
			DRM_ERROR("Failed to wait for trust OS ready for ring creation\n");
			return ret;
		}

		/* Write low address of the ring to C2PMSG_69 */
		psp_ring_reg = lower_32_bits(ring->ring_mem_mc_addr);
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_69, psp_ring_reg);
		/* Write high address of the ring to C2PMSG_70 */
		psp_ring_reg = upper_32_bits(ring->ring_mem_mc_addr);
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_70, psp_ring_reg);
		/* Write size of ring to C2PMSG_71 */
		psp_ring_reg = ring->ring_size;
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_71, psp_ring_reg);
		/* Write the ring initialization command to C2PMSG_64 */
		psp_ring_reg = ring_type;
		psp_ring_reg = psp_ring_reg << 16;
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_64, psp_ring_reg);

		/* there might be handshake issue with hardware which needs delay */
		mdelay(20);

		/* Wait for response flag (bit 31) in C2PMSG_64 */
		ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_64),
				   0x80000000, 0x8000FFFF, false);
	}

	return ret;
}

static int psp_v13_0_ring_destroy(struct psp_context *psp,
				  enum psp_ring_type ring_type)
{
	int ret = 0;
	struct psp_ring *ring = &psp->km_ring;
	struct amdgpu_device *adev = psp->adev;

	ret = psp_v13_0_ring_stop(psp, ring_type);
	if (ret)
		DRM_ERROR("Fail to stop psp ring\n");

	amdgpu_bo_free_kernel(&adev->firmware.rbuf,
			      &ring->ring_mem_mc_addr,
			      (void **)&ring->ring_mem);

	return ret;
}

static uint32_t psp_v13_0_ring_get_wptr(struct psp_context *psp)
{
	uint32_t data;
	struct amdgpu_device *adev = psp->adev;

	if (amdgpu_sriov_vf(adev))
		data = RREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_102);
	else
		data = RREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_67);

	return data;
}

static void psp_v13_0_ring_set_wptr(struct psp_context *psp, uint32_t value)
{
	struct amdgpu_device *adev = psp->adev;

	if (amdgpu_sriov_vf(adev)) {
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_102, value);
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_101,
			     GFX_CTRL_CMD_ID_CONSUME_CMD);
	} else
		WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_67, value);
}

static int psp_v13_0_load_usbc_pd_fw(struct psp_context *psp, uint64_t fw_pri_mc_addr)
{
	struct amdgpu_device *adev = psp->adev;
	uint32_t reg_status;
	int ret, i = 0;

	/*
	 * LFB address which is aligned to 1MB address and has to be
	 * right-shifted by 20 so that LFB address can be passed on a 32-bit C2P
	 * register
	 */
	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_36, (fw_pri_mc_addr >> 20));

	ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_35),
			     0x80000000, 0x80000000, false);
	if (ret)
		return ret;

	/* Fireup interrupt so PSP can pick up the address */
	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_35, (GFX_CMD_USB_PD_USE_LFB << 16));

	/* FW load takes very long time */
	do {
		msleep(1000);
		reg_status = RREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_35);

		if (reg_status & 0x80000000)
			goto done;

	} while (++i < USBC_PD_POLLING_LIMIT_S);

	return -ETIME;
done:

	if ((reg_status & 0xFFFF) != 0) {
		DRM_ERROR("Address load failed - MP0_SMN_C2PMSG_35.Bits [15:0] = %04x\n",
				reg_status & 0xFFFF);
		return -EIO;
	}

	return 0;
}

static int psp_v13_0_read_usbc_pd_fw(struct psp_context *psp, uint32_t *fw_ver)
{
	struct amdgpu_device *adev = psp->adev;
	int ret;

	WREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_35, C2PMSG_CMD_GFX_USB_PD_FW_VER);

	ret = psp_wait_for(psp, SOC15_REG_OFFSET(MP0, 0, regMP0_SMN_C2PMSG_35),
				     0x80000000, 0x80000000, false);
	if (!ret)
		*fw_ver = RREG32_SOC15(MP0, 0, regMP0_SMN_C2PMSG_36);

	return ret;
}

static const struct psp_funcs psp_v13_0_funcs = {
	.init_microcode = psp_v13_0_init_microcode,
	.bootloader_load_kdb = psp_v13_0_bootloader_load_kdb,
	.bootloader_load_sysdrv = psp_v13_0_bootloader_load_sysdrv,
	.bootloader_load_soc_drv = psp_v13_0_bootloader_load_soc_drv,
	.bootloader_load_intf_drv = psp_v13_0_bootloader_load_intf_drv,
	.bootloader_load_dbg_drv = psp_v13_0_bootloader_load_dbg_drv,
	.bootloader_load_sos = psp_v13_0_bootloader_load_sos,
	.ring_init = psp_v13_0_ring_init,
	.ring_create = psp_v13_0_ring_create,
	.ring_stop = psp_v13_0_ring_stop,
	.ring_destroy = psp_v13_0_ring_destroy,
	.ring_get_wptr = psp_v13_0_ring_get_wptr,
	.ring_set_wptr = psp_v13_0_ring_set_wptr,
	.load_usbc_pd_fw = psp_v13_0_load_usbc_pd_fw,
	.read_usbc_pd_fw = psp_v13_0_read_usbc_pd_fw
};

void psp_v13_0_set_psp_funcs(struct psp_context *psp)
{
	psp->funcs = &psp_v13_0_funcs;
}
