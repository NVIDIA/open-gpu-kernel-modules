/*
 * Copyright (c) 2018 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <linux/mlx5/device.h>

#include "accel/tls.h"
#include "mlx5_core.h"
#include "lib/mlx5.h"

#ifdef CONFIG_MLX5_FPGA_TLS
#include "fpga/tls.h"

int mlx5_accel_tls_add_flow(struct mlx5_core_dev *mdev, void *flow,
			    struct tls_crypto_info *crypto_info,
			    u32 start_offload_tcp_sn, u32 *p_swid,
			    bool direction_sx)
{
	return mlx5_fpga_tls_add_flow(mdev, flow, crypto_info,
				      start_offload_tcp_sn, p_swid,
				      direction_sx);
}

void mlx5_accel_tls_del_flow(struct mlx5_core_dev *mdev, u32 swid,
			     bool direction_sx)
{
	mlx5_fpga_tls_del_flow(mdev, swid, GFP_KERNEL, direction_sx);
}

int mlx5_accel_tls_resync_rx(struct mlx5_core_dev *mdev, __be32 handle,
			     u32 seq, __be64 rcd_sn)
{
	return mlx5_fpga_tls_resync_rx(mdev, handle, seq, rcd_sn);
}

bool mlx5_accel_is_tls_device(struct mlx5_core_dev *mdev)
{
	return mlx5_fpga_is_tls_device(mdev) ||
		mlx5_accel_is_ktls_device(mdev);
}

u32 mlx5_accel_tls_device_caps(struct mlx5_core_dev *mdev)
{
	return mlx5_fpga_tls_device_caps(mdev);
}

int mlx5_accel_tls_init(struct mlx5_core_dev *mdev)
{
	return mlx5_fpga_tls_init(mdev);
}

void mlx5_accel_tls_cleanup(struct mlx5_core_dev *mdev)
{
	mlx5_fpga_tls_cleanup(mdev);
}
#endif

#ifdef CONFIG_MLX5_TLS
int mlx5_ktls_create_key(struct mlx5_core_dev *mdev,
			 struct tls_crypto_info *crypto_info,
			 u32 *p_key_id)
{
	u32 sz_bytes;
	void *key;

	switch (crypto_info->cipher_type) {
	case TLS_CIPHER_AES_GCM_128: {
		struct tls12_crypto_info_aes_gcm_128 *info =
			(struct tls12_crypto_info_aes_gcm_128 *)crypto_info;

		key      = info->key;
		sz_bytes = sizeof(info->key);
		break;
	}
	case TLS_CIPHER_AES_GCM_256: {
		struct tls12_crypto_info_aes_gcm_256 *info =
			(struct tls12_crypto_info_aes_gcm_256 *)crypto_info;

		key      = info->key;
		sz_bytes = sizeof(info->key);
		break;
	}
	default:
		return -EINVAL;
	}

	return mlx5_create_encryption_key(mdev, key, sz_bytes,
					  MLX5_ACCEL_OBJ_TLS_KEY,
					  p_key_id);
}

void mlx5_ktls_destroy_key(struct mlx5_core_dev *mdev, u32 key_id)
{
	mlx5_destroy_encryption_key(mdev, key_id);
}
#endif
