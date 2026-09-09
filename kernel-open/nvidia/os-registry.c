/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define NV_DEFINE_REGISTRY_KEY_TABLE
#include "os-interface.h"
#include "nv-linux.h"
#include "nv-reg.h"
#include "nv-gpu-info.h"
#include <linux/firmware.h>

#define NV_BINARY_REGISTRY_FIRMWARE_DIR "nvidia/"

static NV_STATUS
nv_parse_binary_registry_token(const char *param_name, char *token,
                               char **key_name, char **path)
{
    if ((key_name == NULL) || (path == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Extract key name before first '='; reject if missing or empty
    if (((*key_name = strsep(&token, "=")) == NULL) || !strlen(*key_name))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s contains malformed token: missing key name\n",
                  param_name);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Extract file path after first '='; reject if missing or empty
    if (((*path = strsep(&token, "=")) == NULL) || !strlen(*path))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s contains malformed token for key \"%s\": missing path\n",
                  param_name, *key_name);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Reject tokens with more than one '=' (e.g. "key=path=extra")
    if (strsep(&token, "=") != NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s contains malformed token for key \"%s\": too many '=' separators\n",
                  param_name, *key_name);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

static NV_STATUS
nv_write_binary_registry_file(nvidia_stack_t *sp, nv_state_t *nv,
                              const char *key_name, const char *relative_path,
                              const char *param_name)
{
    int fw_status;
    char *firmware_path = NULL;
    NV_STATUS status;
    size_t firmware_path_len;
    const struct firmware *fw = NULL;
    nv_linux_state_t *nvl;

    if ((nv == NULL) || (key_name == NULL) ||
        (relative_path == NULL) || (param_name == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    firmware_path_len = strlen(NV_BINARY_REGISTRY_FIRMWARE_DIR) +
                        strlen(relative_path) + 1;

    status = os_alloc_mem((void **)&firmware_path, firmware_path_len);
    if (status != NV_OK)
    {
        return status;
    }

    snprintf(firmware_path, firmware_path_len, "%s%s",
             NV_BINARY_REGISTRY_FIRMWARE_DIR, relative_path);

    fw_status = request_firmware(&fw, firmware_path, nvl->dev);
    if (fw_status != 0)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s key \"%s\" failed to load firmware file \"%s\" (rc=%d)\n",
                  param_name, key_name, firmware_path, fw_status);
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    if (fw->size > NV_U32_MAX)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s key \"%s\" firmware file \"%s\" too large (%zu bytes)\n",
                  param_name, key_name, firmware_path, fw->size);
        status = NV_ERR_INVALID_DATA;
        goto done;
    }

    status = rm_write_registry_binary(sp, nv, key_name,
                                      (NvU8 *)fw->data, (NvU32)fw->size);
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s key \"%s\" failed to write registry binary (status=0x%x)\n",
                  param_name, key_name, status);
    }

done:
    if (fw != NULL)
    {
        release_firmware(fw);
    }

    os_free_mem(firmware_path);
    return status;
}

/*!
 * @brief This function parses the PCI BDF identifier string and returns the
 * Domain, Bus, Device and function components from the PCI BDF string.
 *
 * This parser is highly adaptable and hence allows PCI BDF string in following
 * 3 formats.
 *
 * 1)  bus:slot                 : Domain and function defaults to 0.
 * 2)  domain:bus:slot          : Function defaults to 0.
 * 3)  domain:bus:slot.func     : Complete PCI dev id string.
 *
 * @param[in]  pci_dev_str      String containing the BDF to be parsed.
 * @param[out] pci_domain       Pointer where pci_domain is to be returned.
 * @param[out] pci_bus          Pointer where pci_bus is to be returned.
 * @param[out] pci_slot         Pointer where pci_slot is to be returned.
 * @param[out] pci_func         Pointer where pci_func is to be returned.
 *
 * @return NV_TRUE if succeeds, or NV_FALSE otherwise.
 */
static NV_STATUS pci_str_to_bdf(char *pci_dev_str, NvU32 *pci_domain,
    NvU32 *pci_bus, NvU32 *pci_slot, NvU32 *pci_func)
{
    char *option_string = NULL;
    char *token, *string;
    NvU32 domain, bus, slot;
    NV_STATUS status = NV_OK;

    //
    // remove_spaces() allocates memory, hence we need to keep a pointer
    // to the original string for freeing at end of function.
    //
    if ((option_string = rm_remove_spaces(pci_dev_str)) == NULL)
    {
        // memory allocation failed, returning
        return NV_ERR_GENERIC;
    }

    string = option_string;

    if (!strlen(string) || !pci_domain || !pci_bus || !pci_slot || !pci_func)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    if ((token = strsep(&string, ".")) != NULL)
    {
        // PCI device can have maximum 8 functions only.
        if ((string != NULL) && (!(*string >= '0' && *string <= '7') ||
            (strlen(string) > 1)))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: Invalid PCI function in token %s\n",
                      pci_dev_str);
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
        else if (string == NULL)
        {
            *pci_func = 0;
        }
        else
        {
            *pci_func = (NvU32)(*string - '0');
        }

        domain = simple_strtoul(token, &string, 16);

        if ((string == NULL) || (*string != ':') || (*(string + 1) == '\0'))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: Invalid PCI domain/bus in token %s\n",
                      pci_dev_str);
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        token = string;
        bus = simple_strtoul((token + 1), &string, 16);

        if (string == NULL)
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: Invalid PCI bus/slot in token %s\n",
                      pci_dev_str);
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if (*string != '\0')
        {
            if ((*string != ':') || (*(string + 1) == '\0'))
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: Invalid PCI slot in token %s\n",
                          pci_dev_str);
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            token = string;
            slot = (NvU32)simple_strtoul(token + 1, &string, 16);
            if ((slot == 0) && ((token + 1) == string))
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: Invalid PCI slot in token %s\n",
                          pci_dev_str);
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }
            *pci_domain = domain;
            *pci_bus = bus;
            *pci_slot = slot;
        }
        else
        {
            *pci_slot = bus;
            *pci_bus = domain;
            *pci_domain = 0;
        }
        status = NV_OK;
    }
    else
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }

