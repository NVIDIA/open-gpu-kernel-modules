/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define  __NO_VERSION__

#include <linux/i2c.h>

#include "os-interface.h"
#include "nv-linux.h"

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)

static int nv_i2c_algo_master_xfer(struct i2c_adapter *adapter, struct i2c_msg msgs[], int num)
{
    nv_state_t *nv = (nv_state_t *)adapter->algo_data;
    unsigned int i = 0;
    int rc;
    NV_STATUS rmStatus = NV_OK;
    nvidia_stack_t *sp = NULL;
    const unsigned int supported_i2c_flags = I2C_M_RD
#if defined(I2C_M_DMA_SAFE)
    | I2C_M_DMA_SAFE
#endif
    ;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        return rc;
    }

    rc = -EIO;

    for (i = 0; ((i < (unsigned int)num) && (rmStatus == NV_OK)); i++)
    {
        if (msgs[i].flags & ~supported_i2c_flags)
        {
            /* we only support basic I2C reads/writes, reject any other commands */
            rc = -EINVAL;
            nv_printf(NV_DBG_ERRORS, "NVRM: Unsupported I2C flags used. (flags:0x%08x)\n",
                      msgs[i].flags);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
        }
        else
        {
            rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                       (msgs[i].flags & I2C_M_RD) ?
                                           NV_I2C_CMD_READ : NV_I2C_CMD_WRITE,
                                       (NvU8)(msgs[i].addr & 0x7f), 0,
                                       (NvU32)(msgs[i].len & 0xffffUL),
                                       (NvU8 *)msgs[i].buf);
        }
    }

    nv_kmem_cache_free_stack(sp);

    return (rmStatus != NV_OK) ? rc : num;
}

static int nv_i2c_algo_smbus_xfer(
    struct i2c_adapter *adapter,
    u16 addr,
    unsigned short flags,
    char read_write,
    u8 command,
    int size,
    union i2c_smbus_data *data
)
{
    nv_state_t *nv = (nv_state_t *)adapter->algo_data;
    int rc;
    NV_STATUS rmStatus = NV_OK;
    nvidia_stack_t *sp = NULL;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        return rc;
    }

    rc = -EIO;

    switch (size)
    {
        case I2C_SMBUS_QUICK:
            rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                       (read_write == I2C_SMBUS_READ) ?
                                           NV_I2C_CMD_SMBUS_QUICK_READ :
                                           NV_I2C_CMD_SMBUS_QUICK_WRITE,
                                       (NvU8)(addr & 0x7f), 0, 0, NULL);
            break;

        case I2C_SMBUS_BYTE:
            if (read_write == I2C_SMBUS_READ)
            {
                rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                           NV_I2C_CMD_READ,
                                           (NvU8)(addr & 0x7f), 0, 1,
                                           (NvU8 *)&data->byte);
            }
            else
            {
                u8 data = command;
                rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                           NV_I2C_CMD_WRITE,
                                           (NvU8)(addr & 0x7f), 0, 1,
                                           (NvU8 *)&data);
            }
            break;

        case I2C_SMBUS_BYTE_DATA:
            rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                       (read_write == I2C_SMBUS_READ) ?
                                           NV_I2C_CMD_SMBUS_READ :
                                           NV_I2C_CMD_SMBUS_WRITE,
                                       (NvU8)(addr & 0x7f), (NvU8)command, 1,
                                       (NvU8 *)&data->byte);
            break;

        case I2C_SMBUS_WORD_DATA:
            if (read_write != I2C_SMBUS_READ)
            {
                u16 word = data->word;
                data->block[1] = (word & 0xff);
                data->block[2] = (word >> 8);
            }

            rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                       (read_write == I2C_SMBUS_READ) ?
                                           NV_I2C_CMD_SMBUS_READ :
                                           NV_I2C_CMD_SMBUS_WRITE,
                                       (NvU8)(addr & 0x7f), (NvU8)command, 2,
                                       (NvU8 *)&data->block[1]);

            if (read_write == I2C_SMBUS_READ)
            {
                data->word = ((NvU16)data->block[1]) |
                             ((NvU16)data->block[2] << 8);
            }
            break;

        case I2C_SMBUS_BLOCK_DATA:
            rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                        (read_write == I2C_SMBUS_READ) ?
                                            NV_I2C_CMD_SMBUS_BLOCK_READ :
                                            NV_I2C_CMD_SMBUS_BLOCK_WRITE,
                                        (NvU8)(addr & 0x7f), (NvU8)command,
                                        sizeof(data->block),
                                        (NvU8 *)data->block);
            break;

        case I2C_SMBUS_I2C_BLOCK_DATA:
            rmStatus = rm_i2c_transfer(sp, nv, (void *)adapter,
                                       (read_write == I2C_SMBUS_READ) ?
                                           NV_I2C_CMD_BLOCK_READ :
                                           NV_I2C_CMD_BLOCK_WRITE,
                                       (NvU8)(addr & 0x7f), (NvU8)command,
                                       (NvU8)data->block[0],
                                       (NvU8 *)&data->block[1]);
            break;

        default:
            rc = -EINVAL;
            rmStatus = NV_ERR_INVALID_ARGUMENT;
    }

    nv_kmem_cache_free_stack(sp);

    return (rmStatus != NV_OK) ? rc : 0;
}