done:
    // Freeing the memory allocated by remove_spaces().
    os_free_mem(option_string);
    return status;
}

/*!
 * @brief This function checks NVreg_CoherentGPUMemoryMode on GPU initialization
 * to set the right defaults, which can be platform specific and implied. Previously
 * the default was CDMM is an opt-in. The new logic is called on GPU initialization
 * and if the user has not provided a preference, CDMM mode is enabled.
 *
 * @param[in] sp        Pointer to nvidia_stack_t
 */
void nv_enable_cdmm_mode(nvidia_stack_t *sp)
{
    // The user has made a preference, just return
    if (NVreg_CoherentGPUMemoryMode != NULL)
    {
        return;
    }

    NVreg_EnableUserNUMAManagement = 0;
    rm_write_registry_dword(sp, NULL, NV_REG_ENABLE_USER_NUMA_MANAGEMENT,
                            NVreg_EnableUserNUMAManagement);
    return;
}

/*!
 * @brief Parse and apply per-device binary registry key files configured via
 * NVreg_RegistryBinaryFilePerDevice.
 *
 * @param[in]  sp          pointer to nvidia_stack_t struct.
 * @param[in]  current_nv  pointer to the currently probing GPU state.
 *
 * @return NV_OK if succeeds, or NV_STATUS error code otherwise.
 */
NV_STATUS nv_parse_per_device_binary_option_string(nvidia_stack_t *sp,
                                                   nv_state_t *current_nv)
{
    NV_STATUS status = NV_OK;
    char *option_string = NULL;
    char *ptr, *token;
    char *name, *value;
    NvU32 domain, bus, slot, func;
    nv_state_t *nv = NULL;

    if (current_nv == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (NVreg_RegistryBinaryFilePerDevice == NULL)
    {
        return NV_OK;
    }

    option_string = rm_remove_spaces(NVreg_RegistryBinaryFilePerDevice);
    if (option_string == NULL)
    {
        return NV_ERR_GENERIC;
    }

    ptr = option_string;

    while ((token = strsep(&ptr, ";")) != NULL)
    {
        if (!strlen(token))
        {
            continue;
        }

        status = nv_parse_binary_registry_token("NVreg_RegistryBinaryFilePerDevice",
                                                token, &name, &value);
        if (status != NV_OK)
        {
            break;
        }

        if (strcmp(name, NV_REG_PCI_DEVICE_BDF) == 0)
        {
            status = pci_str_to_bdf(value, &domain, &bus, &slot, &func);

            if (status != NV_OK)
            {
                nv = NULL;
                status = NV_OK;
                continue;
            }

            if ((current_nv->pci_info.domain == domain) &&
                (current_nv->pci_info.bus == bus) &&
                (current_nv->pci_info.slot == slot) &&
                (current_nv->pci_info.function == func))
            {
                nv = current_nv;
            }
            else
            {
                nv = NULL;
            }
            continue;
        }

        if (nv == NULL)
        {
            continue;
        }

        status = nv_write_binary_registry_file(sp, nv, name,
                                               value, "NVreg_RegistryBinaryFilePerDevice");
        if (status != NV_OK)
        {
            break;
        }
    }

    os_free_mem(option_string);
    return status;
}

/*!
 * @brief This function parses the registry keys per GPU device. It accepts a
 * semicolon separated list of key=value pairs. The first key value pair MUST be
 * "pci=DDDD:BB:DD.F;" where DDDD is Domain, BB is Bus Id, DD is device slot
 * number and F is the Function. This PCI BDF is used to identify which GPU to
 * assign the registry keys that follows next.
 * If a GPU corresponding to the value specified in "pci=DDDD:BB:DD.F;" is NOT
 * found, then all the registry keys that follows are skipped, until we find next
 * valid pci identified "pci=DDDD:BB:DD.F;". Following are the valid formats for
 * the value of the "pci" string:
 * 1)  bus:slot                 : Domain and function defaults to 0.
 * 2)  domain:bus:slot          : Function defaults to 0.
 * 3)  domain:bus:slot.func     : Complete PCI dev id string.
 *
 *
 * @param[in]  sp          pointer to nvidia_stack_t struct.
 * @param[in]  current_nv  pointer to the currently probing GPU state.
 *
 * @return NV_OK if succeeds, or NV_STATUS error code otherwise.
 */
NV_STATUS nv_parse_per_device_option_string(nvidia_stack_t *sp,
                                            nv_state_t *current_nv)
{
    NV_STATUS status = NV_OK;
    char *option_string = NULL;
    char *ptr, *token;
    char *name, *value;
    NvU32 data, domain, bus, slot, func;
    nv_state_t *nv = NULL;

    if (current_nv == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (NVreg_RegistryDwordsPerDevice != NULL)
    {
        if ((option_string = rm_remove_spaces(NVreg_RegistryDwordsPerDevice)) == NULL)
        {
            return NV_ERR_GENERIC;
        }

        ptr = option_string;

        while ((token = strsep(&ptr, ";")) != NULL)
        {
            if (!(name = strsep(&token, "=")) || !strlen(name))
            {
                continue;
            }

            if (!(value = strsep(&token, "=")) || !strlen(value))
            {
                continue;
            }

            if (strsep(&token, "=") != NULL)
            {
                continue;
            }

            // If this key is "pci", then value is pci_dev id string
            // which needs special parsing as it is NOT a dword.
            if (strcmp(name, NV_REG_PCI_DEVICE_BDF) == 0)
            {
                status = pci_str_to_bdf(value, &domain, &bus, &slot, &func);

                // Check if PCI_DEV id string was in a valid format or NOT.
                if (NV_OK != status)
                {
                    // lets reset cached pci dev
                    nv = NULL;
                }
                else if ((current_nv->pci_info.domain == domain) &&
                         (current_nv->pci_info.bus == bus) &&
                         (current_nv->pci_info.slot == slot) &&
                         (current_nv->pci_info.function == func))
                {
                    nv = current_nv;
                }
                else
                {
                    //
                    // BDF doesn't match the currently probing GPU;
                    // skip registry keys until next matching
                    // pci= entry.
                    //
                    nv = NULL;
                }
                continue;
            }

            //
            // Check if cached pci_dev string in the commandline is in valid
            // format, else we will skip all the successive registry entries
            // (<key, value> pairs) until a valid PCI_DEV string is encountered
            // in the commandline.
            //
            if (!nv)
                continue;

            data = (NvU32)simple_strtoul(value, NULL, 0);

            rm_write_registry_dword(sp, nv, name, data);
        }

        os_free_mem(option_string);
    }
    return status;
}

/*
 * Compare given string UUID with the GpuBlacklist or ExcludedGpus registry
 * parameter string and return whether the UUID is in the GPU exclusion list
 */
NvBool nv_is_uuid_in_gpu_exclusion_list(const char *uuid)
{
    const char *input;
    char *list;
    char *ptr;
    char *token;

    //
    // When both NVreg_GpuBlacklist and NVreg_ExcludedGpus are defined
    // NVreg_ExcludedGpus takes precedence.
    //
    if (NVreg_ExcludedGpus != NULL)
        input = NVreg_ExcludedGpus;
    else if (NVreg_GpuBlacklist != NULL)
        input = NVreg_GpuBlacklist;
    else
        return NV_FALSE;

    if ((list = rm_remove_spaces(input)) == NULL)
        return NV_FALSE;

    ptr = list;

    while ((token = strsep(&ptr, ",")) != NULL)
    {
        if (strcmp(token, uuid) == 0)
        {
            os_free_mem(list);
            return NV_TRUE;
        }
    }
    os_free_mem(list);
    return NV_FALSE;
}

NV_STATUS NV_API_CALL os_registry_init(void)
{
    nv_parm_t *entry;
    unsigned int i;
    nvidia_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    if (NVreg_RmNvlinkBandwidth != NULL)
    {
        rm_write_registry_string(sp, NULL,
                                 "RmNvlinkBandwidth",
                                 NVreg_RmNvlinkBandwidth,
                                 strlen(NVreg_RmNvlinkBandwidth));
    }

    if (NVreg_RmMsg != NULL)
    {
        rm_write_registry_string(sp, NULL,
                "RmMsg", NVreg_RmMsg, strlen(NVreg_RmMsg));
    }

    //
    // CoherentGPUMemoryMode=driver just implies the older 
    // EnableUserNUMAManagement=0 option
    //
    if (NVreg_CoherentGPUMemoryMode != NULL)
    {
        if (strcmp(NVreg_CoherentGPUMemoryMode, "driver") == 0)
        {
            NVreg_EnableUserNUMAManagement = 0;
        }
        else
        {
            NVreg_EnableUserNUMAManagement = 1;
        }
    }

    rm_parse_option_string(sp, NVreg_RegistryDwords);

    for (i = 0; (entry = &nv_parms[i])->name != NULL; i++)
    {
        rm_write_registry_dword(sp, NULL, entry->name, *entry->data);
    }

    nv_kmem_cache_free_stack(sp);

    return NV_OK;
}