static u32 nv_i2c_algo_functionality(struct i2c_adapter *adapter)
{
    nv_state_t *nv = (nv_state_t *)adapter->algo_data;
    u32 ret = I2C_FUNC_I2C;
    nvidia_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return 0;
    }

    if (rm_i2c_is_smbus_capable(sp, nv, adapter))
    {
        ret |= (I2C_FUNC_SMBUS_QUICK |
                I2C_FUNC_SMBUS_BYTE |
                I2C_FUNC_SMBUS_BYTE_DATA |
                I2C_FUNC_SMBUS_WORD_DATA |
                I2C_FUNC_SMBUS_BLOCK_DATA |
                I2C_FUNC_SMBUS_I2C_BLOCK);
    }

    nv_kmem_cache_free_stack(sp);

    return ret;
}

static struct i2c_algorithm nv_i2c_algo = {
    .master_xfer      = nv_i2c_algo_master_xfer,
    .smbus_xfer       = nv_i2c_algo_smbus_xfer,
    .functionality    = nv_i2c_algo_functionality,
};

struct i2c_adapter nv_i2c_adapter_prototype = {
    .owner             = THIS_MODULE,
    .algo              = &nv_i2c_algo,
    .algo_data         = NULL,
};

void* NV_API_CALL nv_i2c_add_adapter(nv_state_t *nv, NvU32 port)
{
    NV_STATUS rmStatus;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct i2c_adapter *pI2cAdapter = NULL;
    int osstatus = 0;

    // get a i2c adapter
    rmStatus = os_alloc_mem((void **)&pI2cAdapter,sizeof(struct i2c_adapter));

    if (rmStatus != NV_OK)
        return NULL;

    // fill in with default structure
    os_mem_copy(pI2cAdapter, &nv_i2c_adapter_prototype, sizeof(struct i2c_adapter));

    pI2cAdapter->dev.parent = nvl->dev;

    if (nvl->pci_dev != NULL)
    {
        snprintf(pI2cAdapter->name, sizeof(pI2cAdapter->name),
            "NVIDIA i2c adapter %u at %x:%02x.%u", port, nv->pci_info.bus,
             nv->pci_info.slot, PCI_FUNC(nvl->pci_dev->devfn));
    }
    else
    {
        snprintf(pI2cAdapter->name, sizeof(pI2cAdapter->name),
            "NVIDIA SOC i2c adapter %u", port);
    }

    // add our data to the structure
    pI2cAdapter->algo_data = (void *)nv;

    // attempt to register with the kernel
    osstatus = i2c_add_adapter(pI2cAdapter);

    if (osstatus)
    {
        // free the memory and NULL the ptr
        os_free_mem(pI2cAdapter);

        pI2cAdapter = NULL;
    }

    return ((void *)pI2cAdapter);
}

void NV_API_CALL nv_i2c_del_adapter(nv_state_t *nv, void *data)
{
    struct i2c_adapter *pI2cAdapter = (struct i2c_adapter *)data;

    if (pI2cAdapter)
    {
        // release with the OS
        i2c_del_adapter(pI2cAdapter);
        os_free_mem(pI2cAdapter);
    }
}

#else // defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)

void NV_API_CALL nv_i2c_del_adapter(nv_state_t *nv, void *data)
{
}

void* NV_API_CALL nv_i2c_add_adapter(nv_state_t *nv, NvU32 port)
{
    return NULL;
}

#endif // defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
